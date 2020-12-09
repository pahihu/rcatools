/*
 * History
 * =======
 * 201201AP     initial revision, expr evaluator
 * 201202AP     16bit only, shr/shl, logical ops
 * 201203AP     labels, goto
 *              symbols pass syms[] idx, instead of offset
 *              ternary cond, logical AND/OR/NOT
 *
 * NB. ++ increments by 1, addresses in words
 *     '&' and '*' converts word addresses to byte addresses
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "calc18.h"
#include "y.tab.h"

/* SP - points to empty space

   PUSH8   STXD
   POP8    IRX ;LDX

   PUSH16  STXD ;STXD
   POP16   IRX ;LDXA ;LDX

 */

char *fn;
int lowreg; // lowest regvar
int nregvars; // no.of regvars
int ex(NODE *p);
static void gmul2(void);
static void glvaluvar(NODE *p, int regena, int pushma);

static int lbl = 0;

#define LO(x)   ((x) & 0xFF)
#define HI(x)   LO((x)>>8)

#define ADDR(p) (getoffs((p)->x))

static void prinode1(char *msg,NODE *p) {
   static char *typs[] = { "INT", "CON", "ID", "OPR" };

   if (!p) {
      fprintf(stderr,"NULL\n");
      return;
   }
   fprintf(stderr,"%s: t:%s x:%d\n",msg,typs[1+p->t],p->x);
   prinode1("A0",p->a[0]);
   prinode1("A1",p->a[1]);
}

static void prinode(NODE *p) {
   prinode1("HEAD",p);
}

static int isvar(NODE *p) {
   if (!p)
      return 0;
   return ID == p->t;
}

static int isreg(NODE *p) {
   if (!p)
      return 0;
   if (isvar(p))
      return C_REG == getcls(p->x);
   return 0;
}

static char *regnm(int x) {
   static char buf[8];

   sprintf(buf,"R%X", x);
   return buf;
}

static char *reg(NODE *p) {
   assert(isreg(p));

   return regnm(getoffs(p->x));
}

static int isimm(NODE *p) {
   if (!p)
      return 0;
   return CON == p->t;
}

static int isimm8(NODE *p) {
   return isimm(p) && !HI(p->x); 
}

static int isbyte(NODE *p) {
   return isvar(p) || isimm8(p);
}

static void gpush(int n, char *reg) {
   if (1 == n) {
      H(" ..PUSH %s.0\n",reg);
      H(" GLO %s ;STXD\n",reg);
   }
   else {
      H(" ..PUSH %s\n",reg);
      H(" GHI %s ;STXD\n",reg);
      H(" GLO %s ;STXD\n",reg);
   }
}

static void gpushi(int x) {
   H(" ..PUSHI %02X%02X\n",HI(x),LO(x));
   H(" LDI #%02X ;STXD\n",HI(x));
   H(" LDI #%02X ;STXD\n",LO(x));
}

static void gpushac(int n) {
   gpush(n,"AC");
}

static void gpop(int n, char *reg) {
   if (1 == n) {
      H(" ..POP %s.0\n",reg);
      if (0 == strcmp(reg, "AC"))
         H(" LDI #00 ;PHI AC\n");
      H(" IRX ;LDX ;PLO %s\n", reg);
   }
   else {
      H(" ..POP %s\n",reg);
      H(" IRX ;LDXA ;PLO %s\n", reg);
      H(" LDX ;PHI %s\n", reg);
   }
}

static void gmov(char *dst, char *src) {
   H(" ..%s=%s\n",dst,src);
   H(" GLO %s ;PLO %s\n",src,dst);
   H(" GHI %s ;PHI %s\n",src,dst);
}

static void gdtoac(void) {
   H(" ..AC=(D,D)\n");
   H(" PHI AC ;PLO AC\n");
}

static void gpushvar(NODE *p) {
   char *r;

   assert(isvar(p));

   glvaluvar(p, 1, 0);
   if (isreg(p)) {
      r = reg(p);
      H(" GHI %s ;STXD\n",r);
      H(" GLO %s ;STXD\n",r);
   }
   else {
      H(" LDA MA ;PLO AUX\n");
      H(" LDN MA ;STXD\n");
      H(" GLO AUX; STXD\n");
   }
}

static void gbinary(NODE *p) {
   char *r;

   if (isvar(p->a[1]))
      gpushvar(p->a[1]);
   else {
      ex(p->a[1]);
      gpushac(2);  // (LO,HI) @ X
   }
   ex(p->a[0]); // AC
}

static void gsub(NODE *p, int cmp) {
   NODE *con;

   if (CON == p->a[1]->t) {
      con = p->a[1];

      ex(p->a[0]);
      if (1 == con->x && !cmp) {
         H(" DEC AC\n");
      }
      else {
         H(" ..AC=AC-%04X\n",con->x);
         /* NB. ggt() calls with cmp=2, because it needs the AC val */
         if (1 == cmp) {
            H(" GLO AC ;SMI #%02X\n", LO(con->x));
            H(" GHI AC ;SMBI #%02X\n", HI(con->x));
         }
         else {
            H(" GLO AC ;SMI #%02X ;PLO AC\n", LO(con->x));
            H(" GHI AC ;SMBI #%02X ;PHI AC\n", HI(con->x));
         }
      }
   }
   else {
      gbinary(p);
      H(" ..AC=AC-*SP++\n");
      if (1 == cmp) {
         H(" GLO AC ;IRX ;SM\n");
         H(" GHI AC ;IRX ;SMB\n");
      }
      else {
         H(" GLO AC ;IRX ;SM ;PLO AC\n");
         H(" GHI AC ;IRX ;SMB ;PHI AC\n");
      }
   }
}

static void glt(NODE *p) {
   gsub(p,1); // A - AUX @ X
   // DF=0
   H(" ..LT?\n");
   H(" ..D=DF? #00 : #FF\n");
   H(" LDI #FF ;ADCI #00\n");  // DF=1 FALSE, DF=0 TRUE
   gdtoac();
}

static void gge(NODE *p) {
   gsub(p,1);
   H(" ..D=DF? #FF : #00\n");
   H(" LDI #FF ;ADCI #00\n");
   H(" XRI #FF\n");
   gdtoac();
}

static void ggt(NODE *p) {
   int lbl1;

   gsub(p,2);
   H(" ..D=DF? XX : #00\n");
   H(" ..DF=0? RESULT IS ZERO\n");
   H(" LBNF L%04d\n", lbl1=lbl++);
   H(" ..IF AC=0 DF=0 ELSE DF=1\n");
   H(" LSNZ ; GLO AC ;LSZ\n");
   H(" LDI #01 ;SHR\n");
   H("L%04d:\n",lbl1);
   H(" ..AC=DF? #FF : #00\n");
   H(" LDI #00 ;LSNF ;LDI #FF\n");
   gdtoac();
}

static void geq0() {
   H(" ..AC=0==AC? #FFFF : #0000\n");
   H(" GLO AC ;LSNZ\n");
   H(" GHI AC ;LSZ\n");
   H(" LDI #FF\n");
   H(" XRI #FF\n");
   gdtoac();
}

static void geq(NODE *p) {
   gsub(p,0);
   geq0();
}

static void gtobool(void) {
   H(" ..AC=AC? #FFFF : #0000\n");
   H(" GLO AC ;LSNZ\n");
   H(" GHI AC ;LSZ\n");
   H(" LDI #FF\n");
   gdtoac();
}

static void gcom(void) {
   H(" ..COM AC\n");
   H(" GHI AC ;XRI #FF ;PHI AC\n");
   H(" GLO AC ;XRI #FF ;PLO AC\n");
}


static void gadd(NODE *p) {
   NODE *con;

   if (CON == p->a[1]->t) {
      con = p->a[1];

      ex(p->a[0]);
      if (1 == con->x)
         H(" INC AC\n");
      else {
         H(" ..AC=AC+%04X\n",con->x);
         H(" GLO AC ;ADI #%02X ;PLO AC\n", LO(con->x));
         H(" GHI AC ;ADCI #%02X ;PHI AC\n", HI(con->x));
      }
   }
   else {
      gbinary(p);
      H(" ..AC=AC+*SP++\n");
      H(" IRX ;GLO AC ;ADD ;PLO AC\n");
      H(" IRX ;GHI AC ;ADC ;PHI AC\n");
   }
}

static void glog(NODE *p, char *xop, char *imop) {
   NODE *con;

   if (CON == p->a[1]->t) {
      con = p->a[1];

      ex(p->a[0]);
      H(" ..AC=AC %s %04X\n",xop,con->x);
      H(" GLO AC ;%s #%02X ;PLO AC\n", imop, LO(con->x));
      H(" GHI AC ;%s #%02X ;PHI AC\n", imop, HI(con->x));
   }
   else {
      gbinary(p);
      H(" ..AC=AC %s *SP++\n",xop);
      H(" IRX ;GLO AC ;%s ;PLO AC\n", xop);
      H(" IRX ;GHI AC ;%s ;PHI AC\n", xop);
   }
}

static void gwhile(NODE *cond, NODE *body, NODE *end) {
   int lbl1, lbl2, lbl3;

   H("L%04d:\n", lbl1=lbl++);
   ex(cond);
   H(" ..0==AC?\n");
   H(" GHI AC ;LBNZ L%04d\n", lbl2=lbl++); H(" GLO AC ;LBZ L%04d\n", lbl3=lbl++); H("L%04d: ..BODY\n",lbl2);
   ex(body);
   if (end)
      ex(end);
   H(" LBR L%04d\n",lbl1);
   H("L%04d: ..END\n",lbl3);
}

static void gldi(int x) {
   H(" ..AC=%02X%02X\n",HI(x),LO(x));
   if (HI(x) == LO(x))
      H(" LDI #%02X ;PHI AC ;PLO AC\n",HI(x));
   else {
      H(" LDI #%02X ;PHI AC\n",HI(x));
      H(" LDI #%02X ;PLO AC\n", LO(x));
   }
}

static void glvaluvar(NODE *p, int regena, int pushma) {
   int cls, offs;
   char *sym;

   assert(isvar(p));

   cls  = getcls(p->x);
   sym  = getsym(p->x);
   offs = getoffs(p->x);
   H(" ..MA=ADDR(%s)\n",getsym(p->x));
   switch (cls) {
   case C_UNDEF:
      fprintf(stderr,"glvalu(): %s undefined\n",sym);
      exit(1);
      break;
   case C_EXTRN:
      if (pushma) {
         H(" LDI A.1(L%s) ;STXD\n",sym);
         H(" LDI A.0(L%s) ;STXD\n",sym);
      }
      else {
         H(" LDI A.0(L%s) ;PLO MA\n",sym);
         H(" LDI A.1(L%s) ;PHI MA\n",sym);
      }
      break;
   case C_AUTO:
      if (pushma) {
         H(" GLO FP ;SMI #%02X ;PLO AUX\n",LO(offs));
         H(" GHI FP ;SMBI #%02X ;STXD\n",HI(offs));
         H(" GLO AUX ;STXD\n");
      }
      else {
         H(" GLO FP ;SMI #%02X ;PLO MA\n",LO(offs));
         H(" GHI FP ;SMBI #%02X ;PHI MA\n",HI(offs));
      }
      break;
   case C_LABEL:
      fprintf(stderr,"glvalu(): %s is label\n",sym);
      exit(1);
      break;
   case C_PARAM:
      if (pushma) {
         H(" GLO FP ;ADI #%02X ;PLO AUX\n",LO(offs));
         H(" GHI FP ;ADCI #%02X ;STXD\n",HI(offs));
         H(" GLO AUX ;STXD\n");
      }
      else {
         H(" GLO FP ;ADI #%02X ;PLO MA\n",LO(offs));
         H(" GHI FP ;ADCI #%02X ;PHI MA\n",HI(offs));
      }
      break;
   case C_REG:
      if (!regena) {
         fprintf(stderr,"glvalu(): register not allowed\n");
         exit(1);
      }
      break;
   default:
      fprintf(stderr,"glvalu(): unknown cls %d\n",cls);
      exit(1);
   }
}

static void glvalu(NODE *p, int regena) { // addr in MA

   if (isvar(p)) {
      glvaluvar(p, regena, 0);
      return;
   }
   else if (OPR == p->t) {
      if (UNARY + '*' == p->x) {
         ex(p->a[0]);
         H(" ..MA=2*AC\n");
         H(" GLO AC ;SHL  ;PLO MA\n");
         H(" GHI AC ;SHLC ;PHI MA\n");
         return;
      }
      else if ('[' == p->x) {
         // base
         switch (p->a[0]->t) {
         case ID:
            // glvalu(p->a[0], 0);    // id[x]
            // gpush(2,"MA");
            glvaluvar(p->a[0], 0, 1);
            break;
         case CON:              // const[x]
            gpushi(2*p->a[0]->x);
            break;
         default:               // (expr)[x]
            ex(p->a[0]);
            // gmul2();
            // gpushac(2);
            H(" GLO AC ;SHL ;PLO AUX\n");
            H(" GHI AC ;SHLC ;STXD\n");
            H(" GLO AUX ;STXD\n");
         }
         if (isimm(p->a[1]))
            gldi(2*p->a[1]->x);
         else {
            ex(p->a[1]); // index
            gmul2();
         }
         H(" ..MA=AC+*SP++\n");
         H(" GLO AC ;IRX ;ADD ;PLO MA\n");
         H(" GHI AC ;IRX ;ADC ;PHI MA\n");
         return;
      }
   }
   fprintf(stderr,"unknown lvalu: t=%d x=%d\n",p->t,p->x);
   abort();
}

static void gdiv2(void) {
   H(" ..AC=AC / 2\n");
   H(" GHI AC ;SHR ;PHI AC\n");
   H(" GLO AC ;SHRC ;PLO AC\n");
}

static void gshr(NODE *p) {
   int lbl1, lbl2;
   int i;
   NODE *con;

   con = 0;
   if (CON == p->a[1]->t) {
      con = p->a[1];
      if (con->x < 4) {
         ex(p->a[0]);
         H(" ..AC=AC >> %d\n",con->x);
         for (i = 0; i < con->x; i++)
            gdiv2();
         return;
      }
      else {
         H(" ..PUSH %02X\n",LO(con->x));
         H(" LDI #%02X ;STXD\n", LO(con->x));
      }
   }
   if (!con) {
      ex(p->a[1]);
      gpushac(1);
   }
   ex(p->a[0]);
   gpop(1, "AUX");
   H(" ..0==AUX.0?\n");
   H(" LBZ L%04d\n", lbl1=lbl++);
   H("L%04d: ..LOOP\n", lbl2=lbl++);
   gdiv2();
   H(" ..IF (--AUX.0) GOTO LOOP\n");
   H(" DEC AUX ;GLO AUX ;LBNZ L%04d\n", lbl2);
   H("L%04d:\n", lbl1);
}

static void gmul2(void) {
   H(" ..AC=AC * 2\n");
   H(" GLO AC ;SHL ;PLO AC\n");
   H(" GHI AC ;SHLC ;PHI AC\n");
}

static void gshl(NODE *p) {
   int lbl1, lbl2;
   NODE *con;
   int i;

   con = 0;
   if (CON == p->a[1]->t) {
      con = p->a[1];
      if (con->x < 4) { 
         ex(p->a[0]);
         H(" ..AC=AC << %d\n",con->x);
         for (i = 0; i < con->x; i++)
            gmul2();
         return;
      }
      else {
         H(" ..PUSH %02X\n",LO(con->x));
         H(" LDI #%02X ;STXD\n", LO(con->x));
      }
   }
   if (!con) {
      ex(p->a[1]);
      gpushac(1);
   }
   ex(p->a[0]);
   gpop(1, "AUX");
   H(" ..0==AUX.0?\n");
   H(" LBZ L%04d\n", lbl1=lbl++);
   H("L%04d: ..SHL LOOP\n", lbl2=lbl++);
   gmul2();
   H(" ..IF (--AUX.0) GOTO LOOP\n");
   H(" DEC AUX ;GLO AUX ;LBNZ L%04d\n", lbl2);
   H("L%04d:\n", lbl1);
}

static void gstvar(NODE *p) {
   if (isreg(p)) {
      gmov(reg(p), "AC");
   }
   else {
      H(" ..ST [MA],AC\n");
      H(" GLO AC ;STR MA ;INC MA\n");
      H(" GHI AC ;STR MA\n");
   }
}

static void gpreinc(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      H(" ..++%s\n",r);
      H(" INC %s\n",r);
      gmov("AC", r);
   }
   else {
      ex(p);
      H(" ..++AC\n");
      H(" INC AC\n");
      H(" DEC MA\n");
      gstvar(p);
   }
}

static void gpredec(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      H(" ..--%s\n",r);
      H(" DEC %s\n",r);
      gmov("AC", r);
   }
   else {
      ex(p);
      H(" DEC AC\n");
      H(" DEC MA\n");
      gstvar(p);
   }
}

static void gpostinc(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      gmov("AC", r);
      H(" INC %s\n", r);
   }
   else {
      gpreinc(p);
      H(" DEC AC\n");
   }
}

static void gpostdec(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      gmov("AC", r);
      H(" DEC %s\n",r);
   }
   else {
      gpredec(p);
      H(" INC AC\n");
   }
}

static void gdiv(NODE *p) {
   H(" ..DIV\n");
   if (isvar(p->a[1]))
      gpushvar(p->a[1]);
   else {
      ex(p->a[1]);
      gpushac(2);
   }
   ex(p->a[0]);
   H(" LDI A.0(UDIV) ;PLO SUB\n");
   H(" SEP SUB\n");
}

static void gldvar(NODE *p) {
   if (isreg(p)) {
      gmov("AC", reg(p));
   }
   else {
      H(" LDA MA ;PLO AC\n");
      H(" LDN MA ;PHI AC\n");
   }
}

static int idlist(int t,NODE *p, int offs) {
   int n, newoffs;
   NODE *q;

   if (!p)
      return offs;

   assert(OPR == p->t && ILST == p->x);
   newoffs = idlist(t,p->a[0],ILST==t? 2+offs : offs);
   q = p->a[1];
   assert(isvar(q));
   n = q->x;
   switch (t) {
   case ILST:
      // fprintf(stderr,"defpar%d: %s\n",offs,getsym(n));
      defcls(n, C_PARAM, offs);
      break;
   case EXTDEF:
      // fprintf(stderr,"defext: %s\n",getsym(n));
      defcls(n, C_EXTRN, 0);
      break;
   case AUTODEF:
      q = q->a[0]; // storage size
      assert(CON == q->t);
      newoffs += q->x;
      // fprintf(stderr,"defauto: %s %d\n",getsym(n), newoffs);
      defcls(n, C_AUTO, newoffs);
      break;
   case REGDEF:
      newoffs--;
      if (newoffs == 0x0B) {
         fprintf(stderr,"too many register variables\n");
         exit(1);
      }
      defcls(n, C_REG, newoffs);
      break;
   }
   return newoffs;
}

static int exprlist(NODE *p, int offs) {
   int n;
   NODE *q;

   if (!p)
      return offs;

   assert(OPR == p->t && XLST == p->x);
   offs = exprlist(p->a[0],offs);
   H(" ..PUSH ARG%d\n",++offs);
   if (isvar(p->a[1]))
      gpushvar(p->a[1]);
   else {
      ex(p->a[1]);
      gpushac(2);
   }

   return offs;
}

static void galign(void) {
   H(" ..ALIGN\n");
   H(" ORG*+(* AND 1)\n");
}

static void gmod(NODE *p) {
   gdiv(p);
   gmov("AC","MQ");
}

int ilog2(int x)
{
   int i, m;

   m = 1;
   for (m = 1, i = 0; i < 8; m *= 2, i++) {
      if (x == m)
         return i;
   }
   return -1;
}

static int gspecmul(NODE *p) {
   NODE *con, r;
   int i;

// fprintf(stderr,"before swap\n");
// prinode(p);
   if (isimm(p->a[1])) {
      con = p->a[0]; p->a[0] = p->a[1]; p->a[1] = con;
   }
// fprintf(stderr,"after swap\n");
// prinode(p);
   if (isimm(p->a[0])) {
      con = p->a[0];
      i = ilog2(con->x);
      if (i < 0)
         return 0;
      if (i == 0)
         return 1;

      r = *p;
      r.a[0] = p->a[1];
      r.a[1] = con(i);
// fprintf(stderr,"before gshl\n");
// prinode(&r);
      gshl(&r);
      freenod(r.a[1]);
      return 1;
   }
   return 0;
}

static int gspecdiv(NODE *p) {
   NODE r, *con; 
   int i;

   if (isimm(p->a[1])) {
      con = p->a[1];
      i = ilog2(con->x);
      if (i < 0)
         return 0;
      if (i == 0)
         return 1;

      r = *p;
      r.a[1] = con(i);
      gshr(&r);
      freenod(r.a[1]);
      return 1;
   }
   return 0;
}

static void gand(NODE *p) {
   glog(p, "AND", "ANI");
}

static int gspecmod(NODE *p) {
   NODE *con; 
   int i;

   if (isimm(p->a[1])) {
      con = p->a[1];
      i = ilog2(con->x);
      if (i < 0)
         return 0;
      if (i == 0)
         return 1;

      con->x--;
      gand(con);
      return 1;
   }
   return 0;
}

int ex(NODE *p) {
   int lbl1, lbl2, lbl3;
   int clobber;
   NODE *q, *stmt;
   char *sym;
   int offs, i, n;
   int argcnt;

   if (!p) return 0;
   if (INT == p->t) {
      fprintf(stderr,"ex(): internal node!\n");
      exit(1);
   }
   switch (p->t) {
   case CON:
      gldi(p->x);
      break;
   case STR:
      H(" ..STR %s [%d]\n",p->s,p->x=lbl++);
      H(" LDI A.1(L%d) ;SHR ;PHI AC\n",p->x);
      H(" LDI A.0(L%d) ;SHRC ;PLO AC\n",p->x);
      break;
   case ID: glvalu(p, 1); gldvar(p); break;
   case OPR:
      switch (p->x) {
      case VARDEF:
         sym = getsym(p->a[0]->x);
         defcls(p->a[0]->x,C_EXTRN,0);
         H(" ..EXTRN %s\n",sym);
         galign();
         H("L%s:\n",sym);
         H(" ORG*+%d\n",p->a[1]->x);
         break;
      case VECDEF:
         sym = getsym(p->a[0]->x);
         defcls(p->a[0]->x,C_EXTRN,0);
         H(" ..EXTRN %s[]\n",sym);
         galign();
         H("L%s:\n",sym);
         H(" ORG*+%d\n",p->a[1]->x);
         break;
      case ILST: // params
         idlist(ILST, p, 5);
         break;
      case EXTDEF:
         idlist(EXTDEF, p->a[0], 0);
         break;
      case AUTODEF:
         if (0x0F != lowreg) {
            fprintf(stderr,"auto vars after register vars\n");
            exit(1);
         }
         offs = idlist(AUTODEF, p->a[0], -1);
         H(" ..AUTO SIZE %d,ADJUST SP\n",offs);
         offs++;
         if (offs) {
            H(" GLO SP ;SMI #%02X; PLO SP\n",LO(offs));
            H(" GHI SP ;SMBI #%02X; PHI SP\n",HI(offs));
         }
         break;
      case REGDEF:
         lowreg = idlist(REGDEF, p->a[0], 0x0F);
         H(" ..#REG VARS %d\n", 0x0F - lowreg);
         for (i = lowreg; i < 0x0F; i++)
            gpush(2,regnm(lowreg));
         break;
      case FUNDEF:
         sym = getsym(p->a[0]->x);
         H(" ..FN %s\n",fn = sym);
         lowreg = 0x0F;
         galign();
         H("L%s:\n",sym);
         gpush(2, "FP");
         gmov("FP", "SP");
         q = p->a[1];
         ex(q->a[0]); // ID list
         ex(q->a[1]); // stmt
         H("E%s:\n",sym); // fn epilogue
         for(i = 0x0F; i > lowreg; i--)
            gpop(2,regnm(i-1));
         gmov("SP", "FP");
         gpop(2, "FP");
         H(" SEP SRET\n");
         dropsyms();
         fn = NULL;
         break;
      case XLST: // expr list
         argcnt = exprlist(p, 0);
         break;
      case FUNCALL:
         sym = getsym(p->a[0]->x);
         H(" ..CALL %s\n",sym);
         argcnt = ex(p->a[1]); // push args
         H(" SEP SCALL,A(L%s)\n",sym);
         argcnt *= 2; // 2byte args
         if (argcnt) {
            if (argcnt < 7) {
               H(" ");
               for (i = 0; i < argcnt; i++) {
                  if (i) H(" ;");
                  H("IRX");
               }
               H("\n");
            }
            else {
               H(" GLO SP ;ADI #%02X ;PLO SP\n",LO(argcnt));
               H(" GHI SP ;ADCI #%02X ;PHI SP\n",HI(argcnt));
            }
         }
         break;
      case FOR:
         stmt = p->a[0]; q = p->a[1];
         ex(q->a[0]);    q = q->a[1];
         gwhile(q->a[0], stmt, q->a[1]);
         break;
      case WHILE:
         gwhile(p->a[0], p->a[1], NULL);
         break;
      case IF:
         ex(p->a[0]);
         q = p->a[1];
         H(" ..0==AC?\n");
         H(" GHI AC ;LBNZ L%04d\n", lbl1=lbl++);
         H(" GLO AC ;LBZ  L%04d\n", lbl2=lbl++);
         H("L%04d:\n",lbl1);
         ex(q->a[0]);
         if (q->a[1]) {  // IF expr THEN stmt1 ELSE stmt2
            H(" LBR L%04d\n",lbl3=lbl++);
            H("L%04d: ..ELSE\n",lbl2);
            ex(q->a[1]); 
            H("L%04d: ..END\n",lbl3);
         }
         else
            H("L%04d: ..END\n",lbl2);
         break;
      case RETURN:
         if (p->a[0])
            ex(p->a[0]);
         H(" LBR E%s\n", fn); // fn epilogue
         break;
      case ',': // fn params
         if (isvar(p->a[0]))
            gpushvar(p->a[0]);
         else {
            ex(p->a[0]);
            gpushac(2);
         }
         ex(p->a[1]);
         break;
      case ';':
         ex(p->a[0]);
         ex(p->a[1]);
         break;
      case ':':
         H(" ..LABEL\n");
         H("L%s:\n",getsym(p->a[0]->x));
         ex(p->a[1]);
         break;
      case '?':
         ex(p->a[0]); q = p->a[1];
         H(" ..0==AC?\n");
         H(" GLO AC ;STXD\n");
         H(" IRX ;GHI AC ;OR\n");
         H(" LBZ L%04d\n", lbl1=lbl++);
         ex(q->a[0]);
         H(" LBR L%04d\n", lbl2=lbl++);
         H("L%04d: ..ELSE", lbl1);
         ex(q->a[1]);
         H("L%04d: ..END\n", lbl2);
         break;
      case GOTO:
         if (C_LABEL != getcls(p->a[0]->x)) {
            fprintf(stderr,"not a label: %s\n",getsym(p->a[0]->x));
            exit(1);
         }
         H(" ..GOTO\n");
         H(" LBR L%s\n",getsym(p->a[0]->x));
         break;
      case '=':
         ex(p->a[1]);
         if (isvar(p->a[0])) {
            // MA = expr
            glvalu(p->a[0], 1);
            gstvar(p->a[0]);
         }
         else {
            // * expr = expr
            // expr '[' expr ']' = expr
            gpushac(2);
            glvalu(p->a[0], 0); // addr in MA
            H(" IRX ;LDXA ;PLO AC; STR MA; INC MA\n");
            H(" LDX ;PHI AC ;STR MA\n");
            // gpop(2, "AC");
            // gstvar(p->a[0]);
         }
         break;
      case  PREINC: gpreinc(p->a[0]); break;
      case  PREDEC: gpredec(p->a[0]); break;
      case POSTINC: gpostinc(p->a[0]); break;
      case POSTDEC: gpostdec(p->a[0]); break;
      case '[': H(" ..AREF\n"); glvalu(p, 0); gldvar(p); break;
      case UNARY + '-':
         if (isimm(p->a[0])) {
            i = -(p->a[0]->x);
            H(" ..AC=-%d\n",p->a[0]->x);
            H(" LDI #%02X ;PLO AC\n",LO(i));
            H(" LDI #%02X ;PHI AC\n",HI(i));
         } else {
            ex(p->a[0]);
            H(" ..NEG AC\n");
            H(" GLO AC ;SDI #00 ;PLO AC\n");
            H(" GHI AC ;SDBI #00 ;PHI AC\n");
         }
         break;
      case '~': ex(p->a[0]); gcom(); break;
      case '!': ex(p->a[0]); geq0(); break;
      case LAND:
         ex(p->a[0]);
         gtobool();
         H(" ..LAND: IF AC==0 SKIP\n");
         H(" LBZ L%04d\n", lbl1=lbl++);
         ex(p->a[1]);
         gtobool();
         H("L%04d: ..SKIP\n", lbl1);
         break;
      case LOR:
         ex(p->a[0]);
         gtobool();
         H(" ..LOR: IF AC==1 SKIP\n");
         H(" LBNZ L%04d\n", lbl1=lbl++);
         ex(p->a[1]);
         gtobool();
         H("L%04d: ..SKIP\n", lbl1);
         break;
      case UNARY + '&':
         glvalu(p->a[0], 0);
         H(" ..AC=WADDR(MA)\n");
         H(" GHI MA ;SHR  ;PHI AC\n");
         H(" GLO MA ;SHRC ;PLO AC\n");
         break;
      case UNARY + '*':
         ex(p->a[0]);
         H(" ..LD AC,[AC]\n");
         gmul2();
         H(" LDA AC ;PLO AUX\n");
         H(" LDN AC ;PHI AC\n");
         H(" GLO AUX ;PLO AC\n");
         break;
      default:
         switch (p->x) {
         case '+': gadd(p); break;
         case '-': gsub(p,0); break;
         case '*':
            if (!gspecmul(p)) {
               if (isvar(p->a[1]))
                  gpushvar(p->a[1]);
               else {
                  ex(p->a[1]);
                  gpushac(2);
               }
               ex(p->a[0]);
               // gpop(2, "AUX");
               H(" ..MUL AC,AUX\n");
               H(" LDI A.0(UMULT) ;PLO SUB\n");
               H(" SEP SUB\n");
            }
            break;
         case '/':
            if (!gspecdiv(p))
               gdiv(p);
            break;
         case '%':
            if (!gspecmod(p))
               gmod(p);
            break;
         case '<': glt(p); break;
         case '>': ggt(p); break;
         case GE:  gge(p); break;
         case LE:  ggt(p); gcom(); break;
         case NE:  geq(p); gcom(); break;
         case EQ:  geq(p); break;
         case '&': glog(p, "AND", "ANI"); break;
         case '|': glog(p, "OR", "ORI"); break;
         case '^': glog(p, "XOR", "XRI"); break;
         case SHR: gshr(p); break;
         case SHL: gshl(p); break;
         default:
            fprintf(stderr,"unknown node (%d,%d)!\n",p->t,p->x);
            exit(1);
         }
      }
      break;
   default:
      fprintf(stderr,"unknown node (%d,%d)!\n",p->t,p->x);
      exit(1);
   }

   return argcnt;
}

/* vim: set ts=3 sw=3 et: */

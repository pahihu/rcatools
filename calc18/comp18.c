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

#define  SP 2
#define  PC 3
#define  SCALL 4
#define  SRET  5
#define  ARGPTR   6
#define  AC 7
#define  RETVAL 7
#define  AUX 8
#define  SUB 9
#define  MA  10
#define  MQ  10
#define  TMP 11
#define  FP  15

#define Z   static void

int Err = 0;
char *fn;
int opttime = 1;
int nregvars; // no.of regvars
int regpar = 0; // no params in registers
int ex(NODE *p);
Z glvaluvar(NODE *p);
Z WPOP(int r);
Z WMOV(int dst, int src);

typedef struct {
   int prev; // prev switch
   int lend; // end of switch
   int ltab; // switch table
   int lmaxcase; // max of cases
   int tab[256];  // switch table labels (case branches)
} swdesc;

#define MAX_SWITCHES 4
int nswitches;
swdesc switches[MAX_SWITCHES];
int currsw = -1;
#define MAXAUTOS 8
int nautos = 0;
NODE *autos[MAXAUTOS];
int autooffs = -1; // next AUTO offset
int lastoprx = -1; // last OPR type
int nparams = 0; // no.of params
int needfp = 1; // need FP
int funhd = 0; // fn header


static int lbl = 0; // label generator

#define LO(x)   ((x) & 0xFF)
#define HI(x)   LO((x)>>8)

#define ADDR(p) (getoffs((p)->x))

static int xstrcmp(char *a,char *b) {
   if (!a || !b)
      return 1;
   return strcmp(a,b);
}

static int toprintable(int c) {
   if (!c)
      return c;
   return (31 < c && c < 128)? c : '.';
}

static char* printable(char *str) {
   static char buf[128], *p;
   int i;

   i = 0; p = buf;
   while (i++ < 127 && (*p++ = toprintable(*str++)));
   *p = '\0';
   return buf;
}

Z prinode1(FILE *fout,char *msg,NODE *p) {
   static char *typs[] = { "INT", "CON", "ID", "OPR" };

   if (!p) {
      fprintf(fout,"NULL\n");
      return;
   }
   fprintf(fout,"%s: t:%s x:%d\n",msg,typs[1+p->t],p->x);
   prinode1(fout,"A0",p->a[0]);
   prinode1(fout,"A1",p->a[1]);
}

Z prinode(NODE *p) {
   prinode1(stdout,"HEAD",p);
}

Z swpush(void) {
   assert(nswitches < MAX_SWITCHES + 1);

   memset(&switches[nswitches],0,sizeof(swdesc));
   switches[nswitches].lend = lbl++;
   switches[nswitches].ltab = lbl++;
   switches[nswitches].lmaxcase = lbl++;
   switches[nswitches].prev = currsw;
   currsw = nswitches;
   nswitches++;
}

Z swpop(void) {
   assert(-1 < currsw);
   currsw = switches[currsw].prev;
}

Z swdef(void) {
   int i, j, l;
   int maxcase;
   swdesc *sw;

   for (i = 0; i < nswitches; i++) {
      sw = &switches[i];
      maxcase = 0;
      for (j = 256; j; j--)
         if (sw->tab[j-1]) {
            maxcase = j;
            break;
         }
      if (0 == maxcase) {
         fprintf(stderr,"switch without cases\n");
         Err = 1;
      }
      H(" ..SWITCH%d\n",i);
      H("L%04d EQU %d\n",sw->lmaxcase,maxcase);
      H("L%04d:\n",sw->ltab);
      for (j = 0; j < maxcase; j++) {
         l = sw->tab[j]? sw->tab[j] : sw->lend;
         H(" DW A(L%04d)\n",l);
      }
   }
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
   char *buf[] = {
      "DMA",   "INT",  "SP",     "PC",
      "SCALL", "SRET", "ARGPTR", "AC",
      "AUX",   "SUB",  "MA",     "TMP",
      "RC",    "RD",   "RE",     "FP",
   };

   return buf[x];
}

static char *reg(int r) {
   return regnm(r);
}

static int isimm(NODE *p) {
   if (!p)
      return 0;
   return CON == p->t;
}

static int isstr(NODE *p) {
   if (!p)
      return 0;
   return STR == p->t;
}

static int isimm8(NODE *p) {
   return isimm(p) && !HI(p->x); 
}

static int isbyte(NODE *p) {
   return isvar(p) || isimm8(p);
}

static int isrelop(NODE *p) {
   int x;
   if (!p)
      return 0;

   x = p->x;
   return OPR == p->t &&
      (x == EQ || x == NE || x == '<' || x == '>' || x == LE || x == GE ||
       x == LAND || x == LOR || x == '!');
}

static int isdef(int x) {
   return (x == EXTDEF || x == AUTODEF || x == REGDEF);
}

Z gxopi(NODE *p) {
   NODE *q;

   if (!p)
      return;
   if (isimm(p->a[0])) {
      q = p->a[1]; p->a[1] = p->a[0]; p->a[0] = q;
   }
}

Z WINC(int r) {
   H(" INC %s\n",reg(r));
}

Z WDEC(int r) {
   H(" DEC %s\n",reg(r));
}

Z WADI(int dst,int src,int x) {
   int i;

   if (dst == src && x < 7) {
      H(" ");
      for (i = 0; i < x; i++) {
         if (i) H(" ;");
         H("INC %s",reg(src));
      }
      H("\n");
   }
   else {
      H(" GLO %s ;ADI #%02X ;PLO %s\n",reg(src),LO(x),reg(dst));
      H(" GHI %s ;ADCI #%02X ;PHI %s\n",reg(src),HI(x),reg(dst));
   }
}

Z WADD(int dst,int src) {
   H(" GLO %s ;IRX ;ADD ;PLO %s\n",reg(src),reg(dst));
   H(" GHI %s ;IRX ;ADC ;PHI %s\n",reg(src),reg(dst));
}

Z WADDR(int dst,int src0,int src1) {
   H(" GLO %s ;STR SP ;GLO %s ;ADD ;PLO %s\n",reg(src1),reg(src0),reg(dst));
   H(" GHI %s ;STR SP ;GHI %s ;ADC ;PHI %s\n",reg(src1),reg(src0),reg(dst));
}

Z WCMI(int r,int x) {
   H(" GLO %s ;SMI #%02X\n",reg(r),LO(x));
   H(" GHI %s ;SMBI #%02X\n",reg(r),HI(x));
}

Z WCD(int r) {
   H(" GLO %s ;IRX ;SD\n",reg(r));
   H(" GHI %s ;IRX ;SDB\n",reg(r));
}

Z _WSMI(int dst,int src,int x) {
   H(" GLO %s ;SMI #%02X ;PLO %s\n",reg(src),LO(x),reg(dst));
   H(" GHI %s ;SMBI #%02X ;PHI %s\n",reg(src),HI(x),reg(dst));
}

Z WSMI(int dst,int src,int x) {
   int i;

   if (dst == src && x < 7) {
      H(" ");
      for (i = 0; i < x; i++) {
         if (i) H(" ;");
         H("DEC %s",reg(src));
      }
      H("\n");
   }
   else
      _WSMI(dst,src,x);
}

Z WSDI(int dst,int src,int x) {
   H(" GLO %s ;SDI #%02X ;PLO %s\n",reg(src),LO(x),reg(dst));
   H(" GHI %s ;SDBI #%02X ;PHI %s\n",reg(src),HI(x),reg(dst));
}

Z WSM(int dst,int src) {
   H(" GLO %s ;IRX ;SM ;PLO %s\n",reg(src),reg(dst));
   H(" GHI %s ;IRX ;SMB ;PHI %s\n",reg(src),reg(dst));
}

Z WSD(int dst,int src) {
   H(" GLO %s ;IRX ;SD ;PLO %s\n",reg(src),reg(dst));
   H(" GHI %s ;IRX ;SDB ;PHI %s\n",reg(src),reg(dst));
}

Z WSMR(int dst,int src0,int src1) {
   H(" GLO %s ;STR SP ;GLO %s ;SM ;PLO %s\n",reg(src1),reg(src0),reg(dst));
   H(" GHI %s ;STR SP ;GHI %s ;SMB ;PHI %s\n",reg(src1),reg(src0),reg(dst));
}

Z WLOG(int dst,int src,char *xop) {
   H(" GLO %s ;IRX ;%s ;PLO %s\n",reg(src),xop,reg(dst));
   H(" GHI %s ;IRX ;%s ;PHI %s\n",reg(src),xop,reg(dst));
}

Z WLOGI(int dst,int src,int x,char *imop) {
   H(" GLO %s ;%s #%02X ;PLO %s\n",reg(src),imop,LO(x),reg(dst));
   H(" GHI %s ;%s #%02X ;PHI %s\n",reg(src),imop,HI(x),reg(dst));
}

Z WSHL(int dst,int src) {
   H(" ..SHL %s,%s\n",reg(dst),reg(src));
   H(" GLO %s ;SHL ;PLO %s\n",reg(src),reg(dst));
   H(" GHI %s ;SHLC ;PHI %s\n",reg(src),reg(dst));
}

Z WSHR(int dst,int src) {
   H(" ..SHR %s,%s\n",reg(dst),reg(src));
   H(" GHI %s ;SHR ;PHI %s\n",reg(src),reg(dst));
   H(" GLO %s ;SHRC ;PLO %s\n",reg(src),reg(dst));
}

Z WLDN(int dst,int src) {
   H(" LDA %s ;PLO %s\n",reg(src),reg(dst));
   H(" LDN %s ;PHI %s\n",reg(src),reg(dst));
}

Z WSTR(int dst,int src) {
   H(" ..STR %s,%s\n",reg(dst),reg(src));
   H(" GLO %s ;STR %s ;INC %s\n",reg(src),reg(dst),reg(dst));
   H(" GHI %s ;STR %s\n",reg(src),reg(dst));
}

Z WLDI(int r,int x) {
   H(" ..LDI %s,%02X%02X\n",reg(r),HI(x),LO(x));
   if (HI(x) == LO(x))
      H(" LDI #%02X ;PHI %s ;PLO %s\n",HI(x),reg(r),reg(r));
   else {
      H(" LDI #%02X ;PHI %s\n",HI(x),reg(r));
      H(" LDI #%02X ;PLO %s\n", LO(x),reg(r));
   }
}

Z WCOM(int dst,int src) {
   H(" ..COM %s,%s\n",reg(dst),reg(src));
   H(" GLO %s ;XRI #FF ;PLO %s\n",reg(src),reg(dst));
   H(" GHI %s ;XRI #FF ;PHI %s\n",reg(src),reg(dst));
}

Z WPUSH(int r) {
   H(" ..PUSH %s\n",reg(r));
   H(" GHI %s ;STXD\n",reg(r));
   H(" GLO %s ;STXD\n",reg(r));
}

Z WPUSHI(int x) {
   H(" ..PUSHI %02X%02X\n",HI(x),LO(x));
   H(" LDI #%02X ;STXD\n",HI(x));
   H(" LDI #%02X ;STXD\n",LO(x));
}

Z WPOP(int r) {
   H(" ..POP %s\n",reg(r));
   H(" IRX ;LDXA ;PLO %s\n", reg(r));
   H(" LDX ;PHI %s\n", reg(r));
}

Z WMOV(int dst, int src) {
   H(" ..MOV %s,%s\n",reg(dst),reg(src));
   H(" GLO %s ;PLO %s\n",reg(src),reg(dst));
   H(" GHI %s ;PHI %s\n",reg(src),reg(dst));
}

Z WMOV0(int dst, int src) {
   H(" GLO %s ;PLO %s\n",reg(src),reg(dst));
}

Z WLDD(void) {
   H(" ..AC=(D,D)\n");
   H(" PHI AC ;PLO AC\n");
}

Z gldvar(NODE *p) {
   WLDN(AC,MA);
}

Z gbinary(NODE *p) {
   if (p) {
      ex(p->a[0]);
      WPUSH(AC);
      ex(p->a[1]);
   }
}

Z gsub(NODE *p) {
   int imm1, x1;

   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;

      if (imm1 && 1 == x1) {
         ex(p->a[0]);
         WDEC(AC);
         return;
      }
      else if (imm1) {
         ex(p->a[0]);
         WSMI(AC,AC,x1);
         return;
      }
   }
   gbinary(p);
   WSD(AC,AC);
}

Z gcmp(NODE *p,int needres) {
   int imm1, x1;

   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;
      if (imm1) {
         ex(p->a[0]);
         needres? _WSMI(AC,AC,x1) : WCMI(AC,x1);
         return;
      }
   }
   gbinary(p);
   needres? WSD(AC,AC) : WCD(AC);
}

Z glt(NODE *p) {
   gcmp(p,0);
   // DF=0
   H(" ..LT?\n");
   H(" ..D=DF? #00 : #FF\n");
   H(" LDI #FF ;ADCI #00\n");  // DF=1 FALSE, DF=0 TRUE
   WLDD();
}

Z gge(NODE *p) {
   glt(p);
   WCOM(AC,AC);
}

Z ggt(NODE *p) {
   int lbl1;

   gcmp(p,1);
   H(" ..D=DF? XX : #00\n");
   H(" ..DF=0? RESULT IS ZERO\n");
   H(" LBNF L%04d\n", lbl1=lbl++);
   H(" ..IF AC=0 DF=0 ELSE DF=1\n");
   H(" LSNZ ; GLO AC ;LSZ\n");
   H(" LDI #01 ;SHR\n");
   H("L%04d:\n",lbl1);
   H(" ..AC=DF? #FF : #00\n");
   H(" LDI #00 ;LSNF ;LDI #FF\n");
   WLDD();
}

Z gle(NODE *p) {
   ggt(p);
   WCOM(AC,AC);
}

Z GNE0(void) {
   H(" GLO AC ;LSNZ\n");
   H(" GHI AC ;LSZ\n");
   H(" LDI #FF\n");
}

Z gne0(NODE *p) {
   H(" ..AC=0!=AC? #FFFF : #0000\n");
   GNE0();
   WLDD();
}

Z gne(NODE *p) {
   gcmp(p,1);
   gne0(p);
}

Z GEQ0(void) {
   GNE0();
   H(" XRI #FF\n");
}

Z geq0(NODE *p) {
   H(" ..AC=0==AC? #FFFF : #0000\n");
   GEQ0();
   WLDD();
}

Z geq(NODE *p) {
   gcmp(p,1);
   geq0(p);
}

Z gtobool(NODE *p) {
   H(" ..AC=AC? #FFFF : #0000\n");
   gne0(p);
}

Z gadd(NODE *p) {
   int imm1, x1;

   gxopi(p);
   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;

      if (imm1 && 1 == x1) {
         ex(p->a[0]);
         WINC(AC);
         return;
      }
      else if (imm1) {
         ex(p->a[0]);
         WADI(AC,AC,x1);
         return;
      }
   }
   gbinary(p);
   WADD(AC,AC);
}

Z glog(NODE *p, char *xop, char *imop) {
   int imm1, x1;

   gxopi(p);
   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;
      if (imm1) {
         ex(p->a[0]);
         WLOGI(AC,AC,x1,imop);
         return;
      }
   }
   gbinary(p);
   WLOG(AC,AC,xop);
}

Z gwhile(NODE *cond, NODE *body, NODE *end) {
   int lbl1, lbl2, lbl3;

   H("L%04d:\n", lbl1=lbl++);
   ex(cond);
   H(" ..0==AC?\n");
   H(" GHI AC ;LBNZ L%04d\n", lbl2=lbl++);
   H(" GLO AC ;LBZ L%04d\n", lbl3=lbl++);
   H("L%04d: ..BODY\n",lbl2);
   ex(body);
   if (end)
      ex(end);
   H(" LBR L%04d\n",lbl1);
   H("L%04d: ..END\n",lbl3);
}

Z glvaluvar(NODE *p) {
   int cls, offs;
   char *sym;

   assert(isvar(p));

   cls  = getcls(p->x);
   sym  = getsym(p->x);
   offs = getoffs(p->x);
   H(" ..LDI MA,ADDR(%s)\n",getsym(p->x));
   switch (cls) {
   case C_UNDEF:
      fprintf(stderr,"%s undefined\n",sym);
      Err = 1;
      defcls(p->x, C_EXTRN, 0);
      break;
   case C_EXTRN:
      H(" LDI A.0(L%s) ;PLO MA\n",sym);
      H(" LDI A.1(L%s) ;PHI MA\n",sym);
      break;
   case C_AUTO:
      if (!opttime && !HI(offs))
         H(" LDI A.0(AUT8); PLO SUB ;SEP SUB,#%02X\n",LO(offs));
      else
         WSMI(MA,FP,offs);
      break;
   case C_LABEL:
      fprintf(stderr,"glvalu(): %s is label\n",sym);
      exit(1);
      break;
   case C_PARAM:
      if (!opttime && !HI(offs))
         H(" LDI A.0(PAR8); PLO SUB ;SEP SUB,#%02X\n",LO(offs));
      else
         WADI(MA,FP,offs);
      break;
   default:
      fprintf(stderr,"glvalu(): unknown cls %d\n",cls);
      exit(1);
   }
}

Z gindex(NODE *p) {
   gbinary(p);
   WADD(MA,AC);
   WSHL(MA,MA);
}


Z glvalu(NODE *p) { // addr in MA

   if (isvar(p)) {
      glvaluvar(p);
      return;
   }
   else if (OPR == p->t) {
      if (UNARY + '*' == p->x) {
         ex(p->a[0]);
         H(" ..SHL MA,AC\n");
         WSHL(MA,AC);
         return;
      }
      else if ('[' == p->x) {
         gindex(p);
         return;
      }
   }
   fprintf(stderr,"unknown lvalu: t=%d x=%d\n",p->t,p->x);
   abort();
}

Z gshl(NODE *p) {
   int lbl1, lbl2;
   int imm1, x1;

   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;
      if (imm1 && 1 == x1) {
         ex(p->a[0]);
         WSHL(AC,AC);
         return;
      }
   }
   gbinary(p);
   WMOV0(AUX,AC);
   WPOP(AC);

   H(" ..0==AUX.0?\n");
   H(" GLO AUX ;LBZ L%04d\n", lbl1=lbl++);
   H("L%04d: ..SHL LOOP\n", lbl2=lbl++);
   WSHL(AC,AC);
   H(" ..IF (--AUX.0) GOTO LOOP\n");
   H(" DEC AUX ;GLO AUX ;LBNZ L%04d\n", lbl2);
   H("L%04d:\n", lbl1);
}

Z gshr(NODE *p) {
   int lbl1, lbl2;
   int imm1, x1;

   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;
      if (imm1 && 1 == x1) {
         ex(p->a[0]);
         WSHR(AC,AC);
         return;
      }
   }
   gbinary(p);
   WMOV0(AUX,AC);
   WPOP(AC);

   H(" ..0==AUX.0?\n");
   H(" GLO AUX ;LBZ L%04d\n", lbl1=lbl++);
   H("L%04d: ..LOOP\n", lbl2=lbl++);
   WSHR(AC,AC);
   H(" ..IF (--AUX.0) GOTO LOOP\n");
   H(" DEC AUX ;GLO AUX ;LBNZ L%04d\n", lbl2);
   H("L%04d:\n", lbl1);
}

Z gstvar(NODE *p) {
   WSTR(MA,AC); 
}

Z gpreinc(NODE *p) {
   ex(p);
   WINC(AC);
   WDEC(MA);
   gstvar(p);
}

Z gpredec(NODE *p) {
   ex(p);
   WDEC(AC);
   WDEC(MA);
   gstvar(p);
}

Z gpostinc(NODE *p) {
   gpreinc(p);
   WDEC(AC);
}

Z gpostdec(NODE *p) {
   gpredec(p);
   WINC(AC);
}

Z gcall(char *subr) {
   H(" ..CALL %s\n",subr);
   // NB. all runtime routines should be on the same page!
   // H(" LDI A.1(%s) ;PHI SUB\n",subr);
   H(" LDI A.0(%s) ;PLO SUB\n",subr);
   H(" SEP SUB\n");
}

Z gdiv(NODE *p) {
   int imm1, x1;

   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;
      if (imm1 && 2 == x1) {
         ex(p->a[0]);
         WSHR(AC,AC);
         return;
      }
   }
   gbinary(p);
   // dividend on stack, divisor in AC
   WPOP(AUX);
   WPUSH(AC);
   WMOV(AC,AUX);
   gcall("UDIV");
}

static int idlist(int t,NODE *p, int offs) {
   int n, newoffs;
   char *sym;
   NODE *q;

   if (!p)
      return offs;

   assert(OPR == p->t && ILST == p->x);
   newoffs = idlist(t,p->a[0],offs);
   q = p->a[1];
   n = q->x;
   switch (t) {
   case ILST:
      assert(isvar(q));
      newoffs += 2;
      // fprintf(stderr,"defpar%d: %s\n",offs,getsym(n));
      defcls(n, C_PARAM, newoffs);
      break;
   case EXTDEF:
      assert(isvar(q));
      // fprintf(stderr,"defext: %s\n",getsym(n));
      defcls(n, C_EXTRN, 0);
      break;
   case AUTODEF:
   case REGDEF:
      assert(isvar(q));
      q = q->a[0]; // storage size
      if (!q) // VAR, size is 2
         newoffs += 2;
      else { // VEC, size is vector size + ptr size
         assert(CON == q->t);
         newoffs += q->x + 2;
      }
      // fprintf(stderr,"defauto: %s %d\n",getsym(n), newoffs);
      defcls(n, C_AUTO, newoffs);
      break;
   case INIVPTR: // vector ptr inits at run-time
      assert(isvar(q));
      q = q->a[0];
      if (q) {
         //  VEC0.1
         //  VEC0.0 <-+
         //  PTR.1    |
         //  PTR.0  --+
         offs = getoffs(n);
         sym = getsym(n);
         H(" ..INIT %s=&%s[0]\n",sym,sym);
         offs -= 2;
         WSMI(MA,FP,offs);
         WSHR(AUX,MA);
         WDEC(MA); WDEC(MA);
         WSTR(MA,AUX);
      }
      break;
   case VARDEF:
   case VECDEF:
      assert(isvar(q) || isimm(q) || isstr(q));
      newoffs += 2;
      if (isvar(q))
         H(" DW A(L%s)\n",getsym(n));
      else if (isimm(q))
         H(" DW #%02X%02X\n",LO(n),HI(n));
      else if (isstr(q)) {
         H(" ..STR %s [%d]\n",printable(q->s),q->x=lbl++);
         H(" DB A.0(L%d SHR 1),A.1(L%d SHR 1)\n",q->x,q->x);
      }
   }
   return newoffs;
}

static int revidlist(int t,NODE *p, int offs) {
   int n, newoffs;
   NODE *q;

   if (!p)
      return offs;

   assert(OPR == p->t && ILST == p->x);
   q = p->a[1];
   n = q->x;
   assert(isvar(q));
   // fprintf(stderr,"defpar%d: %s\n",offs,getsym(n));
   defcls(n, C_PARAM, offs);
   return revidlist(t,p->a[0],2+offs);
}

static int exprlist(NODE *p, int offs, int nargs) {
   int n, x1;
   NODE *q;
   char *s1;

   if (!p)
      return offs;

   assert(OPR == p->t && XLST == p->x);
   H(" ..PUSH ARG%d\n",nargs - offs++);
   ex(p->a[1]);
   WPUSH(AC);
   return exprlist(p->a[0],offs,nargs);
}

static int len(NODE *p, int t) {
   int n, x1;
   NODE *q;
   char *s1;

   if (!p)
      return 0;

   assert(OPR == p->t && t == p->x);
   return len(p->a[0], t) + 1;
}

Z galign(void) {
   H(" ..ALIGN\n");
   H(" ORG*+(* AND 1)\n");
}

Z gmod(NODE *p) { gdiv(p); WMOV(AC,MQ); }
Z gmul(NODE *p) {
   int imm1, x1;

   gxopi(p);
   if (p) {
      imm1 = isimm(p->a[1]);
      x1   = p->a[1]->x;

      if (imm1 && 2 == x1) {
         ex(p->a[0]);
         WSHL(AC,AC);
         return;
      }
   }
   gbinary(p);
   gcall("UMULT");
}

Z gand(NODE *p) { glog(p, "AND", "ANI"); }
Z gxor(NODE *p) { glog(p, "XOR", "XRI"); }
Z gor(NODE *p)  { glog(p, "OR", "ORI");  }

Z gasgnop(NODE *p) {
   char *reg0, *reg1;
   int imm1, x;

   glvalu(p->a[0]);
   WPUSH(MA);
   gldvar(p->a[0]);
   WPUSH(AC);
   ex(p->a[1]);

   switch (p->x) {
   case AOR:  gor(NULL); break;
   case AAND: gand(NULL); break;
   case AEQ:  geq(NULL); break;
   case ANE:  gne(NULL); break;
   case ALT:  glt(NULL); break;
   case AGE:  gge(NULL); break;
   case ALE:  gle(NULL); break;
   case AGT:  ggt(NULL); break;
   case ASHL: gshl(NULL); break;
   case ASHR: gshr(NULL); break;
   case AADD: gadd(NULL); break;
   case ASUB: gsub(NULL); break;
   case AMOD: gmod(NULL); break;
   case AMUL: gmul(NULL); break;
   case ADIV: gdiv(NULL); break;
   default:
      fprintf(stderr,"unknown asgn op %d\n",p->x);
      exit(1);
   }

   // AC - result, stack lvalu addr
   WPOP(MA);
   WSTR(MA,AC);
}

Z gasgn(NODE *p) {
   glvalu(p->a[0]); // MA is the addr
   WPUSH(MA);
   ex(p->a[1]);
   WPOP(MA);
   WSTR(MA,AC);
}

Z glnot(NODE *p) {
   ex(p->a[0]);
   geq0(p->a[0]);
}

Z gland(NODE *p) {
   int lbl1;

   ex(p->a[0]);
   gtobool(p->a[0]);
   // D contains flag
   H(" ..LAND: IF AC==0 SKIP\n");
   H(" LBZ L%04d\n", lbl1=lbl++);
   ex(p->a[1]);
   gtobool(p->a[1]);
   H("L%04d: ..SKIP\n", lbl1);
   WLDD();
}

Z glor(NODE *p) {
   int lbl1;

   ex(p->a[0]);
   gtobool(p->a[0]);
   H(" ..LOR: IF AC==1 SKIP\n");
   H(" LBNZ L%04d\n", lbl1=lbl++);
   ex(p->a[1]);
   gtobool(p->a[1]);
   H("L%04d: ..SKIP\n", lbl1);
   WLDD();
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
      WLDI(AC,p->x);
      break;
   case STR:
      H(" ..STR %s [%d]\n",printable(p->s),p->x=lbl++);
      H(" LDI A.1(L%d SHR 1) ;PHI AC\n",p->x);
      H(" LDI A.0(L%d SHR 1) ;PLO AC\n",p->x);
      break;
   case ID: glvalu(p); gldvar(p); break;
   case OPR:
      if (';' != p->x && !isdef(p->x) && (isdef(lastoprx) || funhd)) {
         // def inits
         if (regpar)
            needfp = nparams > 2 || (autooffs + 1);
         if (needfp) {
            if (!opttime && !HI(autooffs+1))
               H(" LDI A.0(SUBENT) ;PLO SUB ;SEP SUB,#%02X\n",LO(autooffs+1));
            else {
               WPUSH(FP);
               WMOV(FP,SP);
               if (autooffs + 1) {
                  H(" ..AUTO OFFSET %d\n",autooffs);
                  H(" ..SMI SP,SP,%d\n",autooffs+1);
                  WSMI(SP,SP,autooffs + 1);
               }
            }
            if (autooffs + 1) {
               // init vector ptrs
               for (i = 0; i < nautos; i++)
                  idlist(INIVPTR,autos[i],0);
            }
         }
         funhd = 0;
      }
      lastoprx = p->x;
      switch (p->x) {
      case VARDEF:
         sym = getsym(p->a[0]->x);
         defcls(p->a[0]->x,C_EXTRN,0);
         H(" ..EXTRN %s\n",sym);
         galign();
         q = p->a[1];
         H("L%s:\n",sym);
         // either init list, or set to zero
         // NB. init may be a list
         if (q->a[1])
            idlist(VARDEF,q->a[1],0);
         else
            H(" DW 0\n");
         break;
      case VECDEF:
         sym = getsym(p->a[0]->x);
         defcls(p->a[0]->x,C_EXTRN,0);
         q = p->a[1];
         H(" ..EXTRN %s[]\n",sym);
         galign();
         H("L%s:\n",sym);
         // 1st word is ptr
         H(" DB A.0((L%s+2) SHR 1),A.1((L%s+2) SHR 1)\n",sym,sym);
         // inlude ptr size in vec size
         q->a[0]->x += 2;
         offs = idlist(VECDEF,q->a[1],0);
         // if init size is less than vector size
         // bump org
         if (offs < q->a[0]->x)
            H(" ORG L%s+%d\n",sym,q->a[0]->x);
         break;
      case ILST: // params
         idlist(ILST, p, 3);
         break;
      case EXTDEF:
         idlist(EXTDEF, p->a[0], 0);
         break;
      case AUTODEF:
      case REGDEF:
         autooffs = idlist(p->x, p->a[0], autooffs);
         if (MAXAUTOS == nautos) {
            fprintf(stderr,"too much auto defs (max 8)\n");
            Err = 1;
         }
         else
            autos[nautos++] = p->a[0];
         break;
      case SWITCH:
         swpush();
         ex(p->a[0]);
         H(" GLO AC ;SMI L%04d\n",switches[currsw].lmaxcase);
         H(" LBDF L%04d\n",switches[currsw].lend);
         H(" LDI A.1(L%04d) ;STXD\n",switches[currsw].ltab);
         H(" LDI A.0(L%04d) ;STR SP\n",switches[currsw].ltab);
         gcall("SWITCH");
         ex(p->a[1]);
         H("L%04d:\n",switches[currsw].lend);
         swpop();
         break;
      case CASE:
         if (currsw < 0) {
            fprintf(stderr, "no switch\n");
            Err = 1;
         }
         if (!isimm(p->a[0])) {
            fprintf(stderr, "case expr is not constant\n");
            exit(1);
         }
         i = LO(p->a[0]->x);
         H("L%04d: ..CASE %d\n",lbl,i);
         switches[currsw].tab[i] = lbl++;
         break;
      case FUNDEF:
         sym = getsym(p->a[0]->x);
         if (varstat || dbg)
            fprintf(stderr,"FN %s\n",sym);
         H(" ..FN %s\n",fn = sym);
         nswitches = 0; currsw = -1;
         lastoprx = -1; autooffs = -1; nautos = 0;
         nparams = 0; needfp = 1;
         galign();
         H("L%s:\n",sym);
         q = p->a[1];
         nparams = len(q->a[0],ILST);
         ex(q->a[0]); // ID list
         funhd = 1;
         ex(q->a[1]); // stmt
         H("E%s:\n",sym); // fn epilogue
         if (needfp) {
            if (!opttime)
               H(" LDI A.0(SUBRET) ;PLO SUB ;SEP SUB\n");
            else {
               WMOV(SP, FP);
               WPOP(FP);
            }
         }
         H(" SEP SRET\n");
         swdef();
         dropsyms();
         fn = NULL;
         break;
      case XLST: // expr list
         argcnt = len(p, XLST);
         exprlist(p, 0, argcnt);
         break;
      case FUNCALL:
         sym = getsym(p->a[0]->x);
         if (C_UNDEF == getcls(p->a[0]->x)) // see bref 6.1
            defcls(p->a[0]->x,C_EXTRN,0);
         argcnt = ex(p->a[1]); // push args
         H(" ..CALL %s\n",sym);
         H(" SEP SCALL,A(L%s)\n",sym);
         argcnt *= 2; // 2byte args
         if (argcnt) {
            WADI(SP,SP,argcnt);
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
         {
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
         }
         break;
      case RETURN:
         if (p->a[0])
            ex(p->a[0]);
         H(" LBR E%s\n", fn); // fn epilogue
         break;
      case ',': // fn params
         ex(p->a[0]);
         WPUSH(AC);
         ex(p->a[1]);
         break;
      case ';':
         ex(p->a[0]);
         ex(p->a[1]);
         break;
      case ':':
         H(" ..LABEL\n");
         if (!getoffs(p->a[0]->x))
            setoffs(p->a[0]->x,lbl++);
         H("L%04d:\n",getoffs(p->a[0]->x));
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
            Err = 1;
         }
         H(" ..GOTO\n");
         if (!getoffs(p->a[0]->x))
            setoffs(p->a[0]->x,lbl++);
         H(" LBR L%04d\n",getoffs(p->a[0]->x));
         break;
      case '=': H(" ..ASSIGN\n"); gasgn(p); break;
      case AOR: case AAND:
      case AEQ: case ANE: case ALT: case ALE: case AGT: case AGE:
      case ASHL: case ASHR:
      case AADD: case ASUB: case AMOD: case AMUL: case ADIV:
         gasgnop(p);
         break;
      case  PREINC: gpreinc(p->a[0]); break;
      case  PREDEC: gpredec(p->a[0]); break;
      case POSTINC: gpostinc(p->a[0]); break;
      case POSTDEC: gpostdec(p->a[0]); break;
      case '[': H(" ..AREF\n"); glvalu(p); gldvar(p); break;
      case UNARY + '-':
         ex(p->a[0]);
         H(" ..NEG AC\n");
         WSDI(AC,AC,0);
         break;
      case '~': ex(p->a[0]); WCOM(AC,AC); break;
      case '!': glnot(p); break;
      case LAND: gland(p); break;
      case LOR: glor(p); break;
      case UNARY + '&':
         glvalu(p->a[0]);
         WSHR(AC,MA);
         break;
      case UNARY + '*':
         ex(p->a[0]);
         H(" ..DEREF\n");
         WSHL(AUX,AC);
         WLDN(AC,AUX);
         break;
      default:
         switch (p->x) {
         case '+': gadd(p); break;
         case '-': gsub(p); break;
         case '*': gmul(p); break;
         case '/': gdiv(p); break;
         case '%': gmod(p); break;
         case '<': glt(p); break;
         case '>': ggt(p); break;
         case GE:  gge(p); break;
         case LE:  gle(p); break;
         case NE:  gne(p); break;
         case EQ:  geq(p); break;
         case '&': gand(p); break;
         case '|': gor(p); break;
         case '^': gxor(p); break;
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

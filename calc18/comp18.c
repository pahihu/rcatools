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

#define Z   static void

int Err = 0;
char *fn;
int lowreg; // lowest regvar
int nregvars; // no.of regvars
int ex(NODE *p);
Z glvaluvar(NODE *p, int regena, int pushma);
Z WPOP(char *reg);

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
      "R0", "R1", "R2", "R3",
      "R4", "R5", "R6", "R7",
      "R8", "R9", "RA", "RB",
      "RC", "RD", "RE", "RF",
   };

   return buf[x];
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
       x == '!');
}

Z garith(char *dst,NODE *arg0,NODE *arg1,
         char *xop,char *imop,
         char *xophi,char *imophi)
{
   char *reg0, *reg1;
   int imm0, imm1, x0, x1;

   reg0 = isreg(arg0)? reg(arg0) : NULL;
   reg1 = isreg(arg1)? reg(arg1) : NULL;
   imm0 = isimm(arg0);
   imm1 = isimm(arg1);
   x0 = arg0->x;
   x1 = arg1->x;

   if (!xophi) xophi = xop;
   if (!imophi) imophi = imop;

Lreg0:
   if (reg0) {
      if (reg1) { // reg0 op reg1
         H(" GLO %s ;STR SP ;GLO %s ;%s ;PLO %s\n",reg1,reg0,xop,dst);
         H(" GHI %s ;STR SP ;GHI %s ;%s ;PHI %s\n",reg1,reg0,xophi,dst);
      }
      else if (imm1) { // reg0 op imm1
         H(" GLO %s ;%s %02X ;PLO %s\n",reg0,imop,LO(x1),dst);
         H(" GHI %s ;%s %02X ;PHI %s\n",reg0,imophi,HI(x1),dst);
      }
      else { // reg0 op stack
         H(" GLO %s ;IRX ;%s ;PLO %s\n",reg0,xop,dst);
         H(" GHI %s ;IRX ;%s ;PHI %s\n",reg0,xophi,dst);
      }
   }
   else if (imm0) {
      if (reg1) { // imm0 op reg1
         H(" GLO %s ;STR SP ;LDI #%02X ;%s ;PLO %s\n",reg1,LO(x0),xop,dst);
         H(" GLO %s ;STR SP ;LDI #%02X ;%s ;PHI %s\n",reg1,HI(x0),xophi,dst);
      }
      else if (imm1) { // imm0 op imm1
         fprintf(stderr,"garith(): imm0 op imm1\n");
         exit(1);
      }
      else { // imm0 op stack
         H(" LDI #%02X ;IRX ;%s ;PLO %s\n",LO(x0),xop,dst);
         H(" LDI #%02X ;IRX ;%s ;PLO %s\n",HI(x0),xophi,dst);
      }
   }
   else {
      if (!strcmp(xop,"SM")) {
         xop = "SD"; xophi = "SDB";
         imop = "SDI"; imophi = "SDBI";
      }
      if (reg1) { // stack op reg1
         H(" GLO %s ;IRX ;%s ;PLO %s\n",reg1,xop,dst);
         H(" GHI %s ;IRX ;%s ;PHI %s\n",reg1,xophi,dst);
      }
      else if (imm1) { // stack op imm1
         H(" LDI #%02X ;IRX ;%s ;PLO %s\n",LO(x1),xop,dst);
         H(" LDI #%02X ;IRX ;%s ;PHI %s\n",HI(x1),xop,dst);
      }
      else { // stack0 op stack1
         // ( stack1 stack0 -- stack0 op stack1)
         // pop stack0 into AC
         WPOP("AC"); reg0 = "AC"; goto Lreg0;
      }
   }
}

Z WADI(char *dst,char *src,int x) {
   H(" GLO %s ;ADI #%02X ;PLO %s\n",src,LO(x),dst);
   H(" GHI %s ;ADCI #%02X ;PHI %s\n",src,HI(x),dst);
}

Z WADD(char *dst,char *src) {
   H(" GLO %s ;IRX ;ADD ;PLO %s\n",src,dst);
   H(" GHI %s ;IRX ;ADC ;PHI %s\n",src,dst);
}

Z WADDR(char *dst,char *src0,char *src1) {
   H(" GLO %s ;STR SP ;GLO %s ;ADD ;PLO %s\n",src1,src0,dst);
   H(" GLO %s ;STR SP ;GHI %s ;ADC ;PHI %s\n",src1,src0,dst);
}

Z WCMI(char *reg,int x) {
   H(" GLO %s ;SMI #%02X\n", reg,LO(x));
   H(" GHI %s ;SMBI #%02X\n", reg,HI(x));
}

Z WCM(char *reg) {
   H(" GLO %s ;IRX ;SM\n",reg);
   H(" GHI %s ;IRX ;SMB\n",reg);
}

Z WCMR(char *reg0, char *reg1) {
   H(" GLO %s ;STR SP ;GLO %s ;SM\n", reg1, reg0);
   H(" GHI %s ;STR SP ;GHI %s ;SMB\n", reg1, reg0);
}

Z WSMI(char *dst,char *src,int x) {
   H(" GLO %s ;SMI #%02X ;PLO %s\n", src,LO(x),dst);
   H(" GHI %s ;SMBI #%02X ;PHI %s\n", src,HI(x),dst);
}

Z WSM(char *dst,char *src) {
   H(" GLO %s ;IRX ;SM ;PLO %s\n",src,dst);
   H(" GHI %s ;IRX ;SMB ;PHI %s\n",src,dst);
}

Z WSMR(char *dst,char *src0,char *src1) {
   H(" GLO %s ;STR SP ;GLO %s ;SM ;PLO %s\n",src1,src0,dst);
   H(" GHI %s ;STR SP ;GHI %s ;SMB ;PHI %s\n",src1,src0,dst);
}

Z WSHL(char *dst,char *src) {
   H(" ..SHL %s,%s\n",dst,src);
   H(" GLO %s ;SHL ;PLO %s\n",src,dst);
   H(" GHI %s ;SHLC ;PHI %s\n",src,dst);
}

Z WSHR(char *dst,char *src) {
   H(" ..SHR %s,%s\n",dst,src);
   H(" GHI %s ;SHR ;PHI %s\n",src,dst);
   H(" GLO %s ;SHRC ;PLO %s\n",src,dst);
}

Z WLDI(char *reg,int x) {
   H(" ..LDI %s,%02X%02X\n",reg,HI(x),LO(x));
   if (HI(x) == LO(x))
      H(" LDI #%02X ;PHI %s ;PLO %s\n",HI(x),reg,reg);
   else {
      H(" LDI #%02X ;PHI %s\n",HI(x),reg);
      H(" LDI #%02X ;PLO %s\n", LO(x),reg);
   }
}

Z WCOM(char *dst,char *src) {
   H(" ..COM %s,%s\n",dst,src);
   H(" GLO %s ;XRI #FF ;PLO %s\n",src,dst);
   H(" GHI %s ;XRI #FF ;PHI %s\n",src,dst);
}

Z _gpush(int n, char *reg) {
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

Z WPUSH(char *reg) {
   _gpush(2,reg);
}

Z WPUSH0(char *reg) {
   _gpush(1,reg);
}

Z WPUSHI(int x) {
   H(" ..PUSHI %02X%02X\n",HI(x),LO(x));
   H(" LDI #%02X ;STXD\n",HI(x));
   H(" LDI #%02X ;STXD\n",LO(x));
}

Z _gpop(int n, char *reg) {
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

Z WPOP0(char *reg) {
   _gpop(1,reg);
}

Z WPOP(char *reg) {
   _gpop(2,reg);
}

Z WMOV(char *dst, char *src) {
   H(" ..MOV %s,%s\n",dst,src);
   H(" GLO %s ;PLO %s\n",src,dst);
   H(" GHI %s ;PHI %s\n",src,dst);
}

Z gdtoac(void) {
   H(" ..AC=(D,D)\n");
   H(" PHI AC ;PLO AC\n");
}

Z gpushvar(NODE *p) {
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

Z gldvar(NODE *p) {
   if (isreg(p)) {
      WMOV("AC", reg(p));
   }
   else {
      H(" LDA MA ;PLO AC\n");
      H(" LDN MA ;PHI AC\n");
   }
}

Z gbinary(NODE *p) {
   char *reg0,*reg1;

   reg0 = isreg(p->a[0])? reg(p->a[0]) : NULL;
   reg1 = isreg(p->a[1])? reg(p->a[1]) : NULL;
   if (reg1)
      p->a[1]->r = reg1;
   else if (isvar(p->a[1])) {
      if (reg0) {
         glvaluvar(p->a[1], 1, 0);
         gldvar(p->a[1]);
         p->a[1]->r = "AC";
      }
      else
         gpushvar(p->a[1]);
   }
   else {
      ex(p->a[1]);
      WPUSH("AC");  // (LO,HI) @ X
   }
   if (reg0)
      p->a[0]->r = reg0;
   else {
      ex(p->a[0]); // AC
      p->a[0]->r = "AC";
   }
}

Z gsub(NODE *p, int cmp) {
   NODE *con;
   char *r, *reg0, *reg1;

   r = "AC";
   if (isimm(p->a[1])) {
      con = p->a[1];

      if (isreg(p->a[0]))
         r = reg(p->a[0]);
      else
         ex(p->a[0]);
      if (1 == con->x && !cmp) {
         if (strcmp(r,"AC"))
            WMOV("AC",r);
         H(" DEC AC\n");
      }
      else {
         /* NB. ggt() calls with cmp=2, because it needs the AC val */
         if (1 == cmp) {
            H(" ..CMI %s,%04X\n",r,con->x);
            WCMI(r,con->x);
         }
         else {
            H(" ..SMI AC,%s,%04X\n",r,con->x);
            WSMI("AC",r,con->x);
         }
      }
   }
   else {
      gbinary(p);
      reg0 = p->a[0]->r; reg1 = p->a[1]->r;
      if (reg0 && reg1) {
         if (1 == cmp) {
            H(" ..CMR %s,%s\n",reg0,reg1);
            WCMR(reg0,reg1);
            return;
         }
         else {
            H(" ..SMR AC,%s,%s\n",reg0,reg1);
            WSMR("AC",reg0,reg1);
            return;
         }
      }
      if (1 == cmp) {
         H(" ..CM %s,*SP++\n",reg0);
         WCM(r);
      }
      else {
         H(" ..SM AC,%s,*SP++\n",reg0);
         WSM("AC",r);
      }
   }
}

Z GLT(void) {
   H(" LDI #FF ;ADCI #00\n");  // DF=1 FALSE, DF=0 TRUE
}

Z glt(NODE *p) {
   gsub(p,1); // A - AUX @ X
   // DF=0
   H(" ..LT?\n");
   H(" ..D=DF? #00 : #FF\n");
   GLT();
   if (0 == (p->attr & A_SIMCMP))
      gdtoac();
}

Z gge(NODE *p) {
   gsub(p,1);
   H(" ..D=DF? #FF : #00\n");
   GLT();
   H(" XRI #FF\n");
   if (0 == (p->attr & A_SIMCMP))
      gdtoac();
}

Z GGT(void) {
   int lbl1;

   H(" ..D=DF? XX : #00\n");
   H(" ..DF=0? RESULT IS ZERO\n");
   H(" LBNF L%04d\n", lbl1=lbl++);
   H(" ..IF AC=0 DF=0 ELSE DF=1\n");
   H(" LSNZ ; GLO AC ;LSZ\n");
   H(" LDI #01 ;SHR\n");
   H("L%04d:\n",lbl1);
   H(" ..AC=DF? #FF : #00\n");
   H(" LDI #00 ;LSNF ;LDI #FF\n");
}

Z ggt(NODE *p) {

   if (isimm(p->a[0]) && p->a[0]->x) {
      // (4 > a) == (3 >= a)
      p->a[0]->x--;
      gge(p);
      return;
   }
   if (isimm(p->a[1]) && 65535 != p->a[1]->x) {
      // (a > 2) == (a >= 3)
      p->a[1]->x++;
      gge(p);
      return;
   }
   gsub(p,2);
   GGT();
   if (0 == (p->attr & A_SIMCMP))
      gdtoac();
}

Z gle(NODE *p) {
   if (isimm(p->a[1]) && 65535 != p->a[1]->x) {
      // a <= 4 == a < 5
      p->a[1]->x++;
      glt(p);
      return;
   }
   if (isimm(p->a[0]) && p->a[0]->x) {
      // 4 <= a == 3 < a
      p->a[0]->x--;
      glt(p);
      return;
   }
   ggt(p);
   if (p->attr & A_SIMCMP)
      H(" XRI #FF\n");
   else
      WCOM("AC","AC");
}

Z GNE0(void) {
   H(" GLO AC ;LSNZ\n");
   H(" GHI AC ;LSZ\n");
   H(" LDI #FF\n");
}

Z gne0(NODE *p) {
   H(" ..AC=0!=AC? #FFFF : #0000\n");
   GNE0();
   if (0 == (p->attr & A_SIMCMP))
      gdtoac();
}

Z gne(NODE *p) {
   gsub(p,0);
   gne0(p);
}

Z GEQ0(void) {
   GNE0();
   H(" XRI #FF\n");
}

Z geq0(NODE *p) {
   H(" ..AC=0==AC? #FFFF : #0000\n");
   GEQ0();
   if (0 == (p->attr & A_SIMCMP))
      gdtoac();
}

Z geq(NODE *p) {
   gsub(p,0);
   geq0(p);
}

Z gtobool(void) {
   H(" ..AC=AC? #FFFF : #0000\n");
   GNE0();
   gdtoac();
}


Z gadd(NODE *p) {
   NODE *con;
   int n;
   char *r, *reg0, *reg1;

   r = "AC";
   if (CON == p->a[1]->t) {
      con = p->a[1];

      if (isreg(p->a[0]))
         r = reg(p->a[0]);
      else
         ex(p->a[0]);
      if (1 == con->x) {
         if (strcmp(r,"AC"))
            WMOV("AC",r);
         H(" INC AC\n");
      }
      else {
         H(" ..ADI AC,%s,%04X\n",r,con->x);
         WADI("AC",r,con->x);
      }
   }
   else {
      gbinary(p);
      reg0 = p->a[0]->r; reg1 = p->a[1]->r;
      if (reg0 && reg1) {
         H(" ..ADDR AC,%s,%s\n",reg0,reg1);
         WADDR("AC",reg0,reg1);
      }
      else {
         H(" ..ADD AC,%s,*SP++\n",reg0);
         WADD("AC",reg0);
      }
   }
}

Z glog(NODE *p, char *xop, char *imop) {
   NODE *con;
   char *r, *reg0, *reg1;

   r = "AC";
   if (CON == p->a[1]->t) {
      con = p->a[1];

      if (isreg(p->a[0]))
         r = reg(p->a[0]);
      else
         ex(p->a[0]);
      H(" ..%s AC,%s,%04X\n",imop,r,con->x);
      H(" GLO %s ;%s #%02X ;PLO AC\n", r, imop, LO(con->x));
      H(" GHI %s ;%s #%02X ;PHI AC\n", r, imop, HI(con->x));
   }
   else {
      gbinary(p);
      reg0 = p->a[0]->r; reg1 = p->a[1]->r;
      if (reg0 && reg1) {
         H(" ..%sR AC,%s,%s\n",xop,reg0,reg1);
         H(" GLO %s ;STR SP ;GLO %s ;%s ;PLO AC\n", reg1, reg0, xop);
         H(" GHI %s ;STR SP ;GHI %s ;%s ;PHI AC\n", reg1, reg0, xop);
      }
      else {
         H(" ..%s AC,%s,*SP++\n",xop,reg0);
         H(" GLO %s ;IRX ;%s ;PLO AC\n", reg0, xop);
         H(" GHI %s ;IRX ;%s ;PHI AC\n", reg0, xop);
      }
   }
}

Z gwhile(NODE *cond, NODE *body, NODE *end) {
   int lbl1, lbl2, lbl3;
   int simcmp;

   simcmp = 0;
   H("L%04d:\n", lbl1=lbl++);
   if (isrelop(cond)) {
      simcmp = 1;
      cond->attr |= A_SIMCMP;
   }
   ex(cond);
   H(" ..0==AC?\n");
   if (simcmp) {
      // D contains either zero/or non-zero
      H(" LBZ L%04d\n", lbl3=lbl++);
   }
   else {
      H(" GHI AC ;LBNZ L%04d\n", lbl2=lbl++);
      H(" GLO AC ;LBZ L%04d\n", lbl3=lbl++);
      H("L%04d: ..BODY\n",lbl2);
   }
   ex(body);
   if (end)
      ex(end);
   H(" LBR L%04d\n",lbl1);
   H("L%04d: ..END\n",lbl3);
}

Z glvaluvar(NODE *p, int regena, int pushma) {
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
         WSMI("MA","FP",offs);
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
         WADI("MA","FP",offs);
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

Z gindex0(NODE *p) {
   int x;
   char *r, *reg1, *reg0;
   int imm1;

   reg0 = isreg(p->a[0])? reg(p->a[0]) : NULL;
   imm1 = isimm(p->a[1]);
   reg1 = isreg(p->a[1])? reg(p->a[1]) : NULL;

   // base
   switch (p->a[0]->t) {
   case ID: // id[x]
      if (reg0) {
         H(" ..ABASE IS REG\n");
         if (!imm1 && !reg1)
            WPUSH(reg0);
         else
            WMOV("AUX",reg0);
      }
      else {
         H(" ..ABASE IS ID\n");
         glvaluvar(p->a[0], 0, 0);
         // MA contains the address of id, dereference it
         H(" ..LDN AUX,MA\n");
         H(" LDA MA ;PLO AUX\n");
         H(" LDN MA ;PHI AUX\n");
      }
      break;
   case CON:              // const[x]
      H(" ..ABASE IS CONST\n");
      WLDI("AUX", p->a[0]->x);
      break;
   default:               // (expr)[x]
      H(" ..ABASE IS EXPR\n");
      ex(p->a[0]);
      WMOV("AUX","AC");
   }
// AUX contains base addr
   if (imm1) {
      H(" ..IDX W/ CONST\n");
      x = p->a[1]->x;
      WADI("MA","AUX",x);
   }
   else if (reg1) {
      H(" ..IDX W/ REG\n");
      H(" ..ADD MA,AUX,%s\n",reg1);
      H(" GLO %s ;STR SP ;GLO AUX ;ADD ;PLO MA\n",reg1);
      H(" GHI %s ;STR SP ;GHI AUX ;ADC ;PHI MA\n",reg1);
   }
   else {
      H(" ..IDX W/ EXPR\n");
      if (!reg0)
         WPUSH("AUX");
      ex(p->a[1]); // index
      H(" ..ADD MA,AC,*SP++\n");
      WADD("MA","AC");
   }
   WSHL("MA","MA");
}

Z gindex(NODE *p) {
   char *r, *reg1, *reg0;
   int imm0, imm1, x0, x1;

   reg0 = isreg(p->a[0])? reg(p->a[0]) : NULL;
   reg1 = isreg(p->a[1])? reg(p->a[1]) : NULL;
   imm0 = isimm(p->a[0]);
   imm1 = isimm(p->a[1]);
   x0   = p->a[0]->x;
   x1   = p->a[1]->x;

   if (reg0) {
      if (reg1) { // reg0[reg1]
         H(" ..reg0[reg1]\n");
         WADDR("MA",reg0,reg1);
      }
      else if (imm1) { // reg0[imm1]
         H(" ..reg0[imm1]\n");
         WADI("MA",reg0,x1);
      }
      else { // reg0[expr]
         ex(p->a[1]); // val in AC
         H(" ..reg0[expr]\n");
         WADDR("MA","AC",reg0);
      }
   }
   else {
      if (isvar(p->a[0])) {
         glvaluvar(p->a[0], 0, 0); // MA is address
         if (reg1) { // ID[reg1]
            H(" ..ID[reg1]\n");
            H(" GLO %s ;STR SP ;LDA MA ;ADD ;PLO AUX\n",reg1);
            H(" GHI %s ;STR SP ;LDN MA ;ADC ;PHI AUX\n",reg1);
            WSHL("MA","AUX");
            return;
         }
         else if (imm1) { // ID[imm1]
            H(" ..ID[imm1]\n");
            H(" LDA MA ;ADI #%02X ;PLO AUX\n",LO(x1));
            H(" LDN MA ;ADCI #%02X ;PHI AUX\n",HI(x1));
            WSHL("MA","AUX");
            return;
         }
         else { // ID[expr]
            H(" ..ID[expr]\n");
            H(" LDA MA ;PLO AUX\n");
            H(" LDN MA ;STXD\n");
            H(" GLO AUX ;STXD\n");
            ex(p->a[1]); // val in AC
            WADD("MA","AC");
         }
      }
      else if (imm0) {
         if (reg1) { // imm0[reg1]
            H(" ..imm0[reg1]\n");
            WADI("MA",reg1,x0);
         }
         else if (imm1) { // imm0[imm1]
            H(" ..imm0[imm1\n");
            x0 = 2*(x0 + x1);
            WLDI("MA",x0);
            return;
         }
         else { // imm0[expr]
            H(" ..imm0[expr]\n");
            ex(p->a[1]); // val in AC
            WADI("MA","AC",x0);
         }
      }
      else {
         ex(p->a[0]); // val in AC
         if (reg1) { // expr0[reg1]
            H(" ..expr0[reg1]\n");
            WADDR("MA","AC",reg1);
         }
         else if (imm1) { // expr0[imm1]
            H(" ..expr0[imm1]\n");
            WADI("MA","AC",x1);
         }
         else { // expr0[expr1]
            H(" ..expr0[expr1]\n");
            WPUSH("AC");
            ex(p->a[1]); // val in AC
            WADD("MA","AC");
         }
      }
   }
   WSHL("MA","MA");
}


Z glvalu(NODE *p, int regena) { // addr in MA

   if (isvar(p)) {
      glvaluvar(p, regena, 0);
      return;
   }
   else if (OPR == p->t) {
      if (UNARY + '*' == p->x) {
         ex(p->a[0]);
         H(" ..SHL MA,AC\n");
         WSHL("MA","AC");
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

Z gshln(void) {
   int lbl1, lbl2;

   H(" ..0==AUX.0?\n");
   H(" LBZ L%04d\n", lbl1=lbl++);
   H("L%04d: ..SHL LOOP\n", lbl2=lbl++);
   WSHL("AC","AC");
   H(" ..IF (--AUX.0) GOTO LOOP\n");
   H(" DEC AUX ;GLO AUX ;LBNZ L%04d\n", lbl2);
   H("L%04d:\n", lbl1);
}

Z gshl(NODE *p) {
   char *reg0, *reg1;
   int i, imm0, imm1, x0, x1;
   NODE *con;

   reg0 = isreg(p->a[0])? reg(p->a[0]) : NULL;
   imm0 = isimm(p->a[0]);
   x0   = p->a[0]->x;
   reg1 = isreg(p->a[1])? reg(p->a[1]) : NULL;
   imm1 = isimm(p->a[1]);
   x1   = p->a[1]->x;

   if (reg0) {
      if (reg1) { // reg0 << reg1
         H(" ..reg0<<reg1\n");
         WMOV("AC",reg0);
         H(" GLO %s ;PLO AUX\n",reg1);
         gshln();
      }
      else if (imm1) { // reg0 << imm1
         H(" ..reg0<<imm1\n");
Limm:
         if (x1 > 15) {
            H(" LDI #00 ;PLO AC ;PHI AC\n");
         }
         else {
            if (x1 > 7) {
               H(" GLO %s ;PHI AC\n",reg0);
               H(" LDI #00 ;PLO AC\n");
               x1 -= 8;
            }
            else if (xstrcmp(reg0,"AC"))
               WMOV("AC",reg0);
            if (x1 < 3) {
               for (i = 0; i < x1; i++)
                  WSHL("AC","AC");
            }
            else {
               H(" LDI #%02X ;PLO AUX\n",x1);
               gshln();
            }
         }
      }
      else { // reg0 << expr
         H(" ..reg0<<expr\n");
         ex(p->a[1]);
         H(" GLO AC ;PLO AUX\n");
         WMOV("AC",reg0);
         gshln();
      }
   }
   else if (imm0) {
      if (reg1) { // imm0 << reg1
         H(" ..imm0<<reg1\n");
         WLDI("AC",x0);
         H(" GLO %s ;PLO AUX\n",reg1);
         gshln();
      }
      else if (imm1) { // imm0 << imm1
         fprintf(stderr,"gshr(): imm0 op imm1\n");
         abort();
      }
      else { // imm0 << expr
         H(" ..imm0<<expr\n");
         ex(p->a[1]);
         H(" GLO AC ;PLO AUX\n");
         WLDI("AC",x0);
         gshln();
      }
   }
   else {
      if (reg1) { // expr << reg1
         H(" ..expr<<reg1\n");
         ex(p->a[0]);
         H(" GLO %s ;PLO AUX\n",reg1);
         gshln();
      }
      else if (imm1) { // expr << imm1
         H(" ..expr<<imm1\n");
         ex(p->a[0]);
         reg0 = "AC";
         goto Limm;
      }
      else { // expr << expr
         H(" ..expr<<expr\n");
         ex(p->a[1]);
         WPUSH0("AC");
         ex(p->a[0]);
         WPOP0("AUX");
         gshln();
      }
   }
}

Z gshrn(void) {
   int lbl1, lbl2;
   H(" ..0==AUX.0?\n");
   H(" LBZ L%04d\n", lbl1=lbl++);
   H("L%04d: ..LOOP\n", lbl2=lbl++);
   WSHR("AC","AC");
   H(" ..IF (--AUX.0) GOTO LOOP\n");
   H(" DEC AUX ;GLO AUX ;LBNZ L%04d\n", lbl2);
   H("L%04d:\n", lbl1);
}

Z gshr(NODE *p) {
   char *reg0, *reg1;
   int i, imm0, imm1, x0, x1;
   NODE *con;

   reg0 = isreg(p->a[0])? reg(p->a[0]) : NULL;
   imm0 = isimm(p->a[0]);
   x0   = p->a[0]->x;
   reg1 = isreg(p->a[1])? reg(p->a[1]) : NULL;
   imm1 = isimm(p->a[1]);
   x1   = p->a[1]->x;

   if (reg0) {
      if (reg1) { // reg0 >> reg1
         H(" ..reg0>>reg1\n");
         WMOV("AC",reg0);
         H(" GLO %s ;PLO AUX\n",reg1);
         gshrn();
      }
      else if (imm1) { // reg0 >> imm1
         H(" ..reg0>>imm1\n");
Limm:
         if (x1 > 15) {
            H(" LDI #00 ;PLO AC ;PHI AC\n");
         }
         else {
            if (x1 > 7) {
               H(" GHI %s ;PLO AC\n",reg0);
               H(" LDI #00 ;PHI AC\n");
               x1 -= 8;
            }
            else if (xstrcmp(reg0,"AC"))
               WMOV("AC",reg0);
            if (x1 < 3) {
               for (i = 0; i < x1; i++)
                  WSHR("AC","AC");
            }
            else {
               H(" LDI #%02X ;PLO AUX\n",x1);
               gshrn();
            }
         }
      }
      else { // reg0 >> expr
         H(" ..reg0>>expr\n");
         ex(p->a[1]);
         H(" GLO AC ;PLO AUX\n");
         WMOV("AC",reg0);
         gshrn();
      }
   }
   else if (imm0) {
      if (reg1) { // imm0 >> reg1
         H(" ..imm0>>reg1\n");
         WLDI("AC",x0);
         H(" GLO %s ;PLO AUX\n",reg1);
         gshrn();
      }
      else if (imm1) { // imm0 >> imm1
         fprintf(stderr,"gshr(): imm0 >> imm1\n");
         abort();
      }
      else { // imm0 >> expr
         H(" ..imm0>>expr\n");
         ex(p->a[1]);
         H(" GLO AC ;PLO AUX\n");
         WLDI("AC",x0);
         gshrn();
      }
   }
   else {
      if (reg1) { // expr >> reg1
         H(" ..expr>>reg1\n");
         ex(p->a[0]);
         H(" GLO %s ;PLO AUX\n",reg1);
         gshrn();
      }
      else if (imm1) { // expr >> imm1
         H(" ..expr>>imm1\n");
         ex(p->a[0]);
         reg0 = "AC";
         goto Limm;
      }
      else { // expr >> expr
         H(" ..expr>>expr\n");
         ex(p->a[1]);
         WPUSH0("AC");
         ex(p->a[0]);
         WPOP0("AUX");
         gshrn();
      }
   }
}

Z gstvar(NODE *p) {
   if (isreg(p)) {
      WMOV(reg(p), "AC");
   }
   else {
      H(" ..STR MA,AC\n");
      H(" GLO AC ;STR MA ;INC MA\n");
      H(" GHI AC ;STR MA\n");
   }
}

Z gpreinc(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      H(" INC %s\n",r);
      WMOV("AC", r);
   }
   else {
      ex(p);
      H(" INC AC\n");
      H(" DEC MA\n");
      gstvar(p);
   }
}

Z gpredec(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      H(" DEC %s\n",r);
      WMOV("AC", r);
   }
   else {
      ex(p);
      H(" DEC AC\n");
      H(" DEC MA\n");
      gstvar(p);
   }
}

Z gpostinc(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      WMOV("AC", r);
      H(" INC %s\n", r);
   }
   else {
      gpreinc(p);
      H(" DEC AC\n");
   }
}

Z gpostdec(NODE *p) {
   char *r;

   if (isreg(p)) {
      r = reg(p);
      WMOV("AC", r);
      H(" DEC %s\n",r);
   }
   else {
      gpredec(p);
      H(" INC AC\n");
   }
}

Z gcall(char *subr) {
   H(" ..CALL %s\n",subr);
   // NB. all runtime routines should be on the same page!
   // H(" LDI A.1(%s) ;PHI SUB\n",subr);
   H(" LDI A.0(%s) ;PLO SUB\n",subr);
   H(" SEP SUB\n");
}

Z gdiv(NODE *p) {
   if (isvar(p->a[1]))
      gpushvar(p->a[1]);
   else {
      ex(p->a[1]);
      WPUSH("AC");
   }
   ex(p->a[0]);
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
         //  VEC0.0 <-- MA
         //  PTR.1
         //  PTR.0  <-- MA >> 1
         offs = getoffs(n);
         sym = getsym(n);
         H(" ..INIT %s=&%s[0]\n",sym,sym);
         offs -= 2;
         H(" GLO FP ;SMI #%02X ;PLO MA\n",LO(offs));
         // *--ptr = ptr >> 1
         H(" GHI FP ;SMBI #%02X ;PHI MA\n",HI(offs));
         H(" SHR ;PHI AUX\n");
         H(" GLO MA ;SHRC\n");
         H(" DEC MA ;DEC MA\n");
         H(" STR MA ;INC MA\n");
         H(" GHI AUX ;STR MA\n");
      }
      break;
   case REGDEF:
      assert(isvar(q));
      newoffs--;
      if (newoffs == 0x0B) {
         fprintf(stderr,"too many register variables\n");
         Err = 1;
      }
      defcls(n, C_REG, newoffs);
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
static int exprlist(NODE *p, int offs) {
   int n;
   NODE *q;

   if (!p)
      return offs;

   assert(OPR == p->t && XLST == p->x);
   H(" ..PUSH ARG%d\n",++offs);
   if (isvar(p->a[1])) /* push in reverse order */
      gpushvar(p->a[1]);
   else {
      ex(p->a[1]);
      WPUSH("AC");
   }
   return exprlist(p->a[0],offs);
}

Z galign(void) {
   H(" ..ALIGN\n");
   H(" ORG*+(* AND 1)\n");
}

Z gmod(NODE *p) {
   gdiv(p);
   WMOV("AC","MQ");
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
   int i, n;

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

Z gand(NODE *p) {
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

Z gsavma(void) {
   H(" LDI A.1(SAVMA) ;PHI SUB\n");
   H(" LDI A.0(SAVMA) ;PLO SUB\n");
   H(" GLO MA ;STR SUB ;INC SUB\n");
   H(" GHI MA ;STR SUB\n");
}

Z grestma(void) {
   // NB. cannot use AUX
   H(" LDI A.1(SAVMA) ;PHI SUB\n");
   H(" LDI A.0(SAVMA) ;PLO SUB\n");
   H(" LDA SUB ;PLO MA\n");
   H(" LDN SUB ;PHI MA\n");
   // restore SUB.1
   H(" LDI A.1(UMULT) ;PHI SUB\n");
}

Z GLO(char *reg,char *reg1) {
   if (!reg) reg = "AC";
   if (reg1) {
      H(" GLO %s ;STR SP ;",reg1);
   }
   if (!xstrcmp(reg,"MA"))
      H(" LDN MA");
   else
      H(" GLO %s",reg);
}

Z GHI(char *reg,char *reg1) {
   if (!reg) reg = "AC";
   if (reg1)
      H(" GHI %s ;STR SP ;",reg1);
   if (!xstrcmp(reg,"MA"))
      H(" LDN MA");
   else
      H(" GHI %s",reg);
}

Z PLO(char *reg) {
   if (xstrcmp(reg,"MA"))
      H(" ;PLO %s\n",reg);
   else
      H(" ;STR MA ;INC MA\n");
}

Z PHI(char *reg) {
   if (xstrcmp(reg,"MA"))
      H(" ;PHI %s\n",reg);
   else
      H(" ;STR MA\n");
}

Z PLOAC(char *reg) {
   H(" ;PLO AC");
   if (xstrcmp(reg,"MA"))
      H(" ;PLO %s\n",reg);
   else
      H(" ;STR MA ;INC MA\n");
}

Z PHIAC(char *reg) {
   H(" ;PHI AC");
   if (xstrcmp(reg,"MA"))
      H(" ;PHI %s\n",reg);
   else
      H(" ;STR MA\n");
}

#define IMM(imm,imop)   ((imm)? (imop) : NULL)

Z instr(char *reg1,char *xop, char *imop, int x) {
   if (imop)
      H(" ;%s #%02X",imop,x);
   else {
      if (reg1)
         H(" ;%s",xop);
      else
         H(" ;IRX ;%s",xop);
   }
}

// load X0 in AC, X1 in SP
Z gasgnld(char *reg0,char *reg1,int imm1,int x) {
   // reg0 imm1
   // 0    0         MA=&VAR,SP=X1
   // 0    1         MA=&VAR
   // 1    0         Rx=VAR,AC=X1
   // 1    1         Rx=VAR
   if (!xstrcmp(reg0,"MA")) {
      gldvar(NULL);
      H(" DEC MA\n");
   }
   if (xstrcmp(reg0,"MA") || imm1) {
      if (reg1)
         WPUSH(reg1);
      if (xstrcmp(reg0,"MA"))
         WMOV("AC",reg0);
      if (imm1)
         WPUSHI(x);
   }
   else if (reg1)
      WPUSH(reg1);
}

// load X0 in AC, X1 in AUX
Z gasgnld2(char *reg0,char *reg1,int imm1,int x) {
   if (!xstrcmp(reg0,"MA")) {
      gldvar(NULL);
      H(" DEC MA\n");
   }
   if (xstrcmp(reg0,"MA") || imm1) {
      if (reg1)
         WMOV("AUX",reg1);
      if (xstrcmp(reg0,"MA"))
         WMOV("AC",reg0);
      if (imm1)
         WLDI("AUX",x);
   }
   else if (reg1)
      WMOV("AUX",reg1);
   else
      WPOP("AUX");
}

Z gasgnop(NODE *p) {
   char *reg0, *reg1;
   int imm1, x;

   reg0 = isreg(p->a[0])? reg(p->a[0]) : NULL;
   imm1 = isimm(p->a[1]);
   reg1 = NULL;

   if (imm1)
      x = p->a[1]->x;
   else {
      ex(p->a[1]);
      if (reg0)
         reg1 = "AC";
      else
         WPUSH("AC");
   }

   if (!reg0) {
      glvalu(p->a[0], 1);
      reg0 = "MA";
      // gldvar(p->a[0]);
      // H(" DEC MA\n");
   }

   /*
      reg0 imm1
      0    0         MA=&VAR,SP=X1
      0    1         MA=&VAR
      1    0         Rx=VAR,AC=X1
      1    1         Rx=VAR
   */

   switch (p->x) {
   case AOR:
      H(" ..AOR\n");
      GLO(reg0,reg1); instr(reg1,"OR",IMM(imm1,"ORI"),LO(x)); PLOAC(reg0);
      GHI(reg0,reg1); instr(reg1,"OR",IMM(imm1,"ORI"),HI(x)); PHIAC(reg0);
      break;
   case AAND:
      H(" ..AAND\n");
      GLO(reg0,reg1); instr(reg1,"AND",IMM(imm1,"ANI"),LO(x)); PLOAC(reg0);
      GHI(reg0,reg1); instr(reg1,"AND",IMM(imm1,"ANI"),HI(x)); PHIAC(reg0);
      break;
   case AEQ:
   case ANE:
      H(" ..AEQ,ANE\n");
      GLO(reg0,reg1); instr(reg1,"SM",IMM(imm1,"SMI"),LO(x)); PLO("AC");
      GHI(reg0,reg1); instr(reg1,"SMB",IMM(imm1,"SMBI"),HI(x)); PHI("AC");
      GNE0();
      if (AEQ == p->x)
         H(" XRI #FF\n");
      PLOAC(reg0); PHIAC(reg0);
      break;
   case ALT:
   case AGE:
      H(" ..ALT,AGE\n");
      GLO(reg0,reg1); instr(reg1,"SM",IMM(imm1,"SMI"),LO(x));
      GHI(reg0,reg1); instr(reg1,"SM",IMM(imm1,"SMBI"),HI(x));
      GLT();
      if (AGE == p->x)
         H(" XRI #FF\n");
      PLOAC(reg0); PHIAC(reg0);
      break;
   case ALE:
   case AGT:
      H(" ..ALE,AGT\n");
      GLO(reg0,reg1); instr(reg1,"SM",IMM(imm1,"SMI"),LO(x)); PLO("AC");
      GHI(reg0,reg1); instr(reg1,"SMB",IMM(imm1,"SMBI"),HI(x)); PHI("AC");
      GGT();
      if (ALE == p->x)
         H(" XRI #FF\n");
      PLOAC(reg0); PHIAC(reg0);
      break;
   case ASHL: // XXX
      H(" ..ASHL\n");
      gasgnld2(reg0,reg1,imm1,x);
      gshln();
      GLO("AC",NULL); PLO(reg0);
      GHI("AC",NULL); PHI(reg0);
      break; 
   case ASHR: // XXX
      H(" ..ASHR\n");
      gasgnld2(reg0,reg1,imm1,x);
      gshrn();
      GLO("AC",NULL); PLO(reg0);
      GHI("AC",NULL); PHI(reg0);
      break;
   case AADD:
      H(" ..AADD\n");
      GLO(reg0,reg1); instr(reg1,"ADD",IMM(imm1,"ADI"),LO(x)); PLOAC(reg0);
      GHI(reg0,reg1); instr(reg1,"ADC",IMM(imm1,"ADCI"),HI(x)); PHIAC(reg0);
      break;
   case ASUB:
      H(" ..ASUB\n");
      GLO(reg0,reg1); instr(reg1,"SM",IMM(imm1,"SMI"),LO(x)); PLOAC(reg0);
      GHI(reg0,reg1); instr(reg1,"SMB",IMM(imm1,"SMBI"),HI(x)); PHIAC(reg0);
      break;
   case AMOD:
      // NB. === MA/MQ are the same ===
      gasgnld(reg0,reg1,imm1,x);
      if (!reg0 || !xstrcmp(reg0,"MA")) gsavma();
      gcall("UDIV");
      WMOV("AUX", "MQ");
      if (!reg0 || !xstrcmp(reg0,"MA")) grestma();
      GLO("AUX",NULL); PLOAC(reg0);
      GHI("AUX",NULL); PHIAC(reg0);
      break;
   case AMUL:
      gasgnld(reg0,reg1,imm1,x);
      if (!reg0 || !xstrcmp(reg0,"MA")) gsavma();
      gcall("UMULT");
      if (!reg0 || !xstrcmp(reg0,"MA")) grestma();
      GLO("AC",NULL); PLO(reg0);
      GHI("AC",NULL); PHI(reg0);
      break;
   case ADIV:
      gasgnld(reg0,reg1,imm1,x);
      if (!reg0 || !xstrcmp(reg0,"MA")) gsavma();
      gcall("UDIV");
      if (!reg0 || !xstrcmp(reg0,"MA")) grestma();
      GLO("AC",NULL); PLO(reg0);
      GHI("AC",NULL); PHI(reg0);
      break;
   default:
      fprintf(stderr,"unknown asgn op %d\n",p->x);
      exit(1);
   }
}

Z gasgn(NODE *p) {
   char *reg1, *reg0;
   int imm1, x1;

   reg0 = isreg(p->a[0])? reg(p->a[0]) : NULL;
   reg1 = isreg(p->a[1])? reg(p->a[1]) : NULL;
   imm1 = isimm(p->a[1]);
   x1   = p->a[1]->x;

   if (reg0) {
      if (reg1) { // reg0 = reg1
         H(" GLO %s ;PLO %s ;PLO AC\n",reg1,reg0);
         H(" GHI %s ;PHI %s ;PHI AC\n",reg1,reg0);
      }
      else if (imm1) { // reg0 = imm1
         H(" LDI #%02X ;PLO %s ;PLO AC\n",LO(x1),reg0);
         H(" LDI #%02X ;PHI %s ;PHI AC\n",HI(x1),reg0);
      }
      else { // reg0 = expr
         ex(p->a[1]); // val in AC
         H(" GLO AC ;PLO %s\n",reg0);
         H(" GHI AC ;PHI %s\n",reg0);
      }
   }
   else {
      if (reg1) { // lvalu = reg1
         glvalu(p->a[0],1); // addr in MA
         H(" GLO %s ;PLO AC ;STR MA ;INC MA\n",reg1);
         H(" GHI %s ;PHI AC ;STR MA\n",reg1);
      }
      else if (imm1) { // lvalu = imm1
         glvalu(p->a[0],1); // addr in MA
         H(" LDI #%02X ;PLO AC ;STR MA ;INC MA\n",LO(x1));
         H(" LDI #%02X ;PHI AC ;STR MA\n",HI(x1));
      }
      else { // lvalu = expr
         ex(p->a[1]); // val in AC
         WPUSH("AC");
         glvalu(p->a[0], 1); // addr in MA
         H(" IRX ;LDXA ;PLO AC; STR MA; INC MA\n");
         H(" LDX ;PHI AC ;STR MA\n");
      }
   }
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
      WLDI("AC",p->x);
      break;
   case STR:
      H(" ..STR %s [%d]\n",printable(p->s),p->x=lbl++);
      H(" LDI A.1(L%d SHR 1) ;PHI AC\n",p->x);
      H(" LDI A.0(L%d SHR 1) ;PLO AC\n",p->x);
      break;
   case ID: glvalu(p, 1); gldvar(p); break;
   case OPR:
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
         if (0x0F != lowreg) {
            fprintf(stderr,"auto vars after register vars\n");
            Err = 1;
         }
         offs = idlist(AUTODEF, p->a[0], -1);
         H(" ..AUTO SIZE %d\n",offs);
         H(" ..SMI SP,SP,%d\n",offs+1);
         offs++;
         if (offs) {
            H(" GLO SP ;SMI #%02X; PLO SP\n",LO(offs));
            H(" GHI SP ;SMBI #%02X; PHI SP\n",HI(offs));
         }
         // init vector ptrs
         idlist(INIVPTR,p->a[0],0);
         break;
      case REGDEF:
         lowreg = idlist(REGDEF, p->a[0], 0x0F);
         H(" ..#REG VARS %d\n", 0x0F - lowreg);
         for (i = lowreg; i < 0x0F; i++)
            WPUSH(regnm(i));
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
         i = LO(p->a[0]->x);
         H("L%04d: ..CASE %d\n",lbl,i);
         switches[currsw].tab[i] = lbl++;
         break;
      case FUNDEF:
         sym = getsym(p->a[0]->x);
         H(" ..FN %s\n",fn = sym);
         lowreg = 0x0F; nswitches = 0; currsw = -1;
         galign();
         H("L%s:\n",sym);
         WPUSH("FP");
         WMOV("FP", "SP");
         q = p->a[1];
         ex(q->a[0]); // ID list
         ex(q->a[1]); // stmt
         H("E%s:\n",sym); // fn epilogue
         for(i = 0x0F; i > lowreg; i--)
            WPOP(regnm(i-1));
         WMOV("SP", "FP");
         WPOP("FP");
         H(" SEP SRET\n");
         swdef();
         dropsyms();
         fn = NULL;
         break;
      case XLST: // expr list
         argcnt = exprlist(p, 0);
         break;
      case FUNCALL:
         sym = getsym(p->a[0]->x);
         if (C_UNDEF == getcls(p->a[0]->x)) {
            // fprintf(stderr,"%s undefined\n",sym);
            defcls(p->a[0]->x,C_EXTRN,0);
         }
         argcnt = ex(p->a[1]); // push args
         H(" ..CALL %s\n",sym);
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
         {  int simcmp;

            simcmp = 0;
            if (isrelop(p->a[0])) {
               simcmp = 1;
               p->a[0]->attr |= A_SIMCMP;
            }
            ex(p->a[0]);
            q = p->a[1];
            H(" ..0==AC?\n");
            if (simcmp)
               H(" LBZ L%04d\n", lbl2=lbl++);
            else {
               H(" GHI AC ;LBNZ L%04d\n", lbl1=lbl++);
               H(" GLO AC ;LBZ  L%04d\n", lbl2=lbl++);
               H("L%04d:\n",lbl1);
            }
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
         if (isvar(p->a[0]))
            gpushvar(p->a[0]);
         else {
            ex(p->a[0]);
            WPUSH("AC");
         }
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
      case '[': H(" ..AREF\n"); glvalu(p, 1); gldvar(p); break;
      case UNARY + '-':
         if (isimm(p->a[0])) {
            i = -(p->a[0]->x);
            H(" ..LDI AC,%02X%02X\n",HI(i),LO(i));
            H(" LDI #%02X ;PLO AC\n",LO(i));
            H(" LDI #%02X ;PHI AC\n",HI(i));
         } else {
            ex(p->a[0]);
            H(" ..NEG AC\n");
            H(" GLO AC ;SDI #00 ;PLO AC\n");
            H(" GHI AC ;SDBI #00 ;PHI AC\n");
         }
         break;
      case '~': ex(p->a[0]); WCOM("AC","AC"); break;
      case '!': ex(p->a[0]); geq0(p->a[0]); break;
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
         H(" ..SHR AC,MA\n");
         H(" GHI MA ;SHR  ;PHI AC\n");
         H(" GLO MA ;SHRC ;PLO AC\n");
         break;
      case UNARY + '*':
         ex(p->a[0]);
         H(" ..DEREF\n");
         WSHL("AC","AC");
         H(" ..LDN AC,AC\n");
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
                  WPUSH("AC");
               }
               ex(p->a[0]);
               gcall("UMULT");
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
         case LE:  gle(p); break;
         case NE:  gne(p); break;
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

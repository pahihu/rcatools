/*
 * History
 * =======
 * 201201AP     initial revision, expr evaluator
 * 201202AP     16bit only, shr/shl, logical ops
 * 201203AP     labels, goto
 *              symbols pass syms[] idx, instead of offset
 *              ternary cond, logical AND/OR/NOT
 *
 * NB. ++ increments by 2, addresses in bytes
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc18.h"
#include "y.tab.h"

/* SP - points to empty space

   PUSH8   STXD
   POP8    IRX ;LDX

   PUSH16  STXD ;STXD
   POP16   IRX ;LDXA ;LDX

 */

int ex(NODE *p);

static int lbl = 0;

#define LO(x)   ((x) & 0xFF)
#define HI(x)   LO((x)>>8)
#define H  printf

#define ADDR(p) (getoffs((p)->x))

static int ischar(NODE *p) {
   if (!p)
      return 0;
   return ID == p->t;
}

static int isimm8(NODE *p) {
   if (!p)
      return 0;
   return (CON == p->t) && !HI(p->x);
}

static int isbyte(NODE *p) {
   return ischar(p) || isimm8(p);
}

static void gpushac(int n) {
   if (1 == n) {
      H(" ..PUSH AC.0\n");
      H(" GLO AC ;STXD\n");
   }
   else {
      H(" ..PUSH AC\n");
      H(" GHI AC ;STXD\n");
      H(" GLO AC ;STXD\n");
   }
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

static void gdtoac(void) {
   H(" ..AC=(D,D)\n");
   H(" PHI AC ;PLO AC\n");
}

static void gbinary(NODE *p) {
   ex(p->a[1]);
   gpushac(2);  // (LO,HI) @ X
   ex(p->a[0]); // AC
}

static void gsub(NODE *p) {
   NODE *con;

   if (CON == p->a[1]->t) {
      con = p->a[1];

      ex(p->a[0]);
      if (1 == con->x) {
         H(" DEC AC\n");
      }
      else {
         H(" ..AC=AC-%04X\n",con->x);
         H(" GLO AC ;SMI #%02X ;PLO AC\n", LO(con->x));
         H(" GHI AC ;SMBI #%02X ;PHI AC\n", HI(con->x));
      }
   }
   else {
      gbinary(p);
      H(" ..AC=AC-*SP++\n");
      H(" IRX ;GLO AC ;SM ;PLO AC\n");
      H(" IRX ;GHI AC ;SMB ;PHI AC\n");
   }
}

static void glt(NODE *p) {
   gsub(p); // A - AUX @ X
   // DF=0
   H(" ..D=DF? #00 : #FF\n");
   H(" LDI #FF ;ADCI #00\n");  // DF=1 FALSE, DF=0 TRUE
   gdtoac();
}

static void ggt(NODE *p) {
   gsub(p);
   H(" ..D=MSB(AC)? #00 : #FF\n");
   H(" GHI AC ;SHL\n"); // DF=1 FALSE, DF=0 TRUE
   H(" LDI #FF ;ADCI #00\n");  // DF=1 FALSE, DF=0 TRUE
   gdtoac();
}

static void geq(NODE *p) {
   int lbl1;

   gbinary(p);
   H(" ..AC==*SP++?\n");
   H(" GHI AC ;XOR ;IRX ;LBNZ L%04d\n", lbl1=lbl++);
   H(" GLO AC ;XOR ;LBNZ L%04d\n", lbl1);
   H(" LDI #FF\n");
   H(" LSKP\n");
   H("L%04d:\n", lbl1);
   H(" LDI #00\n");
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
      H(" GLO AC ;%s %02X ;PLO AC\n", imop, LO(con->x));
      H(" GHI AC ;%s %02X ;PHI AC\n", imop, HI(con->x));
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
   H(" GHI AC ;LBNZ L%04d\n", lbl2=lbl++);
   H(" GLO AC ;LBZ L%04d\n", lbl3=lbl++);
   H("L%04d: ..BODY\n",lbl2);
   ex(body);
   if (end)
      ex(end);
   H(" LBR L%04d\n",lbl1);
   H("L%04d: ..END\n",lbl3);
}

static void glvalu(NODE *p) { // addr in VAR
   int adr;

   if (ID == p->t) {
      adr = ADDR(p); // points to LOW
      H(" ..VAR.0=ADDR(%s)\n",getsym(p->x));
      H(" LDI #%02X ;PLO VAR\n", adr);
      return;
   }
   else if (OPR == p->t) {
      if (UNARY + '*' == p->x) {
         ex(p->a[0]);
         H(" ..VAR=AC\n");
         H(" GLO AC ;PLO VAR\n");
         H(" GHI AC ;PHI VAR\n");
         return;
      }
      else if ('[' == p->x) {
         ex(p->a[0]); // base
         gpushac(2);
         ex(p->a[1]); // index
         H(" ..VAR=AC+*SP++\n");
         H(" IRX ;GLO AC ;ADD ;PLO VAR\n");
         H(" IRX ;GHI AC ;ADC ;PHI VAR\n");
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
   NODE *con;

   con = 0;
   if (CON == p->a[1]->t) {
      con = p->a[1];
      if (1 == con->x) {
         gdiv2();
         return;
      }
      else {
         H(" ..AC.0=%02X\n",LO(con->x));
         H(" LDI #%02X ;PLO AC\n", LO(con->x));
         gpushac(1);
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

   con = 0;
   if (CON == p->a[1]->t) {
      con = p->a[1];
      if (1 == con->x) {
         gmul2();
         return;
      }
      else {
         H(" ..AC.0=%02X\n",LO(con->x));
         H(" LDI #%02X ;PLO AC\n", LO(con->x));
         gpushac(1);
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

static void gstvar(void) {
   H(" ..ST [VAR],AC\n");
   H(" SEP STVAR\n");
   // H(" GLO AC ;STR VAR ;INC VAR\n");
   // H(" GHI AC ;STR VAR\n");
}

static void gpreinc(NODE *p) {
   ex(p->a[0]);
   H(" ..++AC\n");
   H(" INC AC ;INC AC\n");
   H(" DEC VAR\n");
   gstvar();
}

static void gpredec(NODE *p) {
   ex(p->a[0]);
   H(" DEC AC ;DEC AC\n");
   H(" DEC VAR\n");
   gstvar();
}

static void gdiv(NODE *p) {
   H(" ..DIV\n");
   ex(p->a[1]);
   gpushac(1);
   ex(p->a[0]);
   gpop(1, "AUX");
   H(" LDI A.0(UDIV) ;PLO SUB ;SEP SUB\n");
}

static void gldvar(void) {
   H(" LDA VAR ;PLO AC\n");
   H(" LDN VAR ;PHI AC\n");
}


int ex(NODE *p) {
   int lbl1, lbl2, lbl3;
   int clobber;
   NODE *q, *stmt;

   if (!p) return 0;
   if (INT == p->t) {
      fprintf(stderr,"ex(): internal node!\n");
      exit(1);
   }
   switch (p->t) {
   case CON:
      H(" ..AC=%04X\n",p->x);
      if (HI(p->x) == LO(p->x))
        H(" LDI #%02X ;PHI AC ;PLO AC\n",HI(p->x));
      else {
        H(" LDI #%02X ;PHI AC\n",HI(p->x));
        H(" LDI #%02X ;PLO AC\n", LO(p->x));
      }
      break;
   case ID: glvalu(p); gldvar(); break;
   case OPR:
      switch (p->x) {
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
      case PRINT:
         ex(p->a[0]);
         H(" ..PUTC\n");
         H(" LDI A.0(PUTC) ;PLO SUB ;SEP SUB\n");
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
         if (LABEL != getoffs(p->a[0]->x)) {
            fprintf(stderr,"not a label: %s\n",getsym(p->a[0]->x));
            exit(1);
         }
         H(" ..GOTO\n");
         H(" LBR L%s\n",getsym(p->a[0]->x));
         break;
      case '=':
         clobber = 0;
         ex(p->a[1]);
         if (ID == p->a[0]->t) {
            // VAR = expr
            glvalu(p->a[0]);
         }
         else {
            // * expr = expr
            // expr '[' expr ']' = expr
            gpushac(2);
            glvalu(p->a[0]); // addr in VAR
            gpop(2, "AC");
            clobber = 1;
         }
         gstvar();
         if (clobber) {
            H(" ..VAR.1=DSEG.1\n");
            H(" LDI A.1(DSEG) ;PHI VAR\n");
         }
         break;
      case  PREINC: gpreinc(p); break;
      case  PREDEC: gpredec(p); break;
      case POSTINC: gpreinc(p); H(" DEC AC ;DEC AC\n"); break;
      case POSTDEC: gpredec(p); H(" INC AC ;INC AC\n"); break;
      case '[': H(" ..AREF\n"); glvalu(p); gldvar(); break;
      case UNARY + '-':
         ex(p->a[0]);
         H(" ..NEG AC\n");
         H(" GLO AC ;XRI #FF ;ADI #01 ;PLO AC\n");
         H(" GHI AC ;XRI #FF ;ADCI #00 ;PHI AC\n");
         break;
      case '~': ex(p->a[0]); gcom(); break;
      case '!': ex(p->a[0]); geq0(); break;
      case LAND:
         ex(p->a[0]);
         gtobool();
         H(" ..LAND: IF AC==0 SKIP\n");
         H(" LBZ L%0d\n", lbl1=lbl++);
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
         H(" ..ADDR %s\n", getsym(p->a[0]->x));
         H(" LDI #%02X ;PLO AC\n",ADDR(p->a[0]));
         H(" GHI VAR ;PHI AC\n");
         break;
      case UNARY + '*':
         ex(p->a[0]);
         H(" ..LD AC,[AC]\n");
         H(" LDA AC ;PLO AUX\n");
         H(" LDN AC ;PHI AC\n");
         H(" GLO AUX ;PLO AC\n");
         break;
      default:
         switch (p->x) {
         case '+': gadd(p); break;
         case '-': gsub(p); break;
         case '*':
            ex(p->a[1]);
            gpushac(1);
            ex(p->a[0]);
            gpop(1, "AUX");
            H(" ..MUL AC,AUX.0\n");
            H(" LDI A.0(UMULT) ;PLO SUB ;SEP SUB\n");
            break;
         case '/': gdiv(p); break;
         case '%':
            gdiv(p);
            H(" ..MOV AC,AUX.0\n");
            H(" GLO AUX ;PLO AC\n");
            H(" LDI #00 ;PHI AC\n");
            break;
         case '<': glt(p); break;
         case '>': ggt(p); break;
         case GE:  glt(p); gcom(); break;
         case LE:  ggt(p); gcom(); break;
         case NE:  geq(p); gcom(); break;
         case EQ:  geq(p); break;
         case '&': glog(p, "AND", "ANI"); break;
         case '|': glog(p, "OR", "ORI"); break;
         case '^': glog(p, "XOR", "XRI"); break;
         case SHR: gshr(p); break;
         case SHL: gshr(p); break;
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

   return 0;
}

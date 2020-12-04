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
int ex(NODE *p);
static void gmul2(void);

static int lbl = 0;

#define LO(x)   ((x) & 0xFF)
#define HI(x)   LO((x)>>8)
#define H  printf

#define ADDR(p) (getoffs((p)->x))

static int isvar(NODE *p) {
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

static void glvalu(NODE *p) { // addr in VARPTR
   int cls, offs;
   char *sym;

   if (isvar(p)) {
      cls  = getcls(p->x);
      sym  = getsym(p->x);
      offs = getoffs(p->x);
      H(" ..VARPTR=ADDR(%s)\n",getsym(p->x));
      switch (cls) {
      case C_UNDEF:
        fprintf(stderr,"glvalu(): %s undefined\n",sym);
        exit(1);
        break;
      case C_EXTRN:
        H(" LDI A.0(L%s) ;PLO VARPTR\n",sym);
        H(" LDI A.1(L%s) ;PHI VARPTR\n",sym);
        break;
      case C_AUTO:
        H(" GLO FP ;SMI #%02X ;PLO VARPTR\n",LO(offs));
        H(" GHI FP ;SMBI #%02X ;PHI VARPTR\n",HI(offs));
        break;
      case C_LABEL:
        fprintf(stderr,"glvalu(): %s is label\n",sym);
        exit(1);
        break;
      case C_PARAM:
        H(" GLO FP ;ADD #%02X ;PLO VARPTR\n",LO(offs));
        H(" GHI FP ;ADCI #%02X ;PLO VARPTR\n",HI(offs));
        break;
      }
      return;
   }
   else if (OPR == p->t) {
      if (UNARY + '*' == p->x) {
         ex(p->a[0]);
         H(" ..VARPTR=2*AC\n");
         H(" GLO AC ;SHL  ;PLO VARPTR\n");
         H(" GHI AC ;SHLC ;PHI VARPTR\n");
         return;
      }
      else if ('[' == p->x) {
         ex(p->a[0]); // base
         gmul2();
         gpushac(2);
         ex(p->a[1]); // index
         gmul2();
         H(" ..VARPTR=AC+*SP++\n");
         H(" IRX ;GLO AC ;ADD ;PLO VARPTR\n");
         H(" IRX ;GHI AC ;ADC ;PHI VARPTR\n");
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
   H(" ..ST [VARPTR],AC\n");
   H(" GLO AC ;STR VARPTR ;INC VARPTR\n");
   H(" GHI AC ;STR VARPTR\n");
}

static void gpreinc(NODE *p) {
   ex(p->a[0]);
   H(" ..++AC\n");
   H(" INC AC\n");
   H(" DEC VARPTR\n");
   gstvar();
}

static void gpredec(NODE *p) {
   ex(p->a[0]);
   H(" DEC AC\n");
   H(" DEC VARPTR\n");
   gstvar();
}

static void gdiv(NODE *p) {
   H(" ..DIV\n");
   ex(p->a[1]);
   gpushac(1);
   ex(p->a[0]);
   gpop(1, "AUX");
   H(" LDI A.1(UDIV) ;PHI SUB\n");
   H(" LDI A.0(UDIV) ;PLO SUB\n");
   H(" SEP SUB\n");
}

static void gldvar(void) {
   H(" LDA VARPTR ;PLO AC\n");
   H(" LDN VARPTR ;PHI AC\n");
}


int ex(NODE *p) {
   int lbl1, lbl2, lbl3;
   int clobber;
   NODE *q, *stmt;
   char *sym;
   int oframe, offs, n;
   int argcnt;

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
      case VARDEF:
         sym = getsym(p->a[0]->x);
         H(" ..EXTRN %s\n",sym);
         H("L%s:\n",sym);
         H(" ORG*+%d\n",p->a[1]->x);
         break;
      case VECDEF:
         sym = getsym(p->a[0]->x);
         H(" ..EXTRN %s[]\n",sym);
         H("L%s:\n",sym);
         H(" ORG*+%d\n",p->a[1]->x);
         break;
      case ILST:
         offs = 1;
         while (p) {
            assert(isvar(p->a[0]));
            n = p->a[0]->x;
fprintf(stderr,"defpar%d: %s\n",offs,getsym(n));
            defcls(n, C_PARAM, offs);
            p = p->a[1]; offs += 2;
         }
         break;
      case EXTDEF:
         p = p->a[0]; // id_list
         assert(OPR == p->t && ILST == p->x);
         while (p) {
            assert(isvar(p->a[0]));
            n = p->a[0]->x;
fprintf(stderr,"defext: %s\n",getsym(n));
            defcls(n, C_EXTRN, 0);
            p = p->a[1];
         }
         break;
      case AUTODEF:
         p = p->a[0]; // id_list
         assert(OPR == p->t && ILST == p->x);
         offs = 1;
         while (p) {
            assert(isvar(p->a[0]));
            n = p->a[0]->x;
            q = p->a[0]->a[0]; // storage size
            assert(CON == q->t);
fprintf(stderr,"defauto: %s %d\n",getsym(n), offs);
            defcls(n, C_AUTO, offs);
            offs += q->x;
            p = p->a[1];
         }
         H(" ..AUTO SIZE %d,ADJUST SP\n",offs);
         H(" GLO SP ;SMI #%02X; PLO SP\n",LO(offs));
         H(" GHI SP ;SMBI #%02X; PHI SP\n",HI(offs));
         break;
      case FUNDEF:
         sym = getsym(p->a[0]->x);
         H(" ..FN %s\n",sym);
         H("L%s:\n",sym);
         gpush(2, "FP");
         gmov("FP", "SP");
         q = p->a[1];
         oframe = getnsyms();
         ex(q->a[0]); // ID list
         ex(q->a[1]); // stmt
         H("E%s:\n",sym); // fn epilogue
         gmov("SP", "FP");
         gpop(2, "FP");
         H(" SEP SRET\n");
         dropsyms(oframe);
         break;
      case XLST: // expr list
         offs = 1; argcnt = 0;
         while (p) {
            H(" ..PUSH ARG%d\n",offs++);
            argcnt++;
            ex(p->a[0]);
            gpushac(2);
            p = p->a[1];
         }
         break;
      case FUNCALL:
         sym = getsym(p->a[0]->x);
         H(" ..CALL %s\n",sym);
         argcnt = ex(p->a[1]); // push args
         H(" SEP SCALL,A(L%s)\n",sym);
         argcnt *= 2; // 2byte args
         H(" GLO SP ;ADI #%02X ;PLO SP\n",LO(argcnt));
         H(" GHI SP ;ADCI #%02X ;PHI SP\n",HI(argcnt));
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
      case PRINT:
         ex(p->a[0]);
         H(" ..PUTC\n");
         H(" LDI A.1(PUTC) ;PHI SUB\n");
         H(" LDI A.0(PUTC) ;PLO SUB\n");
         H(" SEP SUB\n");
         break;
      case RETURN:
         if (p->a[0])
            ex(p->a[0]);
         H(" LBR E%s\n", fn); // fn epilogue
         break;
      case ',': // fn params
         ex(p->a[0]);
         gpushac(2);
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
            // VARPTR = expr
            glvalu(p->a[0]);
         }
         else {
            // * expr = expr
            // expr '[' expr ']' = expr
            gpushac(2);
            glvalu(p->a[0]); // addr in VARPTR
            gpop(2, "AC");
         }
         gstvar();
         break;
      case  PREINC: gpreinc(p); break;
      case  PREDEC: gpredec(p); break;
      case POSTINC: gpreinc(p); H(" DEC AC\n"); break;
      case POSTDEC: gpredec(p); H(" INC AC\n"); break;
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
         H(" ..AC=WADDR(%s)\n", getsym(p->a[0]->x));
         glvalu(p->a[0]);
         H(" GHI VARPTR ;SHR  ;PHI AC\n");
         H(" GLO VARPTR ;SHRC ;PLO AC\n");
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
         case '-': gsub(p); break;
         case '*':
            ex(p->a[1]);
            gpushac(1);
            ex(p->a[0]);
            gpop(1, "AUX");
            H(" ..MUL AC,AUX.0\n");
            H(" LDI A.1(UMULT) ;PHI SUB\n");
            H(" LDI A.0(UMULT) ;PLO SUB\n");
            H(" SEP SUB\n");
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

   return argcnt;
}

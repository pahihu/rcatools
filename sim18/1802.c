// CDP1802

/*
 * History
 * =======
 *
 * 201114AP initial version
 * 201115AP disassembler, GLL-MAG register names
 * 201116AP initial FDD support
 * 201117AP built-in UT40 monitor
 *          configurable memory size
 *          two-level I/O
 * 201118AP DMA_IN/DMA_OUT/INT processing
 *          counting MCLK
 *          Mike Riley's port extended + mapper
 *          ?D disassemble
 *          !A 1-line assembler
 *
 */

// TIM
//    'OUT7' 67 STROBE OUT
//    BUS0 - serial char output
//    BUS6 - reset PTP reader control
//    BUS7 - PTP reader control
//    EF4  - serial char input

// DISK
//    'OUT4' 64 DATA
//    'OUT5' 65 CMD
//    'INP6' 6E INPUT

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef unsigned char Byte;
typedef unsigned short Word;

#define Lo(x)           ((x) & 15)
#define Hi(x)           Lo((x) >> 4)
#define HiLo(x,y)       ((Lo(x) << 4) + Lo(y))
#define LO(x)           (0xFF & (x))
#define HI(x)           LO((x) >> 8)
#define HILO(x,y)       ((LO(x) << 8) + LO(y))
#define FLAG(x)         ((x) ? 1 : 0)
#define CY(x)           FLAG(0x10000 & (x))
#define MASK16          0xFFFF

#define LBR             r[P] = HILO(memrd(r[P]),memrd(r[P]+1))
#define NLBR            r[P] = r[P] + 2

int MAX_MEM;

Byte I, N;
Byte X, P, T;
int DF, D, Q, IE, EF;
#define EF1 1
#define EF2 2
#define EF3 4
#define EF4 8
Word r[16];
Byte *M = NULL;
int DMA_IN, DMA_OUT, INT;
Byte BUS, NLINES;
int MCLK;  // machine-cycle
int  TLIO; // Two-level I/O
Byte SEL;
int RC;    // Mark Riley, port extender
int MAPPER[16];

char *m, *ma[16], *a, *aa[16];

static char *gllregs[16] = {
   "DMA", "INT", "  R", "  P",
   " R4", " R5", " R6", " R7",
   " R8", "  W", "  A", "  B",
   "  U", "  I", "  S", "  F"
};

static char *cdpregs[16] = {
   " R0", " R1", " R2", " R3",
   " R4", " R5", " R6", " R7",
   " R8", " R9", " RA", " RB",
   " RC", " RD", " RE", " RF"
};

int trace = 0;
char **regs = cdpregs;

FILE *fdd[4];
int nfdd = 0;

Byte memrd(int addr)
{
   int bank;

   if (RC) {
      bank = (addr & 0x0F000) >> 12;
      addr = MAPPER[bank] | (addr & 0x0FFF);
   }
   return M[addr];
}

Byte memwr(int addr, Byte data)
{
   int bank;

   if (RC) {
      bank = (addr & 0x0F000) >> 12;
      addr = MAPPER[bank] | (addr & 0x0FFF);
   }
   M[addr] = data;
   return data;
}

void dasminit(void)
{
   int i;

   for (i = 0; i < 16; i++)
      ma[i] = aa[i] = 0;

   m = "0   INC DEC 3   LDA STR 6   7   GLO GHI PLO PLI 12  SEP SEX 15  ";
   a = "R";
   ma[ 0] = "IDL LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN ";
   aa[ 0] = "-RRRRRRRRRRRRRRR";
   ma[ 3] = "BR  BQ  BZ  BDF B1  B2  B3  B4  NBR BNQ BNZ BNF BN1 BN2 BN3 BN4 ";
   aa[ 3] = "1";
   ma[ 6] = "IRX OUT1OUT2OUT3OUT4OUT5OUT6OUT7INP0INP1INP2INP3INP4INP5INP6INP7";
   aa[ 6] = "-";
   ma[ 7] = "RET DIS LDXASTXDADC SDB SHRCSMB SAV MARKREQ SEQ ADCISDBISHLCSMBI";
   aa[ 7] = "------------##-#";
   ma[12] = "LBR LBQ LBZ LBDFNOP LSNQLSNZLSNFLSKPLBNQLBNZLBNFLSIELSQ LSZ LSDF";
   aa[12] = "2222-----222----";
   ma[15] = "LDX OR  AND XOR ADD SD  SHR SM  LDI ORI ANI XRI ADI SDI SHL SMI ";
   aa[15] = "--------######-#";
}

int dasm(Word p)
{
   Byte c0de;
   Word adr;
   char mnemo[5];
   char *args, arg, tmp[16];
   int i, nargs;

   c0de = memrd(p);
   args = a;
   strncpy(mnemo, m + 4*Hi(c0de), 4);
   mnemo[4] = '\0';
   if (isdigit(mnemo[0])) {
      i = atol(mnemo);
      args = aa[i];
      strncpy(mnemo, ma[i] + 4*Lo(c0de), 4);
      mnemo[4] = '\0';
   }
   arg = !args[1]? args[0] : args[Lo(c0de)];
   tmp[0] = '\0';
   nargs = 0;
   switch (arg) {
   case '-': break;
   case '#':
      nargs = 1;
      sprintf(tmp," %02XH", memrd(p+1));
      break;
   case 'R':
      sprintf(tmp," %s", regs[Lo(c0de)]);
      break;
   case '1':
      nargs = 1;
      adr = HILO(HI(p),memrd(p+1));
      sprintf(tmp," %04XH", adr);
      break;
   case '2':
      nargs = 2;
      adr = HILO(memrd(p+1),memrd(p+2));
      sprintf(tmp," %04XH", adr);
      break;
   }
   printf("%04X ", p);
   for (i = 0; i < 1 + nargs; i++)
      printf("%02X", memrd(p+i));
   for (; i < 4; i++)
      printf("  ");
   printf("%s", mnemo);
   if (tmp[0])
      printf("%s", tmp);
   printf("\n");
   return 1 + nargs;
}

char *prinb(int n, int m)
{
   static char buf[9];
   int i;

   i = 0;
   while (m) {
      buf[i++] = n & m? '1' : '0';
      m >>= 1;
   }
   buf[i] = '\0';
   return buf;
}

void prinregs()
{
   int i;

   printf("TIO:   %X SEL: %s\n", TLIO? 1 : 0, prinb(SEL, 128));
   printf("  P:   %X  IE:   %X   T:  %02X   D: %03X   X:   %X  EF:%s\n",
            P, IE, T, D + (DF? 256 : 0), X, prinb(EF, 8));
   for (i = 0; i < 16; i++) {
      printf("%s:%04X ", regs[i], r[i]);
      if (7 == i) printf("\n");
   }
   printf("\n");
   getchar();
}

void cycle(void)
{
   MCLK++;
}

Byte xinp(Byte port)
{
   if (port < 16)
      return MAPPER[port] >> 12;
   return BUS;
}

Byte inp(Byte port)
{
   if (TLIO && 1 == port)
      return SEL;
   if (RC && 5 == port)
      return SEL;
   if (RC && 6 == port)
      return xinp(SEL);
   return BUS;
}

void xout(Byte port, Byte data)
{
   if (port && port < 16)
      MAPPER[port] = data << 12;
   BUS = data;
}

void out(Byte port, Byte data)
{
   if (TLIO && 1 == port)
      SEL = data;
   else if (RC && 5 == port)
      SEL = data;
   else if (RC && 6 == port)
      xout(SEL, data);
   BUS = data;
}

void xecute(Word p)
{  
   Word W;
   int i;

   r[P] = p;
   while (1) {
      if (trace) {
         dasm(r[P]);
         printf("\n");
         prinregs();
      }
      W = memrd(r[P]); r[P]++; cycle();
      I = Hi(W); N = Lo(W);
      switch (I) {
      case 0:
         if (0==N) {
            // IDL, idle
            // wait for DMA or INT;
            BUS = memrd(r[0]);
            while (!DMA_IN && !DMA_OUT && !INT)
               cycle();
         } else {
            // LDN, load via N
            D = memrd(r[N]);
         }
         break;
      case 1: // INC, incr reg N      
         r[N]++;
         break;
      case 2: // DEC, decr reg N
         r[N]--;
         break;
      case 3: // short branch
         W = 0;
         switch (7 & N) {
         case 0: // BR, br (NBR)
            W = 1;
            break;
         case 1: // BQ, br if Q=1 (BNQ)
            W = (1 == Q);
            break;
         case 2: // BZ, br if D=0 (BNZ)
            W = (0 == D);
            break;
         case 3: // BDF, br if DF=1 (BNF)
            W = (1 == DF);
            break;
         case 4: // B1, br if EF1=1 (BN1)
            W = (0 != (EF1 & EF));
            break;
         case 5: // B2, br if EF1=1 (BN2)
            W = (0 != (EF2 & EF));
            break;
         case 6: // B3, br if EF2=1 (BN3)
            W = (0 != (EF3 & EF));
            break;
         case 7: // B4, br if EF4=1 (BN4)
            W = (0 != (EF4 & EF));
            break;
         }
         if (8 & N)
            W = !W;
         if (W)
            r[P] = HILO(HI(r[P]),memrd(r[P]));
         break;
      case 4: // LDA, load advance
         D = memrd(r[N]); r[N]++;
         break;
      case 5: // STR, store via N
         memwr(r[N], D);
         break;
      case 6: 
         if (0==N) {
            // IRX, incr reg X
            r[X]++;
         } else {
            if (8 & N) {
               // INP, input N
               NLINES = N;
               D = inp(NLINES);
               memwr(r[X], D);
            } else {
               // OUT, output N
               NLINES = N;
               out(NLINES, memrd(r[X])); r[X]++;
            }
         }
         break;
      case 7:
         switch (N) {
         case 0: // RET, return
            W = memrd(r[X]); r[X]++;
            X = Hi(W); P = Lo(W);
            IE = 1;
            break;
         case 1: // DIS, disable
            W = memrd(r[X]); r[X]++;
            X = Hi(W); P = Lo(W);
            IE = 0;
            break;
         case 2: // LDXA, load via X and advance
            D = memrd(r[X]); r[X]++;
            break;
         case 3: // STXD, store via X, decrement
            memwr(r[X], D); r[X]--;
            break;
         case 4: // ADC, add w/ carry
            D = memrd(r[X]) + D + DF;
            DF = CY(D); D &= MASK16;
            break;
         case 5: // SDB, sub D w/ borrow
            D =  memrd(r[X]) - D - (1 - DF);
            DF = FLAG(D < 0); D &= MASK16;
            break;
         case 6: // SHRC, shift right w/ carry
            W = D & 1;
            D = D >> 1;
            if (DF) D += 0x80;
            DF = W;
            break;
         case 7: // SMB, sub mem w/ borrow
            D = D - memrd(r[X]) - (1 - DF);
            DF = FLAG(D < 0); D &= MASK16;
            break;
         case 8: // SAV, save
            memwr(r[X], T);
            break;
         case 9: // MARK, push X,P to stack
            T = HiLo(X,P);
            memwr(r[2], T);
            X = P; r[2]--;
            break;
         case 0xA: // REQ, reset Q
            Q = 0;
            break;
         case 0xB: // SEQ, set Q
            Q = 1;
            break;
         case 0xC: // ADCI, add w/ cy imm
            D = memrd(r[P]) + D + DF; r[P]++;
            DF = CY(D); D &= MASK16;
            break;
         case 0xD: // SDBI, sub D w/ borrow imm
            D =  memrd(r[P]) - D - (1 - DF); r[P]++;
            DF = FLAG(D < 0); D &= MASK16;
            break;
         case 0xE: // SHLC, shift left w/ carry
            W = FLAG(D & 0x80);
            D = (D << 1) + DF;
            DF = W;
            break;
         case 0xF: // SMBI, sub mem w/ borrow imm
            D = D - memrd(r[P]) - (1 - DF); r[P]++;
            DF = FLAG(D < 0); D &= MASK16;
            break;
         }
         break;
      case 8: // GLO, get low reg N
         D = LO(r[N]);
         break;
      case 9: // GHI, get high reg N
         D = HI(r[N]);
         break;
      case 0xA: // PLO, put low reg N
         r[N] = HILO(HI(r[N]), D);
         break;
      case 0xB: // PHI, put high reg N
         r[N] = HILO(D, LO(r[N]));
         break;
      case 0xC:
         switch (N) {
         case 0: // LBR
            LBR;
            break;
         case 1: // LBQ, if Q=1
            if (1==Q) LBR;
            else NLBR;
            break;
         case 2: // LBZ, if D=0
            if (0==D) LBR;
            else NLBR;
            break;
         case 3: // LBDF, if DF=1
            if (1==DF) LBR;
            else NLBR;
            break;
         case 4: // NOP, nop
            break;
         case 5: // LSNQ, skip if Q=0
            if (0==Q) NLBR;
            break;
         case 6: // LSNZ, skip if D=0
            if (0==D) NLBR;
            break;
         case 7: // LSNF, skip if DF=0
            if (0==DF) NLBR;
            break;
         case 8: // NLBR, LSKP
            NLBR;
            break;
         case 9: // LBNQ
            if (0==Q) LBR;
            else NLBR;
            break;
         case 0xA: // LBNZ
            if (0==D) LBR;
            else NLBR;
            break;
         case 0xB: // LBNF
            if (0==DF) LBR;
            else NLBR;
            break;
         case 0xC: // LSIE, skip if IE=1
            if (1==IE) NLBR;
            break;
         case 0xD: // LSQ
            if (1==Q) NLBR;
            break;
         case 0xE: // LSZ
            if (0==D) NLBR;
            break;
         case 0xF: // LSDF
            if (1==DF) NLBR;
            break;
         }
         cycle();
         break;
      case 0xD: // SEP, set P
         P = N;
         break;
      case 0xE: // SEX, set X
         X = N;
         break;
      case 0xF:
         switch (N) {
         case 0: // LDX, load via X
            D = memrd(r[X]);
            break;
         case 1: // OR, or
            D = D | memrd(r[X]);
            break;
         case 2: // AND, and
            D = D & memrd(r[X]);
            break;
         case 3: // XOR, exclusive or
            D = D ^ memrd(r[X]);
            break;
         case 4: // ADD, add
            D = memrd(r[X]) + D;
            DF = CY(D); D &= MASK16;
            break;
         case 5: // SD, sub D
            D = memrd(r[X]) - D;
            DF = FLAG(D < 0); D &= MASK16;
            break;
         case 6: // SHR, shift right
            DF = D & 1; D = D >> 1;
            break;
         case 7: // SM, sub memory
            D = D - memrd(r[X]);
            DF = FLAG(D < 0); D &= MASK16;
            break;
         case 8: // LDI, load imm
            D = memrd(r[P]); r[P]++;
            break;
         case 9: // ORI, or imm
            D = D | memrd(r[P]); r[P]++;
            break;
         case 0xA: // ANI, and imm
            D = D & memrd(r[P]); r[P]++;
            break;
         case 0xB: // XRI, exclusive or imm
            D = D ^ memrd(r[P]); r[P]++;
            break;
         case 0xC: // ADI, add imm
            D = memrd(r[P]) + D; r[P]++;
            DF = CY(D); D &= MASK16;
            break;
         case 0xD: // SDI, sub D imm
            D = memrd(r[P]) - D; r[P]++;
            DF = FLAG(D < 0); D &= MASK16;
            break;
         case 0xE: // SHL, shift left
            DF = FLAG(D & 0x80); D = D << 1;
            break;
         case 0xF: // SMI, sub mem imm
            D = D - memrd(r[P]); r[P]++;
            DF = FLAG(D < 0); D &= MASK16;
            break;
         }
         break;
      } // switch I
      cycle();
      if (DMA_IN) {
         memwr(r[0], BUS); r[0]++;
      }
      else if (DMA_OUT) {
         BUS = memrd(r[0]); r[0]++;
      }
      else if (INT) {
         T = HiLo(X,P);
         X = 2; P = 1;
         IE = 0;
      }
   } // while
}

static int ucase(int c)
{
   if ('a' <= c && c <= 'z')
      c = c + 'A' - 'a';
   return c;
}

static int ishex(int c)
{
   return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F');
}

static int digit(int c)
{
   if (c <= '9')
      return c - '0';
   return 10 + c - 'A';
}

static int utc;

int utget(FILE *inp)
{
   int c;
   
   c = ucase(fgetc(inp));
   if (c != EOF && inp != stdin)
      printf("%c", c);
   return c;
}

void uteat(FILE *inp)
{
   while (utc != EOF && utc != '\n')
      utc = utget(inp);
}

char *utnam(FILE *inp)
{
   static char buf[32];
   int i;

   while (' ' == utc)
      utc = utget(inp);

   i = 0;
   while (' ' != utc && i < 31) {
      buf[i++] = utc;
      utc = utget(inp);
   }
   buf[i] = '\0';
   return buf;
}

Word utadr(FILE *inp)
{
   Word adr = 0;

   while (' ' == utc)
      utc = utget(inp);

   while (ishex(utc)) {
      adr = 16 * adr + digit(utc);
      utc = utget(inp);
   }

   return adr;
}

int utasm(FILE *inp, Word dst)
{
   char *mnemo, *args, *p;
   int  i, arg;
   Byte c0de;
   Word adr;

   mnemo = utnam(inp);
   if (strlen(mnemo) > 4)
      goto LError;
   p = strstr(m, mnemo);
   if (p) {
      c0de = HiLo((p - m) / 4, 0);
      args = a;
   }
   else {
      for (i = 0; i < 16; i++) {
         if (!ma[i]) continue;
         p = strstr(ma[i], mnemo);
         if (!p)
            continue;
         c0de = HiLo(i, (p - ma[i])/4);
         args = aa[i];
         break;
      }
   }
   arg = !args[1]? args[0] : args[Lo(c0de)];
   if (arg == 'R') {
      adr = utadr(inp);
      c0de += (adr & 0xF);
   }
   memwr(dst++, c0de);
   if (arg != '-' && arg != 'R') {
      adr = utadr(inp);
      if (arg == '2')
         memwr(dst++, HI(adr));
      memwr(dst++, LO(adr));
   }
   return dst;
LError:
   return -1;
}

int utbyt(FILE *inp)
{
   int byt = 0;

   if (!ishex(utc))
      return -1;
   byt = 16 * byt + digit(utc);
   utc = utget(inp);

   if (!ishex(utc))
      return -1;
   byt = 16 * byt + digit(utc);
   utc = utget(inp);

   return byt;
}

#define H   printf
#define O   fprintf

int ut40mon(FILE *inp)
{
   Word adr, len;
   int i, byt, cont;
   int err, unit, track;
   int bytes, cmd;
   char *fn;
   FILE *out;

   err = 0; utc = 0; bytes = 0;
   H("* ");
   while (!feof(inp)) {
      if (utc == '\n') {
         H("* ");
         utc = utget(inp);
      }
      while (utc != EOF && utc != '?' && utc != '!' && utc != '$')
         utc = utget(inp);

      switch (utc) {
      case '?':
         utc = utget(inp);
         if (utc == 'M') {
            utc = utget(inp);
            adr = utadr(inp);
            len = utadr(inp);
            if (!len) len = 1;
            for (i = 0; i < len; i++) {
               if (0 == (i & 15)) {
                  if (i)
                     H(";\n");
                  H("%04X", adr);
               }
               if (0 == (i & 1))
                  H(" ");
               H("%02X",memrd(adr++));
            }
            H("\n");
         }
         else if (utc == 'D') {
            utc = utget(inp);
            adr = utadr(inp);
            len = utadr(inp);
            if (!len) len = 1;
            len += adr;
            for (;adr < len;)
               adr += dasm(adr);
            H("\n");
         }
         else if (utc == 'R') {
            utc = utget(inp);
            for (i = 0; i < 16; i++) {
               if (8 == i) H("\n");
               H("%04X ", r[i]);
            }
            H("\n");
         }
         else
            goto LError;
         break;
      case '!':
         utc = utget(inp);
         if (utc == 'M') {
            utc = utget(inp);
            while (utc == ' ' || utc == '\n')
               utc = utget(inp);
            adr = utadr(inp);
            while (utc != EOF && utc != '\n') {
               while (utc == ' ')
                  utc = utget(inp);

               if (utc == ',') {
                  uteat(inp);
                  if (utc == EOF)
                     break;
                  if (utc != '\n')
                     goto LError;
                  utc = utget(inp);
                  continue;
               }
               else if (utc == ';') {
                  uteat(inp);
                  if (utc == EOF)
                     break;
                  if (utc != '\n')
                     goto LError;
                  utc = utget(inp);
                  adr = utadr(inp);
                  continue;
               }
               
               byt = utbyt(inp);
               if (byt < 0)
                  goto LError;
               memwr(adr++, byt); bytes++;
            }
         }
         else if (utc == 'A') {
            utc = utget(inp);
            while (utc == ' ' || utc == '\n')
               utc = utget(inp);
            adr = utadr(inp);
            H("start adr: %04X\n", adr);
            while (utc != EOF && utc != '\n') {
               while (utc == ' ')
                  utc = utget(inp);

               if (utc == ',') {
                  uteat(inp);
                  if (utc == EOF)
                     break;
                  if (utc != '\n')
                     goto LError;
                  utc = utget(inp);
                  continue;
               }

               byt = utasm(inp, adr);
               if (byt < 0)
                  goto LError;
               adr = byt;
            }
         }
         else
            goto LError;
         break;
      case '$':
         utc = utget(inp);
         if (utc == 'U' || utc == 'P') {
            cmd = utc;
            utc = utget(inp);
            adr = utadr(inp);
            if (cmd == 'U')
               TLIO++;
            else if (cmd == 'P')
               SEL = 0x00;
            for (i = 0; i < 16; i++)
               r[i] = HILO(memrd(0x8C00+2*i),memrd(0x8C00+2*i+1));
            xecute(adr);
            for (i = 0; i < 16; i++) {
               memwr(0x8C00+2*i+0, HI(r[i]));
               memwr(0x8C00+2*i+1, LO(r[i]));
            }
            if (cmd == 'U')
               TLIO--;
         }
         else if (utc == 'L') {
            uteat(inp);
            H("READ? ");
            utc = utget(inp);
            adr = utadr(inp);
            err = 1;
            if (adr) {
               unit = HI(adr); track = LO(adr);
               err = unit>3 || track>70 || track<1 || !fdd[unit];
               if (!err) {
                  H("loading (%X,%2X)...\n", unit, track);
                  fseek(fdd[unit], (track - 1) * 9 * 512, SEEK_SET);
                  ut40mon(fdd[unit]);
               }
            }
            if (err) {
               H("DRIVE NOT ON\n"); err = 0;
            }
         }
         else if (utc == 'X' || utc == 'Y') {
            // X: comma cont. 20bytes/line
            // Y: semicol cont. 16bytes/line
            cont = utc == 'X'? 20 : 16;

            utc = utget(inp);
            fn  = utnam(inp);
            adr = utadr(inp);
            len = utadr(inp);
            out = strcmp(fn,"TTY")? fopen(fn,"w+") : stdout;
            O(out,"!M");
            if (20 == cont)
               O(out,"%04X",adr);
            for (i = 0; i <= (len - adr); i++) {
               if (0 == (i % cont)) {
                  if (i)
                     O(out,"%c", 20==cont? ',' : ';');
                  O(out, "\n");
                  if (16 == cont)
                     O(out,"%04X",adr + i);
               }
               if ((16 == cont) && !(i & 1))
                  O(out," ");
               O(out,"%02X", memrd(adr + i));
            }
            O(out,"\n");
            if (out != stdout)
               fclose(out);
         }
         else if (utc == 'Q')
            exit(0);
         else
            goto LError;
         break;
      case EOF:
         break;
      default:
LError:  uteat(inp);
         H(" ?\n");
      }
   }

   return bytes;
}


int readbin(char *fn, int offs)
{
   FILE *fin;
   int bytes;
   char buf[128];

   fin = fopen(fn, "rb");
   bytes = 0;
   while (!feof(fin)) {
      if ((offs + bytes) > MAX_MEM)
         break;
      bytes += fread(&M[offs + bytes], 1, 128, fin);

   }
   fclose(fin);

   return bytes;
}

int readidi(char *fn, int offs)
{
   FILE *fin;
   int bytes;

   fin = fopen(fn, "rt");
   bytes = ut40mon(fin);
   fclose(fin);

   return bytes;
}

void fini(void)
{
   int i;

   if (MCLK)
      printf("MCLK = %d\n", MCLK);
   for (i = 0; i < 4; i++)
      if (fdd[i])
         fclose(fdd[i]);
}

void storage(void)
{
   if (M)
      return;

   M = (Byte *)malloc(MAX_MEM);
   if (!M) {
      fprintf(stderr,"out of storage\n");
      exit(1);
   }
}

void usage(void)
{
   fprintf(stderr,"usage: sim18 [-2gimutx] [-bxxxx] [-d -exxxx] [-f<fdd.img>] -s<kbytes> <file>...\n");
   fprintf(stderr,"options:\n");
   fprintf(stderr,"   -2         enable TLIO\n");
   fprintf(stderr,"   -bxxxx     disasm/load/start 'begin' address\n");
   fprintf(stderr,"   -d         disassemble only\n");
   fprintf(stderr,"   -exxxx     end address\n");
   fprintf(stderr,"   -ffdd.img  use disk file (max. 4)\n");
   fprintf(stderr,"   -g         GLL-MAG register names\n");
   fprintf(stderr,"   -m         set M-rec fmt\n");
   fprintf(stderr,"   -s<kbytes> memory size (default 64KB)\n");
   fprintf(stderr,"   -t         trace\n");
   fprintf(stderr,"   -u         start UT40 (?DMR,!AM,$LPQUXY)\n");
   fprintf(stderr,"   -x         Mark Riley's port extender+mapper\n");
   fprintf(stderr,"   file       load bin/M-rec fmt at 'begin' adr\n");
   fprintf(stderr,"\n");
   fprintf(stderr,"UT40 commands:\n");
   fprintf(stderr,"   ?Dadr len                 disassemble\n");
   fprintf(stderr,"   ?Madr len                 memory dump\n");
   fprintf(stderr,"   ?R                        register dump\n");
   fprintf(stderr,"   !Aadr instr[, instr..]    1-line asm\n");
   fprintf(stderr,"   !Madr bytes[,; bytes..]   memory entry\n");
   fprintf(stderr,"   $L                        load (unit,track)\n");
   fprintf(stderr,"   $Padr                     xecute\n");
   fprintf(stderr,"   $Q                        quit\n");
   fprintf(stderr,"   $Uadr                     xecute, TLIO enabled\n");
   fprintf(stderr,"   $X file adr len           xmit comma sep\n");
   fprintf(stderr,"   $Y file adr len           xmit semicolon sep\n");
   exit(1);
}

int main(int argc, char *argv[])
{
   FILE *fin;
   int i, bytes;
   int onlydasm, begin, end, ut40;
   char *endptr;
   int (*readdat)(char*,int);

   if (argc < 2)
      usage();

   regs = cdpregs;
   onlydasm = begin = end = 0;
   ut40 = 0;
   trace = 0;
   readdat = readbin;
   MAX_MEM = 64 * 1024;

   nfdd = 0;
   for (i = 0; i < 4; i++)
      fdd[i] = NULL;

   TLIO = 0; SEL = 0x00;

   RC = 0; SEL = 0;
   for (i = 0; i < 16; i++)
      MAPPER[i] = 0;

   atexit(fini);
   for (i = 1; i < argc; i++) {
      if (*argv[i] == '-') {
         switch (argv[i][1]) {
         case '2': TLIO = 1; break;
         case 'd': onlydasm = 1; break;
         case 'b': begin = strtol(argv[i] + 2, &endptr, 16); break;
         case 'e': end   = strtol(argv[i] + 2, &endptr, 16); break;
         case 'f':
            if (nfdd < 4) {
               fin = fopen(argv[i]+1, "r+");
               if (!fin)
                  fprintf(stderr,"no FDD %s",argv[i]+1);
               else
                  fdd[nfdd++] = fin;
            }
            else
               fprintf(stderr,"max. 4 FDDs\n");
            break;
         case 'g': regs = gllregs; break;
         case 'i': readdat = readidi; break;
         case 'u': ut40 = 1; break;
         case 's': MAX_MEM = atoi(argv[i] + 2);
                   if (MAX_MEM > 1024) MAX_MEM = 1024;
                   MAX_MEM *= 1024;
                   break;
         case 't': trace = 1; break;
         case 'x': RC = 1; break;
         default: usage();
         }
      }
      else {
         storage();
         printf("load %s at %04X\n", argv[i], begin);
         bytes = readdat(argv[i], begin);
         printf("read %d bytes\n", bytes);
         begin = 0; readdat = readbin;
      }
   }

   storage();

   I = 0; N = 0; Q = 0; IE = 1;
   X = 0; P = 0; r[0] = 0;
   DMA_IN = DMA_OUT = INT = 0;
   MCLK = 0;

   dasminit();

   if (onlydasm) {
      if (!end) end = bytes;
      for (i = begin; i <= end; )
         i += dasm(i);
   }
   else if (ut40) {
      SEL = 0x01;
      ut40mon(stdin);
   }
   else
      xecute(begin);

   return 0;
}

// vim:ts=3 sw=3 et:

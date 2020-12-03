// CDS II - CDP18S005

/*
 * History
 * =======
 *
 * 201114AP initial version
 * 201115AP disassembler, GLL-MAG register names
 * 201116AP initial FDD support
 * 201117AP built-in UT20 monitor (BUT20)
 *          configurable memory size
 *          two-level I/O
 * 201118AP DMA_IN/DMA_OUT/INT processing
 *          counting MCLK
 *          Mike Riley's port extended + mapper
 *          ?D disassemble
 *          !A 1-line assembler
 *          INP0 stops xecute loop
 *          'Q' stops tracing
 * 201119AP fixed INP/OUT asm/disasm
 *          IDL in UT20 stops xecute loop
 *          HH, HHHH, assembles constant
 *          fixed SD/SDB asm
 *          fixed borrow gen, 8bit results in D
 *          fixed not-taken branches
 *          simplified LBR/LSKP
 *          TLIO 1854 char I/O
 *          fixed NOP
 *          fig-FORTH register names
 *          fixed SHL
 * 201120AP fixed carry/borrow gen
 *          updated disk geometry: 128 b/sec * 26 sec/tr * 76 tr
 * 201122AP fixed page-crossing BR
 *          TIM and FDD interfaces
 *          LPR interface
 * 201123AP FDD interface
 *          switchable UART iface group
 * 201124AP FDD interface
 *          MPM-232 disk geometry & controller
 *          break with Ctrl-C
 *          stop at IDL
 *          fig-FORTH disk read works
 *          fig-FORTH disk write works
 * 201125AP BUT40 brkptr $Badr 
 * 201126AP bytes read in hex
 * 201127AP $L reads track# in decimal
 *          handle/emit DC3 EOF
 * 201202AP register name definitions with -r
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
#include <errno.h>

#include "curterm.h"

typedef unsigned char Byte;
typedef unsigned short Word;

#define H   printf
#define O   fprintf

#define Lo(x)           ((x) & 15)
#define Hi(x)           Lo((x) >> 4)
#define HiLo(x,y)       ((Lo(x) << 4) + Lo(y))
#define LO(x)           (0xFF & (x))
#define HI(x)           LO((x) >> 8)
#define HILO(x,y)       ((LO(x) << 8) + LO(y))
#define FLAG(x)         ((x) ? 1 : 0)
#define CY(x)           FLAG(0x100 & (x))
#define MASK8           0x00FF

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
unsigned MCLK;  // machine-cycle
int  TLIO; // Two-level I/O
Byte SEL;
int RC;    // Mark Riley, port extender
int MAPPER[16];
int GRP_UART = 2; // default UART group

char *m, *ma[16], *a, *aa[16];

static char *cdpregs[16] = {
   "R0 ", "R1 ", "R2 ", "R3 ",
   "R4 ", "R5 ", "R6 ", "R7 ",
   "R8 ", "R9 ", "RA ", "RB ",
   "RC ", "RD ", "RE ", "RF "
};

static char *custregs[16];

int trace = 0, brkadr = 0;
int ut20 = 0;
int noidle = 1; // no IDL processing
char **regs = cdpregs;

#define NFDD   4
FILE *fdd[NFDD];
int fddro[NFDD];
int nfdd = 0;

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

/*
 *  TIM - CDP18S507
 *
 *  2MHz - 0.5us
 *  16CLK - 8us
 *  baud  us    MCLK
 *    110 9091  2273
 *    300 3333   833
 *   1200  833   208
 *
 *  Format: 7/-/2 LSB first, parity bit ignored
 *
 */

#define TIM_110   2273
#define TIM_300    833
#define TIM_1200   208

int SIOTX, SIO_OUT, SIO_DLY, SIO_OCNT;
int SIORX, SIO_IN0, SIO_IN, SIO_ICNT;

void timrst(void)
{
   // serial input - chk char
   SIOTX = 0;
   // EF4 - idle
   EF |= EF4;
   SIORX = 0;
   SIO_IN0 = 1;
}

void timin(void)
{
   int c;

   switch (SIOTX) {
   case 0: EF |= EF4;                        // idle state, wait for KBD
           if (MCLK & 32767)
               return;
           if (has_key()) {
               c = getkey(0);
// H("got %02X\r\n",c);
               kbflush();
               c &= 0x7F;
               putchar(c); fflush(stdout);   // local echo
               SIO_OUT = (c << 1);           // 1S+7D
               SIO_OUT |= 0x700;             // 1P+2S
// H("sending %03X\r\n",SIO_OUT);
               SIO_DLY = TIM_300;            // 300baud
               SIO_OCNT = 0x0B;              // send 11bits: 1S+7D+1P+2S
               SIOTX = 1;
           }
           break;
   case 1: if (SIO_DLY)                      // wait
               SIO_DLY--;
           else {
               SIO_DLY = TIM_300;            // reset delay
               if (SIO_OCNT && SIO_OCNT--) {
                   if (1 & SIO_OUT)          // send LSB
                       EF |=  EF4;
                   else
                       EF &= ~EF4;
// H("sent %d\r\n",EF & EF4);
                   SIO_OUT >>= 1;
               }
               else
                   SIOTX = 0;                // back to wait for char
           }
           break;
   }
}

void timout(Byte data)
{
   int bus0;

   bus0 = 1 & data;
// H("rcvd %d\r\n",bus0);
   if (0 == SIORX) {                   // idle, wait for char
      if ((1 == SIO_IN0) && !bus0) {   // detect START bit
         SIO_IN = 0;
         SIO_ICNT = 0x0B;              // rcv 11bits: 1S+7D+1P+2S
         SIORX = 1;
// H("begin\r\n");
      }
   }
   if (1 == SIORX) {                   // receive
      if (SIO_ICNT && SIO_ICNT--) {
         if (bus0) {
            SIO_IN |= (1 << 10);       // get bit
         }
         SIO_IN >>= 1;
      }
      if (!SIO_ICNT) {
// H("end\r\n");
// H("rcvd %03X\r\n",SIO_IN);
         if (0x300 == (0x300 & SIO_IN)) { // detect 2 STOP bits
            SIO_IN &= 0x7F;
            putchar(SIO_IN);
            fflush(stdout);
         }
         SIORX = 0;                    // back to idle state
      }
   }
   SIO_IN0 = bus0; // save curr BUS0
}

/*
 * DSK - CDP18S813
 *
 * IBM 33FD
 *    128 bytes/sector, 26 sectors/track, 77 tracks/disk
 *    tracks:  0 - 76
 *    sectors: 1 - 26 
 *
 * transfer rate: 250kbit/s
 *
 *                      ms    MCL
 * sector read/write     6    1500
 * one track move       10    2500
 * head settling        20    5000
 *
 * controller capability
 * -   4 drives
 * - 128 tracks
 * -  32 sectors/track
 *
 */
#define DSK_BSEC      128
#define DSK_STRK       26
#define DSK_TRKS       77

#define DSK_RWSECT   1500
#define DSK_TRSEEK   2500
#define DSK_HDSETL   5000

struct {
   Byte cmd;
   Byte data;
   Byte input;
   Byte unit;
   Byte sector;
   Byte track;
   Byte status;
   Byte rbuf[DSK_BSEC], rptr;
   Byte wbuf[DSK_BSEC], wptr;

   int  busy;       // busy delay;
   Byte prevtr;     // prev. track to calc seek time
   long offtr;      // curr. track offset
} DSK;

#define  DE_BSY   0x01
#define  DE_CRC   0x08
#define  DE_WRP   0x10
#define  DE_FAIL  0x20
#define  DE_ACT   0x40

void fddrst(void)
{
   DSK.busy   = 0;
   DSK.status = 0;
   DSK.prevtr = random() % DSK_TRKS;
   DSK.offtr  = DSK_STRK * DSK_BSEC * DSK.prevtr;
}

void fddcyc(void)
{
   if (DSK.busy && !--DSK.busy) {
// H("/BSY\r\n");
      DSK.status &= ~DE_BSY;
      DSK.input   = DSK.status;     // update input from disk ctrl
   }
}

int skipcmd(Byte data)
{
   return 0x41 == data || 0x31 == data;
}

void fddctrl(Byte data)
{
   size_t s;
   int sent;

//   if (!skipcmd(data))
//      H("FDD %02X",data);

   sent = 0;
   DSK.cmd = data;
   switch (data) {
   case 0x00: // read status
      DSK.input = DSK.status; sent = 1;
      break;
   case 0x03: // read sector
      if (!fdd[DSK.unit])
         DSK.status |= DE_FAIL;
      else {
         DSK.status |= DE_BSY;
         DSK.busy = DSK_RWSECT;
         s = fseek(fdd[DSK.unit], DSK.offtr+DSK_BSEC*(DSK.sector-1), SEEK_SET);
         if (!(s < 0)) {
            s = fread(DSK.rbuf, sizeof(Byte), DSK_BSEC, fdd[DSK.unit]);
            if (DSK_BSEC != s)
               DSK.status |= DE_FAIL;
         }
         else
            DSK.status |= DE_FAIL;
      }
      break;
   case 0x05: // write sector
      if (!fdd[DSK.unit])
         DSK.status |= DE_FAIL;
      if (DE_WRP & DSK.status)
         DSK.status |= DE_FAIL;
      else {
         DSK.status |= DE_BSY;
         DSK.busy = DSK_RWSECT;
         s = fseek(fdd[DSK.unit], DSK.offtr+DSK_BSEC*(DSK.sector-1), SEEK_SET);
         if (s < 0)
            DSK.status |= DE_FAIL;
         else {
            s = fwrite(DSK.wbuf, sizeof(Byte), DSK_BSEC, fdd[DSK.unit]);
            if (DSK_BSEC != s)
               DSK.status |= DE_FAIL;
            else
               fflush(fdd[DSK.unit]);
            DSK.wptr = 0;
         }
      }
      break;
   case 0x07: // read CRC
      DSK.status |= DE_BSY;
      DSK.busy = DSK_RWSECT;
      DSK.status &= ~DE_CRC;
      break;
   case 0x09: // seek track
LSEEK:
      if (!fdd[DSK.unit])
         DSK.status |= DE_FAIL;
      else {
         DSK.status |= DE_BSY;
         DSK.busy  = DSK_HDSETL + abs(DSK.track - DSK.prevtr) * DSK_TRSEEK;
// H(" BSY=%04X", DSK.busy);
         DSK.offtr = DSK_STRK * DSK_BSEC * DSK.track;
         s = fseek(fdd[DSK.unit], DSK.offtr, SEEK_SET);
         if (s < 0)
            DSK.status |= DE_FAIL;
         else
            DSK.prevtr = DSK.track;
      }
      break;
   case 0x0B: // clear error flags
      DSK.status &= ~(DE_CRC | 0x80);
      break;
   case 0x0D: // seek 0
      DSK.track = 0;
      goto LSEEK;
      break;
   case 0x11: // load track#, (0 - 76)
      DSK.status |= DE_BSY;
      DSK.busy = 12;
      DSK.track = DSK.data;
// H(" T=%02X", DSK.track);
      if (DSK.track > (DSK_TRKS - 1))
         DSK.status |= DE_CRC;
      break;
   case 0x21: // load unit-sec#, (0 - 3), (1 - 26)
      DSK.unit = (0xC0 & DSK.data) >> 6;
      DSK.sector = (0x1F & DSK.data);
// H(" U/S=%02X U=%X S=%02X", DSK.data, DSK.unit, DSK.sector);
      if (!fdd[DSK.unit])
         DSK.status |= DE_FAIL;
      else if (!DSK.sector || DSK.sector > DSK_STRK)
         DSK.status |= DE_CRC;
      else {
         DSK.status &= ~0x06;
         DSK.status |= (DSK.unit << 1);
         DSK.status |= DE_ACT;
         if (fddro[DSK.unit])
            DSK.status |= DE_WRP;
      }
      break;
   case 0x31: // load write buffer
      DSK.wbuf[DSK.wptr] = DSK.data;
      DSK.wptr = 0x7F & (DSK.wptr + 1);
      break;
   case 0x40: // read buffer
      DSK.rptr = 0;
      DSK.input = DSK.rbuf[DSK.rptr]; sent = 1;
      break;
   case 0x41: // shift read buffer
      DSK.rptr = 0x7F & (DSK.rptr + 1);
      DSK.input = DSK.rbuf[DSK.rptr]; sent = 1;
      break;
   case 0x81: // clear
      DSK.busy = 0;
      DSK.status &= ~DE_BSY;
      break;
   }
   // send status
   if (!sent)
      DSK.input = DSK.status;

//    if (!skipcmd(data))
//        H("\r\n");
}

/*
 * LPR - Line Printer
 *
 * 150 lines/min, 120 chars/line
 *
 * 833 MCLK/char
 *
 */
char lprnm[32] = "lpr.out";
FILE *lpr = NULL;
int LPR_OUT, LPR_DLY;

void lprout(Byte data)
{
   if (!LPR_DLY) {
      LPR_OUT = ~data;
      LPR_DLY = 833;
      EF |= EF1;
   }
}

void lprin(void)
{
   if (LPR_DLY && !--LPR_DLY) {
      if (!lpr)
         lpr = fopen(lprnm, "a");
      fputc(LPR_OUT, lpr);
      fflush(lpr);
      EF &= ~EF1;
   }
}

void dasminit(void)
{
   int i;

   for (i = 0; i < 16; i++)
      ma[i] = aa[i] = 0;

   m = "0   INC DEC 3   LDA STR 6   7   GLO GHI PLO PHI 12  SEP SEX 15  ";
   a = "R";
   ma[ 0] = "IDL LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN ";
   aa[ 0] = "-RRRRRRRRRRRRRRR";
   ma[ 3] = "BR  BQ  BZ  BDF B1  B2  B3  B4  SKP BNQ BNZ BNF BN1 BN2 BN3 BN4 ";
   aa[ 3] = "11111111-1111111";
   ma[ 6] = "IRX OUT OUT OUT OUT OUT OUT OUT INP INP INP INP INP INP INP INP ";
   aa[ 6] = "-NNNNNNNNNNNNNNN";
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
   case 'N':
      sprintf(tmp," %d", 7 & Lo(c0de));
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
}

void cycle(void)
{
   MCLK++;
   if (TLIO && 1 == SEL) {
      timin();
      lprin();
      fddcyc();
   }
}

Byte eflag(int x)
{
//H("EF %d\r\n",EF & x);
   return EF & x;
}

Byte xinp(Byte port)
{
   if (port < 16)
      return MAPPER[port] >> 12;
   return BUS;
}

Byte inp(Byte port)
{
   int c;
// H("INP %d\r\n",port);

   if (TLIO)
      switch (port) {
      case 1: return SEL;
      case 2: if (GRP_UART == SEL) {
                 fflush(stdout); // 1854 char in
                 c = getchar();
                 if (c == '\n')
                    c = '\r';
                 return c;
              }
              break;
      case 3: if (GRP_UART == SEL) {
                 return 0x81; // 1854 status
              }
              break;
      case 6: if (1 == SEL) {
                 return DSK.input;
              }
              break;
      }
   else if (RC)
      switch (port) {
      case 5: return SEL;
      case 6: xinp(SEL);
      }
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
// H("%04X OUT %d,%02X\r\n",r[P],port,data);
   if (TLIO)
      switch (port) {
      case 1: SEL = data; break;
      case 2: if (GRP_UART == SEL) {
                 data &= 0x7F; // 1854 char out
                 putchar(data);
              }
              break;
      case 3: if (GRP_UART == SEL) {
                 ; // 1854 ctrl
              }
              break;
      case 4: if (1 == SEL) {
                 DSK.data = data;
              }
              break;
      case 5: if (1 == SEL) {
                 fddctrl(data);
              }
              break;
      case 6: if (1 == SEL) {
                 lprout(data);
              }
              break;
      case 7: if (1 == SEL) {
                 timout(data);
                 if (0x04 & data) // deselect TLIO
                    TLIO = 0;
              }
              break;
      }
   else if (RC)
      switch (port) {
      case 5: SEL = data;
      case 6: xout(SEL, data);
      }
   BUS = data;
}

void fini(void);

void xecute(Word p)
{  
   Word W;
   int cond, done;

   r[P] = p; done = 0;
   while (!done) {
      if (has_ctrlc()) {
         H("\r\n\r\nCtrl-C pressed!\r\n");
         done = 1;
         break;
      }
      if (brkadr && brkadr == r[P])
         trace = 1;
      if (trace) {
         H("\n"); dasm(r[P]);
         H("\n"); prinregs();
         cond = ucase(getchar());
         if ('C' == cond)
            trace = 0;
         else if ('Q' == cond)
            break;
      }
      W = memrd(r[P]); r[P]++; cycle();
      I = Hi(W); N = Lo(W);
      switch (I) {
      case 0:
         if (0==N) {
            // IDL, idle
            // wait for DMA or INT;
            if (noidle) {
               H("idle\r\n");
               fflush(stdout);
               done = 1;
            }
            else {
               fflush(stdout);
               BUS = memrd(r[0]);
               while (!DMA_IN && !DMA_OUT && !INT)
                  cycle();
            }
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
         cond = 0;
         switch (7 & N) {
         case 0: // BR, br (NBR,SKP)
            cond = 1;
            break;
         case 1: // BQ, br if Q=1 (BNQ)
            cond = (1 == Q);
            break;
         case 2: // BZ, br if D=0 (BNZ)
            cond = (0 == D);
            break;
         case 3: // BDF, br if DF=1 (BNF)
            cond = (1 == DF);
            break;
         case 4: // B1, br if EF1=1 (BN1)
            cond = (0 != eflag(EF1));
            break;
         case 5: // B2, br if EF1=1 (BN2)
            cond = (0 != eflag(EF2));
            break;
         case 6: // B3, br if EF2=1 (BN3)
            cond = (0 != eflag(EF3));
            break;
         case 7: // B4, br if EF4=1 (BN4)
            cond = (0 != eflag(EF4));
            break;
         }
         if (8 & N)
            cond = !cond;
         if (cond) {
            W = memrd(r[P]);
            r[P] = HILO(HI(r[P]),W);
         }
         else
            r[P]++;
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
               if (8 == N)
                  done = 1;
               else {
                  NLINES = 7 & N;
                  D = inp(NLINES);
                  memwr(r[X], D);
               }
            } else {
               // OUT, output N
               NLINES = 7 & N;
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
            DF = CY(D); D &= MASK8;
            break;
         case 5: // SDB, sub D w/ borrow
            D =  memrd(r[X]) - D - (1 - DF);
            DF = 1 - FLAG(D < 0); D &= MASK8;
            break;
         case 6: // SHRC, shift right w/ carry
            W = D & 1;
            D = D >> 1;
            if (DF) D += 0x80;
            DF = W;
            break;
         case 7: // SMB, sub mem w/ borrow
            D = D - memrd(r[X]) - (1 - DF);
            DF = 1 - FLAG(D < 0); D &= MASK8;
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
            DF = CY(D); D &= MASK8;
            break;
         case 0xD: // SDBI, sub D w/ borrow imm
            D =  memrd(r[P]) - D - (1 - DF); r[P]++;
            DF = 1 - FLAG(D < 0); D &= MASK8;
            break;
         case 0xE: // SHLC, shift left w/ carry
            D = (D << 1) + DF;
            DF = CY(D); D &= MASK8;
            break;
         case 0xF: // SMBI, sub mem w/ borrow imm
            D = D - memrd(r[P]) - (1 - DF); r[P]++;
            DF = 1 - FLAG(D < 0); D &= MASK8;
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
         switch (7 & N) {
         case 0: // LBR (NLBR, LSKP)
            cond = 1;
            break;
         case 1: // LBQ, if Q=1 (LBNQ)
            cond = (1==Q);
            break;
         case 2: // LBZ, if D=0 (LBNZ)
            cond = (0==D);
            break;
         case 3: // LBDF, if DF=1 (LBNF)
            cond = (1==DF);
            break;
         case 4: // NOP, nop (LSIE)
            cond = 8&N? (0==IE) : 0;
            break;
         case 5: // LSNQ, skip if Q=0 (LSQ)
            cond = (0==Q);
            break;
         case 6: // LSNZ, skip if D=0 (LSZ)
            cond = (0!=D);
            break;
         case 7: // LSNF, skip if DF=0 (LSDF)
            cond = (0==DF);
            break;
         }
         if (8 & N)
            cond = !cond;

         if (4 & N) {
            if (cond) NLBR;
         }
         else if (cond) LBR;
         else NLBR;

         cycle();
         break;
      case 0xD: // SEP, set P
         P = N;
         break;
      case 0xE: // SEX, set X
         X = N;
         break;
      case 0xF:
         W = 8 & N? P : X;
         switch (7 & N) {
         case 0: // LDX, load via X (LDI)
            D = memrd(r[W]);
            break;
         case 1: // OR, or (ORI)
            D = D | memrd(r[W]);
            break;
         case 2: // AND, and (ANI)
            D = D & memrd(r[W]);
            break;
         case 3: // XOR, exclusive or (XRI)
            D = D ^ memrd(r[W]);
            break;
         case 4: // ADD, add (ADI)
            D = memrd(r[W]) + D;
            DF = CY(D); D &= MASK8;
            break;
         case 5: // SD, sub D (SDI)
            D = memrd(r[W]) - D;
            DF = 1 - FLAG(D < 0); D &= MASK8;
            break;
         case 6: // SHR, shift right (SHL)
            if (8 & N) {
               D = D << 1;
               DF = CY(D); D &= MASK8;
               N = 7 & N;
            }
            else {
               DF = D & 1; D = D >> 1;
            }
            break;
         case 7: // SM, sub memory (SMI)
            D = D - memrd(r[W]);
            DF = 1 - FLAG(D < 0); D &= MASK8;
            break;
         }
         if (8 & N) r[P]++;
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
   prinregs();
   fflush(stdout);
}

#define DC3 0x13

int issep(int c)
{
   return c == ' ' || c == ',' || c == ';' || c == '\n';
}

static int utc;

int uteof(FILE *inp)
{
   return feof(inp) || DC3 == utc;
}

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

void utskip(FILE *inp, int nl)
{
   while (utc == ' ' || (nl && utc == '\n'))
      utc = utget(inp);
}

char *utnam(FILE *inp)
{
   static char buf[32];
   int i;

   while (' ' == utc)
      utc = utget(inp);

   i = 0;
   while (!issep(utc) && i < 31) {
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

int utasm(FILE *inp, Word dst)
{
   char *mnemo, *args, *p;
   int  i, arg, sel, len;
   Byte c0de;
   Word adr;

   mnemo = utnam(inp);
   len = strlen(mnemo);
   if (len > 4)
      goto LError;
   for (i = len; i < 4; i++)
      mnemo[i] = ' ';
   mnemo[i] = '\0';
   args = 0;
   p = strstr(m, mnemo);
   if (p) {
      sel  = (p - m) / 4;
      c0de = HiLo(sel, 0);
      args = a;
   }
   else {
      for (i = 0; i < 16; i++) {
         if (!ma[i]) continue;
         p = strstr(ma[i], mnemo);
         if (!p)
            continue;
         sel  = (p - ma[i]) / 4;
         c0de = HiLo(i, sel);
         args = aa[i];
         if (c0de == 0x01 || c0de == 0x61)
            c0de--;
         break;
      }
   }
   if (!args) {
      adr = 0;
      while (ishex(*mnemo))
         adr = 16 * adr + digit(*mnemo++);
      if (len > 2)
         memwr(dst++, HI(adr));
      memwr(dst++, LO(adr));
      return dst;
   }
   arg = !args[1]? args[0] : args[sel];
   if (arg == 'R' || arg == 'N') {
      adr = 0x0F & utadr(inp);
      if (arg == 'N') adr &= 0x07;
      c0de += adr;
   }
   memwr(dst++, c0de);
   if (arg != '-' && arg != 'R' && arg != 'N') {
      adr = utadr(inp);
      if (arg == '2')
         memwr(dst++, HI(adr));
      memwr(dst++, LO(adr));
   }
   return dst;
LError:
   return -1;
}

int ut20mon(FILE *inp)
{
   Word adr, len;
   int i, byt, cont;
   int err, unit, track;
   int bytes, cmd;
   char *fn;
   FILE *out;
   int savutc = utc;

   err = 0; utc = 0; bytes = 0;
   H("*");
   while (!uteof(inp)) {
      if (utc == '\n') {
         H("*");
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
            utskip(inp,1);
            adr = utadr(inp);
            utskip(inp,1);
            while (utc != EOF && utc != '\n') {
               utskip(inp,0);

               if (utc == ',' || utc == ';') {
                  byt = utc;
                  uteat(inp);
                  if (utc == EOF)
                     break;
                  if (utc != '\n')
                     goto LError;
                  utc = utget(inp);
                  if (';' == byt)
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
            utskip(inp,1);
            adr = utadr(inp);
            while (utc != EOF && utc != '\n') {
               utskip(inp,0);

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
         if (utc == 'B') {
            utc = utget(inp);
            brkadr = utadr(inp);
         } 
         else if (utc == 'U' || utc == 'P') {
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
               unit = HI(adr);
               track = LO(adr); track = 10*Hi(track) + Lo(track); // decimal!
               err = unit>3 || track>(DSK_TRKS-1) || !fdd[unit];
               if (!err) {
                  H("loading (%X,%2X)...\n", unit, track);
                  fseek(fdd[unit], track * DSK_STRK * DSK_BSEC, SEEK_SET);
                  ut20mon(fdd[unit]);
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
            if (out != stdout) {
               O(out,"%c",DC3); // write DC3
               fclose(out);
            }
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

   utc = savutc;
   return bytes;
}

int readregs(char *fn)
{
   FILE *fin;
   char buf[128];
   int i, n;

   fin = fopen(fn, "r");
   if (!fin) return 1;
   for (i = 0; i < 16; i++) {
      fscanf(fin, "%s", buf);
      strcat(buf, "   ");
      n = strlen(buf);
      if (n > 3)
         buf[3] = '\0';
      custregs[i] = strdup(buf);
   }
   fclose(fin);
   return 0;
}


int readbin(char *fn, int offs)
{
   FILE *fin;
   int bytes;

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
   bytes = ut20mon(fin);
   fclose(fin);

   return bytes;
}

void fini(void)
{
   int i;

   prepterm(0);
   if (lpr)
      fclose(lpr);
   if (MCLK)
      printf("MCLK = %u\n", MCLK);
   for (i = 0; i < NFDD; i++)
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
   fprintf(stderr,"usage: sim18 [-24imprutx] [-bxxxx] [-d -exxxx] [-f<fdd.img>] -s<kbytes> <file>...\n");
   fprintf(stderr,"options:\n");
   fprintf(stderr,"   -2         enable TLIO\n");
   fprintf(stderr,"   -4         fig-Forth compat, UART on Grp 1\n");
   fprintf(stderr,"   -bxxxx     disasm/load/start 'begin' address\n");
   fprintf(stderr,"   -c         calc18 register names\n");
   fprintf(stderr,"   -d         disassemble only\n");
   fprintf(stderr,"   -exxxx     end address\n");
   fprintf(stderr,"   -f<fdd>    use disk file (max. 4)\n");
   fprintf(stderr,"   -i         enable IDL processing\n");
   fprintf(stderr,"   -m         set M-rec fmt\n");
   fprintf(stderr,"   -p<file>   lpr file name, default \"lpr.out\"\n");
   fprintf(stderr,"   -r<regdef> read register names from <regdef>\n");
   fprintf(stderr,"   -s<kbytes> memory size (default 64KB)\n");
   fprintf(stderr,"   -t         trace\n");
   fprintf(stderr,"   -u         start BUT20 (?DMR,!AM,$BLPQUXY)\n");
   fprintf(stderr,"   -x         Mark Riley's port extender+mapper\n");
   fprintf(stderr,"   file       load bin/M-rec fmt at 'begin' adr\n");
   fprintf(stderr,"\n");
   fprintf(stderr,"BUT20 commands:\n");
   fprintf(stderr,"   ?Dadr len                 disassemble\n");
   fprintf(stderr,"   ?Madr len                 memory dump\n");
   fprintf(stderr,"   ?R                        register dump\n");
   fprintf(stderr,"   !Aadr instr[, instr..]    1-line asm\n");
   fprintf(stderr,"   !Madr bytes[,; bytes..]   memory entry\n");
   fprintf(stderr,"   $Badr                     set brkptr, 0 disable\n");
   fprintf(stderr,"   $L                        load (unit,track)\n");
   fprintf(stderr,"   $Padr                     xecute\n");
   fprintf(stderr,"   $Q                        quit\n");
   fprintf(stderr,"   $Uadr                     xecute, TLIO enabled\n");
   fprintf(stderr,"   $X file adr len           xmit comma sep\n");
   fprintf(stderr,"   $Y file adr len           xmit semicolon sep\n");
   fprintf(stderr,"\n");
   fprintf(stderr,"Trace commands:\n");
   fprintf(stderr,"   C                         continue\n");
   fprintf(stderr,"   Q                         stop execution\n");
   exit(1);
}

int main(int argc, char *argv[])
{
   FILE *fin;
   int i, bytes;
   int onlydasm, begin, end;
   char *endptr;
   int (*readdat)(char*,int);

   if (argc < 2)
      usage();

   srandom(1802);

   regs = cdpregs;
   onlydasm = begin = end = 0;
   ut20 = 0;
   trace = 0;
   noidle = 1;
   readdat = readbin;
   MAX_MEM = 64 * 1024;

   nfdd = 0;
   for (i = 0; i < 4; i++) {
      fdd[i] = NULL;
      fddro[i] = 0;
   }

   TLIO = 0; SEL = 0x00;

   RC = 0; SEL = 0;
   for (i = 0; i < 16; i++)
      MAPPER[i] = 0;

   atexit(fini);
   for (i = 1; i < argc; i++) {
      if (*argv[i] == '-') {
         switch (argv[i][1]) {
         case '2': TLIO = 1; break;
         case '4': GRP_UART = 1; break;
         case 'd': onlydasm = 1; break;
         case 'b': begin = strtol(argv[i] + 2, &endptr, 16); break;
         case 'e': end   = strtol(argv[i] + 2, &endptr, 16); break;
         case 'f':
            if (nfdd < 4) {
               fin = fopen(argv[i]+2, "r+");
               if (!fin) {
                  if (EACCES == errno) {
                     fddro[nfdd] = 1;
                     fin = fopen(argv[i]+2, "r");
                  }
               }
               if (!fin)
                  fprintf(stderr,"no FDD %s",argv[i]+2);
               else
                  fdd[nfdd++] = fin;
            }
            else
               fprintf(stderr,"max. 4 FDDs\n");
            break;
         case 'i': noidle = 0; break;
         case 'm': readdat = readidi; break;
         case 'p': strcpy(lprnm, argv[i]+2);
                   break;
         case 'r': if (readregs(argv[i]+2))
                      fprintf(stderr,"no reg defs %s",argv[i]+2);
                   else
                      regs = custregs;
                   break;
         case 'u': ut20 = 1; break;
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
         printf("read %d (#%02X) bytes\n", bytes, bytes);
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
   else if (ut20) {
      SEL = 0x01;
      ut20mon(stdin);
   }
   else {
      if (TLIO) {
         prepterm(1);
         cbreak(0);
         timrst();
         fddrst();
      }
      xecute(begin);
   }

   return 0;
}

// vim:ts=3 sw=3 et:

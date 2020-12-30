/* Memory dump and disassembler */

printable(c) {
   return ((31 < c && c < 128)? c : '.');
}

htoi(s) {
   register i, c, n;

   n = i = 0;
   while ((c = char(s,i++)) != '*e' && c != ' ')
      n = (n << 4) + c - (c <= '9'? '0' : 'A' - 10);
   return (n);
}

dump(a,n) {
   register i, j;

   i=0;
   while (i<n) {
      puthex4(a); putchar(' ');
      j=0;
      while (j<8) {
         putchar(' '); puthex2(char(0,a + j++));
      }
      putchar('  '); /* 2B */
      j=0;
      while (j++<8)
         putchar(printable(char(0,a++)));
      i =+ 8;
      putchar('*n');
   }
}

/* disassembler */

#define Lo(x)  (x & 017)
#define Hi(x)  Lo(x >> 4)

dasm(p) {
   extrn m, a, ma, aa;
   auto c0de, arg, nargs, N;
   register i, mnemo, args;

   c0de = char(0,p); N = Lo(c0de);
/* puthex4(p); putchar(' '); puthex2(c0de); puthex(Hi(c0de)); putchar(' '); puthex(N); putchar('*n'); */
   args = a;
   mnemo = m + 2*Hi(c0de);
/* puts(mnemo); putchar('*n'); */
   if (char(mnemo,1) == ' ') {
      i = htoi(mnemo,10);
/* puts("change to "); printn(i,10); putchar('*n'); */
      args = aa[i];
      mnemo = ma[i] + 2*N;
/* puts(mnemo); putchar('*n'); */
   }
   /* arg = ('*e' == char(args,1))? char(args,0) : char(args,N); */
   arg = char(args,('*e' == char(args,1))? 0 : N) - '0';
   
   nargs = (arg & 3) + 1;
   puthex4(p); putchar(' ');
   i = 0;
   while (i < nargs)
      puthex2(char(0,p+i++));
   while (i++ < 4)
      putchar('  '); /* 2B */
   putchar(mnemo[0]);
   putchar(mnemo[1]);
   putchar(' ');

   switch arg {
   case 0:
      goto end;
   case 1:
      puthex2(char(0,p+1));
      goto end;
   case 2:
      puthex4((char(0,p+1) << 8) + char(0,p+2));
      goto end;
   case 4:
   case 8:
      if (arg == 4) N =& 7;
      puthex(N);
      goto end;
   }
end:
   putchar('*n');
   return (nargs);
}

/* dasm tables */
m "0   INC DEC 3   LDA STR 6   7   GLO GHI PLO PHI C   SEP SEX F   ";
a "8";
ma[0]
   "IDL LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN LDN ",
   1,
   2,
   "BR  BQ  BZ  BDF B1  B2  B3  B4  SKP BNQ BNZ BNF BN1 BN2 BN3 BN4 ",
   4,
   5,
   "IRX OUT OUT OUT OUT OUT OUT OUT INP INP INP INP INP INP INP INP ",
   "RET DIS LDXASTXDADC SDB SHRCSMB SAV MARKREQ SEQ ADCISDBISHLCSMBI",
   8,
   9,
   10,
   11,
   "LBR LBQ LBZ LBDFNOP LSNQLSNZLSNFLSKPLBNQLBNZLBNFLSIELSQ LSZ LSDF",
   13,
   14,
   "LDX OR  AND XOR ADD SD  SHR SM  LDI ORI ANI XRI ADI SDI SHL SMI ";
   
aa[0]
   "0888888888888888",
   1,
   2,
   "1111111101111111",
   4,
   5,
   "0444444444444444",
   "0000000000001101",
   8,
   9,
   10,
   11,
   "2222000002220000",
   13,
   14,
   "0000000011111101";

prompt(msg) {
   auto buf 16;
   register s;

   puts(msg);
   gets(s = buf);
   return (htoi(s));
}

main() {
   register a, n, i;

   a = prompt(" address: $");
   n = prompt("  length: $");

   puts("memory dump:*n");
   dump(a, n);

   puts("*ndisassembly:*n");
   i = 0;
   while (i < n) {
      i =+ dasm(a + i);
   }
}

#define NEED_hex
#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

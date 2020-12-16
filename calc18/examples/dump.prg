/* Memory dump or disassembler */

hex(n) {
   putchar(char("0123456789ABCDEF",n&017));
}

hex2(n) {
   hex(n >> 4); hex(n);
}

hex4(n) {
   hex2(n >> 8); hex2(n);
}

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

   i=0; a =>> 1;
   while (i<n) {
      hex4(2*a); putchar('');
      j=0;
      while (j<8) {
         putchar(' '); hex2(char(a,j++));
      }
      putchar('  ');
      j=0;
      while (j++<8)
         putchar(printable(char(a,i++)));
      a =+ 4;
      putchar('*n');
   }
}

/* disassembler */

#define Lo(x)  (x & 017)
#define Hi(x)  Lo(x >> 4)

dasm(p) {
   extrn m, a, ma, aa;
   auto c0de, arg, nargs, I, N;
   register i, mnemo, args;

   c0de = char(0,p); I = Hi(c0de); N = Lo(c0de);
/* hex4(p); putchar(' '); hex2(c0de); hex(I); putchar(' '); hex(N); putchar('*n'); */
   args = a;
   mnemo = m + 2*I;
/* puts(mnemo); putchar('*n'); */
   if (char(mnemo,1) == ' ') {
      i = htoi(mnemo,10);
/* puts("change to "); printn(i,10); putchar('*n'); */
      args = aa[i];
      mnemo = ma[i] + 2*N;
/* puts(mnemo); putchar('*n'); */
   }
   arg = ('*e' == char(args,1))? char(args,0) : char(args,N);
   
   nargs = (arg - '0') & 3;
   nargs++;
   hex4(p); putchar(' ');
   i = 0;
   while (i < nargs)
      hex2(char(0,p+i++));
   while (i++ < 4)
      putchar('  ');
   i = 0;
   while (i < 4)
      putchar(char(mnemo,i++));
   putchar(' ');

   switch (arg - '0') {
   case 0:
      goto end;
   case 1:
      hex2(char(0,p+1));
      goto end;
   case 2:
      hex4((char(0,p+1) << 8) + char(0,p+2));
      goto end;
   case 4:
   case 8:
      if (arg == '4') N =& 7;
      hex(N);
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

#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

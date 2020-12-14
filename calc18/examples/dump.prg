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

dump(a,n) {
   auto i, j;

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
   auto c0de, mnemo, args, arg, hastmp, tmp, i, nargs, I, N;

   c0de = char(0,p); I = Hi(c0de); N = Lo(c0de);
/* hex4(p); putchar(' '); hex2(c0de); hex(I); putchar(' '); hex(N); putchar('*n'); */
   args = a;
   mnemo = m + 2*I;
/* puts(mnemo); nl(); */
   if (isdigit(char(mnemo,0))) {
      i = atoi(mnemo,10);
/* puts("change to "); printn(i,10); nl(); */
      args = aa[i];
      mnemo = ma[i] + 2*N;
/* puts(mnemo); nl(); */
   }
   arg = ('*e' == char(args,1))? char(args,0) : char(args,N);
   
   nargs = 0; hastmp = 0;
   switch (arg) {
   case '-':
      goto end;
   case '1':
      nargs = 1; hastmp = 1;
      tmp = char(0,p+1);
      goto end;
   case '2':
      nargs = 2; hastmp = 1;
      tmp = (char(0,p+1) << 8) + char(0,p+2);
      goto end;
   case 'N':
   case 'R':
      hastmp = 1; tmp = N;
      if (arg == 'N') tmp =& 7;
      goto end;
   }
end:
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
   if (hastmp)
      printn(tmp,16);
   putchar('*n');
   return (nargs);
}

/* dasm tables */
m "0   INC DEC 3   LDA STR 6   7   GLO GHI PLO PHI 12  SEP SEX 15  ";
a "R";
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
   "-RRRRRRRRRRRRRRR",
   1,
   2,
   "11111111-1111111",
   4,
   5,
   "-NNNNNNNNNNNNNNN",
   "------------11-1",
   8,
   9,
   10,
   11,
   "2222-----222----",
   13,
   14,
   "--------111111-1";

prompt(msg) {
   auto s, buf 16;

   puts(msg);
   gets(s = &buf[0]);
   return (atoi(s,16));
}

main() {
   auto a, n, i;

   a = prompt(" address: ");
   n = prompt(" length: ");

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

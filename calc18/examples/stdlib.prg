/* stdlib functions */

/*
 * History
 * =======
 * 201229AP gethex/puthex
 * 201228AP vectored I/O: ttyputc/ttygetc, ioget[]/ioput[] vectors
 *          rd.unit/wr.unit
 * 201227AP disk sector read/write
 *
 */

xputc(c) { /* translate char */
   if (!c || c == '*e')
      return;
   if (c == '*n')
      putc(13);
   putc(c);
}

ttyputc(c) { /* put char to UART */
   extrn putc;
   register h;

   xputc(c & 0377);
   if (h = c >> 8)
      xputc(h);
   return (c);
}

ttygetc() { /* get char from UART */
   extrn getc;
   auto c;

   c = getc();
   if (c == 13)
      c = '*n';
   ttyputc(c); /* echo */
   return (c);
}

ttygo() {
   extrn ttyinit, ioget, ioput, ttygetc, ttyputc;

   ioget[0] = &ttygetc;
   ioput[1] = &ttyputc;
   ttyinit = 1;
}

putchar(c) {
   extrn ttyinit, ioput, wr.unit;

   if (!ttyinit)
      ttygo();
   return ((ioput[wr.unit])(c,wr.unit));
}

getchar() {
   extrn ttyinit, ioget, rd.unit;

   if (!ttyinit)
      ttygo();
   return ((ioget[rd.unit])(rd.unit));
}

puts(s) {
   register c,i;

   i = 0;
   while ((c = char(s,i++)) != '*e')
      putchar(c);
}

gets(s) { /* get string */
   register i, c;

   i = 0;
   while ('*n' != (c = getchar()))
      lchar(s,i++,c);
   lchar(s,i,'*e');
}

printn(n,b) { /* print number in base b */
   extrn DIGITS;
   register a;

   if (a=n/b)
      printn(a,b);
   putchar(char(DIGITS,n%b));
}

#ifdef NEED_hex
puthex(n) {
   extrn DIGITS;

   putchar(char(DIGITS,n));
}

puthex2(n) {
   puthex(n >> 4);
   puthex(n & 017);
}

puthex4(n) {
   puthex2(n >> 8);
   puthex2(n & 0377);
}

gethex() {
   auto c;

   c = getchar();
   if (c >= 'A')
      return (c - 'A' + 10);
   return (c - '0');
}

gethex2() {
   auto n;

   n = gethex();
   return ((n << 4) + gethex());
}

gethex4() {
   auto n;

   n = gethex2();
   return ((n << 8) + gethex2());
}
#endif

#ifdef NEED_atoi
isdigit(c) {
   return ('0' <= c && c <= '9');
}

isnum(c) {
   return (isdigit(c) || ('A' <= c && c <= 'Z'));
}

digit(c) {
   if (c <= '9')
      return (c - '0');
   return (10 + c - 'A');
}

atoi(s,b) { /* convert string to number in base b */
   auto c;
   register n, d;

   n = 0;
   while (isnum(c = char(0,s++))) {
      d = digit(c);
      if (b - 1 < d)
         return (0177777);
      n = b * n + d;
   }
   return (n);
}
#endif

#ifdef NEED_str
strcopy(d,s) {
   register i;

   i = 0;
   while (lchar(d,i,char(s,i)) != '*e')
      i++;
   return(d);
}

concat(a,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10) { /* terminate w/ zero */
   auto i, adx;
   register c, j, x;

   j = 0; adx = &x1;
   while (x = *adx++) {
      i = 0;
      while ((c=char(x,i++)) != '*e')
         lchar(a,j++,c);
   }
   lchar(a,j,'*e');
   return (a);
}

strlen(bStr) {
   auto i;

   i = 0;
   while (char(0,bStr++) != '*e') i++;
   return (i);
}

strchr(bStr,b) {
   auto c;

   while ((c = char(0,bStr)) != '*e') {
      if (b == c)
         return (bStr);
      bStr++;
   }
   return (0);
}

strncmp(s1,s2,n) {
   auto i, c1, c2;

   i = 0;
   while (i < n && (c1 = char(0,s1)) == (c2 = char(0,s2))) {
      s1++; s2++;
   }
   return (c1 - c2);
}
#endif

#ifdef NEED_getarg
getarg(a,s,n) {
   register c, i;

   while(((c = char(s,n)) != '*e') && (c == ' ' || c == '*t'))
      n++;
   i = 0;
   while(((c = char(s,n)) != '*e') && (c != ' ' && c != '*t')) {
      lchar(a,i++,c); n++;
   }
   lchar(a,i,'*e');
   return (n);
}
#endif

/* Thompson: User's Reference to B */

#ifdef NEED_printf
printf(fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9) {
   extrn printn, char, putchar;
   auto adx, i;
   register c, j, x;

   i = 0; /* fmt index */
   adx = &x1;  /* argument pointer */
loop:
   while ((c=char(fmt,i++)) != '%') {
      if ('*e' == c)
         return;
      putchar(c);
   }
   x = *adx++;
   switch (c = char(fmt,i++)) - 'a' {
   case 'd' - 'a': /* decimal */
      if (x & 0100000) {
         x = -x;
         putchar('-');
      }
   case 'o' - 'a': /* octal */
      printn(x,c=='o'?8:10);
      goto loop;

   case 'c' - 'a': /* char */
      putchar(x);
      goto loop;

   case 's' - 'a': /* string */
      j = 0;
      while ((c = char(x,j++)) != '*e')
         putchar(c);
      goto loop;
   }
   putchar('%');
   i--;
   adx--;
   goto loop;
}
#endif

#ifdef NEED_diskio
dskgo() {
   extrn ioget, ioput;
   auto i;

   i = 2;
   while (i < 8) {
      ioget[i] = 0; /* &fgetc */
      ioput[i] = 0; /* &fputc */
      i++;
   }
   homdsk();
}

mkdcb(unit,track,sector) {
   extrn DCB;

   DCB[1] = 0;
   DCB[0] = track + (--sector << 8);
   if (unit)
      DCB[0] =+ 040000;
   return (DCB);
}

dskrd(dcb,buf) {
   auto i, stc;

   i = 0;
   while (i < 128) {
      stc = eread(&dcb[1]);
      if (!(stc & 0400))
         puts("eread() failed*n");
      lchar(buf,i++,stc);
   }
}

dskwr(dcb,buf) {
   auto i, c, st;

   i = 0;
   while (i < 128) {
      c = char(buf,i++);
      st = ewrite(&dcb[1],c);
      if (!st)
         puts("ewrite() failed*n");
   }
}

DCB[2] 0, 0;
#endif

DIGITS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
rd.unit 0;
wr.unit 1;
ttyinit 0;
ioget[8];
ioput[8];

/* vim: set ts=3 sw=3 et: */

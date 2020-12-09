/* stdlib functions */

putci(c) {
   if (!c || c == '*e')
      return;
   if (c == '*n')
      putc(13);
   putc(c);
}

putchar(c) { /* put char to UART */
   extrn putc;

   putci(c & 0377);
   putci(c >> 8);
   return (c);
}

nl() {
   putchar('*n');
}

getchar() { /* get char from UART */
   extrn getc;
   auto c;

   c = getc();
   if (c == 13)
      c = '*n';
   return (c);
}

char(s,i) { /* i-th char of string s */
   auto n;

   n = *(s+(i>>1));
   /* s[] contains "ABCD", accessed a 'BA' 'DC' */
   if (i&01)
      n=n>>8;
   return (n&0377);
}

lchar(s,i,c) { /* store char c in the i-th pos of string s */
   auto n, x;

   n = *(s + (x = i>>1));
   if (i&1)
      n = (n & 0377) + (c << 8);
   else
      n = (n & 0177400) + c;
   *(s+x) = n;
}

puts(s) {
   auto c,i;

   i = 0;
   while ((c = char(s,i++)) != '*e')
      putchar(c);
}

gets(s) { /* get string */
   auto i, c;

   i = 0;
   while ('*n' != (c = getchar())) {
      lchar(s,i++,c);
      putchar(c);
   }
   putchar(c);
   lchar(s,i,'*e');
}

printn(n,b) { /* print number in base b */
   auto a;

   if (a=n/b)
      printn(a,b);
   /* putchar(n % b + '0'); */
   putchar(char("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",n%b));
}

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
   auto i, c, n, d;

   n = i = 0;
   while (isdigit(c = char(s,i++))) {
      d = digit(c);
      if (b - 1 < d)
         return (0177777);
      n = b * n + digit(c);
   }
   return (n);
}

/* vim: set ts=3 sw=3 et: */

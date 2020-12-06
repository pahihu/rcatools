/* stdlib functions */
putchar(c) { /* put char to UART */
   if (c == '*n')
      putc(13);
   putc(c);
}

nl() {
   putchar('*n');
}

getchar() { /* get char from UART */
   auto c;
   c = getc();
   if (c == 13)
      c = '*n';
   return (c);
}

gets(s) { /* get string */
   auto c;

   while ('*n' != (c = getchar()))
      putchar(*s++ = c);
   putchar(c);
   *s++ = '*0';
}

prints(s) { /* print string */
   auto c;

   while (c = *s++)
      putchar(c);
}

printn(n,b) { /* print number in base b */
   auto a;

   if (a=n/b)
      printn(a,b);
   putchar(n % b + '0');
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

ston(s,b) { /* convert string to number in base b */
   auto c, n, d;

   n = 0;
   while (isdigit(c = *s++)) {
      d = digit(c);
      if (b - 1 < d)
         return (0177777);
      n = b * n + digit(c);
   }
   return (n);
}


g;
v[10];
buf[16];

putchar(c) {
   if (c == '*n')
      putc(13);
   putc(c);
}

itoa(n) {
   auto a, sign;

   sign = 0;
   if (n & 0100000) {
      sign = 1; n = -n;
   }
   a = &buf[16];
   *--a = 0;
   while (n) {
      *--a = '0' + n % 10;
      n = n / 10;
   }
   if (sign) {
      *--a = '-';
   }
   return (a);
}

puts(s) {
   auto c;
   while (c = *s++)
      putchar(c);
}

/* Thompson: B Manual */
printn(n,b) {
   auto a;

   if (a=n/b) /* assignment, not test for equality */
      printn(a,b); /* recursive */
   putchar(n%b + '0');
}

main() {
   auto c, a;

   g = 'G'; v[0] = 'V'; v[9] = '9';
   putchar('*n');
   c = putchar('E'); putchar('*n');
   putchar(c); putchar('*n');
   putchar(g); putchar('*n');
   putchar(v[0]); putchar('*n');
   putchar(v[9]); putchar('*n');
   a = itoa(42); puts(a);
   printn(42,10); putchar('*n');
   printn(42,8); putchar('*n');
   printn(42,2); putchar('*n');
}

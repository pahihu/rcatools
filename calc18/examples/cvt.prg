putchar(c) {
   if (c == '*n')
      putc(13);
   putc(c);
}

getchar() {
   auto c;
   c = getc();
   if (c == 13)
      c = '*n';
   return (c);
}

gets(s) {
   auto c;

   while ('*n' != (c = getchar()))
      putchar(*s++ = c);
   putchar(c);
   *s++ = '*0';
}

puts(s) {
   auto c;

   while (c = *s++)
      putchar(c);
}

printn(n,b) {
   auto a;

   if (a=n/b)
      printn(a,b);
   putchar(n % b + '0');
}

ishex(c) {
   return (('0' <= c && c <= '9') || ('A' <= c && c <= 'F'));
}

digit(c) {
   if (c <= '9')
      return (c - '0');
   return (10 + c - 'A');
}

hex(s) {
   auto c, n;

   n = 0;
   while (ishex(c = *s++))
      n = (n << 4) + digit(c);
   return (n);
}

main() {
   auto s, buf 16, n;

   gets(s = &buf[0]); putchar('*n');
   puts(s); putchar('*n');
   n = hex(s);
   printn(n, 10); putchar('*n');
}

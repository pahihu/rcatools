/* calc18 Fibonacci example */

nl() {
   putc(13); putc(10);
}

itoa(n) {
   extrn buf;
   auto a;

   a = &buf[16];
   *--a = 0;
   while (n) {
      *--a = '0' + n % 10;
      n = n / 10;
   }
   return (a);
}

puts(s) {
   auto c;
   while (c = *s++)
      putc(c);
}

puti(n) {
   puts(itoa(n));
}

fib(n) {
   if (n < 2)
      return (n);
   return (fib(n - 1) + fib(n - 2));
}

main() {
   auto a, n;

   n = fib(23); 
   puti(n); nl();
}

buf[16];

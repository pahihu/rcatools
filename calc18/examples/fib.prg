/* calc18 Fibonacci example */

fib(n) {
   if (n < 2)
      return (n);
   return (fib(n - 1) + fib(n - 2));
}

main() {
   auto n;

   n = fib(24);
   printn(n, 10); putchar('*n');
}

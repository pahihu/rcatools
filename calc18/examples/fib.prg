/* calc18 Fibonacci example */

#include "stdlib.prg"

fib(n) {
   if (n < 2)
      return (n);
   return (fib(n - 1) + fib(n - 2));
}

main() {
   auto a, n;

   n = fib(23); 
   printn(n, 10); nl();
}

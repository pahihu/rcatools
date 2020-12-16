#include "stdlib.prg"

putn(n) {
   printn(n,10); putchar(' ');
}

main() {
   auto n, d, r1, r2;

   n = 0;
Loop:
   d = 1;
   while (d < 256) {
      r1 = n / d; r2 = n % d;
      putchar('*n'); putn(n); putn(d); putn(r1); putn(r2);
      d++;
   } 
   if (++n) goto Loop;
   putchar('*n');
}

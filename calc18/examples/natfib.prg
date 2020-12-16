/* calc18 Fibonacci example */

#include "stdlib.prg"

/* NB. ln -s natfib.sr include.sr */

main() {
   auto n;

   /* hack to pass arg to natfib() */
   n = 24; n = natfib();
   printn(n, 10); putchar('*n');
}

/* vim: set ts=3 sw=3 et: */

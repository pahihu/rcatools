#include "stdlib.prg"

show(msg,n) {
   puts(msg); putchar(' '); printn(n,10); nl();
}

main() {
   auto x, y, z;

   x = 1; y = 2; z = 3;

   x =+ 4;
   y =* 256;
   z =- 2;
   show("x",x);
   show("y",y);
   show("z",z);
}

/* vim: set ts=3 sw=3 et: */

#include "stdlib.prg"

show(msg,n) {
   puts(msg); putchar(' '); printn(n,10); nl();
}

main() {
   auto x, y, z;
   register a, b, c;

   x = 1; y = 5; z = 3;

   x =+ 4;
   y =* 256;
   z =- 2;

   show("x",x);
   show("y",y);
   show("z",z);

   a = 1; b = 2; c = 3;

   a =+ 4;
   b =| 8;
   b =* 256;
   c =- 2;
 
   show("a",a);
   show("b",b);
   show("c",c);

   x = 0;
   a = -!x++;
   show("x = 0",a);
   x = 0;
   a = -!x++;
   show("x = 1",a);
}

/* vim: set ts=3 sw=3 et: */

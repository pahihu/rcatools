show(msg,n) {
   printf("%s %d*n",msg,n);
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
   x = 1;
   a = -!x++;
   show("x = 1",a);
   x = 42;
   show("**&x = ",*&x);
   show("&(**x) = ",&*x);
}

#define NEED_printf
#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

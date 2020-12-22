printf(fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9) {
   auto buf 128;

   sprintf(buf,fmt,x1,x2,x3,x4,x5,x6,x7,x8,x9);
   puts(buf);
}

main() {
   printf("%s + %s is %d%!*n","alpha","beta",42);
}

#include "sprintf.prg"
#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

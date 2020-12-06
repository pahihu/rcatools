#include "stdlib.prg"

main() {
   auto s, buf 16, n;

   gets(s = &buf[0]); nl();
   prints(s); nl();
   n = ston(s, 16);
   printn(n, 10); nl();
}

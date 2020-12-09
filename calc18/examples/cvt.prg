#include "stdlib.prg"

main() {
   auto s, buf 16, n;

   gets(s = &buf[0]); nl();
   puts(s); nl();
   n = atoi(s, 16);
   printn(n, 10); nl();
}

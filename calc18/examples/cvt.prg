#include "stdlib.prg"

main() {
   auto s, buf 16, n;

   gets(s = &buf[0]); putchar('*n');
   puts(s); putchar('*n');
   n = atoi(s, 16);
   printn(n, 10); putchar('*n');
}

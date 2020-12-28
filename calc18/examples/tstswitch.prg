main() {
   auto buf 16, s, n;

   while (1) {
      puts(" enter a number: ");
      gets(s = &buf[0]);
      n = atoi(2*s, 10);
      puts("got "); printn(n, 10); putchar('*n');
      if (!n) goto end;
      switch n {
      case 1:
         puts("one*n");
         goto loop;
      case 2:
      case 3:
         puts("two or three*n");
         goto loop;
      case 10:
         puts("ten*n");
         goto loop;
      }
      puts("none*n");
loop: ;
   }
end: puts("bye!*n");
   return;
}

#define NEED_atoi
#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

#include "stdlib.prg"
#include "char.prg"

main() {
   auto buf 16, s, n;

   while (1) {
      puts(" enter a number: ");
      gets(s = &buf[0]);
      n = ston(s, 10);
      puts("got "); printn(n, 10); nl();
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

/* vim: set ts=3 sw=3 et: */

#include "fds.h"

/*
 * MEM FILE <BEGIN> <END>
 * MEM FILE <END>
 * MEM FILE <BEGIN> <END> <START>
 */

main() {
   extrn NArgs, Args, FDErr, wr.unit;
   auto uni;
   auto beg, end, start;
   auto s, i, c;
   auto fnm MAXNAME_W, old.wr;

   uni = 0;
   if (NArgs < 3 || NArgs > 5)
      return (FDErr = FDE_SYNTAX);

   if (fdParseFName(&uni, Args[1]))
      return (FDErr = FDE_SYNTAX);

   start = beg = 0;
   if (3 == NArgs)
      end = atoi(Args[2],16);
   else {
      start = beg = atoi(Args[2],16); end = atoi(Args[3],16);
      if (5 == NArgs)
         start = atoi(Args[4],16);
   }

   s = end - beg + 1;
   s = 6 + (2 * s) + ((s + 19) / 20) * 2 + 1 + 6 + 1;

   if (openw(2,Args[1],s))
      return (FDErr);
   old.wr = wr.unit; wr.unit = 2;

   /* save to disk */
   puts("!M"); puthex4(beg); putchar('*n');
   i = 0;
   while (beg < end) {
      c = char(0,beg++);
      puthex2(c);
      if (0 == (++i % 20))
         putchar(',*n');
   }
   puts("$U"); puthex4(start); putchar('*n');
   fdClose(2);
   wr.unit = old.wr;

   return (0);
}

/* vim: set ts=3 sw=3 et: */

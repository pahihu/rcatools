#include "fds.h"

/* TYPE FILE */
main() {
   extrn NArgs, Args, FDErr, rd.unit;
   auto c;

   if (2 != NArgs)
      return (FDErr = FDE_SYNTAX);
   if (openr(2,Args[1]))
      return (FDErr);

   old.rd = rd.unit; rd.unit = 2;
   c = getchar();
   while (!eof(rd.unit)) {
      putchar(c);
      c = getchar();
   }
   close(rd.unit); rd.unit = old.rd;

   return (0);
}

/* vim: set ts=3 sw=3 et: */

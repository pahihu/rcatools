#include "fds.h"

/* COPY FILE1[:UNI] FILE2[:UNI] */
main() {
   extrn NArgs, Args, FDErr, FDesc, rd.unit, wr.unit;
   auto c, cb2;
   auto old.rd, old.wr;

   if (3 != NArgs)
      return (FDErr = FDE_SYNTAX);

   if (openr(2,Args[1]))
      return (FDErr);
   cb2 = FDesc + (2 << 4); /* sizeof_IOCB */
   if (openw(3,Args[2],cb2[IOCB_size])) {
      close(2);
      return (FDErr);
   }

   old.rd = rd.unit; old.wr = wr.unit;
   rd.unit = 2;
   wr.unit = 3;
   c = getchar();
   while (!eof(rd.unit)) {
      putchar(c);
      c = getchar();
   }
   close(rd.unit); rd.unit = old.rd;
   flush(); wr.unit = old.wr;

   return (0);
}

/* vim: set ts=3 sw=3 et: */

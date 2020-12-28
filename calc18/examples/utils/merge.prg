#include "fds.h"

/* MERGE FILE1 FILE2 FILE3 ... FILEN */
main() {
   extrn NArgs, Args, FDErr, FDesc, rd.unit, wr.unit;
   auto i;
   auto prevss, ss;
   auto c, cb2, old.rd, old.wr;

   if (NArgs < 3)
      return (FDErr = FDE_SYNTAX);

   cb2 = FDesc + (2 << 4); /* sizeof_IOCB */

   /* calc size */
   ss = prevss = 0;
   i = 2;
   while (i < NArgs) {
      if (char(0,Args[i]) == 0)
         ss++;
      else {
         if (openr(2,Args[i]))
            return (FDErr);
         ss =+ cb2[IOCB_size];
         close(2);
      }
      if (ss < prevss)
         return (FDErr = FDE_DISKFULL);
      prevss = ss;
      i++;
   }

   if (openw(3,Args[1],ss))
      return (FDErr);

   old.wr = wr.unit; wr.unit = 3;

   i = 2;
   while (i < NArgs) {
      if (char(0,Args[i]) == 0)
         putchar(DC3);
      else {
         if (openr(2,Args[i]))
            return (FDErr);
         old.rd = rd.unit; rd.unit = 2;
         c = getchar();
         while (!eof(rd.unit))  {
            putchar(c);
            c = getchar();
         }
         close(rd.unit); rd.unit = old.rd;
      }
   }
   flush(); wr.unit = old.wr;

   return (0);
}

/* vim: set ts=3 sw=3 et: */

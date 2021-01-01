#include "fds.h"

/* EXAM [UNI] TRACK SEC */
main() {
   extrn NArgs, Args, FDErr, FDU;
   auto uni, t, s;
   auto i, j, c;
   auto buf;
   auto w;

   uni = 0;
   if (3 < NArgs)
      return (FDErr = FDE_SYNTAX);

   t = atoi(Args[1],10); s = atoi(Args[2],10);
   if (NArgs > 3) {
      uni = t; t = s; s = atoi(Args[3]);
   }

   fdSelect(uni);
   buf = fdRead(mkdcb(FDU,t,s),0);
   i = 0;
   while (i < 8) {
      w = buf;
      j = 0;
      while (j++ < 8) {
         hex4(*w++); putchar(' ');
      }
      putchar(' ');
      j = 0;
      while (j < 16) {
         c = char(buf,j++);
         if (c < 32)
            c = '.';
         putchar(0277 & c);
      }
      putchar('*n');
      buf =+ 8;
      i++;
   }

   return (0);
}

/* vim: set ts=3 sw=3 et: */

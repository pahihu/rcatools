#include "fds.h"

/* SYSGEN [UNI] */
main() {
   extrn NArgs, Args, FDErr;
   auto uni;
   auto i, s;
   auto map;
   auto dummy MAXNAME_W;
   auto dir;

   uni = 0;
   if (NArgs > 1)
      uni = atoi(Args[1],10);

   fdSelect(uni);

   /* initialize track map, t0 allocated */
   map = fdRead(mkdcb(FDU,0,MAP),0);
   i = 0;
   while (i < MAXBUF_W)
      map[i++] = 0;
   lchar(map,0,1);
   fdWrite(FDCB,0);

   // write directory file
   lchar(dummy,0,'*e');
   dummy = BYTES(dummy);
   s = DIRBEG;
   while (s < DIREND) {
      dir = fdRead(mkdcb(FDU,0,s),0);
      i = 0; de = dir;
      while (i++ < MAXDIRENT) {
         fdStr2FName(de, dummy);
         de[DIRENT_tracks] = DE_EMPTY;
         de[DIRENT_size]   = 0;
         de =+ sizeof_DIRENT;
      }
      fdWrite(FDCB,0);
      s++;
   }

   return (0);
}

/* vim: set ts=3 sw=3 et: */

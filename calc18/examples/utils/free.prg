#include "fds.h"

#define DIRBEG  2
#define DIREND 18

/* FREE [UNIT] */
main() {
   extrn NArgs, Args, FDErr, FDU;
   auto uni;
   auto s, i;
   auto nt, nf, map;
   auto dir, de;

   uni = 0;
   if (1 != NArgs)
      uni = atoi(Args[1],10);

   fdSelect(uni);

   /* scan dir entries */
   nf = 0;
   s = DIRBEG;
   while (s < DIREND) {
      dir = fdRead(mkdcb(FDU,0,s),0);
      i = 0; de = dir;
      while (i++ < MAXDIRENT) {
         if (DE_EMPTY == (0177 & de[DIRENT_tracks]))
            nf++;
         de =+ sizeof_DIRENT;
      }
   }

   /* read map, count free tracks */
   nt = 0;
   map = fdRead(DCB(FDU,0,1),0);
   map = BYTES(map);
   i = 0;
   while (i++ < 77)
      if (0 == char(0,map++))
         nt++;

   printn(nt,10); puts(" TRACKS "); printn(nf,10); puts(" FILES*n");

   return (0);
}

/* vim: set ts=3 sw=3 et: */

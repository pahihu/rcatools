#include "fds.h"

/* DIR [PAT][:UNI] */
main() {
   extrn NArgs, Args, FDErr;
   auto uni, bPat;

   uni = 0; bPat = 0;
   if (NArgs > 1) {
      if (fdParseFName(&uni, Args[1]))
         return (FDErr = FDE_SYNTAX);
      bPat = Args[1];
   }

   return (fdProcDir(uni, pat, 0));
}

/* vim: set ts=3 sw=3 et: */

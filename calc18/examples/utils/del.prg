#include "fds.h"

/* DEL PAT[:UNI] */
main() {
   extrn NArgs, Args, FDErr;
   auto uni;

   uni = 0;
   if (2 != NArgs)
      return (FDErr = FDE_SYNTAX);

   if (fdParseFName(&uni, Args[1]))
      return (FDErr = FDE_SYNTAX);

   return (fdDel(uni, Args[1]));
}

/* vim: set ts=3 sw=3 et: */

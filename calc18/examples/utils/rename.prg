#include "fds.h"

/* RENAME FILE1[:UNI] FILE2[:UNI] */
main() {
   extrn NArgs, Args, FDErr;
   auto suni, duni;

   suni = duni = 0;
   if (3 != NArgs)
      return (FDErr = FDE_SYNTAX);

   if (fdParseFName(&suni, Args[1]))
      return (FDErr = FDE_SYNTAX);

   if (fdParseFName(&duni, Args[2]))
      return (FDErr = FDE_SYNTAX);

   return (fdRename(suni > duni? suni : duni, Args[1], Args[2]));
}

/* vim: set ts=3 sw=3 et: */

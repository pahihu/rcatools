#include "fds.h"

isempt(c) {
   return (c < 33);
}

issep(c) {
   return (!c || c == ' ' || c == ',' || c == ';' || c == '/');
}

ucase(c) {
   if ('a' <= c && c <= 'z')
      c = c - 'a' + 'A';
   return (c);
}

/* SHELL */
cmdShell() {
   extrn FDErr, FDErrmsg, NArgs, Args;
   auto c, q, buf 64;
   auto i, fnm MAXNAME_W;

   while (1) {
      FDErr = 0;
      puts("> ");
      if (!(q = gets(buf)))
         return (0);
      q = BYTES(q);
      i = strlen(q);
      if (i && '*n' == char(0,q+i-1)
         lchar(0,q+i-1,'*e');
      NArgs = 0;
      while ((NArgs < 8) && char(0,q) != '*e') {
         while (isempt(c = char(0,q))) q++;
         Args[NArgs++] = q;
         while (!issep(c = ucase(char(0,q))))
            lchar(0,q++,c);
         lchar(0,q++,'*e');
      }
      if (NArgs) {
         exec(Args[0]);
         if (FDErr)
            printf(%d: %s*n",FDErr,FDErrmsg[FDErr]);
      }
   }
   return 0;
}

NArgs 0;
Args[8];

/* vim: set ts=3 sw=3 et: */

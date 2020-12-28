static int isempt(int c) {
   return c < 33;
}

static int issep(int c) {
   return c == 0 || c == ' ' || c == ',' || c == ';' || c == '/';
}

// SHELL
int cmdShell(void)
{
   char c, *q, buf[128];
   struct {
      char *nm;
      int (*cmd)(void);
   } cmdtab[] = {
      {"COPY",   cmdCopy},
      {"DEL",    cmdDel},
      {"DIR",    cmdDir},
      {"EXAM",   cmdExam},
      {"FREE",   cmdFree},
      {"MEM",    cmdMem},
      {"MERGE",  cmdMerge},
      {"RENAME", cmdRename},
      {"SYSGEN", cmdSysgen},
      {"TYPE",   cmdType},
      {0, 0}
   };
   int i;

   while (1) {
      FDErr = 0;
      printf("> ");
      if (!(q = fgets(buf,127,stdin)))
         return 0;
      i = strlen(q);
      if (i && '\n' == q[i-1])
         q[i-1] = '\0';
      nargs = 0;
      while ((nargs < 8) && *q) {
         while (isempt(c = *q)) q++;
         args[nargs++] = q;
         while (!issep(c = ucase(*q)))
            *q++ = c;
         *q++ = '\0';
      }
      if (nargs) {
         if (dbg) {
            for (i = 0; i < nargs; i++)
               printf("arg%d=[%s] ",i,args[i]);
            printf("\n");
         }
         for (i = 0; cmdtab[i].nm; i++) {
            if (!strcmp(cmdtab[i].nm,args[0])) {
               (*cmdtab[i].cmd)();
               if (FDErr)
                  printf("%d: %s\n",FDErr, FDErrmsg[FDErr]);
               break;
            }
         }
         if (!cmdtab[i].nm)
            printf("CMD NOT FOUND\n");
      }
   }
   return 0;
}

NArgs 0;
Args[8];

/* vim: set ts=3 sw=3 et: */

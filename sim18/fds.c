// FDS - track based file-system

/*
 * History
 * =======
 *
 * 201128AP initial revision, untested
 * 201130AP cmd wrappers, untested
 * 201222AP shell, fdSelect() error handling
 * 201225AP SYSGEN, FREE, DEL, DIR, EXAM, MEM, RENAME works with simple fnames
 *          extended M, begin MERGE
 * 201226AP Open/Get/Put/Close API, MERGE, TYPE
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "1802.h"

#define PACKED  __attribute__((__packed__))

#define MAXNAME   10

typedef struct PACKED _DIRENT { // 16bytes
    Byte name[MAXNAME];
    Byte track;
    Byte ntracks;
    Word size;
    Byte rsvd[2];
} DIRENT;

#define DE_EMPTY        ((Byte)0x00)
#define DE_DELETED      ((Byte)0x80)


#define MAXDIRENT  8

typedef struct PACKED _DIR {
    DIRENT entries[MAXDIRENT];
} DIR;

#define FDE_DISKFULL    1
#define FDE_DIRFULL     2
#define FDE_EXISTS      3
#define FDE_NOTFOUND    4
#define FDE_SYNTAX      5
#define FDE_DISK        6
#define FDE_IOCB        7

static char *FDErrmsg[] = {
   "OK",
   "DISK FULL",
   "DIR. FULL",
   "FILE EXIST",
   "F.N. NOT FOUND",
   "SYNTAX ERROR",
   "DISK ERROR",
   "IOCB ERROR"
};

// track 0
// sector1 track map
// sector2 dir          max.128 entries
// ...
// sector17

#define MAP         1
#define DIRBEG      2
#define DIREND     18
#define MAXFILES  128

#define MAXUNI     3
#define MAXSEC    26
#define MAXTRACK  76

#define MAXBUF    128

int FDErr;
Byte FDUnit;;
Byte FDBuf[MAXBUF];
long FDPos[4];
typedef struct _IOCB {
   Word pos, size;
   Byte eof, rw;
   Byte u, t, s;
   Byte bufpos;
   Byte buf[MAXBUF];
   Byte nm[MAXNAME];
} IOCB;
IOCB FDesc[8];

static int dbg = 1;

static Word fdHex(char *s)
{
   int c;
   Word w;
   
   w = 0;
   while ((c = *s++) && ishex(c))
      w = 16 * w + digit(c);

   return w;
}

static Word fdDecimal(char *s)
{
   int c;
   Word w;

   w = 0;
   while ((c = *s++) && isdig(c))
      w = 10 * w + c - '0';

   return w;
}

static Byte fdHash(Byte *nm)
{
   Word h;
   int i;

   h = 0;
   for (i = 0; i < MAXNAME; i++)
      h = h + (*nm++) + (h & 0x100? 1 : 0);

   return h;
}

Byte fdSelect(Byte uni)
{
   if (uni > MAXUNI)
      uni = MAXUNI;
   FDUnit = uni;
   return fdd[FDUnit]? 0 : (FDErr = FDE_DISK);
}

static void doSeek(char *msg)
{
   if (fseek(fdd[FDUnit], FDPos[FDUnit], SEEK_SET) < 0) {
      printf("%s(): seek failed\n",msg);
      abort();
   }
}

void fdSeek(Byte t,Byte s)
{
   if (t > MAXTRACK)
      t = MAXTRACK;
   if (s < 1)
      s = 1;
   else if (s > MAXSEC)
      s = MAXSEC;
   FDPos[FDUnit] = MAXBUF * (MAXSEC * t + (s - 1));
   doSeek("fdSeek");
}

void *fdRead(Byte *buf)
{
   doSeek("fdRead");
   if (!buf) buf = FDBuf;
   fread(buf, sizeof(Byte), MAXBUF, fdd[FDUnit]);
   return buf;
}

void fdWrite(Byte *buf)
{
   doSeek("fdWrite");
   if (!buf) buf = FDBuf;
   fwrite(buf, sizeof(Byte), MAXBUF, fdd[FDUnit]);
   fflush(fdd[FDUnit]);
}

void fdFName2Str(char *str, Byte *nm)
{
   int i, c;

   i = 0;
   while (i < MAXNAME && ' ' != (c = *nm++)) {
      *str++ = c; i++;
   }
   *str++ = '\0';
}

void fdStr2FName(Byte *fnm, char *str)
{
   int i, c;

   for (i = 0; i < MAXNAME; i++) // fill w/ space
      fnm[i] = ' ';

   for (i = 0; i < MAXNAME; i++) { // cpy max 10chars
      c = *str++;
      if (!c || c == ':')
         break;
      *fnm++ = c;
   }
}

int fdMatch(char *nm, char *pat)
{
   int p;

   if (!pat)
      return 1;

   p = *pat;
   if (0 == p)
      return 1;

   if ('*' == p) // 0, 1, many
      return fdMatch(nm, pat + 1)            // matched 0 chars
             || fdMatch(nm + 1, pat + 1)     // matched 1 chars
             || fdMatch(nm + 1, pat);        // matches many chars

   return (p == *nm)                         // 1st match
          && fdMatch(nm + 1, pat + 1);       //    AND tail match
}

Byte fdParseFName(Byte *uni, char *nm)
{
   int n;
   char *p;

   *uni = 0;
   n = strlen(nm);
   if (n) {
      p = strchr(nm, ':');
      if (p) {
         // NB. allow :xxx
         if (0 == p[1]) // xxxx:
            return 1;
         *uni = fdDecimal(p + 1);
      }
   }
   return 0;
}

#define TRACKSIZE (MAXBUF * MAXSEC)

Byte fdTracks(Word size)
{
   int t;

   /* 1 track = 128 * 26 = 3328 bytes
    *
    */
   
   if (size < (TRACKSIZE + 1))
      return 1;
   t = (size + (TRACKSIZE - 1)) / TRACKSIZE;
   return t;
}

// allocate tracks
Byte fdTAlloc(Byte nt)
{
   Byte *map;
   int i, j;

   fdSeek(0, MAP);
   map = fdRead(0);
   for (i = 0; i < MAXTRACK + 1 - nt; i++) {
      if (*map == 0) {
         if (1 == nt) {
            map[0] = 1;
            fdWrite(0);
            return i;
         }
         for (j = 0; j < nt; j++)
            if (map[j])
               break;
         if (j == nt) {
            for (j = 0; j < nt; j++)
               map[j] = 1;
            fdWrite(0);
            return i;
         }
      }
      map++;
   }
   return 0;
}

// deallocate tracks
void fdTDealloc(Byte t, Byte nt)
{
   int i;
   Byte *map;

   // update map
   fdSeek(0, MAP); map = fdRead(0);
   map += t;
   for (i = 0; i < nt; i++)
      *map++ = 0;
   fdWrite(0);
}

int fdProcDir(Byte uni, char *pat, int del)
{
   DIR *dir;
   char buf[16];
   int i, s, ff, hastar;
   Byte t, nt;

   hastar = pat? strchr(pat, '*') != NULL : 0;

   if (fdSelect(uni))
      return FDErr;
   for (s = DIRBEG; s < DIREND; s++) {
      fdSeek(0, s);
      dir = fdRead(0);
      for (i = 0; i < MAXDIRENT; i++) {
         DIRENT *de = &dir->entries[i];
         if (0x7F & de->track) {
            fdFName2Str(buf, de->name);
            ff = 1;
            if (pat)
               ff = hastar? fdMatch(buf, pat) : 0 == strncmp(buf, pat, MAXNAME);
            if (ff) {
               if (del) {
                  // fddbuf contains the dirent, de is valid
                  t  = de->track;
                  nt = de->ntracks;

                  // delete, write back map sector
                  de->track = DE_DELETED; fdWrite(0);

                  // update map
                  fdTDealloc(t, nt);
               }
               else
                  printf("%-10s %02d %d (%d,%d)\n",buf,de->track,de->size,s,i);
            }
         } // not empty, not deleted
      } // dirents
   } // DIR
   return 0;
}

char *Z0(Byte *nm) {
   static char buf[16];
   int i;

   for (i = 0; i < MAXNAME; i++)
      buf[i] = *nm++;
   buf[i] = '\0';
   return buf;
}

char *Z1(Byte *nm) {
   static char buf[16];
   int i;

   for (i = 0; i < MAXNAME; i++)
      buf[i] = *nm++;
   buf[i] = '\0';
   return buf;
}

DIRENT *DE;     // DIRENT

Byte fdFind(Byte *nm)
{
   int h, i, j, s, t;
   int ds, dj; // 1st deleted sector/entry
   DIR *dir;

   /* max. 128 files
    * 8 entry/sec
    * DIR is 16 sectors (s2 - s17)
    * if    0 == track, entry not used
    * if 0x80 == track, entry deleted
    */

   h = fdHash(nm) & (MAXFILES - 1);
   s = DIRBEG + (h / MAXDIRENT);
   if (dbg) printf("fdFind(): h=%d s=%d\n",h,s);
   ds = 0;
   for (i = 0; i < DIREND - DIRBEG; i++) {
      fdSeek(0, s);
      if (dbg) printf("checking %d\n",s);
      dir = fdRead(0);
      for (j = 0; j < MAXDIRENT; j++) {
         DE = &dir->entries[j];
          t = DE->track;
         if (DE_EMPTY == t)                     // empty ?
            return 0;                           //    not found
         if (!ds && DE_DELETED == t) {          // remember 1st deleted entry
            ds = s; dj = j;
         }
         if (dbg) printf("%s ? %s\n",Z0(nm),Z1(DE->name));
         if (0 == strncmp((char*)nm, (char*)DE->name, MAXNAME)) {  // check name
            if (0x80 & t) // deleted            //    deleted entry?
               return 0;                        //       return not found
            return j + 1;                       //    normal entry, found
         }
      }
      s++;                                      // next sector
      if (DIREND == s)                          // wrap-around
         s = 2;
   }
   // no empty entry, only normal + deleted entries
   if (ds) {                                    // deleted entry?
      fdSeek(0,ds); dir = fdRead(0);            //    read back
      DE = &dir->entries[dj];                   //    reset DE
      return dj + 1;
   }
   // DIR full
   DE = 0;                                      // clear DE
   return 0;
}

Byte fdCreate(Byte *nm, Word size)
{
   int i;
   Byte t, nt;

   nt = fdTracks(size);
    t = fdTAlloc(nt);
   if (!t) {
      FDErr = FDE_DISKFULL; return 0;
   }

   i = fdFind(nm);
   if (i) {
      FDErr = FDE_EXISTS;
      goto LError;
   }

   if (!DE) {
      FDErr = FDE_DIRFULL;
      goto LError;
   }

   // write directory entry
   for (i = 0; i < MAXNAME; i++)
      DE->name[i] = nm[i];
   DE->track   = t;
   DE->ntracks = nt;
   DE->size    = size;
   fdWrite(0);

   return t;

LError:
   fdTDealloc(t, nt);
   return 0;
}

// del
int fdDel(Byte uni,char *pat)
{
   fdProcDir(uni, pat, 1);
   return 0;
}

// rename file1 file2
int fdRename(Byte uni, char *nm, char *newnm)
{
   int i;
   Byte fnm[10];

   fdStr2FName(fnm, nm);
   if (fdSelect(uni))
      return FDErr;
   i = fdFind(fnm);
   if (!i)
      return FDErr = FDE_NOTFOUND;

   // update direntry
   fdStr2FName(DE->name, newnm);
   fdWrite(0);

   return 0;
}

#define EOT DC3

Byte fdOpenR(Byte fd, char *nm)
{
   IOCB *cb;

   cb = &FDesc[fd];
   if (cb->size)
      return FDErr = FDE_IOCB;
   if (fdParseFName(&cb->u,nm))
      return FDErr = FDE_SYNTAX;
   fdStr2FName(cb->nm,nm);

   if (fdSelect(cb->u))
      return FDErr;
   if (!fdFind(cb->nm))
      return FDErr = FDE_NOTFOUND;

   cb->pos  = 0;
   cb->size = DE->size;
   cb->t    = DE->track;
   cb->s    = 1;
   cb->eof  = 0;
   cb->rw   = 0;
   cb->bufpos = MAXBUF;

   return 0;
}

Byte fdOpenW(Byte fd, char *nm, Word size)
{
   IOCB *cb;
   Byte t;

   cb = &FDesc[fd];
   if (cb->size)
      return FDErr = FDE_IOCB;
   if (fdParseFName(&cb->u,nm))
      return FDErr = FDE_SYNTAX;
   fdStr2FName(cb->nm,nm);

   if (fdSelect(cb->u))
      return FDErr;
   if (!(t = fdCreate(cb->nm,size)))
      return FDErr;

   cb->pos  = 0;
   cb->size = size;
   cb->t    = t;
   cb->s    = 1;
   cb->eof  = 0;
   cb->rw   = 1;
   cb->bufpos = 0;

   return 0;
}

void fdNextSector(IOCB *cb)
{
   if (cb->s == MAXSEC) {
      cb->t++; cb->s = 1;
   }
   else
      cb->s++;
}

Byte fdGet(Byte fd)
{
   IOCB *cb;

   cb = &FDesc[fd];
   if (!cb->size)
      return FDErr = FDE_IOCB;
   if (cb->pos == cb->size)
      return EOT;
   if (MAXBUF == cb->bufpos) {
      if (fdSelect(cb->u))
         return EOT;
      fdSeek(cb->t, cb->s);
      fdRead(cb->buf);
      cb->bufpos = 0;
      fdNextSector(cb);
   }
   cb->pos++;
   return cb->buf[cb->bufpos++];
}

Byte fdPut(Byte fd, Byte c)
{
   IOCB *cb;

   cb = &FDesc[fd];
   if (!cb->size)
      return FDErr = FDE_IOCB;
   if (cb->pos == cb->size)
      return FDErr = FDE_DISKFULL;
   if (MAXBUF == cb->bufpos) {
      if (fdSelect(cb->u))
         return FDErr;
      fdSeek(cb->t, cb->s);
      fdWrite(cb->buf);
      cb->bufpos = 0;
      fdNextSector(cb);
   }
   cb->pos++;
   cb->buf[cb->bufpos++] = c;
   return 0;
}

Byte fdClose(Byte fd)
{
   IOCB *cb;

   cb = &FDesc[fd];
   if (!cb->size)
      return FDErr = FDE_IOCB;
   if (cb->rw) {
      if (cb->bufpos) {
         if (fdSelect(cb->u))
            return FDErr;
         fdSeek(cb->t, cb->s);
         fdWrite(cb->buf);
      }
   }
   cb->size = 0;
   return 0;
}

Byte fdEof(Byte fd)
{
   IOCB *cb;

   cb = &FDesc[fd];
   if (!cb->size)
      return FDErr = FDE_IOCB;
   return cb->pos == cb->size? 1 : 0;
}


// DIR [PAT][:UNI]
int cmdDir(int argc,char*argv[])
{
   Byte uni;
   char *pat;

   uni = 0; pat = NULL;
   if (argc > 1) {
      if (fdParseFName(&uni, argv[1]))
         return FDErr = FDE_SYNTAX;
      pat = argv[1];
   }

   return fdProcDir(uni, pat, 0);
}

// COPY FILE1[:UNI] FILE2[:UNI]
int cmdCopy(int argc, char*argv[])
{
   Byte c;

   if (3 != argc)
      return FDErr = FDE_SYNTAX;

   if (fdOpenR(0,argv[1]))
      return FDErr;
   if (fdOpenW(1,argv[2],FDesc[0].size)) {
      fdClose(0);
      return FDErr;
   }

   c = fdGet(0);
   while (!fdEof(0)) {
      fdPut(1,c);
      c = fdGet(0);
   }
   fdClose(1); fdClose(0);

   return 0;
}

// DEL PAT[:UNI]
int cmdDel(int argc, char*argv[])
{
   Byte uni;

   uni = 0;
   if (2 != argc)
      return FDErr = FDE_SYNTAX;

   if (fdParseFName(&uni, argv[1]))
      return FDErr = FDE_SYNTAX;

   return fdDel(uni, argv[1]);
}

// MERGE FILE1 FILE2 FILE3 ... FILEN
int cmdMerge(int argc, char *argv[])
{
   int i;
   Word prevss, ss;
   Byte c;

   if (argc < 3)
      return FDErr = FDE_SYNTAX;

   /* calc size */
   ss = prevss = 0;
   for (i = 2; i < argc; i++) {
      if (*argv[i] == 0)
         ss++;
      else {
         if (fdOpenR(0,argv[i]))
            return FDErr;
         ss += FDesc[0].size;
         fdClose(0);
      }
      if (ss < prevss)
         return FDE_DISKFULL;
      prevss = ss;
   }

   if (fdOpenW(1,argv[1],ss))
      return FDErr;

   for (i = 2; i < argc; i++) {
      if (*argv[i] == 0)
         fdPut(1,DC3);
      else {
         if (fdOpenR(0,argv[i]))
            return FDErr;
         c = fdGet(0);
         while (!fdEof(0))  {
            fdPut(1,c);
            c = fdGet(0);
         }
         fdClose(0);
      }
   }
   fdClose(1);

   return 0;
}

// RENAME FILE1[:UNI] FILE2[:UNI]
int cmdRename(int argc, char*argv[])
{
   Byte suni, duni;

   suni = duni = 0;
   if (3 != argc)
      return FDErr = FDE_SYNTAX;

   if (fdParseFName(&suni, argv[1]))
      return FDErr = FDE_SYNTAX;

   if (fdParseFName(&duni, argv[2]))
      return FDErr = FDE_SYNTAX;

   return fdRename(suni > duni? suni : duni, argv[1], argv[2]);
}

// FREE [UNIT]
int cmdFree(int argc, char*argv[])
{
   Byte uni;
   int s, i;
   Byte nt, nf, *map;
   DIR *dir;

   uni = 0;
   if (1 != argc)
      uni = fdDecimal(argv[1]);

   if (fdSelect(uni))
      return FDErr;

   // scan dir entries
   nf = 0;
   for (s = 2; s < 18; s++) {
      fdSeek(0, s);
      dir = fdRead(0);
      for (i = 0; i < 8; i++) {
         DIRENT *de = &dir->entries[i];
         if (DE_EMPTY == (0x7F & de->track))
            nf++;
      }
   }

   // read map, count free tracks
   nt = 0;
   fdSeek(0, 1);
   map = fdRead(0);
   for (i = 0; i < 77; i++)
      if (0 == *map++)
         nt++;

   printf("%02d TRACKS %03d FILES\n",nt,nf);

   return 0;
}

// EXAM [UNI] TRACK SEC
int cmdExam(int argc, char*argv[])
{
   Byte uni, t, s;
   int i, j, c;
   Byte *buf;
   Word *w;

   uni = 0;
   if (3 < argc)
      return FDErr = FDE_SYNTAX;

   t = fdDecimal(argv[1]); s = fdDecimal(argv[2]);
   if (argc > 3) {
      uni = t; t = s; s = fdDecimal(argv[3]);
   }

   if (fdSelect(uni))
      return FDErr;

   fdSeek(t, s);
   buf = fdRead(0);
   for (i = 0; i < 8; i++) {
      w = (Word*) buf;
      for (j = 0; j < 8; j++)
         printf("%04X ",*w++);
      printf(" ");
      for (j = 0; j < 16; j++) {
         c = buf[j];
         if (c < 32)
            c = '.';
         printf("%c",127 & c);
      }
      printf("\n");
      buf += 16;
   }

   return 0;
}

// MEM FILE <BEGIN> <END>
// MEM FILE <END>
// MEM FILE <BEGIN> <END> <START>
int cmdMem(int argc, char*argv[])
{
   Byte uni;
   Word beg, end, start;
   Word s;
   Byte t;
   Byte fnm[MAXNAME];

   uni = 0;
   if (argc < 3 || argc > 5)
      return FDErr = FDE_SYNTAX;

   if (fdParseFName(&uni, argv[1]))
      return FDErr = FDE_SYNTAX;

   start = beg = 0;
   if (3 == argc)
      end = fdHex(argv[2]);
   else {
      start = beg = fdHex(argv[2]); end = fdHex(argv[3]);
      if (5 == argc)
         start = fdHex(argv[4]);
   }

   s = end - beg + 1;
   s = 6 + (2 * s) + ((s + 19) / 20) * 2 + 1 + 6 + 1;

   if (fdSelect(uni))
      return FDErr;

   fdStr2FName(fnm,argv[1]);
   t = fdCreate(fnm, s);
   if (!t)
      return FDErr;

   // save to disk
   fdSeek(t, 1);
   ut20xy(fdd[FDUnit],beg,end,CMD_X,start);
   fflush(fdd[FDUnit]);

   return 0;
}

// SYSGEN [UNI]
int cmdSysgen(int argc, char*argv[])
{
   Byte uni;
   int i, s;
   Byte *map;
   char dummy[10];
   DIR *dir;


   uni = 0;
   if (argc > 1)
      uni = fdDecimal(argv[1]);

   if (fdSelect(uni))
      return FDErr;

   // initialize track map, t0 allocated
   fdSeek(0, MAP);
   map = fdRead(0);
   for (i = 0; i < MAXBUF; i++)
      map[i] = 0;
   map[0] = 1;
   fdWrite(0);

   // write directory file
   dummy[0] = '\0';
   for (s = DIRBEG; s < DIREND; s++) {
      fdSeek(0, s);
      dir = fdRead(0);
      for (i = 0; i < MAXDIRENT; i++) {
         DIRENT *de = &dir->entries[i];
         fdStr2FName(de->name, dummy);
         de->track   = DE_EMPTY;
         de->ntracks = 0;
         de->size    = 0;
      }
      fdWrite(0);
   }

   return 0;
}

int cmdType(int argc,char*argv[])
{
   Byte c;

   if (2 != argc)
      return FDErr = FDE_SYNTAX;
   if (fdOpenR(0,argv[1]))
      return FDErr;

   c = fdGet(0);
   while (!fdEof(0)) {
      printf("%c",c);
      c = fdGet(0);
   }
   fdClose(0);

   return 0;
}

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
      int (*cmd)(int,char**);
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
   char *args[8];
   int nargs, i;

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
               (*cmdtab[i].cmd)(nargs,args);
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

// vim:ts=3 sw=3 et:

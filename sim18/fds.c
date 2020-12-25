// FDS - track based file-system

/*
 * FDS commands:
 *    ?dir
 *    ?copy source dest
 *    ?del file
 *    ?rename file1 file2
 *    ?free
 *    ?exam ttss
 *    ?mem file <begin> <end>
 *    ?sysgen
 *    -merge dst src1 .. srcN
 *
 * History
 * =======
 *
 * 201128AP initial revision, untested
 * 201130AP cmd wrappers, untested
 * 201222AP shell, fdSelect() error handling
 * 201225AP SYSGEN, FREE, DEL, DIR, EXAM, MEM, RENAME works with simple fnames
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

// track 0
// sector1 track map
// sector2 dir          max.128 entries
// ...
// sector17

#define MAP         1
#define DIRBEG      2
#define DIREND     17
#define MAXFILES  128

#define MAXUNI     3
#define MAXSEC    26
#define MAXTRACK  76

#define MAXBUF    128

int FDErr;
Byte FDUnit;;
Byte fddbuf[4][MAXBUF];
long fddpos[4];

static int dbg = 0;

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
   Byte c;

   while ((c = *nm++))
      h = h + c + (h & 0x100? 1 : 0);

   return h;
}

int fdSelect(Byte uni)
{
   if (uni > MAXUNI)
      uni = MAXUNI;
   FDUnit = uni;
   return fdd[FDUnit]? 0 : FDE_DISK;
}

static void doSeek(char *msg)
{
   if (fseek(fdd[FDUnit], fddpos[FDUnit], SEEK_SET) < 0) {
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
   fddpos[FDUnit] = MAXBUF * (MAXSEC * t + (s - 1));
   doSeek("fdSeek");
}

void *fdRead(void)
{
   doSeek("fdRead");
   fread(fddbuf[FDUnit], sizeof(Byte), MAXBUF, fdd[FDUnit]);
   return fddbuf[FDUnit];
}

void fdWrite(void)
{
   doSeek("fdWrite");
   fwrite(fddbuf[FDUnit], sizeof(Byte), MAXBUF, fdd[FDUnit]);
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
      if (!c)
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

int fdParseFName(Byte *uni, char *nm)
{
   int n;
   char *p;

   *uni = 0;
   n = strlen(nm);
   if (n) {
      p = strchr(nm, ':');
      if (!p)
         return 1;
      if (p == nm)   // :xxx
         return 0;
      if (0 == p[1]) // xxxx:
         return 0;
      *uni = fdDecimal(p + 1);
   }
   return 1;
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
   map = fdRead();
   for (i = 0; i < MAXTRACK + 1 - nt; i++) {
      if (*map == 0) {
         if (1 == nt) {
            map[0] = 1;
            fdWrite();
            return i;
         }
         for (j = 0; j < nt; j++)
            if (map[j])
               break;
         if (j == nt) {
            for (j = 0; j < nt; j++)
               map[j] = 1;
            fdWrite();
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
   fdSeek(0, MAP); map = fdRead();
   map += t;
   for (i = 0; i < nt; i++)
      *map++ = 0;
   fdWrite();
}

int fdProcDir(Byte uni, char *pat, int del)
{
   DIR *dir;
   char buf[16];
   int i, s, ff, hastar;
   Byte t, nt;

   hastar = pat? strchr(pat, '*') != NULL : 0;

   if ((i = fdSelect(uni)))
      return FDErr = i;
   for (s = DIRBEG; s < DIREND; s++) {
      fdSeek(0, s);
      dir = fdRead();
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
                  de->track = DE_DELETED; fdWrite();

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
      dir = fdRead();
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
      fdSeek(0,ds); dir = fdRead();             //    read back
      DE = &dir->entries[dj];                   //    reset DE
      return dj + 1;
   }
   // DIR full
   DE = 0;                                      // clear DE
   return 0;
}

Byte fdCreate(char *nm, Word size)
{
   int i;
   Byte t, nt;
   Byte fnm[MAXNAME];

   nt = fdTracks(size);
    t = fdTAlloc(nt);
   if (!t) {
      FDErr = FDE_DISKFULL; return 0;
   }

   fdStr2FName(fnm,nm);
   i = fdFind(fnm);
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
      DE->name[i] = fnm[i];
   DE->track   = t;
   DE->ntracks = nt;
   DE->size    = size;
   fdWrite();

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
   if ((i = fdSelect(uni)))
      return FDErr = i;
   i = fdFind(fnm);
   if (!i)
      return FDErr = FDE_NOTFOUND;

   // update direntry
   fdStr2FName(DE->name, newnm);
   fdWrite();

   return 0;
}

// copy file1 file2
int fdCopy(Byte suni, char *src, Byte duni, char *dst)
{
   Byte fsrc[10];
   int i, j;
   Byte ts, nts;
   Word ss;
   Byte td;

   fdStr2FName(fsrc, src);

   if ((i = fdSelect(duni)))
      return FDErr = i;
   if ((i = fdSelect(suni)))
      return FDErr = i;

   i = fdFind(fsrc);
   if (!i)
      return FDErr = FDE_NOTFOUND;

   ts = DE->track; nts = DE->ntracks; ss = DE->size;

   fdSelect(duni);
   td = fdCreate(dst, ss);
   if (!td)
      return FDErr;

   // copy nts tracks from ts to td
   for (i = 0; i < nts; i++) {
      for (j = 0; j < 26; j++) {
         fdSelect(suni);
         fdSeek(ts, j+1); fdRead();
         fdSelect(duni);
         fdSeek(td, j+1); fdWrite();
      }
   }

   return 0;
}

// free
int fdFree(Byte uni)
{
   int s, i;
   Byte nt, nf, *map;
   DIR *dir;

   if ((i = fdSelect(uni)))
      return FDErr = i;

   // scan dir entries
   nf = 0;
   for (s = 2; s < 18; s++) {
      fdSeek(0, s);
      dir = fdRead();
      for (i = 0; i < 8; i++) {
         DIRENT *de = &dir->entries[i];
         if (DE_EMPTY == (0x7F & de->track))
            nf++;
      }
   }

   // read map, count free tracks
   nt = 0;
   fdSeek(0, 1);
   map = fdRead();
   for (i = 0; i < 77; i++)
      if (0 == *map++)
         nt++;

   printf("%02d TRACKS %03d FILES\n",nt,nf);

   return 0;
}

// exam
int fdExam(Byte uni, Byte t, Byte s)
{
   int i, j, c;
   Byte *buf;
   Word *w;

   if ((i = fdSelect(uni)))
      return FDErr = i;

   fdSeek(t, s);
   buf = fdRead();
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

// mem file begin end
int fdMem(Byte uni, char *nm, Word b, Word e)
{
   Word s;
   Byte t;
   int i;

   s = e - b;
   s = 6 + (2 * s) + (s / 20) * 2 + 1;

   if ((i = fdSelect(uni)))
      return FDErr = i;

   t = fdCreate(nm, s);
   if (!t)
      return FDErr;

   // save to disk
   fdSeek(t, 1);
   ut20xy(fdd[FDUnit],b,e,CMD_X);
   fflush(fdd[FDUnit]);

   return 0;
}

int fdSysgen(Byte uni)
{
   int i, s;
   Byte *map;
   char dummy[10];
   DIR *dir;

   if ((i = fdSelect(uni)))
      return FDErr = i;

   // initialize track map, t0 allocated
   fdSeek(0, MAP);
   map = fdRead();
   for (i = 0; i < MAXBUF; i++)
      map[i] = 0;
   map[0] = 1;
   fdWrite();

   // write directory file
   dummy[0] = '\0';
   for (s = DIRBEG; s < DIREND; s++) {
      fdSeek(0, s);
      dir = fdRead();
      for (i = 0; i < MAXDIRENT; i++) {
         DIRENT *de = &dir->entries[i];
         fdStr2FName(de->name, dummy);
         de->track   = DE_EMPTY;
         de->ntracks = 0;
         de->size    = 0;
      }
      fdWrite();
   }
   return 0;
}

// DIR [PAT][:UNI]
int cmdDir(int argc,char*argv[])
{
   Byte uni;
   char *pat;

   uni = 0; pat = NULL;
   if (argc > 1) {
      if (!fdParseFName(&uni, argv[1]))
         return FDErr = FDE_SYNTAX;
      pat = argv[1];
   }

   return fdProcDir(uni, pat, 0);
}

// COPY FILE1[:UNI] FILE2[:UNI]
int cmdCopy(int argc, char*argv[])
{
   Byte suni, duni;

   suni = 0; duni = 0;
   if (3 != argc)
      return FDErr = FDE_SYNTAX;
   if (!fdParseFName(&suni, argv[1]))
      return FDErr = FDE_SYNTAX;
   if (!fdParseFName(&duni, argv[2]))
      return FDErr = FDE_SYNTAX;
   
   return fdCopy(suni, argv[1], duni, argv[2]);
}

// DEL PAT[:UNI]
int cmdDel(int argc, char*argv[])
{
   Byte uni;

   uni = 0;
   if (2 != argc)
      return FDErr = FDE_SYNTAX;

   if (!fdParseFName(&uni, argv[1]))
      return FDErr = FDE_SYNTAX;

   return fdDel(uni, argv[1]);
}

// RENAME FILE1[:UNI] FILE2[:UNI]
int cmdRename(int argc, char*argv[])
{
   Byte suni, duni;

   suni = duni = 0;
   if (3 != argc)
      return FDErr = FDE_SYNTAX;

   if (!fdParseFName(&suni, argv[1]))
      return FDErr = FDE_SYNTAX;

   if (!fdParseFName(&duni, argv[2]))
      return FDErr = FDE_SYNTAX;

   return fdRename(suni > duni? suni : duni, argv[1], argv[2]);
}

// FREE [UNIT]
int cmdFree(int argc, char*argv[])
{
   Byte uni;

   uni = 0;
   if (1 != argc)
      uni = fdDecimal(argv[1]);

   return fdFree(uni);
}

// EXAM [UNI] TRACK SEC
int cmdExam(int argc, char*argv[])
{
   Byte uni, t, s;

   uni = 0;
   if (3 < argc)
      return FDErr = FDE_SYNTAX;

   t = fdDecimal(argv[1]); s = fdDecimal(argv[2]);
   if (argc > 3) {
      uni = t; t = s; s = fdDecimal(argv[3]);
   }

   return fdExam(uni, t, s);
}

// MEM FILE <BEGIN> <END>
int cmdMem(int argc, char*argv[])
{
   Byte uni;
   Word beg, end;

   uni = 0;
   if (4 != argc)
      return FDErr = FDE_SYNTAX;

   if (!fdParseFName(&uni, argv[1]))
      return FDErr = FDE_SYNTAX;

   beg = fdHex(argv[2]); end = fdHex(argv[3]);

   return fdMem(uni, argv[1], beg, end);
}

// SYSGEN [UNI]
int cmdSysgen(int argc, char*argv[])
{
   Byte uni;

   uni = 0;
   if (argc > 1)
      uni = fdDecimal(argv[1]);

   return fdSysgen(uni);
}

static int isempt(int c) {
   return (c < 33);
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
      {"RENAME", cmdRename},
      {"SYSGEN", cmdSysgen},
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
         while (!isempt(c = ucase(*q)))
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
                  printf("ERROR %d\n",FDErr);
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

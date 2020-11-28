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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACKED  __attribute__((__packed__))

typedef unsigned short Word;
typedef unsigned char Byte;

typedef struct PACKED _DIRENT { // 16bytes
    Byte name[10];
    Byte track;
    Byte ntracks;
    Word size;
    Byte rsvd[2];
} DIRENT;

#define DE_EMPTY        ((Byte)0x00)
#define DE_DELETED      ((Byte)0x80)

typedef struct PACKED _DIR {
    DIRENT entries[8];  // 8 entry/sec
} DIR;

#define FDE_DISKFULL    1
#define FDE_DIRFULL     2
#define FDE_EXISTS      3
#define FDE_NOTFOUND    4

extern void ut40x(FILE*,Word,Word);

// track 0
// sector1 track map
// sector2 dir          max.128 entries
// ...
// sector17

int FDErr;
Byte FDUnit;;
FILE *fdd[4];
Byte fddbuf[4][128];

static Byte fdHash(Byte *nm)
{
   Word h;
   Byte c;

   while ((c = *nm++))
      h = h + c + (h & 0x100? 1 : 0);

   return h;
}

void fdSelect(Byte uni)
{
   if (uni > 3)
      uni = 3;
   FDUnit = uni;
}

void fdSeek(Byte t,Byte s)
{
   if (t > 76)
      t = 76;
   if (s < 1)
      s = 1;
   else if (s > 26)
      s = 26;
   fseek(fdd[FDUnit], 128 * (26 * t + (s - 1)), SEEK_SET);
}

void *fdRead(void)
{
   fread(fddbuf[FDUnit], sizeof(Byte), 128, fdd[FDUnit]);
   fseek(fdd[FDUnit], -128, SEEK_CUR);
   return fddbuf[FDUnit];
}

void fdWrite(void)
{
   fwrite(fddbuf[FDUnit], sizeof(Byte), 128, fdd[FDUnit]);
   fflush(fdd[FDUnit]);
   fseek(fdd[FDUnit], -128, SEEK_CUR);
}

void fdFName2Str(char *str, Byte *nm)
{
   int i, c;

   i = 0;
   while (i < 10 && ' ' != (c = *nm++)) {
      *str++ = c; i++;
   }
   *str++ = '\0';
}

void fdStr2FName(Byte *fnm, char *str)
{
   int i, c;

   for (i = 0; i < 10; i++) // fill w/ space
      fnm[i] = ' ';

   for (i = 0; i < 10; i++) { // cpy max 10chars
      c = *str++;
      if (!c)
         break;
      *fnm++ = c;
   }
}

void fdDir(Byte uni)
{
   DIR *dir;
   char buf[16];
   int i, s;

   fdSelect(uni);
   for (s = 2; s < 18; s++) {
      fdSeek(0, s);
      dir = fdRead();
      for (i = 0; i < 8; i++) {
         DIRENT *de = &dir->entries[i];
         if (0x7F & de->track) {
            fdFName2Str(buf, de->name);
            printf("%s %02d %d\n",buf,de->track,de->size);
         }
      }
   }
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

   h = fdHash(nm) & 0x7F;
   s = 2 + (h / 8);
   ds = 0;
   for (i = 0; i < 16; i++) {
      fdSeek(0, s);
      dir = fdRead();
      for (j = 0; j < 8; j++) {
         DE = &dir->entries[j];
          t = DE->track;
         if (DE_EMPTY == t)                     // empty ?
            return 0;                           //    not found
         if (!ds && DE_DELETED == t) {          // remember 1st deleted entry
            ds = s; dj = j;
         }
         if (0 == strncmp((char*)nm, (char*)DE->name, 10)) {  // check name
            if (0x80 & t) // deleted            //    deleted entry?
               return 0;                        //       return not found
            return j + 1;                       //    normal entry, found
         }
      }
      s++;                                      // next sector
      if (18 == s)                              // wrap-around
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

Byte fdTracks(Word size)
{
   int t;

   /* 1 track = 128 * 26 = 3328 bytes
    *
    */
   
   if (size < 3329)
      return 1;
   t = (size + 3327) / 3328;
   return t;
}

// allocate tracks
Byte fdTAlloc(Byte nt)
{
   Byte *map;
   int i, j;

   fdSeek(0, 1);
   map = fdRead();
   for (i = 0; i < 77 - nt; i++) {
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
   fdSeek(0, 1); map = fdRead();
   map += t;
   for (i = 0; i < nt; i++)
      *map++ = 0;
   fdWrite();
}

Byte fdCreate(char *nm, Word size)
{
   int i;
   Byte t, nt;
   Byte fnm[10];

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
   for (i = 0; i < 10; i++)
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
int fdDel(Byte uni,char *nm)
{
   int i, t, nt;
   Byte fnm[10];

   fdStr2FName(fnm, nm);
   fdSelect(uni);
   i = fdFind(fnm);
   if (!i)
      return FDErr = FDE_NOTFOUND;

   // fddbuf contains the dirent, de is valid
    t = DE->track;
   nt = DE->ntracks;

   // delete, write back map sector
   DE->track = DE_DELETED; fdWrite(); DE = 0;

   // update map
   fdTDealloc(t, nt);

   return 0;
}

// rename file1 file2
int fdRename(Byte uni, char *nm, char *newnm)
{
   int i;
   Byte fnm[10];

   fdStr2FName(fnm, nm);
   fdSelect(uni);
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
   fdSelect(suni);
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

   fdSelect(uni);

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

   fdSelect(uni);
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
         printf("%c",c);
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

   s = e - b;
   s = 6 + (2 * s) + (s / 20) * 2 + 1;

   fdSelect(uni);
   t = fdCreate(nm, s);
   if (!t)
      return FDErr;

   // save to disk
   fdSeek(t, 1);
   ut40x(fdd[FDUnit],b,e);
   fflush(fdd[FDUnit]);

   return 0;
}

void fdSysgen(Byte uni)
{
   int i, s;
   Byte *map;
   char dummy[10];
   DIR *dir;

   fdSelect(uni);

   // initialize track map, t0 allocated
   fdSeek(0, 1);
   map = fdRead();
   for (i = 0; i < 128; i++)
      map[i] = 0;
   map[0] = 1;
   fdWrite();

   // write directory file
   dummy[0] = '\0';
   for (s = 2; s < 18; s++) {
      fdSeek(0, s);
      dir = fdRead();
      for (i = 0; i < 8; i++) {
         DIRENT *de = &dir->entries[i];
         fdStr2FName(de->name, dummy);
         de->track   = DE_EMPTY;
         de->ntracks = 0;
         de->size    = 0;
      }
      fdWrite();
   }
}


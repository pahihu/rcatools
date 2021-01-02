/* FDS - track based file-system */

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

#include "fds.h"

/* 
 * track 0
 * sector1 track map
 * sector2 dir          max.128 entries
 * ...
 * sector17
 */

#define MAP         1
#define DIRBEG      2
#define DIREND     18
#define MAXFILES  128

#define MAXUNI      3
#define MAXSEC     26
#define MAXTRACK   76

fdHash(bName) {
   auto h, i;

   i = h = 0;
   while (i++ < MAXNAME)
      h = h + char(0,bName++) + (h & 0400? 1 : 0);

   return (h);
}

fdRead(dcb,buf) {
   extrn FDCB, FDBuf;

   FDCB[0] = dcb[0];
   FDCB[1] = dcb[1];
   if (!buf) buf = FDBuf;
   dskrd(dcb,buf);
   return (buf);
}

fdWrite(dcb,buf) {
   extrn FDCB, FDBuf;

   FDCB[0] = dcb[0];
   FDCB[1] = dcb[1];
   if (!buf) buf = FDBuf;
   dskwr(dcb,buf);
}

fdFName2Str(str,nm)
{
   auto i, c;

   i = 0;
   while (i < MAXNAME && ' ' != (c = char(nm,i)))
      lchar(str,i++,c);
   lchar(str,i,'*e');
}

fdStr2FName(fnm,bStr)
{
   auto i, c;

   i = 0; while (i < (MAXNAME/2)) fnm[i++] = '  '; /* fill w/ space */

   i = 0;
   while (i < MAXNAME) { /* cpy max. 10chars */
      c = char(0,bStr++);
      if (!c || c == ':')
         return;
      lchar(fnm,i++,c);
   }
}

fdMatch(bName,bPat) /* char ptrs */
{
   auto c;

   if (!bPat)
      return (1);

   c = char(0,bPat);;
   if ('*e' == c)
      return (1);

   if ('**' == c) /* 0, 1, many */
      return (fdMatch(bName,bPat+1)         /* matched 0 chars    */
              || fdMatch(bName+1,bPat+1)    /* matched 1 chars    */
              || fdMatch(bName+1,bPat));    /* matches many chars */

   return ((c == char(0,bName))             /* 1st match */
           && fdMatch(bName+1,bPat+1));     /*    AND tail match */
}

fdParseFName(pUni,bName)
{
   auto n, p;

   *(pUni) = 0;
   n = strlen(bName);
   if (n) {
      p = strchr(bName, ':');
      if (p) {
         /* NB. allow :xxx */
         if ('*e' == char(0,p++)) /* xxxx: */
            return (1);
         *(pUni) = atoi(p,10);
      }
   }
   return (0);
}

#define TRACKSIZE (MAXBUF * MAXSEC)

fdTracks(size) {
   auto t;

   /* 1 track = 128 * 26 = 3328 bytes
    *
    */
   
   if (size < (TRACKSIZE + 1))
      return (1);
   return ((size + (TRACKSIZE - 1)) / TRACKSIZE);
}

/* allocate tracks */
fdTAlloc(nt) {
   extrn FDU, FDCB;
   auto map, i, j;

   map = fdRead(mkdcb(FDU,0,MAP),0);
   map = BYTES(map);
   i = 0;
   while (i < MAXTRACK + 1 - nt) {
      if (char(0,map) == 0) {
         if (1 == nt) {
            lchar(0,map,1);
            fdWrite(FDCB,0);
            return (i);
         }
         j = 0;
         while (j < nt && !char(0,map+j)) j++;
         if (j == nt) {
            j = 0;
            while (j < nt)
               lchar(0,map+j,1);
            fdWrite(FDCB,0);
            return (i);
         }
      }
      map++;
   }
   return (0);
}

/* deallocate tracks */
fdTDealloc(t,nt) {
   extrn FDU, FDCB;
   auto i, map;

   /* update map */
   map = fdRead(mkdcb(FDU,0,MAP),0);
   map = BYTES(map) + t;
   i = 0;
   while (i < nt)
      lchar(0,map++,0);
   fdWrite(FDCB,0);
}

fdSelect(uni) {
   extrn FDU;

   if (uni > MAXUNI)
      uni = MAXUNI;

   FDU = uni;
   return (0);
}

fdProcDir(uni,bPat,del) {
   extrn FDCB, FDU;
   auto dir, de, det, buf 8, i, s, ff, hastar, t, nt;

   hastar = bPat? strchr(bPat, '**') != 0 : 0;

   fdSelect(uni);
   s = DIRBEG;
   while (s < DIREND) {
      dir = fdRead(mkdcb(FDU,0,s),0);
      i = 0; de = dir;
      while (i < MAXDIRENT) {
         if (0177 & (det = de[DIRENT_tracks])) {
            fdFName2Str(buf, de);
            ff = 1;
            if (bPat)
               ff = hastar? fdMatch(BYTES(buf),bPat)
                          : 0 == strncmp(BYTES(buf),bPat,MAXNAME);
            if (ff) {
               if (del) {
                  /* fddbuf contains the dirent, de is valid */
                  t  = det; nt = t >> 8; t  =& 0377;

                  /* delete, write back map sector */
                  de[DIRENT_tracks]= DE_DELETED; fdWrite(FDCB,0);

                  /* update map */
                  fdTDealloc(t, nt);
               }
               else {
                  puts(buf);
                  printn(det & 0377,10);
                  printn(de[DIRENT_size]);
                  putchar('*n');
               }
            }
         } /* not empty, not deleted */
         i++; de =+ sizeof_DIRENT;
      } /* dirents */
      s++;
   } /* DIR */
   return (0);
}

fdFind(bName) {
   extrn FDU, DE;
   auto h, i, j, s, t;
   auto ds, dj; /* 1st deleted sector/entry */
   auto dir, det;

   /* max. 128 files
    * 8 entry/sec
    * DIR is 16 sectors (s2 - s17)
    * if    0 == track, entry not used
    * if 0x80 == track, entry deleted
    */

   h = fdHash(bName) & (MAXFILES - 1);
   s = DIRBEG + (h / MAXDIRENT);
   ds = 0;
   i = 0;
   while (i < DIREND - DIRBEG) {
      dir = fdRead(mkdcb(FDU,0,s),0);
      j = 0; DE = dir;
      while (j < MAXDIRENT) {
          t = (det = DE[DIRENT_tracks]) & 0377;
         if (DE_EMPTY == t)                     /* empty ?                    */
            return (0);                         /*    not found               */
         if (!ds && DE_DELETED == t) {          /* remember 1st deleted entry */
            ds = s; dj = j;
         }
         if (0 == strncmp(bName, BYTES(DE), MAXNAME)) {  /* check name */
            if (0200 & t)                       /*    deleted entry?       */
               return (0);                      /*       return not found  */
            return (j + 1);                     /*    normal entry, found  */
         }
         j++; DE =+ sizeof_DIRENT;
      }
      s++;                                      /* next sector */
      if (DIREND == s)                          /* wrap-around */
         s = 2;
      i++;
   }
   /* no empty entry, only normal + deleted entries */
   if (ds) {                                    /* deleted entry? */
      dir = fdRead(mkdcb(FDU,0,ds),0);          /*    read back   */
      DE = dir + dj * sizeof_DIRENT;            /*    reset DE    */
      return (0);
   }
   /* DIR full */
   DE = 0;                                      /* clear DE */
   return (0);
}

fdCreate(bName,size) {
   extrn FDErr, FDCB, DE;
   auto i;
   auto t, nt;

   nt = fdTracks(size);
    t = fdTAlloc(nt);
   if (!t) {
      FDErr = FDE_DISKFULL; return (0);
   }

   if ((i = fdFind(bName))) {
      FDErr = FDE_EXISTS;
      goto LError;
   }

   if (!DE) {
      FDErr = FDE_DIRFULL;
      goto LError;
   }

   /* write directory entry */
   i = 0;
   while (i < MAXNAME)
      lchar(DE,i++,char(0,bName++));
   DE[DIRENT_tracks] = (nt << 8) + t;
   DE[DIRENT_size]   = size;
   DE[DIRENT_type]   = 0; /* ASCII file */
   fdWrite(FDCB,0);

   return (t);

LError:
   fdTDealloc(t, nt);
   return (0);
}

/* del */
fdDel(uni,bPat) {
   fdProcDir(uni, bPat, 1);
   return (0);
}

/* rename file1 file2 */
fdRename(uni,bName,bNewname) {
   extrn FDErr, DE, FDCB;
   auto i, fnm MAXNAME_W; 

   fdStr2FName(fnm, bName);
   fdSelect(uni);
   if (!(i = fdFind(BYTES(fnm))))
      return (FDErr = FDE_NOTFOUND);

   /* update direntry */
   fdStr2FName(DE, bNewname);
   fdWrite(FDCB,0);

   return (0);
}

FDErr 0;          /* error code */
FDErrmsg[0] "OK", "DISK FULL", "DIR. FULL", "FILE EXIST",
            "F.N. NOT FOUND", "SYNTAX ERROR", "DISK ERROR", "IOCB ERROR",
            "LOADER ERROR";
FDU;              /* current unit */
FDCB 2;           /* last DCB */
FDBuf[MAXBUF_W];  /* I/O buffer */
DE 0;             /* DIRENT */

NArgs 0;          /* cmd.line args */
Args[8];


/* vim: set ts=3 sw=3 et: */

#include "fds.h"

/*
 * History
 * =======
 * 201229AP loadbin,loadhex,exec
 * 201228AP openr,openw,close,fgetc,fputc,flush
 *
 */

open(fd,bName,size,rw) {
   extrn FDErr, DCB, DE, FDesc, IOBufs;
   auto cb, uni, p;
   auto track;

   cb = FDesc + (fd << 4); /* sizeof_IOCB */
   if (cb[0])
      return (FDErr = FDE_IOCB);
   if (fdParseFName(&uni,bName))
      return (FDErr = FDE_SYNTAX);
   fdStr2FName(cb,bName);

   fdSelect(uni);
   if (rw) {
      if (!(track = fdCreate(BYTES(cb),size)))
         return (FDErr);
   }
   else {
      if (!fdFind(BYTES(cb)))
         return (FDErr = FDE_NOTFOUND);
      size  = DE[DIRENT_size];
      track = DE[DIRENT_tracks] & 0377;
   }

   p = cb + IOCB_pos;
   *(p++) = 0;    /* pos  */
   *(p++) = size; /* size */
   *(p++) = 0;    /* eof  */
   *(p++) = rw;   /* rw   */
   *(p++) = rw? 0 : MAXBUF; /* bufpos */
   mkdcb(uni,track,1);
   *(p++) = DCB[0]; /* dcb */
   *(p++) = DCB[1];
   *(p)   = IOBufs + (fd << 6); /* buf, MAXBUF_W */

   return (0);
}

openr(fd,bName) {
   return (open(fd,bName,0,0));
}

openw(fd,bName,size) {
   return (open(fd,bName,size,1));
}

fgetc(fd) {
   extrn FDErr, FDesc;
   auto cb, buf;

   cb  = FDesc + (fd << 4); /* sizeof_IOCB */
   if (!cb[0]) {
      FDErr = FDE_IOCB;
      return (EOT);
   }
   else if (cb[IOCB_pos] == cb[IOCB_size])
      return (EOT);
   buf = cb[IOCB_buf];
   if (MAXBUF == cb[IOCB_bufpos]) {
      fdRead(cb + IOCB_dcb,buf);
      cb[IOCB_bufpos] = 0;
   }
   cb[IOCB_pos]++;
   return (buf[cb[IOCB_bufpos]++]);
}

fputc(c,fd) {
   extrn FDErr, FDesc;
   auto cb, e, buf;

   cb  = FDesc + (fd << 4); /* sizeof_IOCB */
   if (!cb[0]) {
      FDErr = FDE_IOCB; return (EOT);
   }
   else if (cb[IOCB_pos] == cb[IOCB_size]) {
      FDErr = FDE_DISKFULL; return (EOT);
   }
   buf = cb[IOCB_buf];
   if (MAXBUF == cb[IOCB_bufpos]) {
      fdWrite(cb + IOCB_dcb,buf);
      cb[IOCB_bufpos] = 0;
   }
   cb[IOCB_pos]++;
   return (buf[cb[IOCB_bufpos]++] = c);
}

close(fd) {
   extrn FDErr, FDesc;
   auto cb, buf;

   cb = FDesc + (fd << 4); /* sizeof_IOCB */
   if (!cb[0])
      return (FDErr = FDE_IOCB);
   if (cb[IOCB_rw]) {
      if (cb[IOCB_bufpos]) {
         fdWrite(cb + IOCB_dcb,cb[IOCB_buf]);
      }
   }
   return (cb[0] = 0);
}

flush() {
   extrn wr.unit;

   close(wr.unit);
}

eof(fd) {
   extrn FDErr, FDesc;
   auto cb;

   cb = FDesc + (fd << 4); /* sizeof_IOCB */
   if (!cb[0])
      return (FDErr = FDE_IOCB);
   return (cb[IOCB_pos] == cb[IOCB_size]);
}

getadr() {
   auto adr;

   adr = getchar();
   return ((adr << 8) + getchar());
}

/* multiple segments, {adr:2,len:2,data:len}+ */
loadbin(bName) {
   extrn FDErr, rd.unit;
   auto old.rd, c;
   auto adr, n;

   if (openr(2,bName))
      return (FDErr);
   old.rd = rd.unit; rd.unit = 2;

   while (!eof(rd.unit)) {
      adr = getadr();
      if (!(n = getadr())) /* len=0, start adr */
         goto End;
      while (n--)
         lchar(0,adr++,getchar());
   }
End:
   close(rd.unit); rd.unit = old.rd;
   return (adr);
}

loadhex(bName) {
   extrn FDErr, rd.unit;
   auto old.rd, c, adr;

   if (openr(2,bName))
      return (FDErr);

   old.rd = rd.unit; rd.unit = 2;
   while (!eof(rd.unit)) {
LAgain:
      c = getchar();
      if (c == '!') {
         c = getchar();
         if (c == 'M' || c == 'U') {
            adr = gethex4(); goto LAgain;
         }
         else
            return (1);
      }
      else if (c == ',' || c == '*n')
         goto LAgain;
      lchar(0,adr++,gethex2());
   }
   close(rd.unit); rd.unit = old.rd;
   return (adr);
}

exec(bName) {
   extrn FDErr, DE;
   auto fnm MAXNAME_W;
   auto entry;

   fdStr2FName(fnm,bName);
   if (!fdFind(BYTES(fnm)))
      return (FDErr = FDE_NOTFOUND);
   entry = DE[DIRENT_type]? loadbin(bName) : loadhex(bName);
   if (entry & 1) /* odd entry means fmt error */
      return (FDErr = FDE_LOAD);
   entry =>> 1;
   return ((entry)());
}

FDesc[128]; /* 8 * sizeof_IOCB */
IOBufs[512]; /* 8 * MAXBUF_W */

/* vim: set ts=3 sw=3 et: */

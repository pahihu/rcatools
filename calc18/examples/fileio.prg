#include "fds.h"

openr(fd,bName) {
   extrn FDErr, DCB, DE, FDesc;
   auto cb, uni;

   cb = FDesc + (fd << 4); /* sizeof_IOCB */
   if (cb[0])
      return (FDErr = FDE_IOCB);
   if (fdParseFName(&uni,bName))
      return (FDErr = FDE_SYNTAX);
   fdStr2FName(cb,bName);

   fdSelect(uni);
   if (!fdFind(BYTES(cb)))
      return (FDErr = FDE_NOTFOUND);

   cb[IOCB_pos]    = 0;
   cb[IOCB_size]   = DE[DIRENT_size];
   mkdcb(uni,DE[DIRENT_tracks] & 0377,1);
   cb[IOCB_dcb]    = DCB[0];
   cb[IOCB_dcb+1]  = DCB[1];
   cb[IOCB_eof]    = 0;
   cb[IOCB_rw]     = 0;
   cb[IOCB_bufpos] = MAXBUF;

   return (0);
}

openw(fd,bName,size) {
   extrn FDErr, DCB, FDesc;
   auto cb, uni, dcb;
   auto t;

   cb = FDesc + (fd << 4); /* sizeof_IOCB */
   if (cb[0])
      return (FDErr = FDE_IOCB);
   if (fdParseFName(&uni,bName))
      return (FDErr = FDE_SYNTAX);
   fdStr2FName(BYTES(cb),bName);

   fdSelect(uni);
   if (!(t = fdCreate(BYTES(cb),size)))
      return (FDErr);

   cb[IOCB_pos]    = 0;
   cb[IOCB_size]   = size;
   mkdcb(uni,t,1);
   cb[IOCB_dcb  ]  = DCB[0];
   cb[IOCB_dcb+1]  = DCB[1];
   cb[IOCB_eof]    = 0;
   cb[IOCB_rw]     = 1;
   cb[IOCB_bufpos] = 0;

   return (0);
}

fgetc(fd) {
   extrn FDErr, FDesc, IOBufs;
   auto cb, buf;

   cb  = FDesc + (fd << 4); /* sizeof_IOCB */
   buf = IOBufs + (fd << 6); /* MAXBUF_W */
   if (!cb[0])
      return (FDErr = FDE_IOCB);
   if (cb[IOCB_pos] == cb[IOCB_size])
      return (EOT);
   if (MAXBUF == cb[IOCB_bufpos]) {
      fdRead(cb + IOCB_dcb,buf);
      cb[IOCB_bufpos] = 0;
   }
   cb[IOCB_pos]++;
   return (buf[cb[IOCB_bufpos]++]);
}

fputc(c,fd) {
   extrn FDErr, FDesc, IOBufs;;
   auto cb, buf;

   cb  = FDesc + (fd << 4); /* sizeof_IOCB */
   buf = IOBufs + (fd << 6); /* MAXBUF_W */
   if (!cb[0])
      return (FDErr = FDE_IOCB);
   if (cb[IOCB_pos] == cb[IOCB_size])
      return (FDErr = FDE_DISKFULL);
   if (MAXBUF == cb[IOCB_bufpos]) {
      fdWrite(cb + IOCB_dcb,buf);
      cb[IOCB_bufpos] = 0;
   }
   cb[IOCB_pos]++;
   buf[cb[IOCB_bufpos]++] = c;
   return (0);
}

close(fd) {
   extrn FDErr, FDesc, IOBufs;;
   auto cb, buf;

   cb = FDesc + (fd << 4); /* sizeof_IOCB */
   if (!cb[0])
      return (FDErr = FDE_IOCB);
   if (cb[IOCB_rw]) {
      if (cb[IOCB_bufpos]) {
         buf = IOBufs + (fd << 6); /* MAXBUF_W */
         fdWrite(cb + IOCB_dcb,buf);
      }
   }
   cb[0] = 0;
   return (0);
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
   return (cb[IOCB_pos] == cb[IOCB_size]? 1 : 0);
}

FDesc[128]; /* 8 * sizeof_IOCB */
IOBufs[512]; /* 8 * MAXBUF_W */

/* vim: set ts=3 sw=3 et: */

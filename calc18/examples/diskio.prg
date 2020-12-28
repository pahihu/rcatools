main() {
   extrn DCB;
   auto buf 64, i;

   puts("dskgo...*n");
   dskgo();

   puts("mkdcb...*n");
   mkdcb(0,2,1);

   puts("dskwr...*n");
   i = 0;
   while (i < 128) {
      lchar(buf,i,i); i++;
   }
   dskwr(DCB,buf);

   puts("clear buf...*n");
   i = 0; while (i < 64) buf[i++] = 0;

   puts("mkdcb...*n");
   mkdcb(0,2,1);

   puts("dskrd...*n");
   dskrd(DCB,buf);

   i = 0;
   while (i < 128) {
      hex2(char(buf,i++)); putchar(' ');
      if (!(i & 7))
         putchar('*n');
   }
}

hex(n) {
   putchar(char("0123456789ABCDEF",n & 017));
}

hex2(n) {
   hex(n >> 4);
   hex(n);
}

#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

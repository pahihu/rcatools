main() {
   auto buf 64, dcb 2, i;

   puts("dskgo...*n");
   dskgo();

   puts("mkdcb...*n");
   mkdcb(dcb,0,1,1);

   puts("reads...*n");
   reads(dcb,buf);

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

mkdcb(dcb,u,t,s) {
   dcb[1] = 0;
   dcb[0] = t + (--s << 8);
   if (u)
      dcb[0] =+ 040000;
}

reads(dcb,buf) {
   auto i, stc;

   i = 0;
   while (i < 128) {
      stc = eread(&dcb[1]);
      if (!(stc & 0400))
         puts("eread() failed*n");
      lchar(buf,i++,stc);
   }
}

writes(dcb,buf) {
   auto i, c, st;

   i = 0;
   while (i < 128) {
      c = char(buf,i++);
      st = ewrite(&dcb[1],c);
      if (!st)
         puts("ewrite() failed*n");
   }
}

#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

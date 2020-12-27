#include "elfbios.prg"

main() {
   auto buf 64, adr, byt, c, i, j, k;

   while (1) {
      f_msg("> "); f_inputl(buf,128);
      i = 0; c = char(buf,i++);
      if (c == '?') {
         f_hexin(buf,i,&adr);
         i = 0; adr = (adr >> 4) << 4;
         while (i < 64) {
            k = f_hexout4(buf,0,adr);
            lchar(buf,k++,':');
            j = 0;
            while (j++ < 16) {
               lchar(buf,k++,' ');
               k = f_hexout2(buf,k,char(0,adr++));
            }
            lchar(buf,k++,'*n'); lchar(buf,k,'*e');
            f_msg(buf);
            i =+ 16;
         }
      }
      else if (c == '!') {
         i = f_hexin(buf,i,&adr);
         while (char(buf,i) != '*e') {
            i = f_hexin(buf,i,&byt);
            lchar(0,adr++,byt);
         }
      }
      else if (c == '=') {
         i = f_hexin(buf,i,&adr);
         i = f_hexin(buf,i,&byt);
         f_hexin(buf,i,&j);
         f_memcpy(byt,adr,j);
      }
      else if (c == '@') {
         f_hexin(buf,i,&adr);
         (adr>>1)();
      }
      else if (c == '/')
         return;
   }
}

/* vim: set ts=3 sw=3 et: */

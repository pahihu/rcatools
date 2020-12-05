main() {
   auto x, y, z, a, c;

   x = y = 3;
   *0777 = 077;
   putc('@' + 2*3);
   putc('@' + 30 / 5);
   putc(13);
   putc(10);
   z = '@' + 6 % 7;
   while (z) {
      putc('0' + z % 10);
      z = z / 10;
      return;
   }
   putc(13);
   putc(10);
   z = '@' + 6 % 7;
   a = 0777;
   *(a--) = 0;
   while (z) {
      *(a--) = '0' + z % 10;
      z = z / 10;
   }
   while (c = *(++a)) {
      putc(c);
   }
   return (3 + z);
   end: ;
}

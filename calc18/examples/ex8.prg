main() {
   auto a, z, buf 16, c;

   z = 70;
   a = &buf[16];
   *--a = 0;
   while (z) {
      *--a = '0' + z % 10;
      z = z / 10;
   }
   print 13; print 10;
   while (c = *a++) {
      print c;
   }
}

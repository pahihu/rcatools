main() {
   auto a, z, sign, c;

   z = -32767;
   a = 0777;
   *(a--) = 0;
   sign = 0;
   if (z < 0) {
      sign = 1;
      z = -z;
   }
   while (z) {
      *(a--) = '0' + z % 10;
      z = z / 10;
   }
   if (sign) {
      *(a--) = '-';
   }
   print 13; print 10;
   while (c = *(++a)) {
      print c;
   }
}

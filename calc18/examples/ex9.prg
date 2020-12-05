sub(n) {
   auto a, buf 16, c;

   a = &buf[16];
   *--a = 0;
   while (n) {
      *--a = '0' + n % 10;
      n = n / 10;
   }
   while (c = *a++) {
      print c;
   }
}

main() {
   auto x, y, z, a, c;

   *0777 = 077;
   x = y = 3;
   print '@' + 2*3;
   print '@' + 30 / 5;
   print 13;
   print 10;
   z = '@' + 6 % 7;
   while (z) {
      print '0' + z % 10;
      z = z / 10;
   }
   print 13;
   print 10;
   z = '@' + 6 % 7;
   sub(z);
   print 'D'; print 13; print 10;
   end: ;
}

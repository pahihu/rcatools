main() {
   auto z, a, c;

   z = 70;
   a = 0777;
   *a = 0; a--;
   while (z) {
      *a = '0' + z % 10;
      a--;
      z = z / 10;
   }
   print 13; print 10;
   ++a;
   while (c = *a) {
      print c;
      a++;
   }
}

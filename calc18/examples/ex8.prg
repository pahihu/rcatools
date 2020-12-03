z = 70;
a = 01777;
*a = 0; a = a - 2;
while (z) {
   *a = '0' + z % 10;
   a = a - 2;
   z = z / 10;
}
print 13; print 10;
a = a + 2;
while (c = *a) {
   print c;
   a = a + 2;
}

z = -32767;
a = 01777;
*a = 0; a = a - 2;
sign = 0;
if (z < 0) {
   sign = 1;
   z = -z;
}
while (z) {
   *a = 060 + z % 10;
   a = a - 2;
   z = z / 10;
}
if (sign) {
   *a = 45;
   a = a - 2;
}
print 13; print 10;
a = a + 2;
while (c = *a) {
   print c;
   a = a + 2;
}

x = y = 3;
*0777 = 077;
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
a = 0777;
*(a--) = 0;
while (z) {
   *(a--) = '0' + z % 10;
   z = z / 10;
}
while (c = *(++a)) {
   print c;
}
end: ;

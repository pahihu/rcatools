x = y = 3;
*0777 = 077;
print '@' + 2*3;
print '@' + 30 / 5;
print 13;
print 10;
z = '@' + 6 % 7;
while (z) {
   print '0' + z % 10;
   z = z / 10 + fn1();
   return;
}
print 13;
print 10;
z = '@' + 6 % 7;
a = 0777;
*(a--) = 0;
while (z) {
   *(a--) = '0' + z % 10;
   z = z / 10 + fn2(1, 2+3, a+2);
}
while (c = *(++a)) {
   print c;
}
return (3 + z);
end: ;

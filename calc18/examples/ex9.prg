sub(n) {
   auto a, buf 16, c;

   a = &buf[16];
   *--a = 0;
   while (n) {
      *--a = '0' + n % 10;
      n = n / 10;
   }
   while (c = *a++) {
      putc(c);
   }
}

main() {
   auto x, y, z, a, c;

   *0777 = 077;
   x = y = 3;
   putc('@' + 2*3);
   putc('@' + 30 / 5);
   putc(13);
   putc(10);
   z = '@' + 6 % 7;
   while (z) {
      putc('0' + z % 10);
      z = z / 10;
   }
   putc(13);
   putc(10);
   z = '@' + 6 % 7;
   sub(z);
   putc('D'); putc(13); putc(10);
   end: ;
}

g;
v[10];
buf[16];

nl() {
   print 13;
   print 10;
}

putc(c) {
   print c;
}

itoa(n) {
   auto a, sign;

   sign = 0;
   if (n & 0100000) {
      sign = 1; n = -n;
   }
   a = &buf[16];
   *--a = 0;
   while (n) {
      *--a = '0' + n % 10;
      n = n / 10;
   }
   if (sign) {
      *--a = '-';
   }
   return (a);
}

puts(s) {
   auto c;
   while (c = *s++)
      print c;
}

main() {
   auto c, a;

   g = 'G'; v[0] = 'V'; v[9] = '9';
   nl();
   c = putc('E'); nl();
   putc(c); nl();
   putc(g); nl();
   putc(v[0]); nl();
   putc(v[9]); nl();
   a = itoa(-42);
   puts(a);
}

main() {
   auto a, b;
   auto c, d;
   auto s 8;
   register e, f;

   puts(" enter a string: ");
   gets(s); puts(s); putchar('*n');

   a = 2; b = 3; c = 4; d = 5;
   e = 6; f = 7;
   puts("a ** b = "); printn(a*b,10); putchar('*n');
   puts("c ** d = "); printn(c*d,10); putchar('*n');
   puts("2**f - e = "); printn(2*f-e,10); putchar('*n');
}

/* vim: set ts=3 sw=3 et: */

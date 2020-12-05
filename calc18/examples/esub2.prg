v[2000];
n;

putchar(c) {
   if (c == '*n')
      putc(13);
   putc(c);
}

main() {
   auto i, c, col, a;

   n = 2000;

   i = col = 0;
   while (i < n)
      v[i++] = 1;
   while (col<2*n) {
      a = n+1;
      c = i = 0;
      while (i<n) {
         c = c + v[i] * 10;
         v[i++] = c % a;
         c = c / a--;
      }
      putchar(c+'0');
      if (!(++col%5))
         putchar(col%50?' ':'*n');
   }
   putchar('*n*n');
}

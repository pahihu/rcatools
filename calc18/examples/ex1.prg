putchar(c) {
   if (c == '*n')
      putc(13);
   putc(c);

}

main() {
   auto x;
   x = 65;
   while (x < 91) {
     putchar(x);
     x = x + 1;
   }
}

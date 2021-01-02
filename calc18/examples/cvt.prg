main() {
   auto s, buf 16, n;

   puts("enter a hex number: $"); gets(s = &buf[0]); putchar('*n');
   puts(s); putchar('*n');
   n = atoi(2*s, 16);
   printn(n, 10); putchar('*n');
}

/* vim: set ts=3 sw=3 et: */

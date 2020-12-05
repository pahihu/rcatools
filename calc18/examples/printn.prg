putchar(c) {
   print c;
}

nl() {
   putchar(13);
   putchar(10);
}

/* Thompson: B Manual */
printn(n,b) {
   auto a;

   if (a=n/b) /* assignment, not test for equality */
      printn(a,b); /* recursive */
   putchar(n%b + '0');
}

main() {
   printn(42,10); nl();
   printn(42,8); nl();
   printn(42,2); nl();
}

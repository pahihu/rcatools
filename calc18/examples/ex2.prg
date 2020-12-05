nl() {
   putc(13);
   putc(10);
}

main() {
   auto x;
   for (x = 'A'; x <= 'Z'; x = x + 1) {
     putc(x);
   }
   nl();
   putc('E'); nl();
}

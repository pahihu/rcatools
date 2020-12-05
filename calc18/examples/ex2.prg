nl() {
   print 13;
   print 10;
}

putc(c) {
   print c;
}

main() {
   auto x;
   for (x = 'A'; x <= 'Z'; x = x + 1) {
     print x;
   }
   nl();
   putc('E'); nl();
}

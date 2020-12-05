g ;
v[10] ;


nl() {
   print 13;
   print 10;
}

putc(c) {
   print c;
   return (c+1);
}

main() {
   auto c;

   g = 'G'; v[0] = 'V';
   nl();
   c = putc('E'); nl();
   putc(c); nl();
   putc(g); nl();
   putc(v[0]); nl();
   v[9] = '9';
   putc(v[9]); nl();
}

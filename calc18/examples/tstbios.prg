#include "elfbios.prg"

prind(s,tod) {
   auto i;

   f_msg("date: ");
   i = f_dttoas(s,0,tod);
   lchar(s,i,'*e');
   f_msg(s);
   f_type('*n');
   return (s);
}

print(s,tod) {
   auto i;

   f_msg("time: ");
   i = f_tmtoas(s,0,tod);
   lchar(s,i,'*e');
   f_msg(s);
   f_type('*n');
   return (s);
}

prinn(n) {
   auto s 16, i;

   i = f_uintout(s,0,n);
   lchar(s,i,'*e');
   f_msg(s); f_type('*n');
}

prinh(n) {
   auto s 16, i;

   i = f_hexout4(s,0,n);
   lchar(s,i,'*e');
   f_msg(s); f_type('*n');
}

printod(tod) {
   auto s 16, i;

   i = f_hexout4(s,0,tod[0]);
   lchar(s,i++,' ');
   i = f_hexout4(s,i,tod[1]);
   lchar(s,i,'*e');
   f_msg(s); f_type('*n');
}

main() {
   extrn mq;
   auto tod 2, bdt 2, sd 16, st 16, s 16, i;
   auto n;

   f_gettod(tod);
   printod(tod);
   prind(sd,tod);
   print(st,tod);

   f_msg(sd); f_type('*n');
   f_msg(st); f_type('*n');

   i = f_astodt(sd,0,bdt);
   prind(s,bdt);
   i = f_astotm(st,0,bdt);
   print(s,bdt);
   printod(bdt);

   n = f_mul16(4321,4321);
   prinh(mq); prinh(n);
}

/* vim: set ts=3 sw=3 et: */

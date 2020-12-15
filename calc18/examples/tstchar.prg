#include "stdlib.prg"
#include "printf.prg"

myoct(msg,n) {
   printf("%s @ %o*n",msg,n);
}

lower(s) {
   auto c, i;

   i = -1;
   while ((c=char(s,++i)) != '*e')
      if (c >= 'A' && c <= 'Z') lchar(s,i,c-'A'+'a');
   return(s);
}

upper(s) {
   auto c, i;

   i = -1;
   while ((c=char(s,++i)) != '*e')
      if (c >= 'a' && c <= 'z') lchar(s,i,c-'a'+'A');
   return(s);
}

main() {
   extrn a, b, s, rd.unit;
   auto s1 8, s2 8, bigs 16, a1 8, a2 8, a3 8, n;

   puts("hello, world!*n");
   putchar(a); putchar(b);
   puts(s[0]); puts(s[1]);

   myoct("s1",s1);
   myoct("s2",s2);
printf("before cpy: s1 == s2 ? %d*n",s1 == s2);
   strcopy(s1, "string1");
printf("after cpy s1: %s*n",s1);
printf("after cpy: s1 == s2 ? %d*n",s1 == s2);
   strcopy(s2, "string2");
printf("after cpy s2: %s*n",s2);
printf("after cpy: s1 == s2 ? %d*n",s1 == s2);

   printf("s1 == s2 ? %d*n",s1 == s2);

   printf("s1: %s*n",s1);
   printf("s2: %s*n",s2);

   printf("rd.unit = %d*n",rd.unit);
   printf("lower case %s*n",a = lower("AbraCAdaBrA"));
   printf("upper case %s*n",upper(a));

   puts(concat(bigs,"hello",", world!",0)); putchar('*n');

   b = "well done";
   n = getarg(a1,b,0);
   n = getarg(a2,b,n);
   getarg(a3,b,n);
   printf("a1 = %s a2 = %s a3 = %s*n",a1,a2,a3);
}

a 'hi' ;
b '!*n' ;
s[2] "hello, ", "world!*n";
rd.unit 0;

/* vim: set ts=3 sw=3 et: */

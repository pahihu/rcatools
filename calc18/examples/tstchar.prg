#include "stdlib.prg"
#include "printf.prg"

strcpy(d,s) {
   auto i;

   i = 0;
   while (lchar(d,i,char(s,i)) != '*e')
      i++;
}

myoct(msg,n) {
   printf("%s @ %o*n",msg,n);
}

main() {
   extrn a, b, s;
   auto s1 8, s2 8;

   puts("hello, world!*n");
   putchar(a); putchar(b);
   puts(s[0]); puts(s[1]);

   myoct("s1",s1);
   myoct("s2",s2);
printf("before cpy: s1 == s2 ? %d*n",s1 == s2);
   strcpy(s1, "string1");
printf("after cpy s1: %s*n",s1);
printf("after cpy: s1 == s2 ? %d*n",s1 == s2);
   strcpy(s2, "string2");
printf("after cpy s2: %s*n",s2);
printf("after cpy: s1 == s2 ? %d*n",s1 == s2);

   printf("s1 == s2 ? %d*n",s1 == s2);

   printf("s1: %s*n",s1);
   printf("s2: %s*n",s2);
}

a 'hi' ;
b '!*n' ;
s[2] "hello, ", "world!*n";

/* vim: set ts=3 sw=3 et: */

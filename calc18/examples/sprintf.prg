/* Thompson: User's Reference to B */

sprintn(buf,i,n,b) {
   auto a;

   if (a=n/b)
      i=sprintn(buf,i,a,b);
   lchar(buf,i++,char("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",n%b));
   return (i);
}

sprintf(buf,fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9) {
   auto adx, x, c, i, j, k;

   k = 0; /* out idx */
   i = 0; /* fmt index */
   adx = &x1;  /* argument pointer */
loop:
   while ((c=char(fmt,i++)) != '%') {
      if (lchar(buf,k++,c) == '*e') {
         return;
      }
   }
   x = *adx++;
   switch (c = char(fmt,i++)) - 'a' {
   case 'd' - 'a': /* decimal */
      if (x & 0100000) {
         x = -x;
         lchar(buf,k++,'-');
      }
   case 'o' - 'a': /* octal */
      k = sprintn(buf,k,x,c=='o'?8:10);
      goto loop;

   case 'c' - 'a': /* char */
      lchar(buf,k++,x);
      goto loop;

   case 's' - 'a': /* string */
      j = 0;
      while ((c = char(x,j++)) != '*e')
         lchar(buf,k++,c);
      goto loop;
   }
   lchar(buf,k++,'%');
   i--;
   adx--;
   goto loop;
}

/* vim: set ts=3 sw=3 et: */

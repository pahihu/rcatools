/* stdlib functions */

putci(c) {
   if (!c || c == '*e')
      return;
   if (c == '*n')
      putc(13);
   putc(c);
}

putchar(c) { /* put char to UART */
   extrn putc;

   putci(c & 0377);
   putci(c >> 8);
   return (c);
}

getchar() { /* get char from UART */
   extrn getc;
   auto c;

   c = getc();
   if (c == 13)
      c = '*n';
   return (c);
}

char(s,i) { /* i-th char of string s */
   register n;

   n = s[i>>1];
   /* s[] contains "ABCD", accessed a 'BA' 'DC' */
   if (i&01)
      n=n>>8;
   return (n&0377);
}

lchar(s,i,c) { /* store char c in the i-th pos of string s */
   register n, x;

   n = s[x = i>>1];
   if (i&1)
      n = (n & 0377) + (c << 8);
   else
      n = (n & 0177400) + c;
   s[x] = n;
   return (c);
}

puts(s) {
   register c,i;

   i = 0;
   while ((c = char(s,i++)) != '*e')
      putchar(c);
}

gets(s) { /* get string */
   register i, c;

   i = 0;
   while ('*n' != (c = getchar())) {
      lchar(s,i++,c);
      putchar(c);
   }
   putchar(c);
   lchar(s,i,'*e');
}

printn(n,b) { /* print number in base b */
   register a;

   if (a=n/b)
      printn(a,b);
   /* putchar(n % b + '0'); */
   putchar(char("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ",n%b));
}

#ifdef NEED_atoi
isdigit(c) {
   return ('0' <= c && c <= '9');
}

isnum(c) {
   return (isdigit(c) || ('A' <= c && c <= 'Z'));
}

digit(c) {
   if (c <= '9')
      return (c - '0');
   return (10 + c - 'A');
}

atoi(s,b) { /* convert string to number in base b */
   auto c;
   register n, d, i;

   n = i = 0;
   while (isnum(c = char(s,i++))) {
      d = digit(c);
      if (b - 1 < d)
         return (0177777);
      n = b * n + d;
   }
   return (n);
}
#endif

#ifdef NEED_strcopy
strcopy(d,s) {
   register i;

   i = 0;
   while (lchar(d,i,char(s,i)) != '*e')
      i++;
   return(d);
}
#endif

#ifdef NEED_concat
concat(a,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10) { /* terminate w/ zero */
   auto i, adx;
   register c, j, x;

   j = 0; adx = &x1;
   while (x = *adx++) {
      i = 0;
      while ((c=char(x,i++)) != '*e')
         lchar(a,j++,c);
   }
   lchar(a,j,'*e');
   return (a);
}
#endif

#ifdef NEED_getarg
getarg(a,s,n) {
   register c, i;

   while(((c = char(s,n)) != '*e') && (c == ' ' || c == '*t'))
      n++;
   i = 0;
   while(((c = char(s,n)) != '*e') && (c != ' ' && c != '*t')) {
      lchar(a,i++,c); n++;
   }
   lchar(a,i,'*e');
   return (n);
}
#endif

/* Thompson: User's Reference to B */

#ifdef NEED_printf
printf(fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9) {
   extrn printn, char, putchar;
   auto adx, i;
   register c, j, x;

   i = 0; /* fmt index */
   adx = &x1;  /* argument pointer */
loop:
   while ((c=char(fmt,i++)) != '%') {
      if ('*e' == c)
         return;
      putchar(c);
   }
   x = *adx++;
   switch (c = char(fmt,i++)) - 'a' {
   case 'd' - 'a': /* decimal */
      if (x & 0100000) {
         x = -x;
         putchar('-');
      }
   case 'o' - 'a': /* octal */
      printn(x,c=='o'?8:10);
      goto loop;

   case 'c' - 'a': /* char */
      putchar(x);
      goto loop;

   case 's' - 'a': /* string */
      j = 0;
      while ((c = char(x,j++)) != '*e')
         putchar(c);
      goto loop;
   }
   putchar('%');
   i--;
   adx--;
   goto loop;
}
#endif

/* vim: set ts=3 sw=3 et: */

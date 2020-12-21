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


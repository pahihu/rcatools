typedef unsigned char u_char;
typedef unsigned int u_int;

u_int v[2000];
u_int n;

void putchar(u_char c) {
   if (c == '\n')
      putc(13);
   putc(c);
}

void main(void) {
   u_int i, c, col, a;

   n = 2000;

   i = col = 0;
   while (i < n)
      v[i++] = 1;
   while (col<2*n) {
      a = n+1;
      c = i = 0;
      while (i<n) {
         c = c + v[i] * 10;
         v[i++] = c % a;
         c = c / a--;
      }
      putchar(c+'0');
      if (!(++col%5))
         putchar(col%50?' ':'\n');
   }
   putchar('\n');
   putchar('\n');
}

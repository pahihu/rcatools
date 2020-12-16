/* LCC1802 example */

#define n 2000

typedef unsigned char u_char;
typedef unsigned int u_int;

u_int v[2000];

void putchar(u_char c) {
   if (c == '\n')
      putc(13);
   putc(c);
}

void main(void) {
   u_int i, c, col, a, vi, *pv;

   i = col = 0;
   while (i < n)
      v[i++] = 1;
   while (col<2*n) {
      a = n+1;
      c = 0; pv = v;
      while (a != 1) {
         /* c += 10 * v[i]; */
         c += (vi += 4 * (vi = 2 * (*pv)));
         *pv++ = c % a;
         c /= a--;
      }
      putchar(c+'0');
      if (!(++col%5))
         putchar(col%50?' ':'\n');
   }
   putchar('\n');
   putchar('\n');
}

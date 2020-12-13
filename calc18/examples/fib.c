/* LCC1802 example */

typedef unsigned char u_char;
typedef unsigned int u_int;

u_char buf[16];

void nl(void) {
   putc(13);
   putc(10);
}

u_char *itoa(u_int n) {
   u_char *a;

   a = &buf[16];
   *--a = 0;
   while (n) {
      *--a = '0' + n % 10;
      n = n / 10;
   }
   return (a);
}

void puts(u_char *s) {
   char c;
   while (c = *s++)
      putc(c);
}

void puti(u_int n) {
   puts(itoa(n));
}

u_int fib(u_int n) {
   if (n < 2)
      return (n);
   return (fib(n - 1) + fib(n - 2));
}

void main(void) {
   u_char *a;
   u_int n;

   n = fib(24); 
   puti(n); nl();
}

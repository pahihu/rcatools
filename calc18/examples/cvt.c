#define putchar putc
#define getchar getc

typedef unsigned char u_char;
typedef unsigned int u_int;

void nl() {
   putchar(13); putchar(10);
}

void gets(u_char *s) {
   u_char c;

   while ('\r' != (c = getchar()))
      putchar(*s++ = c);
   putchar(c);
   *s++ = '\0';
}

void puts(u_char *s) {
   u_char c;

   while (c = *s++)
      putchar(c);
}

void printn(u_int n,u_int b) {
   u_int a;

   if (a=n/b)
      printn(a,b);
   putchar(n % b + '0');
}

u_int ishex(u_char c) {
   return (('0' <= c && c <= '9') || ('A' <= c && c <= 'F'));
}

u_int digit(u_char c) {
   if (c <= '9')
      return (c - '0');
   return (10 + c - 'A');
}

u_int hex(u_char *s) {
   u_char c;
   u_int n;

   n = 0;
   while (ishex(c = *s++))
      n = 16 * n + digit(c);
   return (n);
}

void main() {
   u_char *s, buf[16];
   u_int n;

   gets(s = &buf[0]); nl();
   puts(s); nl();
   n = hex(s);
   printn(n, 10); nl();
}

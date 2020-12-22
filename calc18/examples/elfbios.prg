/* Elfish BIOS */

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

#define CTRL_C 03

f_type (c) {
   if (c == 13) {
      putc(27);
      putc('[');
      putc('2');
      putc('J');
   }
   else
      putc(c);
}

f_read (c)
   return (getc());

f_msg(s) {
   auto c, i;

   i = 0;
   while ((c = char(s,i++)) != '*e')
      f_type(c);
}

f_input(buf) {
   extrn f_inputl;
   return (f_inputl(buf,256));
}

f_strcmp(s1, s2) {
   auto i, c1, c2;

   i = 0;
   while ((c1 = char(s1,i)) == (c2 = char(s2,i)))
      i++;
   return (c1 - c2);
}

f_ltrim(s) { /* return next idx */
   auto c, i;
   
   i = 0;
   while (char(s,i) < 32)
      i++;
   return (i);
}

f_strcpy(d,s) { /* byte cpy */
   auto i;

   i = 0;
   while (lchar(d,i,char(s,i)) != '*e')
      i++;
}

f_memcpy(d,s,n) { /* word cpy */
   auto i;

   n =>> 1;
   i = 0;
   while (i < n) {
      d[i] = s[i]; i++;
   }
}

f_wrtsec(buf,sec) { /* TODO */
   extrn fdc;
   return (fdc);
}

f_rdsec(buf,sec) { /* TODO */
   extrn fdc;
   return (fdc);
}

f_seek0() { /* TODO */
   extrn fdc;
   return (fdc);
}

f_seek(trk) { /* TODO */
   extrn fdc;
   return (fdc);
}

f_drive(d) { /* TODO */
   return;
}

f_setbd() { /* fixed */
   extrn bd.rate;
   return (bd.rate);
}

f_mul16(a,b) { /* TODO: hi word? */
   extrn mq;
   auto ah,al,bh,bl;

   al = a & 0377; ah = a >> 8;
   bl = b & 0377; bh = b >> 8;
   mq = (ah * bh) + ((ah*bl+al*bh) >> 8);
   return (a*b);
}

f_div16(a,b) {
   extrn mq;
   auto q;

   q = a/b; mq = a%b;
   return (q);
}

f_idereset() { /* not supported */
   return;
}

f_idewrite(buf,ss,cyl,dev) { /* not supported */
   return (0);
}

f_ideread(buf,ss,cyl,dev) { /* not supported */
   return (0);
}

f_initcall() { /* in B run-time */
   return;
}

f_bootide() { /* not supported */
   return;
}

f_isnum(c) {
   return ('0' <= c && c <= '9');
}

f_ishex(c) {
   c =& ~32; /* toupper */
   return (f_isnum(c) || ('A' <= c && c <= 'F'));
}

f_isalpha(c) {
   c =& ~32;
   return ('A' <= c && c <= 'Z');
}

f_isalnum(c) {
   return (f_isalpha(c) || f_isnum(c));
}

f_isterm(c) {
   return (!f_isalnum(c));
}

digit(c) {
   if (c <= '9')
      return (c - '0');
   c =& ~32;
   return (c - 'A' + 10);
}

f_hexin(s,pn) { /* return next not converted index */
   auto c, i, n;

   n = i = 0;
   while (f_ishex(c = char(s,i))) {
      n = (n << 4) + digit(c); i++;
   }
   *(pn) = n;
   return (i);
}

hexc(n) {
   return (char("0123456789ABCDEF",n & 017));
}

f_hexout2(buf,n) {
   lchar(buf,0,hexc(n >> 4));
   lchar(buf,1,hexc(n));
   return (buf + 1);
}

f_hexout4(buf,n) {
   buf = f_hexout2(buf,n >> 8);
   return (f_hexout2(buf,n & 0377));
}

f_tty(c)
   return (putc(c));

f_minimon() {
   return; /* TODO */
}

f_freemem() {
   return (077777);
}

f_atoi(s,i,pn) {
   auto c, n;

   n = 0;
   while (f_isnum(c = char(s,i++)))
      n = 10 * n + digit(c);
   *(pn) = n;
   return (i);
}

f_uintout(buf,i,u) {
   auto a;

   if (a = u/10)
      i = f_uintout(buf,i,a);
   lchar(buf,i++,'0' + u % 10);
   return (i);
}

f_intout(buf,i,n) {
   if (n & 0177777) {
      lchar(buf,i++,'-');
      n = -n;
   }
   return (f_uintout(buf,i,n));
}

f_inmsg() { /* inline msg */
   return;
}

f_inputl(buf,n) {
   auto c, i;

   i = 0;
   while (i < n && (c = f_read()) != '*n' && c != CTRL_C)
      lchar(buf,i++,c);
   return (c == CTRL_C);
}

f_brktest() { /* TODO: chk for serial break */
   return (0);
}

f_findtkn(s,symtab) {
   auto i;

   i = 0;
   while (symtab[i]) {
      if (0 == f_strcmp(symtab[i],s))
         return (i);
   }
   return (0);
}

f_idnum(s) {
   auto n, i, t;

   t = 0;
   if (f_isnum(char(s,0))) {
      i = f_atoi(s,&n);
   } else if (f_ishex(char(s,0))) {
      t = 1;
      i = f_hexin(s,&n);
   }
   return (t + (!char(s,i))? 0200 : 0);
}

#define DEV_9118 128
#define DEV_1861  64
#define DEV_NVR   32
#define DEV_RTC   16
#define DEV_UART   8
#define DEV_BIO    4
#define DEV_FDD    2
#define DEV_IDE    1

f_getdev() {
   return (DEV_NVR + DEV_RTC + DEV_UART + DEV_BIO + DEV_FDD);
}

f_bread() { /* TODO */
   return (0);
}

f_btype(c) { /* TODO */
   return;
}

f_btest() { /* TODO */
   return (0);
}

f_utype(c) { /* TODO */
   return;
}

f_uread() { /* TODO */
   return (0);
}

f_usetbd(bd) { /* fixed UART speed */
   return (1);
}

f_idesize(drive) { /* no IDE disk */
   return (0);
}

f_ideid(buf,drive) {
   return (1);
}

f_rdnvr(offs,buf,n) {
   extrn rtc;
   auto i;

   if (!(f_getdev() & DEV_RTC))
      return (1);
   i = 0;
   while (i < n)
      lchar(buf,i++,char(rtc,offs++));
   return (0);
}

f_wrnvr(offs,buf,n) {
   extrn rtc;
   auto i;

   if (!(f_getdev() & DEV_RTC))
      return (1);
   i = 0;
   while (i < n)
      lchar(rtc,offs++,char(buf,i++));
   return (0);
}

f_gettod(buf) {
   return (f_rdnvr(0,buf,10));
}

f_settod(buf) {
   return (f_wrnvr(0,buf,10));
}

f_rtctest() { /* hardwired */
   return (128 + 114);
}

#define RTC_SS  0
#define RTC_MM  2
#define RTC_HH  4
#define RTC_DOW 6
#define RTC_D   7
#define RTC_M   8
#define RTC_Y   9

sprintn(buf,i,n) {
   auto a;

   if (a=n/10)
      i=sprintn(buf,i,a);
   lchar(buf,i++,char("0123456789",n%10));
   return (i);
}

f_dttoas(buf,i,tod) {
   i = sprintn(buf,i,char(tod,RTC_M));
   lchar(buf,i++,'/');
   i = sprintn(buf,i,char(tod,RTC_D));
   lchar(buf,i++,'/');
   return (sprintn(buf,i,char(tod,RTC_Y)+2000));
}

f_tmtoas(buf,i,tod) {
   i = sprintn(buf,i,char(tod,RTC_HH));
   lchar(buf,i++,':');
   i = sprintn(buf,i,char(tod,RTC_MM));
   lchar(buf,i++,':');
   return (sprintn(buf,i,char(tod,RTC_MM)));
}

f_astodt(buf,i,tod) { /* m/d/y */
   auto n;

   i = f_atoi(buf,i,&n);
   lchar(tod,RTC_M,n);
   i = f_atoi(buf,i,&n);
   lchar(tod,RTC_D,n);
   i = f_atoi(buf,i,&n);
   lchar(tod,RTC_Y,n-2000);
   return (i);
}

f_astotm(buf,i,tod) { /* hh:mm:ss */
   auto n;

   i = f_atoi(buf,i,&n);
   lchar(tod,RTC_HH,n);
   i = f_atoi(buf,i,&n);
   lchar(tod,RTC_MM,n);
   i = f_atoi(buf,i,&n);
   lchar(tod,RTC_SS,n);
   return (i);
}

rtc 037200; /* $7Dxx */

bd.rate 300; /* terminal baud rate */
fdc 0; /* FDC status */
mq 0; /* MQ reg - remainder */

/* vim: set ts=3 sw=3 et: */

/*
 * Elfish BIOS 
 *
 * History
 * =======
 *
 * 201223AP direct char pointers with [l]char(0,ptr,...)
 *          fixed unpkd()
 *
 */

#define CTRL_C 03

f_type (c) {
   if (c == 13) {
      putc(033);
      putc('[');
      putc('2');
      putc('J');
      return;
   }
   putc(c);
}

f_read () {
   auto c;

   c = getc();
   if (c == 13)
      c = '*n';
   return (c);
}

f_msg(s) {
   auto c;

   s =<< 1;
   while ((c = char(0,s++)) != '*e')
      f_type(c);
}

f_input(buf) {
   extrn f_inputl;
   return (f_inputl(buf,256));
}

f_strcmp(s1, s2) {
   auto c1, c2;

   s1 =<< 1; s2 =<< 1;
   while ((c1 = char(0,s1++)) == (c2 = char(0,s2++)));
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
   d =<< 1; s =<< 1;
   while (lchar(0,d++,char(0,s++)) != '*e');
}

f_memcpy(d,s,n) {
   auto i;

   i = 0;
   while (i++ < n)
      lchar(0,d++,char(0,s++));
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
   if (f_isnum(c))
      return (1);
   c =& ~32; /* toupper */
   return ('A' <= c && c <= 'F');
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

f_hexin(s,i,pn) { /* return next not converted index */
   auto c, n;

   n = 0;
   while (f_ishex(c = char(s,i++)))
      n = (n << 4) + digit(c);
   *(pn) = n;
   return (i);
}

hexc(n) {
   return (char("0123456789ABCDEF",n & 017));
}

f_hexout2(buf,i,n) {
   lchar(buf,i++,hexc(n >> 4));
   lchar(buf,i++,hexc(n));
   return (i);
}

f_hexout4(buf,i,n) {
   i = f_hexout2(buf,i,n >> 8);
   return (f_hexout2(buf,i,n & 0377));
}

f_tty(c)
   return (putc(c));

f_minimon() {
   return; /* TODO */
}

f_freemem() {
   return (037777);
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
   if (n & 0100000) {
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
   while (i < n && (c = f_read()) != '*n' && c != CTRL_C) {
      lchar(buf,i++,c);
      f_type(c);
   }
   f_type(c);
   lchar(buf,i,'*e');
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

   if (!(f_getdev() & DEV_RTC))
      return (1);
   f_memcpy(buf << 1,rtc + offs, n);
   return (0);
}

f_wrnvr(offs,buf,n) {
   extrn rtc;
   auto i;

   if (!(f_getdev() & DEV_RTC))
      return (1);
   f_memcpy(rtc + offs, buf << 1, n);
   return (0);
}

#define RTC_SS  0
#define RTC_MM  2
#define RTC_HH  4
#define RTC_DOW 6
#define RTC_D   7
#define RTC_M   8
#define RTC_Y   9

packd(tod,raw) { /* pack date */
   tod[0] = (char(raw,RTC_Y) << 9)
          + (char(raw,RTC_M) << 5)
          +  char(raw,RTC_D);
}

packt(tod,raw) { /* pack time */
   tod[1] = (char(raw,RTC_HH) << 11)
          + (char(raw,RTC_MM) <<  5)
          + (char(raw,RTC_SS) >>  1);
}

unpkd(raw,tod) { /* unpack date */
   lchar(raw,RTC_Y,tod[0] >> 9);
   lchar(raw,RTC_M,(tod[0] >> 5) & 017);
   lchar(raw,RTC_D,tod[0] & 037);
}

unpkt(raw,tod) { /* unpack time */
   lchar(raw,RTC_HH,tod[1] >> 11);
   lchar(raw,RTC_MM,(tod[1] >> 5) & 077);
   lchar(raw,RTC_SS,(tod[1] & 037) << 1);
}

f_gettod(tod) {
   auto raw 5;

   if (f_rdnvr(0,raw,10))
      return (1);
   packd(tod,raw);
   packt(tod,raw);
}

f_settod(tod) {
   auto raw 5;

   unpkd(raw,tod);
   unpkt(raw,tod);
   return (f_wrnvr(0,raw,10));
}

f_rtctest() { /* have RTC, with 114bytes of NVR */
   return (128 + 114);
}


sprintn(buf,i,n) {
   auto a;

   if (a=n/10)
      i=sprintn(buf,i,a);
   lchar(buf,i++,char("0123456789",n%10));
   return (i);
}

f_dttoas(buf,i,tod) {
   auto raw 5;

   unpkd(raw,tod);
   i = sprintn(buf,i,char(raw,RTC_M));
   lchar(buf,i++,'/');
   i = sprintn(buf,i,char(raw,RTC_D));
   lchar(buf,i++,'/');
   return (sprintn(buf,i,char(raw,RTC_Y)+2000));
}

f_tmtoas(buf,i,tod) {
   auto raw 5;

   unpkt(raw,tod);
   i = sprintn(buf,i,char(raw,RTC_HH));
   lchar(buf,i++,':');
   i = sprintn(buf,i,char(raw,RTC_MM));
   lchar(buf,i++,':');
   return (sprintn(buf,i,char(raw,RTC_SS)));
}

f_astodt(buf,i,tod) { /* m/d/y */
   auto n, raw 5;

   i = f_atoi(buf,i,&n);
   lchar(raw,RTC_M,n);
   i = f_atoi(buf,i,&n);
   lchar(raw,RTC_D,n);
   i = f_atoi(buf,i,&n);
   lchar(raw,RTC_Y,n-2000);
   packd(tod,raw);
   return (i);
}

f_astotm(buf,i,tod) { /* hh:mm:ss */
   auto n, raw 5;

   i = f_atoi(buf,i,&n);
   lchar(raw,RTC_HH,n);
   i = f_atoi(buf,i,&n);
   lchar(raw,RTC_MM,n);
   i = f_atoi(buf,i,&n);
   lchar(raw,RTC_SS,n);
   packt(tod,raw);
   return (i);
}

rtc 077400; /* $7Fxx */

bd.rate 300; /* terminal baud rate */
fdc 0; /* FDC status */
mq 0; /* MQ reg - remainder */

/* vim: set ts=3 sw=3 et: */

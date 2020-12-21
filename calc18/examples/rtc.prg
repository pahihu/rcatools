main() {
   extrn RTC,months,days;
   auto ss,mm,hh,d,m,y;

   ss = char(RTC,0);
   mm = char(RTC,2);
   hh = char(RTC,4);
   d  = char(RTC,7);
   m  = char(RTC,8);
   y  = char(RTC,9);
   printf("Today is %s %d, %d at %d:%d:%d*n",months[m-1],d,y+2000,hh,mm,ss);
}

RTC 037200; /* $7Dxx */
months[0] "Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec";
days[0] "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat";

#define NEED_printf
#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

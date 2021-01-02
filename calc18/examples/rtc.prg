main() {
   extrn RTC,months,days;
   auto ss,mm,hh,d,m,y,dow;

   ss = char(RTC,0);
   mm = char(RTC,2);
   hh = char(RTC,4);
   dow = char(RTC,6);
   d  = char(RTC,7);
   m  = char(RTC,8);
   y  = char(RTC,9);
   printf("Today is %s %s %d, %d at %d:%d:%d*n",days[dow-1],months[m-1],d,y+2000,hh,mm,ss);
}

RTC 037600; /* $7Fxx */
months[0] "Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec";
days[0] "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat";

/* vim: set ts=3 sw=3 et: */

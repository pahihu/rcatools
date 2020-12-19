test00() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test00*n");
}

test01() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test01*n");
}

test02() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test02*n");
}

test03() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test03*n");
}

test04() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test04*n");
}

test05() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test05*n");
}

test06() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test06*n");
}

test07() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test07*n");
}

test08() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test08*n");
}

test09() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test09*n");
}

test10() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test10*n");
}

test11() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test11*n");
}

test12() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test12*n");
}

test13() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test13*n");
}

test14() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test14*n");
}

test15() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("test15*n");
}

main() {
   test00(); test01(); test02(); test03();
   test04(); test05(); test06(); test07();
   test08(); test09(); test10(); test11();
   test12(); test13(); test14(); test15();
}

#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

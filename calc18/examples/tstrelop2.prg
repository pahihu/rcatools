test00() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("fail00*n");
   else
      puts("pass00*n");
}

test01() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("fail01*n");
   else
      puts("pass01*n");
}

test02() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("fail02*n");
   else
      puts("pass02*n");
}

test03() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 1; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("fail03*n");
   else
      puts("pass03*n");
}

test04() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("fail04*n");
   else
      puts("pass04*n");
}

test05() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass05*n");
   else
      puts("fail05*n");
}

test06() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass06*n");
   else
      puts("fail06*n");
}

test07() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 1; c = 1; d = 2; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass07*n");
   else
      puts("fail07*n");
}

test08() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("fail08*n");
   else
      puts("pass08*n");
}

test09() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass09*n");
   else
      puts("fail09*n");
}

test10() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass10*n");
   else
      puts("fail10*n");
}

test11() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 1; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass11*n");
   else
      puts("fail11*n");
}

test12() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 1; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("fail12*n");
   else
      puts("pass12*n");
}

test13() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 1; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass13*n");
   else
      puts("fail13*n");
}

test14() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 2; g = 1; h = 1;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass14*n");
   else
      puts("fail14*n");
}

test15() {
   auto a,b,c,d,e,f,g,h;

   a = 1; b = 2; c = 1; d = 2; e = 1; f = 2; g = 1; h = 2;
   if (((a < b) || (c < d)) && ((e < f) || (g < h)))
      puts("pass15*n");
   else
      puts("fail15*n");
}

main() {
   test00(); test01(); test02(); test03();
   test04(); test05(); test06(); test07();
   test08(); test09(); test10(); test11();
   test12(); test13(); test14(); test15();
}

#include "stdlib.prg"

/* vim: set ts=3 sw=3 et: */

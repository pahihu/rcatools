#ifndef __CALC18_H
#define __CALC18_H

#define CON 0
#define ID  1
#define OPR 2

typedef struct _NODE {
   int t;               // type: CON, ID, OPR
   int x;               // value
   struct _NODE *a[2];  // args
} NODE;

#define UNARY   128
#define INT     -1

#define PREINC  512+0
#define PREDEC  512+1
#define POSTINC 512+2
#define POSTDEC 512+3

#define LABEL   65536
void deflbl(int n);
char *getsym(int n);
int getoffs(int n);

#endif

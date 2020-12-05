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

#define INT     -1

#define C_UNDEF   0
#define C_EXTRN   1
#define C_AUTO    2
#define C_LABEL   3
#define C_PARAM   4

void defcls(int n, int cls, int offs);
char *getsym(int n);
int getoffs(int n);
int getcls(int n);
void dropsyms(void);
extern char *fn; // current fn

#endif

#ifndef __CALC18_H
#define __CALC18_H

#define CON 0
#define ID  1
#define OPR 2
#define STR 3

typedef struct _NODE {
   int t;               // type: CON, ID, OPR, STR
   int x;               // value
   char *r;             // reg
   char *s;             // string value
   int attr;            // attributes
   struct _NODE *a[2];  // args
} NODE;

#define A_SIMCMP        1

#define INT     -1

#define C_UNDEF   0
#define C_EXTRN   1
#define C_AUTO    2
#define C_LABEL   3
#define C_PARAM   4
#define C_REG     5

void defcls(int n, int cls, int offs);
char *getsym(int n);
int getoffs(int n);
void setoffs(int n, int offs);
int getcls(int n);
void dropsyms(void);
extern char *fn; // current fn
extern int Err;
extern int dbg, varstat;

NODE *nod(int typ, int n, char *s, NODE *arg0, NODE *arg1);
void freenod(NODE*);
int ex(NODE*);

#define opr(x,l,r)      nod(OPR,x,0,l,r)
#define opr3(x,a0,a1,a2)  nod(OPR,x,0,a0,nod(INT,x,0,a1,a2))
#define id(x)           nod(ID,x,0,NULL,NULL)
#define con(x)          nod(CON,x,0,NULL,NULL)
#define str(x)          nod(STR,0,x,NULL,NULL)

#define H printf
#define UNARY           1024

#endif

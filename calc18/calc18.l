%{
#include <stdlib.h>
#include "calc18.h"
#include "y.tab.h"
void yyerror(char*);

#define MAXSYM 128
int nsyms = 0, dseg = 0;
struct {
   char *sym;   // name
   int cls;     // storage cls
   int offs;    // addr/offset
} syms[MAXSYM];

int findsym(char*);
int defsym(char*);
int findid(char*);

/*
 * History
 * =======
 * 201202AP     initial revision, single char vars
 * 201203AP     keywords, symbols, char const
 *              goto, logical AND/OR/NOT
 * 201204AP     return, symbol table mgmt, extrn, auto
 *
 */
%}

%%

[a-zA-Z][a-zA-Z0-9]* {
                   int s;
                   if ((s = findid(yytext)))
                      return s;
                   if ((s = findsym(yytext)) < 0)
                      s = defsym(yytext);
                   yylval.sym = s;
                   return VAR;
                }

[0-9]+          {  yylval.con = strtol(yytext, (char **)NULL, '0' == *yytext ? 8 : 10);
                   return CONST;
                }

[-()<>=+*/;{}&|^~\[\]%:\?\!,.] {
                   return *yytext;
                }
"'"[^']+"'"     {
                   unsigned int c;
                   char *p = yytext + 1;
                   yylval.con = 0;
                   while ('\'' != (c = *p++))
                      yylval.con = 256 * yylval.con + c;
                   yylval.con &= 65535;
                   // fprintf(stderr,"%s = %d\n",yytext,yylval.con);
                   return CONST;
                }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"++"            return INC;
"--"            return DEC;
"<<"            return SHL;
">>"            return SHR;
"&&"            return LAND;
"||"            return LOR;

[ \t\n]+        { /*ignore whitespace */
                  char *p = yytext;
                  int c;
                  while ((c = *p++))
                     if ('\n' == c)
                        yylineno++;
                }

%%
int yywrap(void) {
   return 1;
}

int findsym(char *s) {
   int i;

   // search backwards
   for (i = nsyms; i; i--)
      if (0 == strcmp(syms[i-1].sym, s))
         return i-1;
   return -1;
}

int defsym(char *s) {
   int n;

   if (nsyms == MAXSYM) {
      fprintf(stderr,"symbol table full");
      exit(1);
   }
   n = nsyms++;
   syms[n].sym  = strdup(s);
   syms[n].offs = -1;
   syms[n].cls  = C_UNDEF;
   fprintf(stderr,"sym %s\n", yytext);

   return n;
}

int findid(char *s) {
   static struct {
      char *id;
      int tok;
   } ids[] = {
      {"auto",  AUTO},
      {"else" , ELSE},
      {"extrn", EXTRN},
      {"for"  , FOR},
      {"goto" , GOTO},
      {"if"   , IF},
      {"print", PRINT},
      {"return",RETURN},
      {"while", WHILE},
      {0      , 0}
   };
   int i;

   for (i = 0; ids[i].id; i++)
      if (0 == strcmp(ids[i].id, s))
         return ids[i].tok;
   return 0;
}

static char* clsnm[] = { "undef", "extrn", "auto", "label", "param" };

void redeferr(int n) {
   fprintf(stderr,"%s redefined, was %s\n",syms[n].sym,clsnm[syms[n].cls]);
   exit(1);
}

void defcls(int n, int cls, int offs) {
   if (C_UNDEF == syms[n].cls) {
      fprintf(stderr,"%s %s %d\n",clsnm[cls],syms[n].sym,offs);
      syms[n].cls = cls;
      syms[n].offs = offs;
      return;
   }
   redeferr(n);
}

char *getsym(int n) {
   return syms[n].sym;
}

int getoffs(int n) {
   if (syms[n].offs < 0) {
      syms[n].offs = dseg; dseg += 2;
      fprintf(stderr,"var %02X %s\n",syms[n].offs,syms[n].sym);
   }
   return syms[n].offs;
}

int getcls(int n) {
   return syms[n].cls;
}

int getnsyms(void) {
   return nsyms;
}

void dropsyms(int n) {
   int i;
   for (i = n; i < nsyms; i++)
      free(syms[i].sym);
   nsyms = n;
}


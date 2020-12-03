%{
#include <stdlib.h>
#include "calc18.h"
#include "y.tab.h"
void yyerror(char*);

#define MAXSYM 128
int nsyms = 0, dseg = 0;
struct {
   char *sym;
   int offs;
} syms[MAXSYM];

int findsym(char*);
int findid(char*);

/*
 * History
 * =======
 * 201202AP     initial revision, single char vars
 * 201203AP     keywords, symbols, char const
 *              logical AND/OR/NOT
 *
 */
%}

%%

[a-zA-Z][a-zA-Z0-9]* {
                   int s;
                   if ((s = findid(yytext)))
                      return s;
                   if ((s = findsym(yytext)) < 0) {
                      if (nsyms == MAXSYM) {
                         fprintf(stderr,"symbol table full");
                         exit(1);
                      }
                      s = nsyms++;
                      syms[s].sym = strdup(yytext);
                      syms[s].offs = -1;
                      fprintf(stderr,"sym %s\n", yytext);
                   }
                   yylval.sym = s;
                   return VAR;
                }

[0-9]+          {  yylval.con = strtol(yytext, (char **)NULL, '0' == *yytext ? 8 : 10);
                   return CONST;
                }

[-()<>=+*/;{}&|^~\[\]%:\?\!.] {
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
   for (i = 0; i < nsyms; i++) {
      if (0 == strcmp(syms[i].sym, s))
         return i;
   }
   return -1;
}

int findid(char *s) {
   static struct {
      char *id;
      int tok;
   } ids[] = {
      {"else" , ELSE},
      {"for"  , FOR},
      {"goto" , GOTO},
      {"if"   , IF},
      {"print", PRINT},
      {"while", WHILE},
      {0      , 0}
   };
   int i;

   for (i = 0; ids[i].id; i++)
      if (0 == strcmp(ids[i].id, s))
         return ids[i].tok;
   return 0;
}

void deflbl(int n) {
   if (LABEL != syms[n].offs) {
      fprintf(stderr,"lbl %s\n",syms[n].sym);
      syms[n].offs = LABEL;
   }
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

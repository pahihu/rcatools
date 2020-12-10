%{
/* Code based on Niemann: "Lex & Yacc" */
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

#define MAX_STR   128
void appendc(int c);
char strcon[MAX_STR+1];
int nstrcon;
int unescape(int c);

int dbg = 0;

#define LO(x)  ((x) & 0377)
#define HI(x)  LO((x) >> 8)

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

%x CHARCON STRCON

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

[-()<>=+*/;{}&|^~\[\]%:\?\!,] {
                   return *yytext;
                }

\'              { yylval.con = 0; BEGIN(CHARCON); }
<CHARCON>\'     { BEGIN(INITIAL);
                  yylval.con &= 65535;
                  if (HI(yylval.con))
                     yylval.con = 256 * LO(yylval.con) + HI(yylval.con);
                  return CONST;
                }
<CHARCON>\*.    { yylval.con *= 256; yylval.con += unescape(yytext[1]); }
<CHARCON>\n     { yyerror("unterminated char const"); }
<CHARCON>[^\*\n\']+ {
                  char *yptr = yytext;
                  while (*yptr) {
                     yylval.con *= 256;
                     yylval.con += 255 & *yptr++;
                  }
                }
\"              { nstrcon = 0; BEGIN(STRCON); }
<STRCON>\"      { BEGIN(INITIAL); strcon[nstrcon] = 0;
                  yylval.str = strdup(strcon);
                  return STRING;
                }
<STRCON>\*.     { appendc(unescape(yytext[1])); }
<STRCON>\n      { yyerror("unterminated string"); }
<STRCON>[^\*\n\"] { char *yptr = yytext;
                  while (*yptr)
                     appendc(*yptr++);
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

"=|"            return AOR;
"=&"            return AAND;
"==="           return AEQ;
"=!="           return ANE;
"=<"            return ALT;
"=<="           return ALE;
"=>"            return AGT;
"=>="           return AGE;
"=<<"           return ASHL;
"=>>"           return ASHR;
"=+"            return AADD;
"=-"            return ASUB;
"=%"            return AMOD;
"=*"            return AMUL;
"=/"            return ADIV;

"/\*"           { /* flex man page */
                  int c;
                  while(1) {
                     while ('*' != (c = input()) && EOF != c);
                     if ('*' == c) {
                        while ('*' == (c = input()));
                        if ('/' == c)
                           break;
                     }
                     if (EOF == c) {
                        yyerror("unterminated comment");
                        break;
                     }
                  }
                }

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
   if (dbg)
      fprintf(stderr,"sym %s\n", yytext);

   return n;
}

int findid(char *s) {
   static struct {
      char *id;
      int tok;
   } ids[] = {
      {"auto",  AUTO},
      {"case",  CASE},
      {"else" , ELSE},
      {"extrn", EXTRN},
      {"for"  , FOR},
      {"goto" , GOTO},
      {"if"   , IF},
      {"register", REGISTER},
      {"return",RETURN},
      {"switch",SWITCH},
      {"while", WHILE},
      {0      , 0}
   };
   int i;

   for (i = 0; ids[i].id; i++)
      if (0 == strcmp(ids[i].id, s))
         return ids[i].tok;
   return 0;
}

static char* clsnm[] = { "undef", "extrn", "auto", "label", "param", "reg" };

void redeferr(int n) {
   fprintf(stderr,"%s redefined, was %s\n",syms[n].sym,clsnm[syms[n].cls]);
   exit(1);
}

void defcls(int n, int cls, int offs) {
   if (C_UNDEF == syms[n].cls) {
      if (dbg)
         fprintf(stderr,"%s %s %d\n",clsnm[cls],syms[n].sym,offs);
      syms[n].cls = cls;
      syms[n].offs = offs;
      return;
   } else if (C_LABEL == syms[n].cls)
      return;
   redeferr(n);
}

char *getsym(int n) {
   return syms[n].sym;
}

int getoffs(int n) {
   return syms[n].offs;
}

void setoffs(int n, int offs) {
   syms[n].offs = offs;
}

int getcls(int n) {
   return syms[n].cls;
}

void dropsyms(void) {
   int i;
   for (i = 0; i < nsyms; i++)
      syms[i].cls = C_UNDEF;
}

int unescape(int c) {
   switch (c) {
   case '0': c = 0; break;
   case 'e': c = 0x13; break; // DC3
   case '(': c = '{'; break;
   case ')': c = '}'; break;
   case 't': c = '\t'; break;
   case '*':
   case '\'': 
   case '"': break;
   case 'n': c = '\n'; break;
   default:
      break;
   }
   return c;
}

void appendc(int c) {
   if (nstrcon == MAX_STR) {
      yyerror("string too long");
      return;
   }
   strcon[nstrcon++] = c;
}

/* vim: set ts=3 sw=3 et: */

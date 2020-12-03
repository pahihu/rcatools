%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "calc18.h"

extern int yylineno;

NODE *nod(int typ, int n, NODE *arg0, NODE *arg1);
void freenod(NODE*);
int ex(NODE*);
int yylex(void);

void yyerror(char*);
int vars[26];

#define opr(x,l,r)      nod(OPR,x,l,r)
#define opr3(x,a0,a1,a2)  nod(OPR,x,a0,nod(OPR,x,a1,a2))
#define id(x)           nod(ID,x,NULL,NULL)
#define con(x)          nod(CON,x,NULL,NULL)

/*
 * History
 * =======
 * 201202AP initial revision
 * 201203AP resolved yacc conflicts
 *          labels, goto lbl, logical AND/OR/NOT
 *
 */
%}

%union {
   int con;
   char sym;
   NODE *ptr;
};

%token <con> CONST
%token <sym> VAR
%token WHILE IF PRINT FOR INC DEC GOTO
%nonassoc IFX
%nonassoc ELSE

%nonassoc LVALUE
%left '='
%left '?' ':'
%left LOR
%left LAND
%left '|'
%left '^'
%left '&'
%left GE LE EQ NE '>' '<'
%left SHR SHL
%left '+' '-'
%left '*' '/' '%'
%nonassoc UNOP
%nonassoc GRP

%type <ptr> stmt expr stmt_list simplestmt lvalue base

%%

program:
        function        { exit(0); }
        ;

function:
        function stmt   { ex($2); freenod($2); }
        | /* NULL */
        ;

simplestmt:                     { $$ = opr(';', NULL, NULL); }
        | expr                  { $$ = $1; }
        ;

stmt:
          ';'                   { $$ = opr(';', NULL, NULL); }
        | expr ';'              { $$ = $1; }
        | PRINT expr ';'        { $$ = opr(PRINT, $2, NULL); }
        | WHILE '(' expr ')' stmt
                                { $$ = opr(WHILE, $3, $5); }
        /*  1   2    3    4          5   6    7 */
        | FOR '(' stmt stmt simplestmt ')' stmt
                                { $$ = opr(FOR, $7, opr(INT, $3, opr(INT, $4, $5))); }
        | IF '(' expr ')' stmt %prec IFX
                                { $$ = opr(IF, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt
                                { $$ = opr3(IF, $3, $5, $7); }
        | '{' stmt_list '}'     { $$ = $2; }
        | VAR ':' stmt          { deflbl($1); $$ = opr(':', id($1), $3); }
        | GOTO VAR ';'          { deflbl($2);
                                  $$ = opr(GOTO, id($2), NULL);
                                }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', $1, $2); }
        ;

expr:
          '(' expr ')'          { $$ = $2; }
        | lvalue %prec LVALUE   { $$ = $1; }
        | CONST                 { $$ = con($1); }
        | lvalue '=' expr       { $$ = opr('=', $1, $3); }
        | INC lvalue %prec UNOP { $$ = opr(PREINC, $2, NULL); }
        | DEC lvalue %prec UNOP { $$ = opr(PREDEC, $2, NULL); }
        | lvalue INC %prec GRP  { $$ = opr(POSTINC, $1, NULL); }
        | lvalue DEC %prec GRP  { $$ = opr(POSTDEC, $1, NULL); }
        | '-' expr %prec UNOP   { $$ = opr(UNARY + '-', $2, NULL); }
        | '!' expr %prec UNOP   { $$ = opr('!', $2, NULL); }
        | '~' expr %prec UNOP   { $$ = opr('~', $2, NULL); }
        | '&' lvalue %prec UNOP { $$ = opr(UNARY + '&', $2, NULL); }
        | expr '+' expr         { $$ = opr('+', $1, $3); }
        | expr '-' expr         { $$ = opr('-', $1, $3); }
        | expr '*' expr         { $$ = opr('*', $1, $3); }
        | expr '/' expr         { $$ = opr('/', $1, $3); }
        | expr '%' expr         { $$ = opr('%', $1, $3); }
        | expr SHR expr         { $$ = opr(SHR, $1, $3); }
        | expr SHL expr         { $$ = opr(SHL, $1, $3); }
        | expr '<' expr         { $$ = opr('<', $1, $3); }
        | expr '>' expr         { $$ = opr('>', $1, $3); }
        | expr GE expr          { $$ = opr(GE, $1, $3); }
        | expr LE expr          { $$ = opr(LE, $1, $3); }
        | expr NE expr          { $$ = opr(NE, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, $1, $3); }
        | expr '&' expr         { $$ = opr('&', $1, $3); }
        | expr '^' expr         { $$ = opr('^', $1, $3); }
        | expr '|' expr         { $$ = opr('|', $1, $3); }
        | expr LAND expr        { $$ = opr(LAND, $1, $3); }
        | expr LOR expr         { $$ = opr(LOR, $1, $3); }
        | expr '?' expr ':' expr
                                { $$ = opr('?', $1, opr(INT, $3, $5)); }
        ;

lvalue:
          VAR                   { $$ = id($1); }
        | '*' base %prec UNOP   { $$ = opr(UNARY + '*', $2, NULL); }
        | base '[' expr ']'     { $$ = opr('[', $1, $3); }
        ;

base:     VAR                   { $$ = id($1); }
        | CONST                 { $$ = con($1); }
        | '(' expr ')'          { $$ = $2; }
        ;

%%
NODE *nod(int typ, int x, NODE *a0,NODE *a1) {
   NODE *ret;

   ret = malloc(sizeof(NODE));
   ret->t = typ;
   ret->x = x;
   ret->a[0] = a0;
   ret->a[1] = a1;
   return ret;
}

void freenod(NODE *p) {
   int i;

   if (!p) return;
   if (OPR == p->t)
      for (i = 0; i < 2; i++)
         if (p->a[i])
            freenod(p->a[i]);
   free(p);
}

void yyerror(char *s) {
   fprintf(stderr, "%d: %s\n", yylineno, s);
}

int main(void) {
   yyparse();
   return 0;
}


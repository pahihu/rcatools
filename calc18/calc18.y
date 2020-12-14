%{
/* Code based on Niemann: "Lex & Yacc" */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "calc18.h"

extern int yylineno;
int yylex(void);

void yyerror(char*);

/*
 * History
 * =======
 * 201202AP initial revision
 * 201203AP resolved yacc conflicts
 *          labels, goto lbl, logical AND/OR/NOT
 * 201204AP function call, return
 *
 */

#define HILO(x)   (65535 & (x))

// NB. leaks CONFOLDx leak memory
#define CONFOLD1(op,cop,a) \
   (a && CON == a->t)? con(HILO(cop (a->x))) : opr(op,a,NULL)

#define CONFOLD2(op,a,cop,b) \
   (a && b && CON == a->t && CON == b->t) \
   ? con(HILO(HILO(a->x) cop HILO(b->x))) \
   : opr(op,a,b)

#define LOGFOLD1(op,cop,a) \
   (a && CON == a->t)? con(cop (a->x)? 0xFFFF : 0) : opr(op,a,NULL)

#define LOGFOLD2(op,a,cop,b) \
   (a && b && CON == a->t && CON == b->t) \
   ? con((a->x) cop (b->x)? 0xFFFF : 0) \
   : opr(op,a,b)

%}

%union {
   int con;
   char sym;
   char *str;
   NODE *ptr;
};

%token <con> CONST
%token <sym> VAR
%token <str> STRING
%token WHILE IF FOR INC DEC GOTO RETURN EXTRN AUTO REGISTER FUNCALL PREINC PREDEC POSTINC POSTDEC ILST XLST FUNDEF VARDEF VECDEF EXTDEF AUTODEF REGDEF SWITCH CASE INIVPTR
%nonassoc IFX
%nonassoc ELSE

%nonassoc LVALUE
%right '=' AOR AAND AEQ ANE ALT ALE AGT AGE ASHL ASHR AADD ASUB AMOD AMUL ADIV
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
%right UNOP
%left GRP

%type <ptr> stmt compound_stmt stmt_list expr expr_list id id_list simplestmt lvalue base extdef

%%

program:
          definition            { exit(Err); }
        ;

definition:
          definition extdef     { ex($2); freenod($2); }
        | /* NULL */
        ;


extdef:
          VAR id_list ';'          { $$ = opr(VARDEF, id($1), opr(INT,con(2),$2)); }
        | VAR '[' CONST ']' id_list ';' { $$ = opr(VECDEF, id($1), opr(INT,con(2*$3),$5)); }
        | VAR '(' id_list ')' stmt { $$ = opr(FUNDEF, id($1), opr(INT, $3, $5)); }
        ;

id_list:
          /* NULL */            { $$ = NULL; }
        | id                    { $$ = opr(ILST, NULL, $1); }
        | id_list ',' id        { $$ = opr(ILST, $1, $3); }
        ;

id:
          VAR                   { $$ = id($1); }
        | VAR CONST             { $$ = id($1); $$->a[0] = con(2*$2); }
        | CONST                 { $$ = con($1); $$->a[0] = con($1); }
        | STRING                { $$ = str($1); }
        ;

simplestmt:                     { $$ = opr(';', NULL, NULL); }
        | expr                  { $$ = $1; }
        ;

stmt:
          ';'                   { $$ = opr(';', NULL, NULL); }
        | EXTRN id_list ';'     { $$ = opr(EXTDEF, $2, NULL); }
        | AUTO id_list ';'      { $$ = opr(AUTODEF, $2, NULL); }
        | REGISTER id_list ';'  { $$ = opr(REGDEF, $2, NULL); }
        | expr ';'              { $$ = $1; }
        | WHILE '(' expr ')' stmt
                                { $$ = opr(WHILE, $3, $5); }
        | compound_stmt         { $$ = $1; }
        | SWITCH expr compound_stmt { $$ = opr(SWITCH,$2,$3); }
        | CASE CONST ':'        { $$ = opr(CASE, con($2), NULL); }
        /*  1   2    3    4          5   6    7 */
        | FOR '(' stmt stmt simplestmt ')' stmt
                                { $$ = opr(FOR, $7, opr(INT, $3, opr(INT, $4, $5))); }
        | IF '(' expr ')' stmt %prec IFX
                                { $$ = opr3(IF, $3, $5, NULL); }
        | IF '(' expr ')' stmt ELSE stmt
                                { $$ = opr3(IF, $3, $5, $7); }
        | VAR ':' stmt          { defcls($1,C_LABEL,0); $$ = opr(':', id($1), $3); }
        | GOTO VAR ';'          { defcls($2,C_LABEL,0);
                                  $$ = opr(GOTO, id($2), NULL);
                                }
        | RETURN ';'            { $$ = opr(RETURN, NULL, NULL); }
        | RETURN '(' expr ')' ';' { $$ = opr(RETURN, $3, NULL); }
        | error ';'             { $$ = NULL; }
        | error '}'             { $$ = NULL; }
        ;

compound_stmt:
        '{' stmt_list '}'       { $$ = $2; }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', $1, $2); }
        ;

expr_list:
          /* NULL */            { $$ = NULL; }
        | expr                  { $$ = opr(XLST, NULL, $1); }
        | expr_list ',' expr    { $$ = opr(XLST, $1, $3); }
        ;

expr:
          '(' expr ')'          { $$ = $2; }
        | lvalue %prec LVALUE   { $$ = $1; }
        | CONST                 { $$ = con($1); }
        | STRING                { $$ = str($1); }
        | lvalue '=' expr       { $$ = opr('=', $1, $3); }
        | lvalue AOR expr       { $$ = opr(AOR, $1, $3); }
        | lvalue AAND expr      { $$ = opr(AAND, $1, $3); }
        | lvalue AEQ expr       { $$ = opr(AEQ, $1, $3); }
        | lvalue ANE expr       { $$ = opr(ANE, $1, $3); }
        | lvalue ALT expr       { $$ = opr(ALT, $1, $3); }
        | lvalue ALE expr       { $$ = opr(ALE, $1, $3); }
        | lvalue AGT expr       { $$ = opr(AGT, $1, $3); }
        | lvalue AGE expr       { $$ = opr(AGE, $1, $3); }
        | lvalue ASHL expr      { $$ = opr(ASHL, $1, $3); }
        | lvalue ASHR expr      { $$ = opr(ASHR, $1, $3); }
        | lvalue AADD expr      { $$ = opr(AADD, $1, $3); }
        | lvalue ASUB expr      { $$ = opr(ASUB, $1, $3); }
        | lvalue AMOD expr      { $$ = opr(AMOD, $1, $3); }
        | lvalue AMUL expr      { $$ = opr(AMUL, $1, $3); }
        | lvalue ADIV expr      { $$ = opr(ADIV, $1, $3); }
        | INC lvalue %prec UNOP { $$ = opr(PREINC, $2, NULL); }
        | DEC lvalue %prec UNOP { $$ = opr(PREDEC, $2, NULL); }
        | lvalue INC %prec GRP  { $$ = opr(POSTINC, $1, NULL); }
        | lvalue DEC %prec GRP  { $$ = opr(POSTDEC, $1, NULL); }
        | VAR '(' expr_list ')' %prec GRP { $$ = opr(FUNCALL, id($1), $3); }
        | '-' expr %prec UNOP   { $$ = CONFOLD1(UNARY + '-',-,$2); }
        | '!' expr %prec UNOP   { $$ = LOGFOLD1('!',!,$2); }
        | '~' expr %prec UNOP   { $$ = CONFOLD1('~',~,$2); }
        | '&' lvalue %prec UNOP { $$ = opr(UNARY + '&', $2, NULL); }
        | expr '+' expr         { $$ = CONFOLD2('+',$1,+,$3); }
        | expr '-' expr         { $$ = CONFOLD2('-',$1,-,$3); }
        | expr '*' expr         { $$ = CONFOLD2('*',$1,*,$3); }
        | expr '/' expr         { $$ = CONFOLD2('/',$1,/,$3); }
        | expr '%' expr         { $$ = CONFOLD2('%',$1,%,$3); }
        | expr SHR expr         { $$ = CONFOLD2(SHR,$1,>>,$3); }
        | expr SHL expr         { $$ = CONFOLD2(SHL,$1,<<,$3); }
        | expr '<' expr         { $$ = LOGFOLD2('<',$1,<,$3); }
        | expr '>' expr         { $$ = LOGFOLD2('>',$1,>,$3); }
        | expr GE expr          { $$ = LOGFOLD2(GE, $1,>=,$3); }
        | expr LE expr          { $$ = LOGFOLD2(LE, $1,<=,$3); }
        | expr NE expr          { $$ = LOGFOLD2(NE, $1,!=,$3); }
        | expr EQ expr          { $$ = LOGFOLD2(EQ, $1,==,$3); }
        | expr '&' expr         { $$ = CONFOLD2('&',$1,&,$3); }
        | expr '^' expr         { $$ = CONFOLD2('^',$1,^,$3); }
        | expr '|' expr         { $$ = CONFOLD2('|',$1,|,$3); }
        | expr LAND expr        { $$ = opr(LAND, $1, $3); }
        | expr LOR expr         { $$ = opr(LOR, $1, $3); }
        | expr '?' expr ':' expr
                                { $$ = opr('?', $1, opr(INT, $3, $5)); }
        ;

lvalue:
          VAR                   { $$ = id($1); }
        | '*' expr %prec UNOP   { $$ = opr(UNARY + '*', $2, NULL); }
        | base '[' expr ']'     { $$ = opr('[', $1, $3); }
        ;

base:     VAR                   { $$ = id($1); }
        | CONST                 { $$ = con($1); }
        | '(' expr ')'          { $$ = $2; }
        ;

%%
NODE *nod(int typ, int x, char *s, NODE *a0,NODE *a1) {
   NODE *ret;

   ret = malloc(sizeof(NODE));
   ret->t = typ;
   ret->x = x;
   ret->s = s;
   ret->a[0] = a0;
   ret->a[1] = a1;
   ret->attr = 0;
   ret->r    = NULL;
   return ret;
}

void freenod(NODE *p) {
   int i, n;
   char *s;

   if (!p) return;
   if (OPR == p->t || INT == p->t)
      for (i = 0; i < 2; i++)
         if (p->a[i])
            freenod(p->a[i]);
   if (p->s) {
      s = p->s;
      H(" ..STR%d=%s\n",p->x,s);
      H(" ORG*+(* AND 1)\n");
      H("L%d: CONST #",p->x);
      n = strlen(s);
      for (i = 0; i < n; i++)
         H("%02X",s[i]);
      H("13"); // DC3
      H("\n");
      free(s);
   }
   free(p);
}

void yyerror(char *s) {
   fprintf(stderr, "%d: %s\n", yylineno, s);
   Err = 1;
}

extern int dbg;
int main(int argc, char*argv[]) {
   int i;

   for (i = 1; i < argc; i++) {
      if ('-' == *argv[i])
         switch (argv[i][1]) {
         case 'd': dbg = 1; break;
         }
   }
   yyparse();
   exit(Err);
}

/* vim: set ts=3 sw=3 et: */

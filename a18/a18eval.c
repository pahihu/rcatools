/*
	HEADER:		CUG149;
	TITLE:		1805A Cross-Assembler (Portable);
	FILENAME:	A18EVAL.C;
	VERSION:	2.5;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the RCA 1802, 1804, 1805, 1805A, 1806, and
			1806A microprocessors.  The program is written in
			portable C rather than BDS C.  All assembler features
			are supported except relocation, linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			RCA, CDP1802, CDP1805A;

	SEE-ALSO:	CUG113, 1802 Cross-Assembler;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program has compiled successfully on 2 UNIX
			compilers, 5 MSDOS compilers, and 2 CP/M compilers.
			A port to BDS C would be extremely difficult, but see
			volume CUG113.  A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      1805A Cross-Assembler in Portable C

		   Copyright (c) 1985 William C. Colley, III

Revision History:

Ver	Date		Description

2.0	APR 1985	Recoded from BDS C version 1.1.  WCC3.

2.1	AUG 1985	Greatly shortened the routines find_symbol() and
			new_symbol().  Fixed bugs in expression evaluator.
			Added compilation instructions for Aztec C86,
			Microsoft C, and QNIX C.  Added optional optimizations
			for 16-bit machines.  Adjusted structure members for
			fussy compilers.  WCC3.

2.2	SEP 1985	Added the INCL pseudo-op and associated stuff.  WCC3.

2.3	JUL 1986	Added compilation instructions and tweaks for CI-C86,
			Eco-C88, and Lattice C.  WCC3.

2.4	JAN 1987	Fixed bug that made "BYTE 0," legal syntax.  WCC3.

2.5	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A18 FILE.ASM -L".  WCC3 per Alex Cameron.

	Aug 2010	change "line" to "lline", Turbo C issue HRJ

	June 2018	add backslash char escape support - L. Christensen

This file contains the assembler's expression evaluator and lexical analyzer.
The lexical analyzer chops the input character stream up into discrete tokens
that are processed by the expression analyzer and the line assembler.  The
expression analyzer processes the token stream into unsigned results of
arithmetic expressions.
*/

/*  Get global goodies:  */

#include "a18.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h> /* for backslash code */

unsigned char valbytes[128];

/* local  prototypes HRJ*/
static unsigned eval(unsigned);
static void exp_error(char);
void unlex(void);
TOKEN *lex(void);
static void make_number(unsigned);
int popc(void);
void pushc(char);
int isalph(char);
static int ischar(char), ishex(char);
int isnum(char), isnumprefix(char);
static int isalpnum(char c);

/* external prototypes HRJ*/
void pops(char *), trash(void);
OPCODE *find_operator(char *);
SYMBOL *find_symbol(char *);

int  asm_line(void);
void lclose(void), lopen(char *), lputs(void);
void hclose(void), hopen(char *), hputc(unsigned);
void fatal_error(char *), warning(char *);
void hseek(unsigned);
void unlex(void);
char *backsub(char *str); /*loren Christesen*/


/*  Get access to global mailboxes defined in A68.C:			*/

extern char lline[];
extern int filesp, forwd, pass;
extern unsigned pc;
extern FILE *filestk[], *source;
extern TOKEN token;
extern int comment, multisep;

/*  Expression analysis routine.  The token stream from the lexical	*/
/*  analyzer is processed as an arithmetic expression and reduced to an	*/
/*  unsigned value.  If an error occurs during the evaluation, the	*/
/*  global flag	forwd is set to indicate to the line assembler that it	*/
/*  should not base certain decisions on the result of the evaluation.	*/

static int bad;
static unsigned evalcount;

unsigned expr(void)
{
    SCRATCH unsigned u;

    DIAG(printf(" expr"));
    evalcount = 0;
    bad = FALSE;
    u = eval(START);
    return bad ? 0 : u;
}

unsigned evals(void)
{
    return evalcount;
}

static unsigned eval(pre)
unsigned pre;
{
    register unsigned op, u, v;


    DIAG(printf(" eval"));
    evalcount++;
    for (;;) {
	u = op = lex() -> valu;
	DIAG(printf(" tok_type=%d",token.attr & TYPE));
	switch (token.attr & TYPE) {
	    case SEP:	if (pre != START) unlex();
	    case MULTI:
	    case EOL:	exp_error('E');
			DIAG(printf(" lave1"));
			return (u); /* return had no value HRJ*/

	    case OPR:	if (!(token.attr & UNARY)) { exp_error('E');  break; }
			u = (('$' == op || '*' == op) ? pc :
			    eval((op == '+' || op == '-') ?
				(unsigned) UOP1 : token.attr & PREC));
			switch (op) {
			    case '-': u = word(0-u);  break; /* had (-u) HRJ */

			    case NOT:	u ^= 0xffff;  break;

			    case HIGH:	u = high(u); break;

			    case LOW:	u = low(u); break;
                            case 'A':   break;
			}
			if (MULTI == (token.attr & TYPE))
			    return u;

	    case VAL:
	    case STR:	for (;;) {
			    op = lex() -> valu;
			    switch (token.attr & TYPE) {
				case SEP:   if (pre != START) unlex();
				case MULTI:
				case EOL:   if (pre == LPREN) exp_error('(');
					    DIAG(printf(" lave2"));
					    return u;

				case STR:
				case VAL:   exp_error('E');  break;

				case OPR:   if (!(token.attr & BINARY)) {
						exp_error('E');  break;
					    }
					    if ((token.attr & PREC) >= pre) {
						unlex();  return u;
					    }
					    if (op != ')')
						v = eval(token.attr & PREC);
					    switch (op) {
						case '+':   u += v;  break;

						case '-':   u -= v;  break;

						case '*':   u *= v;  break;

						case '/':   u /= v;  break;

						case MOD:   u %= v;  break;

						case AND:   u &= v;  break;

						case OR:    u |= v;  break;

						case XOR:   u ^= v;  break;

						case '<':   u = u < v;  break;

						case LE:    u = u <= v;  break;

						case '=':   u = u == v;  break;

						case GE:    u = u >= v;  break;

						case '>':   u = u > v;  break;

						case NE:    u = u != v;  break;

						case SHL:   if (v > 15)
								exp_error('E');
							    else u <<= v;
							    break;

						case SHR:   if (v > 15)
								exp_error('E');
							    else u >>= v;
							    break;

						case ')':   if (pre == LPREN)
								return u;
							    exp_error('(');
							    break;
					    }
					    clamp(u);
					    break;
			    }
			}
			break;
	}
    }
}

static void exp_error(char c)
{
    forwd = bad = TRUE;  error(c);
}

/*  Lexical analyzer.  The source input character stream is chopped up	*/
/*  into its component parts and the pieces are evaluated.  Symbols are	*/
/*  looked up, operators are looked up, etc.  Everything gets reduced	*/
/*  to an attribute word, a numeric value, and (possibly) a string	*/
/*  value.								*/

static int oldt = FALSE;
static int quote = FALSE;

TOKEN *lex(void) /* function lex() */
{
    SCRATCH char c, first, *p;
    SCRATCH unsigned b;
    SCRATCH OPCODE *o;
    SCRATCH SYMBOL *s;
    unsigned make_number2(char *str,unsigned base);


    if (oldt) { oldt = FALSE;  return &token; }
    trash();
LAGAIN:
    if (isalph(c = popc())) {
        if ('T' == c) {
            c = popc();
            if (c == '\'') {
                pushc(c); goto LAGAIN;
            }
            else {
                pushc(c); c = 'T';
            }
        }
	pushc(c);  pops(token.sval);
	DIAG(printf(" lex_sval=[%s]",token.sval));
	if ((o = find_operator(token.sval))) {
	    token.attr = o -> attr;
	    token.valu = o -> valu;
	}
	else {
	    token.attr = VAL;  token.valu = 0;
            first = token.sval[0];
	    if (isnumprefix(token.sval[0])) {
		DIAG(printf(" H%c",first)); /* hrj */
                make_number(0);
	    }
	    else if ((s = find_symbol(token.sval))) {
		token.valu = s -> valu;
		if (pass == 2 && s -> attr & FORWD) forwd = TRUE;
	    }
	    else {
		if (pass == 1) /* not defined yet */
		    token.valu = word(pc + 1);
		else exp_error('U');
	    }
	}
    }
    else if (isnum(c)) {
	pushc(c);  pops(token.sval);
        make_number(0);
    }
    else switch (c) {
	case '(':   token.attr = UNARY + LPREN + OPR;
		    goto opr1;

	case ')':   token.attr = BINARY + RPREN + OPR;
		    goto opr1;

	case '+':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '-':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '*':   token.attr = BINARY + MULT + OPR;
		    goto opr1;

	case '/':   token.attr = BINARY + MULT + OPR;
opr1:		    token.valu = c;
		    break;

	case '<':   token.valu = c;
		    if ((c = popc()) == '=') token.valu = LE;
		    else if (c == '>') token.valu = NE;
		    else pushc(c);
		    goto opr2;

	case '=':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = LE;
		    else if (c == '>') token.valu = GE;
		    else pushc(c);
		    goto opr2;

	case '>':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = NE;
		    else if (c == '=') token.valu = GE;
		    else pushc(c);
opr2:		    token.attr = BINARY + RELAT + OPR;
		    break;

	case '\'':
	case '"':  /*new code L. Christensen */
			{
				int doescape = FALSE;		// flag whether to substitute.
				quote = TRUE;
				token.attr = STR;

				// copy the quoted string to sval.
				for (p = token.sval; (*p = popc()); ++p) {

					if (*p == c) {
						break;				// closing quote found, done.
					} else if (*p == '\\') {
						*(++p) = popc();	// escape sequence, copy next char literally.
						doescape = TRUE;
					} else if (*p == '\n') {
						exp_error('"');		// hit newline before closing quote.
						break;
					}

				}	// for p = token.sval.

				*p = '\0';		// null terminate, (overwrites the closing quote.)

				if(doescape) {
				// do backslash escape substitution.
					char *subbed = backsub(token.sval);
					strcpy(token.sval, subbed);
					free(subbed);
				}

				quote = FALSE;
				if ((token.valu = token.sval[0]) && token.sval[1])
					token.valu = (token.valu << 8) + token.sval[1];
				break;
			}
	case ',':   token.attr = SEP;
		    break;

        case '\n':  token.attr = EOL;
		    DIAG(printf(" eol"));
		    break;

        default:    if (multisep == c) {
                        token.attr = MULTI;
		        DIAG(printf(" stmtsep"));
                    }
    }
    return &token;
}

int isnumprefix(char c)
{
    return ('$' == c) || ('%' == c) || ('@' == c) || ('#' == c);
}

unsigned make_number2(char *str, unsigned base)
{
    SCRATCH char *p;
    SCRATCH unsigned valu, d, i, n, b;

    DIAG(printf(" number=[%s]",str));
    n = strlen(str);
    if (0 == base) {
        b = 0;
        if ('$' == *str || '#' == *str) {
	    b = 16; str++;
        }
	else if('@' == *str) {
	    b = 8; str++;
        }
	else if ('%' == *str) {
	    b = 2; str++;
	}
	else {
	    switch (toupper(str[n-1])) {
	        case 'B':    b = 2; break;

	        case 'O':
	        case 'Q':    b= 8;  break;

	        case 'D':    b = 10;  break;

	        case 'H':    b = 16;  break;
	    }
	}
        if (b) n--;
        base = b ? b : 10;
    }
    valu = 0;
    for (p = str, i = 0; i < n; p++, i++) {
	d = toupper(*p) - (isnum(*p) ? '0' : 'A' - 10);
	valu = valu * base + d;
        if (16 == base && 0 == (n & 1)) {
            if (i & 1)
                valbytes[i / 2] = valu & 0xFF;
        }
	if (!ishex(*p) || d >= base) { exp_error('D');  break; }
    }
    clamp(valu);
    DIAG(printf(" -> %d (%04X)\n",valu,valu));
    return valu;
}

static void make_number(base)
unsigned base;
{
    SCRATCH char *p;
    SCRATCH unsigned d;

    token.attr = VAL;
    token.valu = make_number2(token.sval, base);
    clamp(token.valu);
    return;
}

int isalph(char c)
{
    return (c >= '@' && c <= '~') || c == '#' || c == '*' || c == '$' || c == '%' || c == '&' || c == '.' || c == ':' || c == '?';

}

int isterm(char c)
{
    return c == '@' || c == '#' || c == '*' || c == '$' || c == '%' || c == '&' || c == ':' || c == '?';
}

int isnum(char c)
{
    return c >= '0' && c <= '9';
}

static int ishex(char c)
{
    return isnum(c) || ((c = toupper(c)) >= 'A' && c <= 'F');
}

static int isalpnum(char c) /* was isalnum(), already in ctype.h HRJ */
{
    return isalph(c) || isnum(c);
}

/*  Push back the current token into the input stream.  One level of	*/
/*  pushback is supported.						*/

void unlex(void)
{
    oldt = TRUE;
    return;
}

/*  Get an alphanumeric string into the string value part of the	*/
/*  current token.  Leading blank space is trashed.			*/

void pops(s)
char *s;
{
    int beg = 1;

    trash();
    for (; isalpnum(*s = popc()); ++s) {
        if (!beg && isterm(*s))
            break;
        beg = 0;
    }
    if (*s == ':') {
        s++;
    } else
        pushc(*s);
    *s = '\0';
    return;
}

/*  Trash blank space and push back the character following it.		*/

void trash()
{
    SCRATCH char c;

    while ((c = popc()) == ' ');
    pushc(c);
    return;
}

/*  Get character from input stream.  This routine does a number of	*/
/*  other things while it's passing back characters.  All control	*/
/*  characters except \t and \n are ignored.  \t is mapped into ' '.	*/
/*  Semicolon is mapped to \n.  In addition, a copy of all input is set	*/
/*  up in a line buffer for the benefit of the listing.			*/

static int oldc, oldc2, eol, xoldc;
static char *lptr;

static int xgetc(FILE *source)
{
    SCRATCH int c;

    if (xoldc) { c = xoldc; xoldc = '\0'; return c; }
    c = getc(source);
    if (c != EOF)
        c &= 0377;
    return c;
}

int popc(void)
{
    SCRATCH int c, doskip;

    if (oldc) { c = oldc;  oldc = oldc2; oldc2 = '\0';  return c; }
    if (eol) return '\n';
    for (;;) {
        c = xgetc(source);
        if (c != EOF && !quote && (comment == c)) {
            doskip = 1;
            if (c == '.') {
               c = xgetc(source);
               if (c == '.')
                  *lptr++ = '.';
               else {
                  xoldc = c;
                  c = '.';
                  doskip = 0;
               }
            }
            if (doskip) {
	        do *lptr++ = c;
	        while ((c = xgetc(source)) != EOF && c != '\n');
            }
	}
	if (c == EOF) c = '\n';
	if ((*lptr++ = c) >= ' ' && c <= '~') return c;
	if (c == '\n') { eol = TRUE;  *lptr = '\0';  xoldc = '\0'; return '\n'; }
	if (c == '\t') return quote ? '\t' : ' ';
    }
}

/*  Push character back onto input stream.  Only one level of push-back	*/
/*  supported.  \0 cannot be pushed back, but nobody would want to.	*/

void pushc(char c)
{
    oldc2 = oldc;
    oldc  = c;
    return;
}

/*  Begin new line of source input.  This routine returns non-zero if	*/
/*  EOF	has been reached on the main source file, zero otherwise.	*/

void multiline()
{
  oldc = oldc2 = xoldc = '\0'; oldt = eol = FALSE;
}

int newline()
{
    multiline(); lptr = lline;
    while (feof(source)) {
	if (ferror(source)) fatal_error(ASMREAD);
	if (filesp) {
	    fclose(source);
	    source = filestk[--filesp];
	}
	else return TRUE;
    }
    return FALSE;
}


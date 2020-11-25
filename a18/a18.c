/*
	HEADER:		CUG149;
	TITLE:		1805A Cross-Assembler (Portable);
	FILENAME:	A18.C;
	VERSION:	2.5+;
	DATE:		Aug 2010;

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

2.5+ JAN 2010	Program fixed to compile with LCC in Windows.
				Mostly by updating function declarations,
				replacing () with (VOID), etc. SOme fixes need
				when char and int were used interchangably,
				unsigned was given signs, etc.
				labels ending with colon now have colon stripped
				All fixes have HRJ in comment lines    HRJ.

	 Apr 9 2010 HRJ - bad checksum, fixed

         Aug 30 2010 HRJ - fixes for Turbo C 2.01 in MS-DOS
      May 2017 HRJ binary file output
	  Mar 2018 HRJ Christensen - added uppercase hex on listing option
     Mar 2018   Added SCRT pseudo ops "CALL" and "RETN". L. Christensen.

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines to
the line assembly routine, and sends the results to the listing and object file
output routines.  It also coordinates the activities of everything.  The line
assembly routine uses the expression analyzer and the lexical analyzer to
parse the source line convert it into the object bytes that it represents.
*/

/*  Get global goodies:  */

#include "a18.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* eternal routines HRJ*/

    int  asm_line(int cont);
    void lclose(void), lopen(char *), lputs(void);
    void hclose(void), hopen(char *), hputc(unsigned);
    void bclose(void), bopen(char *), bputc(unsigned);
    void xclose(void), xopen(char *);
    void fatal_error(char *), warning(char *);
    void pops(char *), pushc(int), trash(void);
	void hseek(unsigned);
	void unlex(void);
	int isalph(char); /* was int isalph(int) HRJ */

	/* these are local but used before defined HRJ */
	static void do_label(void),normal_op(void), pseudo_op(void);
	static void flush(void);

    extern unsigned char valbytes[128];

/*  Define global mailboxes for all modules:				*/

/* Turbo C has "line" as graphic function HRJ */

char errcode, lline[MAXLINE + 1], title[MAXLINE];
int pass = 0, newpass;
int eject, filesp, forwd, listhex, caphex = 1, lineno;
int comment = ';', multisep = '!', ut4mon = 0, autobr = 0;
unsigned cvtlo = 0;
unsigned address, bytes, errors, listleft, pagelen, pc;
unsigned line_bytes, line_obj[MAXLINE], *obj;
unsigned filler;
int (*symcmp)(const char*,const char*);
FILE *filestk[FILES], *source;
TOKEN token;

SYMBOL *find_symbol(char *), *new_symbol(char *);

static
void rcaregs(void)
{
    SCRATCH int i;
    SCRATCH char buf[8];
    SCRATCH SYMBOL *l;

    for (i = 0; i < 16; i++) {
        sprintf(buf, "R%X", i);
        l = new_symbol(buf);
	l -> attr = FORWD + VAL;
	l -> valu = i;
    }
}

/*  Mainline routine.  This routine parses the command line, sets up	*/
/*  the assembler at the beginning of each pass, feeds the source text	*/
/*  to the line assembler, feeds the result to the listing and hex file	*/
/*  drivers, and cleans everything up at the end of the run.		*/

static int done, extend, ifsp, off;

static
void usage(void)
{
  fprintf(stderr,"usage: a18 <file.asm> [-f number] [-air] [-l|-L|-m file.lst] [-o file.hex] [-b file.bin] [-x file.exp]\n");
  fprintf(stderr,"       -f <number>    use low byte of number as filler\n");
  fprintf(stderr,"       -h             usage\n");
  fprintf(stderr,"       -a             convert SBR/LBR\n");
  fprintf(stderr,"       -i             ignore case in labels\n");
  fprintf(stderr,"       -r             define register names R1..RF\n");
  fprintf(stderr,"       -m file.idi    listing file in IDIOT/UT4 format\n");
  fprintf(stderr,"       -l file.lst    listing file in lowercase hex\n");
  fprintf(stderr,"       -L file.lst    listing file in uppercase hex\n");
  fprintf(stderr,"       -o file.hex    Intel Hex output\n");
  fprintf(stderr,"       -b file.bin    binary file output\n");
  fprintf(stderr,"       -x file.exp    symbol export output\n");
  exit(1);
}

static
int do_line(int prn, int oldcont)
{
    SCRATCH unsigned *o;
    SCRATCH int cont, i;

    DIAG(if (prn) printf("\ndo_line"));

    bytes = 0;
    obj = &line_obj[line_bytes];
    for (i = 0; i < 2*BIGINST; obj[i++] = NOP);

    newpass = pass;
    cont = asm_line(oldcont);
    pc = word(pc + bytes);
    line_bytes += bytes;
    if (pass == 2) {
	if (0 == cont) {
            lputs();
            for (o = line_obj; line_bytes--;) {
                bputc(*o); hputc(*o++);
            }
	}
    }
    pass = newpass;
    return cont;
}

int main(argc,argv)
int argc;
char **argv;
{
    SCRATCH unsigned *o;
    SCRATCH int cont, i;
    int newline(void);
    void multiline(void);
    unsigned make_number2(char*, unsigned);

    printf("1802/1805A Cross-Assembler (Portable) Ver 2.5+\n");
    printf("Copyright (c) 1985 William C. Colley, III\n");
	printf("fixes for LCC/Windows by HRJ Aug 2010\n\n");

    filler = 0x00; symcmp = strcmp;
    while (--argc > 0) {
	if (**++argv == '-') {
	    switch ((*++*argv)) { // removed toupper
		case 'f':
		case 'F':   if (!*++*argv) {
			        if (!--argc) fatal_error(NOFILLER);
				else ++argv;
			    }
			    filler = 0xff & make_number2(*argv, 0);
			    break;
		case 'h':
		case 'H':   usage();
			    break;
                case 'a':
                case 'A':   autobr = 1;
                            break;
		case 'i':
		case 'I':   symcmp = strcasecmp;
			    break;
                case 'r':
                case 'R':   multisep = ';';
                            comment = '.';
                            rcaregs();
                            break;
		case 'l':   caphex = 0;  // hex address, data lowercase
		case 'L':   
dolst:			    if (!*++*argv) {
				if (!--argc) { warning(NOLST);  break; }
				else ++argv;
			    }
			    lopen(*argv);
			    break;
		case 'm':   
		case 'M':   ut4mon = 1;  // UT4 format
			    goto dolst;
		case 'o':
		case 'O':   if (!*++*argv) {
				if (!--argc) { warning(NOHEX);  break; }
				else ++argv;
			    }
			    hopen(*argv);
			    break;
		case 'b':
		case 'B':   if (!*++*argv) {
				if (!--argc) { warning(NOBIN);  break; }
				else ++argv;
			    }
			    bopen(*argv);
			    break;
		case 'x':
		case 'X':   if (!*++*argv) {
				if (!--argc) { warning(NOEXP);  break; }
				else ++argv;
			    }
			    xopen(*argv);
			    break;

		default:    warning(BADOPT);
	    }
	}
	else if (filestk[0]) warning(TWOASM);
	else if (!(filestk[0] = fopen(*argv,"r"))) fatal_error(ASMOPEN);
    }
    if (!filestk[0]) fatal_error(NOASM);

    while (++pass < 3) {
	fseek(source = filestk[0],0L,0);  done = extend = off = FALSE;
	errors = filesp = ifsp = pagelen = pc = 0;  title[0] = '\0';
	lineno = 1;
	while (!done) {
	    errcode = ' ';
	    if (newline()) {
		DIAG(printf("\n--newline--"));
		error('*');
		strcpy(lline,"\tEND\n");
		done = eject = TRUE;  listhex = FALSE;
		line_bytes = 0;
		lineno++;
	    }
	    else {
    		address = pc; eject = forwd = listhex = FALSE;
		line_bytes = 0;

		cont = do_line(1, 0);
		lineno++;
		while (cont) {
		    multiline();
		    cont = do_line(0, cont);
		}
	    }
	}
    }

    fclose(filestk[0]);  lclose();  hclose(); bclose(); xclose();

    if (errors) printf("%d Error(s)\n",errors);
    else printf("No Errors\n");

    exit(errors);
    return errors;
}

/*  Line assembly routine.  This routine gets expressions and tokens	*/
/*  from the source file using the expression evaluator and lexical	*/
/*  analyzer, respectively.  It fills a buffer with the machine code	*/
/*  bytes and returns nothing.						*/

static char label[MAXLINE];
static int ifstack[IFDEPTH] = { ON };

static OPCODE *opcod;

int asm_line(int cont)
{
    SCRATCH int i, j;
    int popc(void);
    OPCODE *find_code(char *), *find_operator(char *);


    DIAG(printf("\n\tasm_line"));

    label[0] = '\0';
    if (!cont && (i = popc()) != ' ' && i != '\n') {
	if (isalph((char) i)) {
	    pushc(i);  pops(label);
		/* HRJ remove colon from label */
		j = strlen(label); if (label[j-1]==':') label[j-1] ='\0';
		DIAG(printf(" asm_label>>%s<<",label)); /* HRJ diagnostic */
	    if (find_operator(label)) { label[0] = '\0';  error('L'); }
	}
	else {
	    error('L');
	    while ((i = popc()) != ' ' && i != '\n');
	}
    }

LOPS:
    trash(); opcod = NULL;
    if (2 == cont)
        pushc(',');
    if ((i = popc()) != '\n') {
        if (i == ',') {
            DIAG(printf(" comma"));
            opcod = find_code("CONST");
        }
        else if (i == '=') {
            DIAG(printf(" equal"));
            opcod = find_code("EQU");
        }
	else if (!isalph((char) i)) error('S');
	else {
	    pushc(i);  pops(token.sval);
	    DIAG(printf(" sval[%s]",token.sval));
	    if (!(opcod = find_code(token.sval))) {
                if (!cont && !label[0]) {
                    strcpy(label, token.sval);
		    j = strlen(label); if (label[j-1]==':') label[j-1] ='\0';
		    DIAG(printf(" asm_label>>%s<<",label)); /* HRJ diagnostic */
                    goto LOPS;
                }
                error('O');
            }
	}
	if (!opcod) { listhex = TRUE;  bytes = BIGINST; }
    }

    if (opcod && opcod -> attr & ISIF) { if (label[0]) error('L'); }
    else if (off) { listhex = FALSE;  flush();  return 0; }

    if (!opcod) { do_label();  flush(); }
    else {
	listhex = TRUE;
	token.attr = EOL;
	if (opcod -> attr & PSEUDO) {DIAG(printf(" pseudo")); pseudo_op();}
	else {DIAG(printf(" normal")); normal_op();}
	if (MULTI == (token.attr & TYPE))
	    return 1;
        if (SEP == (token.attr & TYPE)) {
            return 2;
        }
	while ((i = popc()) != '\n') {
	    if (i == multisep) return 1;
            if (i == ',') return 2;
	    if (i != ' ') {
		if (pass != 1)
		    error('T');
	    }
	}
    }
    source = filestk[filesp];
    return 0;
}

static void flush(void)
{
    int popc(void);

    while (popc() != '\n');
}

static void do_label(void)
{
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol(char *), *new_symbol(char *);

    if (label[0]) {
	DIAG(printf(" do_label>>%s<<",label)); /* HRJ diagnostic */
	listhex = TRUE;
	if (pass == 1) {
	    if ((l = find_symbol(label))) { /* AP exported */
		l -> valu = pc;
		DIAG(printf(" val=%04X",pc));
	    }
	    else if (!((l = new_symbol(label)) -> attr)) {
		l -> attr = FORWD + VAL;
		l -> valu = pc;
		DIAG(printf(" val=%04X",pc));
	    }
	}
	else {
	    if ((l = find_symbol(label))) {
		l -> attr &= ~FORWD;
		if (l -> valu != pc) {
		    DIAG(printf("\nlabel=[%s] pc=%04X val=%04X",label,pc,l->valu));
		    error('M');
		}
	    }
	    else error('P');
	}
    }
}

static void normal_op(void)
{
    SCRATCH unsigned attrib, *objp, operand;
    unsigned expr(void);
    TOKEN *lex(void);
    void do_label(void), unlex(void), clear_symbols(unsigned);
    SCRATCH int cvt, done;
    SCRATCH unsigned c0de; 

    do_label();
    bytes = (attrib = opcod -> attr) & BYTES;

    if (pass != 1) {
        objp = obj;
        if (attrib & IS1805) {
	    *objp++ = PREBYTE;
	    if (!extend) error('O');
        }
        *objp++ = opcod -> valu;  objp[0] = objp[1] = 0;
    }

    cvt = done = 0;
    while (attrib & (REGTYP + NUMTYP)) {
	operand = expr();
        DIAG(printf(" attrib=%d operand=%d",attrib,operand));
	switch (attrib & REGTYP) {
	    case IOPORT:    if (operand > 7) { error('R');  return; }

	    case NOT_R0:    if (!operand) { error('R');  return; }

	    case ANY:	    if (operand > 15) { error('R');  return; }
			    if (pass != 1) *(objp - 1) += operand;
			    attrib &= ~(REGTYP);
			    break;

	    case NONE:	    switch (attrib & NUMTYP) { /*none was null HRJ */
				case SIXTN:	if (high(operand) == high(pc + 1)) {
						    c0de = opcod -> valu;
						    cvt = autobr &&
                                                          ((0xC0 <= c0de && c0de < 0xC4) ||
						    	   (0xC8 <= c0de && c0de < 0xCC));
						    if (cvt) {
							if (pass != 1) {
						            bytes--;
						            objp[-1] -= 0x90; /* convert to SBR */
                                                            *objp++   = low(operand);
                                                            if (cvtlo < pc + 1) {
							        printf("%04X converted to SBR\n",pc);
                                                                clear_symbols(pc + 1); cvtlo = pc + 1;
                                                                newpass = 1;
                                                            }
                                                            done = 1;
                                                        }
                                                    }
                                                }
                                                if (!done && pass != 1) {
LSIXTN:                                             *objp++ = high(operand);
						    *objp = low(operand);
						}
						break;

				case BRANCH:    if (high(operand) != high(pc + bytes - 1)) {
						    c0de = opcod -> valu;
						    cvt = autobr &&
                                                          ((0x30 <= c0de && c0de < 0x34) ||
						    	   (0x38 <= c0de && c0de < 0x3C));
						    if (cvt) {
							if (pass != 1) {
						            bytes++;
						            objp[-1] += 0x90; /* convert to LBR */
                                                            *objp++   = high(operand);
						            *objp     = low(operand);
                                                            if (cvtlo < pc + 2) {
							        printf("%04X converted to LBR\n",pc);
                                                                clear_symbols(pc + 2); cvtlo = pc + 2;
                                                                newpass = 1;
                                                            }
                                                            done = 1;
						        }
						    }
						    else {
						        error('B');  return;
						    }
						}
						if (!done && pass != 1)
						    *objp = low(operand);
						break;

				case IMMED:	if (operand > 0xff &&
						    operand < 0xff80) {
						    error('V');  return;
						}
						if (pass != 1)
						    *objp = low(operand);
			    }
			    attrib &= ~NUMTYP;
			    break;
	}
    }
}

static void pseudo_op(void)
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u, ulen, i;
    SCRATCH SYMBOL *l;
    unsigned expr(void);
    unsigned evals(void);
    SYMBOL *find_symbol(char *), *new_symbol(char *);
    TOKEN *lex(void);
    int isnum(char), isnumprefix(char);


    o = obj;
    switch (opcod -> valu) {
	case BLK:   do_label();
		    u = word(pc + expr());
		    if (forwd) error('P');
		    else {
			pc = u;
			if (pass == 2) hseek(pc);
		    }
		    break;

	case BYTE:  do_label(); DIAG(printf(" psop BYTE")); /* HRJ debug*/
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0; /* return 0 on seperator */
			else if ((token.attr & TYPE) == STR) { /* string added HRJ*/
	    		    for (s = token.sval; *s; *o++ = *s++)
			    ++bytes;
			    if ((lex() -> attr & TYPE) != SEP) unlex();
			} /* end string HRJ */
			else { /* its some kind of value or expression HRJ */
			    unlex();
			    if ((u = expr()) > 0xff && u < 0xff80) {
				u = 0;  error('V'); /* byte values only */
			    }
				*o++ = low(u);  ++bytes;
			}
			DIAG(printf(" u=%i s=%u attr=%i",u, obj[0], token.attr)); /* hrj debug*/
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case CPU:   listhex = FALSE;  do_label();
		    u = expr();
		    if (forwd) error('P');
		    else if (u != 1802 && u != 1805) error('V');
		    else extend = u == 1805;
		    break;

	/* loren:  SCRT "CALL" and "RETN" pseudo ops */
	case CALL:  do_label();  bytes = 3;
		    obj[0] = (0xd0 + 4);  /* SEP R4 */
		    u = expr();
		    obj[1] = high(u);  obj[2] = low(u);
		    break;

	case RETN:  bytes = 1;
		    obj[0] = (0xd0 + 5);  /* D5, SEP R5 */
		    break;

	case EJCT:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) != EOL) {
			unlex();  pagelen = expr();
			if (pagelen > 0 && pagelen < 3) {
			    pagelen = 0;  error('V');
			}
		    }
		    eject = TRUE;
		    break;

	case ELSE:  listhex = FALSE;
		    if (ifsp) off = (ifstack[ifsp] = -ifstack[ifsp]) != ON;
		    else error('I');
		    break;

	case END:   do_label();
		    if (filesp) { listhex = FALSE;  error('*'); }
		    else {
			done = eject = TRUE;
			if (ut4mon) eject = FALSE;
			if (pass == 2 && (lex() -> attr & TYPE) != EOL) {
			    unlex();  hseek(address = expr());
			}
			if (ifsp) error('I');
		    }
		    break;

	case ENDI:  listhex = FALSE;
		    if (ifsp) off = ifstack[--ifsp] != ON;
		    else error('I');
		    break;

	case EQU:   if (label[0]) {
			if (pass == 1) {
			    if ((l = find_symbol(label))) /* AP autobr */
                                ;
			    else if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if ((l = find_symbol(label))) {
				l -> attr &= ~FORWD;
				address = expr();
				if (forwd) error('P');
				if (l -> valu != address) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case IF:    if (++ifsp == IFDEPTH) fatal_error(IFOFLOW);
		    address = expr();
		    if (forwd) { error('P');  address = TRUE; }
		    if (off) { listhex = FALSE;  ifstack[ifsp] = ZERO; } /* was NULL but error */
		    else {
			ifstack[ifsp] = address ? ON : OFF;
			if (!address) off = TRUE;
		    }
		    break;

	case LOAD:  do_label();  bytes = 6;
		    obj[0] = obj[3] = LDI;
		    if ((u = expr()) > 15) { u = 0;  error('R'); }
		    obj[2] = PHI + u;  obj[5] = PLO + u;
		    u = expr();
		    obj[1] = high(u);  obj[4] = low(u);
		    break;

	case INCL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == STR) {
			if (++filesp == FILES) fatal_error(FLOFLOW);
			if (!(filestk[filesp] = fopen(token.sval,"r"))) {
			    --filesp;  error('V');
			}
		    }
		    else error('S');
		    break;

	case ORG:   u = expr();
		    if (forwd) error('P');
		    else {
			pc = address = u;
			if (pass == 2) hseek(pc);
		    }
		    do_label();
		    break;

	case PAGE:  address = pc = (pc + 255) & 0xff00;
		    if (pass == 2) hseek(pc);
		    do_label();
		    break;

	case SET:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)
				|| (l -> attr & SOFT)) {
				l -> attr = FORWD + SOFT + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if ((l = find_symbol(label))) {
				address = expr();
				if (forwd) error('P');
				else if (l -> attr & SOFT) {
				    l -> attr &= ~FORWD;
				    l -> valu = address;
				}
				else error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case TEXT:  do_label(); DIAG(printf(" psop TEXT")); /* HRJ debug*/
		    while ((lex() -> attr & TYPE) != EOL) {
			if ((token.attr & TYPE) == STR) {
			    for (s = token.sval; *s; *o++ = *s++)
				++bytes;
			    if ((lex() -> attr & TYPE) != SEP) unlex();
			}
			else error('S');
			DIAG(printf(" s=%u",obj[0]));  /* hrj */
		    }
		    break;

	case TITL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error('S');
		    else strcpy(title,token.sval);
		    break;

	case WORD:  do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else { unlex();  u = expr(); }
			*o++ = high(u);  *o++ = low(u);
			bytes += 2;
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case CONST: do_label(); DIAG(printf(" psop CONST"));
		    do {
			ulen = 2;
			if ((lex() -> attr & TYPE) == SEP) {
			    DIAG(printf(" <SEP>"));
			    u = 0;
			}
			else if ((token.attr & TYPE) == STR) { /* strings */
			    DIAG(printf(" <STRING>"));
	    		    for (s = token.sval; *s; *o++ = *s++)
			    ++bytes;
			    if ((lex() -> attr & TYPE) != SEP) unlex();
			    continue;
			}
			else { /* expression */
			    DIAG(printf(" <EXPR>"));
			    unlex();
			    u = expr();
			    if (1 == evals()) { /* value? */
				if (isnum(token.sval[0]) || 
				    isnumprefix(token.sval[0]))
                                {
			            ulen = strlen(token.sval);
                                    if (token.sval[0] == '#')
                                        ulen = ulen / 2;
                                    else if (ulen > 4)
                                        ulen = 2;
                                    else
                                        ulen = 1;
				}
				else
				    ulen = 2;
			    }
			}
                        if (ulen > 2) {
                            DIAG(printf(" multi.B"));
                            for (i = 0; i < ulen; i++) {
                                *o++ = valbytes[i];
                                bytes++;
                            }
                        }
			else if (2 == ulen) {
			    DIAG(printf(" .W"));
			    *o++ = high(u);  *o++ = low(u);
			    bytes += 2;
			}
			else if (1 == ulen) {
			    DIAG(printf(" .B"));
			    *o++ = low(u);
			    bytes++;
			}
		    } while((token.attr & TYPE) == SEP);
		    break;
	case EXPSYM: do_label(); DIAG(printf(" psop EXPORT"));
		    do {
		        if ((lex() -> attr & TYPE) == VAL) {
			    if (isalph(token.sval[0])) {
			        if (pass == 1) {
				    if ((l = find_symbol(token.sval)))
				        l -> attr |= EXPO;
				    else if (!(l = new_symbol(token.sval)) -> attr)
					l -> attr = FORWD + EXPO + VAL;
				}
			    }
			    else {
			        error('P');
				break;
			    }
			}
			else {
			    error('P');
			    break;
			}
			if ((lex() -> attr & TYPE) != SEP) unlex();
		    } while((token.attr & TYPE) == SEP);
		    break;
    }
    return;
}

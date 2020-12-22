/*
	HEADER:		CUG149;
	TITLE:		1805A Cross-Assembler (Portable);
	FILENAME:	A18UTIL.C;
	VERSION:	2.5;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the RCA 1802, 1804, 1805, 1805A, 1806, and
			1806A microprocessors.  The program is written in
			portable C rather than BDS C.  All assembler features
			are supported except relocation, linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			RCA, CDP1802, CDP1805;

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

	2010		changes by Herb Johnson to compile under lcc-32 for MS-DOS.
			fixes as late as April 2010. Also see "a18octal.txt" to
			change listing from hexidecimal values to octal.

	Aug 2010	Fixes for Turbo C HRJ
	May 2017    binary out file HRJ
	Mar 2018	uppercase on hex address/code on listing HRJ Christensen
	Mar 2018   Added SCRT pseudo ops "CALL" and "RETN". L. Christensen.
	June 2018	Added / slash representations L. Christensen

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a18.h"
#include <string.h>
#include <ctype.h>
#ifndef __APPLE__
#include <malloc.h> /* for lcc-32 HRJ */
/* #include <alloc.h> for Turbo C HRJ */
#endif
#include <stdio.h>
#include <stdlib.h>

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

/* char *calloc(); */

/*HRJ local declarations */

static OPCODE *bccsearch(OPCODE *, OPCODE *, char *);
static void list_sym(SYMBOL *);
void fatal_error(char *);
static void check_page(void);
static void record(unsigned);
static void putb(unsigned);
static int ustrcmp(char *, char*);
void warning(char *);
static void binrecord(unsigned);
void bseek(unsigned);
void bputc(unsigned);
char *backsub(char *str);
static void resetout(void);

/*  Get access to global mailboxes defined in A18.C:			*/

extern char errcode, lline[], title[];
extern int eject, listhex, caphex, ut4mon, lineno;
extern unsigned address, line_bytes, errors, listleft, line_obj[], pagelen, pc;

/*  The symbol table is a binary tree of variable-length blocks drawn	*/
/*  from the heap with the calloc() function.  The root pointer lives	*/
/*  here:								*/

static SYMBOL *sroot = NULL;

/*  Add new symbol to symbol table.  Returns pointer to symbol even if	*/
/*  the symbol already exists.  If there's not enough memory to store	*/
/*  the new symbol, a fatal error occurs.				*/

SYMBOL *new_symbol_(nam,path,line)
char *nam;
char *path;
int line;
{
    SCRATCH int i;
    SCRATCH SYMBOL **p, *q;
    extern int (*symcmp)(const char*,const char*);

    DIAG(printf(" %s:%d new_symbol>>%s<<",path,line,nam));  /* HRJ diagnostic*/
    for (p = &sroot; (q = *p) && (i = symcmp(nam,q -> sname)); ) {
	p = i < 0 ? &(q -> left) : &(q -> right);
    }
    if (!q) {
	if (!(*p = q = (SYMBOL *)calloc(1,sizeof(SYMBOL) + strlen(nam))))
	    fatal_error(SYMBOLS);
	strcpy(q -> sname,nam);  /* copy nam to sname HRJ*/
    }
    if (q -> del) {
        q -> del  = 0;
        q -> attr = 0;
    }
    return q;
}

/*  Look up symbol in symbol table.  Returns pointer to symbol or NULL	*/
/*  if symbol not found.						*/

SYMBOL *find_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL *p;
    extern int (*symcmp)(const char*,const char*);

    DIAG(printf(" find_symbol>>%s<<",nam));  /* HRJ diagnostic*/
    for (p = sroot; p && (i = symcmp(nam,p -> sname));
	p = i < 0 ? p -> left : p -> right)
        ;
    DIAG(printf(" found=%p",p));
    if (p && p -> del)
        p = NULL;
    return p;
}

static void clear_symbols_(sp, adr)
SYMBOL *sp;
unsigned adr;
{
    if (sp) {
	clear_symbols_(sp -> left, adr);
        if (sp -> valu > adr) {
            sp -> del = 1;
        }
	clear_symbols_(sp -> right, adr);
    }
    return;
}

/* Clear all symbols after given address */
void clear_symbols(adr)
int adr;
{
    clear_symbols_(sroot, adr);
}

/*  Opcode table search routine.  This routine pats down the opcode	*/
/*  table for a given opcode and returns either a pointer to it or	*/
/*  NULL if the opcode doesn't exist.					*/

OPCODE *find_code(nam)
char *nam;
{
    /* OPCODE *bsearch();*/

    static OPCODE opctbl[] = {
	{ 1,					0x74,	"ADC"	},
	{ IMMED + 2,				0x7c,	"ADCI"	},
	{ 1,					0xf4,	"ADD"	},
	{ IMMED + 2,				0xfc,	"ADI"	},
	{ 1,					0xf2,	"AND"	},
	{ IMMED + 2,				0xfa,	"ANI"	},
	{ BRANCH + 2,				0x34,	"B1"	},
	{ BRANCH + 2,				0x35,	"B2"	},
	{ BRANCH + 2,				0x36,	"B3"	},
	{ BRANCH + 2,				0x37,	"B4"	},
	{ IS1805 + BRANCH + 3,			0x3e,	"BCI"	},
	{ BRANCH + 2,				0x33,	"BDF"	},
	{ BRANCH + 2,				0x33,	"BGE"	},
	{ BRANCH + 2,				0x3b,	"BL"	},
	{ PSEUDO,				BLK,	"BLK"	},
	{ BRANCH + 2,				0x3b,	"BM"	},
	{ BRANCH + 2,				0x3c,	"BN1"	},
	{ BRANCH + 2,				0x3d,	"BN2"	},
	{ BRANCH + 2,				0x3e,	"BN3"	},
	{ BRANCH + 2,				0x3f,	"BN4"	},
	{ BRANCH + 2,				0x3b,	"BNF"	},
	{ BRANCH + 2,				0x39,	"BNQ"	},
	{ BRANCH + 2,				0x3a,	"BNZ"	},
	{ BRANCH + 2,				0x33,	"BPZ"	},
	{ BRANCH + 2,				0x31,	"BQ"	},
	{ BRANCH + 2,				0x30,	"BR"	},
	{ IS1805 + BRANCH + 3,			0x3f,	"BXI"	},
	{ PSEUDO,				BYTE,	"BYTE"	},
	{ BRANCH + 2,				0x32,	"BZ"	},
	{ PSEUDO,				CALL,	"CALL"	}, /* loren: SCRT */
	{ IS1805 + 2,				0x0d,	"CID"	},
	{ IS1805 + 2,				0x0c,	"CIE"	},
	{ PSEUDO,				CONST,  "CONST" }, /* CONST */
	{ PSEUDO,				CPU,	"CPU"	},
	{ IS1805 + IMMED + 3,			0x7c,	"DACI"	},
	{ IS1805 + 2,				0x74,	"DADC"	},
	{ IS1805 + 2,				0xf4,	"DADD"	},
	{ IS1805 + IMMED + 3,			0xfc,	"DADI"	},
	{ PSEUDO,				BYTE,	"DB"	}, /* HRJ */
	{ IS1805 + ANY + SIXTN + 4,		0x20,	"DBNZ"	},
	{ PSEUDO,				CONST,  "DC"    }, /* AP */
	{ ANY + 1,				0x20,	"DEC"	},
	{ 1,					0x71,	"DIS"	},
	{ PSEUDO,				BLK,	"DS"	}, /* AP */
	{ IS1805 + 2,				0x76,	"DSAV"	},
	{ IS1805 + IMMED + 3,			0x7f,	"DSBI"	},
	{ IS1805 + 2,				0xf7,	"DSM"	},
	{ IS1805 + 2,				0x77,	"DSMB"	},
	{ IS1805 + IMMED + 3,			0xff,	"DSMI"	},
	{ IS1805 + 2,				0x01,	"DTC"	},
	{ PSEUDO,				WORD,	"DW"	},
	{ PSEUDO,				EJCT,	"EJCT"	},
	{ PSEUDO + ISIF,			ELSE,	"ELSE"	},
	{ PSEUDO,				END,	"END"	},
	{ PSEUDO + ISIF,			ENDI,	"ENDI"	},
	{ PSEUDO,				EQU,	"EQU"	},
	{ IS1805 + 2,				0x09,	"ETQ"	},
	{ PSEUDO,				EXPSYM, "EXPORT"}, /* AP */
	{ IS1805 + 2,				0x08,	"GEC"	},
	{ ANY + 1,				0x90,	"GHI"	},
	{ ANY + 1,				0x80,	"GLO"	},
	{ 1,					0x00,	"IDL"	},
	{ PSEUDO + ISIF,			IF,	"IF"	},
	{ ANY + 1,				0x10,	"INC"	},
	{ PSEUDO,				INCL,	"INCL"	},
	{ IOPORT + 1,				0x68,	"INP"	},
	{ 1,					0x60,	"IRX"	},
	{ SIXTN + 3,				0xc3,	"LBDF"	},
	{ SIXTN + 3,				0xcb,	"LBNF"	},
	{ SIXTN + 3,				0xc9,	"LBNQ"	},
	{ SIXTN + 3,				0xca,	"LBNZ"	},
	{ SIXTN + 3,				0xc1,	"LBQ"	},
	{ SIXTN + 3,				0xc0,	"LBR"	},
	{ SIXTN + 3,				0xc2,	"LBZ"	},
	{ ANY + 1,				0x40,	"LDA"	},
	{ IS1805 + 2,				0x06,	"LDC"	},
	{ IMMED + 2,				0xf8,	"LDI"	},
	{ NOT_R0 + 1,				0x00,	"LDN"	},
	{ 1,					0xf0,	"LDX"	},
	{ 1,					0x72,	"LDXA"	},
	{ PSEUDO,				LOAD,	"LOAD"	},
	{ 1,					0xcf,	"LSDF"	},
	{ 1,					0xcc,	"LSIE"	},
	{ 1,					0xc8,	"LSKP"	},
	{ 1,					0xc7,	"LSNF"	},
	{ 1,					0xc5,	"LSNQ"	},
	{ 1,					0xc6,	"LSNZ"	},
	{ 1,					0xcd,	"LSQ"	},
	{ 1,					0xce,	"LSZ"	},
	{ 1,					0x79,	"MARK"	},
	{ BRANCH + 2,				0x38,	"NBR"	},
	{ SIXTN + 3,				0xc8,	"NLBR"	},
	{ 1,					0xc4,	"NOP"	},
	{ 1,					0xf1,	"OR"	},
	{ PSEUDO,				ORG,	"ORG"	},
	{ IMMED + 2,				0xf9,	"ORI"	},
	{ IOPORT + 1,				0x60,	"OUT"	},
	{ PSEUDO,				PAGE,	"PAGE"	},
	{ ANY + 1,				0xb0,	"PHI"	},
	{ ANY + 1,				0xa0,	"PLO"	},
	{ 1,					0x7a,	"REQ"	},
	{ 1,					0x70,	"RET"	},
	{ PSEUDO,				RETN,	"RETN"	}, /* loren: SCRT */
	{ IS1805 + ANY + SIXTN + 4,		0xc0,	"RLDI"	},
	{ IS1805 + ANY + 2,			0x60,	"RLXA"	},
	{ IS1805 + ANY + 2,			0xb0,	"RNX"	},
	{ 1,					0x7e,	"RSHL"	},
	{ 1,					0x76,	"RSHR"	},
	{ IS1805 + ANY + 2,			0xa0,	"RSXD"	},
	{ 1,					0x78,	"SAV"	},
	{ IS1805 + ANY + SIXTN + 4,		0x80,	"SCAL"	},
	{ IS1805 + 2,				0x05,	"SCM1"	},
	{ IS1805 + 2,				0x03,	"SCM2"	},
	{ 1,					0xf5,	"SD"	},
	{ 1,					0x75,	"SDB"	},
	{ IMMED + 2,				0x7d,	"SDBI"	},
	{ IMMED + 2,				0xfd,	"SDI"	},
	{ ANY + 1,				0xd0,	"SEP"	},
	{ 1,					0x7b,	"SEQ"	},
	{ PSEUDO,				SET,	"SET"	},
	{ ANY + 1,				0xe0,	"SEX"	},
	{ 1,					0xfe,	"SHL"	},
	{ 1,					0x7e,	"SHLC"	},
	{ 1,					0xf6,	"SHR"	},
	{ 1,					0x76,	"SHRC"	},
	{ 1,					0x38,	"SKP"	},
	{ 1,					0xf7,	"SM"	},
	{ 1,					0x77,	"SMB"	},
	{ IMMED + 2,				0x7f,	"SMBI"	},
	{ IMMED + 2,				0xff,	"SMI"	},
	{ IS1805 + 2,				0x04,	"SPM1"	},
	{ IS1805 + 2,				0x02,	"SPM2"	},
	{ IS1805 + ANY + 2,			0x90,	"SRET"	},
	{ IS1805 + 2,				0x07,	"STM"	},
	{ IS1805 + 2,				0x00,	"STPC"	},
	{ ANY + 1,				0x50,	"STR"	},
	{ 1,					0x73,	"STXD"	},
	{ PSEUDO,				TEXT,	"TEXT"	},
	{ PSEUDO,				TITL,	"TITL"	},
	{ PSEUDO,				WORD,	"WORD"	},
	{ IS1805 + 2,				0x0b,	"XID"	},
	{ IS1805 + 2,				0x0a,	"XIE"	},
	{ 1,					0xf3,	"XOR"	},
	{ IMMED + 2,				0xfb,	"XRI"	}
    };

    return bccsearch(opctbl,opctbl + (sizeof(opctbl) / sizeof(OPCODE)),nam);
}

/*  Operator table search routine.  This routine pats down the		*/
/*  operator table for a given operator and returns either a pointer	*/
/*  to it or NULL if the opcode doesn't exist.				*/

OPCODE *find_operator(nam)
char *nam;
{
    /* OPCODE *bsearch();*/

    static OPCODE oprtbl[] = {
	{ UNARY  + UOP1  + OPR,		'$',		"$"	},
	{ UNARY  + UOP1  + OPR,		'$',		"*"	}, /* AP */
	{ UNARY  + UOP3  + OPR,		'A',		"A"	}, /* AP */
	{ UNARY  + UOP3  + OPR,		LOW,		"A.0"	}, /* AP */
	{ UNARY  + UOP3  + OPR,		HIGH,		"A.1"	}, /* AP */
	{ BINARY + LOG1  + OPR,		AND,		"AND"	},
	{ BINARY + RELAT + OPR,		'=',		"EQ"	},
	{ BINARY + RELAT + OPR,		GE,		"GE"	},
	{ BINARY + RELAT + OPR,		'>',		"GT"	},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"	},
	{ BINARY + RELAT + OPR,		LE,		"LE"	},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"	},
	{ BINARY + RELAT + OPR,		'<',		"LT"	},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"	},
	{ BINARY + RELAT + OPR,		NE,		"NE"	},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"	},
	{ BINARY + LOG2  + OPR,		OR,		"OR"	},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"	},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	}
    };

    return bccsearch(oprtbl,oprtbl + (sizeof(oprtbl) / sizeof(OPCODE)),nam);
}

static int ustrcmp(s,t)
char *s, *t;
{
    SCRATCH int i;

    while (!(i = toupper(*s++) - toupper(*t)) && *t++);
    return i;
}

static OPCODE *bccsearch(OPCODE *lo, OPCODE *hi, char *nam)
{
    SCRATCH int i;
    SCRATCH OPCODE *chk;

    for (;;) {
	chk = lo + (hi - lo) / 2;
	if (!(i = ustrcmp(chk -> oname,nam))) return chk;
	if (chk == lo) return NULL;
	if (i < 0) lo = chk;
	else hi = chk;
    };
}


/*  Buffer storage for line listing routine.  This allows the listing	*/
/*  output routines to do all operations without the main routine	*/
/*  having to fool with it.						*/

static FILE *list = NULL;

/*  Listing file open routine.  If a listing file is already open, a	*/
/*  warning occurs.  If the listing file doesn't open correctly, a	*/
/*  fatal error occurs.  If no listing file is open, all calls to	*/
/*  lputs() and lclose() have no effect.				*/

void lopen(nam)
char *nam;
{


    if (list) warning(TWOLST);
    else if (!(list = fopen(nam,"w"))) fatal_error(LSTOPEN);
    if (list && ut4mon)
        fprintf(list,"!M\n");
    return;
}

/*  Listing file line output routine.  This routine processes the	*/
/*  source line saved by popc() and the output of the line assembler in	*/
/*  buffer obj into a line of the listing.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

/*  L. Christensen, added external toggle for hex listing in capitals.
                    (extern int caphex) */

void lputs()
{
    SCRATCH int i, j, monout;
    SCRATCH unsigned *o;

    monout = ut4mon;
    if (list) {
	i = line_bytes;  o = line_obj;
	do {
	    if (!monout || (monout && (errcode - 32)))
	        fprintf(list,"%c  ",errcode);
	    if (listhex || monout) {
		fprintf(list,(caphex ? "%04X " : "%04x "), address /* pc - line_bytes */);
		if (!monout) fprintf(list, " ");
		for (j = monout ? 2*BIGINST : BIGINST; j; --j) {
		    if (i) {
                        --i;  ++address;
			if (monout)
                            fprintf(list,"%02X",*o++);
			else
                            fprintf(list,(caphex ? " %02X" : " %02x"), *o++);
	            }
		    else {
                        if (monout) {
                            fprintf(list,"; ");
                            monout = 0;
                        }
                        else
                            fprintf(list,(ut4mon ? "  " : "   "));
                    }
		}
		if (ut4mon) {
		    fprintf(list,"%c ", (monout ? ';' : ' '));
		    fprintf(list," %04d",lineno);
		}
	    }
	    else fprintf(list,"%18s","");
	    fprintf(list,"   %s",lline);  strcpy(lline,"\n");
	    check_page();
	    if (ferror(list)) fatal_error(DSKFULL);
	} while (listhex && i);
    }
    return;
}

/*  Listing file close routine.  THe symbol table is appended to the	*/
/*  listing in alphabetic order by symbol name, and the listing file is	*/
/*  closed.  If the disk fills up, a fatal error occurs.		*/

static int col = 0;

void lclose()
{

    if (list) {
	if (ut4mon)
	    fprintf(list,"0000\n");
	else if (sroot) {
	    list_sym(sroot);
	    if (col) fprintf(list,"\n");
	}
	if (!ut4mon) fprintf(list,"\f");
	if (ferror(list) || fclose(list) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void list_sym(sp)
SYMBOL *sp;
{

    if (sp) {
	list_sym(sp -> left);
        if (0 == sp -> del) {
	    fprintf(list,(caphex ? "%04X  " : "%04x  "),sp -> valu);
	    fprintf(list,"%-10s",sp -> sname);
	    if ((col = ++col % SYMCOLS)) fprintf(list,"    ");
	    else {
	        fprintf(list,"\n");
	        if (sp -> right) check_page();
	    }
        }
	list_sym(sp -> right);
    }
    return;
}

static FILE *expsym = NULL;

/*  Export symbol file open routine.  If an export file is already 	*/
/*  open, a warning occurs.  If the export file doesn't open correctly	*/
/*  a fatal error occurs.  If no export file is open, all calls to	*/
/*   xclose() have no effect.				        	*/

void xopen(nam)
char *nam;
{
    if (expsym) warning(TWOEXP);
    else if (!(expsym = fopen(nam,"w"))) fatal_error(EXPOPEN);
    return;
}

static void exp_sym(sp)
SYMBOL *sp;
{
    char buf[16];
    int isalph(char);

    if (sp) {
	exp_sym(sp -> left);
	if (0 == sp -> del && sp -> attr & EXPO) {
	    fprintf(expsym, "%-10s EQU ",sp -> sname);
	    sprintf(buf,(caphex ? "%XH" : "%xH"),sp -> valu);
	    fprintf(expsym, isalph(buf[0]) ? "0%s" : "%s", buf);
	    fprintf(expsym,"\n");
	}
	exp_sym(sp -> right);
    }
    return;
}

/*  Export file close routine.  The symbol table is appended to the	*/
/*  export in alphabetic order by symbol name, and the export file is	*/
/*  closed.  If the disk fills up, a fatal error occurs.		*/

void xclose()
{

    if (expsym) {
	if (sroot)
	    exp_sym(sroot);
	if (ferror(expsym) || fclose(expsym) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void check_page()
{
    if (pagelen && !--listleft) eject = TRUE;
    if (eject) {
	eject = FALSE;  listleft = pagelen;  fprintf(list,"\f");
	if (title[0]) { fprintf(list,"%s\n\n",title);  listleft -= 2; }
    }
    return;
}

/* binary file support HRJ May 2017 */

/*  Buffer storage for binary output file.  This allows this module to	*/
/*  do all of the required buffering and record forming without the	*/
/*  main routine having to fool with it. Based on hex file support, HRJ */

static FILE *bin = NULL;
static unsigned bincnt = 0;
static unsigned binaddr = 0, binstart = 0;
static unsigned binbuf[BINSIZE+1];

/*  binary file open routine.  If file is already open, a warning	*/
/*  occurs.  If the  file doesn't open correctly, a fatal error	*/
/*  occurs.  If no bin file is open, all calls to bputc(), bseek(), and	*/
/*  bclose() have no effect.						*/

void bopen(nam)
char *nam;
{

    if (bin) warning(TWOBIN);
    else if (!(bin = fopen(nam,"wb"))) fatal_error(BINOPEN);
    return;
}

/*  Binary file write routine.  The data byte is appended to the current	*/
/*  record.  If the record fills up, it gets written to disk.  If the	*/
/*  disk fills up, a fatal error occurs.				*/

void bputc(c)
unsigned c;
{

    if (bin) {
	binbuf[bincnt++] = c;
	if (bincnt == BINSIZE) binrecord(0);
    }
    return;
}

/*  Binary file address set routine. If a record is currently open,	*/
/*  it gets written to disk.  If the disk fills up, a fatal error	*/
/*  occurs.	If the new addr a is not the current binaddress, then we	*/
/*  fill the range with 0...unless it's the start where the binaddress is 0. */
/*  In which case, we print a warning that the binary doesn't start at 0 */

void bseek(a)
unsigned a;
{
    extern unsigned filler;
    int i;
    if (bin) {
        if (bincnt) binrecord(0);
        if (a < binaddr) { /* can't run binary backwards ! */
            if (a == binstart)
                resetout();
            else {
                printf("binary file address change from %4X to %4X, cant fix this!\n",binaddr, a);
                fatal_error(BINNEG);
            }
        }
        if ((binaddr == 0) && (a != 0)){
            if (a != binstart) {
                printf("binary file address starts at %4X\n",a);
                binstart = a;
            }
            binaddr = a;
        }
        else if (a != binaddr) {
            if (a - binaddr > 1)
                printf("binary file address change from %4X to %4X, fill with %02X!\n",binaddr,a,filler);
            for (i = binaddr; i < a; bputc(filler), i++);
        }
    }

    return;
}

/*  Binary file close routine.  Any open record is written to disk, the	*/
/*  EOF record is added, and file is closed.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void bclose()
{

    if (bin) {
	  if (bincnt) binrecord(0);
	  if (fclose(bin) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void binrecord(typ)
unsigned typ;
{
    SCRATCH unsigned i;
    /* HRJ typ not really used */

    for (i = 0; i < bincnt; ++i) putc(binbuf[i],bin);

    binaddr += bincnt;  bincnt = 0;

    if (ferror(bin)) fatal_error(DSKFULL);
    return;
}

/* also look at hseek,  binary file support HRJ */

/* end binary file support */


/*  Buffer storage for hex output file.  This allows this module to	*/
/*  do all of the required buffering and record forming without the	*/
/*  main routine having to fool with it.				*/

static FILE *hex = NULL;
static unsigned cnt = 0;
static unsigned addr = 0;
static unsigned sum = 0;
static unsigned buf[HEXSIZE];

/*  Hex file open routine.  If a hex file is already open, a warning	*/
/*  occurs.  If the hex file doesn't open correctly, a fatal error	*/
/*  occurs.  If no hex file is open, all calls to hputc(), hseek(), and	*/
/*  hclose() have no effect.						*/

void hopen(nam)
char *nam;
{

    if (hex) warning(TWOHEX);
    else if (!(hex = fopen(nam,"w"))) fatal_error(HEXOPEN);
    return;
}

/*  Hex file write routine.  The data byte is appended to the current	*/
/*  record.  If the record fills up, it gets written to disk.  If the	*/
/*  disk fills up, a fatal error occurs.				*/

void hputc(c)
unsigned c;
{

    if (hex) {
	buf[cnt++] = c;
	if (cnt == HEXSIZE) record(0);
    }
    return;
}

/*  Hex & binary file address set routine.  The specified address becomes the	*/
/*  load address of the next record.  If a record is currently open,	*/
/*  it gets written to disk.  If the disk fills up, a fatal error	*/
/*  occurs.								*/

void hseek(a)
unsigned a;
{
    if (hex) {
	if (cnt) record(0);
	addr = a;
    }
	bseek(a); /* if binary file, process likewise */

    return;
}

/*  Hex file close routine.  Any open record is written to disk, the	*/
/*  EOF record is added, and file is closed.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void hclose()
{

    if (hex) {
	if (cnt) record(0);
	record(1);
	if (fclose(hex) == EOF) fatal_error(DSKFULL);
    }
    return;
}

/* typ is either data record 0 or end record 1 */
static void record(typ)
unsigned typ;
{
    SCRATCH unsigned i;


    putc(':',hex);  putb(cnt);  putb(high(addr));
    putb(low(addr));  putb(typ);
    for (i = 0; i < cnt; ++i) putb(buf[i]);
    putb(low(0-sum));  putc('\n',hex); /* was (-sum) HRJ*/
	/*note: putb() of checksum will "clear" sum but safer to do it literally - HRJ*/
    addr += cnt;  cnt = 0; sum = 0;

    if (ferror(hex)) fatal_error(DSKFULL);
    return;
}

static void putb(b)
unsigned b;
{
    static char digit[] = "0123456789ABCDEF";

    putc(digit[b >> 4],hex);
    putc(digit[b & 0x0f],hex);
    sum += b;  return;
}

/*  Reset output files. */
static void resetout(void)
{
    if (bin) {
        fseek(bin, 0, SEEK_SET);
        binaddr = 0;
    }
    if (list) fseek(list, 0, SEEK_SET);
    if (hex) {
        fseek(hex, 0, SEEK_SET);
        cnt = 0;
    }
}

/*  Error handler routine.  If the current error code is non-blank,	*/
/*  the error code is filled in and the	number of lines with errors	*/
/*  is adjusted.							*/

void error_(char code,char *path,int line)
{
    if (errcode == ' ') { errcode = code;  ++errors; }
    DIAG(printf("%s:%d: error '%c' at line %d\n",path,line,code,lineno));
    return;
}

/*  Fatal error handler routine.  A message gets printed on the stderr	*/
/*  device, and the program bombs.					*/

void fatal_error(msg)
char *msg;
{
    printf("Fatal Error -- %s\n",msg);
    exit(-1);
}

/*  Non-fatal error handler routine.  A message gets printed on the	*/
/*  stderr device, and the routine returns.				*/

void warning(msg)
char *msg;
{
    printf("Warning -- %s\n",msg);
    return;
}

/*
 * loren:  string backslash substitution.
 *
 *	supported escape sequences;
 *	 \", \', \a, \b, \f, \n, \r, \t, \v, \nnn, \Xnn, and \xnn
 *
 */
char *backsub(char *str) {

	char *in, *out;
	char *subbed = malloc(strlen(str) + 1);

	for(in = str, out = subbed; *in; in++) {

		if(*in == '\\') {
			// do backslash escapes.
			char c;

			switch (c = *++in) {
				case '\"':
					*out++ = '\"';
					break;
				case '\'':
					*out++ = '\'';
					break;
				case '\\':
					*out++ = '\\';
					break;
				case 'a':
					*out++ = '\a';
					break;
				case 'b':
					*out++ = '\b';
					break;
				case 'f':
					*out++ = '\f';
					break;
				case 'n':
					*out++ = '\n';
					break;
				case 'r':
					*out++ = '\r';
					break;
				case 't':
					*out++ = '\t';
					break;
				case 'v':
					*out++ = '\v';
					break;
				//case '0': *out++ = 0;	break; won't work, null is terminator!
				default:
				{
					int v, n;
					// should be a numerical value, check for octal format.
					if(isdigit(c) && isdigit(*(in + 1)) && isdigit(*(in + 2))) {
						sscanf(in, "%o", &v);
						*out++ = (char)v;
						in += 2;
					// check for hex.
					} else if(((c == 'x') || (c == 'X')) &&
								isxdigit(*(in + 1)) &&
								isxdigit(*(in + 2))) {
						++in;
						sscanf(in, "%x", &v);
						*out++ = (char)v;
						in += 1;
					} else {
						// bad format spec, just treat as literal.
						error('\\');
						*out++ = c;
					}	// if numerical value.
				}	// default.
			}	// switch *in.

		} else {
			// character not escaped, just copy it over.
			*out++ = *in;
		}	// if backslashed.
	}	// for in, out.

	// null terminate the string.
	*out = '\x00';

	return(subbed);

}	// backsub.

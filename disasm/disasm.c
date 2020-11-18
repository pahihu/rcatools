#include <stdio.h>
#include <string.h>

#define NONE	0
#define SHORTBR 1
#define LONGBR  2
#define ABSVAL  3
#define CONTVAL 4
#define IGNORE	5
#define END	6
#define LONGSKIP 7

#define EPROMSIZE	36000

unsigned char data[EPROMSIZE+1];

struct opcode {
	unsigned char opcode;
	char mnemonic[16];
	int argc;
	int argt;
	char desc[128];
};

static unsigned char cdp;

static struct opcode x68[] = {
{ 0x68, "INVALID", 0, NONE, "INVALID INSTRUCTION"},
{ 0x68, "INVALID", 0, NONE, "INVALID INSTRUCTION"},
{ 0x68, "INVALID", 0, NONE, "INVALID INSTRUCTION"},
{ 0x68, "INVALID", 0, NONE, "INVALID INSTRUCTION"},
{ 0x68, "INVALID", 0, NONE, "INVALID INSTRUCTION"},
{ 0x68, "DOUBLEFETCH", 1, NONE, "Opcode for double fetched instructions"},
{ 0x68, "DOUBLEFETCH", 1, NONE, "Opcode for double fetched instructions"},
};


static struct opcode opcodes[256]={
{ 0x00, "IDL", 0 , END ,"Idle or wait for interrupt or DMA request"},
{ 0x01, "LDN R1", 0 , NONE ,"Load D with (R1)"},
{ 0x02, "LDN R2", 0 , NONE ,"Load D with (R2)"},
{ 0x03, "LDN R3", 0 , NONE ,"Load D with (R3)"},
{ 0x04, "LDN R4", 0 , NONE ,"Load D with (R4)"},
{ 0x05, "LDN R5", 0 , NONE ,"Load D with (R5)"},
{ 0x06, "LDN R6", 0 , NONE ,"Load D with (R6)"},
{ 0x07, "LDN R7", 0 , NONE ,"Load D with (R7)"},
{ 0x08, "LDN R8", 0 , NONE ,"Load D with (R8)"},
{ 0x09, "LDN R9", 0 , NONE ,"Load D with (R9)"},
{ 0x0a, "LDN RA", 0 , NONE ,"Load D with (RA)"},
{ 0x0b, "LDN RB", 0 , NONE ,"Load D with (RB)"},
{ 0x0c, "LDN RC", 0 , NONE ,"Load D with (RC)"},
{ 0x0d, "LDN RD", 0 , NONE ,"Load D with (RD)"},
{ 0x0e, "LDN RE", 0 , NONE ,"Load D with (RE)"},
{ 0x0f, "LDN RF", 0 , NONE ,"Load D with (RF)"},

{ 0x10, "INC R0", 0 , NONE ,"Increment (R0)"},
{ 0x11, "INC R1", 0 , NONE ,"Increment (R1)"},
{ 0x12, "INC R2", 0 , NONE ,"Increment (R2)"},
{ 0x13, "INC R3", 0 , NONE ,"Increment (R3)"},
{ 0x14, "INC R4", 0 , NONE ,"Increment (R4)"},
{ 0x15, "INC R5", 0 , NONE ,"Increment (R5)"},
{ 0x16, "INC R6", 0 , NONE ,"Increment (R6)"},
{ 0x17, "INC R7", 0 , NONE ,"Increment (R7)"},
{ 0x18, "INC R8", 0 , NONE ,"Increment (R8)"},
{ 0x19, "INC R9", 0 , NONE ,"Increment (R9)"},
{ 0x1a, "INC RA", 0 , NONE ,"Increment (RA)"},
{ 0x1b, "INC RB", 0 , NONE ,"Increment (RB)"},
{ 0x1c, "INC RC", 0 , NONE ,"Increment (RC)"},
{ 0x1d, "INC RD", 0 , NONE ,"Increment (RD)"},
{ 0x1e, "INC RE", 0 , NONE ,"Increment (RE)"},
{ 0x1f, "INC RF", 0 , NONE ,"Increment (RF)"},

{ 0x20, "DEC R0", 0 , NONE ,"Decrement (R0)"},
{ 0x21, "DEC R1", 0 , NONE ,"Decrement (R1)"},
{ 0x22, "DEC R2", 0 , NONE ,"Decrement (R2)"},
{ 0x23, "DEC R3", 0 , NONE ,"Decrement (R3)"},
{ 0x24, "DEC R4", 0 , NONE ,"Decrement (R4)"},
{ 0x25, "DEC R5", 0 , NONE ,"Decrement (R5)"},
{ 0x26, "DEC R6", 0 , NONE ,"Decrement (R6)"},
{ 0x27, "DEC R7", 0 , NONE ,"Decrement (R7)"},
{ 0x28, "DEC R8", 0 , NONE ,"Decrement (R8)"},
{ 0x29, "DEC R9", 0 , NONE ,"Decrement (R9)"},
{ 0x2a, "DEC RA", 0 , NONE ,"Decrement (RA)"},
{ 0x2b, "DEC RB", 0 , NONE ,"Decrement (RB)"},
{ 0x2c, "DEC RC", 0 , NONE ,"Decrement (RC)"},
{ 0x2d, "DEC RD", 0 , NONE ,"Decrement (RD)"},
{ 0x2e, "DEC RE", 0 , NONE ,"Decrement (RE)"},
{ 0x2f, "DEC RF", 0 , NONE ,"Decrement (RF)"},

{ 0x30, "BR ", 1 ,SHORTBR, "Short branch"},
{ 0x31, "BQ ", 1 ,SHORTBR, "Short branch on Q=1"},
{ 0x32, "BZ ", 1 ,SHORTBR, "Short branch on D=0"},
{ 0x33, "BDF", 1 ,SHORTBR, "Short branch on DF=1"},
{ 0x34, "B1 ", 1 ,SHORTBR, "Short branch on EF1=1"},
{ 0x35, "B2 ", 1 ,SHORTBR, "Short branch on EF2=1"},
{ 0x36, "B3 ", 1 ,SHORTBR, "Short branch on EF3=1"},
{ 0x37, "B4 ", 1 ,SHORTBR, "Short branch on EF4=1"},
{ 0x38, "SKP", 1 ,IGNORE, "Skip next byte"},
{ 0x39, "BNQ", 1 ,SHORTBR, "Short branch on Q=0"},
{ 0x3a, "BNZ", 1 ,SHORTBR, "Short branch on D!=0"},
{ 0x3b, "BNF", 1 ,SHORTBR, "Short branch on DF=0"},
{ 0x3c, "BN1", 1 ,SHORTBR, "Short branch on EF1=0"},
{ 0x3d, "BN2", 1 ,SHORTBR, "Short branch on EF2=0"},
{ 0x3e, "BN3", 1 ,SHORTBR, "Short branch on EF3=0"},
{ 0x3f, "BN4", 1 ,SHORTBR, "Short branch on EF4=0"},

{ 0x40, "LDA R0", 0, NONE, "Load D from (R0), increment R0"},
{ 0x41, "LDA R1", 0, NONE, "Load D from (R1), increment R1"},
{ 0x42, "LDA R2", 0, NONE, "Load D from (R2), increment R2"},
{ 0x43, "LDA R3", 0, NONE, "Load D from (R3), increment R3"},
{ 0x44, "LDA R4", 0, NONE, "Load D from (R4), increment R4"},
{ 0x45, "LDA R5", 0, NONE, "Load D from (R5), increment R5"},
{ 0x46, "LDA R6", 0, NONE, "Load D from (R6), increment R6"},
{ 0x47, "LDA R7", 0, NONE, "Load D from (R7), increment R7"},
{ 0x48, "LDA R8", 0, NONE, "Load D from (R8), increment R8"},
{ 0x49, "LDA R9", 0, NONE, "Load D from (R9), increment R9"},
{ 0x4a, "LDA RA", 0, NONE, "Load D from (RA), increment RA"},
{ 0x4b, "LDA RB", 0, NONE, "Load D from (RB), increment RB"},
{ 0x4c, "LDA RC", 0, NONE, "Load D from (RC), increment RC"},
{ 0x4d, "LDA RD", 0, NONE, "Load D from (RD), increment RD"},
{ 0x4e, "LDA RE", 0, NONE, "Load D from (RE), increment RE"},
{ 0x4f, "LDA RF", 0, NONE, "Load D from (RF), increment RF"},

{ 0x50, "STR R0", 0, NONE, "Store D to (R0)"},
{ 0x51, "STR R1", 0, NONE, "Store D to (R1)"},
{ 0x52, "STR R2", 0, NONE, "Store D to (R2)"},
{ 0x53, "STR R3", 0, NONE, "Store D to (R3)"},
{ 0x54, "STR R4", 0, NONE, "Store D to (R4)"},
{ 0x55, "STR R5", 0, NONE, "Store D to (R5)"},
{ 0x56, "STR R6", 0, NONE, "Store D to (R6)"},
{ 0x57, "STR R7", 0, NONE, "Store D to (R7)"},
{ 0x58, "STR R8", 0, NONE, "Store D to (R8)"},
{ 0x59, "STR R9", 0, NONE, "Store D to (R9)"},
{ 0x5a, "STR RA", 0, NONE, "Store D to (RA)"},
{ 0x5b, "STR RB", 0, NONE, "Store D to (RB)"},
{ 0x5c, "STR RC", 0, NONE, "Store D to (RC)"},
{ 0x5d, "STR RD", 0, NONE, "Store D to (RD)"},
{ 0x5e, "STR RE", 0, NONE, "Store D to (RE)"},
{ 0x5f, "STR RF", 0, NONE, "Store D to (RF)"},

{ 0x60, "IRX", 0, NONE, "Increment register X"},
{ 0x61, "OUT 1", 0, NONE, "Output (R(X)); Increment R(X), N=001"},
{ 0x62, "OUT 2", 0, NONE, "Output (R(X)); Increment R(X), N=010"},
{ 0x63, "OUT 3", 0, NONE, "Output (R(X)); Increment R(X), N=011"},
{ 0x64, "OUT 4", 0, NONE, "Output (R(X)); Increment R(X), N=100"},
{ 0x65, "OUT 5", 0, NONE, "Output (R(X)); Increment R(X), N=101"},
{ 0x66, "OUT 6", 0, NONE, "Output (R(X)); Increment R(X), N=110"},
{ 0x67, "OUT 7", 0, NONE, "Output (R(X)); Increment R(X), N=111"},
{ 0x68, "DOUBLEFETCH", 1, NONE, "Opcode for double fetched instructions"},
{ 0x69, "INP 1", 0, NONE, "Input to (R(X)) and D, N=001"},
{ 0x6a, "INP 2", 0, NONE, "Input to (R(X)) and D, N=010"},
{ 0x6b, "INP 3", 0, NONE, "Input to (R(X)) and D, N=011"},
{ 0x6c, "INP 4", 0, NONE, "Input to (R(X)) and D, N=100"},
{ 0x6d, "INP 5", 0, NONE, "Input to (R(X)) and D, N=101"},
{ 0x6e, "INP 6", 0, NONE, "Input to (R(X)) and D, N=110"},
{ 0x6f, "INP 7", 0, NONE, "Input to (R(X)) and D, N=111"},

{ 0x70, "RET", 0, END, "Return from interrupt, set IE=1"},
{ 0x71, "DIS", 0, END, "Disable. Return from interrupt, set IE=0"},
{ 0x72, "LDXA", 0, NONE, "Load via X and advance"},
{ 0x73, "STXD", 0, NONE, "Store via X and decrement"},
{ 0x74, "ADC", 0, NONE, "Add with carry"},
{ 0x75, "SDB", 0, NONE, "Substract D with borrow"},
{ 0x76, "SHRC", 0, NONE, "Shift right with carry"},
{ 0x77, "SMB", 0, NONE, "Substract memory with borrow"},
{ 0x78, "SAVE", 0, NONE, "Save"},
{ 0x79, "MARK", 0, NONE, "Push X,P; mark subroutine call"},
{ 0x7a, "REQ", 0, NONE, "Reset Q=0"},
{ 0x7b, "SEQ", 0, NONE, "Set Q=1"},
{ 0x7c, "ADCI", 1, ABSVAL, "Add with carry immediate"},
{ 0x7d, "SDBI", 1, ABSVAL, "Substract D with borrow immediate"},
{ 0x7e, "SHLC", 0, NONE, "Shift left with carry"},
{ 0x7f, "SMBI", 0, NONE, "Substract memory toh borrow, immediate"},

{ 0x80, "GLO R0", 0, NONE, "Get low register R0"},
{ 0x81, "GLO R1", 0, NONE, "Get low register R1"},
{ 0x82, "GLO R2", 0, NONE, "Get low register R2"},
{ 0x83, "GLO R3", 0, NONE, "Get low register R3"},
{ 0x84, "GLO R4", 0, NONE, "Get low register R4"},
{ 0x85, "GLO R5", 0, NONE, "Get low register R5"},
{ 0x86, "GLO R6", 0, NONE, "Get low register R6"},
{ 0x87, "GLO R7", 0, NONE, "Get low register R7"},
{ 0x88, "GLO R8", 0, NONE, "Get low register R8"},
{ 0x89, "GLO R9", 0, NONE, "Get low register R9"},
{ 0x8a, "GLO RA", 0, NONE, "Get low register RA"},
{ 0x8b, "GLO RB", 0, NONE, "Get low register RB"},
{ 0x8c, "GLO RC", 0, NONE, "Get low register RC"},
{ 0x8d, "GLO RD", 0, NONE, "Get low register RD"},
{ 0x8e, "GLO RE", 0, NONE, "Get low register RE"},
{ 0x8f, "GLO RF", 0, NONE, "Get low register RF"},

{ 0x90, "GHI R0", 0, NONE, "Get high register R0"},
{ 0x91, "GHI R1", 0, NONE, "Get high register R1"},
{ 0x92, "GHI R2", 0, NONE, "Get high register R2"},
{ 0x93, "GHI R3", 0, NONE, "Get high register R3"},
{ 0x94, "GHI R4", 0, NONE, "Get high register R4"},
{ 0x95, "GHI R5", 0, NONE, "Get high register R5"},
{ 0x96, "GHI R6", 0, NONE, "Get high register R6"},
{ 0x97, "GHI R7", 0, NONE, "Get high register R7"},
{ 0x98, "GHI R8", 0, NONE, "Get high register R8"},
{ 0x99, "GHI R9", 0, NONE, "Get high register R9"},
{ 0x9a, "GHI RA", 0, NONE, "Get high register RA"},
{ 0x9b, "GHI RB", 0, NONE, "Get high register RB"},
{ 0x9c, "GHI RC", 0, NONE, "Get high register RC"},
{ 0x9d, "GHI RD", 0, NONE, "Get high register RD"},
{ 0x9e, "GHI RE", 0, NONE, "Get high register RE"},
{ 0x9f, "GHI RF", 0, NONE, "Get high register RF"},

{ 0xa0, "PLO R0", 0, NONE, "Put low register R0"},
{ 0xa1, "PLO R1", 0, NONE, "Put low register R1"},
{ 0xa2, "PLO R2", 0, NONE, "Put low register R2"},
{ 0xa3, "PLO R3", 0, NONE, "Put low register R3"},
{ 0xa4, "PLO R4", 0, NONE, "Put low register R4"},
{ 0xa5, "PLO R5", 0, NONE, "Put low register R5"},
{ 0xa6, "PLO R6", 0, NONE, "Put low register R6"},
{ 0xa7, "PLO R7", 0, NONE, "Put low register R7"},
{ 0xa8, "PLO R8", 0, NONE, "Put low register R8"},
{ 0xa9, "PLO R9", 0, NONE, "Put low register R9"},
{ 0xaa, "PLO RA", 0, NONE, "Put low register RA"},
{ 0xab, "PLO RB", 0, NONE, "Put low register RB"},
{ 0xac, "PLO RC", 0, NONE, "Put low register RC"},
{ 0xad, "PLO RD", 0, NONE, "Put low register RD"},
{ 0xae, "PLO RE", 0, NONE, "Put low register RE"},
{ 0xaf, "PLO RF", 0, NONE, "Put low register RF"},

{ 0xb0, "PHI R0", 0, NONE, "Put high register R0"},
{ 0xb1, "PHI R1", 0, NONE, "Put high register R1"},
{ 0xb2, "PHI R2", 0, NONE, "Put high register R2"},
{ 0xb3, "PHI R3", 0, NONE, "Put high register R3"},
{ 0xb4, "PHI R4", 0, NONE, "Put high register R4"},
{ 0xb5, "PHI R5", 0, NONE, "Put high register R5"},
{ 0xb6, "PHI R6", 0, NONE, "Put high register R6"},
{ 0xb7, "PHI R7", 0, NONE, "Put high register R7"},
{ 0xb8, "PHI R8", 0, NONE, "Put high register R8"},
{ 0xb9, "PHI R9", 0, NONE, "Put high register R9"},
{ 0xba, "PHI RA", 0, NONE, "Put high register RA"},
{ 0xbb, "PHI RB", 0, NONE, "Put high register RB"},
{ 0xbc, "PHI RC", 0, NONE, "Put high register RC"},
{ 0xbd, "PHI RD", 0, NONE, "Put high register RD"},
{ 0xbe, "PHI RE", 0, NONE, "Put high register RE"},
{ 0xbf, "PHI RF", 0, NONE, "Put high register RF"},

{ 0xc0, "LBR", 2, LONGBR, "Long branch"},
{ 0xc1, "LBQ", 2, LONGBR, "Long branch on Q=1"},
{ 0xc2, "LBZ", 2, LONGBR, "Long branch on D=0"},
{ 0xc3, "LBDF", 2, LONGBR, "Long branch on DF=1"},
{ 0xc4, "NOP", 0, NONE, "No operation"},
{ 0xc5, "LSNQ", 0, LONGSKIP, "Long skip on Q=0"},
{ 0xc6, "LSNZ", 0, LONGSKIP, "Long skip on D!=0"},
{ 0xc7, "LSNF", 0, LONGSKIP, "Long skip on DF=0"},
{ 0xc8, "LSKP", 2, NONE, "Long skip"},
{ 0xc9, "LBNQ", 2, LONGBR, "Long branch on Q=0"},
{ 0xca, "LBNZ", 2, LONGBR, "Long branch on D!=0"},
{ 0xcb, "LBNF", 2, LONGBR, "Long branch on DF=0"},
{ 0xcc, "LSIE", 0, LONGSKIP, "Long skip on IE=1"},
{ 0xcd, "LSQ", 0, LONGSKIP, "Long skip on Q=1"},
{ 0xce, "LSZ", 0, LONGSKIP, "Long skip on D=0"},
{ 0xcf, "LSDF", 0, LONGSKIP, "Long skip on DF=1"},

{ 0xd0, "SEP R0", 0, NONE, "Set P=R0 as program counter"},
{ 0xd1, "SEP R1", 0, NONE, "Set P=R1 as program counter"},
{ 0xd2, "SEP R2", 0, NONE, "Set P=R2 as program counter"},
{ 0xd3, "SEP R3", 0, NONE, "Set P=R3 as program counter"},
{ 0xd4, "SEP R4", 0, NONE, "Set P=R4 as program counter"},
{ 0xd5, "SEP R5", 0, NONE, "Set P=R5 as program counter"},
{ 0xd6, "SEP R6", 0, NONE, "Set P=R6 as program counter"},
{ 0xd7, "SEP R7", 0, NONE, "Set P=R7 as program counter"},
{ 0xd8, "SEP R8", 0, NONE, "Set P=R8 as program counter"},
{ 0xd9, "SEP R9", 0, NONE, "Set P=R9 as program counter"},
{ 0xda, "SEP RA", 0, NONE, "Set P=RA as program counter"},
{ 0xdb, "SEP RB", 0, NONE, "Set P=RB as program counter"},
{ 0xdc, "SEP RC", 0, NONE, "Set P=RC as program counter"},
{ 0xdd, "SEP RD", 0, NONE, "Set P=RD as program counter"},
{ 0xde, "SEP RE", 0, NONE, "Set P=RE as program counter"},
{ 0xdf, "SEP RF", 0, NONE, "Set P=RF as program counter"},

{ 0xe0, "SEX R0", 0, NONE, "Set X=R0 as datapointer"},
{ 0xe1, "SEX R1", 0, NONE, "Set X=R1 as datapointer"},
{ 0xe2, "SEX R2", 0, NONE, "Set X=R2 as datapointer"},
{ 0xe3, "SEX R3", 0, NONE, "Set X=R3 as datapointer"},
{ 0xe4, "SEX R4", 0, NONE, "Set X=R4 as datapointer"},
{ 0xe5, "SEX R5", 0, NONE, "Set X=R5 as datapointer"},
{ 0xe6, "SEX R6", 0, NONE, "Set X=R6 as datapointer"},
{ 0xe7, "SEX R7", 0, NONE, "Set X=R7 as datapointer"},
{ 0xe8, "SEX R8", 0, NONE, "Set X=R8 as datapointer"},
{ 0xe9, "SEX R9", 0, NONE, "Set X=R9 as datapointer"},
{ 0xea, "SEX RA", 0, NONE, "Set X=RA as datapointer"},
{ 0xeb, "SEX RB", 0, NONE, "Set X=RB as datapointer"},
{ 0xec, "SEX RC", 0, NONE, "Set X=RC as datapointer"},
{ 0xed, "SEX RD", 0, NONE, "Set X=RD as datapointer"},
{ 0xee, "SEX RE", 0, NONE, "Set X=RE as datapointer"},
{ 0xef, "SEX RF", 0, NONE, "Set X=RF as datapointer"},

{ 0xf0, "LDX", 0, NONE, "Pop stack. Place value in D register"},
{ 0xf1, "OR", 0, NONE, "Logical OR  D with (R(X))"},
{ 0xf2, "AND", 0, NONE, "Logical AND: D with (R(X))"},
{ 0xf3, "XOR", 0, NONE, "Logical exclusive OR  D with (R(X))"},
{ 0xf4, "ADD", 0, NONE, "Add D: D,DF= D+(R(X))"},
{ 0xf5, "SD", 0, NONE, "Substract D: D,DF=(R(X))-D"},
{ 0xf6, "SHR", 0, NONE, "Shift right D"},
{ 0xf7, "SM", 0, NONE, "Substract memory: DF,D=D-(R(X))"},
{ 0xf8, "LDI", 1, ABSVAL, "Load D immediate"},
{ 0xf9, "ORI", 1, ABSVAL, "Logical OR D with value"},
{ 0xfa, "ANI", 1, ABSVAL, "Logical AND D with value"},
{ 0xfb, "XRI", 1, ABSVAL, "Logical XOR D with value"},
{ 0xfc, "ADI", 1, ABSVAL, "Add D,DF with value"},
{ 0xfd, "SDI", 1, ABSVAL, "Substract D,DF from value"},
{ 0xfe, "SHL", 0, NONE, "Shift left D"},
{ 0xff, "SMI", 1, ABSVAL, "Substract D,DF to value"}
};

static struct opcode opcodes2[256]={
{ 0x00, "STPC", 0 , NONE ,"stop counter"},
{ 0x01, "DTC", 0 , NONE ,"decrement timer/counter"},
{ 0x02, "SPM2", 0 , NONE ,"set pulse width mode 2 and start"},
{ 0x03, "SCM2", 0 , NONE ,"set counter mode 2 and start"},
{ 0x04, "SPM1", 0 , NONE ,"set pulse width mode 1 and start"},
{ 0x05, "SCM1", 0 , NONE ,"set counter mode 1 and start"},
{ 0x06, "LDC", 0 , NONE ,"load counter"},
{ 0x07, "STM", 0 , NONE ,"set timer mode and start"},
{ 0x08, "GEC", 0 , NONE ,"get counter"},
{ 0x09, "ETQ", 0 , NONE ,"enable toggle Q"},
{ 0x0a, "XIE", 0 , NONE ,"external interrupt enable"},
{ 0x0b, "XID", 0 , NONE ,"external interrupt disable"},
{ 0x0c, "CIE", 0 , NONE ,"counter interrupt enable"},
{ 0x0d, "CID", 0 , NONE ,"counter interrupt disable"},
{ 0x0e, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x0f, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},

{ 0x10, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x11, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x12, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x13, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x14, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x15, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x16, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x17, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x18, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x19, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x1a, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x1b, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x1c, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x1d, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x1e, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x1f, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},

{ 0x20, "DBNZ R0", 2, LONGBR, "decrement R0 and long branch if not 0"},
{ 0x21, "DBNZ R1", 2, LONGBR, "decrement R1 and long branch if not 0"},
{ 0x22, "DBNZ R2", 2, LONGBR, "decrement R2 and long branch if not 0"},
{ 0x23, "DBNZ R3", 2, LONGBR, "decrement R3 and long branch if not 0"},
{ 0x24, "DBNZ R4", 2, LONGBR, "decrement R4 and long branch if not 0"},
{ 0x25, "DBNZ R5", 2, LONGBR, "decrement R5 and long branch if not 0"},
{ 0x26, "DBNZ R6", 2, LONGBR, "decrement R6 and long branch if not 0"},
{ 0x27, "DBNZ R7", 2, LONGBR, "decrement R7 and long branch if not 0"},
{ 0x28, "DBNZ R8", 2, LONGBR, "decrement R8 and long branch if not 0"},
{ 0x29, "DBNZ R9", 2, LONGBR, "decrement R9 and long branch if not 0"},
{ 0x2a, "DBNZ RA", 2, LONGBR, "decrement RA and long branch if not 0"},
{ 0x2b, "DBNZ RB", 2, LONGBR, "decrement RB and long branch if not 0"},
{ 0x2c, "DBNZ RC", 2, LONGBR, "decrement RC and long branch if not 0"},
{ 0x2d, "DBNZ RD", 2, LONGBR, "decrement RD and long branch if not 0"},
{ 0x2e, "DBNZ RE", 2, LONGBR, "decrement RE and long branch if not 0"},
{ 0x2f, "DBNZ RF", 2, LONGBR, "decrement RF and long branch if not 0"},

{ 0x30, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x31, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x32, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x33, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x34, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x35, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x36, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x37, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x38, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x39, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x3a, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x3b, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x3c, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x3d, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x3e, "BCI", 0 , NONE ,"short branch on counter interrupt"},
{ 0x3f, "BXI", 0 , NONE ,"short branch on external interrupt"},

{ 0x40, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x41, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x42, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x43, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x44, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x45, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x46, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x47, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x48, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x49, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x4a, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x4b, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x4c, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x4d, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x4e, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x4f, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},

{ 0x50, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x51, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x52, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x53, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x54, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x55, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x56, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x57, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x58, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x59, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x5a, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x5b, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x5c, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x5d, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x5e, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x5f, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},

{ 0x60, "RLXA R0", 2, NONE, "load memory to R0"},
{ 0x61, "RLXA R1", 2, NONE, "load memory to R1"},
{ 0x62, "RLXA R2", 2, NONE, "load memory to R2"},
{ 0x63, "RLXA R3", 2, NONE, "load memory to R3"},
{ 0x64, "RLXA R4", 2, NONE, "load memory to R4"},
{ 0x65, "RLXA R5", 2, NONE, "load memory to R5"},
{ 0x66, "RLXA R6", 2, NONE, "load memory to R6"},
{ 0x67, "RLXA R7", 2, NONE, "load memory to R7"},
{ 0x68, "RLXA R8", 2, NONE, "load memory to R8"},
{ 0x69, "RLXA R9", 2, NONE, "load memory to R9"},
{ 0x6a, "RLXA RA", 2, NONE, "load memory to RA"},
{ 0x6b, "RLXA RB", 2, NONE, "load memory to RB"},
{ 0x6c, "RLXA RC", 2, NONE, "load memory to RC"},
{ 0x6d, "RLXA RD", 2, NONE, "load memory to RD"},
{ 0x6e, "RLXA RE", 2, NONE, "load memory to RE"},
{ 0x6f, "RLXA RF", 2, NONE, "load memory to RF"},

{ 0x70, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x71, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x72, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x73, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x74, "DADC", 0 , NONE ,"decimal add with carry"},
{ 0x75, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x76, "DSAV", 0 , NONE ,"save T,D,DF"},
{ 0x77, "DSMB", 0 , NONE ,"decimal substract memory with borrow"},
{ 0x78, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x79, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x7a, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x7b, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x7c, "DACI", 1 , ABSVAL ,"decimal add with carry immediate"},
{ 0x7d, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x7e, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0x7f, "DSBI", 1 , ABSVAL ,"decimal substract memory with borrow, immediate"},

{ 0x80, "SCAL R0", 0, NONE, "standard call to (R0)"},
{ 0x81, "SCAL R1", 0, NONE, "standard call to (R1)"},
{ 0x82, "SCAL R2", 0, NONE, "standard call to (R2)"},
{ 0x83, "SCAL R3", 0, NONE, "standard call to (R3)"},
{ 0x84, "SCAL R4", 0, NONE, "standard call to (R4)"},
{ 0x85, "SCAL R5", 0, NONE, "standard call to (R5)"},
{ 0x86, "SCAL R6", 0, NONE, "standard call to (R6)"},
{ 0x87, "SCAL R7", 0, NONE, "standard call to (R7)"},
{ 0x88, "SCAL R8", 0, NONE, "standard call to (R8)"},
{ 0x89, "SCAL R9", 0, NONE, "standard call to (R9)"},
{ 0x8a, "SCAL RA", 0, NONE, "standard call to (RA)"},
{ 0x8b, "SCAL RB", 0, NONE, "standard call to (RB)"},
{ 0x8c, "SCAL RC", 0, NONE, "standard call to (RC)"},
{ 0x8d, "SCAL RD", 0, NONE, "standard call to (RD)"},
{ 0x8e, "SCAL RE", 0, NONE, "standard call to (RE)"},
{ 0x8f, "SCAL RF", 0, NONE, "standard call to (RF)"},

{ 0x90, "SRET R0", 0, NONE, "standard return to (R0)"},
{ 0x91, "SRET R1", 0, NONE, "standard return to (R1)"},
{ 0x92, "SRET R2", 0, NONE, "standard return to (R2)"},
{ 0x93, "SRET R3", 0, NONE, "standard return to (R3)"},
{ 0x94, "SRET R4", 0, NONE, "standard return to (R4)"},
{ 0x95, "SRET R5", 0, NONE, "standard return to (R5)"},
{ 0x96, "SRET R6", 0, NONE, "standard return to (R6)"},
{ 0x97, "SRET R7", 0, NONE, "standard return to (R7)"},
{ 0x98, "SRET R8", 0, NONE, "standard return to (R8)"},
{ 0x99, "SRET R9", 0, NONE, "standard return to (R9)"},
{ 0x9a, "SRET RA", 0, NONE, "standard return to (RA)"},
{ 0x9b, "SRET RB", 0, NONE, "standard return to (RB)"},
{ 0x9c, "SRET RC", 0, NONE, "standard return to (RC)"},
{ 0x9d, "SRET RD", 0, NONE, "standard return to (RD)"},
{ 0x9e, "SRET RE", 0, NONE, "standard return to (RE)"},
{ 0x9f, "SRET RF", 0, NONE, "standard return to (RF)"},

{ 0xa0, "RSXD R0", 0, NONE, "store register R0 in memory"},
{ 0xa1, "RSXD R1", 0, NONE, "store register R1 in memory"},
{ 0xa2, "RSXD R2", 0, NONE, "store register R2 in memory"},
{ 0xa3, "RSXD R3", 0, NONE, "store register R3 in memory"},
{ 0xa4, "RSXD R4", 0, NONE, "store register R4 in memory"},
{ 0xa5, "RSXD R5", 0, NONE, "store register R5 in memory"},
{ 0xa6, "RSXD R6", 0, NONE, "store register R6 in memory"},
{ 0xa7, "RSXD R7", 0, NONE, "store register R7 in memory"},
{ 0xa8, "RSXD R8", 0, NONE, "store register R8 in memory"},
{ 0xa9, "RSXD R9", 0, NONE, "store register R9 in memory"},
{ 0xaa, "RSXD RA", 0, NONE, "store register RA in memory"},
{ 0xab, "RSXD RB", 0, NONE, "store register RB in memory"},
{ 0xac, "RSXD RC", 0, NONE, "store register RC in memory"},
{ 0xad, "RSXD RD", 0, NONE, "store register RD in memory"},
{ 0xae, "RSXD RE", 0, NONE, "store register RE in memory"},
{ 0xaf, "RSXD RF", 0, NONE, "store register RF in memory"},

{ 0xb0, "RNX R0", 0, NONE, "copy register R0 to R(X)"},
{ 0xb1, "RNX R1", 0, NONE, "copy register R1 to R(X)"},
{ 0xb2, "RNX R2", 0, NONE, "copy register R2 to R(X)"},
{ 0xb3, "RNX R3", 0, NONE, "copy register R3 to R(X)"},
{ 0xb4, "RNX R4", 0, NONE, "copy register R4 to R(X)"},
{ 0xb5, "RNX R5", 0, NONE, "copy register R5 to R(X)"},
{ 0xb6, "RNX R6", 0, NONE, "copy register R6 to R(X)"},
{ 0xb7, "RNX R7", 0, NONE, "copy register R7 to R(X)"},
{ 0xb8, "RNX R8", 0, NONE, "copy register R8 to R(X)"},
{ 0xb9, "RNX R9", 0, NONE, "copy register R9 to R(X)"},
{ 0xba, "RNX RA", 0, NONE, "copy register RA to R(X)"},
{ 0xbb, "RNX RB", 0, NONE, "copy register RB to R(X)"},
{ 0xbc, "RNX RC", 0, NONE, "copy register RC to R(X)"},
{ 0xbd, "RNX RD", 0, NONE, "copy register RD to R(X)"},
{ 0xbe, "RNX RE", 0, NONE, "copy register RE to R(X)"},
{ 0xbf, "RNX RF", 0, NONE, "copy register RF to R(X)"},

{ 0xc0, "RLDI R0", 2, ABSVAL, "register load immediate R0"},
{ 0xc1, "RLDI R1", 2, ABSVAL, "register load immediate R1"},
{ 0xc2, "RLDI R2", 2, ABSVAL, "register load immediate R2"},
{ 0xc3, "RLDI R3", 2, ABSVAL, "register load immediate R3"},
{ 0xc4, "RLDI R4", 2, ABSVAL, "register load immediate R4"},
{ 0xc5, "RLDI R5", 2, ABSVAL, "register load immediate R5"},
{ 0xc6, "RLDI R6", 2, ABSVAL, "register load immediate R6"},
{ 0xc7, "RLDI R7", 2, ABSVAL, "register load immediate R7"},
{ 0xc8, "RLDI R8", 2, ABSVAL, "register load immediate R8"},
{ 0xc9, "RLDI R9", 2, ABSVAL, "register load immediate R9"},
{ 0xca, "RLDI RA", 2, ABSVAL, "register load immediate RA"},
{ 0xcb, "RLDI RB", 2, ABSVAL, "register load immediate RB"},
{ 0xcc, "RLDI RC", 2, ABSVAL, "register load immediate RC"},
{ 0xcd, "RLDI RD", 2, ABSVAL, "register load immediate RD"},
{ 0xce, "RLDI RE", 2, ABSVAL, "register load immediate RE"},
{ 0xcf, "RLDI RF", 2, ABSVAL, "register load immediate RF"},

{ 0xd0, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd1, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd2, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd3, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd4, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd5, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd6, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd7, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd8, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xd9, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xda, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xdb, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xdc, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xdd, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xde, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xdf, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},

{ 0xe0, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe1, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe2, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe3, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe4, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe5, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe6, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe7, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe8, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xe9, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xea, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xeb, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xec, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xed, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xee, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xef, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},

{ 0xf0, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xf1, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xf2, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xf3, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xf4, "DADD", 0 , NONE ,"decimal add"},
{ 0xf5, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xf6, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xf7, "DSM", 0 , NONE ,"decimal substract memory"},
{ 0xf8, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xf9, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xfa, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xfb, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xfc, "DADI", 1 , ABSVAL ,"decimal add immediate"},
{ 0xfd, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xfe, "INVALID", 0 , NONE ,"INVALID INSTRUCTION"},
{ 0xff, "DSMI", 1 , ABSVAL ,"decimal substract memory, immediate"}
};

unsigned char hex2char(char *hex) {
	unsigned char chr=0;

	if(hex[0]<'A')
		chr=16*(hex[0]-'0');
	else
	if(hex[0]<'a')
		chr=16*(hex[0]-'A'+10);
	else
		chr=16*(hex[0]-'a'+10);

	if(hex[1]<'A')
		chr+=(hex[1]-'0');
	else
	if(hex[1]<'a')
		chr+=(hex[1]-'A'+10);
	else
		chr+=(hex[1]-'a'+10);

	return chr;
}

unsigned int readhex(FILE *in, FILE *out)
{
	char buffer[128];
	unsigned int pointer;
	unsigned int bytecount;
	int i;

	bytecount=0;

	for(pointer=0; pointer<EPROMSIZE; pointer++)
		data[pointer]=0;
	
	while(!feof(in)) {
		fgets(buffer,128,stdin);
		if(buffer[0]!=':') {
#ifdef SHOWHEX
			printf("Comment: %s",buffer);
#endif
			continue;
		}
		if(hex2char(buffer+7)) {
#ifdef SHOWHEX
			printf("no data: %s",buffer);
#endif
			continue;
		}
		pointer=256*hex2char(buffer+3)+hex2char(buffer+5);
#ifdef SHOWHEX
		printf("%04X: %s", pointer, buffer+3);
#endif
		for(i=0; i<hex2char(buffer+1); i++) {
			data[pointer+i]=hex2char(buffer+9+i+i);
			bytecount++;
		}
	}

	for (pointer=0; pointer<EPROMSIZE; pointer++)
		fputc(data[pointer],out);

	printf("\n%u bytes count\n",bytecount);

	return bytecount;
}

unsigned int readbin(FILE *in, FILE *out)
{
	unsigned int pointer;
	unsigned int bytecount;
	int i;
        unsigned char *pdata;

	bytecount=0;

	for(pointer=0; pointer<EPROMSIZE; pointer++)
		data[pointer]=0;
	
        pdata = data;
	while(!feof(in)) {
		i = fread(pdata, 1, 128, in);
                pdata += i;
                bytecount += i;
        }

	printf("\n%u bytes count\n",bytecount);

	return bytecount;
}

int main(int argc, char *argv[]) 
{

	int i;
	char buffer[32];
	unsigned int pointer;
	unsigned int begin, end;
	FILE *in, *out;
        char *infile;
	unsigned char c0de;
	unsigned int arg;	
        unsigned int (*readat)(FILE*,FILE*);
        struct opcode *opc0de;

        cdp=6;
	begin=0x0;
        infile=NULL; in=stdin;
        readat=readhex;
	for(i=0; i< argc; i++) {
		if(argv[i][0]=='-')
			switch(argv[i][1]) {
				case 'b': begin=256*hex2char(argv[i]+2)+hex2char(argv[i]+4);
					break;
				case 'e': end=256*hex2char(argv[i]+2)+hex2char(argv[i]+4);
                                        break;
                                case 'r': infile=argv[i]+2;
                                        readat=readbin;
                                        break;
                                case '2': cdp=2;
                                        break;

				default:
					break;
			}	
	}

        out=NULL;
        if (infile)
                in=fopen(infile,"rb");
        else 
	        out=fopen("output.bin","w+");
	readat(in, out);
        fclose(infile? in : out);


	printf("1806 disasembler (c)1998 Herman Robers PA3FYW\nUsage: disasm [-bxxxx] [-exxxx] [-rrom.bin] [-2]< hexfile.hex > disasm.asm, (example: disasm -b00e0 -e0a00 < dump.hex > dump.asm)\nDisassembling from %04XH to %04XH.\n\n",begin,end);

	for(pointer=begin; pointer<end; pointer++) {
		buffer[0]='\0';

	        c0de = data[pointer];
                opc0de = c0de==0x68? &x68[cdp] : &opcodes[c0de];

		if(c0de!=0x68 || cdp==2) {
			for(i=0; i<=opc0de->argc; i++) {
				sprintf(buffer+strlen(buffer),"%02X ",data[pointer+i]);
			}	
			printf("%04X %-12s ",pointer, buffer);		
			sprintf(buffer,"%s ",opc0de->mnemonic);
			arg = 0;
			if(1 == opc0de->argc) {
				arg = data[pointer+1];
			 	// sprintf(buffer+strlen(buffer),"%02X ",arg);
			}	
			if(2 == opc0de->argc) {
				arg = data[pointer+1]*256 + data[pointer+2];
			 	// sprintf(buffer+strlen(buffer),"%04X ",arg);
			}	
			switch (opc0de->argt) {
			case NONE:	break;
			case SHORTBR:	sprintf(buffer+strlen(buffer),"%04XH ",(0xFF00 & pointer) + arg); break;
			case LONGBR:	sprintf(buffer+strlen(buffer),"%04XH ",arg); break;
			case ABSVAL:	sprintf(buffer+strlen(buffer),"%02XH ",arg); break;
			case CONTVAL:	break;
			case IGNORE:	break; /* sprintf(buffer+strlen(buffer),"%02XH ",arg); break; */
			case END:	break;
			case LONGSKIP:  break;
			}
			printf("%-13s  ; %s\n",buffer,opc0de->desc);
			pointer += opc0de->argc;
		}
		else {
			pointer++;
			c0de = data[pointer];
			sprintf(buffer,"68 ");
			for(i=0; i<=opcodes2[c0de].argc; i++) {
				sprintf(buffer+strlen(buffer),"%02X ",data[pointer+i]);
			}	
			printf("%04X %-12s ",pointer, buffer);		
			sprintf(buffer,"%s ",opcodes2[c0de].mnemonic);
			if(1 == opcodes2[c0de].argc) {
				arg = data[pointer+1];
			 	// sprintf(buffer+strlen(buffer),"%02X ",data[pointer+1]);
			}	
			if(2 == opcodes2[c0de].argc) {
				arg = data[pointer+1]*256 + data[pointer+2];
			 	// sprintf(buffer+strlen(buffer),"%02X%02X ",data[pointer+1], data[pointer+2]);
			}	
			switch (opcodes[c0de].argt) {
			case NONE:	break;
			case SHORTBR:	sprintf(buffer+strlen(buffer),"%04XH ",(0xFF00 & pointer) + arg); break;
			case LONGBR:	sprintf(buffer+strlen(buffer),"%04XH ",arg); break;
			case ABSVAL:	sprintf(buffer+strlen(buffer),"%02XH ",arg); break;
			case CONTVAL:	break;
			case IGNORE:	sprintf(buffer+strlen(buffer),"%02XH ",arg); break;
			case END:	break;
			case LONGSKIP:  break;
			}
			printf("%-13s  ; %s\n",buffer,opcodes2[c0de].desc);
			pointer += opcodes2[c0de].argc;
		}
	}
		
/*
	for(i=0; ; i++) {
		if(i!=opcodes[i].opcode) {
			printf("opcode: %d != %d\n",i, opcodes[i].opcode);
			break;
		}
		printf("%02X: %-10s %d (%d)\n",opcodes[i].opcode, opcodes[i].mnemonic, opcodes[i].argc, opcodes[i].argt);
	}
*/
	return 0; 

}

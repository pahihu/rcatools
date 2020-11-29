.. Jul 10, 2020 Gregory Simmons, David Schultz, taf123
.. 
..************************************************
..NAME:  UT62/4
..DESC:  RCA MICROBOARD MONITOR FOR CASSETTE I/O
..DATE:  6/16/82


..       UT62 IS A MONITOR PROGRAM DESIGNED TO
..       EXAMINE MEMORY, ALTER MEMORY, AND TO
..       TO BEGIN PROGRAM EXECUTION AT A GIVEN
..       LOCATION. ALSO, PROGRAMS CAN BE SAVED ON TAPE. THESE FUNCTIONS
..       ARE ACCOMPLISHED THROUGH A SERIES OF MONITOR
..       COMMANDS WHICH
..       ARE INITIATED BY TYPING A,B,C,D,E,F,I,J,M.P,R,S,OR W.
..       THE  FUNCTIONS INCLUDE MEMORY INSERT[I],
..       MEMORY DISPLAY[D], MEMORY MOVE[M],
..       MEMORY FILL[F], MEMORY SUBSTITUTE[S],READ TAPE[R],
..       RUN EDITOR[E],RUN ASSEMBLER[A],RUN BASIC[B],
..       WRITE TAPE[W],REWIND TAPES[J],COPY TAPE TO SCREEN [C],
..       AND RUN PROGRAM[P].
..       TAPE READ LOADS MEMORY FROM TAPE.
..       TAPE WRITE SAVES MEMORY ON TAPE FROM START TO
..       END ADDRESS AS SPECIFIED.
..       ALSO INCLUDED ARE THE STANDARD READ AND
..       TYPE ROUTINES WHICH PROVIDE COMMUNICATION
..       WITH THE USERS TERMINAL.
..       UPON INVOCATION, THE CONTENTS OF THE
..       CPU REGISTERS ARE SAVED IN RAM AT #8C00.
..       THE CONTENTS OF R0, R1, AND R4.1 ARE
..       DESTROYED, HOWEVER, BY THE PROCESS.
..       THEIR CONTENTS CAN BE EXAMINED BY DOING
..       D8C00 FOR 20 BYTES.

..***********************************************
..       SYSTEM EQUATES, CONSTANTS & OFFSETS
..***********************************************

..      REGISTER ASSIGNMENTS - GEN. & UTILITIES

SP=#02..STACKPOINTER
PC=#03..PROGRAM COUNTER
CALL=#04..CALL ROUTINE COUNTER
RETN=#05..RETURN ROUTINE COUNTER
LINK=#06..SUBROUTINE DATA LINK
BYTSTR=#07..TEMPORARY STORAGE DURING WRITE
STRADD=#07..START ADDRESS FOR TAPE WRITE
PARITY=#07..FOR TAPE ROUTINES
TMPRG1=#07...TEMPORARY REGISTER
BITRDR=#08...POINTS TO THE ROUTINE TO READ TAPED BITS
BITWRT=#08...POINTS TO ROUTINE TO WRITE A BIT TO TAPE
ENDADD=#08...HOLDS END ADDRESS FOR WRITE TO TAPE
ADRPTR=#08...HOLDS ADDRESS DURING STORE FROM TP
TMPRG2=#08...TEMPORARY REGISTER
LDRCNT=#09...COUNTER FOR TAPE LEADER
BITCNT=#09...COUNTER FOR BITS PER WORD FROM TAPE
TMPRG3=#09...TEMPORARY REGISTER
BUFPTR=#0A...POINTS TO TAPE READ/WRITE BUFFERS
CNT=#0A...BYTE COUNT
SRC=#0B...SOURCE REGISTER
TPTR=#0B...TABLE POINTER
BLKFLG=#0B...FLAG TO SIGNAL TO WRITE A BLOCK
DELAY=#0C...DELAY ROUTINE COUNTER
PTER=#0C...IOCB PTR
PTR=#0C...DITTO
ASL=#0D...HEX INPUT REGISTER
DEST=#0D...DESTINATION REGISTER
AUX=#0E...AUX.1 HOLDS BIT TIME CONSTANT
CHAR=#0F...STORES ASCII I/O
FLAG=#0F...FLAGS FOR TAPE

..      RAM / ROM ALLOCATIONS

RDBUFF=#8E7F
TOPSTK=#8CFF
WRAM=#8C1F
WRTBUF=#8E80
UT62=#8000

..

ABAS=#b0
AEDIT=#90
AASM=#9B
PRMPGR=#E0

..      ASCII CONTROL CHARACTERS

NULL=#00        ...NULL
COMMA=#2C       ...COMMA
SEMCOL=#3B      ...SEMICOLON
BS=#08          ...BACK SPACE
LF=#0A          ...LINE FEED
CR=#0D          ...CARRIAGE RETURN
EOF=#13         ...END OF FILE
SPACE=#20       ...SPACE (WORD SEPARATOR)
CRLF=#0D0A      ...RETURN,LINEFEED

..      CONSTANTS

BDSEL=#01...PORT FOR TWO LEVEL I/O SELECT
BTHDRV=#C0 ..SELECTS BOTH TAPE DRIVES
DRIVE0=#40...SELECTS TAPE DRIVE 0
DRIVE1=#80...SELECTS TAPE DRIVE 1
HHIGH=#01...WRITE A HIGH TO TAPE
LNECNT=#0F...# OF BYTES PER LINE IN DISPLAY ROUTINE
LINES=#14...NUMBER OF LINES PER SCREEN LOAD
LLOW=#00...WRITE A LOW TO TAPE
PGMSRT=#0005 ..START ADDRESS FOR INIT1
PROMPT=#2A...PROMPT CHARACTER
TAPEIO=#02...SELECTS TAPE RECORDER
TPEOFF=#00...TURN OFF TAPES
TPESEL=#04...PORT TO SELECT TAPES
TPEWRT=#05...PORT TO WRITE TO TAPE
TRMINL=#01...SELECTS THE TERMINAL

..**************************************************************
                ORG UT62

        DIS;,0             ..DISABLE
        LDI A.1(UT62) ;PHI R0..ESTABLISH PROGRAM COUNTER AT
                           ..8000 HEX

..********************************************************
..                  REGISTER SAVE
..       SAVES CONTENTS OF THE CPU REGISTERS @#8C00.
...*******************************************************

        LDI A.1(WRAM-1) ;PHI R1 ..THIS SEQUENCE PERFORMS
        LDI A.0(WRAM-1) ;PLO R1..THE REGISTER SAVE
        LDI #A0;PHI R4   ..MODIFIED INSTRUCTION
        SEX R1
                           ..SET UP SEP INSTRUCTION
LOOP:   LDI #D0;STR R1     ..FOR RETURN
        XOR             ..CHECK IT WROTE
        BNZ UT62A
        DEC R1             ..PREPARE FOR 9N INST.
        GHI R4;ADI #70
        BDF *+4
        ADI #21        ..NO, 8N  ;9N
        ADI #7F           ..YES, 9N  ;8(N-1)
        PHI R4
        STR R1              ..SAVE INST. & EXECUTE IT
        SEP R1
        STXD        ..STORE RESULT IN RAM
        DEC R1             ..BACK UP & CHECK IF
        GHI R4;XRI #91   ..STORAGE DONE?
        BNZ LOOP    ..NEXT BYTE  

UT62A:  LBR INIT

START:  SEP CALL;,A(TIMALC)

..******************************************************************
..                  OUTPUT THE UTILITY PROMPT
..******************************************************************

PRMPT:  SEP CALL;,A(TPOFF)       ..TURN OFF TAPES,SEL RCA GRP
        SEP CALL;,A(OSTRNG)       ..OUTPUT PROMPT TO USER TERMINAL
        ,A(CRLF),PROMPT,0


..******************************************************************
..               MONITOR COMMAND INTERPRETER
..      FETCHES THE ADDRESS FROM THE COMMAND TABLE AND SETS
..      THE PROGRAM COUNTER TO IT
..   REG USED:  PTR, CHAR, SP, ASL
..******************************************************************
        LDI A.0(TAB2);PLO TPTR
        LDI A.1(TAB2);PHI TPTR   ..GET COMMAND TABLE ADDR

SCNLTR: SEP CALL;,A(READ)  ..READ COMMAND
        SEX TPTR

SCAN:   GHI CHAR;XOR       ..LOOK FOR MATCH
        INC TPTR
        BZ GOTHRE ..YES, COMPUTE ADDRESS
        INC TPTR;INC TPTR  ..INC THE POINTER TO THE
        LDN TPTR              ..NEXT COMMAND
        BZ ERROR  ..ERROR IF END OF TABLE
        BR SCAN          ..ELSE CONTINUE LOOKING

GOTHRE: DEC SP; DEC SP     ..FAKE IT FOR THE RETURN
                           ..PICK UP COMMAND ADDRESS
        LDA TPTR;PHI LINK  ..AND TRANSFER TO THE
                           ..SUBROUTINE BY EXECUTING
        LDA TPTR;PLO LINK  ..A RETURN INSTRUCTION
        LDI 0 ;PHI ASL;PLO ASL
        SEP R5               ..P=3,X=2,R4 ;SEP CALL;,,R5 ;RETURN,R2=#8CFF

..******************************************************************
..                   COMMAND TABLES
..******************************************************************

TAB2:   ,T'D',A(DISPLY)     ..MEMORY DISPLAY
        ,T'I',A(INSERT)      ..INSERT INTO MEMORY
        ,T'M',A(MOVE)        ..MOVE A BLOCK OF MEMORY
        ,T'F',A(FILL)        ..FILL A BLOCK OF MEMORY
        ,T'S',A(SUBST)       ..BYTE SUBSTITUTION
        ,T'P',A(RUN)         ..RUN A USER PROGRAM
        ,T'R',A(TPLOAD)      ..READ FROM TAPE TO MEMORY
        ,T'J',A(TPWIND)      ..REWIND TAPES
        ,T'W',A(TPWRIT)      ..WRITE ONTO TAPE FROM MEMORY
        ,T'E',A(EDITOR)      ..START THE EDITOR
        ,T'A',A(ASSEMB)      ..START THE ASSEMBLER
        ,T'B',A(BASIC)       ..START BASIC
        ,T'Z',A(PROMIT)      ..START CPU AT #E000,P&X=0
        ,T'C',A(TPSCRN)      ..COPY FROM TAPE TO SCREEN
        ,0

..**************************************************************
..                  UTILITY ERROR MESSAGE
..           NOTE: ENTRY HERE RESETS STACK TO TOP

..REG USED:  CHAR
..**************************************************************
ERROR:  LDI A.0(TOPSTK);PLO SP
        LDI A.1(TOPSTK);PHI SP
        SEP CALL;,A(OSTRNG)
        ,A(CRLF),T'ERROR',0
PRMPT1: LBR RENTER

..*********************************************************
..                  START OF SUBROUTINES
..
..************************************************************
..              MONITOR SUBSTITUTE FUNCTION
..      DISPLAYS THE FIRST BYTE FROM THE ADDRESS GIVEN FOLLOWED
..      BY A HYPHEN.  IF A HEX PAIR IS ENTERED FOLLOWED BY A SPACE,
..      IT IS SUBSTITUTED FOR THE BYTE DISPLAYED, IF A SPACE IS
..      ENTERED THERE IS NO CHANGE. IN EITHER CASE THE DATA BYTE FROM
..      THE NEXT ADDRESS WILL THEN BE DISPLAYED. THE ROUTINE IS ENDED
..      BY ENTERING A RETURN.
..REG USED: ASL, SRC, CHAR
..***********************************************

SUBST:  SEP CALL;,A(READHX)..INPUT ADDRESS
        GHI ASL;PHI SRC    ..SAVE STRT ADDRESS
        GLO ASL;PLO SRC

DECODE: GHI CHAR;XRI LF      ..FIRST NON-HEX MUST BE
        BZ ADDOUT ..A LINEFEED OR
        XRI #07   ..TERMINATION OR
        BZ PRMPT1
        XRI #2D   ..A SPACE
        BZ OLDDTA
        BR ERROR         ..ELSE ERROR

ADDOUT: SEP CALL;,A(OSTRNG);,CR,0
        SEP CALL;,A(OUT1)

OLDDTA: SEP DELAY;,#17     ..WAIT TO FINISH READ
        LDN SRC ;PHI CHAR       ..STAY ON SAME LINE
        SEP CALL;,A(TYPE2)         ..HEX OUTPUT
        SEP CALL;,A(OSTRNG)        ..OUTPUT A HYPHEN
        ,T'-',0
        LDN SRC ;PLO ASL..COPY DATA FROM CELL INTO ASL

GETDTA: SEP CALL;,A(READHX)       ..GET ANY CHANGE
        GLO ASL;STR SRC    ..RESTORE THE DATA INTO THE CELL
        INC SRC            ..OPEN THE NEXT CELL
        BR DECODE       ..EXAMINE INPUT

OUT1:   SEP DELAY;,#17
        GHI SRC;PHI CHAR     ..ROUTINE TO OUTPUT A HEX PAIR
        SEP CALL;,A(TYPE2)         ..AND A SPACE
        GLO SRC;PHI CHAR
        SEP CALL;,A(TYPE2)
        SEP CALL;,A(OSTRNG);,SPACE,0
        SEP R5

.. **********************************************************
..       DELAY IS 2(1 + AUX.1(3 + @LINK)) & IS
..       USED BY TYPE, READ, AND TIMALC.  AUX.1
..       IS ASSUMED TO HOLD A DELAY CONSTANT =
..       ((BIT TIME OF TERMINAL)/
..       (20 * INSTR TIME OF COSMAC)) - 1.  THIS
..       CONSTANT CAN BE GENERATED AUTOMATICALLY
..       BY THE TIMALC ROUTINE.
..REG USED:  AUX, PC
..***********************************************************

        ORG UT62+#EA

DEXIT:  SEP DELAY; SEP DELAY    ..4 NOP'S
        SEP DELAY; SEP DELAY
        SEP PC                  ..RETURN

DELAY1: GHI AUX;SHR;PLO AUX     ..SHIFT OUT ECHO BIT

DELAY2: DEC AUX                 ..AUX.0 - BIT DELAY
        LDA PC                  ..LOAD CONSTANT

DELAY3: SMI 1                   ..LOOP UNTIL ZERO
        BNZ DELAY3
        GLO AUX
        BZ DEXIT                ..DONE ?
        DEC PC                  ..POINT BACK TO THE CONSTANT
        BR DELAY2             ..AND LOOP


..***********************************************
..       CALCULATED BIT TIME AND ECHO FLAG.  WAITS
..       FOR LF(NO ECHO) OR CR (ECHO) TO BE TYPED
..       IN.  ALSO SETS UP POINTER TO THE DELAY
..       ROUTINE.  AUX.1 ENDS UP HOLDING, IN THE
..       MOST SIGNIFICANT 7 BITS, THE DELAY CONSTANT
..       THE LEAST SIGNIFICANT BIT IS ZERO FOR ECHO
..       OR ONE FOR NO ECHO.
..REG USED:  AUX, CHAR
..***************************************************

TIMALC: GHI PC;PHI DELAY           ..DELAY SUBROUTINE ADDRESS
        LDI A.0(DELAY1);PLO DELAY
        LDI 0;PLO AUX;PLO CHAR
        B4 *           ..WAIT FOR THE START BIT
        BN4 *          ..WAIT FOR FIRST NON-ZERO
        LDI 3          ..DATA BIT. SET UP FOR 10
                       ..EXECUTIONS SO THAT THE
TC2:    SMI 1          ..ROUND-OFF IS MINIMAL
        BNZ TC2        ..SEE IF THE DATA CHANGED.
        GLO CHAR       ..BR IF IT HAD, ELSE LOOK
        BNZ ZERO       ..FOR A CHANGE TO ZERO
        B4 INCR        ..BRANCH IF NO
        INC CHAR       ..YES, SET SWITCH

ZERO:   B4 DAUX    ..LOOK FOR A CHANGE TO 1

INCR:   INC AUX    ..SET UP FOR 20 INST. LOOPS
        LDI 7
        BR TC2

DAUX:   DEC AUX; DEC AUX
        GLO AUX;ORI 1;PHI AUX
        SEP DELAY ;,#0C       ..WAIT
        BN4 WAIT       ..BR IF LF =] NO ECHO
        GHI AUX;ANI #FE;PHI AUX  ..CR =] ECHO

WAIT:   SEP DELAY ;,#26
        SEP R5

..**************************************************
..       READS ONE BYTE INTO CHAR.1.  WHEN ENTERED
..       VIA READAH, ANY HEX INPUT IS ASSEMBLED
..       INTO ASL AND DF =1, ELSE DF = 0 ON RETURN.
..       NOTE-DON'T FOLLOW READ WITH IMMEDIATE TYPE.
..       INSERT DELAY OR ENTER TYPE VIA TYPED.
..REG USED:  CHAR, AUX & ASL
..****************************************************

CKDEC:  ADI 7               ..CHECK FOR ASCII DECIMAL
        BDF NFND         ..OUT OF RANGE
        ADI #0A              ..SUBTRACT NET 30
        BDF FND

NFND:   ADI 0               ..SET DF = 0

REXIT:  GHI CHAR           ..PUT INPUT INTO D
        SEP R5                    .. & RETURN

READAH: LDI 0;SKP                 ..SKIP TO READ1 WITH D=0

READ:   GHI PC                   ..CONSTANT GREATER THAN 0

READ1:  PLO CHAR                ..SAVE ENTRY POINT

READ2:  LDI #80; PHI CHAR      ..BIT COUNT, ;SHR; UNTIL DF=1
        BN4 *          ..WAIT FOR END OF DATA BIT
        B4 *           ..WAIT FOR START BIT
        SEP DELAY;,#02         ..HALF BIT TIME DELAY

NOBIT:  SEX SP; SEX SP          ..DELAY

BIT:    GHI AUX;SHR                 ..ECHO ?
        BDF READ3        ..BR IF NO
        B4 OUTBIT      ..IS IT A 1 ? 
        SEQ                     ..NO
        BR NOECHO

OUTBIT: REQ                     ..YES
        BR NOECHO

READ3:  NOP; NOP                ..DELAY

NOECHO: SEP DELAY;,#07         ..ONE BIT TIME DELAY
        NOP; NOP                ..MORE DELAY
        GHI CHAR;SHR;PHI CHAR   ..NEXT BIT
        BDF NEXT
        ORI #80
        BN4 NOBIT      ..BR IF INPUT AS ZERO
        PHI CHAR
        BR BIT                ..CONTINUE

NEXT:   REQ                     ..OUTPUT STOP BIT
        BZ READ2        ..IF D=0, CHAR.1 IS A NULL
        GLO CHAR             ..CHECK ENTRY
        BNZ REXIT       ..ENTERED VIA READ

CKHXE:  GHI CHAR;SMI #41     ..CHECK FOR ASCII HEX
        BNF CKDEC       ..CHECK FOR ASCII DECIMAL
        SMI 6                      ..A THRU F
        BDF NFND         ..NO
        ADI #10                    ..SUBTRACT NET 37

FND     PLO AUX            ..SAVE TEMPORARILY
        GHI ASL             ..SHIFT DATA INTO ASL
        SHL;SHL;SHL;SHL;STR SP      ..SHL 4X
        GLO ASL
        SHR;SHR;SHR;SHR
        OR;PHI ASL
        GLO ASL
        SHL;SHL;SHL;SHL;STR SP
        GLO AUX;ANI #0F
        OR;PLO ASL
        SMI 0                      ..SET DF = 1
        BR REXIT

..***********************************************
..       TYPES ONE BYTE FROM CHAR.1 AS AN ASCII
..       CHARACTER OR AS TWO HEX DIGITS.  LINE-FEEDS
..       ARE FOLLOWED BY SIX NULLS.  USES REGISTERS
..       AUX AND CHAR AND A STACK LOCATION.  TYPED
..       & TYPE5D ALLOW AN EARLIER READ TO COMPLETE.
..
..       AUX.0 HOLDS OUTPUT CHARACTER (AT FIRST),
..       THEN THE DELAY CONSTANT BETWEEN BITS.
..       CHAR.0 HOLDS THE NUMBER OF BITS (11) IN
..       ITS LOWER DIGIT AND THE FOLLOWING CODE IN
..       ITS UPPER DIGIT:
..             0 -  BYTE OUTPUT
..             1 - FIRST HEX OUTPUT
..             2 - LAST NULL OUTPUT
..             8 - LF OUTPUT
..
..REG USED:  CHAR, AUX
..************************************************************

                ORG UT62+#0198

TYPED:  SEP DELAY;,#17       ..THREE BIT TIME DELAY 
        BR TYPE

TYPE5D: SEP DELAY;,#17       ..THREE BIT TIME DELAY
        SKP

TEXIT:  SEP R5

TYPE5:  LDA R5                    ..PICK UP DATA
        SKP

TYPE6:  LDA R6                   ..PICK UP DATA
        SKP

TYPE:   GHI CHAR;PLO AUX           ..KEEP A COPY
        XRI LF                 ..IS IT A LINE-FEED ?
        BNZ TY5
        LDI #8b                ..# BITS + # NULLS
        BR  TY6

TYPE2:  GHI CHAR;SHR;SHR;SHR;SHR
        ADI #F6               ..CONVERT TO HEX
        BNF TY4         ..IF A OR MORE, ADD 37
        ADI 7

TY4:    SMI #C6;PLO AUX          ..ELSE ADD 30
        LDI #1B                   ..10 ADI  NO. OF BITS
        LSKP

TY5:    LDI #0B               ..NO OF BITS

TY6:    PLO CHAR
        GLO ASL;STXD           ..SAVE USER'S RD.0

BEGIN:  SEQ                     ..START BIT
        GLO AUX;PLO ASL          ..COPY CHAR TO RD.0

PREBIT: SEP DELAY;,#07         ..1 BIT TIME DELAY
        DEC CHAR                ..DEC BIT COUNT
        SD                      ..SET DF = 1
        GLO ASL;SHRC;PLO ASL
        BDF OUT1A        ..BR IF BIT IS A ONE
        SEQ                     ..ELSE OUTPUT A ZERO
        BR OUT1B

OUT1A:  REQ; REQ                ..OUTPUT A ONE AND DELAY

OUT1B:  GLO CHAR;ANI #0F       ..DONE ?
        NOP;NOP;NOP;NOP         ..DELAY
        BNZ PREBIT       ..BR IF NOT

NXCHAR: GLO CHAR;ADI #FB;PLO CHAR
        BNF FIXED       ..SEP R5 IF NO MORE
        SMI #1B         ..TEST FOR ALTERNATIVES
        BZ FIXED        ..TYPED LAST NULL
        BNF HEX1        ..TYPED FIRST HEX
        LDI 0           ..TYPED LF OR NULL
        BR HEX3

HEX1:   GHI CHAR;ANI #0F ..GET 2ND HEX DIGIT
        ADI #F6          ..CONVERT TO HEX
        LSNF             ..IF A OR MORE,
        ADI 7            ..ADD NET 36

HEX2:   SMI #C6          ..ELSE ADD NET 30

HEX3:   PLO AUX          ..AND SAVE
        BR BEGIN

FIXED:  INC SP
        LDN R2;PLO ASL
        BR TEXIT         ..RESTORE USER'S RD.P

..*****************************************************
..                INPUT OPTION
..       ALLOWS ENTRY OF EITHER STARTING AND ENDING
..       ADDRESSES OR BYTE COUNT.EXITS WITH THE
..       STARTING ADDRESS IN REG SRC AND THE BYTE
..       COUNT IN REG CNT.  RETURNS WITH  DF =1
..       IF SYNTAX ERROR EXISTS
..REG USED:  ASL, SRC, CHAR, CNT
..*****************************************************

OPTION: SEP CALL;,A(READHX)     ..GET THE STARTING ADDRESS
        GHI ASL;PHI SRC         ..AND SAVE IT
        GLO ASL;PLO SRC
        LDI 0;PLO ASL;PHI ASL   ..CLEAR THE INPUT REG.
        GHI CHAR;XRI SPACE       ..FIRST NONSMI HEX MUST
        BZ CNTIN        ..BE A SPACE OR
        XRI #0D         ..A HYPHEN
        BNZ PRMPT2      ..ELSE A SYNTAX ERROR
        SEP CALL;,A(READHX) ..EXPECT ENDING ADDRESS

BYTCNT: GLO  SRC;STR SP              ..CALCULATE THE BYTE COUNT
        GLO ASL;SM;PLO CNT
        GHI SRC;STR SP
        GHI ASL;SMB;PHI CNT
        BDF EXITOK         ..CHECK FOR SRC LESS THAN  ASL
INVERT: GLO ASL;STR SP          ..ELSE EXCHANGE THE CONTENTS OF
        GLO SRC;PLO ASL         ..SRC AND ASL

        LDN SP;PLO SRC
        GHI ASL;STR SP
        GHI SRC;PHI ASL
        LDN SP;PHI SRC
        BR BYTCNT            ..RECALCULATE

CNTIN:  SEP CALL;,A(READHX)  ..INPUT THE BYTE COUNT
        GLO ASL;SMI 1;PLO CNT
        GHI ASL;SMBI 0;PHI CNT
        BDF EXITOK
        INC CNT
EXITOK: SEP R5                    ..RETURN WHEN DONE

..*********************************************************
..                      FILL ROUTINE
..       LOADS MEMORY BEGINNING AT ADDRESS CONTAINED
..       IN SRC WITH DATA CONTAINED IN ASL.0 FOR
..       THE NUMBER OF BYTES SPECIFIED BY CNT.
..       USER CALLABLE % USRFIL.
..REG USED:  ASL, SRC, CNT ,CHAR
..*********************************************************

FILL:   SEP CALL;,A(READAD)     ..GET THE ADDRESSES
        SEP CALL;,A(USRFIL)     ..CALL THE MOVE

PRMPT2: LBR RENTER              ..GOTO UT62 AND PROMPT

NXTCEL: INC SRC                 ..POINT TO NEXT CELL
        DEC CNT                 ..REDUCE BYTE COUNT
USRFIL: GLO ASL;STR SRC         ..LOAD THE DATA;USER ENTRY PT.
        GLO CNT                 ..LOOP UNTIL COUNT = 0
        BNZ NXTCEL
        GHI CNT
        BNZ NXTCEL
        SEP R5                  ..EXIT THE CALL

..***********************************************************
..            ROUTINE TO READ FROM TAPE TO SCREEN
..      AFTER EACH 20 RETURNS, ROUTINE OUTPUTS "****" AND
..      WAITS FOR BREAK KEY TO BE HIT TO CONTINUE.
..      HALTS ON BREAK, EXITS ON Q OR CONTINUE ON OTHER
..***********************************************************
TPSCRN: SEP CALL;,A(FINDTP)     ..GET TAPE #
        GHI RC;STXD             ..SAVE IOCB
        GLO RC;STXD
        SEP CALL;,A(OSTRNG),LF,0..OUTPUT A LINEFEED
        INC SP                  ..RESTORE IOCB
        LDA SP;PLO RC
        LDN SP;PHI RC
        LDI LINES;PLO CNT       ..INITIALIZE FOR 20 LINES
 
MORLST: SEP CALL;,A(READT)      ..GET A BYTE

CHKOUT: GHI RC;STXD
        GLO RC;STXD
        GHI CHAR;XRI EOF
        BZ PRMPT2                ..WE'RE DONE
        XRI #1E                 ..ASSEMBLY LISTINGS DON'T HAVE
        BNZ SHOWIT
        SEP CALL;,A(OSTRNG)        ..LINE FEEDS
        ,A(CRLF),0
        DEC CNT;GLO CNT            ..DID 20 RETURNS GO BY ?
        BNZ NXTLNE
        SEP CALL;,A(OSTRNG)
        ,T'****',0             ..PROMPT FOR A KEY WHEN READY
        SEP CALL;,A(READ)
        SEP CALL;,A(OSTRNG)
        ,A(CRLF),0
        LDI LINES;PLO CNT

NXTLNE: INC SP
        LDXA;PLO RC
        LDN SP;PHI RC
        SEP CALL;,A(READT)
        XRI LF
        BZ MORLST
                              ..IT ISN'T A LINE FEED SO
        BR CHKOUT             ..SEE WHAT IT IS

SHOWIT: LDI A.0(DELAY1);PLO DELAY
        LDI A.1(DELAY1);PHI DELAY
        SEP CALL;,A(TYPE)     ..PUT IT TO THE TERMINAL
        B4 MRLST1     ..HALT ON BREAK ASSERTED
        SEP CALL;,A(READ)
        XRI T'Q'      ..EXIT ON Q OR CONTINUE ON
        BZ PRMPT2     ..ANYTHING ELSE

MRLST1: INC SP                  ..RESTORE IOCB POINTER
        LDXA;PLO RC
        LDN SP;PHI RC
        BR MORLST

..****************************************************************
..                      OUTPUT
..       FORMATS AND OUTPUTS MEMORY DATA BEGINNING
..       AT THE  ADDRESS IN REG SRC FOR THE NUMBER
..       OF BYTES SPECIFIED IN REG CNT
..REG USED:  SRC, CNT, CHAR
..****************************************************************

DISPLY: SEP CALL;,A(OPTION)     ..GET STARTING ADDRESS
        XRI CR                  ..TERMINATE WITH CR
        LBNZ ERROR

OUTPUT: SEP CALL;,A(OSTRNG),LF,0..START ON A NEW LINE
        SEP CALL;,A(OUT1)      ..OUTPUT THE ADDRESS
                               ..OF THE CURRENTLY OPENED
                               ..CELL

SPCOUT: SEP CALL;,A(OSTRNG)
        ,SPACE,0

DATOUT: LDA SRC;PHI CHAR       ..RETRIEVE THE CELL DATA
        SEP CALL;,A(TYPE2)     ..AND OUTPUT IT
        GLO CNT
        BNZ NOTDON             ..REQUESTED NO. OF BYTES
        GHI CNT                ..HAVE BEEN SENT
        BZ PRMPT2              ..GET A NEW COMMAND

NOTDON: DEC CNT                ..DEC THE BYTE COUNT
        GLO SRC;ANI LNECNT
        BNZ SAMELN             ..END OF CURRENT LINE?
        SEP CALL,A(OSTRNG)
        ,T';',CR,0
        BR OUTPUT

SAMELN: SHR
        BDF DATOUT             ..WITHIN PAIR
        BR SPCOUT              ..ELSE BETWEEN PAIRS

..**************************************************************
..      READ 1 CHARACTER FROM TAPE AND DF=1 IF HEX
..**************************************************************

RDHEX:  SEP CALL;,A(READT)
        SEP CALL;,A(CKHXE)
        SEP R5

..**************************************************************
..              UT62 MOVE COMMAND
..              CALLS USRMOV AND REQUESTS SRC&DEST ADDR'S
..**************************************************************
MOVE:   SEP CALL;,A(READAD)     ..GET SRC&DEST ADDR'S
        SEP CALL;,A(USRMOV)     ..DO THE MOVE
        LBDF ERROR              ..ERROR IF OVER FFFF ON MOVE
        LBR PRMPT2               ..IF OK,GOTO UT62 PROMPT 
..*********************************************************
..    SUBROUTINE TO GET THE ADDRESSES FOR OTHER ROUTINES
..*********************************************************
READAD: SEP CALL;,A(OPTION)     ..DETERMINE THE MODE
        XRI SPACE               ..MUST BE A SPACE
        BNZ ERR1                ..ELSE ERROR
        PLO ASL;PHI ASL         ..CLEAR INPUT REGISTER
        SEP CALL;,A(READHX)     ..INPUT THE CONSTANT
        XRI CR                  .. 'CR' TERMINATES
        BNZ ERR1                ..ELSE ERROR
        SEP R5
..*****************************************************
..      FILLS ASL UNTIL A CARRIAGE RETURN IS ENTERED
..*****************************************************
READCR: SEP CALL;,A(READAH)
        XRI CR
        BNZ READCR
        SEP R5
..**************************************************************
..              READ A BIT OF DATA FROM TAPE
..              ENTERED WITH SET P TO BITRDR
..**************************************************************

RBYE:   SEP PC

RDBIT:  LDI #16         ..0/1 BIT DISTINGUISH TIMER

..**************************************************************
..                              FOR 2 HMZ CRYSTAL
..                      THE TIMING LOOP USED GIVES 22X24US DELAY(528)
..                      TO TEST FOR A 1/0 TRANSITION DATA FROM TAPE
..                      TYPICALLY HAS A 350/750US DIFFERENCE. (0/1)
..**************************************************************

        BN2 *           ..WAIT FOR TRANSITION
        SEP BITRDR      ..DELAY AND THEN RETEST
        BN2 RDBIT       ..TO BE SURE
HERE:   BN2 SLIDE1      ..TIME OUT LOOP 
HERE1:  SMI 1; BNZ HERE

HERE2:  INC PARITY      ..SET BIT READ AS A ONE
        GHI PC

SLIDE2: SHL             ..PUT BIT IN DF
SLIDE3: B2*             ..WAIT FOR BIT TO END
        SEP BITRDR      ..DELAY AND RETEST TO
        B2 SLIDE3       ..BE SURE
        BR RBYE

SLIDE1: SMI 1           ..DECREMENT LOOP COUNT AND RETEST
        BN2 SLIDE2      ..BRANCH IF STILL FALSE TO SET BIT A ZERO
        BNZ HERE1       ..IF TIME OUT GO TO SET BIT A ONE
        BR HERE2        ..ELSE CONTINUE TIMEOUT

..**************************************************************
                ORG UT62+#035D

PRMPT5: LBR RENTER
ERR1:   LBR ERROR               ..GENERAL FOR THIS PAGE

..**************************************************************
..DESC:  STANDARD CALL AND RETURN
..REG USED:  SP,PC,CALL,RETURN,LINK & STACK
..**************************************************************
                  ORG UT62+#0363
                .. STANDARD CALL

EXITC:  SEP PC                  ..GO TO IT

CALLR:  SEX SP                  ..SET R(X)
        GHI LINK;STXD           ..SAVE THE CURRENT LINK ON
        GLO LINK;STXD           ..THE STACK
        GHI PC;PHI LINK
        GLO PC;PLO LINK
        LDA LINK;PHI PC         ..PICK UP THE SUBROUTINE
        LDA LINK;PLO PC         ..ADDRESS
        BR EXITC
                ..STANDARD RETURN

EXITR:  SEP PC                  ..RETURN TO MAIN PGM.
        
RETR:   GHI LINK;PHI PC
        GLO LINK;PLO PC
        SEX SP;INC SP           ..SET THE STACK POINTER
        LDXA;PLO LINK           ..RESTORE THE CONTENTS OF
        LDX;PHI LINK         ..LINK
        GHI CHAR                ..PUT THE CONTENTS OF CHAR.1 INTO D
                                ..BEFORE RETURNING
        BR EXITR

..*********************************************************************
..               REGISTER INITIALIZATION ROUTINE
..      INITIALIZES REGISTER C TO THE DELAY ROUTINE, REG 2 AS A STACK
..      POINTER TO LOCATION 8CFF HEX, REG 4 TO CALL, REG 5 TO RETURN
..      AND REG 3 AS PROGRAM COUNTER. FOR ENTER1 REG 3 IS 0005, FOR
..      ENTER2 REG 3 MUST BE PRESET.
..      REG USED:  PC, DELAY, CALL, RETURN, SP
..*********************************************************************

INIT:   LDI A.0(START);PLO PC
        LDI A.1(START);PHI PC
        BR ENTER2

ENTER1: LDI A.0(PGMSRT);PLO PC   
        LDI A.1(PGMSRT);PHI PC

ENTER2: LDI A.0(DELAY1);PLO DELAY   ..DELAY ROUTINE
        LDI A.1(DELAY1);PHI DELAY
        LDI A.1(CALLR);PHI CALL;PHI RETN
        LDI A.0(CALLR);PLO CALL
        LDI A.0(RETR);PLO RETN
        LDI A.0(TOPSTK);PLO SP
        LDI A.1(TOPSTK);PHI SP
        SEX SP;SEP PC

..***********************************************************
..              HEX BYTE INSERT ROUTINE
..      INSERTS HEX PAIRS INTO MEMORY STARTING AT A SPECIFIED
..      ADDRESS. AFTER A ";" ALL IS IGNORED UNTIL A RETURN
..      THEN A NEW ADDRESS IS EXPECTED. ANY NON-HEX DATA IS
..      IGNORED BETWEEN HEX PAIRS BUT NOTHING IS PERMITTED
..      BETWEEN MEMBERS OF THE PAIR. ROUTINE IS TERMINATED
..      WITH A RETURN, EXCEPT AFTER A ";".
..      REG USED:  ASL, SRC, CHAR
..***********************************************************

INSERT: LDI 0; PHI ASL;PLO ASL  ..CLEAR INPUT REGISTER

FSTHEX: SEP CALL;,A(READHX)     ..WAIT FOR FIRST NON-HEX INPUT
        XRI SPACE               ..FIRST NON-HEX MUST BE
        BNZ ERR1                ..A SPACE
        GHI ASL;PHI SRC         ..STORE STARTING ADDRESS
        GLO ASL;PLO SRC        

NXTCHR: SEP CALL;,A(READAH)     ..GET FIRST CHARACTER
        BNF NTDATA
        SEP CALL;,A(READAH)     ..OTHERWISE SECOND ONE MUST BE HEX
        BNF ERR1
        GLO ASL;STR SRC         ..STORE THE DATA
        INC SRC                 ..POINT TO THE NEXT CELL
        BR NXTCHR            .. & CONTINUE

NTDATA: XRI CR                  ..DONE IF CR
        BZ PRMPT5
        GHI CHAR
        XRI SEMCOL              ..IF ";" WAIT FOR CRLF
        BNZ COMCHK              ..ELSE CHECK IF ","
        SEP CALL;,A(CKCRLF)
        BR INSERT               ..GO GET NEW ADDRESS

COMCHK: GHI CHAR                ..CHECK FOR ","
        XRI COMMA
        BNZ NXTCHR              ..IGNORE IF NOT , ; CR
        SEP CALL;,A(CKCRLF)     ..WAIT FOR CRLF
        BR NXTCHR

CKCRLF: SEP CALL;,A(READ)       ..GET A CHAR
        XRI CR                  ..IS IT CR?
        BNZ ERR1                ..BETTER BE!
        SEP CALL;,A(READ)       ..GET NEXT CHAR
        XRI LF                  ..IS IT LF?
        BNZ ERR1                ..ERROR IF NO!
        SEP R5                  ..EXIT IF ALL OK

..************************************************
..              UTILITY ENTRY TABLE
..************************************************
           ORG UT62+#03F0

OSTRNG: LBR MSGE
INIT1:  LBR ENTER1
INIT2:  LBR ENTER2
GOUT62: LBR RENTER
CKHEX:  LBR CKHXE

..********************************************************
                ..TAPES OFF ROUTINE
TPOFF:  SEX PC
        OUT BDSEL;,TAPEIO       ..SELECT TAPE BD
        OUT TPESEL;,TPEOFF      ..OUTPUT OFF BITS
        OUT BDSEL;,TRMINL       ..SELECT RCA GRP
        SEP R5                  ..RETURN TO COLLER

..***************************************************************
..              ..REGISTER SAVE ROUTINES
..            SAVES THE CONTENTS OF REGISTERS TMPRG1 (7),
..            TMPRG2 (8), TMPRG3 (9) AND CNT (A) ON THE STACK.
..            USED 8 BYTES OF STACK, AND LEAVES STACK POINTER
..            AT A FREE BYTE
..****************************************************************

REGSAV: GLO CNT;STXD
        GHI CNT;STXD
        GLO TMPRG3;STXD
        GHI TMPRG3;STXD
        GLO TMPRG2;STXD
        GHI TMPRG2;STXD
        GLO TMPRG1;STXD
        GHI TMPRG1;STXD
        GHI CHAR       ..TO COMPENSATE FOR BASIC CALL
        SEP R5

..*******************************************************************
..              OSTRNG
..*******************************************************************

MSGE:   LDI #EF;PLO DELAY
        LDI #80;PHI DELAY

MSGE1:  LDA LINK;PHI CHAR
        BZ EXIM
        SEP CALL;,A(TYPED)
        BR MSGE1

EXIM:   SEP R5

..**************************************************************
..              MOVE ROUTINE
..      COPIES A BLOCK OF MEMORY FROM ONE CONTINUOUS AREA
..      TO ANOTHER CONTINUOUS AREA IN MEMORY. THERE IS NO
..      RESTRICTION AS TO THE DIRECTION OF THE MOVE AND THE
..      AREAS MAY OVERLAP.
..      REG USED:  SRC, DEST, CHAR, & CNT
..*************************************************************

USRMOV: SEX SP
        GLO SRC;STR SP          ..TEST THE RELATIVE POSITION
        GLO DEST;SM             ..OF SOURCE & DESTINATION
        BNZ DIRECT              ..NOT EQUAL!
        GHI SRC;STR SP          ..RETURN IF THEY ARE EQUAL
        GHI DEST;SMB
        BZ USRBYE               ..EXIT TO CALLER

DIRECT: GLO SRC;STR SP          ..ELSE TEST FOR UP OR DOWN
        GLO DEST;SM             ..DIRECTION OF THE MOVE
        GHI SRC;STR SP
        GHI DEST;SMB
        BDF MOVUP

MOVDN:  LDN SRC;STR DEST        ..DO THE MOVE DOWN AND
        GLO CNT                 ..AND CHECK IF DONE
        BNZ MOVDN1
        GHI CNT
        BZ USRBYE               ..EXIT TO CALLER

MOVDN1: INC SRC;INC DEST        ..ADJUST THE POINTERS
        DEC CNT                 ..REDUCE THE BYTE COUNT
        BR MOVDN                ..FINISHED

MOVUP:  GLO  CNT;STR SP         ..SET THE POINTERS TO THE
        GLO SRC;ADD;PLO SRC     ..TOP OF MOVE AREAS
        GHI CNT;STR SP
        GHI SRC;ADC;PHI SRC
        GLO CNT;STR SP
        GLO DEST;ADD;PLO DEST
        GHI CNT;STR SP
        GHI DEST;ADC;PHI DEST
        BNF UP

ERRGO:  BR USRBYE+1             ..EXIT DF=1 IF OVERFLOW

UP:     LDN SRC;STR DEST        ..DO THE MOVE UP
        GLO CNT                 ..AND CHECK IF DONE
        BNZ UP1
        GHI CNT
        BZ USRBYE               ..EXIT TO CALLER

UP1:    DEC SRC;DEC DEST;DEC CNT..ADJUST THE POINTERS
        BR UP

USRBYE: SHR                     ..SET DF=0 IF A[FFFF
        SEP R5                  ..EXIT TO CALLER

..***************************************************************
..
..LOADS USER MEMORY FROM TAPE, ASSUMES TAPE HAS "M" OR "I" TO
..INDICATE START OF DATA TO BE STORED. "P" FOLLOWED BY AN ADDRESS
..STARTS PROGRAM EXECUTION WITH P=0 AND X=0.
..***************************************************************

TPLOAD: SEP CALL;,A(FINDTP)     ..SELECT TAPE
        GHI RC;STXD
        GLO RC;STXD
        SEP CALL;,A(OSTRNG)
        ,LF,T'LOADING',0
        INC SP
        LDXA;PLO RC
        LDN SP;PHI RC

READZ:  SEP CALL;,A(READT)      ..READ 1 ASCII DIGIT
        XRI T'P'                ..CHECK FOR PROGRAM EXECUTE
        BZ ADLP
        XRI #1D                 ..SET MEMORY?
        LSZ
        XRI $04                 ..INSERT MEMORY?
        BZ READX1
        XRI #5A                 ..CHECK FOR EOF(DC3)
        BZ PRMPT4
        BR READZ

                ..READ THE HEX ADDRESS

READX1: SEP CALL;,A(RDHEX) ..IGNORE ALL TILL HEX DIGIT
        BDF READX2          ..THEN GET SECOND ONE
        XRI #2E             ..UNLESS "." WHICH MEANS COMMENT
        BNZ READX1
READXA: SEP CALL;,A(READT)  ..THEN IGNORE ALL TILL END OF LINE
        XRI CR
        BNZ READXA
        BR READX1

READX2: SEP CALL;,A(RDHEX) ..READ NEXT ASCII CHARACTER
        BDF READX2       ..IF HEX KEEP READING
        XRI SPACE        ..OTHERWISE MUST BE A SPACE
        BNZ ERRORX
        GHI ASL;PHI ADRPTR
        GLO ASL;PLO ADRPTR

READX3: SEP CALL;,A(RDHEX) ..READ NEXT ASCII CHARACTER
        BDF READXB       ..IF HEX THEN IT'S DATA & NEED ANOTHER
        XRI CR           ..OR COULD BE END OF LINE
        BZ READZ         ..THEN START AGAIN
        XRI #36          ..OR IF ";" THEN IT IS END OF DATA
        BZ READXA        ..FOR THIS LINE
        BR READX3             ..CAN ONLY BE MORE DATA

READXB: SEP CALL;,A(RDHEX) ..READ 2ND ASCII CHARACTER
        BNF ERRORX          ..BETTER BE A HEX DIGIT
        GLO ASL;STR ADRPTR  ..STORE AT THE SPECIFIED ADDRESS
        SEX ADRPTR;XOR      ..MAKE SURE THERE WAS RAM THERE
        BZ WRTOK
        SEP CALL;,A(NOTRAM)

WRTOK:  INC ADRPTR
        BR READX3

                ..ROUTINE TO START A PROGRAM RUNNING

ADLP:   SEP CALL;,A(RDHEX)  ..GET THE START ADDRESS
        BDF ADLP
        LBR RUN1

                ..ERROR MESSAGE

ERRORX: SEP CALL;,A(OSTRNG)
        ,A(CRLF),T'FORMAT ERROR',0
PRMPT4: LBR RENTER

..*************************************************************
..      USER ENTRY POINTS FOR TAPE ROUTINES
..*************************************************************
                  ORG #8500

        BR TWRITE             ..ENTRY TO TAPE WRITE

        BR READT              ..ENTRY TO TAPE READ

..*************************************************************
..              TAPE REWIND ROUTINE
..*************************************************************

REWIND: SEP CALL;,A(OSTRNG)
        ,A(CRLF),T'REWIND,THEN HIT ANY KEY',0
        SEX PC
        OUT BDSEL;,TAPEIO
        OUT TPESEL;,BTHDRV
        OUT BDSEL;,TRMINL
        SEP CALL;,A(READ)    ..WAIT FOR KEY
        LBR TPOFF

TPWIND: SEP CALL;,A(REWIND)  ..FOR THE UTILITY CALL "J"
        LBR RENTER

..*******************************************************
..      STARTS A USER PROGRAM WITH SPECIFIED ADDRESS
..      IN REGISTER 0, AND X=0. THE SCREEN IS CLEARED
..      AND THE CURSOR IS HOMED.
..REG USED:  CHAR, ASL, R0
..*************************************************

RUN:    SEP CALL;,A(READHX)     ..LOOK FOR STARTING ADDRESS
        XRI CR                  ..FIRST NON-HEX MUST BE A
        LBNZ ERROR              ..CR, ELSE SYNTAX ERROR

RUN1:   GHI ASL;PHI R0          ..GET THE ADDRESS
        GLO ASL;PLO R0
        SEX R0
        SEP R0                  ..AND GO!

..********************************************************************
..                      READ TAPE ROUTINE
..      INITIALLY READS ONE BLOCK OF 384 BYTES INTO A BUFFER FROM TAPE
..      THEN EACH CALL TO IT PUTS ONE BYTE INTO CHAR.1 AND DECREMENTS
..      DOWN THE BUFFER UNTILL IT IS EMPTY THEN GETS NEXT BUFFER LOAD
..      USES A 3 BYTE BUFFER WHICH WHEN NOT IN USE IS STORED ON STACK
..              LOWEST ADDRESS IS BLOCK COUNT
..              NEXT IS DRIVE # IN TOP2 BITS AND MSB OF BYTE COUNT
..              HIGHEST IS REST OF BYTE COUNT
..      PTR (REGISTER C) NORMALLY POINTS AT THE HIGHEST BYTE
..  %%% NOTE: REGISTER C MUST CONTAIN THE IOCB ADDRESS AS SET UP
..            IN THE ROUTINE FINDTP.  %%%
..********************************************************************

READT:  GHI PC;STXD             ..INIT PARITY FLAG
        LDN PTR                 ..IF BYTE CNT GREATER THAN 0, THEN JUST PUT IT
        BNZ EXRD                ..IN CHAR.1
        DEC PTR
        LDN PTR;SHR             ..TEST HI BIT OF IOCB
        BDF EXRD1
        LDN PTR;ORI 1           ..SET BYTE COUNT TO MAXIMUM
        STR PTR
        INC PTR
        LDI #80;STR PTR
        SEP CALL;,A(RDBLOK)     ..GET NEXT BLOCK FROM TAPE
        BNF TAG1                ..IF NO PARITY ERR
        INC SP                  ..OTHERWISE SET PARITY FLAG
        LDI 0;STXD
        GHI PTR;STXD            ..SAVE IOCB!
        GLO PTR;STXD
        SEP CALL;,A(OSTRNG)
        ,A(CRLF),T'PARITY BAD'
        ,A(CRLF),0
        INC SP                  ..RESTORE IOCB
        LDA SP;PLO PTR
        LDN SP;PHI PTR
        SKP

EXRD1:  INC PTR

TAG1:   LDN PTR                 ..DEC BYTE COUNT IN THE IOCB
EXRD:   SMI  1;STR PTR
        PLO CHAR                ..CHAR POINTS INTO THE READ BUFFER
        DEC PTR
        LDN PTR;SMBI 0;STR PTR
        ANI 1;ADI A.1(RDBUFF-256);PHI CHAR
        INC PTR
        LDN CHAR;PHI CHAR       ..PUT THE BYTE INTO CHAR.1
        INC SP                  ..SET PARITY FLAG (1=PASS)
        LDN SP;SHL
        SEP R5

..******************************************************************
..      WRITE TAPE ROUTINE
..      PUTS BYTE FROM CHAR.1 INTO BLOCK BUFFER.
..      WHEN BUFFER FULL,WRITES IT TO SELECTED TAPE.
..       NOTE: REGISTER C MUST CONTAIN THE IOCB ADDRESS
..******************************************************************

TWRITE: GHI CHAR;STR SP         ..SAVE BYTE
        LDN PTR;ADI A.0(WRTBUF);PLO CHAR
        DEC PTR                 ..CHAR POINTS TO NEXT FREE
        LDN PTR;ANI 1           ..BUFFER LOCATION
        ADCI A.1(WRTBUF);PHI CHAR
        LDN SP;STR CHAR         ..WRITE BYTE TO BUFFER
        INC CHAR
        INC PTR                 ..PT TO IOCB3
        LDN PTR;ADI 1;STR PTR   ..INC BYTE CNT
        DEC PTR
        LDN PTR;ADCI 0;STR PTR
        INC PTR
        GHI CHAR;XRI #90        ..BUFF FULL?
        BNZ EXWT
        SEP CALL;,A(WRBLOK)     ..ELSE,WRITE BUFFER TO TAPE
        DEC PTR;DEC PTR         ..PT TO BLK CNT
        LDN PTR;ADI 1;STR PTR   ..INC CNT
        INC PTR                 ..POINT TO BYTE COUNT
        LDN PTR;ANI #FE         ..SET IT TO 0
        STR PTR;INC PTR
        LDI 0;STR PTR

EXWT:   SMI 0
        SEP R5

..**************************************************************
..      MESSAGE FOR NO RAM AT A LOCATION FOR STORE OF A READ
..**************************************************************

NOTRAM: GHI RC;STXD
        GLO RC;STXD
        SEP CALL;,A(OSTRNG)
        ,A(CRLF),T'NO RAM @',0
        GHI ADRPTR;PHI CHAR     ..TELL HIM THE ADDRESS
        SEP CALL;,A(TYPE2)
        GLO ADRPTR;PHI CHAR
        SEP CALL;,A(TYPE2)
        INC SP
        LDXA;PLO RC
        LDN SP;PHI RC
        SEP R5
..****************************************************************
..              ROM ENTRY FROM UTILITY
..****************************************************************
EDITOR: LDI AEDIT;PHI R0
        BR GOADD

ASSEMB: LDI AASM;PHI R0
        BR GOADD

BASIC:  LDI ABAS;PHI R0
        BR GOADD

PROMIT: LDI PRMPGR;PHI R0

GOADD:  LDI  0;PLO R0
        SEX R0
        SEP R0                  ..GO TO ROM

..*******************************************************************
..              WRITE BLOCK ROUTINE
..      WRITES A BLOCK OF 384 BYTES TO TAPE   
..*******************************************************************

WRBLOK: SEP CALL;,A(REGSAV)
        GLO RB;STR SP             ..NEED EXTRA STORAGE
        LDI A.0(WRTBUF);PLO BUFPTR..POINT TO WRITE BUFFER   
        LDI A.1(WRTBUF);PHI BUFPTR
        PLO BLKFLG                ..INIT BLOCK FLAG 
        LDI A.0(WRBIT);PLO BITWRT
        LDI A.1(WRBIT);PHI BITWRT
        SEX PC
        OUT BDSEL;,TAPEIO
        SEX PTR;DEC PTR         ..POINT TO DRIVE #
        OUT TPESEL              ..SELECT DRIVE(S)
        DEC PTR;DEC PTR         ..PT TO BLOCK COUNT
        LDN PTR
        BNZ LDR2
        LDI #C0;LSKP            ..COUNT FOR INITIAL LEADER
LDR2:   LDI #10                 ..COUNT FOR INTER BLOCK LEADER
        PHI LDRCNT
LEADER: ADI 0                   ..SET DF=0 TO WRITE A LOW
        SEP BITWRT              ..WRITE ONE ZERO
        DEC LDRCNT
        GHI LDRCNT
        BNZ LEADER
SET1:   LDI #10;PLO PARITY      ..PRESET PARITY EVEN
        LDI 8;PLO BITCNT        ..PRESET BIT COUNTER
        GLO BLKFLG
        BZ NOTBLK
        LDA PTR;PHI BYTSTR      ..STORE THE BYTE TO BE WRITTEN
        LDI 0;PLO BLKFLG        ..CLEAR THE BLOCK FLAG
        LSKP                    ..IT IS THE BLOCK START

NOTBLK: LDA BUFPTR;PHI BYTSTR   ..STORE FIRST BYTE
        SMI 0
        SEP BITWRT              ..WRITE START BIT

SET:    GHI BYTSTR;SHR          ..MOVE EACH BIT INTO DF FOR
        PHI BYTSTR              ..OUTPUT
        SEP BITWRT
        DEC BITCNT
        GLO BITCNT
        BNZ SET                 ..DO IT FOR ALL BITS
        INC PARITY              ..SET ODD PARITY
        GLO PARITY;SHR
        SEP BITWRT              ..WRITE OUT PARITY
        GHI BUFPTR;XRI #90      ..BUFFER DONE?
        BNZ SET1
        SEP BITWRT              ..REPEAT PARITY FOR 2 BITS
        SEP BITWRT
        INC PTR                 ..PT TO IOCB#3
        LDN SP;PLO RB           ..REPLACE REG B
        
RESTOR: SEP CALL;,A(REGSTR)
        LBR TPOFF
..**************************************************************
..              ROUTINE TO WRITE A BIT ONTO TAPE
..              ENTERED WITH SEP TO BITWRTR  
..**************************************************************

BYE:    SEP PC

WRBIT:  SEX BITWRT
        LDI #FF;PLO FLAG        ..FLAG FOR SIGNAL HIGH (FF),
                                ..LOW (00)
        LDI #0C                 ..0 BIT TIMING
        BNF OUTAPE
        LDI #27                 ..1 BIT TIMING
        INC PARITY              ..INC PARITY CTR IF 1 BIT

OUTAPE: OUT TPEWRT;,HHIGH
        PHI CHAR                ..SAVE BIT TIMING IN CHAR.1
TEST1:  SMI 1                   ..DEC TIMER
        BNZ TEST1               ..KEEP HI TIL CNT = 0
        GLO FLAG
        BZ BYE          ..EXIT WHEN TAPE SIGNAL IS A LOW
        LDI 0;PLO FLAG  ..SET FLAG FOR A LOW
        OUT TPEWRT;,LLOW
        GHI CHAR        ..GET THE BIT TIMING
        BR TEST1

..****************************************************************
..                      READ BLOCK ROUTINE
..      READS ONE BLOCK, 384 BYTES, FROM TAPE INTO THE READ BUFFER
..      EXITS WITH DF AS PARITY FLAG, 0 IS GOOD
..****************************************************************

RDBLOK: SEP CALL;,A(REGSAV)
        LDI A.0(RDBUFF);PLO BUFPTR
        LDI A.1(RDBUFF);PHI BUFPTR
        PHI FLAG                ..INITIALIZE BLOCK FLAG TO SET
        LDI 0;PLO FLAG          ..AND PARITY FLAG TO GOOD
        LDI A.0(RDBIT);PLO BITRDR
        LDI A.1(RDBIT);PHI BITRDR
        SEX PC
        OUT BDSEL;,TAPEIO
        DEC PTR                 ..MOVE TO DRIVE # IN IOCB
        LDN PTR;SHL
        INC PTR                 ..BACK TO HOME IN IOCB
        BDF DR1
        OUT TPESEL;,DRIVE0
        LSKP

DR1:    OUT TPESEL;,DRIVE1

LDR1:   LDI #08;PHI LDRCNT

LEDER1: SEP BITRDR              ..GET 1 BIT
        BDF LDR1         ..MUST BE A 0 TO BE THE LEADER
        DEC LDRCNT
        GHI LDRCNT
        BNZ LEDER1       ..LOOP TIL LEADER COUNTER EMPTYS
 
STRT1:  SEP BITRDR       ..LOOP TIL START BIT
        BNF STRT1
        LDI 9;PLO BITCNT
        PLO PARITY

SHIFT:  GHI TMPRG1       ..KEEP SHIFTING THE BIT INTO THE MSB
        SHRC;PHI TMPRG1  ..POSITION
        DEC BITCNT
        SEP BITRDR
        GLO BITCNT
        BNZ SHIFT        ..ASSEMBLE THE BYTE
        GLO PARITY       ..READ PARITY ON LAST PASS
        SHR
        LSDF             ..FOR NO PARITY ERROR
        GHI PC;PLO FLAG  ..ELSE SET PARITY FLAG
        GHI FLAG         ..IS IT BLOCK # ?
        BZ BYTE
        DEC PTR;DEC PTR  ..POINT TO BLOCK #
        GHI TMPRG1;STR PTR..WRITE IT TO IOCB
        INC PTR;INC PTR  ..POINT TO HOME
        LDI 0;PHI FLAG   ..RESET BLK FLAG
        BR STRT1

BYTE:   GHI TMPRG1;STR BUFPTR ..PUT BYTE IN BUFFER
        DEC BUFPTR
        GLO BUFPTR;XRI #FF
        BNZ STRT1
        GHI BUFPTR;XRI #8C
        BNZ STRT1        ..UNTIL BUFFER IS FULL
        GLO FLAG;SHL             ..SET/RESET PARITY
        BR RESTOR

..*************************************************************************
..                      TAPE SAVE ROUTINE
..              COPIES DATA FROM MEMORY TO TAPE
..*************************************************************************

TPWRIT: SEP CALL;,A(OSTRNG)
        ,A(CRLF),T'FROM ',0
        LDI 0;PHI STRADD;PLO STRADD..CLEAR THE START ADDRESS
        PLO TMPRG3           ..POINT TO BEGINNING OF THE BUFFER
        PHI ASL;PLO ASL
        SEP CALL;,A(READCR)
        GHI ASL;PHI STRADD
        GLO ASL;PLO STRADD
        LDI 0;PLO ASL;PHI ASL
        SEP CALL;,A(OSTRNG)
        ,LF,T'TO ',0
        SEP CALL;,A(READCR)
        GHI ASL;PHI ENDADD
        GLO ASL;PLO ENDADD
        INC ENDADD
        SEP CALL;,A(FINDTP)
        SEP CALL;,A(WRITEX)   ..WRITE AN "I" TO TAPE FOR
        ,T'I',A(CRLF),0       .."INSERT"
        SEP CALL;,A(WRTH)     ..WRITE THE ADDRESS
CKEND:  GLO ENDADD;STR SP    ..DID WE DO ALL THE ADDRESSES?
        GLO STRADD;XOR
        BNZ WRTD2
        GHI ENDADD;STR SP
        GHI STRADD;XOR
        BZ FINISH

WRTD2:  GLO STRADD;ANI #1F ..CHECK IF 32 PAIRS OF DATA ARE
        BNZ WRTD1           ..PRINTED IN ONE LINE
        SEP CALL;,A(WRITEX)
        ,T';',A(CRLF),0     ..";" ENDS THE HEX DATA ON A LINE
        SEP CALL;,A(WRTH)   ..WRITE THE NEXT ADDRESS
        
WRTD1:  LDA STRADD;PHI TMPRG3..STORE THE DATA BYTE
        SEP CALL;,A(ASCII)    ..CONVERT THE BYTE TO ASCII
                              ..AND WRITE IT OUT
        BR CKEND

FINISH: SEP CALL;,A(WRITEX)   ..AT END WRITE OUT
        ,CR,EOF,0             ..RETURN AND'DC3'
        LDI #80;PLO TMPRG3    ..LOAD A NUMBER SO THAT WE CAN
        LDI #FE;PHI TMPRG3    ..FILL THE BUFFER WITH 383 BYTES

FILBFR: LDI EOF;PHI CHAR      ..OF DC3'S
        SEP CALL;,A(TWRITE)
        INC TMPRG3;GHI TMPRG3
        BNZ FILBFR
        SEP CALL;,A(OSTRNG)
        ,LF,T'DONE',0

        LBR RENTER

..****************************************************************************
                ..ROUTINE TO WRITE A HEX  ADDRESS TO TAPE
..****************************************************************************

WRTH:   GHI STRADD;PHI TMPRG3   
        SEP CALL;,A(ASCII)
        GLO STRADD;PHI TMPRG3
        SEP CALL;,A(ASCII)
        SEP CALL;,A(WRITEX)
        ,SPACE,0

WRITER: SEP R5

..****************************************************************
..              ROUTINE TO GET THE TAPE DRIVE NUMBER
..              AND SET UP THE IOCB
..****************************************************************

                ORG #8774  ..TO COMPENSATE FOR THE PROM PROGRAMMER

FINDTP: SEP CALL;,A(OSTRNG)
        ,A(CRLF),T'TAPE# ',0

ASK2:   LDI 0;PLO ASL
        SEP CALL;,A(READCR)       ..GET THE DRIVE #
        GLO ASL;SHR               ..VALID ENTRY?
        BZ FOUND
        SEP CALL;,A(OSTRNG)
        ,LF,T'0 OR 1?',0
        BR ASK2

                ..FORM THE IOCB VALUES ON THE STACK

FOUND:  STXD                     ..STORE IT FOR IOCB
        LDI DRIVE0
        BNF DRBIT                ..IF DRIVE 0 SELECTED
        SHL                      ..ELSE SET TO 1

DRBIT:  STXD                     ..STORE 0 COUNT + DRIVE FOR IOCB
        LDI 0;STXD               ..SET BLOCK#=0
        BR FIXERR                ..NEED TO JUMP OVER THE NEXT ORG

..*********************************************************************
..              TAPE WRITE SUBROUTINES
..*********************************************************************

                ..WRITE IMMEDIATE

                ORG #87A6       ..ONCE AGAIN FOR THE PROM PROGRAMMER

WRITEX: LDA  LINK;PHI CHAR      ..GET THE IMMEDIATE BYTE AND
        BZ WRITER
        SEP CALL;,A(TWRITE)    ..WRITE IT TO TAPE UNTIL GET
        BR WRITEX              .. A 0

..********************************************************************
..              REST OF THE FIND TAPE ROUTINE
..********************************************************************

FIXERR: GHI SP;PHI PTR         ..POINT PTR TO IOCB AREA
        GLO SP;PLO PTR
        INC PTR;INC PTR
        INC PTR                ..POINT TO HOME IN IOCB
        SEP R5

..*******************************************************************
                ..ROUTINE TO OUTPUT ASCII BYTE TO TAPE
..*******************************************************************

ASCII:  GHI TMPRG3               ..DO THE HIGH NIBBLE
        SHR;SHR;SHR;SHR;PLO TMPRG3
        SEP CALL;,A(ASCIIR)
        GHI TMPRG3;ANI #0F       ..AND THE LOW ONE
        PLO TMPRG3
        SEP CALL;,A(ASCIIR)
        SEP R5

ASCIIR: GLO TMPRG3;ADI #30;PHI CHAR
        SMI #3A;BM ASCI ..FOR 0 TO 9
        ADI #41;PHI CHAR..FOR A TO F

ASCI:   SEP CALL;,A(TWRITE)
        SEP R5

..****************************************************************
..                 REGISTER RESTORE
..        RESTORES THE REGISTERS SAVED BY REGSAV,
..        ASSUMES STACK POINTER IS 1 BYTE ABOVE THE 
..        STORED DATA. EXITS POINTING TO A FREE BYTE
..****************************************************************

REGSTR: IRX
        LDXA;PHI TMPRG1
        LDXA;PLO TMPRG1
        LDXA;PHI TMPRG2
        LDXA;PLO TMPRG2
        LDXA;PHI TMPRG3
        LDXA;PLO TMPRG3
        LDXA;PHI CNT
        LDN SP;PLO CNT
        GHI CHAR            ..TO COMPENSATE FOR BASIC
        SEP R5

..*****************************************************
..      FILLS ASL AS LONG AS HEX DIGITS ARE ENTERED
..*****************************************************

READHX: SEP CALL;,A(READAH)
        BDF READHX
        SEP R5

..*******************************************************************
..      ENTRY POINT FOR BASIC "BYE" COMMAND AND FOR GENERAL REENTER
..*******************************************************************

                  ORG #87F0

RENTER: LDI A.0(RENTR1);PLO R0 ..CAN BE ENTERED WITH X
        LDI A.1(RENTR1);PHI R0 .. AND P SET TO
        SEP R0                  ..ANYTHING. RESETS ALL THE SCRT
RENTR1: LDI A.0(PRMPT);PLO PC   .. REGISTERS
        LDI A.1(PRMPT);PHI PC
        LBR ENTER2

        END



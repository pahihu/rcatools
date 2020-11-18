;GLL-MAG microFORTH nucleus

DMA     EQU     0
INT     EQU     1
R       EQU     2      ;return stack ptr
P       EQU     3      ;program stack
R4      EQU     4
R5      EQU     5
R6      EQU     6
R7      EQU     7
R8      EQU     8
W       EQU     9      ;current word exec
A       EQU     10     ;temp reg
B       EQU     11     ;temp reg
U       EQU     12     ;user area
I       EQU     13     ;instr. ptr
S       EQU     14     ;data stack ptr
F       EQU     15     ;NEXT

FILLER  EQU     0

        ORG 0
        DIS
        DW  0
ZERO:   ; ZERO
        DW  0
NXT:    SEP  P
NEXT:   ; 'NEXT
        LDA  I
        PHI  W
        LDA  I
        PLO  W          ; W = M[I]
        LDA  W
        PHI  P
        LDA  W
        PLO  P          ; P = M[W]
        BR   NXT
        DB   FILLER
        DB   FILLER

        DW   LIT
LIT:    ; LIT
        GHI  F
LLIT:   PHI  B
        LDA  I
        DEC  S
        STR  S
        GHI  B
        DEC  S
        STR  S
        SEP  F

        DW   BRTICK
BRTICK: ; [']
        LDA  I
        BR   LLIT

        DW   EXE
EXE:    ; EXECUTE
        LDA  S
        PHI  W
        LDA  S
        PLO  W
        DEC  W
        DEC  W
        INC  F
        INC  F
        INC  F
        INC  F
        SEP  F

VAR:    ; 'VARIABLE'
        GLO  W
        DEC  S
        STR  S
        GHI  W
        DEC  S
        STR  S
        SEP  F

USER:   ; 'USER'
        GLO  U
        SEX  W
        ADD 
        DEC  S
        STR  S
        GHI  U
        DEC  S
        STR  S
        SEP  F
        DB   FILLER
        DB   FILLER

CONST:  ; 'CONSTANT'
        LDA  W
        PHI  B
        LDA  W
        DEC  S
        STR  S
        GHI  B
        DEC  S
        STR  S
        SEP  F

DOESG:  ; 'DOES>'
        GLO  I
        DEC  R
        STR  R
        GHI  I
        DEC  R
        STR  R
        LDA  W
        PHI  I
        LDA  W
        PLO  I
        GLO  W
        DEC  S
        STR  S
        GHI  W
        DEC  S
        STR  S
        SEP  F

        DW   SEMIS
SEMIS:  ; ;S
        LDA  R
        PHI  I
        LDA  R
        PLO  I
        SEP  F

COLON:  ; ':'
        GLO  I
        DEC  R
        STR  R
        GHI  I
        DEC  R
        STR  R
        GHI  W
        PHI  I
        GLO  W
        PLO  I
        SEP  F

        DW   POP2
DO:     ; DO
        SEX  R
        DEC  R
        STXD
        GHI  A
        STXD
        GLO  B
        STXD
        GHI  B
        STR  R
        SEP  F

        DW   DOEND
IF:     ; IF
        DB   FILLER

        DW   DOEND
END:    ; END
        DB   FILLER

        DW   L006D
ELSE:   ; ELSE
        DB   FILLER

        DW   L006D
WHILE:  ; WHILE
        DB   FILLER

        DW   LUP
LOOP:   ; LOOP
        DB   FILLER
        DB   FILLER
        DB   FILLER

        DW   PLUP
PLOOP:  ; +LOOP
        DB   FILLER
PLUP:   SEX  S
        INC  P
LUP:    SEX  W
        LDXA
        PHI  B
        LDXA
        INC  R
        SEX  R
        ADD 
        PLO  B
        STXD
        GHI  B
        ADC 
        PHI  B
        STR  R
        INC  R
        INC  R
        INC  R
        GLO  B
        SM  
        DEC  R
        GHI  B
        SMB 
        BNF  L006B
        INC  R
        INC  R
        INC  I
        SEP  F
L006B:  DEC  R
        DEC  R
L006D:  SEX  I
        GLO  I
        ADD 
        PLO  I
        GHI  I
        SEX  W
        ADC 
        PHI  I
        SEP  F
DOEND:  ; END
        LDA  S
        SEX  S
        OR  
        INC  S
        BZ   L006D
        INC  I
        SEP  F

        DW   USTA1
USTAR:  ; U*
        SHR 
        PLO  B
        INC  S
        SEX  S
L0129:  GHI  B
        BNF  L012D
        ADD 
L012D:  SHRC
        PHI  B
        GLO  B
        SHRC
        PLO  B
        DEC  A
        GLO  A
        BNZ  L0129
        GLO  B
LUSEND: STXD
        GHI  B
        STR  S
        SEP  F
        DB   FILLER

        DW   USLA1
USLASH: ; U/
        PHI  A
        LDA  S
        PHI  B
        LDN  S
        SHL 
        PLO  B
        GHI  A
        STR  S
        SEX  S
L014B:  GHI  B
        SHLC
        PHI  B
        SM  
        BNF  L0152
        PHI  B
L0152:  GLO  B
        SHLC
        PLO  B
        DEC  A
        GLO  A
        BNZ  L014B
        GLO  B
        STXD
        GHI  F
        BR   LUSEND
        ; STXD
        ; GHI  B
        ; STXD            ; ???
        ; SEP  F

USTA1:  GHI  F
        PHI  B
USLA1:  LDI  08H
        PLO  A
        INC  S
        LDA  S
        SEP  W

        DW   LCZ
CZ:     ; CZ

        DW   PUT
CAT:    ; C@
        LDN  B
L01CB:  STR  S
LCZ:    GHI  F
        DEC  S
        STR  S
        SEP  F

        DW   POP1
CSTO:   ; C!
        INC  S
        LDA  S
        STR  A
        SEP  F

        DW   ZEQU
ZEQU:   ; 0=
        LDA  S
        SEX  S
        OR  
        BZ   L01C9
        LDI  01H
L01C9:  SDI  01H
        BR   L01CB
        DB   FILLER
        DB   FILLER

        DW   ZLESS
ZLESS:  ; 0<
        LDA  S
        ANI  80H
        BZ   L01CB
        LDI  01H
        BR   L01CB

        DW   BINARY
PLUS:   ; +
LADD:   ADD 
        STXD
        GHI  B
        ADC 
        STR  S
        SEP  F

        DW   PUT
TSTAR:  ; 2*
        SEX  S
        BR   LADD

        DW   BINARY
MINUS:  ; -
        SD  
        STXD
        GHI  B
        SDB 
        STR  S
        SEP  F

        DW   BINARY
FAND:   ; AND
        AND 
        STXD
        GHI  B
        AND 
        STR  S
        SEP  F

BINARY: ; BINARY
        LDA  S
        PHI  B
        LDA  S
        INC  S
        SEX  S
        SEP  W

        DW   PLUS1
ONEP:   ; 1+

        DW   PLUS2
TWOP:   ; 2+
PLUS1:  LDI  01H
        LSKP
PLUS2:  LDI  02H
        PLO  B
        GHI  F
        PHI  B
        GLO  B
        INC  S
        SEX  S
        BR   LADD
        
        DW   POP2
MOVE:   ; MOVE
        LDA  S
        PHI  W
        LDA  S
        PLO  W
L0111:  LDA  W
        STR  A
        INC  A
        DEC  B
        GLO  B
        BNZ  L0111
        GHI  B
        BNZ  L0111
        SEP  F

        DW   LDUP
DUP:    ; DUP

        DW   OVER
OVER:   ; OVER
        INC  S
        INC  S
        LDA  S
        PHI  B
        LDN  S
        DEC  S
        DEC  S
LOVR:   DEC  S
        DEC  S
        STR  S
        GHI  B
        DEC  S
        STR  S
        SEP  F

        DW   MDUP
MDUP:   ; -DUP
        LDA  S
        SEX  S
        OR
        DEC  S
        BNZ  LDUP
        SEP  F
LDUP:   LDA  S
        PHI  B
        LDN  S
        PLO  B
        BR   LOVR

        DW   DROP
DROP:   ; DROP
        INC  S
        INC  S
        SEP  F

        DW   POP2
SWAP:   ; SWAP
        DEC  S
        GLO  B
        SEX  S
        STXD
        GHI  B
        STXD
        GLO  A
        STXD
        GHI  A
        STR  S
        SEP  F

        DW   PUT
AT:     ; @
        LDA  B
        PHI  A
        LDA  B
        STR  S
        GHI  A
        DEC  S
        STR  S
        SEP  F

        DW  POP1
STO:    ; !
        LDA  S
        STR  A
        INC  A
        LDA  S
        STR  A
        SEP  F

        DW   POP2
PSTO:   ; +!
        INC  B
        SEX  B
        ADD 
        STXD
        GHI  A
        ADC 
        STR  B
        SEP  F

        DW   LR
LR:     ; <R
        LDA  S
        PHI  B
        LDA  S
        DEC  R
        STR  R
        GHI  B
        DEC  R
        STR  R
        SEP  F

        DW   RG
RG:     ; R>
        LDA  R
        PHI  B
        LDA  R
        DEC  S
        STR  S
        GHI  B
        DEC  S
        STR  S
        SEP  F

        DW   FI
FI:     ; I
        LDA  R
        PHI  B
        LDN  R
        DEC  R
        DEC  S
        STR  S
        GHI  B
        DEC  S
        STR  S
        SEP  F

        DW   POP2
ROT:    ; ROT
        LDA  S          ; R4 A B -- A B R4
        PHI  R4
        LDN  S
        PLO  R4
        SEX  S
        GLO  A
        STXD
        GHI  A
        STXD
        GLO  B
        STXD
        GHI  B
        STXD
        GLO  R4
        STXD
        GHI  R4
        STR  S
        SEP  F

PUT:    ; PUT
        LDA  S
        PHI  B
        LDN  S
        PLO  B
        SEP  W

POP2:   ; 2POP
        LDA  S
        PHI  B
        LDA  S
        PLO  B
POP1:   ; 1POP
        LDA  S
        PHI  A
        LDA  S
        PLO  A
        SEP  W

        END

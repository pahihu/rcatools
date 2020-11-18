;exerpted and edited UT71 CALL/RETN support code, Herb Johnson Mar 2018
;http://www.retrotechnology.com/memship/UT71_rom.html - read the whole UT71 code
;http://www.retrotechnology.com/memship/rcabug_scrt.html - read about SCRT

;please don't entirely rely on this code as I've just chopped it up
;ONLY to illustrate use of CALL and RTRN, initialization and use - Herb Johnson

	;; Converted from ASM8 to A18
	;; 9/9/2016
	;; DW Schultz
;NAME: UT71. VER 0.0

R0     EQU 0
R1     EQU 1

SP     EQU 002H ;STACK POINTER
PC     EQU 003H ;PROGRAM COUNTER
CALL   EQU 004H ;CALL ROUTINE REGISTER
RETN   EQU 005H ;RETURN ROUTINE REGISTER
LINK   EQU 006H ;SUBROUTINE.DATA LINK (return stack)
DELAY  EQU 00CH ;DELAY ROUTINE REG
CHAR   EQU 00FH ; STORES ASCII I/O

UT71   EQU 08000H
TOPSTK EQU 008CFFH
WRAM   EQU 08C1FH

     ORG UT71            ;entry to ROM monitor P=X=R0

 	DIS              ;on reset P=R0=0000H
 	DB 0                 ;DISABLE
 	LDI HIGH(UT71) 	 ;ESTABLISH PROGRAM COUNTER AT
	PHI R0           ;8000 HEX and "jump" there
 

	LDI 	HIGH(WRAM) ;TOP OF SAVE AREA
	PHI	R1 
	LDI	LOW(WRAM)
	PLO	R1
	SEX 	R1	
LOOP  DEC 	R1         ;POINT BELOW WHERE SAVED R IS TO GO
	LDI	0D0H
	STXD            ;LOAD "SEP R0" INSTRUCTION FOR RETURN
     ;additional initialization and command setup
     LBR INIT

     ;bulk of UT71 monitor code not included

;stub of address so you can "assemble" this code, please dont "execute" it!

START:  BR  START  
DELAY1: ;magic delay routine
TYPED:  ;magic print-to-console routine
        BR  START

;*******************************************

;example of subroutine call and subroutine
 
EXAM: SEP	CALL		;or use CALL OSTRNG pseudoop
	DW	OSTRNG	;called address
	DB 	0DH,0AH,0AH,00 ;option to pass along data 
                     ;if implemented MUST be part of subroutine
      BR  EXAM 


OSTRNG  ;routine to print
       LDA LINK		;LINK reg points to string after CALL address & increments?
	  PHI CHAR
       BZ EXITM      ;print characters in string until null 
       SEP CALL
       DW (TYPED)	;send char in D to console (print it)  
       BR OSTRNG
EXITM  SEP RETN



;*************************************************************************
;     REGISTER INITIALIZATION ROUTINE
;     entered with R0 as program counter 

INIT LDI LOW(START)   ;REG 3 AS PROGRAM COUNTER
	PLO PC
	LDI HIGH(START)
	PHI PC 

ENTER2 LDI LOW(DELAY1)
	PLO DELAY       ;REG C to DELAY ROUTINE
	LDI HIGH(DELAY1)
	PHI DELAY
	LDI HIGH(CALLR) ;REG 4 TO CALL
	PHI CALL
	PHI RETN
	LDI LOW(CALLR)
	PLO CALL
	LDI LOW(RETR)   ;REG 5 TO RETURN
	PLO RETN
	LDI LOW(TOPSTK) ;REG 2 AS A STACK POINTER TO LOCATION 3CFF HEX
	PLO SP
	LDI HIGH(TOPSTK)
	PHI SP
	SEX SP
	SEP PC


;*************************************************************************
;DESC: STANDARD SEP CALL and RETN 
;REG USED: SP,PC,SEP CALL;,A(,RETURN,LINK & STACK
;*************************************************************************

	ORG UT71+0363H

; STANDARD CALL

EXITC SEP PC             ;GO TO CALLED ROUTINE

CALLR SEX SP 		    ;SET R(X)
	GHI LINK
	STXD                ;SAVE THE CURRENT LINK ON
	GLO LINK
	STXD                ;THE STACK
	GHI PC
	PHI LINK
	GLO PC
	PLO LINK
	LDA LINK
	PHI PC              ;PICK UP THE SUBROUTINE
	LDA LINK
	PLO PC              ;ADDRESS
	BR EXITC

;  STANDARD RETURN

EXITR  SEP PC            ;RETURN TO MAIN PGM

RETR GHI LINK            ;recover calling program return addr
	PHI PC
	GLO LINK
	PLO PC
	SEX SP
	INC SP              ;SET THE STACK POINTER
	LDXA
	PLO LINK            ;RESTORE THE CONTENTS OF
	LDX
	PHI LINK            ;LINK
	BR EXITR


	END


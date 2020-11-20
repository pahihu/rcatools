;                       R C A B U G
;       AN ASCII MONITOR PROBREM FOR THE RCA 1802
;                WRITTEN BY TOM CRAWFORD
;
;  modified source code version 0.0.3
;
;  This edited source file contains exerpts from 
;  Tom Crawford's "RCABug" program, as found in Ipso Facto Issue 10,
;      Feb. 1979, with modifications.
;
;  modified by greg simmons 2016-02-10 as follows:
;  2.  DLDI, CALL and RETRN macros are expanded manually (since A18 isn't a macro assembler)
;
;  heavily edited by Herb Johnson HRJ Feb 2015, to extract CALL/RTRN support,
;  register save/restore support, interrupt and DMA support,
;  most as per RCA SCRT standard. Plus, setup for init and executive loop.
;
;  updated Mar 3 2016 in repsonse to Greg Simmons comments by HRJ
;
; Greg says:
; "The RCA "User Manual for the CDP1802 COSMAC Microprocessor" MPM-201B, pages 54 through 63,
;  refers to the "SCRT", "Standard Call and Return Technique", which this program implements.  
;  It might be tricky to implement by macro, since 3 things are required:
;  1.  The code for the call and return routines 
;  2.  Initialization code to set R4 to point to the call routine
;    and R5 to point to the return routine
;   3.  The actual call and return macros
; It would be cool to have a way to make the assembler handle all this." - Greg
;
; MACROS
; ------  
;
; DLDI macro
;
;      DLDI     Value, Reg    ; where value is 16 bit
; expands to
;      LDI      LOW Value
;      PLO      Reg
;      LDI      HIGH Value
;      PHI      Reg
; or use the A18 "LOAD" pseudo-op 
;      LOAD     Reg, Value   ; which expands to
;      LDI      HIGH Value
;      PHI      Reg
;      LDI      LOW Value
;      PLO      Reg
;
; CALL and RETRN macros (as supported by routines in this code)
;
;      CALL     Addr		;expands to
;      SEP      R4
;      WORD     Addr
;
;      RETRN                  ;expands to
;      SEP       R5
;
; Interrupt instructions DIS and RET
;
; DIS and RET 
; ------------
; 
; Briefly, DIS and RET change the IE flag and also are affected by the X register.
; When DIS or RET are preceeded by setting X to the same register as
; the current program counter, as in SEX R4; DIS; [byte value];
; DIS or RET  use the following byte as an operand not as an executed instruction
; The high nybble of that byte sets the X register; the low nybble the program counter.
; In the call/return code, $24 sets X-->R2 and PC-->R4; or $25 sets X-->R2 and PC-->R5.
; Read the RCA COSMAC User's manual, or notes provided with this code extract. - Herb
;      
;               REGISTER USE SUMMARY
;
;  GENERAL USE:
;       R0      DMA POINTER (forced by CPU, also PC after reset)
;       R1      INTERRUPT ROUTINE POINTER (forced by CPU)
;       R2      STACK POINTER
;       R3      RUNNING PROGRAM COUNTER
;       R4      SCRT "CALL" ROUTINE POINTER"
;       R5      SCRT "RETURN" ROUTINE POINTER
;       R6      RETURN ADDRESS AND SUBR. DATA POINTER
;               (SEE NOTE IN SAVEREG SUBROUTINE
;       RF.1    D-REGISTER STORAGE DURING "CALL" AND "RETURN"
;
; memory use from top of memory (whereever)
; 
MAXRAM   ORG    $0FFF           ; HIGHEST ADDRESS OF RAM AVAILABLE FOR MONITOR'S USE
; user option to store variables here "above" the stack
VAR0     EQU    MAXRAM-1        ; 16-bit value at MAXRAM-1 and MAXRAM
VAR1     EQU    VAR0-2          ; 16 bit value at VAR0-2 & VAR0-1
VAR2     EQU    VAR1-1          ; 8 bit value at VAR-1
; stack is "below", should be set at "even" address in principle
TOP      EQU    VAR2-1
DMABSS   EQU    TOP             ; undefined DMA routine storage address
STACK1   EQU    TOP             ; stack will use memory below variables
;        ...                    ; 
;     
INTRT1   EQU    INIT            ; USE INIT. FOR INTERRUPTS FOR NOW.


;               S Y S T E M   I N I T I A L Z A T I O N
;
;       THIS ROUTINE IS ENTERED AT POWER-UP AND AT RESET.
;       IT INITIALIZES THE VALUES OF REGISTERS 0 TO 5,
;       SETS R3 TO BE THE PROGRAM COUNTER, AND ENTERS THE 
;       OPERATING SYSTEM MONITOR ROUTINE. ("EXEC")
;
; INIT, START and TAB1 all need to be on the same memory page (same high byte addr)
;
         ORG    $3800           ; MONITOR STARTING MEMORY ADDRESS (page where ever)

INIT     EQU    $               ; STARTING ADDRESS OF MONITOR
INITLZ   LDI    HIGH INIT       ; FIRST, SET THIS ROUTINE TO RUN
         PHI    R3              ; WITH R3 AS PROGRAM COUNTER
         PHI    R7
         LDI    LOW START
         PLO    R3
         SEP    R3
;      
START    LDI    LOW TAB1        ; SET R7 TO POINT TO THE TABLE 
         PLO    R7              ; OF INITIAL VALUES.
         LDA    R7              ; INITIALIZE REGISTER 5
         PHI    R5              ;       ( SCRT RETURN ROUTINE POINTER )
         LDA    R7              
         PLO    R5              
         LDA    R7              ; INITIALIZE REGISTER 4
         PHI    R4              ;       ( SCRT CALL ROUTINE POINTER )
         LDA    R7
         PLO    R4
         LDA    R7              ; INITIALIZE REGISTER 2
         PHI    R2              ;       ( SYSTEM STACK POINTER )
         LDA    R7
         PLO    R2
         LDA    R7              ; INITIALIZE REGISTER 1
         PHI    R1              ;       ( INTERRUPT SERVICE POINTER )
         LDA    R7
         PLO    R1
         LDA    R7              ; INITIALIZE REGISTER 0
         PHI    R0              ;       ( DMA POINTER )
         LDA    R7
         PLO    R0
         
         SEX    R2              ; SET X-REGISTER = R2
         
; other initializations such as Q, OUTs, EFs, program variables, etc.
;        ....
      
         LBR    EXCINT          ; NOW GO TO SYSTEM EXECUTIVE ROUTINE
;
;       TABLE OF INITIAL REGISTER VALUES FOLLOWS
;       (THIS TABLE MUST START IN SAME PAGE AS LABEL  "INIT")
TAB1     WORD  RTN001   ; SCRT RETURN
         WORD  CALL1    ; SCRT CALL ROUTINE START ADDRESS
         WORD  STACK1   ; OP. SYS. STACK POINTER INIT. VALUE
         WORD  INTRT1   ; INTERRUPT SERVICE ROUTINE START ADDRESS
         WORD  DMABSS   ; DMA STORAGE INITIAL ADDRESS

; INIT, START and TAB1 all need to be on the same memory page (same high byte addr)
;

;       .SBTTL   "CALL1" ROUTINE
;       .SLW
;
;       STANDARD SUBROUTINE "CALL1" ROUTINE
;
;       THIS ROUTINE IS PART OF S.C.R.T.
;       IT IS USED TO TRANSFER CONTROL TO A SUBROUTINE
; 
;       CALLING PROCEDURE... SIMPLY EXECUTE SEP R4. THIS WILL
;       CAUSE "CALL1" TO BE ENTERED. THE ADDRESS OF THE
;       SUBROUTINE MUST FOLLOW THE SEP INSTRUCTION, FOLLOWED
;       IMMEDIATELY BY ANY DATA BYTES THE SUBROUTINE EXPECTS.
;       AN EXAMPLE IS ...
;               SEP R4
;               ---  HIGH BYTE OF SUBROUTINE ADDRESS IS HERE
;               ---  LOW  BYTE OF SUBROUTINE ADDRESS IS HERE
;               ---  IF REQURED, DATA BYTES ARE HERE.
;       NOTE THAT R4 MUST HAVE PREVIOUSLY BEEN SET UP TO
;       POINT TO THE ENTRY ADDRESS OF "CALL1".
;
;       THE CONTENTS OF D ARE SAVED IN RF.1, AND RESTORED
;       BEFORE ENTERING THE SUBROUTINE.
;
EXITA    SEP    R3              ; R3 POINTS TO SUBROUTINE
CALL1    SEX    R4              ; INHIBIT INTERRUPTS
         DIS            
         DB     $24             ; see notes on DIS and RET
         PHI    RF              ; SAVE D
         GHI    R6              ; SAVE R6 ON STACK, SINCE R6 WILL BE
         STXD                   ; USED TO SAVE THE RETURN ADDRESS
         GLO    R6
         STXD
         GHI    R3              ; COPY R3 INTO R6 TO SAVE RETURN ADDRESS
         PHI    R6
         GLO    R3
         PLO    R6
         LDA    R6              ; LOAD THE SUBROUTINE ADDRESS INTO R3
         PHI    R3
         LDA    R6
         PLO    R3
         GHI    RF              ; RESTORE D
         SEX    R4
         RET                    ; PERMIT INTERRUPTS
         DB     $24             ; see notes on DIS and RET
         BR     EXITA           ; BRANCH TO EXITA. THIS WILL CAUSE
;                                  ENTRY INTO THE SUBROUTINE, AND LEAVE
;                                  R4 SET FOR THE NEXT ENTRY TO "CALL1"          .
;       .SBTTL   "RTN001"
;       .SLW
;
;
;       STANDARD SUBROUTINE "RTN001" ROUTINE
;
;       THIS ROUTINE IS PART OF S.C.R.T
;       IT IS USED TO RETURN CONTROL FROM A SUBROUTINE BACK
;       TO THE CALLING ROUTINE.
;
;       CALLING PROCEDURE... SIMPLY EXECUTE  SEP R5. THIS
;       WILL CAUSE "RTN001" TO BE ENTERED. THE RETURN ROUTINE THEN
;       SETS R3 TO THE LAST SAVED RETURN ADDRESS (SAVED IN R6),
;       EXTRACTS THE PREVIOUS SAVED ADDRESS FROM THE STACK AND
;       STORES IT IN R6, THEN TRANSFERS PROGRAM CONTROL TO THE ADDRESS
;       IT PLACED IN R3.
;
;       NOTE THAT R5 MUST HAVE PREVIOUSLY BEEN SET TO POINT TO
;       THE ENTRY ADDRESS OF "RTN001".
;
;       THE CONTENTS OF D ARE SAVED IN RF.1 AND RESTORED
;       BEFORE ENTERING CALLING ROUTINE
;
EXITR    SEP    R3              ; R3 POINTS BACK TO CALLING ROUTINE
RTN001   SEX    R5              ; INHIBIT INTERRUPTS
         DIS
         DB     $25             ; see notes on DIS and RET
         PHI    RF              ; SAVE D
         GHI    R6              ; COPY R6 INTO R3 (R6 CONTAINS
         PHI    R3              ;    THE RETURN ADDRESS)
         GLO    R6
         PLO    R3
         SEX    R2              ; POINT TO STACK
         INC    R2              ; POINT STACK TO SAVED OLD R6
         LDXA                   ;   RESTORE THE SAVED OLD R6 INTO R6
         PLO    R6
         LDX
         PHI    R6
         GHI    RF              ; RESTORE D
         SEX    R5              
         RET                    ; PERMIT INTERRUPTS
         DB     $25             ; see notes on DIS and RET
         BR     EXITR           ; BRANCH TO EXITR. THIS WILL CAUSE
;                                 PROGRAM CONTROL TO RETURN FROM THE
;                                 SUBROUTINE, AND LEAVE R5 SET FOR THE
;                                 NEXT ENTRY TO "RTN001".
;
;
;       .SBTTL  SAVE AND RESTORE REGISTERS
;       .SLW
;
;               S A V R E G - SAVE REGISTERS ROUTINE.
;
;       THIS ROUTINE SAVES REGISTERS R10, R9, R8, AND R7 ON THE STACK.
;       IT DOES NOT DESTROY D IF ENTERED VIA +CALL.
;       NOTE: - IF SAVREG IS CALLED FROM A SUBROUTINE WHICH WAS CALLED
;               VIA THE SCRT CALL ROUTINE THE CONTENTS OF REGISTER SIX
;               WILL NOT POINT TO THE RETURN POINT OF THE CALL TO THE
;               SUBROUTINE. REGISTER 6 WILL BE CORRECT AFTER A CALL TO
;               THE RESTORE REGISTERS ROUTINE.
         

SAVREG   GHI    R10
         STXD
         GLO    R10
         STXD
         GHI    R9
         STXD
         GLO    R9
         STXD
         GHI    R8         
         STXD
         GLO    R8
         STXD
         GHI    R7
         STXD
         GLO    R7
         STXD
         GHI    RF
;        RETRN                  ; RECOVER CONTENTS OF D.
         SEP    R5
;
;
;               R E S R E G - RESTORE REGISTERS ROUTINE.
;
;       THIS ROUTINE RESTORES REGISTERS R7, R8, R9 AND R10 FROM THE STACK.
;       IT DOES NOT DESTROY D IF ENTERED VIA +CALL.
;
RESREG   IRX
         LDXA
         PLO    R7
         LDXA
         PHI    R7
         LDXA
         PLO    R8
         LDXA
         PHI    R8
         LDXA
         PLO    R9
         LDXA
         PHI    R9
         LDXA
         PLO    R10
         LDX
         PHI    R10
         GHI    RF              ; RECOVER CONTENTS OF D.
;        RETRN
         SEP    R5         

;
; interrupt service routine
; not defined in RCABUG, if left here in this code
; it would fall through to init executive program
; as initialized to start, amounts to reset
INTRP    EQU    $
; see  RCA's example ISR at "INTER", further in this code. 

; USER EXECUTIVE PROGRAM
;
;       perform any initializations to your executive program
EXCINT   EQU     $
;        ....
; then fall through to executive control loop
;
; THE EXECUTIVE CONTROL LOOP BEGINS HERE.
;
CONTRL   EQU    $
;        ...                    ; loop through possible executive actions
;        CALL   EX1
         SEP    R4
         WORD   EX1
         ...
;        CALL   EX2
         SEP    R4
         WORD   EX2
;        ...
         BR     CONTRL
;
;
; DMA service routine
; not defined, look at 1802 processor conditions on LOAD
;
;
; user support routines and programs
;
; stubs for the exec loop above
EX1      EQU    $
EX2      EQU    $
;        RETRN
         SEP    R5
;	
; example of calling and called routine w/ passed value, first calling
;
;        CALL   DELAY1          ;
         SEP    R4              
         WORD   DELAY1          ; the called address
         WORD   22369           ; a constant passed to it
;        ....                   ; return comes here
;
; now called routine acquires value via R6 auto-advance
;
DELAY1   EQU    $               ; DELAY1 ROUTINE STARTS HERE
         LDA    R6              ; GET THE DELAY COUNT HIGH BYTE
         PHI    R7              ; AND SAVE IT IN R7
         LDA    R6              ; GET THE DELAY COUNT LOW BYTE, AND
;        ...                    ; do other business
;        RETRN                  ; ...and return
         SEP    R5
;
; possible interrrupt service routine, suggested by RCA User's Guide
;
; on intrp disable DIS, M(RX)->X,P ,RX=RX+1, IE=0
; on hardware interrupt, X,P -> T, PC=R1, X->R2, IE=0
; on SAV, T -> memory at(RX) 
; on instruction RET, M(RX)->X,P ,RX=RX+1, IE=1
; also see MARK instruction
; 
EXITI    RET                    ;restore X, P, R2 & enable intrpts
;
; set this routine for interrupt address to enter when interrupt
INTER    DEC    R2              ;decr stack pointer
         SAV                    ; save old X, P
         DEC    R2
         STXD                   ; save D
         SHRC                   ;DF saved in MSB of D
         STXD                   ; save DF
         ; optionally save other registers
         ; by GLO/GHI Rx, STXD operations
;
; perform interrupt handler
;
         INC     R2             ; point to last saved
         ; recover saved registers
         ; by LDA R2, PHI/PLO Rx operations
         LDA     R2             ;restore DF
         SHL
         LDA     R2             ; restore D
         BR      EXITI
;



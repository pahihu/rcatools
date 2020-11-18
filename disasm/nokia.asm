
32768 bytes count
1806 disasembler (c)1998 Herman Robers PA3FYW
Usage: disasm [-bxxxx] [-exxxx] < hexfile.hex > disasm.asm, (example: disasm -b00e0 -e0a00 < dump.hex > dump.asm)
Dissasembling from 0x00e0 to 0x0fff.

00e0 f8 00        ldi 00         ; Load D immediate
00e2 b3           phi r3         ; Put high register R3
00e3 f8 e8        ldi e8         ; Load D immediate
00e5 a3           plo r3         ; Put low register R3
00e6 e2           sex r2         ; Set P=R2 as datapointer
00e7 d3           sep r3         ; Set P=R3 as program counter
00e8 f8 00        ldi 00         ; Load D immediate
00ea be           phi re         ; Put high register RE
00eb ad           plo rd         ; Put low register RD
00ec b9           phi r9         ; Put high register R9
00ed a9           plo r9         ; Put low register R9
00ee b8           phi r8         ; Put high register R8
00ef b0           phi r0         ; Put high register R0
00f0 a0           plo r0         ; Put low register R0
00f1 f8 08        ldi 08         ; Load D immediate
00f3 b8           phi r8         ; Put high register R8
00f4 d4           sep r4         ; Set P=R4 as program counter
00f5 3a cf        bnz  cf        ; Short branch on D!=0
00f7 d4           sep r4         ; Set P=R4 as program counter
00f8 3a 0a        bnz  0a        ; Short branch on D!=0
00fa 8e           glo re         ; Get low register RE
00fb c2 01 73     lbz 0173       ; Long branch on D=0
00fe fb 0f        xri 0f         ; Logical XOR D with value
0100 c2 1a ac     lbz 1aac       ; Long branch on D=0
0104 68 cb 87 ff  rldi rb 87ff   ; register load immediate RB
0107 eb           sex rb         ; Set P=RB as datapointer
0108 f8 00        ldi 00         ; Load D immediate
010a 73           stxd           ; Store via X and devrement
010b 8b           glo rb         ; Get low register RB
010c fb 7f        xri 7f         ; Logical XOR D with value
010e 3a 08        bnz  08        ; Short branch on D!=0
0111 68 cb 87 7e  rldi rb 877e   ; register load immediate RB
0114 f8 00        ldi 00         ; Load D immediate
0116 73           stxd           ; Store via X and devrement
0117 8b           glo rb         ; Get low register RB
0118 fb fc        xri fc         ; Logical XOR D with value
011a 3a 14        bnz  14        ; Short branch on D!=0
011c 9b           ghi rb         ; Get high register RB
011d fb 83        xri 83         ; Logical XOR D with value
011f 3a 14        bnz  14        ; Short branch on D!=0
0122 68 cb 83 f5  rldi rb 83f5   ; register load immediate RB
0125 f8 00        ldi 00         ; Load D immediate
0127 73           stxd           ; Store via X and devrement
0128 8b           glo rb         ; Get low register RB
0129 fb ff        xri ff         ; Logical XOR D with value
012b 3a 25        bnz  25        ; Short branch on D!=0
012d 9b           ghi rb         ; Get high register RB
012e fb 7f        xri 7f         ; Logical XOR D with value
0130 3a 25        bnz  25        ; Short branch on D!=0
0132 f8 c0        ldi c0         ; Load D immediate
0134 a7           plo r7         ; Put low register R7
0135 f8 08        ldi 08         ; Load D immediate
0137 57           str r7         ; Store D to (R7)
0138 e7           sex r7         ; Set P=R7 as datapointer
0139 64           out 4          ; Output (R(X)); Increment R(X), N=100
013a 17           inc r7         ; Increment (R7)
013b f8 00        ldi 00         ; Load D immediate
013d 57           str r7         ; Store D to (R7)
013e 64           out 4          ; Output (R(X)); Increment R(X), N=100
013f 17           inc r7         ; Increment (R7)
0140 57           str r7         ; Store D to (R7)
0141 64           out 4          ; Output (R(X)); Increment R(X), N=100
0142 17           inc r7         ; Increment (R7)
0143 57           str r7         ; Store D to (R7)
0144 64           out 4          ; Output (R(X)); Increment R(X), N=100
0145 17           inc r7         ; Increment (R7)
0146 f8 01        ldi 01         ; Load D immediate
0148 57           str r7         ; Store D to (R7)
0149 64           out 4          ; Output (R(X)); Increment R(X), N=100
014a f8 05        ldi 05         ; Load D immediate
014c 57           str r7         ; Store D to (R7)
014d 17           inc r7         ; Increment (R7)
014e f8 03        ldi 03         ; Load D immediate
0150 57           str r7         ; Store D to (R7)
0151 64           out 4          ; Output (R(X)); Increment R(X), N=100
0152 17           inc r7         ; Increment (R7)
0153 f8 00        ldi 00         ; Load D immediate
0155 57           str r7         ; Store D to (R7)
0156 64           out 4          ; Output (R(X)); Increment R(X), N=100
0157 17           inc r7         ; Increment (R7)
0158 57           str r7         ; Store D to (R7)
0159 64           out 4          ; Output (R(X)); Increment R(X), N=100
015b 68 ca 83 e0  rldi ra 83e0   ; register load immediate RA
015e f8 f0        ldi f0         ; Load D immediate
0160 5a           str ra         ; Store D to (RA)
0161 1a           inc ra         ; Increment (RA)
0162 1a           inc ra         ; Increment (RA)
0163 1a           inc ra         ; Increment (RA)
0164 8a           glo ra         ; Get low register RA
0165 fb ec        xri ec         ; Logical XOR D with value
0167 3a 5e        bnz  5e        ; Short branch on D!=0
0169 f8 f4        ldi f4         ; Load D immediate
016b a7           plo r7         ; Put low register R7
016c f8 fe        ldi fe         ; Load D immediate
016e aa           plo ra         ; Put low register RA
016f f8 02        ldi 02         ; Load D immediate
0171 57           str r7         ; Store D to (R7)
0172 5a           str ra         ; Store D to (RA)
0173 d4           sep r4         ; Set P=R4 as program counter
0174 3e bc        bn3  bc        ; Short branch on EF3=0
0176 d4           sep r4         ; Set P=R4 as program counter
0177 3c ba        bn1  ba        ; Short branch on EF1=0
017a 68 c7 83 38  rldi r7 8338   ; register load immediate R7
017e 68 ca 83 da  rldi ra 83da   ; register load immediate RA
0181 07           ldn r7         ; Load D with (R7)
0182 fa 02        ani 02         ; Logical AND D with value
0184 3a 8c        bnz  8c        ; Short branch on D!=0
0186 0a           ldn ra         ; Load D with (RA)
0187 fa f7        ani f7         ; Logical AND D with value
0189 5a           str ra         ; Store D to (RA)
018a 30 90        br  90         ; Short branch
018c 0a           ldn ra         ; Load D with (RA)
018d f9 08        ori 08         ; Logical OR D with value
018f 5a           str ra         ; Store D to (RA)
0190 8e           glo re         ; Get low register RE
0191 ca 02 0b     lbnz 020b      ; Long branch on D!=0
0194 c4           nop            ; No operation
0195 c4           nop            ; No operation
0196 36 9b        b3  9b         ; Short branch on EF3=1
0198 c0 02 0b     lbr 020b       ; Long branch
019b f8 ca        ldi ca         ; Load D immediate
019d a7           plo r7         ; Put low register R7
019e 07           ldn r7         ; Load D with (R7)
019f fa 08        ani 08         ; Logical AND D with value
01a1 c2 02 0b     lbz 020b       ; Long branch on D=0
01a4 f8 73        ldi 73         ; Load D immediate
01a6 a7           plo r7         ; Put low register R7
01a7 f8 20        ldi 20         ; Load D immediate
01a9 57           str r7         ; Store D to (R7)
01aa d4           sep r4         ; Set P=R4 as program counter
01ab 38 3b        skp  3b        ; Skip next byte
01ad d4           sep r4         ; Set P=R4 as program counter
01ae 35 a1        b2  a1         ; Short branch on EF2=1
01b0 c8 d4 3e     lskp d43e      ; Long skip
01b3 18           inc r8         ; Increment (R8)
01b4 d4           sep r4         ; Set P=R4 as program counter
01b5 39 0e        bnq  0e        ; Short branch on Q=0
01b7 d4           sep r4         ; Set P=R4 as program counter
01b8 3a 86        bnz  86        ; Short branch on D!=0
01ba d4           sep r4         ; Set P=R4 as program counter
01bb 37 1f        b4  1f         ; Short branch on EF4=1
01bd 01           ldn r1         ; Load D with (R1)
01be d4           sep r4         ; Set P=R4 as program counter
01bf 3b e1        bnf  e1        ; Short branch on DF=0
01c1 d4           sep r4         ; Set P=R4 as program counter
01c2 3e 4a        bn3  4a        ; Short branch on EF3=0
01c4 d4           sep r4         ; Set P=R4 as program counter
01c5 3d 05        bn2  05        ; Short branch on EF2=0
01c7 f8 38        ldi 38         ; Load D immediate
01c9 a7           plo r7         ; Put low register R7
01ca 07           ldn r7         ; Load D with (R7)
01cb fa 10        ani 10         ; Logical AND D with value
01cd 3a d5        bnz  d5        ; Short branch on D!=0
01cf 99           ghi r9         ; Get high register R9
01d0 f9 04        ori 04         ; Logical OR D with value
01d2 b9           phi r9         ; Put high register R9
01d3 30 d9        br  d9         ; Short branch
01d5 98           ghi r8         ; Get high register R8
01d6 f9 40        ori 40         ; Logical OR D with value
01d8 b8           phi r8         ; Put high register R8
01d9 98           ghi r8         ; Get high register R8
01da fa f7        ani f7         ; Logical AND D with value
01dc f9 20        ori 20         ; Logical OR D with value
01de b8           phi r8         ; Put high register R8
01df 89           glo r9         ; Get low register R9
01e0 f9 e0        ori e0         ; Logical OR D with value
01e2 a9           plo r9         ; Put low register R9
01e3 d4           sep r4         ; Set P=R4 as program counter
01e4 37 1f        b4  1f         ; Short branch on EF4=1
01e6 02           ldn r2         ; Load D with (R2)
01e7 e7           sex r7         ; Set P=R7 as datapointer
01e8 f8 c6        ldi c6         ; Load D immediate
01ea a7           plo r7         ; Put low register R7
01eb 07           ldn r7         ; Load D with (R7)
01ec f9 01        ori 01         ; Logical OR D with value
01ee 57           str r7         ; Store D to (R7)
01ef 64           out 4          ; Output (R(X)); Increment R(X), N=100
01f0 f8 ce        ldi ce         ; Load D immediate
01f2 a7           plo r7         ; Put low register R7
01f3 07           ldn r7         ; Load D with (R7)
01f4 f9 01        ori 01         ; Logical OR D with value
01f6 57           str r7         ; Store D to (R7)
01f7 64           out 4          ; Output (R(X)); Increment R(X), N=100
01f8 d4           sep r4         ; Set P=R4 as program counter
01f9 38 21        skp  21        ; Skip next byte
01fb 9e           ghi re         ; Get high register RE
01fc f9 02        ori 02         ; Logical OR D with value
01fe be           phi re         ; Put high register RE
0200 68 07        stm            ; set timer mode and start
0201 80           glo r0         ; Get low register R0
0202 f9 80        ori 80         ; Logical OR D with value
0204 a0           plo r0         ; Put low register R0
0205 d4           sep r4         ; Set P=R4 as program counter
0206 37 dc        b4  dc         ; Short branch on EF4=1
0208 c0 0c ac     lbr 0cac       ; Long branch
020b f8 c6        ldi c6         ; Load D immediate
020d a7           plo r7         ; Put low register R7
020e 07           ldn r7         ; Load D with (R7)
020f fa e7        ani e7         ; Logical AND D with value
0211 57           str r7         ; Store D to (R7)
0212 e7           sex r7         ; Set P=R7 as datapointer
0213 64           out 4          ; Output (R(X)); Increment R(X), N=100
0214 f8 ca        ldi ca         ; Load D immediate
0216 a7           plo r7         ; Put low register R7
0217 07           ldn r7         ; Load D with (R7)
0218 fa f7        ani f7         ; Logical AND D with value
021a 57           str r7         ; Store D to (R7)
021b 64           out 4          ; Output (R(X)); Increment R(X), N=100
021c d4           sep r4         ; Set P=R4 as program counter
021d 3a 86        bnz  86        ; Short branch on D!=0
0220 68 c7 83 2e  rldi r7 832e   ; register load immediate R7
0223 f8 00        ldi 00         ; Load D immediate
0225 57           str r7         ; Store D to (R7)
0226 f8 db        ldi db         ; Load D immediate
0228 a7           plo r7         ; Put low register R7
0229 07           ldn r7         ; Load D with (R7)
022a fa 80        ani 80         ; Logical AND D with value
022c 57           str r7         ; Store D to (R7)
022d f8 00        ldi 00         ; Load D immediate
022f a7           plo r7         ; Put low register R7
0230 57           str r7         ; Store D to (R7)
0231 f8 dc        ldi dc         ; Load D immediate
0233 a7           plo r7         ; Put low register R7
0234 f8 00        ldi 00         ; Load D immediate
0236 57           str r7         ; Store D to (R7)
0238 68 c7 83 d7  rldi r7 83d7   ; register load immediate R7
023b f8 00        ldi 00         ; Load D immediate
023d 57           str r7         ; Store D to (R7)
023e f8 37        ldi 37         ; Load D immediate
0240 a7           plo r7         ; Put low register R7
0241 e7           sex r7         ; Set P=R7 as datapointer
0242 f8 00        ldi 00         ; Load D immediate
0244 73           stxd           ; Store via X and devrement
0245 87           glo r7         ; Get low register R7
0246 fb 2f        xri 2f         ; Logical XOR D with value
0248 3a 42        bnz  42        ; Short branch on D!=0
024b 68 c7 83 23  rldi r7 8323   ; register load immediate R7
024e f8 1c        ldi 1c         ; Load D immediate
0250 57           str r7         ; Store D to (R7)
0251 17           inc r7         ; Increment (R7)
0252 f8 20        ldi 20         ; Load D immediate
0254 57           str r7         ; Store D to (R7)
0255 d4           sep r4         ; Set P=R4 as program counter
0256 38 3b        skp  3b        ; Skip next byte
0258 d4           sep r4         ; Set P=R4 as program counter
0259 37 1f        b4  1f         ; Short branch on EF4=1
025b 01           ldn r1         ; Load D with (R1)
025c 8e           glo re         ; Get low register RE
025d 73           stxd           ; Store via X and devrement
025e d4           sep r4         ; Set P=R4 as program counter
025f 3b 25        bnf  25        ; Short branch on DF=0
0261 d4           sep r4         ; Set P=R4 as program counter
0262 3b e1        bnf  e1        ; Short branch on DF=0
0264 60           irx            ; Increment register X
0265 f0           ldx            ; Pop stack. Place value in D register
0266 ae           plo re         ; Put low register RE
0267 f8 10        ldi 10         ; Load D immediate
0269 a0           plo r0         ; Put low register R0
026a f8 7f        ldi 7f         ; Load D immediate
026c b9           phi r9         ; Put high register R9
026d f8 6c        ldi 6c         ; Load D immediate
026f a9           plo r9         ; Put low register R9
0270 d4           sep r4         ; Set P=R4 as program counter
0271 37 1f        b4  1f         ; Short branch on EF4=1
0273 02           ldn r2         ; Load D with (R2)
0274 f8 00        ldi 00         ; Load D immediate
0276 b9           phi r9         ; Put high register R9
0277 a0           plo r0         ; Put low register R0
0278 e3           sex r3         ; Set P=R3 as datapointer
0279 71           dis            ; Disable. Return from interrupt, set IE=0
027a 23           dec r3         ; Decrement (R3)
027b d4           sep r4         ; Set P=R4 as program counter
027c 48           lda r8         ; Load D from (R8), increment R8
027d 9d           ghi rd         ; Get high register RD
027e 8b           glo rb         ; Get low register RB
027f c2 02 93     lbz 0293       ; Long branch on D=0
0282 f8 da        ldi da         ; Load D immediate
0284 aa           plo ra         ; Put low register RA
0285 0a           ldn ra         ; Load D with (RA)
0286 fa 08        ani 08         ; Logical AND D with value
0288 ca 02 8e     lbnz 028e      ; Long branch on D!=0
028b c0 1a ac     lbr 1aac       ; Long branch
028e f8 01        ldi 01         ; Load D immediate
0290 be           phi re         ; Put high register RE
0291 30 94        br  94         ; Short branch
0293 c4           nop            ; No operation
0294 e3           sex r3         ; Set P=R3 as datapointer
0295 70           ret            ; Return from interrupt, set IE=1
0296 23           dec r3         ; Decrement (R3)
0297 d4           sep r4         ; Set P=R4 as program counter
0298 3e 4a        bn3  4a        ; Short branch on EF3=0
029a f8 68        ldi 68         ; Load D immediate
029c a9           plo r9         ; Put low register R9
029d d4           sep r4         ; Set P=R4 as program counter
029e 37 1f        b4  1f         ; Short branch on EF4=1
02a0 05           ldn r5         ; Load D with (R5)
02a1 f8 59        ldi 59         ; Load D immediate
02a3 a7           plo r7         ; Put low register R7
02a4 f8 6f        ldi 6f         ; Load D immediate
02a6 57           str r7         ; Store D to (R7)
02a7 d4           sep r4         ; Set P=R4 as program counter
02a8 35 d5        b2  d5         ; Short branch on EF2=1
02aa f8 40        ldi 40         ; Load D immediate
02ac a9           plo r9         ; Put low register R9
02ad f8 93        ldi 93         ; Load D immediate
02af aa           plo ra         ; Put low register RA
02b0 f8 00        ldi 00         ; Load D immediate
02b2 5a           str ra         ; Store D to (RA)
02b3 f8 da        ldi da         ; Load D immediate
02b5 aa           plo ra         ; Put low register RA
02b6 0a           ldn ra         ; Load D with (RA)
02b7 fa 08        ani 08         ; Logical AND D with value
02b9 ca 4a 9d     lbnz 4a9d      ; Long branch on D!=0
02bc d4           sep r4         ; Set P=R4 as program counter
02bd 3d 05        bn2  05        ; Short branch on EF2=0
02bf d4           sep r4         ; Set P=R4 as program counter
02c0 3c 6f        bn1  6f        ; Short branch on EF1=0
02c2 d4           sep r4         ; Set P=R4 as program counter
02c3 35 c9        b2  c9         ; Short branch on EF2=1
02c5 d4           sep r4         ; Set P=R4 as program counter
02c6 3a c5        bnz  c5        ; Short branch on D!=0
02c8 30 e9        br  e9         ; Short branch
02ca 9e           ghi re         ; Get high register RE
02cb fa 0d        ani 0d         ; Logical AND D with value
02cd be           phi re         ; Put high register RE
02ce f8 7f        ldi 7f         ; Load D immediate
02d0 aa           plo ra         ; Put low register RA
02d1 0a           ldn ra         ; Load D with (RA)
02d2 fa f7        ani f7         ; Logical AND D with value
02d4 5a           str ra         ; Store D to (RA)
02d5 d4           sep r4         ; Set P=R4 as program counter
02d6 37 e6        b4  e6         ; Short branch on EF4=1
02d8 f8 5b        ldi 5b         ; Load D immediate
02da aa           plo ra         ; Put low register RA
02db 0a           ldn ra         ; Load D with (RA)
02dc 2a           dec ra         ; Decrement (RA)
02dd 2a           dec ra         ; Decrement (RA)
02de 5a           str ra         ; Store D to (RA)
02df d4           sep r4         ; Set P=R4 as program counter
02e0 35 d5        b2  d5         ; Short branch on EF2=1
02e2 d4           sep r4         ; Set P=R4 as program counter
02e3 35 a1        b2  a1         ; Short branch on EF2=1
02e5 14           inc r4         ; Increment (R4)
02e6 d4           sep r4         ; Set P=R4 as program counter
02e7 3a c5        bnz  c5        ; Short branch on D!=0
02e9 3d ee        bn2  ee        ; Short branch on EF2=0
02eb c0 1a ac     lbr 1aac       ; Long branch
02ee d4           sep r4         ; Set P=R4 as program counter
02ef 3d 7f        bn2  7f        ; Short branch on EF2=0
02f1 f8 28        ldi 28         ; Load D immediate
02f3 a7           plo r7         ; Put low register R7
02f4 e7           sex r7         ; Set P=R7 as datapointer
02f5 f8 00        ldi 00         ; Load D immediate
02f7 73           stxd           ; Store via X and devrement
02f8 73           stxd           ; Store via X and devrement
02f9 73           stxd           ; Store via X and devrement
02fa 73           stxd           ; Store via X and devrement
02fb d4           sep r4         ; Set P=R4 as program counter
02fc 3d b3        bn2  b3        ; Short branch on EF2=0
02fe f8 7f        ldi 7f         ; Load D immediate
0300 aa           plo ra         ; Put low register RA
0301 0a           ldn ra         ; Load D with (RA)
0302 fa f0        ani f0         ; Logical AND D with value
0304 fb 10        xri 10         ; Logical XOR D with value
0306 ca 03 14     lbnz 0314      ; Long branch on D!=0
0309 f8 ce        ldi ce         ; Load D immediate
030b aa           plo ra         ; Put low register RA
030c 0a           ldn ra         ; Load D with (RA)
030d f9 04        ori 04         ; Logical OR D with value
030f 5a           str ra         ; Store D to (RA)
0310 ea           sex ra         ; Set P=RA as datapointer
0311 64           out 4          ; Output (R(X)); Increment R(X), N=100
0312 30 24        br  24         ; Short branch
0314 0a           ldn ra         ; Load D with (RA)
0315 fa f0        ani f0         ; Logical AND D with value
0317 fb 50        xri 50         ; Logical XOR D with value
0319 32 24        bz  24         ; Short branch on D=0
031b f8 c6        ldi c6         ; Load D immediate
031d aa           plo ra         ; Put low register RA
031e 0a           ldn ra         ; Load D with (RA)
031f f9 04        ori 04         ; Logical OR D with value
0321 5a           str ra         ; Store D to (RA)
0322 ea           sex ra         ; Set P=RA as datapointer
0323 64           out 4          ; Output (R(X)); Increment R(X), N=100
0324 d4           sep r4         ; Set P=R4 as program counter
0325 35 a1        b2  a1         ; Short branch on EF2=1
0327 0c           ldn rc         ; Load D with (RC)
0328 30 31        br  31         ; Short branch
032a c4           nop            ; No operation
032b 8b           glo rb         ; Get low register RB
032c ff 09        smi 09         ; Substract D,DF to value
032e cb 06 e5     lbnf 06e5      ; Long branch on DF=0
0331 f8 39        ldi 39         ; Load D immediate
0333 a7           plo r7         ; Put low register R7
0334 e7           sex r7         ; Set P=R7 as datapointer
0335 6a           inp 2          ; Input to (R(X)) and D, N=010
0336 f8 c6        ldi c6         ; Load D immediate
0338 aa           plo ra         ; Put low register RA
0339 0a           ldn ra         ; Load D with (RA)
033a fa fb        ani fb         ; Logical AND D with value
033c fa fe        ani fe         ; Logical AND D with value
033e 5a           str ra         ; Store D to (RA)
033f ea           sex ra         ; Set P=RA as datapointer
0340 64           out 4          ; Output (R(X)); Increment R(X), N=100
0341 f8 ce        ldi ce         ; Load D immediate
0343 aa           plo ra         ; Put low register RA
0344 0a           ldn ra         ; Load D with (RA)
0345 fa fa        ani fa         ; Logical AND D with value
0347 5a           str ra         ; Store D to (RA)
0348 64           out 4          ; Output (R(X)); Increment R(X), N=100
0349 07           ldn r7         ; Load D with (R7)
034a fa 80        ani 80         ; Logical AND D with value
034c c2 06 e5     lbz 06e5       ; Long branch on D=0
034f d4           sep r4         ; Set P=R4 as program counter
0350 35 a1        b2  a1         ; Short branch on EF2=1
0352 01           ldn r1         ; Load D with (R1)
0353 df           sep rf         ; Set P=RF as program counter
0354 d4           sep r4         ; Set P=R4 as program counter
0355 37 55        b4  55         ; Short branch on EF4=1
0357 8b           glo rb         ; Get low register RB
0358 ff 09        smi 09         ; Substract D,DF to value
035a cb 06 e5     lbnf 06e5      ; Long branch on DF=0
035d d4           sep r4         ; Set P=R4 as program counter
035e 38 21        skp  21        ; Skip next byte
0360 f8 00        ldi 00         ; Load D immediate
0362 ac           plo rc         ; Put low register RC
0363 bc           phi rc         ; Put high register RC
0364 30 6c        br  6c         ; Short branch
0366 f8 00        ldi 00         ; Load D immediate
0368 bc           phi rc         ; Put high register RC
0369 f8 0d        ldi 0d         ; Load D immediate
036b ac           plo rc         ; Put low register RC
036c 9d           ghi rd         ; Get high register RD
036d fa ed        ani ed         ; Logical AND D with value
036f bd           phi rd         ; Put high register RD
0370 f8 25        ldi 25         ; Load D immediate
0372 a7           plo r7         ; Put low register R7
0373 07           ldn r7         ; Load D with (R7)
0374 32 80        bz  80         ; Short branch on D=0
0376 fc 01        adi 01         ; Add D,DF with value
0378 57           str r7         ; Store D to (R7)
0379 fb 41        xri 41         ; Logical XOR D with value
037b 3a 80        bnz  80        ; Short branch on D!=0
037d f8 00        ldi 00         ; Load D immediate
037f 57           str r7         ; Store D to (R7)
0380 17           inc r7         ; Increment (R7)
0381 07           ldn r7         ; Load D with (R7)
0382 fb 6c        xri 6c         ; Logical XOR D with value
0384 3a 8d        bnz  8d        ; Short branch on D!=0
0386 57           str r7         ; Store D to (R7)
0387 d4           sep r4         ; Set P=R4 as program counter
0388 3d ce        bn2  ce        ; Short branch on EF2=0
038a c3 02 c2     lbdf 02c2      ; Long branch on DF=1
038d 98           ghi r8         ; Get high register R8
038e fa 10        ani 10         ; Logical AND D with value
0390 ca 03 9c     lbnz 039c      ; Long branch on D!=0
0393 90           ghi r0         ; Get high register R0
0394 fa c0        ani c0         ; Logical AND D with value
0396 ca 03 9c     lbnz 039c      ; Long branch on D!=0
0399 e3           sex r3         ; Set P=R3 as datapointer
039a c4           nop            ; No operation
039b c4           nop            ; No operation
039c df           sep rf         ; Set P=RF as program counter
039d f8 7f        ldi 7f         ; Load D immediate
039f aa           plo ra         ; Put low register RA
03a0 9c           ghi rc         ; Get high register RC
03a1 ff 02        smi 02         ; Substract D,DF to value
03a3 33 e1        bdf  e1        ; Short branch on DF=1
03a5 0a           ldn ra         ; Load D with (RA)
03a6 fa 03        ani 03         ; Logical AND D with value
03a8 32 b2        bz  b2         ; Short branch on D=0
03aa d4           sep r4         ; Set P=R4 as program counter
03ab 34 72        b1  72         ; Short branch on EF1=1
03ad c3 0b 00     lbdf 0b00      ; Long branch on DF=1
03b0 30 d3        br  d3         ; Short branch
03b2 9e           ghi re         ; Get high register RE
03b3 fa 02        ani 02         ; Logical AND D with value
03b5 3a d3        bnz  d3        ; Short branch on D!=0
03b7 0a           ldn ra         ; Load D with (RA)
03b8 fa 08        ani 08         ; Logical AND D with value
03ba 3a d3        bnz  d3        ; Short branch on D!=0
03bc 88           glo r8         ; Get low register R8
03bd ff 38        smi 38         ; Substract D,DF to value
03bf 3b d3        bnf  d3        ; Short branch on DF=0
03c1 f8 63        ldi 63         ; Load D immediate
03c3 a7           plo r7         ; Put low register R7
03c4 07           ldn r7         ; Load D with (R7)
03c5 fb 0c        xri 0c         ; Logical XOR D with value
03c7 32 d3        bz  d3         ; Short branch on D=0
03c9 fb 08        xri 08         ; Logical XOR D with value
03cb 32 d3        bz  d3         ; Short branch on D=0
03cd 0a           ldn ra         ; Load D with (RA)
03ce fa 03        ani 03         ; Logical AND D with value
03d0 c2 06 e5     lbz 06e5       ; Long branch on D=0
03d3 9d           ghi rd         ; Get high register RD
03d4 fa 02        ani 02         ; Logical AND D with value
03d6 c2 03 8d     lbz 038d       ; Long branch on D=0
03d9 9d           ghi rd         ; Get high register RD
03da fa 88        ani 88         ; Logical AND D with value
03dc fb 88        xri 88         ; Logical XOR D with value
03de c2 04 03     lbz 0403       ; Long branch on D=0
03e1 0a           ldn ra         ; Load D with (RA)
03e2 fa 03        ani 03         ; Logical AND D with value
03e4 c2 06 e5     lbz 06e5       ; Long branch on D=0
03e7 0a           ldn ra         ; Load D with (RA)
03e8 ff 01        smi 01         ; Substract D,DF to value
03ea 5a           str ra         ; Store D to (RA)
03eb fa 03        ani 03         ; Logical AND D with value
03ed c2 06 e5     lbz 06e5       ; Long branch on D=0
03f0 fb 01        xri 01         ; Logical XOR D with value
03f2 ca 03 66     lbnz 0366      ; Long branch on D!=0
03f5 f8 59        ldi 59         ; Load D immediate
03f7 a7           plo r7         ; Put low register R7
03f8 47           lda r7         ; Load D from (R7), increment R7
03f9 57           str r7         ; Store D to (R7)
03fa 0a           ldn ra         ; Load D with (RA)
03fb fa 0f        ani 0f         ; Logical AND D with value
03fd f9 10        ori 10         ; Logical OR D with value
03ff 5a           str ra         ; Store D to (RA)
0400 c0 03 66     lbr 0366       ; Long branch
0403 d4           sep r4         ; Set P=R4 as program counter
0404 3c 59        bn1  59        ; Short branch on EF1=0
0406 cb 04 11     lbnf 0411      ; Long branch on DF=0
0409 9e           ghi re         ; Get high register RE
040a fa 01        ani 01         ; Logical AND D with value
040c c2 05 74     lbz 0574       ; Long branch on D=0
040f 30 14        br  14         ; Short branch
0411 d4           sep r4         ; Set P=R4 as program counter
0412 36 78        b3  78         ; Short branch on EF3=1
0414 f8 7f        ldi 7f         ; Load D immediate
0416 aa           plo ra         ; Put low register RA
0417 0a           ldn ra         ; Load D with (RA)
0418 fa 08        ani 08         ; Logical AND D with value
041a ca 06 9a     lbnz 069a      ; Long branch on D!=0
041d 9b           ghi rb         ; Get high register RB
041e fa f0        ani f0         ; Logical AND D with value
0420 fb c0        xri c0         ; Logical XOR D with value
0422 32 29        bz  29         ; Short branch on D=0
0424 fb 80        xri 80         ; Logical XOR D with value
0426 ca 03 e1     lbnz 03e1      ; Long branch on D!=0
0429 9d           ghi rd         ; Get high register RD
042a fa 40        ani 40         ; Logical AND D with value
042c c2 05 33     lbz 0533       ; Long branch on D=0
042f f8 f5        ldi f5         ; Load D immediate
0431 a7           plo r7         ; Put low register R7
0432 07           ldn r7         ; Load D with (R7)
0433 fa 20        ani 20         ; Logical AND D with value
0435 32 4f        bz  4f         ; Short branch on D=0
0437 f8 f3        ldi f3         ; Load D immediate
0439 a7           plo r7         ; Put low register R7
043a 07           ldn r7         ; Load D with (R7)
043b c2 05 33     lbz 0533       ; Long branch on D=0
043e 52           str r2         ; Store D to (R2)
043f 17           inc r7         ; Increment (R7)
0440 07           ldn r7         ; Load D with (R7)
0441 f3           xor            ; Logical exclusive OR  D with (R(X))
0442 ca 05 33     lbnz 0533      ; Long branch on D!=0
0445 27           dec r7         ; Decrement (R7)
0446 f8 00        ldi 00         ; Load D immediate
0448 57           str r7         ; Store D to (R7)
0449 17           inc r7         ; Increment (R7)
044a 17           inc r7         ; Increment (R7)
044b 07           ldn r7         ; Load D with (R7)
044c fa df        ani df         ; Logical AND D with value
044e 57           str r7         ; Store D to (R7)
044f f8 59        ldi 59         ; Load D immediate
0451 a7           plo r7         ; Put low register R7
0452 47           lda r7         ; Load D from (R7), increment R7
0453 17           inc r7         ; Increment (R7)
0454 57           str r7         ; Store D to (R7)
0455 9e           ghi re         ; Get high register RE
0456 fa 0b        ani 0b         ; Logical AND D with value
0458 be           phi re         ; Put high register RE
0459 0a           ldn ra         ; Load D with (RA)
045a fa fc        ani fc         ; Logical AND D with value
045c f9 03        ori 03         ; Logical OR D with value
045e 5a           str ra         ; Store D to (RA)
045f 99           ghi r9         ; Get high register R9
0460 fa 20        ani 20         ; Logical AND D with value
0462 3a 70        bnz  70        ; Short branch on D!=0
0464 98           ghi r8         ; Get high register R8
0465 fa fe        ani fe         ; Logical AND D with value
0467 b8           phi r8         ; Put high register R8
0468 99           ghi r9         ; Get high register R9
0469 f9 20        ori 20         ; Logical OR D with value
046b b9           phi r9         ; Put high register R9
046c 89           glo r9         ; Get low register R9
046d f9 20        ori 20         ; Logical OR D with value
046f a9           plo r9         ; Put low register R9
0470 9d           ghi rd         ; Get high register RD
0471 fa 10        ani 10         ; Logical AND D with value
0473 ca 0b 00     lbnz 0b00      ; Long branch on D!=0
0476 f8 c6        ldi c6         ; Load D immediate
0478 a7           plo r7         ; Put low register R7
0479 07           ldn r7         ; Load D with (R7)
047a f9 04        ori 04         ; Logical OR D with value
047c fa fe        ani fe         ; Logical AND D with value
047e 57           str r7         ; Store D to (R7)
047f e7           sex r7         ; Set P=R7 as datapointer
0480 64           out 4          ; Output (R(X)); Increment R(X), N=100
0481 d4           sep r4         ; Set P=R4 as program counter
0482 35 a1        b2  a1         ; Short branch on EF2=1
0484 05           ldn r5         ; Load D with (R5)
0485 f8 39        ldi 39         ; Load D immediate
0487 aa           plo ra         ; Put low register RA
0488 ea           sex ra         ; Set P=RA as datapointer
0489 6a           inp 2          ; Input to (R(X)) and D, N=010
048a fa 80        ani 80         ; Logical AND D with value
048c 32 95        bz  95         ; Short branch on D=0
048e f8 28        ldi 28         ; Load D immediate
0490 a7           plo r7         ; Put low register R7
0491 07           ldn r7         ; Load D with (R7)
0492 fc 01        adi 01         ; Add D,DF with value
0494 57           str r7         ; Store D to (R7)
0495 f8 38        ldi 38         ; Load D immediate
0497 a7           plo r7         ; Put low register R7
0498 07           ldn r7         ; Load D with (R7)
0499 fa 01        ani 01         ; Logical AND D with value
049b ca 05 0d     lbnz 050d      ; Long branch on D!=0
049e f8 be        ldi be         ; Load D immediate
04a0 a7           plo r7         ; Put low register R7
04a1 0a           ldn ra         ; Load D with (RA)
04a2 fa 80        ani 80         ; Logical AND D with value
04a4 3a c3        bnz  c3        ; Short branch on D!=0
04a6 07           ldn r7         ; Load D with (R7)
04a7 fa 80        ani 80         ; Logical AND D with value
04a9 32 b3        bz  b3         ; Short branch on D=0
04ab 07           ldn r7         ; Load D with (R7)
04ac fa 7f        ani 7f         ; Logical AND D with value
04ae 57           str r7         ; Store D to (R7)
04af 89           glo r9         ; Get low register R9
04b0 f9 01        ori 01         ; Logical OR D with value
04b2 a9           plo r9         ; Put low register R9
04b3 f8 bb        ldi bb         ; Load D immediate
04b5 a7           plo r7         ; Put low register R7
04b6 07           ldn r7         ; Load D with (R7)
04b7 fa 80        ani 80         ; Logical AND D with value
04b9 c2 05 0d     lbz 050d       ; Long branch on D=0
04bc 07           ldn r7         ; Load D with (R7)
04bd fa 7f        ani 7f         ; Logical AND D with value
04bf 57           str r7         ; Store D to (R7)
04c0 c0 05 09     lbr 0509       ; Long branch
04c3 07           ldn r7         ; Load D with (R7)
04c4 fa 80        ani 80         ; Logical AND D with value
04c6 3a d0        bnz  d0        ; Short branch on D!=0
04c8 07           ldn r7         ; Load D with (R7)
04c9 f9 80        ori 80         ; Logical OR D with value
04cb 57           str r7         ; Store D to (R7)
04cc 89           glo r9         ; Get low register R9
04cd f9 01        ori 01         ; Logical OR D with value
04cf a9           plo r9         ; Put low register R9
04d0 f8 ce        ldi ce         ; Load D immediate
04d2 a7           plo r7         ; Put low register R7
04d3 07           ldn r7         ; Load D with (R7)
04d4 f9 04        ori 04         ; Logical OR D with value
04d6 57           str r7         ; Store D to (R7)
04d7 e7           sex r7         ; Set P=R7 as datapointer
04d8 64           out 4          ; Output (R(X)); Increment R(X), N=100
04d9 f8 c6        ldi c6         ; Load D immediate
04db a7           plo r7         ; Put low register R7
04dc 07           ldn r7         ; Load D with (R7)
04dd fa fb        ani fb         ; Logical AND D with value
04df 57           str r7         ; Store D to (R7)
04e0 64           out 4          ; Output (R(X)); Increment R(X), N=100
04e1 d4           sep r4         ; Set P=R4 as program counter
04e2 35 a1        b2  a1         ; Short branch on EF2=1
04e4 04           ldn r4         ; Load D with (R4)
04e5 f8 bb        ldi bb         ; Load D immediate
04e7 a7           plo r7         ; Put low register R7
04e8 f8 39        ldi 39         ; Load D immediate
04ea aa           plo ra         ; Put low register RA
04eb ea           sex ra         ; Set P=RA as datapointer
04ec 6a           inp 2          ; Input to (R(X)) and D, N=010
04ed fa 80        ani 80         ; Logical AND D with value
04ef ca 04 ff     lbnz 04ff      ; Long branch on D!=0
04f2 07           ldn r7         ; Load D with (R7)
04f3 fa 80        ani 80         ; Logical AND D with value
04f5 c2 05 0d     lbz 050d       ; Long branch on D=0
04f8 07           ldn r7         ; Load D with (R7)
04f9 fa 7f        ani 7f         ; Logical AND D with value
04fb 57           str r7         ; Store D to (R7)
04fc c0 05 09     lbr 0509       ; Long branch
04ff 07           ldn r7         ; Load D with (R7)
0500 fa 80        ani 80         ; Logical AND D with value
0502 ca 05 0d     lbnz 050d      ; Long branch on D!=0
0505 07           ldn r7         ; Load D with (R7)
0506 f9 80        ori 80         ; Logical OR D with value
0508 57           str r7         ; Store D to (R7)
0509 89           glo r9         ; Get low register R9
050a f9 01        ori 01         ; Logical OR D with value
050c a9           plo r9         ; Put low register R9
050d e7           sex r7         ; Set P=R7 as datapointer
050e f8 c6        ldi c6         ; Load D immediate
0510 a7           plo r7         ; Put low register R7
0511 07           ldn r7         ; Load D with (R7)
0512 fa fb        ani fb         ; Logical AND D with value
0514 57           str r7         ; Store D to (R7)
0515 64           out 4          ; Output (R(X)); Increment R(X), N=100
0516 f8 ce        ldi ce         ; Load D immediate
0518 a7           plo r7         ; Put low register R7
0519 07           ldn r7         ; Load D with (R7)
051a fa fb        ani fb         ; Logical AND D with value
051c 57           str r7         ; Store D to (R7)
051d 64           out 4          ; Output (R(X)); Increment R(X), N=100
051e f8 26        ldi 26         ; Load D immediate
0520 a7           plo r7         ; Put low register R7
0521 07           ldn r7         ; Load D with (R7)
0522 fc 01        adi 01         ; Add D,DF with value
0524 57           str r7         ; Store D to (R7)
0525 d4           sep r4         ; Set P=R4 as program counter
0526 39 fb        bnq  fb        ; Short branch on Q=0
0528 3d 2d        bn2  2d        ; Short branch on EF2=0
052a c0 1a ac     lbr 1aac       ; Long branch
052d d4           sep r4         ; Set P=R4 as program counter
052e 3d 1d        bn2  1d        ; Short branch on EF2=0
0530 c0 03 66     lbr 0366       ; Long branch
0533 9e           ghi re         ; Get high register RE
0534 fa 04        ani 04         ; Logical AND D with value
0536 3a 41        bnz  41        ; Short branch on D!=0
0538 9e           ghi re         ; Get high register RE
0539 fa 0f        ani 0f         ; Logical AND D with value
053b f9 04        ori 04         ; Logical OR D with value
053d be           phi re         ; Put high register RE
053e c0 03 f5     lbr 03f5       ; Long branch
0541 0a           ldn ra         ; Load D with (RA)
0542 fa f0        ani f0         ; Logical AND D with value
0544 fb 10        xri 10         ; Logical XOR D with value
0546 3a 4d        bnz  4d        ; Short branch on D!=0
0548 98           ghi r8         ; Get high register R8
0549 fa 01        ani 01         ; Logical AND D with value
054b 3a 5d        bnz  5d        ; Short branch on D!=0
054d 9e           ghi re         ; Get high register RE
054e fa f0        ani f0         ; Logical AND D with value
0550 ff 50        smi 50         ; Substract D,DF to value
0552 c2 06 e5     lbz 06e5       ; Long branch on D=0
0555 9e           ghi re         ; Get high register RE
0556 fa f0        ani f0         ; Logical AND D with value
0558 ff 30        smi 30         ; Substract D,DF to value
055a cb 06 e5     lbnf 06e5      ; Long branch on DF=0
055d d4           sep r4         ; Set P=R4 as program counter
055e 37 8d        b4  8d         ; Short branch on EF4=1
0560 9e           ghi re         ; Get high register RE
0561 fa 0b        ani 0b         ; Logical AND D with value
0563 be           phi re         ; Put high register RE
0564 f8 7f        ldi 7f         ; Load D immediate
0566 aa           plo ra         ; Put low register RA
0567 0a           ldn ra         ; Load D with (RA)
0568 f9 08        ori 08         ; Logical OR D with value
056a 5a           str ra         ; Store D to (RA)
056b f8 f3        ldi f3         ; Load D immediate
056d a7           plo r7         ; Put low register R7
056e f8 00        ldi 00         ; Load D immediate
0570 57           str r7         ; Store D to (R7)
0571 c0 02 c2     lbr 02c2       ; Long branch
0574 9e           ghi re         ; Get high register RE
0575 fa 02        ani 02         ; Logical AND D with value
0577 ca 06 9a     lbnz 069a      ; Long branch on D!=0
057a 80           glo r0         ; Get low register R0
057b fa 10        ani 10         ; Logical AND D with value
057d c2 06 71     lbz 0671       ; Long branch on D=0
0581 68 ca 83 31  rldi ra 8331   ; register load immediate RA
0584 0a           ldn ra         ; Load D with (RA)
0585 fa 10        ani 10         ; Logical AND D with value
0587 ca 06 71     lbnz 0671      ; Long branch on D!=0
058b 68 ca 83 93  rldi ra 8393   ; register load immediate RA
058e 0a           ldn ra         ; Load D with (RA)
058f fb 04        xri 04         ; Logical XOR D with value
0591 3a ac        bnz  ac        ; Short branch on D!=0
0594 68 ca 83 9a  rldi ra 839a   ; register load immediate RA
0597 4a           lda ra         ; Load D from (RA), increment RA
0598 fb 0a        xri 0a         ; Logical XOR D with value
059a 3a ac        bnz  ac        ; Short branch on D!=0
059c 4a           lda ra         ; Load D from (RA), increment RA
059d fb 06        xri 06         ; Logical XOR D with value
059f 3a ac        bnz  ac        ; Short branch on D!=0
05a1 4a           lda ra         ; Load D from (RA), increment RA
05a2 fb 01        xri 01         ; Logical XOR D with value
05a4 3a ac        bnz  ac        ; Short branch on D!=0
05a6 4a           lda ra         ; Load D from (RA), increment RA
05a7 fb 01        xri 01         ; Logical XOR D with value
05a9 c2 06 71     lbz 0671       ; Long branch on D=0
05ad 68 ca 80 01  rldi ra 8001   ; register load immediate RA
05b0 4a           lda ra         ; Load D from (RA), increment RA
05b1 ca 06 57     lbnz 0657      ; Long branch on D!=0
05b4 4a           lda ra         ; Load D from (RA), increment RA
05b5 3a e0        bnz  e0        ; Short branch on D!=0
05b7 4a           lda ra         ; Load D from (RA), increment RA
05b8 3a ed        bnz  ed        ; Short branch on D!=0
05ba f8 04        ldi 04         ; Load D immediate
05bc aa           plo ra         ; Put low register RA
05bd 0a           ldn ra         ; Load D with (RA)
05be c2 05 c7     lbz 05c7       ; Long branch on D=0
05c2 68 ca 80 20  rldi ra 8020   ; register load immediate RA
05c5 30 fa        br  fa         ; Short branch
05c7 f8 05        ldi 05         ; Load D immediate
05c9 aa           plo ra         ; Put low register RA
05ca 0a           ldn ra         ; Load D with (RA)
05cb 32 d3        bz  d3         ; Short branch on D=0
05ce 68 ca 80 30  rldi ra 8030   ; register load immediate RA
05d1 30 fa        br  fa         ; Short branch
05d3 f8 06        ldi 06         ; Load D immediate
05d5 aa           plo ra         ; Put low register RA
05d6 0a           ldn ra         ; Load D with (RA)
05d7 c2 06 71     lbz 0671       ; Long branch on D=0
05db 68 ca 80 40  rldi ra 8040   ; register load immediate RA
05de 30 fa        br  fa         ; Short branch
05e0 80           glo r0         ; Get low register R0
05e1 fa 20        ani 20         ; Logical AND D with value
05e3 c2 06 57     lbz 0657       ; Long branch on D=0
05e6 80           glo r0         ; Get low register R0
05e7 fb 20        xri 20         ; Logical XOR D with value
05e9 a0           plo r0         ; Put low register R0
05ea c0 05 ba     lbr 05ba       ; Long branch
05ed 80           glo r0         ; Get low register R0
05ee fa 40        ani 40         ; Logical AND D with value
05f0 c2 06 57     lbz 0657       ; Long branch on D=0
05f3 80           glo r0         ; Get low register R0
05f4 fb 40        xri 40         ; Logical XOR D with value
05f6 a0           plo r0         ; Put low register R0
05f7 c0 05 ba     lbr 05ba       ; Long branch
05fa f8 93        ldi 93         ; Load D immediate
05fc a7           plo r7         ; Put low register R7
05fd 07           ldn r7         ; Load D with (R7)
05fe ae           plo re         ; Put low register RE
05ff ea           sex ra         ; Set P=RA as datapointer
0600 f8 9a        ldi 9a         ; Load D immediate
0602 a7           plo r7         ; Put low register R7
0603 f0           ldx            ; Pop stack. Place value in D register
0604 c2 06 3c     lbz 063c       ; Long branch on D=0
0607 47           lda r7         ; Load D from (R7), increment R7
0608 f3           xor            ; Logical exclusive OR  D with (R(X))
0609 ca 06 3c     lbnz 063c      ; Long branch on D!=0
060c 60           irx            ; Increment register X
060d f0           ldx            ; Pop stack. Place value in D register
060e c2 06 57     lbz 0657       ; Long branch on D=0
0611 2e           dec re         ; Decrement (RE)
0612 8e           glo re         ; Get low register RE
0613 c2 06 3c     lbz 063c       ; Long branch on D=0
0616 47           lda r7         ; Load D from (R7), increment R7
0617 f3           xor            ; Logical exclusive OR  D with (R(X))
0618 ca 06 3c     lbnz 063c      ; Long branch on D!=0
061b 60           irx            ; Increment register X
061c f0           ldx            ; Pop stack. Place value in D register
061d c2 06 57     lbz 0657       ; Long branch on D=0
0620 2e           dec re         ; Decrement (RE)
0621 8e           glo re         ; Get low register RE
0622 c2 06 3c     lbz 063c       ; Long branch on D=0
0625 47           lda r7         ; Load D from (R7), increment R7
0626 f3           xor            ; Logical exclusive OR  D with (R(X))
0627 ca 06 3c     lbnz 063c      ; Long branch on D!=0
062a 60           irx            ; Increment register X
062b f0           ldx            ; Pop stack. Place value in D register
062c c2 06 57     lbz 0657       ; Long branch on D=0
062f 2e           dec re         ; Decrement (RE)
0630 8e           glo re         ; Get low register RE
0631 c2 06 3c     lbz 063c       ; Long branch on D=0
0634 47           lda r7         ; Load D from (R7), increment R7
0635 f3           xor            ; Logical exclusive OR  D with (R(X))
0636 ca 06 3c     lbnz 063c      ; Long branch on D!=0
0639 c0 06 57     lbr 0657       ; Long branch
063c 8a           glo ra         ; Get low register RA
063d fa f0        ani f0         ; Logical AND D with value
063f aa           plo ra         ; Put low register RA
0640 f8 80        ldi 80         ; Load D immediate
0642 ba           phi ra         ; Put high register RA
0643 8a           glo ra         ; Get low register RA
0644 fb 10        xri 10         ; Logical XOR D with value
0646 c2 05 ba     lbz 05ba       ; Long branch on D=0
0649 8a           glo ra         ; Get low register RA
064a fb 20        xri 20         ; Logical XOR D with value
064c c2 05 c7     lbz 05c7       ; Long branch on D=0
064f 8a           glo ra         ; Get low register RA
0650 fb 40        xri 40         ; Logical XOR D with value
0652 32 71        bz  71         ; Short branch on D=0
0654 c0 05 d3     lbr 05d3       ; Long branch
0657 e2           sex r2         ; Set P=R2 as datapointer
0659 68 ca 83 93  rldi ra 8393   ; register load immediate RA
065c 0a           ldn ra         ; Load D with (RA)
065d 3a 64        bnz  64        ; Short branch on D!=0
065f 99           ghi r9         ; Get high register R9
0660 fa 08        ani 08         ; Logical AND D with value
0662 3a 90        bnz  90        ; Short branch on D!=0
0665 68 ca 83 d7  rldi ra 83d7   ; register load immediate RA
0668 f8 77        ldi 77         ; Load D immediate
066a 5a           str ra         ; Store D to (RA)
066b d4           sep r4         ; Set P=R4 as program counter
066c 36 af        b3  af         ; Short branch on EF3=1
066e c0 02 ca     lbr 02ca       ; Long branch
0671 e2           sex r2         ; Set P=R2 as datapointer
0673 68 ca 83 31  rldi ra 8331   ; register load immediate RA
0676 0a           ldn ra         ; Load D with (RA)
0677 fa ef        ani ef         ; Logical AND D with value
0679 5a           str ra         ; Store D to (RA)
067a d4           sep r4         ; Set P=R4 as program counter
067b 37 2c        b4  2c         ; Short branch on EF4=1
067d d4           sep r4         ; Set P=R4 as program counter
067e 35 8f        b2  8f         ; Short branch on EF2=1
0680 33 90        bdf  90        ; Short branch on DF=1
0682 99           ghi r9         ; Get high register R9
0683 fa 08        ani 08         ; Logical AND D with value
0685 3a 90        bnz  90        ; Short branch on D!=0
0687 99           ghi r9         ; Get high register R9
0688 fa 20        ani 20         ; Logical AND D with value
068a c2 07 06     lbz 0706       ; Long branch on D=0
068d c0 03 60     lbr 0360       ; Long branch
0690 9e           ghi re         ; Get high register RE
0691 f9 02        ori 02         ; Logical OR D with value
0693 be           phi re         ; Put high register RE
0694 d4           sep r4         ; Set P=R4 as program counter
0695 37 d8        b4  d8         ; Short branch on EF4=1
0697 c0 02 c2     lbr 02c2       ; Long branch
069a 9b           ghi rb         ; Get high register RB
069b fa f0        ani f0         ; Logical AND D with value
069d fb 30        xri 30         ; Logical XOR D with value
069f 32 b0        bz  b0         ; Short branch on D=0
06a1 f8 7f        ldi 7f         ; Load D immediate
06a3 aa           plo ra         ; Put low register RA
06a4 0a           ldn ra         ; Load D with (RA)
06a5 fa f0        ani f0         ; Logical AND D with value
06a7 fb 10        xri 10         ; Logical XOR D with value
06a9 32 e5        bz  e5         ; Short branch on D=0
06ab 9b           ghi rb         ; Get high register RB
06ac fb 40        xri 40         ; Logical XOR D with value
06ae 3a e5        bnz  e5        ; Short branch on D!=0
06b0 9d           ghi rd         ; Get high register RD
06b1 fa 40        ani 40         ; Logical AND D with value
06b3 3a c7        bnz  c7        ; Short branch on D!=0
06b5 d4           sep r4         ; Set P=R4 as program counter
06b6 3c 59        bn1  59        ; Short branch on EF1=0
06b8 3b e5        bnf  e5        ; Short branch on DF=0
06ba f8 7f        ldi 7f         ; Load D immediate
06bc aa           plo ra         ; Put low register RA
06bd 0a           ldn ra         ; Load D with (RA)
06be fa 08        ani 08         ; Logical AND D with value
06c0 3a e5        bnz  e5        ; Short branch on D!=0
06c2 99           ghi r9         ; Get high register R9
06c3 fa 08        ani 08         ; Logical AND D with value
06c5 32 e5        bz  e5         ; Short branch on D=0
06c7 d4           sep r4         ; Set P=R4 as program counter
06c8 37 8a        b4  8a         ; Short branch on EF4=1
06ca d4           sep r4         ; Set P=R4 as program counter
06cb 3c 59        bn1  59        ; Short branch on EF1=0
06cd 3b db        bnf  db        ; Short branch on DF=0
06cf f8 2e        ldi 2e         ; Load D immediate
06d1 aa           plo ra         ; Put low register RA
06d2 0a           ldn ra         ; Load D with (RA)
06d3 fa 10        ani 10         ; Logical AND D with value
06d5 ca 0e 52     lbnz 0e52      ; Long branch on D!=0
06d8 c0 07 90     lbr 0790       ; Long branch
06db f8 2e        ldi 2e         ; Load D immediate
06dd aa           plo ra         ; Put low register RA
06de 0a           ldn ra         ; Load D with (RA)
06df fa ef        ani ef         ; Logical AND D with value
06e1 5a           str ra         ; Store D to (RA)
06e2 c0 0e 52     lbr 0e52       ; Long branch
06e5 d4           sep r4         ; Set P=R4 as program counter
06e6 38 3b        skp  3b        ; Skip next byte
06e8 d4           sep r4         ; Set P=R4 as program counter
06e9 3c 59        bn1  59        ; Short branch on EF1=0
06eb 33 f2        bdf  f2        ; Short branch on DF=1
06ed d4           sep r4         ; Set P=R4 as program counter
06ee 36 78        b3  78         ; Short branch on EF3=1
06f0 30 f8        br  f8         ; Short branch
06f2 9e           ghi re         ; Get high register RE
06f3 fa 03        ani 03         ; Logical AND D with value
06f5 c2 05 74     lbz 0574       ; Long branch on D=0
06f8 99           ghi r9         ; Get high register R9
06f9 fa 20        ani 20         ; Logical AND D with value
06fb c2 07 06     lbz 0706       ; Long branch on D=0
06fe 99           ghi r9         ; Get high register R9
06ff fa df        ani df         ; Logical AND D with value
0701 b9           phi r9         ; Put high register R9
0702 89           glo r9         ; Get low register R9
0703 f9 20        ori 20         ; Logical OR D with value
0705 a9           plo r9         ; Put low register R9
0706 f8 59        ldi 59         ; Load D immediate
0708 aa           plo ra         ; Put low register RA
0709 0a           ldn ra         ; Load D with (RA)
070a fc 01        adi 01         ; Add D,DF with value
070c 5a           str ra         ; Store D to (RA)
070d fb df        xri df         ; Logical XOR D with value
070f 3a 14        bnz  14        ; Short branch on D!=0
0711 f8 01        ldi 01         ; Load D immediate
0713 5a           str ra         ; Store D to (RA)
0714 d4           sep r4         ; Set P=R4 as program counter
0715 35 d5        b2  d5         ; Short branch on EF2=1
0717 f8 59        ldi 59         ; Load D immediate
0719 aa           plo ra         ; Put low register RA
071a 0a           ldn ra         ; Load D with (RA)
071b fb 01        xri 01         ; Logical XOR D with value
071d ca 07 27     lbnz 0727      ; Long branch on D!=0
0720 d4           sep r4         ; Set P=R4 as program counter
0721 35 a1        b2  a1         ; Short branch on EF2=1
0723 14           inc r4         ; Increment (R4)
0724 f8 59        ldi 59         ; Load D immediate
0726 aa           plo ra         ; Put low register RA
0727 4a           lda ra         ; Load D from (RA), increment RA
0728 52           str r2         ; Store D to (R2)
0729 0a           ldn ra         ; Load D with (RA)
072a f3           xor            ; Logical exclusive OR  D with (R(X))
072b ca 02 e9     lbnz 02e9      ; Long branch on D!=0
072e f8 7f        ldi 7f         ; Load D immediate
0730 aa           plo ra         ; Put low register RA
0731 0a           ldn ra         ; Load D with (RA)
0732 fa f0        ani f0         ; Logical AND D with value
0734 fb 50        xri 50         ; Logical XOR D with value
0736 32 3c        bz  3c         ; Short branch on D=0
0738 0a           ldn ra         ; Load D with (RA)
0739 fc 10        adi 10         ; Add D,DF with value
073b 5a           str ra         ; Store D to (RA)
073c 9e           ghi re         ; Get high register RE
073d fa 02        ani 02         ; Logical AND D with value
073f c2 07 5c     lbz 075c       ; Long branch on D=0
0742 9e           ghi re         ; Get high register RE
0743 fa f0        ani f0         ; Logical AND D with value
0745 fb f0        xri f0         ; Logical XOR D with value
0747 ca 07 89     lbnz 0789      ; Long branch on D!=0
074a 99           ghi r9         ; Get high register R9
074b fa 08        ani 08         ; Logical AND D with value
074d c2 07 56     lbz 0756       ; Long branch on D=0
0750 d4           sep r4         ; Set P=R4 as program counter
0751 36 af        b3  af         ; Short branch on EF3=1
0753 c0 07 78     lbr 0778       ; Long branch
0756 d4           sep r4         ; Set P=R4 as program counter
0757 36 a3        b3  a3         ; Short branch on EF3=1
0759 c0 07 78     lbr 0778       ; Long branch
075c 0a           ldn ra         ; Load D with (RA)
075d fa 08        ani 08         ; Logical AND D with value
075f 3a 6a        bnz  6a        ; Short branch on D!=0
0761 9e           ghi re         ; Get high register RE
0762 fa 04        ani 04         ; Logical AND D with value
0764 ca 07 89     lbnz 0789      ; Long branch on D!=0
0767 c0 02 e9     lbr 02e9       ; Long branch
076a 9e           ghi re         ; Get high register RE
076b fa f0        ani f0         ; Logical AND D with value
076d fb e0        xri e0         ; Logical XOR D with value
076f 3a 89        bnz  89        ; Short branch on D!=0
0771 f8 f5        ldi f5         ; Load D immediate
0773 a7           plo r7         ; Put low register R7
0774 07           ldn r7         ; Load D with (R7)
0775 f9 20        ori 20         ; Logical OR D with value
0777 57           str r7         ; Store D to (R7)
0778 9e           ghi re         ; Get high register RE
0779 fa 09        ani 09         ; Logical AND D with value
077b be           phi re         ; Put high register RE
077c f8 7f        ldi 7f         ; Load D immediate
077e aa           plo ra         ; Put low register RA
077f 0a           ldn ra         ; Load D with (RA)
0780 fa f7        ani f7         ; Logical AND D with value
0782 5a           str ra         ; Store D to (RA)
0783 d4           sep r4         ; Set P=R4 as program counter
0784 37 e6        b4  e6         ; Short branch on EF4=1
0786 c0 02 e9     lbr 02e9       ; Long branch
0789 9e           ghi re         ; Get high register RE
078a fc 10        adi 10         ; Add D,DF with value
078c be           phi re         ; Put high register RE
078d c0 02 e9     lbr 02e9       ; Long branch
0790 df           sep rf         ; Set P=RF as program counter
0791 88           glo r8         ; Get low register R8
0792 ff 38        smi 38         ; Substract D,DF to value
0794 cb 07 90     lbnf 0790      ; Long branch on DF=0
0798 68 c7 83 2e  rldi r7 832e   ; register load immediate R7
079b 07           ldn r7         ; Load D with (R7)
079c fa fd        ani fd         ; Logical AND D with value
079e 57           str r7         ; Store D to (R7)
079f f8 63        ldi 63         ; Load D immediate
07a1 a7           plo r7         ; Put low register R7
07a2 07           ldn r7         ; Load D with (R7)
07a3 fb 03        xri 03         ; Logical XOR D with value
07a5 32 ac        bz  ac         ; Short branch on D=0
07a7 fb 07        xri 07         ; Logical XOR D with value
07a9 ca 06 e5     lbnz 06e5      ; Long branch on D!=0
07ac d4           sep r4         ; Set P=R4 as program counter
07ad 13           inc r3         ; Increment (R3)
07ae 30 00        br  00         ; Short branch
07b0 f1           or             ; Logical OR  D with (R(X))
07b2 68 ca 83 ff  rldi ra 83ff   ; register load immediate RA
07b5 f8 00        ldi 00         ; Load D immediate
07b7 5a           str ra         ; Store D to (RA)
07b9 68 ca 83 70  rldi ra 8370   ; register load immediate RA
07bc 0a           ldn ra         ; Load D with (RA)
07bd fa fd        ani fd         ; Logical AND D with value
07bf fa fb        ani fb         ; Logical AND D with value
07c1 5a           str ra         ; Store D to (RA)
07c2 d4           sep r4         ; Set P=R4 as program counter
07c3 13           inc r3         ; Increment (R3)
07c4 30 10        br  10         ; Short branch
07c6 02           ldn r2         ; Load D with (R2)
07c7 d4           sep r4         ; Set P=R4 as program counter
07c8 38 44        skp  44        ; Skip next byte
07ca df           sep rf         ; Set P=RF as program counter
07cb 9c           ghi rc         ; Get high register RC
07cc ff 0a        smi 0a         ; Substract D,DF to value
07ce c3 07 ef     lbdf 07ef      ; Long branch on DF=1
07d1 9d           ghi rd         ; Get high register RD
07d2 fa 12        ani 12         ; Logical AND D with value
07d4 fb 12        xri 12         ; Logical XOR D with value
07d6 ca 07 ca     lbnz 07ca      ; Long branch on D!=0
07d9 9d           ghi rd         ; Get high register RD
07da fa 20        ani 20         ; Logical AND D with value
07dc ca 09 0d     lbnz 090d      ; Long branch on D!=0
07df 9b           ghi rb         ; Get high register RB
07e0 fa f0        ani f0         ; Logical AND D with value
07e2 fb 50        xri 50         ; Logical XOR D with value
07e4 c2 07 f9     lbz 07f9       ; Long branch on D=0
07e7 d4           sep r4         ; Set P=R4 as program counter
07e8 3c 59        bn1  59        ; Short branch on EF1=0
07ea c3 07 ca     lbdf 07ca      ; Long branch on DF=1
07ed 30 f6        br  f6         ; Short branch
07ef d4           sep r4         ; Set P=R4 as program counter
07f0 36 a3        b3  a3         ; Short branch on EF3=1
07f2 98           ghi r8         ; Get high register R8
07f3 f9 01        ori 01         ; Logical OR D with value
07f5 b8           phi r8         ; Put high register R8
07f6 c0 02 ca     lbr 02ca       ; Long branch
07f9 d4           sep r4         ; Set P=R4 as program counter
07fa 3a a5        bnz  a5        ; Short branch on D!=0
07fc 0a           ldn ra         ; Load D with (RA)
07fd d4           sep r4         ; Set P=R4 as program counter
07fe 13           inc r3         ; Increment (R3)
07ff 30 00        br  00         ; Short branch
0801 f1           or             ; Logical OR  D with (R(X))
0802 d4           sep r4         ; Set P=R4 as program counter
0803 13           inc r3         ; Increment (R3)
0804 30 10        br  10         ; Short branch
0806 04           ldn r4         ; Load D with (R4)
0807 df           sep rf         ; Set P=RF as program counter
0808 d4           sep r4         ; Set P=R4 as program counter
0809 3c 59        bn1  59        ; Short branch on EF1=0
080b cb 09 16     lbnf 0916      ; Long branch on DF=0
080e 9c           ghi rc         ; Get high register RC
080f ff 0e        smi 0e         ; Substract D,DF to value
0811 c3 09 13     lbdf 0913      ; Long branch on DF=1
0814 90           ghi r0         ; Get high register R0
0815 fa 0f        ani 0f         ; Logical AND D with value
0817 fb 0f        xri 0f         ; Logical XOR D with value
0819 c2 08 29     lbz 0829       ; Long branch on D=0
081c fb 0e        xri 0e         ; Logical XOR D with value
081e ca 08 41     lbnz 0841      ; Long branch on D!=0
0821 d4           sep r4         ; Set P=R4 as program counter
0822 13           inc r3         ; Increment (R3)
0823 30 00        br  00         ; Short branch
0825 0f           ldn rf         ; Load D with (RF)
0826 c0 08 41     lbr 0841       ; Long branch
082a 68 ca 83 ff  rldi ra 83ff   ; register load immediate RA
082d 0a           ldn ra         ; Load D with (RA)
082e fa cf        ani cf         ; Logical AND D with value
0830 fb 8f        xri 8f         ; Logical XOR D with value
0832 ca 08 41     lbnz 0841      ; Long branch on D!=0
0835 0a           ldn ra         ; Load D with (RA)
0836 f9 40        ori 40         ; Logical OR D with value
0838 5a           str ra         ; Store D to (RA)
0839 d4           sep r4         ; Set P=R4 as program counter
083a 13           inc r3         ; Increment (R3)
083b 30 c0        br  c0         ; Short branch
083d 02           ldn r2         ; Load D with (R2)
083e c0 08 07     lbr 0807       ; Long branch
0842 68 ca 83 ff  rldi ra 83ff   ; register load immediate RA
0845 0a           ldn ra         ; Load D with (RA)
0846 fa cf        ani cf         ; Logical AND D with value
0848 32 4e        bz  4e         ; Short branch on D=0
084a fa 40        ani 40         ; Logical AND D with value
084c 32 58        bz  58         ; Short branch on D=0
084f 68 ca 83 70  rldi ra 8370   ; register load immediate RA
0852 0a           ldn ra         ; Load D with (RA)
0853 fa 04        ani 04         ; Logical AND D with value
0855 ca 08 99     lbnz 0899      ; Long branch on D!=0
0858 9d           ghi rd         ; Get high register RD
0859 fa 12        ani 12         ; Logical AND D with value
085b fb 12        xri 12         ; Logical XOR D with value
085d ca 08 07     lbnz 0807      ; Long branch on D!=0
0860 9d           ghi rd         ; Get high register RD
0861 fa 20        ani 20         ; Logical AND D with value
0863 ca 09 0d     lbnz 090d      ; Long branch on D!=0
0867 68 ca 83 ff  rldi ra 83ff   ; register load immediate RA
086a 0a           ldn ra         ; Load D with (RA)
086b fa 8f        ani 8f         ; Logical AND D with value
086d c2 08 83     lbz 0883       ; Long branch on D=0
0870 d4           sep r4         ; Set P=R4 as program counter
0871 34 35        b1  35         ; Short branch on EF1=1
0873 60           irx            ; Increment register X
0874 1b           inc rb         ; Increment (RB)
0875 cb 08 83     lbnf 0883      ; Long branch on DF=0
0879 68 ca 83 70  rldi ra 8370   ; register load immediate RA
087c 0a           ldn ra         ; Load D with (RA)
087d f9 02        ori 02         ; Logical OR D with value
087f 5a           str ra         ; Store D to (RA)
0880 c0 08 8b     lbr 088b       ; Long branch
0883 d4           sep r4         ; Set P=R4 as program counter
0884 34 35        b1  35         ; Short branch on EF1=1
0886 60           irx            ; Increment register X
0887 13           inc r3         ; Increment (R3)
0888 cb 08 07     lbnf 0807      ; Long branch on DF=0
088c 68 ca 83 70  rldi ra 8370   ; register load immediate RA
088f 0a           ldn ra         ; Load D with (RA)
0890 f9 04        ori 04         ; Logical OR D with value
0892 5a           str ra         ; Store D to (RA)
0893 f8 00        ldi 00         ; Load D immediate
0895 bc           phi rc         ; Put high register RC
0896 c0 08 07     lbr 0807       ; Long branch
0899 d4           sep r4         ; Set P=R4 as program counter
089a 3a 5c        bnz  5c        ; Short branch on D!=0
089c d4           sep r4         ; Set P=R4 as program counter
089d 38 8f        skp  8f        ; Skip next byte
089f df           sep rf         ; Set P=RF as program counter
08a0 d4           sep r4         ; Set P=R4 as program counter
08a1 38 96        skp  96        ; Skip next byte
08a3 90           ghi r0         ; Get high register R0
08a4 fa 0f        ani 0f         ; Logical AND D with value
08a6 fb 0f        xri 0f         ; Logical XOR D with value
08a8 c2 08 b0     lbz 08b0       ; Long branch on D=0
08ab fb 0e        xri 0e         ; Logical XOR D with value
08ad ca 08 9f     lbnz 089f      ; Long branch on D!=0
08b0 98           ghi r8         ; Get high register R8
08b1 fa 7f        ani 7f         ; Logical AND D with value
08b3 b8           phi r8         ; Put high register R8
08b5 68 ca 87 73  rldi ra 8773   ; register load immediate RA
08b8 f8 06        ldi 06         ; Load D immediate
08ba 5a           str ra         ; Store D to (RA)
08bc 68 ca 83 7f  rldi ra 837f   ; register load immediate RA
08bf 98           ghi r8         ; Get high register R8
08c0 fa 80        ani 80         ; Logical AND D with value
08c2 ca 08 f4     lbnz 08f4      ; Long branch on D!=0
08c5 d4           sep r4         ; Set P=R4 as program counter
08c6 13           inc r3         ; Increment (R3)
08c7 30 70        br  70         ; Short branch
08c9 02           ldn r2         ; Load D with (R2)
08ca df           sep rf         ; Set P=RF as program counter
08cb d4           sep r4         ; Set P=R4 as program counter
08cc 38 96        skp  96        ; Skip next byte
08ce 9d           ghi rd         ; Get high register RD
08cf fa 12        ani 12         ; Logical AND D with value
08d1 fb 12        xri 12         ; Logical XOR D with value
08d3 3a e3        bnz  e3        ; Short branch on D!=0
08d5 d4           sep r4         ; Set P=R4 as program counter
08d6 34 35        b1  35         ; Short branch on EF1=1
08d8 60           irx            ; Increment register X
08d9 16           inc r6         ; Increment (R6)
08da 3b e3        bnf  e3        ; Short branch on DF=0
08dc 80           glo r0         ; Get low register R0
08dd fa fe        ani fe         ; Logical AND D with value
08df a0           plo r0         ; Put low register R0
08e0 c0 0c 48     lbr 0c48       ; Long branch
08e3 9d           ghi rd         ; Get high register RD
08e4 fa 20        ani 20         ; Logical AND D with value
08e6 ca 09 0d     lbnz 090d      ; Long branch on D!=0
08e9 90           ghi r0         ; Get high register R0
08ea fa 0f        ani 0f         ; Logical AND D with value
08ec fb 01        xri 01         ; Logical XOR D with value
08ee ca 08 ca     lbnz 08ca      ; Long branch on D!=0
08f1 c0 08 bf     lbr 08bf       ; Long branch
08f4 d4           sep r4         ; Set P=R4 as program counter
08f5 13           inc r3         ; Increment (R3)
08f6 30 00        br  00         ; Short branch
08f8 0f           ldn rf         ; Load D with (RF)
08f9 f8 00        ldi 00         ; Load D immediate
08fb bc           phi rc         ; Put high register RC
08fc 80           glo r0         ; Get low register R0
08fd fa fe        ani fe         ; Logical AND D with value
08ff a0           plo r0         ; Put low register R0
0900 df           sep rf         ; Set P=RF as program counter
0901 9c           ghi rc         ; Get high register RC
0902 ff d6        smi d6         ; Substract D,DF to value
0904 cb 09 1d     lbnf 091d      ; Long branch on DF=0
0907 d4           sep r4         ; Set P=R4 as program counter
0908 36 a3        b3  a3         ; Short branch on EF3=1
090a c0 0f 19     lbr 0f19       ; Long branch
090d d4           sep r4         ; Set P=R4 as program counter
090e 36 a3        b3  a3         ; Short branch on EF3=1
0910 c0 0f 26     lbr 0f26       ; Long branch
0913 d4           sep r4         ; Set P=R4 as program counter
0914 36 a3        b3  a3         ; Short branch on EF3=1
0916 98           ghi r8         ; Get high register R8
0917 f9 01        ori 01         ; Logical OR D with value
0919 b8           phi r8         ; Put high register R8
091a c0 0f 19     lbr 0f19       ; Long branch
091d d4           sep r4         ; Set P=R4 as program counter
091e 3c 59        bn1  59        ; Short branch on EF1=0
0920 cb 0f 19     lbnf 0f19      ; Long branch on DF=0
0923 9d           ghi rd         ; Get high register RD
0924 fa 12        ani 12         ; Logical AND D with value
0926 fb 12        xri 12         ; Logical XOR D with value
0928 ca 09 00     lbnz 0900      ; Long branch on D!=0
092b d4           sep r4         ; Set P=R4 as program counter
092c 34 35        b1  35         ; Short branch on EF1=1
092e 60           irx            ; Increment register X
092f 16           inc r6         ; Increment (R6)
0930 c3 09 3c     lbdf 093c      ; Long branch on DF=1
0933 9d           ghi rd         ; Get high register RD
0934 fa 20        ani 20         ; Logical AND D with value
0936 ca 09 0d     lbnz 090d      ; Long branch on D!=0
0939 c0 09 00     lbr 0900       ; Long branch
093c 90           ghi r0         ; Get high register R0
093d fa f0        ani f0         ; Logical AND D with value
093f f9 02        ori 02         ; Logical OR D with value
0941 b0           phi r0         ; Put high register R0
0942 d4           sep r4         ; Set P=R4 as program counter
0943 3b 25        bnf  25        ; Short branch on DF=0
0945 c0 0c 48     lbr 0c48       ; Long branch
0948 ff ff        smi ff         ; Substract D,DF to value
094a ff ff        smi ff         ; Substract D,DF to value
094c ff ff        smi ff         ; Substract D,DF to value
094e ff ff        smi ff         ; Substract D,DF to value
0950 ff ff        smi ff         ; Substract D,DF to value
0952 ff ff        smi ff         ; Substract D,DF to value
0954 ff ff        smi ff         ; Substract D,DF to value
0956 ff ff        smi ff         ; Substract D,DF to value
0958 ff ff        smi ff         ; Substract D,DF to value
095a ff ff        smi ff         ; Substract D,DF to value
095c ff ff        smi ff         ; Substract D,DF to value
095e ff ff        smi ff         ; Substract D,DF to value
0960 ff ff        smi ff         ; Substract D,DF to value
0962 ff ff        smi ff         ; Substract D,DF to value
0964 ff ff        smi ff         ; Substract D,DF to value
0966 ff ff        smi ff         ; Substract D,DF to value
0968 ff ff        smi ff         ; Substract D,DF to value
096a ff ff        smi ff         ; Substract D,DF to value
096c ff ff        smi ff         ; Substract D,DF to value
096e ff ff        smi ff         ; Substract D,DF to value
0970 ff ff        smi ff         ; Substract D,DF to value
0972 ff ff        smi ff         ; Substract D,DF to value
0974 ff ff        smi ff         ; Substract D,DF to value
0976 ff ff        smi ff         ; Substract D,DF to value
0978 ff ff        smi ff         ; Substract D,DF to value
097a ff ff        smi ff         ; Substract D,DF to value
097c ff ff        smi ff         ; Substract D,DF to value
097e ff ff        smi ff         ; Substract D,DF to value
0980 ff ff        smi ff         ; Substract D,DF to value
0982 ff ff        smi ff         ; Substract D,DF to value
0984 ff ff        smi ff         ; Substract D,DF to value
0986 ff ff        smi ff         ; Substract D,DF to value
0988 ff ff        smi ff         ; Substract D,DF to value
098a ff ff        smi ff         ; Substract D,DF to value
098c ff ff        smi ff         ; Substract D,DF to value
098e ff ff        smi ff         ; Substract D,DF to value
0990 ff ff        smi ff         ; Substract D,DF to value
0992 ff ff        smi ff         ; Substract D,DF to value
0994 ff ff        smi ff         ; Substract D,DF to value
0996 ff ff        smi ff         ; Substract D,DF to value
0998 ff ff        smi ff         ; Substract D,DF to value
099a ff ff        smi ff         ; Substract D,DF to value
099c ff ff        smi ff         ; Substract D,DF to value
099e ff ff        smi ff         ; Substract D,DF to value
09a0 ff ff        smi ff         ; Substract D,DF to value
09a2 ff ff        smi ff         ; Substract D,DF to value
09a4 ff ff        smi ff         ; Substract D,DF to value
09a6 ff ff        smi ff         ; Substract D,DF to value
09a8 ff ff        smi ff         ; Substract D,DF to value
09aa ff ff        smi ff         ; Substract D,DF to value
09ac ff ff        smi ff         ; Substract D,DF to value
09ae ff ff        smi ff         ; Substract D,DF to value
09b0 ff ff        smi ff         ; Substract D,DF to value
09b2 ff ff        smi ff         ; Substract D,DF to value
09b4 ff ff        smi ff         ; Substract D,DF to value
09b6 ff ff        smi ff         ; Substract D,DF to value
09b8 ff ff        smi ff         ; Substract D,DF to value
09ba ff ff        smi ff         ; Substract D,DF to value
09bc ff ff        smi ff         ; Substract D,DF to value
09be ff ff        smi ff         ; Substract D,DF to value
09c0 ff ff        smi ff         ; Substract D,DF to value
09c2 ff ff        smi ff         ; Substract D,DF to value
09c4 ff ff        smi ff         ; Substract D,DF to value
09c6 ff ff        smi ff         ; Substract D,DF to value
09c8 ff ff        smi ff         ; Substract D,DF to value
09ca ff ff        smi ff         ; Substract D,DF to value
09cc ff ff        smi ff         ; Substract D,DF to value
09ce ff ff        smi ff         ; Substract D,DF to value
09d0 ff ff        smi ff         ; Substract D,DF to value
09d2 ff ff        smi ff         ; Substract D,DF to value
09d4 ff ff        smi ff         ; Substract D,DF to value
09d6 ff ff        smi ff         ; Substract D,DF to value
09d8 ff ff        smi ff         ; Substract D,DF to value
09da ff ff        smi ff         ; Substract D,DF to value
09dc ff ff        smi ff         ; Substract D,DF to value
09de ff ff        smi ff         ; Substract D,DF to value
09e0 ff ff        smi ff         ; Substract D,DF to value
09e2 ff ff        smi ff         ; Substract D,DF to value
09e4 ff ff        smi ff         ; Substract D,DF to value
09e6 ff ff        smi ff         ; Substract D,DF to value
09e8 ff ff        smi ff         ; Substract D,DF to value
09ea ff ff        smi ff         ; Substract D,DF to value
09ec ff ff        smi ff         ; Substract D,DF to value
09ee ff ff        smi ff         ; Substract D,DF to value
09f0 ff ff        smi ff         ; Substract D,DF to value
09f2 ff ff        smi ff         ; Substract D,DF to value
09f4 ff ff        smi ff         ; Substract D,DF to value
09f6 ff ff        smi ff         ; Substract D,DF to value
09f8 ff ff        smi ff         ; Substract D,DF to value
09fa ff ff        smi ff         ; Substract D,DF to value
09fc ff ff        smi ff         ; Substract D,DF to value
09fe ff ff        smi ff         ; Substract D,DF to value
0a00 ff ff        smi ff         ; Substract D,DF to value
0a02 ff ff        smi ff         ; Substract D,DF to value
0a04 ff ff        smi ff         ; Substract D,DF to value
0a06 ff ff        smi ff         ; Substract D,DF to value
0a08 ff ff        smi ff         ; Substract D,DF to value
0a0a ff ff        smi ff         ; Substract D,DF to value
0a0c ff ff        smi ff         ; Substract D,DF to value
0a0e ff ff        smi ff         ; Substract D,DF to value
0a10 ff ff        smi ff         ; Substract D,DF to value
0a12 ff ff        smi ff         ; Substract D,DF to value
0a14 ff ff        smi ff         ; Substract D,DF to value
0a16 ff ff        smi ff         ; Substract D,DF to value
0a18 ff ff        smi ff         ; Substract D,DF to value
0a1a ff ff        smi ff         ; Substract D,DF to value
0a1c ff ff        smi ff         ; Substract D,DF to value
0a1e ff ff        smi ff         ; Substract D,DF to value
0a20 ff ff        smi ff         ; Substract D,DF to value
0a22 ff ff        smi ff         ; Substract D,DF to value
0a24 ff ff        smi ff         ; Substract D,DF to value
0a26 ff ff        smi ff         ; Substract D,DF to value
0a28 ff ff        smi ff         ; Substract D,DF to value
0a2a ff ff        smi ff         ; Substract D,DF to value
0a2c ff ff        smi ff         ; Substract D,DF to value
0a2e ff ff        smi ff         ; Substract D,DF to value
0a30 ff ff        smi ff         ; Substract D,DF to value
0a32 ff ff        smi ff         ; Substract D,DF to value
0a34 ff ff        smi ff         ; Substract D,DF to value
0a36 ff ff        smi ff         ; Substract D,DF to value
0a38 ff ff        smi ff         ; Substract D,DF to value
0a3a ff ff        smi ff         ; Substract D,DF to value
0a3c ff ff        smi ff         ; Substract D,DF to value
0a3e ff ff        smi ff         ; Substract D,DF to value
0a40 ff ff        smi ff         ; Substract D,DF to value
0a42 ff ff        smi ff         ; Substract D,DF to value
0a44 ff ff        smi ff         ; Substract D,DF to value
0a46 ff ff        smi ff         ; Substract D,DF to value
0a48 ff ff        smi ff         ; Substract D,DF to value
0a4a ff ff        smi ff         ; Substract D,DF to value
0a4c ff ff        smi ff         ; Substract D,DF to value
0a4e ff ff        smi ff         ; Substract D,DF to value
0a50 ff ff        smi ff         ; Substract D,DF to value
0a52 ff ff        smi ff         ; Substract D,DF to value
0a54 ff ff        smi ff         ; Substract D,DF to value
0a56 ff ff        smi ff         ; Substract D,DF to value
0a58 ff ff        smi ff         ; Substract D,DF to value
0a5a ff ff        smi ff         ; Substract D,DF to value
0a5c ff ff        smi ff         ; Substract D,DF to value
0a5e ff ff        smi ff         ; Substract D,DF to value
0a60 ff ff        smi ff         ; Substract D,DF to value
0a62 ff ff        smi ff         ; Substract D,DF to value
0a64 ff ff        smi ff         ; Substract D,DF to value
0a66 ff ff        smi ff         ; Substract D,DF to value
0a68 ff ff        smi ff         ; Substract D,DF to value
0a6a ff ff        smi ff         ; Substract D,DF to value
0a6c ff ff        smi ff         ; Substract D,DF to value
0a6e ff ff        smi ff         ; Substract D,DF to value
0a70 ff ff        smi ff         ; Substract D,DF to value
0a72 ff ff        smi ff         ; Substract D,DF to value
0a74 ff ff        smi ff         ; Substract D,DF to value
0a76 ff ff        smi ff         ; Substract D,DF to value
0a78 ff ff        smi ff         ; Substract D,DF to value
0a7a ff ff        smi ff         ; Substract D,DF to value
0a7c ff ff        smi ff         ; Substract D,DF to value
0a7e ff ff        smi ff         ; Substract D,DF to value
0a80 ff ff        smi ff         ; Substract D,DF to value
0a82 ff ff        smi ff         ; Substract D,DF to value
0a84 ff ff        smi ff         ; Substract D,DF to value
0a86 ff ff        smi ff         ; Substract D,DF to value
0a88 ff ff        smi ff         ; Substract D,DF to value
0a8a ff ff        smi ff         ; Substract D,DF to value
0a8c ff ff        smi ff         ; Substract D,DF to value
0a8e ff ff        smi ff         ; Substract D,DF to value
0a90 ff ff        smi ff         ; Substract D,DF to value
0a92 ff ff        smi ff         ; Substract D,DF to value
0a94 ff ff        smi ff         ; Substract D,DF to value
0a96 ff ff        smi ff         ; Substract D,DF to value
0a98 ff ff        smi ff         ; Substract D,DF to value
0a9a ff ff        smi ff         ; Substract D,DF to value
0a9c ff ff        smi ff         ; Substract D,DF to value
0a9e ff ff        smi ff         ; Substract D,DF to value
0aa0 ff ff        smi ff         ; Substract D,DF to value
0aa2 ff ff        smi ff         ; Substract D,DF to value
0aa4 ff ff        smi ff         ; Substract D,DF to value
0aa6 ff ff        smi ff         ; Substract D,DF to value
0aa8 ff ff        smi ff         ; Substract D,DF to value
0aaa ff ff        smi ff         ; Substract D,DF to value
0aac ff ff        smi ff         ; Substract D,DF to value
0aae ff ff        smi ff         ; Substract D,DF to value
0ab0 ff ff        smi ff         ; Substract D,DF to value
0ab2 ff ff        smi ff         ; Substract D,DF to value
0ab4 ff ff        smi ff         ; Substract D,DF to value
0ab6 ff ff        smi ff         ; Substract D,DF to value
0ab8 ff ff        smi ff         ; Substract D,DF to value
0aba ff ff        smi ff         ; Substract D,DF to value
0abc ff ff        smi ff         ; Substract D,DF to value
0abe ff ff        smi ff         ; Substract D,DF to value
0ac0 ff ff        smi ff         ; Substract D,DF to value
0ac2 ff ff        smi ff         ; Substract D,DF to value
0ac4 ff ff        smi ff         ; Substract D,DF to value
0ac6 ff ff        smi ff         ; Substract D,DF to value
0ac8 ff ff        smi ff         ; Substract D,DF to value
0aca ff ff        smi ff         ; Substract D,DF to value
0acc ff ff        smi ff         ; Substract D,DF to value
0ace ff ff        smi ff         ; Substract D,DF to value
0ad0 ff ff        smi ff         ; Substract D,DF to value
0ad2 ff ff        smi ff         ; Substract D,DF to value
0ad4 ff ff        smi ff         ; Substract D,DF to value
0ad6 ff ff        smi ff         ; Substract D,DF to value
0ad8 ff ff        smi ff         ; Substract D,DF to value
0ada ff ff        smi ff         ; Substract D,DF to value
0adc ff ff        smi ff         ; Substract D,DF to value
0ade ff ff        smi ff         ; Substract D,DF to value
0ae0 ff ff        smi ff         ; Substract D,DF to value
0ae2 ff ff        smi ff         ; Substract D,DF to value
0ae4 ff ff        smi ff         ; Substract D,DF to value
0ae6 ff ff        smi ff         ; Substract D,DF to value
0ae8 ff ff        smi ff         ; Substract D,DF to value
0aea ff ff        smi ff         ; Substract D,DF to value
0aec ff ff        smi ff         ; Substract D,DF to value
0aee ff ff        smi ff         ; Substract D,DF to value
0af0 ff ff        smi ff         ; Substract D,DF to value
0af2 ff ff        smi ff         ; Substract D,DF to value
0af4 ff ff        smi ff         ; Substract D,DF to value
0af6 ff ff        smi ff         ; Substract D,DF to value
0af8 ff ff        smi ff         ; Substract D,DF to value
0afa ff ff        smi ff         ; Substract D,DF to value
0afc ff ff        smi ff         ; Substract D,DF to value
0afe ff ff        smi ff         ; Substract D,DF to value
0b00 d4           sep r4         ; Set P=R4 as program counter
0b01 13           inc r3         ; Increment (R3)
0b02 30 10        br  10         ; Short branch
0b04 f1           or             ; Logical OR  D with (R(X))
0b05 d4           sep r4         ; Set P=R4 as program counter
0b06 38 17        skp  17        ; Skip next byte
0b08 f8 25        ldi 25         ; Load D immediate
0b0a a7           plo r7         ; Put low register R7
0b0b 07           ldn r7         ; Load D with (R7)
0b0c 32 10        bz  10         ; Short branch on D=0
0b0e f9 80        ori 80         ; Logical OR D with value
0b10 f9 01        ori 01         ; Logical OR D with value
0b12 57           str r7         ; Store D to (R7)
0b13 d4           sep r4         ; Set P=R4 as program counter
0b14 38 44        skp  44        ; Skip next byte
0b16 f8 00        ldi 00         ; Load D immediate
0b18 bc           phi rc         ; Put high register RC
0b19 f8 09        ldi 09         ; Load D immediate
0b1b ac           plo rc         ; Put low register RC
0b1c df           sep rf         ; Set P=RF as program counter
0b1d 9c           ghi rc         ; Get high register RC
0b1e ff 08        smi 08         ; Substract D,DF to value
0b20 c3 0b 4a     lbdf 0b4a      ; Long branch on DF=1
0b23 9d           ghi rd         ; Get high register RD
0b24 fa 12        ani 12         ; Logical AND D with value
0b26 fb 12        xri 12         ; Logical XOR D with value
0b28 ca 0b 1c     lbnz 0b1c      ; Long branch on D!=0
0b2b d4           sep r4         ; Set P=R4 as program counter
0b2c 34 40        b1  40         ; Short branch on EF1=1
0b2e 20           dec r0         ; Decrement (R0)
0b2f c3 0b 00     lbdf 0b00      ; Long branch on DF=1
0b32 d4           sep r4         ; Set P=R4 as program counter
0b33 36 13        b3  13         ; Short branch on EF3=1
0b35 f8 f1        ldi f1         ; Load D immediate
0b37 aa           plo ra         ; Put low register RA
0b38 d4           sep r4         ; Set P=R4 as program counter
0b39 34 1c        b1  1c         ; Short branch on EF1=1
0b3b c3 0b 56     lbdf 0b56      ; Long branch on DF=1
0b3e f8 5a        ldi 5a         ; Load D immediate
0b40 aa           plo ra         ; Put low register RA
0b41 0a           ldn ra         ; Load D with (RA)
0b42 2a           dec ra         ; Decrement (RA)
0b43 5a           str ra         ; Store D to (RA)
0b44 d4           sep r4         ; Set P=R4 as program counter
0b45 35 d5        b2  d5         ; Short branch on EF2=1
0b47 c0 0b 1c     lbr 0b1c       ; Long branch
0b4a f8 25        ldi 25         ; Load D immediate
0b4c aa           plo ra         ; Put low register RA
0b4d 0a           ldn ra         ; Load D with (RA)
0b4e fa 80        ani 80         ; Logical AND D with value
0b50 ca 02 c2     lbnz 02c2      ; Long branch on D!=0
0b53 c0 03 66     lbr 0366       ; Long branch
0b56 99           ghi r9         ; Get high register R9
0b57 fa 9f        ani 9f         ; Logical AND D with value
0b59 b9           phi r9         ; Put high register R9
0b5a 89           glo r9         ; Get low register R9
0b5b f9 20        ori 20         ; Logical OR D with value
0b5d a9           plo r9         ; Put low register R9
0b5e f8 00        ldi 00         ; Load D immediate
0b60 bc           phi rc         ; Put high register RC
0b61 ac           plo rc         ; Put low register RC
0b62 d4           sep r4         ; Set P=R4 as program counter
0b63 38 21        skp  21        ; Skip next byte
0b65 d4           sep r4         ; Set P=R4 as program counter
0b66 3d b3        bn2  b3        ; Short branch on EF2=0
0b68 df           sep rf         ; Set P=RF as program counter
0b69 9c           ghi rc         ; Get high register RC
0b6a ff 04        smi 04         ; Substract D,DF to value
0b6c 3b 71        bnf  71        ; Short branch on DF=0
0b6e c0 02 ca     lbr 02ca       ; Long branch
0b71 9d           ghi rd         ; Get high register RD
0b72 fa 12        ani 12         ; Logical AND D with value
0b74 fb 12        xri 12         ; Logical XOR D with value
0b76 3a 68        bnz  68        ; Short branch on D!=0
0b78 9d           ghi rd         ; Get high register RD
0b79 fa 20        ani 20         ; Logical AND D with value
0b7b ca 0f 26     lbnz 0f26      ; Long branch on D!=0
0b7e 9b           ghi rb         ; Get high register RB
0b7f fa f0        ani f0         ; Logical AND D with value
0b81 fb 50        xri 50         ; Logical XOR D with value
0b83 3a 68        bnz  68        ; Short branch on D!=0
0b85 d4           sep r4         ; Set P=R4 as program counter
0b86 13           inc r3         ; Increment (R3)
0b87 30 10        br  10         ; Short branch
0b89 f4           add            ; Add D: D,DF= D+(R(X))
0b8a d4           sep r4         ; Set P=R4 as program counter
0b8b 38 44        skp  44        ; Skip next byte
0b8d df           sep rf         ; Set P=RF as program counter
0b8e 9c           ghi rc         ; Get high register RC
0b8f ff 09        smi 09         ; Substract D,DF to value
0b91 c3 0f 3d     lbdf 0f3d      ; Long branch on DF=1
0b94 9d           ghi rd         ; Get high register RD
0b95 fa 12        ani 12         ; Logical AND D with value
0b97 fb 12        xri 12         ; Logical XOR D with value
0b99 ca 0b 8d     lbnz 0b8d      ; Long branch on D!=0
0b9c 9d           ghi rd         ; Get high register RD
0b9d fa 20        ani 20         ; Logical AND D with value
0b9f ca 0f 26     lbnz 0f26      ; Long branch on D!=0
0ba2 d4           sep r4         ; Set P=R4 as program counter
0ba3 34 35        b1  35         ; Short branch on EF1=1
0ba5 60           irx            ; Increment register X
0ba6 19           inc r9         ; Increment (R9)
0ba7 cb 0b 8d     lbnf 0b8d      ; Long branch on DF=0
0baa 99           ghi r9         ; Get high register R9
0bab f9 40        ori 40         ; Logical OR D with value
0bad b9           phi r9         ; Put high register R9
0bae 89           glo r9         ; Get low register R9
0baf f9 20        ori 20         ; Logical OR D with value
0bb1 a9           plo r9         ; Put low register R9
0bb2 98           ghi r8         ; Get high register R8
0bb3 f9 02        ori 02         ; Logical OR D with value
0bb5 f9 20        ori 20         ; Logical OR D with value
0bb7 b8           phi r8         ; Put high register R8
0bb8 d4           sep r4         ; Set P=R4 as program counter
0bb9 36 66        b3  66         ; Short branch on EF3=1
0bbb f8 79        ldi 79         ; Load D immediate
0bbd a7           plo r7         ; Put low register R7
0bbe f8 04        ldi 04         ; Load D immediate
0bc0 57           str r7         ; Store D to (R7)
0bc1 d4           sep r4         ; Set P=R4 as program counter
0bc2 36 be        b3  be         ; Short branch on EF3=1
0bc4 f8 00        ldi 00         ; Load D immediate
0bc6 bc           phi rc         ; Put high register RC
0bc7 ac           plo rc         ; Put low register RC
0bc8 df           sep rf         ; Set P=RF as program counter
0bc9 9c           ghi rc         ; Get high register RC
0bca ff 08        smi 08         ; Substract D,DF to value
0bcc c3 0b f5     lbdf 0bf5      ; Long branch on DF=1
0bcf 9c           ghi rc         ; Get high register RC
0bd0 ff 04        smi 04         ; Substract D,DF to value
0bd2 cb 0b de     lbnf 0bde      ; Long branch on DF=0
0bd5 f8 c8        ldi c8         ; Load D immediate
0bd7 a7           plo r7         ; Put low register R7
0bd8 07           ldn r7         ; Load D with (R7)
0bd9 fa f7        ani f7         ; Logical AND D with value
0bdb 57           str r7         ; Store D to (R7)
0bdc e7           sex r7         ; Set P=R7 as datapointer
0bdd 64           out 4          ; Output (R(X)); Increment R(X), N=100
0bde 9d           ghi rd         ; Get high register RD
0bdf fa 20        ani 20         ; Logical AND D with value
0be1 c2 0b c8     lbz 0bc8       ; Long branch on D=0
0be4 d4           sep r4         ; Set P=R4 as program counter
0be5 36 f0        b3  f0         ; Short branch on EF3=1
0be7 98           ghi r8         ; Get high register R8
0be8 fa df        ani df         ; Logical AND D with value
0bea b8           phi r8         ; Put high register R8
0beb c0 0f 26     lbr 0f26       ; Long branch
0bee 98           ghi r8         ; Get high register R8
0bef fa df        ani df         ; Logical AND D with value
0bf1 b8           phi r8         ; Put high register R8
0bf2 c0 0f 3d     lbr 0f3d       ; Long branch
0bf5 d4           sep r4         ; Set P=R4 as program counter
0bf6 36 f0        b3  f0         ; Short branch on EF3=1
0bf8 df           sep rf         ; Set P=RF as program counter
0bf9 9c           ghi rc         ; Get high register RC
0bfa ff d6        smi d6         ; Substract D,DF to value
0bfc c3 0b ee     lbdf 0bee      ; Long branch on DF=1
0bff 9d           ghi rd         ; Get high register RD
0c00 fa 12        ani 12         ; Logical AND D with value
0c02 fb 12        xri 12         ; Logical XOR D with value
0c04 3a 14        bnz  14        ; Short branch on D!=0
0c06 d4           sep r4         ; Set P=R4 as program counter
0c07 34 35        b1  35         ; Short branch on EF1=1
0c09 60           irx            ; Increment register X
0c0a 19           inc r9         ; Increment (R9)
0c0b c3 0b c1     lbdf 0bc1      ; Long branch on DF=1
0c0e 9d           ghi rd         ; Get high register RD
0c0f fa 20        ani 20         ; Logical AND D with value
0c11 ca 0b e7     lbnz 0be7      ; Long branch on D!=0
0c14 9e           ghi re         ; Get high register RE
0c15 fa 01        ani 01         ; Logical AND D with value
0c17 ca 0c 23     lbnz 0c23      ; Long branch on D!=0
0c1a d4           sep r4         ; Set P=R4 as program counter
0c1b 3c 59        bn1  59        ; Short branch on EF1=0
0c1d c3 0c 2f     lbdf 0c2f      ; Long branch on DF=1
0c20 c0 0b f8     lbr 0bf8       ; Long branch
0c23 d4           sep r4         ; Set P=R4 as program counter
0c24 3c 59        bn1  59        ; Short branch on EF1=0
0c26 c3 0b f8     lbdf 0bf8      ; Long branch on DF=1
0c29 d4           sep r4         ; Set P=R4 as program counter
0c2a 36 95        b3  95         ; Short branch on EF3=1
0c2c c0 0b f8     lbr 0bf8       ; Long branch
0c2f d4           sep r4         ; Set P=R4 as program counter
0c30 3a 76        bnz  76        ; Short branch on D!=0
0c32 d4           sep r4         ; Set P=R4 as program counter
0c33 13           inc r3         ; Increment (R3)
0c34 30 8e        br  8e         ; Short branch
0c36 f4           add            ; Add D: D,DF= D+(R(X))
0c37 99           ghi r9         ; Get high register R9
0c38 fa bf        ani bf         ; Logical AND D with value
0c3a b9           phi r9         ; Put high register R9
0c3b 89           glo r9         ; Get low register R9
0c3c f9 20        ori 20         ; Logical OR D with value
0c3e a9           plo r9         ; Put low register R9
0c3f 98           ghi r8         ; Get high register R8
0c40 fa fd        ani fd         ; Logical AND D with value
0c42 f9 20        ori 20         ; Logical OR D with value
0c44 b8           phi r8         ; Put high register R8
0c45 d4           sep r4         ; Set P=R4 as program counter
0c46 37 d8        b4  d8         ; Short branch on EF4=1
0c48 d4           sep r4         ; Set P=R4 as program counter
0c49 37 9d        b4  9d         ; Short branch on EF4=1
0c4c 68 ca 83 ff  rldi ra 83ff   ; register load immediate RA
0c4f f8 00        ldi 00         ; Load D immediate
0c51 5a           str ra         ; Store D to (RA)
0c52 d4           sep r4         ; Set P=R4 as program counter
0c53 3a b2        bnz  b2        ; Short branch on D!=0
0c55 d4           sep r4         ; Set P=R4 as program counter
0c56 3d 87        bn2  87        ; Short branch on EF2=0
0c58 f8 ce        ldi ce         ; Load D immediate
0c5a aa           plo ra         ; Put low register RA
0c5b ea           sex ra         ; Set P=RA as datapointer
0c5c 0a           ldn ra         ; Load D with (RA)
0c5d f9 01        ori 01         ; Logical OR D with value
0c5f 5a           str ra         ; Store D to (RA)
0c60 64           out 4          ; Output (R(X)); Increment R(X), N=100
0c61 e2           sex r2         ; Set P=R2 as datapointer
0c62 98           ghi r8         ; Get high register R8
0c63 fa 20        ani 20         ; Logical AND D with value
0c65 3a 6f        bnz  6f        ; Short branch on D!=0
0c67 d4           sep r4         ; Set P=R4 as program counter
0c68 37 1f        b4  1f         ; Short branch on EF4=1
0c6a 02           ldn r2         ; Load D with (R2)
0c6b d4           sep r4         ; Set P=R4 as program counter
0c6c 3a 91        bnz  91        ; Short branch on D!=0
0c6e 03           ldn r3         ; Load D with (R3)
0c6f 98           ghi r8         ; Get high register R8
0c70 f9 20        ori 20         ; Logical OR D with value
0c72 b8           phi r8         ; Put high register R8
0c73 d4           sep r4         ; Set P=R4 as program counter
0c74 3d 7f        bn2  7f        ; Short branch on EF2=0
0c76 f8 31        ldi 31         ; Load D immediate
0c78 a7           plo r7         ; Put low register R7
0c79 07           ldn r7         ; Load D with (R7)
0c7a f9 01        ori 01         ; Logical OR D with value
0c7c 57           str r7         ; Store D to (R7)
0c7d c0 0c ac     lbr 0cac       ; Long branch
0c80 d4           sep r4         ; Set P=R4 as program counter
0c81 38 21        skp  21        ; Skip next byte
0c83 f8 76        ldi 76         ; Load D immediate
0c85 a7           plo r7         ; Put low register R7
0c86 f8 02        ldi 02         ; Load D immediate
0c88 57           str r7         ; Store D to (R7)
0c89 f8 0c        ldi 0c         ; Load D immediate
0c8b ac           plo rc         ; Put low register RC
0c8c f8 00        ldi 00         ; Load D immediate
0c8e bc           phi rc         ; Put high register RC
0c8f 30 b3        br  b3         ; Short branch
0c91 df           sep rf         ; Set P=RF as program counter
0c92 9d           ghi rd         ; Get high register RD
0c93 fa 01        ani 01         ; Logical AND D with value
0c95 3a ac        bnz  ac        ; Short branch on D!=0
0c97 d4           sep r4         ; Set P=R4 as program counter
0c98 3d 87        bn2  87        ; Short branch on EF2=0
0c9a d4           sep r4         ; Set P=R4 as program counter
0c9b 39 dc        bnq  dc        ; Short branch on Q=0
0c9e 68 ca 83 2e  rldi ra 832e   ; register load immediate RA
0ca1 0a           ldn ra         ; Load D with (RA)
0ca2 fa 40        ani 40         ; Logical AND D with value
0ca4 ca 0d ea     lbnz 0dea      ; Long branch on D!=0
0ca7 d4           sep r4         ; Set P=R4 as program counter
0ca8 3d 1d        bn2  1d        ; Short branch on EF2=0
0caa 30 91        br  91         ; Short branch
0cac d4           sep r4         ; Set P=R4 as program counter
0cad 38 8f        skp  8f        ; Skip next byte
0caf 9d           ghi rd         ; Get high register RD
0cb0 fa fd        ani fd         ; Logical AND D with value
0cb2 bd           phi rd         ; Put high register RD
0cb3 df           sep rf         ; Set P=RF as program counter
0cb4 d4           sep r4         ; Set P=R4 as program counter
0cb5 39 dc        bnq  dc        ; Short branch on Q=0
0cb7 d4           sep r4         ; Set P=R4 as program counter
0cb8 38 96        skp  96        ; Skip next byte
0cba 07           ldn r7         ; Load D with (R7)
0cbb fb 03        xri 03         ; Logical XOR D with value
0cbd c2 0c 91     lbz 0c91       ; Long branch on D=0
0cc0 d4           sep r4         ; Set P=R4 as program counter
0cc1 34 72        b1  72         ; Short branch on EF1=1
0cc3 9d           ghi rd         ; Get high register RD
0cc4 fa 02        ani 02         ; Logical AND D with value
0cc6 c2 0c b3     lbz 0cb3       ; Long branch on D=0
0cc9 9d           ghi rd         ; Get high register RD
0cca fa 10        ani 10         ; Logical AND D with value
0ccc c2 0c 80     lbz 0c80       ; Long branch on D=0
0ccf 9d           ghi rd         ; Get high register RD
0cd0 fa 20        ani 20         ; Logical AND D with value
0cd2 ca 0d e4     lbnz 0de4      ; Long branch on D!=0
0cd6 68 ca 83 2e  rldi ra 832e   ; register load immediate RA
0cd9 0a           ldn ra         ; Load D with (RA)
0cda fa 01        ani 01         ; Logical AND D with value
0cdc c2 0d f0     lbz 0df0       ; Long branch on D=0
0cdf 0a           ldn ra         ; Load D with (RA)
0ce0 fa 02        ani 02         ; Logical AND D with value
0ce2 ca 0c 91     lbnz 0c91      ; Long branch on D!=0
0ce5 0a           ldn ra         ; Load D with (RA)
0ce6 f9 02        ori 02         ; Logical OR D with value
0ce8 5a           str ra         ; Store D to (RA)
0ce9 d4           sep r4         ; Set P=R4 as program counter
0cea 13           inc r3         ; Increment (R3)
0ceb 30 00        br  00         ; Short branch
0ced f1           or             ; Logical OR  D with (R(X))
0cee d4           sep r4         ; Set P=R4 as program counter
0cef 13           inc r3         ; Increment (R3)
0cf0 30 82        br  82         ; Short branch
0cf2 04           ldn r4         ; Load D with (R4)
0cf4 68 0d        cid            ; counter interrupt disable
0cf5 f8 00        ldi 00         ; Load D immediate
0cf8 68 06        ldc            ; load counter
0cfa 68 07        stm            ; set timer mode and start
0cfc 68 c7 83 de  rldi r7 83de   ; register load immediate R7
0cff f8 00        ldi 00         ; Load D immediate
0d01 57           str r7         ; Store D to (R7)
0d02 17           inc r7         ; Increment (R7)
0d03 57           str r7         ; Store D to (R7)
0d04 e2           sex r2         ; Set P=R2 as datapointer
0d05 96           ghi r6         ; Get high register R6
0d06 73           stxd           ; Store via X and devrement
0d07 86           glo r6         ; Get low register R6
0d08 73           stxd           ; Store via X and devrement
0d0a 68 ca 83 2c  rldi ra 832c   ; register load immediate RA
0d0d 0a           ldn ra         ; Load D with (RA)
0d0e 32 16        bz  16         ; Short branch on D=0
0d10 fb 0a        xri 0a         ; Logical XOR D with value
0d12 ca 0d 19     lbnz 0d19      ; Long branch on D!=0
0d15 c8 f8 0a     lskp f80a      ; Long skip
0d18 5a           str ra         ; Store D to (RA)
0d19 1a           inc ra         ; Increment (RA)
0d1a 0a           ldn ra         ; Load D with (RA)
0d1b 32 22        bz  22         ; Short branch on D=0
0d1d fb 0a        xri 0a         ; Logical XOR D with value
0d1f 3a 25        bnz  25        ; Short branch on D!=0
0d21 c8 f8 0a     lskp f80a      ; Long skip
0d24 5a           str ra         ; Store D to (RA)
0d26 68 ca 83 ef  rldi ra 83ef   ; register load immediate RA
0d29 e7           sex r7         ; Set P=R7 as datapointer
0d2b 68 c7 83 2c  rldi r7 832c   ; register load immediate R7
0d2e 47           lda r7         ; Load D from (R7), increment R7
0d2f fe           shl            ; Shift left D
0d30 fe           shl            ; Shift left D
0d31 fe           shl            ; Shift left D
0d32 fe           shl            ; Shift left D
0d33 f4           add            ; Add D: D,DF= D+(R(X))
0d34 ea           sex ra         ; Set P=RA as datapointer
0d35 73           stxd           ; Store via X and devrement
0d36 fd f6        sdi f6         ; Substract D,DF from value
0d38 3b a0        bnf  a0        ; Short branch on DF=0
0d3a fb f6        xri f6         ; Logical XOR D with value
0d3c 32 cb        bz  cb         ; Short branch on D=0
0d3e ea           sex ra         ; Set P=RA as datapointer
0d40 68 c7 33 00  rldi r7 3300   ; register load immediate R7
0d43 f8 10        ldi 10         ; Load D immediate
0d45 ae           plo re         ; Put low register RE
0d46 f8 ef        ldi ef         ; Load D immediate
0d48 aa           plo ra         ; Put low register RA
0d49 07           ldn r7         ; Load D with (R7)
0d4a f7           sm             ; Substract memory: DF,D=D-(R(X))
0d4b 2a           dec ra         ; Decrement (RA)
0d4c 5a           str ra         ; Store D to (RA)
0d4d 87           glo r7         ; Get low register R7
0d4e fc 08        adi 08         ; Add D,DF with value
0d50 a7           plo r7         ; Put low register R7
0d51 0a           ldn ra         ; Load D with (RA)
0d52 e7           sex r7         ; Set P=R7 as datapointer
0d53 f7           sm             ; Substract memory: DF,D=D-(R(X))
0d54 3b 60        bnf  60        ; Short branch on DF=0
0d56 87           glo r7         ; Get low register R7
0d57 ff 07        smi 07         ; Substract D,DF to value
0d59 a7           plo r7         ; Put low register R7
0d5a 1e           inc re         ; Increment (RE)
0d5b 1e           inc re         ; Increment (RE)
0d5c 1a           inc ra         ; Increment (RA)
0d5d ea           sex ra         ; Set P=RA as datapointer
0d5e 30 49        br  49         ; Short branch
0d60 8e           glo re         ; Get low register RE
0d61 a7           plo r7         ; Put low register R7
0d62 72           ldxa           ; Load via X and advance
0d63 b6           phi r6         ; Put high register R6
0d64 f0           ldx            ; Pop stack. Place value in D register
0d65 a6           plo r6         ; Put low register R6
0d66 0a           ldn ra         ; Load D with (RA)
0d67 32 7d        bz  7d         ; Short branch on D=0
0d69 ff 01        smi 01         ; Substract D,DF to value
0d6b 5a           str ra         ; Store D to (RA)
0d6c 87           glo r7         ; Get low register R7
0d6d fc 10        adi 10         ; Add D,DF with value
0d6f a7           plo r7         ; Put low register R7
0d70 86           glo r6         ; Get low register R6
0d71 f7           sm             ; Substract memory: DF,D=D-(R(X))
0d72 a6           plo r6         ; Put low register R6
0d73 27           dec r7         ; Decrement (R7)
0d74 96           ghi r6         ; Get high register R6
0d75 77           smb            ; Substract memory with borrow
0d76 b6           phi r6         ; Put high register R6
0d77 87           glo r7         ; Get low register R7
0d78 ff 0f        smi 0f         ; Substract D,DF to value
0d7a a7           plo r7         ; Put low register R7
0d7b 30 66        br  66         ; Short branch
0d7d ea           sex ra         ; Set P=RA as datapointer
0d7f 68 ca 83 ef  rldi ra 83ef   ; register load immediate RA
0d82 86           glo r6         ; Get low register R6
0d83 73           stxd           ; Store via X and devrement
0d84 96           ghi r6         ; Get high register R6
0d85 73           stxd           ; Store via X and devrement
0d86 86           glo r6         ; Get low register R6
0d87 73           stxd           ; Store via X and devrement
0d88 96           ghi r6         ; Get high register R6
0d89 73           stxd           ; Store via X and devrement
0d8a f8 e2        ldi e2         ; Load D immediate
0d8c aa           plo ra         ; Put low register RA
0d8d f8 01        ldi 01         ; Load D immediate
0d8f 73           stxd           ; Store via X and devrement
0d90 f8 00        ldi 00         ; Load D immediate
0d92 73           stxd           ; Store via X and devrement
0d93 f8 f0        ldi f0         ; Load D immediate
0d95 73           stxd           ; Store via X and devrement
0d96 80           glo r0         ; Get low register R0
0d97 f9 80        ori 80         ; Logical OR D with value
0d99 a0           plo r0         ; Put low register R0
0d9a 89           glo r9         ; Get low register R9
0d9b f9 02        ori 02         ; Logical OR D with value
0d9d a9           plo r9         ; Put low register R9
0d9e 30 d5        br  d5         ; Short branch
0da1 68 c7 83 e0  rldi r7 83e0   ; register load immediate R7
0da4 f8 f0        ldi f0         ; Load D immediate
0da6 57           str r7         ; Store D to (R7)
0da7 17           inc r7         ; Increment (R7)
0da8 f8 00        ldi 00         ; Load D immediate
0daa 57           str r7         ; Store D to (R7)
0dab 17           inc r7         ; Increment (R7)
0dac ea           sex ra         ; Set P=RA as datapointer
0dad 1a           inc ra         ; Increment (RA)
0dae f8 fe        ldi fe         ; Load D immediate
0db0 f7           sm             ; Substract memory: DF,D=D-(R(X))
0db1 3b c3        bnf  c3        ; Short branch on DF=0
0db3 57           str r7         ; Store D to (R7)
0db4 f8 f8        ldi f8         ; Load D immediate
0db6 f7           sm             ; Substract memory: DF,D=D-(R(X))
0db7 3b cb        bnf  cb        ; Short branch on DF=0
0db9 32 be        bz  be         ; Short branch on D=0
0dbb f8 20        ldi 20         ; Load D immediate
0dbd c8 f8 10     lskp f810      ; Long skip
0dc0 57           str r7         ; Store D to (R7)
0dc1 30 cb        br  cb         ; Short branch
0dc3 e7           sex r7         ; Set P=R7 as datapointer
0dc4 f8 aa        ldi aa         ; Load D immediate
0dc6 73           stxd           ; Store via X and devrement
0dc7 73           stxd           ; Store via X and devrement
0dc8 f8 fa        ldi fa         ; Load D immediate
0dca 73           stxd           ; Store via X and devrement
0dcb 80           glo r0         ; Get low register R0
0dcc fa 7f        ani 7f         ; Logical AND D with value
0dce a0           plo r0         ; Put low register R0
0dd0 68 00        stpc           ; stop counter
0dd1 89           glo r9         ; Get low register R9
0dd2 f9 02        ori 02         ; Logical OR D with value
0dd4 a9           plo r9         ; Put low register R9
0dd5 e2           sex r2         ; Set P=R2 as datapointer
0dd6 60           irx            ; Increment register X
0dd7 72           ldxa           ; Load via X and advance
0dd8 a6           plo r6         ; Put low register R6
0dd9 f0           ldx            ; Pop stack. Place value in D register
0dda b6           phi r6         ; Put high register R6
0ddb f8 83        ldi 83         ; Load D immediate
0ddd b7           phi r7         ; Put high register R7
0dde c0 0c 91     lbr 0c91       ; Long branch
0de1 c0 0f 19     lbr 0f19       ; Long branch
0de4 d4           sep r4         ; Set P=R4 as program counter
0de5 36 a3        b3  a3         ; Short branch on EF3=1
0de7 c0 0f 26     lbr 0f26       ; Long branch
0dea d4           sep r4         ; Set P=R4 as program counter
0deb 36 a3        b3  a3         ; Short branch on EF3=1
0ded c0 0f 3d     lbr 0f3d       ; Long branch
0df0 d4           sep r4         ; Set P=R4 as program counter
0df1 36 13        b3  13         ; Short branch on EF3=1
0df3 f8 59        ldi 59         ; Load D immediate
0df5 aa           plo ra         ; Put low register RA
0df6 4a           lda ra         ; Load D from (RA), increment RA
0df7 52           str r2         ; Store D to (R2)
0df8 0a           ldn ra         ; Load D with (RA)
0df9 f3           xor            ; Logical exclusive OR  D with (R(X))
0dfa ca 0e 0a     lbnz 0e0a      ; Long branch on D!=0
0dfd d4           sep r4         ; Set P=R4 as program counter
0dfe 13           inc r3         ; Increment (R3)
0dff 33 00        bdf  00        ; Short branch on DF=1
0e01 f1           or             ; Logical OR  D with (R(X))
0e02 d4           sep r4         ; Set P=R4 as program counter
0e03 13           inc r3         ; Increment (R3)
0e04 33 10        bdf  10        ; Short branch on DF=1
0e06 04           ldn r4         ; Load D with (R4)
0e07 c0 0e 45     lbr 0e45       ; Long branch
0e0a f8 f1        ldi f1         ; Load D immediate
0e0c aa           plo ra         ; Put low register RA
0e0d d4           sep r4         ; Set P=R4 as program counter
0e0e 34 1c        b1  1c         ; Short branch on EF1=1
0e10 cb 0e 48     lbnf 0e48      ; Long branch on DF=0
0e13 f8 00        ldi 00         ; Load D immediate
0e15 ac           plo rc         ; Put low register RC
0e16 bc           phi rc         ; Put high register RC
0e17 d4           sep r4         ; Set P=R4 as program counter
0e18 38 21        skp  21        ; Skip next byte
0e1a df           sep rf         ; Set P=RF as program counter
0e1b d4           sep r4         ; Set P=R4 as program counter
0e1c 3c 59        bn1  59        ; Short branch on EF1=0
0e1e cb 0f 19     lbnf 0f19      ; Long branch on DF=0
0e21 9c           ghi rc         ; Get high register RC
0e22 ff 04        smi 04         ; Substract D,DF to value
0e24 c3 0e 48     lbdf 0e48      ; Long branch on DF=1
0e27 9d           ghi rd         ; Get high register RD
0e28 fa 12        ani 12         ; Logical AND D with value
0e2a fb 12        xri 12         ; Logical XOR D with value
0e2c ca 0e 1a     lbnz 0e1a      ; Long branch on D!=0
0e2f 9d           ghi rd         ; Get high register RD
0e30 fa 20        ani 20         ; Logical AND D with value
0e32 ca 0d e4     lbnz 0de4      ; Long branch on D!=0
0e35 d4           sep r4         ; Set P=R4 as program counter
0e36 34 35        b1  35         ; Short branch on EF1=1
0e38 50           str r0         ; Store D to (R0)
0e39 00           idl            ; Idle or wait for interrupt or DMA request
0e3a cb 0e 1a     lbnf 0e1a      ; Long branch on DF=0
0e3d d4           sep r4         ; Set P=R4 as program counter
0e3e 13           inc r3         ; Increment (R3)
0e3f 30 10        br  10         ; Short branch
0e41 f4           add            ; Add D: D,DF= D+(R(X))
0e42 d4           sep r4         ; Set P=R4 as program counter
0e43 3b 0f        bnf  0f        ; Short branch on DF=0
0e45 c0 0c 52     lbr 0c52       ; Long branch
0e48 d4           sep r4         ; Set P=R4 as program counter
0e49 35 bc        b2  bc         ; Short branch on EF2=1
0e4b d4           sep r4         ; Set P=R4 as program counter
0e4c 35 a1        b2  a1         ; Short branch on EF2=1
0e4e 14           inc r4         ; Increment (R4)
0e4f c0 0c 55     lbr 0c55       ; Long branch
0e52 df           sep rf         ; Set P=RF as program counter
0e53 88           glo r8         ; Get low register R8
0e54 ff 38        smi 38         ; Substract D,DF to value
0e56 cb 0e 52     lbnf 0e52      ; Long branch on DF=0
0e59 f8 63        ldi 63         ; Load D immediate
0e5b a7           plo r7         ; Put low register R7
0e5c 07           ldn r7         ; Load D with (R7)
0e5d fb 03        xri 03         ; Logical XOR D with value
0e5f 32 66        bz  66         ; Short branch on D=0
0e61 fb 07        xri 07         ; Logical XOR D with value
0e63 ca 06 e5     lbnz 06e5      ; Long branch on D!=0
0e66 d4           sep r4         ; Set P=R4 as program counter
0e67 13           inc r3         ; Increment (R3)
0e68 30 e0        br  e0         ; Short branch
0e6a f2           and            ; Logical AND: D with (R(X))
0e6b d4           sep r4         ; Set P=R4 as program counter
0e6c 38 44        skp  44        ; Skip next byte
0e6e df           sep rf         ; Set P=RF as program counter
0e6f d4           sep r4         ; Set P=R4 as program counter
0e70 3c 59        bn1  59        ; Short branch on EF1=0
0e72 3b 82        bnf  82        ; Short branch on DF=0
0e74 f8 2e        ldi 2e         ; Load D immediate
0e76 aa           plo ra         ; Put low register RA
0e77 0a           ldn ra         ; Load D with (RA)
0e78 fa 10        ani 10         ; Logical AND D with value
0e7a 3a 8d        bnz  8d        ; Short branch on D!=0
0e7c d4           sep r4         ; Set P=R4 as program counter
0e7d 34 00        b1  00         ; Short branch on EF1=1
0e7f c0 0f 3d     lbr 0f3d       ; Long branch
0e82 f8 2e        ldi 2e         ; Load D immediate
0e84 aa           plo ra         ; Put low register RA
0e85 0a           ldn ra         ; Load D with (RA)
0e86 fa 10        ani 10         ; Logical AND D with value
0e88 32 8d        bz  8d         ; Short branch on D=0
0e8a c0 02 ca     lbr 02ca       ; Long branch
0e8d 9c           ghi rc         ; Get high register RC
0e8e ff 09        smi 09         ; Substract D,DF to value
0e90 3b 9b        bnf  9b        ; Short branch on DF=0
0e92 d4           sep r4         ; Set P=R4 as program counter
0e93 34 00        b1  00         ; Short branch on EF1=1
0e95 d4           sep r4         ; Set P=R4 as program counter
0e96 36 af        b3  af         ; Short branch on EF3=1
0e98 c0 02 ca     lbr 02ca       ; Long branch
0e9b 9d           ghi rd         ; Get high register RD
0e9c fa 12        ani 12         ; Logical AND D with value
0e9e fb 12        xri 12         ; Logical XOR D with value
0ea0 3a 6e        bnz  6e        ; Short branch on D!=0
0ea2 9d           ghi rd         ; Get high register RD
0ea3 fa 20        ani 20         ; Logical AND D with value
0ea5 ca 0e df     lbnz 0edf      ; Long branch on D!=0
0ea8 d4           sep r4         ; Set P=R4 as program counter
0ea9 34 35        b1  35         ; Short branch on EF1=1
0eab 50           str r0         ; Store D to (R0)
0eac 00           idl            ; Idle or wait for interrupt or DMA request
0ead 3b 6e        bnf  6e        ; Short branch on DF=0
0eaf d4           sep r4         ; Set P=R4 as program counter
0eb0 13           inc r3         ; Increment (R3)
0eb1 30 e0        br  e0         ; Short branch
0eb3 f4           add            ; Add D: D,DF= D+(R(X))
0eb4 df           sep rf         ; Set P=RF as program counter
0eb5 d4           sep r4         ; Set P=R4 as program counter
0eb6 3c 59        bn1  59        ; Short branch on EF1=0
0eb8 3b c8        bnf  c8        ; Short branch on DF=0
0eba f8 2e        ldi 2e         ; Load D immediate
0ebc aa           plo ra         ; Put low register RA
0ebd 0a           ldn ra         ; Load D with (RA)
0ebe fa 10        ani 10         ; Logical AND D with value
0ec0 3a d3        bnz  d3        ; Short branch on D!=0
0ec2 d4           sep r4         ; Set P=R4 as program counter
0ec3 34 00        b1  00         ; Short branch on EF1=1
0ec5 c0 0f 3d     lbr 0f3d       ; Long branch
0ec8 f8 2e        ldi 2e         ; Load D immediate
0eca aa           plo ra         ; Put low register RA
0ecb 0a           ldn ra         ; Load D with (RA)
0ecc fa 10        ani 10         ; Logical AND D with value
0ece 32 d3        bz  d3         ; Short branch on D=0
0ed0 c0 0f 3d     lbr 0f3d       ; Long branch
0ed3 9c           ghi rc         ; Get high register RC
0ed4 ff 09        smi 09         ; Substract D,DF to value
0ed6 cb 0e e5     lbnf 0ee5      ; Long branch on DF=0
0ed9 d4           sep r4         ; Set P=R4 as program counter
0eda 34 00        b1  00         ; Short branch on EF1=1
0edc c0 0f 3d     lbr 0f3d       ; Long branch
0edf d4           sep r4         ; Set P=R4 as program counter
0ee0 34 00        b1  00         ; Short branch on EF1=1
0ee2 c0 0f 26     lbr 0f26       ; Long branch
0ee5 90           ghi r0         ; Get high register R0
0ee6 fa 0f        ani 0f         ; Logical AND D with value
0ee8 fb 0f        xri 0f         ; Logical XOR D with value
0eea c2 0e fa     lbz 0efa       ; Long branch on D=0
0eed 90           ghi r0         ; Get high register R0
0eee fa 0f        ani 0f         ; Logical AND D with value
0ef0 fb 01        xri 01         ; Logical XOR D with value
0ef2 ca 0e fa     lbnz 0efa      ; Long branch on D!=0
0ef5 d4           sep r4         ; Set P=R4 as program counter
0ef6 13           inc r3         ; Increment (R3)
0ef7 30 00        br  00         ; Short branch
0ef9 0f           ldn rf         ; Load D with (RF)
0efa 9d           ghi rd         ; Get high register RD
0efb fa 12        ani 12         ; Logical AND D with value
0efd fb 12        xri 12         ; Logical XOR D with value
0eff ca 0e b4     lbnz 0eb4      ; Long branch on D!=0
0f02 9d           ghi rd         ; Get high register RD
0f03 fa 20        ani 20         ; Logical AND D with value
0f05 ca 0e df     lbnz 0edf      ; Long branch on D!=0
0f08 d4           sep r4         ; Set P=R4 as program counter
0f09 34 35        b1  35         ; Short branch on EF1=1
0f0b 60           irx            ; Increment register X
0f0c 13           inc r3         ; Increment (R3)
0f0d cb 0e b4     lbnf 0eb4      ; Long branch on DF=0
0f10 d4           sep r4         ; Set P=R4 as program counter
0f11 34 06        b1  06         ; Short branch on EF1=1
0f13 d4           sep r4         ; Set P=R4 as program counter
0f14 36 af        b3  af         ; Short branch on EF3=1
0f16 c0 0f 3d     lbr 0f3d       ; Long branch
0f1a 68 ca 83 2e  rldi ra 832e   ; register load immediate RA
0f1d 0a           ldn ra         ; Load D with (RA)
0f1e fa bc        ani bc         ; Logical AND D with value
0f20 f9 20        ori 20         ; Logical OR D with value
0f22 5a           str ra         ; Store D to (RA)
0f23 c0 0f 45     lbr 0f45       ; Long branch
0f27 68 ca 83 da  rldi ra 83da   ; register load immediate RA
0f2a 0a           ldn ra         ; Load D with (RA)
0f2b fa 02        ani 02         ; Logical AND D with value
0f2d 32 35        bz  35         ; Short branch on D=0
0f2f 0a           ldn ra         ; Load D with (RA)
0f30 f9 01        ori 01         ; Logical OR D with value
0f32 5a           str ra         ; Store D to (RA)
0f33 30 39        br  39         ; Short branch
0f35 0a           ldn ra         ; Load D with (RA)
0f36 fa fe        ani fe         ; Logical AND D with value
0f38 5a           str ra         ; Store D to (RA)
0f39 89           glo r9         ; Get low register R9
0f3a f9 40        ori 40         ; Logical OR D with value
0f3c a9           plo r9         ; Put low register R9
0f3e 68 ca 83 2e  rldi ra 832e   ; register load immediate RA
0f41 0a           ldn ra         ; Load D with (RA)
0f42 fa 9c        ani 9c         ; Logical AND D with value
0f44 5a           str ra         ; Store D to (RA)
0f45 d4           sep r4         ; Set P=R4 as program counter
0f46 37 a6        b4  a6         ; Short branch on EF4=1
0f48 f8 3a        ldi 3a         ; Load D immediate
0f4a aa           plo ra         ; Put low register RA
0f4b 0a           ldn ra         ; Load D with (RA)
0f4c fa 02        ani 02         ; Logical AND D with value
0f4e 3a 6f        bnz  6f        ; Short branch on D!=0
0f50 9d           ghi rd         ; Get high register RD
0f51 fa 01        ani 01         ; Logical AND D with value
0f53 3a 62        bnz  62        ; Short branch on D!=0
0f55 d4           sep r4         ; Set P=R4 as program counter
0f56 13           inc r3         ; Increment (R3)
0f57 8d           glo rd         ; Get low register RD
0f58 00           idl            ; Idle or wait for interrupt or DMA request
0f59 11           inc r1         ; Increment (R1)
0f5a d4           sep r4         ; Set P=R4 as program counter
0f5b 13           inc r3         ; Increment (R3)
0f5c 8d           glo rd         ; Get low register RD
0f5d 81           glo r1         ; Get low register R1
0f5e 04           ldn r4         ; Load D with (R4)
0f5f c0 0f 8f     lbr 0f8f       ; Long branch
0f62 d4           sep r4         ; Set P=R4 as program counter
0f63 13           inc r3         ; Increment (R3)
0f64 30 00        br  00         ; Short branch
0f66 f1           or             ; Logical OR  D with (R(X))
0f67 d4           sep r4         ; Set P=R4 as program counter
0f68 13           inc r3         ; Increment (R3)
0f69 30 81        br  81         ; Short branch
0f6b 04           ldn r4         ; Load D with (R4)
0f6c c0 0f 8f     lbr 0f8f       ; Long branch
0f6f 90           ghi r0         ; Get high register R0
0f70 fa 0f        ani 0f         ; Logical AND D with value
0f72 fb 0f        xri 0f         ; Logical XOR D with value
0f74 ca 0f 7d     lbnz 0f7d      ; Long branch on D!=0
0f77 90           ghi r0         ; Get high register R0
0f78 fa f0        ani f0         ; Logical AND D with value
0f7a f9 01        ori 01         ; Logical OR D with value
0f7c b0           phi r0         ; Put high register R0
0f7d df           sep rf         ; Set P=RF as program counter
0f7e 90           ghi r0         ; Get high register R0
0f7f fa 0f        ani 0f         ; Logical AND D with value
0f81 fb 01        xri 01         ; Logical XOR D with value
0f83 3a 7d        bnz  7d        ; Short branch on D!=0
0f85 d4           sep r4         ; Set P=R4 as program counter
0f86 13           inc r3         ; Increment (R3)
0f87 8d           glo rd         ; Get low register RD
0f88 00           idl            ; Idle or wait for interrupt or DMA request
0f89 01           ldn r1         ; Load D with (R1)
0f8a d4           sep r4         ; Set P=R4 as program counter
0f8b 13           inc r3         ; Increment (R3)
0f8c 8d           glo rd         ; Get low register RD
0f8d 81           glo r1         ; Get low register R1
0f8e 04           ldn r4         ; Load D with (R4)
0f8f d4           sep r4         ; Set P=R4 as program counter
0f90 38 44        skp  44        ; Skip next byte
0f93 68 ca 83 2e  rldi ra 832e   ; register load immediate RA
0f96 0a           ldn ra         ; Load D with (RA)
0f97 fa 20        ani 20         ; Logical AND D with value
0f99 c2 0f d1     lbz 0fd1       ; Long branch on D=0
0f9c 0a           ldn ra         ; Load D with (RA)
0f9d fb 20        xri 20         ; Logical XOR D with value
0f9f 5a           str ra         ; Store D to (RA)
0fa0 df           sep rf         ; Set P=RF as program counter
0fa1 9c           ghi rc         ; Get high register RC
0fa2 ff 09        smi 09         ; Substract D,DF to value
0fa4 33 d1        bdf  d1        ; Short branch on DF=1
0fa6 9d           ghi rd         ; Get high register RD
0fa7 fa 12        ani 12         ; Logical AND D with value
0fa9 fb 12        xri 12         ; Logical XOR D with value
0fab 3a a0        bnz  a0        ; Short branch on D!=0
0fad d4           sep r4         ; Set P=R4 as program counter
0fae 34 35        b1  35         ; Short branch on EF1=1
0fb0 60           irx            ; Increment register X
0fb1 1d           inc rd         ; Increment (RD)
0fb2 3b be        bnf  be        ; Short branch on DF=0
0fb5 68 c7 83 da  rldi r7 83da   ; register load immediate R7
0fb8 07           ldn r7         ; Load D with (R7)
0fb9 f9 01        ori 01         ; Logical OR D with value
0fbb 57           str r7         ; Store D to (R7)
0fbc 30 cd        br  cd         ; Short branch
0fbe d4           sep r4         ; Set P=R4 as program counter
0fbf 34 35        b1  35         ; Short branch on EF1=1
0fc1 60           irx            ; Increment register X
0fc2 1f           inc rf         ; Increment (RF)
0fc3 3b d1        bnf  d1        ; Short branch on DF=0
0fc6 68 c7 83 da  rldi r7 83da   ; register load immediate R7
0fc9 07           ldn r7         ; Load D with (R7)
0fca fa fe        ani fe         ; Logical AND D with value
0fcc 57           str r7         ; Store D to (R7)
0fcd 89           glo r9         ; Get low register R9
0fce f9 40        ori 40         ; Logical OR D with value
0fd0 a9           plo r9         ; Put low register R9
0fd1 d4           sep r4         ; Set P=R4 as program counter
0fd2 36 af        b3  af         ; Short branch on EF3=1
0fd5 68 ca 83 ff  rldi ra 83ff   ; register load immediate RA
0fd8 f8 00        ldi 00         ; Load D immediate
0fda 5a           str ra         ; Store D to (RA)
0fdb c0 02 ca     lbr 02ca       ; Long branch
0fde 90           ghi r0         ; Get high register R0
0fdf ff 40        smi 40         ; Substract D,DF to value
0fe1 b0           phi r0         ; Put high register R0
0fe2 f8 1b        ldi 1b         ; Load D immediate
0fe4 aa           plo ra         ; Put low register RA
0fe5 0a           ldn ra         ; Load D with (RA)
0fe6 a7           plo r7         ; Put low register R7
0fe7 fc 01        adi 01         ; Add D,DF with value
0fe9 5a           str ra         ; Store D to (RA)
0fea f8 7e        ldi 7e         ; Load D immediate
0fec aa           plo ra         ; Put low register RA
0fed 07           ldn r7         ; Load D with (R7)
0fee 5a           str ra         ; Store D to (RA)
0fef 9b           ghi rb         ; Get high register RB
0ff0 fa f0        ani f0         ; Logical AND D with value
0ff2 f9 09        ori 09         ; Logical OR D with value
0ff4 bb           phi rb         ; Put high register RB
0ff5 88           glo r8         ; Get low register R8
0ff6 ca 10 03     lbnz 1003      ; Long branch on D!=0
0ff9 f8 7d        ldi 7d         ; Load D immediate
0ffb aa           plo ra         ; Put low register RA
0ffc f8 00        ldi 00         ; Load D immediate
0ffe ea           sex ra         ; Set P=RA as datapointer

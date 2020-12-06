char(s,i) { /* i-th char of string s */
   auto n;

   n = *(s+(i>>1));
   /* s[] contains "ABCD", accessed a 'BA' 'DC' */
   if (i&01)
      n=n>>8;
   return (n&0377);
}

lchar(s,i,c) { /* store char c in the i-th pos of string s */
   auto n, x;

   n = *(s + (x = i>>1));
   if (i&1)
      n = (n & 0377) + (c << 8);
   else
      n = (n & 0177400) + c;
   *(s+x) = n;
}

prints(s) {
   auto c,i;

   i = 0;
   while ((c = char(s,i++) != '*e')
      putchar(c);
}

ADDSP R1,R2
   GLO R2 ;IRX ;ADD ;PLO R1
   GHI R2 ;IRX ;ADD ;PHI R2

ADD R1,R2,R3
   GLO R2 ;STR SP
   GLO R3 ;ADD ;PLO R1
   GHI R2 ;STR SP
   GHI R3 ;ADC ;PHI R1

ADI R1,R2,IMM
   GLO R2 ;ADI LOW(IMM) ;PLO R1
   GHI R2 ;ADCI HIGH(IMM) ;PHI R1

SM R1,R2,R3
   GLO R2 ;STR SP
   GLO R3 ;SM ;PLO R1
   GHI R2 ;STR SP
   GHI R3 ;SMB ;PHI R1

SMI R1,R2,IMM
   GLO R2 ;SMI LOW(IMM) ;PLO R1
   GHI R2 ;SMBI HIGH(IMM) ;PHI R1

SMSP R1,R2
   GLO R2 ;IRX ;SM ;PLO R1
   GHI R2 ;IRX ;SMBI ;PHI R1

LDI R1,IMM
   LDI LOW(IMM) ;PLO R1
   LDI HIGH(IMM) ;PHI R1

SHR R1,R2
   GHI R2 ;SHR ;PHI R1
   GLO R2 ;SHRC ;PLO R2

SHL R1,R2
   GLO R2 ;SHL ;PLO R1
   GHI R2 ;SHLC ;PHI R1

LDN R1,[R2]
   LDA R2 ;PLO R1
   LDN R2 ;PHI R2; DEC R2

LDA R1,[R2]
   LDA R2 ;PLO R1
   LDA R2 ;PHI R1

STR [R1],R2
   GLO R2 ;STR R1 ;INC R1
   GHI R2 ;STR R1 ;DEC R1

STA [R1],R2
   GLO R2 ;STR R1 ;INC R1
   GHI R2 ;STR R1 ;INC R1

PUSH R1
   GHI R1 ;STXD
   GLO R1 ;STXD

POP R1
   IRX ;LDXA ;PLO R1
   LDX ;PHI R1

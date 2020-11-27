 PTER=#0C
 LOADER=#843F
        ORG#8000
L1:     LDI#00,#55
        ,#0D0A00
        SEP R4,A(WAIT),T' ALPHA? ',#00
        LDI A.0(LOADER); PLO R7
        SEP R4,A(LOADER),L'ALPHA',T'ALPHA',#00
        LDI LOADER.1 ;LDI LOADER.0
        SEP RF

        ORG#8234
WAIT:   NOP
        SEP RF
        END

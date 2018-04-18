        ORG 0
        MOVE    1, R0
        SHL     R0, 2, R0       ; assert R0 == 4
        MOVE    9, R1
        SHL     R1, %D 30, R1   ; assert R1 == 0x4000000
        MOVE    %D 200, R2
        SHR     R2, 3, R2       ; assert R2 == 25
        MOVE    -1, R3
        SHR     R3, %D 20, R3   ; assert R3 == 0xFFF
        MOVE    -020, R4
        ASHR    R4, 4, R4       ; assert R4 == -2
        MOVE    -1, R5
        ASHR    R5, %D 40, R5   ; assert R5 == -1
        MOVE    -1, R6
        SHR     R6, %D 40, R6   ; assert R6 == 0
        HALT

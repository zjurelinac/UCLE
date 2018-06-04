        ORG 0
        MOVE    MSTART, SP

        MOVE    1, R0
        STORE   R0, (M1)
        LOAD    R0, (M1)        ; assert R0 == 1

        LOAD    R1, (NUM1)
        STORE   R1, (SP+4)
        LOAD    R1, (SP+4)      ; assert R1 == 0xABCDDCBA

        MOVE    2, R2
        STOREH  R2, (M3)
        LOADH   R2, (M3)        ; assert R2 == 2

        LOAD    R3, (NUM1)
        STOREH  R3, (SP+0C)
        LOADH   R3, (SP+0C)     ; assert R3 == 0xDCBA

        MOVE    3, R4
        STOREB  R4, (M5)
        LOADB   R4, (M5)        ; assert R4 == 3

        LOAD    R5, (NUM1)
        STOREB  R5, (SP+14)
        LOADB   R5, (SP+14)     ; assert R5 == 0xBA

        MOVE    0FF, R6
        STOREB  R6, (NUM1)
        LOAD    R6, (NUM1)      ; assert R6 == 0xABCDCFF
        HALT

NUM1    DW 0ABCDDCBA

MSTART
M1      DW 0
M2      DW 0
M3      DH 0
M4      DH 0
M5      DB 0
M6      DB 0

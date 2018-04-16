        ORG 0
        MOVE    MSTART, SP
        LOAD    R0, (M1)        ; assert R0 == 0x11111111
        LOAD    R1, (SP+4)      ; assert R1 == 0x22222222
        LOADH   R2, (M3)        ; assert R2 == 0x3333
        LOADH   R3, (SP+0C)     ; assert R3 == 0x4444
        LOADB   R4, (M5)        ; assert R4 == 0x55
        LOADB   R5, (SP + 14)   ; assert R5 == 0x66
        LOADB   R6, (SP)        ; assert R6 == 0x11
        HALT

MSTART
M1      DW 11111111
M2      DW 22222222
M3      DH 3333
M4      DH 4444
M5      DB 55
M6      DB 66

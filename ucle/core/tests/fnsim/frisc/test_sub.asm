        ORG 0
        MOVE    0, R0
        SUB     R0, 0, R0       ; assert R0 == 0
        MOVE    1, R1
        SUB     R1, 1, R1       ; assert R1 == 0
        MOVE    2, R2
        SUB     R2, 1, R2       ; assert R2 == 1
        MOVE    3, R3
        SUB     R3, R2, R3      ; assert R3 == 2
        MOVE    4, R4
        SUB     R4, -6, R4      ; assert R4 == 10
        MOVE    5, R5
        SUB     R5, 7, R5       ; assert R5 == -2
        MOVE    6, R6
        SUB     R6, R4, R6      ; assert R6 == -4
        HALT

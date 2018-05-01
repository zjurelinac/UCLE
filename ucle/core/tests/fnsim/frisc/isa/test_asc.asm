        ORG 0
        MOVE    0, R0
        ADC     R0, 1, R0       ; assert R0 == 1
        MOVE    1, R1
        MOVE    2, SR
        ADC     R1, 1, R1       ; assert R1 == 3
        MOVE    -1, R2
        MOVE    2, SR
        ADC     R2, 0, R2       ; assert R2 == 0
        MOVE    SR, R3
        ADC     R3, 0, R3       ; assert R3 == 11
        MOVE    4, R4
        MOVE    0, SR
        SBC     R4, 1, R4       ; assert R4 == 3
        MOVE    5, R5
        MOVE    2, SR
        SBC     R5, 1, R5       ; assert R5 == 3
        MOVE    1, R6
        MOVE    2, SR
        SBC     R6, 1, R6       ; assert R6 == -1
        HALT

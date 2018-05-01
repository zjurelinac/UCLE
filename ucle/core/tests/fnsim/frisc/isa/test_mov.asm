        ORG 0

        MOVE    7, SR

        JP_NC   CONT_0
        MOVE    1, R0
CONT_0                          ; assert R0 == 1
        JP_NV   CONT_1
        MOVE    1, R1
CONT_1                          ; assert R1 == 1
        JP_NN   CONT_2
        MOVE    1, R2
CONT_2                          ; assert R2 == 1
        JP_NZ   CONT_3
        MOVE    1, R3           ; assert R3 == 0
CONT_3

        MOVE    SR, R4
        XOR     R4, 4, R4
        MOVE    R4, SR

        JP_V    CONT_4
        MOVE    0, R4
CONT_4                          ; assert R4 == 0

        MOVE    SR, R5
        XOR     R5, 2, R5
        MOVE    R5, SR

        JP_C    CONT_5
        MOVE    0, R5
CONT_5                          ; assert R5 == 0

        MOVE    SR, R6          ; assert R6 == 1

        HALT

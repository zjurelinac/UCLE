        ORG     0
        MOVE    0, R0
        ADD     R0, 1, R0       ; assert R0 == 1
        MOVE    1, R1
        ADD     R1, 1, R1       ; assert R1 == 2
        MOVE    2, R2
        ADD     R2, R1, R2      ; assert R2 == 4
        MOVE    3, R3
        ADD     R3, -1, R3      ; assert R3 == 2
        MOVE    4, R4
        ADD     R4, -4, R4      ; assert R4 == 0
        MOVE    5, R5
        ADD     R5, -6, R5      ; assert R5 == -1
        MOVE    6, R6
        ADD     R6, R6, R6      ; assert R6 == 12
        HALT

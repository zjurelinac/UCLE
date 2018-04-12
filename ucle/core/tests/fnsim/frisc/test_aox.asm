        ORG 0
        MOVE    0E, R0
        AND     R0, 5, R0       ; assert R0 == 4
        MOVE    0ABCD, R1
        AND     R1, 0F0F0, R1   ; assert R1 == 0xA0C0
        MOVE    2, R2
        OR      R2, 0C, R2      ; assert R2 == 0E
        MOVE    0A0CD, R3
        OR      R3, 0B00, R3    ; assert R3 == 0xABCD
        MOVE    4, R4
        XOR     R4, 7, R4       ; assert R4 == 3
        MOVE    0AAAA, R5
        XOR     R5, 0FFFF, R5   ; assert R5 == 0x5555
        MOVE    -2, R6
        XOR     R6, -1, R6      ; assert R6 == 1
        HALT

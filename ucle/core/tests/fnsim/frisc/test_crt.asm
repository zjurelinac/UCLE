        ORG 0
        MOVE    200, SP

        MOVE    5, R0
        CALL    FN1         ; assert R0 == 10

        MOVE    0ABCD, R1
        CALL    FN2         ; assert R1 == 0xCDAB

        MOVE    7, R2
        PUSH    R2
        MOVE    5, R2
        PUSH    R2
        CALL    FN3
        POP     R2          ; assert R2 == 2
        ADD     SP, 4, SP

        MOVE    5, R3
        CMP     R3, 3
        CALL_NE FN4_T
        CALL_EQ FN4_F       ; assert R3 == 1

        HALT

;==============================================================

FN1     ADD     R0, R0, R0
        RET

FN2     PUSH    R0
        AND     R1, 0FF, R0
        SHR     R1, 8, R1
        SHL     R0, 8, R0
        ADD     R1, R0, R1
        POP     R0
        RET

FN3     PUSH    R0
        PUSH    R1

        LOAD    R0, (SP+10)
        LOAD    R1, (SP+0C)
        SUB     R0, R1, R0
        STORE   R0, (SP+0C)

        POP     R1
        POP     R0
        RET

FN4_T   MOVE 1, R3
        RET

FN4_F   MOVE 0, R3
        RET

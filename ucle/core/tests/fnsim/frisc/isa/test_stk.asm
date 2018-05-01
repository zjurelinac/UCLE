        ORG 0
        MOVE STACK_END, SP

        MOVE 7, R0
        PUSH R0
        LOAD R0, (SP)           ; assert R0 == 7

        MOVE 0ABCD, R1
        PUSH R1
        LOAD R1, (SP)           ; assert R1 == 0xABCD

        POP R2                  ; assert R2 == 0xABCD
        POP R3                  ; assert R3 == 7

        LOAD R4, (NUM)
        PUSH R4
        SHR R4, 8, R4
        PUSH R4
        SHR R4, 8, R4
        PUSH R4
        SHR R4, 8, R4
        PUSH R4
        SHR R4, 8, R4

        POP R4                  ; assert R4 == 0x11
        POP R5                  ; assert R5 == 0x1122
        POP R6                  ; assert R6 == 0x112233

                                ; assert SP == 0x3FC

        HALT

NUM     DW 11223344

        ORG 200
STACK   DS 200
STACK_END

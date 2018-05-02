CT_CR       EQU 10000
CT_LR       EQU 10004
CT_DC       EQU 10004
CT_BS       EQU 10008
CT_IACK     EQU 1000C

            ORG 0
            MOVE 1000, SP
            JP MAIN
INT_VEC     DW 100

NMI_HNDLR   PUSH R0
            MOVE SR, R0
            PUSH R0

            ; Do stuff

            POP R0
            MOVE R0, SR
            POP R0
            RETN

            ORG 100
INT_HNDLR   ; PUSH R0
            ; MOVE SR, R0
            ; PUSH R0

            STORE R0, (CT_BS)
            MOVE 0ABCD, R6

            ; POP R0
            ; MOVE R0, SR
            ; POP R0
            RETI

MAIN        MOVE %B 10000, R0
            MOVE R0, SR         ; GIE = 1

            MOVE 5, R0
            STORE R0, (CT_LR)

            MOVE 3, R0          ; start = 1, do_int = 1
            STORE R0, (CT_CR)

            ADD R0, 0, R0
            LOAD R1, (CT_DC)
            LOAD R2, (CT_DC)
            LOAD R3, (CT_DC)
            LOAD R4, (CT_DC)
            HALT

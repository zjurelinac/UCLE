CT_CR       EQU 10000
CT_LR       EQU 10004
CT_BS       EQU 10008
CT_IACK     EQU 1000C

            ORG 0
            MOVE 1000, SP
            JP MAIN
INT_VEC     DW 200

NMI_HNDLR   PUSH R0
            MOVE SR, R0
            PUSH R0

            ; Do stuff

            POP R0
            MOVE R0, SR
            POP R0
            RETN

            ORG 100
INT_HNDLR   RETI

MAIN        MOVE 1, R0
            STORE R0, (CT_CR)

            ADD R0, 0, R0
            HALT

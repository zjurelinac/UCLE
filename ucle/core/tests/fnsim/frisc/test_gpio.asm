GPIO1_CR    EQU     10000
GPIO1_DR    EQU     10004
GPIO1_BS    EQU     10008
GPIO1_IACK  EQU     1000C

GPIO2_CR    EQU     20000
GPIO2_DR    EQU     20004
GPIO2_BS    EQU     20008
GPIO2_IACK  EQU     2000C

            ORG     0
            MOVE    1000, SP
            JP      MAIN
INT_VEC     DW      100

            ORG     0C
NMI_HNDLR   PUSH    R0
            MOVE    SR, R0
            PUSH    R0

            ; Do stuff

            POP     R0
            MOVE    R0, SR
            POP     R0
            RETN

            ORG     100
INT_HNDLR   PUSH    R0
            MOVE    SR, R0
            PUSH    R0

            ; Do stuff

            POP     R0
            MOVE    R0, SR
            POP     R0
            RETI

MAIN        MOVE    %B 10000, R0
            MOVE    R0, SR          ; GIE = 1

            MOVE    %B 10, R0
            STORE   R0, (GPIO1_CR)

            MOVE    0FF, R1
LOOP        STORE   R1, (GPIO1_DR)
            SUB     R1, 1, R1
            JP_NZ   LOOP

            HALT

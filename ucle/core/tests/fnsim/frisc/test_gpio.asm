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
INT_HNDLR   ; PUSH    R0
            ; MOVE    SR, R0
            ; PUSH    R0

            ; Do stuff
            STORE   R0, (GPIO2_BS)
            MOVE    1, R6
            STORE   R0, (GPIO2_IACK)

            ; POP     R0
            ; MOVE    R0, SR
            ; POP     R0
            RETI

MAIN        MOVE    %B 10000, R0
            MOVE    R0, SR          ; GIE = 1

;             ;  LED test program
;             MOVE    %B 10, R0
;             STORE   R0, (GPIO1_CR)

;             MOVE    0FF, R1
; LOOP1       STORE   R1, (GPIO1_DR)
;             SUB     R1, 1, R1
;             JP_NZ   LOOP1

            ; SWITCH test program
            LOAD    R0, (SW_CR)
            STORE   R0, (GPIO2_CR)

LOOP2       ADD     R2, 1, R2
            CMP     R6, 1
            JP_NZ   LOOP2

            HALT

            ;     --------aaaaaaaammmmmmmm---opiMM
SW_CR       DW %B 00000000111111110000111100010111

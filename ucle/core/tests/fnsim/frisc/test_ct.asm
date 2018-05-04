CT1_CR      EQU     10000
CT1_LR      EQU     10004
CT1_DC      EQU     10004
CT1_BS      EQU     10008
CT1_IACK    EQU     1000C

CT2_CR      EQU     20000
CT2_LR      EQU     20004
CT2_DC      EQU     20004
CT2_BS      EQU     20008
CT2_IACK    EQU     2000C

            ORG     0
            MOVE    1000, SP
            JP      MAIN
INT_VEC     DW      100

            ORG     0C
; NMI_HNDLR   PUSH R0
;             MOVE SR, R0
;             PUSH R0
;
            ; Do stuff
            STORE   R0, (CT2_BS)
            MOVE    1, R6
            STORE   R0, (CT2_IACK)
;
;             POP R0
;             MOVE R0, SR
;             POP R0
            RETN

            ORG     100
INT_HNDLR   ; PUSH R0
            MOVE SR, R0
            PUSH R0

            STORE   R0, (CT2_BS)
            ADD     R2, 1, R2
            MOVE    1, R6

            POP R0
            MOVE R0, SR
            ; POP R0
            RETI

MAIN        MOVE    %B 10000, R0
            MOVE    R0, SR          ; GIE = 1

            MOVE    %D 1000, R0
            STORE   R0, (CT1_LR)    ; Count down from 1000

            MOVE    1, R0           ; START = 1, DO_INT = 0, NMI = 0
            STORE   R0, (CT1_CR)    ; Start CT1

            MOVE    %D 1000, R0     ; Cound down from 1000
            STORE   R0, (CT2_LR)

            MOVE    3, R0           ; START = 1, DO_INT = 1, NMI = 0
            STORE   R0, (CT2_CR)    ; Start CT2

LOOP        ADD     R1, 1, R1       ; Count cycles till interrupt, cnt = R1 * 3
            CMP     R6, 1           ; On CT interrupt, R6 will become 1, then stop
            JP_NZ   LOOP
            HALT

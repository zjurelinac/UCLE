CT_CR       EQU 10000
CT_LR       EQU 10004
CT_DC       EQU 10004
CT_BS       EQU 10008
CT_IACK     EQU 1000C

            ORG 0
            MOVE 1000, SP
            JP MAIN
INT_VEC     DW 100

            ORG 0C
; NMI_HNDLR   PUSH R0
;             MOVE SR, R0
;             PUSH R0
;
            ; Do stuff
            STORE R0, (CT_BS)
            MOVE 1, R3
            MOVE 0DCBA, R5

;
;             POP R0
;             MOVE R0, SR
;             POP R0
            RETN

            ORG 100
INT_HNDLR   ; PUSH R0
            ; MOVE SR, R0
            ; PUSH R0

            STORE R0, (CT_BS)
            MOVE 1, R3

            ; POP R0
            ; MOVE R0, SR
            ; POP R0
            RETI

MAIN        MOVE %B 10000, R0
            MOVE R0, SR         ; GIE = 1

            MOVE %D 3000, R0
            STORE R0, (CT_LR)   ; Count down from 3000

            MOVE 7, R0          ; start = 1, do_int = 1, nmi = 1
            STORE R0, (CT_CR)   ; Start CT

            MOVE 0, R2
LOOP        ADD R2, 1, R2       ; Count cycles till interrupt, cnt = R2 * 3
            CMP R3, 1           ; On CT interrupt, R3 will become 1, then stop
            JP_NZ LOOP
            LOAD R4, (CT_DC)    ; Store current CT value to R4
            HALT

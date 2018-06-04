GPIO_DR EQU 0FFF10000

        ORG 0
        ; ADD R2, GPIO_DR, R2
        ; XOR R3, R4, R5
        ; CMP R0, 7
        ; MOVE R2, R3
        ; MOVE 0FFFC, SP
        ; MOVE SR, R1
        ; MOVE R1, SR
        ; LOAD R2, (LABEL1)
        ; LOADB R3, (R1)
        ; STORE R4, (SP-8)
        ; PUSH R1
        ; POP SP
        JP MAIN
        JP_ULE MAIN
        JR_SGT MAIN
        JP (R2)

        DW 0ABCDEFFE
LABEL1  DB 5, 6, 7, 8, 9, %D 10, %D 11, %D 12, 0D, 0E, 0F

        ; ORG %D 100
MAIN
        HALT

        DS %D 40
        DW 5, 6, 7
        DB 1, 2, 3
        DH 7, 8, 9, 10, 11, 12
        DW 1
        ; stuff

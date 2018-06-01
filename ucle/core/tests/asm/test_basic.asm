GPIO_DR EQU 0FFFFF100

        ORG 0
        MOVE 1000, SP
        JP MAIN

        ORG %D 100
MAIN
        HALT

        DS %D 40
        DW 5, 6, 7
        DB 1, 2, 3
        DH 7, 8, 9, 10, 11, 12
        DW 1
        ; stuff

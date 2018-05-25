DMA_SRC     EQU     10000
DMA_DST     EQU     10004
DMA_CNT     EQU     10008
DMA_CR      EQU     1000C
DMA_START   EQU     10010
DMA_BS      EQU     10014

            ORG     0
            MOVE    1000, SP
            JP      MAIN

            ORG     8
INT_VEC     DW      100

            ORG     0C
NMI_HNDLR   RETN

            ORG     100
INT_HNDLR   STORE   R0, (DMA_BS)
            ADD     R6, 1, R6
            RETI

MAIN        MOVE    %B 10000, R0
            MOVE    R0, SR          ; GIE = 1

            MOVE    DATA_SRC, R0
            STORE   R0, (DMA_SRC)

            MOVE    DATA_DST, R0
            STORE   R0, (DMA_DST)

            MOVE    %D 10, R0
            STORE   R0, (DMA_CNT)

            MOVE    %B 0011, R0
            STORE   R0, (DMA_CR)

            STORE   R0, (DMA_START)
            ; LOAD    R4, (DATA_DST)
            ; HALT

LOOP        ADD     R1, 1, R1
            CMP     R6, 1
            JP_NZ   LOOP

            HALT


            ORG 400
DATA_SRC    DW 1
            DW 2
            DW 3
            DW 4
            DW 5
            DW 6
            DW 7
            DW 8
            DW 9
            DW %D 10


            ORG 600
DATA_DST    DS 40

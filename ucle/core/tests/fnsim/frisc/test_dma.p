                       DMA_SRC     EQU     10000
                       DMA_DST     EQU     10004
                       DMA_CNT     EQU     10008
                       DMA_CR      EQU     1000C
                       DMA_START   EQU     10010
                       DMA_BS      EQU     10014
                       
                                   ORG     0
00000000  00 10 80 07              MOVE    1000, SP
00000004  0C 01 00 C4              JP      MAIN
                       
                                   ORG     8
00000008  00 01 00 00  INT_VEC     DW      100
                       
                                   ORG     0C
0000000C  03 00 00 D8  NMI_HNDLR   RETN
                       
                                   ORG     100
00000100  14 00 01 B8  INT_HNDLR   STORE   R0, (DMA_BS)
00000104  01 00 60 27              ADD     R6, 1, R6
00000108  01 00 00 D8              RETI
                       
0000010C  10 00 00 04  MAIN        MOVE    %B 10000, R0
00000110  00 00 10 00              MOVE    R0, SR          ; GIE = 1
                       
00000114  00 04 00 04              MOVE    DATA_SRC, R0
00000118  00 00 01 B8              STORE   R0, (DMA_SRC)
                       
0000011C  00 06 00 04              MOVE    DATA_DST, R0
00000120  04 00 01 B8              STORE   R0, (DMA_DST)
                       
00000124  0A 00 00 04              MOVE    %D 10, R0
00000128  08 00 01 B8              STORE   R0, (DMA_CNT)
                       
0000012C  03 00 00 04              MOVE    %B 0011, R0
00000130  0C 00 01 B8              STORE   R0, (DMA_CR)
                       
00000134  10 00 01 B8              STORE   R0, (DMA_START)
                                   ; LOAD    R4, (DATA_DST)
                                   ; HALT
                       
00000138  01 00 90 24  LOOP        ADD     R1, 1, R1
0000013C  01 00 60 6C              CMP     R6, 1
00000140  38 01 00 C6              JP_NZ   LOOP
                       
00000144  00 00 00 F8              HALT
                       
                       
                                   ORG 400
00000400  01 00 00 00  DATA_SRC    DW 1
00000404  02 00 00 00              DW 2
00000408  03 00 00 00              DW 3
0000040C  04 00 00 00              DW 4
00000410  05 00 00 00              DW 5
00000414  06 00 00 00              DW 6
00000418  07 00 00 00              DW 7
0000041C  08 00 00 00              DW 8
00000420  09 00 00 00              DW 9
00000424  0A 00 00 00              DW %D 10
                       
                       
                                   ORG 600
                       DATA_DST    DS 40

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
00000000  00 10 80 07              MOVE    1000, SP
00000004  20 01 00 C4              JP      MAIN
00000008  00 01 00 00  INT_VEC     DW      100
                       
                                   ORG     0C
                       ; NMI_HNDLR   PUSH R0
                       ;             MOVE SR, R0
                       ;             PUSH R0
                       ;
                                   ; Do stuff
0000000C  08 00 02 B8              STORE   R0, (CT2_BS)
00000010  01 00 00 07              MOVE    1, R6
00000014  0C 00 02 B8              STORE   R0, (CT2_IACK)
                       ;
                       ;             POP R0
                       ;             MOVE R0, SR
                       ;             POP R0
00000018  03 00 00 D8              RETN
                       
                                   ORG     100
                       INT_HNDLR   ; PUSH R0
00000100  00 00 20 00              MOVE SR, R0
00000104  00 00 00 88              PUSH R0
                       
00000108  08 00 02 B8              STORE   R0, (CT2_BS)
0000010C  01 00 20 25              ADD     R2, 1, R2
00000110  01 00 00 07              MOVE    1, R6
                       
00000114  00 00 00 80              POP R0
00000118  00 00 10 00              MOVE R0, SR
                                   ; POP R0
0000011C  01 00 00 D8              RETI
                       
00000120  10 00 00 04  MAIN        MOVE    %B 10000, R0
00000124  00 00 10 00              MOVE    R0, SR          ; GIE = 1
                       
00000128  E8 03 00 04              MOVE    %D 1000, R0
0000012C  04 00 01 B8              STORE   R0, (CT1_LR)    ; Count down from 1000
                       
00000130  01 00 00 04              MOVE    1, R0           ; START = 1, DO_INT = 0, NMI = 0
00000134  00 00 01 B8              STORE   R0, (CT1_CR)    ; Start CT1
                       
00000138  E8 03 00 04              MOVE    %D 1000, R0     ; Cound down from 1000
0000013C  04 00 02 B8              STORE   R0, (CT2_LR)
                       
00000140  03 00 00 04              MOVE    3, R0           ; START = 1, DO_INT = 1, NMI = 0
00000144  00 00 02 B8              STORE   R0, (CT2_CR)    ; Start CT2
                       
00000148  01 00 90 24  LOOP        ADD     R1, 1, R1       ; Count cycles till interrupt, cnt = R1 * 3
0000014C  01 00 60 6C              CMP     R6, 1           ; On CT interrupt, R6 will become 1, then stop
00000150  48 01 00 C6              JP_NZ   LOOP
00000154  00 00 00 F8              HALT

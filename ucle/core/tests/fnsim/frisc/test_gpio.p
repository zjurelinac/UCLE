                       GPIO1_CR    EQU     10000
                       GPIO1_DR    EQU     10004
                       GPIO1_BS    EQU     10008
                       GPIO1_IACK  EQU     1000C
                       
                       GPIO2_CR    EQU     20000
                       GPIO2_DR    EQU     20004
                       GPIO2_BS    EQU     20008
                       GPIO2_IACK  EQU     2000C
                       
                                   ORG     0
00000000  00 10 80 07              MOVE    1000, SP
00000004  1C 01 00 C4              JP      MAIN
00000008  00 01 00 00  INT_VEC     DW      100
                       
                                   ORG     0C
0000000C  00 00 00 88  NMI_HNDLR   PUSH    R0
00000010  00 00 20 00              MOVE    SR, R0
00000014  00 00 00 88              PUSH    R0
                       
                                   ; Do stuff
                       
00000018  00 00 00 80              POP     R0
0000001C  00 00 10 00              MOVE    R0, SR
00000020  00 00 00 80              POP     R0
00000024  03 00 00 D8              RETN
                       
                                   ORG     100
00000100  00 00 00 88  INT_HNDLR   PUSH    R0
00000104  00 00 20 00              MOVE    SR, R0
00000108  00 00 00 88              PUSH    R0
                       
                                   ; Do stuff
                       
0000010C  00 00 00 80              POP     R0
00000110  00 00 10 00              MOVE    R0, SR
00000114  00 00 00 80              POP     R0
00000118  01 00 00 D8              RETI
                       
0000011C  10 00 00 04  MAIN        MOVE    %B 10000, R0
00000120  00 00 10 00              MOVE    R0, SR          ; GIE = 1
                       
00000124  02 00 00 04              MOVE    %B 10, R0
00000128  00 00 01 B8              STORE   R0, (GPIO1_CR)
                       
0000012C  FF 00 80 04              MOVE    0FF, R1
00000130  04 00 81 B8  LOOP        STORE   R1, (GPIO1_DR)
00000134  01 00 90 34              SUB     R1, 1, R1
00000138  30 01 00 C6              JP_NZ   LOOP
                       
0000013C  00 00 00 F8              HALT

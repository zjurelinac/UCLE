                       CT_CR       EQU 10000
                       CT_LR       EQU 10004
                       CT_BS       EQU 10008
                       CT_IACK     EQU 1000C
                       
                                   ORG 0
00000000  00 10 80 07              MOVE 1000, SP
00000004  04 01 00 C4              JP MAIN
00000008  00 02 00 00  INT_VEC     DW 200
                       
0000000C  00 00 00 88  NMI_HNDLR   PUSH R0
00000010  00 00 20 00              MOVE SR, R0
00000014  00 00 00 88              PUSH R0
                       
                                   ; Do stuff
                       
00000018  00 00 00 80              POP R0
0000001C  00 00 10 00              MOVE R0, SR
00000020  00 00 00 80              POP R0
00000024  03 00 00 D8              RETN
                       
                                   ORG 100
00000100  01 00 00 D8  INT_HNDLR   RETI
                       
00000104  01 00 00 04  MAIN        MOVE 1, R0
00000108  00 00 01 B8              STORE R0, (CT_CR)
                       
0000010C  00 00 00 24              ADD R0, 0, R0
00000110  00 00 00 F8              HALT

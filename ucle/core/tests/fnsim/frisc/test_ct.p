                       CT_CR       EQU 10000
                       CT_LR       EQU 10004
                       CT_DC       EQU 10004
                       CT_BS       EQU 10008
                       CT_IACK     EQU 1000C
                       
                                   ORG 0
00000000  00 10 80 07              MOVE 1000, SP
00000004  0C 01 00 C4              JP MAIN
00000008  00 01 00 00  INT_VEC     DW 100
                       
0000000C  00 00 00 88  NMI_HNDLR   PUSH R0
00000010  00 00 20 00              MOVE SR, R0
00000014  00 00 00 88              PUSH R0
                       
                                   ; Do stuff
                       
00000018  00 00 00 80              POP R0
0000001C  00 00 10 00              MOVE R0, SR
00000020  00 00 00 80              POP R0
00000024  03 00 00 D8              RETN
                       
                                   ORG 100
                       INT_HNDLR   ; PUSH R0
                                   ; MOVE SR, R0
                                   ; PUSH R0
                       
00000100  08 00 01 B8              STORE R0, (CT_BS)
00000104  CD AB 00 07              MOVE 0ABCD, R6
                       
                                   ; POP R0
                                   ; MOVE R0, SR
                                   ; POP R0
00000108  01 00 00 D8              RETI
                       
0000010C  10 00 00 04  MAIN        MOVE %B 10000, R0
00000110  00 00 10 00              MOVE R0, SR         ; GIE = 1
                       
00000114  05 00 00 04              MOVE 5, R0
00000118  04 00 01 B8              STORE R0, (CT_LR)
                       
0000011C  03 00 00 04              MOVE 3, R0          ; start = 1, do_int = 1
00000120  00 00 01 B8              STORE R0, (CT_CR)
                       
00000124  00 00 00 24              ADD R0, 0, R0
00000128  04 00 81 B0              LOAD R1, (CT_DC)
0000012C  04 00 01 B1              LOAD R2, (CT_DC)
00000130  04 00 81 B1              LOAD R3, (CT_DC)
00000134  04 00 01 B2              LOAD R4, (CT_DC)
00000138  00 00 00 F8              HALT

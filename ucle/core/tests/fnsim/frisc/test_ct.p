                       CT_CR       EQU 10000
                       CT_LR       EQU 10004
                       CT_DC       EQU 10004
                       CT_BS       EQU 10008
                       CT_IACK     EQU 1000C
                       
                                   ORG 0
00000000  00 10 80 07              MOVE 1000, SP
00000004  0C 01 00 C4              JP MAIN
00000008  00 01 00 00  INT_VEC     DW 100
                       
                                   ORG 0C
                       ; NMI_HNDLR   PUSH R0
                       ;             MOVE SR, R0
                       ;             PUSH R0
                       ;
                                   ; Do stuff
0000000C  08 00 01 B8              STORE R0, (CT_BS)
00000010  01 00 80 05              MOVE 1, R3
00000014  BA DC 80 06              MOVE 0DCBA, R5
                       
                       ;
                       ;             POP R0
                       ;             MOVE R0, SR
                       ;             POP R0
00000018  03 00 00 D8              RETN
                       
                                   ORG 100
                       INT_HNDLR   ; PUSH R0
                                   ; MOVE SR, R0
                                   ; PUSH R0
                       
00000100  08 00 01 B8              STORE R0, (CT_BS)
00000104  01 00 80 05              MOVE 1, R3
                       
                                   ; POP R0
                                   ; MOVE R0, SR
                                   ; POP R0
00000108  01 00 00 D8              RETI
                       
0000010C  10 00 00 04  MAIN        MOVE %B 10000, R0
00000110  00 00 10 00              MOVE R0, SR         ; GIE = 1
                       
00000114  B8 0B 00 04              MOVE %D 3000, R0
00000118  04 00 01 B8              STORE R0, (CT_LR)   ; Count down from 3000
                       
0000011C  07 00 00 04              MOVE 7, R0          ; start = 1, do_int = 1, nmi = 1
00000120  00 00 01 B8              STORE R0, (CT_CR)   ; Start CT
                       
00000124  00 00 00 05              MOVE 0, R2
00000128  01 00 20 25  LOOP        ADD R2, 1, R2       ; Count cycles till interrupt, cnt = R2 * 3
0000012C  01 00 30 6C              CMP R3, 1           ; On CT interrupt, R3 will become 1, then stop
00000130  28 01 00 C6              JP_NZ LOOP
00000134  04 00 01 B2              LOAD R4, (CT_DC)    ; Store current CT value to R4
00000138  00 00 00 F8              HALT

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
00000004  10 01 00 C4              JP      MAIN
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
                       INT_HNDLR   ; PUSH    R0
                                   ; MOVE    SR, R0
                                   ; PUSH    R0
                       
                                   ; Do stuff
00000100  08 00 02 B8              STORE   R0, (GPIO2_BS)
00000104  01 00 00 07              MOVE    1, R6
00000108  0C 00 02 B8              STORE   R0, (GPIO2_IACK)
                       
                                   ; POP     R0
                                   ; MOVE    R0, SR
                                   ; POP     R0
0000010C  01 00 00 D8              RETI
                       
00000110  10 00 00 04  MAIN        MOVE    %B 10000, R0
00000114  00 00 10 00              MOVE    R0, SR          ; GIE = 1
                       
                       ;             ;  LED test program
                       ;             MOVE    %B 10, R0
                       ;             STORE   R0, (GPIO1_CR)
                       
                       ;             MOVE    0FF, R1
                       ; LOOP1       STORE   R1, (GPIO1_DR)
                       ;             SUB     R1, 1, R1
                       ;             JP_NZ   LOOP1
                       
                                   ; SWITCH test program
00000118  30 01 00 B0              LOAD    R0, (SW_CR)
0000011C  00 00 02 B8              STORE   R0, (GPIO2_CR)
                       
00000120  01 00 20 25  LOOP2       ADD     R2, 1, R2
00000124  01 00 60 6C              CMP     R6, 1
00000128  20 01 00 C6              JP_NZ   LOOP2
                       
0000012C  00 00 00 F8              HALT
                       
                                   ;     --------aaaaaaaammmmmmmm---opiMM
00000130  17 0F FF 00  SW_CR       DW %B 00000000111111110000111100010111

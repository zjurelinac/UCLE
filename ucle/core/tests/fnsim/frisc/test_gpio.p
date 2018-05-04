                       GPIO1_CR    EQU     10000
                       GPIO1_DR    EQU     10004
                       GPIO1_BS    EQU     10008
                       GPIO1_IACK  EQU     1000C
                       
                       GPIO2_CR    EQU     20000
                       GPIO2_DR    EQU     20004
                       GPIO2_BS    EQU     20008
                       GPIO2_IACK  EQU     2000C
                       
                       GPIO3_CR    EQU     30000
                       GPIO3_DR    EQU     30004
                       GPIO3_BS    EQU     30008
                       GPIO3_IACK  EQU     3000C
                       
                       GPIO4_CR    EQU     40000
                       GPIO4_DR    EQU     40004
                       GPIO4_BS    EQU     40008
                       GPIO4_IACK  EQU     4000C
                       
                                   ORG     0
00000000  00 10 80 07              MOVE    1000, SP
00000004  14 01 00 C4              JP      MAIN
00000008  00 01 00 00  INT_VEC     DW      100
                       
                                   ORG     0C
                       ; NMI_HNDLR   PUSH    R0
                       ;             MOVE    SR, R0
                       ;             PUSH    R0
                       
                       ;             ; Do stuff
                       
                       ;             POP     R0
                       ;             MOVE    R0, SR
                       ;             POP     R0
0000000C  03 00 00 D8              RETN
                       
                                   ORG     100
                       INT_HNDLR   ; PUSH    R0
                                   ; MOVE    SR, R0
                                   ; PUSH    R0
                       
                                   ; Do stuff
00000100  08 00 04 B8              STORE   R0, (GPIO4_BS)
00000104  01 00 60 27              ADD     R6, 1, R6
00000108  04 00 04 BB              STORE   R6, (GPIO4_DR)
0000010C  0C 00 04 B8              STORE   R0, (GPIO4_IACK)
                       
                                   ; POP     R0
                                   ; MOVE    R0, SR
                                   ; POP     R0
00000110  01 00 00 D8              RETI
                       
00000114  10 00 00 04  MAIN        MOVE    %B 10000, R0
00000118  00 00 10 00              MOVE    R0, SR          ; GIE = 1
                       
                       ;             ;  LED test program
                       ;             MOVE    %B 10, R0
                       ;             STORE   R0, (GPIO1_CR)
                       
                       ;             MOVE    0FF, R1
                       ; LOOP1       STORE   R1, (GPIO1_DR)
                       ;             SUB     R1, 1, R1
                       ;             JP_NZ   LOOP1
                       
                       ;             ; SWITCH test program
                       ;             LOAD    R0, (SW_CR)
                       ;             STORE   R0, (GPIO2_CR)
                       
                       ; LOOP2       ADD     R2, 1, R2
                       ;             CMP     R6, 1
                       ;             JP_NZ   LOOP2
                       
                       ;             ; UART RX test program
                       ;             MOVE    %B 0101, R0
                       ;             STORE   R0, (GPIO3_CR)
                       
                       ; LOOP3       ADD     R2, 1, R2
                       ;             CMP     R6, %D 127
                       ;             JP_NZ   LOOP3
                       
                                   ; UART TX test program
0000011C  04 00 00 04              MOVE    %B 0100, R0
00000120  00 00 04 B8              STORE   R0, (GPIO4_CR)
                       
00000124  01 00 20 25  LOOP4       ADD     R2, 1, R2
00000128  7F 00 60 6C              CMP     R6, %D 127
0000012C  24 01 00 C6              JP_NZ   LOOP4
                       
00000130  00 00 00 F8              HALT
                       
                                   ;     --------aaaaaaaammmmmmmm---opiMM
00000134  17 0F FF 00  SW_CR       DW %B 00000000111111110000111100010111

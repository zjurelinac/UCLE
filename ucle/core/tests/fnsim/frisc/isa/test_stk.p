                               ORG 0
00000000  00 04 80 07          MOVE STACK_END, SP
                       
00000004  07 00 00 04          MOVE 7, R0
00000008  00 00 00 88          PUSH R0
0000000C  00 00 70 B4          LOAD R0, (SP)           ; assert R0 == 7
                       
00000010  CD AB 80 04          MOVE 0ABCD, R1
00000014  00 00 80 88          PUSH R1
00000018  00 00 F0 B4          LOAD R1, (SP)           ; assert R1 == 0xABCD
                       
0000001C  00 00 00 81          POP R2                  ; assert R2 == 0xABCD
00000020  00 00 80 81          POP R3                  ; assert R3 == 7
                       
00000024  58 00 00 B2          LOAD R4, (NUM)
00000028  00 00 00 8A          PUSH R4
0000002C  08 00 40 5E          SHR R4, 8, R4
00000030  00 00 00 8A          PUSH R4
00000034  08 00 40 5E          SHR R4, 8, R4
00000038  00 00 00 8A          PUSH R4
0000003C  08 00 40 5E          SHR R4, 8, R4
00000040  00 00 00 8A          PUSH R4
00000044  08 00 40 5E          SHR R4, 8, R4
                       
00000048  00 00 00 82          POP R4                  ; assert R4 == 0x11
0000004C  00 00 80 82          POP R5                  ; assert R5 == 0x1122
00000050  00 00 00 83          POP R6                  ; assert R6 == 0x112233
                       
                                                       ; assert SP == 0x3FC
                       
00000054  00 00 00 F8          HALT
                       
00000058  44 33 22 11  NUM     DW 11223344
                       
                               ORG 200
                       STACK   DS 200
                       STACK_END

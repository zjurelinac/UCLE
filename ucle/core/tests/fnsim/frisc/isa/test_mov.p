                               ORG 0
                       
00000000  07 00 10 04          MOVE    7, SR
                       
00000004  0C 00 00 C5          JP_NC   CONT_0
00000008  01 00 00 04          MOVE    1, R0
                       CONT_0                          ; assert R0 == 1
0000000C  14 00 80 C5          JP_NV   CONT_1
00000010  01 00 80 04          MOVE    1, R1
                       CONT_1                          ; assert R1 == 1
00000014  1C 00 80 C4          JP_NN   CONT_2
00000018  01 00 00 05          MOVE    1, R2
                       CONT_2                          ; assert R2 == 1
0000001C  24 00 00 C6          JP_NZ   CONT_3
00000020  01 00 80 05          MOVE    1, R3           ; assert R3 == 0
                       CONT_3
                       
00000024  00 00 20 02          MOVE    SR, R4
00000028  04 00 40 1E          XOR     R4, 4, R4
0000002C  00 00 18 00          MOVE    R4, SR
                       
00000030  38 00 40 C5          JP_V    CONT_4
00000034  00 00 00 06          MOVE    0, R4
                       CONT_4                          ; assert R4 == 0
                       
00000038  00 00 A0 02          MOVE    SR, R5
0000003C  02 00 D0 1E          XOR     R5, 2, R5
00000040  00 00 1A 00          MOVE    R5, SR
                       
00000044  4C 00 C0 C4          JP_C    CONT_5
00000048  00 00 80 06          MOVE    0, R5
                       CONT_5                          ; assert R5 == 0
                       
0000004C  00 00 20 03          MOVE    SR, R6          ; assert R6 == 1
                       
00000050  00 00 00 F8          HALT

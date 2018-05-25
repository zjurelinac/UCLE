                               ORG 0
00000000  00 00 00 04          MOVE    0, R0
00000004  01 00 00 2C          ADC     R0, 1, R0       ; assert R0 == 1
00000008  01 00 80 04          MOVE    1, R1
0000000C  02 00 10 04          MOVE    2, SR
00000010  01 00 90 2C          ADC     R1, 1, R1       ; assert R1 == 3
00000014  FF FF 0F 05          MOVE    -1, R2
00000018  02 00 10 04          MOVE    2, SR
0000001C  00 00 20 2D          ADC     R2, 0, R2       ; assert R2 == 0
00000020  00 00 A0 01          MOVE    SR, R3
00000024  00 00 B0 2D          ADC     R3, 0, R3       ; assert R3 == 11
00000028  04 00 00 06          MOVE    4, R4
0000002C  00 00 10 04          MOVE    0, SR
00000030  01 00 40 3E          SBC     R4, 1, R4       ; assert R4 == 3
00000034  05 00 80 06          MOVE    5, R5
00000038  02 00 10 04          MOVE    2, SR
0000003C  01 00 D0 3E          SBC     R5, 1, R5       ; assert R5 == 3
00000040  01 00 00 07          MOVE    1, R6
00000044  02 00 10 04          MOVE    2, SR
00000048  01 00 60 3F          SBC     R6, 1, R6       ; assert R6 == -1
0000004C  00 00 00 F8          HALT

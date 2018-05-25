                               ORG 0
00000000  0E 00 00 04          MOVE    0E, R0
00000004  05 00 00 14          AND     R0, 5, R0       ; assert R0 == 4
00000008  CD AB 80 04          MOVE    0ABCD, R1
0000000C  F0 F0 90 14          AND     R1, 0F0F0, R1   ; assert R1 == 0xA0C0
00000010  02 00 00 05          MOVE    2, R2
00000014  0C 00 20 0D          OR      R2, 0C, R2      ; assert R2 == 0E
00000018  CD A0 80 05          MOVE    0A0CD, R3
0000001C  00 0B B0 0D          OR      R3, 0B00, R3    ; assert R3 == 0xABCD
00000020  04 00 00 06          MOVE    4, R4
00000024  07 00 40 1E          XOR     R4, 7, R4       ; assert R4 == 3
00000028  AA AA 80 06          MOVE    0AAAA, R5
0000002C  FF FF D0 1E          XOR     R5, 0FFFF, R5   ; assert R5 == 0x5555
00000030  FE FF 0F 07          MOVE    -2, R6
00000034  FF FF 6F 1F          XOR     R6, -1, R6      ; assert R6 == 1
00000038  00 00 00 F8          HALT

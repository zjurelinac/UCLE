                               ORG 0
00000000  01 00 00 04          MOVE    1, R0
00000004  02 00 00 54          SHL     R0, 2, R0       ; assert R0 == 4
00000008  09 00 80 04          MOVE    9, R1
0000000C  1E 00 90 54          SHL     R1, %D 30, R1   ; assert R1 == 0x4000000
00000010  C8 00 00 05          MOVE    %D 200, R2
00000014  03 00 20 5D          SHR     R2, 3, R2       ; assert R2 == 25
00000018  FF FF 8F 05          MOVE    -1, R3
0000001C  14 00 B0 5D          SHR     R3, %D 20, R3   ; assert R3 == 0xFFF
00000020  E0 FF 0F 06          MOVE    -020, R4
00000024  04 00 40 66          ASHR    R4, 4, R4       ; assert R4 == -2
00000028  FF FF 8F 06          MOVE    -1, R5
0000002C  28 00 D0 66          ASHR    R5, %D 40, R5   ; assert R5 == -1
00000030  FF FF 0F 07          MOVE    -1, R6
00000034  28 00 60 5F          SHR     R6, %D 40, R6   ; assert R6 == 0
00000038  00 00 00 F8          HALT

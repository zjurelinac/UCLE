                               ORG     0
00000000  00 00 00 04          MOVE    0, R0
00000004  01 00 00 24          ADD     R0, 1, R0       ; assert R0 == 1
00000008  01 00 80 04          MOVE    1, R1
0000000C  01 00 90 24          ADD     R1, 1, R1       ; assert R1 == 2
00000010  02 00 00 05          MOVE    2, R2
00000014  00 00 22 21          ADD     R2, R1, R2      ; assert R2 == 4
00000018  03 00 80 05          MOVE    3, R3
0000001C  FF FF BF 25          ADD     R3, -1, R3      ; assert R3 == 2
00000020  04 00 00 06          MOVE    4, R4
00000024  FC FF 4F 26          ADD     R4, -4, R4      ; assert R4 == 0
00000028  05 00 80 06          MOVE    5, R5
0000002C  FA FF DF 26          ADD     R5, -6, R5      ; assert R5 == -1
00000030  06 00 00 07          MOVE    6, R6
00000034  00 00 6C 23          ADD     R6, R6, R6      ; assert R6 == 12
00000038  00 00 00 F8          HALT

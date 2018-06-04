                               ORG 0
00000000  60 00 80 07          MOVE    MSTART, SP
                       
00000004  01 00 00 04          MOVE    1, R0
00000008  60 00 00 B8          STORE   R0, (M1)
0000000C  60 00 00 B0          LOAD    R0, (M1)        ; assert R0 == 1
                       
00000010  5C 00 80 B0          LOAD    R1, (NUM1)
00000014  04 00 F0 BC          STORE   R1, (SP+4)
00000018  04 00 F0 B4          LOAD    R1, (SP+4)      ; assert R1 == 0xABCDDCBA
                       
0000001C  02 00 00 05          MOVE    2, R2
00000020  68 00 00 A9          STOREH  R2, (M3)
00000024  68 00 00 A1          LOADH   R2, (M3)        ; assert R2 == 2
                       
00000028  5C 00 80 B1          LOAD    R3, (NUM1)
0000002C  0C 00 F0 AD          STOREH  R3, (SP+0C)
00000030  0C 00 F0 A5          LOADH   R3, (SP+0C)     ; assert R3 == 0xDCBA
                       
00000034  03 00 00 06          MOVE    3, R4
00000038  70 00 00 9A          STOREB  R4, (M5)
0000003C  70 00 00 92          LOADB   R4, (M5)        ; assert R4 == 3
                       
00000040  5C 00 80 B2          LOAD    R5, (NUM1)
00000044  14 00 F0 9E          STOREB  R5, (SP+14)
00000048  14 00 F0 96          LOADB   R5, (SP+14)     ; assert R5 == 0xBA
                       
0000004C  FF 00 00 07          MOVE    0FF, R6
00000050  5C 00 00 9B          STOREB  R6, (NUM1)
00000054  5C 00 00 B3          LOAD    R6, (NUM1)      ; assert R6 == 0xABCDCFF
00000058  00 00 00 F8          HALT
                       
0000005C  BA DC CD AB  NUM1    DW 0ABCDDCBA
                       
                       MSTART
00000060  00 00 00 00  M1      DW 0
00000064  00 00 00 00  M2      DW 0
00000068  00 00 00 00  M3      DH 0
0000006C  00 00 00 00  M4      DH 0
00000070  00 00 00 00  M5      DB 0
00000074  00 00 00 00  M6      DB 0

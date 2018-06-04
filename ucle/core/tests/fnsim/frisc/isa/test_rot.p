                               ORG 0
00000000  01 00 00 04          MOVE    1, R0
00000004  0A 00 00 44          ROTL    R0, %D 10, R0   ; assert R0 == 1024
00000008  03 00 80 04          MOVE    3, R1
0000000C  22 00 90 44          ROTL    R1, %D 34, R1   ; assert R1 == 0x0C
00000010  3C 00 00 B1          LOAD    R2, (NUM)
00000014  0C 00 20 45          ROTL    R2, %D 12, R2   ; assert R2 == 0xBCDCDABA
00000018  01 00 80 05          MOVE    1, R3
0000001C  16 00 B0 4D          ROTR    R3, %D 22, R3   ; assert R3 == 1024
00000020  06 00 00 06          MOVE    6, R4
00000024  21 00 40 4E          ROTR    R4, %D 33, R4   ; assert R4 == 3
00000028  3C 00 80 B2          LOAD    R5, (NUM)
0000002C  0C 00 D0 4E          ROTR    R5, %D 12, R5   ; assert R5 = 0xDCDABABC
00000030  01 00 00 07          MOVE    1, R6
00000034  00 04 60 4F          ROTR    R6, %D 1024, R6 ; assert R6 == 1
00000038  00 00 00 F8          HALT
                       
0000003C  CD CD AB AB  NUM     DW 0ABABCDCD

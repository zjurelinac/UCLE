                               ORG 0
00000000  24 00 80 07          MOVE    MSTART, SP
00000004  24 00 00 B0          LOAD    R0, (M1)        ; assert R0 == 0x11111111
00000008  04 00 F0 B4          LOAD    R1, (SP+4)      ; assert R1 == 0x22222222
0000000C  2C 00 00 A1          LOADH   R2, (M3)        ; assert R2 == 0x3333
00000010  0C 00 F0 A5          LOADH   R3, (SP+0C)     ; assert R3 == 0x4444
00000014  34 00 00 92          LOADB   R4, (M5)        ; assert R4 == 0x55
00000018  14 00 F0 96          LOADB   R5, (SP + 14)   ; assert R5 == 0x66
0000001C  00 00 70 97          LOADB   R6, (SP)        ; assert R6 == 0x11
00000020  00 00 00 F8          HALT
                       
                       MSTART
00000024  11 11 11 11  M1      DW 11111111
00000028  22 22 22 22  M2      DW 22222222
0000002C  33 33 00 00  M3      DH 3333
00000030  44 44 00 00  M4      DH 4444
00000034  55 00 00 00  M5      DB 55
00000038  66 00 00 00  M6      DB 66

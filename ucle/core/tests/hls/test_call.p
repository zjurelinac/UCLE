                               ORG 0
00000000  00 10 80 07          MOVE 1000, SP
00000004  10 00 00 C4          JP MAIN
                       
                       
00000008  00 00 02 20  ADDER   ADD R0, R1, R0
0000000C  00 00 00 D8          RET
                       
                       
00000010  05 00 00 04  MAIN    MOVE 5, R0
00000014  10 00 80 04          MOVE 10, R1
00000018  08 00 00 CC          CALL ADDER
0000001C  00 00 80 02          MOVE R0, R5
00000020  00 00 00 F8          HALT

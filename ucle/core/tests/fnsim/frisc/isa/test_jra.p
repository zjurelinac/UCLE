                               ORG 0
00000000  04 00 00 D4          JR      SKIP
00000004  01 00 00 04          MOVE    1, R0
                       SKIP                    ; assert R0 == 0
                       
00000008  05 00 80 07          MOVE    5, SP
                       
0000000C  04 00 70 6C  C1      CMP     SP, 4
00000010  08 00 00 D5          JR_ULT  T1
00000014  00 00 80 04  F1      MOVE    0, R1
00000018  04 00 00 D4          JR      C2
0000001C  01 00 80 04  T1      MOVE    1, R1
                                               ; assert R1 == 0
00000020  06 00 70 6C  C2      CMP     SP, 6
00000024  30 00 00 C5          JP_ULT  T2
00000028  00 00 00 05  F2      MOVE    0, R2
0000002C  04 00 00 D4          JR      C3
00000030  01 00 00 05  T2      MOVE    1, R2
                                               ; assert R2 == 1
00000034  05 00 70 6C  C3      CMP     SP, 5
00000038  08 00 C0 D5          JR_EQ   T3
0000003C  00 00 80 05  F3      MOVE    0, R3
00000040  04 00 00 D4          JR      END
00000044  01 00 80 05  T3      MOVE    1, R3
                                               ; assert R3 == 1
                       
00000048  00 00 00 F8  END     HALT

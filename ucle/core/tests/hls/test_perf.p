                               ORG 0
00000000  18 00 00 B0          LOAD R0, (N)
00000004  00 00 00 6C  LOOP    CMP R0, 0
00000008  14 00 C0 C5          JP_EQ END
0000000C  01 00 00 34          SUB R0, 1, R0
00000010  04 00 00 C4          JP LOOP
                       
00000014  00 00 00 F8  END     HALT
                       
00000018  00 E1 F5 05  N       DW %D 100000000

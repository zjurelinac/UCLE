                               ORG 0
00000000  10 00 00 B0          LOAD R0, (N)
00000004  01 00 00 34  LOOP    SUB R0, 1, R0
00000008  04 00 00 C6          JP_NE LOOP
                       
0000000C  00 00 00 F8  END     HALT
                       
00000010  01 E1 F5 05  N       DW %D 100000001

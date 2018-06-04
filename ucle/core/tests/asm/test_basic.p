                       GPIO_DR EQU 0FFF10000
                               ORG 0
                               ; ADD R2, GPIO_DR, R2
                               ; XOR R3, R4, R5
                               ; CMP R0, 7
                               ; MOVE R2, R3
                               ; MOVE 0FFFC, SP
                               ; MOVE SR, R1
                               ; MOVE R1, SR
                               ; LOAD R2, (LABEL1)
                               ; LOADB R3, (R1)
                               ; STORE R4, (SP-8)
                               ; PUSH R1
                               ; POP SP
00000000  20 00 00 C4          JP MAIN
00000004  20 00 40 C6          JP_ULE MAIN
00000008  20 00 80 D7          JR_SGT MAIN
0000000C  00 00 04 C0          JP (R2)
00000010  FE EF CD AB          DW 0ABCDEFFE
00000014  05 06 07 08  LABEL1  DB 5, 6, 7, 8, 9, %D 10, %D 11, %D 12, 0D, 0E, 0F
00000018  09 0A 0B 0C  
0000001C  0D 0E 0F 00  
                               ; ORG %D 100
                       MAIN
00000020  00 00 00 F8          HALT
                               DS %D 40
0000004C  05 00 00 00          DW 5, 6, 7
00000050  06 00 00 00  
00000054  07 00 00 00  
00000058  01 02 03 00          DB 1, 2, 3
0000005C  07 00 08 00          DH 7, 8, 9, 10, 11, 12
00000060  09 00 10 00  
00000064  11 00 12 00  
00000068  01 00 00 00          DW 1
                               ; stuff

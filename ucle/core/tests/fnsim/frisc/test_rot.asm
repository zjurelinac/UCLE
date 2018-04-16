        ORG 0
        MOVE    1, R0
        ROTL    R0, %D 10, R0   ; assert R0 == 1024
        MOVE    3, R1
        ROTL    R1, %D 34, R1   ; assert R1 == 0x0C
        LOAD    R2, (NUM)
        ROTL    R2, %D 12, R2   ; assert R2 == 0xBCDCDABA
        MOVE    1, R3
        ROTR    R3, %D 22, R3   ; assert R3 == 1024
        MOVE    6, R4
        ROTR    R4, %D 33, R4   ; assert R4 == 3
        LOAD    R5, (NUM)
        ROTR    R5, %D 12, R5   ; assert R5 = 0xDCDABABC
        MOVE    1, R6
        ROTR    R6, %D 1024, R6 ; assert R6 == 1
        HALT

NUM     DW 0ABABCDCD

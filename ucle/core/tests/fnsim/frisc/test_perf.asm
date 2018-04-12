        ORG 0
        LOAD R0, (N)
LOOP    SUB R0, 1, R0
        JP_NE LOOP

        HALT

N       DW %D 500000001

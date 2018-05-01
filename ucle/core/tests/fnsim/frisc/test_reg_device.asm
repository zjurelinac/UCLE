            ORG 0
            MOVE 7, R0
            MOVE 1, R6

            STORE R0, (10000)
            MOVE 2, R6

            LOAD R1, (10000)
            MOVE 3, R6

            MOVE 0789AB, R0
            STORE R0, (10004)
            MOVE 4, R6

            LOADB R2, (10004)
            MOVE 5, R6

            MOVE 0CD, R0
            STOREB R0, (10004)
            MOVE 6, R6

            LOADH R3, (10004)
            MOVE 7, R6

            HALT

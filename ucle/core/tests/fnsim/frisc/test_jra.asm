        ORG 0
        JR      SKIP
        MOVE    1, R0
SKIP                    ; assert R0 == 0

        MOVE    5, SP

C1      CMP     SP, 4
        JR_ULT  T1
F1      MOVE    0, R1
        JR      C2
T1      MOVE    1, R1
                        ; assert R1 == 0
C2      CMP     SP, 6
        JP_ULT  T2
F2      MOVE    0, R2
        JR      C3
T2      MOVE    1, R2
                        ; assert R2 == 1
C3      CMP     SP, 5
        JR_EQ   T3
F3      MOVE    0, R3
        JR      END
T3      MOVE    1, R3
                        ; assert R3 == 1

END     HALT

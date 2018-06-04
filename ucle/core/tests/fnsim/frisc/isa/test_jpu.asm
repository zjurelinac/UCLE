        ORG 0
        MOVE 5, R0
_1C     CMP R0, 5   ; 5 > 5 = 0
        JP_UGT _1T
_1F     JP _2C
_1T     MOVE 1, R1

_2C     CMP R0, 5   ; 5 >= 5 = 1
        JP_UGE _2T
_2F     JP _3C
_2T     MOVE 1, R2

_3C     CMP R0, 5   ; 5 <= 5 = 1
        JP_ULE _3T
_3F     JP _4C
_3T     MOVE 1, R3

_4C     CMP R0, 5   ; 5 < 5 = 0
        JP_ULT _4T
_4F     JP _5C
_4T     MOVE 1, R4

_5C     CMP R0, 3   ; 5 > 3 = 1
        JP_UGT _5T
_5F     JP _6C
_5T     MOVE 1, R5

_6C     CMP R0, 6   ; 5 <= 6 = 1
        JP_ULE _6T
_6F     JP _7C
_6T     MOVE 1, R6

_7C     CMP R0, -1  ; 5 >= -1 = 0
        JP_UGT _7T
_7F     JP _END
_7T     MOVE 1, R7

_END    HALT

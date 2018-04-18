#! /usr/bin/env python3

import os
import subprocess


TESTS = [
    # MOVE operation
    # { "file": "fnsim/frisc/test_mov.p", "checks": [ "rs.R0=1", "rs.R1=1", "rs.R2=1", "rs.R3=0", "rs.R4=0", "rs.R5=0", "rs.R6=1" ] },

    # ALU operations
    # { "file": "fnsim/frisc/test_add.p", "checks": [ "rs.R0=1", "rs.R1=2", "rs.R2=4", "rs.R3=2", "rs.R4=0", "rs.R5=0xFFFFFFFF", "rs.R6=12" ] },
    # { "file": "fnsim/frisc/test_sub.p", "checks": [ "rs.R0=0", "rs.R1=0", "rs.R2=1", "rs.R3=2", "rs.R4=10", "rs.R5=0xFFFFFFFE", "rs.R6=0xFFFFFFFC" ] },
    # { "file": "fnsim/frisc/test_aox.p", "checks": [ "rs.R0=4", "rs.R1=0xA0C0", "rs.R2=0xE", "rs.R3=0xABCD", "rs.R4=3", "rs.R5=0x5555", "rs.R6=1" ] },
    # { "file": "fnsim/frisc/test_asc.p", "checks": [ "rs.R0=1", "rs.R1=3", "rs.R2=0", "rs.R3=11", "rs.R4=3", "rs.R5=3", "rs.R6=0xFFFFFFFF" ] },

    # { "file": "fnsim/frisc/test_slr.p", "checks": [ "rs.R0=4", "rs.R1=0x40000000", "rs.R2=25", "rs.R3=0xFFF", "rs.R4=0xFFFFFFFE", "rs.R5=0xFFFFFFFF", "rs.R6=0", '-v' ] },
    # { "file": "fnsim/frisc/test_rot.p", "checks": [ "rs.R0=1024", "rs.R1=0x0C", "rs.R2=0xBCDCDABA", "rs.R3=1024", "rs.R4=3", "rs.R5=0xDCDABABC", "rs.R6=1" ] },

    # Memory operations
    # { "file": "fnsim/frisc/test_ldr.p", "checks": [ "rs.R0=0x11111111", "rs.R1=0x22222222", "rs.R2=0x3333", "rs.R3=0x4444", "rs.R4=0x55", "rs.R5=0x66", "rs.R6=0x11" ] },
    # { "file": "fnsim/frisc/test_str.p", "checks": [ "rs.R0=1", "rs.R1=0xABCDDCBA", "rs.R2=2", "rs.R3=0xDCBA", "rs.R4=3", "rs.R5=0xBA", "rs.R6=0xABCDDCFF" ] },
    # { "file": "fnsim/frisc/test_stk.p", "checks": [ "rs.R0=7", "rs.R1=0xABCD", "rs.R2=0xABCD", "rs.R3=7", "rs.R4=0x11", "rs.R5=0x1122", "rs.R6=0x112233", "rs.SP=0x3FC" ] },

    # Control operations
    # { "file": "fnsim/frisc/test_jpu.p", "checks": [ "rs.R1=0", "rs.R2=1", "rs.R3=1", "rs.R4=0", "rs.R5=1", "rs.R6=1", "rs.SP=0" ] },
    # { "file": "fnsim/frisc/test_jps.p", "checks": [ "rs.R1=0", "rs.R2=1", "rs.R3=1", "rs.R4=0", "rs.R5=0", "rs.R6=0", "rs.SP=1" ] }
    # { "file": "fnsim/frisc/test_crt.p", "checks": [ "rs.R0=10", "rs.R1=0xCDAB", "rs.R2=2", "rs.R3=1" ] }
]


def main():
    for test in TESTS:
        subprocess.run(["../build/core/debug/fnsim-cli", "frisc", os.path.join("tests", test["file"]), "check"] + test["checks"])


if __name__ == '__main__':
    main()

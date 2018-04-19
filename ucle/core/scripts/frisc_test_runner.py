#! /usr/bin/env python3

import os
import subprocess


TESTS = [
    # MOVE operation
    { "file": "fnsim/frisc/test_mov.p", "checks": [ "r.R0=1", "r.R1=1", "r.R2=1", "r.R3=0", "r.R4=0", "r.R5=0", "r.R6=1" ] },

    # ALU operations
    { "file": "fnsim/frisc/test_add.p", "checks": [ "r.R0=1", "r.R1=2", "r.R2=4", "r.R3=2", "r.R4=0", "r.R5=-1", "r.R6=12" ] },
    { "file": "fnsim/frisc/test_sub.p", "checks": [ "r.R0=0", "r.R1=0", "r.R2=1", "r.R3=2", "r.R4=10", "r.R5=-2", "r.R6=-4" ] },
    { "file": "fnsim/frisc/test_aox.p", "checks": [ "r.R0=4", "r.R1=0xA0C0", "r.R2=0xE", "r.R3=0xABCD", "r.R4=3", "r.R5=0x5555", "r.R6=1" ] },
    { "file": "fnsim/frisc/test_asc.p", "checks": [ "r.R0=1", "r.R1=3", "r.R2=0", "r.R3=11", "r.R4=3", "r.R5=3", "r.R6=-1" ] },

    { "file": "fnsim/frisc/test_slr.p", "checks": [ "r.R0=4", "r.R1=0x40000000", "r.R2=25", "r.R3=0xFFF", "r.R4=-2", "r.R5=-1", "r.R6=0" ] },
    { "file": "fnsim/frisc/test_rot.p", "checks": [ "r.R0=1024", "r.R1=0x0C", "r.R2=0xBCDCDABA", "r.R3=1024", "r.R4=3", "r.R5=0xDCDABABC", "r.R6=1" ] },

    # Memory operations
    { "file": "fnsim/frisc/test_ldr.p", "checks": [ "r.R0=0x11111111", "r.R1=0x22222222", "r.R2=0x3333", "r.R3=0x4444", "r.R4=0x55", "r.R5=0x66", "r.R6=0x11" ] },
    { "file": "fnsim/frisc/test_str.p", "checks": [ "r.R0=1", "r.R1=0xABCDDCBA", "r.R2=2", "r.R3=0xDCBA", "r.R4=3", "r.R5=0xBA", "r.R6=0xABCDDCFF" ] },
    { "file": "fnsim/frisc/test_stk.p", "checks": [ "r.R0=7", "r.R1=0xABCD", "r.R2=0xABCD", "r.R3=7", "r.R4=0x11", "r.R5=0x1122", "r.R6=0x112233", "r.SP=0x3FC" ] },

    # Control operations
    { "file": "fnsim/frisc/test_jpu.p", "checks": [ "r.R1=0", "r.R2=1", "r.R3=1", "r.R4=0", "r.R5=1", "r.R6=1", "r.SP=0" ] },
    { "file": "fnsim/frisc/test_jps.p", "checks": [ "r.R1=0", "r.R2=1", "r.R3=1", "r.R4=0", "r.R5=0", "r.R6=0", "r.SP=1" ] },
    { "file": "fnsim/frisc/test_crt.p", "checks": [ "r.R0=10", "r.R1=0xCDAB", "r.R2=2", "r.R3=1" ] },
    { "file": "fnsim/frisc/test_jra.p", "checks": [ "r.R0=0", "r.R1=0", "r.R2=1", "r.R3=1" ] }
]


def main():
    for test in TESTS:
        subprocess.run(["../build/core/debug/fnsim-cli", "frisc", os.path.join("tests", test["file"]), "check"] + test["checks"])


if __name__ == '__main__':
    main()

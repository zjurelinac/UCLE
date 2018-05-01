#! /usr/bin/env python3

import os
import subprocess


TESTS = [
    # MOVE operation
    { "file": "fnsim/frisc/isa/test_mov.p", "checks": [ "r.R0=1", "r.R1=1", "r.R2=1", "r.R3=0", "r.R4=0", "r.R5=0", "r.R6=1" ] },

    # ALU operations
    { "file": "fnsim/frisc/isa/test_add.p", "checks": [ "r.R0=1", "r.R1=2", "r.R2=4", "r.R3=2", "r.R4=0", "r.R5=-1", "r.R6=12" ] },
    { "file": "fnsim/frisc/isa/test_sub.p", "checks": [ "r.R0=0", "r.R1=0", "r.R2=1", "r.R3=2", "r.R4=10", "r.R5=-2", "r.R6=-4" ] },
    { "file": "fnsim/frisc/isa/test_aox.p", "checks": [ "r.R0=4", "r.R1=0xA0C0", "r.R2=0xE", "r.R3=0xABCD", "r.R4=3", "r.R5=0x5555", "r.R6=1" ] },
    { "file": "fnsim/frisc/isa/test_asc.p", "checks": [ "r.R0=1", "r.R1=3", "r.R2=0", "r.R3=11", "r.R4=3", "r.R5=3", "r.R6=-1" ] },

    { "file": "fnsim/frisc/isa/test_slr.p", "checks": [ "r.R0=4", "r.R1=0x40000000", "r.R2=25", "r.R3=0xFFF", "r.R4=-2", "r.R5=-1", "r.R6=0" ] },
    { "file": "fnsim/frisc/isa/test_rot.p", "checks": [ "r.R0=1024", "r.R1=0x0C", "r.R2=0xBCDCDABA", "r.R3=1024", "r.R4=3", "r.R5=0xDCDABABC", "r.R6=1" ] },

    # Memory operations
    { "file": "fnsim/frisc/isa/test_ldr.p", "checks": [ "r.R0=0x11111111", "r.R1=0x22222222", "r.R2=0x3333", "r.R3=0x4444", "r.R4=0x55", "r.R5=0x66", "r.R6=0x11" ] },
    { "file": "fnsim/frisc/isa/test_str.p", "checks": [ "r.R0=1", "r.R1=0xABCDDCBA", "r.R2=2", "r.R3=0xDCBA", "r.R4=3", "r.R5=0xBA", "r.R6=0xABCDDCFF" ] },
    { "file": "fnsim/frisc/isa/test_stk.p", "checks": [ "r.R0=7", "r.R1=0xABCD", "r.R2=0xABCD", "r.R3=7", "r.R4=0x11", "r.R5=0x1122", "r.R6=0x112233", "r.SP=0x3FC" ] },

    # Control operations
    { "file": "fnsim/frisc/isa/test_jpu.p", "checks": [ "r.R1=0", "r.R2=1", "r.R3=1", "r.R4=0", "r.R5=1", "r.R6=1", "r.SP=0" ] },
    { "file": "fnsim/frisc/isa/test_jps.p", "checks": [ "r.R1=0", "r.R2=1", "r.R3=1", "r.R4=0", "r.R5=0", "r.R6=0", "r.SP=1" ] },
    { "file": "fnsim/frisc/isa/test_crt.p", "checks": [ "r.R0=10", "r.R1=0xCDAB", "r.R2=2", "r.R3=1" ] },
    { "file": "fnsim/frisc/isa/test_jra.p", "checks": [ "r.R0=0", "r.R1=0", "r.R2=1", "r.R3=1" ] }
]


def main():
    passed, failed = 0, 0
    failed_outs = []

    for i, test in enumerate(TESTS):
        proc = subprocess.run(["../build/core/debug/fnsim-cli", "frisc", os.path.join("tests", test["file"]), "check"] + test["checks"],
                              stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

        if proc.returncode == 0:
            passed += 1
        else:
            failed += 1
            failed_outs.append((i, proc.stdout))

    total = passed + failed

    if failed == 0:
        print('All %d tests passed.' % passed)
    else:
        print('%d tests out of %d failed (%.2f%% success rate):' % (failed, total, (100 * passed / total)))
        for i, output in failed_outs:
            print('(T%d): %s' % (i, output.decode()))



if __name__ == '__main__':
    main()

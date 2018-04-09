#! /usr/bin/env python3

import os
import subprocess


TESTS = [
    { "file": "fnsim/test_add.p", "checks": [ "regs.R0=1" ] },
    { "file": "fnsim/test_jump_ucond.p", "checks": [ "regs.R1=0", "regs.R2=1", "regs.R3=1", "regs.R4=0", "regs.R5=1", "regs.R6=1", "regs.SP=0", '-v' ] },
    { "file": "fnsim/test_jump_scond.p", "checks": [ "regs.R1=0", "regs.R2=1", "regs.R3=1", "regs.R4=0", "regs.R5=0", "regs.R6=0", "regs.SP=1", '-v' ] }
]


def main():
    for test in TESTS:
        subprocess.run(["../build/core/debug/fnsim-cli", "frisc", "-r", os.path.join("tests", test["file"]), "check"] + test["checks"])


if __name__ == '__main__':
    main()

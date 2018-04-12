#! /usr/bin/env python3

import os
import subprocess


TESTS = [
    { "file": "fnsim/frisc/test_add.p", "checks": [ "rs.R0=1",    "rs.R1=2",      "rs.R2=4",      "rs.R3=2",      "rs.R4=0",      "rs.R5=0xFFFFFFFF", "rs.R6=12" ] },
    { "file": "fnsim/frisc/test_sub.p", "checks": [ "rs.R0=0",    "rs.R1=0",      "rs.R2=1",      "rs.R3=2",      "rs.R4=10",     "rs.R5=0xFFFFFFFE", "rs.R6=0xFFFFFFFC" ] },
    { "file": "fnsim/frisc/test_aox.p", "checks": [ "rs.R0=4",    "rs.R1=0xA0C0", "rs.R2=0xE",    "rs.R3=0xABCD", "rs.R4=3",      "rs.R5=0x5555",     "rs.R6=1" ] },
    { "file": "fnsim/frisc/test_jump_ucond.p", "checks": [ "rs.R1=0", "rs.R2=1", "rs.R3=1", "rs.R4=0", "rs.R5=1", "rs.R6=1", "rs.SP=0", '-v' ] },
    { "file": "fnsim/frisc/test_jump_scond.p", "checks": [ "rs.R1=0", "rs.R2=1", "rs.R3=1", "rs.R4=0", "rs.R5=0", "rs.R6=0", "rs.SP=1", '-v' ] }
]


def main():
    for test in TESTS:
        subprocess.run(["../build/core/debug/fnsim-cli", "frisc", os.path.join("tests", test["file"]), "check"] + test["checks"])


if __name__ == '__main__':
    main()

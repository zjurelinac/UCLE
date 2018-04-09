#! /usr/bin/env python3

import os
import subprocess


TESTS = [
    { "file": "fnsim/test_add.p", "checks": [ "regs.R0=1", "regs.R1=0" ] },
]


def main():
    for test in TESTS:
        subprocess.run(["../build/core/debug/fnsim-cli", "frisc", os.path.join("tests", test["file"]), "check"] + test["checks"])


if __name__ == '__main__':
    main()

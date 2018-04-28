import json
import time
import subprocess
import sys


class Simulator:
    def __init__(self, cmd):
        self.p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    def read_response(self):
        resp = self.p.stdout.readline().decode()
        return json.loads(resp)

    def send_cmd(self, name, *args):
        self.p.stdin.write((json.dumps({'command': name, 'args': args}) + '\n').encode())
        self.p.stdin.flush()
        return self.read_response()


def main():
    sim = Simulator(['../../build/core/debug/fnsim-cli', 'frisc', '../tests/fnsim/frisc/test_add.p', '-j'])
    print(sim.send_cmd('start'))
    print(sim.send_cmd('step'))
    print(sim.send_cmd('info'))
    print(sim.send_cmd('break', 'add', 12))
    print(sim.send_cmd('break', 'list'))
    print(sim.send_cmd('cont'))


if __name__ == "__main__":
    main()

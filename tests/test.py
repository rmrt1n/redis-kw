from pwn import *
import os
import sys
from str_cmds import *

def check(c, inp, expected, msg):
    c.recvuntil('> ')
    c.sendline(inp);
    res = c.recvline().strip()[1:].decode('utf-8')
    ok = True if res == expected else False
    if not ok:
        print(msg)
        exit(1)

def test_all(c):
    test_set(c)

if __name__ == '__main__':
    c = remote('localhost', 6379);
    test_all(c);
    c.close()

from pwn import *
import os

def check(c, inp, expected, msg):
    c.recvuntil('> ')
    c.sendline(inp);
    res = c.recvline().strip()[1:].decode('utf-8')
    ok = True if res == expected else False
    if not ok:
        print(msg)
        exit(1)

def checklines(c, n, inp, expected, msg):
    c.recvuntil('> ')
    c.sendline(inp);
    for i in range(n):
        res = c.recvline().strip().replace(b'\x00', b'').decode('utf-8')
        ok = True if res == expected[i] else False
        if not ok:
            print(msg)
            exit(1)


def cleanup(c):
    c.recvuntil('> ')
    c.sendline('del a b c d');
    res = c.recvline().strip()[1:].decode('utf-8')
    c.recvuntil('> ')
    c.sendline('exists a b c d');
    res = c.recvline().strip().replace(b'\x00', b'').decode('utf-8')
    if res != '(integer) 0':
        print('cleanup failed')
        exit(1)


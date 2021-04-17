from pwn import *
import os
import sys
import str_cmds, key_cmds, list_cmds, set_cmds, hash_cmds

def test_all(c):
    key_cmds.test_key_cmds(c)
    str_cmds.test_str_cmds(c)
    hash_cmds.test_hash_cmds(c)
    list_cmds.test_list_cmds(c)
    set_cmds.test_set_cmds(c)

if __name__ == '__main__':
    c = remote('localhost', 6379);
    test_all(c)
    c.close()

from common import *

def test_key_cmds(c):
    print('testing key cmds')
    test_del(c)
    test_type(c)
    test_exists(c)
    print('all key cmds passed')

def test_del(c):
    # check gen
    check(c, 'set a hello', 'OK',
          'failed to set value to str key')
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to set value to hash key')
    check(c, 'rpush c 1', '(integer) 1',
          'failed to set value to list key')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to set value to set key')
    check(c, 'del a', '(integer) 1',
          'failed to del str key')
    check(c, 'del a', '(integer) 0',
          'deleted (deleted str key)')
    check(c, 'del b c', '(integer) 2',
          'failed to del multiple keys (hash & list type)')
    check(c, 'del b', '(integer) 0',
          'deleted (deleted hash key)')
    check(c, 'del c', '(integer) 0',
          'deleted (deleted list key)')
    check(c, 'del d e', '(integer) 1',
          'failed to delete set key & non existing key')
    check(c, 'del f', '(integer) 0',
          'deleted non existing key')
    # check args
    check(c, 'del', 'ERR wrong number of arguments for \'del\'',
          'failed to recognize syntax error')
    cleanup(c)
    print('\tdel cmd test passed')

def test_type(c):
    # check gen
    check(c, 'set a hello', 'OK',
          'failed to set value to str key')
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to set value to hash key')
    check(c, 'rpush c 1', '(integer) 1',
          'failed to set value to list key')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to set value to set key')
    check(c, 'type a', '"string"',
          'failed to recognize str type')
    check(c, 'type b', '"hash"',
          'failed to recognize hash type')
    check(c, 'type c', '"list"',
          'failed to recognize list type')
    check(c, 'type d', '"set"',
          'failed to recognize set type')
    check(c, 'type e', '"none"',
          'recognized non existing key (nil)')
    # check args
    check(c, 'type', 'ERR wrong number of arguments (given 0, expected 1)',
          'recognized non existing key (nil)')
    check(c, 'type a b', 'ERR wrong number of arguments (given 2, expected 1)',
          'recognized non existing key (nil)')
    cleanup(c)
    print('\ttype cmd test passed')

def test_exists(c):
    # check gen
    check(c, 'set a hello', 'OK',
          'failed to set value to str key')
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to set value to hash key')
    check(c, 'rpush c 1', '(integer) 1',
          'failed to set value to list key')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to set value to set key')
    check(c, 'exists a', '(integer) 1',
          'failed to recognize existing str type')
    check(c, 'exists b c', '(integer) 2',
          'failed to recognize multiple keys (hash & list type)')
    check(c, 'exists d e', '(integer) 1',
          'failed to recognize existing & nonexisting keys (set & nil)')
    check(c, 'exists e', '(integer) 0',
          'recognized non existing key (nil)')
    # check args
    check(c, 'exists', 'ERR wrong number of arguments for \'exists\'',
          'failed to recognized syntax error')
    cleanup(c)
    print('\texists cmd test passed')


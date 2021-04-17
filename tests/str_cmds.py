from common import *

def test_str_cmds(c):
    print('testing str cmds')
    test_set(c)
    test_get(c)
    print('all str cmds tests passed')

def test_set(c):
    # check gen
    check(c, 'set a hello', 'OK',
          'failed to set a to hello')
    check(c, 'set a hi', 'OK',
          'failed to change value of existing str')
    check(c, 'set a "hello world"', 'OK',
          'failed to set string with spaces')
    check(c, 'set a', 'OK',
          'failed to set value to empty str')
    # check args
    check(c, 'set a 1 2', 'ERR syntax error',
          'failed to recognize syntax error')
    # check type
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'set b hash', 'OK',
          'failed to change hash to str')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'set c list', 'OK',
          'failed to change list to str')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'set d set', 'OK',
          'failed to change set to str')
    cleanup(c)
    print('\tset cmd tests passed')

def test_get(c):
    # check gen
    check(c, 'set a hello', 'OK',
          'failed to set a to hello')
    check(c, 'get a', '"hello"',
          'failed to get value from str key')
    check(c, 'set a hi', 'OK',
          'failed to change value of existing str')
    check(c, 'get a', '"hi"',
          'failed to get value from updated str key')
    check(c, 'set a "hello world"', 'OK',
          'failed to set string with spaces')
    check(c, 'get a', '"hello world"',
          'failed to get str value with spaces')
    check(c, 'set a', 'OK',
          'failed to set value to empty str')
    check(c, 'get a', '""',
          'failed to get empty str')
    check(c, 'get b', '(nil)',
          'got something from non existing key')
    # check args
    check(c, 'get', 'ERR wrong number of arguments (given 0, expected 1)',
          'got something from non existing key')
    check(c, 'get a 1', 'ERR wrong number of arguments (given 2, expected 1)',
          'got something from non existing key')
    # check type
    check(c, 'del a', '(integer) 1',
          'failed to delete str key')
    check(c, 'get b', '(nil)',
          'got something from deleted key')
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'get b', 'ERR wrongtype operation',
          'got something from hash key')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'get c', 'ERR wrongtype operation',
          'got something from list key')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'get d', 'ERR wrongtype operation',
          'got something from set key')
    cleanup(c)
    print('\tget cmd tests passed')


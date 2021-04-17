from common import *

def test_hash_cmds(c):
    print('testing hash cmds')
    test_hset(c)
    test_hget(c)
    test_hdel(c)
    print('all hash cmds passed')

def test_hset(c):
    # check gen
    check(c, 'hset a 1 0', '(integer) 1',
          'failed to hset value to empty hash')
    check(c, 'hset a 1 2', '(integer) 1',
          'failed to change existing key from hash')
    check(c, 'hset a 2 3 3 4 4 5', '(integer) 3',
          'failed to hset multiple values to hash')
    # check args
    check(c, 'hset', 'ERR wrong number of arguments for \'hset\'',
          'failed to recognize syntax error')
    check(c, 'hset a', 'ERR wrong number of arguments for \'hset\'',
          'failed to recognize syntax error')
    check(c, 'hset a 1', 'ERR wrong number of arguments for \'hset\'',
          'failed to recognize syntax error')
    check(c, 'hset a 1 2 3', 'ERR wrong number of arguments for \'hset\'',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hset b 1 0', 'ERR wrongtype operation',
          'added keyval to key of wrong value (str)')
    check(c, 'hset c 1 0', 'ERR wrongtype operation',
          'added keyval to key of wrong value (list)')
    check(c, 'hset d 1 0', 'ERR wrongtype operation',
          'added keyval to key of wrong value (set)')
    cleanup(c)
    print('\thset cmd test passed')

def test_hget(c):
    # check gen
    check(c, 'hset a 1 2 2 3 3 4 4 5', '(integer) 4',
          'failed to hset multiple values to hash')
    check(c, 'hget a 1', '"2"',
          'failed to get value from hash')
    check(c, 'hget a 2', '"3"',
          'failed to get value from hash')
    check(c, 'hget a 5', '(nil)',
          'got value from non existing hash key')
    check(c, 'hdel a 1', '(integer) 1',
          'failed to delete key from hash')
    check(c, 'hget a 1', '(nil)',
          'got something from deleted hash key')
    check(c, 'del a', '(integer) 1',
          'failed to delete hash key')
    check(c, 'hget a 1', '(nil)',
          'got something from deleted hash')
    # check args
    check(c, 'hget', 'ERR wrong number of arguments (given 0, expected 2)',
          'failed to recognize syntax error')
    check(c, 'hget a', 'ERR wrong number of arguments (given 1, expected 2)',
          'failed to recognize syntax error')
    check(c, 'hget a 1 2',
          'ERR wrong number of arguments (given 3, expected 2)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hget b 1', 'ERR wrongtype operation',
          'got value from key of wrong type (str)')
    check(c, 'hget c 1', 'ERR wrongtype operation',
          'got value from key of wrong type (list)')
    check(c, 'hget d 1', 'ERR wrongtype operation',
          'got value from key of wrong type (set)')
    cleanup(c)
    print('\thget cmd test passed')

def test_hdel(c):
    # check gen
    check(c, 'hset a 1 2 2 3 3 4 4 5', '(integer) 4',
          'failed to hset multiple values to hash')
    check(c, 'hdel a 1', '(integer) 1',
          'failed to del key from hash')
    check(c, 'hdel a 0', '(integer) 0',
          'deleted non existing key from hash')
    check(c, 'hdel a 1', '(integer) 0',
          'deleted (deleted key) from hash')
    check(c, 'hdel a 2 3 4', '(integer) 3',
          'failed to delete multiple keys from hash')
    check(c, 'exists a', '(integer) 0',
          'failed to delete hash after deleting all keys')
    check(c, 'hdel a 1', '(integer) 0',
          'deleted something from deleted hash')
    check(c, 'hdel b 1', '(integer) 0',
          'deleted something from non existing hash')
    # check args
    check(c, 'hdel', 'ERR wrong number of arguments (given 0, expected 1+)',
          'failed to recognize syntax error')
    check(c, 'hdel a', 'ERR wrong number of arguments (given 1, expected 1+)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hdel b 1', 'ERR wrongtype operation',
          'got value from key of wrong type (str)')
    check(c, 'hdel c 1', 'ERR wrongtype operation',
          'got value from key of wrong type (list)')
    check(c, 'hdel d 1', 'ERR wrongtype operation',
          'got value from key of wrong type (set)')
    cleanup(c)
    print('\thdel cmd test passed')


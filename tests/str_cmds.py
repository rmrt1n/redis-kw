from common import *

def test_str_cmds(c):
    print('testing str cmds')
    test_set(c)
    test_get(c)
    test_mset(c)
    test_mget(c)
    test_incr(c)
    test_decr(c)
    test_incrby(c)
    test_decrby(c)
    test_strlen(c)
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
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'set b hash', 'OK',
          'failed to change hash to str')
    check(c, 'set c list', 'OK',
          'failed to change list to str')
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
    check(c, 'del a', '(integer) 1',
          'failed to delete str key')
    check(c, 'get b', '(nil)',
          'got something from deleted key')
    # check args
    check(c, 'get', 'ERR wrong number of arguments (given 0, expected 1)',
          'got something from non existing key')
    check(c, 'get a 1', 'ERR wrong number of arguments (given 2, expected 1)',
          'got something from non existing key')
    # check type
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset b 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'get b', 'ERR wrongtype operation',
          'got something from hash key')
    check(c, 'get c', 'ERR wrongtype operation',
          'got something from list key')
    check(c, 'get d', 'ERR wrongtype operation',
          'got something from set key')
    cleanup(c)
    print('\tget cmd tests passed')

def test_mset(c):
    # check gen
    check(c, 'mset a 1 b 2 c 3', 'OK',
          'failed to mset multiple keys')
    check(c, 'mset a 2 b 3 c 4', 'OK',
          'failed to change value of multiple keys')
    check(c, 'del a', '(integer) 1',
          'failed to del str key')
    check(c, 'mset a 3', 'OK',
          'failed to mset value of deleted str key')
    check(c, 'mset c "hellow sladkjflaksjflkasj"', 'OK',
          'failed to mset str with spaces')
    # check args
    check(c, 'mset', 'ERR wrong number of arguments for \'mset\'',
          'failed to recognize syntax error')
    # check type
    cleanup(c)
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset b 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'mset b hello', 'OK',
          'failed to mset hash to str')
    check(c, 'mset c hello', 'OK',
          'failed to mset list to str')
    check(c, 'mset d hello', 'OK',
          'failed to mset set to str')
    cleanup(c)
    print('\tmset cmd tests passed')

def test_mget(c):
    # check gen
    check(c, 'mset a 1 b 2 c 3', 'OK',
          'failed to mset multiple keys')
    checklines(c, 3, 'mget a b c',
               ['1) "1"', '2) "2"', '3) "3"'],
               'failed to mget values from multiple keys')
    checklines(c, 3, 'mget a b d',
               ['1) "1"', '2) "2"', '3) (nil)'],
               'failed to mget values from multiple keys & non existing key')
    check(c, 'mset a 2 b 3 c 4', 'OK',
          'failed to change value of multiple keys')
    checklines(c, 3, 'mget a b c',
               ['1) "2"', '2) "3"', '3) "4"'],
               'failed to mget values from changed keys')
    check(c, 'del a', '(integer) 1',
          'failed to del str key')
    checklines(c, 1, 'mget a', ['1) (nil)'],
               'got something from deleted keys')
    checklines(c, 1, 'mget d', ['1) (nil)'],
               'got something from non existing keys')
    check(c, 'mset a "hello world"', 'OK',
          'failed to mset value to str with spaces')
    checklines(c, 2, 'mget a c', ['1) "hello world"', '2) "4"'],
               'failed to get str with spaces')
    # check args
    check(c, 'mget', 'ERR wrong number of arguments for \'mget\'',
          'failed to recognize syntax error')
    # check type
    cleanup(c)
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    checklines(c, 3, 'mget b c d',
               ['1) (nil)', '2) (nil)', '3) (nil)'],
               'got something from different types')
    cleanup(c)
    print('\tmget cmd tests passed')

def test_incr(c):
    # check gen
    check(c, 'set a 1', 'OK',
          'failed to set a to 1')
    check(c, 'incr a', '(integer) 2',
          'failed to increment value')
    check(c, 'set a hello', 'OK',
          'failed to set a to hello')
    check(c, 'incr a', 'ERR value is not an integer or out of range',
          'incremented value that is not integer')
    check(c, 'set a -1', 'OK',
          'failed to set a to -1')
    check(c, 'incr a', '(integer) 0',
          'failed to increment negative values')
    # check args
    check(c, 'incr', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'incr a 1', 'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'incr b', 'ERR wrongtype operation',
          'incremented key of wrong type (hash)')
    check(c, 'incr c', 'ERR wrongtype operation',
          'incremented key of wrong type (list)')
    check(c, 'incr d', 'ERR wrongtype operation',
          'incremented key of wrong type (set)')
    cleanup(c)
    print('\tincr cmd tests passed')

def test_decr(c):
    # check gen
    check(c, 'set a 1', 'OK',
          'failed to set a to 1')
    check(c, 'decr a', '(integer) 0',
          'failed to decrement value')
    check(c, 'set a hello', 'OK',
          'failed to set a to hello')
    check(c, 'decr a', 'ERR value is not an integer or out of range',
          'incremented value that is not integer')
    check(c, 'set a -1', 'OK',
          'failed to set a to -1')
    check(c, 'decr a', '(integer) -2',
          'failed to decrement negative values')
    check(c, 'set a 0', 'OK',
          'failed to set a to -1')
    check(c, 'decr a', '(integer) -1',
          'failed to decrement from zero')
    # check args
    check(c, 'decr', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'decr a 1', 'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'decr b', 'ERR wrongtype operation',
          'incremented key of wrong type (hash)')
    check(c, 'decr c', 'ERR wrongtype operation',
          'incremented key of wrong type (list)')
    check(c, 'decr d', 'ERR wrongtype operation',
          'incremented key of wrong type (set)')
    cleanup(c)
    print('\tdecr cmd tests passed')

def test_incrby(c):
    # check gen
    check(c, 'set a 1', 'OK',
          'failed to set a to 1')
    check(c, 'incrby a 40', '(integer) 41',
          'failed to increment value')
    check(c, 'set a hello', 'OK',
          'failed to set a to hello')
    check(c, 'incr a', 'ERR value is not an integer or out of range',
          'incremented value that is not integer')
    check(c, 'set a -1', 'OK',
          'failed to set a to -1')
    check(c, 'incrby a 30', '(integer) 29',
          'failed to increment negative values')
    check(c, 'incrby a -2', '(integer) 27',
          'failed to increment by negative values')
    check(c, 'incrby a 0', '(integer) 27',
          'failed to increment by zero')
    # check args
    check(c, 'incrby', 'ERR wrong number of arguments (given 0, expected 2)',
          'failed to recognize syntax error')
    check(c, 'incrby a', 'ERR wrong number of arguments (given 1, expected 2)',
          'failed to recognize syntax error')
    check(c, 'incrby a 1 2',
          'ERR wrong number of arguments (given 3, expected 2)',
          'failed to recognize syntax error')
    # check type
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'incrby b 1', 'ERR wrongtype operation',
          'incremented key of wrong type (hash)')
    check(c, 'incrby c 1', 'ERR wrongtype operation',
          'incremented key of wrong type (list)')
    check(c, 'incrby d 1', 'ERR wrongtype operation',
          'incremented key of wrong type (set)')
    cleanup(c)
    print('\tincrby cmd tests passed')

def test_decrby(c):
    # check gen
    check(c, 'set a 1', 'OK',
          'failed to set a to 1')
    check(c, 'decrby a 40', '(integer) -39',
          'failed to decrement value')
    check(c, 'set a hello', 'OK',
          'failed to set a to hello')
    check(c, 'incr a', 'ERR value is not an integer or out of range',
          'decremented value that is not integer')
    check(c, 'set a -1', 'OK',
          'failed to set a to -1')
    check(c, 'decrby a 30', '(integer) -31',
          'failed to decrement negative values')
    check(c, 'decrby a -2', '(integer) -29',
          'failed to decrement by negative values')
    check(c, 'decrby a 0', '(integer) -29',
          'failed to decrement by zero')
    # check args
    check(c, 'decrby', 'ERR wrong number of arguments (given 0, expected 2)',
          'failed to recognize syntax error')
    check(c, 'decrby a', 'ERR wrong number of arguments (given 1, expected 2)',
          'failed to recognize syntax error')
    check(c, 'decrby a 1 2',
          'ERR wrong number of arguments (given 3, expected 2)',
          'failed to recognize syntax error')
    # check type
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'decrby b 1', 'ERR wrongtype operation',
          'incremented key of wrong type (hash)')
    check(c, 'decrby c 1', 'ERR wrongtype operation',
          'incremented key of wrong type (list)')
    check(c, 'decrby d 1', 'ERR wrongtype operation',
          'incremented key of wrong type (set)')
    cleanup(c)
    print('\tdecrby cmd tests passed')

def test_strlen(c):
    # check gen
    check(c, 'mset a hello b "hello world" c "" d 1', 'OK',
          'failed to set multiple values')
    check(c, 'strlen a', '(integer) 5',
          'failed to get length of string "hello"')
    check(c, 'strlen b', '(integer) 11',
          'failed to get length of string "hello world"')
    check(c, 'strlen c', '(integer) 0',
          'failed to get length of empty string')
    check(c, 'strlen d', '(integer) 1',
          'failed to get length of string "1"')
    check(c, 'del d', '(integer) 1',
          'failed to delete key')
    check(c, 'strlen d', '(integer) 0',
          'got something from deleted key')
    check(c, 'strlen e', '(integer) 0',
          'got something from non existing key')
    cleanup(c)
    # check args
    check(c, 'strlen', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'strlen a 1',
          'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'hset b 1 2', '(integer) 1',
          'failed to hset a 1 to 2')
    check(c, 'lpush c 1', '(integer) 1',
          'failed to push to list')
    check(c, 'sadd d 1', '(integer) 1',
          'failed to add 1 to set')
    check(c, 'strlen b', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'strlen c', 'ERR wrongtype operation',
          'got something from key of wrong type (list)')
    check(c, 'strlen d', 'ERR wrongtype operation',
          'got something from key of wrong type (set)')
    cleanup(c)
    print('\tstrlen cmd tests passed')

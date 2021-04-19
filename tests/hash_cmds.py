from common import *

def test_hash_cmds(c):
    print('testing hash cmds')
    test_hset(c)
    test_hget(c)
    test_hdel(c)
    test_hgetall(c)
    test_hexists(c)
    test_hkeys(c)
    test_hvals(c)
    test_hmget(c)
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

def test_hgetall(c):
    # check gen
    check(c, 'hset a 1 2 2 3 3 4 4 5', '(integer) 4',
          'failed to hset multiple values to hash')
    checklines(c, 4, 'hgetall a',
               ['1) "1"', '2) "2"', '3) "2"', '4) "3"',
                '5) "3"', '6) "4"', '7) "4"', '8) "5"'],
               'failed to getall keyvals from hash')
    check(c, 'hdel a 2 3 4', '(integer) 3',
          'failed to hdel multiple values to hash')
    checklines(c, 2, 'hgetall a',
               ['1) "1"', '2) "2"'],
               'failed to getall keyvals from hash with 1 key')
    check(c, 'del a', '(integer) 1',
          'failed to delete hash key')
    check(c, 'exists a', '(integer) 0',
          'got something from deleted key')
    check(c, 'hgetall a', '(empty list or set)',
          'got something from deleted hash key')
    check(c, 'hgetall b', '(empty list or set)',
          'got something from non existing hash key')
    # check args
    check(c, 'hgetall', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hgetall b', 'ERR wrongtype operation',
          'got value from key of wrong type (str)')
    check(c, 'hgetall c', 'ERR wrongtype operation',
          'got value from key of wrong type (list)')
    check(c, 'hgetall d', 'ERR wrongtype operation',
          'got value from key of wrong type (set)')
    cleanup(c)
    print('\thgetall cmd test passed')

def test_hexists(c):
    # check gen
    check(c, 'hset a 1 2 2 3 3 4 4 5', '(integer) 4',
          'failed to hset multiple values to hash')
    check(c, 'hexists a 1', '(integer) 1',
          'failed to recognize existing hash key')
    check(c, 'hset a 1 "hello world"', '(integer) 1',
          'failed to set existing hash key to str with spaces')
    check(c, 'hexists a 1', '(integer) 1',
          'failed to recognize changed hash key')
    check(c, 'hdel a 1', '(integer) 1',
          'failed to delete key from hash')
    check(c, 'hexists a 1', '(integer) 0',
          'recognized deleted key as existing')
    check(c, 'hexists a 5', '(integer) 0',
          'recognized non existing key as existing')
    # check args
    check(c, 'hexists', 'ERR wrong number of arguments (given 0, expected 2)',
          'failed to recognize syntax error')
    check(c, 'hexists a', 'ERR wrong number of arguments (given 1, expected 2)',
          'failed to recognize syntax error')
    check(c, 'hexists a 1 2',
          'ERR wrong number of arguments (given 3, expected 2)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hexists b 1', 'ERR wrongtype operation',
          'got value from key of wrong type (str)')
    check(c, 'hexists c 1', 'ERR wrongtype operation',
          'got value from key of wrong type (hash)')
    check(c, 'hexists d 1', 'ERR wrongtype operation',
          'got value from key of wrong type (set)')
    cleanup(c)
    print('\thexists cmd test passed')

def test_hkeys(c):
    # check gen
    check(c, 'hset a a 1 b 2 c 3 d 4', '(integer) 4',
          'failed to hset multiple value to empty hash')
    checklines(c, 4, 'hkeys a', ['1) "a"', '2) "b"', '3) "c"', '4) "d"'],
               'failed to get keys from hash')
    check(c, 'hdel a a', '(integer) 1',
          'failed to delete key from hash')
    checklines(c, 3, 'hkeys a', ['1) "b"', '2) "c"', '3) "d"'],
               'failed to get keys from hash with deleted key')
    check(c, 'hset a a "hello world"', '(integer) 1',
          'failed to set hash with str with spaces')
    checklines(c, 4, 'hkeys a', ['1) "b"', '2) "c"', '3) "d"', '4) "a"'],
               'failed to get keys from hash with deleted key')
    check(c, 'del a', '(integer) 1',
          'failed to delete hash key')
    check(c, 'hkeys a', '(empty list or set)',
          'got something from deleted hash')
    check(c, 'hkeys b', '(empty list or set)',
          'got something from non existing key')
    # check args
    check(c, 'hkeys', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'hkeys a 1', 'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hkeys b', 'ERR wrongtype operation',
          'got something from key of wrong type (str)')
    check(c, 'hkeys c', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'hkeys d', 'ERR wrongtype operation',
          'got something from key of wrong type (set)')
    cleanup(c)
    print('\thkeys cmd test passed')

def test_hvals(c):
    # check gen
    check(c, 'hset a a 1 b 2 c 3 d 4', '(integer) 4',
          'failed to hset multiple value to empty hash')
    checklines(c, 4, 'hvals a', ['1) "1"', '2) "2"', '3) "3"', '4) "4"'],
               'failed to get vals from hash')
    check(c, 'hdel a a', '(integer) 1',
          'failed to delete key from hash')
    checklines(c, 3, 'hvals a', ['1) "2"', '2) "3"', '3) "4"'],
               'failed to get vals from hash with deleted key')
    check(c, 'hset a a "hello world"', '(integer) 1',
          'failed to set hash with str with spaces')
    checklines(c, 4, 'hvals a',
               ['1) "2"', '2) "3"', '3) "4"', '4) "hello world"'],
               'failed to get vals from hash with deleted key')
    check(c, 'del a', '(integer) 1',
          'failed to delete hash key')
    check(c, 'hvals a', '(empty list or set)',
          'got something from deleted hash')
    check(c, 'hvals b', '(empty list or set)',
          'got something from non existing key')
    # check args
    check(c, 'hvals', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'hvals a 1', 'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hvals b', 'ERR wrongtype operation',
          'got something from key of wrong type (str)')
    check(c, 'hvals c', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'hvals d', 'ERR wrongtype operation',
          'got something from key of wrong type (set)')
    cleanup(c)
    print('\thvals cmd test passed')

def test_hmget(c):
    # check gen
    check(c, 'hset a a 1 b 2 c 3 d 4 e 5', '(integer) 5',
          'failed to hset multiple values to empty hash')
    checklines(c, 1, 'hmget a a', ['1) "1"'],
               'failed to get 1 value from hash key')
    checklines(c, 1, 'hmget a a b c', ['1) "1"', '2) "2"', '3) "3"'],
               'failed to get multiple values from hash')
    checklines(c, 1, 'hmget a a',
               ['1) "1"', '2) "2"', '3) "3"', '4) "4"', '5) "5"'],
               'failed to get all values from hash')
    check(c, 'hset a a 2 b 3 c 4', '(integer) 3',
          'failed to change multiple values from hash')
    checklines(c, 1, 'hmget a a b c', ['1) "2"', '2) "3"', '3) "4"'],
               'failed to get multiple values from hash with changed values')
    # check args
    check(c, 'hmget', 'ERR wrong number of arguments (given 0, expected 1+)',
          'failed to recognize syntax error')
    check(c, 'hmget a', 'ERR wrong number of arguments (given 1, expected 1+)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush c 1 2', '(integer) 2',
          'failed to push c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'hmget b 1', 'ERR wrongtype operation',
          'got something from key of wrong type (str)')
    check(c, 'hmget c 1', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'hmget d 1', 'ERR wrongtype operation',
          'got something from key of wrong type (set)')
    cleanup(c)
    print('\thmget cmd test passed')


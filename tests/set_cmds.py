from common import *

def test_set_cmds(c):
    print('testing set cmds')
    test_sadd(c)
    test_srem(c)
    test_sismember(c)
    test_smembers(c)
    print('all set cmds passed')

def test_sadd(c):
    # check gen
    check(c, 'sadd a 1', '(integer) 1',
          'failed to add value to empty set')
    check(c, 'sadd a 2 3 4', '(integer) 3',
          'failed to add multiple values to existing set')
    check(c, 'sadd a 1', '(integer) 0',
          'added existing set member')
    # check args
    check(c, 'sadd', 'ERR wrong number of arguments for \'sadd\'',
          'failed to recognize syntax error')
    check(c, 'sadd a', 'ERR wrong number of arguments for \'sadd\'',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'lpush d 1 2', '(integer) 2',
          'failed to lpush 2 values')
    check(c, 'sadd b 1', 'ERR wrongtype operation',
          'added to key of wrong type (str)')
    check(c, 'sadd c 1', 'ERR wrongtype operation',
          'added to key of wrong type (hash)')
    check(c, 'sadd d 1', 'ERR wrongtype operation',
          'added to key of wrong type (list)')
    cleanup(c)
    print('\tsadd cmd test passed')

def test_srem(c):
    # check gen
    check(c, 'sadd a 1 2 3 4 5', '(integer) 5',
          'failed to add value to empty set')
    check(c, 'srem a 1', '(integer) 1',
          'failed to remove value from set')
    check(c, 'srem a 1', '(integer) 0',
          'removed (removed value)')
    check(c, 'srem a 6', '(integer) 0',
          'removed non existing set member')
    check(c, 'srem a 2 3 4 5', '(integer) 4',
          'failed to remove multiple values from set')
    check(c, 'exists a', '(integer) 0',
          'failed to remove key after removing all set members')
    check(c, 'srem a 1', '(integer) 0',
          'removed something from deleted set')
    # check args
    check(c, 'srem', 'ERR wrong number of arguments for \'srem\'',
          'failed to recognize syntax error')
    check(c, 'srem a', 'ERR wrong number of arguments for \'srem\'',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'lpush d 1 2', '(integer) 2',
          'failed to lpush 2 values')
    check(c, 'sadd b 1', 'ERR wrongtype operation',
          'added to key of wrong type (str)')
    check(c, 'sadd c 1', 'ERR wrongtype operation',
          'added to key of wrong type (hash)')
    check(c, 'sadd d 1', 'ERR wrongtype operation',
          'added to key of wrong type (list)')
    cleanup(c)
    print('\tsrem cmd test passed')

def test_sismember(c):
    # check gen
    check(c, 'sadd a 1 2 3 4 5', '(integer) 5',
          'failed to add value to empty set')
    check(c, 'sismember a 1', '(integer) 1',
          'failed to recognize set member')
    check(c, 'sismember a 0', '(integer) 0',
          'failed to recognize non set member')
    # check args
    check(c, 'sismember a',
          'ERR wrong number of arguments (given 1, expected 2)',
          'failed to recognize non set member')
    check(c, 'sismember a 1 2',
          'ERR wrong number of arguments (given 3, expected 2)',
          'failed to recognize non set member')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'lpush d 1 2', '(integer) 2',
          'failed to lpush 2 values')
    check(c, 'sismember b 1', 'ERR wrongtype operation',
          'got something from key of wrong type (str)')
    check(c, 'sismember c 1', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'sismember d 1', 'ERR wrongtype operation',
          'got something from key of wrong type (list)')
    cleanup(c)
    print('\tsismember cmd test passed')

def test_smembers(c):
    # check gen
    check(c, 'sadd a 1 2 3 4 5', '(integer) 5',
          'failed to add value to empty set')
    checklines(c, 5, 'smembers a',
               ['1) "1"', '2) "2"', '3) "3"', '4) "4"', '5) "5"'],
               'failed to get all members from set')
    check(c, 'sadd a "hello world"', '(integer) 1',
          'failed to add str with spaces to existing set')
    checklines(c, 6, 'smembers a',
               ['1) "1"', '2) "2"', '3) "3"',
                '4) "4"', '5) "5"', '6) "hello world"'],
               'failed to get all members from updated set')
    check(c, 'srem a 1 2 3 4 5', '(integer) 5',
          'failed to remove values from set')
    checklines(c, 1, 'smembers a', ['1) "hello world"'],
          'failed to get members from set with deleted members')
    check(c, 'srem a "hello world"', '(integer) 1',
          'failed to remove value from set')
    check(c, 'exists a', '(integer) 0',
          'deleting from set doesnt delete key')
    check(c, 'smembers a', '(empty list or set)',
          'got something from deleted set')
    check(c, 'smembers b', '(empty list or set)',
          'got something from non existing value')
    # check args
    check(c, 'smembers', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'lpush d 1 2', '(integer) 2',
          'failed to lpush 2 values')
    check(c, 'smembers b', 'ERR wrongtype operation',
          'got something from key of wrong type (str)')
    check(c, 'smembers c', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'smembers d', 'ERR wrongtype operation',
          'got something from key of wrong type (list)')
    cleanup(c)
    print('\tsmembers cmd test passed')


from common import *

def test_list_cmds(c):
    print('testing list cmds')
    test_lpush(c)
    test_rpush(c)
    test_lpop(c)
    test_rpop(c)
    print('all list tests passed')

def test_lpush(c):
    # check gen
    check(c, 'lpush a 1', '(integer) 1',
          'failed to init new list key')
    check(c, 'lpush a 2 1', '(integer) 2',
          'failed to lpush multiple values to existing list key')
    # check args
    check(c, 'lpush a', 'ERR wrong number of arguments for \'lpush\'',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'lpush b 1', 'ERR wrongtype operation',
          'lpushed value to wrongtype key (str)')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'lpush c 2', 'ERR wrongtype operation',
          'lpushed value to wrongtype key (hash)')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'lpush d 2 2', 'ERR wrongtype operation',
          'lpushed value to wrongtype key (set)')
    cleanup(c)
    print('\tlpush cmd test passed')

def test_rpush(c):
    # check gen
    check(c, 'rpush a 1', '(integer) 1',
          'failed to init new list key')
    check(c, 'rpush a 2 1', '(integer) 2',
          'failed to rpush multiple values to existing list key')
    # check args
    check(c, 'rpush a', 'ERR wrong number of arguments for \'rpush\'',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'rpush b 1', 'ERR wrongtype operation',
          'rpushed value to wrongtype key (str)')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'rpush c 2', 'ERR wrongtype operation',
          'rpushed value to wrongtype key (hash)')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'rpush d 2 2', 'ERR wrongtype operation',
          'rpushed value to wrongtype key (set)')
    cleanup(c)
    print('\trpush cmd test passed')

def test_lpop(c):
    # check gen
    check(c, 'lpush a 1 2 3', '(integer) 3',
          'failed to push values to list')
    check(c, 'lpop a', '"3"',
          'failed to pop correct value from list')
    check(c, 'lpush a 3', '(integer) 1',
          'failed to push value to list')
    check(c, 'lpop a', '"3"',
          'failed to pop correct value from list')
    check(c, 'lpop a', '"2"',
          'failed to pop correct value from list')
    check(c, 'lpop a', '"1"',
          'failed to pop correct value from list')
    check(c, 'lpop a', '(nil)',
          'got something from empty list')
    check(c, 'exists a', '(integer) 0',
          'failed to delete list key by popping last value')
    # check args
    check(c, 'lpop', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'lpop a 2', 'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset c 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'lpop b', 'ERR wrongtype operation',
          'popped from key of wrong type (str)')
    check(c, 'lpop c', 'ERR wrongtype operation',
          'popped from key of wrong type (hash)')
    check(c, 'lpop d', 'ERR wrongtype operation',
          'popped from key of wrong type (set)')
    cleanup(c)
    print('\tlpop cmd test passed')

def test_rpop(c):
    # check gen
    check(c, 'rpush a 1 2 3', '(integer) 3',
          'failed to push values to list')
    check(c, 'rpop a', '"3"',
          'failed to pop correct value from list')
    check(c, 'rpush a 3', '(integer) 1',
          'failed to push value to list')
    check(c, 'rpop a', '"3"',
          'failed to pop correct value from list')
    check(c, 'rpop a', '"2"',
          'failed to pop correct value from list')
    check(c, 'rpop a', '"1"',
          'failed to pop correct value from list')
    check(c, 'rpop a', '(nil)',
          'got something from empty list')
    check(c, 'exists a', '(integer) 0',
          'failed to delete list key by popping last value')
    # check args
    check(c, 'rpop', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'rpop a 2', 'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'rpop b', 'ERR wrongtype operation',
          'popped from key of wrong type (str)')
    check(c, 'rpop c', 'ERR wrongtype operation',
          'popped from key of wrong type (hash)')
    check(c, 'rpop d', 'ERR wrongtype operation',
          'popped from key of wrong type (set)')
    cleanup(c)
    print('\trpop cmd test passed')


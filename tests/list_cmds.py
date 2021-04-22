from common import *

def test_list_cmds(c):
    print('testing list cmds')
    test_lpush(c)
    test_rpush(c)
    test_lpop(c)
    test_rpop(c)
    test_llen(c)
    test_lindex(c)
    test_lset(c)
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
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'lpush b 1', 'ERR wrongtype operation',
          'lpushed value to wrongtype key (str)')
    check(c, 'lpush c 2', 'ERR wrongtype operation',
          'lpushed value to wrongtype key (hash)')
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

def test_llen(c):
    # check gen
    check(c, 'lpush a 1 2 3 4 5', '(integer) 5',
          'failed to lpush values to list')
    check(c, 'llen a', '(integer) 5',
          'failed to get correct list length')
    check(c, 'rpush a "hello world"', '(integer) 1',
          'failed to rpush spaced string to list')
    check(c, 'llen a', '(integer) 6',
          'failed to get correct list length')
    check(c, 'lpop a', '"5"',
          'failed to lpop value from list')
    check(c, 'llen a', '(integer) 5',
          'failed to get correct list length')
    check(c, 'del a', '(integer) 1',
          'failed to delete list key')
    check(c, 'llen a', '(integer) 0',
          'got something from deleted list')
    check(c, 'llen b', '(integer) 0',
          'got something from non existing list');
    # check args
    check(c, 'llen', 'ERR wrong number of arguments (given 0, expected 1)',
          'failed to recognize syntax error')
    check(c, 'llen a 2', 'ERR wrong number of arguments (given 2, expected 1)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'llen b', 'ERR wrongtype operation',
          'got length of key of wrong type (str)')
    check(c, 'llen c', 'ERR wrongtype operation',
          'got length of key of wrong type (hash)')
    check(c, 'llen d', 'ERR wrongtype operation',
          'got length of key of wrong type (set)')
    cleanup(c)
    print('\tllen cmd test passed')

def test_lindex(c):
    # check gen
    check(c, 'lpush a 1 2 3 4 5', '(integer) 5',
          'failed to init new list key')
    check(c, 'lindex a 0', '"5"',
          'failed to get item from index 0 from list')
    check(c, 'lindex a 2', '"3"',
          'failed to get item from index 2 from list')
    check(c, 'lindex a 4', '"1"',
          'failed to get item from index 4 from list')
    check(c, 'lindex a -1', '"1"',
          'failed to get item from negative index -1 from list')
    check(c, 'lindex a -3', '"3"',
          'failed to get item from negative index -3 from list')
    check(c, 'lindex a 9', '(nil)',
          'got something from out of range index')
    check(c, 'del a', '(integer) 1',
          'failed to delete list')
    check(c, 'lindex a 1', '(nil)',
          'got something from deleted key');
    check(c, 'lindex b 1', '(nil)',
          'got something from non existing key')
    # check args
    check(c, 'lindex', 'ERR wrong number of arguments (given 0, expected 2)',
          'failed to recognize syntax error')
    check(c, 'lindex a', 'ERR wrong number of arguments (given 1, expected 2)',
          'failed to recognize syntax error')
    check(c, 'lindex a 1 2',
          'ERR wrong number of arguments (given 3, expected 2)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'lindex b 1', 'ERR wrongtype operation',
          'got something from key of wrong type (str)')
    check(c, 'lindex c 1', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'lindex d 1', 'ERR wrongtype operation',
          'got something from key of wrong type (set)')
    cleanup(c)
    print('\tlindex cmd test passed')

def test_lrange(c):
    # check gen
    check(c, 'lpush a 5 4 3 2 1', '(integer) 5',
          'failed to init new list key')
    checklines(c, 1, 'lrange a 0 0', ['1) "1"'],
          'failed to get first item of list')
    checklines(c, 3, 'lrange a 0 2', ['1) "1"', '2) "2"', '3) "3"'],
          'failed to get range of items')
    checklines(c, 5, 'lrange a 0 -1',
          ['1) "1"', '2) "2"', '3) "3"', '4) "4"', '5) "5"'],
          'failed to get range with a negative index')
    checklines(c, 2, 'lrange a -2 -1', ['1) "4"', '2) "5"'],
          'failed to get range with 2 negative indexes')
    checklines(c, 1, 'lrange a 0 9',
          ['ERR index out of range or not integer'],
          'got something from out of bound index')
    checklines(c, 1, 'lrange a 0 -8',
          ['ERR index out of range or not integer'],
          'got something from out of bound index')
    checklines(c, 1, 'lrange a 0 a',
          ['ERR index out of range or not integer'],
          'got something from not integer index')
    # check args
    check(c, 'lrange', 'ERR wrong number of arguments (given 0, expected 3)',
          'failed to recognize syntax error')
    check(c, 'lrange a', 'ERR wrong number of arguments (given 1, expected 3)',
          'failed to recognize syntax error')
    check(c, 'lrange a 1',
          'ERR wrong number of arguments (given 2, expected 3)',
          'failed to recognize syntax error')
    check(c, 'lrange a 1 2 3',
          'ERR wrong number of arguments (given 4, expected 3)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'lrange b 1 2', 'ERR wrongtype operation',
          'got something from key of wrong type (str)')
    check(c, 'lrange c 1 2', 'ERR wrongtype operation',
          'got something from key of wrong type (hash)')
    check(c, 'lrange d 1 2', 'ERR wrongtype operation',
          'got something from key of wrong type (set)')
    cleanup(c)
    print('\tlrange cmd test passed')

def test_lset(c):
    # check gen
    check(c, 'lpush a 1 2 3 4 5', '(integer) 5',
          'failed to init new list key')
    check(c, 'lset a 0 hello', 'OK',
          'failed to set 1 to hello')
    check(c, 'lindex a 0', '"hello"',
          'failed to set 1 to hello')
    check(c, 'lset a -1 world', 'OK',
          'failed to set with negative index')
    check(c, 'lindex a -1', '"world"',
          'failed to set with negative index')
    check(c, 'lset a 2 "hi there"', 'OK',
          'failed to set value of str with spaces')
    check(c, 'lindex a 2', '"hi there"',
          'failed to set value of str with spaces')
    # check args
    check(c, 'lset', 'ERR wrong number of arguments (given 0, expected 3)',
          'failed to recognize syntax error')
    check(c, 'lset a', 'ERR wrong number of arguments (given 1, expected 3)',
          'failed to recognize syntax error')
    check(c, 'lset a 1', 'ERR wrong number of arguments (given 2, expected 3)',
          'failed to recognize syntax error')
    check(c, 'lset a 1 2 3',
          'ERR wrong number of arguments (given 4, expected 3)',
          'failed to recognize syntax error')
    # check type
    check(c, 'set b hello', 'OK',
          'failed to set b to hello')
    check(c, 'hset c 1 2 3 4', '(integer) 2',
          'failed to hset b 1 to 2, 3 to 4')
    check(c, 'sadd d 1 2 3', '(integer) 3',
          'failed to add to set d, 1 2 3')
    check(c, 'lset b 0 hello', 'ERR wrongtype operation',
          'set to key of wrong type (str)')
    check(c, 'lset c 0 hello', 'ERR wrongtype operation',
          'set to key of wrong type (hash)')
    check(c, 'lset d 0 hello', 'ERR wrongtype operation',
          'set to key of wrong type (set)')
    cleanup(c)
    print('\tlset cmd test passed')


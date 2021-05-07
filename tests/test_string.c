#include <string.h>
#include "../src/common.h"
#include "miniunit.h"
#include "test.h"

static void test_set(HashTable *ht) {
    test_case("test set", {
        // test gen
        expect("empty set", compare(ht, "set", "$2\r\nOK\r\n"));
        expect("set a to ''", compare(ht, "set a", "$2\r\nOK\r\n"));
        expect("set a to hello", compare(ht, "set a hello", "$2\r\nOK\r\n"));
        // test argc
        expect("set argc err", compare(ht, "set a b c",
               "-ERR wrong number of arguments (given 3, expected 0..2)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("set hash", compare(ht, "set b hello", "$2\r\nOK\r\n"));
        expect("set list", compare(ht, "set c hello", "$2\r\nOK\r\n"));
        expect("set set", compare(ht, "set d hello", "$2\r\nOK\r\n"));
    });
    cleanup(ht);
}

static void test_get(HashTable *ht) {
    test_case("test get", {
        // test gen
        expect("set a to ''", compare(ht, "set a", "$2\r\nOK\r\n"));
        expect("get a", compare(ht, "get a", "$0\r\n\r\n"));
        expect("set a to hello", compare(ht, "set a hello", "$2\r\nOK\r\n"));
        expect("get a", compare(ht, "get a", "$5\r\nhello\r\n"));
        expect("set a 'h w'", compare(ht, "set a 'hello world'", "$2\r\nOK\r\n"));
        expect("get a", compare(ht, "get a", "$11\r\nhello world\r\n"));
        expect("get non existing key", compare(ht, "get b", "$-1\r\n"));
        // test argc
        expect("empty get", compare(ht, "get",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("get err argc", compare(ht, "get a b",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("get hash err", compare(ht, "get b",
               "-ERR wrongtype operation\r\n"));
        expect("get list err", compare(ht, "get c",
               "-ERR wrongtype operation\r\n"));
        expect("get set err", compare(ht, "get d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_mset(HashTable *ht) {
    test_case("test mset", {
        // test gen
        expect("mset 1 keyval", compare(ht, "mset a 1", "$2\r\nOK\r\n"));
        expect("get a val", compare(ht, "get a", "$1\r\n1\r\n"));
        expect("mset 2 keyval", compare(ht, "mset b 2 c 3", "$2\r\nOK\r\n"));
        expect("get b val", compare(ht, "get b", "$1\r\n2\r\n"));
        expect("get c val", compare(ht, "get c", "$1\r\n3\r\n"));
        expect("mset 1 ex & 1 nex", compare(ht, "mset a 2 d 4", "$2\r\nOK\r\n"));
        expect("get a val", compare(ht, "get a", "$1\r\n2\r\n"));
        cleanup(ht);
        // test argc
        expect("empty mset", compare(ht, "mset",
               "-ERR wrong number of arguments (given 0, expected 2+)\r\n"));
        expect("get err argc", compare(ht, "mset 1",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        expect("get err argc", compare(ht, "mset 1 2 3",
               "-ERR wrong number of arguments (given 3, expected 2+)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("mset hash", compare(ht, "mset b hello", "$2\r\nOK\r\n"));
        expect("mset list", compare(ht, "mset c hello", "$2\r\nOK\r\n"));
        expect("mset set", compare(ht, "mset d hello", "$2\r\nOK\r\n"));
    });
    cleanup(ht);
}

void test_mget(HashTable *ht) {
    test_case("test mget", {
        // test gen
        expect("mset keyvals", compare(ht, "mset a 1 b 2 c 3", "$2\r\nOK\r\n"));
        expect("mget 1 keyval", compare(ht, "mget a", "*1\r\n:1\r\n"));
        expect("mget keyvals", compare(ht, "mget b c", "*2\r\n:2\r\n:3\r\n"));
        expect("mget ex & nex", compare(ht, "mget a d", "*2\r\n:1\r\n$-1\r\n"));
        cleanup(ht);
        // test argc
        expect("empty mget", compare(ht, "mget",
               "-ERR wrong number of arguments (given 0, expected 1+)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("mget hash", compare(ht, "mset b hello", "$2\r\nOK\r\n"));
        expect("mget list", compare(ht, "mset c hello", "$2\r\nOK\r\n"));
        expect("mget set", compare(ht, "mset d hello", "$2\r\nOK\r\n"));
    });
    cleanup(ht);
}

void test_strlen(HashTable *ht) {
    test_case("test strlen", {
        // test gen
        expect("mset multiple vals",
               compare(ht, "mset a 1 b hello c 'hello world'", "$2\r\nOK\r\n"));
        expect("strlen a", compare(ht, "strlen a", ":1\r\n"));
        expect("strlen b", compare(ht, "strlen b", ":5\r\n"));
        expect("strlen c", compare(ht, "strlen c", ":11\r\n"));
        expect("strlen non existing key", compare(ht, "strlen d", ":0\r\n"));
        cleanup(ht);
        // test argc
        expect("empty strlen", compare(ht, "strlen",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("strlen err argc", compare(ht, "strlen 1 2",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("strlen hash", compare(ht, "strlen b",
               "-ERR wrongtype operation\r\n"));
        expect("strlen list", compare(ht, "strlen c",
               "-ERR wrongtype operation\r\n"));
        expect("strlen set", compare(ht, "strlen d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_incr(HashTable *ht) {
    test_case("test incr", {
        // test gen
        expect("mset new keys",
               compare(ht, "mset a 1 b 0 c -1 d hello", "$2\r\nOK\r\n"));
        expect("incr pos int", compare(ht, "incr a", ":2\r\n"));
        expect("incr zero", compare(ht, "incr b", ":1\r\n"));
        expect("incr neg int", compare(ht, "incr c", ":0\r\n"));
        expect("incr str", compare(ht, "incr d",
               "-ERR value is not an integer or out of range\r\n"));
        cleanup(ht);
        // test argc
        expect("empty incr", compare(ht, "incr",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("incr err argc", compare(ht, "incr a 1",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("incr hash", compare(ht, "incr b",
               "-ERR wrongtype operation\r\n"));
        expect("incr list", compare(ht, "incr c",
               "-ERR wrongtype operation\r\n"));
        expect("incr set", compare(ht, "incr d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_decr(HashTable *ht) {
    test_case("test decr", {
        // test gen
        expect("mset new keys",
               compare(ht, "mset a 1 b 0 c -1 d hello", "$2\r\nOK\r\n"));
        expect("decr pos int", compare(ht, "decr a", ":0\r\n"));
        expect("decr zero", compare(ht, "decr b", ":-1\r\n"));
        expect("decr neg int", compare(ht, "decr c", ":-2\r\n"));
        expect("decr str", compare(ht, "decr d",
               "-ERR value is not an integer or out of range\r\n"));
        cleanup(ht);
        // test argc
        expect("empty decr", compare(ht, "decr",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("decr err argc", compare(ht, "decr a 1",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("decr hash", compare(ht, "decr b", "-ERR wrongtype operation\r\n"));
        expect("decr list", compare(ht, "decr c",
               "-ERR wrongtype operation\r\n"));
        expect("decr set", compare(ht, "decr d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_incrby(HashTable *ht) {
    test_case("test incrby", {
        // test gen
        expect("mset new keys",
               compare(ht, "mset a 1 b 0 c -1 d hello", "$2\r\nOK\r\n"));
        expect("incrby pos int", compare(ht, "incrby a 10", ":11\r\n"));
        expect("incrby zero", compare(ht, "incrby b 2", ":2\r\n"));
        expect("incrby neg int", compare(ht, "incrby c 11", ":10\r\n"));
        expect("incrby neg operand", compare(ht, "incrby a -11", ":0\r\n"));
        expect("incrby str", compare(ht, "incrby d 1",
               "-ERR value is not an integer or out of range\r\n"));
        expect("incrby str operand", compare(ht, "incrby a hello",
               "-ERR value is not an integer or out of range\r\n"));
        cleanup(ht);
        // test argc
        expect("empty incrby", compare(ht, "incrby",
               "-ERR wrong number of arguments (given 0, expected 2)\r\n"));
        expect("incrby err argc", compare(ht, "incrby a",
               "-ERR wrong number of arguments (given 1, expected 2)\r\n"));
        expect("incrby err argc", compare(ht, "incrby a 1 2",
               "-ERR wrong number of arguments (given 3, expected 2)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("incrby hash", compare(ht, "incrby b 1",
               "-ERR wrongtype operation\r\n"));
        expect("incrby list", compare(ht, "incrby c 1",
               "-ERR wrongtype operation\r\n"));
        expect("incrby set", compare(ht, "incrby d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_decrby(HashTable *ht) {
    test_case("test decrby", {
        // test gen
        expect("mset new keys",
               compare(ht, "mset a 1 b 0 c -1 d hello", "$2\r\nOK\r\n"));
        expect("decrby pos int", compare(ht, "decrby a 10", ":-9\r\n"));
        expect("decrby zero", compare(ht, "decrby b 2", ":-2\r\n"));
        expect("decrby neg int", compare(ht, "decrby c 11", ":-12\r\n"));
        expect("decrby neg operand", compare(ht, "decrby a -11", ":2\r\n"));
        expect("decrby str", compare(ht, "decrby d 1",
               "-ERR value is not an integer or out of range\r\n"));
        expect("decrby str operand", compare(ht, "decrby a hello",
               "-ERR value is not an integer or out of range\r\n"));
        cleanup(ht);
        // test argc
        expect("empty decrby", compare(ht, "decrby",
               "-ERR wrong number of arguments (given 0, expected 2)\r\n"));
        expect("decrby err argc", compare(ht, "decrby a",
               "-ERR wrong number of arguments (given 1, expected 2)\r\n"));
        expect("decrby err argc", compare(ht, "decrby a 1 2",
               "-ERR wrong number of arguments (given 3, expected 2)\r\n"));
        // test type
        expect("hset b", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("decrby hash", compare(ht, "decrby b 1",
               "-ERR wrongtype operation\r\n"));
        expect("decrby list", compare(ht, "decrby c 1",
               "-ERR wrongtype operation\r\n"));
        expect("decrby set", compare(ht, "decrby d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_interpret_str(HashTable *ht) {
    test_set(ht);
    test_get(ht);
    test_mset(ht);
    test_mget(ht);
    test_strlen(ht);
    test_incr(ht);
    test_decr(ht);
    test_incrby(ht);
    test_decrby(ht);
}


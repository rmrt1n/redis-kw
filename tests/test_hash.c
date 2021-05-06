#include <string.h>
#include "../src/common.h"
#include "miniunit.h"
#include "test.h"

static void test_hset(HashTable *ht) {
    test_case("test hset", {
        // test gen
        expect("hset a 1 0", compare(ht, "hset a 1 0", ":1\r\n"));
        expect("hset a 1 2", compare(ht, "hset a 1 2", ":0\r\n"));
        expect("hset a 2 3 3 4", compare(ht, "hset a 2 3 3 4", ":2\r\n"));
        // test argc
        expect("empty hset", compare(ht, "hset",
               "-ERR wrong number of arguments (given 0, expected 3+)\r\n"));
        expect("hset err argc", compare(ht, "hset 1",
               "-ERR wrong number of arguments (given 1, expected 3+)\r\n"));
        expect("hset err argc", compare(ht, "hset 1 2 3 4",
               "-ERR wrong number of arguments (given 4, expected 3+)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hset str", compare(ht, "hset b 1 2",
               "-ERR wrongtype operation\r\n"));
        expect("hset list", compare(ht, "hset c 1 2",
               "-ERR wrongtype operation\r\n"));
        expect("hset set", compare(ht, "hset d 1 2",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

static void test_hget(HashTable *ht) {
    test_case("test hget", {
        // test gen
        expect("hset a 1 2", compare(ht, "hset a 1 2", ":1\r\n"));
        expect("hget a 1 -> 2", compare(ht, "hget a 1", "$1\r\n2\r\n"));
        expect("hset a 1 ''", compare(ht, "hset a 1 ''", ":0\r\n"));
        expect("hget a 1 -> ''", compare(ht, "hget a 1", "$0\r\n\r\n"));
        expect("hget non existing key", compare(ht, "hget b 1", "$-1\r\n"));
        // test argc
        expect("empty hget", compare(ht, "hget",
               "-ERR wrong number of arguments (given 0, expected 2)\r\n"));
        expect("hget err argc", compare(ht, "hget a",
               "-ERR wrong number of arguments (given 1, expected 2)\r\n"));
        expect("hget err argc", compare(ht, "hget a 1 2",
               "-ERR wrong number of arguments (given 3, expected 2)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hget str", compare(ht, "hget b 1",
               "-ERR wrongtype operation\r\n"));
        expect("hget list", compare(ht, "hget c 1",
               "-ERR wrongtype operation\r\n"));
        expect("hget set", compare(ht, "hget d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

static void test_hdel(HashTable *ht) {
    test_case("test hdel", {
        // test gen
        expect("hset a 1 2 3 4 5 6", compare(ht, "hset a 1 2 3 4 5 6", ":3\r\n"));
        expect("hdel existing key", compare(ht, "hdel a 1", ":1\r\n"));
        expect("hdel non existing key", compare(ht, "hdel a 7", ":0\r\n"));
        expect("hdel ex & nex key", compare(ht, "hdel a 3 5 0", ":2\r\n"));
        expect("a no longer exists", compare(ht, "exists a", ":0\r\n"));
        // test argc
        expect("empty hdel", compare(ht, "hdel",
               "-ERR wrong number of arguments (given 0, expected 2+)\r\n"));
        expect("empty hdel", compare(ht, "hdel a",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hdel str", compare(ht, "hdel b 1",
               "-ERR wrongtype operation\r\n"));
        expect("hdel list", compare(ht, "hdel c 1",
               "-ERR wrongtype operation\r\n"));
        expect("hdel set", compare(ht, "hdel d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}
void test_interpret_hash(HashTable *ht) {
    test_hset(ht);
    test_hget(ht);
    test_hdel(ht);
}


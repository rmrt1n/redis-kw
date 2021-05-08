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

void test_hgetall(HashTable *ht) {
    test_case("test hgetall", {
        // test gen
        expect("hset new hash", compare(ht, "hset a 1 2 3 4 5 6", ":3\r\n"));
        // hgetall not ordered by insertion
        expect("hgetall a", compare(ht, "hgetall a",
               "*6\r\n:5\r\n:6\r\n:1\r\n:2\r\n:3\r\n:4\r\n"));
        expect("change value in hash", compare(ht, "hset a 1 hello", ":0\r\n"));
        expect("hgetall a", compare(ht, "hgetall a",
               "*6\r\n:5\r\n:6\r\n:1\r\n$5\r\nhello\r\n:3\r\n:4\r\n"));
        expect("hgetall b", compare(ht, "hgetall b", "*0\r\n"));
        // test argc
        expect("empty hgetall", compare(ht, "hgetall",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("hgetall err argc", compare(ht, "hgetall 1 2",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hgetall str", compare(ht, "hgetall b",
               "-ERR wrongtype operation\r\n"));
        expect("hgetall list", compare(ht, "hgetall c",
               "-ERR wrongtype operation\r\n"));
        expect("hgetall set", compare(ht, "hgetall d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hlen(HashTable *ht) {
    test_case("test hlen", {
        // test gen
        expect("hset new hash", compare(ht, "hset a 1 2 3 4 5 6", ":3\r\n"));
        expect("hlen a", compare(ht, "hlen a", ":3\r\n"));
        expect("added keyvals to a", compare(ht, "hset a 7 8 9 0", ":2\r\n"));
        expect("hlen a", compare(ht, "hlen a", ":5\r\n"));
        expect("delete keyval from a", compare(ht, "hdel a 1", ":1\r\n"));
        expect("hlen a", compare(ht, "hlen a", ":4\r\n"));
        expect("hlen non existing hash", compare(ht, "hlen b", ":0\r\n"));
        // test argc
        expect("empty hgetall", compare(ht, "hgetall",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("hgetall err argc", compare(ht, "hgetall 1 2",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hlen str", compare(ht, "hlen b",
               "-ERR wrongtype operation\r\n"));
        expect("hlen list", compare(ht, "hlen c",
               "-ERR wrongtype operation\r\n"));
        expect("hlen set", compare(ht, "hlen d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hexists(HashTable *ht) {
    test_case("test hexists", {
        // test gen
        expect("hset new hash", compare(ht, "hset a 1 2 3 4 5 6", ":3\r\n"));
        expect("hexists 1", compare(ht, "hexists a 1", ":1\r\n"));
        expect("hexists 2", compare(ht, "hexists a 3", ":1\r\n"));
        expect("hexists 0", compare(ht, "hexists a 0", ":0\r\n"));
        expect("hexists non exists", compare(ht, "hexists b 0", ":0\r\n"));
        // test argc
        expect("empty hexists", compare(ht, "hexists",
               "-ERR wrong number of arguments (given 0, expected 2)\r\n"));
        expect("hexists a", compare(ht, "hexists a",
               "-ERR wrong number of arguments (given 1, expected 2)\r\n"));
        expect("hexists a 1 2", compare(ht, "hexists a 2 1",
               "-ERR wrong number of arguments (given 3, expected 2)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hexists str", compare(ht, "hexists b 1",
               "-ERR wrongtype operation\r\n"));
        expect("hexists list", compare(ht, "hexists c 1",
               "-ERR wrongtype operation\r\n"));
        expect("hexists set", compare(ht, "hexists d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hkeys(HashTable *ht) {
    test_case("test hkeys", {
        // test gen
        expect("hset new hash", compare(ht, "hset a 1 2 3 4 5 6", ":3\r\n"));
        // keys not ordered by insertion
        expect("hkeys a", compare(ht, "hkeys a", "*3\r\n:5\r\n:1\r\n:3\r\n"));
        expect("hset new values", compare(ht, "hset a 7 8 9 0", ":2\r\n"));
        expect("hkeys a", compare(ht, "hkeys a",
               "*5\r\n:7\r\n:9\r\n:1\r\n:3\r\n:5\r\n"));
        expect("hkeys non existing hash", compare(ht, "hkeys b", "*0\r\n"));
        // test argc
        expect("empty hkeys", compare(ht, "hkeys",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("hkeys err argc", compare(ht, "hkeys 1 2",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hkeys str", compare(ht, "hkeys b",
               "-ERR wrongtype operation\r\n"));
        expect("hkeys list", compare(ht, "hkeys c",
               "-ERR wrongtype operation\r\n"));
        expect("hkeys set", compare(ht, "hkeys d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hvals(HashTable *ht) {
    test_case("test hvals", {
        // test gen
        expect("hset new hash", compare(ht, "hset a 1 2 3 4 5 6", ":3\r\n"));
        // keys not ordered by insertion
        expect("hvals a", compare(ht, "hvals a", "*3\r\n:6\r\n:2\r\n:4\r\n"));
        expect("hset new values", compare(ht, "hset a 7 8 9 0", ":2\r\n"));
        expect("hvals a", compare(ht, "hvals a",
                "*5\r\n:8\r\n:0\r\n:2\r\n:4\r\n:6\r\n"));
        expect("hvals non existing hash", compare(ht, "hvals b", "*0\r\n"));
        // test argc
        expect("empty hvals", compare(ht, "hvals",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("hvals err argc", compare(ht, "hvals 1 2",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hvals str", compare(ht, "hvals b",
               "-ERR wrongtype operation\r\n"));
        expect("hvals list", compare(ht, "hvals c",
               "-ERR wrongtype operation\r\n"));
        expect("hvals set", compare(ht, "hvals d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hmget(HashTable *ht) {
    test_case("test hmget", {
        // test gen
        expect("hset new hash", compare(ht, "hset a 1 2 3 4 5 6", ":3\r\n"));
        expect("hmget 1 val", compare(ht, "hmget a 1", "*1\r\n:2\r\n"));
        expect("hmget multiple vals",
               compare(ht, "hmget a 1 3 5", "*3\r\n:2\r\n:4\r\n:6\r\n"));
        expect("hmget non existing val",
               compare(ht, "hmget a 0", "*1\r\n$-1\r\n"));
        expect("hmget ex & nex",
               compare(ht, "hmget a 1 2 3", "*3\r\n:2\r\n$-1\r\n:4\r\n"));
        expect("hmget non existing hash", compare(ht, "hmget b 1", "*0\r\n"));
        // test argc
        expect("empty hmget", compare(ht, "hmget",
               "-ERR wrong number of arguments (given 0, expected 2+)\r\n"));
        expect("hmget err argc", compare(ht, "hmget 1",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hmget str", compare(ht, "hmget b 1",
               "-ERR wrongtype operation\r\n"));
        expect("hmget list", compare(ht, "hmget c 1",
               "-ERR wrongtype operation\r\n"));
        expect("hmget set", compare(ht, "hmget d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_interpret_hash(HashTable *ht) {
    test_hset(ht);
    test_hget(ht);
    test_hdel(ht);
    test_hgetall(ht);
    test_hlen(ht);
    test_hexists(ht);
    test_hkeys(ht);
    test_hvals(ht);
    test_hmget(ht);
}


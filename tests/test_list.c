#include <string.h>
#include "../src/common.h"
#include "miniunit.h"
#include "test.h"

static void test_push(HashTable *ht) {
    test_case("test push", {
        // test gen
        expect("lpush new list", compare(ht, "lpush a 1", ":1\r\n"));
        expect("lpush 3 items", compare(ht, "lpush a 2 3 4", ":4\r\n"));
        expect("rpush 2 items", compare(ht, "rpush a 5 6", ":6\r\n"));
        // test argc
        expect("empty lpush", compare(ht, "lpush",
               "-ERR wrong number of arguments (given 0, expected 2+)\r\n"));
        expect("rpush err argc", compare(ht, "rpush a",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("lpush str", compare(ht, "lpush b 1",
               "-ERR wrongtype operation\r\n"));
        expect("rpush hash", compare(ht, "rpush c 1",
               "-ERR wrongtype operation\r\n"));
        expect("lpush set", compare(ht, "lpush d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

static void test_pop(HashTable *ht) {
    test_case("test pop", {
        // test gen
        expect("rpush 5 values", compare(ht, "rpush a 1 2 3 4 5", ":5\r\n"));
        expect("lpop a = 1", compare(ht, "lpop a", "$1\r\n1\r\n"));
        expect("rpop a = 5", compare(ht, "rpop a", "$1\r\n5\r\n"));
        expect("lpop a = 2", compare(ht, "lpop a", "$1\r\n2\r\n"));
        expect("rpop a = 4", compare(ht, "rpop a", "$1\r\n4\r\n"));
        expect("lpop a = 3", compare(ht, "lpop a", "$1\r\n3\r\n"));
        expect("deleted a", compare(ht, "exists a", ":0\r\n"));
        expect("lpop non existing key", compare(ht, "lpop b", "$-1\r\n"));
        // test argc
        expect("empty lpop", compare(ht, "lpop",
                "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("rpop err argc", compare(ht, "rpop a 1",
                "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("lpop str", compare(ht, "lpop b",
               "-ERR wrongtype operation\r\n"));
        expect("rpop hash", compare(ht, "rpop c",
               "-ERR wrongtype operation\r\n"));
        expect("rpop set", compare(ht, "rpop d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_llen(HashTable *ht) {
    test_case("test llen", {
        // test gen
        expect("lpush new list", compare(ht, "lpush a 1 2 3", ":3\r\n"));
        expect("llen a = 3", compare(ht, "llen a", ":3\r\n"));
        expect("lpush a", compare(ht, "lpush a 1 2 3", ":6\r\n"));
        expect("llen a = 3", compare(ht, "llen a", ":6\r\n"));
        expect("lpop a", compare(ht, "lpop a", "$1\r\n3\r\n"));
        expect("llen a = 3", compare(ht, "llen a", ":5\r\n"));
        expect("llen non existing key", compare(ht, "llen b", ":0\r\n"));
        // test argc
        expect("empty llen", compare(ht, "llen",
                "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("llen err argc", compare(ht, "llen 1 2",
                "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("llen str", compare(ht, "llen b",
               "-ERR wrongtype operation\r\n"));
        expect("llen hash", compare(ht, "llen c",
               "-ERR wrongtype operation\r\n"));
        expect("llen set", compare(ht, "llen d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_lindex(HashTable *ht) {
    test_case("test lindex", {
        // test gen
        expect("rpush new list", compare(ht, "rpush a 1 2 3 4 5", ":5\r\n"));
        expect("lindex a[0] = 1", compare(ht, "lindex a 0", "$1\r\n1\r\n"));
        expect("lindex a[2] = 3", compare(ht, "lindex a 2", "$1\r\n3\r\n"));
        expect("lindex a[-1] = 5", compare(ht, "lindex a -1", "$1\r\n5\r\n"));
        expect("lindex a[-5] = 1", compare(ht, "lindex a -5", "$1\r\n1\r\n"));
        expect("lindex a[-6] = err", compare(ht, "lindex a -6",
               "-ERR value is not an integer or out of range\r\n"));
        expect("lindex a[5] = err", compare(ht, "lindex a 5",
               "-ERR value is not an integer or out of range\r\n"));
        expect("lindex a[a] = err", compare(ht, "lindex a a",
               "-ERR value is not an integer or out of range\r\n"));
        // test argc
        expect("empty lindex", compare(ht, "lindex",
                "-ERR wrong number of arguments (given 0, expected 2)\r\n"));
        expect("lindex err argc", compare(ht, "lindex a",
                "-ERR wrong number of arguments (given 1, expected 2)\r\n"));
        expect("lindex err argc", compare(ht, "lindex a 1 2",
                "-ERR wrong number of arguments (given 3, expected 2)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("lindex str", compare(ht, "lindex b 0",
               "-ERR wrongtype operation\r\n"));
        expect("lindex hash", compare(ht, "lindex c 0",
               "-ERR wrongtype operation\r\n"));
        expect("lindex set", compare(ht, "lindex d 0",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_lrange(HashTable *ht) {
    test_case("test lrange", {
        // test gen
        expect("rpush new list", compare(ht, "rpush a 1 2 3 4 5", ":5\r\n"));
        expect("lrange 1 element",
               compare(ht, "lrange a 0 0", "*1\r\n:1\r\n"));
        expect("lrange all element", compare(ht, "lrange a 0 4",
               "*5\r\n:1\r\n:2\r\n:3\r\n:4\r\n:5\r\n"));
        expect("lrange all element with neg id", compare(ht, "lrange a 0 -1",
               "*5\r\n:1\r\n:2\r\n:3\r\n:4\r\n:5\r\n"));
        expect("last 3 elements", compare(ht, "lrange a -3 -1",
               "*3\r\n:3\r\n:4\r\n:5\r\n"));
        expect("index error", compare(ht, "lrange a 0 9",
               "-ERR value is not an integer or out of range\r\n"));
        expect("index error", compare(ht, "lrange a 9 0",
               "-ERR value is not an integer or out of range\r\n"));
        expect("index error", compare(ht, "lrange a -2 -4",
               "-ERR value is not an integer or out of range\r\n"));
        expect("lrange non existing list", compare(ht, "lrange b 0 -1",
               "*0\r\n"));
        // test argc
        expect("empty lrange", compare(ht, "lrange",
                "-ERR wrong number of arguments (given 0, expected 3)\r\n"));
        expect("lrange err argc", compare(ht, "lrange a",
                "-ERR wrong number of arguments (given 1, expected 3)\r\n"));
        expect("lrange err argc", compare(ht, "lrange a 1",
                "-ERR wrong number of arguments (given 2, expected 3)\r\n"));
        expect("lrange err argc", compare(ht, "lrange a 1 2 3",
                "-ERR wrong number of arguments (given 4, expected 3)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("lrange str", compare(ht, "lrange b 0 1",
               "-ERR wrongtype operation\r\n"));
        expect("lrange hash", compare(ht, "lrange c 0 1",
               "-ERR wrongtype operation\r\n"));
        expect("lrange set", compare(ht, "lrange d 0 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_lset(HashTable *ht) {
    test_case("test lset", {
        // test gen
        expect("rpush new list", compare(ht, "rpush a 1 2 3 4 5", ":5\r\n"));
        expect("lset a[0] zero", compare(ht, "lset a 0 zero", "$2\r\nOK\r\n"));
        expect("lpop a = zero", compare(ht, "lpop a", "$4\r\nzero\r\n"));
        expect("lset id err", compare(ht, "lset a 9 hi",
               "-ERR value is not an integer or out of range\r\n"));
        expect("lset neg id err", compare(ht, "lset a -8 hi",
               "-ERR value is not an integer or out of range\r\n"));
        expect("lset alpha id err", compare(ht, "lset a s hi",
               "-ERR value is not an integer or out of range\r\n"));
        expect("lset non existing list",
               compare(ht, "lset b 0 zero", "$-1\r\n"));
        // test argc
        expect("empty lset", compare(ht, "lset",
                "-ERR wrong number of arguments (given 0, expected 3)\r\n"));
        expect("lset err argc", compare(ht, "lset a",
                "-ERR wrong number of arguments (given 1, expected 3)\r\n"));
        expect("lset err argc", compare(ht, "lset a 1",
                "-ERR wrong number of arguments (given 2, expected 3)\r\n"));
        expect("lset err argc", compare(ht, "lset a 1 2 3",
                "-ERR wrong number of arguments (given 4, expected 3)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("lset str", compare(ht, "lset b 0 1",
               "-ERR wrongtype operation\r\n"));
        expect("lset hash", compare(ht, "lset c 0 1",
               "-ERR wrongtype operation\r\n"));
        expect("lset set", compare(ht, "lset d 0 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_lrem(HashTable *ht) {
    test_case("test lrem", {
        // test gen
        expect("rpush new list",
               compare(ht, "rpush a 1 1 2 2 2 2 2 1 1", ":9\r\n"));
        expect("lrem first 3 1s", compare(ht, "lrem a 3 1", ":3\r\n"));
        expect("lrange 0:-1 a", compare(ht, "lrange a 0 -1",
               "*6\r\n:2\r\n:2\r\n:2\r\n:2\r\n:2\r\n:1\r\n"));
        expect("lrem last 2 2s", compare(ht, "lrem a -2 2", ":2\r\n"));
        expect("lrange 0:-1 a", compare(ht, "lrange a 0 -1",
               "*4\r\n:2\r\n:2\r\n:2\r\n:1\r\n"));
        expect("lrem last 2 1s", compare(ht, "lrem a 2 1", ":1\r\n"));
        expect("lrange 0:-1 a", compare(ht, "lrange a 0 -1",
               "*3\r\n:2\r\n:2\r\n:2\r\n"));
        expect("lrem all 2s", compare(ht, "lrem a 0 2", ":3\r\n"));
        expect("list deleted", compare(ht, "lrem a 0 1", ":0\r\n"));
        // test argc
        expect("empty lrem", compare(ht, "lrem",
                "-ERR wrong number of arguments (given 0, expected 3)\r\n"));
        expect("lrem err argc", compare(ht, "lrem a",
                "-ERR wrong number of arguments (given 1, expected 3)\r\n"));
        expect("lrem err argc", compare(ht, "lrem 1 2",
                "-ERR wrong number of arguments (given 2, expected 3)\r\n"));
        expect("lrem err argc", compare(ht, "lrem 1 2 3 4",
                "-ERR wrong number of arguments (given 4, expected 3)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("lrem str", compare(ht, "lrem b 0 1",
               "-ERR wrongtype operation\r\n"));
        expect("lrem hash", compare(ht, "lrem c 0 1",
               "-ERR wrongtype operation\r\n"));
        expect("lrem set", compare(ht, "lrem d 0 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_lpos(HashTable *ht) {
    test_case("test lpos", {
        // test gen
        expect("rpush new list", compare(ht, "rpush a 1 2 3 4 5", ":5\r\n"));
        expect("lpos 1 = 0", compare(ht, "lpos a 1", ":0\r\n"));
        expect("lpos 5 = 4", compare(ht, "lpos a 5", ":4\r\n"));
        expect("lpos 8 = ??", compare(ht, "lpos a 8", "$-1\r\n"));
        expect("lpos non existing list", compare(ht, "lpos b 5", "$-1\r\n"));
        // test argc
        expect("empty lpos", compare(ht, "lpos",
                "-ERR wrong number of arguments (given 0, expected 2)\r\n"));
        expect("lpos err argc", compare(ht, "lpos a",
                "-ERR wrong number of arguments (given 1, expected 2)\r\n"));
        expect("lpos err argc", compare(ht, "lpos a 1 2",
                "-ERR wrong number of arguments (given 3, expected 2)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c 1 2", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("lpos str", compare(ht, "lpos b 0",
               "-ERR wrongtype operation\r\n"));
        expect("lpos hash", compare(ht, "lpos c 0",
               "-ERR wrongtype operation\r\n"));
        expect("lpos set", compare(ht, "lpos d 0",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_interpret_list(HashTable *ht) {
    test_push(ht);
    test_pop(ht);
    test_llen(ht);
    test_lindex(ht);
    test_lrange(ht);
    test_lset(ht);
    test_lpos(ht);
    test_lrem(ht);
}


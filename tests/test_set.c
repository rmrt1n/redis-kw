#include <string.h>
#include "../src/common.h"
#include "miniunit.h"
#include "test.h"

void test_sadd(HashTable *ht) {
    test_case("test sadd", {
        // test gen
        expect("sadd new set", compare(ht, "sadd a 1", ":1\r\n"));
        expect("sadd 2 new item", compare(ht, "sadd a 2 3", ":2\r\n"));
        expect("sadd 1 ex & 1 nex", compare(ht, "sadd a 0 2", ":1\r\n"));
        // test argc
        expect("empty sadd", compare(ht, "sadd",
               "-ERR wrong number of arguments (given 0, expected 2+)\r\n"));
        expect("sadd err argc", compare(ht, "sadd a",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("lpush d", compare(ht, "lpush d 1", ":1\r\n"));
        expect("sadd str", compare(ht, "sadd b 1",
               "-ERR wrongtype operation\r\n"));
        expect("sadd hash", compare(ht, "sadd c 1",
               "-ERR wrongtype operation\r\n"));
        expect("sadd list", compare(ht, "sadd d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_srem(HashTable *ht) {
    test_case("test srem", {
        // test gen
        expect("sadd new set", compare(ht, "sadd a 1 2 3 4 5", ":5\r\n"));
        expect("srem 1 member", compare(ht, "srem a 1", ":1\r\n"));
        expect("srem multiple members", compare(ht, "srem a 2 3", ":2\r\n"));
        expect("srem ex & nex", compare(ht, "srem a 4 5 6", ":2\r\n"));
        expect("deleted set", compare(ht, "exists a", ":0\r\n"));
        expect("srem non existing set", compare(ht, "srem b 1", ":0\r\n"));
        expect("srem err argc", compare(ht, "srem a",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        // test argc
        expect("empty srem", compare(ht, "srem",
               "-ERR wrong number of arguments (given 0, expected 2+)\r\n"));
        expect("srem err argc", compare(ht, "srem a",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("lpush d", compare(ht, "lpush d 1", ":1\r\n"));
        expect("srem str", compare(ht, "srem b 1",
               "-ERR wrongtype operation\r\n"));
        expect("srem hash", compare(ht, "srem c 1",
               "-ERR wrongtype operation\r\n"));
        expect("srem list", compare(ht, "srem d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_sismember(HashTable *ht) {
    test_case("test sismember", {
        // test gen
        expect("sadd new set", compare(ht, "sadd a 1 2 3 4 5", ":5\r\n"));
        expect("1 in a", compare(ht, "sismember a 1", ":1\r\n"));
        expect("2 in a", compare(ht, "sismember a 2", ":1\r\n"));
        expect("0 not in a", compare(ht, "sismember a 0", ":0\r\n"));
        expect("check non existing key",
               compare(ht, "sismember b 0", ":0\r\n"));
        // test argc
        expect("empty sismember", compare(ht, "sismember",
               "-ERR wrong number of arguments (given 0, expected 2)\r\n"));
        expect("sismember err argc", compare(ht, "sismember 1",
               "-ERR wrong number of arguments (given 1, expected 2)\r\n"));
        expect("sismember err argc", compare(ht, "sismember 1 2 3",
               "-ERR wrong number of arguments (given 3, expected 2)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("lpush d", compare(ht, "lpush d 1", ":1\r\n"));
        expect("sismember str", compare(ht, "sismember b 1",
               "-ERR wrongtype operation\r\n"));
        expect("sismember hash", compare(ht, "sismember c 1",
               "-ERR wrongtype operation\r\n"));
        expect("sismember list", compare(ht, "sismember d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_smembers(HashTable *ht) {
    test_case("test smembers", {
        // test gen
        expect("sadd new set", compare(ht, "sadd a 1 2 3 4 5", ":5\r\n"));
        expect("smembers a", compare(ht, "smembers a",
               "*5\r\n:1\r\n:2\r\n:3\r\n:4\r\n:5\r\n"));
        expect("sadd new members", compare(ht, "sadd a 1 6 7 8", ":3\r\n"));
        expect("smembers a", compare(ht, "smembers a",
               "*8\r\n:8\r\n:1\r\n:2\r\n:3\r\n:4\r\n:5\r\n:6\r\n:7\r\n"));
        expect("smembers non existing set", compare(ht, "smembers b", "*0\r\n"));
        // test argc
        expect("empty smembers", compare(ht, "smembers",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("smembers err argc", compare(ht, "smembers a 1",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("lpush d", compare(ht, "lpush d 1", ":1\r\n"));
        expect("smembers str", compare(ht, "smembers b",
               "-ERR wrongtype operation\r\n"));
        expect("smembers hash", compare(ht, "smembers c",
               "-ERR wrongtype operation\r\n"));
        expect("smembers list", compare(ht, "smembers d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_smismember(HashTable *ht) {
    test_case("test smismember", {
        // test gen
        expect("sadd new set", compare(ht, "sadd a 1 2 3 4 5", ":5\r\n"));
        expect("smismember 1 value",
               compare(ht, "smismember a 1", "*1\r\n:1\r\n"));
        expect("smismember non existing member",
               compare(ht, "smismember a 0", "*1\r\n:0\r\n"));
        expect("smismember multiple values",
               compare(ht, "smismember a 1 2 3", "*3\r\n:1\r\n:1\r\n:1\r\n"));
        expect("smismember multiple ex & nex values",
               compare(ht, "smismember a 1 2 3 0 9 4 1",
                       "*7\r\n:1\r\n:1\r\n:1\r\n:0\r\n:0\r\n:1\r\n:1\r\n"));
        expect("smismember non existing set",
               compare(ht, "smismember b 1", "*0\r\n"));
        // test argc
        expect("empty smismember", compare(ht, "smismember",
               "-ERR wrong number of arguments (given 0, expected 2+)\r\n"));
        expect("smismember err argc", compare(ht, "smismember a",
               "-ERR wrong number of arguments (given 1, expected 2+)\r\n"));
        // test type
        expect("set b", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("hset c", compare(ht, "hset c 1 2", ":1\r\n"));
        expect("lpush d", compare(ht, "lpush d 1", ":1\r\n"));
        expect("smismember str", compare(ht, "smismember b 1",
               "-ERR wrongtype operation\r\n"));
        expect("smismember hash", compare(ht, "smismember c 1",
               "-ERR wrongtype operation\r\n"));
        expect("smismember list", compare(ht, "smismember d 1",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_interpret_set(HashTable *ht) {
    test_sadd(ht);
    test_srem(ht);
    test_sismember(ht);
    test_smembers(ht);
    test_smismember(ht);
}


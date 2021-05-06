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

void test_interpret_set(HashTable *ht) {
    test_sadd(ht);
    test_srem(ht);
}

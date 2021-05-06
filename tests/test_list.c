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
        expect("rpop hash", compare(ht, "rpop b",
               "-ERR wrongtype operation\r\n"));
        expect("rpop set", compare(ht, "rpop d",
               "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_interpret_list(HashTable *ht) {
    test_push(ht);
    test_pop(ht);
}


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

void test_interpret_str(HashTable *ht) {
    test_set(ht);
    test_get(ht);
}

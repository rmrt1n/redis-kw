#include <string.h>
#include "../src/common.h"
#include "miniunit.h"
#include "test.h"

static void test_del(HashTable *ht) {
    test_case("test del", {
        // test gen
        expect("del non existing item", compare(ht, "del 1", ":0\r\n"));
        expect("set a 1", compare(ht, "set a 1", "$2\r\nOK\r\n"));
        expect("set b 1", compare(ht, "set b 1", "$2\r\nOK\r\n"));
        expect("set c 1", compare(ht, "set c 1", "$2\r\nOK\r\n"));
        expect("del existing str", compare(ht, "del a", ":1\r\n"));
        expect("del 2 b, c, d", compare(ht, "del b c d", ":2\r\n"));
        // test argc
        expect("empty del", compare(ht, "del",
               "-ERR wrong number of arguments (given 0, expected 1+)\r\n"));
    });
    cleanup(ht);
}

static void test_exists(HashTable *ht) {
    test_case("test exists", {
        // test gen
        expect("set a 1", compare(ht, "set a 1", "$2\r\nOK\r\n"));
        expect("a exists", compare(ht, "exists a", ":1\r\n"));
        expect("b not exist", compare(ht, "exists b", ":0\r\n"));
        expect("a ex, b nex", compare(ht, "exists a b", ":1\r\n"));
        // test argc
        expect("empty exists", compare(ht, "exists",
               "-ERR wrong number of arguments (given 0, expected 1+)\r\n"));
    });
    cleanup(ht);
}

static void test_type(HashTable *ht) {
    test_case("test type", {
        // test gen
        expect("set a 1", compare(ht, "set a 1", "$2\r\nOK\r\n"));
        expect("a = str", compare(ht, "type a", "$6\r\nstring\r\n"));
        expect("hset b 1 2", compare(ht, "hset b 1 2", ":1\r\n"));
        expect("b = hash", compare(ht, "type b", "$4\r\nhash\r\n"));
        expect("lpush c 1", compare(ht, "lpush c 1", ":1\r\n"));
        expect("c = list", compare(ht, "type c", "$4\r\nlist\r\n"));
        expect("sadd d 1", compare(ht, "sadd d 1", ":1\r\n"));
        expect("d = set", compare(ht, "type d", "$3\r\nset\r\n"));
        expect("type non existing item", compare(ht, "type 1", "$4\r\nnone\r\n"));
        // test argc
        expect("empty type", compare(ht, "type",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("type err argc", compare(ht, "type 1 2",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
    });
    cleanup(ht);
}

void test_interpret_key(HashTable *ht) {
    test_del(ht);
    test_exists(ht);
    test_type(ht);
}

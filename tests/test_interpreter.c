#include <string.h>
#include "../src/common.h"
#include "test.h"
#include "miniunit.h"

static void cleanup(HashTable *ht) {
    interpret(parse("del a b c d"), ht);
}

static bool compare(HashTable *ht, char *cmd, char *expected) {
    char *res = interpret(parse(cmd), ht);
    // if (res != NULL) puts(res);
    if (expected == NULL) return res == NULL;
    return strcmp(res, expected) == 0;
}

void test_del(HashTable *ht) {
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

void test_exists(HashTable *ht) {
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

void test_type(HashTable *ht) {
    test_case("test type", {
        // test gen
        expect("set a 1", compare(ht, "set a 1", "$2\r\nOK\r\n"));
        expect("a = str", compare(ht, "type a", "$6\r\nstring\r\n"));
        // expect("hset b 1 2", compare(ht, "hset b 1 2", ":1\r\n"));
        // expect("b = hash", compare(ht, "type b", "$4\r\nhash\r\n"));
        // expect("lpush c 1", compare(ht, "lpush c 1", ":1\r\n"));
        // expect("c = list", compare(ht, "type c", "$4\r\nlist\r\n"));
        // expect("sadd d 1", compare(ht, "sadd d 1", ":1\r\n"));
        // expect("d = set", compare(ht, "type d", "$3\r\nset\r\n"));
        expect("type non existing item", compare(ht, "type 1", "$4\r\nnone\r\n"));
        // test argc
        expect("empty type", compare(ht, "type",
               "-ERR wrong number of arguments (given 0, expected 1)\r\n"));
        expect("type err argc", compare(ht, "type 1 2",
               "-ERR wrong number of arguments (given 2, expected 1)\r\n"));
    });
    cleanup(ht);
}

void test_set(HashTable *ht) {
    test_case("test set", {
        // test gen
        expect("empty set", compare(ht, "set", "$2\r\nOK\r\n"));
        expect("set a to ''", compare(ht, "set a", "$2\r\nOK\r\n"));
        expect("set a to hello", compare(ht, "set a hello", "$2\r\nOK\r\n"));
        // test argc
        expect("set argc err", compare(ht, "set a b c",
               "-ERR wrong number of arguments (given 3, expected 0..2)\r\n"));
    });
    cleanup(ht);
}

void test_get(HashTable *ht) {
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
        // expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        // expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("get hash err", compare(ht, "get b",
               "-ERR wrongtype operation\r\n"));
        // expect("get list err", compare(ht, "get c",
               // "-ERR wrongtype operation\r\n"));
        // expect("get set err", compare(ht, "get d",
               // "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hset(HashTable *ht) {
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
        // expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        // expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hset str", compare(ht, "hset b 1 2",
               "-ERR wrongtype operation\r\n"));
        // expect("hset list", compare(ht, "hset c 1 2",
               // "-ERR wrongtype operation\r\n"));
        // expect("hset set", compare(ht, "hset d 1 2",
               // "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hget(HashTable *ht) {
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
        // expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        // expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hget str", compare(ht, "hget b 1",
               "-ERR wrongtype operation\r\n"));
        // expect("hget list", compare(ht, "hget c 1",
               // "-ERR wrongtype operation\r\n"));
        // expect("hget set", compare(ht, "hget d 1",
               // "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

void test_hdel(HashTable *ht) {
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
        // expect("lpush c", compare(ht, "lpush c 1", ":1\r\n"));
        // expect("sadd d", compare(ht, "sadd d 1", ":1\r\n"));
        expect("hdel str", compare(ht, "hdel b 1",
               "-ERR wrongtype operation\r\n"));
        // expect("hdel list", compare(ht, "hdel c 1",
               // "-ERR wrongtype operation\r\n"));
        // expect("hdel set", compare(ht, "hdel d 1",
               // "-ERR wrongtype operation\r\n"));
    });
    cleanup(ht);
}

// void test_(HashTable *ht) {
    // test_case("test ", {
        // // test gen
        // // test argc
        // // test type
    // });
    // cleanup(ht);
// }

void test_etc(HashTable *ht) {
    test_case("test unknown", {
        expect("unknown", compare(ht, "lks", "-ERR unrecognized command\r\n"));
        expect("unknown", compare(ht, "ks", "-ERR unrecognized command\r\n"));
        expect("unknown", compare(ht, "93", "-ERR unrecognized command\r\n"));
        expect("noop", compare(ht, "", ""));
        expect("quit", compare(ht, "quit", NULL));
    });
}

void test_interpret() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_del(ht);
    test_exists(ht);
    test_type(ht);
    test_set(ht);
    test_get(ht);
    test_hset(ht);
    test_hget(ht);
    test_hdel(ht);
    test_etc(ht);
    htable_free(ht);
    return;
}

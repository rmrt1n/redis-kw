#include <string.h>
#include "../src/common.h"
#include "test.h"
#include "miniunit.h"

void cleanup(HashTable *ht) {
    interpret(ht, parse("del a b c d"));
}

bool compare(HashTable *ht, char *cmd, char *expected) {
    char *res = interpret(ht, parse(cmd));
    // if (res != NULL) puts(res);
    if (expected == NULL) return res == NULL;
    return strcmp(res, expected) == 0;
}

// template
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
        expect("quit", compare(ht, "quit", "q"));
        expect("shutdown", compare(ht, "shutdown", "x"));
    });
}

void test_interpret() {
    HashTable *ht = htable_init(512);
    test_interpret_key(ht);
    test_interpret_str(ht);
    test_interpret_hash(ht);
    test_interpret_list(ht);
    test_interpret_set(ht);
    test_etc(ht);
    htable_free(ht);
}

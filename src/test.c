#include <stdio.h>
#include <string.h>
#include "common.h"

// minunit
#define mu_assert(message, test) do { if (!(test)) return message;  } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                               if (message) return message; } while (0)

int tests_run = 0;

char *test_htable() {
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);
    mu_assert("error creating hash table", ht != NULL);
    htable_free(ht);
    return 0;
}

char *test_string_cmds() {
    HtableAction res;
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);

    // set
    res = htable_set(ht, "a", "hello");
    mu_assert("failed to set value", res.status == OK);
    // get
    res = htable_get(ht, "a");
    mu_assert("1 failed to get value from key",
              res.status == OK && strcmp(res.value, "hello") == 0);
    // set replace
    res = htable_set(ht, "a", "test");
    mu_assert("failed to set value", res.status == OK);
    // get replaced
    res = htable_get(ht, "a");
    mu_assert("2 failed to get value from key",
              res.status == OK && strcmp(res.value, "test") == 0);
    // del
    res = htable_del(ht, "a");
    mu_assert("failed to delete value", res.status == OK);
    // get deleted
    res = htable_get(ht, "a");
    mu_assert("2 failed to get value from key", res.status == NIL);
    // del deleted
    res = htable_del(ht, "a");
    mu_assert("failed to delete value", res.status == NIL);

    htable_free(ht);
    return 0;
}

char *test_hash_cmds() {
    HtableAction res;
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);

    // hset
    res = htable_hset(ht, "a", "1", "2");
    mu_assert("failed to set hash value", res.status == OK);
    // hget
    res = htable_hget(ht, "a", "1");
    mu_assert("1 failed to get hash value",
              res.status == OK && strcmp(res.value, "2") == 0);
    // hset replace
    res = htable_hset(ht, "a", "1", "3");
    mu_assert("failed to set hash value", res.status == OK);
    // hget replace
    res = htable_hget(ht, "a", "1");
    mu_assert("2 failed to get hash value",
              res.status == OK && strcmp(res.value, "3") == 0);
    // hdel
    res = htable_hdel(ht, "a", "1");
    mu_assert("failed to delete hash value", res.status == OK);
    // hget deleted
    res = htable_hget(ht, "a", "1");
    mu_assert("got something from deleted value", res.status == NIL);
    // hdel deleted
    res = htable_hdel(ht, "a", "1");
    mu_assert("failed to delete hash value", res.status == NIL);

    htable_free(ht);
    return 0;
}

char *test_str_hash() {
    HtableAction res;
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);

    // hset 1
    res = htable_hset(ht, "a", "1", "2");
    mu_assert("failed to set hash value", res.status == OK);
    // hset 2
    res = htable_hset(ht, "a", "2", "2");
    mu_assert("failed to set hash value", res.status == OK);
    // hset 3
    res = htable_hset(ht, "a", "3", "2");
    mu_assert("failed to set hash value", res.status == OK);
    // hget 1
    res = htable_hget(ht, "a", "2");
    mu_assert("failed to get hash value",
              res.status == OK && strcmp(res.value, "2") == 0);
    // hget 2
    res = htable_hget(ht, "a", "3");
    mu_assert("failed to get hash value",
              res.status == OK && strcmp(res.value, "2") == 0);
    // hget 3
    res = htable_hget(ht, "a", "1");
    mu_assert("failed to get hash value",
              res.status == OK && strcmp(res.value, "2") == 0);
    // set replace hset
    res = htable_set(ht, "a", "hello");
    mu_assert("failed to set str to hash", res.status == OK);
    // hget replaced
    res = htable_hget(ht, "a", "1");
    mu_assert("got value from wrong type", res.status == ERR);
    // get replaced
    res = htable_get(ht, "a");
    mu_assert("failed to get str value",
              res.status == OK && strcmp(res.value, "hello") == 0);
    // hdel str
    res = htable_hdel(ht, "a", "1");
    mu_assert("deleted wrong type", res.status == ERR);

    htable_free(ht);
    return 0;
}

char *test_list_cmds() {
    HtableAction res;
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);

    // lpush 1
    res = htable_push(ht, "a", "1", LEFT);
    mu_assert("1 failed to lpush item to empty list", res.status == OK);
    // lpush 2
    res = htable_push(ht, "a", "2", LEFT);
    mu_assert("2 failed to lpush item to empty list", res.status == OK);
    // rpush 1
    res = htable_push(ht, "a", "1", RIGHT);
    mu_assert("1 failed to rpush item to empty list", res.status == OK);
    // rpush 2
    res = htable_push(ht, "a", "2", RIGHT);
    mu_assert("2 failed to rpush item to empty list", res.status == OK);
    // lpop 1
    res = htable_pop(ht, "a", LEFT);
    mu_assert("1 failed to lpop item from list",
              res.status == OK && strcmp(res.value, "2") == 0);
    // lpop 2
    res = htable_pop(ht, "a", LEFT);
    mu_assert("2 failed to lpop item from list",
              res.status == OK && strcmp(res.value, "1") == 0);
    // rpop 1
    res = htable_pop(ht, "a", RIGHT);
    mu_assert("1 failed to rpop item from list",
              res.status == OK && strcmp(res.value, "2") == 0);
    // rpop 2
    res = htable_pop(ht, "a", RIGHT);
    mu_assert("2 failed to rpop item from list",
              res.status == OK && strcmp(res.value, "1") == 0);
    // lpop from empty list
    res = htable_pop(ht, "a", LEFT);
    mu_assert("got something from deleted list item", res.status == NIL);
    // rpop from empty list
    res = htable_pop(ht, "a", RIGHT);
    mu_assert("got something from deleted list item", res.status == NIL);

    htable_free(ht);
    return 0;
}

char *test_keys_cmds() {
    HtableAction res;
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);

    // setting values
    res = htable_set(ht, "str", "hello");
    mu_assert("failed to set str", res.status == OK);
    res = htable_hset(ht, "hash", "1", "2");
    mu_assert("failed to set hash", res.status == OK);
    res = htable_push(ht, "list", "hi", LEFT);
    mu_assert("failed to set list", res.status == OK);

    // exists cmd
    res = htable_exists(ht, "str");
    mu_assert("couldn't get existing key", res.status == OK);
    res = htable_exists(ht, "hash");
    mu_assert("couldn't get existing key", res.status == OK);
    res = htable_exists(ht, "list");
    mu_assert("couldn't get existing key", res.status == OK);

    // type cmd
    res = htable_type(ht, "str");
    mu_assert("failed to recognized str",
              res.status == OK && strcmp(res.value, "string") == 0);
    res = htable_type(ht, "hash");
    mu_assert("failed to recognized hash",
              res.status == OK && strcmp(res.value, "hash") == 0);
    res = htable_type(ht, "list");
    mu_assert("failed to recognized list",
              res.status == OK && strcmp(res.value, "list") == 0);

    // geting values
    res = htable_get(ht, "str");
    mu_assert("failed to get existing str",
              res.status == OK && strcmp(res.value, "hello") == 0);
    res = htable_hget(ht, "hash", "1");
    mu_assert("failed to get existing str",
              res.status == OK && strcmp(res.value, "2") == 0);
    res = htable_pop(ht, "list", LEFT);
    mu_assert("failed to get existing list",
              res.status == OK && strcmp(res.value, "hi") == 0);

    htable_free(ht);
    return 0;
}

char *test_set_cmds() {
    HtableAction res;
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);

    // sadd 1
    res = htable_sadd(ht, "a", "1");
    mu_assert("1 failed to add item to set", res.status == OK);
    // sadd 2
    res = htable_sadd(ht, "a", "2");
    mu_assert("2 failed to add item to set", res.status == OK);
    // sadd 3
    res = htable_sadd(ht, "a", "3");
    mu_assert("3 failed to add item to set", res.status == OK);

    // sismember 1
    res = htable_sismember(ht, "a", "1");
    mu_assert("1 failed to get existing set member", res.status == OK);
    mu_assert("1 wrong value from set member", strcmp(res.value, "1") == 0);
    // sismember 2
    res = htable_sismember(ht, "a", "2");
    mu_assert("2 failed to get existing set member", res.status == OK);
    mu_assert("2 wrong value from set member", strcmp(res.value, "2") == 0);
    // sismember 3
    res = htable_sismember(ht, "a", "3");
    mu_assert("3 failed to get existing set member", res.status == OK);
    mu_assert("3 wrong value from set member", strcmp(res.value, "3") == 0);
    // sismember 4
    res = htable_sismember(ht, "a", "4");
    mu_assert("4 failed to get existing set member", res.status == NIL);

    // srem 1
    res = htable_srem(ht, "a", "1");
    mu_assert("1 failed to remove item from set", res.status == OK);
    // srem 2
    res = htable_srem(ht, "a", "2");
    mu_assert("1 failed to remove item from set", res.status == OK);
    // srem 3
    res = htable_srem(ht, "a", "3");
    mu_assert("1 failed to remove item from set", res.status == OK);

    htable_free(ht);
    return 0;
}

char *test_all() {
    mu_run_test(test_htable);
    mu_run_test(test_string_cmds);
    mu_run_test(test_hash_cmds);
    mu_run_test(test_str_hash);
    mu_run_test(test_list_cmds);
    mu_run_test(test_keys_cmds);
    mu_run_test(test_set_cmds);
    return 0;
}

int main() {
    char *res = test_all();
    if (res != 0) {
        puts(res);
    } else {
        puts("all tests passed");
    }
    printf("tests run: %d\n", tests_run);
    return res != 0;
}


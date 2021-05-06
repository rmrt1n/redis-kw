#include <string.h>
#include "../src/common.h"
#include "miniunit.h"

static void test_creation() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_case("test htable create", {
        expect("ht not null", ht != NULL);
        expect("used = 0, size = 2", ht->used == 0 && ht->size == HT_BASE_SIZE);
    });
    htable_free(ht);
}

static void test_insert() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_case("test htable insertion", {
        htable_set(ht, "a", "str");
        expect("used = 1, size = 2", ht->used == 1 && ht->size == 2);
        htable_set(ht, "a", "hello");
        expect("used = 1, size = 2", ht->used == 1 && ht->size == 2);

        htable_hset(ht, "b", "1", "hash");
        expect("used = 2, size = 5", ht->used == 2 && ht->size == 5);
        htable_hset(ht, "b", "2", "hash");
        expect("used = 2, size = 5", ht->used == 2 && ht->size == 5);

        htable_push(ht, "c", "1", LEFT);
        expect("used = 3, size = 5", ht->used == 3 && ht->size == 5);
        htable_push(ht, "c", "2", RIGHT); htable_push(ht, "c", "3", LEFT);
        expect("used = 3, size = 5", ht->used == 3 && ht->size == 5);

        htable_sadd(ht, "d", "1");
        expect("used = 4, size = 11", ht->used == 4 && ht->size == 11);
        htable_sadd(ht, "d", "2");
        expect("used = 4, size = 11", ht->used == 4 && ht->size == 11);
    });
    htable_free(ht);
}

static void test_delete() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_case("test htable deletion", {
        htable_set(ht, "a", "str"); htable_set(ht, "a", "hello");
        expect("deleting a", htable_del(ht, "a"));
        expect("deleted a", !htable_exists(ht, "a"));

        htable_hset(ht, "b", "1", "hash"); htable_hset(ht, "b", "2", "hash");
        expect("deleting b", htable_del(ht, "b"));
        expect("deleted b", !htable_exists(ht, "b"));

        htable_push(ht, "c", "1", LEFT); htable_push(ht, "c", "2", RIGHT);
        expect("deleting c", htable_del(ht, "c"));
        expect("deleted c", !htable_exists(ht, "c"));

        htable_sadd(ht, "d", "1"); htable_sadd(ht, "d", "2");
        expect("deleting d", htable_del(ht, "d"));
        expect("deleted d", !htable_exists(ht, "d"));
    });
    htable_free(ht);
}

static void test_str_funcs() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_case("test str functions", {
        // set
        htable_set(ht, "a", "1"); htable_set(ht, "a", "2");
        htable_set(ht, "b", "3"); htable_set(ht, "c", "4");
        expect("used 3, size 5", ht->used == 3 && ht->size == 5);

        // get
        expect("a = 2", strcmp(htable_get(ht, "a"), "2") == 0);
        expect("b = 3", strcmp(htable_get(ht, "b"), "3") == 0);
        expect("c = 4", strcmp(htable_get(ht, "c"), "4") == 0);

        expect("deleting a", htable_del(ht, "a"));
        expect("used 2, size 5", ht->used == 2 && ht->size == 5);
        expect("a == NULL", htable_get(ht, "a") == NULL);

        expect("deleting b && c", htable_del(ht, "b") && htable_del(ht, "c"));
        expect("b & c== NULL", htable_get(ht, "b") == NULL &&
                               htable_get(ht, "c") == NULL);
        expect("used 0, size 2", ht->used == 0 && ht->size == 2);

    });
    htable_free(ht);
}

static void test_hash_funcs() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_case("test hash functions", {
        // hset
        htable_hset(ht, "a", "1", "2"); htable_hset(ht, "a", "2", "3");
        htable_hset(ht, "a", "3", "4"); htable_hset(ht, "a", "4", "5");
        expect("used 1, size 2", ht->used == 1 && ht->size == 2);

        // hlen
        expect("hash len = 4", htable_hlen(ht, "a") == 4);

        // hgetall
        char **hgetall = htable_hgetall(ht, "a");
        expect("a:1 == 2", strcmp(hgetall[0], "1") == 0 &&
                           strcmp(hgetall[1], "2") == 0);
        expect("a:2 == 3", strcmp(hgetall[2], "2") == 0 &&
                           strcmp(hgetall[3], "3") == 0);
        expect("a:3 == 4", strcmp(hgetall[4], "3") == 0 &&
                           strcmp(hgetall[5], "4") == 0);
        expect("a:4 == 5", strcmp(hgetall[6], "4") == 0 &&
                           strcmp(hgetall[7], "5") == 0);
        expect("last item of char** is NULL", hgetall[8] == NULL);
        free(hgetall);

        // hget
        expect("hget a:1 = '2'", strcmp(htable_hget(ht, "a", "1"), "2") == 0);
        expect("hget a:2 = '3'", strcmp(htable_hget(ht, "a", "2"), "3") == 0);
        expect("hget a:3 = '4'", strcmp(htable_hget(ht, "a", "3"), "4") == 0);
        expect("hget a:4 = '5'", strcmp(htable_hget(ht, "a", "4"), "5") == 0);
        expect("hget a:5 = NULL", htable_hget(ht, "a", "5") == NULL);

        // hdel 
        expect("hdel a:1 action", htable_hdel(ht, "a", "1"));
        expect("hdel a:1 proof", htable_hlen(ht, "a") == 3 &&
                                 htable_hget(ht, "a", "1") == NULL);
        expect("hdel a:2 action", htable_hdel(ht, "a", "2"));
        expect("hdel a:1 proof", htable_hlen(ht, "a") == 2 &&
                                 htable_hget(ht, "a", "2") == NULL);
        expect("hdel a:3 action", htable_hdel(ht, "a", "3"));
        expect("hdel a:4 action", htable_hdel(ht, "a", "4"));
        expect("hdel a:5 action", !htable_hdel(ht, "a", "4"));
        expect("items deleted hash deleted", !htable_exists(ht, "a"));
    });
    htable_free(ht);
}

static void test_list_funcs() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_case("test list functions", {
        htable_push(ht, "a", "1", LEFT); htable_push(ht, "a", "2", LEFT);
        htable_push(ht, "a", "3", RIGHT); htable_push(ht, "a", "4", RIGHT);
        expect("used 1, size 2", ht->used == 1 && ht->size == 2);

        // llen
        expect("list len = 4", htable_llen(ht, "a") == 4);

        // lrange 
        char **lrange = htable_lrange(ht, "a", 0, 3);
        expect("lrange a 0 4 pt.1", strcmp(lrange[0], "2") == 0 &&
                                    strcmp(lrange[1], "1") == 0);
        expect("lrange a 0 4 pt.2", strcmp(lrange[2], "3") == 0 &&
                                    strcmp(lrange[3], "4") == 0);
        lrange = htable_lrange(ht, "a", 0, 2);
        expect("lrange a 0 2", strcmp(lrange[0], "2") == 0 &&
                               strcmp(lrange[1], "1") == 0);
        lrange = htable_lrange(ht, "a", 0, 0);
        expect("lrange a 0 0", strcmp(lrange[0], "2") == 0);

        // lpos
        expect("lpos 2 == 0, 1 == 1", htable_lpos(ht, "a", "2") == 0 &&
                                      htable_lpos(ht, "a", "1") == 1);
        expect("lpos 3 == 2, 4 == 3", htable_lpos(ht, "a", "3") == 2 &&
                                      htable_lpos(ht, "a", "4") == 3);

        // (l/r)pop
        expect("list lpop 2", strcmp(htable_pop(ht, "a", LEFT), "2") == 0 &&
                              htable_llen(ht, "a") == 3);
        expect("list rpop 4", strcmp(htable_pop(ht, "a", RIGHT), "4") == 0 &&
                              htable_llen(ht, "a") == 2);

        // lindex
        expect("list index 0 == '1'",
               strcmp(htable_lindex(ht, "a", 0), "1") == 0);
        expect("list index 1 == '3'",
               strcmp(htable_lindex(ht, "a", 1), "3") == 0);

        // lset
        expect("set list[0] to hello", htable_lset(ht, "a", 0, "hello"));

        // (l/r)pop
        expect("list lpop hello",
               strcmp(htable_pop(ht, "a", LEFT), "hello") == 0 &&
               htable_llen(ht, "a") == 1);
        expect("list rpop 3", strcmp(htable_pop(ht, "a", RIGHT), "3") == 0);
        expect("items deleted list deleted", !htable_exists(ht, "a"));
        
        // lrem
        for (int i = 0; i < 5; i++) htable_push(ht, "a", "2", LEFT);
        for (int i = 0; i < 2; i++) htable_push(ht, "a", "1", LEFT);
        for (int i = 0; i < 2; i++) htable_push(ht, "a", "1", RIGHT);
        expect("pushed 9 items", htable_llen(ht, "a") == 9);
        expect("lrem first 2", htable_lrem(ht, "a", 2, "1") == 2);
        expect("lrem first 2 proof",
               strcmp(htable_lindex(ht, "a", 0), "2") == 0 &&
               htable_llen(ht, "a") == 7);
        expect("lrem last 1", htable_lrem(ht, "a", -1, "1") == 1);
        expect("lrem last 1 proof",
               htable_lindex(ht, "a", 6) == NULL && htable_llen(ht, "a") == 6);
        // lrange = htable_lrange(ht, "a", 0, 0);
        // for (int i = 0; i <= 0; i++) puts(lrange[i]);
        expect("lrem all 2", htable_lrem(ht, "a", 0, "2") == 5);
        expect("lrem all 2 proof",
               strcmp(htable_lindex(ht, "a", 0), "1") == 0 &&
               htable_llen(ht, "a") == 1);
    });
    htable_free(ht);
}

static void test_set_funcs() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    test_case("test set functions", {
        // sadd
        htable_sadd(ht, "a", "1"); htable_sadd(ht, "a", "2");
        htable_sadd(ht, "a", "3"); htable_sadd(ht, "a", "4");
        expect("used 1, size 2", ht->used == 1 && ht->size == 2);

        // sismember
        expect("can't add existing member", !htable_sadd(ht, "a", "1"));
        expect("1 ismember of a", htable_sismember(ht, "a", "1"));
        expect("2 ismember of a", htable_sismember(ht, "a", "2"));
        expect("3 ismember of a", htable_sismember(ht, "a", "3"));
        expect("4 ismember of a", htable_sismember(ht, "a", "4"));

        // smembers
        char **smembers = htable_smembers(ht, "a");
        expect("1 & 2 is member of a", strcmp(smembers[0], "1") == 0 &&
                                       strcmp(smembers[1], "2") == 0);
        expect("1 & 2 is member of a", strcmp(smembers[2], "3") == 0 &&
                                       strcmp(smembers[3], "4") == 0);

        // srem
        expect("removing a:1", htable_srem(ht, "a", "1"));
        expect("removing a:2", htable_srem(ht, "a", "2"));
        expect("removing a:3", htable_srem(ht, "a", "3"));
        expect("removing a:3 again", !htable_srem(ht, "a", "3"));
        expect("removing a:4", htable_srem(ht, "a", "4"));
        expect("items deleted set deleted", !htable_exists(ht, "a"));
    });
    htable_free(ht);
}

void test_htable() {
    test_creation();
    test_insert();
    test_delete();
    test_str_funcs();
    test_hash_funcs();
    test_list_funcs();
    test_set_funcs();
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

HashTableItem HT_DELETED;

static void htable_insert(HashTable *ht, int type, char *key, void *value);

HashTable *htable_init(int size) {
    HashTable *ht = dmalloc(sizeof(HashTable));
    ht->size = next_prime(size);
    ht->used = 0;
    ht->items = calloc(ht->size, sizeof(HashTableItem *));
    return ht;
}

static HashTableItem *item_init(int type, char *key, void *value) {
    HashTableItem *item = dmalloc(sizeof(HashTableItem));
    item->type = type;
    item->key = strdup(key);
    item->value = value;
    return item;
}

static void item_free(HashTableItem *item) {
    switch(item->type) {
        case STR_T: free(item->value); break;
        case HASH_T: htable_free((HashTable *)item->value); break;
        case LIST_T: list_free((List *)item->value); break;
        case SET_T: set_free((Set *)item->value); break;
    }
    free(item->key);
    free(item);
}

static bool is_deleted(HashTableItem *item) {
    return item == &HT_DELETED;
}

void htable_free(HashTable *ht) {
    if (ht == NULL) return;
    for (int i = 0; i < ht->size; i++) {
        HashTableItem *tmp = ht->items[i];
        if (tmp != NULL && !is_deleted(tmp)) item_free(tmp);
    }
    free(ht->items);
    free(ht);
}

static void item_swap(HashTable *new_ht, HashTableItem *item) {
    htable_insert(new_ht, item->type, item->key, item->value);
    item->value = NULL;
}

static void htable_resize(HashTable *ht, int new_size) {
    if (new_size < HT_BASE_SIZE) return;

    HashTable *new_ht = htable_init(new_size);
    for (int i = 0; i < ht->size; i++) {
        HashTableItem *cur_item = ht->items[i];
        if (cur_item != NULL && !is_deleted(cur_item)) {
            item_swap(new_ht, cur_item);
        }
    }

    int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    HashTableItem **tmp = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp;

    htable_free(new_ht);
}

static void htable_resize_up(HashTable *ht) {
    int load = ht->used * 100 / ht->size;
    if (load > 70) htable_resize(ht, ht->size * 2);
}

static void htable_resize_down(HashTable *ht) {
    int load = ht->used * 100 / ht->size;
    if (load < 10) htable_resize(ht, ht->size / 2);
}

static void htable_insert(HashTable *ht, int type, char *key, void *value) {
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        HashTableItem *cur_item = ht->items[hash];

        if (cur_item == NULL || is_deleted(cur_item)) {
            ht->items[hash] = item_init(type, key, value);
            ht->used++;
            htable_resize_up(ht);
            break;
        }
    }
}

HashTableItem *htable_search(HashTable *ht, char *key) {
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        HashTableItem *cur_item = ht->items[hash];
        if (cur_item == NULL) return NULL;
        if (!is_deleted(cur_item) && strcmp(cur_item->key, key) == 0) {
            return cur_item;
        }
    }
    return NULL;
}

bool htable_exists(HashTable *ht, char *key) {
    HashTableItem *item = htable_search(ht, key);
    return item != NULL ? true : false;
}

char *htable_type(HashTable *ht, char *key) {
    HashTableItem *item = htable_search(ht, key);
    if (item == NULL) return strdup("none");
    switch (item->type) {
        case STR_T: return strdup("string");
        case HASH_T: return strdup("hash");
        case LIST_T: return strdup("list");
        case SET_T: return strdup("set");
    }
    return NULL;
}

bool htable_del(HashTable *ht, char *key) {
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        HashTableItem *cur_item = ht->items[hash];

        if (cur_item == NULL) return false;

        if (!is_deleted(cur_item) && strcmp(cur_item->key, key) == 0) {
            item_free(cur_item);
            ht->items[hash] = &HT_DELETED;
            ht->used--;
            htable_resize_down(ht);
            return true;
        }
    }
    return false;
}

static void htable_update_str(HashTable *ht, char *key, void *value) {
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        HashTableItem *cur_item = ht->items[hash];
        if (cur_item == NULL) break;
        if (!is_deleted(cur_item) && strcmp(cur_item->key, key) == 0) {
            item_free(cur_item);
            ht->items[hash] = item_init(STR_T, key, value);
            break;
        }
    }
}

static bool htable_update_hash(HashTable *ht, char *key,
                               char *field, char *value) {
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        HashTableItem *cur_item = ht->items[hash];
        if (cur_item == NULL) break;
        if (!is_deleted(cur_item) && strcmp(cur_item->key, key) == 0) {
            HashTable *tmp = (HashTable *)cur_item->value;
            return htable_set(tmp, field, value);
        }
    }
    return false;
}

static int htable_update_list(HashTable *ht, char *key, char *value, int dir) {
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        HashTableItem *cur_item = ht->items[hash];
        if (cur_item == NULL) break;
        if (!is_deleted(cur_item) && strcmp(cur_item->key, key) == 0) {
            List *tmp = (List *)cur_item->value;
            dir == LEFT ? list_lpush(tmp, value) : list_rpush(tmp, value);
            return tmp->len;
        }
    }
    return 0;
}

static bool htable_update_set(HashTable *ht, char *key, char *value) {
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        HashTableItem *cur_item = ht->items[hash];
        if (cur_item == NULL) break;
        if (!is_deleted(cur_item) && strcmp(cur_item->key, key) == 0) {
            Set *tmp = (Set *)cur_item->value;
            return set_add(tmp, value);
        }
    }
    return false;
}

bool htable_set(HashTable *ht, char *key, char *value) {
    // allocate mem for str constants
    char *dup = strdup(value);
    if (htable_exists(ht, key)) {
        htable_update_str(ht, key, dup);
        return false;
    }
    htable_insert(ht, STR_T, key, dup);
    return true;
}

bool htable_hset(HashTable *ht, char *key, char *field, char *value) {
    if (htable_exists(ht, key)) {
        return htable_update_hash(ht, key, field, value);
    }
    HashTable *new_ht = htable_init(HT_BASE_SIZE);
    htable_set(new_ht, field, value);
    htable_insert(ht, HASH_T, key, new_ht);
    return true;
}

int htable_push(HashTable *ht, char *key, char *value, int dir) {
    if (htable_exists(ht, key)) {
        return htable_update_list(ht, key, value, dir);
    }
    List *new_ls = list_init();
    dir == LEFT ? list_lpush(new_ls, value) : list_rpush(new_ls, value);
    htable_insert(ht, LIST_T, key, new_ls);
    return 1;
}

bool htable_sadd(HashTable *ht, char *key, char *value) {
    if (htable_exists(ht, key)) {
        return htable_update_set(ht, key, value);
    }
    Set *new_st = set_init(HT_BASE_SIZE);
    set_add(new_st, value);
    htable_insert(ht, SET_T, key, new_st);
    return true;
}

static bool is_type(char *given, char *expected) {
    return strcmp(given, expected) == 0 || strcmp(given, "none") == 0;
}

char *htable_get(HashTable *ht, char *key) {
    HashTableItem *res = htable_search(ht, key);
    return res != NULL ? (char *)res->value : NULL;
}

char *htable_hget(HashTable *ht, char *key, char *field) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return NULL;
    HashTable *tmp_ht = (HashTable *)tmp->value;
    return htable_get(tmp_ht, field);
}

char *htable_pop(HashTable *ht, char *key, int dir) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return NULL;
    List *tmp_ls = (List *)tmp->value;
    ListNode *tmp_nd = dir == LEFT ? list_lpop(tmp_ls) : list_rpop(tmp_ls);
    char *res = tmp_nd->value;
    if (tmp_ls->len == 0) htable_del(ht, key);
    return res;
}

bool htable_sismember(HashTable *ht, char *key, char *value) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return false;
    Set *tmp_st = (Set *)tmp->value;
    return set_ismember(tmp_st, value);
}

int htable_hlen(HashTable *ht, char *key) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return 0;
    HashTable *tmp_ht = (HashTable *)tmp->value;
    return tmp_ht != NULL ? tmp_ht->used : 0;
}

int htable_llen(HashTable *ht, char *key) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return 0;
    List *tmp_ls = (List *)tmp->value;
    return tmp_ls != NULL ? tmp_ls->len : 0;
}

// helper function to check if index is valid for a list
// modifies id in interpreter.c
int htable_check_id(HashTable *ht, char *key, int *id) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return -1;
    List *tmp_ls = (List *)tmp->value;
    *id = *id < 0 ? tmp_ls->len + *id : *id;
    if (*id >= 0 && *id < tmp_ls->len) return 1;
    return 0;
}

// helper function for lrange
int htable_check_ids(HashTable *ht, char *key, int *begin, int *end) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return -1;
    List *tmp_ls = (List *)tmp->value;
    *begin = *begin < 0 ? tmp_ls->len + *begin : *begin;
    *end = *end < 0 ? tmp_ls->len + *end : *end;
    if ((*begin <= *end) &&
        (*begin >= 0 && *begin < tmp_ls->len) &&
        (*end >= 0 && *end < tmp_ls->len)) return 1;
    return 0;
}

char *htable_lindex(HashTable *ht, char *key, int id) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return NULL;
    List *tmp_ls = (List *)tmp->value;
    ListNode *tmp_nd = list_index(tmp_ls, id);
    return tmp_nd != NULL ? tmp_nd->value : NULL;
}

bool htable_lset(HashTable *ht, char*key, int id, char *value) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return false;
    List *tmp_ls = (List *)tmp->value;
    return list_set(tmp_ls, id, value);
}

bool htable_hdel(HashTable *ht, char *key, char *field) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return false;
    HashTable *tmp_ht = (HashTable *)tmp->value;
    bool res = htable_del(tmp_ht, field);
    if (tmp_ht->used == 0) htable_del(ht, key);
    return res;
}

int htable_lrem(HashTable *ht, char *key, int count, char *value) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return 0;
    List *tmp_ls = (List *)tmp->value;
    int res = list_rem(tmp_ls, count, value);
    if (tmp_ls->len == 0) htable_del(ht, key);
    return res;
}

bool htable_srem(HashTable *ht, char *key, char *value) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return false;
    Set *tmp_st = (Set *)tmp->value;
    bool res = set_rem(tmp_st, value);
    if (tmp_st->used == 0) htable_del(ht, key);
    return res;
}

int htable_lpos(HashTable *ht, char *key, char *value) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return -1;
    List *tmp_ls = (List *)tmp->value;
    return list_pos(tmp_ls, value);
}

char **htable_hgetall(HashTable *ht, char *key) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return NULL;
    HashTable *tmp_ht = (HashTable *)tmp->value;

    char **res = calloc((tmp_ht->used * 2 + 1), sizeof(char *));
    int id = 0;
    for (int i = 0; i < tmp_ht->size; i++) {
        HashTableItem *cur_item = tmp_ht->items[i];
        if (cur_item != NULL && !is_deleted(cur_item)) {
            res[id++] = strdup(cur_item->key);
            res[id++] = strdup(cur_item->value);
        }
        if (id == tmp_ht->used * 2) {
            res[id] = NULL;
            return res;
        }
    }
    return NULL;
}

char **htable_hkeyvals(HashTable *ht, char *key, int ky) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return NULL;
    HashTable *tmp_ht = (HashTable *)tmp->value;

    char **res = calloc(tmp_ht->used + 1, sizeof(char *));
    int id = 0;
    for (int i = 0; i < tmp_ht->size; i++) {
        HashTableItem *cur_item = tmp_ht->items[i];
        if (cur_item != NULL && !is_deleted(cur_item)) {
            res[id++] = strdup(ky ? cur_item->key : cur_item->value);
        }
        if (id == tmp_ht->used) {
            res[id] = NULL;
            return res;
        }
    }
    return NULL;
}

char **htable_lrange(HashTable *ht, char *key, int begin, int end) {
    char *type = htable_type(ht, key);
    if (is_type(type, "list")) {
        free(type);
        HashTableItem *tmp = htable_search(ht, key);
        List *tmp_ls = (List *)tmp->value;
        return list_range(tmp_ls, begin, end);
    }
    return NULL;
}

char **htable_smembers(HashTable *ht, char *key) {
    HashTableItem *tmp = htable_search(ht, key);
    if (tmp == NULL) return NULL;
    Set *tmp_st = (Set *)tmp->value;
    return set_members(tmp_st);
}


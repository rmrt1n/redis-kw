#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

HashTableItem HT_DELETED;

HashTable *htable_init(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->size = next_prime(size);
    ht->used = 0;
    ht->items = calloc(ht->size, sizeof(HashTableItem *));
    return ht;
}

static HashTableItem *item_init(int type, char *key, void *value) {
    HashTableItem *item = malloc(sizeof(HashTableItem));
    item->key = key;
    item->value = value;
    return item;
}

static void item_free(HashTableItem *item) {
    switch(item->type) {
        case STR_T: free(item->value); break;
        case HASH_T: break;
        case LIST_T: break;
        case SET_T: break;
    }
    free(item->key);
    free(item);
}

static bool is_deleted(HashTableItem *item) {
    return item == &HT_DELETED;
}

void htable_free(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        HashTableItem *tmp = ht->items[i];
        if (tmp != NULL && !is_deleted(tmp)) item_free(tmp);
    }
    free(ht->items);
    free(ht);
}

void htable_insert(HashTable *ht, int type, char *key, void *value) {
    HashTableItem *cur_item;
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        cur_item = ht->items[hash];

        if (cur_item == NULL || is_deleted(cur_item)) {
            ht->items[hash] = item_init(type, key, value);
            ht->used++;
            return;
        }
    }
}

static void htable_update_str(HashTable *ht, char *key, void *value) {
    HashTableItem *cur_item;
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        cur_item = ht->items[hash];
        if (cur_item != NULL &&
            !is_deleted(cur_item) &&
            strcmp(cur_item->key, key) == 0)
        {
            item_free(cur_item);
            ht->items[hash] = item_init(STR_T, key, value);
            break;
        }
    }
}

static void htable_update_hash(HashTable *ht, char *key,
                               char *field, char *value) {
    HashTableItem *cur_item;
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        cur_item = ht->items[hash];
        if (cur_item != NULL &&
            !is_deleted(cur_item) &&
            strcmp(cur_item->key, key) == 0)
        {
            HashTable *tmp = (HashTable *)cur_item->value;
            htable_set(tmp, field, value);
            break;
        }
    }
}

static void htable_update_list(HashTable *ht, char *key, char *value, int dir) {
    HashTableItem *cur_item;
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        cur_item = ht->items[hash];
        if (cur_item != NULL &&
            !is_deleted(cur_item) &&
            strcmp(cur_item->key, key) == 0)
        {
            List *tmp = (List *)cur_item->value;
            dir == LEFT ? list_lpush(tmp, value) : list_rpush(tmp, value);
            break;
        }
    }
}

static bool htable_update_set(HashTable *ht, char *key, char *value) {
    HashTableItem *cur_item;
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        cur_item = ht->items[hash];
        if (cur_item != NULL &&
            !is_deleted(cur_item) &&
            strcmp(cur_item->key, key) == 0)
        {
            Set *tmp = (Set *)cur_item->value;
            return set_add(tmp, value);
        }
    }
    return false;
}

bool htable_exists(HashTable *ht, char *key) {
    HashTableItem *cur_item;
    for (int i = 0; i < ht->size; i++) {
        int hash = hash_func(key, ht->size, i);
        cur_item = ht->items[hash];
        if (cur_item != NULL && !is_deleted(cur_item))
            if (strcmp(cur_item->key, key) == 0) return true;
    }
    return false;
}

void htable_set(HashTable *ht, char *key, char *value) {
    if (htable_exists(ht, key)) {
        htable_update_str(ht, key, value);
    } else {
        htable_insert(ht, STR_T, key, value);
    }
}

void htable_hset(HashTable *ht, char *key, char *field, char *value) {
    if (htable_exists(ht, key)) {
        htable_update_hash(ht, key, field, value);
    } else {
        HashTable *new_ht = htable_init(HT_BASE_SIZE);
        htable_insert(new_ht, STR_T, key, value);
        htable_insert(ht, HASH_T, key, new_ht);
    }
}

void htable_push(HashTable *ht, char *key, char *value, int dir) {
    if (htable_exists(ht, key)) {
        htable_update_list(ht, key, value, dir);
    } else {
        List *new_ls = list_init();
        dir == LEFT ? list_lpush(new_ls, value) : list_rpush(new_ls, value);
        htable_insert(ht, LIST_T, key, new_ls);
    }
}

void htable_sadd(HashTable *ht, char *key, char *value) {
    if (htable_exists(ht, key)) {
        htable_update_set(ht, key, value);
    } else {
        Set *new_st = set_init(HT_BASE_SIZE);
        set_add(new_st, value);
        htable_insert(ht, SET_T, key, new_st);
    }
}


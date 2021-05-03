#include <stdio.h>
#include "common.h"

int main() {
    HashTable *ht = htable_init(2);
    htable_set(ht, "a", "str");
    printf("used %d, size %d\n", ht->used, ht->size);
    htable_hset(ht, "b", "1", "hash");
    printf("used %d, size %d\n", ht->used, ht->size);
    htable_push(ht, "c", "left", LEFT);
    printf("used %d, size %d\n", ht->used, ht->size);
    htable_sadd(ht, "d", "set");
    printf("used %d, size %d\n", ht->used, ht->size);

    // for (int i = 0; i < ht->size; i++) {
        // HashTableItem *tmp = ht->items[i];
        // if (tmp != NULL) printf("key %s, type %d\n", tmp->key, tmp->type); 
    // }
    
    htable_del(ht, "a");
    printf("used %d, size %d\n", ht->used, ht->size);
    htable_del(ht, "b");
    printf("used %d, size %d\n", ht->used, ht->size);
    htable_del(ht, "c");
    printf("used %d, size %d\n", ht->used, ht->size);
    htable_del(ht, "d");
    printf("used %d, size %d\n", ht->used, ht->size);
    return 0;
}

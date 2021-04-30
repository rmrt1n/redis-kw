#include <stdio.h>
#include "common.h"

int main() {
    HashTable *ht = htable_init(5);
    htable_set(ht, "a", "1");
    htable_hset(ht, "b", "1", "2");
    htable_push(ht, "c", "1", LEFT);
    htable_sadd(ht, "d", "1");
    for (int i = 0; i < ht->size; i++) {
        HashTableItem *tmp = ht->items[i];
        if (tmp != NULL) printf("key %s\n", tmp->key); 
    }
    return 0;
}

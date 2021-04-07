#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"

static ulong hash_func(char *str, int arr_size) {
    unsigned long hash = 5381;
    int c;
    // hash = 5381 * 33 + c
    while ((c = *str++)) hash = ((hash << 5) + hash) + c; 
    return hash % arr_size;
}

HashTable *htable_init(int size) {
    HashTable *htable = malloc(sizeof(HashTable));
    
    if (htable == NULL) {
        fputs("unable to create hash table", stderr);
        exit(1);
    }

    htable->size = size;
    htable->entries = malloc(size * sizeof(Entry *));

    return htable;
}

Entry *entry_init(int type, char *key) {
    Entry *entry = malloc(sizeof(Entry));
    entry->type = type;
    entry->key = strdup(key);
    return entry;
}

void htable_free(HashTable *htable) {
    if (htable == NULL) return;
    for (int i = 0; i < htable->size; i++) {
        if (htable->entries[i] != NULL) {
            while (htable->entries[i] != NULL) {
                Entry *tmp = htable->entries[i]->next;
                switch (htable->entries[i]->type) {
                    case STR:
                        free(htable->entries[i]->key);
                        free(htable->entries[i]->value);
                        free(htable->entries[i]);
                        break;
                    case HASH:
                        htable_free((HashTable *)htable->entries[i]->value);
                        break;
                }
                htable->entries[i] = tmp;
            }
            free(htable->entries[i]);
        }
    }
    free(htable->entries);
    free(htable);
}

HtableAction htable_set(HashTable *htable, char *key, char *value) {
    Entry *keyval = entry_init(STR, key);
    keyval->value = strdup(value);

    ulong hash = hash_func(key, htable->size);
    if (htable->entries[hash] != NULL) {
        Entry *tmp = htable->entries[hash];
        while (tmp != NULL && strcmp(tmp->key, keyval->key) != 0) {
            tmp = tmp->next;
        }

        if (tmp != NULL) {
            if (tmp->type == STR) {
                free(tmp->value);
                tmp->value = strdup(keyval->value);
                free(keyval->key);
                free(keyval->value);
                free(keyval);
            } else {
                HtableAction result = {.status = ERR, .value = NULL};
                return result;
            }
        } else {
            keyval->next = htable->entries[hash];
            htable->entries[hash] = keyval;
        }
    } else {
        keyval->next = NULL;
        htable->entries[hash] = keyval;
    }
    
    HtableAction result = {.status = SUCCESS, .value = NULL};
    return result;
}

HtableAction htable_get(HashTable *htable, char *key) {
    HtableAction result;
    ulong hash = hash_func(key, htable->size);

    Entry *tmp = htable->entries[hash];
    while (tmp != NULL && strcmp(tmp->key, key) != 0) tmp = tmp->next;

    if (tmp == NULL) {
        result = (HtableAction){.status = NIL, .value = NULL};
    } else {
        if (tmp->type == STR) {
            result.status = SUCCESS;
            result.value = malloc(strlen((char *)tmp->value) + 1);
            strcpy(result.value, (char *)tmp->value);
        } else {
            result = (HtableAction){.status = ERR, .value = NULL};
        }
    }

    return result;
}

HtableAction htable_del(HashTable *htable, char *key) {
    HtableAction result;
    ulong hash = hash_func(key, htable->size);

    if (htable->entries[hash] != NULL) {
        Entry *tmp = htable->entries[hash], *tmp_prev = NULL;

        if (tmp->next == NULL) {
            if (tmp->type == STR) {
                free(tmp->key);
                free(tmp->value);
                free(tmp);
                htable->entries[hash] = NULL;
                result = (HtableAction){.status = SUCCESS, .value = NULL};
            } else if (tmp->type == HASH) {
                free(tmp->key);
                htable_free((HashTable *)tmp->value);
                free(tmp);
                htable->entries[hash] = NULL;
                result = (HtableAction){.status = SUCCESS, .value = NULL};
            } else {
                result = (HtableAction){.status = ERR, .value = NULL};
            }
        } else {
            while (tmp != NULL && strcmp(tmp->key, key) != 0) {
                tmp_prev = tmp;
                tmp = tmp->next; 
            }
            if (tmp != NULL) {
                if (tmp->type == STR) {
                    tmp_prev->next = tmp->next;
                    free(tmp->key); 
                    free(tmp->value); 
                    free(tmp);
                    result = (HtableAction){.status = SUCCESS, .value = NULL};
                } else if (tmp->type == HASH){
                    free(tmp->key);
                    htable_free((HashTable *)tmp->value);
                    free(tmp);
                    result = (HtableAction){.status = SUCCESS, .value = NULL};
                } else {
                    result = (HtableAction){.status = ERR, .value = NULL};
                }
            } else {
                result = (HtableAction){.status = NIL, .value = NULL};
            }
        }
    } else {
        result = (HtableAction){.status = NIL, .value = NULL};
    }

    return result;
}

HtableAction htable_hset(HashTable *htable, char *key, char *field, char *value) {
    Entry *keyval = entry_init(HASH, key);
    keyval->value = htable_init(4096);

    ulong hash = hash_func(key, htable->size);
    if (htable->entries[hash] != NULL) {
        Entry *tmp = htable->entries[hash];
        while (tmp != NULL && strcmp(tmp->key, keyval->key) != 0) {
            tmp = tmp->next;
        } 

        if (tmp != NULL) {
            if (tmp->type == HASH) {
                htable_set((HashTable *)tmp->value, field, value);
                free(keyval->key);
                htable_free((HashTable *)keyval->value);
                free(keyval);
            } else {
                HtableAction result = {.status = ERR, .value = NULL};
                return result;
            }
        } else {
            htable_set((HashTable *)keyval->value, field, value);
            keyval->next = htable->entries[hash];
            htable->entries[hash] = keyval;
        }

    } else {
        htable_set((HashTable *)keyval->value, field, value);
        keyval->next = NULL;
        htable->entries[hash] = keyval;
    }

    HtableAction result = {.status = SUCCESS, .value = NULL};
    return result;
}

HtableAction htable_hget(HashTable *htable, char *key, char *field) {
    HtableAction result;
    ulong hash = hash_func(key, htable->size);

    Entry *tmp = htable->entries[hash];
    if (tmp == NULL) {
        result = (HtableAction){.status = NIL, .value = NULL};
    } else {
        if (tmp->type == HASH) {
            HtableAction instr = htable_get((HashTable *)tmp->value, field);
            result = instr;
        } else {
            result = (HtableAction){.status = ERR, .value = NULL};
        }
    }
    
    return result;
}

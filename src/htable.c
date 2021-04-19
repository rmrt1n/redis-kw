#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

Item HTABLE_DELETED = {0, NULL, NULL};

HashTable *htable_init(int size) {
    HashTable *htable = malloc(sizeof(HashTable));
    htable->size = next_prime(size);
    htable->used = 0;
    htable->keys = calloc(1, sizeof(int));
    htable->items = calloc(htable->size, sizeof(Item *));
    return htable;
}

Item *item_init(int type, char *key, void *value) {
    Item *item = malloc(sizeof(Item));
    item->type = type;
    item->key = strdup(key);
    switch (type) {
        case STR:
            item->value = strdup((char *)value);
            break;
        case HASH:
            item->value = htable_init(HTABLE_BASE_SIZE);
            break;
        case LIST:
            item->value = list_init();
            break;
        case TSET:
            item->value = set_init(HTABLE_BASE_SIZE);
            break;
    }
    return item;
}

void item_free(Item *item) {
    switch (item->type) {
        case STR:
            free_all(3, item->key, item->value, item);
            break;
        case HASH:
            htable_free((HashTable *)item->value);
            free_all(2, item->key, item);
            break;
        case LIST:
            list_free((List *)item->value);
            break;
        case TSET:
            set_free((Set *)item->value);
            break;
    }
}

void htable_free(HashTable *htable) {
    for (int i = 0; i < htable->size; i++) {
        Item *tmp = htable->items[i];
        if (tmp != NULL && tmp != &HTABLE_DELETED) item_free(tmp);
        if (tmp == &HTABLE_DELETED) tmp = NULL;
    }
    free_all(3, htable->keys, htable->items, htable);
}

void htable_resize(HashTable *htable, int new_size) {
    if (new_size < HTABLE_BASE_SIZE) return;

    HashTable *new_ht = htable_init(new_size);
    for (int i = 0; i < htable->used; i++) {
        Item *tmp = htable->items[htable->keys[i]];
        switch (tmp->type) {
            case STR:
                htable_set(new_ht, tmp->key, (char *)tmp->value);
                break;
            case HASH:;
                HashTable *tmp_ht = (HashTable *) tmp->value;
                for (int j = 0; j < tmp_ht->used; j++) {
                    Item *tmp_itm = tmp_ht->items[tmp_ht->keys[j]];
                    htable_hset(new_ht, tmp->key,
                                tmp_itm->key, (char *)tmp_itm->value);
                }
                break;
            case LIST:;
                List *tmp_ls = (List *) tmp->value;
                Node *tmp_nd = tmp_ls->head;
                while (tmp_nd != NULL) {
                    htable_push(new_ht, tmp->key, tmp_nd->value, LEFT);
                    tmp_nd = tmp_nd->next;
                }
                break;
            case TSET:;
                Set *tmp_st = (Set *) tmp->value;
                for (int i = 0; i < tmp_st->used; i++) {
                    char *tmp_m = (char *)tmp_st->members[tmp_st->keys[i]];
                    htable_sadd(new_ht, tmp->key, tmp_m);
                }
                break;
        }
    }
    
    // swap size
    int tmp_size = htable->size;
    htable->size = new_ht->size;
    new_ht->size = tmp_size;

    // swap keys
    int *tmp_keys = htable->keys;
    htable->keys = new_ht->keys;
    new_ht->keys = tmp_keys;

    // swap items
    Item **tmp_itms = htable->items;
    htable->items = new_ht->items;
    new_ht->items = tmp_itms;

    htable_free(new_ht);
}

void htable_resize_up(HashTable *htable) {
    int load = htable->used * 100 / htable->size;
    if (load > 70) {
        int new_size = htable->size * 2;
        htable_resize(htable, new_size);
    }
}

void htable_resize_down(HashTable *htable) {
    int load = htable->used * 100 / htable->size;
    if (load < 10) {
        int new_size = htable->size / 2;
        htable_resize(htable, new_size);
    }
}

int djb2(char *str, int size) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash % size;
}

int sdbm(char *str, int size) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++)) hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % size;
}

int hash_func(char *key, int size, int i) {
    int hash = djb2(key, size);
    int res = i == 0 ? hash : hash + (i * (sdbm(key, size)));
    return res % size;
}

void htable_add_key(HashTable *htable, int keyhash) {
    htable->keys = realloc(htable->keys, htable->used * sizeof(int));
    htable->keys[htable->used - 1] = keyhash;
}

void htable_del_key(HashTable *htable, int keyhash) {
    int n = htable->used + 1;
    int *newarr = calloc(htable->used, sizeof(int));

    int id = 0;
    for (int i = 0; i < n; i++) {
        if (htable->keys[i] == keyhash) continue;
        newarr[id++] = htable->keys[i];
    }

    free(htable->keys);
    htable->keys = newarr;
}

HashTable *get_ht_ref(HashTable *htable, char *key) {
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];
    HashTable *ref;

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    not_null ? ref = (HashTable *)htable->items[hash]->value : NULL;
    // ref = (HashTable *)cur_item->value;
    return ref;
}

HtableAction htable_set(HashTable *htable, char *key, char *value) {
    htable_resize_up(htable);
    Item *new_item = item_init(STR, key, value);
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            item_free(cur_item);
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    htable->items[hash] = new_item;
    if (!not_null) {
        htable->used++;
        htable_add_key(htable, hash);
    }

    return (HtableAction){OK, NULL};
}

HtableAction htable_get(HashTable *htable, char *key) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];
    
    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == STR) {
                result.status = OK;
                result.value = strdup(cur_item->value);
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}

HtableAction htable_del(HashTable *htable, char *key) {
    htable_resize_down(htable);
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            item_free(cur_item);
            htable->items[hash] = &HTABLE_DELETED;
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    if (not_null) {
        htable->used--;
        htable_del_key(htable, hash);
        result = (HtableAction){OK, NULL};
    } else {
        result = (HtableAction){NIL, NULL};
    }

    return result;
}

HtableAction htable_hset(HashTable *htable, char *key, char *field, char *value) {
    htable_resize_up(htable);
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == HASH) {
                result = (HtableAction){OK, NULL};
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    
    result = not_null ? result : (HtableAction){OK, NULL};
    if (result.status == ERR) return result;

    if (not_null) {
        htable_set((HashTable *)cur_item->value, field, value);
    } else {
        Item *new_item = item_init(HASH, key, NULL);
        HashTable *tmp = (HashTable *)new_item->value;
        htable_set(tmp, field, value);
        htable->items[hash] = new_item;
        htable->used++;
        htable_add_key(htable, hash);
    }

    return result;
}

HtableAction htable_hget(HashTable *htable, char *key, char *field) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == HASH) {
                result = htable_get((HashTable *)cur_item->value, field);
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }
    
    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}

HtableAction htable_hgetall(HashTable *htable, char *key) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];
    char **vals = malloc(sizeof(char *));

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == HASH) {
                HashTable *ht = (HashTable *)cur_item->value;
                int size = 1, id = 0;
                for (int j = 0; j < ht->used; j++) {
                    int hash = ht->keys[j];
                    size += 2;
                    vals = realloc(vals, size * sizeof(char *));
                    vals[id++] = strdup(ht->items[hash]->key);
                    vals[id++] = strdup((char *)ht->items[hash]->value);
                }
                vals = realloc(vals, (size + 1) * sizeof(char *));
                vals[ht->used * 2] = NULL;
                vals[ht->used * 2 + 1] = NULL; // for hvals
                result = (HtableAction){OK, vals};
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }
    
    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}

HtableAction htable_hdel(HashTable *htable, char *key, char *field) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == HASH) {
                result = htable_del((HashTable *)cur_item->value, field);
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    if (result.status == OK) {
        HashTable *tmp = (HashTable *)cur_item->value;
        if (tmp->used == 0) htable_del(htable, key);
    }

    return result;
}

HtableAction htable_exists(HashTable *htable, char *key) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            result = (HtableAction){OK, NULL};
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}

HtableAction htable_type(HashTable *htable, char *key) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            result.status = OK;
            switch (cur_item->type) {
                case STR: result.value = strdup("string"); break;
                case HASH: result.value = strdup("hash"); break;
                case LIST: result.value = strdup("list"); break;
                case TSET: result.value = strdup("set"); break;
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}


HtableAction htable_push(HashTable *htable, char *key, char *value, int dir) {
    htable_resize_up(htable);
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == LIST) {
                result = (HtableAction){OK, NULL};
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        } 
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){OK, NULL};
    if (result.status == ERR) return result;

    if (not_null) {
        List *tmp = (List *)htable->items[hash]->value;
        list_push(tmp, value, dir); 
    } else {
        Item *new_item = item_init(LIST, key, value);
        List *tmp = (List *)new_item->value;
        list_push(tmp, value, dir); 
        htable->items[hash] = new_item;
        htable->used++;
        htable_add_key(htable, hash);
    }

    return result;
}

HtableAction htable_pop(HashTable *htable, char *key, int dir) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == LIST) {
                List *tmp = (List *)htable->items[hash]->value;
                Node *popped = list_pop(tmp, dir);
                result.status = OK;
                result.value = strdup(popped->value);
                list_node_free(popped);
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    if (result.status == OK) {
        List *tmp = (List *)htable->items[hash]->value;
        if (tmp->head == NULL) htable_del(htable, key); 
    }

    return result;
}

HtableAction htable_llen(HashTable *htable, char *key) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == LIST) {
                List *tmp = (List *)htable->items[hash]->value;
                result.status = OK;
                result.value = &tmp->len;
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}

HtableAction htable_sadd(HashTable *htable, char *key, char *value) {
    htable_resize_up(htable);
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == TSET) {
                result = (HtableAction){OK, NULL};
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){OK, NULL};
    if (result.status == ERR) return result;

    if (not_null) {
        result = set_add((Set *)cur_item->value, value);
    } else {
        Item *new_item = item_init(TSET, key, NULL);
        Set *tmp = (Set *)new_item->value;
        result = set_add(tmp, value);
        htable->items[hash] = new_item;
        htable->used++;
        htable_add_key(htable, hash);
    }

    return result;
}

HtableAction htable_srem(HashTable *htable, char *key, char *value) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == TSET) {
                result = set_rem((Set *)cur_item->value, value);
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    if (result.status == OK) {
        Set *tmp = (Set *)cur_item->value;
        if (tmp->used == 0) htable_del(htable, key);
    }

    return result;
}

HtableAction htable_sismember(HashTable *htable, char *key, char *value) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == TSET) {
                result = set_get((Set *)cur_item->value, value);
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}

HtableAction htable_smembers(HashTable *htable, char *key) {
    HtableAction result;
    int hash = hash_func(key, htable->size, 0);
    Item *cur_item = htable->items[hash];
    char **vals = malloc(sizeof(char *));

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &HTABLE_DELETED) {
        if (strcmp(cur_item->key, key) == 0) {
            if (cur_item->type == TSET) {
                Set *st = (Set *)cur_item->value;
                int id = 1;
                for (int j = 0; j < st->used; j++) {
                    int hash = st->keys[j];
                    vals = realloc(vals, (++id) * sizeof(char *));
                    vals[j] = strdup((char *)st->members[hash]);
                }
                vals[st->used] = NULL;
                result = (HtableAction){OK, vals};
            } else {
                result = (HtableAction){ERR, NULL};
            }
            not_null++;
            break;
        }
        hash = hash_func(key, htable->size, i++);
        cur_item = htable->items[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}


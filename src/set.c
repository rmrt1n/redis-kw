#include <stdlib.h>
#include <string.h>
#include "common.h"

char SET_DELETED = 0;

Set *set_init(int size) {
    Set *set = malloc(sizeof(Set));
    set->size = next_prime(size);
    set->used = 0;
    set->keys = calloc(1, sizeof(int));
    set->members = calloc(set->size, sizeof(char *));
    return set;
}

void set_free(Set *set) {
    for (int i = 0; i < set->size; i++) {
        char *tmp = set->members[i];
        if (tmp != NULL && tmp != &SET_DELETED) free_all(1, tmp);
    }
    free_all(3, set->keys, set->members, set);
}

void set_resize(Set *set, int new_size) {
    if (new_size < HTABLE_BASE_SIZE) return;

    Set *new_st = set_init(new_size);
    for (int i = 0; i < set->used; i++) {
        char *tmp = set->members[set->keys[i]];
        set_add(new_st, tmp);
    }

    int tmp_size = set->size;
    set->size = new_st->size;
    new_st->size = tmp_size;
    
    int *tmp_keys = set->keys;
    set->keys = new_st->keys;
    new_st->keys = tmp_keys;

    char **tmp_members = set->members;
    set->members = new_st->members;
    new_st->members = tmp_members;

    set_free(new_st);
}

void set_resize_up(Set *set) {
    int load = set->used * 100 / set->size;
    if (load > 70) {
        int new_size = set->size * 2;
        set_resize(set, new_size);
    }
}

void set_resize_down(Set *set) {
    int load = set->used * 100 / set->size;
    if (load < 10) {
        int new_size = set->size / 2;
        set_resize(set, new_size);
    }
}

void set_add_key(Set *set, int keyhash) {
    set->keys = realloc(set->keys, set->used * sizeof(int));
    set->keys[set->used - 1] = keyhash;
}

void set_del_key(Set *set, int keyhash) {
    int n = set->used + 1;
    int *newarr = calloc(set->used, sizeof(int));

    int id = 0;
    for (int i = 0; i < n; i++) {
        if (set->keys[i] == keyhash) continue;
        newarr[id++] = set->keys[i];
    }

    free(set->keys);
    set->keys = newarr;
}

HtableAction set_add(Set *set, char *value) {
    set_resize_up(set);
    HtableAction result = {OK, NULL};
    int hash = hash_func(value, set->size, 0);
    char *cur_item = set->members[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &SET_DELETED) {
        if (strcmp(cur_item, value) == 0) {
            result = (HtableAction){NIL, NULL};
            not_null++;
            break;
        }
        hash = hash_func(value, set->size, i++);
        cur_item = set->members[hash];
    }

    if (!not_null) {
        set->members[hash] = strdup(value); 
        set->used++;
        set_add_key(set, hash);
    }

    return result;
}

HtableAction set_get(Set *set, char *value) {
    HtableAction result;
    int hash = hash_func(value, set->size, 0);
    char *cur_item = set->members[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &SET_DELETED) {
        if (strcmp(cur_item, value) == 0) {
            result.status = OK;
            result.value = strdup(cur_item);
            not_null++;
            break;
        }
        hash = hash_func(value, set->size, i++);
        cur_item = set->members[hash];
    }

    result = not_null ? result : (HtableAction){NIL, NULL};
    return result;
}

HtableAction set_rem(Set *set, char *value) {
    set_resize_down(set);
    HtableAction result;
    int hash = hash_func(value, set->size, 0);
    char *cur_item = set->members[hash];

    int i = 1, not_null = 0;
    while (cur_item != NULL && cur_item != &SET_DELETED) {
        if (strcmp(cur_item, value) == 0) {
            free_all(1, cur_item);
            set->members[hash] = &SET_DELETED;
            not_null++;
            break;
        }
        hash = hash_func(value, set->size, i++);
        cur_item = set->members[hash];
    }

    if (not_null) {
        set->used--;
        set_del_key(set, hash);
        result = (HtableAction){OK, NULL};
    } else {
        result = (HtableAction){NIL, NULL};
    }

    return result;
}



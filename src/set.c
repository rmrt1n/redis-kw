#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

char SET_DELETED;

Set *set_init(int size) {
    Set *set = dmalloc(sizeof(Set));
    set->size = next_prime(size);
    set->used = 0;
    set->members = calloc(set->size, sizeof(Set *));
    return set;
}

static bool is_deleted(char *member) {
    return member == &SET_DELETED;
}

void set_free(Set *set) {
    if (set == NULL) return;
    for (int i = 0; i < set->size; i++) {
        char *tmp = set->members[i];
        if (tmp != NULL && !is_deleted(tmp)) free(tmp);
    }
    free(set->members);
    free(set);
}

static void set_resize(Set *set, int new_size) {
    if (new_size < HT_BASE_SIZE) return;

    Set *new_st = set_init(new_size);
    for (int i = 0; i < set->size; i++) {
        char *cur_item = set->members[i];
        if (cur_item != NULL && !is_deleted(cur_item)) {
            set_add(new_st, cur_item);
            cur_item = NULL;
        }
    }

    int tmp_size = set->size;
    set->size = new_st->size;
    new_st->size = tmp_size;

    char **tmp = set->members;
    set->members = new_st->members;
    new_st->members = tmp;

    set_free(new_st);
}

static void set_resize_up(Set *set) {
    int load = set->used * 100 / set->size;
    if (load > 70) set_resize(set, set->size * 2);
}

static void set_resize_down(Set *set) {
    int load = set->used * 100 / set->size;
    if (load < 10) set_resize(set, set->size / 2);
}

bool set_add(Set *set, char *key) {
    for (int i = 0; i < set->size; i++) {
        int hash = hash_func(key, set->size, i);
        char *cur_item = set->members[hash];
        if (cur_item == NULL || is_deleted(cur_item)) {
            set->members[hash] = strdup(key);
            set->used++;
            set_resize_up(set);
            return true;
        }
        if (strcmp(cur_item, key) == 0) return false;
    }
    return false;
}

bool set_rem(Set *set, char *key) {
    for (int i = 0; i < set->size; i++) {
        int hash = hash_func(key, set->size, i);
        char *cur_item = set->members[hash];

        if (cur_item == NULL) return false;

        if (!is_deleted(cur_item) && strcmp(cur_item, key) == 0) {
            free(cur_item);
            set->members[hash] = &SET_DELETED;
            set->used--;
            set_resize_down(set);
            return true;
        }
    }
    return false;
}

bool set_ismember(Set *set, char *key) {
    for (int i = 0; i < set->size; i++) {
        int hash = hash_func(key, set->size, i);
        char *cur_item = set->members[hash];
        if (cur_item == NULL) return false;
        if (!is_deleted(cur_item) && strcmp(cur_item, key) == 0) return true;
    }
    return false;
}

char **set_members(Set *set) {
    char **members = calloc(set->size + 1, sizeof(char *));
    int id = 0;
    for (int i = 0; i < set->size; i++) {
        char *cur_item = set->members[i];
        if (cur_item != NULL && !is_deleted(cur_item)) {
            members[id++] = strdup(cur_item);
        }
    }
    return members;
}


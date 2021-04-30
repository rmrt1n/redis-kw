#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

char SET_DELETED;

Set *set_init(int size) {
    Set *set = malloc(sizeof(Set));
    set->size = next_prime(size);
    set->used = 0;
    set->members = calloc(set->size, sizeof(Set *));
    return set;
}

static bool is_deleted(char *member) {
    return member == &SET_DELETED;
}

void set_free(Set *set) {
    for (int i = 0; i < set->size; i++) {
        char *tmp = set->members[i];
        if (tmp != NULL && !is_deleted(tmp)) free(tmp);
    }
    free(set->members);
    free(set);
}

bool set_add(Set *set, char *value) {
    char *cur_item;
    for (int i = 0; i < set->size; i++) {
        int hash = hash_func(value, set->size, i);
        cur_item = set->members[hash];
        if (cur_item == NULL || is_deleted(cur_item)) {
            set->members[hash] = value;
            set->used++;
            return true;
        }
        if (strcmp(cur_item, value) == 0) return false;
    }
    return false;
}


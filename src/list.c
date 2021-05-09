#include <stdlib.h>
#include <string.h>
#include "common.h"

List *list_init() {
    List *ls = dmalloc(sizeof(List));
    ls->len = 0;
    ls->head = ls->tail = NULL;
    return ls;
}

static ListNode *node_init(char *value) {
    ListNode *node = dmalloc(sizeof(ListNode));
    node->value = strdup(value);
    node->next = node->prev = NULL;
    return node;
}

static void node_free(ListNode *node) {
    free(node->value);
    free(node);
}

void list_free(List *ls) {
    if (ls == NULL) return;
    ListNode *next, *cur = ls->head;
    while (ls->len--) {
        next = cur->next;
        node_free(cur);
        cur = next;
    }
    free(ls);
}

void list_lpush(List *ls, char *value) {
    ListNode *new_node = node_init(value);
    if (ls->len > 0) {
        new_node->next = ls->head;
        ls->head->prev = new_node;
        ls->head = new_node;
    } else {
        ls->head = ls->tail = new_node;
    }
    ls->len++;
}

void list_rpush(List *ls, char *value) {
    ListNode *new_node = node_init(value);
    if (ls->len > 0) {
        new_node->prev = ls->tail;
        ls->tail->next = new_node;
        ls->tail = new_node;
    } else {
        ls->tail = ls->head = new_node;
    }
    ls->len++;
}

ListNode *list_lpop(List *ls) {
    if (ls->len <= 0) return NULL;
    ListNode *node = ls->head;
    if (ls->len - 1 > 0) {
        ls->head = node->next;
        ls->head->prev = NULL;
    } else {
        ls->head = ls->tail = NULL;
    }
    ls->len--;
    node->next = node->prev = NULL;
    return node;
}

ListNode *list_rpop(List *ls) {
    if (ls->len <= 0) return NULL;
    ListNode *node = ls->tail;
    if (ls->len - 1 > 0) {
        ls->tail = node->prev;
        ls->tail->next = NULL;
    } else {
        ls->head = ls->tail = NULL;
    }
    ls->len--;
    node->next = node->prev = NULL;
    return node;
}

ListNode *list_index(List *ls, int id) {
    int i = 0;
    ListNode *cur = ls->head;
    while (i++ < id && cur != NULL) {
        cur = cur->next;
    }
    return cur;
}

bool list_set(List *ls, int id, char *value) {
    int i = 0;
    ListNode *cur = ls->head;
    while (i++ < id && cur != NULL) cur = cur->next;
    if (cur != NULL) {
        free(cur->value);
        cur->value = strdup(value);
        return true;
    }
    return false;
}

int list_pos(List *ls, char *value) {
    int i = 0;
    ListNode *cur = ls->head;
    while (cur != NULL) {
        if (strcmp(cur->value, value) == 0) return i;
        cur = cur->next;
        i++;
    }
    return -1;
}

static void node_del(List *ls, ListNode *nd) {
    // ListNode *tmp = nd;
    if (ls->head == NULL || nd == NULL) return;
    if (ls->len - 1 == 0) {
        ls->head = ls->tail = NULL;
        return;
    }
    if (nd == ls->head) {
        ls->head = nd->next;
        ls->head->prev = NULL;
    } else if (nd == ls->tail) {
        ls->tail = nd->prev;
        ls->tail->next = NULL;
    } else {
        nd->prev->next = nd->next;
        nd->next->prev = nd->prev;
    }
}

int list_rem(List *ls, int count, char *value) {
    int i = 0, pos = count;
    ListNode *cur = count >= 0 ? ls->head : ls->tail;
    while (cur != NULL) {
        int flag = 0;
        if (pos != 0 && count == 0) break;
        if (strcmp(cur->value, value) == 0) {
            node_del(ls, cur);
            i++;
            pos >= 0 ? count-- : count++;
            ls->len--;
            flag++;
        }
        ListNode *tmp = cur;
        cur = pos >= 0 ? cur->next : cur->prev;
        if (flag) node_free(tmp);
    }
    
    return i;
}

char **list_range(List *ls, int begin, int end) {
    ListNode *cur = ls->head;
    char **res = calloc(end - begin + 2, sizeof(char *));

    int id = 0, i = 0;
    while (id < begin) {
        id++;
        cur = cur->next;
    }

    while (id <= end) {
        res[i++] = strdup(cur->value);
        id++;
        cur = cur->next;
    }

    res[i] = NULL;
    return res;
}


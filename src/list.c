#include <stdlib.h>
#include "common.h"

List *list_init() {
    List *ls = malloc(sizeof(List));
    ls->len = 0;
    ls->head = ls->tail = NULL;
    return ls;
}

static ListNode *node_init(char *value) {
    ListNode *node = malloc(sizeof(ListNode));
    node->value = value;
    node->next = node->prev = NULL;
    return node;
}

static void node_free(ListNode *node) {
    free(node->value);
    free(node);
}

void list_free(List *ls) {
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
        ls->head = new_node;
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
        ls->tail = new_node;
    }
    ls->len++;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

Node *list_node_init(char *value) {
    Node *node = malloc(sizeof(node));
    node->value = strdup(value);
    node->next = node->prev = NULL;
    return node;
}

List *list_init() {
    List *list = malloc(sizeof(list));
    list->len = 0;
    list->head = list->tail = NULL;
    return list;
}

void list_node_free(Node *node) {
    free_all(2, node->value, node);
}

void list_free(List *list) {
    int len = list->len;
    Node *cur = list->head;
    Node *next;

    while (len--) {
        next = cur->next;
        list_node_free(cur);
        cur = next;
    }

    free_all(1, list);
}

void list_lpush(List *list, char *value) {
    Node *new_node = list_node_init(value);
    if (list->len > 0) {
        new_node->next = list->head;
        new_node->prev = NULL;
        list->head->prev = new_node;
        list->head = new_node;
    } else {
        list->head = list->tail = new_node;
        new_node->next = new_node->prev = NULL;
    }
    list->len++;
}

void list_rpush(List *list, char *value) {
    Node *new_node = list_node_init(value);
    if (list->len > 0) {
        new_node->next = NULL;
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    } else {
        list->head = list->tail = new_node;
        new_node->next = new_node->prev = NULL;
    }
    list->len++;
}

Node *list_lpop(List *list) {
    if (list->len <= 0) return NULL;
    Node *node = list->head;
    if (list->len - 1 > 0) {
        list->head = node->next;
        list->head->prev = NULL;
    } else {
        list->head = list->tail = NULL;
    }
    list->len--;
    node->next = node->prev = NULL;
    return node;
}

Node *list_rpop(List *list) {
    if (list->len <= 0) return NULL;
    Node *node = list->tail;
    if (list->len - 1 > 0) {
        list->tail = node->prev;
        list->tail->next = NULL;
    } else {
        list->head = list->tail = NULL;
    }
    list->len--;
    node->next = node->prev = NULL;
    return node;
}

void list_push(List *list, char *value, int direction) {
    direction == LEFT ? list_lpush(list, value) : list_rpush(list, value);
}

Node *list_pop(List *list, int direction) {
    return direction == LEFT ? list_lpop(list) : list_rpop(list);
}

Node *list_index(List *list, int index) {
    int id = 0;
    Node *node = list->head;
    while (id++ < index && node != NULL) node = node->next;
    return node;
}

char **list_range(List *list, int start, int end) {
    Node *node = list->head;
    char **res = malloc((end - start + 2) * sizeof(char *));
    int id = 0;
    while (id < start) {
        id++;
        node = node->next;
    }
    char **p = res;
    while (id <= end) {
        *p++ = node->value;
        id++;
        node = node->next;
    }
    *p = NULL;
    return res;
}


#ifndef HTABLE_H
#define HTABLE_H

#include <stdbool.h>

#define HT_BASE_SIZE 2

typedef struct HashTableItem {
    enum {STR_T, HASH_T, LIST_T, SET_T} type;
    char *key;
    void *value;
} HashTableItem;

typedef struct HashTable {
    int size;
    int used;
    HashTableItem **items;
} HashTable;

typedef struct ListNode {
    char *value;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

typedef struct List {
    int len;
    ListNode *head;
    ListNode *tail;
} List;

enum ListDirection {LEFT, RIGHT};

typedef struct Set {
    int size;
    int used;
    char **members;
} Set;

// common.c
int next_prime(int n);
int hash_func(char *key, int size, int i);

// htable.c
HashTable *htable_init(int size);
void htable_free(HashTable *ht);
void htable_set(HashTable *ht, char *key, char *value);
void htable_hset(HashTable *ht, char *key, char *field, char *value);
void htable_push(HashTable *ht, char *key, char *value, int dir);
void htable_sadd(HashTable *ht, char *key, char *value);

// list.c
List *list_init();
void list_free(List *ls);
void list_lpush(List *ls, char *value);
void list_rpush(List *ls, char *value);

// set.c
Set *set_init(int size);
void set_free(Set *set);
bool set_add(Set *set, char *value);


#endif

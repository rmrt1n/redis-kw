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
bool htable_del(HashTable *ht, char *key);
bool htable_exists(HashTable *ht, char *key);
void htable_set(HashTable *ht, char *key, char *value);
void htable_hset(HashTable *ht, char *key, char *field, char *value);
void htable_push(HashTable *ht, char *key, char *value, int dir);
bool htable_sadd(HashTable *ht, char *key, char *value);
char *htable_get(HashTable *ht, char *key);
char *htable_hget(HashTable *ht, char *key, char *field);
char *htable_pop(HashTable *ht, char *key, int dir);
bool htable_sismember(HashTable *ht, char *key, char *value);
int htable_hlen(HashTable *ht, char *key);
int htable_llen(HashTable *ht, char *key);
char *htable_lindex(HashTable *ht, char *key, int id);
bool htable_lset(HashTable *ht, char*key, int id, char *value);
bool htable_hdel(HashTable *ht, char *key, char *field);
int htable_lrem(HashTable *ht, char *key, int count, char *value);
bool htable_srem(HashTable *ht, char *key, char *value);
int htable_lpos(HashTable *ht, char *key, char *value);
char **htable_hgetall(HashTable *ht, char *key);
char **htable_lrange(HashTable *ht, char *key, int begin, int end);
char **htable_smembers(HashTable *ht, char *key);
char **set_members(Set *set);

// list.c
List *list_init(void);
void list_free(List *ls);
void list_lpush(List *ls, char *value);
void list_rpush(List *ls, char *value);
ListNode *list_lpop(List *ls);
ListNode *list_rpop(List *ls);
ListNode *list_index(List *ls, int id);
bool list_set(List *ls, int id, char *value);
int list_pos(List *ls, char *value);
int list_rem(List *ls, int count, char *value);
char **list_range(List *ls, int begin, int end);

// set.c
Set *set_init(int size);
void set_free(Set *set);
bool set_add(Set *set, char *value);
bool set_rem(Set *set, char *key);
bool set_ismember(Set *set, char *key);


#endif

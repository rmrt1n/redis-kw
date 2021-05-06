#ifndef HTABLE_H
#define HTABLE_H

#include <stdbool.h>

#define LOCALHOST "127.0.0.1"
#define PORT_NUM 6379
#define SA struct sockaddr
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

typedef struct Parser {
    char *string;
    int pos;
    char current_char;
} Parser;

typedef struct Command {
    enum {
        DEL, EXISTS, TYPE,
        SET, GET, MSET, MGET, INCR, DECR, INCRBY, DECRBY, STRLEN,
        HSET, HGET, HDEL, HGETALL, HEXISTS, HKEYS, HVALS, HMGET, HLEN,
        LPUSH, LPOP, RPUSH, RPOP, LLEN, LINDEX, LRANGE, LSET,
        SADD, SREM, SISMEMBER, SMEMBERS, SMISMEMBER,
        QUIT, UNKNOWN, NOOP
    } type;
    int argc;
    char **argv;
} Command;

// helper.c
int next_prime(int n);
int hash_func(char *key, int size, int i);
int ndigits(int x);
bool is_number(char *str);
int strtoi(char *str);

// htable.c
HashTable *htable_init(int size);
void htable_free(HashTable *ht);
bool htable_del(HashTable *ht, char *key);
bool htable_exists(HashTable *ht, char *key);
char *htable_type(HashTable *ht, char *key);
bool htable_set(HashTable *ht, char *key, char *value);
bool htable_hset(HashTable *ht, char *key, char *field, char *value);
int htable_push(HashTable *ht, char *key, char *value, int dir);
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

// parser.c
Parser *parser_init(char *msg);
void parser_free(Parser *parser);
Command *parse(char *msg);
void command_free(Command *cmd);

// interpreter.c
char *interpret(Command *cmd, HashTable *ht);

// server.c
int init_server(void);
int accept_connection(int sfd);
void close_socket(int sockfd);
void close_client(int cfd);
int rediskw(int cfd, HashTable *ht);
char *readline(int cfd);
void writeline(int cfd, char *msg);

// client.c
int connect_server(char *addr, int port);
void repl(int sfd);

#endif

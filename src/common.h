#ifndef COMMON_H
#define COMMON_H

#define PORT_NUM 6379
#define SA struct sockaddr
#define HTABLE_BASE_SIZE 50

typedef struct Item {
    enum {STR, HASH, LIST, TSET} type;
    char *key;
    void *value;
} Item;

typedef struct HashTable {
    int size;
    int used;
    int *keys;
    Item **items;
} HashTable;

typedef struct HtableAction{
    enum {OK, NIL, ERR} status;
    void *value;
} HtableAction;

typedef struct Node {
    char *value;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct List {
    int len;
    Node *head;
    Node *tail;
} List;

enum list_direction {LEFT, RIGHT};

typedef struct Set {
    int size;
    int used;
    int *keys;
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
        SET, GET, MSET, MGET, INCR, DECR, INCRBY, DECRBY,
        HSET, HGET, HDEL, HGETALL,
        LPUSH, LPOP, RPUSH, RPOP, LLEN,
        SADD, SREM, SISMEMBER, SMEMBERS,
        QUIT, UNKNOWN, NOOP
    } type;
    int argc;
    char **argv;
} Command;

// common.c helper functions
void free_all(int n, ...);
int ndigits(int x);
int isnumber(char *str);
int strtoi(char *str);
char *intostr(int n);
int isprime(int n);
int next_prime(int n);

// server.c
int init_server(void);
int accept_connection(int sockfd);
void close_connection(int sockfd);
char *readline(int client_sock);
void writeline(int client_sock, char *msg);
void repl(int client_sock, HashTable *htable);

// parser.c
Parser *parser_init(char *msg);
Command *parse(char *msg);

// interpreter.c
void execute(int client_sock, HashTable *htable, char *msg);

// htable.c
int hash_func(char *key, int size, int i);
HashTable *htable_init(int size);
void htable_free(HashTable *htable);
void htable_resize_up(HashTable *htable);
void htable_resize_down(HashTable *htable);
HtableAction htable_set(HashTable *htable, char *key, char *value);
HtableAction htable_get(HashTable *htable, char *key);
HtableAction htable_del(HashTable *htable, char *key);
HtableAction htable_exists(HashTable *htable, char *key);
HtableAction htable_type(HashTable *htable, char *key);
HtableAction htable_hset(HashTable *htable, char *key, char *field, char *value);
HtableAction htable_hget(HashTable *htable, char *key, char *field);
HtableAction htable_hgetall(HashTable *htable, char *key);
HtableAction htable_hdel(HashTable *htable, char *key, char *field);
HtableAction htable_push(HashTable *htable, char *key, char *value, int dir);
HtableAction htable_pop(HashTable *htable, char *key, int dir);
HtableAction htable_llen(HashTable *htable, char *key);
HtableAction htable_sadd(HashTable *htable, char *key, char *value);
HtableAction htable_srem(HashTable *htable, char *key, char *value);
HtableAction htable_sismember(HashTable *htable, char *key, char *value);
HtableAction htable_smembers(HashTable *htable, char *key);

// list.c
Node *list_node_init(char *value);
List *list_init(void);
void list_node_free(Node *node);
void list_free(List *list);
void list_push(List *list, char *value, int direction);
Node *list_pop(List *list, int direction);

// set.c
Set *set_init(int size);
void set_free(Set *set);
HtableAction set_add(Set *set, char *value);
HtableAction set_get(Set *set, char *value);
HtableAction set_rem(Set *set, char *value);

#endif

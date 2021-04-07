#ifndef COMMON_H
#define COMMON_H

typedef struct Entry {
    enum {STR, HASH} type;
    char *key;
    void *value;
    struct Entry *next;
} Entry;

typedef struct HashTable {
    int size;
    Entry **entries;
} HashTable;

typedef struct HtableAction {
    enum {SUCCESS, NIL, ERR} status;
    char *value;
} HtableAction;

typedef struct Parser {
    char *string;
    int pos;
    char current_char;
} Parser;

typedef struct Command {
    enum {
        DEL, PING, EXISTS,
        SET, GET, GETRANGE, GETSET, MGET, STRLEN, INCR, DECR,
        HSET, HGET,
        QUIT, UNKNOWN, NOOP
    } type;
    int argc;
    char **args;
} Command;

// io funcs
char *readline(int client_sock);
void writeline(int client_sock, char *msg);

#endif

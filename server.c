#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUMBER 6379 // default port of redis
#define SA struct sockaddr
#define ulong unsigned long int

typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

typedef struct HashTable {
    int size;
    Entry **entries;
} HashTable;

typedef struct HtableAction {
    enum {SUCCESS, NIL} status;
    char *value;
} HtableAction;

typedef struct Command {
    enum {
        DEL, PING, EXISTS,
        SET, GET, GETRANGE, GETSET, MGET, STRLEN, INCR, DECR,
        QUIT, UNKNOWN, NOOP
    } type;
    int argc;
    char **args;
} Command;

typedef struct Lexer {
    char *string;
    int pos;
    char current_char;
} Lexer;

// hash table functions
ulong hash_func(char *str, int size);
HashTable *htable_init(int size);
HtableAction htable_set(HashTable *htable, char *key, char *value);
HtableAction htable_get(HashTable *htable, char *key);
HtableAction htable_del(HashTable *htable, char *key);
HtableAction htable_exists(HashTable *htable, char *key);

// server functions
int init_server(struct sockaddr_in *serv_addr);
int accept_connection(int sockfd, struct sockaddr_in *client);

// parser functions
Lexer *lexer_init(char *msg);
Command *command_init(int type, int argc, char **args);
void lexer_advance(Lexer *lexer);
char *get_next_token(Lexer *lexer);
int get_argc(Lexer *lexer);
Command *parse(char *msg);

// command handler functions
void exec_set(int client_sock, HashTable *htable, Command *cmd);
void exec_get(int client_sock, HashTable *htable, Command *cmd);
void exec_getrange(int client_sock, HashTable *htable, Command *cmd);
void exec_getset(int client_sock, HashTable *htable, Command *cmd);
void exec_mget(int client_sock, HashTable *htable, Command *cmd);
void exec_strlen(int client_sock, HashTable *htable, Command *cmd);
void exec_incr(int client_sock, HashTable *htable, Command *cmd);
void exec_decr(int client_sock, HashTable *htable, Command *cmd);
void exec_del(int client_sock, HashTable *htable, Command *cmd);
void exec_exists(int client_sock, HashTable *htable, Command *cmd);
void exec_ping(int client_sock, Command *cmd);
void exec_unknown(int client_sock);

// interpreter functions
void print_status(int client_sock, HtableAction action);
char *readline(int client_sock);
void writeline(int client_sock, char *msg);
void execute(int client_sock, HashTable *htable, char *msg);
void repl(int client_sock, HashTable *htable);

// helper functions
int ndigits(int x);
int isnumber(char *str);
int strtoi(char *str);
char *substr(char *str, int start, int end);

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, client;
    HashTable *htable = htable_init(4096);

    // start server
    sockfd = init_server(&serv_addr);
    // accept connection
    int client_sock = accept_connection(sockfd, &client);

    repl(client_sock, htable);

    puts("server closed");
    close(sockfd);
    
    return 0;
}

int init_server(struct sockaddr_in *serv_addr) {
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fputs("failed to create socket", stderr);
        exit(1);
    }

    // assign ip & port
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr->sin_port = htons(PORT_NUMBER);

    // bind socket to ip
    if (bind(sockfd, (SA *)serv_addr, sizeof(*serv_addr)) != 0) {
        fputs("failed to bind socket", stderr);
        exit(1);
    }

    // server listen
    if (listen(sockfd, 5) != 0) {
        fputs("listen failed", stderr);
        exit(1);
    }

    puts("server started");
    return sockfd;
} 

int accept_connection(int sockfd, struct sockaddr_in *client) {
    int len = sizeof(struct sockaddr_in);
    int client_sock = accept(sockfd, (SA *)client, &len);
    if (client_sock < 0) {
        fputs("failed to accept connection", stderr);
        exit(1);
    }

    writeline(client_sock, "connected");
    return client_sock;
} 

ulong hash_func(char *str, int arr_size) {
    unsigned long hash = 5381;
    int c;
    // hash = 5381 * 33 + c
    while (c = *str++) hash = ((hash << 5) + hash) + c; 
    return hash % arr_size;
}

HashTable *htable_init(int size) {
    HashTable *htable = malloc(sizeof(HashTable));
    
    if (htable == NULL) {
        fputs("unable to create hash table", stderr);
        exit(1);
    }

    htable->size = size;
    htable->entries = malloc(size * sizeof(Entry *));

    return htable;
}

HtableAction htable_set(HashTable *htable, char *key, char *value) {
    Entry *keyval = malloc(sizeof(Entry));

    keyval->key = strdup(key);
    keyval->value = strdup(value);

    ulong hash = hash_func(key, htable->size);
    if (htable->entries[hash] != NULL) {
        Entry *tmp = htable->entries[hash];
        while (tmp != NULL) {
            if (strcmp(tmp->key, keyval->key) == 0) break;
            tmp = tmp->next;
        }

        if (tmp != NULL) {
            free(tmp->value);
            tmp->value = strdup(keyval->value);
            free(keyval->key);
            free(keyval->value);
            free(keyval);
        } else {
            keyval->next = NULL;
            (htable->entries[hash])->next = keyval;
        }
    } else {
        keyval->next = NULL;
        htable->entries[hash] = keyval;
    }
    
    HtableAction result = {.status = SUCCESS, .value = NULL};
    return result;
}

HtableAction htable_get(HashTable *htable, char *key) {
    HtableAction result;
    ulong hash = hash_func(key, htable->size);

    Entry *tmp = htable->entries[hash];
    while (tmp != NULL && strcmp(tmp->key, key) != 0) tmp = tmp->next;

    if (tmp == NULL) {
        result = (HtableAction){.status = NIL, .value = NULL};
    } else {
        result.status = SUCCESS;
        result.value = malloc(strlen(tmp->value) + 1);
        strcpy(result.value, tmp->value);
    }

    return result;
}

HtableAction htable_del(HashTable *htable, char *key) {
    HtableAction result;
    ulong hash = hash_func(key, htable->size);

    if (htable->entries[hash] != NULL) {
        Entry *tmp = htable->entries[hash], *tmp_prev = NULL;

        if (tmp->next == NULL) {
            free(tmp->key);
            free(tmp->value);
            free(tmp);
            htable->entries[hash] = NULL;
            result = (HtableAction){.status = SUCCESS, .value = NULL};
        } else {
            while (tmp != NULL && strcmp(tmp->key, key) != 0) {
                tmp_prev = tmp;
                tmp = tmp->next; 
            }
            if (tmp != NULL) {
                tmp_prev->next = tmp->next;
                free(tmp->key); 
                free(tmp->value); 
                free(tmp);
                result = (HtableAction){.status = SUCCESS, .value = NULL};
            } else {
                result = (HtableAction){.status = NIL, .value = NULL};
            }
        }
    } else {
        result = (HtableAction){.status = NIL, .value = NULL};
    }

    return result;
}

HtableAction htable_exists(HashTable *htable, char *key) {
    HtableAction result; 
    ulong hash = hash_func(key, htable->size);

    Entry *tmp = htable->entries[hash];
    while (tmp != NULL && strcmp(tmp->key, key) != 0) tmp = tmp->next;

    if (tmp != NULL) {
        result = (HtableAction){.status = SUCCESS, .value = NULL};
    } else {
        result = (HtableAction){.status = NIL, .value = NULL};
    }

    return result;
}

void print_status(int client_sock, HtableAction action) {
    if (action.status == SUCCESS) {
        writeline(client_sock, "OK");
    } else {
        writeline(client_sock, "(nil)");
    }
}

char *readline(int client_sock) {
    char *msg = malloc(1024);
    read(client_sock, msg, 1024);
    msg[strlen(msg) - 1] = '\0';
    return msg;
}

int isnumber(char *str) {
    int i = *str == '-' ? 1 : 0;
    for (i; i < strlen(str); i++) {
        if (isdigit(str[i]) == 0) return 0;
    }
    return 1;
}

int strtoi(char *str) {
    int res = 0, i = *str == '-' ? 1 : 0;
    int neg = i;
    for (i; i < strlen(str); i++) {
        res = res * 10 + (str[i] - '0');
    }
    if (neg) res *= -1;
    return res;
}

char *substr(char *str, int start, int end) {
    int n = strlen(str);
    if (start > end) {
        return "\"\"";
    } else {
        char *res = malloc(end - start + 1);
        snprintf(res, end - start + 2, "%s", str+start);
        return res;
    }
}

void writeline(int client_sock, char *msg) {
    char *tmp = malloc(strlen(msg) + 2);
    sprintf(tmp, "%s\n", msg);
    write(client_sock, tmp, strlen(tmp) + 1);
    free(tmp);
}

char *quote_encase(char *str) {
    char *tmp = malloc(strlen(str) + 3);
    sprintf(tmp, "\"%s\"", str);
    return tmp;
}

void print_wrong_argc(int client_sock, int given, int expected) {
    char *tmp = malloc(50 + ndigits(given) + ndigits(expected));
    sprintf(tmp, "ERR wrong number of arguments (given %d, expected %d)",
            given, expected);
    writeline(client_sock, tmp);
    free(tmp);
}

int ndigits(int x) {
    int n = x < 0 ? x * -1 : x;
    int res = 0;
    while (n > 0) {
        n /= 10;
        res++;
    }
    return res;
}

Command *command_init(int type, int argc, char **args) {
    Command *cmd = malloc(sizeof(Command));
    cmd->type = type;
    cmd->argc = argc;
    cmd->args = args;
    return cmd;
}

Lexer *lexer_init(char *msg) {
    Lexer *lexer = malloc(sizeof(lexer));
    lexer->string = msg;
    lexer->pos = 0;
    lexer->current_char = msg[0];
    return lexer;
}

void lexer_advance(Lexer *lexer) {
    if (lexer->pos < strlen(lexer->string)) {
        lexer->pos++;
        lexer->current_char = lexer->string[lexer->pos];
    }
}

char *get_next_token(Lexer *lexer) {
    char *token = calloc(1, 1);
    if (lexer->pos < strlen(lexer->string)) {
        if (isspace(lexer->current_char)) {
            while (isspace(lexer->current_char)) lexer_advance(lexer);
        }

        // handle if whitespace skips until terminating null byte
        if (lexer->current_char == '\0') return NULL;

        if (lexer->current_char == '"') {
            lexer_advance(lexer);
            while (lexer->current_char != '"' && lexer->pos < strlen(lexer->string)) {
                    char *temp = calloc(2, 1);
                    temp[0] = lexer->current_char;

                    token = realloc(token, strlen(token) + 2);
                    strcat(token, temp);

                    free(temp);
                    lexer_advance(lexer);
            }
            lexer_advance(lexer);
            return token;
        }

        if (isspace(lexer->current_char) == 0) {
            while (isspace(lexer->current_char) == 0 && lexer->pos < strlen(lexer->string)) {
                char *temp = calloc(2, 1);
                temp[0] = lexer->current_char;

                token = realloc(token, strlen(token) + 2);
                strcat(token, temp);

                free(temp);
                lexer_advance(lexer);
            }
            return token;
        }
    } 

    return NULL;
}

int get_argc(Lexer *lexer) {
    Lexer *dup = lexer_init(lexer->string); 
    int argc = 0;
    while (get_next_token(dup) != NULL) argc++;
    free(dup);
    return argc - 1; // -1 for command
}

Command *parse(char *msg) {
    Lexer *lexer = lexer_init(msg);
    char *token = get_next_token(lexer);
    int argc = get_argc(lexer);
    Command *cmd;
    if (argc >= 0) {
        int type;
        if (strcmp(token, "set") == 0) {
            type = SET; 
        } else if (strcmp(token, "get") == 0) {
            type = GET; 
        } else if (strcmp(token, "getrange") == 0) {
            type = GETRANGE; 
        } else if (strcmp(token, "getset") == 0) {
            type = GETSET; 
        } else if (strcmp(token, "mget") == 0) {
            type = MGET; 
        } else if (strcmp(token, "strlen") == 0) {
            type = STRLEN; 
        } else if (strcmp(token, "incr") == 0) {
            type = INCR; 
        } else if (strcmp(token, "decr") == 0) {
            type = DECR; 
        } else if (strcmp(token, "del") == 0) {
            type = DEL; 
        } else if (strcmp(token, "ping") == 0) {
            type = PING;
        } else if (strcmp(token, "exists") == 0) {
            type = EXISTS;
        } else if (strcmp(token, "quit") == 0) {
            type = QUIT;
        } else {
            type = UNKNOWN; 
        }

        // parse arguments
        char **args = malloc(argc * sizeof(char *));
        for (int i = 0; i < argc; i++) {
            token = get_next_token(lexer);
            args[i] = malloc(strlen(token) + 1);
            strcpy(args[i], token);
        }

        cmd = command_init(type, argc, args);
    } else {
        cmd = command_init(NOOP, 0, NULL);
    }

    free(lexer);
    free(token);
    return cmd;
}

void exec_set(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "OK");
    } else if (cmd->argc == 1) {
        HtableAction instr = htable_set(htable, cmd->args[0], ""); 
        print_status(client_sock, instr);
    } else if (cmd->argc == 2) {
        HtableAction instr = htable_set(htable, cmd->args[0], cmd->args[1]); 
        print_status(client_sock, instr);
    } else {
        writeline(client_sock, "ERR syntax error");
    }
}

void exec_get(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]); 
        if (instr.status == SUCCESS) {
            char *tmp = malloc(strlen(instr.value) + 2);
            sprintf(tmp, "\"%s\"", instr.value);
            writeline(client_sock, tmp);
            free(tmp);
        } else {
            print_status(client_sock, instr);
        }
    } else {
        char tmp[55];
        sprintf(tmp, "ERR wrong number of arguments (given %d, expected 1)", cmd->argc);
        writeline(client_sock, tmp);
    }
}

void exec_getset(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 2) {
        HtableAction get_instr = htable_get(htable, cmd->args[0]);
        if (get_instr.status == SUCCESS) {
            char *tmp = malloc(strlen(get_instr.value) + 2);
            sprintf(tmp, "\"%s\"", get_instr.value);
            writeline(client_sock, tmp);
            free(tmp);
        }
        HtableAction set_instr = htable_set(htable, cmd->args[0], cmd->args[1]);
    } else {
        char tmp[55];
        sprintf(tmp, "ERR wrong number of arguments (given %d, expected 2)", cmd->argc);
        writeline(client_sock, tmp);
    }
}

void exec_mget(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "ERR wrong number of arguments for 'mget'");
    } else {
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction instr = htable_get(htable, cmd->args[i]);
            if (instr.status == SUCCESS) {
                // + 5 = '\0' + "" + ) + ' '
                char *tmp = malloc(strlen(instr.value) + ndigits(i + 1) + 5);
                char *quoted_val = quote_encase(instr.value);
                sprintf(tmp, "%d) %s", i + 1, quoted_val);
                writeline(client_sock, tmp);
                free(tmp);
                free(quoted_val);
            } else {
                // + 8 = '\0' + ") (nil)"
                char *tmp = malloc(ndigits(i + 1) + 8);
                sprintf(tmp, "%d) (nil)", i + 1);
                writeline(client_sock, tmp);
                free(tmp);
            }
        }
    }
}

void exec_strlen(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            int n = strlen(instr.value);
            char *tmp = malloc(n + 1);
            sprintf(tmp, "%d", n);
            writeline(client_sock, tmp);
            free(tmp);
        } else {
            writeline(client_sock, "0");
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 1);
    }
}

void exec_incr(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            if (isnumber(instr.value)) {
                int n = strtoi(instr.value) + 1;
                char *val = malloc(ndigits(n) + 2);
                sprintf(val, "%d", n);
                HtableAction set_instr = htable_set(htable, cmd->args[0], val);
                print_status(client_sock, set_instr);
            } else {
                writeline(client_sock, "ERR value is not an integer or out of range");
            }
        } else {
            HtableAction instr = htable_set(htable, cmd->args[0], "1");
            print_status(client_sock, instr);
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 1);
    }
}

void exec_decr(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            if (isnumber(instr.value)) {
                int n = strtoi(instr.value) - 1;
                char *val = malloc(ndigits(n) + 2);
                sprintf(val, "%d", n);
                HtableAction set_instr = htable_set(htable, cmd->args[0], val);
                print_status(client_sock, set_instr);
            } else {
                writeline(client_sock, "ERR value is not an integer or out of range");
            }
        } else {
            HtableAction instr = htable_set(htable, cmd->args[0], "-1");
            print_status(client_sock, instr);
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 1);
    }
}

void exec_getrange(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 3) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            if (isnumber(cmd->args[1]) && isnumber(cmd->args[2])) {
                int n = strlen(instr.value);
                int start = strtoi(cmd->args[1]), end = strtoi(cmd->args[2]);
                start = start < 0 ? n + start : start;
                end = end < 0 ? n + end : end;
                if ((start >= 0 && start < n) && (end >= 0 && end < n)) {
                    writeline(client_sock, substr(instr.value, start, end));
                } else {
                    writeline(client_sock, "ERR value is not an integer or out of range");
                }
            } else {
                writeline(client_sock, "ERR value is not an integer or out of range");
            }
        } else {
            print_status(client_sock, instr);
        }
    } else {
        char tmp[55];
        sprintf(tmp, "ERR wrong number of arguments (given %d, expected 3)\n", cmd->argc);
        writeline(client_sock, tmp);
    }
}

void exec_del(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "ERR wrong number of arguments for 'del'");
    } else {
        int success = 0;
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction instr = htable_del(htable, cmd->args[i]); 
            if (instr.status == SUCCESS) success++;
        }
        char *tmp = malloc(ndigits(success) + 1);
        sprintf(tmp, "%d", success);
        writeline(client_sock, tmp);
        free(tmp);
    }
}

void exec_ping(int client_sock, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "PONG");
    } else if (cmd->argc == 1) {
        char *tmp = malloc(strlen(cmd->args[0]) + 1);
        sprintf(tmp, "\"%s\"", cmd->args[0]);
        writeline(client_sock, tmp);
        free(tmp);
    } else {
        char tmp[58];
        sprintf(tmp, "ERR wrong number of arguments (given %d, expected 0..1)", cmd->argc);
        writeline(client_sock, tmp);
    }
}

void exec_exists(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "ERR wrong number of arguments for 'exists'");
    } else {
        int success = 0;
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction instr = htable_exists(htable, cmd->args[i]);
            if (instr.status == SUCCESS) success++;
        }
        char *tmp = malloc(ndigits(success) + 1);
        sprintf(tmp, "%d", success);
        writeline(client_sock, tmp);
        free(tmp);
    }
}

void exec_unknown(int client_sock) {
    writeline(client_sock, "ERR unrecognized command");
}

void execute(int client_sock, HashTable *htable, char *msg) {
    Command *cmd = parse(msg);
    switch (cmd->type) {
        case SET: exec_set(client_sock, htable, cmd); break;
        case GET: exec_get(client_sock, htable, cmd); break;
        case GETRANGE: exec_getrange(client_sock, htable, cmd); break;
        case GETSET: exec_getset(client_sock, htable, cmd); break;
        case MGET: exec_mget(client_sock, htable, cmd); break;
        case STRLEN: exec_strlen(client_sock, htable, cmd); break;
        case INCR: exec_incr(client_sock, htable, cmd); break;
        case DECR: exec_decr(client_sock, htable, cmd); break;
        case DEL: exec_del(client_sock, htable, cmd); break;
        case EXISTS: exec_exists(client_sock, htable, cmd); break;
        case PING: exec_ping(client_sock, cmd); break;
        case UNKNOWN: exec_unknown(client_sock); break;
        case QUIT: exit(0); break;
    }
    free(cmd->args);
    free(cmd);
}

void repl(int client_sock, HashTable *htable) {
    while (1) {
        char *msg;

        write(client_sock, "> ", 3);
        msg = readline(client_sock);
        execute(client_sock, htable, msg);

        free(msg);
    }
}


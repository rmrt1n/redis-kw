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
    enum {SUCCESS, FAIL} status;
    char *value;
} HtableAction;

ulong hash_func(char *str, int size);
HashTable *htable_init(int size);
HtableAction htable_set(HashTable *htable, char *key, char *value);
HtableAction htable_get(HashTable *htable, char *key);
HtableAction htable_del(HashTable *htable, char *key);

int init_server(struct sockaddr_in *serv_addr);
int accept_connection(int sockfd, struct sockaddr_in *client);

void print_status(int client_sock, HtableAction action);
char *readline(int client_sock);
void execute(int client_sock, HashTable *htable, char *msg);
void repl(int client_sock, HashTable *htable);

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

    write(client_sock, "connected\n", 11);
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
        result = (HtableAction){.status = FAIL, .value = NULL};
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

    Entry *tmp = htable->entries[hash], *tmp_prev;
    while (tmp != NULL && strcmp(tmp->key, key) != 0) {
        tmp_prev = tmp;
        tmp = tmp->next;
    }
    
    if (tmp == NULL) {
        result = (HtableAction){.status = FAIL, .value = NULL};
    } else {
        tmp_prev->next = tmp->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
        result = (HtableAction){.status = SUCCESS, .value = NULL};
    }
    
    return result;
}

void print_status(int client_sock, HtableAction action) {
    if (action.status == SUCCESS) {
        write(client_sock, "OK\n", 4);
    } else {
        write(client_sock, "ERR\n", 5);
    }
}

char *readline(int client_sock) {
    char *msg = malloc(1024);
    read(client_sock, msg, 1024);
    msg[strlen(msg) - 1] = '\0';
    return msg;
}

void execute(int client_sock, HashTable *htable, char *msg) {
    char *token = strdup(msg);
    while (token = strtok_r(msg, " \n\t", &msg)) {
        if (strcmp(token, "set") == 0) {
            char *key = strtok_r(msg, " \n\t", &msg);
            char *value = strtok_r(msg, " \n\t", &msg);
            HtableAction instr = htable_set(htable, key, value); 
            print_status(client_sock, instr);
        } else if (strcmp(token, "get") == 0) {
            char *key = strtok_r(msg, " \n\t", &msg);
            HtableAction instr = htable_get(htable, key); 
            if (instr.status == SUCCESS) {
                char *temp = malloc(strlen(instr.value) + 1);
                sprintf(temp, "\"%s\"\n", instr.value);
                write(client_sock, temp, strlen(temp) + 1);
                free(temp);
            } else {
                print_status(client_sock, instr);
            }
        } else if (strcmp(token, "del") == 0) {
            char *key = strtok_r(msg, " \n\t", &msg);
            HtableAction instr = htable_get(htable, key); 
            print_status(client_sock, instr);
        } else if (strcmp(token, "quit") == 0) {
            exit(0);
        } else {
            char *invalid_msg = malloc(24);
            sprintf(invalid_msg, "unknown command %s\n", token);
            write(client_sock, invalid_msg, strlen(invalid_msg) + 1);
            break;
        }
    }
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

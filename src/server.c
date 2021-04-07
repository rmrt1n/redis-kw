#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "htable.h"
#include "interpreter.h"

#define PORT_NUMBER 6379 // default port of redis
#define SA struct sockaddr
#define ulong unsigned long int

// server functions
int init_server(struct sockaddr_in *serv_addr);
int accept_connection(int sockfd, struct sockaddr_in *client);
void repl(int client_sock, HashTable *htable);

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, client;
    HashTable *htable = htable_init(4096);

    // start server
    sockfd = init_server(&serv_addr);
    // accept connection
    int client_sock = accept_connection(sockfd, &client);
    // server function
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
    unsigned int len = sizeof(struct sockaddr_in);
    int client_sock = accept(sockfd, (SA *)client, &len);
    if (client_sock < 0) {
        fputs("failed to accept connection", stderr);
        exit(1);
    }

    writeline(client_sock, "connected");
    return client_sock;
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


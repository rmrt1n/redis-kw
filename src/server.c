#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"

int init_server() {
    struct sockaddr_in serv_addr;
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fputs("failed to create socket", stderr);
        exit(1);
    }

    // assign ip & port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT_NUM);

    // bind socket to ip
    if (bind(sockfd, (SA *)&serv_addr, sizeof(serv_addr)) != 0) {
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

int accept_connection(int sockfd) {
    struct sockaddr_in client_addr;
    unsigned int len = sizeof(struct sockaddr_in);
    int client = accept(sockfd, (SA *)&client_addr, &len);
    if (client < 0) {
        fputs("failed to accept connection", stderr);
        exit(1);
    }

    writeline(client, "connected");
    return client;
} 

void close_connection(int sockfd) {
    puts("closing server");
    close(sockfd);
}

char *readline(int client) {
    char *msg = malloc(1024);
    read(client, msg, 1024);
    msg[strlen(msg) - 1] = '\0';
    return msg;
}

void writeline(int client, char *msg) {
    char *tmp = malloc(strlen(msg) + 2);
    sprintf(tmp, "%s\n", msg);
    write(client, tmp, strlen(tmp) + 1);
    free(tmp);
}

void repl(int client, HashTable *htable) {
    while (1) {
        char *msg;

        write(client, "> ", 3);
        msg = readline(client);
        execute(client, htable, msg);

        free(msg);
    }
}


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
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        fputs("failed to create socket", stderr);
        exit(1);
    }

    // assign ip & port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT_NUM);

    // bind socket to ip
    if (bind(sfd, (SA *)&serv_addr, sizeof(serv_addr)) != 0) {
        fputs("failed to bind socket", stderr);
        exit(1);
    }

    // server listen
    if (listen(sfd, 5) != 0) {
        fputs("listen failed", stderr);
        exit(1);
    }

    puts("server started");
    return sfd;
} 

int accept_connection(int sfd) {
    struct sockaddr_in client_addr;
    unsigned int len = sizeof(struct sockaddr_in);
    int cfd = accept(sfd, (SA *)&client_addr, &len);
    if (cfd < 0) {
        fputs("failed to accept connection", stderr);
        exit(1);
    }
    return cfd;
} 

// void close_connection(int sockfd) {
    // puts("closing server");
    // close(sockfd);
// }

char *readline(int cfd) {
    char *msg = calloc(1024, 1);
    read(cfd, msg, 1024);
    msg[strlen(msg) - 1] = '\0';
    return msg;
}

void writeline(int cfd, char *msg) {
    char *tmp = malloc(strlen(msg) + 2);
    sprintf(tmp, "%s\n", msg);
    write(cfd, tmp, strlen(tmp) + 1);
    free_all(1, tmp);
}

void rediskw(int cfd, HashTable *ht) {
    while (1) {
        // write(client, "> ", 3);
        char *msg = readline(cfd);
        Command *cmd = parse(msg);
        char *resp = interpret(cmd, ht);
        write(cfd, resp, strlen(resp));
        free_all(1, msg);
    }
}


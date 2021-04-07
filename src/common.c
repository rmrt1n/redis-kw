#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

char *readline(int client_sock) {
    char *msg = malloc(1024);
    read(client_sock, msg, 1024);
    msg[strlen(msg) - 1] = '\0';
    return msg;
}

void writeline(int client_sock, char *msg) {
    char *tmp = malloc(strlen(msg) + 2);
    sprintf(tmp, "%s\n", msg);
    write(client_sock, tmp, strlen(tmp) + 1);
    free(tmp);
}


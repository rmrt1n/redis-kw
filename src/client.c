#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"

int connect_server(char *addr, int port) {
    struct sockaddr_in serv_addr;
    // create socket && verification
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        fputs("failed to create socket", stderr);
        exit(1);
    }

    // assign ip & port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr);
    serv_addr.sin_port = htons(port);

    // connect
    if (connect(sfd, (SA *)&serv_addr, sizeof(serv_addr)) != 0) {
        fputs("failed to connect to server", stderr);
        exit(1);
    }

    return sfd;
}

static void print_quote_encase(char *str) {
    char *tmp = malloc(strlen(str) + 3);
    sprintf(tmp, "\"%s\"", str);
    puts(tmp);
    free(tmp);
}

static int parse_int(char *resp) {
    resp++;
    int n = 0, min = 0;
    if (*resp == '-') resp++, min++;
    while (*resp != '\r') {
        n = n * 10 + (*resp - '0');
        resp++;
    }
    return min ? n * -1 : n;
}

static char *parse_str(char *resp) {
    int n = parse_int(resp);
    if (n < 0) return NULL;

    while (*resp != '\n') resp++;
    resp++;

    char *result = calloc(n + 1, sizeof(char));
    for (int i = 0; i < n; i++) result[i] = resp[i];
    result[n] = '\0';

    return result;
}

static char *parse_err(char *resp) {
    resp++;
    char *result = malloc(strlen(resp) - 1);
    for (int i = 0; i < strlen(resp) - 2; i++) result[i] = resp[i];
    // result[strlen(result)] = '\0';
    return result;
}

static void parse_resp(char *resp) {
    char *tmp;
    switch(*resp) {
        case '$':
            tmp = parse_str(resp); 
            tmp == NULL ? puts("(nil)") : print_quote_encase(tmp);
            free(tmp);
            break;
        case ':':
            printf("(integer) %d\n", parse_int(resp));
            break;
        case '*': break;
        case '-':
            puts(parse_err(resp));
        default: break;
    }
}

void repl(int sfd) {
    while (1) {
        char *inp = malloc(1024);
        printf("redis-kw> ");

        fgets(inp, 1024, stdin);
        writeline(sfd, inp);

        char *resp = readline(sfd);
        // printf("%d\n", resp == NULL);
        if (resp == NULL) break;
        parse_resp(resp);
        free(inp);
    }
}


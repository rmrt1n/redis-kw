#include <stdio.h>
#include <unistd.h>
#include "common.h"

int main() {
    HashTable *htable = htable_init(4096);

    // start server
    int sockfd = init_server();
    // accept connection
    int client_sock = accept_connection(sockfd);
    // server function
    repl(client_sock, htable);
    // close server
    close_connection(sockfd);
    
    return 0;
}

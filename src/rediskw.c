#include <stdio.h>
#include <stdlib.h>
#include "common.h"

static void close_server(int sfd, HashTable *ht) {
    close_socket(sfd);
    htable_free(ht);
    exit(0);
}

int main() {
    HashTable *ht = htable_init(HT_BASE_SIZE);
    int sfd = init_server();
    while (1) {
        int cfd = accept_connection(sfd);
        int code = rediskw(cfd, ht);
        code == 1 ? close_server(sfd, ht) : close_socket(cfd);
    }
    return 0;
}

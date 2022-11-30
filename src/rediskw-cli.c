#include "common.h"

int main() {
    int sfd = connect_server(LOCALHOST, PORT_NUM);
    repl(sfd);
    close_socket(sfd);
    return 0;
}

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "common.h"

void execute(int client_sock, HashTable *htable, char *msg);

#endif

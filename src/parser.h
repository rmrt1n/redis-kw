#ifndef PARSER_H
#define PARSER_H

#include "common.h"

Parser *parser_init(char *msg);
Command *parse(char *msg);

#endif

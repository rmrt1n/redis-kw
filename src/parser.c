#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

static Command *command_init(int type, int argc, char **args) {
    Command *cmd = malloc(sizeof(Command));
    cmd->type = type;
    cmd->argc = argc;
    cmd->args = args;
    return cmd;
}

static void parser_advance(Parser *parser) {
    if (parser->pos < strlen(parser->string)) {
        parser->pos++;
        parser->current_char = parser->string[parser->pos];
    }
}

static char *get_next_token(Parser *parser) {
    char *token = calloc(1, 1);
    if (parser->pos < strlen(parser->string)) {
        if (isspace(parser->current_char)) {
            while (isspace(parser->current_char)) parser_advance(parser);
        }

        // handle if whitespace skips until terminating null byte
        if (parser->current_char == '\0') return NULL;

        if (parser->current_char == '"') {
            parser_advance(parser);
            while (parser->current_char != '"' && parser->pos < strlen(parser->string)) {
                    char *temp = calloc(2, 1);
                    temp[0] = parser->current_char;

                    token = realloc(token, strlen(token) + 2);
                    strcat(token, temp);

                    free(temp);
                    parser_advance(parser);
            }
            parser_advance(parser);
            return token;
        }

        if (isspace(parser->current_char) == 0) {
            while (isspace(parser->current_char) == 0 && parser->pos < strlen(parser->string)) {
                char *temp = calloc(2, 1);
                temp[0] = parser->current_char;

                token = realloc(token, strlen(token) + 2);
                strcat(token, temp);

                free(temp);
                parser_advance(parser);
            }
            return token;
        }
    } 

    return NULL;
}

static int get_argc(Parser *parser) {
    Parser *dup = parser_init(parser->string); 
    int argc = 0;
    while (get_next_token(dup) != NULL) argc++;
    free(dup);
    return argc - 1; // -1 for command
}

Parser *parser_init(char *msg) {
    Parser *parser = malloc(sizeof(parser));
    parser->string = msg;
    parser->pos = 0;
    parser->current_char = msg[0];
    return parser;
}

Command *parse(char *msg) {
    Parser *parser = parser_init(msg);
    char *token = get_next_token(parser);
    int argc = get_argc(parser);
    Command *cmd;
    if (argc >= 0) {
        int type;
        if (strcmp(token, "del") == 0) {
            type = DEL; 
        } else if (strcmp(token, "exists") == 0) {
            type = EXISTS;
        } else if (strcmp(token, "set") == 0) {
            type = SET; 
        } else if (strcmp(token, "get") == 0) {
            type = GET; 
        } else if (strcmp(token, "getrange") == 0) {
            type = GETRANGE; 
        } else if (strcmp(token, "getset") == 0) {
            type = GETSET; 
        } else if (strcmp(token, "mget") == 0) {
            type = MGET; 
        } else if (strcmp(token, "strlen") == 0) {
            type = STRLEN; 
        } else if (strcmp(token, "incr") == 0) {
            type = INCR; 
        } else if (strcmp(token, "decr") == 0) {
            type = DECR; 
        } else if (strcmp(token, "hset") == 0) {
            type = HSET; 
        } else if (strcmp(token, "hget") == 0) {
            type = HGET; 
        } else if (strcmp(token, "ping") == 0) {
            type = PING;
        } else if (strcmp(token, "quit") == 0) {
            type = QUIT;
        } else {
            type = UNKNOWN; 
        }

        // parse arguments
        char **args = malloc(argc * sizeof(char *));
        for (int i = 0; i < argc; i++) {
            token = get_next_token(parser);
            args[i] = malloc(strlen(token) + 1);
            strcpy(args[i], token);
        }

        cmd = command_init(type, argc, args);
    } else {
        cmd = command_init(NOOP, 0, NULL);
    }

    free(parser);
    free(token);
    return cmd;
}
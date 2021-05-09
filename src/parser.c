#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

static Command *command_init(int type, int argc, char **argv) {
    Command *cmd = dmalloc(sizeof(Command));
    cmd->type = type;
    cmd->argc = argc;
    cmd->argv = argv;
    return cmd;
}

void command_free(Command *cmd) {
    for (int i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
    free(cmd->argv);
    free(cmd);
}

static void parser_advance(Parser *parser) {
    if (parser->pos < strlen(parser->string)) {
        parser->pos++;
        parser->current_char = parser->string[parser->pos];
    }
}

static char *parse_id(Parser *parser) {
    char *token = dmalloc(sizeof(char));
    *token = '\0';
    int n = 1;
    while (!isspace(parser->current_char) &&
            parser->pos < strlen(parser->string)) {
        token = drealloc(token, ++n);
        char *tmp = token;
        sprintf(token, "%s%c", tmp, parser->current_char);
        parser_advance(parser);
    }
    token = realloc(token, ++n);
    token[n-1] = '\0';
    return token;
}


static char *parse_string(Parser *parser) {
    // first quote
    char *token = dmalloc(sizeof(char));
    *token = '\0';
    int n = 1;
    parser_advance(parser);
    while ((parser->current_char != '"' && parser->current_char != '\'') &&
           parser->pos < strlen(parser->string)) {
        token = drealloc(token, ++n);
        char *tmp = token;
        sprintf(token, "%s%c", tmp, parser->current_char);
        parser_advance(parser);
    }
    // last quote
    parser_advance(parser);
    token = realloc(token, ++n);
    token[n-1] = '\0';
    return token;
}

static char *get_next_token(Parser *parser) {
    if (parser->pos < strlen(parser->string)) {
        // skip whitespace
        if (isspace(parser->current_char)) {
            while (isspace(parser->current_char)) parser_advance(parser);
        }

        // handle if whitespace skips until terminating null byte
        if (parser->current_char == '\0') return NULL;

        // parse string literals
        if (parser->current_char == '"' || parser->current_char == '\'') {
            return parse_string(parser);
        }

        // parse identifiers
        if (isspace(parser->current_char) == 0) return parse_id(parser);
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
    Parser *parser = dmalloc(sizeof(Parser));
    parser->string = strdup(msg);
    parser->pos = 0;
    parser->current_char = msg[0];
    return parser;
}

void parser_free(Parser *parser) {
    free(parser->string);
    free(parser);
}

Command *parse(char *msg) {
    Parser *parser = parser_init(msg);
    char *token = get_next_token(parser);
    int argc = get_argc(parser);
    Command *cmd;
    if (argc >= 0) {
        int type;
        if (strcmp(token, "del") == 0) type = DEL;
        else if (strcmp(token, "exists") == 0) type = EXISTS;
        else if (strcmp(token, "type") == 0) type = TYPE;
        else if (strcmp(token, "set") == 0) type = SET;
        else if (strcmp(token, "get") == 0) type = GET;
        else if (strcmp(token, "mset") == 0) type = MSET;
        else if (strcmp(token, "mget") == 0) type = MGET;
        else if (strcmp(token, "incr") == 0) type = INCR;
        else if (strcmp(token, "decr") == 0) type = DECR;
        else if (strcmp(token, "incrby") == 0) type = INCRBY;
        else if (strcmp(token, "decrby") == 0) type = DECRBY;
        else if (strcmp(token, "strlen") == 0) type = STRLEN;
        else if (strcmp(token, "hset") == 0) type = HSET;
        else if (strcmp(token, "hget") == 0) type = HGET;
        else if (strcmp(token, "hdel") == 0) type = HDEL;
        else if (strcmp(token, "hgetall") == 0) type = HGETALL;
        else if (strcmp(token, "hexists") == 0) type = HEXISTS;
        else if (strcmp(token, "hkeys") == 0) type = HKEYS;
        else if (strcmp(token, "hvals") == 0) type = HVALS;
        else if (strcmp(token, "hmget") == 0) type = HMGET;
        else if (strcmp(token, "hlen") == 0) type = HLEN;
        else if (strcmp(token, "lpush") == 0) type = LPUSH;
        else if (strcmp(token, "lpop") == 0) type = LPOP;
        else if (strcmp(token, "rpush") == 0) type = RPUSH;
        else if (strcmp(token, "llen") == 0) type = LLEN;
        else if (strcmp(token, "lindex") == 0) type = LINDEX;
        else if (strcmp(token, "lrange") == 0) type = LRANGE;
        else if (strcmp(token, "lset") == 0) type = LSET;
        else if (strcmp(token, "lrem") == 0) type = LREM;
        else if (strcmp(token, "lpos") == 0) type = LPOS;
        else if (strcmp(token, "rpop") == 0) type = RPOP;
        else if (strcmp(token, "sadd") == 0) type = SADD;
        else if (strcmp(token, "srem") == 0) type = SREM;
        else if (strcmp(token, "sismember") == 0) type = SISMEMBER;
        else if (strcmp(token, "smembers") == 0) type = SMEMBERS;
        else if (strcmp(token, "smismember") == 0) type = SMISMEMBER;
        else if (strcmp(token, "quit") == 0) type = QUIT;
        else if (strcmp(token, "shutdown") == 0) type = SHUTDOWN;
        else type = UNKNOWN;
        
        // parse arguments
        char **args = dmalloc(argc * sizeof(char *));
        for (int i = 0; i < argc; i++) {
            token = get_next_token(parser);
            args[i] = dmalloc((strlen(token) + 1) * sizeof(char));
            strcpy(args[i], token);
        }
        cmd = command_init(type, argc, args);
    } else {
        cmd = command_init(NOOP, 0, NULL);
    }

    parser_free(parser);
    free(token);
    return cmd;
}


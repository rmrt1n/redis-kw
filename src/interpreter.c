#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

static char *reply_string(char *str) {
    if (str == NULL) return strdup("$-1\r\n");
    int n = strlen(str);
    char *res = calloc(n + ndigits(n) + 5, sizeof(char));
    sprintf(res, "$%d\r\n%s\r\n", n, str);
    return res;
}

static char *reply_integer(int x) {
    char *res = calloc(ndigits(x) + 3, sizeof(char));
    sprintf(res, ":%d\r\n", x);
    return res;
}

static char *reply_array_n(char **arr, int n) {
    char *res = calloc(ndigits(n) + 3, sizeof(char));
    sprintf(res, "*%d\r\n", n);
    for (int i = 0; i < n; i++) {
        char *tmp;
        if (arr[i] == NULL)  {
            tmp = reply_string(arr[i]);
        } else {
            tmp = is_number(arr[i])
                ? reply_integer(strtoi(arr[i]))
                : reply_string(arr[i]);
        }
        strncat(res, tmp, strlen(tmp));
    }
    return res;
}

static char *reply_array(char **arr) {
    if (arr == NULL) return strdup("*0\r\n");
    int n = 0;
    char *res = calloc(1, sizeof(char));
    while (*arr != NULL) {
        char *tmp = is_number(*arr)
            ? reply_integer(strtoi(*arr))
            : reply_string(*arr);
        strncat(res, tmp, strlen(tmp));
        arr++;
        n++;
    }
    char *new_res = malloc(strlen(res) + ndigits(n) + 3);
    sprintf(new_res, "*%d\r\n%s", n, res);
    return new_res;
}

static char *reply_err_argc(int given, char *expected) {
    char *res = calloc(ndigits(given) + strlen(expected) + 52, sizeof(char));
    sprintf(res, "-ERR wrong number of arguments (given %d, expected %s)\r\n",
            given, expected);
    return res;
}

static char *reply_err_type() {
    return strdup("-ERR wrongtype operation\r\n");
}

static bool is_type(char *given, char *expected) {
    return strcmp(given, expected) == 0 || strcmp(given, "none") == 0;
}

char *exec_del(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 1) {
        int oks = 0;
        for (int i = 0; i < cmd->argc; i++) {
            oks += htable_del(ht, cmd->argv[i]);
        }
        return reply_integer(oks);
    }
    return reply_err_argc(cmd->argc, "1+");
}

char *exec_exists(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 1) {
        int oks = 0;
        for (int i = 0; i < cmd->argc; i++) {
            oks += htable_exists(ht, cmd->argv[i]);
        }
        return reply_integer(oks);
    }
    return reply_err_argc(cmd->argc, "1+");
}

char *exec_type(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *res = htable_type(ht, cmd->argv[0]);
        return reply_string(res);
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_set(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 0 && cmd->argc <= 2) {
        if (cmd->argc == 1) htable_set(ht, cmd->argv[0], "");
        if (cmd->argc == 2) htable_set(ht, cmd->argv[0], cmd->argv[1]);
        return reply_string("OK");
    }
    return reply_err_argc(cmd->argc, "0..2");
}

char *exec_get(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "string")) {
            free(type);
            char *res = htable_get(ht, cmd->argv[0]);
            return reply_string(res);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_mset(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 2 && cmd->argc % 2 == 0) {
        for (int i = 0; i < cmd->argc; i += 2) {
            htable_set(ht, cmd->argv[i], cmd->argv[i+1]);
        }
        return reply_string("OK");
    }
    return reply_err_argc(cmd->argc, "2+");
}

char *exec_mget(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "string")) {
            free(type);
            char **res = calloc(cmd->argc, sizeof(char *));
            for (int i = 0; i < cmd->argc; i++) {
                char *tmp = htable_get(ht, cmd->argv[i]);
                res[i] = tmp == NULL ? NULL : strdup(tmp);
            }
            return reply_array_n(res, cmd->argc);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1+");
}

char *exec_hset(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 3 && cmd->argc % 2 == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            free(type);
            int oks = 0;
            for (int i = 1; i < cmd->argc; i += 2) {
                oks += htable_hset(ht, cmd->argv[0],
                                   cmd->argv[i], cmd->argv[i+1]);
            }
            return reply_integer(oks);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "3+");
}

char *exec_hget(HashTable *ht, Command *cmd) {
    if (cmd->argc == 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            free(type);
            char *res = htable_hget(ht, cmd->argv[0], cmd->argv[1]);
            return reply_string(res);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2");
}

char *exec_hdel(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            free(type);
            int oks = 0;
            for (int i = 1; i < cmd->argc; i++) {
                oks += htable_hdel(ht, cmd->argv[0], cmd->argv[i]);
            }
            return reply_integer(oks);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2+");
}

char *exec_hgetall(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            char **res = htable_hgetall(ht, cmd->argv[0]);
            return reply_array(res);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_hlen(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            return reply_integer(htable_hlen(ht, cmd->argv[0]));
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_push(HashTable *ht, Command *cmd, int dir) {
    if (cmd->argc >= 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "list")) {
            free(type);
            int len;
            for (int i = 1; i < cmd->argc; i++) {
                len = htable_push(ht, cmd->argv[0], cmd->argv[i], dir);
            }
            return reply_integer(len);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2+");
}

char *exec_pop(HashTable *ht, Command *cmd, int dir) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "list")) {
            free(type);
            char *res = htable_pop(ht, cmd->argv[0], dir);
            return reply_string(res);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_llen(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "list")) {
            return reply_integer(htable_llen(ht, cmd->argv[0]));
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_sadd(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "set")) {
            free(type);
            int oks = 0;
            for (int i = 1; i < cmd->argc; i++) {
                oks += htable_sadd(ht, cmd->argv[0], cmd->argv[i]);
            }
            return reply_integer(oks);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2+");
}

char *exec_srem(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "set")) {
            int oks = 0;
            for (int i = 1; i < cmd->argc; i++) {
                oks += htable_srem(ht, cmd->argv[0], cmd->argv[i]);
            }
            return reply_integer(oks);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2+");
}

// char *exec_(HashTable *ht, Command *cmd) {
    // if (cmd->argc) {
        // char *type = htable_type(ht, cmd->argv[0]);
        // if (is_type(type, "")) {

        // }
        // return reply_err_type();
    // }
    // return reply_err_argc(cmd->argc, "");
// }

char *exec_unknown(Command *cmd) {
    return strdup("-ERR unrecognized command\r\n");
}

char *interpret(Command *cmd, HashTable *ht) {
    char *res;
    switch (cmd->type) {
        case DEL: res = exec_del(ht, cmd); break;
        case EXISTS: res = exec_exists(ht, cmd); break;
        case TYPE: res = exec_type(ht, cmd); break;
        case SET: res = exec_set(ht, cmd); break;
        case GET: res = exec_get(ht, cmd); break;
        case MSET: res = exec_mset(ht, cmd); break;
        case MGET: res = exec_mget(ht, cmd); break;
        // case INCR: res = exec_incr(ht, cmd); break;
        // case DECR: res = exec_decr(ht, cmd); break;
        // case INCRBY: res = exec_incrby(ht, cmd); break;
        // case DECRBY: res = exec_decrby(ht, cmd); break;
        // case STRLEN: res = exec_strlen(ht, cmd); break;
        case HSET: res = exec_hset(ht, cmd); break;
        case HGET: res = exec_hget(ht, cmd); break;
        case HDEL: res = exec_hdel(ht, cmd); break;
        case HGETALL: res = exec_hgetall(ht, cmd); break;
        // case HEXISTS: res = exec_hexists(ht, cmd); break;
        // case HKEYS: res = exec_hkeys(ht, cmd); break;
        // case HVALS: res = exec_hvals(ht, cmd); break;
        // case HMGET: res = exec_hmget(ht, cmd); break;
        case HLEN: res = exec_hlen(ht, cmd); break;
        case LPUSH: res = exec_push(ht, cmd, LEFT); break;
        case LPOP: res = exec_pop(ht, cmd, LEFT); break;
        case RPUSH: res = exec_push(ht, cmd, RIGHT); break;
        case RPOP: res = exec_pop(ht, cmd, RIGHT); break;
        case LLEN: res = exec_llen(ht, cmd); break;
        // case LINDEX: res = exec_lindex(ht, cmd); break;
        // case LRANGE: res = exec_lrange(ht, cmd); break;
        // case LSET: res = exec_lset(ht, cmd); break;
        case SADD: res = exec_sadd(ht, cmd); break;
        case SREM: res = exec_srem(ht, cmd); break;
        // case SISMEMBER: res = exec_sismember(ht, cmd); break;
        // case SMEMBERS: res = exec_smembers(ht, cmd); break;
        // case SMISMEMBER: res = exec_smismember(ht, cmd); break;
        case UNKNOWN: res = exec_unknown(cmd); break;
        case QUIT: res = strdup("q"); break;
        case SHUTDOWN: res = strdup("x"); break;
        case NOOP: res = strdup("");
        // default: break;
    }
    command_free(cmd);
    return res;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

static char *reply_string(char *str) {
    if (str == NULL) return strdup("$-1\r\n");
    int n = strlen(str);
    char *res = dmalloc((n + ndigits(n) + 7) * sizeof(char));
    sprintf(res, "$%d\r\n%s\r\n", n, str);
    return res;
}

static char *reply_integer(int x) {
    char *res = dmalloc((ndigits(x) + 5) * sizeof(char));
    sprintf(res, ":%d\r\n", x);
    return res;
}

static char *reply_array_n(char **arr, int n) {
    char *res = dmalloc((ndigits(n) + 5) * sizeof(char));
    sprintf(res, "*%d\r\n", n);
    for (int i = 0; i < n; i++) {
        char *tmp;
        if (arr[i] == NULL)  {
            tmp = reply_string(NULL);
        } else {
            tmp = is_number(arr[i])
                ? reply_integer(strtoi(arr[i]))
                : reply_string(arr[i]);
        }
        res = drealloc(res, (strlen(res) + strlen(tmp) + 1) * sizeof(char));
        strncat(res, tmp, strlen(tmp) + 1);
    }
    return res;
}

static char *reply_array(char **arr) {
    if (arr == NULL) return strdup("*0\r\n");
    char *res = dmalloc(sizeof(char));
    *res = '\0';
    int n = 0;
    while (*arr != NULL) {
        char *tmp = is_number(*arr)
            ? reply_integer(strtoi(*arr))
            : reply_string(*arr);
        res = drealloc(res, (strlen(res) + strlen(tmp) + 1) * sizeof(char));
        strncat(res, tmp, strlen(tmp) + 1);
        arr++;
        n++;
    }
    char *new_res = dmalloc((strlen(res) + ndigits(n) + 5) * sizeof(char));
    sprintf(new_res, "*%d\r\n%s", n, res);
    return new_res;
}

static char *reply_err_argc(int given, char *expected) {
    char *res = dmalloc((ndigits(given) + strlen(expected) + 54) * sizeof(char));
    sprintf(res, "-ERR wrong number of arguments (given %d, expected %s)\r\n",
            given, expected);
    return res;
}

static char *reply_err_type() {
    return strdup("-ERR wrongtype operation\r\n");
}

static char *reply_err_intid() {
    return strdup("-ERR value is not an integer or out of range\r\n");
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
            char **res = dmalloc(cmd->argc * sizeof(char *));
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

char *exec_incr(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "string")) {
            free(type);
            char *res = htable_get(ht, cmd->argv[0]);
            if (res == NULL) {
                htable_set(ht, cmd->argv[0], "1");
                return reply_integer(1);
            }
            if (is_number(res)) {
                int tmp = strtoi(res) + 1;
                htable_set(ht, cmd->argv[0], intostr(tmp));
                return reply_integer(tmp);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_decr(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "string")) {
            free(type);
            char *res = htable_get(ht, cmd->argv[0]);
            if (res == NULL) {
                htable_set(ht, cmd->argv[0], "-1");
                return reply_integer(-1);
            }
            if (is_number(res)) {
                int tmp = strtoi(res) - 1;
                htable_set(ht, cmd->argv[0], intostr(tmp));
                return reply_integer(tmp);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_incrby(HashTable *ht, Command *cmd) {
    if (cmd->argc == 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "string")) {
            free(type);
            char *res = htable_get(ht, cmd->argv[0]);
            if (res == NULL && is_number(cmd->argv[1])) {
                int tmp = strtoi(cmd->argv[1]);
                htable_set(ht, cmd->argv[0], intostr(tmp));
                return reply_integer(tmp);
            }
            if (is_number(res) && is_number(cmd->argv[1])) {
                int tmp = strtoi(res) + strtoi(cmd->argv[1]);
                htable_set(ht, cmd->argv[0], intostr(tmp));
                return reply_integer(tmp);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2");
}

char *exec_decrby(HashTable *ht, Command *cmd) {
    if (cmd->argc == 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "string")) {
            free(type);
            char *res = htable_get(ht, cmd->argv[0]);
            if (res == NULL && is_number(cmd->argv[1])) {
                int tmp = strtoi(cmd->argv[1]);
                htable_set(ht, cmd->argv[0], intostr(tmp));
                return reply_integer(tmp);
            }
            if (is_number(res) && is_number(cmd->argv[1])) {
                int tmp = strtoi(res) - strtoi(cmd->argv[1]);
                htable_set(ht, cmd->argv[0], intostr(tmp));
                return reply_integer(tmp);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2");
}

char *exec_strlen(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "string")) {
            free(type);
            char *res = htable_get(ht, cmd->argv[0]);
            return reply_integer(res == NULL ? 0 : strlen(res));
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
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
            free(type);
            char **res = htable_hgetall(ht, cmd->argv[0]);
            return reply_array(res);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_hexists(HashTable *ht, Command *cmd) {
    if (cmd->argc == 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            free(type);
            char *res = htable_hget(ht, cmd->argv[0], cmd->argv[1]);
            return reply_integer(res == NULL ? 0 : 1);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2");
}

char *exec_hkeyvals(HashTable *ht, Command *cmd, int key) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            free(type);
            char **res = htable_hkeyvals(ht, cmd->argv[0], key);
            return reply_array(res);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_hmget(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            free(type);
            if (!htable_exists(ht, cmd->argv[0])) return reply_array(NULL);
            char **res = dmalloc((cmd->argc - 1) * sizeof(char *));
            int id = 0;
            for (int i = 1; i < cmd->argc; i++) {
                char *tmp = htable_hget(ht, cmd->argv[0], cmd->argv[i]);
                res[id++] = tmp == NULL ? NULL : strdup(tmp);
            }
            return reply_array_n(res, cmd->argc - 1);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2+");
}

char *exec_hlen(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "hash")) {
            free(type);
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
            free(type);
            return reply_integer(htable_llen(ht, cmd->argv[0]));
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_lindex(HashTable *ht, Command *cmd) {
    if (cmd->argc == 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "list")) {
            free(type);
            if (is_number(cmd->argv[1])) {
                int id = strtoi(cmd->argv[1]);
                int code = htable_check_id(ht, cmd->argv[0], &id);
                if (!code) return reply_err_intid();
                char *res = code > 0 
                    ? htable_lindex(ht, cmd->argv[0], id)
                    : NULL;
                return reply_string(res);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2");
}

char *exec_lrange(HashTable *ht, Command *cmd) {
    if (cmd->argc == 3) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "list")) {
            free(type);
            if (is_number(cmd->argv[1]) && is_number(cmd->argv[2])) {
                int bgn = strtoi(cmd->argv[1]), end = strtoi(cmd->argv[2]);
                int code = htable_check_ids(ht, cmd->argv[0], &bgn, &end);
                if (!code) return reply_err_intid();
                char **res = code > 0
                    ? htable_lrange(ht, cmd->argv[0], bgn, end)
                    : NULL;
                return reply_array(res);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "3");
}

char *exec_lset(HashTable *ht, Command *cmd) {
    if (cmd->argc == 3) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "list")) {
            free(type);
            if (is_number(cmd->argv[1])) {
                int id = strtoi(cmd->argv[1]);
                int code = htable_check_id(ht, cmd->argv[0], &id);
                if (!code) return reply_err_intid();
                htable_lset(ht, cmd->argv[0], id, cmd->argv[2]);
                char *res = code > 0 ?  "OK" : NULL;
                return reply_string(res);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "3");
}

char *exec_lrem(HashTable *ht, Command *cmd) {
    if (cmd->argc == 3) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "list")) {
            free(type);
            if (is_number(cmd->argv[1])) {
                int count = strtoi(cmd->argv[1]);
                int res = htable_lrem(ht, cmd->argv[0], count, cmd->argv[1]);
                return reply_integer(res);
            }
            return reply_err_intid();
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "3");
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
            free(type);
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

char *exec_sismember(HashTable *ht, Command *cmd) {
    if (cmd->argc == 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "set")) {
            free(type);
            int x = htable_sismember(ht, cmd->argv[0], cmd->argv[1]);
            return reply_integer(x);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2");
}

char *exec_smembers(HashTable *ht, Command *cmd) {
    if (cmd->argc == 1) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "set")) {
            free(type);
            char **res = htable_smembers(ht, cmd->argv[0]);
            return reply_array(res);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "1");
}

char *exec_smismember(HashTable *ht, Command *cmd) {
    if (cmd->argc >= 2) {
        char *type = htable_type(ht, cmd->argv[0]);
        if (is_type(type, "set")) {
            free(type);
            if (!htable_exists(ht, cmd->argv[0])) return reply_array(NULL);
            char **res = dmalloc((cmd->argc - 1) * sizeof(char *));
            int id = 0;
            for (int i = 1; i < cmd->argc; i++) {
                int x = htable_sismember(ht, cmd->argv[0], cmd->argv[i]);
                res[id++] = intostr(x);
            }
            return reply_array_n(res, cmd->argc - 1);
        }
        return reply_err_type();
    }
    return reply_err_argc(cmd->argc, "2+");
}

// char *exec_(HashTable *ht, Command *cmd) {
    // if (cmd->argc) {
        // char *type = htable_type(ht, cmd->argv[0]);
        // if (is_type(type, "")) {
            // free(type);
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
        case INCR: res = exec_incr(ht, cmd); break;
        case DECR: res = exec_decr(ht, cmd); break;
        case INCRBY: res = exec_incrby(ht, cmd); break;
        case DECRBY: res = exec_decrby(ht, cmd); break;
        case STRLEN: res = exec_strlen(ht, cmd); break;
        case HSET: res = exec_hset(ht, cmd); break;
        case HGET: res = exec_hget(ht, cmd); break;
        case HDEL: res = exec_hdel(ht, cmd); break;
        case HGETALL: res = exec_hgetall(ht, cmd); break;
        case HEXISTS: res = exec_hexists(ht, cmd); break;
        case HKEYS: res = exec_hkeyvals(ht, cmd, 1); break;
        case HVALS: res = exec_hkeyvals(ht, cmd, 0); break;
        case HMGET: res = exec_hmget(ht, cmd); break;
        case HLEN: res = exec_hlen(ht, cmd); break;
        case LPUSH: res = exec_push(ht, cmd, LEFT); break;
        case LPOP: res = exec_pop(ht, cmd, LEFT); break;
        case RPUSH: res = exec_push(ht, cmd, RIGHT); break;
        case RPOP: res = exec_pop(ht, cmd, RIGHT); break;
        case LLEN: res = exec_llen(ht, cmd); break;
        case LINDEX: res = exec_lindex(ht, cmd); break;
        case LRANGE: res = exec_lrange(ht, cmd); break;
        case LSET: res = exec_lset(ht, cmd); break;
        case LREM: res = exec_lrem(ht, cmd); break;
        case SADD: res = exec_sadd(ht, cmd); break;
        case SREM: res = exec_srem(ht, cmd); break;
        case SISMEMBER: res = exec_sismember(ht, cmd); break;
        case SMEMBERS: res = exec_smembers(ht, cmd); break;
        case SMISMEMBER: res = exec_smismember(ht, cmd); break;
        case UNKNOWN: res = exec_unknown(cmd); break;
        case QUIT: res = strdup("q"); break;
        case SHUTDOWN: res = strdup("x"); break;
        case NOOP: res = strdup("");
    }
    command_free(cmd);
    return res;
}


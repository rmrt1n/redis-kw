#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "interpreter.h"
#include "htable.h"
#include "parser.h"

static int ndigits(int x) {
    int n = x < 0 ? x * -1 : x;
    int res = 0;
    while (n > 0) {
        n /= 10;
        res++;
    }
    return res;
}

static int isnumber(char *str) {
    int i = *str == '-' ? 1 : 0;
    int n = strlen(str);
    for (; i < n; i++) {
        if (isdigit(str[i]) == 0) return 0;
    }
    return 1;
}

static int strtoi(char *str) {
    int res = 0, i = *str == '-' ? 1 : 0;
    int neg = i, n = strlen(str);
    for (; i < n; i++) {
        res = res * 10 + (str[i] - '0');
    }
    if (neg) res *= -1;
    return res;
}

static char *substr(char *str, int start, int end) {
    if (start > end) {
        return "\"\"";
    } else {
        char *res = malloc(end - start + 1);
        snprintf(res, end - start + 2, "%s", str+start);
        return res;
    }
}

static char *quote_encase(char *str) {
    char *tmp = malloc(strlen(str) + 3);
    sprintf(tmp, "\"%s\"", str);
    return tmp;
}

static void print_wrong_argc(int client_sock, int given, int expected) {
    char *tmp = malloc(50 + ndigits(given) + ndigits(expected));
    sprintf(tmp, "ERR wrong number of arguments (given %d, expected %d)",
            given, expected);
    writeline(client_sock, tmp);
    free(tmp);
}

static void print_status(int client_sock, HtableAction action) {
    if (action.status == SUCCESS) {
        writeline(client_sock, "OK");
    } else if (action.status == ERR){
        writeline(client_sock,
                  "WRONGTYPE Operation against a key holding the wrong kind of value");
    } else {
        writeline(client_sock, "(nil)");
    }
}

static void exec_del(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "ERR wrong number of arguments for 'del'");
    } else {
        int success = 0;
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction instr = htable_del(htable, cmd->args[i]); 
            if (instr.status == SUCCESS) success++;
        }
        char *tmp = malloc(ndigits(success) + 1);
        sprintf(tmp, "%d", success);
        writeline(client_sock, tmp);
        free(tmp);
    }
}

void exec_exists(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "ERR wrong number of arguments for 'exists'");
    } else {
        int success = 0;
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction instr = htable_get(htable, cmd->args[i]);
            if (instr.status == SUCCESS) success++;
        }
        char *tmp = malloc(ndigits(success) + 1);
        sprintf(tmp, "%d", success);
        writeline(client_sock, tmp);
        free(tmp);
    }
}

static void exec_set(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "OK");
    } else if (cmd->argc == 1) {
        HtableAction instr = htable_set(htable, cmd->args[0], ""); 
        print_status(client_sock, instr);
    } else if (cmd->argc == 2) {
        HtableAction instr = htable_set(htable, cmd->args[0], cmd->args[1]); 
        print_status(client_sock, instr);
    } else {
        writeline(client_sock, "ERR syntax error");
    }
}

void exec_get(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]); 
        if (instr.status == SUCCESS) {
            char *tmp = malloc(strlen(instr.value) + 2);
            sprintf(tmp, "\"%s\"", instr.value);
            writeline(client_sock, tmp);
            free(tmp);
        } else {
            print_status(client_sock, instr);
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 1);
    }
}

void exec_getrange(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 3) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            if (isnumber(cmd->args[1]) && isnumber(cmd->args[2])) {
                int n = strlen(instr.value);
                int start = strtoi(cmd->args[1]), end = strtoi(cmd->args[2]);
                start = start < 0 ? n + start : start;
                end = end < 0 ? n + end : end;
                if ((start >= 0 && start < n) && (end >= 0 && end < n)) {
                    writeline(client_sock, substr(instr.value, start, end));
                } else {
                    writeline(client_sock, "ERR value is not an integer or out of range");
                }
            } else {
                writeline(client_sock, "ERR value is not an integer or out of range");
            }
        } else {
            print_status(client_sock, instr);
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 3);
    }
}

void exec_getset(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 2) {
        HtableAction get_instr = htable_get(htable, cmd->args[0]);
        if (get_instr.status == SUCCESS) {
            char *tmp = malloc(strlen(get_instr.value) + 2);
            sprintf(tmp, "\"%s\"", get_instr.value);
            writeline(client_sock, tmp);
            free(tmp);
        }
        htable_set(htable, cmd->args[0], cmd->args[1]);
    } else {
        print_wrong_argc(client_sock, cmd->argc, 2);
    }
}

void exec_mget(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "ERR wrong number of arguments for 'mget'");
    } else {
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction instr = htable_get(htable, cmd->args[i]);
            if (instr.status == SUCCESS) {
                // + 5 = '\0' + "" + ) + ' '
                char *tmp = malloc(strlen(instr.value) + ndigits(i + 1) + 5);
                char *quoted_val = quote_encase(instr.value);
                sprintf(tmp, "%d) %s", i + 1, quoted_val);
                writeline(client_sock, tmp);
                free(tmp);
                free(quoted_val);
            } else {
                // + 8 = '\0' + ") (nil)"
                char *tmp = malloc(ndigits(i + 1) + 8);
                sprintf(tmp, "%d) (nil)", i + 1);
                writeline(client_sock, tmp);
                free(tmp);
            }
        }
    }
}

void exec_strlen(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            int n = strlen(instr.value);
            char *tmp = malloc(n + 1);
            sprintf(tmp, "%d", n);
            writeline(client_sock, tmp);
            free(tmp);
        } else {
            writeline(client_sock, "0");
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 1);
    }
}

void exec_incr(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            if (isnumber(instr.value)) {
                int n = strtoi(instr.value) + 1;
                char *val = malloc(ndigits(n) + 2);
                sprintf(val, "%d", n);
                HtableAction set_instr = htable_set(htable, cmd->args[0], val);
                print_status(client_sock, set_instr);
            } else {
                writeline(client_sock, "ERR value is not an integer or out of range");
            }
        } else {
            HtableAction instr = htable_set(htable, cmd->args[0], "1");
            print_status(client_sock, instr);
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 1);
    }
}

void exec_decr(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction instr = htable_get(htable, cmd->args[0]);
        if (instr.status == SUCCESS) {
            if (isnumber(instr.value)) {
                int n = strtoi(instr.value) - 1;
                char *val = malloc(ndigits(n) + 2);
                sprintf(val, "%d", n);
                HtableAction set_instr = htable_set(htable, cmd->args[0], val);
                print_status(client_sock, set_instr);
            } else {
                writeline(client_sock, "ERR value is not an integer or out of range");
            }
        } else {
            HtableAction instr = htable_set(htable, cmd->args[0], "-1");
            print_status(client_sock, instr);
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 1);
    }
}

void exec_hset(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc >= 3 && cmd->argc % 2 == 1) {
        for (int i = 1; i < cmd->argc; i += 2) {
            HtableAction instr = htable_hset(htable, cmd->args[0],
                                             cmd->args[i], cmd->args[i+1]);
            print_status(client_sock, instr);
        }
    } else {
        writeline(client_sock, "ERR wrong number of arguments for 'hset'");
    }
}

void exec_hget(int client_sock, HashTable *htable, Command *cmd) {
    if (cmd->argc == 2) {
        HtableAction instr = htable_hget(htable, cmd->args[0], cmd->args[1]);
        if (instr.status == SUCCESS) {
            char *tmp = quote_encase(instr.value);
            writeline(client_sock, tmp);
            free(tmp);
        } else {
            print_status(client_sock, instr);
        }
    } else {
        print_wrong_argc(client_sock, cmd->argc, 2);
    }
}

void exec_ping(int client_sock, Command *cmd) {
    if (cmd->argc == 0) {
        writeline(client_sock, "PONG");
    } else if (cmd->argc == 1) {
        char *tmp = malloc(strlen(cmd->args[0]) + 1);
        sprintf(tmp, "\"%s\"", cmd->args[0]);
        writeline(client_sock, tmp);
        free(tmp);
    } else {
        char tmp[58];
        sprintf(tmp, "ERR wrong number of arguments (given %d, expected 0..1)", cmd->argc);
        writeline(client_sock, tmp);
    }
}

void exec_unknown(int client_sock) {
    writeline(client_sock, "ERR unrecognized command");
}

void execute(int client_sock, HashTable *htable, char *msg) {
    Command *cmd = parse(msg);
    switch (cmd->type) {
        case DEL: exec_del(client_sock, htable, cmd); break;
        case EXISTS: exec_exists(client_sock, htable, cmd); break;
        case SET: exec_set(client_sock, htable, cmd); break;
        case GET: exec_get(client_sock, htable, cmd); break;
        case GETRANGE: exec_getrange(client_sock, htable, cmd); break;
        case GETSET: exec_getset(client_sock, htable, cmd); break;
        case MGET: exec_mget(client_sock, htable, cmd); break;
        case STRLEN: exec_strlen(client_sock, htable, cmd); break;
        case INCR: exec_incr(client_sock, htable, cmd); break;
        case DECR: exec_decr(client_sock, htable, cmd); break;
        case HSET: exec_hset(client_sock, htable, cmd); break;
        case HGET: exec_hget(client_sock, htable, cmd); break;
        case PING: exec_ping(client_sock, cmd); break;
        case UNKNOWN: exec_unknown(client_sock); break;
        case QUIT: exit(0); break;
        case NOOP: 
        default: break; 
    }
    free(cmd->args);
    free(cmd);
}


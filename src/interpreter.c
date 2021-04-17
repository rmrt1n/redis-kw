#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "common.h"

static int ndigits(int x) {
    int n = x < 0 ? x * -1 : x;
    int res = 0;
    while (n > 0) {
        n /= 10;
        res++;
    }
    return res;
}

static void print_status(int client, HtableAction action) {
    switch (action.status) {
        case OK: writeline(client, "OK"); break;
        case NIL: writeline(client, "(nil)"); break;
        case ERR: writeline(client, "ERR wrongtype operation"); break;
        default: writeline(client, "Unknown status code");
    }
}

static void print_quote_encase(int client, char *str) {
    char *tmp = malloc(strlen(str) + 3);
    sprintf(tmp, "\"%s\"", str);
    writeline(client, tmp);
    free_all(1, tmp);
}

static void print_wrong_argc(int client, int given, char *expected) {
    char *tmp = malloc(50 + ndigits(given) + strlen(expected));
    sprintf(tmp, "ERR wrong number of arguments (given %d, expected %s)",
            given, expected);
    writeline(client, tmp);
    free_all(1, tmp);
}

static void print_integer(int client, int n) {
    char *tmp = malloc(11 + ndigits(n));
    sprintf(tmp, "(integer) %d", n);
    writeline(client, tmp);
    free_all(1, tmp);
}

static void print_numbering(int client, int n) {
    char *tmp = malloc(3 + ndigits(n));
    sprintf(tmp, "%d) ", n);
    write(client, tmp, strlen(tmp) + 1);
    free_all(1, tmp);
}

static void exec_set(int client, HashTable *htable, Command *cmd) {
    HtableAction res;
    switch (cmd->argc) {
        case 0:
            writeline(client, "OK");
            break;
        case 1:
            res = htable_set(htable, cmd->argv[0], "");
            print_status(client, res);
            break;
        case 2:
            res = htable_set(htable, cmd->argv[0], cmd->argv[1]);
            print_status(client, res);
            break;
        default:
            writeline(client, "ERR syntax error");
    }
}

static void exec_get(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction res = htable_get(htable, cmd->argv[0]);
        if (res.status == OK) {
            print_quote_encase(client, res.value);
        } else {
            print_status(client, res);
        }
    } else {
        print_wrong_argc(client, cmd->argc, "1");
    }
}

static void exec_mset(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc >= 2 && cmd->argc % 2 == 0) {
        for (int i = 0; i < cmd->argc; i+= 2) {
            htable_set(htable, cmd->argv[i], cmd->argv[i+1]);
        }
        writeline(client, "OK");
    } else {
        writeline(client, "ERR wrong number of arguments for 'mset'");
    }
}

static void exec_mget(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc > 0) {
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction res = htable_get(htable, cmd->argv[i]);
            print_numbering(client, i + 1);
            if (res.status == OK) {
                print_quote_encase(client, res.value);
            } else {
                writeline(client, "(nil)");
            }
        }
    } else {
        writeline(client, "ERR wrong number of arguments for 'mget'");
    }
}

static void exec_del(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc > 0) {
        int oks = 0;
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction res = htable_del(htable, cmd->argv[i]);
            if (res.status == OK) oks++;
        }
        print_integer(client, oks);
    } else {
        writeline(client, "ERR wrong number of arguments for 'del'");
    }
}

static void exec_exists(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc > 0) {
        int oks = 0;
        for (int i = 0; i < cmd->argc; i++) {
            HtableAction res = htable_exists(htable, cmd->argv[i]);
            if (res.status == OK) oks++;
        }
        print_integer(client, oks);
    } else {
        writeline(client, "ERR wrong number of arguments for 'exists'");
    }
}

static void exec_type(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc == 1) {
        HtableAction res = htable_type(htable, cmd->argv[0]);
        print_quote_encase(client, res.status == OK ? res.value : "none");
    } else {
        print_wrong_argc(client, cmd->argc, "1");
    }
}

static void exec_hset(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc >= 3 && cmd->argc % 2 == 1) {
        int oks = 0;
        for (int i = 1; i < cmd->argc; i += 2) {
            HtableAction res = htable_hset(htable, cmd->argv[0],
                                           cmd->argv[i], cmd->argv[i+1]);
            if (res.status == ERR) {
                print_status(client, res);
                return;
            }
            oks++;
        }
        print_integer(client, oks);
    } else {
        writeline(client, "ERR wrong number of arguments for 'hset'");
    }
}

static void exec_hget(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc == 2) {
        HtableAction res = htable_hget(htable, cmd->argv[0], cmd->argv[1]);
        if (res.status == OK) {
            print_quote_encase(client, res.value);
        } else {
            print_status(client, res);
        }
    } else {
        print_wrong_argc(client, cmd->argc, "2");
    }
}

static void exec_hdel(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc > 1) {
        int oks = 0;
        for (int i = 1; i < cmd->argc; i++) {
            HtableAction res = htable_hdel(htable, cmd->argv[0], cmd->argv[i]);
            switch (res.status) {
                case OK: oks++; break;
                case NIL: continue; break;
                case ERR: print_status(client, res); return; break;
            }
        }
        print_integer(client, oks);
    } else {
        print_wrong_argc(client, cmd->argc, "1+");
    }
}

static void exec_push(int client, HashTable *htable, Command *cmd, int dir) {
    if (cmd->argc > 1) {
        int oks = 0;
        for (int i = 1; i < cmd->argc; i++) {
            HtableAction res = htable_push(htable, cmd->argv[0],
                                           cmd->argv[i], dir);
            if (res.status == ERR) {
                print_status(client, res);
                return;
            }
            oks++;
        }
        print_integer(client, oks);
    } else {
        if (dir == LEFT) {
            writeline(client, "ERR wrong number of arguments for 'lpush'");
        } else {
            writeline(client, "ERR wrong number of arguments for 'rpush'");
        }
    }
}

static void exec_pop(int client, HashTable *htable, Command *cmd, int dir) {
    if (cmd->argc == 1) {
        HtableAction res = htable_pop(htable, cmd->argv[0], dir);
        if (res.status == OK) {
            print_quote_encase(client, res.value);
        } else {
            print_status(client, res);
        }
    } else {
        print_wrong_argc(client, cmd->argc, "1");
    }
}

static void exec_sadd(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc > 1) {
        int oks = 0;
        for (int i = 1; i < cmd->argc; i++) {
            HtableAction res = htable_sadd(htable, cmd->argv[0], cmd->argv[i]);
            switch (res.status) {
                case OK: oks++; break;
                case NIL: continue; break;
                case ERR: print_status(client, res); return; break;
            }
        }
        print_integer(client, oks);
    } else {
        writeline(client, "ERR wrong number of arguments for 'sadd'");
    }
}

static void exec_srem(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc > 1) {
        int oks = 0;
        for (int i = 1; i < cmd->argc; i++) {
            HtableAction res = htable_srem(htable, cmd->argv[0], cmd->argv[i]);
            switch (res.status) {
                case OK: oks++; break;
                case NIL: continue; break;
                case ERR: print_status(client, res); return; break;
            }
        }
        print_integer(client, oks);    
    } else {
        writeline(client, "ERR wrong number of arguments for 'srem'");
    }
}

static void exec_sismember(int client, HashTable *htable, Command *cmd) {
    if (cmd->argc == 2) {
        HtableAction res = htable_sismember(htable, cmd->argv[0], cmd->argv[1]);
        switch (res.status) {
            case OK: print_integer(client, 1); break;
            case NIL: print_integer(client, 0); break;
            case ERR: print_status(client, res); break;
        }
    } else {
        print_wrong_argc(client, cmd->argc, "2");
    }
}

static void exec_unknown(int client) {
    writeline(client, "ERR unrecognized command");
}

void execute(int client, HashTable *htable, char *msg) {
    Command *cmd = parse(msg);
    switch (cmd->type) {
        case DEL: exec_del(client, htable, cmd); break;
        case EXISTS: exec_exists(client, htable, cmd); break;
        case TYPE: exec_type(client, htable, cmd); break;
        case SET: exec_set(client, htable, cmd); break;
        case GET: exec_get(client, htable, cmd); break;
        case MSET: exec_mset(client, htable, cmd); break;
        case MGET: exec_mget(client, htable, cmd); break;
        case HSET: exec_hset(client, htable, cmd); break;
        case HGET: exec_hget(client, htable, cmd); break;
        case HDEL: exec_hdel(client, htable, cmd); break;
        case LPUSH: exec_push(client, htable, cmd, LEFT); break;
        case LPOP: exec_pop(client, htable, cmd, LEFT); break;
        case RPUSH: exec_push(client, htable, cmd, RIGHT); break;
        case RPOP: exec_pop(client, htable, cmd, RIGHT); break;
        case SADD: exec_sadd(client, htable, cmd); break;
        case SREM: exec_srem(client, htable, cmd); break;
        case SISMEMBER: exec_sismember(client, htable, cmd); break;
        case UNKNOWN: exec_unknown(client); break;
        case QUIT: exit(0); break;
        case NOOP: break;
    }
    free_all(2, cmd->argv, cmd);
}


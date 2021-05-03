#include "common.h"

char *exec_del()

char *interpret(Command *cmd, HashTable *htable) {
    switch (cmd->type) {
        case DEL: exec_del(client, htable, cmd); break;
        // case EXISTS: exec_exists(client, htable, cmd); break;
        // case TYPE: exec_type(client, htable, cmd); break;
        // case SET: exec_set(client, htable, cmd); break;
        // case GET: exec_get(client, htable, cmd); break;
        // case MSET: exec_mset(client, htable, cmd); break;
        // case MGET: exec_mget(client, htable, cmd); break;
        // case INCR: exec_incr(client, htable, cmd); break;
        // case DECR: exec_decr(client, htable, cmd); break;
        // case INCRBY: exec_incrby(client, htable, cmd); break;
        // case DECRBY: exec_decrby(client, htable, cmd); break;
        // case STRLEN: exec_strlen(client, htable, cmd); break;
        // case HSET: exec_hset(client, htable, cmd); break;
        // case HGET: exec_hget(client, htable, cmd); break;
        // case HDEL: exec_hdel(client, htable, cmd); break;
        // case HGETALL: exec_hgetall(client, htable, cmd); break;
        // case HEXISTS: exec_hexists(client, htable, cmd); break;
        // case HKEYS: exec_hkeys(client, htable, cmd); break;
        // case HVALS: exec_hvals(client, htable, cmd); break;
        // case HMGET: exec_hmget(client, htable, cmd); break;
        // case HLEN: exec_hlen(client, htable, cmd); break;
        // case LPUSH: exec_push(client, htable, cmd, LEFT); break;
        // case LPOP: exec_pop(client, htable, cmd, LEFT); break;
        // case RPUSH: exec_push(client, htable, cmd, RIGHT); break;
        // case RPOP: exec_pop(client, htable, cmd, RIGHT); break;
        // case LLEN: exec_llen(client, htable, cmd); break;
        // case LINDEX: exec_lindex(client, htable, cmd); break;
        // case LRANGE: exec_lrange(client, htable, cmd); break;
        // case LSET: exec_lset(client, htable, cmd); break;
        // case SADD: exec_sadd(client, htable, cmd); break;
        // case SREM: exec_srem(client, htable, cmd); break;
        // case SISMEMBER: exec_sismember(client, htable, cmd); break;
        // case SMEMBERS: exec_smembers(client, htable, cmd); break;
        // case SMISMEMBER: exec_smismember(client, htable, cmd); break;
        // case UNKNOWN: exec_unknown(client); break;
        // case QUIT: exit(0); break;
        // case NOOP: break;
    }
    command_free(cmd);
}


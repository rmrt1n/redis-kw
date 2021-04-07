#ifndef HTABLE_H
#define HTABLE_H

#include "common.h"

HashTable *htable_init(int size);
HtableAction htable_set(HashTable *htable, char *key, char *value);
HtableAction htable_get(HashTable *htable, char *key);
HtableAction htable_del(HashTable *htable, char *key);
HtableAction htable_hset(HashTable *htable, char *key, char *field, char *value);
HtableAction htable_hget(HashTable *htable, char *key, char *field);

#endif

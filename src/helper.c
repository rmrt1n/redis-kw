#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "common.h"

static int is_prime(int x) {
    if (x < 2) return 0;
    if (x == 2) return 1;
    if (x % 2 == 0) return 0;
    for (int i = 3; i <= floor(sqrt((double) x)); i += 2) {
        if (x % i == 0) return 0;
    }
    return 1;
}

int next_prime(int x) {
    if (x < 2) return 2;
    int y = x;
    while (!is_prime(y)) y++;
    return y;
}

static int djb2(char *str, int size) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash % size;
}

static int sdbm(char *str, int size) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++)) hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % size;
}

int hash_func(char *key, int size, int i) {
    int hash = djb2(key, size);
    int res = i == 0 ? hash : hash + (i * (sdbm(key, size)));
    return res % size;
}

int ndigits(int x) {
    int n = x < 0 ? x * -1 : x;
    int res = 0;
    while (n > 0) {
        n /= 10;
        res++;
    }
    return res;
}

bool is_number(char *str) {
    int n = strlen(str);
    int i = *str == '-' ? 1 : 0;
    for (; i < n; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

int strtoi(char *str) {
    int res = 0, i = *str == '-' ? 1 : 0;
    int neg = i, n = strlen(str);
    for (; i < n; i++) {
        res = res * 10 + (str[i] - '0');
    }
    if (neg) res *= -1;
    return res;
}

char *intostr(int x) {
    char *res = malloc(ndigits(x) + 1);
    sprintf(res, "%d", x);
    return res;
}


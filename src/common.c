#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include "common.h"

void free_all(int n, ...) {
    va_list args;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        void *vp = va_arg(args, void *);
        free(vp);
        vp = NULL;
    }
    va_end(args);
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

int isnumber(char *str) {
    int i = *str == '-' ? 1 : 0;
    int n = strlen(str);
    for (; i < n; i++) {
        if (isdigit(str[i]) == 0) return 0;
    }
    return 1;
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

char * intostr(int n) {
    char *res = malloc(ndigits(n) + 2);
    sprintf(res, "%d", n);
    return res;
}

int isprime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= floor(sqrt((double) n)); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int next_prime(int n) {
    if (n < 2) return 2;
    int x = n;
    while (!isprime(x)) x++;
    return x;
}


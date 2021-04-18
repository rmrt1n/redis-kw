#include <stdlib.h>
#include <stdarg.h>
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


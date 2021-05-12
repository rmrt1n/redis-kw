#include <stdio.h>
#include <time.h>
#include "test.h"
#define MINIUNIT_MAIN
#include "miniunit.h"
#define B  1000000000.0

int main() {
    double dur;
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    test_htable();
    test_parser();
    test_interpret();
    clock_gettime(CLOCK_REALTIME, &end);
    dur = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / B;
    printf("test duration = %lf\n", dur);
    return 0;
}


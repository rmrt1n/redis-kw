#include <stdio.h>
#include <string.h>
#include "../src/common.h"

// minunit
#define mu_assert(message, test) do { if (!(test)) return message;  } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                               if (message) return message; } while (0)

int tests_run = 0;

char *test_resizing() {
    HashTable *ht = htable_init(HTABLE_BASE_SIZE);
    mu_assert("ht not initialized to size 2", ht->size == 2);

    // add until load > 0.7
    htable_set(ht, "a", "1");
    mu_assert("ht size not 2 and ht used not 1",
              ht->size == 2 && ht->used == 1);
    htable_hset(ht, "b", "1", "2");
    mu_assert("ht size not 2 and ht used not 2",
              ht->size == 2 && ht->used == 2);
    // resize to 5
    htable_push(ht, "c", "1", LEFT);
    // printf("ht size = %d and ht used = %d", ht->size, ht->used);
    mu_assert("ht size not 5 and ht used not 3",
              ht->size == 5 && ht->used == 3);

    htable_free(ht);
    return 0;
}

char *test_all() {
    mu_run_test(test_resizing);
    return 0;
}

int main() {
    char *res = test_all();
    if (res != 0) {
        puts(res);
    } else {
        puts("all tests passed");
    }
    printf("tests run: %d\n", tests_run);
    return res != 0;
}


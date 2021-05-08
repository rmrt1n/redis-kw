#include <string.h>
#include "../src/common.h"
#include "test.h"
#include "miniunit.h"

static bool check_cmd(Command *cmd, int type, char *argv[]) {
    for (int i = 0; i < cmd->argc; i++) {
        // if (cmd->argv[i] != NULL) puts(cmd->argv[i]);
        if (strcmp(cmd->argv[i], argv[i]) != 0) return false;
    }
    return true;
}

void test_parser() {
    test_case("test parser", {
        expect("parse: del", check_cmd(parse("del"), DEL, NULL));
        expect("parse: set 1", check_cmd(parse("set 1"), SET, (char *[]){"1"}));
        expect("parse: lpush 1 2 3", check_cmd(parse("lpush 1 2 3"), LPUSH,
                                               (char *[]){"1", "2", "3"}));
        expect("parse: sadd 1 2 3 4", check_cmd(parse("sadd 1 2 3 4"), SADD,
                                                (char *[]){"1", "2", "3", "4"}));
        expect("parse: ''", check_cmd(parse(""), NOOP, NULL));
    });
    return;
}

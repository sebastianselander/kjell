#ifndef BUILTINS_H
#define BUILTINS_H

#include "utils.h"

ExitInfo cbsh_cd(char **args);
ExitInfo cbsh_help(char **args);
ExitInfo cbsh_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    ":q",
};

ExitInfo (*builtin_func[]) (char**) = {
    &cbsh_cd,
    &cbsh_help,
    &cbsh_exit,
    &cbsh_exit,
};

int cbsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

#endif

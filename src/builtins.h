#ifndef BUILTINS_H
#define BUILTINS_H

#include "utils.h"
#include "gram/Absyn.h"

void cbsh_cd(Shell *shell, ListIdentifier args);
void cbsh_help(Shell *shell, ListIdentifier args);
void cbsh_exit(Shell *shell, ListIdentifier args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    ":q",
};

void (*builtin_func[]) (Shell*, ListIdentifier) = {
    &cbsh_cd,
    &cbsh_help,
    &cbsh_exit,
    &cbsh_exit,
};

int cbsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

#endif

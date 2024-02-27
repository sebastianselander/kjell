#ifndef BUILTINS_H
#define BUILTINS_H

#include "utils.h"
#include "gram/Absyn.h"

void kjell_cd(Shell *shell, ListIdentifierLen args);
void kjell_help(Shell *shell, ListIdentifierLen args);
void kjell_exit(Shell *shell, ListIdentifierLen args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    ":q",
};

void (*builtin_func[]) (Shell*, ListIdentifierLen) = {
    &kjell_cd,
    &kjell_help,
    &kjell_exit,
    &kjell_exit,
};

int kjell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

#endif

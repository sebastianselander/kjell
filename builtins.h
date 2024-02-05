#ifndef BUILTINS_H
#define BUILTINS_H

int cbsh_cd(char **args);
int cbsh_help(char **args);
int cbsh_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
};

int (*builtin_func[]) (char**) = {
    &cbsh_cd,
    &cbsh_help,
    &cbsh_exit,
};

int cbsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

#endif

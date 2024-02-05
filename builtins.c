#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define HOME_PATH "/home/sebastian"
int cbsh_cd(char** args) {
    char *arg;
    if (args[1] == NULL) {
        arg = HOME_PATH;
    } else {
        arg = args[1];
        if (chdir(arg) != 0) {
            perror("cd");
        }
    }
    return 1;
}

int cbsh_exit(char **args) {
    return 0;
}

int cbsh_help(char **args) {
    printf("help!\n");
    return 1;
}

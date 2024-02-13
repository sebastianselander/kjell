#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HOME_PATH "/home/sebastian"
ExitInfo cbsh_cd(char **args) {
    ExitInfo exit_info = exit_info_init();
    char *arg;
    if (args[1] == NULL) {
        arg = HOME_PATH;
    } else {
        arg = args[1];
    }
    if (chdir(arg) != 0) {
        perror("cd");
        exit_info.exit_code = 1;
    }
    return exit_info;
}

ExitInfo cbsh_exit(char **args) {
    ExitInfo exit_info = exit_info_init();
    exit_info.terminate = true;
    return exit_info;
}

ExitInfo cbsh_help(char **args) {
    ExitInfo exit_info = exit_info_init();
    printf("help!\n");
    return exit_info;
}

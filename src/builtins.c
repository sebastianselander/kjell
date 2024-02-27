#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gram/Absyn.h"

#define HOME_PATH "/home/sebastian"
void cbsh_cd(Shell *shell, ListIdentifier args) {
    ExitInfo exit_info = exit_info_init();
    char *arg;
    if (args[0].identifier_ == NULL) {
        arg = HOME_PATH;
    } else {
        arg = args[0].identifier_;
    }
    if (chdir(arg) != 0) {
        perror("cd");
        exit_info.exit_code = 1;
    }
}

void cbsh_exit(Shell *shell, ListIdentifier args) {
    ExitInfo exit_info = exit_info_init();
    exit_info.terminate = true;
}

void cbsh_help(Shell *shell, ListIdentifier args) {
    ExitInfo exit_info = exit_info_init();
    printf("help!\n");
}

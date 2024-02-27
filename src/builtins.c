#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gram/Absyn.h"

#define HOME_PATH "/home/sebastian"
void kjell_cd(Shell *shell, ListIdentifierLen lil) {
    ExitInfo exit_info = exit_info_init();
    if (lil.list_len > 1) {
        shell->exit_code = 1;
        printf("cd: too many arguments\n");
        return;
    }
    char *path = HOME_PATH;
    if (lil.list_len > 0) {
        char *first_arg = lil.list->identifier_;
        if (strcmp("-", first_arg) == 0) {
            printf("Previous: %s\n", shell->previous_path);
            path = shell->previous_path;
        } else {
            path = first_arg;
        }
    }
    if (chdir(path) != 0) {
        perror("cd");
        exit_info.exit_code = 1;
    }
    shell->previous_path = shell->current_path;
    shell->current_path = getcwd(NULL, 0);
}

void kjell_exit(Shell *shell, ListIdentifierLen args) {
    shell->exit = true;
}

void kjell_help(Shell *shell, ListIdentifierLen args) {
    ExitInfo exit_info = exit_info_init();
    printf("help!\n");
}

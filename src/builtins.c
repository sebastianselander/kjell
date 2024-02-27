#include "gram/Absyn.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void kjell_cd(Shell *shell, ListIdentifierLen lil) {
    ExitInfo exit_info = exit_info_init();
    if (lil.list_len > 1) {
        shell->exit_code = 1;
        printf("cd: too many arguments\n");
        return;
    }
    char *path = getenv("HOME");
    if (lil.list_len > 0) {
        char *first_arg = lil.list->identifier_;
        if (strcmp("-", first_arg) == 0) {
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

void kjell_exec(Shell *shell, ListIdentifierLen args) {
    if (args.list_len == 0) {
        return;
    }
    char **args_list = malloc(sizeof(char*) * args.list_len);
    for (int i = 0; i < args.list_len; i++) {
        args_list[i] = args.list->identifier_;
        args.list = args.list->listidentifier_;
    }
    if (execvp(args_list[0], args_list) == -1) {
        perror(args_list[0]);
    }
    exit(EXIT_SUCCESS);
}

void kjell_exit(Shell *shell, ListIdentifierLen args) {
    shell->exit = true;
}

void kjell_help(Shell *shell, ListIdentifierLen args) {
    printf("help!\n");
}

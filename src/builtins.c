#include "gram/Absyn.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void kjell_cd(Shell *shell, ListIdentifierLen lil) {
    if (lil.list_len > 1) {
        shell->exit_code = EXIT_FAILURE;
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
    int status = chdir(path);
    if (status != 0) {
        perror("cd");
        shell->exit_code = EXIT_FAILURE;
    } else {
        shell->previous_path = shell->current_path;
        shell->current_path = getcwd(NULL, 0);
        shell->exit_code = EXIT_SUCCESS;
    }
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

void kjell_echo(Shell *shell, ListIdentifierLen args) {
    char** args_list = make_chrarray("echo", args); 
    for (int i = 1; i < args.list_len + 1; i++) {
        if (args_list[i][0] == '$') {
            char* envvar = &args_list[i][1]; 
            printf("%s", getenv(envvar));
        } else {
            printf("%s", args_list[i]);
        }
        printf(" ");
    }
    printf("\n");
}

void kjell_exit(Shell *shell, ListIdentifierLen args) {
    shell->exit = true;
}

void kjell_help(Shell *shell, ListIdentifierLen args) {
    printf("help!\n");
}

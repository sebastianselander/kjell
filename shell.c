#include "shell.h"
#include <string.h>

#define bool char

char *BUILT_INS = {"cd"};

bool is_cd(char *command) {
    return !strcmp(&BUILT_INS[0], command);
}

void pwd() {
    char *cwd = getcwd(NULL, 0);
    printf("Current path: %s\n", cwd);
    free(cwd);
}

void run_cd(char *command, char *arg_path, char **prev_path) {
    int err = chdir(arg_path);
    if (err != 0) {
        perror("");
        exit(1);
    } else {
        *prev_path = getcwd(NULL, 0);
    }
}

char **tokenize(char *string, int *i) {
    int j = *i;
    char *token = strtok(string, " \n");
    char **array = malloc(sizeof(char) * 1024);
    while (token) {
        array[j] = token;
        token = strtok(NULL, " \n");
        j++;
    }
    array[j] = NULL;
    i = &j;
    return array;
}

int shell() {
    char *buf = NULL, *token;
    size_t count = 0;
    ssize_t nread;
    pid_t pid;
    int status, i = 0;
    char **array;
    char *cwd = getcwd(NULL, 0);
    char **prev_path = &cwd;

    while (1) {
        pwd();
        fflush(stdout);
        write(STDOUT_FILENO, "CShell > ", 9);
        fflush(stdout);
        nread = getline(&buf, &count, stdin);
        if (nread == -1) {
            perror("Exiting shell");
            exit(EXIT_FAILURE);
        }
        array = tokenize(buf, &i);
        /* token = strtok(buf, " \n"); */
        /* array = malloc(sizeof(char*) * 1024); */
        while (token) {
            array[i] = token;
            token = strtok(NULL, " \n");
            i++;
        }
        array[i] = NULL;
        char *command = array[0];
        pid = fork();
        if (pid == 0) {
            if (is_cd(command)) {
                if (i == 2) {
                    char *argument = array[1];
                    if (strcmp(argument, "~") == 0) {
                        run_cd(command, "/home/sebastian", prev_path);
                    } else if (strcmp(argument, "-") == 0) {
                        run_cd(command, *prev_path, prev_path);
                    } else {
                        run_cd(command, array[1], prev_path);
                    }
                } else {
                    run_cd(command, "/home/sebastian", prev_path);
                }
            } else {
                if (execve(command, array, NULL) == -1) {
                    perror("");
                    exit(127);
                }
            }
        }
        wait(&status);
        buf = NULL;
    }
    free(buf);
    free(prev_path);
    free(cwd);
    return 0;
}

int main(int argc, char *argv[]) {
    shell();
}

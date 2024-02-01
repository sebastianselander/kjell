#include "shell.h"
#include <string.h>

#define bool char
#define HOME "/home/sebastian"

char *BUILT_INS = {"cd"};

bool is_cd(char *command) {
    return !strcmp(&BUILT_INS[0], command);
}

void pwd() {
    char *cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
    fflush(stdout);
    free(cwd);
}

void run_cd(char *command, char *arg_path, char **prev_path) {
    *prev_path = getcwd(NULL, 0);
    int err = chdir(arg_path);
    if (err != 0) {
        perror("");
        exit(1);
    }
}

char **tokenize(char *string, int *size) {
    char *token = strtok(string, " \n");
    char **array = malloc(sizeof(char) * 1024);
    while (token) {
        array[*size] = token;
        token = strtok(NULL, " \n");
        *size = *size + 1;
    }
    array[*size] = NULL;
    return array;
}

void printArray(char **array, int size) {
    printf("{ ");
    for (int i = 0; i < size; i++) {
        printf("%s ", array[i]);
    }
    printf("} \n");
}

// returns 0 on success, -1 if command was not cd
int execcd() {

}

int shell() {
    char *buf = NULL, *token;
    size_t count = 0;
    ssize_t nread;
    pid_t pid;
    int status;
    char *cwd = getcwd(NULL, 0);
    char **prev_path = &cwd;

    while (1) {
        pwd();
        write(STDOUT_FILENO, "CShell > ", 9);
        nread = getline(&buf, &count, stdin);
        if (nread == -1) {
            perror("Exiting shell");
            exit(EXIT_FAILURE);
        }
        int i = 0;
        char **array = tokenize(buf, &i);
        printArray(array, i);
        char *command = array[0];
        pid = fork();
        if (pid != -1) {
            if (pid != 0) {
                printf("pid [%d]\n", pid);
            } else {
                if (is_cd(command)) {
                    if (i == 2) {
                        char *argument = array[1];
                        if (strcmp(argument, "~") == 0) {
                            run_cd(command, HOME, prev_path);
                        } else if (strcmp(argument, "-") == 0) {
                            run_cd(command, *prev_path, prev_path);
                        } else {
                            run_cd(command, array[1], prev_path);
                        }
                    } else if (i == 1) {
                        run_cd(command, HOME, prev_path);
                    } else {
                        fprintf(stderr, "one argument required\n");
                    }
                } else {
                    array[0] = NULL; // Should be filename
                    if (execve(command, array, NULL) == -1) {
                        perror("");
                        exit(127);
                    }
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

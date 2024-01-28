#include "shell.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[]) {
    char *buf = NULL, *token;
    size_t count = 0;
    ssize_t nread;
    pid_t child_pid;
    int status, i;
    char **array;

    while (TRUE) {
        write(STDOUT_FILENO, "CShell$ ", 9);
        nread = getline(&buf, &count, stdin);
        if (nread == -1) {
            perror("Exiting shell");
            exit(1);
        }
        token = strtok(buf, " \n");
        array = malloc(sizeof(char*) * 1024);
        i = 0;
        while (token) {
            array[i] = token;
            token = strtok(NULL, " \n");
            i++;
        }
        array[i] = NULL;
        child_pid = fork();
        switch (child_pid) {
            case -1:
                perror("Failed to create child process\n");
                exit(41);
                break;
            case 0:
                if (execve(array[0], array, NULL) == -1) {
                    perror("Failed to execute");
                    exit(97);
                }
                break;
            default:
                wait(&status);
                break;
        }
        printf("%s", buf);
    }
    free(buf);
    return 0;
}

#include "shell.h"

#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[]) {
    char *buf = NULL;
    size_t count = 0;
    ssize_t nread;
    pid_t child_pid;
    int status;

    while (TRUE) {
        write(STDOUT_FILENO, "CShell$ ", 9);
        nread = getline(&buf, &count, stdin);
        if (nread == -1) {
            perror("Exiting shell");
            exit(1);
        }
        child_pid = fork();
        switch (child_pid) {
            case -1:
                perror("Failed to create child process");
                exit(41);
                break;
            case 0:
                printf("Successfully created child");
                break;
            default:
                wait(&status);
                break;
        }
        printf("buf = %s", buf);
    }
    free(buf);
    return 0;
}

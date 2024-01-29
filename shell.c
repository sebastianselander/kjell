#include "shell.h"
#include <string.h>

#define TRUE 1
#define FALSE 0

size_t len(char *string) {
    size_t i = 0;
    while(string[i]) {
        i++;
    }
    return i;
}

char *concat(char *first, char *second) {
    size_t length = len(first) + len(second);
    char* new = (char*) malloc(sizeof(char) * length + 1);
    int i = 0;
    while (first[i]) {
        new[i] = first[i];
        i++;
    }
    int j = 0;
    while(second[j]) {
        new[i] = second[j];
        i++;
        j++;
    }
    new[i] = 0;
    return new;
}

int shell() {
    char *buf = NULL, *token;
    size_t count = 0;
    ssize_t nread;
    pid_t child_pid;
    int status, i;
    char **array;

    while (TRUE) {
        write(STDOUT_FILENO, "CShell > ", 7);
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
                    perror(array[0]);
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

int main(int argc, char *argv[]) {
    char *left;
    char *right;
    right = "bello";
    size_t count;
    int nread = getline(&left, &count, stdin);
    left[len(left) - 1] = 0;
    char *test = concat(left, right);
    printf("%s\n", test);
    free(left);
    printf("Freed left");
    free(right);
    printf("Freed right");
}

#include "shell.h"
#include <string.h>
#include "builtins.h"

char* read_line() {
    char *line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("read line");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

#define TOK_DELIM " \r\t\n\a"
#define TOK_BUFSIZE 64
char** split_line(char* line) {
    int bufsize = TOK_BUFSIZE;
    int position = 0;

    char **tokens = malloc(sizeof(char*) * bufsize);
    char *token;

    if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void prompt() {
    char *cwd = getcwd(NULL, 0);
    printf("%s\n> ", cwd);
    free(cwd);
}

int shell_launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");
    } else {
        pid_t wpid = wait(&status);
        printf("status: %d", status);
    }

    return 1;
}

int shell_execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < cbsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return shell_launch(args);
}

void shell_loop() {
    while (1) {
        prompt();
        char *line = read_line();
        char **args = split_line(line);
        int status = shell_execute(args);

        free(line);
        free(args);

        if (!status) {
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    shell_loop();
}

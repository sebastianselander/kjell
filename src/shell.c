#include "shell.h"
#include "builtins.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include <string.h>

typedef struct ExitInfo ExitInfo;

char *read_line() {
    char *line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
        } else {
            perror("read line");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

void prompt(int exit_code) {
    char *cwd = getcwd(NULL, 0);
    char *green = "\033[32m";
    char *red = "\033[31m";
    char *close = "\033[0m";
    if (exit_code) {
        printf("%s\n%s$%s ", cwd, red, close);
    } else {
        printf("%s\n%s$ %s", cwd, green, close);
    }
    free(cwd);
}

ExitInfo shell_launch(char **args, int exit_code) {
    pid_t pid;
    int exit_c = exit_code;
    bool negate = false;

    if (strcmp(args[0], "!") == 0) {
        negate = true;
        // shift all elements down one step, removing the bang
        for (int i = 1;; i++) {
            args[i - 1] = args[i];
            if (args[i] == NULL) {
                break;
            }
        }
    }

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");
    } else {
        pid_t wpid = wait(&exit_c);
        normalize_status(&exit_c);
        if (negate) {
            exit_c = !exit_c;
        }
    }

    ExitInfo exit_info = exit_info_init();
    exit_info.exit_code = exit_c;
    return exit_info;
}

ExitInfo shell_execute(char **args, int exit_code) {
    if (args[0] == NULL) {
        ExitInfo exit_info = {0, false};
        return exit_info;
    }

    for (int i = 0; i < cbsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return shell_launch(args, exit_code);
}

void shell_loop() {
    ExitInfo exit_info = exit_info_init();

    while (1) {
        prompt(exit_info.exit_code);
        const char *line = read_line();
        size_t line_len = strlen(line) - 1; //drop the newline character
        String input = { .text = line, .text_len = line_len };

        Parsed p = parse(input);
        AST* expression = p.expression;
        if (!expression) {
            printf("ERROR: %s\n", p.error_msg);
            ast_free(p.expression);
            continue;
        }
        ast_print(p.expression);
        printf("\n");
        ast_free(expression);
    }
}

int main() {
    shell_loop();
}

#include "shell.h"
#include "builtins.h"
#include "gram/Absyn.h"
#include "gram/Parser.h"
#include "gram/Printer.h"
#include "utils.h"
#include <assert.h>
#include <string.h>

typedef struct ExitInfo ExitInfo;

char *read_line() {
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

/* ExitInfo shell_launch(Shell *shell, Identifier command, ListIdentifier args) { */
/*     pid_t pid = fork(); */
/**/
/*     if (pid == 0) { */
/*         if (execvp(args[0], args) == -1) { */
/*             perror("shell"); */
/*         } */
/*         exit(EXIT_FAILURE); */
/*     } else if (pid < 0) { */
/*         perror("shell"); */
/*     } else { */
/*         pid_t wpid = wait(&exit_c); */
/*         normalize_status(&exit_c); */
/*         if (negate) { */
/*             exit_c = !exit_c; */
/*         } */
/*     } */
/**/
/*     ExitInfo exit_info = exit_info_init(); */
/*     exit_info.exit_code = exit_c; */
/*     return exit_info; */
/* } */

void interpret_command(Shell *shell, Identifier command, ListIdentifier args) {
    if (strlen(command) == 0) {
        assert(args == NULL);
        shell->exit_code = 0;
        return;
    }
    for (int i = 0; i < cbsh_num_builtins(); i++) {
        if (strcmp(command, builtin_str[i]) == 0) {
            (*builtin_func[i])(shell, args);
            return;
        }
    }

}

ListIdentifier interpret_args(Shell *shell, Args args) {
    printf("Interpret Args\n");
    switch (args->kind) {
    case is_ArgsList: {
        ListIdentifier li = args->u.argslist_.listidentifier_;
        for (int i = 0; li[i].identifier_ != 0; ++i) {
            printf("%s\n", li[i].identifier_);
        }
        return li;
    }
    case is_ArgsCommand: {
        return NULL;
    }
    }
}

void interpret_subshell(Shell *shell, Subshell subshell) {
    printf("Interpret Subshell\n");
    switch (subshell->kind) {
    case is_Subsh: {
    } break;
    case is_Command: {
        Identifier command = subshell->u.command_.identifier_;
        printf("\nCommand is: %s\n", command);
        Args args = subshell->u.command_.args_;
        ListIdentifier list_identifier = interpret_args(shell, args);
        interpret_command(shell, command, list_identifier);
    } break;
    }
}

void interpret_bang(Shell *shell, Bang bang) {
    printf("Interpret Bang\n");
    switch (bang->kind) {
    case is_Bng: {
    } break;
    case is_NextBang: {
        Subshell subshell = bang->u.nextbang_.subshell_;
        interpret_subshell(shell, subshell);
    } break;
    }
}

void interpret_expression(Expression expression, Shell *shell) {
    printf("Interpret Expression\n");
    switch (expression->kind) {
    case is_Sequential: {
        Bang left = expression->u.sequential_.bang_;
        Expression right = expression->u.sequential_.expression_;
        interpret_bang(shell, left);
    } break;
    case is_Or:
        break;
    case is_And:
        break;
    case is_NextExpr: {
        Bang bang = expression->u.nextexpr_.bang_;
        interpret_bang(shell, bang);
    } break;
    case is_Empty:
        break;
    }
}

void shell_loop() {
    ExitInfo exit_info = exit_info_init();
    while (1) {
        prompt(exit_info.exit_code);
        char *line = read_line();
        size_t line_len = strlen(line);
        line[line_len - 1] = 0;
        Expression expr = psExpression(line);
        Shell shell = {0};
        if (!expr) {
            printf("Failed parsing\n");
        } else {
            printf("Success parsing\n");
            interpret_expression(expr, &shell);
            free_Expression(expr);
        }
    }
}

int main(int argc, char *argv[]) { shell_loop(); }

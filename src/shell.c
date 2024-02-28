#include "shell.h"
#include "builtins.h"
#include "gram/Absyn.h"
#include "gram/Parser.h"
#include "gram/Printer.h"
#include "utils.h"
#include <assert.h>
#include <string.h>

typedef struct ExitInfo ExitInfo;

size_t argslist_len(ListIdentifier list) {
    int i = 0;
    while (list != NULL) {
        i++;
        list = list->listidentifier_;
    }
    return i;
}


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

void prompt(Shell *shell) {
    char *green = "\033[32m";
    char *red = "\033[31m";
    char *close = "\033[0m";
    if (shell->exit_code == EXIT_FAILURE) {
        printf("%s$%s ", red, close);
    } else {
        printf("%s$%s ", green, close);
    }
}

bool shell_execute_builtin(Shell *shell, Identifier command,
                           ListIdentifierLen args) {
    for (int i = 0; i < kjell_num_builtins(); i++) {
        if (strcmp(command, builtin_str[i]) == 0) {
            (*builtin_func[i])(shell, args);
            return true;
        }
    }
    return false;
}

#define BUFSIZE_INIT 1024
#define PATH "PATH="
void shell_execute_external(Shell *shell, Identifier command,
                            ListIdentifierLen args) {
    pid_t pid = fork();
    if (pid == 0) {
        char **args_list = make_chrarray(command, args);
        if (execvp(args_list[0], args_list) == -1) {
            perror(command);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("pid");
    } else {
        int status;
        pid_t wpid = wait(&status);
        shell->exit_code = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    }
}

// Precondition: command is not be empty
void interpret_command(Shell *shell, Identifier command,
                       ListIdentifierLen args) {
    if (!shell_execute_builtin(shell, command, args)) {
        shell_execute_external(shell, command, args);
    }
}

ListIdentifierLen interpret_args(Shell *shell, Args args) {
    switch (args->kind) {
    case is_ArgsList: {
        ListIdentifier li = args->u.argslist_.listidentifier_;
        if (li == NULL) {
            ListIdentifierLen lil = {0};
            lil.list_len = 0;
            lil.list = NULL;
            return lil;
        } else {
            ListIdentifierLen lil = {0};
            lil.list_len = argslist_len(li);
            lil.list = li;
            return lil;
        }
    }
    case is_ArgsCommand: {
        ListIdentifierLen lil = {0};
        return lil;
    }
    }
}

void interpret_subshell(Shell *shell, Subshell subshell) {
    switch (subshell->kind) {
    case is_Subsh: {
    } break;
    case is_Command: {
        Identifier command = subshell->u.command_.identifier_;
        Args args = subshell->u.command_.args_;
        ListIdentifierLen list_identifier_len = interpret_args(shell, args);
        interpret_command(shell, command, list_identifier_len);
    } break;
    }
}

void interpret_bang(Shell *shell, Bang bang) {
    switch (bang->kind) {
    case is_Bng: {
        Subshell subshell = bang->u.bng_.subshell_;
        interpret_subshell(shell, subshell);
        shell->exit_code = shell->exit_code ? EXIT_SUCCESS : EXIT_FAILURE;
    } break;
    case is_NextBang: {
        Subshell subshell = bang->u.nextbang_.subshell_;
        interpret_subshell(shell, subshell);
    } break;
    }
}

void interpret_expression(Shell *shell, Expression expression) {
    switch (expression->kind) {
    case is_Sequential: {
        Bang left = expression->u.sequential_.bang_;
        Expression right = expression->u.sequential_.expression_;
        interpret_bang(shell, left);
        interpret_expression(shell, right);
    } break;
    case is_Or: {
        Bang left = expression->u.or_.bang_;
        Expression right = expression->u.sequential_.expression_;
        interpret_bang(shell, left);
        if (shell->exit_code != EXIT_FAILURE) {
            interpret_expression(shell, right);
        }
    } break;
    case is_And: {
        Bang left = expression->u.or_.bang_;
        Expression right = expression->u.sequential_.expression_;
        interpret_bang(shell, left);
        if (shell->exit_code == EXIT_SUCCESS) {
            interpret_expression(shell, right);
        }
    } break;
    case is_NextExpr: {
        Bang bang = expression->u.nextexpr_.bang_;
        interpret_bang(shell, bang);
    } break;
    case is_Empty:
        return;
    }
}

Shell shell_init() {
    Shell shell = {0};
    shell.exit_code = 0;
    shell.current_path = getcwd(NULL, 0);
    shell.previous_path = getcwd(NULL, 0);
    return shell;
}

int main(int argc, char *argv[]) {
    Shell shell = shell_init(argv[0]);
    while (!shell.exit) {
        prompt(&shell);
        char *line = read_line();
        size_t line_len = strlen(line);
        line[line_len - 1] = 0; // remove newline
        Expression expr = psExpression(line);
        if (!expr) {
            shell.exit_code = 1;
        } else {
            interpret_expression(&shell, expr);
            free_Expression(expr);
        }
        free(line);
    }
}

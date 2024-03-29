#include "shell.h"
#include "builtins.h"
#include "gram/Absyn.h"
#include "gram/Parser.h"
#include "gram/Printer.h"
#include "utils.h"
#include <assert.h>

#define BUFSIZE_INIT 1024

typedef struct ExitInfo ExitInfo;

void interpret_bang(Shell *shell, Bang bang);
void interpret_subshell(Shell *shell, Subshell subshell);
void interpret_command(Shell *shell, Identifier command,
                       ListIdentifierLen args);
ListIdentifierLen interpret_args(Shell *shell, Args args);
void interpret_expression(Shell *shell, Expression expression);

void restore_cee(Shell *shell) {
    /*
     * Currently only current working directory is relevant
     * TODO: implement more as more stuff is added
     * Command execution environement:
     * https://www.gnu.org/software/bash/manual/html_node/Command-Execution-Environment.html
     */
    if (chdir(shell->current_path) != 0) {
        perror("restore");
        exit(EXIT_FAILURE);
    }
}

size_t argslist_len(ListIdentifier list) {
    int i = 0;
    while (list != NULL) {
        i++;
        list = list->listidentifier_;
    }
    return i;
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
        if (wpid == -1) {
            perror("Child process failed");
        }
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
        Expression expr = subshell->u.subsh_.expression_;
        pid_t pid = fork();
        if (pid == 0) {
            Shell subsh = *shell;
            interpret_expression(&subsh, expr);
            restore_cee(shell);
        } else if (pid < 0) {
            perror("pid");
        } else {
            int status;
            pid_t wpid = wait(&status);
            if (wpid == -1) {
                perror("Child process failed");
            }
            shell->exit_code = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
        }
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
        shell->exit_code = !shell->exit_code;
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
    case is_Pipe: {
        Bang left = expression->u.pipe_.bang_;
        Expression right = expression->u.pipe_.expression_;
        // TODO: some stuff here
        shell->in_pipe = true;
        interpret_bang(shell, left);
        interpret_expression(shell, right);
        shell->in_pipe = false;
    } break;
    case is_Or: {
        Bang left = expression->u.or_.bang_;
        Expression right = expression->u.sequential_.expression_;
        interpret_bang(shell, left);
        if (shell->exit_code == EXIT_FAILURE) {
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
    shell.in_pipe = false;
    shell.exit_code = 0;
    shell.current_path = getcwd(NULL, 0);
    shell.previous_path = getcwd(NULL, 0);
    return shell;
}

char *kjell_getline() {
    char *line = NULL;
    size_t bufsize;
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
        } else {
            perror("read line");
            exit(EXIT_FAILURE);
        }
    }
    size_t line_len = strlen(line);
    line[line_len - 1] = 0; // remove newline
    return line;
}

char *kjell_read() {
    char *result = malloc(sizeof(char *) * BUFSIZE_INIT);
    result[0] = 0;
    while (1) {
        char *line = kjell_getline();
        size_t line_len = strlen(line);
        if (line_len > 0) {
            char last_chr = line[line_len - 1];
            if (str_ends_in(line, "\\")) {
                line[line_len - 1] = 0;
                result = strcat(result, line);
                printf("> ");
                free(line);
                continue;
            }
            if (str_ends_in(line, "&&") || str_ends_in(line, "|")) {
                result = strcat(result, line);
                printf("> ");
                free(line);
                continue;
            }
            result = strcat(result, line);
        }
        return result;
    }
}

void kjell(int argc, char *argv[]) {
    Shell shell = shell_init();
    if (argc == 1) {
        while (!shell.exit) {
            prompt(&shell);
            char *line = kjell_read();
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
    if (argc == 2) {
        char *line = argv[1];
        size_t line_len = strlen(line);
        Expression expr = psExpression(line);
        if (!expr) {
            shell.exit_code = 1;
        } else {
            interpret_expression(&shell, expr);
            free_Expression(expr);
        }
    }
    if (argc > 2) {
        printf("Too many arguments: %d\n", argc);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    kjell(argc, argv);
}

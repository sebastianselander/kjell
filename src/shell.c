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

char **make_argslist_execvp(Identifier command, ListIdentifierLen list) {
    ListIdentifier l = list.list;
    char **args = malloc(sizeof(char *) * list.list_len + 1);
    args[0] = command;
    for (int i = 0; i < list.list_len; i++) {
        args[i+1] = l->identifier_;
        l = l->listidentifier_;
    }
    return args;
}

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

void prompt(Shell *shell) {
    char *cwd = shell->current_path;
    char *green = "\033[32m";
    char *red = "\033[31m";
    char *close = "\033[0m";
    if (shell->exit_code) {
        printf("%s\n%s$%s ", cwd, red, close);
    } else {
        printf("%s\n%s$ %s", cwd, green, close);
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
void shell_execute_external(Shell *shell, Identifier command,
                            ListIdentifierLen args) {
    printf("external\n");
    pid_t pid = fork();
    if (pid == 0) {
        char **args_list = make_argslist_execvp(command, args);
        if (execvp(command, args_list) == -1) {
            perror(command);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("pid");
    } else {
        int status;
        pid_t wpid = wait(&status);
        status != 0 ? shell->exit_code = status : 1;
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
    // printf("Interpret Args\n");
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
    // printf("Interpret Subshell\n");
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
    // printf("Interpret Bang\n");
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
    // printf("Interpret Expression\n");
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

Shell shell_init(char* file_name) {
    Shell shell = {0};
    shell.file_name = file_name;
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
            interpret_expression(expr, &shell);
            free_Expression(expr);
        }
        free(line);
    }
}

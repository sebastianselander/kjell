#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "lexer.h"
#include "parser.h"

#define AST_NEW(tag, ...)                                                      \
    ast_new((AST){tag, {.tag = (struct tag){__VA_ARGS__}}})

#define DEBUG true
#define BUFFERSIZE_INIT 1024

int i, error = 0;
int i_max = 0;

void debug(char *string, int i) {
    if (DEBUG && i == -1) {
        printf("%s\n", string);
    } else if (DEBUG) {
        printf("%s: %d\n", string, i);
    }
}

/* TODO:
    - Refactor / rewrite. Way too ad-hoccy
    - Implement automatically resizing buffers instead of exiting on reaching cap
*/


AST *ast_new(AST ast) {
    AST *ptr = malloc(sizeof(AST));
    if (ptr != NULL) {
        *ptr = ast;
    }
    return ptr;
}

void ast_print(AST *ptr) {
    AST ast = *ptr;
    switch (ast.tag) {
    case AST_TEXT: {
        struct AST_TEXT data = ast.data.AST_TEXT;
        Tokens tokens = data.tokens;
        for (int i = 0; i < tokens.tokens_len; i++) {
            if (i != 0)
                printf(" ");
            print_str(tokens.tokens[i].text, tokens.tokens[i].text_len);
        }
        return;
    }
    case AST_BANG: {
        struct AST_BANG data = ast.data.AST_BANG;
        printf("! (");
        ast_print(data.shell);
        printf(")");
        return;
    }
    case AST_SUBSHELL: {
        struct AST_SUBSHELL data = ast.data.AST_SUBSHELL;
        printf("[");
        ast_print(data.shell);
        printf("]");
        return;
    }
    case AST_AND: {
        struct AST_AND data = ast.data.AST_AND;
        printf("(");
        ast_print(data.left);
        printf(" && ");
        ast_print(data.right);
        printf(")");
        return;
    }
    case AST_OR: {
        struct AST_OR data = ast.data.AST_OR;
        printf("(");
        ast_print(data.left);
        printf(" || ");
        ast_print(data.right);
        printf(")");
        return;
    }
    case AST_SEQ: {
        struct AST_SEQ data = ast.data.AST_SEQ;
        ast_print(data.left);
        printf("; ");
        ast_print(data.right);
        return;
        }
    }
}

void ast_free(AST *ptr) {
    if (ptr == NULL)
        printf("NULL POINTER\n");
    AST ast = *ptr;
    switch (ast.tag) {
    case AST_TEXT: {
        struct AST_TEXT data = ast.data.AST_TEXT;
        tokens_free(data.tokens);
        break;
    }
    case AST_BANG: {
        struct AST_BANG data = ast.data.AST_BANG;
        ast_free(data.shell);
        break;
    }
    case AST_SUBSHELL: {
        struct AST_SUBSHELL data = ast.data.AST_SUBSHELL;
        ast_free(data.shell);
        break;
    }
    case AST_AND: {
        struct AST_AND data = ast.data.AST_AND;
        ast_free(data.left);
        ast_free(data.right);
        break;
    }
    case AST_OR: {
        struct AST_OR data = ast.data.AST_OR;
        ast_free(data.left);
        ast_free(data.right);
        break;
    }
    case AST_SEQ: {
        struct AST_SEQ data = ast.data.AST_SEQ;
        ast_free(data.left);
        ast_free(data.right);
        break;
    }
    }
    free(ptr);
}

/*

Before fixing left-recursion:

progarm -> seq       | empty

seq -> seq ";" or    | or
or -> or "||" and    | and
and -> and "&&" bang | bang
bang -> "!" parens   | parens
parens -> identifier | "(" seq ")"

Removing left recursion, Nyqvist style:
expression ::= seq;
seq        ::= (or ";" or)*;
or         ::= and "||" and;
and        ::= bang "&&" bang;
bang       ::= "!" subshell;
subshell   ::= [ident] | "(" expression ")"


*/

Parser parser_new(Tokens tokens) {
    Parser parser = {
        .source.tokens_len = 0,
        .source.tokens = malloc(BUFFERSIZE_INIT),
        .ast = NULL,
        .hasErrored = false,
        .error_msg = ""
    };
    return parser;
}

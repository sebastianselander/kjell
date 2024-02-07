#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

#define AST_NEW(tag, ...) ast_new((AST) {tag, {.tag=(struct tag){__VA_ARGS__}}})

typedef struct AST AST;

typedef struct Parse {
    AST *ast;
    char *rest;
} Parse;

struct AST {
    enum {
        AST_BANG,
        AST_AND,
        AST_OR,
        AST_SEQ,
        AST_SUBSHELL,
        AST_TEXT,
    } tag;
    union {
        struct AST_TEXT {
            char *text;
        } AST_TEXT;
        struct AST_BANG {
            AST *shell;
        } AST_BANG;
        struct AST_SUBSHELL {
            AST *shell;
        } AST_SUBSHELL;
        struct AST_AND {
            AST *left;
            AST *right;
        } AST_AND;
        struct AST_OR {
            AST *left;
            AST *right;
        } AST_OR;
        struct AST_SEQ {
            AST *left;
            AST *right;
        } AST_SEQ;
    } data;
};

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
            printf("%s", data.text);
            return;
        }
        case AST_BANG: {
            struct AST_BANG data = ast.data.AST_BANG;
            printf("! ");
            ast_print(data.shell);
            return;
        }
        case AST_SUBSHELL: {
            struct AST_SUBSHELL data = ast.data.AST_SUBSHELL;
            printf("(");
            ast_print(data.shell);
            printf(")");
            return;
        }
        case AST_AND: {
            struct AST_AND data = ast.data.AST_AND;
            ast_print(data.left);
            printf(" && ");
            ast_print(data.right);
            return;
        }
        case AST_OR: {
            struct AST_OR data = ast.data.AST_OR;
            ast_print(data.left);
            printf(" || ");
            ast_print(data.right);
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
    AST ast = *ptr;
    switch (ast.tag) {
        case AST_TEXT: {
            struct AST_TEXT data = ast.data.AST_TEXT;
            free(data.text);
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

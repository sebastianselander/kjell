#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "lexer.h"

#define AST_NEW(tag, ...) ast_new((AST) {tag, {.tag=(struct tag){__VA_ARGS__}}})

#define DEBUG false
#define BUFSZ 1024

typedef struct {
    Token* tokens;
    size_t tokens_len;
} TokenL;

int i, error = 0;

void debug(char *string, int i) {
    if (DEBUG && i == -1) {
        printf("%s\n",string);
    } else if (DEBUG) {
         printf("%s: %d\n",string, i);
    }
}

/* TODO:
    - Implement automatically resizing buffers instead of exiting on reaching cap
    - make `;, ||, &&` left-associative (how are they not?)
*/

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
            TokenL tokenl;
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
            debug("TEXT", -1);
            struct AST_TEXT data = ast.data.AST_TEXT;
            TokenL tokenl = data.tokenl;
            for (int i = 0; i < tokenl.tokens_len; i++) {
                if (i != 0) printf(" ");
                print_str(tokenl.tokens[i].text, tokenl.tokens[i].text_len);
            }
            return;
        }
        case AST_BANG: {
            debug("BANG", -1);
            struct AST_BANG data = ast.data.AST_BANG;
            printf("! [");
            ast_print(data.shell);
            printf("]");
            return;
        }
        case AST_SUBSHELL: {
            debug("SUBSHELL", -1);
            struct AST_SUBSHELL data = ast.data.AST_SUBSHELL;
            printf("(");
            ast_print(data.shell);
            printf(")");
            return;
        }
        case AST_AND: {
            debug("AND", -1);
            struct AST_AND data = ast.data.AST_AND;
            printf("[");
            ast_print(data.left);
            printf(" && ");
            ast_print(data.right);
            printf("]");
            return;
        }
        case AST_OR: {
            debug("OR", -1);
            struct AST_OR data = ast.data.AST_OR;
            printf("[");
            ast_print(data.left);
            printf(" || ");
            ast_print(data.right);
            printf("]");
            return;
        }
        case AST_SEQ: {
            debug("SEQ", -1);
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
            token_free(data.tokenl.tokens);
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
seq        ::= or ";" or;
or         ::= and "||" and;
and        ::= bang "&&" bang;
bang       ::= "!" subshell;
subshell   ::= [ident] | "(" expression ")"


*/

Token *input;

AST* EXPRESSION();
AST* SEQUENCE();
AST* OR();
AST* AND();
AST* BANG();
AST* SUBSHELL();


void check_mem_overflow(int i, int cap) {
    if (i >= cap) {
        fprintf(stderr, "Buffer out of memory");
        exit(EXIT_FAILURE);
    }
}

TokenL many(Token_Kind tk) {
    int bufsize = BUFSZ;
    Token* array = malloc(sizeof(Token) * bufsize);
    int j = i;
    while (input[i].kind == tk) {
        check_mem_overflow(i - j, bufsize);
        array[i-j] = input[i];
        i++;
    }

    TokenL tokenl = { .tokens = array, .tokens_len = i - j};
    return tokenl;
}


bool optional(Token_Kind tk) {
    if (input[i].kind == tk) {
        return true;
    }
    return false;
}

bool expect(Token_Kind tk) {
    if (!(input[i].kind == tk)) {
        printf("Expected: %d, got: %d, for:\n", tk, input[i].kind);
        token_print(input[i]);
        error = 1;
        return false;
    }
    i++;
    return true;
}

/*
Removing left recursion, Nyqvist style:
expression ::= seq;
seq        ::= or ";" or;
or         ::= and "||" and;
and        ::= bang "&&" bang;
bang       ::= "!" subshell;
subshell   ::= [ident] | "(" expression ")"

*/

/* 
    Return: null if input is empty
*/
AST* EXPRESSION() {
    return SEQUENCE();
}

AST* SEQUENCE() {
    AST* expr = OR();
    while (expect(TOKEN_SEMICOLON)) {
        AST* right = OR();
        expr = AST_NEW(AST_SEQ, expr, right);
    }
    return expr;
}

AST* OR() {
    AST* expr = AND();
    while(expect(TOKEN_OR)) {
        AST* right = AND();
        expr = AST_NEW(AST_OR, expr, right);
    }
    return expr;
}

AST* AND() {
    AST* left = BANG();
    expect(TOKEN_AND);
    AST* right = BANG();
    AST* and = AST_NEW(AST_AND, left, right);
    return and;
}

AST* BANG() {
    expect(TOKEN_BANG);
    AST* one = SUBSHELL();
    AST* bang = AST_NEW(AST_BANG, one);
    return bang;
}

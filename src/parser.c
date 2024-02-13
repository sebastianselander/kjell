#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "lexer.h"
#include "parser.h"

#define AST_NEW(tag, ...)                                                      \
    ast_new((AST){tag, {.tag = (struct tag){__VA_ARGS__}}})

#define DEBUG false
#define BUFSZ 1024

int i, error = 0;

void debug(char *string, int i) {
    if (DEBUG && i == -1) {
        printf("%s\n", string);
    } else if (DEBUG) {
        printf("%s: %d\n", string, i);
    }
}

/* TODO:
    - Implement automatically resizing buffers instead of exiting on reaching
   cap
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
        debug("TEXT", -1);
        struct AST_TEXT data = ast.data.AST_TEXT;
        TokenL tokenl = data.tokenl;
        for (int i = 0; i < tokenl.tokens_len; i++) {
            if (i != 0)
                printf(" ");
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
    if (ptr == NULL)
        printf("NULL POINTER\n");
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
seq        ::= (or ";" or)*;
or         ::= and "||" and;
and        ::= bang "&&" bang;
bang       ::= "!" subshell;
subshell   ::= [ident] | "(" expression ")"


*/

Token *input;

AST *EXPRESSION();
AST *SEQUENCE();
AST *OR();
AST *AND();
AST *BANG();
AST *SUBSHELL();

void check_mem_overflow(int i, int cap) {
    if (i >= cap) {
        fprintf(stderr, "Buffer out of memory");
        exit(EXIT_FAILURE);
    }
}

TokenL many(Token_Kind tk) {
    int bufsize = BUFSZ;
    Token *array = malloc(sizeof(Token) * bufsize);
    int j = i;
    while (input[i].kind == tk) {
        check_mem_overflow(i - j, bufsize);
        array[i - j] = input[i];
        i++;
    }

    TokenL tokenl = {.tokens = array, .tokens_len = i - j};
    return tokenl;
}

bool optional(Token_Kind tk) {
    if (input[i].kind == tk) {
        return true;
    }
    return false;
}

Token_Kind peek() { return input[i].kind; }

bool try_match(Token_Kind tk) {
    Token_Kind current_token = peek();
    if (!(current_token == tk)) {
        return false;
    }
    i++;
    return true;
}

bool match(Token_Kind tk) {
    Token_Kind current_token = peek();
    if (!(current_token == tk)) {
        printf("Expected: %s, got: %s\n", token_show(tk),
               token_show(current_token));
        /* token_print(input[i]); */
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
AST *EXPRESSION() {
    if (peek() == TOKEN_EOF)
        return NULL;
    return SEQUENCE();
}

AST *SEQUENCE() {
    if (DEBUG)
        printf("SEQUENCE - parsing token: %s\n", token_show(peek()));
    AST *expr = OR();
    while (peek() == TOKEN_SEMICOLON) {
        i++;
        AST *right = OR();
        expr = AST_NEW(AST_SEQ, expr, right);
    }
    return expr;
}

AST *OR() {
    if (DEBUG)
        printf("OR - parsing token: %s\n", token_show(peek()));
    AST *expr = AND();
    while (peek() == TOKEN_OR) {
        i++;
        AST *right = AND();
        expr = AST_NEW(AST_OR, expr, right);
    }
    return expr;
}

AST *AND() {
    if (DEBUG)
        printf("AND - parsing token: %s\n", token_show(peek()));
    AST *expr = BANG();
    while (peek() == TOKEN_AND) {
        i++;
        AST *right = BANG();
        expr = AST_NEW(AST_AND, expr, right);
    }
    return expr;
}

AST *BANG() {
    if (DEBUG)
        printf("BANG - parsing token: %s\n", token_show(peek()));
    if (try_match(TOKEN_BANG)) {
        AST *expr = BANG();
        AST *bang = AST_NEW(AST_BANG, expr);
        return bang;
    }
    return SUBSHELL();
}

AST *SUBSHELL() {
    if (DEBUG)
        printf("SUBSHELL - parsing token: %s\n", token_show(peek()));
    if (peek() == (TOKEN_LPAREN)) {
        i++;
        AST *expr = EXPRESSION();
        AST *subshell = AST_NEW(AST_SUBSHELL, expr);
        match(TOKEN_RPAREN);
        return subshell;
    }
    if (peek() == TOKEN_TEXT) {
        int tokens_count = 0;
        // TODO: Resize buffer
        Token *tokens = malloc(sizeof(Token) * BUFSZ);
        while (peek() == TOKEN_TEXT) {
            tokens[tokens_count] = input[i];
            tokens_count++;
            i++;
        }
        TokenL tokenls = {.tokens = tokens, .tokens_len = tokens_count};
        AST *expr = AST_NEW(AST_TEXT, tokenls);
        return expr;
    }
    return EXPRESSION();
}

AST* parse(Token_Info ti) {
    Token* tokens = ti.tokens;
    size_t tokens_len = ti.tokens_len;

    i = 0;
    input = tokens;

    AST* expr = EXPRESSION();
    i = 0;
    return expr;
}

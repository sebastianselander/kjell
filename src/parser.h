#ifndef PARSER_H

#include "lexer.h"

#define PARSER_H

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

AST* parse(Token_Info ti);
void ast_print(AST* ptr);

#endif

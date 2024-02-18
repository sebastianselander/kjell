#ifndef PARSER_H

#include "lexer.h"

#define PARSER_H

typedef struct AST AST;

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
            Tokens tokens;
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

typedef struct {
    Tokens source;
    AST* ast;
    size_t cursor;
    bool hasErrored;
    char* error_msg;
} Parser;

typedef struct {
    AST* expression;
    char* error_msg;
} Parsed;

Parser parser_new(Tokens source);
void parser_free(Parser p);
void parser_parse(Parser* parser);
void ast_print(AST* ptr);
void ast_free(AST* ptr);

Parsed parse(String input);

#endif

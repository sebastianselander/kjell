#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_SPACE,
    TOKEN_TEXT,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_AND,
    TOKEN_AMPERSAND,
    TOKEN_OR,
    TOKEN_BANG,
    TOKEN_PIPE,
} Token_type;

const char* const token_type_str[] = {
    [TOKEN_EOF] = "TOKEN_EOF",
    [TOKEN_SPACE] = "TOKEN_SPACE",
    [TOKEN_TEXT] = "TOKEN_TEXT",
    [TOKEN_SEMICOLON] = "TOKEN_SEMICOLON",
    [TOKEN_LPAREN] = "TOKEN_LPAREN",
    [TOKEN_RPAREN] = "TOKEN_RPAREN",
    [TOKEN_AND] = "TOKEN_AND",
    [TOKEN_AMPERSAND] = "TOKEN_AMPERSAND",
    [TOKEN_OR] = "TOKEN_OR",
    [TOKEN_BANG] = "TOKEN_BANG",
    [TOKEN_PIPE] = "TOKEN_PIPE",
};

typedef struct {
    Token_type kind;
    const char *text;
    size_t text_len;
} Token;

typedef struct {
    String source;
    size_t cursor;
    Token* tokens;
    size_t tokens_len;
    bool hasErrored;
    char* error_msg;
} Lexer;

#endif

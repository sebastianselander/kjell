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
} Token_Type;

char* token_type_to_str_raw(Token_Type t);
char* token_type_to_str_pretty(Token_Type t);

typedef struct {
    Token_Type type;
    const char *text;
    size_t text_len;
} Token;

typedef struct {
    Token* tokens;
    size_t tokens_len;
} Tokens;

typedef struct {
    String source;
    size_t cursor;
    Tokens tokens;
    bool hasErrored;
    char* error_msg;
} Lexer;

Lexer lexer_new(String string);
void lexer_scan(Lexer* l);
void tokens_free(Tokens tokens);
void tokens_print(Token* tokens);

#endif

#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

typedef enum {
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_BANG,
    TOKEN_TEXT,
    TOKEN_SYMBOL,
    TOKEN_INVALID,
} Token_Kind;

typedef struct {
    Token_Kind kind;
    const char *text;
    size_t text_len;
} Token;

typedef struct {
    const char* content;
    size_t content_len;
    size_t cursor;
} Lexer;

Lexer lexer_new(const char* content, size_t content_len);
Token lexer_next(Lexer *lexer);

void lexer_print_state(Lexer lexer);
void token_print(Token token);
bool lexer_done(Lexer *lexer);

#endif

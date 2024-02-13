#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    TOKEN_SEMICOLON = 0,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_BANG,
    TOKEN_TEXT,
    TOKEN_SYMBOL,
    TOKEN_INVALID,
    TOKEN_EOF,
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

typedef struct {
    Token *tokens;
    size_t tokens_len;

} Token_Info;

typedef struct {
    Token *tokens;
    size_t tokens_len;
} TokenL;

Lexer lexer_new(const char* content, size_t content_len);
Token lexer_next(Lexer *lexer);

Token_Info tokenize(const char* content, size_t content_len);

void lexer_print_state(Lexer lexer);
void token_print(Token token);
bool lexer_continue(Lexer *lexer);
void token_free(Token *ptr);
char* token_show(Token_Kind tok);

#endif

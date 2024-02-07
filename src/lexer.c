#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

typedef enum {
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_BANG,
    TOKEN_TEXT,
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

Lexer lexer_new(const char* content, size_t content_len) {
    Lexer l = {
        .content = content,
        .content_len = content_len,
        .cursor = 0
    };
    return l;
}

#define TOKEN_SYMBOL "!@#$%^&*"
bool is_symbol(char c) {
    const char *symbols = TOKEN_SYMBOL;
    int len = sizeof(TOKEN_SYMBOL) / sizeof(char);
    for (int i = 0; i < len; i++) {
       if (symbols[i] == c) {
           return true;
       }
    }
    return false;
}

#define TOKEN_DELIM " \r\t\n\a"

bool is_delim(char c) {
    const char *delim = TOKEN_DELIM;
    int len = sizeof(TOKEN_DELIM) / sizeof(char);
    for (int i = 0; i < len; i++) {
        if (delim[i] == c) {
            return true;
        }
    }
    return false;
}

void lexer_trim_left(Lexer *lexer) {
    while (lexer->cursor < lexer->content_len && is_delim(lexer->content[lexer->cursor])) {
        lexer->cursor += 1;
    }
}

Token lexer_next(Lexer *lexer) {
    lexer_trim_left(lexer);

    Token token = {
        .text = &lexer->content[lexer->cursor],
    };

    char current_chr = lexer->content[lexer->cursor];

    if (lexer->cursor >= lexer->content_len) return token;

    if (current_chr == '!') {
        lexer->cursor += 1;
        token.kind = TOKEN_BANG;
        token.text_len = 1;
        return token;
    }

    if (current_chr == ';') {
        lexer->cursor += 1;
        token.kind = TOKEN_SEMICOLON;
        token.text_len = 1;
        return token;
    }

    if (current_chr == '&') {
        int next_cursor = lexer->cursor + 1;
        char next_chr = lexer->content[next_cursor];
        if (next_cursor < lexer->content_len && next_chr == '&') {
            lexer->cursor += 2;
            token.text_len = 2;
            token.kind = TOKEN_AND;
            return token;
        }
    }

    return token;
}

void print_str(const char *text, size_t text_len) {
    for (int i = 0; i < text_len; i++) {
        printf("%c", text[i]);
    }
    printf("\n");
}

void token_print(Token token) {
    switch (token.kind) {
        case TOKEN_SEMICOLON: {
            printf("Token: SEMICOLON\n");
            break;
        }
        case TOKEN_LPAREN:{
            printf("Token: LPAREN\n");
            break;
        }
        case TOKEN_RPAREN:{
            printf("Token: RPAREN\n");
            break;
        }
        case TOKEN_AND:{
            printf("Token: AND\n");
            break;
        }
        case TOKEN_OR:{
            printf("Token: OR\n");
            break;
        }
        case TOKEN_BANG:{
            printf("Token: BANG\n");
            break;
        }
        case TOKEN_TEXT:{
            printf("Token: TEXT\n");
            break;
        }
        case TOKEN_INVALID:{
            printf("Token: INVALID\n");
            break;
        }
    }
    print_str(token.text, token.text_len);
}

int main(void) {
    const char *text = "! &&";
    size_t text_len = 4;
    Lexer l = lexer_new(text, text_len);
    while (l.cursor < l.content_len) {
        Token t = lexer_next(&l);
        token_print(t);
    }
}

#include "lexer.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define TOKEN_SYMBOL_SET "!;&|"
bool is_symbol(char c) {
    const char *symbols = TOKEN_SYMBOL_SET;
    int len = sizeof(TOKEN_SYMBOL_SET) / sizeof(char);
    for (int i = 0; i < len; i++) {
        if (symbols[i] == c) {
            return true;
        }
    }
    return false;
}

bool lexer_continue(Lexer *lexer) { return lexer->cursor < lexer->content_len; }

#define BUFSZ 1024
Token_Info tokenize(const char *content, size_t content_len) {
    int bufsize = BUFSZ;
    int i = 0;
    Token *array = malloc(sizeof(Token) * bufsize);
    Lexer l = lexer_new(content, content_len);
    while (lexer_continue(&l)) {
        if (i >= bufsize) {
            bufsize += BUFSZ;
            array = realloc(array, bufsize);
            if (!array) {
                fprintf(stderr, "Tokenize: Allocation error");
                exit(EXIT_FAILURE);
            }
        }
        array[i] = lexer_next(&l);
        i++;
    }
    Token eof = {.kind = TOKEN_EOF, .text = "", .text_len = 0};
    array[i++] = eof;
    Token_Info ti = {.tokens = array, .tokens_len = i};
    return ti;
}

Lexer lexer_new(const char *content, size_t content_len) {
    Lexer l = {.content = content, .content_len = content_len, .cursor = 0};
    return l;
}

void lexer_trim_left(Lexer *lexer) {
    while (lexer->cursor < lexer->content_len &&
           isspace(lexer->content[lexer->cursor])) {
        lexer->cursor += 1;
    }
}

Token lexer_next(Lexer *lexer) {
    lexer_trim_left(lexer);

    Token token = {
        .text = &lexer->content[lexer->cursor],
        .kind = TOKEN_INVALID,
        .text_len = 0,
    };

    if (lexer->cursor >= lexer->content_len) {
        token.kind = TOKEN_EOF;
        token.text = "";
        token.text_len = 0;
        return token;
    }

    if (lexer->content[lexer->cursor] == '(') {
        lexer->cursor += 1;
        token.text_len = 1;
        token.kind = TOKEN_LPAREN;
        return token;
    }
    if (lexer->content[lexer->cursor] == ')') {
        lexer->cursor += 1;
        token.text_len = 1;
        token.kind = TOKEN_RPAREN;
        return token;
    }

    if (is_symbol(lexer->content[lexer->cursor])) {
        while (lexer->cursor < lexer->content_len &&
               is_symbol(lexer->content[lexer->cursor])) {
            lexer->cursor += 1;
            token.text_len += 1;
            token.kind = TOKEN_SYMBOL;
        }
        char *symbol_text = malloc(sizeof(char) * token.text_len);
        int i = 0;
        for (; i < token.text_len; i++) {
            symbol_text[i] = token.text[i];
        }
        symbol_text[i] = 0;
        if (strcmp(symbol_text, ";") == 0) {
            token.kind = TOKEN_SEMICOLON;
        }
        if (strcmp(symbol_text, "!") == 0) {
            token.kind = TOKEN_BANG;
        }
        if (strcmp(symbol_text, "&&") == 0) {
            token.kind = TOKEN_AND;
        }
        if (strcmp(symbol_text, "||") == 0) {
            token.kind = TOKEN_OR;
        }
        free(symbol_text);

        return token;
    }

    bool not_isspace = !isspace(lexer->content[lexer->cursor]);
    bool not_issymbol = !is_symbol(lexer->content[lexer->cursor]);
    bool not_isleftparen = lexer->content[lexer->cursor] != '(';
    bool not_isrightparen = lexer->content[lexer->cursor] != ')';
    if (not_isspace && not_issymbol && not_isleftparen && not_isrightparen) {
        bool cursor_in_range = lexer->cursor < lexer->content_len;
        while (cursor_in_range && not_isspace && not_issymbol &&
               not_isleftparen && not_isrightparen) {
            lexer->cursor += 1;
            token.text_len += 1;
            token.kind = TOKEN_TEXT;
            not_isspace = !isspace(lexer->content[lexer->cursor]);
            not_issymbol = !is_symbol(lexer->content[lexer->cursor]);
            not_isleftparen = lexer->content[lexer->cursor] != '(';
            not_isrightparen = lexer->content[lexer->cursor] != ')';
        }
        char *text_text = malloc(sizeof(char) * token.text_len);
        int i = 0;
        for (; i < token.text_len; i++) {
            text_text[i] = token.text[i];
        }
        text_text[i] = 0;
    }
    return token;
}

void lexer_print_state(Lexer lexer) {
    int content_len = lexer.content_len;
    int cursor = lexer.cursor;
    const char *content = lexer.content;
    printf("\n--Lexer State--\nContent length: %d\nCursor: %d\nContent: %s\n\n",
           content_len, cursor, content);
}

void token_free(Token *ptr) {
    Token token = *ptr;
    free((char *)token.text);
}

char *token_show(Token_Kind tok) {
    char *kind_text = "";
    switch (tok) {
    case TOKEN_SEMICOLON: {
        kind_text = "SEMICOLON";
        break;
    }
    case TOKEN_LPAREN: {
        kind_text = "LPAREN";
        break;
    }
    case TOKEN_RPAREN: {
        kind_text = "RPAREN";
        break;
    }
    case TOKEN_AND: {
        kind_text = "AND";
        break;
    }
    case TOKEN_OR: {
        kind_text = "OR";
        break;
    }
    case TOKEN_BANG: {
        kind_text = "BANG";
        break;
    }
    case TOKEN_TEXT: {
        kind_text = "TEXT";
        break;
    }
    case TOKEN_SYMBOL: {
        kind_text = "SYMBOL";
        break;
    }
    case TOKEN_INVALID: {
        kind_text = "INVALID";
        break;
    }
    case TOKEN_EOF: {
        kind_text = "EOF";
        break;
    }
    }
    return kind_text;
}

void token_print(Token token) {
    char *kind_text = token_show(token.kind);
    printf("Token: {\n");
    printf("    kind: %s\n", kind_text);
    printf("    text: '");
    print_str(token.text, token.text_len);
    printf("'\n    text_len: %d\n}\n", (int)token.text_len);
}

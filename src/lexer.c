#include "lexer.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define BUFFERSIZE_INIT 1024

bool isAtEnd(Lexer *l) { return l->cursor >= l->source.text_len; }

char advance(Lexer *l) { return l->source.text[l->cursor++]; }

bool match(Lexer *l, char expected) {
    if (isAtEnd(l)) {
        return false;
    }
    if (l->source.text[l->cursor] != expected) {
        return false;
    }
    l->cursor += 1;
    return true;
}

char peek(Lexer *l) { return l->source.text[l->cursor]; }

bool isAlpha(char c) { return isalpha(c) || c == '/' || c == '.'; }

void lexer_addToken(Lexer *l, Token token) {
    l->tokens.tokens[l->tokens.tokens_len] = token;
    l->tokens.tokens_len += 1;
}

void lexer_error(Lexer *l, char *msg) {
    l->error_msg = msg;
    l->hasErrored = true;
}

Token eof() {
    Token eof = {.kind = TOKEN_EOF, .text = "", .text_len = 0};
    return eof;
}

Lexer lexer_new(String string) {
    Lexer l = {.source = string,
               .cursor = 0,
               .tokens.tokens = malloc(BUFFERSIZE_INIT),
               .tokens.tokens_len = 0,
               .hasErrored = false,
               .error_msg = "",
    };
    return l;
}

void lexer_scan(Lexer *l) {
    while (!(isAtEnd(l) || l->hasErrored)) {
        char c = advance(l);
        switch (c) {
        case '(': {
            Token lparen = {.kind = TOKEN_LPAREN, .text = "", .text_len = 0};
            lexer_addToken(l, lparen);
        } break;
        case ')': {
            Token rparen = {.kind = TOKEN_RPAREN, .text = "", .text_len = 0};
            lexer_addToken(l, rparen);
        } break;
        case '!': {
            Token bang = {.kind = TOKEN_BANG, .text = "!", .text_len = 1};
            lexer_addToken(l, bang);
        } break;
        case ';': {
            Token semicolon = {
                .kind = TOKEN_SEMICOLON, .text = ";", .text_len = 1};
            lexer_addToken(l, semicolon);
        } break;
        case '&': {
            Token and = {.kind = TOKEN_AND, .text = "&&", .text_len = 2};
            Token ampersand = {
                .kind = TOKEN_AMPERSAND, .text = "&", .text_len = 1};
            match(l, '&') ? lexer_addToken(l, and)
                          : lexer_addToken(l, ampersand);
        } break;
        case '|': {
            Token or = {.kind = TOKEN_OR, .text = "||", .text_len = 2};
            Token pipe = {.kind = TOKEN_PIPE, .text = "|", .text_len = 1};
            match(l, '|') ? lexer_addToken(l, or) : lexer_addToken(l, pipe);
        } break;
        case ' ': {
            const char *text = &l->source.text[l->cursor];
            int len = 1;
            while (match(l, ' ')) {
                len += 1;
            }
            Token space = {.kind = TOKEN_SPACE, .text = text, .text_len = len};
            lexer_addToken(l, space);
        } break;
        default: {
            const char *text = &l->source.text[l->cursor];
            int len = 0;
            while (isAlpha(peek(l))) {
                advance(l);
                len += 1;
            }
            Token token_text = {
                .kind = TOKEN_TEXT, .text = text, .text_len = len};
            lexer_addToken(l, token_text);
        } break;
        };
    }
    lexer_addToken(l, eof());
}

void tokens_free(Tokens tokens) {
    Token* ptr = tokens.tokens;
    free(ptr);
}

void lexer_free(Lexer l) {
    tokens_free(l.tokens);
}

int main() {
    String str = { .text = malloc(1024), .text_len = 0 };
    str.text = "cd      .. || true || true";
    str.text_len = strlen(str.text);
    Lexer l = lexer_new(str);
    lexer_scan(&l);
    for (int i = 0; i < l.tokens.tokens_len; i++) {
        Token t = l.tokens.tokens[i];
        if (t.kind == TOKEN_SPACE) continue;
        printf("%s\n", token_type_str[t.kind]);
    }
    lexer_free(l);
}

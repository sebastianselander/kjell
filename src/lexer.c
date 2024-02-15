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
    Token eof = {.type = TOKEN_EOF, .text = "", .text_len = 0};
    return eof;
}

Lexer lexer_new(String string) {
    Lexer l = {
        .source = string,
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
        int current = l->cursor;
        char c = advance(l);
        switch (c) {
        case '(': {
            Token lparen = {.type = TOKEN_LPAREN, .text = "", .text_len = 0};
            lexer_addToken(l, lparen);
        } break;
        case ')': {
            Token rparen = {.type = TOKEN_RPAREN, .text = "", .text_len = 0};
            lexer_addToken(l, rparen);
        } break;
        case '!': {
            Token bang = {.type = TOKEN_BANG, .text = "!", .text_len = 1};
            lexer_addToken(l, bang);
        } break;
        case ';': {
            Token semicolon = {
                .type = TOKEN_SEMICOLON, .text = ";", .text_len = 1};
            lexer_addToken(l, semicolon);
        } break;
        case '&': {
            Token and = {.type = TOKEN_AND, .text = "&&", .text_len = 2};
            Token ampersand = {
                .type = TOKEN_AMPERSAND, .text = "&", .text_len = 1};
            match(l, '&') ? lexer_addToken(l, and)
                          : lexer_addToken(l, ampersand);
        } break;
        case '|': {
            Token or = {.type = TOKEN_OR, .text = "||", .text_len = 2};
            Token pipe = {.type = TOKEN_PIPE, .text = "|", .text_len = 1};
            match(l, '|') ? lexer_addToken(l, or) : lexer_addToken(l, pipe);
        } break;
        case ' ': break;
        default: {
            const char *text = &l->source.text[current];
            int len = 1;
            while (isAlpha(peek(l))) {
                advance(l);
                len += 1;
            }
            Token token_text = {
                .type = TOKEN_TEXT, .text = text, .text_len = len};
            lexer_addToken(l, token_text);
        } break;
        };
    }
    lexer_addToken(l, eof());
}

void tokens_free(Tokens tokens) {
    Token *ptr = tokens.tokens;
    free(ptr);
}

void lexer_free(Lexer l) { tokens_free(l.tokens); }

char *token_type_to_str_raw(Token_Type t) {
    switch (t) {
    case TOKEN_EOF:
        return "TOKEN_EOF";
    case TOKEN_SPACE:
        return "TOKEN_SPACE";
    case TOKEN_TEXT:
        return "TOKEN_TEXT";
    case TOKEN_SEMICOLON:
        return "TOKEN_SEMICOLON";
    case TOKEN_LPAREN:
        return "TOKEN_LPAREN";
    case TOKEN_RPAREN:
        return "TOKEN_RPAREN";
    case TOKEN_AND:
        return "TOKEN_AND";
    case TOKEN_AMPERSAND:
        return "TOKEN_AMPERSAND";
    case TOKEN_OR:
        return "TOKEN_OR";
    case TOKEN_BANG:
        return "TOKEN_BANG";
    case TOKEN_PIPE:
        return "TOKEN_PIPE";
    default:
        return "UNKNOWN TOKEN";
    }
}

char *token_type_to_str_pretty(Token_Type t) {
    switch (t) {
    case TOKEN_EOF:
        return "eof";
    case TOKEN_SPACE:
        return "' ' ";
    case TOKEN_TEXT:
        return "<text>";
    case TOKEN_SEMICOLON:
        return "';'";
    case TOKEN_LPAREN:
        return "'('";
    case TOKEN_RPAREN:
        return "')'";
    case TOKEN_AND:
        return "'&&'";
    case TOKEN_AMPERSAND:
        return "'&'";
    case TOKEN_OR:
        return "'||'";
    case TOKEN_BANG:
        return "'!'";
    case TOKEN_PIPE:
        return "'|'";
    default:
        return "UNKNOWN TOKEN";
    }
}

void tokens_print(Token *tokens) {
    int i = 0;
    while (tokens[i].type != TOKEN_EOF) {
        Token token = tokens[i];
        printf("Token: {\n");
        printf("    kind: %s\n", token_type_to_str_raw(token.type));
        printf("    text: '");
        print_str(token.text, token.text_len);
        printf("'\n    text_len: %d\n}\n", (int)token.text_len);
        i += 1;
    }
    Token token = tokens[i];
    printf("Token: {\n");
    printf("    kind: %s\n", token_type_to_str_raw(token.type));
    printf("    text: '");
    print_str(token.text, token.text_len);
    printf("'\n    text_len: %d\n}\n", (int)token.text_len);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "utils.h"

#define AST_NEW(tag, ...)                                                      \
    ast_new((AST){tag, {.tag = (struct tag){__VA_ARGS__}}})

#define DEBUG true
#define BUFFERSIZE_INIT 1024

int i, error = 0;
int i_max = 0;

void debug(char *string, int i) {
    if (DEBUG && i == -1) {
        printf("%s\n", string);
    } else if (DEBUG) {
        printf("%s: %d\n", string, i);
    }
}

/* TODO:
    - Implement automatically resizing buffers instead of exiting on reaching
   cap
*/

AST *ast_new(AST ast) {
    AST *ptr = malloc(sizeof(AST));
    if (ptr != NULL) {
        *ptr = ast;
    }
    return ptr;
}

void ast_print(AST *ptr) {
    if (!ptr) {
        printf("Empty ast");
        return;
    }
    AST ast = *ptr;
    switch (ast.tag) {
    case AST_TEXT: {
        struct AST_TEXT data = ast.data.AST_TEXT;
        Tokens tokens = data.tokens;
        for (size_t i = 0; i < tokens.tokens_len; i++) {
            if (i != 0)
                printf(" ");
            print_str(tokens.tokens[i].text, tokens.tokens[i].text_len);
        }
        return;
    }
    case AST_BANG: {
        struct AST_BANG data = ast.data.AST_BANG;
        printf("! (");
        ast_print(data.shell);
        printf(")");
        return;
    }
    case AST_SUBSHELL: {
        struct AST_SUBSHELL data = ast.data.AST_SUBSHELL;
        printf("[");
        ast_print(data.shell);
        printf("]");
        return;
    }
    case AST_AND: {
        struct AST_AND data = ast.data.AST_AND;
        printf("(");
        ast_print(data.left);
        printf(" && ");
        ast_print(data.right);
        printf(")");
        return;
    }
    case AST_OR: {
        struct AST_OR data = ast.data.AST_OR;
        printf("(");
        ast_print(data.left);
        printf(" || ");
        ast_print(data.right);
        printf(")");
        return;
    }
    case AST_SEQ: {
        struct AST_SEQ data = ast.data.AST_SEQ;
        ast_print(data.left);
        printf("; ");
        ast_print(data.right);
        return;
    }
    }
}

void ast_free(AST *ptr) {
    if (ptr == NULL) return;
    AST ast = *ptr;
    switch (ast.tag) {
    case AST_TEXT: {
        struct AST_TEXT data = ast.data.AST_TEXT;
        tokens_free(data.tokens);
        break;
    }
    case AST_BANG: {
        struct AST_BANG data = ast.data.AST_BANG;
        ast_free(data.shell);
        break;
    }
    case AST_SUBSHELL: {
        struct AST_SUBSHELL data = ast.data.AST_SUBSHELL;
        ast_free(data.shell);
        break;
    }
    case AST_AND: {
        struct AST_AND data = ast.data.AST_AND;
        ast_free(data.left);
        ast_free(data.right);
        break;
    }
    case AST_OR: {
        struct AST_OR data = ast.data.AST_OR;
        ast_free(data.left);
        ast_free(data.right);
        break;
    }
    case AST_SEQ: {
        struct AST_SEQ data = ast.data.AST_SEQ;
        ast_free(data.left);
        ast_free(data.right);
        break;
    }
    }
    free(ptr);
}

/*

Before fixing left-recursion:

progarm -> seq       | empty

seq -> seq ";" or    | or
or -> or "||" and    | and
and -> and "&&" bang | bang
bang -> "!" parens   | parens
parens -> identifier | "(" seq ")"

Removing left recursion, Nyqvist style:
expression ::= seq;
seq        ::= (or ";" or)*;
or         ::= and "||" and;
and        ::= bang "&&" bang;
bang       ::= "!" subshell;
subshell   ::= [ident] | "(" expression ")"


*/

Parser parser_new(Tokens tokens) {
    Parser parser = {.source = tokens,
                     .cursor = 0,
                     .ast = NULL,
                     .hasErrored = false,
                     .error_msg = "BUG: this text should not be shown"};
    return parser;
}

void parser_free(Parser p) {
    if (p.error_msg) {
        free(p.error_msg);
    }
    if (p.ast) {
        ast_free(p.ast);
    }
    if (p.source.tokens) {
        tokens_free(p.source);
    }
}

Token parser_peek(Parser *p) { return p->source.tokens[p->cursor]; }

bool parser_isAtEnd(Parser *p) { return parser_peek(p).type == TOKEN_EOF; }

Token parser_previous(Parser *p) { return p->source.tokens[p->cursor - 1]; }

Token parser_advance(Parser *p) {
    if (!parser_isAtEnd(p))
        p->cursor++;
    return parser_previous(p);
}

Token parser_next(Parser *p) { return p->source.tokens[p->cursor + 1]; }

bool parser_check(Parser *p, Token_Type type) {
    if (parser_isAtEnd(p))
        return false;
    return parser_peek(p).type == type;
}

bool parser_match(Parser *p, Token_Type tok) {
    if (parser_check(p, tok)) {
        parser_advance(p);
        return true;
    }
    return false;
}

void consume(Parser *p, Token_Type tok) {
    Token_Type current = parser_peek(p).type;
    if (parser_check(p, tok)) {
        parser_advance(p);
    } else {
        p->hasErrored = true;
        char* msg = malloc(sizeof(char) * 1024);
        sprintf(msg, "Expected %s, but got %s\n", token_type_to_str_pretty(current),
                token_type_to_str_pretty(tok));
        p->error_msg = msg;
    }
}

AST *Or(Parser *p);
AST *Sequence(Parser *p);
AST *Bang(Parser *p);
AST *Subshell(Parser *p);

void parser_parse(Parser *p) {
    AST *expression = Sequence(p);
    if (p->hasErrored) {
        p->ast = NULL;
        return;
    }
    Token_Type current = parser_peek(p).type;
    if (current != TOKEN_EOF) {
        p->hasErrored = true;
        char* msg = malloc(sizeof(char) * 100);
        sprintf(msg, "Expected %s, but got %s\n", token_type_to_str_pretty(TOKEN_EOF),
                token_type_to_str_pretty(current));
        p->error_msg = msg;
        p->ast = NULL;
        return;
    }
    p->ast = expression;
}

AST *Sequence(Parser *p) {
    if (p->hasErrored)
        return NULL;
    AST *expr = Bang(p);
    while (parser_match(p, TOKEN_SEMICOLON)) {
        AST *right = Bang(p);
        expr = AST_NEW(AST_SEQ, expr, right);
    }

    while (parser_match(p, TOKEN_OR)) {
        AST *right = Bang(p);
        expr = AST_NEW(AST_OR, expr, right);
    }

    while (parser_match(p, TOKEN_AND)) {
        AST *right = Bang(p);
        expr = AST_NEW(AST_AND, expr, right);
    }
    return expr;
}

AST *Bang(Parser *p) {
    if (p->hasErrored)
        return NULL;
    if (parser_match(p, TOKEN_BANG)) {
        AST *bang = Bang(p);
        AST *expr = AST_NEW(AST_BANG, bang);
        return expr;
    } else {
        return Subshell(p);
    }
}

AST *Subshell(Parser *p) {
    if (parser_match(p, TOKEN_LPAREN)) {
        AST *subshell = Sequence(p);
        consume(p, TOKEN_RPAREN);
        if (p->hasErrored)
            return NULL;
        AST *expr = AST_NEW(AST_SUBSHELL, subshell);
        return expr;
    }
    Token_Type current = parser_peek(p).type;
    if (current == TOKEN_TEXT) {
        Token *token = malloc(sizeof(Token) * 1024);
        int i = 0;
        for (; parser_match(p, TOKEN_TEXT); i++) {
            token[i] = parser_previous(p);
        }
        if (i == 0) {
            p->hasErrored = true;
            p->error_msg = "Expected text, but got none";
            return NULL;
        } else {
            Tokens tokens = {.tokens = token, .tokens_len = i};
            AST *expr = AST_NEW(AST_TEXT, tokens);
            return expr;
        }
    }
    p->hasErrored = true;
    char msg[100];
    sprintf(msg, "Expected %s, but got %s\n", token_type_to_str_pretty(TOKEN_EOF),
            token_type_to_str_pretty(current));
    p->error_msg = msg;
    return NULL;
}

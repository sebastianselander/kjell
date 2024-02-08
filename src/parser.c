#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "lexer.h"

#define AST_NEW(tag, ...) ast_new((AST) {tag, {.tag=(struct tag){__VA_ARGS__}}})

#define DEBUG false
#define BUFSZ 1024

typedef struct {
    Token* tokens;
    size_t tokens_len;
} TokenL;

int i, error = 0;

void debug(char *string, int i) {
    if (DEBUG && i == -1) {
        printf("%s\n",string);
    } else if (DEBUG) {
         printf("%s: %d\n",string, i);
    }
}

/* TODO:
    - Implement automatically resizing buffers instead of exiting on reaching cap
*/

typedef struct AST AST;

typedef struct Parse {
    AST *ast;
    char *rest;
} Parse;

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
            TokenL tokenl;
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

AST *ast_new(AST ast) {
    AST *ptr = malloc(sizeof(AST));
    if (ptr != NULL) {
        *ptr = ast;
    }
    return ptr;
}

void ast_print(AST *ptr) {
    AST ast = *ptr;
    switch (ast.tag) {
        case AST_TEXT: {
            debug("TEXT", -1);
            struct AST_TEXT data = ast.data.AST_TEXT;
            TokenL tokenl = data.tokenl;
            for (int i = 0; i < tokenl.tokens_len; i++) {
                if (i != 0) printf(" ");
                print_str(tokenl.tokens[i].text, tokenl.tokens[i].text_len);
            }
            return;
        }
        case AST_BANG: {
            debug("BANG", -1);
            struct AST_BANG data = ast.data.AST_BANG;
            printf("! [");
            ast_print(data.shell);
            printf("]");
            return;
        }
        case AST_SUBSHELL: {
            debug("SUBSHELL", -1);
            struct AST_SUBSHELL data = ast.data.AST_SUBSHELL;
            printf("(");
            ast_print(data.shell);
            printf(")");
            return;
        }
        case AST_AND: {
            debug("AND", -1);
            struct AST_AND data = ast.data.AST_AND;
            printf("[");
            ast_print(data.left);
            printf(" && ");
            ast_print(data.right);
            printf("]");
            return;
        }
        case AST_OR: {
            debug("OR", -1);
            struct AST_OR data = ast.data.AST_OR;
            printf("[");
            ast_print(data.left);
            printf(" || ");
            ast_print(data.right);
            printf("]");
            return;
        }
        case AST_SEQ: {
            debug("SEQ", -1);
            struct AST_SEQ data = ast.data.AST_SEQ;
            ast_print(data.left);
            printf("; ");
            ast_print(data.right);
            return;
        }
    }
}

void ast_free(AST *ptr) {
    AST ast = *ptr;
    switch (ast.tag) {
        case AST_TEXT: {
            struct AST_TEXT data = ast.data.AST_TEXT;
            token_free(data.tokenl.tokens);
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

Before left-recursion:

progarm -> seq       | empty

seq -> seq ";" or    | or
or -> or "||" and    | and
and -> and "&&" bang | bang
bang -> "!" parens   | parens
parens -> identifier | "(" seq ")"

After left-recursion fix:

seq -> or seq'
seq' -> ";" or seq' | empty
or -> and or'
or' "||" and or' | empty
and -> bang and'
and' -> "&&" bang and' | empty
bang -> "!" parens | parens
parens -> [identifier] | "(" seq ")"

*/

Token *input;

AST* SEQ();
AST* SEQPRIME();
AST* OR();
AST* ORPRIME();
AST* AND();
AST* ANDPRIME();
AST* BANG();
AST* PARENS();


void check_mem_overflow(int i, int cap) {
    if (i >= cap) {
        fprintf(stderr, "Buffer out of memory");
        exit(EXIT_FAILURE);
    }
}

TokenL many(Token_Kind tk) {
    int bufsize = BUFSZ;
    Token* array = malloc(sizeof(Token) * bufsize);
    int j = i;
    while (input[i].kind == tk) {
        check_mem_overflow(i - j, bufsize);
        array[i-j] = input[i];
        i++;
    }

    TokenL tokenl = { .tokens = array, .tokens_len = i - j};
    return tokenl;
}


bool optional(Token_Kind tk) {
    if (input[i].kind == tk) {
        return true;
    }
    return false;
}

void expect(Token_Kind tk) {
    if (!(input[i].kind == tk)) {
        printf("Expected: %d, got: %d, for:\n", tk, input[i].kind);
        token_print(input[i]);
        error = 1;
        return;
    }
    i++;
}

AST* SEQ() {
    debug("SEQ", i);
    if (error) return NULL;
    AST* left = OR();
    AST* right = SEQPRIME();
    if (right == NULL) {
        return left;
    } else {
        AST* seq = AST_NEW(AST_SEQ, left, right);
        return seq;
    }
}
AST* SEQPRIME() {
    debug("SEQPRIME", i);
    if (error) return NULL;
    if (optional(TOKEN_SEMICOLON)) {
        i++;
        AST* left = OR();
        AST* right = SEQPRIME();
        if (right == NULL) {
             return left;
        } else {
            AST* seq = AST_NEW(AST_SEQ, left, right);
            return seq;
        }
        // TODO: return
    } else {
        return NULL;
    }
}

AST* OR() {
    debug("OR", i);
    if (error) return NULL;
    AST* left = AND();
    AST* right = ORPRIME();
    if (right == NULL) {
        return left;
    } else {
        AST* or = AST_NEW(AST_OR, left, right);
        return or;
    }
}

AST* ORPRIME() {
    debug("ORPRIME", i);
    if (error) return NULL;
    if (optional(TOKEN_OR)) {
        i++;
        AST* left = AND();
        AST* right = ORPRIME();
        if (right == NULL) {
            return left;
        } else {
            AST* or = AST_NEW(AST_OR, left, right);
            return or;
        }
    } else {
        return NULL;
    }
}

AST* AND() {
    debug("AND", i);
    if (error) return NULL;
    AST* left = BANG();
    AST* right = ANDPRIME();
    if (right == NULL) {
        return left;
    } else {
        AST* and = AST_NEW(AST_AND, left, right);
        return and;
    }
}

AST* ANDPRIME() {
    debug("ANDPRIME", i);
    if (error) return NULL;
    if (optional(TOKEN_AND)) {
        i++;
        AST* left = BANG();
        AST* right = ANDPRIME();
        if (right == NULL) {
            return left;
        } else {
            AST* and = AST_NEW(AST_AND, left, right);
            return and;
        }
    } else {
        return NULL;
    }
}

AST* BANG() {
    debug("BANG", i);
    if (error) return NULL;
    if (input[i].kind == TOKEN_BANG) {
        i++;
        AST* parens = PARENS();
        AST* bang = AST_NEW(AST_BANG, parens);
        return bang;
    }
    AST* parens = PARENS();
    return parens;
}

AST* PARENS() {
    debug("PARENS", i);
    if (error) return NULL;
    if (optional(TOKEN_TEXT)) {
        TokenL tokenl = many(TOKEN_TEXT);
        AST* ast = AST_NEW(AST_TEXT, tokenl);
        return ast;
    }
    expect(TOKEN_LPAREN);
    AST* seq = SEQ();
    expect(TOKEN_RPAREN);
    AST* parens = AST_NEW(AST_SUBSHELL, seq);
    return parens;
}

int main(void) {
    const char *text = "! (cd ..) && true || false";
    size_t text_len = strlen(text);
    Token_Info ti = tokenize(text, text_len);
    input = malloc(sizeof(Token) * 100);
    input = ti.tokens;
    AST* ast = SEQ();
    printf("Parsing: %s!\n", error ? "Unsuccessful" : "Successful");
    if (error == 0) {
        ast_print(ast);
        printf("\n");
    }
}

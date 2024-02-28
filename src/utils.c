#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void normalize_status(int *status) {
    if (*status > 0) {
        *status = 1;
    }
}

void print_str(const char *text, size_t text_len) {
    for (size_t i = 0; i < text_len; i++) {
        printf("%c", text[i]);
    }
}

void println_str(const char* text, size_t text_len) {
    print_str(text, text_len);
    printf("\n");
}

bool is_unqualified(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '/') {
            return false;
        }
    }
    return true;
}

char **make_chrarray(Identifier command, ListIdentifierLen list) {
    ListIdentifier l = list.list;
    char **args = malloc(sizeof(char *) * list.list_len + 1);
    args[0] = command;
    for (int i = 0; i < list.list_len; i++) {
        args[i + 1] = l->identifier_;
        l = l->listidentifier_;
    }
    return args;
}

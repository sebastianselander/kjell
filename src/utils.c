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

// returns the new size of buffer
size_t str_concat(size_t buffer_size, char** write_to, char* left, char* right) {
    size_t left_size = strlen(left) + 1;
    size_t right_size = strlen(right) + 1;
    if (left_size + right_size >= buffer_size) {
        buffer_size *= 2;
        printf("seggy? 1\n");
        *write_to = realloc(*write_to, buffer_size);
    }
    *write_to = strcat(*write_to, left);
    *write_to = strcat(*write_to, right);
    return buffer_size;
}

bool str_ends_in(char* string, char* ends_in) {
    size_t string_len = strlen(string);
    size_t ends_in_len = strlen(ends_in);
    if (string_len >= ends_in_len) {
        return (strcmp(&string[string_len - ends_in_len], ends_in)  == 0);
    }
    return false;
}

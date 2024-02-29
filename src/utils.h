#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include "gram/Absyn.h"

typedef struct ExitInfo ExitInfo;

typedef struct {
    bool exit;
    int exit_code;
    bool in_pipe;
    char* current_path;
    char* previous_path;
} Shell;

typedef struct {
    size_t list_len;
    ListIdentifier list;
} ListIdentifierLen;

void normalize_status(int *status);

void print_str(const char *text, size_t text_len);
void println_str(const char* text, size_t text_len);

void string_free(String string);

bool is_unqualified(char* str);
char **make_chrarray(Identifier command, ListIdentifierLen list);
size_t str_concat(size_t buffer_size, char** write_to, char* left, char* right);
bool str_ends_in(char* string, char* ends_in);

#endif

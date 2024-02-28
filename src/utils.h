#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include "gram/Absyn.h"

typedef struct ExitInfo ExitInfo;

typedef struct {
    bool exit;
    int exit_code;
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

#endif

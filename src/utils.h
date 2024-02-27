#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include "gram/Absyn.h"

typedef struct ExitInfo ExitInfo;

typedef struct {
    char* file_name;
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

ExitInfo exit_info_init();

struct ExitInfo {
    int exit_code;
    bool terminate;
}; 

void print_str(const char *text, size_t text_len);
void println_str(const char* text, size_t text_len);

#endif

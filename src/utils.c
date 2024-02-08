#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void normalize_status(int *status) {
    if (*status > 0) {
        *status = 1;
    }
}

ExitInfo exit_info_init() {
    ExitInfo exit_info;
    exit_info.terminate = false;
    exit_info.exit_code = 0;
    return exit_info;
}

void print_str(const char *text, size_t text_len) {
    for (int i = 0; i < text_len; i++) {
        printf("%c", text[i]);
    }
}

void println_str(const char* text, size_t text_len) {
    print_str(text, text_len);
    printf("\n");
}

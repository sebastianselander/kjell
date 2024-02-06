#ifndef UTILS_H
#define UTILS_H

#define true 1
#define false 0
#define bool char

typedef struct ExitInfo ExitInfo;

void normalize_status(int *status);

ExitInfo exit_info_init();

struct ExitInfo {
    int exit_code;
    bool terminate;
}; 

#endif

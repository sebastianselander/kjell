#include "utils.h"

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

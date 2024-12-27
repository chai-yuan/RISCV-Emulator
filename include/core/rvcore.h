#ifndef RVCODE_H
#define RVCODE_H

#include "types.h"

typedef void (*step_func_t)(void *context);
typedef bool (*check_halt_func_t)(void *context);

struct CoreFunc {
    void             *context;
    step_func_t       step;
    check_halt_func_t check_halt;
};

#endif

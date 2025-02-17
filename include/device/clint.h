#ifndef CLINT_H
#define CLINT_H

#include "types.h"

#define CLINT_SIZE 0x10000
#define CLINT_MTIMECMP 0x4000
#define CLINT_MTIME 0xbff8

struct CLINT {
    u8  data[CLINT_SIZE];
    u64 last_update;
};

void clint_init(struct CLINT *clint);

u64 clint_get_time(struct CLINT *clint);

bool clint_check_irq(struct CLINT *clint);

struct DeviceFunc clint_get_func(struct CLINT *clint);

#endif

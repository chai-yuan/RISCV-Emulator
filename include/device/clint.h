#ifndef CLINT_H
#define CLINT_H

#include "types.h"

#define CLINT_SIZE 0x10000
#define CLINT_MTIMECMP 0x4000
#define CLINT_MTIME 0xbff8

struct CLINT {
    u64 mtimecmp, mtime;
};

void clint_init(struct CLINT *clint);

struct DeviceFunc clint_get_func(struct CLINT *clint);

#endif

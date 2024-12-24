#ifndef SRAM_H
#define SRAM_H

#include "device/device.h"

struct Sram {
    void *data;
    u32 len;
};

void sram_init(struct Sram *sram, void *data, u32 len);

struct DeviceFunc sram_get_func(struct Sram *sram);

#endif

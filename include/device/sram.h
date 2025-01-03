#ifndef SRAM_H
#define SRAM_H

#include "device/device.h"

struct Sram {
    u8 *data;
    u32 len;
};

void sram_init(struct Sram *sram, u8 *data, u32 len);

struct DeviceFunc sram_get_func(struct Sram *sram);

#endif

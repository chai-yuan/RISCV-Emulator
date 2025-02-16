#ifndef DEVICE_H
#define DEVICE_H

#include "core/riscv.h"
#include "types.h"

typedef enum exception (*device_read_func_t)(void *context, u64 address, u8 size, usize *data);
typedef enum exception (*device_write_func_t)(void *context, u64 address, u8 size, usize data);
typedef void (*update_func_t)(void *context, u32 interval);
struct DeviceFunc {
    void               *context;
    device_read_func_t  read;
    device_write_func_t write;
    update_func_t       update;
};

void device_read(u8 *buf, u64 address, u8 size, usize *data);

void device_write(u8 *buf, u64 address, u8 size, usize data);

#endif

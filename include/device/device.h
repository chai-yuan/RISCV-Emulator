#ifndef DEVICE_H
#define DEVICE_H

#include "core/riscv.h"
#include "types.h"

#define REG64(base, offset) *(volatile u64 *)((base) + (offset))
#define REG32(base, offset) *(volatile u32 *)((base) + (offset))
#define REG16(base, offset) *(volatile u16 *)((base) + (offset))
#define REG8(base, offset) *(volatile u8 *)((base) + (offset))

typedef enum exception (*device_read_func_t)(void *context, u64 address, u8 size, usize *data);
typedef enum exception (*device_write_func_t)(void *context, u64 address, u8 size, usize data);
typedef void (*update_func_t)(void *context, u32 interval);
typedef bool (*check_external_interrupt_func_t)(void *context);
typedef bool (*check_timer_interrupt_func_t)(void *context);

struct DeviceFunc {
    void                           *context;
    device_read_func_t              read;
    device_write_func_t             write;
    update_func_t                   update;
    check_external_interrupt_func_t check_external_interrupt;
    check_timer_interrupt_func_t    check_timer_interrupt;
};

void device_read(u8 *buf, u64 address, u8 size, usize *data);

void device_write(u8 *buf, u64 address, u8 size, usize data);

#endif

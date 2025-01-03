#ifndef DEVICE_H
#define DEVICE_H

#include "core/riscv.h"
#include "types.h"

#define REG64(base, offset) *(volatile u64 *)((base) + (offset))
#define REG32(base, offset) *(volatile u32 *)((base) + (offset))
#define REG16(base, offset) *(volatile u16 *)((base) + (offset))
#define REG8(base, offset) *(volatile u8 *)((base) + (offset))

typedef u8 *(*get_buffer_func_t)(void *context, usize address);
typedef enum exception (*device_handle_func_t)(void *context, usize address, u8 size, bool write);
typedef void (*update_func_t)(void *context, u32 interval);
typedef bool (*check_external_interrupt_func_t)(void *context);
typedef bool (*check_timer_interrupt_func_t)(void *context);

struct DeviceFunc {
    void                           *context;
    get_buffer_func_t               get_buffer;
    device_handle_func_t            handle;
    update_func_t                   update;
    check_external_interrupt_func_t check_external_interrupt;
    check_timer_interrupt_func_t    check_timer_interrupt;
};

#endif

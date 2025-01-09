#include "device/sram.h"
#include "debug.h"

void sram_init(struct Sram *sram, u8 *data, u32 len) {
    sram->data = data;
    sram->len  = len;
}

static enum exception sram_read(void *context, u64 address, u8 size, usize *data) {
    struct Sram *sram = (struct Sram *)context;
    if (sram->len < address + size)
        return LOAD_ACCESS_FAULT;

    device_read(sram->data, address, size, data);
    return EXC_NONE;
}

static enum exception sram_write(void *context, u64 address, u8 size, usize data) {
    struct Sram *sram = (struct Sram *)context;

    if (sram->len < address + size)
        return STORE_AMO_ACCESS_FAULT;

    device_write(sram->data, address, size, data);
    return EXC_NONE;
}

struct DeviceFunc sram_get_func(struct Sram *sram) {
    return (struct DeviceFunc){
        .context                  = sram,
        .read                     = sram_read,
        .write                    = sram_write,
        .update                   = NULL,
        .check_timer_interrupt    = NULL,
        .check_external_interrupt = NULL,
    };
}

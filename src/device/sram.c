#include "device/sram.h"
#include "debug.h"

void sram_init(struct Sram *sram, u8 *data, u32 len) {
    sram->data = data;
    sram->len  = len;
}

static u8 *sram_get_buffer(void *context, usize address) {
    struct Sram *sram = (struct Sram *)context;
    if (address >= sram->len) {
        ERROR("address out of bounds");
        return NULL;
    }
    return &sram->data[address];
}

static enum exception sram_handle(void *context, usize address, u8 size, bool write) {
    return EXC_NONE;
}

struct DeviceFunc sram_get_func(struct Sram *sram) {
    return (struct DeviceFunc){
        .context                  = sram,
        .get_buffer               = sram_get_buffer,
        .handle                   = sram_handle,
        .update                   = NULL,
        .check_timer_interrupt    = NULL,
        .check_external_interrupt = NULL,
    };
}

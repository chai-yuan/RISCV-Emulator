#include "device/sram.h"

void sram_init(struct Sram *sram, void *data, u32 len) {
    sram->data = data;
    sram->len  = len;
}

enum exception sram_read(void *context, u64 address, u8 size, u64 *value) {
    struct Sram *sram = (struct Sram *)context;
    if (address + size > sram->len) {
        ERROR("address out of bounds");
        return LOAD_ADDRESS_MISALIGNED;
    }
    return read_buffer(sram->data, address, size, value);
}

enum exception sram_write(void *context, u64 address, u8 size, u64 value) {
    struct Sram *sram = (struct Sram *)context;
    if (address + size > sram->len) {
        ERROR("address out of bounds");
        return STORE_AMO_ADDRESS_MISALIGNED;
    }
    return write_buffer(sram->data, address, size, value);
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

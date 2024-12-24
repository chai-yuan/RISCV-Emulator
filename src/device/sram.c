#include "device/sram.h"

void sram_init(struct Sram *sram, void *data, u32 len) {
    sram->data = data;
    sram->len = len;
}

void sram_read(void *context, u64 address, u8 size, u64 *value) {
    struct Sram *sram = (struct Sram *)context;
    if (address + size > sram->len) {
        ERROR("address out of bounds");
        return;
    }
    switch (size) {
    case 1:
        *value = *(u8 *)(sram->data + address);
        break;
    case 2:
        *value = *(u16 *)(sram->data + address);
        break;
    case 4:
        *value = *(u32 *)(sram->data + address);
        break;
    case 8:
        *value = *(u64 *)(sram->data + address);
        break;
    default:
        ERROR("unsupported size");
        break;
    }
}

void sram_write(void *context, u64 address, u8 size, u64 value) {
    struct Sram *sram = (struct Sram *)context;
    if (address + size > sram->len) {
        ERROR("address out of bounds");
        return;
    }
    switch (size) {
    case 1:
        *(u8 *)(sram->data + address) = (u8)value;
        break;
    case 2:
        *(u16 *)(sram->data + address) = (u16)value;
        break;
    case 4:
        *(u32 *)(sram->data + address) = (u32)value;
        break;
    case 8:
        *(u64 *)(sram->data + address) = (u64)value;
        break;
    default:
        ERROR("unsupported size");
        break;
    }
}

struct DeviceFunc sram_get_func(struct Sram *sram) {
    return (struct DeviceFunc){
        .context = sram,
        .read = sram_read,
        .write = sram_write,
        .check_timer_interrupt = NULL,
        .check_external_interrupt = NULL,
    };
}

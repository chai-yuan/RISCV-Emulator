#include "device/device.h"
#include "debug.h"

void device_read(u8 *buf, u64 address, u8 size, usize *data) {
    *data = 0;
    switch (size) {
    case 1:
        *data = REG8(buf, address);
        break;
    case 2:
        *data = REG16(buf, address);
        break;
    case 4:
        *data = REG32(buf, address);
        break;
    case 8:
        *data = REG64(buf, address);
        break;
    default:
        ERROR("error size %d", size);
    }
}

void device_write(u8 *buf, u64 address, u8 size, usize data) {
    switch (size) {
    case 1:
        REG8(buf, address) = data;
        break;
    case 2:
        REG16(buf, address) = data;
        break;
    case 4:
        REG32(buf, address) = data;
        break;
    case 8:
        REG64(buf, address) = data;
        break;
    default:
        ERROR("error size %d", size);
    }
}

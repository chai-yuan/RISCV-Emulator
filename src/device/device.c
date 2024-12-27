#include "device/device.h"

enum exception read_buffer(u8 *data, u64 address, u8 size, u64 *value) {
    switch (size) {
    case 1:
        *value = REG8(data, address);
        break;
    case 2:
        *value = REG16(data, address);
        break;
    case 4:
        *value = REG32(data, address);
        break;
    case 8:
        *value = REG64(data, address);
        break;
    default:
        ERROR("unsupported size");
        return LOAD_ACCESS_FAULT;
    }
    return EXC_NONE;
}

enum exception write_buffer(u8 *data, u64 address, u8 size, u64 value) {
    switch (size) {
    case 1:
        REG8(data, address) = (u8)value;
        break;
    case 2:
        REG16(data, address) = (u16)value;
        break;
    case 4:
        REG32(data, address) = (u32)value;
        break;
    case 8:
        REG64(data, address) = (u64)value;
        break;
    default:
        ERROR("unsupported size");
        return STORE_AMO_ACCESS_FAULT;
    }
    return EXC_NONE;
}

#include "common/debug.h"
#include "device/device.h"
#include <device/serial.h>
#include <stdint.h>
#include <stdio.h>

DeviceAccessStatus serial_read(uint64_t addr, uint8_t size, uint64_t *data) {
    *data = 0;
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus serial_write(uint64_t addr, uint8_t size, uint64_t data) {
    switch (addr) {
    case 0x0: {
        printf("%c", (uint8_t)data);
        break;
    }
    default: {
        warning("未知地址 %x", (uint32_t)addr);
    }
    }
    return DEVICE_ACCESS_OK;
}

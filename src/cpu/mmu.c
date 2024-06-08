#include "common/debug.h"
#include "device/device.h"
#include <bus/bus.h>
#include <cpu/mmu.h>
#include <stdint.h>

DeviceAccessStatus mmu_read(uint64_t addr, uint8_t size, uint64_t *data) {
    uint64_t read_data;
    DeviceAccessStatus ret = bus_read(addr, size, &read_data);
    switch (size) {
    case 1:
        *data = read_data & 0xff;
        break;
    case 2:
        *data = read_data & 0xffff;
        break;
    case 4:
        *data = read_data & 0xffffffff;
        break;
    case 8:
        *data = read_data;
        break;
    default:
        panic("未知读取长度");
    }
    return ret;
}

DeviceAccessStatus mmu_write(uint64_t addr, uint8_t size, uint64_t data) {
    return bus_write(addr, size, data);
}

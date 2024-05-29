#include "bus/bus.h"
#include <cpu/mmu.h>
#include <stdint.h>

DeviceAccessStatus mmu_read_32(uint32_t addr, uint8_t size, uint32_t *data) {
    uint64_t read_data;
    DeviceAccessStatus ret = bus_read(addr, size, &read_data);
    *data = read_data;
    return ret;
}

DeviceAccessStatus mmu_write_32(uint32_t addr, uint8_t size, uint32_t data) {
    return bus_write(addr, size, data);
}

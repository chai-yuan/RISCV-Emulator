#include <bus/bus.h>
#include <cpu/mmu.h>
#include <stdint.h>

DeviceAccessStatus mmu_read(uint64_t addr, uint8_t size, uint64_t *data) {
    return bus_read(addr, size, data);
}

DeviceAccessStatus mmu_write(uint64_t addr, uint8_t size, uint64_t data) {
    return bus_write(addr, size, data);
}

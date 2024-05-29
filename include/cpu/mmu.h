#ifndef MMU_H
#define MMU_H

#include <bus/bus.h>

DeviceAccessStatus mmu_read_32(uint32_t addr, uint8_t size, uint32_t *data);

DeviceAccessStatus mmu_write_32(uint32_t addr, uint8_t size, uint32_t data);

#endif // !MMU_H

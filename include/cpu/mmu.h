#ifndef MMU_H
#define MMU_H

#include <bus/bus.h>

DeviceAccessStatus mmu_read(uint64_t addr, uint8_t size, uint64_t *data);

DeviceAccessStatus mmu_write(uint64_t addr, uint8_t size, uint64_t data);

#endif // !MMU_H

#ifndef ROM_H
#define ROM_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

void rom_init(size_t sram_size, const uint8_t *data, size_t size);

void rom_init_file(size_t sram_size, const char *file_name);

DeviceAccessStatus rom_read(uint64_t addr, uint8_t size, uint64_t *data);

#endif // !ROM_H

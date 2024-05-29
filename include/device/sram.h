#ifndef SRAM_H
#define SRAM_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

void sram_init(size_t sram_size, const uint8_t *data, size_t size);

void sram_init_file(size_t sram_size, const char *file_name);

DeviceAccessStatus sram_read(uint64_t addr, uint8_t size, uint64_t *data);

DeviceAccessStatus sram_write(uint64_t addr, uint8_t size, uint64_t data);

#endif // !SRAM_H

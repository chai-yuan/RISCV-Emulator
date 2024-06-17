#ifndef ram_H
#define ram_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

typedef struct RAM {
    uint8_t *data;
    uint64_t size;
} RAM;

extern DeviceInterface ram_func;

RAM *ram_init(size_t ram_size, const uint8_t *data, size_t size);

RAM *ram_init_file(size_t ram_size, const char *file_name);

DeviceAccessStatus ram_read(void *device, uint64_t addr, uint8_t size,
                            uint64_t *data);

DeviceAccessStatus ram_write(void *device, uint64_t addr, uint8_t size,
                             uint64_t data);

#endif // !ram_H

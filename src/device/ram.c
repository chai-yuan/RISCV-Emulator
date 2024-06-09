#include "common/debug.h"
#include "device/device.h"
#include <device/ram.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

DeviceInterface ram_func = {
    .read = ram_read,
    .write = ram_write,
    .update = NULL,
    .check_intr = NULL,
};

RAM *ram_init(size_t ram_size, const uint8_t *data, size_t size) {
    RAM *ram = malloc(sizeof(RAM));

    uint8_t *ram_mem = malloc(ram_size);
    memcpy(ram_mem, data, size);

    ram->data = ram_mem;
    ram->size = ram_size;

    return ram;
}

RAM *ram_init_file(size_t ram_size, const char *file_name) {
    RAM *ram = malloc(sizeof(RAM));

    uint8_t *ram_mem = malloc(ram_size);
    FILE *file;
    file = fopen(file_name, "rb");
    if (file == NULL) {
        panic("无法打开文件");
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file) / sizeof(int);
    fseek(file, 0, SEEK_SET);
    if (size > ram_size) {
        panic("文件过大");
    }
    fread(ram_mem, sizeof(int), size, file);
    fclose(file);

    ram->data = ram_mem;
    ram->size = ram_size;

    return ram;
}

DeviceAccessStatus ram_read(void *device, uint64_t addr, uint8_t size,
                            uint64_t *data) {
    RAM *ram = (RAM *)device;

    if (ram->size < addr + size) {
        return DEVICE_ACCESS_ERROR;
    }

    switch (size) {
    case 1:
    case 2:
    case 4:
    case 8: {
        *data = *((uint64_t *)(ram->data + addr));
        break;
    }
    default: {
        panic("错误读取长度");
    }
    }
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus ram_write(void *device, uint64_t addr, uint8_t size,
                             uint64_t data) {
    RAM *ram = (RAM *)device;

    if (ram->size <= addr) {
        return DEVICE_ACCESS_ERROR;
    }

    uint8_t *ram_mem = ram->data;
    switch (size) {
    case 1: {
        *((uint8_t *)(ram_mem + addr)) = (uint8_t)data;
        break;
    }
    case 2: {
        *((uint16_t *)(ram_mem + addr)) = (uint16_t)data;
        break;
    }
    case 4: {
        *((uint32_t *)(ram_mem + addr)) = (uint32_t)data;
        break;
    }
    case 8: {
        *((uint64_t *)(ram_mem + addr)) = (uint64_t)data;
        break;
    }
    default: {
        panic("错误写入长度");
    }
    }

    return DEVICE_ACCESS_OK;
}

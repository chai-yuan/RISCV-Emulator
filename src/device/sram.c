#include "common/debug.h"
#include "device/device.h"
#include <device/sram.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint8_t *sram_mem = NULL;

void sram_init(size_t sram_size, const uint8_t *data, size_t size) {
    sram_mem = malloc(sram_size);
    memcpy(sram_mem, data, size);
}

void sram_init_file(size_t sram_size, const char *file_name) {
    sram_mem = malloc(sram_size);
    FILE *file;

    file = fopen(file_name, "rb");
    if (file == NULL) {
        panic("无法打开文件");
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file) / sizeof(int);
    fseek(file, 0, SEEK_SET);
    if (size > sram_size) {
        panic("文件过大");
    }

    fread(sram_mem, sizeof(int), size, file);
    fclose(file);
}

DeviceAccessStatus sram_read(uint64_t addr, uint8_t size, uint64_t *data) {
    switch (size) {
    case 1: {
        *data = *((uint64_t *)(sram_mem + addr)) & 0xff;
        break;
    }
    case 2: {
        *data = *((uint64_t *)(sram_mem + addr)) & 0xffff;
        break;
    }
    case 4: {
        *data = *((uint64_t *)(sram_mem + addr)) & 0xffffffff;
        break;
    }
    default: {
        panic("错误读取长度");
    }
    }
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus sram_write(uint64_t addr, uint8_t size, uint64_t data) {
    switch (size) {
    case 1: {
        *((uint8_t *)(sram_mem + addr)) = (uint8_t)data;
        break;
    }
    case 2: {
        *((uint16_t *)(sram_mem + addr)) = (uint16_t)data;
        break;
    }
    case 4: {
        *((uint32_t *)(sram_mem + addr)) = (uint32_t)data;
        break;
    }
    case 8: {
        *((uint64_t *)(sram_mem + addr)) = (uint64_t)data;
        break;
    }
    default: {
        panic("错误写入长度");
    }
    }
    return DEVICE_ACCESS_OK;
}

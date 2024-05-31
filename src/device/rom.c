#include <device/rom.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint8_t *rom_mem = NULL;

void rom_init(size_t rom_size, const uint8_t *data, size_t size) {
    rom_mem = malloc(rom_size);
    memcpy(rom_mem, data, size);
}

void rom_init_file(size_t rom_size, const char *file_name) {
    rom_mem = malloc(rom_size);
    FILE *file;

    file = fopen(file_name, "rb");
    if (file == NULL) {
        panic("无法打开文件");
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file) / sizeof(int);
    fseek(file, 0, SEEK_SET);
    if (size > rom_size) {
        panic("文件过大");
    }

    fread(rom_mem, sizeof(int), size, file);
    fclose(file);
}

DeviceAccessStatus rom_read(uint64_t addr, uint8_t size, uint64_t *data) {
    switch (size) {
    case 1: {
        *data = *((uint64_t *)(rom_mem + addr)) & 0xff;
        break;
    }
    case 2: {
        *data = *((uint64_t *)(rom_mem + addr)) & 0xffff;
        break;
    }
    case 4: {
        *data = *((uint64_t *)(rom_mem + addr)) & 0xffffffff;
        break;
    }
    default: {
        panic("错误读取长度");
    }
    }
    return DEVICE_ACCESS_OK;
}

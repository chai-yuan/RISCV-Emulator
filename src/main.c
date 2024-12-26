#define ENABLE_DEBUG_MACROS 1 // 默认启用调试宏

#include "common.h"
#include "core/rvcore64.h"
#include "device/bus.h"
#include "device/device.h"
#include "device/sram.h"
#include "string.h"

void *load_binary(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *data = malloc(*size);
    if (!data) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    if (fread(data, 1, *size, file) != *size) {
        perror("Failed to read file");
        free(data);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return data;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return 1;
    }

    const char *binary_file = argv[1];
    size_t binary_size;
    void *binary_data = load_binary(binary_file, &binary_size);
    if (!binary_data) {
        return 1;
    }

    // 初始化 SRAM
    char *sram_data = malloc(128 * 1024 * 1024);
    memcpy(sram_data, binary_data, binary_size);
    free(binary_data);
    struct Sram sram;
    sram_init(&sram, sram_data, 128 * 1024 * 1024);

    // 初始化总线
    struct BusDevice bus;
    bus_device_init(&bus);
    bus_device_add_sub_device(&bus, 0x80000000, 128 * 1024 * 1024, sram_get_func(&sram));
    // 初始化 RISC-V 核心
    struct RiscvCore64 core;
    riscvcore64_init(&core, bus_device_get_func(&bus));
    // 运行模拟器
    while (riscvcore64_check_halt(&core) == false) {
        riscvcore64_step(&core);
    }
    return core.regs[10] != 0;
}

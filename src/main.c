#define ENABLE_DEBUG_MACROS 1 // 默认启用调试宏

#include "core/rvcore.h"
#include "core/rvcore64.h"
#include "device/bus.h"
#include "device/sram.h"
#include "machine/simple.h"
#include "string.h"
#include "types.h"

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
    // 初始化机器并运行
    struct SimpleMachine *machine = malloc(sizeof(struct SimpleMachine));
    simple_machine_init(machine, binary_data, binary_size);

    simple_machine_run(machine);
    return machine->core.regs[10] != 0;
}

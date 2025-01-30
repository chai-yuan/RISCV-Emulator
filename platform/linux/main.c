#include "machine/qemu.h"
#include "string.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

bool get_char(u8 *data) {
    char c;
    if (read(STDIN_FILENO, &c, 1) > 0) {
        *data = (u8)c;
        return true;
    }
    return false;
}

void put_char(u8 data) {
    char c = (char)data;
    printf("%c", c);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return 1;
    }

    // 设置 STDIN_FILENO 为非阻塞模式
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    const char *binary_file = argv[1];
    size_t      binary_size;
    void       *binary_data = load_binary(binary_file, &binary_size);
    if (!binary_data) {
        return 1;
    }

    const u64 memory_size = 128 * 1024 * 1024;
    u8       *memory      = malloc(memory_size);
    memcpy(memory, binary_data, binary_size);
    free(binary_data);
    // 初始化机器并运行
    struct QemuMachine *machine = malloc(sizeof(struct QemuMachine));
    qemu_machine_init(machine, (struct QemuPortableOperations){
                                   .sram_data = memory,
                                   .sram_size = memory_size,
                                   .get_char  = get_char,
                                   .put_char  = put_char,
                               });

    while (1) {
        qemu_machine_step(machine);
    }
    return 0;
}

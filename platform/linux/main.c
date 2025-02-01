#include "linux_plat.h"
#include "machine/qemu.h"
#include "parse_args.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    Config config;
    parse_args(argc, argv, &config);
    init();

    size_t    binary_size;
    void     *binary_data = load_binary(config.bin, &binary_size);
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
    while (config.step--) {
        qemu_machine_step(machine);
    }
    return 0;
}

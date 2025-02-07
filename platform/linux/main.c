#include "debug.h"
#include "linux_plat.h"
#include "machine/nemu.h"
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
    const u64 memory_size = 16 * 1024 * 1024;
    u8       *memory      = malloc(memory_size);
    memcpy(memory, binary_data, binary_size);
    free(binary_data);

    int ret_val = 0;
    if (strcmp(config.machine, "qemu") == 0) {
        INFO("qemu start!");
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
    } else if (strcmp(config.machine, "nemu") == 0) {
        INFO("nemu start!");
        struct NemuMachine *machine = malloc(sizeof(struct NemuMachine));
        nemu_machine_init(machine, (struct NemuPortableOperations){
                                       .sram_data = memory,
                                       .sram_size = memory_size,
                                       .get_char  = get_char,
                                       .put_char  = put_char,
                                   });
        while (config.step-- && machine->core.decode.exception != BREAKPOINT) {
            nemu_machine_step(machine);
        }
        ret_val = machine->core.regs[10];
    }

    ASSERT(ret_val == 0, "ret_val : %d", ret_val);
    return ret_val;
}

#include "debug.h"
#include "machine/qemu.h"
#include "string.h"

u8                 sram_data[128 * 1024 * 1024];
struct QemuMachine machine;

struct RiscvCore *difftest_init(const u8 *data, u64 data_size) {
    INFO("difftest_init");
    memcpy(sram_data, data, data_size);
    INFO("load data size : %lld", data_size);
    qemu_machine_init(&machine, (struct QemuPortableOperations){
                                    .sram_data = sram_data,
                                    .sram_size = 128 * 1024 * 1024,
                                    .get_char  = NULL,
                                    .put_char  = NULL,
                                });

    return &machine.core;
}

void difftest_step(void) { riscvcore_step(&machine.core); }

void difftest_raise_irq(void) {
    riscvcore_raise_irq(&machine.core, 1);
}

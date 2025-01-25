#ifndef QEMU_64_H
#define QEMU_64_H

#include "core/rvcore.h"
#include "device/clint.h"
#include "device/plic.h"
#include "device/sram.h"
#include "device/bus.h"
#include "device/uart.h"

// 模拟兼容qemu rv64的机器实现
struct QemuMachine {
    struct RiscvCore core;
    struct BusDevice bus;
    struct Sram      sram;
    struct Uart      uart;
    struct CLINT     clint;
    struct PLIC      plic;
};

struct QemuPortableOperations {
    u8             *sram_data;
    u64             sram_size;
    get_char_func_t get_char;
    put_char_func_t put_char;
};

void qemu_machine_init(struct QemuMachine *machine, struct QemuPortableOperations init);

void qemu_machine_run(struct QemuMachine *machine);

#endif

#ifndef QEMU_64_H
#define QEMU_64_H

#include "core/rvcore64.h"
#include "device/bus.h"
#include "device/clint.h"
#include "device/sram.h"
#include "device/uart.h"

// 模拟兼容qemu rv64的机器实现
struct Qemu64Machine {
    struct RiscvCore64 core;
    struct BusDevice   bus;
    struct Sram        sram;
    struct Uart        uart;
    struct CLINT       clint;
};

struct Qemu64PortableOperations {
    u8             *sram_data;
    u64             sram_size;
    get_char_func_t get_char;
    put_char_func_t put_char;
};

void qemu64_machine_init(struct Qemu64Machine *machine, struct Qemu64PortableOperations init);

void qemu64_machine_run(struct Qemu64Machine *machine);

#endif

#ifndef NEMU_M_H
#define NEMU_M_H

#include "core/rvcore.h"
#include "device/clint.h"
#include "device/sram.h"
#include "device/uart.h"

struct NemuMachine {
    struct RiscvCore core;
    struct Sram      sram;
    struct Uart      uart;
    struct CLINT     clint;
};

struct NemuPortableOperations {
    u8             *sram_data;
    u64             sram_size;
    get_char_func_t get_char;
    put_char_func_t put_char;
};

void nemu_machine_init(struct NemuMachine *machine, struct NemuPortableOperations init);

void nemu_machine_run(struct NemuMachine *machine);

#endif

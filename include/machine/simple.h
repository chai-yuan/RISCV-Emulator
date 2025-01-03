#ifndef SIMPLE_M_H
#define SIMPLE_M_H

#include "core/rvcore.h"
#include "device/bus.h"
#include "device/clint.h"
#include "device/sram.h"
#include "device/uart.h"

struct SimpleMachine {
    struct RiscvCore core;
    struct BusDevice bus;
    struct Sram      sram;
    struct Uart      uart;
    struct CLINT     clint;
};

struct SimplePortableOperations {
    u8             *sram_data;
    u64             sram_size;
    get_char_func_t get_char;
    put_char_func_t put_char;
};

void simple_machine_init(struct SimpleMachine *machine, struct SimplePortableOperations init);

void simple_machine_run(struct SimpleMachine *machine);

#endif

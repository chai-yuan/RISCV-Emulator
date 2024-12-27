#ifndef SIMPLE_M_H
#define SIMPLE_M_H

#include "core/rvcore64.h"
#include "device/bus.h"
#include "device/clint.h"
#include "device/sram.h"
#include "device/uart.h"

#define SIMPLE_MEM_SIZE 8 * 1024 * 1024

struct SimpleMachine {
    struct RiscvCore64 core;
    struct BusDevice   bus;
    u8                 sram_data[SIMPLE_MEM_SIZE];
    struct Sram        sram;
    struct Uart        uart;
    struct CLINT       clint;
};

void simple_machine_init(struct SimpleMachine *machine, const u8 *mem_init_data,
                         const u32 mem_data_size, get_char_func_t get_char,
                         put_char_func_t put_char);

void simple_machine_run(struct SimpleMachine *machine);

#endif

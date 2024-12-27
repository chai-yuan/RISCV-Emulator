#ifndef NEMU_M_H
#define NEMU_M_H

#include "core/rvcore64.h"
#include "device/bus.h"
#include "device/sram.h"
#include "device/uart.h"

#define NEMU_MEM_SIZE 128 * 1024 * 1024

// 兼容nemu的机器实现
struct NemuMachine {
    struct RiscvCore64 core;
    struct BusDevice   bus;
    u8                 sram_data[NEMU_MEM_SIZE];
    struct Sram        sram;
    struct Uart        uart;
};

void nemu_machine_init(struct NemuMachine *machine, const u8 *mem_init_data,
                       const u32 mem_data_size, put_char_func_t put_char);

void nemu_machine_run(struct NemuMachine *machine);

#endif

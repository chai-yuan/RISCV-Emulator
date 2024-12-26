#ifndef SIMPLE_M_H
#define SIMPLE_M_H

#include "core/rvcore64.h"
#include "device/bus.h"
#include "device/sram.h"

#define SimpleMachine_MEM_SIZE 128 * 1024 * 1024

struct SimpleMachine {
    struct RiscvCore64 core;
    struct BusDevice bus;
    u8 sram_data[SimpleMachine_MEM_SIZE];
    struct Sram sram;
};

void simple_machine_init(struct SimpleMachine *machine, const u8 *mem_init_data,
                         const u32 mem_data_size);

void simple_machine_run(struct SimpleMachine *machine);

#endif

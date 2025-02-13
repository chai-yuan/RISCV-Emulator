#ifndef SPIKE_H
#define SPIKE_H

#include "core/rvcore.h"
#include "device/bus.h"
#include "device/clint.h"
#include "device/plic.h"
#include "device/sram.h"
#include "device/uart16550.h"

// 模拟兼容spike的机器实现
struct SpikeMachine {
    struct RiscvCore core;
    struct BusDevice bus;
    struct Sram      sram;
    struct Sram      rom;
    struct UART16550 uart;
    struct CLINT     clint;
    struct PLIC      plic;
};

struct SpikePortableOperations {
    u8             *sram_data;
    u64             sram_size;
    get_char_func_t get_char;
    put_char_func_t put_char;
};

void spike_machine_init(struct SpikeMachine *machine, struct SpikePortableOperations init);

void spike_machine_step(struct SpikeMachine *machine);

#endif

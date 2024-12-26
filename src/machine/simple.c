#include "machine/simple.h"

void simple_machine_init(struct SimpleMachine *machine, const u8 *mem_init_data,
                         const u32 mem_data_size) {
    bus_device_init(&machine->bus);

    for (u32 i = 0; i < mem_data_size; i++)
        machine->sram_data[i] = mem_init_data[i];
    sram_init(&machine->sram, machine->sram_data, SimpleMachine_MEM_SIZE);

    bus_device_add_sub_device(&machine->bus, 0x80000000, SimpleMachine_MEM_SIZE,
                              sram_get_func(&machine->sram));
    riscvcore64_init(&machine->core, bus_device_get_func(&machine->bus));
}

void simple_machine_run(struct SimpleMachine *machine) {
    struct CoreFunc func = riscvcore64_get_func(&machine->core);

    while (func.check_halt(&machine->core) == false) {
        func.step(&machine->core);
    }
}

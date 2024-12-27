#include "machine/nemu.h"

void nemu_machine_init(struct NemuMachine *machine, const u8 *mem_init_data,
                       const u32 mem_data_size, put_char_func_t put_char) {
    bus_device_init(&machine->bus);

    for (u32 i = 0; i < mem_data_size; i++)
        machine->sram_data[i] = mem_init_data[i];
    sram_init(&machine->sram, machine->sram_data, NEMU_MEM_SIZE);
    uart_init(&machine->uart, NULL, put_char);

    bus_device_add_sub_device(&machine->bus, 0x80000000, NEMU_MEM_SIZE,
                              sram_get_func(&machine->sram));
    bus_device_add_sub_device(&machine->bus, 0xa00003f8, UART_SIZE, uart_get_func(&machine->uart));
    riscvcore64_init(&machine->core, bus_device_get_func(&machine->bus));
}

void nemu_machine_run(struct NemuMachine *machine) {
    struct CoreFunc func = riscvcore64_get_func(&machine->core);

    while (func.check_halt(&machine->core) == false) {
        func.step(&machine->core);
    }
}

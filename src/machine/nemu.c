#include "machine/nemu.h"

void nemu_machine_init(struct NemuMachine *machine, struct NemuPortableOperations init) {
    bus_device_init(&machine->bus);
    sram_init(&machine->sram, init.sram_data, init.sram_size);
    uart_init(&machine->uart, init.get_char, init.put_char);

    bus_device_add(&machine->bus, 0x80000000, machine->sram.len, sram_get_func(&machine->sram));
    bus_device_add(&machine->bus, 0x10000000, UART_SIZE, uart_get_func(&machine->uart));

    riscvcore_init(&machine->core, bus_device_get_func(&machine->bus));
    machine->core.pc = 0x80000000;
}

void nemu_machine_step(struct NemuMachine *machine) {
    riscvcore_step(&machine->core, (struct RiscvEnvInfo){.eint = false});
}

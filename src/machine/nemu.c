#include "machine/nemu.h"

void nemu_machine_init(struct NemuMachine *machine, struct NemuPortableOperations init) {
    sram_init(&machine->sram, init.sram_data, init.sram_size);
    bus_device_add_sub_device(&machine->bus, 0x80000000, init.sram_size,
                              sram_get_func(&machine->sram));

    uart_init(&machine->uart,
              (struct InterruptFunc){.context = NULL, .interrupt_num = 10, .raise_irq = NULL},
              init.get_char, init.put_char);
    bus_device_add_sub_device(&machine->bus, 0x10000000, UART_SIZE, uart_get_func(&machine->uart));

    clint_init(&machine->clint);
    bus_device_add_sub_device(&machine->bus, 0x02000000, CLINT_SIZE,
                              clint_get_func(&machine->clint));

    riscvcore_init(&machine->core, bus_device_get_func(&machine->bus));
}

void nemu_machine_run(struct NemuMachine *machine) {
    struct DeviceFunc bus = bus_device_get_func(&machine->bus);
    while (true) {
        riscvcore_step(&machine->core);
        bus.update(bus.context, 1);
    }
}

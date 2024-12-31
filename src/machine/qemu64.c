#include "machine/qemu64.h"

void qemu64_machine_init(struct Qemu64Machine *machine, struct Qemu64PortableOperations init) {
    bus_device_init(&machine->bus);

    sram_init(&machine->sram, init.sram_data, init.sram_size);
    uart_init(&machine->uart, init.get_char, init.put_char);
    clint_init(&machine->clint);

    bus_device_add_sub_device(&machine->bus, 0x02000000, CLINT_SIZE,
                              clint_get_func(&machine->clint));
    bus_device_add_sub_device(&machine->bus, 0x10000000, UART_SIZE, uart_get_func(&machine->uart));
    bus_device_add_sub_device(&machine->bus, 0x80000000, init.sram_size,
                              sram_get_func(&machine->sram));
    riscvcore64_init(&machine->core, bus_device_get_func(&machine->bus));
}

void qemu64_machine_run(struct Qemu64Machine *machine) {
    struct CoreFunc   core     = riscvcore64_get_func(&machine->core);
    struct DeviceFunc bus      = bus_device_get_func(&machine->bus);
    static u64        step_cnt = 0;

    while (core.check_halt(core.context) == false) {
        core.step(core.context);
        if ((step_cnt++) % 10) {
            bus.update(bus.context, 10);
        }
    }
}

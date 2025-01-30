#include "machine/qemu.h"

void qemu_machine_init(struct QemuMachine *machine, struct QemuPortableOperations init) {
    bus_device_init(&machine->bus);
    sram_init(&machine->sram, init.sram_data, init.sram_size);
    uart_init(&machine->uart, init.get_char, init.put_char);
    plic_init(&machine->plic);
    clint_init(&machine->clint);

    bus_device_add(&machine->bus, 0x80000000, init.sram_size, sram_get_func(&machine->sram));
    bus_device_add(&machine->bus, 0x10000000, UART_SIZE, uart_get_func(&machine->uart));
    bus_device_add(&machine->bus, 0x0C000000, PLIC_SIZE, plic_get_func(&machine->plic));
    bus_device_add(&machine->bus, 0x02000000, CLINT_SIZE, clint_get_func(&machine->clint));

    riscvcore_init(&machine->core, bus_device_get_func(&machine->bus));
}

void qemu_machine_step(struct QemuMachine *machine) {
    struct DeviceFunc bus = bus_device_get_func(&machine->bus);
    bus.update(bus.context, 1);

    plic_update_intterupt(&machine->plic, uart_check_irq(&machine->uart), 10);

    riscvcore_step(&machine->core,
                   (struct RiscvEnvInfo){.eint = plic_check_irq(&machine->plic, 1)});
}

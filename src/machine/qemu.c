#include "machine/qemu.h"

void qemu_machine_init(struct QemuMachine *machine, struct QemuPortableOperations init) {
    sram_init(&machine->sram, init.sram_data, init.sram_size);
    bus_device_add_sub_device(&machine->bus, 0x80000000, init.sram_size,
                              sram_get_func(&machine->sram));

    uart_init(&machine->uart,
              (struct InterruptFunc){
                  .context = &machine->plic, .interrupt_num = 10, .raise_irq = plic_raise_irq},
              init.get_char, init.put_char);
    bus_device_add_sub_device(&machine->bus, 0x10000000, UART_SIZE, uart_get_func(&machine->uart));

    plic_init(&machine->plic, (struct InterruptFunc){.context       = &machine->core,
                                                     .interrupt_num = 1,
                                                     .raise_irq     = riscvcore_raise_irq});
    bus_device_add_sub_device(&machine->bus, 0x0C000000, PLIC_SIZE, plic_get_func(&machine->plic));

    clint_init(&machine->clint);
    bus_device_add_sub_device(&machine->bus, 0x02000000, CLINT_SIZE,
                              clint_get_func(&machine->clint));

    riscvcore_init(&machine->core, bus_device_get_func(&machine->bus));
}

void qemu_machine_run(struct QemuMachine *machine) {
    struct DeviceFunc bus = bus_device_get_func(&machine->bus);
    while (true) {
        riscvcore_step(&machine->core);
        bus.update(bus.context, 1);
    }
}

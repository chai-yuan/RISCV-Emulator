#include "machine/simple.h"

void simple_machine_init(struct SimpleMachine *machine, const u8 *mem_init_data,
                         const u32 mem_data_size, get_char_func_t get_char,
                         put_char_func_t put_char) {
    bus_device_init(&machine->bus);

    for (u32 i = 0; i < mem_data_size; i++)
        machine->sram_data[i] = mem_init_data[i];
    sram_init(&machine->sram, machine->sram_data, SIMPLE_MEM_SIZE);
    uart_init(&machine->uart, get_char, put_char);
    clint_init(&machine->clint);

    bus_device_add_sub_device(&machine->bus, 0x02000000, CLINT_SIZE,
                              clint_get_func(&machine->clint));
    bus_device_add_sub_device(&machine->bus, 0x10000000, UART_SIZE, uart_get_func(&machine->uart));
    bus_device_add_sub_device(&machine->bus, 0x80000000, SIMPLE_MEM_SIZE,
                              sram_get_func(&machine->sram));
    riscvcore64_init(&machine->core, bus_device_get_func(&machine->bus));
}

void simple_machine_run(struct SimpleMachine *machine) {
    struct DeviceFunc bus      = bus_device_get_func(&machine->bus);
    static u64        step_cnt = 0;

    while (machine->core.halt == false) {
        riscvcore64_step(&machine->core);
        if ((step_cnt++) % 10) {
            bus.update(bus.context, 10);
        }
    }
}

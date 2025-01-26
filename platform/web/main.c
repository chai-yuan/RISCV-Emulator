#include "machine/qemu.h"
#include <emscripten.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define memory_size 16 * 1024 * 1024
u8                 memory[memory_size];
struct QemuMachine machine;

// clang-format off
void put_char(u8 c) {
    EM_ASM(
        {
            if (typeof window.putCharToTerminal === 'function') {
                window.putCharToTerminal($0);
            }
        },
        c);
}
// clang-format on

u8 get_char(u8 *data) {
    *data = 0;
    return false;
}

EMSCRIPTEN_KEEPALIVE
void machine_start(uint8_t *addr, uint32_t size) {
    printf("start! : addr %p size %d\n", addr, size);
    memcpy(memory, addr, size);
    printf("copy end\n");

    // 初始化机器并运行
    qemu_machine_init(&machine, (struct QemuPortableOperations){
                                    .sram_data = memory,
                                    .sram_size = memory_size,
                                    .get_char  = get_char,
                                    .put_char  = put_char,
                                });
}

EMSCRIPTEN_KEEPALIVE
void machine_step() {
    struct DeviceFunc bus = bus_device_get_func(&machine.bus);
    for (int i = 0; i < 20; i++) {
        riscvcore_step(&machine.core);
        bus.update(bus.context, 1);
    }
}

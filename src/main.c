#include "bus/bus.h"
#include "common/debug.h"
#include "cpu/riscv32.h"
#include "device/device.h"
#include "device/serial.h"
#include "device/sram.h"
#include "diff.h"
#include <stdio.h>

extern uint8_t *sram_mem;

int main(int argc, char **argv) {
    Log("argc : %d ,argv : %s", argc, argv[1]);

    // 初始化并添加设备
    sram_init_file(0x10000000, argv[1]);
    bus_add_device(
        (Device){0x80000000, 0x10000000, sram_read, sram_write, NULL, NULL});
    bus_add_device(
        (Device){0x10000000, 0x100, serial_read, serial_write, NULL, NULL});

    // 初始化difftest
    // init_difftest("/Project/mini-rv32ima/sim-nemu/mini-rv32ima.so");
    // ref_difftest_memcpy(0x80000000, sram_mem, 0x10000, DIFFTEST_TO_REF);
    // ref_difftest_regcpy(&riscv32core, DIFFTEST_TO_REF);

    while (!riscv32core.halt) {
        //  check_difftest();
        // ref_difftest_exec(1);
        riscv32_step();
        bus_update();
    }

    if (riscv32core.regs[10] == 0) {
        printf(ANSI_FMT("test OK\n", ANSI_FG_GREEN));
    } else {
        printf(ANSI_FMT("test ERROR\n", ANSI_FG_RED));
    }

    return riscv32core.regs[10] != 0;
}

#include "bus/bus.h"
#include "common/debug.h"
#include "cpu/csr.h"
#include "cpu/riscv32.h"
#include "device/clint.h"
#include "device/device.h"
#include "device/ram.h"
#include "device/serial.h"
#include "diff.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    Log("argc : %d ,argv : %s", argc, argv[1]);

    // 初始化总线和设备
    RAM *ram =
        ram_init_file(0x10000000, "/Project/mini-rv32ima/sim-nemu/linuxImage");

    bus_add_device(0x2000000, 0x10000, clint_init(), clint_func);
    bus_add_device(0x10000000, 0x100, serial_init(), serial_func);
    bus_add_device(0x80000000, 0x10000000, ram, ram_func);

    // 初始化处理器
    Riscv32core *riscv32core = malloc(sizeof(Riscv32core));
    riscv32core->pc = 0x80000000;
    riscv32core->regs[10] = 0x00; // hart ID
    riscv32core->regs[11] = 0x809ff000;
    riscv32core->privilege = MACHINE;

    // 初始化difftest
    init_difftest("/Project/mini-rv32ima/sim-nemu/mini-rv32ima.so");
    ref_difftest_memcpy(0x80000000, ram->data, 0x7000000, DIFFTEST_TO_REF);
    ref_difftest_regcpy(riscv32core, DIFFTEST_TO_REF);

    // 运行
    while (!riscv32core->halt) {
        check_difftest(riscv32core);
        ref_difftest_exec(1);

        riscv32_step(riscv32core);
        bus_update();
    }

    if (riscv32core->regs[10] == 0) {
        printf(ANSI_FMT("test OK\n", ANSI_FG_GREEN));
    } else {
        printf(ANSI_FMT("test ERROR\n", ANSI_FG_RED));
    }

    return riscv32core->regs[10] != 0;
}

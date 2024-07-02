/*
 * 利用模拟器组建构建的简易虚拟机
 */

#include "bus/bus.h"
#include "cli_parser.h"
#include "common/debug.h"
#include "cpu/csr.h"
#include "cpu/riscv32.h"
#include "device/clint.h"
#include "device/device.h"
#include "device/ram.h"
#include "device/serial.h"
#include "diff.h"
#include "gdbstub.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Riscv32core *riscv32core;
RAM *ram;

static int emu_read_reg(void *args, int regno, size_t *reg_value) {
    struct emu *emu = (struct emu *)args;
    if (regno > 32) {
        return EFAULT;
    }

    if (regno == 32) {
        *reg_value = riscv32core->pc;
    } else {
        *reg_value = riscv32core->regs[regno];
    }
    return 0;
}

static int emu_write_reg(void *args, int regno, size_t data) {
    struct emu *emu = (struct emu *)args;

    if (regno > 32) {
        return EFAULT;
    }

    if (regno == 32) {
        riscv32core->pc = data;
    } else {
        riscv32core->regs[regno] = data;
    }
    return 0;
}
static int emu_read_mem(void *args, size_t addr, size_t len, void *val) {
    memcpy(val, (void *)ram->data + addr - 0x80000000, len);
    return 0;
}

static int emu_write_mem(void *args, size_t addr, size_t len, void *val) {
    memcpy((void *)ram->data + addr - 0x80000000, val, len);
    return 0;
}

static gdb_action_t emu_cont(void *args) {
    struct emu *emu = (struct emu *)args;

    printf("cont\n");

    return ACT_RESUME;
}

static gdb_action_t emu_stepi(void *args) {
    struct emu *emu = (struct emu *)args;

    riscv32_step(riscv32core);

    return ACT_RESUME;
}

static bool emu_set_bp(void *args, size_t addr, bp_type_t type) { return true; }

static bool emu_del_bp(void *args, size_t addr, bp_type_t type) { return true; }

static void emu_on_interrupt(void *args) {}

struct target_ops emu_ops = {
    .read_reg = emu_read_reg,
    .write_reg = emu_write_reg,
    .read_mem = emu_read_mem,
    .write_mem = emu_write_mem,
    .cont = emu_cont,
    .stepi = emu_stepi,
    .set_bp = emu_set_bp,
    .del_bp = emu_del_bp,
    .on_interrupt = emu_on_interrupt,
};

int main(int argc, char **argv) {
    // 初始化总线和设备
    ram = ram_init_file(0x10000000, argv[1]);
    bus_add_device(0x2000000, 0x10000, clint_init(), clint_func);
    bus_add_device(0x10000000, 0x10, serial_init(), serial_func);
    bus_add_device(0x80000000, 0x10000000, ram, ram_func);

    // 初始化处理器
    riscv32core = malloc(sizeof(Riscv32core));
    riscv32core->pc = 0x80000000;
    riscv32core->regs[10] = 0x00; // hart ID
    riscv32core->regs[11] = 0x809ff000;
    riscv32core->privilege = MACHINE;

    // 初始化difftest
    //    init_difftest("/Project/mini-rv32ima/sim-nemu/mini-rv32ima.so");
    //   ref_difftest_memcpy(0x80000000, ram->data, ram->size, DIFFTEST_TO_REF);
    //  ref_difftest_regcpy(riscv32core, DIFFTEST_TO_REF);

    // 运行
    gdbstub_t gdbstub;
    memset(&gdbstub, 0, sizeof(gdbstub_t));
    if (!gdbstub_init(&gdbstub, &emu_ops,
                      (arch_info_t){
                          .reg_num = 33,
                          .reg_byte = 4,
                          .target_desc = TARGET_RV32,
                      },
                      "127.0.0.1:1234")) {
        fprintf(stderr, "Fail to create socket.\n");
        return -1;
    }

    if (!gdbstub_run(&gdbstub, (void *)riscv32core)) {
        fprintf(stderr, "Fail to run in debug mode.\n");
        return -1;
    }
    gdbstub_close(&gdbstub);

    /* while (!riscv32core->halt) { */
    /*     riscv32_step(riscv32core); */
    /*     bus_update(); */
    /* } */
    /**/
    /* if (riscv32core->regs[10] == 0) { */
    /*     printf(ANSI_FMT("test OK\n", ANSI_FG_GREEN)); */
    /* } else { */
    /*     printf(ANSI_FMT("test ERROR\n", ANSI_FG_RED)); */
    /* } */
    /**/
    /* return riscv32core->regs[10] != 0; */
}

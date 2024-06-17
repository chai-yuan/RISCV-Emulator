#include "common/debug.h"
#include "cpu/csr.h"
#include "cpu/except.h"
#include <cpu/decode.h>
#include <cpu/mmu.h>
#include <cpu/riscv64.h>
#include <stdio.h>

#define R(i) core->regs[i]
#define CSR(i) core->csr[i]
#define Rd core->regs[dec->rd]
#define Rs1 reg_src1
#define Rs2 reg_src2
#define PC core->pc
#define CPU(i) core->i
#define Mr(addr, size, data)                                                   \
    do {                                                                       \
        mmu_read_64(core, dec, addr, size, &data);                             \
    } while (0);
#define Mw(addr, size, data)                                                   \
    do {                                                                       \
        mmu_write_64(core, dec, addr, size, data);                             \
    } while (0);

#define INSTEXE(name, ...)                                                     \
    case inst_##name: {                                                        \
        __VA_ARGS__;                                                           \
        break;                                                                 \
    }

void riscv64_exec(Riscv64core *core, RiscvDecode *dec) {
    uint64_t reg_src1 = core->regs[dec->rs1], reg_src2 = core->regs[dec->rs2];

    switch (dec->instruction) {
        INSTEXE(add, Rd = Rs1 + Rs2);
    default:
        panic("未知指令! %d", dec->instruction);
        dec->except = EXC_IllegalInstruction;
    }
    R(0) = 0;
}

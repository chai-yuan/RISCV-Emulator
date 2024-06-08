#ifndef INST_H
#define INST_H

#include "cpu/csr.h"
#include "cpu/decode.h"
#include "cpu/except.h"
#include "cpu/mmu.h"
#include "cpu/riscv32.h"

void riscv32_inst_exec(Riscv32core *core, RiscvDecode *dec);

#define INSTPAT(pattern, name, ...)                                            \
    do {                                                                       \
        uint64_t key, mask, shift;                                             \
        pattern_decode(pattern, STRLEN(pattern), &key, &mask, &shift);         \
        if ((((uint64_t)dec->inst >> shift) & mask) == key) {                  \
            __VA_ARGS__;                                                       \
            goto exec_end;                                                     \
        }                                                                      \
    } while (0)

#define R(i) core->regs[i]
#define CSR(i) core->csr[i]
#define Rd core->regs[dec->rd]
#define Rs1 core->regs[dec->rs1]
#define Rs2 core->regs[dec->rs2]
#define PC core->pc
#define CPU(i) core->i
#define Mr(addr, size, data)                                                   \
    do {                                                                       \
        uint64_t read_data;                                                    \
        mmu_read(addr, size, &read_data);                                      \
        data = read_data;                                                      \
    } while (0);
#define Mw(addr, size, data)                                                   \
    do {                                                                       \
        mmu_write(addr, size, data);                                           \
    } while (0);

#endif // !INST_H

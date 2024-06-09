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

#endif // !INST_H

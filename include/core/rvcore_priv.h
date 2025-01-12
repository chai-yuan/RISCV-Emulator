#ifndef RV_CORE_PRIV_H
#define RV_CORE_PRIV_H

#include "core/rvcore.h"

void riscv_decode_init(struct RiscvDecode *decode);
void riscv_decode_inst(struct RiscvDecode *decode);

usize riscv_csr_read(struct RiscvCore *core, u16 addr);
void  riscv_csr_write(struct RiscvCore *core, u16 addr, usize value);

enum exception riscvcore_mmu_read(struct RiscvCore *core, usize addr, u8 size, usize *data);
enum exception riscvcore_mmu_write(struct RiscvCore *core, usize addr, u8 size, usize data);
void           riscvcore_mmu_fetch(struct RiscvCore *core);

void riscvcore_exec(struct RiscvCore *core);
void riscv_exception_handle(struct RiscvCore *core);
void riscv_interrupt_handle(struct RiscvCore *core);

#define CSRR(addr) riscv_csr_read(core, addr)
#define CSRW(addr, value) riscv_csr_write(core, addr, value)

#define DR(addr, size, value) core->device_func.read(core->device_func.context, addr, size, value)
#define DW(addr, size, value) core->device_func.write(core->device_func.context, addr, size, value)

#define INSTPAT(pattern, name)                                                                     \
    do {                                                                                           \
        u64 key, mask, shift;                                                                      \
        pattern_decode(pattern, (sizeof(pattern) - 1), &key, &mask, &shift);                       \
        if ((((u64)decode->inst_raw >> shift) & mask) == key) {                                    \
            decode->inst = inst_##name;                                                            \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#endif

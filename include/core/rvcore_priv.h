#ifndef RV_CORE_PRIV_H
#define RV_CORE_PRIV_H

#include "core/rvcore.h"

void riscv_decode_init(struct RiscvDecode *decode);
void riscv_decode_inst(struct RiscvDecode *decode);

usize riscv_csr_read(struct RiscvCore *core, u16 addr);
void  riscv_csr_write(struct RiscvCore *core, u16 addr, usize value);

enum exception riscvcore_mmu_read(struct RiscvCore *core, usize addr, u8 size, usize *data);
enum exception riscvcore_mmu_write(struct RiscvCore *core, usize addr, u8 size, usize data);
void           riscvcore_mmu_fetch(struct RiscvCore *core, struct RiscvDecode *decode);

void riscvcore_exec(struct RiscvCore *core, struct RiscvDecode *decode);

void riscv_exception_handle(struct RiscvCore *core, struct RiscvDecode *decode);

void riscv_interrupt_handle(struct RiscvCore *core, struct RiscvDecode *decode);

#endif

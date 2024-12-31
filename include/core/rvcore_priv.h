#ifndef RV_CORE_PRIV_H
#define RV_CORE_PRIV_H

#include "core/rvcore64.h"

void riscv_decode_init(struct RiscvDecode *decode);
void riscv_decode_inst(struct RiscvDecode *decode);

u64  riscv64_csr_read(struct RiscvCore64 *core, u16 addr);
void riscv64_csr_write(struct RiscvCore64 *core, u16 addr, u64 value);

enum exception riscvcore64_mmu_read(struct RiscvCore64 *core, u64 addr, u8 size, u64 *data);
enum exception riscvcore64_mmu_write(struct RiscvCore64 *core, u64 addr, u8 size, u64 data);
void           riscvcore64_mmu_fetch(struct RiscvCore64 *core, struct RiscvDecode *decode);

void riscvcore64_exec(struct RiscvCore64 *core, struct RiscvDecode *decode);

#endif

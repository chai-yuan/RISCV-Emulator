#ifndef RVMMU_H
#define RVMMU_H

#include "core/rvcore64.h"
#include "core/rvdecode.h"
#include "core/rvprivilege.h"

enum exception riscvcore64_mmu_read(struct RiscvCore64 *core, u64 addr, u8 size, u64 *data);

enum exception riscvcore64_mmu_write(struct RiscvCore64 *core, u64 addr, u8 size, u64 data);

void riscvcore64_mmu_fetch(struct RiscvCore64 *core, struct RiscvDecode *decode);

#endif

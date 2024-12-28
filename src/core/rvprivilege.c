#include "core/rvprivilege.h"

u64 riscv64_csr_read(struct RiscvCore64 *core, u16 addr) { return core->csrs[addr]; }

void riscv64_csr_write(struct RiscvCore64 *core, u16 addr, u64 value) { core->csrs[addr] = value; }

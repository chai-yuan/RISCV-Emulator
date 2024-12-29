#include "core/rvcore_priv.h"

u64 riscv64_csr_read(struct RiscvCore64 *core, u16 addr) {
    if (addr == SIE) {
        return core->csrs[MIE] & core->csrs[MIDELEG];
    }
    return core->csrs[addr];
}

void riscv64_csr_write(struct RiscvCore64 *core, u16 addr, u64 value) {
    if (addr == SIE) {
        core->csrs[MIE] = (core->csrs[MIE] & ~core->csrs[MIDELEG]) | (value & core->csrs[MIDELEG]);
    } else {
        core->csrs[addr] = value;
    }
}

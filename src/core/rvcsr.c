#include "core.h"

usize riscv_csr_read(struct RiscvCore *core, u16 addr) {
    switch (addr) {
    case SSTATUS:
        return core->csrs[MSTATUS] & SSTATUS_VISIBLE;
    case SIE:
        return core->csrs[MIE] & core->csrs[MIDELEG];
    case SIP:
        return core->csrs[MIP] & core->csrs[MIDELEG];
    default:
        return core->csrs[addr];
    }
}

void riscv_csr_write(struct RiscvCore *core, u16 addr, usize value) {
    if ((addr >> 10) == 0x3) // read only
        return;
    if (addr == MISA) // 简化实现，misa不可写
        return;
    if ((addr >= 0x3c0 && addr <= 0x3ef) || (addr >= 0x3a4 && addr <= 0x3af)) // pmp仅开启前16项
        return;

    switch (addr) {
    case SSTATUS: {
        core->csrs[MSTATUS] = (core->csrs[MSTATUS] & ~SSTATUS_VISIBLE) | (value & SSTATUS_VISIBLE);
        break;
    }
    case SIE: {
        usize mask      = core->csrs[MIDELEG];
        core->csrs[MIE] = (core->csrs[MIE] & ~mask) | (value & mask);
        break;
    }
    case SIP: {
        usize mask      = core->csrs[MIDELEG];
        core->csrs[MIP] = (core->csrs[MIP] & ~mask) | (value & mask);
        break;
    }
    default:
        core->csrs[addr] = value;
    }
}

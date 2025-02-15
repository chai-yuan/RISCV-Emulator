#include "core.h"

bool riscv_csr_read(struct RiscvCore *core, u16 addr, usize *value) {
    if (((addr >> 8) & 0x3) > core->mode) // 权限检查
        return false;

    if (addr == 0xc01)
        return false; // time
    if (addr == 0x14d)
        return false; // Sstc
    if (addr == 0x30c)
        return false; // Smstateen/Ssstateen
    if (addr == 0xda0)
        return false; // Sscofpmf
    if (addr == 0x000 || addr == 0x321)
        return false; // Smcntrpmf
    if (addr == 0xfb0)
        return false; // ???

    switch (addr) {
    case SSTATUS:
        *value = core->csrs[MSTATUS] & SSTATUS_VISIBLE;
    case SIE:
        *value = core->csrs[MIE] & core->csrs[MIDELEG];
    case SIP:
        *value = core->csrs[MIP] & core->csrs[MIDELEG];
    default:
        *value = core->csrs[addr];
    }
    return true;
}

void riscv_csr_write(struct RiscvCore *core, u16 addr, usize value) {
    if ((addr >> 10) == 0x3) // read only
        return;
    if (addr == MISA) // 简化实现，misa不可写
        return;
    if ((0x3c0 <= addr && addr <= 0x3ef) || (0x3a4 <= addr && addr <= 0x3af)) // pmp仅开启前16项
        return;
    if ((0xb03 <= addr && addr <= 0xb1f) || (0xb83 <= addr && addr <= 0xb9f)) // 忽略事件触发CSR计数器
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

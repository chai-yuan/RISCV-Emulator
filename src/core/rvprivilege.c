#include "core/rvcore_priv.h"
#include "debug.h"

usize riscv_csr_read(struct RiscvCore *core, u16 addr) {
    if (addr == SIE) {
        return core->csrs[MIE] & core->csrs[MIDELEG];
    }
    return core->csrs[addr];
}

void riscv_csr_write(struct RiscvCore *core, u16 addr, usize value) {
    if (addr == SIE) {
        core->csrs[MIE] = (core->csrs[MIE] & ~core->csrs[MIDELEG]) | (value & core->csrs[MIDELEG]);
    } else {
        core->csrs[addr] = value;
    }
}

void riscv_exception_handle(struct RiscvCore *core, struct RiscvDecode *decode) {
    WARN("Exception occurred: %d", decode->exception);
}

void riscv_interrupt_handle(struct RiscvCore *core, struct RiscvDecode *decode) {}

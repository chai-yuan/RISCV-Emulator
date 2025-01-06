#include "core/rvcore_priv.h"
#include "debug.h"

usize riscv_csr_read(struct RiscvCore *core, u16 addr) {
    switch (addr) {
    case SIE:
        return core->csrs[MIE] & core->csrs[MIDELEG];
    default:
        return core->csrs[addr];
    }
}

void riscv_csr_write(struct RiscvCore *core, u16 addr, usize value) {
    if ((addr >> 10) == 0x3)
        return; // read only
    if (((addr >> 8) & 0x3) <= core->mode) {
        core->decode.exception     = ILLEGAL_INSTRUCTION;
        core->decode.exception_val = core->decode.inst_raw;
        return;
    }

    switch (addr) {
    case SIE:
        core->csrs[MIE] = (core->csrs[MIE] & ~core->csrs[MIDELEG]) | (value & core->csrs[MIDELEG]);
        break;
    default:
        core->csrs[addr] = value;
    }
}

void riscv_exception_handle(struct RiscvCore *core, struct RiscvDecode *decode) {
    WARN("Exception occurred: %d , %x", decode->exception, decode->exception_val);
    if ((core->mode <= SUPERVISOR) && ((CSRR(MEDELEG) >> decode->exception) & 1)) { // 异常委托
        if (core->mode == USER) {
            CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 8));
        } else { // SUPERVISOR
            CSRW(SSTATUS, CSRR(SSTATUS) | (1 << 8));
        }
        core->mode      = SUPERVISOR;
        decode->next_pc = CSRR(STVEC) & ~1;
        CSRW(SEPC, core->pc);
        CSRW(SCAUSE, decode->exception);
        CSRW(STVAL, decode->exception_val);
        if ((CSRR(SSTATUS) >> 1) & 1) {
            CSRW(SSTATUS, CSRR(SSTATUS) | (1 << 5));
        } else {
            CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 5));
        }
        CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 1));
    } else {
        CSRW(MSTATUS, (CSRR(MSTATUS) & ~(3 << 11)) | (core->mode << 11));
        core->mode      = MACHINE;
        decode->next_pc = CSRR(MTVEC) & ~1;

        CSRW(MEPC, core->pc);
        CSRW(MCAUSE, decode->exception);
        CSRW(MTVAL, decode->exception_val);
        if ((CSRR(MSTATUS) >> 3) & 1) {
            CSRW(MSTATUS, CSRR(MSTATUS) | (1 << 7));
        } else {
            CSRW(MSTATUS, CSRR(MSTATUS) & ~(1 << 7));
        }
        CSRW(MSTATUS, CSRR(MSTATUS) & ~(1 << 3));
    }
}

void riscv_interrupt_handle(struct RiscvCore *core, struct RiscvDecode *decode) {
    WARN("interrupt occurred: %d", decode->interrupt);
}

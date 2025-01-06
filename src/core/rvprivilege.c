#include "core/rvcore_priv.h"
#include "debug.h"
#define RCSR(name, val)                                                                            \
    case name:                                                                                     \
        return core->val

#define WCSR(name, val)                                                                            \
    case name:                                                                                     \
        core->val = value;                                                                         \
        break

usize riscv_csr_read(struct RiscvCore *core, u16 addr) {
    switch (addr) {
        RCSR(MSTATUS, mstatus);
        RCSR(MISA, misa);
        RCSR(MEDELEG, medeleg);
        RCSR(MIDELEG, mideleg);
        RCSR(MIE, mie);
        RCSR(MTVEC, mtvec);
        RCSR(MSCRATCH, mscratch);
        RCSR(MEPC, mepc);
        RCSR(MCAUSE, mcause);
        RCSR(MTVAL, mtval);
        RCSR(MIP, mip);

        RCSR(SSTATUS, sstatus);
        RCSR(STVEC, stvec);
        RCSR(SSCRATCH, sscratch);
        RCSR(SEPC, sepc);
        RCSR(SCAUSE, scause);
        RCSR(STVAL, stval);
        RCSR(SIP, sip);
        RCSR(SATP, satp);
    case SIE:
        return core->mie & core->mideleg;
    }
    return 0;
}

void riscv_csr_write(struct RiscvCore *core, u16 addr, usize value) {
    switch (addr) {
        WCSR(MSTATUS, mstatus);
        WCSR(MEDELEG, medeleg);
        WCSR(MIDELEG, mideleg);
        WCSR(MIE, mie);
        WCSR(MTVEC, mtvec);
        WCSR(MSCRATCH, mscratch);
        WCSR(MEPC, mepc);
        WCSR(MCAUSE, mcause);
        WCSR(MTVAL, mtval);
        WCSR(MIP, mip);

        WCSR(SSTATUS, sstatus);
        WCSR(STVEC, stvec);
        WCSR(SEPC, sepc);
        WCSR(SSCRATCH, sscratch);
        WCSR(SCAUSE, scause);
        WCSR(STVAL, stval);
        WCSR(SIP, sip);
        WCSR(SATP, satp);
    case SIE:
        core->mie = (core->mie & ~core->mideleg) | (value & core->mideleg);
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

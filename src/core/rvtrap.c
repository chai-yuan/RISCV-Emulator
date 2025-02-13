#include "core.h"

void riscv_trap_handle(struct RiscvCore *core) {
    usize cause = 0;
    usize pc    = core->pc;
    if (DEC.exception != EXC_NONE) {
        cause = DEC.exception;
    } else if (DEC.interrupt != INT_NONE) {
        cause = DEC.interrupt | INT_MIN;
    } else {
        return;
    }

    if ((core->mode <= SUPERVISOR) && ((CSRR(MEDELEG) >> (u16)cause) & 1)) { // 异常委托
        if (DEC.interrupt != INT_NONE) {
            usize vec   = (CSRR(STVEC) & 1) == 1 ? (4 * cause) : 0;
            DEC.next_pc = (CSRR(STVEC) & ~1) + vec;
        } else {
            DEC.next_pc = CSRR(STVEC) & ~1;
        }

        if (core->mode == USER) {
            CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 8));
        } else { // SUPERVISOR
            CSRW(SSTATUS, CSRR(SSTATUS) | (1 << 8));
        }
        core->mode = SUPERVISOR;

        CSRW(SEPC, pc);
        CSRW(SCAUSE, cause);
        CSRW(STVAL, DEC.exception_val);
        if ((CSRR(SSTATUS) >> 1) & 1) {
            CSRW(SSTATUS, CSRR(SSTATUS) | (1 << 5));
        } else {
            CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 5));
        }
        CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 1));
    } else {
        if (DEC.interrupt != INT_NONE) {
            usize vec   = (CSRR(MTVEC) & 1) == 1 ? (4 * cause) : 0;
            DEC.next_pc = (CSRR(MTVEC) & ~1) + vec;
        } else {
            DEC.next_pc = CSRR(MTVEC) & ~1;
        }

        CSRW(MSTATUS, (CSRR(MSTATUS) & ~(3 << 11)) | (core->mode << 11));
        core->mode = MACHINE;

        CSRW(MEPC, pc);
        CSRW(MCAUSE, cause);
        CSRW(MTVAL, DEC.exception_val);
        if ((CSRR(MSTATUS) >> 3) & 1) {
            CSRW(MSTATUS, CSRR(MSTATUS) | (1 << 7));
        } else {
            CSRW(MSTATUS, CSRR(MSTATUS) & ~(1 << 7));
        }
        CSRW(MSTATUS, CSRR(MSTATUS) & ~(1 << 3));
    }
}

bool riscv_check_pending_interrupt(struct RiscvCore *core) {
    struct mstatusdef *mstatus = (struct mstatusdef *)&core->csrs[MSTATUS];

    if (core->mode == MACHINE && mstatus->mie == 0) {
        return false;
    } else if (core->mode == SUPERVISOR && mstatus->sie == 0) {
        return false;
    }

    usize         pending = CSRR(MIE) & CSRR(MIP);
    struct ipdef *ip      = (struct ipdef *)&pending;

    if (ip->s_e_ip) {
        ip->s_e_ip    = 0;
        DEC.interrupt = SUPERVISOR_EXTERNAL_INTERRUPT;
    }
    if (ip->s_s_ip) {
        ip->s_s_ip    = 0;
        DEC.interrupt = SUPERVISOR_SOFTWARE_INTERRUPT;
    }
    if (ip->s_t_ip) {
        ip->s_t_ip    = 0;
        DEC.interrupt = SUPERVISOR_TIMER_INTERRUPT;
    }
    if (ip->m_e_ip) {
        ip->m_e_ip    = 0;
        DEC.interrupt = MACHINE_EXTERNAL_INTERRUPT;
    }
    if (ip->m_s_ip) {
        ip->m_s_ip    = 0;
        DEC.interrupt = MACHINE_SOFTWARE_INTERRUPT;
    }
    if (ip->m_t_ip) {
        ip->m_t_ip    = 0;
        DEC.interrupt = MACHINE_TIMER_INTERRUPT;
    }

    return DEC.interrupt != INT_NONE;
}

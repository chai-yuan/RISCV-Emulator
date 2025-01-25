#include "core.h"

void riscv_trap_handle(struct RiscvCore *core) {
    usize cause = 0;
    usize pc    = core->pc;
    if (DEC.exception != EXC_NONE) {
        cause = DEC.exception;
    } else if (DEC.interrupt != INT_NONE) {
        cause = DEC.interrupt | INT_MIN;
        pc = core->decode.next_pc;
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
    if (core->mode == MACHINE && (CSRR(MSTATUS) & STATUS_MIE) == 0) {
        return false;
    } else if (core->mode == SUPERVISOR && (CSRR(SSTATUS) & STATUS_SIE) == 0) {
        return false;
    }
    // 检查并设置外部中断
    if (core->device_func.check_interrupt(core->device_func.context)) {
        if (core->mode == MACHINE) {
            CSRW(MIP, CSRR(MIP) | IP_MEIP);
        } else if (core->mode == SUPERVISOR) {
            CSRW(MIP, CSRR(MIP) | IP_SEIP);
        } else if (core->mode == USER) {
            CSRW(MIP, CSRR(MIP) | IP_UEIP);
        }
    }

    u32 pending = CSRR(MIE) & CSRR(MIP);
    if (pending & IP_MEIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_MEIP);
        DEC.interrupt = MACHINE_EXTERNAL_INTERRUPT;
        return true;
    }
    if (pending & IP_MSIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_MSIP);
        DEC.interrupt = MACHINE_SOFTWARE_INTERRUPT;
        return true;
    }
    if (pending & IP_MTIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_MTIP);
        DEC.interrupt = MACHINE_TIMER_INTERRUPT;
        return true;
    }

    if (pending & IP_SEIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_SEIP);
        DEC.interrupt = SUPERVISOR_EXTERNAL_INTERRUPT;
        return true;
    }
    if (pending & IP_SSIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_SSIP);
        DEC.interrupt = SUPERVISOR_SOFTWARE_INTERRUPT;
        return true;
    }
    if (pending & IP_STIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_STIP);
        DEC.interrupt = SUPERVISOR_TIMER_INTERRUPT;
        return true;
    }

    if (pending & IP_UEIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_UEIP);
        DEC.interrupt = USER_EXTERNAL_INTERRUPT;
        return true;
    }
    if (pending & IP_USIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_USIP);
        DEC.interrupt = USER_SOFTWARE_INTERRUPT;
        return true;
    }
    if (pending & IP_UTIP) {
        CSRW(MIP, CSRR(MIP) & ~IP_UTIP);
        DEC.interrupt = USER_TIMER_INTERRUPT;
        return true;
    }

    return DEC.interrupt != INT_NONE;
}

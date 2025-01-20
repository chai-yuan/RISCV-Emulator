#include "core.h"
#include "debug.h"

void riscv_exception_handle(struct RiscvCore *core) {
    struct RiscvDecode *decode = &core->decode;
    WARN("Exception occurred: %d , %x at pc : %x", decode->exception, decode->exception_val,
         core->pc);
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

bool riscv_check_pending_interrupt(struct RiscvCore *core) {
    if (core->mode == MACHINE && (CSRR(MSTATUS) & STATUS_MIE) == 0) {
        return false;
    } else if (core->mode == SUPERVISOR && (CSRR(SSTATUS) & STATUS_SIE) == 0) {
        return false;
    }

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

void riscv_interrupt_handle(struct RiscvCore *core) {}

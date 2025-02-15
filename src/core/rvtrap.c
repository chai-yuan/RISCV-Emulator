#include "core.h"
#include "debug.h"

void riscv_trap_handle_s(struct RiscvCore *core, usize cause) {
    /*struct mstatusdef *status = (struct mstatusdef *)core->csrs[MSTATUS];*/
    /*struct tvecdef    *tvec   = (struct tvecdef *)core->csrs[STVEC];*/
    /**/
    /*usize vec          = (DEC.interrupt != INT_NONE && tvec->mode == 1) ? (4 * cause) : 0;*/
    /*DEC.next_pc        = tvec->base + vec;*/
    /*status->spp        = core->mode & 0x1;*/
    /*core->mode         = SUPERVISOR;*/
    /*core->csrs[SEPC]   = core->pc;*/
    /*core->csrs[SCAUSE] = cause;*/
    /*core->csrs[STVAL]  = DEC.exception_val;*/
    /*status->spie       = status->sie;*/
    /*status->sie        = 0;*/
}

void riscv_trap_handle_m(struct RiscvCore *core, usize cause) {
    usize vec   = (DEC.interrupt != INT_NONE && MTVEC_MODE == 1) ? (4 * cause) : 0;
    DEC.next_pc = MTVEC_BASE + vec;
    MSTATUS_SET_MPP(core->mode);
    core->mode         = MACHINE;
    core->csrs[MEPC]   = core->pc;
    core->csrs[MCAUSE] = cause;
    core->csrs[MTVAL]  = DEC.exception_val;
    MSTATUS_SET_MPIE(MSTATUS_MIE);
    MSTATUS_SET_MIE(0);
}

void riscv_trap_handle(struct RiscvCore *core) {
    if (DEC.interrupt != INT_NONE) {
        usize cause = DEC.interrupt | INT_MIN;
        if ((core->mode <= SUPERVISOR) && ((core->csrs[MIDELEG] >> (u16)cause) & 1)) { // 中断委托
            riscv_trap_handle_s(core, cause);
        } else {
            riscv_trap_handle_m(core, cause);
        }
    } else if (DEC.exception != EXC_NONE) {
        usize cause = DEC.exception;
        if ((core->mode <= SUPERVISOR) && ((core->csrs[MEDELEG] >> (u16)cause) & 1)) { // 异常委托
            riscv_trap_handle_s(core, cause);
        } else {
            riscv_trap_handle_m(core, cause);
        }
    } else {
        return;
    }
}

bool riscv_check_pending_interrupt(struct RiscvCore *core) {
    if (core->mode == MACHINE && MSTATUS_MIE == 0) {
        return false;
    } else if (core->mode == SUPERVISOR && MSTATUS_SIE == 0) {
        return false;
    }

    usize pending = core->csrs[MIP] & core->csrs[MIE];

    if (IP_SSIP(pending))
        DEC.interrupt = SUPERVISOR_SOFTWARE_INTERRUPT;
    if (IP_STIP(pending))
        DEC.interrupt = SUPERVISOR_TIMER_INTERRUPT;
    if (IP_SEIP(pending))
        DEC.interrupt = SUPERVISOR_EXTERNAL_INTERRUPT;
    if (IP_MSIP(pending))
        DEC.interrupt = MACHINE_SOFTWARE_INTERRUPT;
    if (IP_MTIP(pending))
        DEC.interrupt = MACHINE_TIMER_INTERRUPT;
    if (IP_MEIP(pending))
        DEC.interrupt = MACHINE_EXTERNAL_INTERRUPT;

    return DEC.interrupt != INT_NONE;
}

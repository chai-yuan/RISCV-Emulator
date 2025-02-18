#include "core.h"
#include "debug.h"

void riscv_trap_handle_s(struct RiscvCore *core, usize cause) {
    usize vec   = (DEC.interrupt != INT_NONE && STVEC_MODE == 1) ? (4 * cause) : 0;
    DEC.next_pc = STVEC_BASE + vec;
    MSTATUS_SET_SPP(core->mode & 0x1);
    core->mode         = SUPERVISOR;
    core->csrs[SEPC]   = core->pc;
    core->csrs[SCAUSE] = cause;
    core->csrs[STVAL]  = DEC.exception_val;
    MSTATUS_SET_SPIE(MSTATUS_SIE);
    MSTATUS_SET_SIE(0);
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
        core->wfi = false; // 触发中断后清除休眠标志
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
    usize pending = core->csrs[MIP] & core->csrs[MIE];
    if (pending == 0)
        return false;

    usize enable_interrupts = 0;
    usize machine_enable    = core->mode < MACHINE || (core->mode == MACHINE && MSTATUS_MIE);
    enable_interrupts       = pending & ~core->csrs[MIDELEG] & -machine_enable;
    if (enable_interrupts == 0) {
        usize supervisor_enable = core->mode < SUPERVISOR || (core->mode == SUPERVISOR && MSTATUS_SIE);
        enable_interrupts       = pending & core->csrs[MIDELEG] & -supervisor_enable;
    }

    if (enable_interrupts) {
        if (IP_SSIP(enable_interrupts))
            DEC.interrupt = SUPERVISOR_SOFTWARE_INTERRUPT;
        if (IP_STIP(enable_interrupts))
            DEC.interrupt = SUPERVISOR_TIMER_INTERRUPT;
        if (IP_SEIP(enable_interrupts))
            DEC.interrupt = SUPERVISOR_EXTERNAL_INTERRUPT;
        if (IP_MSIP(enable_interrupts))
            DEC.interrupt = MACHINE_SOFTWARE_INTERRUPT;
        if (IP_MTIP(enable_interrupts))
            DEC.interrupt = MACHINE_TIMER_INTERRUPT;
        if (IP_MEIP(enable_interrupts))
            DEC.interrupt = MACHINE_EXTERNAL_INTERRUPT;
    }

    return DEC.interrupt != INT_NONE;
}

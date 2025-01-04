#ifndef RV_CORE_H
#define RV_CORE_H

#include "core/riscv.h"
#include "device/device.h"
#include "types.h"

struct RiscvCore {
    usize regs[32];
    usize pc;

    usize mstatus, medeleg, mideleg, mie, mtvec, mscratch, mepc, mcause, mtval, mip;
    usize sstatus, sie, stvec, sscratch, sepc, scause, stval, sip, satp;

    enum mode         mode;
    bool              reservation_valid;
    usize             reservation_addr;
    bool              halt;
    struct DeviceFunc device_func;
};

void riscvcore_init(struct RiscvCore *core, struct DeviceFunc device_func);

void riscvcore_step(struct RiscvCore *core);

#endif

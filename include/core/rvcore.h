#ifndef RV_CORE_H
#define RV_CORE_H

#include "core/riscv.h"
#include "device/device.h"
#include "types.h"

struct RiscvDecode {
    u32            inst;
    u8             rd, rs1, rs2, rs1_, rs2_;
    isize          immI, immB, immU, immJ, immS;
    usize          csr_imm, next_pc;
    bool           is_inst16;
    enum exception exception;
    usize          exception_val;
    enum interrupt interrupt;
};

struct RiscvEnvInfo {
    bool eint;
};

struct RiscvCore {
    usize     regs[32];
    usize     pc;
    usize     csrs[4096];
    enum mode mode;
    bool      reservation_valid;
    usize     reservation_addr;

    struct RiscvDecode decode;
    struct DeviceFunc  device_func;
};

void riscvcore_init(struct RiscvCore *core, struct DeviceFunc device_func);

void riscvcore_step(struct RiscvCore *core, struct RiscvEnvInfo envinfo);

#endif

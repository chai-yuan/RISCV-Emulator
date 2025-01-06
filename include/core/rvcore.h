#ifndef RV_CORE_H
#define RV_CORE_H

#include "core/riscv.h"
#include "device/device.h"
#include "types.h"

struct RiscvCore {
    usize regs[32];
    usize pc;
    usize csrs[4096];

    enum mode          mode;
    bool               reservation_valid;
    usize              reservation_addr;
    bool               halt;
    struct RiscvDecode decode;
    struct DeviceFunc  device_func;
};

void riscvcore_init(struct RiscvCore *core, struct DeviceFunc device_func);

void riscvcore_step(struct RiscvCore *core);

#endif

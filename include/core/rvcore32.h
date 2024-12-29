#ifndef RV_CORE_32_H
#define RV_CORE_32_H

#include "core/rvcore.h"
#include "device/device.h"
#include "types.h"

struct RiscvCore32 {
    u32               regs[32];
    u32               pc;
    u32               csrs[4096];
    enum mode         mode;
    bool              halt;
    struct DeviceFunc device_func;
};

void riscvcore32_init(struct RiscvCore32 *core, struct DeviceFunc device_func);

struct CoreFunc riscvcore32_get_func(struct RiscvCore32 *core);

#endif

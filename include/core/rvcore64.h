#ifndef RV_CORE_64_H
#define RV_CORE_64_H

#include "types.h"
#include "device/device.h"
#include "core/rvcore.h"

enum mode { USER = 0x0, SUPERVISOR = 0x1, MACHINE = 0x3 };

struct RiscvCore64 {
    u64 regs[32];
    u64 pc;
    u64 csrs[4096];
    enum mode mode;
    bool halt;
    struct DeviceFunc device_func;
};

void riscvcore64_init(struct RiscvCore64 *core, struct DeviceFunc device_func);

struct CoreFunc riscvcore64_get_func(struct RiscvCore64 *core);

#endif

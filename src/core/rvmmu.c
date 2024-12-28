#include "core/rvmmu.h"

enum exception riscvcore64_mmu_read(struct RiscvCore64 *core, u64 addr, u8 size, u64 *data) {
    struct DeviceFunc device = core->device_func;
    return device.read(device.context, addr, size, data);
}

enum exception riscvcore64_mmu_write(struct RiscvCore64 *core, u64 addr, u8 size, u64 data) {
    struct DeviceFunc device = core->device_func;
    return device.write(device.context, addr, size, data);
}

void riscvcore64_mmu_fetch(struct RiscvCore64 *core, struct RiscvDecode *decode) {
    u64 inst;
    decode->exception = riscvcore64_mmu_read(core, core->pc, 4, &inst);
    decode->inst_raw  = inst;
}

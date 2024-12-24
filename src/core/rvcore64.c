#include "core/rvcore64.h"
#include "core/rvdecode.h"

void riscvcore64_init(struct RiscvCore64 *core, struct DeviceFunc device_func) {
    core->pc = 0x80000000;
    core->mode = MACHINE;
    core->device_func = device_func;
}

void riscvcore64_mmu_read(struct RiscvCore64 *core, u64 addr, u8 size,
                          u64 *data) {
    struct DeviceFunc device = core->device_func;
    device.read(device.context, addr, size, data);
}

void riscvcore64_mmu_write(struct RiscvCore64 *core, u64 addr, u8 size,
                           u64 data) {
    struct DeviceFunc device = core->device_func;
    device.write(device.context, addr, size, data);
}

void riscvcore64_mmu_fetch(struct RiscvCore64 *core,
                           struct RiscvDecode *decode) {
    u64 inst;
    riscvcore64_mmu_read(core, core->pc, 4, &inst);
    decode->inst_raw = inst;
}

void riscvcore64_step(struct RiscvCore64 *core) {
    struct RiscvDecode decode; // decode当中保存每一次执行需要用到的临时信息
    riscv_decode(&decode);
}

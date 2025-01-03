#include "core/rvcore_priv.h"

#define REG64(base, offset) *(volatile u64 *)((base) + (offset))
#define REG32(base, offset) *(volatile u32 *)((base) + (offset))
#define REG16(base, offset) *(volatile u16 *)((base) + (offset))
#define REG8(base, offset) *(volatile u8 *)((base) + (offset))

enum exception riscvcore_mmu_read(struct RiscvCore *core, usize addr, u8 size, usize *data) {
    struct DeviceFunc device   = core->device_func;
    u8               *mem_addr = device.get_buffer(device.context, addr);
    switch (size) {
    case 1:
        *data = REG8(mem_addr, 0);
        break;
    case 2:
        *data = REG16(mem_addr, 0);
        break;
    case 4:
        *data = REG32(mem_addr, 0);
        break;
    case 8:
        *data = REG64(mem_addr, 0);
        break;
    }
    return device.handle(device.context, addr, size, false);
}

enum exception riscvcore_mmu_write(struct RiscvCore *core, usize addr, u8 size, usize data) {
    struct DeviceFunc device   = core->device_func;
    u8             *mem_addr = device.get_buffer(device.context, addr);
    switch (size) {
    case 1:
        REG8(mem_addr, 0) = data;
        break;
    case 2:
        REG16(mem_addr, 0) = data;
        break;
    case 4:
        REG32(mem_addr, 0) = data;
        break;
    case 8:
        REG64(mem_addr, 0) = data;
        break;
    }
    return device.handle(device.context, addr, size, true);
}

void riscvcore_mmu_fetch(struct RiscvCore *core, struct RiscvDecode *decode) {
    usize inst;
    decode->exception = riscvcore_mmu_read(core, core->pc, 4, &inst);
    decode->inst_raw  = inst;
}

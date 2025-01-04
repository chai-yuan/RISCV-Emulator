#include "core/rvcore_priv.h"
#include "debug.h"

#define REG64(base, offset) *(volatile u64 *)((base) + (offset))
#define REG32(base, offset) *(volatile u32 *)((base) + (offset))
#define REG16(base, offset) *(volatile u16 *)((base) + (offset))
#define REG8(base, offset) *(volatile u8 *)((base) + (offset))

enum exception riscvcore_mmu_translate(struct RiscvCore *core, usize addr, u64 *paddr) {
#if CURRENT_ARCH == ARCH_RV32 // SV32
    struct DeviceFunc device = core->device_func;

    if ((core->csrs[SATP] >> 31) == 0) {
        *paddr = addr;
        return EXC_NONE;
    }

    u64 ppn   = core->csrs[SATP] & 0x3fffff;
    u64 vpn[] = {
        (addr >> 12) & 0x3ff,
        (addr >> 22) & 0x3ff,
    };

    u64 page_table_addr1 = (ppn << 12) | (vpn[1] << 2);
    INFO("page_table_addr1 : %llx", page_table_addr1);
    u64 page_table_entry = REG64(device.get_buffer(device.context, page_table_addr1), 0);
    INFO("page_table_entry : %llx", page_table_entry);
    u64 page_table_addr2 = (page_table_entry & ~0xfff) | vpn[0];
    INFO("page_table_addr2 : %llx", page_table_addr2);
    u64 page_table_entry2 = REG64(device.get_buffer(device.context, page_table_addr2), 0);
    *paddr                = (page_table_entry2 & ~0xfff) | (addr & 0xfff);

    INFO("vm translate : %x -> %llx", addr, *paddr);
#elif CURRENT_ARCH == ARCH_RV64 // SV39
    *paddr = addr;
    INFO("vm translate : %llx -> %llx", addr, *paddr);
#endif
    return EXC_NONE;
}

enum exception riscvcore_mmu_read(struct RiscvCore *core, usize addr, u8 size, usize *data) {
    struct DeviceFunc device = core->device_func;
    u64               paddr  = 0;
    riscvcore_mmu_translate(core, addr, &paddr);
    u8 *mem_addr = device.get_buffer(device.context, paddr);
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
    struct DeviceFunc device = core->device_func;
    u64               paddr  = 0;
    riscvcore_mmu_translate(core, addr, &paddr);
    u8 *mem_addr = device.get_buffer(device.context, paddr);
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

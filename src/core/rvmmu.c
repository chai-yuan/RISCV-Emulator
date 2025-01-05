#include "core/rvcore_priv.h"
#include "debug.h"

#define REG64(base, offset) *(volatile u64 *)((base) + (offset))
#define REG32(base, offset) *(volatile u32 *)((base) + (offset))
#define REG16(base, offset) *(volatile u16 *)((base) + (offset))
#define REG8(base, offset) *(volatile u8 *)((base) + (offset))

enum exception riscvcore_mmu_translate(struct RiscvCore *core, bool fetch, usize addr, u64 *paddr) {
    bool enable_vm = (CSRR(SATP) >> (sizeof(usize) * 8 - 1));
    if ((CSRR(MSTATUS) & (1 << 17)) && !fetch) { // MPRV
        usize mpp = (CSRR(MSTATUS) & (0x3 << 11)) >> 11;
        enable_vm &= mpp != MACHINE;
    } else {
        enable_vm &= (core->mode != MACHINE);
    }
    INFO("MSTATUS : %x enable_vm : %d , MODE : %d", CSRR(MSTATUS), enable_vm, core->mode);
    if (!enable_vm) {
        *paddr = addr;
        return EXC_NONE;
    }

    struct DeviceFunc device = core->device_func;
#if CURRENT_ARCH == ARCH_RV32 // SV32
    u64 ppn   = CSRR(SATP) & 0x3fffff;
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

enum exception device_read(struct RiscvCore *core, usize paddr, u8 size, usize *data) {
    struct DeviceFunc device   = core->device_func;
    u8               *mem_addr = device.get_buffer(device.context, paddr);
    if (mem_addr == NULL)
        return LOAD_ACCESS_FAULT;
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
    return device.handle(device.context, paddr, size, false);
}

enum exception device_write(struct RiscvCore *core, usize paddr, u8 size, usize data) {
    struct DeviceFunc device   = core->device_func;
    u8               *mem_addr = device.get_buffer(device.context, paddr);
    if (mem_addr == NULL)
        return LOAD_ACCESS_FAULT;
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
    return device.handle(device.context, paddr, size, true);
}

enum exception riscvcore_mmu_read(struct RiscvCore *core, usize addr, u8 size, usize *data) {
    u64 paddr = 0;
    if (riscvcore_mmu_translate(core, false, addr, &paddr) != EXC_NONE) {
        return LOAD_PAGE_FAULT;
    }
    return device_read(core, paddr, size, data);
}

enum exception riscvcore_mmu_write(struct RiscvCore *core, usize addr, u8 size, usize data) {
    u64 paddr = 0;
    if (riscvcore_mmu_translate(core, false, addr, &paddr) != EXC_NONE) {
        return STORE_AMO_PAGE_FAULT;
    }
    return device_write(core, paddr, size, data);
}

void riscvcore_mmu_fetch(struct RiscvCore *core, struct RiscvDecode *decode) {
    u64   paddr = 0;
    usize inst  = 0;
    if (riscvcore_mmu_translate(core, true, core->pc, &paddr) != EXC_NONE) {
        decode->exception = INSTRUCTION_PAGE_FAULT;
    } else if (device_read(core, paddr, 4, &inst) != EXC_NONE) {
        decode->exception = INSTRUCTION_ACCESS_FAULT;
    }
    if (decode->exception != EXC_NONE)
        decode->exception_val = core->pc;
    decode->inst_raw = inst;
}

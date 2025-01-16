#include "core.h"

enum exception mmu_translate(struct RiscvCore *core, enum exception exc, usize addr, u64 *paddr) {
    bool enable_vm = (CSRR(SATP) >> (sizeof(usize) * 8 - 1));
    if (core->mode == MACHINE) {
        if ((CSRR(MSTATUS) & STATUS_MPRV) && (exc != INSTRUCTION_PAGE_FAULT)) { // MPRV
            usize mpp = (CSRR(MSTATUS) & (0x3 << 11)) >> 11;
            enable_vm = enable_vm && mpp != MACHINE;
        } else
            enable_vm = false;
    }
    if (!enable_vm) {
        *paddr = addr;
        return EXC_NONE;
    }

    // u64 dirty = (exc == STORE_AMO_PAGE_FAULT) ? 1 : 0;
#if CURRENT_ARCH == ARCH_RV32 // SV32
    u64 ppn   = CSRR(SATP) & 0x3fffff;
    i32 level = 2;
    u64 vpn[] = {
        (addr >> 12) & 0x3ff,
        (addr >> 22) & 0x3ff,
    };
    usize page_table_entry[] = {0, 0};
    u64   page_table_addr[]  = {0, 0};

    while (1) {
        level--;
        if (level < 0)
            return exc;

        page_table_addr[level] = (ppn << 12) | (vpn[level] << 2);
        if (DR(page_table_addr[level], 4, &page_table_entry[level]) != EXC_NONE)
            return exc;
        ppn = (page_table_entry[level] >> 10) & 0xfffff;

        bool v = page_table_entry[level] & 1;
        bool r = (page_table_entry[level] >> 1) & 1;
        bool w = (page_table_entry[level] >> 2) & 1;
        bool x = (page_table_entry[level] >> 3) & 1;
        bool u = (page_table_entry[level] >> 4) & 1;
        if (v == false)
            return exc; // 检查有效位
        if (u &&
            !((core->mode == USER) || (core->mode == SUPERVISOR && CSRR(SSTATUS) & (1 << 18)) ||
              (core->mode == MACHINE && CSRR(MSTATUS) & (1 << 18))))
            return exc; // 检查U页面权限
        if (r || x || w)
            break; // 检查叶子节点页
    }

    switch (level) {
    case 0: {
        *paddr = (ppn << 12) | (addr & 0xfff);
        break;
    }
    case 1: {
        if (ppn & 0x3ff)
            return exc; // 超级页低位需要置0
        *paddr = (ppn << 12) | (addr & 0x3fffff);
        break;
    }
    default:
        return exc;
    }
    /*
        if (dirty)
            for (int i = 1; i >= 0 && page_table_addr[i] != 0; i--) {
                page_table_entry[i] |= (1 << 7);
                DW(page_table_addr[i], 4, page_table_entry[i]);
            }
    */

#elif CURRENT_ARCH == ARCH_RV64 // SV39
    u64 ppn   = CSRR(SATP) & 0xfffffffffff;
    i32 level = 3;
    u64 vpn[] = {
        (addr >> 12) & 0x3ff,
        (addr >> 21) & 0x1ff,
        (addr >> 30) & 0x1ff,
    };
    u64 page_table_entry = 0;
    u8 *table_memory[]   = {NULL, NULL, NULL};

    while (1) {
        level--;
        if (level < 0)
            return exc;

        u64 page_table_addr = (ppn << 12) | (vpn[level] << 3);
        table_memory[level] = device.get_buffer(device.context, page_table_addr);
        if (table_memory[level] == NULL)
            return exc;
        page_table_entry = REG64(table_memory[level], 0);
        ppn              = (page_table_entry >> 10) & 0xfffffffffff;

        bool v = page_table_entry & 1;
        bool r = (page_table_entry >> 1) & 1;
        bool w = (page_table_entry >> 2) & 1;
        bool x = (page_table_entry >> 3) & 1;
        bool u = (page_table_entry >> 4) & 1;
        if (v == false)
            return exc; // 检查有效位
        if (u &&
            !((core->mode == USER) || (core->mode == SUPERVISOR && CSRR(SSTATUS) & (1 << 18)) ||
              (core->mode == MACHINE && CSRR(MSTATUS) & (1 << 18))))
            return exc; // 检查U页面权限
        if (r || x || w)
            break; // 检查叶子节点页
    }

    switch (level) {
    case 0: {
        *paddr = (ppn << 12) | (addr & 0xfff);
        break;
    }
    case 1: {
        if (ppn & 0x1ff)
            return exc; // 超级页低位需要置0
        *paddr = (ppn << 12) | (vpn[0] << 12) | (addr & 0xfff);
        break;
    }
    case 2: {
        if (ppn & 0x3ffff)
            return exc; // 超级页低位需要置0
        *paddr = (ppn << 12) | (vpn[1] << 21) | (vpn[0] << 12) | (addr & 0xfff);
        break;
    }
    default:
        return exc;
    }

    for (int i = 1; i >= 0 && table_memory[i] != NULL; i--)
        REG64(table_memory[i], 0) |= (dirty << 7);

    INFO("vm translate : %llx -> %llx", addr, *paddr);
#endif
    return EXC_NONE;
}

enum exception riscvcore_mmu_read(struct RiscvCore *core, usize addr, u8 size, usize *data) {
    u64 paddr = addr;
    if (mmu_translate(core, LOAD_PAGE_FAULT, addr, &paddr) != EXC_NONE) {
        return LOAD_PAGE_FAULT;
    }
    return DR(paddr, size, data);
}

enum exception riscvcore_mmu_write(struct RiscvCore *core, usize addr, u8 size, usize data) {
    u64 paddr = addr;
    if (mmu_translate(core, STORE_AMO_PAGE_FAULT, addr, &paddr) != EXC_NONE) {
        return STORE_AMO_PAGE_FAULT;
    }
    return DW(paddr, size, data);
}

void riscvcore_mmu_fetch(struct RiscvCore *core) {
    u64   paddr = core->pc;
    usize inst  = 0;
    if (mmu_translate(core, INSTRUCTION_PAGE_FAULT, core->pc, &paddr) != EXC_NONE) {
        core->decode.exception = INSTRUCTION_PAGE_FAULT;
    } else if (DR(paddr, 4, &inst) != EXC_NONE) {
        core->decode.exception = INSTRUCTION_ACCESS_FAULT;
    }
    if (core->decode.exception != EXC_NONE)
        core->decode.exception_val = core->pc;
    core->decode.inst = inst;
}

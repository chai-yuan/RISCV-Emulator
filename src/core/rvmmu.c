#include "core.h"
#include "debug.h"

enum exception mmu_translate_sv32(struct RiscvCore *core, enum exception exc, usize addr, u64 *paddr) {
    return EXC_NONE;
}

enum exception mmu_translate_sv39(struct RiscvCore *core, enum exception exc, usize addr, u64 *paddr) {
    i32 level = 3;
    u64 ppn   = SATP_PPN;
    u64 pt_addr[3];
    u64 pt_entry[3];
    u64 vaddr[] = {
        (addr >> 12) & 0x1ff,
        (addr >> 21) & 0x1ff,
        (addr >> 30) & 0x1ff,
    };
    // 查找叶节点表项
    while (1) {
        level--;
        if (level < 0)
            return exc;

        pt_addr[level] = (ppn << 12) | (vaddr[level] << 3);
        if (DR(pt_addr[level], 8, (usize *)&pt_entry[level]) != EXC_NONE)
            return exc;

        if (SV39_V(pt_entry[level]) == 0) // 有效位检查
            return exc;
        if (SV39_R(pt_entry[level]) || SV39_X(pt_entry[level]) || SV39_W(pt_entry[level])) { // 叶节点
            switch (level) {
            case 0: {
                *paddr = (SV39_PPN(pt_entry[level]) << 12) | (addr & 0xfff);
                break;
            }
            case 1: {
                *paddr = (SV39_PPN(pt_entry[level]) << 12) | (addr & 0x1fffff);
                break;
            }
            case 2: {
                *paddr = (SV39_PPN(pt_entry[level]) << 12) | (addr & 0x3fffffff);
                break;
            }
            default:
                ERROR("Does not support page level");
            }
            break;
        }

        ppn = SV39_PPN(pt_entry[level]);
    }
    // 设置脏位 TODO

//    INFO("vaddr : %llx -> paddr : %llx", addr, *paddr);
    return EXC_NONE;
}

enum exception mmu_translate(struct RiscvCore *core, enum exception exc, usize addr, u64 *paddr) {
    *paddr = addr;
    if (SATP_MODE == 0) {
        return EXC_NONE;
    }
    if (core->mode == MACHINE) {
        if (!MSTATUS_MPRV || (exc == INSTRUCTION_PAGE_FAULT))
            return EXC_NONE;
    }

#if CURRENT_ARCH == ARCH_RV32 // SV32
    return mmu_translate_sv32(core, exc, addr, paddr);
#elif CURRENT_ARCH == ARCH_RV64 // SV39
    return mmu_translate_sv39(core, exc, addr, paddr);
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
        DEC.exception     = INSTRUCTION_PAGE_FAULT;
        DEC.exception_val = core->pc;
    } else if (DR(paddr, 4, &inst) != EXC_NONE) {
        DEC.exception     = INSTRUCTION_ACCESS_FAULT;
        DEC.exception_val = core->pc;
    }
    core->decode.inst = inst;
}

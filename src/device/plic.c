#include "device/plic.h"
#include "core/riscv.h"

void plic_init(struct PLIC *plic) {
    for (int i = 0; i < sizeof(struct PLIC); i++)
        *((u8 *)plic + i) = 0;
}

bool plic_check_irq(struct PLIC *plic, u32 context_id) {
    if (context_id >= CONTEXT_NUM) {
        return false;
    }

    // 检查是否有任何中断等待处理
    for (int i = 0; i < (SOURCE_NUM + 32) / 32; i++) {
        // 当前挂起且启用的中断
        u32 pending_enabled = plic->pending[i] & plic->enable[context_id][i] & ~plic->claimed[i];

        if (pending_enabled) {
            // 查找优先级高于阈值的中断
            for (int bit = 0; bit < 32; bit++) {
                if (bit + i * 32 > SOURCE_NUM) {
                    break;
                }

                u32 irq = bit + i * 32;
                if (irq == 0)
                    continue; // 0是无效中断源

                if ((pending_enabled & (1u << bit)) && plic->priority[irq] > plic->threshold[context_id]) {
                    return true;
                }
            }
        }
    }

    return false;
}

void plic_update_intterupt(struct PLIC *plic, bool interrupt, u32 interrupt_num) {
    if (interrupt_num == 0 || interrupt_num > SOURCE_NUM) {
        return; // 忽略无效的中断号
    }

    u32 index = interrupt_num / 32;
    u32 bit   = interrupt_num % 32;

    if (interrupt) {
        // 设置中断挂起位
        plic->pending[index] |= (1u << bit);
    } else {
        // 清除中断挂起位
        plic->pending[index] &= ~(1u << bit);

        // 同时清除已声明位
        plic->claimed[index] &= ~(1u << bit);
    }
}

static enum exception plic_read(void *context, u64 addr, u8 size, usize *data) {
    struct PLIC *plic = (struct PLIC *)context;
    if (size != 4 || addr + size > PLIC_SIZE)
        return LOAD_ACCESS_FAULT;

    *data = 0;

    // 优先级寄存器
    if (addr >= PLIC_PRIORITY && addr < PLIC_PRIORITY + 4 * (SOURCE_NUM + 1)) {
        u32 source = (addr - PLIC_PRIORITY) / 4;
        *data      = plic->priority[source];
        return EXC_NONE;
    }

    // 挂起位寄存器
    if (addr >= PLIC_PENDING && addr < PLIC_PENDING + 4 * ((SOURCE_NUM + 31) / 32)) {
        u32 reg_idx = (addr - PLIC_PENDING) / 4;
        *data       = plic->pending[reg_idx];
        return EXC_NONE;
    }

    // M模式中断使能寄存器
    if (addr >= PLIC_MENABLE && addr < PLIC_MENABLE + 4 * ((SOURCE_NUM + 31) / 32)) {
        u32 reg_idx = (addr - PLIC_MENABLE) / 4;
        *data       = plic->enable[0][reg_idx]; // Context 0 = M模式
        return EXC_NONE;
    }

    // S模式中断使能寄存器
    if (addr >= PLIC_SENABLE && addr < PLIC_SENABLE + 4 * ((SOURCE_NUM + 31) / 32)) {
        u32 reg_idx = (addr - PLIC_SENABLE) / 4;
        *data       = plic->enable[1][reg_idx]; // Context 1 = S模式
        return EXC_NONE;
    }

    // M模式优先级阈值
    if (addr == PLIC_MPRIORITY) {
        *data = plic->threshold[0];
        return EXC_NONE;
    }

    // S模式优先级阈值
    if (addr == PLIC_SPRIORITY) {
        *data = plic->threshold[1];
        return EXC_NONE;
    }

    // M模式声明和完成寄存器
    if (addr == PLIC_MCLAIM) {
        // 寻找最高优先级的挂起中断
        u32 max_pri = 0;
        u32 max_id  = 0;

        for (u32 src = 1; src <= SOURCE_NUM; src++) {
            u32 idx = src / 32;
            u32 bit = src % 32;

            if ((plic->pending[idx] & (1u << bit)) &&       // 中断挂起
                (plic->enable[0][idx] & (1u << bit)) &&     // 中断使能
                !(plic->claimed[idx] & (1u << bit)) &&      // 未被认领
                plic->priority[src] > plic->threshold[0] && // 优先级高于阈值
                plic->priority[src] > max_pri) {            // 是最高优先级
                max_pri = plic->priority[src];
                max_id  = src;
            }
        }

        if (max_id > 0) {
            u32 idx = max_id / 32;
            u32 bit = max_id % 32;
            plic->claimed[idx] |= (1u << bit); // 标记为已认领
            plic->claim[0] = max_id;
        }

        *data = plic->claim[0];
        return EXC_NONE;
    }

    // S模式声明和完成寄存器
    if (addr == PLIC_SCLAIM) {
        // 寻找最高优先级的挂起中断
        u32 max_pri = 0;
        u32 max_id  = 0;

        for (u32 src = 1; src <= SOURCE_NUM; src++) {
            u32 idx = src / 32;
            u32 bit = src % 32;

            if ((plic->pending[idx] & (1u << bit)) &&       // 中断挂起
                (plic->enable[1][idx] & (1u << bit)) &&     // 中断使能
                !(plic->claimed[idx] & (1u << bit)) &&      // 未被认领
                plic->priority[src] > plic->threshold[1] && // 优先级高于阈值
                plic->priority[src] > max_pri) {            // 是最高优先级
                max_pri = plic->priority[src];
                max_id  = src;
            }
        }

        if (max_id > 0) {
            u32 idx = max_id / 32;
            u32 bit = max_id % 32;
            plic->claimed[idx] |= (1u << bit); // 标记为已认领
            plic->claim[1] = max_id;
        }

        *data = plic->claim[1];
        return EXC_NONE;
    }

    return EXC_NONE; // 读取未实现的寄存器返回0

    return EXC_NONE;
}

static enum exception plic_write(void *context, u64 addr, u8 size, usize data) {
    struct PLIC *plic = (struct PLIC *)context;
    if (size != 4 || addr + size > PLIC_SIZE)
        return STORE_AMO_ACCESS_FAULT;

    // 优先级寄存器
    if (addr >= PLIC_PRIORITY && addr < PLIC_PRIORITY + 4 * (SOURCE_NUM + 1)) {
        u32 source = (addr - PLIC_PRIORITY) / 4;
        if (source == 0) {
            // 源0是保留的，不能写入
            return EXC_NONE;
        }
        plic->priority[source] = data & 0x7; // 通常PLIC支持3位优先级
        return EXC_NONE;
    }

    // 挂起位寄存器 (通常只读，但某些实现可能支持通过写1来清除)
    if (addr >= PLIC_PENDING && addr < PLIC_PENDING + 4 * ((SOURCE_NUM + 31) / 32)) {
        // 忽略写入挂起寄存器的操作
        return EXC_NONE;
    }

    // M模式中断使能寄存器
    if (addr >= PLIC_MENABLE && addr < PLIC_MENABLE + 4 * ((SOURCE_NUM + 31) / 32)) {
        u32 reg_idx = (addr - PLIC_MENABLE) / 4;
        // 源0是无效的，所以在第一个寄存器中屏蔽位0
        if (reg_idx == 0) {
            plic->enable[0][0] = data & ~1u;
        } else {
            plic->enable[0][reg_idx] = data;
        }
        return EXC_NONE;
    }

    // S模式中断使能寄存器
    if (addr >= PLIC_SENABLE && addr < PLIC_SENABLE + 4 * ((SOURCE_NUM + 31) / 32)) {
        u32 reg_idx = (addr - PLIC_SENABLE) / 4;
        // 源0是无效的，所以在第一个寄存器中屏蔽位0
        if (reg_idx == 0) {
            plic->enable[1][0] = data & ~1u;
        } else {
            plic->enable[1][reg_idx] = data;
        }
        return EXC_NONE;
    }

    // M模式优先级阈值
    if (addr == PLIC_MPRIORITY) {
        plic->threshold[0] = data & 0x7; // 通常3位优先级
        return EXC_NONE;
    }

    // S模式优先级阈值
    if (addr == PLIC_SPRIORITY) {
        plic->threshold[1] = data & 0x7; // 通常3位优先级
        return EXC_NONE;
    }

    // M模式完成寄存器
    if (addr == PLIC_MCLAIM) {
        u32 completed_irq = data;
        if (completed_irq > 0 && completed_irq <= SOURCE_NUM) {
            u32 idx = completed_irq / 32;
            u32 bit = completed_irq % 32;

            // 清除已认领标志
            plic->claimed[idx] &= ~(1u << bit);

            // 如果中断不再挂起，则清除claim
            if (!(plic->pending[idx] & (1u << bit))) {
                plic->claim[0] = 0;
            }
        }
        return EXC_NONE;
    }

    // S模式完成寄存器
    if (addr == PLIC_SCLAIM) {
        u32 completed_irq = data;
        if (completed_irq > 0 && completed_irq <= SOURCE_NUM) {
            u32 idx = completed_irq / 32;
            u32 bit = completed_irq % 32;

            // 清除已认领标志
            plic->claimed[idx] &= ~(1u << bit);

            // 如果中断不再挂起，则清除claim
            if (!(plic->pending[idx] & (1u << bit))) {
                plic->claim[1] = 0;
            }
        }
        return EXC_NONE;
    }

    return EXC_NONE; // 忽略未实现寄存器的写入
    return EXC_NONE;
}

struct DeviceFunc plic_get_func(struct PLIC *plic) {
    return (struct DeviceFunc){
        .context = plic,
        .read    = plic_read,
        .write   = plic_write,
        .update  = NULL,
    };
}

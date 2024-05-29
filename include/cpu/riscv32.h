#ifndef RV32CORE_H
#define RV32CORE_H

#include <common/common.h>

// riscv32处理器状态
typedef struct Riscv32core {
    uint32_t pc;
    // 通用寄存器
    uint32_t regs[32];
    // CSR
    // 其他状态
    bool sleep; // 休眠标志
    bool halt;  // 停机标志
} Riscv32core;

extern Riscv32core riscv32core;

// 处理器运行
void riscv32exec();

// 打印处理器信息
void riscv32dump(const Riscv32core *core);

#endif

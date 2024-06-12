#ifndef RV64CORE_H
#define RV64CORE_H

#include "cpu/decode.h"
#include <common/common.h>
#include <stdint.h>

// riscv64处理器状态
typedef struct Riscv64core {
    uint64_t pc;
    // 通用寄存器
    uint64_t regs[32];
    // CSR
    uint64_t csr[4096];
    // 其他状态
    PrivilegeLevel privilege; // 特权等级
    uint64_t amo_addr;        // 原子指令记录地址
    bool sleep;               // 休眠标志
    bool halt;                // 停机标志
} Riscv64core;

// 处理器运行
void riscv64_step(Riscv64core *core);

void riscv64_exec(Riscv64core *core, RiscvDecode *dec);

// 打印处理器信息
void riscv64_dump(const Riscv64core *core);

#endif

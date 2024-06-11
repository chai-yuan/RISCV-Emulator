#ifndef RV32CORE_H
#define RV32CORE_H

#include "cpu/decode.h"
#include <common/common.h>
#include <stdint.h>

// 定义 RISC-V 特权级枚举类型
typedef enum { USER = 0, SUPERVISOR = 1, MACHINE = 3 } PrivilegeLevel;

// riscv32处理器状态
typedef struct Riscv32core {
    uint32_t pc;
    // 通用寄存器
    uint32_t regs[32];
    // CSR
    uint32_t csr[4096];
    // 其他状态
    PrivilegeLevel privilege; // 特权等级
    uint32_t amo_addr;        // 原子指令记录地址
    bool sleep;               // 休眠标志
    bool halt;                // 停机标志
} Riscv32core;

// 处理器运行
void riscv32_step(Riscv32core *core);

void riscv32_exec(Riscv32core *core, RiscvDecode *dec);

// 打印处理器信息
void riscv32_dump(const Riscv32core *core);

#endif

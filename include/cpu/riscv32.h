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

extern Riscv32core riscv32core;

// 处理器运行
void riscv32_step();
// 取指译码
void riscv32_fetch_decode(RiscvDecode *decode);
// 执行指令
void riscv32_exec(RiscvDecode *decode);
// 处理写回，中断，异常
void riscv32_writeback(RiscvDecode *decode);

// 打印处理器信息
void riscv32_dump(const Riscv32core *core);

#define R(i) riscv32core.regs[i]
#define CSR(i) riscv32core.csr[i]
#define Rd riscv32core.regs[dec->rd]
#define Rs1 riscv32core.regs[dec->rs1]
#define Rs2 riscv32core.regs[dec->rs2]
#define PC riscv32core.pc
#define CPU(i) riscv32core.i
#define Mr(addr, size, data)                                                   \
    do {                                                                       \
        uint64_t read_data;                                                    \
        mmu_read(addr, size, &read_data);                                      \
        data = read_data;                                                      \
    } while (0);
#define Mw(addr, size, data)                                                   \
    do {                                                                       \
        mmu_write(addr, size, data);                                           \
    } while (0);

#endif

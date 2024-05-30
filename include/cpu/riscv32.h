#ifndef RV32CORE_H
#define RV32CORE_H

#include <common/common.h>
#include <stdint.h>

// riscv32处理器状态
typedef struct Riscv32core {
    uint32_t pc;
    // 通用寄存器
    uint32_t regs[32];
    // CSR
    uint32_t csr[4096];
    // 其他状态
    bool sleep; // 休眠标志
    bool halt;  // 停机标志
} Riscv32core;

extern Riscv32core riscv32core;

// 处理器运行
void riscv32exec();

// 打印处理器信息
void riscv32dump(const Riscv32core *core);

#define R(i) riscv32core.regs[i]
#define CSR(i) riscv32core.csr[i]
#define Rd riscv32core.regs[dec.rd]
#define Rs1 riscv32core.regs[dec.rs1]
#define Rs2 riscv32core.regs[dec.rs2]
#define PC riscv32core.pc
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

#include "common/debug.h"
#include "cpu/csr.h"
#include "cpu/decode.h"
#include "cpu/except.h"
#include "cpu/inst.h"
#include "cpu/mmu.h"
#include "device/device.h"
#include <cpu/riscv32.h>
#include <stdint.h>

void riscv32_step(Riscv32core *core) {
    // 取指译码
    RiscvDecode dec;
    uint64_t inst;
    mmu_read(core->pc, 4, &inst);
    dec = decode((uint32_t)inst);
    dec.next_pc = core->pc + 4;

    // 执行
    riscv32_inst_exec(core, &dec);

    // 处理异常和中断,异常优先
    if (dec.except != EXC_None) {
        core->csr[CSR_MCAUSE] = dec.except;
        core->csr[CSR_MTVAL] = core->pc;
        core->csr[CSR_MEPC] = core->pc;
        core->csr[CSR_MSTATUS] = ((core->csr[CSR_MSTATUS] & 0x08) << 4) |
                                 ((core->privilege & 3) << 11);

        core->pc = core->csr[CSR_MTVEC];
        core->privilege = MACHINE;
    } else if (dec.intr != INTR_None) {

    } else {
        core->pc = dec.next_pc;
    }
    return;
}

const char *regs[] = {"$0", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
                      "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                      "a6", "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
                      "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void riscv32_dump(const Riscv32core *core) {
    printf("PC: 0x%08x\n", core->pc);
    for (int i = 0; i < 32; i++) {
        printf("x%2d: %-6s: 0x%08x\n", i, regs[i], core->regs[i]);
    }
}

#include <cpu/csr.h>
#include <cpu/mmu.h>
#include <cpu/riscv64.h>

void riscv64_step(Riscv64core *core) {
    // 取指译码
    RiscvDecode dec;
    mmu_fetch_64(core, &dec);
    decode(&dec);
    dec.next_pc = core->pc + 4;

    // 执行
    riscv64_exec(core, &dec);

    // 处理异常和中断,异常优先
    if (dec.except != EXC_None) {
        core->csr[CSR_MCAUSE] = dec.except;
        core->csr[CSR_MEPC] = core->pc;
        core->csr[CSR_MTVAL] = core->pc;

        core->csr[CSR_MSTATUS] = ((core->csr[CSR_MSTATUS] & 0x08) << 4) |
                                 ((core->privilege & 3) << 11);
        core->pc = core->csr[CSR_MTVEC];
        core->privilege = MACHINE;
    } else if (dec.intr != INTR_None) {
        core->csr[CSR_MCAUSE] = dec.intr | 0x80000000;
        core->csr[CSR_MEPC] = dec.next_pc;
        core->csr[CSR_MTVAL] = 0;

        core->csr[CSR_MSTATUS] = ((core->csr[CSR_MSTATUS] & 0x08) << 4) |
                                 ((core->privilege & 3) << 11);
        core->pc = core->csr[CSR_MTVEC];
        core->privilege = MACHINE;
    } else {
        core->pc = dec.next_pc;
    }
}

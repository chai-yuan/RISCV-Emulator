#ifndef RV_CORE_PRIV_H
#define RV_CORE_PRIV_H

#include "core/rvcore.h"

void riscv_decode_init(struct RiscvDecode *decode);

bool riscv_csr_read(struct RiscvCore *core, u16 addr, usize *value);
void riscv_csr_write(struct RiscvCore *core, u16 addr, usize value);

enum exception riscvcore_mmu_read(struct RiscvCore *core, usize addr, u8 size, usize *data);
enum exception riscvcore_mmu_write(struct RiscvCore *core, usize addr, u8 size, usize data);
void           riscvcore_mmu_fetch(struct RiscvCore *core);

void riscvcore_exec(struct RiscvCore *core);
void riscv_trap_handle(struct RiscvCore *core);
bool riscv_check_pending_interrupt(struct RiscvCore *core);

#define CSRR(addr, value)                                                                                              \
    do {                                                                                                               \
        if (!riscv_csr_read(core, addr, &value)) {                                                                     \
            DEC.exception     = ILLEGAL_INSTRUCTION;                                                                   \
            DEC.exception_val = DEC.inst;                                                                              \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0);
#define CSRW(addr, value) riscv_csr_write(core, addr, value)

#define DR(addr, size, value) core->device_func.read(core->device_func.context, addr, size, value)
#define DW(addr, size, value) core->device_func.write(core->device_func.context, addr, size, value)

typedef void (*instruction_func_t)(struct RiscvCore *core);
struct Instruction {
    u32                mask, match;
    instruction_func_t func;
};

#define RD core->regs[core->decode.rd]
#define RS1 core->regs[core->decode.rs1]
#define RS2 core->regs[core->decode.rs2]
#define DEC core->decode
#define MR(addr, size, data)                                                                                           \
    do {                                                                                                               \
        if (EXC_NONE != (DEC.exception = riscvcore_mmu_read(core, addr, size, &data))) {                               \
            DEC.exception_val = addr;                                                                                  \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0);
#define MW(addr, size, data)                                                                                           \
    do {                                                                                                               \
        if (EXC_NONE != (DEC.exception = riscvcore_mmu_write(core, addr, size, data)))                                 \
            DEC.exception_val = addr;                                                                                  \
    } while (0);

#endif

#include "core.h"
#include "debug.h"

void riscv_decode_init(struct RiscvDecode *decode) {
    decode->inst          = 0;
    decode->exception     = EXC_NONE;
    decode->exception_val = 0;
    decode->interrupt     = INT_NONE;
}

u64 MASK(u64 n) {
    if (n == 64)
        return ~0ull;
    return (1ull << n) - 1ull;
}
u64 BITS(u64 imm, u64 hi, u64 lo) { return (imm >> lo) & MASK(hi - lo + 1ull); }
u64 SEXT(u64 imm, u64 n) {
    if ((imm >> (n - 1)) & 1) {
        return ((~0ull) << n) | imm;
    } else
        return imm & MASK(n);
}

static void riscv_decode_inst(struct RiscvDecode *decode) {
    u32 inst = decode->inst;
    if ((inst & 0x3) == 0x3) {
        decode->is_inst16 = false;
        decode->rd        = BITS(inst, 11, 7);
        decode->rs1       = BITS(inst, 19, 15);
        decode->rs2       = BITS(inst, 24, 20);

        decode->immI = SEXT(BITS(inst, 31, 20), 12);
        decode->immB = (SEXT(BITS(inst, 31, 31), 1) << 12) | (BITS(inst, 30, 25) << 5) | (BITS(inst, 11, 8) << 1) |
                       (BITS(inst, 7, 7) << 11);
        decode->immU = (SEXT(BITS(inst, 31, 12), 20) << 12);
        decode->immJ = (SEXT(BITS(inst, 31, 31), 1) << 20) | (BITS(inst, 30, 21) << 1) | (BITS(inst, 20, 20) << 11) |
                       (BITS(inst, 19, 12) << 12);
        decode->immS = (SEXT(BITS(inst, 31, 25), 7) << 5) | BITS(inst, 11, 7);

        decode->csr_imm = BITS(inst, 31, 20);
    } else {
        decode->is_inst16 = true;
        decode->rd        = BITS(inst, 11, 7);
        decode->rs1       = BITS(inst, 11, 7);
        decode->rs2       = BITS(inst, 6, 2);
        decode->rs1_      = BITS(inst, 9, 7) + 8;
        decode->rs2_      = BITS(inst, 4, 2) + 8;
    }
}

extern struct Instruction instructions[];

void riscvcore_exec(struct RiscvCore *core) {
    riscv_decode_inst(&core->decode);
    DEC.next_pc = DEC.is_inst16 ? core->pc + 2 : core->pc + 4;

    for (u32 i = 0;; i++) {
        if ((DEC.inst & instructions[i].mask) == instructions[i].match) {
            instructions[i].func(core);
            break;
        }
    }

    core->regs[0] = 0;
}

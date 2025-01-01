#include "core/rvcore_priv.h"

#define INSTBEGIN() switch (decode->inst) {
#define INSTEND()                                                                                  \
    default:                                                                                       \
        decode->exception = ILLEGAL_INSTRUCTION;                                                   \
        }
#define INSTEXE(name, ...)                                                                         \
    case inst_##name: {                                                                            \
        __VA_ARGS__;                                                                               \
        break;                                                                                     \
    }

#define Rd core->regs[decode->rd]
#define Mr(addr, size)                                                                             \
    ({                                                                                             \
        u64 data;                                                                                  \
        decode->exception = riscvcore64_mmu_read(core, addr, size, &data);                         \
        data;                                                                                      \
    })
#define Mw(addr, size, data) decode->exception = riscvcore64_mmu_write(core, addr, size, data)
#define CSRR(addr) riscv64_csr_read(core, addr)
#define CSRW(addr, value) riscv64_csr_write(core, addr, value)

void riscvcore64_exec(struct RiscvCore64 *core, struct RiscvDecode *decode) {
    u64 Rs1 = core->regs[decode->rs1], Rs2 = core->regs[decode->rs2];
    decode->next_pc = core->pc + 4;

    INSTBEGIN();
    // 64I
    INSTEXE(add, Rd = Rs1 + Rs2);
    INSTEXE(addw, Rd = (i32)(Rs1 + Rs2));
    INSTEXE(sub, Rd = Rs1 - Rs2);
    INSTEXE(subw, Rd = (i32)(Rs1 - Rs2));
    INSTEXE(xor, Rd = Rs1 ^ Rs2);
    INSTEXE(or, Rd = Rs1 | Rs2);
    INSTEXE(and, Rd = Rs1 & Rs2);
    INSTEXE(sll, Rd = Rs1 << (Rs2 & 0x3F));
    INSTEXE(sllw, Rd = (i32)(Rs1 << (Rs2 & 0x1F)));
    INSTEXE(slt, Rd = (i64)Rs1 < (i64)Rs2 ? 1 : 0);
    INSTEXE(sltu, Rd = Rs1 < Rs2 ? 1 : 0);
    INSTEXE(srl, Rd = Rs1 >> (Rs2 & 0x3F));
    INSTEXE(srlw, Rd = (i32)((u32)Rs1 >> (Rs2 & 0x1F)));
    INSTEXE(sra, Rd = (i64)Rs1 >> (Rs2 & 0x3F));
    INSTEXE(sraw, Rd = (i32)Rs1 >> (Rs2 & 0x1F));
    INSTEXE(addi, Rd = Rs1 + (i64)decode->immI);
    INSTEXE(addiw, Rd = (i32)(Rs1 + (i64)decode->immI));
    INSTEXE(xori, Rd = Rs1 ^ (u64)decode->immI);
    INSTEXE(ori, Rd = Rs1 | (u64)decode->immI);
    INSTEXE(andi, Rd = Rs1 & (u64)decode->immI);
    INSTEXE(slli, Rd = Rs1 << (decode->immI & 0x3F));
    INSTEXE(slliw, Rd = (i32)(Rs1 << (decode->immI & 0x1F)));
    INSTEXE(srli, Rd = Rs1 >> (decode->immI & 0x3F));
    INSTEXE(srliw, Rd = (i32)((u32)Rs1 >> (decode->immI & 0x1F)));
    INSTEXE(srai, Rd = (i64)Rs1 >> (decode->immI & 0x3F));
    INSTEXE(sraiw, Rd = (i32)Rs1 >> (decode->immI & 0x1F));
    INSTEXE(slti, Rd = (i64)Rs1 < (i64)decode->immI ? 1 : 0);
    INSTEXE(sltiu, Rd = Rs1 < (u64)decode->immI ? 1 : 0);
    INSTEXE(lb, u64 data = Mr(Rs1 + decode->immI, 1); Rd = (i8)data;);
    INSTEXE(lh, u64 data = Mr(Rs1 + decode->immI, 2); Rd = (i16)data;);
    INSTEXE(lw, u64 data = Mr(Rs1 + decode->immI, 4); Rd = (i32)data;);
    INSTEXE(lbu, Rd = Mr(Rs1 + decode->immI, 1));
    INSTEXE(lhu, Rd = Mr(Rs1 + decode->immI, 2));
    INSTEXE(lwu, Rd = Mr(Rs1 + decode->immI, 4));
    INSTEXE(ld, Rd = Mr(Rs1 + decode->immI, 8));
    INSTEXE(sb, Mw(Rs1 + decode->immS, 1, Rs2));
    INSTEXE(sh, Mw(Rs1 + decode->immS, 2, Rs2));
    INSTEXE(sw, Mw(Rs1 + decode->immS, 4, Rs2));
    INSTEXE(sd, Mw(Rs1 + decode->immS, 8, Rs2));
    INSTEXE(beq, if ((i64)Rs1 == (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bne, if ((i64)Rs1 != (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(blt, if ((i64)Rs1 < (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bge, if ((i64)Rs1 >= (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bltu, if (Rs1 < Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bgeu, if (Rs1 >= Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(jal, decode->next_pc = core->pc + decode->immJ; Rd = core->pc + 4);
    INSTEXE(jalr, decode->next_pc = (Rs1 + decode->immI) & ~1; Rd = core->pc + 4);
    INSTEXE(lui, Rd = decode->immU);
    INSTEXE(auipc, Rd = core->pc + decode->immU);
    // 64M
    INSTEXE(mul, Rd = ((i64)Rs1 * (i64)Rs2));
    INSTEXE(mulh, Rd = ((i64)Rs1 * (i64)Rs2) >> 32);
    INSTEXE(mulsu, Rd = ((i64)Rs1 * (u64)Rs2) >> 32);
    INSTEXE(mulu, Rd = ((u64)Rs1 * (u64)Rs2) >> 32);
    INSTEXE(mulw, Rd = (i64)(i32)((i32)Rs1 * (i32)Rs2));
    INSTEXE(div, Rd = (Rs2 == 0)                                  ? -1
                      : ((i64)Rs1 == INT64_MIN && (i64)Rs2 == -1) ? Rs1
                                                                  : ((i64)Rs1 / (i64)Rs2););
    INSTEXE(divu, { Rd = (Rs2 == 0) ? UINT64_MAX : Rs1 / Rs2; });
    INSTEXE(divw, {
        i32 src1   = (i32)Rs1;
        i32 src2   = (i32)Rs2;
        i32 result = (src2 == 0) ? -1 : (src1 == INT32_MIN && src2 == -1) ? src1 : src1 / src2;
        Rd         = (i64)(i32)result;
    });
    INSTEXE(divuw, {
        u32 src1   = (u32)Rs1;
        u32 src2   = (u32)Rs2;
        u32 result = (src2 == 0) ? UINT32_MAX : src1 / src2;
        Rd         = (i64)(i32)result;
    });
    INSTEXE(rem, Rd = (Rs2 == 0)                                  ? Rs1
                      : ((i64)Rs1 == INT64_MIN && (i64)Rs2 == -1) ? 0
                                                                  : ((i64)Rs1 % (i64)Rs2););
    INSTEXE(remu, Rd = (Rs2 == 0) ? Rs1 : Rs1 % Rs2);
    INSTEXE(remw, {
        i32 src1   = (i32)Rs1;
        i32 src2   = (i32)Rs2;
        i32 result = (src2 == 0) ? src1 : (src1 == INT32_MIN && src2 == -1) ? 0 : src1 % src2;
        Rd         = (i64)(i32)result;
    });
    INSTEXE(remuw, {
        u32 src1   = (u32)Rs1;
        u32 src2   = (u32)Rs2;
        u32 result = (src2 == 0) ? src1 : src1 % src2;
        Rd         = (i64)(i32)result;
    });
    // 64A
    INSTEXE(lr_w, {
        Rd                      = (i64)(i32)Mr(Rs1, 4);
        core->reservation_valid = true;
        core->reservation_addr  = Rs1;
    });
    INSTEXE(sc_w, {
        if (core->reservation_valid && core->reservation_addr == Rs1) {
            Mw(Rs1, 4, (u32)Rs2);
            Rd = 0;
        } else {
            Rd = 1;
        }
        core->reservation_valid = false;
    });
    INSTEXE(amoswap_w, {
        i32 value = (i32)Mr(Rs1, 4);
        Mw(Rs1, 4, (u32)Rs2);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amoadd_w, {
        i32 value  = (i32)Mr(Rs1, 4);
        i32 result = value + (i32)Rs2;
        Mw(Rs1, 4, (u32)result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amoxor_w, {
        i32 value  = (i32)Mr(Rs1, 4);
        i32 result = value ^ (i32)Rs2;
        Mw(Rs1, 4, (u32)result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amoor_w, {
        i32 value  = (i32)Mr(Rs1, 4);
        i32 result = value | (i32)Rs2;
        Mw(Rs1, 4, (u32)result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amoand_w, {
        i32 value  = (i32)Mr(Rs1, 4);
        i32 result = value & (i32)Rs2;
        Mw(Rs1, 4, (u32)result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amomin_w, {
        i32 value  = (i32)Mr(Rs1, 4);
        i32 result = (value < (i32)Rs2) ? value : (i32)Rs2;
        Mw(Rs1, 4, (u32)result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amomax_w, {
        i32 value  = (i32)Mr(Rs1, 4);
        i32 result = (value > (i32)Rs2) ? value : (i32)Rs2;
        Mw(Rs1, 4, (u32)result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amominu_w, {
        u32 value  = (u32)Mr(Rs1, 4);
        u32 result = (value < (u32)Rs2) ? value : (u32)Rs2;
        Mw(Rs1, 4, result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(amomaxu_w, {
        u32 value  = (u32)Mr(Rs1, 4);
        u32 result = (value > (u32)Rs2) ? value : (u32)Rs2;
        Mw(Rs1, 4, result);
        Rd = (i64)(i32)value;
    });
    INSTEXE(lr_d, {
        Rd                      = Mr(Rs1, 8);
        core->reservation_valid = true;
        core->reservation_addr  = Rs1;
    });
    INSTEXE(sc_d, {
        if (core->reservation_valid && core->reservation_addr == Rs1) {
            Mw(Rs1, 8, Rs2);
            Rd = 0;
        } else {
            Rd = 1;
        }
        core->reservation_valid = false;
    });
    INSTEXE(amoswap_d, {
        i64 value = Mr(Rs1, 8);
        Mw(Rs1, 8, Rs2);
        Rd = value;
    });
    INSTEXE(amoadd_d, {
        i64 value  = Mr(Rs1, 8);
        i64 result = value + Rs2;
        Mw(Rs1, 8, result);
        Rd = value;
    });
    INSTEXE(amoxor_d, {
        i64 value  = Mr(Rs1, 8);
        i64 result = value ^ Rs2;
        Mw(Rs1, 8, result);
        Rd = value;
    });
    INSTEXE(amoor_d, {
        i64 value  = Mr(Rs1, 8);
        i64 result = value | Rs2;
        Mw(Rs1, 8, result);
        Rd = value;
    });
    INSTEXE(amoand_d, {
        i64 value  = Mr(Rs1, 8);
        i64 result = value & Rs2;
        Mw(Rs1, 8, result);
        Rd = value;
    });
    INSTEXE(amomin_d, {
        i64 value  = Mr(Rs1, 8);
        i64 result = (value < Rs2) ? value : Rs2;
        Mw(Rs1, 8, result);
        Rd = value;
    });
    INSTEXE(amomax_d, {
        i64 value  = Mr(Rs1, 8);
        i64 result = (value > Rs2) ? value : Rs2;
        Mw(Rs1, 8, result);
        Rd = value;
    });
    INSTEXE(amominu_d, {
        u64 value  = (u64)Mr(Rs1, 8);
        u64 result = (value < (u64)Rs2) ? value : (u64)Rs2;
        Mw(Rs1, 8, result);
        Rd = (i64)value;
    });
    INSTEXE(amomaxu_d, {
        u64 value  = (u64)Mr(Rs1, 8);
        u64 result = (value > (u64)Rs2) ? value : (u64)Rs2;
        Mw(Rs1, 8, result);
        Rd = (i64)value;
    });
    // zicsr
    INSTEXE(csrrw, Rd = CSRR(decode->immI); CSRW(decode->immI, Rs1));
    INSTEXE(csrrs, Rd = CSRR(decode->immI); CSRW(decode->immI, Rd | Rs1));
    INSTEXE(csrrc, Rd = CSRR(decode->immI); CSRW(decode->immI, Rd & ~Rs1));
    INSTEXE(csrrwi, Rd = CSRR(decode->immI); CSRW(decode->immI, decode->rs1));
    INSTEXE(csrrsi, Rd = CSRR(decode->immI); CSRW(decode->immI, Rd | decode->rs1));
    INSTEXE(csrrci, Rd = CSRR(decode->immI); CSRW(decode->immI, Rd & ~decode->rs1));
    // zifence
    INSTEXE(fence, {});
    INSTEXE(fence_i, {});
    INSTEXE(sfence_vma, {});
    // SYSTEM
    INSTEXE(ecall, {
        if (core->mode == USER) {
            decode->exception = ECALL_FROM_UMODE;
        } else if (core->mode == SUPERVISOR) {
            decode->exception = ECALL_FROM_SMODE;
        } else if (core->mode == MACHINE) {
            decode->exception = ECALL_FROM_MMODE;
        }
    });
    INSTEXE(sret, {
        decode->next_pc = CSRR(SEPC);
        core->mode      = ((CSRR(SSTATUS) >> 8) & 1) == 1 ? SUPERVISOR : USER;
        CSRW(SSTATUS, ((CSRR(SSTATUS) >> 5) & 1) == 1 ? CSRR(SSTATUS) | (1 << 1)
                                                      : CSRR(SSTATUS) & ~(1 << 1));
        CSRW(SSTATUS, CSRR(SSTATUS) | (1 << 5));
        CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 8));
    });
    INSTEXE(mret, {
        decode->next_pc = CSRR(MEPC);
        u64 mpp         = (CSRR(MSTATUS) >> 11) & 3;
        core->mode      = mpp == 2 ? MACHINE : (mpp == 1 ? SUPERVISOR : USER);
        CSRW(MSTATUS, (((CSRR(MSTATUS) >> 7) & 1) == 1) ? CSRR(MSTATUS) | (1 << 3)
                                                        : CSRR(MSTATUS) & ~(1 << 3));
        CSRW(MSTATUS, CSRR(MSTATUS) | (1 << 7));
        CSRW(MSTATUS, CSRR(MSTATUS) & ~(3 << 11));
    });
    INSTEXE(ebreak, core->halt = true);

    INSTEND();

    core->regs[0] = 0;
}

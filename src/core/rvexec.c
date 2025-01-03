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

#define RD core->regs[decode->rd]
#define MR(addr, size, data) decode->exception = riscvcore_mmu_read(core, addr, size, &data);
#define MW(addr, size, data) decode->exception = riscvcore_mmu_write(core, addr, size, data)
#define CSRR(addr) riscv_csr_read(core, addr)
#define CSRW(addr, value) riscv_csr_write(core, addr, value)

void riscvcore_exec(struct RiscvCore *core, struct RiscvDecode *decode) {
    u64 RS1 = core->regs[decode->rs1], RS2 = core->regs[decode->rs2];
    decode->next_pc = core->pc + 4;

    INSTBEGIN();

    INSTEXE(add, RD = RS1 + RS2);
    INSTEXE(sub, RD = RS1 - RS2);
    INSTEXE(xor, RD = RS1 ^ RS2);
    INSTEXE(or, RD = RS1 | RS2);
    INSTEXE(and, RD = RS1 & RS2);
    INSTEXE(sll, RD = RS1 << (RS2 & 0x3F));
    INSTEXE(slt, RD = (isize)RS1 < (isize)RS2 ? 1 : 0);
    INSTEXE(sltu, RD = RS1 < RS2 ? 1 : 0);
    INSTEXE(srl, RD = RS1 >> (RS2 & 0x3F));
    INSTEXE(sra, RD = (isize)RS1 >> (RS2 & 0x3F));
    INSTEXE(addi, RD = RS1 + (isize)decode->immI);
    INSTEXE(xori, RD = RS1 ^ (isize)decode->immI);
    INSTEXE(ori, RD = RS1 | (usize)decode->immI);
    INSTEXE(andi, RD = RS1 & (usize)decode->immI);
    INSTEXE(slli, RD = RS1 << (decode->immI & 0x3F));
    INSTEXE(srli, RD = RS1 >> (decode->immI & 0x3F));
    INSTEXE(srai, RD = (isize)RS1 >> (decode->immI & 0x3F));
    INSTEXE(slti, RD = (isize)RS1 < (isize)decode->immI ? 1 : 0);
    INSTEXE(sltiu, RD = RS1 < (usize)decode->immI ? 1 : 0);
    INSTEXE(lb, usize data; MR(RS1 + decode->immI, 1, data); RD = (i8)data;);
    INSTEXE(lh, usize data; MR(RS1 + decode->immI, 2, data); RD = (i16)data;);
    INSTEXE(lw, usize data; MR(RS1 + decode->immI, 4, data); RD = (i32)data;);
    INSTEXE(lbu, MR(RS1 + decode->immI, 1, RD));
    INSTEXE(lhu, MR(RS1 + decode->immI, 2, RD));
    INSTEXE(sb, MW(RS1 + decode->immS, 1, RS2));
    INSTEXE(sh, MW(RS1 + decode->immS, 2, RS2));
    INSTEXE(sw, MW(RS1 + decode->immS, 4, RS2));
    INSTEXE(beq, if ((isize)RS1 == (isize)RS2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bne, if ((isize)RS1 != (isize)RS2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(blt, if ((isize)RS1 < (isize)RS2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bge, if ((isize)RS1 >= (isize)RS2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bltu, if (RS1 < RS2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bgeu, if (RS1 >= RS2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(jal, decode->next_pc = core->pc + decode->immJ; RD = core->pc + 4);
    INSTEXE(jalr, decode->next_pc = (RS1 + decode->immI) & ~1; RD = core->pc + 4);
    INSTEXE(lui, RD = decode->immU);
    INSTEXE(auipc, RD = core->pc + decode->immU);

    INSTEXE(mul, RD = RS1 * RS2);
    INSTEXE(mulh, {
        if (sizeof(usize) == 4) {
            RD = ((i64)(i32)RS1 * (i64)(i32)RS2) >> 32;
        } else {
            // TODO
        }
    });
    //    INSTEXE(mulsu, RD = ((isize)RS1 * (u64)RS2) >> 32);
    //    INSTEXE(mulu, RD = ((u64)RS1 * (u64)RS2) >> 32);
    //    INSTEXE(div, RD = (RS2 == 0)                                      ? -1
    //                      : ((isize)RS1 == INT64_MIN && (isize)RS2 == -1) ? RS1
    //                                                                      : ((isize)RS1 /
    //                                                                      (isize)RS2););
    //    INSTEXE(divu, { RD = (RS2 == 0) ? UINT64_MAX : RS1 / RS2; });
    //    INSTEXE(rem, RD = (RS2 == 0)                                      ? RS1
    //                      : ((isize)RS1 == INT64_MIN && (isize)RS2 == -1) ? 0
    //                                                                     : ((isize)RS1 %
    //                                                                     (isize)RS2););
    //    INSTEXE(remu, RD = (RS2 == 0) ? RS1 : RS1 % RS2);

    INSTEXE(lr_w, {
        usize data;
        MR(RS1, 4, data);
        RD                      = (isize)data;
        core->reservation_valid = true;
        core->reservation_addr  = RS1;
    });
    INSTEXE(sc_w, {
        if (core->reservation_valid && core->reservation_addr == RS1) {
            MW(RS1, 4, (u32)RS2);
            RD = 0;
        } else {
            RD = 1;
        }
        core->reservation_valid = false;
    });
    INSTEXE(amoswap_w, {
        usize data;
        MR(RS1, 4, data);
        MW(RS1, 4, (u32)RS2);
        RD = (isize)(i32)data;
    });
    INSTEXE(amoadd_w, {
        usize data;
        MR(RS1, 4, data);
        i32 value  = (i32)data;
        i32 result = value + (i32)RS2;
        MW(RS1, 4, (u32)result);
        RD = (isize)value;
    });
    INSTEXE(amoxor_w, {
        usize data;
        MR(RS1, 4, data);
        i32 value  = (i32)data;
        i32 result = value ^ (i32)RS2;
        MW(RS1, 4, (u32)result);
        RD = (isize)value;
    });
    INSTEXE(amoor_w, {
        usize data;
        MR(RS1, 4, data);
        i32 value  = (i32)data;
        i32 result = value | (i32)RS2;
        MW(RS1, 4, (u32)result);
        RD = (isize)value;
    });
    INSTEXE(amoand_w, {
        usize data;
        MR(RS1, 4, data);
        i32 value  = (i32)data;
        i32 result = value & (i32)RS2;
        MW(RS1, 4, (u32)result);
        RD = (isize)value;
    });
    INSTEXE(amomin_w, {
        usize data;
        MR(RS1, 4, data);
        i32 value  = (i32)data;
        i32 result = (value < (i32)RS2) ? value : (i32)RS2;
        MW(RS1, 4, (u32)result);
        RD = (isize)value;
    });
    INSTEXE(amomax_w, {
        usize data;
        MR(RS1, 4, data);
        i32 value  = (i32)data;
        i32 result = (value > (i32)RS2) ? value : (i32)RS2;
        MW(RS1, 4, (u32)result);
        RD = (isize)value;
    });
    INSTEXE(amominu_w, {
        usize data;
        MR(RS1, 4, data);
        u32 value  = (i32)data;
        u32 result = (value < (u32)RS2) ? value : (u32)RS2;
        MW(RS1, 4, result);
        RD = (isize)(i32)value;
    });
    INSTEXE(amomaxu_w, {
        usize data;
        MR(RS1, 4, data);
        u32 value  = (i32)data;
        u32 result = (value > (u32)RS2) ? value : (u32)RS2;
        MW(RS1, 4, result);
        RD = (isize)(i32)value;
    });

    INSTEXE(csrrw, RD = CSRR(decode->immI); CSRW(decode->immI, RS1));
    INSTEXE(csrrs, RD = CSRR(decode->immI); CSRW(decode->immI, RD | RS1));
    INSTEXE(csrrc, RD = CSRR(decode->immI); CSRW(decode->immI, RD & ~RS1));
    INSTEXE(csrrwi, RD = CSRR(decode->immI); CSRW(decode->immI, decode->rs1));
    INSTEXE(csrrsi, RD = CSRR(decode->immI); CSRW(decode->immI, RD | decode->rs1));
    INSTEXE(csrrci, RD = CSRR(decode->immI); CSRW(decode->immI, RD & ~decode->rs1));

    INSTEXE(fence, {});
    INSTEXE(fence_i, {});
    INSTEXE(sfence_vma, {});

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

#if CURRENT_ARCH == ARCH_RV64
    INSTEXE(addw, RD = (i32)(RS1 + RS2));
    INSTEXE(subw, RD = (i32)(RS1 - RS2));
    INSTEXE(sllw, RD = (i32)(RS1 << (RS2 & 0x1F)));
    INSTEXE(srlw, RD = (i32)((u32)RS1 >> (RS2 & 0x1F)));
    INSTEXE(sraw, RD = (i32)RS1 >> (RS2 & 0x1F));
    INSTEXE(addiw, RD = (i32)(RS1 + (i64)decode->immI));
    INSTEXE(slliw, RD = (i32)(RS1 << (decode->immI & 0x1F)));
    INSTEXE(srliw, RD = (i32)((u32)RS1 >> (decode->immI & 0x1F)));
    INSTEXE(sraiw, RD = (i32)RS1 >> (decode->immI & 0x1F));
    INSTEXE(lwu, MR(RS1 + decode->immI, 4, RD));
    INSTEXE(ld, MR(RS1 + decode->immI, 8, RD));
    INSTEXE(sd, MW(RS1 + decode->immS, 8, RS2));

    INSTEXE(lr_d, {
        MR(RS1, 8, RD);
        core->reservation_valid = true;
        core->reservation_addr  = RS1;
    });
    INSTEXE(sc_d, {
        if (core->reservation_valid && core->reservation_addr == RS1) {
            MW(RS1, 8, RS2);
            RD = 0;
        } else {
            RD = 1;
        }
        core->reservation_valid = false;
    });
    INSTEXE(amoswap_d, {
        usize data;
        MR(RS1, 8, data);
        i64 value = data;
        MW(RS1, 8, RS2);
        RD = value;
    });
    INSTEXE(amoadd_d, {
        usize data;
        MR(RS1, 8, data);
        i64 value  = data;
        i64 result = value + (i64)RS2;
        MW(RS1, 8, result);
        RD = value;
    });
    INSTEXE(amoxor_d, {
        usize data;
        MR(RS1, 8, data);
        i64 value  = data;
        i64 result = value ^ (i64)RS2;
        MW(RS1, 8, result);
        RD = value;
    });
    INSTEXE(amoor_d, {
        usize data;
        MR(RS1, 8, data);
        i64 value  = data;
        i64 result = value | (i64)RS2;
        MW(RS1, 8, result);
        RD = value;
    });
    INSTEXE(amoand_d, {
        usize data;
        MR(RS1, 8, data);
        i64 value  = data;
        i64 result = value & (i64)RS2;
        MW(RS1, 8, result);
        RD = value;
    });
    INSTEXE(amomin_d, {
        usize data;
        MR(RS1, 8, data);
        i64 value  = data;
        i64 result = (value < (i64)RS2) ? value : RS2;
        MW(RS1, 8, result);
        RD = value;
    });
    INSTEXE(amomax_d, {
        usize data;
        MR(RS1, 8, data);
        i64 value  = data;
        i64 result = (value > (i64)RS2) ? value : RS2;
        MW(RS1, 8, result);
        RD = value;
    });
    INSTEXE(amominu_d, {
        usize data;
        MR(RS1, 8, data);
        u64 value  = data;
        u64 result = (value < (u64)RS2) ? value : RS2;
        MW(RS1, 8, result);
        RD = value;
    });
    INSTEXE(amomaxu_d, {
        usize data;
        MR(RS1, 8, data);
        u64 value  = data;
        u64 result = (value > (u64)RS2) ? value : RS2;
        MW(RS1, 8, result);
        RD = value;
    });
#endif

    INSTEND();

    core->regs[0] = 0;
}

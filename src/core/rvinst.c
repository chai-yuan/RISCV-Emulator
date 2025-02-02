#include "core.h"
#include "debug.h"
#include "utils.h"

void inst_add(struct RiscvCore *core) { RD = RS1 + RS2; }
void inst_sub(struct RiscvCore *core) { RD = RS1 - RS2; }
void inst_xor(struct RiscvCore *core) { RD = RS1 ^ RS2; }
void inst_or(struct RiscvCore *core) { RD = RS1 | RS2; }
void inst_and(struct RiscvCore *core) { RD = RS1 & RS2; }
void inst_sll(struct RiscvCore *core) { RD = RS1 << (RS2 & 0x3F); }
void inst_slt(struct RiscvCore *core) { RD = (isize)RS1 < (isize)RS2 ? 1 : 0; }
void inst_sltu(struct RiscvCore *core) { RD = RS1 < RS2 ? 1 : 0; }
void inst_srl(struct RiscvCore *core) { RD = RS1 >> (RS2 & 0x3F); }
void inst_sra(struct RiscvCore *core) { RD = (isize)RS1 >> (RS2 & 0x3F); }
void inst_addi(struct RiscvCore *core) { RD = RS1 + (isize)DEC.immI; }
void inst_xori(struct RiscvCore *core) { RD = RS1 ^ (isize)DEC.immI; }
void inst_ori(struct RiscvCore *core) { RD = RS1 | (usize)DEC.immI; }
void inst_andi(struct RiscvCore *core) { RD = RS1 & (usize)DEC.immI; }
void inst_slli(struct RiscvCore *core) { RD = RS1 << (DEC.immI & 0x3F); }
void inst_srli(struct RiscvCore *core) { RD = RS1 >> (DEC.immI & 0x3F); }
void inst_srai(struct RiscvCore *core) { RD = (isize)RS1 >> (DEC.immI & 0x3F); }
void inst_slti(struct RiscvCore *core) { RD = (isize)RS1 < (isize)DEC.immI ? 1 : 0; }
void inst_sltiu(struct RiscvCore *core) { RD = RS1 < (usize)DEC.immI ? 1 : 0; }
void inst_lb(struct RiscvCore *core) {
    usize data;
    MR(RS1 + DEC.immI, 1, data);
    RD = (i8)data;
}
void inst_lh(struct RiscvCore *core) {
    usize data;
    MR(RS1 + DEC.immI, 2, data);
    RD = (i16)data;
}
void inst_lw(struct RiscvCore *core) {
    usize data;
    MR(RS1 + DEC.immI, 4, data);
    RD = (i32)data;
}
void inst_lbu(struct RiscvCore *core) { MR(RS1 + DEC.immI, 1, RD); }
void inst_lhu(struct RiscvCore *core) { MR(RS1 + DEC.immI, 2, RD); }
void inst_sb(struct RiscvCore *core) { MW(RS1 + DEC.immS, 1, RS2); }
void inst_sh(struct RiscvCore *core) { MW(RS1 + DEC.immS, 2, RS2); }
void inst_sw(struct RiscvCore *core) { MW(RS1 + DEC.immS, 4, RS2); }
void inst_beq(struct RiscvCore *core) {
    if ((isize)RS1 == (isize)RS2)
        DEC.next_pc = core->pc + DEC.immB;
}
void inst_bne(struct RiscvCore *core) {
    if ((isize)RS1 != (isize)RS2)
        DEC.next_pc = core->pc + DEC.immB;
}
void inst_blt(struct RiscvCore *core) {
    if ((isize)RS1 < (isize)RS2)
        DEC.next_pc = core->pc + DEC.immB;
}
void inst_bge(struct RiscvCore *core) {
    if ((isize)RS1 >= (isize)RS2)
        DEC.next_pc = core->pc + DEC.immB;
}
void inst_bltu(struct RiscvCore *core) {
    if (RS1 < RS2)
        DEC.next_pc = core->pc + DEC.immB;
}
void inst_bgeu(struct RiscvCore *core) {
    if (RS1 >= RS2)
        DEC.next_pc = core->pc + DEC.immB;
}
void inst_jal(struct RiscvCore *core) {
    DEC.next_pc = core->pc + DEC.immJ;
    RD          = core->pc + 4;
}
void inst_jalr(struct RiscvCore *core) {
    DEC.next_pc = (RS1 + DEC.immI) & ~1;
    RD          = core->pc + 4;
}
void inst_lui(struct RiscvCore *core) { RD = DEC.immU; }
void inst_auipc(struct RiscvCore *core) { RD = core->pc + DEC.immU; }
void inst_mul(struct RiscvCore *core) { RD = RS1 * RS2; }
void inst_mulh(struct RiscvCore *core) {
    if (sizeof(usize) == 4) {
        RD = ((i64)(i32)RS1 * (i64)(i32)RS2) >> 32;
    } else {
        struct uint128 a = signed2u128(RS1), b = signed2u128(RS2);
        struct uint128 ans = u128mul(a, b);
        RD                 = ((u64)ans.nums[3] << 32) | (u64)ans.nums[2];
    }
}
void inst_mulhsu(struct RiscvCore *core) {
    if (sizeof(usize) == 4) {
        RD = ((i64)(i32)RS1 * (u64)RS2) >> 32;
    } else {
        struct uint128 a = signed2u128(RS1), b = unsigned2u128(RS2);
        struct uint128 ans = u128mul(a, b);
        RD                 = ((u64)ans.nums[3] << 32) | (u64)ans.nums[2];
    }
}
void inst_mulhu(struct RiscvCore *core) {
    if (sizeof(usize) == 4) {
        RD = ((u64)RS1 * (u64)RS2) >> 32;
    } else {
        struct uint128 a = unsigned2u128(RS1), b = unsigned2u128(RS2);
        struct uint128 ans = u128mul(a, b);
        RD                 = ((u64)ans.nums[3] << 32) | (u64)ans.nums[2];
    }
}
void inst_div(struct RiscvCore *core) {
    isize src1 = RS1;
    isize src2 = RS2;
    if (src2 == 0) {
        RD = (isize)-1;
    } else if (src1 == INT_MIN && src2 == -1) {
        RD = src1;
    } else {
        RD = src1 / src2;
    }
}
void inst_divu(struct RiscvCore *core) {
    usize src1 = RS1;
    usize src2 = RS2;
    if (src2 == 0) {
        RD = (isize)-1;
    } else {
        RD = src1 / src2;
    }
}
void inst_rem(struct RiscvCore *core) {
    isize src1 = RS1;
    isize src2 = RS2;
    if (src2 == 0) {
        RD = src1;
    } else if (src1 == INT_MIN && src2 == -1) {
        RD = 0;
    } else {
        RD = src1 % src2;
    }
}
void inst_remu(struct RiscvCore *core) {
    usize src1 = RS1;
    usize src2 = RS2;
    if (src2 == 0) {
        RD = src1;
    } else {
        RD = src1 % src2;
    }
}
void inst_lr_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    RD                      = (isize)data;
    core->reservation_valid = true;
    core->reservation_addr  = RS1;
}
void inst_sc_w(struct RiscvCore *core) {
    if (core->reservation_valid && core->reservation_addr == RS1) {
        MW(RS1, 4, (u32)RS2);
        RD = 0;
    } else {
        RD = 1;
    }
    core->reservation_valid = false;
}
void inst_amoswap_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    MW(RS1, 4, (u32)RS2);
    RD = (isize)(i32)data;
}

void inst_amoadd_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    i32 value  = (i32)data;
    i32 result = value + (i32)RS2;
    MW(RS1, 4, (u32)result);
    RD = (isize)value;
}

void inst_amoxor_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    i32 value  = (i32)data;
    i32 result = value ^ (i32)RS2;
    MW(RS1, 4, (u32)result);
    RD = (isize)value;
}

void inst_amoor_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    i32 value  = (i32)data;
    i32 result = value | (i32)RS2;
    MW(RS1, 4, (u32)result);
    RD = (isize)value;
}

void inst_amoand_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    i32 value  = (i32)data;
    i32 result = value & (i32)RS2;
    MW(RS1, 4, (u32)result);
    RD = (isize)value;
}

void inst_amomin_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    i32 value  = (i32)data;
    i32 result = (value < (i32)RS2) ? value : (i32)RS2;
    MW(RS1, 4, (u32)result);
    RD = (isize)value;
}

void inst_amomax_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    i32 value  = (i32)data;
    i32 result = (value > (i32)RS2) ? value : (i32)RS2;
    MW(RS1, 4, (u32)result);
    RD = (isize)value;
}

void inst_amominu_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    u32 value  = (i32)data;
    u32 result = (value < (u32)RS2) ? value : (u32)RS2;
    MW(RS1, 4, result);
    RD = (isize)(i32)value;
}

void inst_amomaxu_w(struct RiscvCore *core) {
    usize data;
    MR(RS1, 4, data);
    u32 value  = (i32)data;
    u32 result = (value > (u32)RS2) ? value : (u32)RS2;
    MW(RS1, 4, result);
    RD = (isize)(i32)value;
}

void inst_csrrw(struct RiscvCore *core) {
    if (((DEC.csr_imm >> 8) & 0x3) > core->mode) {
        DEC.exception     = ILLEGAL_INSTRUCTION;
        DEC.exception_val = DEC.inst;
    } else {
        usize data = CSRR(DEC.csr_imm);
        CSRW(DEC.csr_imm, RS1);
        RD = data;
    }
}

void inst_csrrs(struct RiscvCore *core) {
    if (((DEC.csr_imm >> 8) & 0x3) > core->mode) {
        DEC.exception     = ILLEGAL_INSTRUCTION;
        DEC.exception_val = DEC.inst;
    } else {
        usize data = CSRR(DEC.csr_imm);
        CSRW(DEC.csr_imm, data | RS1);
        RD = data;
    }
}

void inst_csrrc(struct RiscvCore *core) {
    if (((DEC.csr_imm >> 8) & 0x3) > core->mode) {
        DEC.exception     = ILLEGAL_INSTRUCTION;
        DEC.exception_val = DEC.inst;
    } else {
        usize data = CSRR(DEC.csr_imm);
        CSRW(DEC.csr_imm, data & ~RS1);
        RD = data;
    }
}

void inst_csrrwi(struct RiscvCore *core) {
    if (((DEC.csr_imm >> 8) & 0x3) > core->mode) {
        DEC.exception     = ILLEGAL_INSTRUCTION;
        DEC.exception_val = DEC.inst;
    } else {
        usize data = CSRR(DEC.csr_imm);
        CSRW(DEC.csr_imm, DEC.rs1);
        RD = data;
    }
}

void inst_csrrsi(struct RiscvCore *core) {
    if (((DEC.csr_imm >> 8) & 0x3) > core->mode) {
        DEC.exception     = ILLEGAL_INSTRUCTION;
        DEC.exception_val = DEC.inst;
    } else {
        usize data = CSRR(DEC.csr_imm);
        CSRW(DEC.csr_imm, data | DEC.rs1);
        RD = data;
    }
}

void inst_csrrci(struct RiscvCore *core) {
    if (((DEC.csr_imm >> 8) & 0x3) > core->mode) {
        DEC.exception     = ILLEGAL_INSTRUCTION;
        DEC.exception_val = DEC.inst;
    } else {
        usize data = CSRR(DEC.csr_imm);
        CSRW(DEC.csr_imm, data & ~DEC.rs1);
        RD = data;
    }
}

void inst_fence(struct RiscvCore *core) {}
void inst_fence_i(struct RiscvCore *core) {}
void inst_sfence_vma(struct RiscvCore *core) {}

void inst_c_andi(struct RiscvCore *core) {
    isize imm = ((DEC.inst >> 12) & 0x1) << 5 | ((DEC.inst >> 2) & 0x1f);
    imm |= (imm & 0x20) ? ~0x1f : 0;
    core->regs[DEC.rs1_] = (isize)core->regs[DEC.rs1_] & imm;
}
void inst_c_lui(struct RiscvCore *core) {
    isize imm = ((DEC.inst >> 2) & 0x1f) << 12 | ((DEC.inst >> 12) & 0x1) << 17;
    imm |= (imm & 0x20000) ? ~0x1ffff : 0;
    core->regs[DEC.rd] = imm;
}
void inst_c_addi16sp(struct RiscvCore *core) {
    isize imm = ((DEC.inst >> 12) & 0x1) << 9 | ((DEC.inst >> 2) & 0x1) << 5 |
                ((DEC.inst >> 3) & 0x3) << 7 | ((DEC.inst >> 5) & 0x1) << 6 |
                ((DEC.inst >> 6) & 0x1) << 4;
    imm |= (imm & 0x200) ? ~0x1ff : 0;
    if (imm != 0)
        core->regs[2] = ((isize)core->regs[2] + imm);
}
void inst_c_j(struct RiscvCore *core) {
    u32 offset = ((DEC.inst >> 2) & 0x1) << 5 | ((DEC.inst >> 3) & 0x7) << 1 |
                 ((DEC.inst >> 6) & 0x1) << 7 | ((DEC.inst >> 7) & 0x1) << 6 |
                 ((DEC.inst >> 8) & 0x1) << 10 | ((DEC.inst >> 9) & 0x3) << 8 |
                 ((DEC.inst >> 11) & 0x1) << 4 | ((DEC.inst >> 12) & 0x1) << 11;
    offset |= (offset & 0x800) ? ~0x3ff : 0;
    DEC.next_pc = core->pc + offset;
}
void inst_c_jal(struct RiscvCore *core) {
    u32 offset = ((DEC.inst >> 2) & 0x1) << 5 | ((DEC.inst >> 3) & 0x7) << 1 |
                 ((DEC.inst >> 6) & 0x1) << 7 | ((DEC.inst >> 7) & 0x1) << 6 |
                 ((DEC.inst >> 8) & 0x1) << 10 | ((DEC.inst >> 9) & 0x3) << 8 |
                 ((DEC.inst >> 11) & 0x1) << 4 | ((DEC.inst >> 12) & 0x1) << 11;
    offset |= (offset & 0x800) ? ~0x3ff : 0;
    core->regs[1] = core->pc + 2;
    DEC.next_pc   = core->pc + offset;
}
void inst_c_beqz(struct RiscvCore *core) {
    u32 offset = ((DEC.inst >> 2) & 0x1) << 5 | ((DEC.inst >> 3) & 0x3) << 1 |
                 ((DEC.inst >> 5) & 0x3) << 6 | ((DEC.inst >> 10) & 0x3) << 3 |
                 ((DEC.inst >> 12) & 0x1) << 8;
    offset |= (offset & 0x100) ? ~0xff : 0;
    if (core->regs[DEC.rs1_] == 0)
        DEC.next_pc = core->pc + offset;
}
void inst_c_bnez(struct RiscvCore *core) {
    u32 offset = ((DEC.inst >> 2) & 0x1) << 5 | ((DEC.inst >> 3) & 0x3) << 1 |
                 ((DEC.inst >> 5) & 0x3) << 6 | ((DEC.inst >> 10) & 0x3) << 3 |
                 ((DEC.inst >> 12) & 0x1) << 8;
    offset |= (offset & 0x100) ? ~0xff : 0;
    if (core->regs[DEC.rs1_] != 0)
        DEC.next_pc = core->pc + offset;
}
void inst_c_li(struct RiscvCore *core) {
    isize imm = ((DEC.inst >> 12) & 0x1) << 5 | ((DEC.inst >> 2) & 0x1f);
    imm |= (imm & 0x20) ? ~0x1f : 0;
    core->regs[DEC.rd] = imm;
}
void inst_c_nop(struct RiscvCore *core) {}
void inst_c_lwsp(struct RiscvCore *core) {
    usize offset =
        ((DEC.inst >> 2) & 0x3) << 6 | ((DEC.inst >> 4) & 0x7) << 2 | ((DEC.inst >> 12) & 0x1) << 5;
    usize data;
    MR(core->regs[2] + offset, 4, data);
    RD = (isize)data;
}
void inst_c_swsp(struct RiscvCore *core) {
    usize offset = ((DEC.inst >> 7) & 0x3) << 6 | ((DEC.inst >> 9) & 0xf) << 2;
    MW(core->regs[2] + offset, 4, RS2);
}
void inst_c_lw(struct RiscvCore *core) {
    usize imm =
        ((DEC.inst >> 5) & 0x1) << 6 | ((DEC.inst >> 6) & 0x1) << 2 | ((DEC.inst >> 10) & 0x7) << 3;
    usize data;
    MR(core->regs[DEC.rs1_] + imm, 4, data);
    core->regs[DEC.rs2_] = (i32)data;
}
void inst_c_sw(struct RiscvCore *core) {
    usize imm =
        ((DEC.inst >> 5) & 0x1) << 6 | ((DEC.inst >> 6) & 0x1) << 2 | ((DEC.inst >> 10) & 0x7) << 3;
    MW(core->regs[DEC.rs1_] + imm, 4, core->regs[DEC.rs2_]);
}
void inst_c_jalr(struct RiscvCore *core) {
    DEC.next_pc   = core->regs[DEC.rs1];
    core->regs[1] = core->pc + 2;
}
void inst_c_jr(struct RiscvCore *core) { DEC.next_pc = core->regs[DEC.rs1]; }
void inst_c_addi(struct RiscvCore *core) {
    isize imm = ((DEC.inst >> 12) & 0x1) << 5 | ((DEC.inst >> 2) & 0x1f);
    imm |= (imm & 0x20) ? ~0x1f : 0;
    if (imm != 0)
        core->regs[DEC.rd] = (u32)((isize)core->regs[DEC.rd] + imm);
}
void inst_c_addi4spn(struct RiscvCore *core) {
    usize imm = ((DEC.inst >> 5) & 0x1) << 3 | ((DEC.inst >> 6) & 0x1) << 2 |
                ((DEC.inst >> 7) & 0xf) << 6 | ((DEC.inst >> 11) & 0x3) << 4;
    if (imm != 0)
        core->regs[DEC.rs2_] = core->regs[2] + imm;
}
void inst_c_slli(struct RiscvCore *core) {
    u32 shamt = ((DEC.inst >> 12) & 0x1) << 5 | ((DEC.inst >> 2) & 0x1f);
    RD        = RS1 << shamt;
}
void inst_c_srli(struct RiscvCore *core) {
    u32 shamt = ((DEC.inst >> 12) & 0x1) << 5 | ((DEC.inst >> 2) & 0x1f);
    core->regs[DEC.rs1_] >>= shamt;
}
void inst_c_srai(struct RiscvCore *core) {
    u32 shamt            = ((DEC.inst >> 12) & 0x1) << 5 | ((DEC.inst >> 2) & 0x1f);
    core->regs[DEC.rs1_] = (isize)core->regs[DEC.rs1_] >> shamt;
}
void inst_c_mv(struct RiscvCore *core) { RD = RS2; }
void inst_c_add(struct RiscvCore *core) { core->regs[DEC.rs1] += core->regs[DEC.rs2]; }
void inst_c_and(struct RiscvCore *core) { core->regs[DEC.rs1_] &= core->regs[DEC.rs2_]; }
void inst_c_or(struct RiscvCore *core) { core->regs[DEC.rs1_] |= core->regs[DEC.rs2_]; }
void inst_c_xor(struct RiscvCore *core) { core->regs[DEC.rs1_] ^= core->regs[DEC.rs2_]; }
void inst_c_sub(struct RiscvCore *core) { core->regs[DEC.rs1_] -= core->regs[DEC.rs2_]; }

void inst_c_ebreak(struct RiscvCore *core) { DEC.exception = BREAKPOINT; }

void inst_ecall(struct RiscvCore *core) {
    if (core->mode == USER) {
        DEC.exception = ECALL_FROM_UMODE;
    } else if (core->mode == SUPERVISOR) {
        DEC.exception = ECALL_FROM_SMODE;
    } else if (core->mode == MACHINE) {
        DEC.exception = ECALL_FROM_MMODE;
    }
}
void inst_sret(struct RiscvCore *core) {
    core->mode = ((CSRR(SSTATUS) >> 8) & 1) == 1 ? SUPERVISOR : USER;
    CSRW(SSTATUS,
         ((CSRR(SSTATUS) >> 5) & 1) == 1 ? CSRR(SSTATUS) | (1 << 1) : CSRR(SSTATUS) & ~(1 << 1));
    CSRW(SSTATUS, CSRR(SSTATUS) | (1 << 5));
    CSRW(SSTATUS, CSRR(SSTATUS) & ~(1 << 8));
    DEC.next_pc = CSRR(SEPC);
}
void inst_mret(struct RiscvCore *core) {
    u64 mpp    = (CSRR(MSTATUS) >> 11) & 3;
    core->mode = mpp == 3 ? MACHINE : (mpp == 1 ? SUPERVISOR : USER);
    CSRW(MSTATUS,
         (((CSRR(MSTATUS) >> 7) & 1) == 1) ? CSRR(MSTATUS) | (1 << 3) : CSRR(MSTATUS) & ~(1 << 3));
    CSRW(MSTATUS, CSRR(MSTATUS) | (1 << 7));
    CSRW(MSTATUS, CSRR(MSTATUS) & ~(3 << 11));
    if (mpp != 3)
        CSRW(MSTATUS, CSRR(MSTATUS) & ~(1 << 17));
    DEC.next_pc = CSRR(MEPC);
}
void inst_ebeark(struct RiscvCore *core) { DEC.exception = BREAKPOINT; }

void inst_addw(struct RiscvCore *core) { RD = (i32)(RS1 + RS2); }
void inst_subw(struct RiscvCore *core) { RD = (i32)(RS1 - RS2); }
void inst_sllw(struct RiscvCore *core) { RD = (i32)(RS1 << (RS2 & 0x1F)); }
void inst_srlw(struct RiscvCore *core) { RD = (i32)((u32)RS1 >> (RS2 & 0x1F)); }
void inst_sraw(struct RiscvCore *core) { RD = (i32)RS1 >> (RS2 & 0x1F); }
void inst_addiw(struct RiscvCore *core) { RD = (RS1 + (i64)DEC.immI); }
void inst_slliw(struct RiscvCore *core) { RD = (i32)(RS1 << (DEC.immI & 0x1F)); }
void inst_srliw(struct RiscvCore *core) { RD = (i32)((u32)RS1 >> (DEC.immI & 0x1F)); }
void inst_sraiw(struct RiscvCore *core) { RD = (i32)RS1 >> (DEC.immI & 0x1F); }
void inst_lwu(struct RiscvCore *core) { MR(RS1 + DEC.immI, 4, RD); }
void inst_ld(struct RiscvCore *core) { MR(RS1 + DEC.immI, 8, RD); }
void inst_sd(struct RiscvCore *core) { MW(RS1 + DEC.immS, 8, RS2); }

void inst_inv(struct RiscvCore *core) {
    DEC.exception     = ILLEGAL_INSTRUCTION;
    DEC.exception_val = DEC.inst;
    ERROR("Unknow instruction : %x", DEC.inst);
}

struct Instruction instructions[] = {
    {.mask = 0xfe00707f, .match = 0x33, .func = inst_add},
    {.mask = 0xfe00707f, .match = 0x40000033, .func = inst_sub},
    {.mask = 0xfe00707f, .match = 0x4033, .func = inst_xor},
    {.mask = 0xfe00707f, .match = 0x6033, .func = inst_or},
    {.mask = 0xfe00707f, .match = 0x7033, .func = inst_and},
    {.mask = 0xfe00707f, .match = 0x1033, .func = inst_sll},
    {.mask = 0xfe00707f, .match = 0x2033, .func = inst_slt},
    {.mask = 0xfe00707f, .match = 0x3033, .func = inst_sltu},
    {.mask = 0xfe00707f, .match = 0x5033, .func = inst_srl},
    {.mask = 0xfe00707f, .match = 0x40005033, .func = inst_sra},
    {.mask = 0x707f, .match = 0x13, .func = inst_addi},
    {.mask = 0x707f, .match = 0x4013, .func = inst_xori},
    {.mask = 0x707f, .match = 0x6013, .func = inst_ori},
    {.mask = 0x707f, .match = 0x7013, .func = inst_andi},
    {.mask = 0xfc00707f, .match = 0x1013, .func = inst_slli},
    {.mask = 0xfc00707f, .match = 0x5013, .func = inst_srli},
    {.mask = 0xfc00707f, .match = 0x40005013, .func = inst_srai},
    {.mask = 0x707f, .match = 0x2013, .func = inst_slti},
    {.mask = 0x707f, .match = 0x3013, .func = inst_sltiu},
    {.mask = 0x707f, .match = 0x3, .func = inst_lb},
    {.mask = 0x707f, .match = 0x1003, .func = inst_lh},
    {.mask = 0x707f, .match = 0x2003, .func = inst_lw},
    {.mask = 0x707f, .match = 0x4003, .func = inst_lbu},
    {.mask = 0x707f, .match = 0x5003, .func = inst_lhu},
    {.mask = 0x707f, .match = 0x23, .func = inst_sb},
    {.mask = 0x707f, .match = 0x1023, .func = inst_sh},
    {.mask = 0x707f, .match = 0x2023, .func = inst_sw},
    {.mask = 0x707f, .match = 0x67, .func = inst_jalr},
    {.mask = 0x707f, .match = 0x63, .func = inst_beq},
    {.mask = 0x707f, .match = 0x5063, .func = inst_bge},
    {.mask = 0x707f, .match = 0x7063, .func = inst_bgeu},
    {.mask = 0x707f, .match = 0x4063, .func = inst_blt},
    {.mask = 0x707f, .match = 0x6063, .func = inst_bltu},
    {.mask = 0x707f, .match = 0x1063, .func = inst_bne},
    {.mask = 0x7f, .match = 0x37, .func = inst_lui},
    {.mask = 0x7f, .match = 0x6f, .func = inst_jal},
    {.mask = 0x7f, .match = 0x17, .func = inst_auipc},

    {.mask = 0xef83, .match = 0x6101, .func = inst_c_addi16sp},
    {.mask = 0xef83, .match = 0x1, .func = inst_c_nop},
    {.mask = 0xec03, .match = 0x8001, .func = inst_c_srli},
    {.mask = 0xec03, .match = 0x8401, .func = inst_c_srai},
    {.mask = 0xec03, .match = 0x8801, .func = inst_c_andi},
    {.mask = 0xffff, .match = 0x9002, .func = inst_c_ebreak},
    {.mask = 0xfc63, .match = 0x8c01, .func = inst_c_sub},
    {.mask = 0xfc63, .match = 0x8c21, .func = inst_c_xor},
    {.mask = 0xfc63, .match = 0x8c41, .func = inst_c_or},
    {.mask = 0xfc63, .match = 0x8c61, .func = inst_c_and},
    {.mask = 0xf07f, .match = 0x8002, .func = inst_c_jr},
    {.mask = 0xf07f, .match = 0x9002, .func = inst_c_jalr},
    {.mask = 0xe003, .match = 0x0, .func = inst_c_addi4spn},
    {.mask = 0xe003, .match = 0x1, .func = inst_c_addi},
    {.mask = 0xe003, .match = 0x4000, .func = inst_c_lw},
    {.mask = 0xe003, .match = 0xc000, .func = inst_c_sw},
    {.mask = 0xe003, .match = 0xa001, .func = inst_c_j},
    {.mask = 0xe003, .match = 0x2001, .func = inst_c_jal},
    {.mask = 0xe003, .match = 0xc001, .func = inst_c_beqz},
    {.mask = 0xe003, .match = 0xe001, .func = inst_c_bnez},
    {.mask = 0xe003, .match = 0x4001, .func = inst_c_li},
    {.mask = 0xe003, .match = 0x6001, .func = inst_c_lui},
    {.mask = 0xe003, .match = 0xc002, .func = inst_c_swsp},
    {.mask = 0xe003, .match = 0x4002, .func = inst_c_lwsp},
    {.mask = 0xe003, .match = 0x2, .func = inst_c_slli},
    {.mask = 0xf003, .match = 0x9002, .func = inst_c_add},
    {.mask = 0xf003, .match = 0x8002, .func = inst_c_mv},

    {.mask = 0xfe00707f, .match = 0x2000033, .func = inst_mul},
    {.mask = 0xfe00707f, .match = 0x2001033, .func = inst_mulh},
    {.mask = 0xfe00707f, .match = 0x2002033, .func = inst_mulhsu},
    {.mask = 0xfe00707f, .match = 0x2003033, .func = inst_mulhu},
    {.mask = 0xfe00707f, .match = 0x2004033, .func = inst_div},
    {.mask = 0xfe00707f, .match = 0x2005033, .func = inst_divu},
    {.mask = 0xfe00707f, .match = 0x2006033, .func = inst_rem},
    {.mask = 0xfe00707f, .match = 0x2007033, .func = inst_remu},

    {.mask = 0xf800707f, .match = 0x1000202f, .func = inst_lr_w},
    {.mask = 0xf800707f, .match = 0x1800202f, .func = inst_sc_w},
    {.mask = 0xf800707f, .match = 0x0800202f, .func = inst_amoswap_w},
    {.mask = 0xf800707f, .match = 0x0000202f, .func = inst_amoadd_w},
    {.mask = 0xf800707f, .match = 0x2000202f, .func = inst_amoxor_w},
    {.mask = 0xf800707f, .match = 0x4000202f, .func = inst_amoor_w},
    {.mask = 0xf800707f, .match = 0x6000202f, .func = inst_amoand_w},
    {.mask = 0xf800707f, .match = 0x8000202f, .func = inst_amomin_w},
    {.mask = 0xf800707f, .match = 0xa000202f, .func = inst_amomax_w},
    {.mask = 0xf800707f, .match = 0xc000202f, .func = inst_amominu_w},
    {.mask = 0xf800707f, .match = 0xe000202f, .func = inst_amomaxu_w},

    {.mask = 0x0000707f, .match = 0x00001073, .func = inst_csrrw},
    {.mask = 0x0000707f, .match = 0x00002073, .func = inst_csrrs},
    {.mask = 0x0000707f, .match = 0x00003073, .func = inst_csrrc},
    {.mask = 0x0000707f, .match = 0x00005073, .func = inst_csrrwi},
    {.mask = 0x0000707f, .match = 0x00006073, .func = inst_csrrsi},
    {.mask = 0x0000707f, .match = 0x00007073, .func = inst_csrrci},

    {.mask = 0x0000707f, .match = 0x0000000f, .func = inst_fence},
    {.mask = 0x0000707f, .match = 0x0000100f, .func = inst_fence_i},
    {.mask = 0xfe007fff, .match = 0x12000073, .func = inst_sfence_vma},

    {.mask = 0xffffffff, .match = 0x00000073, .func = inst_ecall},
    {.mask = 0xffffffff, .match = 0x10200073, .func = inst_sret},
    {.mask = 0xffffffff, .match = 0x30200073, .func = inst_mret},
    {.mask = 0xffffffff, .match = 0x100073, .func = inst_ebeark},

#if CURRENT_ARCH == ARCH_RV64
    {.mask = 0xfe00707f, .match = 0x3b, .func = inst_addw},
   {.mask = 0xfe00707f, .match = 0x4000003b, .func = inst_subw},
    {.mask = 0xfe00707f, .match = 0x103b, .func = inst_sllw},
    {.mask = 0xfe00707f, .match = 0x503b, .func = inst_srlw},
    {.mask = 0xfe00707f, .match = 0x4000503b, .func = inst_sraw},
    {.mask = 0xfe00707f, .match = 0x101b, .func = inst_slliw},
    {.mask = 0xfe00707f, .match = 0x501b, .func = inst_srliw},
    {.mask = 0xfe00707f, .match = 0x4000501b, .func = inst_sraiw},
    {.mask = 0x707f, .match = 0x1b, .func = inst_addiw},
    {.mask = 0x707f, .match = 0x6003, .func = inst_lwu},
    {.mask = 0x707f, .match = 0x3003, .func = inst_ld},
    {.mask = 0x707f, .match = 0x3023, .func = inst_sd},
#endif

    {.mask = 0x0, .match = 0x0, .func = inst_inv},
};

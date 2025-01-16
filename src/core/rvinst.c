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
void inst_ebeark(struct RiscvCore *core) { DEC.exception = BREAKPOINT; }
void inst_inv(struct RiscvCore *core) {
    DEC.exception     = ILLEGAL_INSTRUCTION;
    DEC.exception_val = DEC.inst_raw;
    ERROR("Unknow instruction : %x", DEC.inst_raw);
}

struct Instruction instructions[] = {
    {.mask = 0xfe00707f, .match = 0x33, .func = inst_add},
    {.mask = 0xfc63, .match = 0x8c01, .func = inst_sub},
    {.mask = 0xfc63, .match = 0x8c21, .func = inst_xor},
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
    {.mask = 0x7f, .match = 0x6f, .func = inst_jal},
    {.mask = 0x707f, .match = 0x67, .func = inst_jalr},
    {.mask = 0x707f, .match = 0x63, .func = inst_beq},
    {.mask = 0x707f, .match = 0x5063, .func = inst_bge},
    {.mask = 0x707f, .match = 0x7063, .func = inst_bgeu},
    {.mask = 0x707f, .match = 0x4063, .func = inst_blt},
    {.mask = 0x707f, .match = 0x6063, .func = inst_bltu},
    {.mask = 0x707f, .match = 0x1063, .func = inst_bne},
    {.mask = 0x7f, .match = 0x17, .func = inst_auipc},

    {.mask = 0xffffffff, .match = 0x100073, .func = inst_ebeark},
    {.mask = 0x0, .match = 0x0, .func = inst_inv}, // 无效指令
};

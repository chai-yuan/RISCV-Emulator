#ifndef RV_DECODE_H
#define RV_DECODE_H

#include "common.h"

enum exception {
    EXC_NONE = -1,
    INSTRUCTION_ADDRESS_MISALIGNED = 0,
    INSTRUCTION_ACCESS_FAULT = 1,
    ILLEGAL_INSTRUCTION = 2,
    BREAKPOINT = 3,
    LOAD_ADDRESS_MISALIGNED = 4,
    LOAD_ACCESS_FAULT = 5,
    STORE_AMO_ADDRESS_MISALIGNED = 6,
    STORE_AMO_ACCESS_FAULT = 7,
    ECALL_FROM_UMODE = 8,
    ECALL_FROM_SMODE = 9,
    ECALL_FROM_MMODE = 11,
    INSTRUCTION_PAGE_FAULT = 12,
    LOAD_PAGE_FAULT = 13,
    STORE_AMO_PAGE_FAULT = 15,
};

enum interrupt {
    INT_NONE = -1,
    USER_SOFTWARE_INTERRUPT = 0,
    SUPERVISOR_SOFTWARE_INTERRUPT = 1,
    MACHINE_SOFTWARE_INTERRUPT = 3,
    USER_TIMER_INTERRUPT = 4,
    SUPERVISOR_TIMER_INTERRUPT = 5,
    MACHINE_TIMER_INTERRUPT = 7,
    USER_EXTERNAL_INTERRUPT = 8,
    SUPERVISOR_EXTERNAL_INTERRUPT = 9,
    MACHINE_EXTERNAL_INTERRUPT = 11,
};

enum instruction {
    // I 类型指令
    inst_add,
    inst_addw,
    inst_sub,
    inst_subw,
    inst_xor,
    inst_or,
    inst_and,
    inst_sll,
    inst_sllw,
    inst_slt,
    inst_sltu,
    inst_srl,
    inst_srlw,
    inst_sra,
    inst_sraw,
    inst_addi,
    inst_addiw,
    inst_xori,
    inst_ori,
    inst_andi,
    inst_slli,
    inst_slliw,
    inst_srli,
    inst_srliw,
    inst_srai,
    inst_sraiw,
    inst_slti,
    inst_sltiu,
    inst_lb,
    inst_lh,
    inst_lw,
    inst_lbu,
    inst_lhu,
    inst_lwu,
    inst_ld,
    inst_sb,
    inst_sh,
    inst_sw,
    inst_sd,
    inst_beq,
    inst_bne,
    inst_blt,
    inst_bge,
    inst_bltu,
    inst_bgeu,
    inst_jal,
    inst_jalr,
    inst_lui,
    inst_auipc,
    // M 类型指令
    inst_mul,
    inst_mulh,
    inst_mulsu,
    inst_mulu,
    inst_div,
    inst_divu,
    inst_rem,
    inst_remu,
    // A 类型指令
    inst_lr_w,
    inst_sc_w,
    inst_amoswap_w,
    inst_amoadd_w,
    inst_amoxor_w,
    inst_amoor_w,
    inst_amoand_w,
    inst_amomin_w,
    inst_amomax_w,
    inst_amominu_w,
    inst_amomaxu_w,
    // Zicsr 类型指令
    inst_csrrw,
    inst_csrrs,
    inst_csrrc,
    inst_csrrwi,
    inst_csrrsi,
    inst_csrrci,
    // Zifence 类型指令
    inst_fence,
    inst_fence_i,
    // 特权指令
    inst_wfi,
    inst_ecall,
    inst_mret,
    inst_ebreak,
    // 无效指令
    inst_inv
};

struct RiscvDecode {
    u32 inst_raw;
    enum instruction inst;
    u32 rd, rs1, rs2;
    i64 immI, immB, immU, immJ, immS;
    u64 next_pc;
    u64 access_addr;
    enum exception exception;
    enum interrupt interrupt;
};

void riscv_decode(struct RiscvDecode *decode);

#endif

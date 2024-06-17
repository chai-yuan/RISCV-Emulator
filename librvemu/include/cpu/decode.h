#ifndef DECODE_H
#define DECODE_H

#include "common/common.h"
#include "common/debug.h"
#include "cpu/except.h"
#include "device/device.h"
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    inst_inv,
    // 32I
    inst_add,
    inst_sub,
    inst_xor,
    inst_or,
    inst_and,
    inst_sll,
    inst_slt,
    inst_sltu,
    inst_srl,
    inst_sra,
    inst_addi,
    inst_xori,
    inst_ori,
    inst_andi,
    inst_slli,
    inst_srli,
    inst_srai,
    inst_slti,
    inst_sltiu,
    inst_lb,
    inst_lh,
    inst_lw,
    inst_lbu,
    inst_lhu,
    inst_sb,
    inst_sh,
    inst_sw,
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
    // 32M
    inst_mul,
    inst_mulh,
    inst_mulsu,
    inst_mulu,
    inst_div,
    inst_divu,
    inst_rem,
    inst_remu,
    // 32A
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
    // Zicsr
    inst_csrrw,
    inst_csrrs,
    inst_csrrc,
    inst_csrrwi,
    inst_csrrsi,
    inst_csrrci,
    // Zifence
    inst_fence,
    inst_fence_i,
    // 特权指令
    inst_wfi,
    inst_ecall,
    inst_mret,
    inst_ebreak,
} Instruction;

typedef struct RiscvDecode {
    uint32_t inst;
    Instruction instruction;
    // 从指令当中获取的信息
    uint32_t rd, rs1, rs2;
    int32_t immI, immB, immU, immJ, immS;
    int32_t imm32;
    int64_t imm64;
    // 执行中得到的信息
    ExceptType except; // 保存异常
    IntrType intr;     // 保存触发后的中断
    uint32_t next_pc;
    uint64_t access_addr;
} RiscvDecode;

// 从指令提取需要的信息
void decode(RiscvDecode *dec);

#endif

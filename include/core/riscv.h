#ifndef RVCODE_H
#define RVCODE_H

#include "types.h"

enum arch { RV32, RV64 };
enum mode { USER = 0x0, SUPERVISOR = 0x1, MACHINE = 0x3 };

enum exception {
    EXC_NONE                       = -1,
    INSTRUCTION_ADDRESS_MISALIGNED = 0,
    INSTRUCTION_ACCESS_FAULT       = 1,
    ILLEGAL_INSTRUCTION            = 2,
    BREAKPOINT                     = 3,
    LOAD_ADDRESS_MISALIGNED        = 4,
    LOAD_ACCESS_FAULT              = 5,
    STORE_AMO_ADDRESS_MISALIGNED   = 6,
    STORE_AMO_ACCESS_FAULT         = 7,
    ECALL_FROM_UMODE               = 8,
    ECALL_FROM_SMODE               = 9,
    ECALL_FROM_MMODE               = 11,
    INSTRUCTION_PAGE_FAULT         = 12,
    LOAD_PAGE_FAULT                = 13,
    STORE_AMO_PAGE_FAULT           = 15,
};

enum interrupt {
    INT_NONE                      = -1,
    USER_SOFTWARE_INTERRUPT       = 0,
    SUPERVISOR_SOFTWARE_INTERRUPT = 1,
    MACHINE_SOFTWARE_INTERRUPT    = 3,
    USER_TIMER_INTERRUPT          = 4,
    SUPERVISOR_TIMER_INTERRUPT    = 5,
    MACHINE_TIMER_INTERRUPT       = 7,
    USER_EXTERNAL_INTERRUPT       = 8,
    SUPERVISOR_EXTERNAL_INTERRUPT = 9,
    MACHINE_EXTERNAL_INTERRUPT    = 11,
};

/* Machine level CSRs */
#define MSTATUS 0x300
#define MISA 0x301
#define MEDELEG 0x302
#define MIDELEG 0x303
#define MIE 0x304
#define MTVEC 0x305
#define MSCRATCH 0x340
#define MEPC 0x341
#define MCAUSE 0x342
#define MTVAL 0x343
#define MIP 0x344
/* Supervisor level CSRs */
#define SSTATUS 0x100
#define SIE 0x104
#define STVEC 0x105
#define SSCRATCH 0x140
#define SEPC 0x141
#define SCAUSE 0x142
#define STVAL 0x143
#define SIP 0x144
#define SATP 0x180

/* STATUS */
enum {
    STATUS_MIE  = 0x8,  // 1:Enable, 0:Disable
    STATUS_MPIE = 0x80, // Save Previous MSTATUS_MIE value
    STATUS_MPP  = 0x1800,
    STATUS_SIE  = 0x2,
    STATUS_SPIE = 0x20,
    STATUS_SPP  = 0x100,
    STATUS_FS   = 0x6000,
    STATUS_XS   = 0x18000,
    STATUS_SUM  = 0x40000,
    STATUS_MXR  = 0x80000,
};
#define STATUS_UXL 0x300000000
#define SSTATUS_VISIBLE                                                                            \
    (STATUS_SIE | STATUS_SPIE | STATUS_SPP | STATUS_FS | STATUS_XS | STATUS_SUM | STATUS_MXR |     \
     STATUS_UXL)

/* IE */
enum {
    IE_MSIE = (1 << 3),  // software
    IE_MTIE = (1 << 7),  // timer
    IE_MEIE = (1 << 11), // external
};
/* MIP */
enum {
    IP_USIP = (1 << 0),
    IP_UTIP = (1 << 4),
    IP_UEIP = (1 << 8),
    IP_SSIP = (1 << 1),
    IP_STIP = (1 << 5),
    IP_SEIP = (1 << 9),
    IP_MSIP = (1 << 3),
    IP_MTIP = (1 << 7),
    IP_MEIP = (1 << 11),
};
#define SIP_WRITABLE (IP_SSIP | IP_USIP | IP_UEIP)
#define MIDELEG_WRITABLE (IP_SSIP | IP_STIP | IP_SEIP)

// clang-format off
enum instruction {
    // I 类型指令
    inst_add,   inst_addw,  inst_sub,   inst_subw,  inst_xor,   inst_or,    inst_and,
    inst_sll,   inst_sllw,  inst_slt,   inst_sltu,  inst_srl,   inst_srlw,  inst_sra,
    inst_sraw,  inst_addi,  inst_addiw, inst_xori,  inst_ori,   inst_andi,  inst_slli,
    inst_slliw, inst_srli,  inst_srliw, inst_srai,  inst_sraiw, inst_slti,  inst_sltiu,
    inst_lb,    inst_lh,    inst_lw,    inst_lbu,   inst_lhu,   inst_lwu,   inst_ld,
    inst_sb,    inst_sh,    inst_sw,    inst_sd,    inst_beq,   inst_bne,   inst_blt,
    inst_bge,   inst_bltu,  inst_bgeu,  inst_jal,   inst_jalr,  inst_lui,   inst_auipc,
    // M 类型指令
    inst_mul,   inst_mulh,  inst_mulhsu, inst_mulhu,  inst_mulw,  inst_div,   inst_divu,
    inst_divw,  inst_divuw, inst_rem,   inst_remw,  inst_remu,  inst_remuw,
    // A 类型指令
    inst_lr_w,  inst_sc_w,  inst_amoswap_w, inst_amoadd_w,  inst_amoxor_w, inst_amoor_w,
    inst_amoand_w, inst_amomin_w, inst_amomax_w, inst_amominu_w, inst_amomaxu_w,
    inst_lr_d,  inst_sc_d,  inst_amoswap_d, inst_amoadd_d,  inst_amoxor_d, inst_amoor_d,
    inst_amoand_d, inst_amomin_d, inst_amomax_d, inst_amominu_d, inst_amomaxu_d,
    // Zicsr 类型指令
    inst_csrrw, inst_csrrs, inst_csrrc, inst_csrrwi, inst_csrrsi, inst_csrrci,
    // Zifence 类型指令
    inst_fence, inst_fence_i,   inst_sfence_vma,
    // 特权指令
    inst_wfi,   inst_ecall, inst_sret,  inst_mret,  inst_ebreak,
    // 无效指令
    inst_inv
};
// clang-format on

#endif

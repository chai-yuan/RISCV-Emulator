#ifndef RV_PRIVILEGE_H
#define RV_PRIVILEGE_H

#include "core/rvcore64.h"
#include "types.h"

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
#define MEDELEG 0x302
#define MIDELEG 0x303
#define MIE 0x304
#define MTVEC 0x305
#define MEPC 0x341
#define MCAUSE 0x342
#define MTVAL 0x343
#define MIP 0x344

/* Supervisor level CSRs */
#define SSTATUS 0x100
#define SIE 0x104
#define STVEC 0x105
#define SEPC 0x141
#define SCAUSE 0x142
#define STVAL 0x143
#define SIP 0x144
#define SATP 0x180

u64 riscv64_csr_read(struct RiscvCore64 *core, u16 addr);

void riscv64_csr_write(struct RiscvCore64 *core, u16 addr, u64 value);

#endif

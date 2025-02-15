#ifndef RVCODE_H
#define RVCODE_H

#include "config.h"
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

enum csraddr {
    // Unprivileged Counter/Timers
    CYCLE   = 0xc00,
    TIME    = 0xc01,
    INSTRET = 0xc02,
    // Supervisor Trap Setup
    SSTATUS    = 0x100,
    SIE        = 0x104,
    STVEC      = 0x105,
    SCOUNTEREN = 0x106,
    // Supervisor Trap Handling
    SSCRATCH = 0x140,
    SEPC     = 0x141,
    SCAUSE   = 0x142,
    STVAL    = 0x143,
    SIP      = 0x144,
    // Supervisor Protection and Translation
    SATP = 0x180,
    // Machine Information Registers
    MVENDORID  = 0xf11,
    MARCHID    = 0xf12,
    MIMPID     = 0xf13,
    MHARTID    = 0xf14,
    MCONFIGPTR = 0xf15,
    // Machine Trap Setup
    MSTATUS    = 0x300,
    MISA       = 0x301,
    MEDELEG    = 0x302,
    MIDELEG    = 0x303,
    MIE        = 0x304,
    MTVEC      = 0x305,
    MCOUNTEREN = 0x306,
    // Machine Trap Handling
    MSCRATCH = 0x340,
    MEPC     = 0x341,
    MCAUSE   = 0x342,
    MTVAL    = 0x343,
    MIP      = 0x344,
    // Machine Memory Protection
    PMPCFG0   = 0x3a0,
    PMPCFG1   = 0x3a1,
    PMPCFG2   = 0x3a2,
    PMPCFG3   = 0x3a3,
    PMPADDR0  = 0x3b0,
    PMPADDR1  = 0x3b1,
    PMPADDR2  = 0x3b2,
    PMPADDR3  = 0x3b3,
    PMPADDR4  = 0x3b4,
    PMPADDR5  = 0x3b5,
    PMPADDR6  = 0x3b6,
    PMPADDR7  = 0x3b7,
    PMPADDR8  = 0x3b8,
    PMPADDR9  = 0x3b9,
    PMPADDR10 = 0x3ba,
    PMPADDR11 = 0x3bb,
    PMPADDR12 = 0x3bc,
    PMPADDR13 = 0x3bd,
    PMPADDR14 = 0x3be,
    PMPADDR15 = 0x3bf,
    // Machine Counter/Timers
    MCYCLE   = 0xb00,
    MINSTRET = 0xb02,
    TSELECT  = 0x7a0,
    TDATA1   = 0x7a1
};

#endif

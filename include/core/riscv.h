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
    STATUS_MPRV = (1 << 17),
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

#endif

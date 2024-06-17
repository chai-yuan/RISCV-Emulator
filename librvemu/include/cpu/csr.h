#ifndef CSR_H
#define CSR_H

enum {
    CSR_MIE = 0x304,
    CSR_MTVAL = 0x343,
    CSR_MIP = 0x344,
    CSR_MSCRATCH = 0x340,
    CSR_MEPC = 0x341,
    CSR_MCAUSE = 0x342,
    CSR_MSTATUS = 0x300,
    CSR_MTVEC = 0x305,
    CSR_MVENDORID = 0xf11,
    CSR_MARCHID = 0xf12,
};

#endif // !CSR_H

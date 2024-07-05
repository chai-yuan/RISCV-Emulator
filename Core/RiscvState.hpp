#ifndef RISCV_STATE_H
#define RISCV_STATE_H

#include <cstdint>
#include <cstdio>
#include <stdexcept>

class RiscvState {
  public:
    enum PrivilegeLevel { User = 0, Supervisor = 1, Machine = 3 };
    enum CSR : uint16_t {
        MHARTID = 0xf14,
        MSTATUS = 0x300,
        MEDELEG = 0x302,
        MIDELEG = 0x303,
        MIE = 0x304,
        MTVEC = 0x305,
        MCOUNTEREN = 0x306,
        MSCRATCH = 0x340,
        MEPC = 0x341,
        MCAUSE = 0x342,
        MTVAL = 0x343,
        MIP = 0x344,

        SSTATUS = 0x100,
        SIE = 0x104,
        STVEC = 0x105,
        SSCRATCH = 0x140,
        SEPC = 0x141,
        SCAUSE = 0x142,
        STVAL = 0x143,
        SIP = 0x144,
        SATP = 0x180
    };
    uint64_t pc;
    uint64_t regs[32];
    uint64_t csrs[4096];

    PrivilegeLevel privilege;
    uint64_t amoAddr;
    bool wfi;

    RiscvState() : pc(0x80000000), regs{0}, csrs{0}, privilege(PrivilegeLevel::Machine), amoAddr(0), wfi(false) {}

    uint32_t csrRead(uint16_t addr) {
        switch (addr) {
        default:
            return csrs[addr];
        }
    }

    void csrWrite(uint16_t addr, uint32_t value) {
        switch (addr) {
        default:
            csrs[addr] = value;
        }
    }
};

#endif // RISCVSTATE_H

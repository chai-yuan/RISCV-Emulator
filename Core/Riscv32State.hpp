#ifndef RISCV32_STATE_H
#define RISCV32_STATE_H

#include <cstdint>
#include <cstdio>
#include <stdexcept>

class Riscv32State {
  public:
    enum class PrivilegeLevel { User = 0, Supervisor = 1, Machine = 3 };
    enum class Csr {
        MIE = 0x304,
        MTVAL = 0x343,
        MIP = 0x344,
        MSCRATCH = 0x340,
        MEPC = 0x341,
        MCAUSE = 0x342,
        MSTATUS = 0x300,
        MTVEC = 0x305,
        MVENDORID = 0xf11,
        MARCHID = 0xf12,
    };

    uint32_t pc;
    uint32_t regs[32];
    uint32_t csr[4096];

    PrivilegeLevel privilege;
    uint32_t amoAddr;
    bool sleep;

    Riscv32State() : pc(0x80000000), regs{0}, csr{0}, privilege(PrivilegeLevel::Machine), amoAddr(0), sleep(false) {}

    void printState() const {
        printf("Processor State:\n");
        printf("  Program Counter (pc): 0x%08x\n", pc);
        printf("  General Purpose Registers (regs):\n");
        for (int i = 0; i < 32; ++i) {
            printf("    x%d: 0x%08x\n", i, regs[i]);
        }
        printf("  Privilege Level: %d\n", static_cast<int>(privilege));
        printf("  Atomic Instruction Address (amo_addr): 0x%08x\n", amoAddr);
        printf("  Sleep: %s\n", sleep ? "true" : "false");
    }
};

#endif // RISCVSTATE_H

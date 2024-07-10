#ifndef RISCV_STATE_H
#define RISCV_STATE_H

#include <cstdint>
#include <cstdio>
#include <stdexcept>

class RiscvState {
  public:
    enum PrivilegeLevel { User = 0, Supervisor = 1, Machine = 3 };
    enum XLEN { RV32 = 1, RV64 = 2, RV128 = 3 };
    enum CSR : uint16_t {
        // Machine Information Registers
        mvendorid = 0xF11,  // Vendor ID.
        marchid = 0xF12,    // Architecture ID.
        mimpid = 0xF13,     // Implementation ID.
        mhartid = 0xF14,    // Hardware thread ID.
        mconfigptr = 0xF15, // Pointer to configuration data structure.
        // Machine Trap Setup
        mstatus = 0x300,    // Machine status register.
        misa = 0x301,       // ISA and extensions.
        medeleg = 0x302,    // Machine exception delegation register.
        mideleg = 0x303,    // Machine interrupt delegation register.
        mie = 0x304,        // Machine interrupt-enable register.
        mtvec = 0x305,      // Machine trap-handler base address.
        mcounteren = 0x306, // Machine counter enable.
        mstatush = 0x310,   // Additional machine status register, RV32 only.
        // Machine Trap Handling
        mscratch = 0x340, // Scratch register for machine trap handlers.
        mepc = 0x341,     // Machine exception program counter.
        mcause = 0x342,   // Machine trap cause.
        mtval = 0x343,    // Machine bad address or instruction.
        mip = 0x344,      // Machine interrupt pending.
        mtinst = 0x34A,   // Machine trap instruction (transformed).
        mtval2 = 0x34B,   // Machine bad guest physical address.

        // Supervisor Trap Setup
        sstatus = 0x100,    // Supervisor status register.
        sie = 0x104,        // Supervisor interrupt-enable register.
        stvec = 0x105,      // Supervisor trap handler base address.
        scounteren = 0x106, // Supervisor counter enable.
        // Supervisor Configuration
        senvcfg = 0x10A, // Supervisor environment configuration register.
        // Supervisor Trap Handling
        sscratch = 0x140, // Scratch register for supervisor trap handlers.
        sepc = 0x141,     // Supervisor exception program counter.
        scause = 0x142,   // Supervisor trap cause.
        stval = 0x143,    // Supervisor bad address or instruction.
        sip = 0x144,      // Supervisor interrupt pending.
        // Supervisor Protection and Translation
        satp = 0x180, // Supervisor address translation and protection.
    };
    uint64_t pc;
    uint64_t regs[32];
    uint64_t csrs[4096];

    PrivilegeLevel privilege;
    uint64_t amoAddr;
    bool wfi;

    RiscvState(XLEN xlen = XLEN::RV32)
        : pc(0x80000000), regs{0}, csrs{0}, privilege(PrivilegeLevel::Machine), amoAddr(0), wfi(false) {
        csrs[CSR::misa] = xlen;
    }

    uint64_t csrRead(uint16_t addr) {
        switch (addr) {
        case CSR::sie:
            return csrs[CSR::mie] & csrs[CSR::mideleg];
        case CSR::sip:
            return csrs[CSR::mip] & csrs[CSR::mideleg];
        case CSR::sstatus:
            return csrs[CSR::mstatus] & MASK_SSTATUS;
        default:
            return csrs[addr];
        }
    }

    void csrWrite(uint16_t addr, uint64_t value) {
        switch (addr) {
        case CSR::sie:
            csrs[CSR::mie] = (csrs[CSR::mie] & !csrs[CSR::mideleg]) | (value & csrs[CSR::mideleg]);
            break;
        case CSR::sip:
            csrs[CSR::mip] = (csrs[CSR::mip] & !csrs[CSR::mideleg]) | (value & csrs[CSR::mideleg]);
            break;
        case CSR::sstatus:
            csrs[CSR::mstatus] = (csrs[CSR::mstatus] & !MASK_SSTATUS) | (value & MASK_SSTATUS);
            break;
        default:
            csrs[addr] = value;
        }
    }

  private:
    // mstatus and sstatus field mask
    const uint64_t MASK_SIE = 1ULL << 1;
    const uint64_t MASK_MIE = 1ULL << 3;
    const uint64_t MASK_SPIE = 1ULL << 5;
    const uint64_t MASK_UBE = 1ULL << 6;
    const uint64_t MASK_MPIE = 1ULL << 7;
    const uint64_t MASK_SPP = 1ULL << 8;
    const uint64_t MASK_VS = 0b11ULL << 9;
    const uint64_t MASK_MPP = 0b11ULL << 11;
    const uint64_t MASK_FS = 0b11ULL << 13;
    const uint64_t MASK_XS = 0b11ULL << 15;
    const uint64_t MASK_MPRV = 1ULL << 17;
    const uint64_t MASK_SUM = 1ULL << 18;
    const uint64_t MASK_MXR = 1ULL << 19;
    const uint64_t MASK_TVM = 1ULL << 20;
    const uint64_t MASK_TW = 1ULL << 21;
    const uint64_t MASK_TSR = 1ULL << 22;
    const uint64_t MASK_UXL = 0b11ULL << 32;
    const uint64_t MASK_SXL = 0b11ULL << 34;
    const uint64_t MASK_SBE = 1ULL << 36;
    const uint64_t MASK_MBE = 1ULL << 37;
    const uint64_t MASK_SD = 1ULL << 63;
    const uint64_t MASK_SSTATUS =
        MASK_SIE | MASK_SPIE | MASK_UBE | MASK_SPP | MASK_FS | MASK_XS | MASK_SUM | MASK_MXR | MASK_UXL | MASK_SD;

    // MIP / SIP field mask
    const uint64_t MASK_SSIP = 1ULL << 1;
    const uint64_t MASK_MSIP = 1ULL << 3;
    const uint64_t MASK_STIP = 1ULL << 5;
    const uint64_t MASK_MTIP = 1ULL << 7;
    const uint64_t MASK_SEIP = 1ULL << 9;
    const uint64_t MASK_MEIP = 1ULL << 11;
};

#endif // RISCVSTATE_H

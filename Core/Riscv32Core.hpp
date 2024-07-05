#ifndef RISCV32_CORE_H
#define RISCV32_CORE_H

#include "RiscvCore.hpp"
#include "RiscvDecode.hpp"
#include "RiscvState.hpp"
#include <cstdint>

class Riscv32Core : public RiscvCore {
  public:
    void exec() override {
#define CSRR(i) state.csrRead(i)
#define CSRW(i, v) state.csrWrite(i, v)
#define Rd state.regs[dec.rd]
#define Rs1 reg_src1
#define Rs2 reg_src2
#define PC state.pc
#define Mr(addr, size) mmuRead(addr, size)
#define Mw(addr, size, data) mmuWrite(addr, size, data)

#define INSTBEGIN() switch (dec.instruction) {
#define INSTEND()                                                                                                      \
    default:                                                                                                           \
        dec.except = RiscvDecode::ExceptType::IllegalInstruction;                                                      \
        }
#define INSTEXE(name, ...)                                                                                             \
    case RiscvDecode::Instruction::inst_##name: {                                                                      \
        __VA_ARGS__;                                                                                                   \
        break;                                                                                                         \
    }

        uint32_t reg_src1 = state.regs[dec.rs1], reg_src2 = state.regs[dec.rs2];

        INSTBEGIN()
        // 32I
        INSTEXE(add, Rd = Rs1 + Rs2);
        INSTEXE(sub, Rd = Rs1 - Rs2);
        INSTEXE(xor, Rd = Rs1 ^ Rs2);
        INSTEXE(or, Rd = Rs1 | Rs2);
        INSTEXE(and, Rd = Rs1 & Rs2);
        INSTEXE(sll, Rd = Rs1 << (Rs2 & 0x1f));
        INSTEXE(slt, Rd = (int32_t)Rs1 < (int32_t)Rs2 ? 1 : 0);
        INSTEXE(sltu, Rd = Rs1 < Rs2 ? 1 : 0);
        INSTEXE(srl, Rd = Rs1 >> (Rs2 & 0x1f));
        INSTEXE(sra, Rd = (int32_t)Rs1 >> (Rs2 & 0x1f));
        INSTEXE(addi, Rd = Rs1 + (int32_t)dec.immI);
        INSTEXE(xori, Rd = Rs1 ^ (uint32_t)dec.immI);
        INSTEXE(ori, Rd = Rs1 | (uint32_t)dec.immI);
        INSTEXE(andi, Rd = Rs1 & (uint32_t)dec.immI);
        INSTEXE(slli, Rd = Rs1 << (dec.immI & 0x1f));
        INSTEXE(srli, Rd = Rs1 >> (dec.immI & 0x1f));
        INSTEXE(srai, Rd = (int32_t)Rs1 >> (dec.immI & 0x1f));
        INSTEXE(slti, Rd = (int32_t)Rs1 < (int32_t)dec.immI ? 1 : 0);
        INSTEXE(sltiu, Rd = Rs1 < (uint32_t)dec.immI ? 1 : 0);
        INSTEXE(lb, uint32_t data = Mr(Rs1 + dec.immI, 1); Rd = (int8_t)data);
        INSTEXE(lh, uint32_t data = Mr(Rs1 + dec.immI, 2); Rd = (int16_t)data);
        INSTEXE(lbu, Rd = Mr(Rs1 + dec.immI, 1));
        INSTEXE(lhu, Rd = Mr(Rs1 + dec.immI, 2));
        INSTEXE(lw, Rd = Mr(Rs1 + dec.immI, 4));
        INSTEXE(sb, Mw(Rs1 + dec.immS, 1, Rs2));
        INSTEXE(sh, Mw(Rs1 + dec.immS, 2, Rs2));
        INSTEXE(sw, Mw(Rs1 + dec.immS, 4, Rs2));
        INSTEXE(beq, if ((int32_t)Rs1 == (int32_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bne, if ((int32_t)Rs1 != (int32_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(blt, if ((int32_t)Rs1 < (int32_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bge, if ((int32_t)Rs1 >= (int32_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bltu, if (Rs1 < Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bgeu, if (Rs1 >= Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(jal, dec.next_pc = PC + dec.immJ; Rd = PC + 4);
        INSTEXE(jalr, dec.next_pc = (Rs1 + dec.immI) & ~1; Rd = PC + 4);
        INSTEXE(lui, Rd = dec.immU);
        INSTEXE(auipc, Rd = PC + dec.immU);
        // 32M
        INSTEXE(mul, Rd = ((int64_t)(int32_t)Rs1 * (int64_t)(int32_t)Rs2));
        INSTEXE(mulh, Rd = ((int64_t)(int32_t)Rs1 * (int64_t)(int32_t)Rs2) >> 32);
        INSTEXE(mulsu, Rd = ((int64_t)(int32_t)Rs1 * (uint64_t)Rs2) >> 32);
        INSTEXE(mulu, Rd = ((uint64_t)Rs1 * (uint64_t)Rs2) >> 32);
        INSTEXE(div, Rd = (Rs2 == 0)                                          ? -1
                          : ((int32_t)Rs1 == INT32_MIN && (int32_t)Rs2 == -1) ? Rs1
                                                                              : ((int32_t)Rs1 / (int32_t)Rs2););
        INSTEXE(divu, { Rd = (Rs2 == 0) ? 0xffffffff : Rs1 / Rs2; });
        INSTEXE(rem, Rd = (Rs2 == 0)                                          ? Rs1
                          : ((int32_t)Rs1 == INT32_MIN && (int32_t)Rs2 == -1) ? 0
                                                                              : ((int32_t)Rs1 % (int32_t)Rs2););
        INSTEXE(remu, Rd = (Rs2 == 0) ? Rs1 : Rs1 % Rs2);
        // 32A
        INSTEXE(lr_w, state.amoAddr = Rs1; Rd = Mr(Rs1, 4));
        INSTEXE(sc_w, {
            Rd = (Rs1 == state.amoAddr) ? (Mw(Rs1, 4, Rs2), 0) : 1;
            state.amoAddr = 0;
        });
        INSTEXE(amoswap_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, Rs2););
        INSTEXE(amoadd_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, Rs2 + Rd););
        INSTEXE(amoand_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, Rs2 & Rd););
        INSTEXE(amoor_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, Rs2 | Rd););
        INSTEXE(amoxor_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, Rs2 ^ Rd););
        INSTEXE(amomax_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, ((int32_t)Rs2 > (int32_t)Rd) ? Rs2 : Rd));
        INSTEXE(amomin_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, ((int32_t)Rs2 < (int32_t)Rd) ? Rs2 : Rd));
        INSTEXE(amominu_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, (Rs2 < Rd) ? Rs2 : Rd));
        INSTEXE(amomaxu_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, (Rs2 > Rd) ? Rs2 : Rd));
        // zicsr
        INSTEXE(csrrw, Rd = CSRR(dec.immI); CSRW(dec.immI, Rs1));
        INSTEXE(csrrs, Rd = CSRR(dec.immI); CSRW(dec.immI, Rd | Rs1));
        INSTEXE(csrrc, Rd = CSRR(dec.immI); CSRW(dec.immI, Rd & ~Rs1));
        INSTEXE(csrrwi, Rd = CSRR(dec.immI); CSRW(dec.immI, dec.rs1));
        INSTEXE(csrrsi, Rd = CSRR(dec.immI); CSRW(dec.immI, Rd | dec.rs1));
        INSTEXE(csrrci, Rd = CSRR(dec.immI); CSRW(dec.immI, Rd & ~dec.rs1));
        // zifence
        INSTEXE(fence, );
        INSTEXE(fence_i, );
        // 特权指令
        INSTEXE(ecall, {
            if (state.privilege == RiscvState::PrivilegeLevel::Machine)
                dec.except = RiscvDecode::ExceptType::EcallFromMMode;
            else if (state.privilege == RiscvState::PrivilegeLevel::Supervisor)
                dec.except = RiscvDecode::ExceptType::EcallFromSMode;
            else if (state.privilege == RiscvState::PrivilegeLevel::User)
                dec.except = RiscvDecode::ExceptType::EcallFromUMode;
            else
                dec.error = true;
        });
        INSTEXE(mret, {});
        INSTEXE(ebreak, );
        INSTEXE(wfi, );
        INSTEND()
        state.regs[0] = 0;

#undef CSR
#undef Rd
#undef Rs1
#undef Rs2
#undef PC
#undef Mr
#undef Mw
#undef INSTBEGIN
#undef INSTEND
#undef INSTEXE
    }
};

#endif

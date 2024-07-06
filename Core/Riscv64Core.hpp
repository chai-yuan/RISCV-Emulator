#ifndef RISCV64_CORE_H
#define RISCV64_CORE_H

#include "RiscvCore.hpp"
#include "RiscvDecode.hpp"
#include "RiscvState.hpp"
#include <cstdint>

class Riscv64Core : public RiscvCore {
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

        uint64_t reg_src1 = state.regs[dec.rs1], reg_src2 = state.regs[dec.rs2];

        INSTBEGIN()
        // 64I
        INSTEXE(add, Rd = Rs1 + Rs2);
        INSTEXE(addw, Rd = (int32_t)(Rs1 + Rs2));
        INSTEXE(sub, Rd = Rs1 - Rs2);
        INSTEXE(subw, Rd = (int32_t)(Rs1 - Rs2));
        INSTEXE(xor, Rd = Rs1 ^ Rs2);
        INSTEXE(or, Rd = Rs1 | Rs2);
        INSTEXE(and, Rd = Rs1 & Rs2);
        INSTEXE(sll, Rd = Rs1 << (Rs2 & 0x3f));
        INSTEXE(sllw, Rd = (int32_t)(Rs1 << (Rs2 & 0x1f)));
        INSTEXE(slt, Rd = (int64_t)Rs1 < (int64_t)Rs2 ? 1 : 0);
        INSTEXE(sltu, Rd = Rs1 < Rs2 ? 1 : 0);
        INSTEXE(srl, Rd = Rs1 >> (Rs2 & 0x3f));
        INSTEXE(srlw, Rd = (int32_t)((uint32_t)Rs1 >> (Rs2 & 0x1f)));
        INSTEXE(sra, Rd = (int64_t)Rs1 >> (Rs2 & 0x3f));
        INSTEXE(sraw, Rd = (int32_t)Rs1 >> (Rs2 & 0x1f));
        INSTEXE(addi, Rd = Rs1 + (int64_t)dec.immI);
        INSTEXE(addiw, Rd = (int32_t)(Rs1 + (int64_t)dec.immI));
        INSTEXE(xori, Rd = Rs1 ^ (uint64_t)dec.immI);
        INSTEXE(ori, Rd = Rs1 | (uint64_t)dec.immI);
        INSTEXE(andi, Rd = Rs1 & (uint64_t)dec.immI);
        INSTEXE(slli, Rd = Rs1 << (dec.immI & 0x3f));
        INSTEXE(slliw, Rd = (int32_t)(Rs1 << (dec.immI & 0x1f)));
        INSTEXE(srli, Rd = Rs1 >> (dec.immI & 0x3f));
        INSTEXE(srliw, Rd = (int32_t)((uint32_t)Rs1 >> (dec.immI & 0x1f)));
        INSTEXE(srai, Rd = (int64_t)Rs1 >> (dec.immI & 0x3f));
        INSTEXE(sraiw, Rd = (int32_t)Rs1 >> (dec.immI & 0x1f));
        INSTEXE(slti, Rd = (int64_t)Rs1 < (int64_t)dec.immI ? 1 : 0);
        INSTEXE(sltiu, Rd = Rs1 < (uint64_t)dec.immI ? 1 : 0);
        INSTEXE(lb, uint64_t data = Mr(Rs1 + dec.immI, 1); Rd = (int8_t)data);
        INSTEXE(lh, uint64_t data = Mr(Rs1 + dec.immI, 2); Rd = (int16_t)data);
        INSTEXE(lw, uint64_t data = Mr(Rs1 + dec.immI, 8); Rd = (int32_t)data);
        INSTEXE(lbu, Rd = Mr(Rs1 + dec.immI, 1));
        INSTEXE(lhu, Rd = Mr(Rs1 + dec.immI, 2));
        INSTEXE(lwu, Rd = Mr(Rs1 + dec.immI, 4));
        INSTEXE(ld, Rd = Mr(Rs1 + dec.immI, 8));
        INSTEXE(sb, Mw(Rs1 + dec.immS, 1, Rs2));
        INSTEXE(sh, Mw(Rs1 + dec.immS, 2, Rs2));
        INSTEXE(sw, Mw(Rs1 + dec.immS, 4, Rs2));
        INSTEXE(sd, Mw(Rs1 + dec.immS, 8, Rs2));
        INSTEXE(beq, if ((int64_t)Rs1 == (int64_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bne, if ((int64_t)Rs1 != (int64_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(blt, if ((int64_t)Rs1 < (int64_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bge, if ((int64_t)Rs1 >= (int64_t)Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bltu, if (Rs1 < Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(bgeu, if (Rs1 >= Rs2) dec.next_pc = PC + dec.immB);
        INSTEXE(jal, dec.next_pc = PC + dec.immJ; Rd = PC + 4);
        INSTEXE(jalr, dec.next_pc = (Rs1 + dec.immI) & ~1; Rd = PC + 4);
        INSTEXE(lui, Rd = dec.immU);
        INSTEXE(auipc, Rd = PC + dec.immU);
        // 64M
        INSTEXE(mul, Rd = Rs1 * Rs2);
        INSTEXE(mulh, Rd = ((__int128_t)(int64_t)Rs1 * (__int128_t)(int64_t)Rs2) >> 64);
        INSTEXE(mulsu, Rd = ((__int128_t)(int64_t)Rs1 * (__uint128_t)Rs2) >> 64);
        INSTEXE(mulu, Rd = ((__uint128_t)Rs1 * (__uint128_t)Rs2) >> 64);
        INSTEXE(div, {
            if (Rs2 == 0) {
                Rd = -1;
            } else if ((int64_t)Rs1 == INT64_MIN && (int64_t)Rs2 == -1) {
                Rd = Rs1;
            } else {
                Rd = (int64_t)Rs1 / (int64_t)Rs2;
            }
        });
        INSTEXE(divu, { Rd = (Rs2 == 0) ? (uint64_t)-1 : Rs1 / Rs2; });
        INSTEXE(rem, {
            if (Rs2 == 0) {
                Rd = Rs1;
            } else if ((int64_t)Rs1 == INT64_MIN && (int64_t)Rs2 == -1) {
                Rd = 0;
            } else {
                Rd = (int64_t)Rs1 % (int64_t)Rs2;
            }
        });
        INSTEXE(remu, Rd = (Rs2 == 0) ? Rs1 : Rs1 % Rs2);
        INSTEXE(mulw, Rd = (int64_t)((int32_t)Rs1 * (int32_t)Rs2));
        INSTEXE(divw, {
            auto dividend = (int32_t)Rs1;
            auto divisor = (int32_t)Rs2;
            if (divisor == 0) {
                Rd = -1;
            } else if (dividend == INT32_MIN && divisor == -1) {
                Rd = dividend;
            } else {
                Rd = dividend / divisor;
            }
        });
        INSTEXE(divuw, {
            // TODO 未通过测试
            auto dividend = (uint32_t)Rs1;
            auto divisor = (uint32_t)Rs2;
            Rd = (divisor == 0) ? (int64_t)-1 : (int32_t)(dividend / divisor);
        });
        INSTEXE(remw, {
            auto dividend = (int32_t)Rs1;
            auto divisor = (int32_t)Rs2;
            if (divisor == 0) {
                Rd = dividend;
            } else if (dividend == INT32_MIN && divisor == -1) {
                Rd = 0;
            } else {
                Rd = dividend % divisor;
            }
        });
        INSTEXE(remuw, {
            auto dividend = (uint32_t)Rs1;
            auto divisor = (uint32_t)Rs2;
            Rd = (divisor == 0) ? (int32_t)dividend : (int32_t)(dividend % divisor);
        });
        // 64A
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
        INSTEXE(amomax_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, ((int64_t)Rs2 > (int64_t)Rd) ? Rs2 : Rd));
        INSTEXE(amomin_w, Rd = Mr(Rs1, 4); Mw(Rs1, 4, ((int64_t)Rs2 < (int64_t)Rd) ? Rs2 : Rd));
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

#ifndef RISCV32_CORE_H
#define RISCV32_CORE_H

#include "Riscv32State.hpp"
#include "RiscvCore.hpp"
#include "RiscvDecode.hpp"
#include <cstdint>

class Riscv32Core : public RiscvCore {
  public:
    Riscv32Core() : state() {}

    Riscv32State *getState() { return &state; }

    RiscvDecode step() override {
        dec = RiscvDecode(); // 重置当前译码状态

        mmuFetch();
        exec();

        if (dec.except != RiscvDecode::ExceptType::None) {
        } else {
            state.pc = dec.next_pc;
        }

        return dec;
    }

    bool checkInterrupt() override {
        // 默认没有中断
        return false;
    }

  private:
    Riscv32State state; // 处理器的持久状态
    RiscvDecode dec;    // 处理器单次执行时产生的状态

    // 指令执行
    void exec() {
#define R(i) state.regs[i]
#define CSR(i) state.csr[i]
#define Rd state.regs[dec.rd]
#define Rs1 reg_src1
#define Rs2 reg_src2
#define PC state.pc
#define Mr(addr, size, data) mmuRead(addr, size, &data);
#define Mw(addr, size, data) mmuWrite(addr, size, data);
#define INSTBEGIN() switch (dec.instruction) {
#define INSTEND()                                                                                                      \
    default:                                                                                                           \
        dec.error = true;                                                                                              \
        dec.except = RiscvDecode::ExceptType::IllegalInstruction;                                                      \
        }
#define INSTEXE(name, ...)                                                                                             \
    case RiscvDecode::Instruction::inst_##name: {                                                                      \
        __VA_ARGS__;                                                                                                   \
        break;                                                                                                         \
    }

        uint32_t reg_src1 = R(dec.rs1), reg_src2 = R(dec.rs2);

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
        INSTEXE(lb, uint32_t data = 0; Mr(Rs1 + dec.immI, 1, data); Rd = (int8_t)data;);
        INSTEXE(lh, uint32_t data = 0; Mr(Rs1 + dec.immI, 2, data); Rd = (int16_t)data;);
        INSTEXE(lw, Mr(Rs1 + dec.immI, 4, Rd));
        INSTEXE(lbu, Mr(Rs1 + dec.immI, 1, Rd));
        INSTEXE(lhu, Mr(Rs1 + dec.immI, 2, Rd));
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
        INSTEXE(div, {
            if (Rs2 == 0) {
                Rd = -1;
            } else {
                Rd = ((int32_t)Rs1 == INT32_MIN && (int32_t)Rs2 == -1) ? Rs1 : ((int32_t)Rs1 / (int32_t)Rs2);
            }
        });
        INSTEXE(divu, { Rd = (Rs2 == 0) ? 0xffffffff : Rs1 / Rs2; });
        INSTEXE(rem, {
            if (Rs2 == 0) {
                Rd = Rs1;
            } else {
                Rd = ((int32_t)Rs1 == INT32_MIN && (int32_t)Rs2 == -1) ? 0 : ((uint32_t)((int32_t)Rs1 % (int32_t)Rs2));
            }
        });
        INSTEXE(remu, Rd = (Rs2 == 0) ? Rs1 : Rs1 % Rs2);
        // 32A
        INSTEXE(lr_w, state.amoAddr = Rs1; Mr(Rs1, 4, Rd));
        INSTEXE(sc_w, {
            if (Rs1 == state.amoAddr) {
                Mw(Rs1, 4, Rs2);
                Rd = 0;
            } else {
                Rd = 1;
            }
            state.amoAddr = 0;
        });
        INSTEXE(amoswap_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2););
        INSTEXE(amoadd_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 + Rd););
        INSTEXE(amoand_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 & Rd););
        INSTEXE(amoor_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 | Rd););
        INSTEXE(amoxor_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 ^ Rd););
        INSTEXE(amomax_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, ((int32_t)Rs2 > (int32_t)Rd) ? Rs2 : Rd));
        INSTEXE(amomin_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, ((int32_t)Rs2 < (int32_t)Rd) ? Rs2 : Rd));
        INSTEXE(amominu_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, (Rs2 < Rd) ? Rs2 : Rd));
        INSTEXE(amomaxu_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, (Rs2 > Rd) ? Rs2 : Rd));
        // zicsr
        INSTEXE(csrrw, Rd = CSR(dec.immI); CSR(dec.immI) = Rs1;);
        INSTEXE(csrrs, Rd = CSR(dec.immI); CSR(dec.immI) |= Rs1;);
        INSTEXE(csrrc, Rd = CSR(dec.immI); CSR(dec.immI) &= ~Rs1);
        INSTEXE(csrrwi, Rd = CSR(dec.immI); CSR(dec.immI) = dec.rs1);
        INSTEXE(csrrsi, Rd = CSR(dec.immI); CSR(dec.immI) |= dec.rs1);
        INSTEXE(csrrci, Rd = CSR(dec.immI); CSR(dec.immI) &= ~dec.rs1);
        // zifence
        INSTEXE(fence, );
        INSTEXE(fence_i, );
        // 特权指令
        INSTEXE(ecall, {
            if (state.privilege == Riscv32State::PrivilegeLevel::Machine)
                dec.except = RiscvDecode::ExceptType::EcallFromMMode;
            else if (state.privilege == Riscv32State::PrivilegeLevel::Supervisor)
                dec.except = RiscvDecode::ExceptType::EcallFromSMode;
            else if (state.privilege == Riscv32State::PrivilegeLevel::User)
                dec.except = RiscvDecode::ExceptType::EcallFromUMode;
            else
                dec.error = true;
        });
        INSTEXE(mret, {});
        INSTEXE(ebreak, );
        INSTEXE(wfi, );
        INSTEND()

        R(0) = 0;

#undef R
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

    // 内存访问相关函数
    void mmuFetch() {
        mmuRead(state.pc, 4, &dec.inst);
        dec.decode();
        dec.next_pc = state.pc + 4;
    }
    void mmuRead(uint32_t addr, uint8_t size, uint32_t *data) {
        uint64_t readData = 0;
        if (read(addr, size, &readData)) {
            dec.accessAddr = addr;
            switch (size) {
            case 1:
                *data = readData & 0xff;
                break;
            case 2:
                *data = readData & 0xffff;
                break;
            case 4:
                *data = readData & 0xffffffff;
                break;
            default:
                dec.error = true;
            }
        } else {
            dec.error = true;
        }
    }
    void mmuWrite(uint32_t addr, uint8_t size, uint32_t data) {
        if (write(addr, size, data)) {
        } else {
            dec.error = true;
        }
    }
};

#endif

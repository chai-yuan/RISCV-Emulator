#include "common/debug.h"
#include "cpu/csr.h"
#include "cpu/except.h"
#include <cpu/decode.h>
#include <cpu/mmu.h>
#include <cpu/riscv32.h>
#include <stdio.h>

#define R(i) core->regs[i]
#define CSR(i) core->csr[i]
#define Rd core->regs[dec->rd]
#define Rs1 reg_src1
#define Rs2 reg_src2
#define PC core->pc
#define CPU(i) core->i
#define Mr(addr, size, data)                                                   \
    do {                                                                       \
        mmu_read_32(core, dec, addr, size, &data);                             \
    } while (0);
#define Mw(addr, size, data)                                                   \
    do {                                                                       \
        mmu_write_32(core, dec, addr, size, data);                             \
    } while (0);

#define INSTEXE(name, ...)                                                     \
    case inst_##name: {                                                        \
        __VA_ARGS__;                                                           \
        break;                                                                 \
    }

void riscv32_exec(Riscv32core *core, RiscvDecode *dec) {
    uint32_t reg_src1 = core->regs[dec->rs1], reg_src2 = core->regs[dec->rs2];

    switch (dec->instruction) {
        INSTEXE(add, Rd = Rs1 + Rs2);
        INSTEXE(sub, Rd = Rs1 - Rs2);
        INSTEXE(xor, Rd = Rs1 ^ Rs2);
        INSTEXE(or, Rd = Rs1 | Rs2);
        INSTEXE(and, Rd = Rs1 & Rs2);
        INSTEXE(sll, Rd = Rs1 << BITS(Rs2, 4, 0));
        INSTEXE(slt, Rd = (int32_t)Rs1 < (int32_t)Rs2 ? 1 : 0);
        INSTEXE(sltu, Rd = Rs1 < Rs2 ? 1 : 0);
        INSTEXE(srl, Rd = Rs1 >> BITS(Rs2, 4, 0));
        INSTEXE(sra, Rd = (int32_t)Rs1 >> BITS(Rs2, 4, 0));
        INSTEXE(addi, Rd = Rs1 + (int32_t)dec->immI);
        INSTEXE(xori, Rd = Rs1 ^ (uint32_t)dec->immI);
        INSTEXE(ori, Rd = Rs1 | (uint32_t)dec->immI);
        INSTEXE(andi, Rd = Rs1 & (uint32_t)dec->immI);
        INSTEXE(slli, Rd = Rs1 << (dec->immI & 0x1f));
        INSTEXE(srli, Rd = Rs1 >> (dec->immI & 0x1f));
        INSTEXE(srai, Rd = (int32_t)Rs1 >> (dec->immI & 0x1f));
        INSTEXE(slti, Rd = (int32_t)Rs1 < (int32_t)dec->immI ? 1 : 0);
        INSTEXE(sltiu, Rd = Rs1 < (uint32_t)dec->immI ? 1 : 0);
        INSTEXE(lb, {
            uint32_t data;
            Mr(Rs1 + dec->immI, 1, data);
            Rd = (int8_t)data;
        });
        INSTEXE(lh, {
            uint32_t data;
            Mr(Rs1 + dec->immI, 2, data);
            Rd = (int16_t)data;
        });
        INSTEXE(lw, Mr(Rs1 + dec->immI, 4, Rd));
        INSTEXE(lbu, Mr(Rs1 + dec->immI, 1, Rd));
        INSTEXE(lhu, Mr(Rs1 + dec->immI, 2, Rd));
        INSTEXE(sb, Mw(Rs1 + dec->immS, 1, Rs2));
        INSTEXE(sh, Mw(Rs1 + dec->immS, 2, Rs2));
        INSTEXE(sw, Mw(Rs1 + dec->immS, 4, Rs2));

        INSTEXE(beq, if ((int32_t)Rs1 == (int32_t)Rs2) dec->next_pc =
                         PC + dec->immB);
        INSTEXE(bne, if ((int32_t)Rs1 != (int32_t)Rs2) dec->next_pc =
                         PC + dec->immB);
        INSTEXE(blt,
                if ((int32_t)Rs1 < (int32_t)Rs2) dec->next_pc = PC + dec->immB);
        INSTEXE(bge, if ((int32_t)Rs1 >= (int32_t)Rs2) dec->next_pc =
                         PC + dec->immB);
        INSTEXE(bltu, if (Rs1 < Rs2) dec->next_pc = PC + dec->immB);
        INSTEXE(bgeu, if (Rs1 >= Rs2) dec->next_pc = PC + dec->immB);

        INSTEXE(jal, dec->next_pc = PC + dec->immJ; Rd = PC + 4);
        INSTEXE(jalr, dec->next_pc = (Rs1 + dec->immI) & ~1; Rd = PC + 4);
        INSTEXE(lui, Rd = dec->immU);
        INSTEXE(auipc, Rd = PC + dec->immU);
        // -----------------------   M   ------------------------------
        INSTEXE(mul, Rd = ((int64_t)(int32_t)Rs1 * (int64_t)(int32_t)Rs2));
        INSTEXE(mulh,
                Rd = ((int64_t)(int32_t)Rs1 * (int64_t)(int32_t)Rs2) >> 32);
        INSTEXE(mulsu, Rd = ((int64_t)(int32_t)Rs1 * (uint64_t)Rs2) >> 32);
        INSTEXE(mulu, Rd = ((uint64_t)Rs1 * (uint64_t)Rs2) >> 32);
        INSTEXE(div, {
            if (Rs2 == 0) {
                Rd = -1;
            } else {
                Rd = ((int32_t)Rs1 == INT32_MIN && (int32_t)Rs2 == -1)
                         ? Rs1
                         : ((int32_t)Rs1 / (int32_t)Rs2);
            }
        });
        INSTEXE(divu, { Rd = (Rs2 == 0) ? 0xffffffff : Rs1 / Rs2; });
        INSTEXE(rem, {
            if (Rs2 == 0) {
                Rd = Rs1;
            } else {
                Rd = ((int32_t)Rs1 == INT32_MIN && (int32_t)Rs2 == -1)
                         ? 0
                         : ((uint32_t)((int32_t)Rs1 % (int32_t)Rs2));
            }
        });
        INSTEXE(remu, Rd = (Rs2 == 0) ? Rs1 : Rs1 % Rs2);
        // -----------------------   A   ------------------------------
        INSTEXE(lr_w, CPU(amo_addr) = Rs1; Mr(Rs1, 4, Rd));
        INSTEXE(sc_w, {
            if (Rs1 == CPU(amo_addr)) {
                Mw(Rs1, 4, Rs2);
                Rd = 0;
            } else {
                Rd = 1;
            }
            CPU(amo_addr) = 0;
        });
        INSTEXE(amoswap_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2););
        INSTEXE(amoadd_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 + Rd););
        INSTEXE(amoand_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 & Rd););
        INSTEXE(amoor_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 | Rd););
        INSTEXE(amoxor_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, Rs2 ^ Rd););
        INSTEXE(amomax_w, Mr(Rs1, 4, Rd);
                Mw(Rs1, 4, ((int32_t)Rs2 > (int32_t)Rd) ? Rs2 : Rd));
        INSTEXE(amomin_w, Mr(Rs1, 4, Rd);
                Mw(Rs1, 4, ((int32_t)Rs2 < (int32_t)Rd) ? Rs2 : Rd));
        INSTEXE(amominu_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, (Rs2 < Rd) ? Rs2 : Rd));
        INSTEXE(amomaxu_w, Mr(Rs1, 4, Rd); Mw(Rs1, 4, (Rs2 > Rd) ? Rs2 : Rd));
        // ----------------------- Zicsr ------------------------------
        INSTEXE(csrrw, Rd = CSR(dec->immI); CSR(dec->immI) = Rs1;);
        INSTEXE(csrrs, Rd = CSR(dec->immI); CSR(dec->immI) |= Rs1;);
        INSTEXE(csrrc, Rd = CSR(dec->immI); CSR(dec->immI) &= ~Rs1);
        INSTEXE(csrrwi, Rd = CSR(dec->immI); CSR(dec->immI) = dec->rs1);
        INSTEXE(csrrsi, Rd = CSR(dec->immI); CSR(dec->immI) |= dec->rs1);
        INSTEXE(csrrci, Rd = CSR(dec->immI); CSR(dec->immI) &= ~dec->rs1);
        // ----------------------- Zifence ------------------------------
        INSTEXE(fence, );
        INSTEXE(fence_i, );
        // ----------------------- 其他指令------------------------------
        INSTEXE(wfi, CPU(sleep) = true; CSR(CSR_MSTATUS) |= 8);
        INSTEXE(ecall, {
            if (CPU(privilege) == MACHINE)
                dec->except = EXC_EcallFromMMode;
            else if (CPU(privilege) == SUPERVISOR)
                dec->except = EXC_EcallFromSMode;
            else if (CPU(privilege) == USER)
                dec->except = EXC_EcallFromUMode;
            else
                panic("未知特权级");
        });
        INSTEXE(mret, {
            uint32_t startmstatus = CSR(CSR_MSTATUS);
            uint32_t privilege = CPU(privilege);
            CSR(CSR_MSTATUS) =
                ((startmstatus & 0x80) >> 4) | ((privilege & 3) << 11) | 0x80;
            CPU(privilege) = (startmstatus >> 11) & 3;
            dec->next_pc = CSR(CSR_MEPC);
        });
        INSTEXE(ebreak, CPU(halt) = true);
    default:
        panic("未知指令! %d", dec->instruction);
        dec->except = EXC_IllegalInstruction;
    }
    R(0) = 0;
}

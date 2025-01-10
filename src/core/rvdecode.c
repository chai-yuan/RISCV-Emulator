#include "core/rvcore_priv.h"

void riscv_decode_init(struct RiscvDecode *decode) {
    decode->inst_raw      = 0;
    decode->exception     = EXC_NONE;
    decode->exception_val = 0;
    decode->interrupt     = INT_NONE;
}

static inline void pattern_decode(const char *str, int len, u64 *key, u64 *mask, u64 *shift);

static void riscv_decode_inst32(struct RiscvDecode *decode) {
    INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add);
    INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub);
    INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor);
    INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or);
    INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and);
    INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll);
    INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt);
    INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu);
    INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl);
    INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra);
    INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi);
    INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori);
    INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori);
    INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi);
    INSTPAT("000000? ????? ????? 001 ????? 00100 11", slli);
    INSTPAT("000000? ????? ????? 101 ????? 00100 11", srli);
    INSTPAT("010000? ????? ????? 101 ????? 00100 11", srai);
    INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti);
    INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu);
    INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb);
    INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh);
    INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw);
    INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu);
    INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu);
    INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb);
    INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh);
    INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw);
    INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq);
    INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne);
    INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt);
    INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge);
    INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu);
    INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu);
    INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal);
    INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr);
    INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui);
    INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc);

    INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul);
    INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh);
    INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulhsu);
    INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu);
    INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div);
    INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu);
    INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem);
    INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu);

    INSTPAT("00010?? ????? ????? 010 ????? 01011 11", lr_w);
    INSTPAT("00011?? ????? ????? 010 ????? 01011 11", sc_w);
    INSTPAT("00100?? ????? ????? 010 ????? 01011 11", amoxor_w);
    INSTPAT("00000?? ????? ????? 010 ????? 01011 11", amoadd_w);
    INSTPAT("00001?? ????? ????? 010 ????? 01011 11", amoswap_w);
    INSTPAT("01000?? ????? ????? 010 ????? 01011 11", amoor_w);
    INSTPAT("01100?? ????? ????? 010 ????? 01011 11", amoand_w);
    INSTPAT("10000?? ????? ????? 010 ????? 01011 11", amomin_w);
    INSTPAT("10100?? ????? ????? 010 ????? 01011 11", amomax_w);
    INSTPAT("11000?? ????? ????? 010 ????? 01011 11", amominu_w);
    INSTPAT("11100?? ????? ????? 010 ????? 01011 11", amomaxu_w);

    INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw);
    INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs);
    INSTPAT("??????? ????? ????? 011 ????? 11100 11", csrrc);
    INSTPAT("??????? ????? ????? 101 ????? 11100 11", csrrwi);
    INSTPAT("??????? ????? ????? 110 ????? 11100 11", csrrsi);
    INSTPAT("??????? ????? ????? 111 ????? 11100 11", csrrci);

    INSTPAT("??????? ????? ????? 000 ????? 00011 11", fence);
    INSTPAT("??????? ????? ????? 001 ????? 00011 11", fence_i);
    INSTPAT("0001001 ????? ????? 000 00000 11100 11", sfence_vma);

    INSTPAT("0001000 00101 00000 000 00000 11100 11", wfi);
    INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall);
    INSTPAT("0001000 00010 00000 000 00000 11100 11", sret);
    INSTPAT("0011000 00010 00000 000 00000 11100 11", mret);
    INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak);
#if CURRENT_ARCH == ARCH_RV64
    INSTPAT("??????? ????? ????? 110 ????? 00000 11", lwu);
    INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld);
    INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd);
    INSTPAT("??????? ????? ????? 000 ????? 00110 11", addiw);
    INSTPAT("0000000 ????? ????? 001 ????? 00110 11", slliw);
    INSTPAT("0000000 ????? ????? 101 ????? 00110 11", srliw);
    INSTPAT("0100000 ????? ????? 101 ????? 00110 11", sraiw);
    INSTPAT("0000000 ????? ????? 000 ????? 01110 11", addw);
    INSTPAT("0100000 ????? ????? 000 ????? 01110 11", subw);
    INSTPAT("0000000 ????? ????? 001 ????? 01110 11", sllw);
    INSTPAT("0000000 ????? ????? 101 ????? 01110 11", srlw);
    INSTPAT("0100000 ????? ????? 101 ????? 01110 11", sraw);
    INSTPAT("00010?? ????? ????? 011 ????? 01011 11", lr_d);
    INSTPAT("00011?? ????? ????? 011 ????? 01011 11", sc_d);
    INSTPAT("00100?? ????? ????? 011 ????? 01011 11", amoxor_d);
    INSTPAT("00000?? ????? ????? 011 ????? 01011 11", amoadd_d);
    INSTPAT("00001?? ????? ????? 011 ????? 01011 11", amoswap_d);
    INSTPAT("01000?? ????? ????? 011 ????? 01011 11", amoor_d);
    INSTPAT("01100?? ????? ????? 011 ????? 01011 11", amoand_d);
    INSTPAT("10000?? ????? ????? 011 ????? 01011 11", amomin_d);
    INSTPAT("10100?? ????? ????? 011 ????? 01011 11", amomax_d);
    INSTPAT("11000?? ????? ????? 011 ????? 01011 11", amominu_d);
    INSTPAT("11100?? ????? ????? 011 ????? 01011 11", amomaxu_d);
    INSTPAT("0000001 ????? ????? 000 ????? 01110 11", mulw);
    INSTPAT("0000001 ????? ????? 100 ????? 01110 11", divw);
    INSTPAT("0000001 ????? ????? 101 ????? 01110 11", divuw);
    INSTPAT("0000001 ????? ????? 110 ????? 01110 11", remw);
    INSTPAT("0000001 ????? ????? 111 ????? 01110 11", remuw);
#endif
    INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv);
}

static void riscv_decode_inst16(struct RiscvDecode *decode) {
    INSTPAT("010 ??????????? 10", c_lwsp);
    INSTPAT("110 ??????????? 10", c_swsp);
    INSTPAT("010 ??????????? 00", c_lw);
    INSTPAT("110 ??????????? 00", c_sw);
    INSTPAT("101 ??????????? 01", c_j);
    INSTPAT("001 ??????????? 01", c_jal);
    INSTPAT("100 0?????????? 10", c_jr);
    INSTPAT("100 1?????????? 10", c_jalr);
    INSTPAT("110 ??????????? 01", c_beqz);
    INSTPAT("111 ??????????? 01", c_bnez);
    INSTPAT("010 ??????????? 01", c_li);
    INSTPAT("011 ??????????? 01", c_lui);
    INSTPAT("000 ??????????? 01", c_addi);
    INSTPAT("011 ??????????? 01", c_addi16sp);
    INSTPAT("000 ??????????? 10", c_slli);
    INSTPAT("000 ??????????? 00", c_addi4spn);
    INSTPAT("100 ?00???????? 01", c_srli);
    INSTPAT("100 ?01???????? 01", c_srai);
    INSTPAT("100 ?10???????? 01", c_andi);
    INSTPAT("100 0?????????? 10", c_mv);
    INSTPAT("100 1?????????? 10", c_add);
    INSTPAT("100 011???11??? 01", c_and);
    INSTPAT("100 011???10??? 01", c_or);
    INSTPAT("100 011???01??? 01", c_xor);
    INSTPAT("100 011???00??? 01", c_sub);
    INSTPAT("100 10000000000 10", c_ebreak);
#if CURRENT_ARCH == ARCH_RV64
    INSTPAT("011 ??????????? 10", c_ldsp);
    INSTPAT("111 ??????????? 10", c_swsp);
    INSTPAT("011 ??????????? 00", c_ld);
#endif
    INSTPAT("????????????????", inv);
}

void riscv_decode_inst(struct RiscvDecode *decode) {
    u32 inst = decode->inst_raw;

    decode->rd  = BITS(inst, 11, 7);
    decode->rs1 = BITS(inst, 19, 15);
    decode->rs2 = BITS(inst, 24, 20);

    decode->immI = SEXT(BITS(inst, 31, 20), 12);
    decode->immB = (SEXT(BITS(inst, 31, 31), 1) << 12) | (BITS(inst, 30, 25) << 5) |
                   (BITS(inst, 11, 8) << 1) | (BITS(inst, 7, 7) << 11);
    decode->immU = (SEXT(BITS(inst, 31, 12), 20) << 12);
    decode->immJ = (SEXT(BITS(inst, 31, 31), 1) << 20) | (BITS(inst, 30, 21) << 1) |
                   (BITS(inst, 20, 20) << 11) | (BITS(inst, 19, 12) << 12);
    decode->immS = (SEXT(BITS(inst, 31, 25), 7) << 5) | BITS(inst, 11, 7);

    decode->csr_imm = BITS(inst, 31, 20);

    if ((inst & 0x3) == 0x3)
        riscv_decode_inst32(decode);
    else
        riscv_decode_inst16(decode);
}

__attribute__((always_inline)) static inline void pattern_decode(const char *str, int len, u64 *key,
                                                                 u64 *mask, u64 *shift) {
    u64 __key = 0, __mask = 0, __shift = 0;
#define macro(i)                                                                                   \
    if ((i) >= len)                                                                                \
        goto finish;                                                                               \
    else {                                                                                         \
        char c = str[i];                                                                           \
        if (c != ' ') {                                                                            \
            __key   = (__key << 1) | (c == '1' ? 1 : 0);                                           \
            __mask  = (__mask << 1) | (c == '?' ? 0 : 1);                                          \
            __shift = (c == '?' ? __shift + 1 : 0);                                                \
        }                                                                                          \
    }

#define macro2(i)                                                                                  \
    macro(i);                                                                                      \
    macro((i) + 1)
#define macro4(i)                                                                                  \
    macro2(i);                                                                                     \
    macro2((i) + 2)
#define macro8(i)                                                                                  \
    macro4(i);                                                                                     \
    macro4((i) + 4)
#define macro16(i)                                                                                 \
    macro8(i);                                                                                     \
    macro8((i) + 8)
#define macro32(i)                                                                                 \
    macro16(i);                                                                                    \
    macro16((i) + 16)
#define macro64(i)                                                                                 \
    macro32(i);                                                                                    \
    macro32((i) + 32)

    macro64(0);
#undef macro
finish:
    *key   = __key >> __shift;
    *mask  = __mask >> __shift;
    *shift = __shift;
}

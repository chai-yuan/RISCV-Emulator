#include "common/debug.h"
#include "cpu/except.h"
#include <cpu/decode.h>
#include <stdint.h>

// --- pattern matching mechanism ---
__attribute__((always_inline)) static inline void
pattern_decode(const char *str, int len, uint64_t *key, uint64_t *mask,
               uint64_t *shift) {
    uint64_t __key = 0, __mask = 0, __shift = 0;
#define macro(i)                                                               \
    if ((i) >= len)                                                            \
        goto finish;                                                           \
    else {                                                                     \
        char c = str[i];                                                       \
        if (c != ' ') {                                                        \
            Assert(c == '0' || c == '1' || c == '?',                           \
                   "invalid character '%c' in pattern string", c);             \
            __key = (__key << 1) | (c == '1' ? 1 : 0);                         \
            __mask = (__mask << 1) | (c == '?' ? 0 : 1);                       \
            __shift = (c == '?' ? __shift + 1 : 0);                            \
        }                                                                      \
    }

#define macro2(i)                                                              \
    macro(i);                                                                  \
    macro((i) + 1)
#define macro4(i)                                                              \
    macro2(i);                                                                 \
    macro2((i) + 2)
#define macro8(i)                                                              \
    macro4(i);                                                                 \
    macro4((i) + 4)
#define macro16(i)                                                             \
    macro8(i);                                                                 \
    macro8((i) + 8)
#define macro32(i)                                                             \
    macro16(i);                                                                \
    macro16((i) + 16)
#define macro64(i)                                                             \
    macro32(i);                                                                \
    macro32((i) + 32)

    macro64(0);
    panic("decode too long");
#undef macro
finish:
    *key = __key >> __shift;
    *mask = __mask >> __shift;
    *shift = __shift;
}

#define INSTPAT(pattern, name)                                                 \
    do {                                                                       \
        uint64_t key, mask, shift;                                             \
        pattern_decode(pattern, STRLEN(pattern), &key, &mask, &shift);         \
        if ((((uint64_t)dec->inst >> shift) & mask) == key) {                  \
            dec->instruction = inst_##name;                                    \
            return;                                                            \
        }                                                                      \
    } while (0)

static void decode_inst(RiscvDecode *dec) {
    INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add);
    INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub);
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
    INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli);
    INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli);
    INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai);
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
    INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulsu);
    INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulu);
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

    INSTPAT("0001000 00101 00000 000 00000 11100 11", wfi);

    INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall);
    INSTPAT("0011000 00010 00000 000 00000 11100 11", mret);
    INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak);

    panic("未知指令");
}

void decode(RiscvDecode *dec) {
    uint32_t inst = dec->inst;

    dec->rd = BITS(inst, 11, 7);
    dec->rs1 = BITS(inst, 19, 15);
    dec->rs2 = BITS(inst, 24, 20);

    int32_t immI = SEXT(BITS(inst, 31, 20), 12);
    int32_t immB = (SEXT(BITS(inst, 31, 31), 1) << 12) |
                   (BITS(inst, 30, 25) << 5) | (BITS(inst, 11, 8) << 1) |
                   (BITS(inst, 7, 7) << 11);
    int32_t immU = (SEXT(BITS(inst, 31, 12), 20) << 12);
    int32_t immJ = (SEXT(BITS(inst, 31, 31), 1) << 20) |
                   (BITS(inst, 30, 21) << 1) | (BITS(inst, 20, 20) << 11) |
                   (BITS(inst, 19, 12) << 12);
    int32_t immS = (SEXT(BITS(inst, 31, 25), 7) << 5) | BITS(inst, 11, 7);

    dec->immI = immI;
    dec->immB = immB;
    dec->immU = immU;
    dec->immJ = immJ;
    dec->immS = immS;

    dec->access_addr = 0;
    dec->except = EXC_None;
    dec->intr = INTR_None;

    decode_inst(dec);
}

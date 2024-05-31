#include "cpu/except.h"
#include <cpu/decode.h>

RiscvDecode decode(uint32_t inst) {
    RiscvDecode ret;
    ret.inst = inst;

    ret.rd = BITS(inst, 11, 7);
    ret.rs1 = BITS(inst, 19, 15);
    ret.rs2 = BITS(inst, 24, 20);

    ret.immI = SEXT(BITS(inst, 31, 20), 12);
    ret.immB = (SEXT(BITS(inst, 31, 31), 1) << 12) | (BITS(inst, 30, 25) << 5) |
               (BITS(inst, 11, 8) << 1) | (BITS(inst, 7, 7) << 11);
    ret.immU = (SEXT(BITS(inst, 31, 12), 20) << 12);
    ret.immJ = (SEXT(BITS(inst, 31, 31), 1) << 20) | (BITS(inst, 30, 21) << 1) |
               (BITS(inst, 20, 20) << 11) | (BITS(inst, 19, 12) << 12);
    ret.immS = (SEXT(BITS(inst, 31, 25), 7) << 5) | BITS(inst, 11, 7);

    ret.except = EXC_None;

    return ret;
}

#include "core/rvcore64.h"
#include "core/rvdecode.h"

void riscvcore64_exec(struct RiscvCore64 *core, struct RiscvDecode *decode);

void riscvcore64_mmu_fetch(struct RiscvCore64 *core, struct RiscvDecode *decode);

void riscvcore64_mmu_read(struct RiscvCore64 *core, u64 addr, u8 size, u64 *data);

void riscvcore64_mmu_write(struct RiscvCore64 *core, u64 addr, u8 size, u64 data);

void riscvcore64_init(struct RiscvCore64 *core, struct DeviceFunc device_func) {
    core->pc = 0x80000000;
    core->mode = MACHINE;
    core->halt = false;
    core->device_func = device_func;
}

void riscvcore64_step(struct RiscvCore64 *core) {
    struct RiscvDecode decode; // decode当中保存每一次执行需要用到的临时信息
    riscv_decode_init(&decode);

    riscvcore64_mmu_fetch(core, &decode);
    riscv_decode_inst(&decode);
    riscvcore64_exec(core, &decode);

    if (decode.exception != EXC_NONE) {
        printf("%x\n", decode.inst_raw);
        ERROR("Exception occurred: %d", decode.exception);
    } else if (decode.interrupt != INT_NONE) {
        ERROR("Interrupt occurred: %d", decode.interrupt);
    } else {
        core->pc = decode.next_pc;
    }
}

bool riscvcore64_check_halt(struct RiscvCore64 *core) { return core->halt; }

void riscvcore64_mmu_read(struct RiscvCore64 *core, u64 addr, u8 size, u64 *data) {
    struct DeviceFunc device = core->device_func;
    device.read(device.context, addr, size, data);
}

void riscvcore64_mmu_write(struct RiscvCore64 *core, u64 addr, u8 size, u64 data) {
    struct DeviceFunc device = core->device_func;
    device.write(device.context, addr, size, data);
}

void riscvcore64_mmu_fetch(struct RiscvCore64 *core, struct RiscvDecode *decode) {
    u64 inst;
    riscvcore64_mmu_read(core, core->pc, 4, &inst);
    decode->inst_raw = inst;
}

void riscvcore64_exec(struct RiscvCore64 *core, struct RiscvDecode *decode) {

#define INSTBEGIN() switch (decode->inst) {
#define INSTEND()                                                                                  \
    default:                                                                                       \
        decode->exception = ILLEGAL_INSTRUCTION;                                                   \
        }
#define INSTEXE(name, ...)                                                                         \
    case inst_##name: {                                                                            \
        __VA_ARGS__;                                                                               \
        break;                                                                                     \
    }

#define Rd core->regs[decode->rd]
#define Mr(addr, size)                                                                             \
    ({                                                                                             \
        u64 data;                                                                                  \
        riscvcore64_mmu_read(core, addr, size, &data);                                             \
        data;                                                                                      \
    })
#define Mw(addr, size, data) riscvcore64_mmu_write(core, addr, size, data)

    u64 Rs1 = core->regs[decode->rs1], Rs2 = core->regs[decode->rs2];
    decode->next_pc = core->pc + 4;

    INSTBEGIN();
    // 64I
    INSTEXE(add, Rd = Rs1 + Rs2);
    INSTEXE(addw, Rd = (i32)(Rs1 + Rs2));
    INSTEXE(sub, Rd = Rs1 - Rs2);
    INSTEXE(subw, Rd = (i32)(Rs1 - Rs2));
    INSTEXE(xor, Rd = Rs1 ^ Rs2);
    INSTEXE(or, Rd = Rs1 | Rs2);
    INSTEXE(and, Rd = Rs1 & Rs2);
    INSTEXE(sll, Rd = Rs1 << (Rs2 & 0x3F));
    INSTEXE(sllw, Rd = (i32)(Rs1 << (Rs2 & 0x1F)));
    INSTEXE(slt, Rd = (i64)Rs1 < (i64)Rs2 ? 1 : 0);
    INSTEXE(sltu, Rd = Rs1 < Rs2 ? 1 : 0);
    INSTEXE(srl, Rd = Rs1 >> (Rs2 & 0x3F));
    INSTEXE(srlw, Rd = (i32)((u32)Rs1 >> (Rs2 & 0x1F)));
    INSTEXE(sra, Rd = (i64)Rs1 >> (Rs2 & 0x3F));
    INSTEXE(sraw, Rd = (i32)Rs1 >> (Rs2 & 0x1F));
    INSTEXE(addi, Rd = Rs1 + (i64)decode->immI);
    INSTEXE(addiw, Rd = (i32)(Rs1 + (i64)decode->immI));
    INSTEXE(xori, Rd = Rs1 ^ (u64)decode->immI);
    INSTEXE(ori, Rd = Rs1 | (u64)decode->immI);
    INSTEXE(andi, Rd = Rs1 & (u64)decode->immI);
    INSTEXE(slli, Rd = Rs1 << (decode->immI & 0x3F));
    INSTEXE(slliw, Rd = (i32)(Rs1 << (decode->immI & 0x1F)));
    INSTEXE(srli, Rd = Rs1 >> (decode->immI & 0x3F));
    INSTEXE(srliw, Rd = (i32)((u32)Rs1 >> (decode->immI & 0x1F)));
    INSTEXE(srai, Rd = (i64)Rs1 >> (decode->immI & 0x3F));
    INSTEXE(sraiw, Rd = (i32)Rs1 >> (decode->immI & 0x1F));
    INSTEXE(slti, Rd = (i64)Rs1 < (i64)decode->immI ? 1 : 0);
    INSTEXE(sltiu, Rd = Rs1 < (u64)decode->immI ? 1 : 0);
    INSTEXE(lb, u64 data = Mr(Rs1 + decode->immI, 1); Rd = (i8)data;);
    INSTEXE(lh, u64 data = Mr(Rs1 + decode->immI, 2); Rd = (i16)data;);
    INSTEXE(lw, u64 data = Mr(Rs1 + decode->immI, 4); Rd = (i32)data;);
    INSTEXE(lbu, Rd = Mr(Rs1 + decode->immI, 1));
    INSTEXE(lhu, Rd = Mr(Rs1 + decode->immI, 2));
    INSTEXE(lwu, Rd = Mr(Rs1 + decode->immI, 4));
    INSTEXE(ld, Rd = Mr(Rs1 + decode->immI, 8));
    INSTEXE(sb, Mw(Rs1 + decode->immS, 1, Rs2));
    INSTEXE(sh, Mw(Rs1 + decode->immS, 2, Rs2));
    INSTEXE(sw, Mw(Rs1 + decode->immS, 4, Rs2));
    INSTEXE(sd, Mw(Rs1 + decode->immS, 8, Rs2));
    INSTEXE(beq, if ((i64)Rs1 == (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bne, if ((i64)Rs1 != (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(blt, if ((i64)Rs1 < (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bge, if ((i64)Rs1 >= (i64)Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bltu, if (Rs1 < Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(bgeu, if (Rs1 >= Rs2) decode->next_pc = core->pc + decode->immB);
    INSTEXE(jal, decode->next_pc = core->pc + decode->immJ; Rd = core->pc + 4);
    INSTEXE(jalr, decode->next_pc = (Rs1 + decode->immI) & ~1; Rd = core->pc + 4);
    INSTEXE(lui, Rd = decode->immU);
    INSTEXE(auipc, Rd = core->pc + decode->immU);
    // 64M
    INSTEXE(mul, Rd = ((i64)Rs1 * (i64)Rs2));
    INSTEXE(mulh, Rd = ((i64)Rs1 * (i64)Rs2) >> 32);
    INSTEXE(mulsu, Rd = ((i64)Rs1 * (u64)Rs2) >> 32);
    INSTEXE(mulu, Rd = ((u64)Rs1 * (u64)Rs2) >> 32);
    INSTEXE(mulw, Rd = (i64)(i32)((i32)Rs1 * (i32)Rs2));
    INSTEXE(div, Rd = (Rs2 == 0)                                  ? -1
                      : ((i64)Rs1 == INT64_MIN && (i64)Rs2 == -1) ? Rs1
                                                                  : ((i64)Rs1 / (i64)Rs2););
    INSTEXE(divu, { Rd = (Rs2 == 0) ? UINT64_MAX : Rs1 / Rs2; });
    INSTEXE(divw, {
        i32 src1 = (i32)Rs1;
        i32 src2 = (i32)Rs2;
        i32 result = (src2 == 0) ? -1 : (src1 == INT32_MIN && src2 == -1) ? src1 : src1 / src2;
        Rd = (i64)(i32)result;
    });
    INSTEXE(divuw, {
        u32 src1 = (u32)Rs1;
        u32 src2 = (u32)Rs2;
        u32 result = (src2 == 0) ? UINT32_MAX : src1 / src2;
        Rd = (i64)(i32)result;
    });
    INSTEXE(rem, Rd = (Rs2 == 0)                                  ? Rs1
                      : ((i64)Rs1 == INT64_MIN && (i64)Rs2 == -1) ? 0
                                                                  : ((i64)Rs1 % (i64)Rs2););
    INSTEXE(remu, Rd = (Rs2 == 0) ? Rs1 : Rs1 % Rs2);
    INSTEXE(remw, {
        i32 src1 = (i32)Rs1;
        i32 src2 = (i32)Rs2;
        i32 result = (src2 == 0) ? src1 : (src1 == INT32_MIN && src2 == -1) ? 0 : src1 % src2;
        Rd = (i64)(i32)result;
    });
    INSTEXE(remuw, {
        u32 src1 = (u32)Rs1;
        u32 src2 = (u32)Rs2;
        u32 result = (src2 == 0) ? src1 : src1 % src2;
        Rd = (i64)(i32)result;
    });
    // SYSTEM
    INSTEXE(ebreak, core->halt = true);

    INSTEND();

    core->regs[0] = 0;
}

#ifndef DECODE_H
#define DECODE_H

#include "common/common.h"
#include "common/debug.h"
#include "cpu/except.h"
#include "device/device.h"
#include <stdint.h>

typedef struct RiscvDecode {
    uint32_t inst;
    // 从指令当中获取的信息
    uint32_t rd, rs1, rs2;
    int32_t immI, immB, immU, immJ, immS;
    // 执行中得到的信息
    ExceptType except; // 保存异常
    IntrType intr;     // 保存触发后的中断
    uint32_t next_pc;
    uint64_t access_addr;
} RiscvDecode;

// 从指令提取需要的信息
RiscvDecode decode(uint32_t inst);

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

#endif // !DECODE_H

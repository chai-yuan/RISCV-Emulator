#ifndef MACRO_H_
#define MACRO_H_

#include "config.h"

#define GET_BITFIELD(value, start, len) (((value) >> (start)) & ((1ll << (len)) - 1))
#define SET_BITFIELD(value, start, len, field)                                                                         \
    ((value) = ((value) & ~(((1ll << (len)) - 1) << (start))) | (((field) & ((1ll << (len)) - 1)) << (start)))

#define IFDEF(macro, ...) MUXDEF(macro, __KEEP, __IGNORE)(__VA_ARGS__)
#define IFNDEF(macro, ...) MUXNDEF(macro, __KEEP, __IGNORE)(__VA_ARGS__)
#define IFONE(macro, ...) MUXONE(macro, __KEEP, __IGNORE)(__VA_ARGS__)
#define IFZERO(macro, ...) MUXZERO(macro, __KEEP, __IGNORE)(__VA_ARGS__)

#define REG64(base, offset) *(volatile u64 *)((base) + (offset))
#define REG32(base, offset) *(volatile u32 *)((base) + (offset))
#define REG16(base, offset) *(volatile u16 *)((base) + (offset))
#define REG8(base, offset) *(volatile u8 *)((base) + (offset))

#if CURRENT_ARCH == ARCH_RV64
    #define IS_RV64(rv64, rv32) rv64
#else
    #define IS_RV64(rv64, rv32) rv32
#endif

#endif

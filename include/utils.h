#ifndef UTILS_H__
#define UTILS_H__

#include "types.h"

struct uint128 {
    u32 nums[4];
};

struct uint128 unsigned2u128(u64 num);
struct uint128 signed2u128(i64 num);
struct uint128 u128mul(struct uint128 a, struct uint128 b);

#endif

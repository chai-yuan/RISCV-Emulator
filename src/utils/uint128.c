#include "utils.h"

struct uint128 unsigned2u128(u64 num) {
    return (struct uint128){.nums = {num & 0xffffffff, num >> 32, 0, 0}};
}
struct uint128 signed2u128(i64 num) {
    if (num < 0) {
        return (struct uint128){.nums = {num & 0xffffffff, num >> 32, 0xffffffff, 0xffffffff}};
    }
    return (struct uint128){.nums = {num & 0xffffffff, num >> 32, 0, 0}};
}

struct uint128 u128mul(struct uint128 a, struct uint128 b) {
    struct uint128 ret    = {0};
    u64            ans[8] = {0};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ans[i + j] += (u64)a.nums[i] * (u64)b.nums[j];
            ans[i + j + 1] += ans[i + j] >> 32;
            ans[i+j] &= 0xffffffff;
        }
    }

    for (int i = 0; i < 4; i++)
        ret.nums[i] = ans[i];
    return ret;
}

#ifndef DIFF_H
#define DIFF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define __EXPORT __attribute__((visibility("default")))
enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

extern void (*ref_difftest_memcpy)(uint64_t addr, void *buf, size_t n,
                                   bool direction);
extern void (*ref_difftest_regcpy)(void *dut, bool direction);
extern void (*ref_difftest_exec)(uint64_t n);
extern void (*ref_difftest_raise_intr)(uint64_t NO);

void init_difftest(char *ref_so_file);

void check_difftest();

#endif // !DIFF_H

#include "cpu/riscv32.h"
#include <common/common.h>
#include <diff.h>
#include <dlfcn.h>
#include <stdint.h>

void (*ref_difftest_memcpy)(uint64_t addr, void *buf, size_t n,
                            bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

void init_difftest(char *ref_so_file) {
    assert(ref_so_file != NULL);

    void *handle;
    handle = dlopen(ref_so_file, RTLD_LAZY);
    assert(handle);

    ref_difftest_memcpy = dlsym(handle, "difftest_memcpy");
    assert(ref_difftest_memcpy);

    ref_difftest_regcpy = dlsym(handle, "difftest_regcpy");
    assert(ref_difftest_regcpy);

    ref_difftest_exec = dlsym(handle, "difftest_exec");
    assert(ref_difftest_exec);

    ref_difftest_raise_intr = dlsym(handle, "difftest_raise_intr");
    assert(ref_difftest_raise_intr);

    void (*ref_difftest_init)(int) = dlsym(handle, "difftest_init");
    assert(ref_difftest_init);

    Log("Differential testing: %s", ANSI_FMT("ON", ANSI_FG_GREEN));
    Log("The result of every instruction will be compared with %s. "
        "This will help you a lot for debugging, but also significantly reduce "
        "the performance. "
        "If it is not necessary, you can turn it off in menuconfig.",
        ref_so_file);

    ref_difftest_init(0);
}

void check_difftest(Riscv32core *dut) {
    Riscv32core diff_cpu;
    bool check = false;
    ref_difftest_regcpy(&diff_cpu, DIFFTEST_TO_DUT);
    if (diff_cpu.pc != dut->pc) {
        check = true;
    }
    for (int i = 0; i < 32; i++) {
        if (diff_cpu.regs[i] != dut->regs[i]) {
            check = true;
        }
    }

    if (check) {
        Log("ref cpu");
        riscv32_dump(&diff_cpu);
        Log("dut cpu");
        riscv32_dump(dut);
        exit(1);
    }
}

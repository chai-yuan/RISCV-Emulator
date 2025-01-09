#include "core/rvcore.h"
#include "core/rvcore_priv.h"
#include "debug.h"

void riscvcore_step(struct RiscvCore *core) {
    riscv_decode_init(&core->decode);

    riscvcore_mmu_fetch(core);
    if (core->decode.exception == EXC_NONE) {
        riscv_decode_inst(&core->decode);
        riscvcore_exec(core);
    }

    if (core->decode.exception != EXC_NONE)
        riscv_exception_handle(core);
    else if (core->decode.interrupt != INT_NONE)
        riscv_interrupt_handle(core);

    core->pc = core->decode.next_pc;
}

void riscvcore_init(struct RiscvCore *core, struct DeviceFunc device_func) {
    for (int i = 0; i < sizeof(struct RiscvCore); i++)
        *((u8 *)core + i) = 0;
    core->pc                = 0x80000000;
    core->mode              = MACHINE;
    core->reservation_valid = false;
    core->halt              = false;
    core->device_func       = device_func;
    riscv_decode_init(&core->decode);
    core->csrs[MISA] = sizeof(usize) == 4 ? (0x1 << 30) : (0x2LL << 62);
    core->csrs[MISA] |= 0x141105; // TODO

#if CURRENT_ARCH == ARCH_RV64
    core->csrs[MSTATUS] |= (1ull << 32) | (1ull << 34); // UXL SXL
    core->csrs[SSTATUS] |= (1ull << 32);
#endif
}

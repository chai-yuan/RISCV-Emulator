#include "core/rvcore.h"
#include "core/rvcore_priv.h"

void riscvcore_step(struct RiscvCore *core) {
    riscv_decode_init(&core->decode);

    riscvcore_mmu_fetch(core, &core->decode);
    if (core->decode.exception == EXC_NONE) {
        riscv_decode_inst(&core->decode);
        riscvcore_exec(core, &core->decode);
    }

    if (core->decode.exception != EXC_NONE)
        riscv_exception_handle(core, &core->decode);
    else if (core->decode.interrupt != INT_NONE)
        riscv_interrupt_handle(core, &core->decode);

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
    core->misa = sizeof(usize) == 4 ? (0x1 << 30) : (0x2LL << 62);
}

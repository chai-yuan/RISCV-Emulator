#include "core/rvcore.h"
#include "core/rvcore_priv.h"

void riscvcore_step(struct RiscvCore *core) {
    struct RiscvDecode decode; // decode当中保存每一次执行需要用到的临时信息
    riscv_decode_init(&decode);

    riscvcore_mmu_fetch(core, &decode);
    riscv_decode_inst(&decode);
    riscvcore_exec(core, &decode);

    if (decode.exception != EXC_NONE)
        riscv_exception_handle(core, &decode);
    if (decode.interrupt != INT_NONE)
        riscv_interrupt_handle(core, &decode);

    core->pc = decode.next_pc;
}

void riscvcore_init(struct RiscvCore *core, struct DeviceFunc device_func) {
    core->pc                = 0x80000000;
    core->mode              = MACHINE;
    core->reservation_valid = false;
    core->halt              = false;
    core->device_func       = device_func;
    for (int i = 0; i < 32; i++)
        core->regs[i] = 0;
    for (int i = 0; i < 4096; i++)
        core->csrs[i] = 0;
}

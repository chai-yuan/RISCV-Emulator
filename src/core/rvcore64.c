#include "core/rvcore64.h"
#include "core/rvcore_priv.h"
#include "debug.h"

void riscvcore64_step(struct RiscvCore64 *core) {
    struct RiscvDecode decode; // decode当中保存每一次执行需要用到的临时信息
    riscv_decode_init(&decode);

    riscvcore64_mmu_fetch(core, &decode);
    riscv_decode_inst(&decode);
    riscvcore64_exec(core, &decode);

    if (decode.exception != EXC_NONE) {
        WARN("Exception occurred: %d at PC : %llx", decode.exception, core->pc);
    } else if (decode.interrupt != INT_NONE) {
        WARN("Interrupt occurred: %d", decode.interrupt);
    }
    core->pc = decode.next_pc;
}

void riscvcore64_init(struct RiscvCore64 *core, struct DeviceFunc device_func) {
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

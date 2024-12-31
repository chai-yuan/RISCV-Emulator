#include "core/rvcore64.h"
#include "core/rvcore_priv.h"
#include "debug.h"

void riscvcore64_step(void *context) {
    struct RiscvCore64 *core = (struct RiscvCore64 *)context;
    struct RiscvDecode  decode; // decode当中保存每一次执行需要用到的临时信息
    riscv_decode_init(&decode);

    riscvcore64_mmu_fetch(core, &decode);
    riscv_decode_inst(&decode);
    riscvcore64_exec(core, &decode);

    if (decode.exception != EXC_NONE) {
        WARN("Exception occurred: %d at PC : %llx", decode.exception, core->pc);
    } else if (decode.interrupt != INT_NONE) {
        WARN("Interrupt occurred: %d", decode.interrupt);
    } else {
        core->pc = decode.next_pc;
    }
}

bool riscvcore64_check_halt(void *context) {
    struct RiscvCore64 *core = (struct RiscvCore64 *)context;
    return core->halt;
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

struct CoreFunc riscvcore64_get_func(struct RiscvCore64 *core) {
    return (struct CoreFunc){
        .context    = core,
        .step       = riscvcore64_step,
        .check_halt = riscvcore64_check_halt,
    };
}

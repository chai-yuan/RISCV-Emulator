#include "core/rvcore.h"
#include "core.h"
#include "core/riscv.h"
#include "debug.h"

void riscvcore_update(struct RiscvCore *core, struct RiscvEnvInfo envinfo) {
    core->csrs[MIP] = 0;
    core->csrs[MIP] |= envinfo.mtint ? (1 << MACHINE_TIMER_INTERRUPT) : 0;
    core->csrs[TIME] = envinfo.time;
}

void riscvcore_step(struct RiscvCore *core, struct RiscvEnvInfo envinfo) {
    riscvcore_update(core, envinfo);

    if (riscv_check_pending_interrupt(core)) {
        riscv_trap_handle(core);
    } else if (!core->wfi) { // 如果没有处于休眠
        riscv_decode_init(&core->decode);
        riscvcore_mmu_fetch(core);
        if (core->decode.exception == EXC_NONE)
            riscvcore_exec(core);

        if (core->decode.exception != EXC_NONE)
            riscv_trap_handle(core);
    }

    core->pc = core->decode.next_pc;
}

void riscvcore_init(struct RiscvCore *core, struct DeviceFunc device_func) {
    for (int i = 0; i < sizeof(struct RiscvCore); i++)
        *((u8 *)core + i) = 0;
    core->pc                = 0x00001000;
    core->mode              = MACHINE;
    core->reservation_valid = false;
    core->wfi               = false;
    core->device_func       = device_func;

    core->csrs[MARCHID] = 0x5;

    // csr 初始化
#if CURRENT_ARCH == ARCH_RV64
    core->csrs[MISA]    = 0x8000000000141105;
    core->csrs[MSTATUS] = 0xa00000000;
#endif
}

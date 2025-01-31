#include "core/rvcore.h"
#include "core.h"

void riscvcore_update(struct RiscvCore *core, struct RiscvEnvInfo envinfo) {
    if (envinfo.eint) {
        if (core->mode == MACHINE) {
            CSRW(MIP, CSRR(MIP) | IP_MEIP);
        } else if (core->mode == SUPERVISOR) {
            CSRW(MIP, CSRR(MIP) | IP_SEIP);
        } else if (core->mode == USER) {
            CSRW(MIP, CSRR(MIP) | IP_UEIP);
        }
    }
}

void riscvcore_step(struct RiscvCore *core, struct RiscvEnvInfo envinfo) {
    riscvcore_update(core, envinfo);

    riscv_decode_init(&core->decode);
    riscvcore_mmu_fetch(core);
    if (core->decode.exception == EXC_NONE)
        riscvcore_exec(core);

    if (core->decode.exception != EXC_NONE || riscv_check_pending_interrupt(core))
        riscv_trap_handle(core);

    core->pc = core->decode.next_pc;
}

void riscvcore_init(struct RiscvCore *core, struct DeviceFunc device_func) {
    for (int i = 0; i < sizeof(struct RiscvCore); i++)
        *((u8 *)core + i) = 0;
    core->pc                = 0x00001000;
    core->mode              = MACHINE;
    core->reservation_valid = false;
    core->device_func       = device_func;
    core->csrs[MISA] = sizeof(usize) == 4 ? (0x1 << 30) : (0x2LL << 62);
    core->csrs[MISA] |= 0x141105; // TODO

#if CURRENT_ARCH == ARCH_RV64
    core->csrs[MSTATUS] |= (1ull << 32) | (1ull << 34); // UXL SXL
    core->csrs[SSTATUS] |= (1ull << 32);
#endif
}

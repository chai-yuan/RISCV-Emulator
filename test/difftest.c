#define ENABLE_DEBUG_MACROS 1 // 默认启用调试宏

#include "debug.h"
#include "machine/qemu64.h"
#include "string.h"

u8                   sram_data[128 * 1024 * 1024];
struct Qemu64Machine machine;

struct RiscvCore64 *difftest_init(const u8 *data, u64 data_size) {
    INFO("difftest_init");
    memcpy(sram_data, data, data_size);

    qemu64_machine_init(&machine, (struct Qemu64PortableOperations){
                                      .sram_data = sram_data,
                                      .sram_size = 128 * 1024 * 1024,
                                      .get_char  = NULL,
                                      .put_char  = NULL,
                                  });

    return &machine.core;
}

void difftest_step() {
    struct CoreFunc   core     = riscvcore64_get_func(&machine.core);
    struct DeviceFunc bus      = bus_device_get_func(&machine.bus);
    static u64        step_cnt = 0;

    core.step(core.context);
    if ((step_cnt++) % 10) {
        bus.update(bus.context, 10);
    }
}

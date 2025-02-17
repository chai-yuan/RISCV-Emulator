#include "device/clint.h"
#include "debug.h"
#include "device/device.h"

void clint_init(struct CLINT *clint) {
    REG64(clint->data, CLINT_MTIME)    = 0;
    REG64(clint->data, CLINT_MTIMECMP) = 0;
}

u64 clint_get_time(struct CLINT *clint) { return REG64(clint->data, CLINT_MTIME); }

bool clint_check_irq(struct CLINT *clint) {
    return REG64(clint->data, CLINT_MTIME) >= REG64(clint->data, CLINT_MTIMECMP);
}

static enum exception clint_read(void *context, u64 addr, u8 size, usize *data) {
    /*if (!((CLINT_MTIMECMP <= addr && addr <= CLINT_MTIMECMP + 8) || (CLINT_MTIME <= addr && addr <= CLINT_MTIME +
     * 8)))*/
    /*    return LOAD_ACCESS_FAULT;*/
    device_read(context, addr, size, data);
    return EXC_NONE;
}

static enum exception clint_write(void *context, u64 addr, u8 size, usize data) {
    /*if (!((CLINT_MTIMECMP <= addr && addr <= CLINT_MTIMECMP + 8) || (CLINT_MTIME <= addr && addr <= CLINT_MTIME +
     * 8)))*/
    /*    return STORE_AMO_ACCESS_FAULT;*/
    device_write(context, addr, size, data);
    return EXC_NONE;
}

static void clint_update(void *context, u32 interval) {
    struct CLINT *clint = (struct CLINT *)context;

    clint->last_update += interval;
    if (clint->last_update < 5000)
        return;
    clint->last_update = 0;

    REG64(clint->data, CLINT_MTIME) += 50;
    INFO("time : %llx, cmp : %llx", REG64(clint->data, CLINT_MTIME), REG64(clint->data, CLINT_MTIMECMP));
}

struct DeviceFunc clint_get_func(struct CLINT *clint) {
    struct DeviceFunc func = {
        .context = clint,
        .read    = clint_read,
        .write   = clint_write,
        .update  = clint_update,
    };
    return func;
}

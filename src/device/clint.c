#include "device/clint.h"
#include "device/device.h"

void clint_init(struct CLINT *clint) {
    REG64(clint->data, CLINT_MTIME)    = 0;
    REG64(clint->data, CLINT_MTIMECMP) = 0;
}

static enum exception clint_read(void *context, u64 addr, u8 size, usize *data) { return EXC_NONE; }

static enum exception clint_write(void *context, u64 addr, u8 size, usize data) { return EXC_NONE; }

static void clint_update(void *context, u32 interval) {}

static bool clint_check_timer_interrupt(void *context) {
    struct CLINT *clint = (struct CLINT *)context;
    return (REG64(clint->data, CLINT_MTIME) >= REG64(clint->data, CLINT_MTIMECMP)) &&
           (REG64(clint->data, CLINT_MTIMECMP) != 0);
}

struct DeviceFunc clint_get_func(struct CLINT *clint) {
    struct DeviceFunc func = {
        .context                  = clint,
        .read                     = clint_read,
        .write                    = clint_write,
        .update                   = clint_update,
        .check_external_interrupt = NULL,
        .check_timer_interrupt    = clint_check_timer_interrupt,
    };
    return func;
}

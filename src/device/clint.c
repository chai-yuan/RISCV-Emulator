#include "device/clint.h"
#include "device/device.h"

void clint_init(struct CLINT *clint) {
    REG64(clint->data, CLINT_MTIME)    = 0;
    REG64(clint->data, CLINT_MTIMECMP) = 0;
}

static u8 *clint_get_buffer(void *context, u64 address) {
    struct CLINT *clint = (struct CLINT *)context;
    return clint->data + address;
}

static enum exception clint_handle(void *context, u64 address, u8 size, bool write) {
    address = address & 0xFFF8;

    switch (address) {
    case CLINT_MTIMECMP:
    case CLINT_MTIME:
        return EXC_NONE;
    default:
        return write ? STORE_AMO_ACCESS_FAULT : LOAD_ACCESS_FAULT;
    }
}

static void clint_update(void *context, u32 interval) {
    //    struct CLINT *clint = (struct CLINT *)context;
    //   REG64(clint->data, CLINT_MTIME) += interval;
}

static bool clint_check_timer_interrupt(void *context) {
    struct CLINT *clint = (struct CLINT *)context;
    return (REG64(clint->data, CLINT_MTIME) >= REG64(clint->data, CLINT_MTIMECMP)) &&
           (REG64(clint->data, CLINT_MTIMECMP) != 0);
}

struct DeviceFunc clint_get_func(struct CLINT *clint) {
    struct DeviceFunc func = {
        .context                  = clint,
        .get_buffer               = clint_get_buffer,
        .handle                   = clint_handle,
        .update                   = clint_update,
        .check_external_interrupt = NULL,
        .check_timer_interrupt    = clint_check_timer_interrupt,
    };
    return func;
}

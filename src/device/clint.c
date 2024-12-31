#include "device/clint.h"
#include "device/device.h"

void clint_init(struct CLINT *clint) {
    REG64(clint->data, CLINT_MTIME)    = 0;
    REG64(clint->data, CLINT_MTIMECMP) = 0;
}

static enum exception clint_read(void *context, u64 address, u8 size, u64 *value) {
    struct CLINT *clint = (struct CLINT *)context;
    address             = address & 0xFFF8;

    switch (address) {
    case CLINT_MTIMECMP:
    case CLINT_MTIME:
        return read_buffer(clint->data, address, size, value);
    default:
        return LOAD_ACCESS_FAULT;
    }
    return EXC_NONE;
}

static enum exception clint_write(void *context, u64 address, u8 size, u64 value) {
    struct CLINT *clint = (struct CLINT *)context;
    address             = address & 0xFFF8;

    switch (address) {
    case CLINT_MTIMECMP:
    case CLINT_MTIME:
        return write_buffer(clint->data, address, size, value);
    default:
        return STORE_AMO_ACCESS_FAULT;
    }
    return EXC_NONE;
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
        .read                     = clint_read,
        .write                    = clint_write,
        .update                   = clint_update,
        .check_external_interrupt = NULL,
        .check_timer_interrupt    = clint_check_timer_interrupt,
    };
    return func;
}

#include "device/clint.h"
#include "core/rvdecode.h"
#include "device/device.h"

void clint_init(struct CLINT *clint) {
    clint->mtimecmp = 0;
    clint->mtime = 0;
}

static enum exception clint_read(void *context, u64 address, u8 size, u64 *value) {
    struct CLINT *clint = (struct CLINT *)context;

    switch (address) {
    case CLINT_MTIMECMP:
        *value = clint->mtimecmp;
        break;
    case CLINT_MTIME:
        *value = clint->mtime;
        break;
    default:
        return LOAD_ACCESS_FAULT;
    }

    return EXC_NONE;
}

static enum exception clint_write(void *context, u64 address, u8 size, u64 value) {
    struct CLINT *clint = (struct CLINT *)context;

    switch (address) {
    case CLINT_MTIMECMP:
        clint->mtimecmp = value;
        break;
    case CLINT_MTIME:
        clint->mtime = value;
        break;
    default:
        return STORE_AMO_ACCESS_FAULT;
    }

    return EXC_NONE;
}

static void clint_update(void *context, u32 interval) {
    struct CLINT *clint = (struct CLINT *)context;
    clint->mtime += interval;
}

static bool clint_check_timer_interrupt(void *context) {
    struct CLINT *clint = (struct CLINT *)context;
    return (clint->mtime >= clint->mtimecmp) && (clint->mtimecmp != 0);
}

struct DeviceFunc clint_get_func(struct CLINT *clint) {
    struct DeviceFunc func = {
        .context = clint,
        .read = clint_read,
        .write = clint_write,
        .update = clint_update,
        .check_external_interrupt = NULL,
        .check_timer_interrupt = clint_check_timer_interrupt,
    };
    return func;
}

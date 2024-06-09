#include "device/device.h"
#include <device/clint.h>
#include <stdint.h>
#include <stdlib.h>

DeviceInterface clint_func = {
    .read = clint_read,
    .write = clint_write,
    .update = clint_update,
    .check_intr = clint_check_intr,
};

#define CLINT_timer 0xbff8
#define CLINT_timermatch 0x4000

Clint *clint_init() {
    Clint *clint = malloc(sizeof(Clint));
    clint->timermatch = clint->timer = 0;
    return clint;
}

DeviceAccessStatus clint_read(void *device, uint64_t addr, uint8_t size,
                              uint64_t *data) {
    Clint *dev = (Clint *)device;
    clint_update(device);

    uint64_t a = addr & ~0x7, offset = (addr & 0x7) * 8;
    if (a == CLINT_timer) {
        *data = dev->timer >> offset;
    } else if (a == CLINT_timermatch) {
        *data = dev->timermatch >> offset;
    } else {
        return DEVICE_ACCESS_ERROR;
    }
    return DEVICE_ACCESS_OK;
}

// 只支持32位和64位写入，之后可以改进
DeviceAccessStatus clint_write(void *device, uint64_t addr, uint8_t size,
                               uint64_t data) {
    Clint *dev = (Clint *)device;
    if (addr == CLINT_timermatch) {
        if (size == 4) {
            dev->timermatch = (dev->timermatch & ~0xffff) | (data & 0xffff);
        } else {
            dev->timermatch = data;
        }
    } else if (addr == CLINT_timermatch + 4) {
        dev->timermatch = (dev->timermatch & 0xffff) | (data << 32);
    } else {
        return DEVICE_ACCESS_ERROR;
    }
    return DEVICE_ACCESS_OK;
}

DeviceIntrType clint_check_intr(void *device) {
    Clint *dev = (Clint *)device;
    if (dev->timermatch != 0 && dev->timermatch > dev->timer) {
        return DEVICE_INTR_TIMER;
    }
    return DEVICE_INTR_NULL;
}

void clint_update(void *device) {
    Clint *dev = (Clint *)device;
    dev->timer++;
}

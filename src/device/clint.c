#include "device/device.h"
#include <device/clint.h>

DeviceInterface clint_func = {
    .read = clint_read,
    .write = clint_write,
    .update = clint_update,
    .check_intr = clint_check_intr,
};

Clint *clint_init() { return NULL; }

DeviceAccessStatus clint_read(void *device, uint64_t addr, uint8_t size,
                              uint64_t *data) {
    Clint *dev = (Clint *)device;
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus clint_write(void *device, uint64_t addr, uint8_t size,
                               uint64_t data) {
    Clint *dev = (Clint *)device;
    return DEVICE_ACCESS_OK;
}

IntrType clint_check_intr(void *device) {
    Clint *dev = (Clint *)device;
    return INTR_NULL;
}

void clint_update(void *device) { Clint *dev = (Clint *)device; }


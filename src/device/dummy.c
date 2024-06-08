#include "device/device.h"
#include <device/dummy.h>

DeviceInterface dummy_func = {
    .read = dummy_read,
    .write = dummy_write,
    .update = dummy_update,
    .check_intr = dummy_check_intr,
};

Dummy *dummy_init() { return NULL; }

DeviceAccessStatus dummy_read(void *device, uint64_t addr, uint8_t size,
                              uint64_t *data) {
    Dummy *dev = (Dummy *)device;
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus dummy_write(void *device, uint64_t addr, uint8_t size,
                               uint64_t data) {
    Dummy *dev = (Dummy *)device;
    return DEVICE_ACCESS_OK;
}

IntrType dummy_check_intr(void *device) {
    Dummy *dev = (Dummy *)device;
    return INTR_NULL;
}

void dummy_update(void *device) { Dummy *dev = (Dummy *)device; }

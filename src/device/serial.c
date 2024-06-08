#include "device/device.h"
#include <device/serial.h>
#include <stdio.h>

DeviceInterface serial_func = {
    .read = serial_read,
    .write = serial_write,
    .update = serial_update,
    .check_intr = serial_check_intr,
};

Serial *serial_init() { return NULL; }

DeviceAccessStatus serial_read(void *device, uint64_t addr, uint8_t size,
                               uint64_t *data) {
    *data = 0;
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus serial_write(void *device, uint64_t addr, uint8_t size,
                                uint64_t data) {
    if (addr == 0) {
        printf("%c", (uint8_t)data);
        fflush(stdout);
    }
    return DEVICE_ACCESS_OK;
}

IntrType serial_check_intr(void *device) { return INTR_NULL; }

void serial_update(void *device) {}

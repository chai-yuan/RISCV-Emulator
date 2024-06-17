#ifndef SERIAL_H
#define SERIAL_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Serial {
} Serial;

extern DeviceInterface serial_func;

Serial *serial_init();

DeviceAccessStatus serial_read(void *device, uint64_t addr, uint8_t size,
                               uint64_t *data);

DeviceAccessStatus serial_write(void *device, uint64_t addr, uint8_t size,
                                uint64_t data);

DeviceIntrType serial_check_intr(void *device);

void serial_update(void *device);

#endif // !SERIAL_H

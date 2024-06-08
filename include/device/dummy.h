#ifndef DUMMY_H
#define DUMMY_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Dummy {
} Dummy;

extern DeviceInterface dummy_func;

Dummy *dummy_init();

DeviceAccessStatus dummy_read(void *device, uint64_t addr, uint8_t size,
                              uint64_t *data);

DeviceAccessStatus dummy_write(void *device, uint64_t addr, uint8_t size,
                               uint64_t data);

IntrType dummy_check_intr(void *device);

void dummy_update(void *device);

#endif // !DUMMY_H


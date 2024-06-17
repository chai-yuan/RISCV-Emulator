#ifndef CLINT_H
#define CLINT_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Clint {
    uint64_t timer, timermatch;
} Clint;

extern DeviceInterface clint_func;

Clint *clint_init();

DeviceAccessStatus clint_read(void *device, uint64_t addr, uint8_t size,
                              uint64_t *data);

DeviceAccessStatus clint_write(void *device, uint64_t addr, uint8_t size,
                               uint64_t data);

DeviceIntrType clint_check_intr(void *device);

void clint_update(void *device);

#endif // !CLINT_H

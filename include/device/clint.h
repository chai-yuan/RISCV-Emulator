#ifndef CLINT_H
#define CLINT_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

IntrType clint_check_intr();

void clint_update();

DeviceAccessStatus clint_read(uint64_t addr, uint8_t size, uint64_t *data);

DeviceAccessStatus clint_write(uint64_t addr, uint8_t size, uint64_t data);

#endif // !CLINT_H

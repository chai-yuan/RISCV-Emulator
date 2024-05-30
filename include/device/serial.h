#ifndef SERIAL_H
#define SERIAL_H

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

DeviceAccessStatus serial_read(uint64_t addr, uint8_t size, uint64_t *data);

DeviceAccessStatus serial_write(uint64_t addr, uint8_t size, uint64_t data);

#endif // !SERIAL_H

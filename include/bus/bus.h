#ifndef BUS_H
#define BUS_H

#include <common/common.h>
#include <device/device.h>
#include <stdint.h>

int bus_add_device(Device device);

IntrType bus_check_intr();

DeviceAccessStatus bus_read(uint64_t addr, uint8_t size, uint64_t *data);

DeviceAccessStatus bus_write(uint64_t addr, uint8_t size, uint64_t data);

#endif // !BUS_H

#include "device/device.h"
#include <bus/bus.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_DEVICES_NUM 8
Device devices[MAX_DEVICES_NUM];
size_t device_num = 0;

int bus_add_device(Device device) {
    devices[device_num] = device;
    device_num++;
    return 0;
}

IntrType bus_check_intr() { return INTR_NULL; }

DeviceAccessStatus bus_read(uint64_t addr, uint8_t size, uint64_t *data) {
    for (size_t i = 0; i < device_num; i++) {
        if (addr >= devices[i].addr &&
            addr < devices[i].addr + devices[i].size) {
            devices[i].read(addr - devices[i].addr, size, data);
            return DEVICE_ACCESS_OK;
        }
    }
    return DEVICE_ACCESS_ERROR;
}

DeviceAccessStatus bus_write(uint64_t addr, uint8_t size, uint64_t data) {
    for (size_t i = 0; i < device_num; i++) {
        if (addr >= devices[i].addr &&
            addr < devices[i].addr + devices[i].size) {
            devices[i].write(addr - devices[i].addr, size, data);
            return DEVICE_ACCESS_OK;
        }
    }
    return DEVICE_ACCESS_ERROR;
}

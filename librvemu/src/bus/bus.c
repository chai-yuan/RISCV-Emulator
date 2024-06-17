#include "common/debug.h"
#include "device/device.h"
#include <bus/bus.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_DEVICES_NUM 8
Device devices[MAX_DEVICES_NUM];
size_t device_num = 0;

int bus_add_device(uint64_t addr, uint64_t size, void *device,
                   DeviceInterface func) {
    if (device_num == MAX_DEVICES_NUM) {
        return -1;
    }

    devices[device_num] = (Device){
        .addr = addr,
        .size = size,
        .device_data = device,
        .func = func,
    };
    device_num++;
    return 0;
}

DeviceIntrType bus_check_intr() {
    for (int i = 0; i < device_num; i++) {
        if (devices[i].func.check_intr != NULL) {
            DeviceIntrType intr =
                devices[i].func.check_intr(devices[i].device_data);

            if (intr != DEVICE_INTR_NULL) {
                return intr;
            }
        }
    }
    return DEVICE_INTR_NULL;
}

void bus_update() {
    for (int i = 0; i < device_num; i++) {
        if (devices[i].func.update != NULL)
            devices[i].func.update(devices[i].device_data);
    }
}

DeviceAccessStatus bus_read(uint64_t addr, uint8_t size, uint64_t *data) {
    for (size_t i = 0; i < device_num; i++) {
        if (addr >= devices[i].addr &&
            addr < devices[i].addr + devices[i].size &&
            devices[i].func.read != NULL) {

            devices[i].func.read(devices[i].device_data, addr - devices[i].addr,
                                 size, data);
            return DEVICE_ACCESS_OK;
        }
    }
    panic("访问设备不存在 addr : 0x%x", (uint32_t)addr);
    return DEVICE_ACCESS_ERROR;
}

DeviceAccessStatus bus_write(uint64_t addr, uint8_t size, uint64_t data) {
    for (size_t i = 0; i < device_num; i++) {
        if (addr >= devices[i].addr &&
            addr < devices[i].addr + devices[i].size &&
            devices[i].func.write != NULL) {

            devices[i].func.write(devices[i].device_data,
                                  addr - devices[i].addr, size, data);
            return DEVICE_ACCESS_OK;
        }
    }
    panic("访问设备不存在 addr : 0x%x", (uint32_t)addr);
    return DEVICE_ACCESS_ERROR;
}

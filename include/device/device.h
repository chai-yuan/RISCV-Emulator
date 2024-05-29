#ifndef DEVICE_H
#define DEVICE_H

#include <common/common.h>
#include <stdint.h>

typedef enum DeviceAccessStatus {
    DEVICE_ACCESS_OK,
    DEVICE_ACCESS_ERROR,
} DeviceAccessStatus;

typedef enum DeviceIntrStatus {
    DEVICE_INTR_NULL,
    DEVICE_INTR_TIME,
} DeviceIntrStatus;

typedef struct Device {
    uint64_t addr;
    uint64_t size;
    DeviceAccessStatus (*read)(uint64_t addr, uint8_t size, uint64_t *data);
    DeviceAccessStatus (*write)(uint64_t addr, uint8_t size, uint64_t data);
    DeviceIntrStatus (*check_intr)();
} Device;

#endif // !DEVICE_H

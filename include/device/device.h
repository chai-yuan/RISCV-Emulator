#ifndef DEVICE_H
#define DEVICE_H

#include <common/common.h>
#include <stdint.h>

typedef enum DeviceAccessStatus {
    DEVICE_ACCESS_OK,
    DEVICE_ACCESS_ERROR,
} DeviceAccessStatus;

typedef enum DeviceIntrType {
    DEVICE_INTR_NULL,
    DEVICE_INTR_TIMER,
} DeviceIntrType;

typedef struct DeviceInterface {
    DeviceAccessStatus (*read)(void *, uint64_t, uint8_t, uint64_t *);
    DeviceAccessStatus (*write)(void *, uint64_t, uint8_t, uint64_t);
    DeviceIntrType (*check_intr)(void *);
    void (*update)(void *);
} DeviceInterface;

typedef struct Device {
    uint64_t addr, size;
    void *device_data;
    DeviceInterface func;
} Device;

#endif // !DEVICE_H

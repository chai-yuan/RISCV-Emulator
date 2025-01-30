#ifndef BUS_H
#define BUS_H

#include "device/device.h"

// 子设备描述结构体
struct SubDevice {
    u64               base; // 基地址
    u64               size; // 地址范围大小
    struct DeviceFunc func; // 设备函数
};

#define MAX_BUS_DEVICES 8

// 总线设备结构体
struct BusDevice {
    struct SubDevice sub_devices[MAX_BUS_DEVICES]; // 子设备数组
    int              num_sub_devices;              // 子设备数量
};

// 总线设备初始化函数
void bus_device_init(struct BusDevice *bus);

// 向总线当中添加子设备
void bus_device_add(struct BusDevice *bus, u64 base, u64 size, struct DeviceFunc func);

// 获得总线设备
struct DeviceFunc bus_device_get_func(struct BusDevice *bus);

#endif

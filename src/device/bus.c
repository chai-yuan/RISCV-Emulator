#include "device/bus.h"

void bus_device_init(struct BusDevice *bus) { bus->num_sub_devices = 0; }

void bus_device_add_sub_device(struct BusDevice *bus, u64 base, u64 size, struct DeviceFunc func) {
    bus->num_sub_devices++;
    if (bus->num_sub_devices >= MAX_BUS_DEVICES) {
        ERROR("Failed to allocate memory for sub devices");
    }

    bus->sub_devices[bus->num_sub_devices - 1] = (struct SubDevice){
        .base = base,
        .size = size,
        .func = func,
    };
}

enum exception bus_device_read(void *context, u64 address, u8 size, u64 *value) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (address >= sub->base && address < sub->base + sub->size) {
            return sub->func.read(sub->func.context, address - sub->base, size, value);
        }
    }

    // 如果没有找到匹配的设备，返回访问错误
    return STORE_AMO_ACCESS_FAULT;
}

enum exception bus_device_write(void *context, u64 address, u8 size, u64 value) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (address >= sub->base && address < sub->base + sub->size) {
            return sub->func.write(sub->func.context, address - sub->base, size, value);
        }
    }

    // 如果没有找到匹配的设备，返回访问错误
    return STORE_AMO_ACCESS_FAULT;
}

bool bus_device_check_external_interrupt(void *context) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (sub->func.check_external_interrupt &&
            sub->func.check_external_interrupt(sub->func.context)) {
            return true;
        }
    }

    return false;
}

bool bus_device_check_timer_interrupt(void *context) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (sub->func.check_timer_interrupt && sub->func.check_timer_interrupt(sub->func.context)) {
            return true;
        }
    }

    return false;
}

void bus_device_update(void *context) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (sub->func.update)
            sub->func.update(sub->func.context);
    }
}

struct DeviceFunc bus_device_get_func(struct BusDevice *bus) {
    return (struct DeviceFunc){
        .context = bus,
        .read = bus_device_read,
        .write = bus_device_write,
        .update = bus_device_update,
        .check_external_interrupt = bus_device_check_external_interrupt,
        .check_timer_interrupt = bus_device_check_timer_interrupt,
    };
}

#include "device/bus.h"
#include "debug.h"

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

static u8 *bus_get_buffer(void *context, u64 address) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (address >= sub->base && address < sub->base + sub->size) {
            return sub->func.get_buffer(sub->func.context, address - sub->base);
        }
    }

    ERROR("address out of bounds, addr : %llx",address);
    return NULL;
}

static enum exception bus_handle(void *context, u64 address, u8 size, bool write) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (address >= sub->base && address < sub->base + sub->size) {
            return sub->func.handle(sub->func.context, address - sub->base, size, write);
        }
    }

    ERROR("address out of bounds, addr : %llx",address);
    return LOAD_ACCESS_FAULT;
}

static bool bus_device_check_external_interrupt(void *context) {
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

static bool bus_device_check_timer_interrupt(void *context) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (sub->func.check_timer_interrupt && sub->func.check_timer_interrupt(sub->func.context)) {
            return true;
        }
    }

    return false;
}

static void bus_device_update(void *context, u32 interval) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (sub->func.update)
            sub->func.update(sub->func.context, interval);
    }
}

struct DeviceFunc bus_device_get_func(struct BusDevice *bus) {
    return (struct DeviceFunc){
        .context                  = bus,
        .get_buffer               = bus_get_buffer,
        .handle                   = bus_handle,
        .update                   = bus_device_update,
        .check_external_interrupt = bus_device_check_external_interrupt,
        .check_timer_interrupt    = bus_device_check_timer_interrupt,
    };
}

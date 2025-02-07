#include "device/bus.h"
#include "debug.h"

void bus_device_init(struct BusDevice *bus) { bus->num_sub_devices = 0; }

void bus_device_add(struct BusDevice *bus, u64 base, u64 size, struct DeviceFunc func) {
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

static enum exception bus_read(void *context, u64 address, u8 size, usize *data) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (address >= sub->base && address < sub->base + sub->size) {
            return sub->func.read(sub->func.context, address - sub->base, size, data);
        }
    }

    ERROR("address out of bounds, addr : %llx", address);
    return LOAD_ACCESS_FAULT;
}

static enum exception bus_write(void *context, u64 address, u8 size, usize data) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (address >= sub->base && address < sub->base + sub->size) {
            return sub->func.write(sub->func.context, address - sub->base, size, data);
        }
    }

    ERROR("address out of bounds, addr : %llx", address);
    return STORE_AMO_ACCESS_FAULT;
}

static void bus_update(void *context, u32 interval) {
    struct BusDevice *bus = (struct BusDevice *)context;

    for (int i = 0; i < bus->num_sub_devices; i++) {
        struct SubDevice *sub = &bus->sub_devices[i];
        if (sub->func.update)
            sub->func.update(sub->func.context, interval);
    }
}

struct DeviceFunc bus_device_get_func(struct BusDevice *bus) {
    return (struct DeviceFunc){
        .context = bus,
        .read    = bus_read,
        .write   = bus_write,
        .update  = bus_update,
    };
}

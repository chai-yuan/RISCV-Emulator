#include "bus/bus.h"
#include "common/debug.h"
#include "device/device.h"
#include <cpu/mmu.h>
#include <stdint.h>

void mmu_fetch_32(Riscv32core *core, RiscvDecode *dec) {
    uint64_t read_data;
    DeviceAccessStatus ret = bus_read(core->pc, 4, &read_data);
    if (ret == DEVICE_ACCESS_OK) {
        dec->inst = read_data & 0xffffffff;
    } else {
        panic("取指出错");
    }
}

void mmu_read_32(Riscv32core *core, RiscvDecode *dec, uint32_t addr,
                 uint8_t size, uint32_t *data) {
    uint64_t read_data;
    DeviceAccessStatus ret = bus_read(addr, size, &read_data);

    if (ret == DEVICE_ACCESS_OK) {
        dec->access_addr = addr;

        switch (size) {
        case 1:
            *data = read_data & 0xff;
            break;
        case 2:
            *data = read_data & 0xffff;
            break;
        case 4:
            *data = read_data & 0xffffffff;
            break;
        default:
            panic("未知读取长度");
        }
    } else {
        panic("访存出错");
    }
}

void mmu_write_32(Riscv32core *core, RiscvDecode *dec, uint32_t addr,
                  uint8_t size, uint32_t data) {

    DeviceAccessStatus ret = bus_write(addr, size, data);
    if (ret == DEVICE_ACCESS_OK) {
    } else {
        panic("访存出错");
    }
}

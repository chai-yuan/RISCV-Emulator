#include "device/device.h"
#include <common/time.h>
#include <device/clint.h>
#include <stdint.h>

#define CLINT_UPDATE_US 0x10 // 更新时间间隔
static uint64_t last_update_time = 0;
static uint64_t timer = 0, timermatch = 0;

IntrType clint_check_intr() {
    if (timermatch != 0 && timer > timermatch) {
        return INTR_TIMER;
    }
    return INTR_NULL;
}

void clint_update() {
    if (last_update_time == 0) {
        last_update_time = get_time_us();
    } else if (get_time_us() - last_update_time > CLINT_UPDATE_US) {
        last_update_time = get_time_us();
        timer++;
    }
}

DeviceAccessStatus clint_read(uint64_t addr, uint8_t size, uint64_t *data) {
    return DEVICE_ACCESS_OK;
}

DeviceAccessStatus clint_write(uint64_t addr, uint8_t size, uint64_t data) {
    return DEVICE_ACCESS_OK;
}

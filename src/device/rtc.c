#include "device/rtc.h"
#include "core/rvdecode.h"
#include "device/device.h"

void rtc_init(struct RTC *rtc, get_time_func_t get_time) {
    rtc->interrupt_interval = 0; // 默认不触发中断
    rtc->last_check_time = get_time();
    rtc->interrupt_pending = false;
    rtc->get_time = get_time;
}

static enum exception rtc_read(void *context, u64 address, u8 size, u64 *value) {
    struct RTC *rtc = (struct RTC *)context;
    u64 current_time = rtc->get_time();

    switch (address) {
        case RTC_TIME_LOW:
            *value = current_time & 0xFFFFFFFF; // 当前时间的低 32 位
            break;
        case RTC_TIME_HIGH:
            *value = (current_time >> 32) & 0xFFFFFFFF; // 当前时间的高 32 位
            break;
        case RTC_INTERRUPT_INTERVAL:
            *value = rtc->interrupt_interval; // 中断触发间隔
            break;
        case RTC_INTERRUPT_STATUS:
            *value = rtc->interrupt_pending ? 1 : 0; // 中断状态
            rtc->interrupt_pending = false; // 读取后清除中断状态
            break;
        default:
            return LOAD_ACCESS_FAULT;
    }

    return EXC_NONE;
}

static enum exception rtc_write(void *context, u64 address, u8 size, u64 value) {
    struct RTC *rtc = (struct RTC *)context;

    switch (address) {
        case RTC_INTERRUPT_INTERVAL:
            rtc->interrupt_interval = value; // 设置中断触发间隔
            rtc->last_check_time = rtc->get_time(); // 重置上次检查时间
            break;
        case RTC_INTERRUPT_STATUS:
            rtc->interrupt_pending = false; // 写入任意值清除中断状态
            break;
        default:
            return STORE_AMO_ACCESS_FAULT;
    }

    return EXC_NONE;
}

static void rtc_update(void *context, u32 interval) {
    struct RTC *rtc = (struct RTC *)context;
    u64 current_time = rtc->get_time();

    if (rtc->interrupt_interval > 0 && current_time - rtc->last_check_time >= rtc->interrupt_interval) {
        rtc->last_check_time = current_time;
        rtc->interrupt_pending = true; // 设置中断待处理标志
    }
}

static bool rtc_check_external_interrupt(void *context) {
    struct RTC *rtc = (struct RTC *)context;
    return rtc->interrupt_pending; // 返回中断待处理状态
}

struct DeviceFunc rtc_get_func(struct RTC *rtc) {
    struct DeviceFunc func = {
        .context = rtc,
        .read = rtc_read,
        .write = rtc_write,
        .update = rtc_update,
        .check_external_interrupt = rtc_check_external_interrupt,
        .check_timer_interrupt = NULL,
    };
    return func;
}

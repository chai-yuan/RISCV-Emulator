#ifndef RTC_H
#define RTC_H

#include "types.h"

#define RTC_SIZE 0x1000
#define RTC_TIME_LOW  0x0000 // 当前时间的低 32 位
#define RTC_TIME_HIGH 0x0004 // 当前时间的高 32 位
#define RTC_INTERRUPT_INTERVAL 0x0008 // 中断触发间隔
#define RTC_INTERRUPT_STATUS   0x000C // 中断状态寄存器

typedef u64 (*get_time_func_t)(void);

struct RTC {
    u64 interrupt_interval; // 中断触发间隔
    u64 last_check_time;    // 上次检查时间
    bool interrupt_pending; // 中断是否待处理
    get_time_func_t get_time; // 获取实时时间的函数
};

void rtc_init(struct RTC *rtc, get_time_func_t get_time);

struct DeviceFunc rtc_get_func(struct RTC *rtc);

#endif

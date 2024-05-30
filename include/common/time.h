#ifndef TIME_H
#define TIME_H

#include <common/common.h>
#include <stdint.h>
#include <sys/time.h>

uint64_t get_time_ms();

uint64_t get_time_us();

#endif // !TIME_H

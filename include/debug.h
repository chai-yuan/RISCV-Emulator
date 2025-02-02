#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"

// 定义调试级别
typedef enum {
    DEBUG_LEVEL_ERROR, // 仅输出错误信息
    DEBUG_LEVEL_WARN,  // 输出警告和错误信息
    DEBUG_LEVEL_INFO   // 输出信息、警告和错误信息
} debug_level_t;

// 设置全局调试级别
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_INFO // 默认调试级别为 INFO
#endif

#ifdef ENABLE_DEBUG_MACROS

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// ANSI 颜色代码
#define COLOR_RED "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

// 调试日志宏
#define DEBUG_PRINT(level, color, fmt, ...)                                                        \
    do {                                                                                           \
        if (level <= DEBUG_LEVEL) {                                                                \
            printf("%s[%s] %s:%d: " fmt COLOR_RESET "\n", color, #level, __FILE__, __LINE__,       \
                   ##__VA_ARGS__);                                                                 \
        }                                                                                          \
    } while (0)

// 错误日志
#define ERROR(fmt, ...)                                                                            \
    do {                                                                                           \
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, COLOR_RED, "ERROR: " fmt, ##__VA_ARGS__);                   \
        exit(1);                                                                                   \
    } while (0)

// 警告日志
#define WARN(fmt, ...) DEBUG_PRINT(DEBUG_LEVEL_WARN, COLOR_YELLOW, "WARN: " fmt, ##__VA_ARGS__)

// 信息日志
#define INFO(fmt, ...) DEBUG_PRINT(DEBUG_LEVEL_INFO, COLOR_GREEN, "INFO: " fmt, ##__VA_ARGS__)

// 检查条件并输出错误日志
#define ASSERT(condition, fmt, ...)                                                                \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            ERROR("Assertion failed: %s at %s:%d\n", #condition, __FILE__, __LINE__);              \
            ERROR(fmt, ##__VA_ARGS__);                                                             \
        }                                                                                          \
    } while (0)

#else

#define ERROR(...)
#define WARN(...)
#define INFO(...)
#define ASSERT(condition, ...)

#endif // ENABLE_DEBUG_MACROS && DEBUG_LEVEL != DEBUG_LEVEL_NONE

#endif // DEBUG_H

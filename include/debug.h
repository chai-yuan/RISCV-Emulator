#ifndef DEBUG_H
#define DEBUG_H

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

// 定义是否允许调试宏
#ifndef ENABLE_DEBUG_MACROS
#define ENABLE_DEBUG_MACROS 1 // 默认启用调试宏
#endif

#ifdef ENABLE_DEBUG_MACROS

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// 调试日志宏
#define DEBUG_PRINT(level, fmt, ...)                                           \
    do {                                                                       \
        if (level <= DEBUG_LEVEL) {                                            \
            printf("[%s] ", #level);                                           \
            printf(fmt, ##__VA_ARGS__);                                        \
        }                                                                      \
    } while (0)

// 错误日志
#define ERROR(fmt, ...)                                                        \
    do {                                                                       \
        DEBUG_PRINT(DEBUG_LEVEL_ERROR, "ERROR: " fmt, ##__VA_ARGS__);          \
        exit(1);                                                               \
    } while (0)

// 警告日志
#define WARN(fmt, ...)                                                         \
    DEBUG_PRINT(DEBUG_LEVEL_WARN, "WARN: " fmt, ##__VA_ARGS__)

// 信息日志
#define INFO(fmt, ...)                                                         \
    DEBUG_PRINT(DEBUG_LEVEL_INFO, "INFO: " fmt, ##__VA_ARGS__)

// 检查条件并输出错误日志
#define ASSERT(condition, fmt, ...)                                            \
    do {                                                                       \
        if (!(condition)) {                                                    \
            ERROR("Assertion failed: %s at %s:%d\n", #condition, __FILE__,     \
                  __LINE__);                                                   \
            ERROR(fmt, ##__VA_ARGS__);                                         \
        }                                                                      \
    } while (0)

#else

// 如果调试宏被禁用或调试级别为 NONE，则定义空宏
#define ERROR(fmt, ...)
#define WARN(fmt, ...)
#define INFO(fmt, ...)
#define ASSERT(condition, fmt, ...)

#endif // ENABLE_DEBUG_MACROS && DEBUG_LEVEL != DEBUG_LEVEL_NONE

#endif // DEBUG_H
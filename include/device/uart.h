#ifndef UART_H
#define UART_H

#include "common.h"

typedef void (*get_char_func_t)(u8 *data);
typedef void (*put_char_func_t)(u8 data);

struct Uart {
    // 外部提供的输入输出函数
    get_char_func_t get_char;
    put_char_func_t put_char;
};

#endif

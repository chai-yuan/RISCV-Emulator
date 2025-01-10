#ifndef UART_H
#define UART_H

#include "types.h"

// 输入输出函数，返回值表示是否成功(是否读取到数据)
typedef bool (*get_char_func_t)(u8 *data);
typedef void (*put_char_func_t)(u8 data);

#define UART_SIZE (0x8)
#define UART_THR (0)  // TX
#define UART_RHR (0)  // RX
#define UART_IER (1)
#define UART_IIR (2)
#define UART_FCR (2)
#define UART_LSR (5)
#define UART_LSR (5)
// 线路状态寄存器标志位
#define UART_LSR_RX_EMPTY (1 << 0)
#define UART_LSR_TX_EMPTY (1 << 5)
#define UART_LSR_THR_SR_EMPTY (1 << 6)

struct Uart {
    u8              data[UART_SIZE]; // 模拟 UART 寄存器
    get_char_func_t get_char;        // 外部提供的输入函数
    put_char_func_t put_char;        // 外部提供的输出函数
};

void uart_init(struct Uart *uart, get_char_func_t get, put_char_func_t put);

struct DeviceFunc uart_get_func(struct Uart *uart);

#endif

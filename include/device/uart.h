#ifndef UART_H
#define UART_H

#include "common.h"

// 输入输出函数，返回值表示是否成功(是否读取到数据)
typedef bool (*get_char_func_t)(u8 *data);
typedef void (*put_char_func_t)(u8 data);

// UART 寄存器偏移量
#define UART_RHR 0x00 // 接收保持寄存器
#define UART_THR 0x00 // 发送保持寄存器
#define UART_LSR 0x05 // 线路状态寄存器

// 线路状态寄存器标志位
#define UART_LSR_RX 0x01 // 接收数据就绪
#define UART_LSR_TX 0x20 // 发送缓冲区空

struct Uart {
    u8 data[8]; // 模拟 UART 寄存器
    get_char_func_t get_char; // 外部提供的输入函数
    put_char_func_t put_char; // 外部提供的输出函数
};

void uart_init(struct Uart *uart, get_char_func_t get, put_char_func_t put);

struct DeviceFunc uart_get_func(struct Uart *uart);

#endif

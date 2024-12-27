#include "device/uart.h"
#include "core/rvdecode.h"
#include "device/device.h"

void uart_init(struct Uart *uart, get_char_func_t get, put_char_func_t put) {
    uart->get_char = get;
    uart->put_char = put;
    uart->data[UART_LSR] = UART_LSR_TX; // 初始化线路状态寄存器，发送缓冲区为空
}

static enum exception uart_read(void *context, u64 address, u8 size, u64 *value) {
    struct Uart *uart = (struct Uart *)context;
    if (size != 1)
        return LOAD_ACCESS_FAULT; // 只支持 1 字节读取

    switch (address) {
    case UART_RHR:
        // 读取接收保持寄存器
        if (uart->data[UART_LSR] & UART_LSR_RX) {
            *value = uart->data[UART_RHR];
            uart->data[UART_LSR] &= ~UART_LSR_RX; // 清除接收数据就绪标志
        } else {
            *value = 0; // 没有数据可读
        }
        break;
    default:
        *value = uart->data[address]; // 读取其他寄存器
    }
    return EXC_NONE;
}

static enum exception uart_write(void *context, u64 address, u8 size, u64 value) {
    struct Uart *uart = (struct Uart *)context;
    if (size != 1)
        return STORE_AMO_ACCESS_FAULT; // 只支持 1 字节写入

    switch (address) {
    case UART_THR:
        // 写入发送保持寄存器
        if (uart->put_char) {
            uart->put_char((u8)value); // 调用外部输出函数
        }
        break;
    default:
        uart->data[address] = (u8)value; // 写入其他寄存器
    }
    return EXC_NONE;
}

static void uart_update(void *context, u32 interval) {
    struct Uart *uart = (struct Uart *)context;
    // 检查是否有输入数据
    u8 data;
    if (uart->get_char && uart->get_char(&data)) {
        uart->data[UART_RHR] = data;         // 将数据写入接收保持寄存器
        uart->data[UART_LSR] |= UART_LSR_RX; // 设置接收数据就绪标志
    }
}

static bool uart_check_external_interrupt(void *context) {
    struct Uart *uart = (struct Uart *)context;
    return uart->data[UART_LSR] & UART_LSR_RX; // 接收到数据就中断
}

struct DeviceFunc uart_get_func(struct Uart *uart) {
    return (struct DeviceFunc){
        .context = uart,
        .read = uart_read,
        .write = uart_write,
        .update = uart_update,
        .check_external_interrupt = uart_check_external_interrupt,
        .check_timer_interrupt = NULL,
    };
}

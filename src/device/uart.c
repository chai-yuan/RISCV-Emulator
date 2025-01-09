#include "device/uart.h"
#include "device/device.h"
#include "types.h"

void uart_init(struct Uart *uart, get_char_func_t get, put_char_func_t put) {
    uart->get_char       = get;
    uart->put_char       = put;
    uart->data[UART_LSR] = UART_LSR_TX; // 初始化线路状态寄存器，发送缓冲区为空
}

static enum exception uart_read(void *context, u64 addr, u8 size, usize *data) { return EXC_NONE; }

static enum exception uart_write(void *context, u64 addr, u8 size, usize data) { return EXC_NONE; }

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
        .context                  = uart,
        .read                     = uart_read,
        .write                    = uart_write,
        .update                   = uart_update,
        .check_external_interrupt = uart_check_external_interrupt,
        .check_timer_interrupt    = NULL,
    };
}

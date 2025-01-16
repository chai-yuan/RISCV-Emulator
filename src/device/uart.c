#include "device/uart.h"
#include "device/device.h"
#include "types.h"

void uart_init(struct Uart *uart, get_char_func_t get, put_char_func_t put) {
    uart->get_char = get;
    uart->put_char = put;
    uart->data[UART_LSR] |= (UART_LSR_TX_EMPTY | UART_LSR_THR_SR_EMPTY);
}

static enum exception uart_read(void *context, u64 addr, u8 size, usize *data) {
    struct Uart *uart = (struct Uart *)context;
    if (size != 1)
        return LOAD_ACCESS_FAULT;

    device_read(uart->data, addr, size, data);
    return EXC_NONE;
}

static enum exception uart_write(void *context, u64 addr, u8 size, usize data) {
    struct Uart *uart = (struct Uart *)context;
    if (size != 1)
        return LOAD_ACCESS_FAULT;

    if (addr == 0)
        uart->put_char(data);

    return EXC_NONE;
}

static void uart_update(void *context, u32 interval) {}

static bool uart_check_external_interrupt(void *context) { return false; }

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

#include "device/uart16550.h"
#include "device/device.h"
#include "types.h"

void uart_init(struct UART16550 *uart, get_char_func_t get, put_char_func_t put) {
    uart->get_char       = get;
    uart->put_char       = put;
    uart->data[UART_LCR] = 0x3;
    uart->data[UART_LSR] = UART_LSR_TX_EMPTY | UART_LSR_THR_SR_EMPTY;
    uart->data[UART_MSR] = (1 << 7) | (1 << 4);
}

bool uart_check_irq(struct UART16550 *uart) { return !(uart->data[UART_IIR] & 1); }

static enum exception uart_read(void *context, u64 addr, u8 size, usize *data) {
    struct UART16550 *uart = (struct UART16550 *)context;
    if (size != 1 || addr > UART_SIZE)
        return LOAD_ACCESS_FAULT;

    switch (addr) {
    case UART_RHR:
        if (uart->data[UART_LCR] & UART_LCR_DIVREG) { // 除数寄存器
            *data = uart->divregs[0];
        } else {
            *data = uart->data[UART_RHR];
            uart->data[UART_LSR] &= ~UART_LSR_DATA_READY;
            uart->data[UART_RHR] = 0;
        }
        break;
    case UART_IER:
        if (uart->data[UART_LCR] & UART_LCR_DIVREG) { // 除数寄存器
            *data = uart->divregs[1];
        } else {
            *data = uart->data[UART_IER];
        }
        break;
    default:
        *data = uart->data[addr];
        break;
    }

    return EXC_NONE;
}

static enum exception uart_write(void *context, u64 addr, u8 size, usize data) {
    struct UART16550 *uart = (struct UART16550 *)context;
    if (size != 1 || addr > UART_SIZE)
        return STORE_AMO_ACCESS_FAULT;

    switch (addr) {
    case UART_THR:
        if (uart->data[UART_LCR] & UART_LCR_DIVREG) { // 除数寄存器
            uart->divregs[0] = data;
        } else {
            if (uart->put_char)
                uart->put_char((u8)data);
            uart->data[UART_LSR] |= UART_LSR_TX_EMPTY;
        }
        break;
    case UART_IER:
        if (uart->data[UART_LCR] & UART_LCR_DIVREG) { // 除数寄存器
            uart->divregs[1] = data;
        } else {
            uart->data[UART_IER] = data;
        }
        break;
    case UART_FCR:
        if (data & UART_FCR_CLEAR) { // 清空读入数据
            uart->data[UART_LSR] &= ~UART_LSR_DATA_READY;
            uart->data[UART_RHR] = 0;
        }
        break;

        // 不允许写入
    case UART_LSR:
    case UART_MSR:
        return STORE_AMO_ACCESS_FAULT;
    default:
        uart->data[addr] = (u8)data;
        break;
    }

    return EXC_NONE;
}

static void uart_update(void *context, u32 interval) {
    struct UART16550 *uart = (struct UART16550 *)context;
    // 更新中断
    uart->data[UART_IIR] = (0x3 << 6);
    u8 no_int            = 1;
    if ((uart->data[UART_IER] & UART_IER_RDA) && (uart->data[UART_LSR] & UART_LSR_DATA_READY))
        uart->data[UART_IIR] |= (1 << 2), no_int = 0;
    if ((uart->data[UART_IER] & UART_IER_THRE) && (uart->data[UART_LSR] & UART_LSR_THR_SR_EMPTY))
        uart->data[UART_IIR] |= (1 << 1), no_int = 0;
    uart->data[UART_IIR] |= no_int;

    // 检查输入
    uart->last_update += interval;
    if (!uart->get_char || uart->last_update < 20000)
        return;
    uart->last_update = 0;

    u8 input_char;
    if (uart->get_char(&input_char)) {
        if (!(uart->data[UART_LSR] & UART_LSR_DATA_READY)) {
            uart->data[UART_RHR] = input_char;
            uart->data[UART_LSR] |= UART_LSR_DATA_READY;
        }
    }
}

struct DeviceFunc uart_get_func(struct UART16550 *uart) {
    return (struct DeviceFunc){
        .context = uart,
        .read    = uart_read,
        .write   = uart_write,
        .update  = uart_update,
    };
}

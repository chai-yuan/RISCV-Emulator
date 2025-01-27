#include "machine/qemu.h"
#include <emscripten.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// 终端缓冲区交互
// 定义缓冲区大小
#define BUFFER_SIZE 128

// 定义环形缓冲区
char     terminal_input_buff[BUFFER_SIZE];
char     terminal_output_buff[BUFFER_SIZE];
uint32_t terminal_input_buff_begin = 0, terminal_input_buff_end = 0;
uint32_t terminal_output_buff_begin = 0, terminal_output_buff_end = 0;

// 向输入缓冲区写入一个字符
EMSCRIPTEN_KEEPALIVE
void input_char(char c) {
    if ((terminal_input_buff_end + 1) % BUFFER_SIZE == terminal_input_buff_begin) {
        return; // 缓冲区已满，丢弃字符
    }
    printf("input get char : %c\n", c);
    terminal_input_buff[terminal_input_buff_end] = c;
    terminal_input_buff_end                      = (terminal_input_buff_end + 1) % BUFFER_SIZE;
}

// 检查输出缓冲区是否有数据
EMSCRIPTEN_KEEPALIVE
int check_output_size() {
    if (terminal_output_buff_begin == terminal_output_buff_end) {
        return 0; // 缓冲区为空
    }
    return 1; // 缓冲区有数据
}

// 从输出缓冲区读取一个字符
EMSCRIPTEN_KEEPALIVE
char output_char() {
    if (terminal_output_buff_begin == terminal_output_buff_end) {
        return '\0'; // 缓冲区为空，返回空字符
    }
    char c                     = terminal_output_buff[terminal_output_buff_begin];
    terminal_output_buff_begin = (terminal_output_buff_begin + 1) % BUFFER_SIZE;
    return c;
}

// 机器定义
#define memory_size 12 * 1024 * 1024
u8                 memory[memory_size];
struct QemuMachine machine;

void put_char(u8 c) {
    terminal_output_buff[terminal_output_buff_end] = c;
    terminal_output_buff_end                       = (terminal_output_buff_end + 1) % BUFFER_SIZE;
    printf("%c", c);
}

u8 get_char(u8 *data) {
    if (terminal_input_buff_begin == terminal_input_buff_end) {
        return 0;
    }
    *data                     = terminal_input_buff[terminal_input_buff_begin];
    terminal_input_buff_begin = (terminal_input_buff_begin + 1) % BUFFER_SIZE;
    printf("get data : %c\n", *data);
    return 1;
}

EMSCRIPTEN_KEEPALIVE
void machine_init(uint8_t *addr, uint32_t size) {
    printf("start! : addr %p size %d\n", addr, size);
    memcpy(memory, addr, size);
    printf("copy end\n");
    // 初始化机器
    qemu_machine_init(&machine, (struct QemuPortableOperations){
                                    .sram_data = memory,
                                    .sram_size = memory_size,
                                    .get_char  = get_char,
                                    .put_char  = put_char,
                                });
}

EMSCRIPTEN_KEEPALIVE
void machine_step() {
    struct DeviceFunc bus = bus_device_get_func(&machine.bus);
    for (int i = 0; i < 4096; i++) {
        riscvcore_step(&machine.core);
        bus.update(bus.context, 1);
    }
}

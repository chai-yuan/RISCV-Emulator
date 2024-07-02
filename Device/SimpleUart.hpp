#ifndef SIMPLE_UART_H
#define SIMPLE_UART_H

#include "AbstractDevice.hpp"
#include "MMIO.hpp"
#include <cstdint>
#include <iostream>

class SimpleUart : public AbstractDevice {
  public:
    // 构造函数
    SimpleUart() {}

    // 析构函数
    ~SimpleUart() {}

    // 实现读操作
    MMIOStatus read(uint64_t addr, uint8_t size, uint64_t *data) override {
        *data = 0;
        if (addr < 0x10) {
            return MMIOStatus::Success;
        } else {
            return MMIOStatus::Failure;
        }
    }

    // 实现写操作
    MMIOStatus write(uint64_t addr, uint8_t size, uint64_t data) override {
        if (addr == 0x0) {
            printf("%c", (uint8_t)data);
            fflush(stdout);
            return MMIOStatus::Success;
        } else {
            return MMIOStatus::Failure;
        }
    }

    // 检查是否发生中断
    bool checkInterrupt() const override { return false; }

    // 手动更新设备状态
    void update() override {
        // 无需更新
    }
};

#endif // SIMPLE_UART_H

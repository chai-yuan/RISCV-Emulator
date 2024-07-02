#ifndef CLINT_H
#define CLINT_H

#include "AbstractDevice.hpp"
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

class Clint : public AbstractDevice {
public:
    // 构造函数
    Clint() : timer(0), timermatch(0) {}

    // 析构函数
    ~Clint() {}

    // 实现读操作
    MMIOStatus read(uint64_t addr, uint8_t size, uint64_t *data) override {
        uint64_t a = addr & ~0x7;
        uint64_t offset = (addr & 0x7) * 8;
        uint64_t mask = (1ULL << (size * 8)) - 1;

        if (a == Addresses::TIMER) {
            *data = (timer >> offset) & mask;
        } else if (a == Addresses::TIMERMATCH) {
            *data = (timermatch >> offset) & mask;
        } else {
            return MMIOStatus::Failure; // 地址不匹配
        }
        return MMIOStatus::Success;
    }

    // 实现写操作
    MMIOStatus write(uint64_t addr, uint8_t size, uint64_t data) override {
        uint64_t a = addr & ~0x7;
        uint64_t offset = (addr & 0x7) * 8;
        uint64_t mask = (1ULL << (size * 8)) - 1;

        if (a == Addresses::TIMERMATCH) {
            timermatch &= ~(mask << offset);       // 清除要写入的位
            timermatch |= (data & mask) << offset; // 写入数据
            return MMIOStatus::Success;
        } else {
            return MMIOStatus::Failure;
        }
    }

    // 检查是否发生中断
    bool checkInterrupt() const override {
        return timermatch != 0 && timermatch < timer;
    }

    // 手动更新设备状态
    void update() override {
        timer++;
    }

private:
    uint64_t timer;
    uint64_t timermatch;

    enum Addresses {
        TIMER = 0xbff8,
        TIMERMATCH = 0x4000
    };
};

#endif // CLINT_H

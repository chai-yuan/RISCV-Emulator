#ifndef EXAMPLEDEVICE_H
#define EXAMPLEDEVICE_H

#include "AbstractDevice.hpp"

class ExampleDevice : public AbstractDevice {
  public:
    // 实现读操作
    MMIOStatus read(uint64_t addr, uint8_t size, uint64_t *data) override {
        // 具体的读操作实现
        return MMIOStatus::Success;
    }

    // 实现写操作
    MMIOStatus write(uint64_t addr, uint8_t size, uint64_t data) override {
        // 具体的写操作实现
        return MMIOStatus::Success;
    }

    // 检查是否发生中断
    bool checkInterrupt() const override {
        // 具体的检查中断实现
        return false;
    }

    // 手动更新设备状态
    void update() override {
        // 具体的手动更新实现
    }
};

#endif // EXAMPLEDEVICE_H

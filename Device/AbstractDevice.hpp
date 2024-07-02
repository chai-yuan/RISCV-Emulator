#ifndef ABSTRACTDEVICE_H
#define ABSTRACTDEVICE_H

#include "MMIO.hpp"

class AbstractDevice : public MMIO {
public:
    // 检查是否发生中断
    virtual bool checkInterrupt() const = 0;

    // 手动更新设备状态
    virtual void update() = 0;
};

#endif // ABSTRACTDEVICE_H

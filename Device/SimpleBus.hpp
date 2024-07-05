#ifndef SIMPLEBUS_H
#define SIMPLEBUS_H

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

#include "MMIO.hpp"

class SimpleBus : public MMIO {
  public:
    // 添加一个 MMIO 设备
    void addDevice(uint64_t start_address, MMIO *device) { devices[start_address] = device; }

    // 实现读操作
    MMIOStatus read(uint64_t addr, uint8_t size, uint64_t *data) override {
        auto it = find_device(addr);
        if (it != devices.end()) {
            return it->second->read(addr - it->first, size, data);
        }
        return MMIOStatus::Failure;
    }

    // 实现写操作
    MMIOStatus write(uint64_t addr, uint8_t size, uint64_t data) override {
        auto it = find_device(addr);
        if (it != devices.end()) {
            return it->second->write(addr - it->first, size, data);
        }
        return MMIOStatus::Failure;
    }

  private:
    std::map<uint64_t, MMIO *> devices;

    // 查找对应的设备
    std::map<uint64_t, MMIO *>::iterator find_device(uint64_t addr) {
        for (auto it = devices.begin(); it != devices.end(); it++) {
            if (it->first <= addr) {
                return it;
            }
        }
        return devices.end();
    }
};

#endif // SIMPLEBUS_H

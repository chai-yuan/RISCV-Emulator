#ifndef MEMORYDEVICE_H
#define MEMORYDEVICE_H

#include "AbstractDevice.hpp"
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <vector>

#include "../Common/DebugUtils.hpp"

class MemoryDevice : public AbstractDevice {
public:
    MemoryDevice(size_t size) : size(size) {
        data = new uint8_t[size]();
    }

    MemoryDevice(size_t size, const std::vector<uint8_t>& initData) : size(size) {
        if (initData.size() > size) {
            ERROR("Initialization data size exceeds memory size");
        }
        data = new uint8_t[size]();
        std::copy(initData.begin(), initData.end(), data);
    }

    ~MemoryDevice() {
        delete[] data;
    }

    MMIOStatus read(uint64_t addr, uint8_t size, uint64_t *data) override {
        if (addr + size > this->size) {
            return MMIOStatus::Failure;
        }

        switch (size) {
            case 1:
                *data = *((uint8_t *)(this->data + addr));
                break;
            case 2:
                *data = *((uint16_t *)(this->data + addr));
                break;
            case 4:
                *data = *((uint32_t *)(this->data + addr));
                break;
            case 8:
                *data = *((uint64_t *)(this->data + addr));
                break;
            default:
                return MMIOStatus::Failure;
        }
        return MMIOStatus::Success;
    }

    MMIOStatus write(uint64_t addr, uint8_t size, uint64_t data) override {
        if (addr + size > this->size) {
            return MMIOStatus::Failure;
        }

        switch (size) {
            case 1:
                *((uint8_t *)(this->data + addr)) = (uint8_t)data;
                break;
            case 2:
                *((uint16_t *)(this->data + addr)) = (uint16_t)data;
                break;
            case 4:
                *((uint32_t *)(this->data + addr)) = (uint32_t)data;
                break;
            case 8:
                *((uint64_t *)(this->data + addr)) = (uint64_t)data;
                break;
            default:
                return MMIOStatus::Failure;
        }
        return MMIOStatus::Success;
    }

    bool checkInterrupt() const override {
        // 内存设备不产生中断
        return false;
    }

    void update() override {
        // 内存设备不需要手动更新
    }

private:
    uint8_t *data;
    size_t size;
};

#endif // MEMORYDEVICE_H

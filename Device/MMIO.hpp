#ifndef MMIO_H
#define MMIO_H

#include <cstdint>

// 枚举类型，表示访问是否成功
enum class MMIOStatus {
    Success,
    Failure
};

class MMIO {
public:
    virtual ~MMIO() = default;

    // 读操作
    virtual MMIOStatus read(uint64_t addr, uint8_t size, uint64_t *data) = 0;

    // 写操作
    virtual MMIOStatus write(uint64_t addr, uint8_t size, uint64_t data) = 0;
};

#endif // MMIO_H

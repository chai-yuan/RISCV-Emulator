#ifndef RISCV_CORE_H
#define RISCV_CORE_H

#include "RiscvDecode.hpp"
#include <cstdint>
#include <iostream>

class RiscvCore {
  public:
    virtual ~RiscvCore() = default;

    virtual RiscvDecode step() = 0;
    virtual bool read(uint64_t addr, uint8_t size, uint64_t *data) = 0;
    virtual bool write(uint64_t addr, uint8_t size, uint64_t data) = 0;
    virtual bool checkInterrupt() = 0;
};

#endif // RISCVCORE_H

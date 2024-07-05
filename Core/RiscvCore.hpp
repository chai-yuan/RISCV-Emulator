#ifndef RISCV_CORE_H
#define RISCV_CORE_H

#include "../Common/DebugUtils.hpp"
#include "RiscvDecode.hpp"
#include "RiscvState.hpp"
#include <cstdint>
#include <iostream>

class RiscvCore {
  public:
    RiscvState state; // 处理器的持久状态
    RiscvDecode dec;  // 处理器单次执行时产生的状态

    virtual ~RiscvCore() = default;
    virtual void exec() = 0;
    virtual bool read(uint64_t addr, uint8_t size, uint64_t *data) = 0;
    virtual bool write(uint64_t addr, uint8_t size, uint64_t data) = 0;
    virtual bool checkInterrupt() { return false; }

    RiscvDecode step() {
        dec = RiscvDecode(); // 重置当前译码状态

        mmuFetch();
        exec();

        if (dec.except != RiscvDecode::ExceptType::ExceptNone) {
            if (dec.except == RiscvDecode::IllegalInstruction) {
                ERROR("unkonw inst :", dec.instruction);
            }
            ERROR("unkown execpt!");
        } else {
            state.pc = dec.next_pc;
        }

        return dec;
    }

  protected:
    // mmu函数
    void mmuFetch() {
        dec.inst = mmuRead(state.pc, 4);
        dec.decode();
        dec.next_pc = state.pc + 4;
    }
    uint64_t mmuRead(uint64_t addr, uint8_t size) {
        uint64_t readData = 0;
        if (read(addr, size, &readData)) {
            dec.accessAddr = addr;
            switch (size) {
            case 1:
                return readData & 0xff;
            case 2:
                return readData & 0xffff;
            case 4:
                return readData & 0xffffffff;
            case 8:
                return readData;
            default:
                dec.error = true;
            }
        } else {
            dec.error = true;
        }

        return 0;
    }
    void mmuWrite(uint64_t addr, uint8_t size, uint64_t data) {
        if (write(addr, size, data)) {
        } else {
            dec.error = true;
        }
    }
};

#endif // RISCVCORE_H

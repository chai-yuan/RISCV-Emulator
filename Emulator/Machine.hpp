#include "../Common/DebugUtils.hpp"
#include "../Common/FileUtils.hpp"
#include "../Core/Riscv32Core.hpp"
#include "../Core/Riscv64Core.hpp"
#include "../Device/MMIO.hpp"
#include "../Device/MemoryDevice.hpp"
#include "../Device/SimpleBus.hpp"
#include "../GdbServer/GdbServer.hpp"
#include <cstdint>
#include <set>
#include <stdexcept>

class Machine : public Riscv64Core, public GdbServer {
  public:
    Machine(std::string initFileName) {
        auto initFile = FileUtils::readFileToBinaryVector(initFileName);
        bus.addDevice(0x80000000, new MemoryDevice(0x8000000, initFile));
    }

    void run(uint64_t num) {
        while (num--) {
            auto dec = step();

            if (dec.instruction == RiscvDecode::Instruction::inst_ebreak) {
                DEBUG("Machine halt");
                if (state.regs[10] == 0) {
                    PASS("pass");
                } else {
                    ERROR("unpass");
                }
                return;
            }
            if (dec.error) {
                ERROR("Machine error");
                return;
            }
        }
    }

    void continueExecution() override {
        while (breakpoint.count(state.pc) == 0) {
            run(1);
        }
    }

    void readRegister(int regNo, uint8_t offset, uint8_t *value) override {
        if (regNo < 32) {
            *value = state.regs[regNo] >> (offset * 8);
        } else if (regNo == 32) {
            *value = state.pc >> (offset * 8);
        } else if (regNo < 4096) {
            *value = state.csrRead(regNo) >> (offset * 8);
        } else {
            ERROR("未知寄存器地址");
        }
    }

    bool readMemory(uint32_t address, uint8_t *value) override {
        if (address >= 0x80000000) {
            uint64_t v;
            read(address, 1, &v);
            *value = (uint8_t)(v & 0xff);
        } else {
            return false;
        }
        return true;
    }

    void setBreakpoint(uint32_t address) override { breakpoint.insert(address); }
    void deleteBreakpoint(uint32_t address) override { breakpoint.erase(address); }

    bool read(uint64_t addr, uint8_t size, uint64_t *data) override {
        auto ret = bus.read(addr, size, data);
        if (ret == MMIOStatus::Failure) {
            ERROR("总线访问出错");
        }
        return true;
    }
    bool write(uint64_t addr, uint8_t size, uint64_t data) override {
        auto ret = bus.write(addr, size, data);
        if (ret == MMIOStatus::Failure) {
            ERROR("总线访问出错");
        }
        return true;
    }

  private:
    std::set<uint32_t> breakpoint;
    SimpleBus bus;
};

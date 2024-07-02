#include "../Common/DebugUtils.hpp"
#include "../Common/FileUtils.hpp"
#include "../Core/Riscv32Core.hpp"
#include "../Device/MMIO.hpp"
#include "../Device/MemoryDevice.hpp"
#include "../Device/SimpleBus.hpp"
#include "../GdbServer/GdbServer.hpp"
#include <cstdint>
#include <stdexcept>

class Machine : public Riscv32Core, public GdbServer {
  public:
    Machine(std::string initFileName) {
        auto initFile = FileUtils::readFileToBinaryVector(initFileName);
        bus.addDevice(0x80000000, new MemoryDevice(0x100000, initFile));

        gdbInit(10086);
    }

    void run(uint64_t num) {
        while (num--) {
            auto dec = step();

            if (dec.instruction == RiscvDecode::Instruction::inst_ebreak) {
                DEBUG("Machine halt");
                if (getState()->regs[10] == 0) {
                    PASS("pass");
                } else {
                    ERROR("unpass");
                }
                return;
            }
            if (dec.error) {
                ERROR("Machine error");
                getState()->printState();
                return;
            }
        }
    }

    void continueExecution() override {
        while (getState()->pc != breakpoint) {
            run(1);
        }
    }

    void readRegister(int regNo, uint32_t *value) override {
        if (regNo < 32) {
            *value = getState()->regs[regNo];
        } else {
            *value = getState()->pc;
        }
    }

    void readMemory(uint32_t address, uint8_t *value) override {
        if (address >= 0x80000000) {
            uint64_t v;
            read(address, 1, &v);
            *value = (uint8_t)(v & 0xff);
        } else {
            *value = 0;
        }
    }
    void setBreakpoint(uint32_t address) override {
        DEBUG("break set:", address);
        breakpoint = address;
    }
    void deleteBreakpoint(uint32_t address) override { breakpoint = 0; }

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
    uint32_t breakpoint = 0;
    SimpleBus bus;
};

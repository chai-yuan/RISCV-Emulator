#ifndef MMU_H
#define MMU_H

#include "cpu/decode.h"
#include "cpu/riscv32.h"
#include "cpu/riscv64.h"
#include <bus/bus.h>
#include <stdint.h>

void mmu_fetch_32(Riscv32core *core, RiscvDecode *dec);

void mmu_read_32(Riscv32core *core, RiscvDecode *dec, uint32_t addr,
                 uint8_t size, uint32_t *data);

void mmu_write_32(Riscv32core *core, RiscvDecode *dec, uint32_t addr,
                  uint8_t size, uint32_t data);

void mmu_fetch_64(Riscv64core *core, RiscvDecode *dec);

void mmu_read_64(Riscv64core *core, RiscvDecode *dec, uint64_t addr,
                 uint8_t size, uint64_t *data);

void mmu_write_64(Riscv64core *core, RiscvDecode *dec, uint64_t addr,
                  uint8_t size, uint64_t data);

#endif // !MMU_H

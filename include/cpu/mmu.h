#ifndef MMU_H
#define MMU_H

#include "cpu/decode.h"
#include "cpu/riscv32.h"
#include <bus/bus.h>
#include <stdint.h>

void mmu_fetch_32(Riscv32core *core, RiscvDecode *dec);

void mmu_read_32(Riscv32core *core, RiscvDecode *dec, uint32_t addr,
                 uint8_t size, uint32_t *data);

void mmu_write_32(Riscv32core *core, RiscvDecode *dec, uint32_t addr,
                  uint8_t size, uint32_t data);

#endif // !MMU_H

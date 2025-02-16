#ifndef CONFIG_H
#define CONFIG_H

#define ARCH_RV32 32
#define ARCH_RV64 64

#define ENABLE_DEBUG_MACROS 1
#define DEBUG_LEVEL DEBUG_LEVEL_INFO

#define CURRENT_ARCH ARCH_RV64

#ifndef CURRENT_ARCH
#define CURRENT_ARCH ARCH_RV64
#elif CURRENT_ARCH != ARCH_RV32 && CURRENT_ARCH != ARCH_RV64
#error "Unsupported architecture."
#endif

#endif

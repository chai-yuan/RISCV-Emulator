#ifndef EXCEPT_H
#define EXCEPT_H

// 定义 RISC-V 特权级枚举类型
typedef enum { USER = 0, SUPERVISOR = 1, MACHINE = 3 } PrivilegeLevel;

typedef enum {
    EXC_InstructionAddressMisaligned = 0,
    EXC_InstructionAccessFault = 1,
    EXC_IllegalInstruction = 2,
    EXC_Breakpoint = 3,
    EXC_LoadAddressMisaligned = 4,
    EXC_LoadAccessFault = 5,
    EXC_StoreAMOAddressMisaligned = 6,
    EXC_StoreAMOAccessFault = 7,
    EXC_EcallFromUMode = 8,
    EXC_EcallFromSMode = 9,
    EXC_EcallFromMMode = 11,
    EXC_InstructionPageFault = 12,
    EXC_LoadPageFault = 13,
    EXC_StoreAMOPageFault = 15,
    EXC_None = 64,
} ExceptType;

typedef enum {
    INTR_SupervisorSoftwareInterrupt = 0,
    INTR_MachineSoftwareInterrupt = 2,
    INTR_SupervisorTimerInterrupt = 4,
    INTR_MachineTimerInterrupt = 7,
    INTR_SupervisorExternalInterrupt = 8,
    INTR_MachineExternalInterrupt = 11,
    INTR_None = 16,
} IntrType;

#endif // !EXCEPT_H

#include "device/plic.h"
#include "core/riscv.h"

void plic_init(struct PLIC *plic) {
    for (int i = 0; i < sizeof(struct PLIC); i++)
        *((u8 *)plic + i) = 0;
    plic->pending = 0;
    plic->sclaim  = 0;
}

bool plic_check_irq(struct PLIC *plic, u32 context_id) {
    if (plic->pending != 0) {
        plic->pending = 0;
        return true;
    }
    return false;
}

void plic_update_intterupt(struct PLIC *plic, bool interrupt, u32 interrupt_num) {
    if (interrupt) {
        plic->pending |= 1u << (interrupt_num % 32);
        plic->mclaim = plic->sclaim = interrupt_num;
    }
}

static enum exception plic_read(void *context, u64 addr, u8 size, usize *data) {
    struct PLIC *plic = (struct PLIC *)context;
    if (size != 4 || addr + size > PLIC_SIZE)
        return LOAD_ACCESS_FAULT;

    switch (addr) {
    case PLIC_PENDING:
        *data = plic->pending;
        break;
    case PLIC_MENABLE:
        *data = plic->menable;
        break;
    case PLIC_MPRIORITY:
        *data = plic->mpriority;
        break;
    case PLIC_MCLAIM:
        *data = plic->mclaim;
        break;
    case PLIC_SPRIORITY:
        *data = plic->spriority;
        break;
    case PLIC_SCLAIM:
        *data = plic->sclaim;
        break;
    default:
        *data = 0;
    }

    return EXC_NONE;
}

static enum exception plic_write(void *context, u64 addr, u8 size, usize data) {
    struct PLIC *plic = (struct PLIC *)context;
    if (size != 4 || addr + size > PLIC_SIZE)
        return STORE_AMO_ACCESS_FAULT;

    switch (addr) {
    case PLIC_PENDING:
        plic->pending = data;
        break;
    case PLIC_MENABLE:
        plic->menable = data;
        break;
    case PLIC_MPRIORITY:
        plic->mpriority = data;
        break;
    case PLIC_MCLAIM:
        plic->mclaim = data; // TODO
        break;
    default:
        break;
    }

    return EXC_NONE;
}

struct DeviceFunc plic_get_func(struct PLIC *plic) {
    return (struct DeviceFunc){
        .context = plic,
        .read    = plic_read,
        .write   = plic_write,
        .update  = NULL,
    };
}

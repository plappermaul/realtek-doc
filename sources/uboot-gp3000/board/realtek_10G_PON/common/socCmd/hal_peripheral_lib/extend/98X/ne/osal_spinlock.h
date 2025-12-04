#ifndef _OSAL_SPINLOCK_H_
#define _OSAL_SPINLOCK_H_
#include "ca_types.h"

typedef enum
{
    SPINLOCK_CMN = 0,  /*spin_lock*/
    SPINLOCK_BH  = 1,  /*spin_lock_bh*/
    SPINLOCK_IRQ = 2,  /*spin_lock_irq*/
    SPINLOCK_SMP = 3,  /*spin_lock_irqsave*/
} CA_SPINLOCK_LEVEL;

ca_int32 ca_spin_lock_init(ca_uint *spinlock_id , CA_SPINLOCK_LEVEL level);
ca_int32 ca_spin_lock(ca_uint spinlock_id);
ca_int32 ca_spin_unlock(ca_uint spinlock_id);
ca_int32 ca_spin_lock_destroy(ca_uint spinlock_id);

#endif


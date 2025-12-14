#include "ca_types.h"
#include "os_core.h"
#include "osal_cmn.h"
#include "osal_spinlock.h"

ca_int32 ca_spin_lock_init(ca_uint *spinlock_id , CA_SPINLOCK_LEVEL level)
{
    *spinlock_id = (ca_uint)OS_SPINLOCK_CREATE(level);
    if(*spinlock_id == 0)
        return CA_OSAL_ERROR;

    return CA_OSAL_OK;
}

ca_int32 ca_spin_lock(ca_uint spinlock_id)
{
    OS_SPINLOCK_LOCK(spinlock_id);
    return CA_OSAL_OK;
}

ca_int32 ca_spin_unlock(ca_uint spinlock_id)
{
    OS_SPINLOCK_UNLOCK(spinlock_id);
    return CA_OSAL_OK;
}

ca_int32 ca_spin_lock_destroy(ca_uint spinlock_id)
{
    OS_SPINLOCK_DESTROY(spinlock_id);
    return CA_OSAL_OK;
}



/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : interrupt lock/unlock API
 *
 */

#ifndef __OSAL_SPL_H__
#define __OSAL_SPL_H__


/*
 * Include Files
 */
#include <common/type.h>

/*
 * Symbol Definition
 */
typedef uintptr osal_spinlock_t;

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      osal_spl_spin_lock_create
 * Description:
 *      create spin lock.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      NULL   - fail to create spinlock.
 *      others - osal_spinlock_t structure
 * Note:
 *      None
 */
extern osal_spinlock_t
osal_spl_spin_lock_create(void);

/* Function Name:
 *      osal_spl_spin_lock_destroy
 * Description:
 *      Destroy spin lock.
 * Input:
 *      lock - spin lock structure.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
osal_spl_spin_lock_destroy(osal_spinlock_t lock);

/* Function Name:
 *      osal_spl_spin_lock
 * Description:
 *      spin lock function.
 * Input:
 *      lock  - spin lock
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - lock success.
 *      RT_ERR_NULL_POINTER - input spinlock is invalid (NULL)
 * Note:
 *      None
 */
extern int32
osal_spl_spin_lock(osal_spinlock_t lock);

/* Function Name:
 *      osal_spl_spin_unlock
 * Description:
 *      spin unlock function.
 * Input:
 *      lock  - spin lock
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - unlock success.
 *      RT_ERR_NULL_POINTER - input spinlock is invalid (NULL)
 * Note:
 *      None
 */
extern int32
osal_spl_spin_unlock(osal_spinlock_t lock);

/* Function Name:
 *      osal_spl_spin_lock_irqsave
 * Description:
 *      spin lock and save irq function.
 * Input:
 *      lock  - spin lock
 *      flags - pointer to a variable for keep irq flag info
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - lock success.
 *      RT_ERR_NULL_POINTER - input spinlock is invalid (NULL)
 * Note:
 *      None
 */
extern int32
osal_spl_spin_lock_irqsave(osal_spinlock_t lock, unsigned long *flags);

/* Function Name:
 *      osal_spl_spin_unlock_irqrestore
 * Description:
 *      spin unlock and restore irq function.
 * Input:
 *      lock  - spin lock
 *      flags - pointer to irq flag info saved by osal_spl_spin_lock_irqsave
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - unlock success.
 *      RT_ERR_NULL_POINTER - input spinlock is invalid (NULL)
 * Note:
 *      None
 */
int32
osal_spl_spin_unlock_irqrestore(osal_spinlock_t lock, unsigned long *flags);

#endif /* __OSAL_SPL_H__ */


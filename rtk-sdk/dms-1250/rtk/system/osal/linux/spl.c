/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <common/error.h>
#include <linux/spinlock.h>
#include <osal/spl.h>
#include <osal/memory.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

typedef struct osal_linux_spinlock_s
{
    spinlock_t    spinlock;
} osal_linux_spinlock_t;
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
osal_spinlock_t
osal_spl_spin_lock_create(void)
{
    osal_linux_spinlock_t *pS;
    pS = osal_alloc(sizeof(osal_linux_spinlock_t));
    if (NULL != pS)
    {
        spin_lock_init(&pS->spinlock);
    }
    return (osal_spinlock_t)(uintptr)pS;
}

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
void
osal_spl_spin_lock_destroy(osal_spinlock_t lock)
{
    osal_linux_spinlock_t *pS;
    pS = (osal_linux_spinlock_t *)(uintptr)lock;
    osal_free(pS);
}

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
int32
osal_spl_spin_lock(osal_spinlock_t lock)
{
    osal_linux_spinlock_t *pS;
    pS = (osal_linux_spinlock_t *)(uintptr)lock;
    RT_INTERNAL_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);

    spin_lock(&pS->spinlock);
    return RT_ERR_OK;
}

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
int32
osal_spl_spin_unlock(osal_spinlock_t lock)
{
    osal_linux_spinlock_t *pS;
    pS = (osal_linux_spinlock_t *)(uintptr)lock;
    RT_INTERNAL_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);

    spin_unlock(&pS->spinlock);
    return RT_ERR_OK;
}

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
int32
osal_spl_spin_lock_irqsave(osal_spinlock_t lock, unsigned long *flags)
{
    osal_linux_spinlock_t *pS;
    unsigned long irqflags;
    pS = (osal_linux_spinlock_t *)(uintptr)lock;
    RT_INTERNAL_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);
    RT_INTERNAL_PARAM_CHK((NULL == flags), RT_ERR_NULL_POINTER);

    spin_lock_irqsave(&pS->spinlock, irqflags);
    *flags = irqflags;
    return RT_ERR_OK;
}

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
osal_spl_spin_unlock_irqrestore(osal_spinlock_t lock, unsigned long *flags)
{
    osal_linux_spinlock_t *pS;
    unsigned long irqflags;
    pS = (osal_linux_spinlock_t *)(uintptr)lock;
    RT_INTERNAL_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);
    RT_INTERNAL_PARAM_CHK((NULL == flags), RT_ERR_NULL_POINTER);

    irqflags = *flags;
    spin_unlock_irqrestore(&pS->spinlock, irqflags);
    return RT_ERR_OK;
}

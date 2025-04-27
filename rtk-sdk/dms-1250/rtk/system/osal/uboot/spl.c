/*
 * Copyright (C) 2016-2022 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *
 *
 * Purpose : For U-Boot usage
 *
 * Feature : interrupt lock/unlock API
 *
 */


/*
 * Include Files
 */
#include <common/error.h>
#include <osal/spl.h>

/*
 * Symbol Definition
 */

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
osal_spinlock_t
osal_spl_spin_lock_create(void)
{
    return 1;
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
    return;
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
    return RT_ERR_OK;
}


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
 *      osal_spl_spin_lock
 * Description:
 *      Interrupt lock function.
 * Input:
 *      pLock  - pointer buffer of interrupt lock
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - interrupt lock success.
 *      RT_ERR_FAILED - fail to interrupt lock routine.
 * Note:
 *      None
 */
int32
osal_spl_spin_lock(osal_spinlock_t *pLock)
{
    return RT_ERR_OK;
}


/* Function Name:
 *      osal_spl_spin_unlock
 * Description:
 *      Interrupt unlock function.
 * Input:
 *      pLock  - pointer buffer of interrupt unlock
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK    - interrupt unlock success.
 * Note:
 *      None
 */
int32
osal_spl_spin_unlock(osal_spinlock_t *pLock)
{
    return RT_ERR_OK;
}




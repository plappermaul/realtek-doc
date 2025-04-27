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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : Atomic relative API
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <osal/atomic.h>

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
 *      osal_atomic_inc
 * Description:
 *      Increase the atomic counter.
 * Input:
 *      None
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
osal_atomic_inc(osal_atomic_t *pAtomic)
{
    RT_INTERNAL_PARAM_CHK((NULL == pAtomic), RT_ERR_NULL_POINTER);

    /* please modify to fit your OS */
    (*pAtomic) += 1;

    return RT_ERR_OK;
} /* end of osal_atomic_inc */

/* Function Name:
 *      osal_atomic_set
 * Description:
 *      Set the atomic counter.
 * Input:
 *      counter - atomic counter to be configured
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
osal_atomic_set(osal_atomic_t *pAtomic, int32 counter)
{
    RT_INTERNAL_PARAM_CHK((NULL == pAtomic), RT_ERR_NULL_POINTER);

    /* please modify to fit your OS */
    (*pAtomic) = counter;

    return RT_ERR_OK;
} /* end of osal_atomic_set */

/* Function Name:
 *      osal_atomic_dec_return
 * Description:
 *      Atomically decrements atomic counter by 1 and returns the result.
 * Input:
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      returns the result of atomic counter
 * Note:
 *      None
 */
int32
osal_atomic_dec_return(osal_atomic_t *pAtomic)
{
    RT_INTERNAL_PARAM_CHK((NULL == pAtomic), RT_ERR_NULL_POINTER);

    /* please modify to fit your OS */
    (*pAtomic) -= 1;

    return *pAtomic;
} /* end of osal_atomic_dec_return */

/* Function Name:
 *      osal_atomic_read
 * Description:
 *      Atomically reads the atomic counter.
 * Input:
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      None
 * Return:
 *      returns the read value of atomic counter
 * Note:
 *      None
 */
int32
osal_atomic_read(osal_atomic_t *pAtomic)
{
    RT_INTERNAL_PARAM_CHK((NULL == pAtomic), RT_ERR_NULL_POINTER);

    /* please modify to fit your OS */
    return *pAtomic;

} /* end of osal_atomic_read */

int64
osal_atomic_test_and_set(osal_atomic_t *pAtomic, int32 counter)
{
    return __sync_lock_test_and_set(pAtomic, counter);
}
/*
 * Copyright (C) 2009-2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Revision:
 * Date: 2018-06-15
 *
 * Purpose : PHY OSAL Module.
 *
 * Feature : OSAL is O.S. Abstraction Layer.
 *           In the following API's implementation, the osal_xxx() are used as example,
 *           and the osal_xxx() is RTK SDK's OSAL, please replace them by customer's OSAL.
 *
 */


#include <common/rt_type.h>
#include <common/rt_error.h>
#include <osal/time.h>
#include <osal/lib.h>
#include <osal/sem.h>
#include <osal/memory.h>
#include <osal/phy_osal.h>


/* Function Name:
 *      phy_osal_memset
 * Description:
 *      fill memory with a constant byte with value of val.
 * Input:
 *      pDst  - the pointer of the destination memory area.
 *      val   - the constant byte to be set.
 *      len   - number of bytes to be set.
 * Output:
 *      None
 * Return:
 *      NULL   - pDst is NULL.
 *      Others - The pointer to the destination memory area.
 * Note:
 *      None
 */
void *
phy_osal_memset(void *pDst, int val, unsigned int len)
{
    void * p;

    p = osal_memset(pDst, val, len);

    return p;
} /* end of phy_osal_memset */

/* Function Name:
 *      phy_osal_memcpy
 * Description:
 *      copy memory area
 * Input:
 *      pDst  - A pointer to the distination memory area.
 *      pSrc  - A pointer to the source memory area.
 *      len   - Number of bytes to be copied.
 * Output:
 *      None
 * Return:
 *      NULL   - pDst or pSrc is NULL.
 *      Others - A pointer to pDst.
 * Note:
 *      None
 */
void *
phy_osal_memcpy(void *pDst, const void *pSrc, unsigned int len)
{
    void * p;

    p = osal_memcpy(pDst, pSrc, len);

    return p;
}


/* Function Name:
 *      phy_osal_alloc
 * Description:
 *      Allocate memory based on user require size.
 * Input:
 *      size - size of allocate memory
 * Output:
 *      None
 * Return:
 *      pointer of the allocated memory area.
 * Note:
 *      None
 */
void *
phy_osal_alloc(uint32 size)
{
    void * p;

    p = osal_alloc(size);

    return p;
} /* end of phy_osal_alloc */


/* Function Name:
 *      phy_osal_free
 * Description:
 *      Free the memory buffer
 * Input:
 *      pAddr - address of buffer that want to free
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_osal_free(void *pAddr)
{
    osal_free(pAddr);
} /* end of phy_osal_free */


/* Function Name:
 *      phy_osal_time_usleep
 * Description:
 *      Suspend for specified number of microseconds.
 * Input:
 *      usec - number of microseconds to sleep.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_osal_time_usleep(uint32 usec)
{
    osal_time_usleep(usec);

} /* end of phy_osal_time_usleep */

/* Function Name:
 *      phy_osal_time_mdelay
 * Description:
 *      Delay for specified number of milliseconds.
 * Input:
 *      msec - Number of milliseconds to delay.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_osal_time_mdelay(uint32 msec)
{
    osal_time_mdelay(msec);
} /* end of phy_osal_time_mdelay */

/* Function Name:
 *      phy_osal_time_udelay
 * Description:
 *      Delay for specified number of microseconds.
 * Input:
 *      usec - Number of microsecond to delay.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_osal_time_udelay(uint32 usec)
{
    osal_time_udelay(usec);
} /* end of phy_osal_time_udelay */

/* Function Name:
 *      phy_osal_strcmp
 * Description:
 *      compare pS1 string and pS2 string.
 * Input:
 *      pS1 - the pointer of the strings to be compared.
 *      pS2 - the pointer of the strings to be compared.
 * Output:
 *      None
 * Return:
 *      0 - pS1 and pS2 match.
 *      1 - pS1 and pS2 mismatch, pS1 is greater than pS2
 *     -1 - pS1 and pS2 mismatch, pS1 is less than pS2
 *     -2 - pS1 or pS2 is NULL
 * Note:
 *      None
 */
int phy_osal_strcmp(const char *pS1, const char *pS2)
{
    int ret;

    ret = osal_strcmp(pS1, pS2);

    return ret;
} /* end of phy_osal_strcmp*/


/* Function Name:
 *      phy_osal_sem_create
 * Description:
 *      Create semaphore.
 * Input:
 *      init_num - semaphore counter.
 * Output:
 *      None
 * Return:
 *      NULL   - fail to create semaphore.
 *      others - semaphore id
 * Note:
 *      None
 */
osal_sem_t
phy_osal_sem_create(int32 init_num)
{
    return (osal_sem_t)osal_sem_create(init_num);
} /* end of osal_sem_create */


/* Function Name:
 *      phy_osal_sem_destroy
 * Description:
 *      Destroy semaphore.
 * Input:
 *      sem - semaphore id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_osal_sem_destroy(osal_sem_t sem)
{
    osal_sem_destroy(sem);
} /* end of phy_osal_sem_destroy */

/* Function Name:
 *      phy_osal_sem_take
 * Description:
 *      Take the semaphore.
 * Input:
 *      sem  - semaphore id
 *      usec - 1. retry timeout (microseconds)
 *             2. OSAL_SEM_WAIT_FOREVER to wait until release
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to take the semaphore.
 *      RT_ERR_FAILED       - fail to take the semaphore.
 *      RT_ERR_NULL_POINTER - input sem is NULL.
 * Note:
 *      None
 */
int32
phy_osal_sem_take(osal_sem_t sem, uint32 usec)
{
    int32 ret;

    ret = osal_sem_take(sem, usec);

    return ret;
} /* end of phy_osal_sem_take */


/* Function Name:
 *      phy_osal_sem_give
 * Description:
 *      Give the semaphore.
 * Input:
 *      sem - semaphore id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to give the semaphore.
 *      RT_ERR_NULL_POINTER - sem is invalid (NULL)
 * Note:
 *      None
 */
int32
phy_osal_sem_give(osal_sem_t sem)
{
    int32 ret;

    ret = osal_sem_give(sem);

    return ret;
} /* end of phy_osal_sem_give */

/* Function Name:
 *      phy_osal_thread_create
 * Description:
 *      Create a thread for running the main routine
 * Input:
 *      pName      - name of thread
 *      stack_size - stack size of thread
 *      thread_pri - priority of thread
 *      f          - main routine
 *      pArg       - input argument of the main routine
 * Output:
 *      None
 * Return:
 *      thread id
 * Note:
 *      Linux - stack_size is currently using default value.
 */
phy_osal_thread_t
phy_osal_thread_create(char *pName, int stack_size, int thread_pri, void (f)(void *), void *pArg)
{

    return osal_thread_create(pName, stack_size, thread_pri, f, pArg);

} /* end of phy_osal_thread_create */


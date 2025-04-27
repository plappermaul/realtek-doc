/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <osal/sem.h>
#include <common/debug/rt_log.h>


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
 *      osal_sem_mutex_create
 * Description:
 *      Create one mutex.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Mutex id
 * Note:
 *      It is NOT a reentrant mutex.
 */
osal_mutex_t
osal_sem_mutex_create(void)
{
    return 1;
} /* end of osal_sem_mutex_create */


/* Function Name:
 *      osal_sem_mutex_destroy
 * Description:
 *      Destroy the mutex.
 * Input:
 *      mutex - mutex id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
osal_sem_mutex_destroy(osal_mutex_t mutex)
{
    return;
} /* end of osal_sem_mutex_destroy */


/* Function Name:
 *      osal_sem_mutex_take
 * Description:
 *      Take the mutex.
 * Input:
 *      mutex - mutex id
 *      usec  - 1. retry timeout (microseconds)
 *              2. OSAL_SEM_WAIT_FOREVER to wait until release
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - success to take the mutex.
 *      RT_ERR_FAILED       - fail to take the mutex.
 *      RT_ERR_NULL_POINTER - input mutex is NULL.
 * Note:
 *      None
 */
int32
osal_sem_mutex_take(osal_mutex_t mutex, uint32 usec)
{
    return RT_ERR_OK;
} /* end of osal_sem_mutex_take */


/* Function Name:
 *      osal_sem_mutex_give
 * Description:
 *      Give the mutex.
 * Input:
 *      mutex - mutex id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - succeed in giving the mutex.
 *      RT_ERR_NULL_POINTER - mutex is invalid (NULL);
 * Note:
 *      None
 */
int32
osal_sem_mutex_give(osal_mutex_t mutex)
{
    return RT_ERR_OK;
} /* end of osal_sem_mutex_give */


/* Function Name:
 *      osal_sem_create
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
osal_sem_create(int32 init_num)
{
    return 1;
} /* end of osal_sem_create */


/* Function Name:
 *      osal_sem_destroy
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
osal_sem_destroy(osal_sem_t sem)
{
    return;
} /* end of osal_sem_destroy */


/* Function Name:
 *      osal_sem_take
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
osal_sem_take(osal_sem_t sem, uint32 usec)
{

    return RT_ERR_OK;

} /* end of osal_sem_take */


/* Function Name:
 *      osal_sem_give
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
osal_sem_give(osal_sem_t sem)
{
    return RT_ERR_OK;
} /* end of osal_sem_give */


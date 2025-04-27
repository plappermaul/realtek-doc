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
 * Feature : semphore relative API
 *
 */

#ifndef __OSAL_SEM_H__
#define __OSAL_SEM_H__


/*
 * Include Files
 */
#include <common/type.h>


/*
 * Symbol Definition
 */
#define OSAL_SEM_WAIT_FOREVER   (0xFFFFFFFF)

/*
 * Data Declaration
 */
typedef uintptr osal_mutex_t;
typedef uintptr osal_sem_t;

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
extern osal_mutex_t
osal_sem_mutex_create(void);

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
extern void
osal_sem_mutex_destroy(osal_mutex_t mutex);

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
extern int32
osal_sem_mutex_take(osal_mutex_t mutex, uint32 usec);


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
extern int32
osal_sem_mutex_give(osal_mutex_t mutex);

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
extern osal_sem_t
osal_sem_create(int32 init_num);

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
extern void
osal_sem_destroy(osal_sem_t sem);


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
extern int32
osal_sem_take(osal_sem_t sem, uint32 usec);

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
extern int32
osal_sem_give(osal_sem_t sem);


#endif /* __OSAL_SEM_H__ */


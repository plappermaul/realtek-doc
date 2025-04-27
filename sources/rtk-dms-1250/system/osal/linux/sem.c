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
 * Feature : semaphore relative API
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
#include <asm/semaphore.h>
#else
#include <linux/semaphore.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
#include <linux/sched/signal.h>
#include <linux/wait.h>
#endif
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <osal/sem.h>
#include <osal/thread.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/* internal used data structure to record the information related to semaphore. */
typedef struct osal_linux_sem_s
{
    struct semaphore    sem;
    wait_queue_head_t   wq;
} osal_linux_sem_t;

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
    return (osal_mutex_t)osal_sem_create(1);
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
    osal_sem_destroy(mutex);
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
    return osal_sem_take((osal_sem_t) mutex, usec);
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
    return osal_sem_give((osal_sem_t)mutex);
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
    osal_linux_sem_t *pS;

    /* allocate memory space for the semaphore. */
    pS = osal_alloc(sizeof(osal_linux_sem_t));

    if (NULL != pS)
    {
        /* initialize pS */
        sema_init(&pS->sem, init_num);
        init_waitqueue_head(&pS->wq);
    }

    return (osal_sem_t)(uintptr)pS;
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
    osal_linux_sem_t *pS;

    pS = (osal_linux_sem_t *)(uintptr)sem;

    osal_free(pS);
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
    uint32 wait_time = 0;
    int32 e_status = RT_ERR_FAILED;
    osal_linux_sem_t *pS;

    pS = (osal_linux_sem_t *)(uintptr)sem;

    RT_INTERNAL_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);

    if (!in_interrupt())
    {
        if (OSAL_SEM_WAIT_FOREVER == usec)
        {
            /* Wait until get the semaphore. */
            e_status = down_interruptible(&pS->sem) ? RT_ERR_FAILED : RT_ERR_OK;
        }
        else
        {
            if(usec >= (1000000/HZ))
            {
                /* Retry per tick. If it fails to trylock sem,then we go to sleep. */
                wait_time = 1000000/HZ;
            }
            else
            {
                /* Busy waiting */
                wait_time = 100;
            }

            while(1)
            {
                /* Try one time without blocking. */
                if (0 == down_trylock(&pS->sem))
                {
                    e_status = RT_ERR_OK;
                    break;
                }

                if (wait_time > usec)
                {
                    wait_time = usec;
                }

                osal_time_usleep(wait_time);
                usec -= wait_time;

                if (usec == 0)
                {
                    e_status = RT_ERR_FAILED;
                    break;
                }

                if ((wait_time *= 2) > 10000)
                {
                    wait_time = 10000;
                }
            }
        }
        /* if we couldn't take the sem, give up CPU */
        if(RT_ERR_OK != e_status)
        {
            schedule();
        }

        /* Check SIGKILL signal */
        if((signal_pending(current)) && (unlikely(sigismember(&current->pending.signal, SIGKILL))))
        {
            flush_signals(current);
            osal_sem_give(sem);
            osal_thread_exit(1);
        }

    }
    else
    {
        e_status = RT_ERR_OK;
    }

    return e_status;

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
    osal_linux_sem_t *pS;

    pS = (osal_linux_sem_t *)(uintptr)sem;

    RT_INTERNAL_PARAM_CHK((NULL == pS), RT_ERR_NULL_POINTER);

    if (!in_interrupt())
    {
        up(&pS->sem);

        schedule();

        /* Check SIGKILL signal */
        if((signal_pending(current)) && (unlikely(sigismember(&current->pending.signal, SIGKILL))))
        {
            flush_signals(current);
            osal_sem_give(sem);
            osal_thread_exit(1);
        }
    }

    return RT_ERR_OK;
} /* end of osal_sem_give */


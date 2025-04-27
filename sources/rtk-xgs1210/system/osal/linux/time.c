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
 * $Revision: 76806 $
 * $Date: 2017-03-22 15:59:25 +0800 (Wed, 22 Mar 2017) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : Time relative API
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>

#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,32,58))
#include <linux/wait.h>
#endif
#include <common/debug/rt_log.h>
#include <osal/time.h>
#include <osal/thread.h>

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
 *      osal_time_usec2Ticks_get
 * Description:
 *      Return number of ticks from input value in microseconds.
 * Input:
 *      usec    - number of microseconds to transform.
 * Output:
 *      pTicks - number of ticks corresponding to the input usec.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      TICK_USEC - number of usec per tick in system.
 */
int32
osal_time_usec2Ticks_get(uint32 usec, uint32 *pTicks)
{
    RT_INTERNAL_PARAM_CHK((NULL == pTicks), RT_ERR_NULL_POINTER);

    /* Round up */
    *pTicks = (uint32)(usec/TICK_USEC) + 1;

    return RT_ERR_OK;
} /* end of osal_time_usec2Ticks_get */

/* Function Name:
 *      osal_time_usecs_get
 * Description:
 *      Return the current time in microseconds
 * Input:
 *      None
 * Output:
 *      pUsec - time in microseconds
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
osal_time_usecs_get(osal_usecs_t *pUsec)
{
    struct timeval ltv;

    RT_INTERNAL_PARAM_CHK((NULL == pUsec), RT_ERR_NULL_POINTER);

    do_gettimeofday(&ltv);
    *pUsec = (osal_usecs_t)(ltv.tv_sec * USEC_PER_SEC + ltv.tv_usec);

    return RT_ERR_OK;
} /* end of osal_time_usecs_get */

/* Function Name:
 *      osal_time_seconds_get
 * Description:
 *      Return the current time in seconds
 * Input:
 *      None
 * Output:
 *      pSec - time in seconds
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
osal_time_seconds_get(osal_time_t *pSec)
{
    struct timeval ltv;

    RT_INTERNAL_PARAM_CHK((NULL == pSec), RT_ERR_NULL_POINTER);

    do_gettimeofday(&ltv);
    *pSec = (osal_time_t)ltv.tv_sec;

    return RT_ERR_OK;
} /* end of osal_time_seconds_get */

/* Function Name:
 *      osal_time_usleep
 * Description:
 *      Suspend calling thread for specified number of microseconds.
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
osal_time_usleep(uint32 usec)
{
    osal_usecs_t u_begin;
    osal_usecs_t u_stamp;
    wait_queue_head_t wq_h;

    /* Can't sleep during the interrpt handling. */
    if (in_interrupt())
    {
        return;
    }
    /* release the cpu control. */
    else
    {
        /* if suspending time < 1 tick (10ms), busy waiting. */
        if (10000 > usec)
        {
            osal_time_usecs_get(&u_begin);
            do {
                yield();
                osal_time_usecs_get(&u_stamp);
            } while((u_stamp - u_begin) < usec);
        }
        /* if suspedning time > 1 tick (10ms), go to sleep. */
        else
        {
            init_waitqueue_head(&wq_h);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,32,58))
            interruptible_sleep_on_timeout(&wq_h, (usec/(1000000/HZ)));
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,32,58))
            wait_event_interruptible_timeout(wq_h, false, (usec/(1000000/HZ)));
#endif
        }

        /* Check SIGKILL and SIGTERM after waking up. */
        if (signal_pending(current) && (unlikely(sigismember(&current->pending.signal, SIGKILL)) || unlikely(sigismember(&current->pending.signal, SIGTERM))))
        {
            flush_signals(current);
            osal_thread_exit(1);
        }
    }
} /* end of osal_time_usleep */

/* Function Name:
 *      osal_time_sleep
 * Description:
 *      Suspend calling thread for specified number of seconds.
 * Input:
 *      sec - number of seconds to sleep.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
osal_time_sleep(uint32 sec)
{
    wait_queue_head_t wq_h;

    /* Can't sleep during the interrpt handling. */
    if (in_interrupt())
    {
        return;
    }
    init_waitqueue_head(&wq_h);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,32,58))
    interruptible_sleep_on_timeout(&wq_h, (sec*HZ));
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,32,58))
    wait_event_interruptible_timeout(wq_h, false, (sec*HZ));
#endif
    /* Check SIGKILL and SIGTERM after waking up. */
    if (signal_pending(current) && (unlikely(sigismember(&current->pending.signal, SIGKILL)) || unlikely(sigismember(&current->pending.signal, SIGTERM))))
    {
        flush_signals(current);
        osal_thread_exit(1);
    }
} /* end of osal_time_sleep */

/* Function Name:
 *      osal_time_udelay
 * Description:
 *      Delay calling thread for specified number of microseconds.
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
osal_time_udelay(uint32 usec)
{
    if (1000 <= usec)
    {
        mdelay(usec/1000);
        usec = usec % 1000;
    }
    udelay(usec);
} /* end of osal_time_udelay */

/* Function Name:
 *      osal_time_mdelay
 * Description:
 *      Delay calling thread for specified number of milliseconds.
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
osal_time_mdelay(uint32 msec)
{
    mdelay(msec);
} /* end of osal_time_mdelay */


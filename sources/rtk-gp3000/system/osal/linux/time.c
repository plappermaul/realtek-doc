/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Feature : Time relative API
 *
 */

/*
 * Include Files
 */
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <common/debug/rt_log.h>
#include <osal/time.h>
#include <osal/thread.h>
#include <osal/memory.h>
#if !defined(CONFIG_KERNEL_2_6_30)
#include <linux/wait.h>
#endif
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
#include <linux/sched/signal.h>
#endif
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
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
    struct timeval ltv;

    RT_INTERNAL_PARAM_CHK((NULL == pUsec), RT_ERR_NULL_POINTER);

    memset(&ltv, 0, sizeof(struct timeval));
    do_gettimeofday(&ltv);
    *pUsec = (osal_usecs_t)(ltv.tv_sec * USEC_PER_SEC + ltv.tv_usec);
#else
    ktime_t kt;

    kt = ktime_get();
    *pUsec = ktime_to_us(kt);
#endif
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
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
    struct timeval ltv;

    RT_INTERNAL_PARAM_CHK((NULL == pSec), RT_ERR_NULL_POINTER);

    memset(&ltv, 0, sizeof(struct timeval));
    do_gettimeofday(&ltv);
    *pSec = (osal_time_t)ltv.tv_sec;
#else
    time64_t tmp;

    tmp = ktime_get_seconds();
    *pSec = (osal_time_t)tmp;
#endif

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
#if defined(CONFIG_KERNEL_2_6_30)
    wait_queue_head_t wq_h;
#else
    DECLARE_WAIT_QUEUE_HEAD(wq);
    DEFINE_WAIT(wait);
#endif
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
#if defined(CONFIG_KERNEL_2_6_30)        
            init_waitqueue_head(&wq_h);
            interruptible_sleep_on_timeout(&wq_h, (usec/(1000000/HZ)));
#else
			prepare_to_wait(&wq, &wait, TASK_INTERRUPTIBLE);
			schedule_timeout(usec/(1000000/HZ));
			finish_wait(&wq, &wait);
#endif
        }

        /* Check SIGKILL and SIGTERM after waking up. */
        if (signal_pending(current))
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
#if defined(CONFIG_KERNEL_2_6_30)
    wait_queue_head_t wq_h;
#else
	DECLARE_WAIT_QUEUE_HEAD(wq);
    DEFINE_WAIT(wait);
#endif
    /* Can't sleep during the interrpt handling. */
    if (in_interrupt())
    {
        return;
    }
#if defined(CONFIG_KERNEL_2_6_30)     	
    init_waitqueue_head(&wq_h);
    interruptible_sleep_on_timeout(&wq_h, (sec*HZ));
#else
	prepare_to_wait(&wq, &wait, TASK_INTERRUPTIBLE);
	schedule_timeout(sec*HZ);
	finish_wait(&wq, &wait);
#endif

    /* Check SIGKILL and SIGTERM after waking up. */
    if (signal_pending(current))
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

uint32 *osal_add_timer(uint32 interval, uint32 data, timer_callback_t cb)
{
    struct timer_list *timer_ptr=NULL;
    timer_ptr = osal_alloc(sizeof(struct timer_list));
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
    init_timer(timer_ptr);
    timer_ptr->function = cb;
    timer_ptr->data = data;
#else
    /* The data should be passed by other method after kernel 4.14 */
    timer_setup(timer_ptr, cb, 0);
#endif
    timer_ptr->expires = jiffies + HZ*interval/1000;
    add_timer(timer_ptr);
#if 0    
    osal_printf("osal_add_timer: 0x%x\n\r",timer_ptr);
#endif    
    return (uint32 *)timer_ptr;
}

void osal_del_timer(uint32 *timer_ptr)
{
    if(timer_ptr == NULL)
        return;
#if 0
    if ( del_timer((struct timer_list *)timer_ptr) == 0 )
        osal_printf("del inactive timer\n\r");
    else
        osal_printf("del active timer\n\r");
#else
    del_timer((struct timer_list *)timer_ptr);
#endif

    osal_free(timer_ptr);
}

int32 osal_mod_timer(uint32 *timer_ptr, uint32 interval)
{
    if(timer_ptr == NULL)
        return -1;

    return mod_timer((struct timer_list *)timer_ptr, 
		      (jiffies + HZ*interval/1000));
}



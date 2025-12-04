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
#include <sys/time.h>
#include <unistd.h>
#include <common/error.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
#define SEC_USEC (1000 * 1000)


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
    if (usec >= SEC_USEC)
    {
        sleep(usec / SEC_USEC);
        usec %= SEC_USEC;
    }

    usleep(usec);

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
    sleep(sec);
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
    usleep(usec);    /* use nanosleep() instead of udelay() */
} /* end of osal_time_udelay */

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
    struct timeval tv;
    struct timezone tz;

    RT_PARAM_CHK((NULL == pUsec), RT_ERR_NULL_POINTER);

    gettimeofday(&tv, &tz);
    *pUsec = (osal_usecs_t)(tv.tv_sec * SEC_USEC + tv.tv_usec);

    return RT_ERR_OK;
} /* end of osal_time_usecs_get */

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
    osal_time_udelay(msec * 1000);
} /* end of osal_time_mdelay */


uint32 *osal_add_timer(uint32 interval, uint32 data, timer_callback_t cb)
{
    if(interval || data || cb)
        return NULL;

    return NULL;
}

void osal_del_timer(uint32 *timer_ptr)
{
    if(timer_ptr)
        return;
    return;
}

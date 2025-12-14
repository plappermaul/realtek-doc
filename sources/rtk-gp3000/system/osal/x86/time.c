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
#include <osal/time.h>

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
int32
osal_time_usec2Ticks_get(uint32 usec, uint32 *pTicks)
{
    if(usec);
    if(pTicks);

	return 0;
}

int32
osal_time_usecs_get(osal_usecs_t *pUsec)
{
	if(pUsec);

	return 0;
}

int32
osal_time_seconds_get(osal_time_t *pSec)
{
	if(pSec);

	return 0;
}
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
    uint32 i;
    for (i=0; i<usec; i++)
    {
    }

	return ;
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
	uint32 i;
    for (i=0; i<sec*1000000; i++)
    {
    }

	return ;
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
	uint32 i;
    for (i=0; i<usec; i++)
    {
    }

	return ;
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
    uint32 i;
    for (i=0; i<msec*1000; i++)
    {
    }

	return ;
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

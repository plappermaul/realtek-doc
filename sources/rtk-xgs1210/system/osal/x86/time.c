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
 * $Revision: 94134 $
 * $Date: 2018-12-07 18:40:51 +0800 (Fri, 07 Dec 2018) $
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
#include <common/util/rt_util_time.h>


#define SEC_USEC (1000 * 1000)


int32
osal_time_usec2Ticks_get(uint32 usec, uint32 *pTicks)
{
    return 0;
}

int32
osal_time_usecs_get(osal_usecs_t *pUsec)
{
    struct timeval tv;
    struct timezone tz;

    RT_PARAM_CHK((NULL == pUsec), RT_ERR_NULL_POINTER);

    gettimeofday(&tv, &tz);
    *pUsec = (osal_usecs_t)(tv.tv_sec * SEC_USEC + tv.tv_usec);

    return RT_ERR_OK;
}

int32
osal_time_seconds_get(osal_time_t *pSec)
{
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
    if (usec >= SEC_USEC)
    {
        sleep(usec / SEC_USEC);
        usec %= SEC_USEC;
    }

    usleep(usec);
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
    sleep(sec);
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
    usleep(usec);
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
    osal_time_udelay(msec * 1000);
} /* end of osal_time_mdelay */

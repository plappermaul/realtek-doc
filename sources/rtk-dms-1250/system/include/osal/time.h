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
 * Feature : Time relative API
 *
 */

#ifndef __OSAL_TIME_H__
#define __OSAL_TIME_H__

/*
 * Include Files
 */
#include <common/type.h>

/*
 * Symbol Definition
 */
typedef uint32  osal_time_t;
typedef uint32  osal_usecs_t;

/*
 * Function Declaration
 */

/* Function Name:
 *      osal_time_usec2Ticks_get
 * Description:
 *      Return number of ticks from input value in microseconds.
 * Input:
 *      usec    - microseconds
 * Output:
 *      pTicks - number of ticks
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      TICK_USEC - number of usec per tick in system.
 */
extern int32
osal_time_usec2Ticks_get(uint32 usec, uint32 *pTicks);

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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
osal_time_usecs_get(osal_usecs_t *pUsec);

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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
osal_time_seconds_get(osal_time_t *pSec);

/* Function Name:
 *      osal_time_usleep
 * Description:
 *      Suspend calling thread for specified number of microseconds.
 * Input:
 *      usec - microseconds.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
osal_time_usleep(uint32 usec);

/* Function Name:
 *      osal_time_sleep
 * Description:
 *      Suspend calling thread for specified number of seconds.
 * Input:
 *      sec - seconds.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
osal_time_sleep(uint32 sec);

/* Function Name:
 *      osal_time_udelay
 * Description:
 *      Delay calling thread for specified number of microseconds.
 * Input:
 *      usec - microseconds.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
osal_time_udelay(uint32 usec);


/* Function Name:
 *      osal_time_mdelay
 * Description:
 *      Delay calling thread for specified number of milliseconds.
 * Input:
 *      msec - milliseconds.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
osal_time_mdelay(uint32 msec);

#endif /* __OSAL_TIME_H__ */


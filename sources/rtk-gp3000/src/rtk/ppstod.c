/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 61228 $
 * $Date: 2016-03-16 14:49:14 +0800 (Thu, 16 Mar 2016) $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) 1PPSToD
 *
 */

 /*
 * Include Files
 */
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/rldp.h>
#include <dal/dal_mgmt.h>
#include <common/rt_type.h>


/* Function Name:
 *      rtk_ppstod_init
 * Description:
 *      Initialize Time module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize 1PPSToD module before calling any Time APIs.
 */
int32
rtk_ppstod_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_init();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_init */

/* Function Name:
 *      rtk_ppstod_delay_get
 * Description:
 *      Get the ToD delay.
 * Input:
 *      None
 * Output:
 *      delay - ToD delay
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
rtk_ppstod_delay_get(uint8 *delay)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_delay_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_delay_get(delay);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_delay_get */

/* Function Name:
 *      rtk_ppstod_todDelay_set
 * Description:
 *      Set the ToD delay.
 * Input:
 *      delay - ToD delay
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ppstod_delay_set(uint8 delay)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_delay_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_delay_set(delay);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_delay_set */

/* Function Name:
 *      rtk_ppstod_pulseWidth_get
 * Description:
 *      Get the 1PPS pulse width
 * Input:
 *      None
 * Output:
 *      width - 1PPS pulse width
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
rtk_ppstod_pulseWidth_get(uint8 *width)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_pulseWidth_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_pulseWidth_get(width);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_pulseWidth_get */

/* Function Name:
 *      rtk_ppstod_pulseWidth_set
 * Description:
 *      Set the 1PPS pulse width
 * Input:
 *      width - 1PPS pulse width
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ppstod_pulseWidth_set(uint8 width)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_pulseWidth_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_pulseWidth_set(width);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_pulseWidth_set */

/* Function Name:
 *      rtk_ppstod_mode_get
 * Description:
 *      Get the configuration about software or hardware manual mode define ToD frame.
 * Input:
 *      None
 * Output:
 *      mode - 1PPS TOD manual frame mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
rtk_ppstod_mode_get(rtk_ppstod_manualMode_t *mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_mode_get(mode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_mode_get */

/* Function Name:
 *      rtk_ppstod_mode_set
 * Description:
 *      Set the configuration about software or hardware manual define ToD frame.
 * Input:
 *      mode - 1PPS TOD manual framw mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_ppstod_mode_set(rtk_ppstod_manualMode_t mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_mode_set(mode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_mode_set */



/* Function Name:
 *      rtk_ppstod_sarpGpsWeek_get
 * Description:
 *      Get the software aid reference point GPS week.
 * Input:
 *      None
 * Output:
 *      week - software aid reference point GPS week
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
rtk_ppstod_sarpGpsWeek_get(uint16 *week)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_sarpGpsWeek_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_sarpGpsWeek_get(week);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_sarpGpsWeek_get */

/* Function Name:
 *      rtk_ppstod_sarpGpsWeek_set
 * Description:
 *      Set the software aid reference point GPS week.
 * Input:
 *      week - software aid reference point GPS week
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_ppstod_sarpGpsWeek_set(uint16 week)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_sarpGpsWeek_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_sarpGpsWeek_set(week);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_sarpGpsWeek_set */

/* Function Name:
 *      rtk_ppstod_sarpUtcSec_get
 * Description:
 *      Get the software aid reference point UTC time.
 * Input:
 *      None
 * Output:
 *      sec - software aid reference point UTC time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
rtk_ppstod_sarpUtcSec_get(uint32 *sec)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_sarpUtcSec_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_sarpUtcSec_get(sec);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_sarpUtcSec_get */

/* Function Name:
 *      rtk_ppstod_sarpUtcSec_set
 * Description:
 *      Set the software aid reference point UTC time.
 * Input:
 *      sec - software aid reference point UTC time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_ppstod_sarpUtcSec_set(uint32 sec)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_sarpUtcSec_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_sarpUtcSec_set(sec);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_sarpUtcSec_set */

/* Function Name:
 *      rtk_ppstod_frameLen_get
 * Description:
 *      Get the configuration about length of define ToD frame.
 * Input:
 *      None
 * Output:
 *      len - length of define ToD frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
rtk_ppstod_frameLen_get(uint8 *len)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_frameLen_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_frameLen_get(len);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_frameLen_get */

/* Function Name:
 *      rtk_ppstod_frameLen_set
 * Description:
 *      Set the configuration about length of define ToD frame.
 * Input:
 *      len - length of define ToD frame
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_ppstod_frameLen_set(uint8 len)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_frameLen_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_frameLen_set(len);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_frameLen_set */

/* Function Name:
 *      rtk_ppstod_frameData_get
 * Description:
 *      Get the software defined ToD frame.
 * Input:
 *      offset  - offset of frame
 * Output:
 *      val     - value of corresponding offset of frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ppstod_frameData_get(uint8 offset,uint8 *val)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_frameData_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_frameData_get(offset, val);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_frameData_get */

/* Function Name:
 *      rtk_ppstod_frameData_set
 * Description:
 *      Set the software defined ToD frame.
 * Input:
 *      offset  - offset of frame
 *      val     - value of corresponding offset of frame
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_ppstod_frameData_set(uint8 offset,uint8 val)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_frameData_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_frameData_set(offset, val);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_frameData_set */

/* Function Name:
 *      rtk_ppstod_baudrate_get
 * Description:
 *      Get the baudrate.
 * Input:
 *      None
 * Output:
 *      rate - baudrate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
rtk_ppstod_baudrate_get(uint32 *rate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_baudrate_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_baudrate_get(rate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_baudrate_get */

/* Function Name:
 *      rtk_ppstod_baudrate_set
 * Description:
 *      Set the baudrate.
 * Input:
 *      rate - baudrate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_ppstod_baudrate_set(uint32 rate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->ppstod_baudrate_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ppstod_baudrate_set(rate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_ppstod_baudrate_set */

 

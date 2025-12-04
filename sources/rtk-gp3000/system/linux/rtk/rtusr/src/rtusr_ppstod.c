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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_PPSTOD_INIT, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == delay), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.delay, delay, sizeof(uint8));
    GETSOCKOPT(RTDRV_PPSTOD_DELAY_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(delay, &ppstod_cfg.delay, sizeof(uint8));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.delay, &delay, sizeof(uint8));
    SETSOCKOPT(RTDRV_PPSTOD_DELAY_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == width), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.width, width, sizeof(uint8));
    GETSOCKOPT(RTDRV_PPSTOD_PULSEWIDTH_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(width, &ppstod_cfg.width, sizeof(uint8));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.width, &width, sizeof(uint8));
    SETSOCKOPT(RTDRV_PPSTOD_PULSEWIDTH_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.mode, mode, sizeof(rtk_ppstod_manualMode_t));
    GETSOCKOPT(RTDRV_PPSTOD_MODE_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(mode, &ppstod_cfg.mode, sizeof(rtk_ppstod_manualMode_t));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.mode, &mode, sizeof(rtk_ppstod_manualMode_t));
    SETSOCKOPT(RTDRV_PPSTOD_MODE_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == week), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.week, week, sizeof(uint16));
    GETSOCKOPT(RTDRV_PPSTOD_SARPGPSWEEK_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(week, &ppstod_cfg.week, sizeof(uint16));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.week, &week, sizeof(uint16));
    SETSOCKOPT(RTDRV_PPSTOD_SARPGPSWEEK_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == sec), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.sec, sec, sizeof(uint32));
    GETSOCKOPT(RTDRV_PPSTOD_SARPUTCSEC_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(sec, &ppstod_cfg.sec, sizeof(uint32));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.sec, &sec, sizeof(uint32));
    SETSOCKOPT(RTDRV_PPSTOD_SARPUTCSEC_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == len), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.len, len, sizeof(uint8));
    GETSOCKOPT(RTDRV_PPSTOD_FRAMELEN_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(len, &ppstod_cfg.len, sizeof(uint8));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.len, &len, sizeof(uint8));
    SETSOCKOPT(RTDRV_PPSTOD_FRAMELEN_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == val), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.offset, &offset, sizeof(uint8));
    GETSOCKOPT(RTDRV_PPSTOD_FRAMEDATA_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(val, &ppstod_cfg.val, sizeof(uint8));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.offset, &offset, sizeof(uint8));
    osal_memcpy(&ppstod_cfg.val, &val, sizeof(uint8));
    SETSOCKOPT(RTDRV_PPSTOD_FRAMEDATA_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == rate), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&ppstod_cfg.rate, rate, sizeof(uint32));
    GETSOCKOPT(RTDRV_PPSTOD_BAUDRATE_GET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);
    osal_memcpy(rate, &ppstod_cfg.rate, sizeof(uint32));

    return RT_ERR_OK;
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
    rtdrv_ppstodCfg_t ppstod_cfg;
    memset(&ppstod_cfg, 0x0, sizeof(rtdrv_ppstodCfg_t));

    /* function body */
    osal_memcpy(&ppstod_cfg.rate, &rate, sizeof(uint32));
    SETSOCKOPT(RTDRV_PPSTOD_BAUDRATE_SET, &ppstod_cfg, rtdrv_ppstodCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_ppstod_baudrate_set */

 

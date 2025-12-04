/*
 * Copyright (C) 2013 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (星期五, 03 五月 2013) $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
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
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : TIME */
 
/* Function Name:
 *      rtk_time_transparentPort_set
 * Description:
 *      Set transparent ports to the specified device.
 * Input:
 *      port   - ports
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_time_portTransparentEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&time_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_TIME_PORTTRANSPARENTENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_portTransparentEnable_set */

/* Function Name:
 *      rtk_time_transparentPort_get
 * Description:
 *      Get transparent ports to the specified device.
 * Input:
 *      port - ports
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer enable point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_portTransparentEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_TIME_PORTTRANSPARENTENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pEnable, &time_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_time_portTransparentEnable_get */

/* Function Name:
 *      rtk_time_init
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
 *      Must initialize Time module before calling any Time APIs.
 */
int32
rtk_time_init(void)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_TIME_INIT, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_init */

/* Function Name:
 *      rtk_time_portPtpEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_time_portPtpEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_TIME_PORTPTPENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pEnable, &time_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_time_portPtpEnable_get */

/* Function Name:
 *      rtk_time_portPtpEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      None
 */
int32
rtk_time_portPtpEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&time_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_TIME_PORTPTPENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_portPtpEnable_set */

/* Function Name:
 *      rtk_time_curTime_get
 * Description:
 *      Get the current time.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the current time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pTimeStamp point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_curTime_get(rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.ponTod.timeStamp, pTimeStamp, sizeof(rtk_time_timeStamp_t));
    GETSOCKOPT(RTDRV_TIME_CURTIME_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pTimeStamp, &time_cfg.ponTod.timeStamp, sizeof(rtk_time_timeStamp_t));

    return RT_ERR_OK;
}   /* end of rtk_time_curTime_get */

/* Function Name:
 *      rtk_time_curTime_latch
 * Description:
 *      Latch the current time.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_time_curTime_latch(void)
{
    rtdrv_timeCfg_t time_cfg;

    /* function body */
    GETSOCKOPT(RTDRV_TIME_CURTIME_LATCH, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_curTime_latch */

/* Function Name:
 *      rtk_time_refTime_get
 * Description:
 *      Get the reference time.
 * Input:
 *      None
 * Output:
 *      pSign      - pointer buffer of sign
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pTimeStamp/pSign point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_refTime_get(uint32 *pSign, rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSign), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.ponTod.timeStamp, pTimeStamp, sizeof(rtk_time_timeStamp_t));
    GETSOCKOPT(RTDRV_TIME_REFTIME_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pSign, &time_cfg.sign, sizeof(uint32));
    osal_memcpy(pTimeStamp, &time_cfg.ponTod.timeStamp, sizeof(rtk_time_timeStamp_t));

    return RT_ERR_OK;
}   /* end of rtk_time_refTime_get */

/* Function Name:
 *      rtk_time_refTime_set
 * Description:
 *      Set the reference time.
 * Input:
 *      sign      - significant
 *      timeStamp - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
rtk_time_refTime_set(uint32 sign, rtk_time_timeStamp_t timeStamp)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.sign, &sign, sizeof(uint32));
    osal_memcpy(&time_cfg.ponTod.timeStamp, &timeStamp, sizeof(rtk_time_timeStamp_t));
    SETSOCKOPT(RTDRV_TIME_REFTIME_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_refTime_set */

/* Function Name:
 *      rtk_time_frequency_set
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_time_frequency_set(uint32 freq)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.freq, &freq, sizeof(uint32));
    SETSOCKOPT(RTDRV_TIME_FREQUENCY_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_frequency_set */

/* Function Name:
 *      rtk_time_frequency_get
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_time_frequency_get(uint32 *freq)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == freq), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.freq, freq, sizeof(uint32));
    GETSOCKOPT(RTDRV_TIME_FREQUENCY_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(freq, &time_cfg.freq, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_time_frequency_get */

/* Function Name:
 *      rtk_time_ptpIgrMsgAction_set
 * Description:
 *      Set ingress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 *      igr_action    - ingress action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NOT_ALLOWED 		  - Invalid action.
 *      RT_ERR_INPUT 			      - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_time_ptpIgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t igr_action)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.type, &type, sizeof(rtk_time_ptpMsgType_t));
    osal_memcpy(&time_cfg.igr_action, &igr_action, sizeof(rtk_time_ptpIgrMsg_action_t));
    SETSOCKOPT(RTDRV_TIME_PTPIGRMSGACTION_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_ptpIgrMsgAction_set */

/* Function Name:
 *      rtk_time_ptpIgrMsgAction_get
 * Description:
 *      Get ingress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 * Output:
 *      igr_action    - ingress action.
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_NULL_POINTER  - Pointer igr_action point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_ptpIgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t *igr_action)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == igr_action), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.type, &type, sizeof(rtk_time_ptpMsgType_t));
    GETSOCKOPT(RTDRV_TIME_PTPIGRMSGACTION_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(igr_action, &time_cfg.igr_action, sizeof(rtk_time_ptpIgrMsg_action_t));

    return RT_ERR_OK;
}   /* end of rtk_time_ptpIgrMsgAction_get */

/* Function Name:
 *      rtk_time_ptpEgrMsgAction_set
 * Description:
 *      Set egress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 *      egr_action    - egress action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NOT_ALLOWED 		  - Invalid action.
 *      RT_ERR_INPUT 			      - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_time_ptpEgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t egr_action)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.type, &type, sizeof(rtk_time_ptpMsgType_t));
    osal_memcpy(&time_cfg.egr_action, &egr_action, sizeof(rtk_time_ptpEgrMsg_action_t));
    SETSOCKOPT(RTDRV_TIME_PTPEGRMSGACTION_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_ptpEgrMsgAction_set */

/* Function Name:
 *      rtk_time_ptpEgrMsgAction_get
 * Description:
 *      Get egress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 * Output:
 *      egr_action    - egress action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Pointer egr_action point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_ptpEgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t *egr_action)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == egr_action), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.type, &type, sizeof(rtk_time_ptpMsgType_t));
    GETSOCKOPT(RTDRV_TIME_PTPEGRMSGACTION_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(egr_action, &time_cfg.egr_action, sizeof(rtk_time_ptpEgrMsg_action_t));

    return RT_ERR_OK;
}   /* end of rtk_time_ptpEgrMsgAction_get */

/* Function Name:
 *      rtk_time_meanPathDelay_set
 * Description:
 *      Set rtk_time_meanPathDelay_set of PTP system time.
 * Input:
 *      delay - mean path delay value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_time_meanPathDelay_set(uint32 delay)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.delay, &delay, sizeof(uint32));
    SETSOCKOPT(RTDRV_TIME_MEANPATHDELAY_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_meanPathDelay_set */

/* Function Name:
 *      rtk_time_meanPathDelay_get
 * Description:
 *      Get rtk_time_meanPathDelay_get of PTP system time.
 * Input:
 *      delay    - mean path delay.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - - Pointer delay point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_meanPathDelay_get(uint32 *delay)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == delay), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.delay, delay, sizeof(uint32));
    GETSOCKOPT(RTDRV_TIME_MEANPATHDELAY_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(delay, &time_cfg.delay, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_time_meanPathDelay_get */

/* Function Name:
 *      rtk_time_rxTime_set
 * Description:
 *      Set the RX time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *      
 */
int32
rtk_time_rxTime_set(rtk_time_timeStamp_t timeStamp)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.ponTod.timeStamp, &timeStamp, sizeof(rtk_time_timeStamp_t));
    SETSOCKOPT(RTDRV_TIME_RXTIME_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_rxTime_set */

/* Function Name:
 *      rtk_time_rxTime_get
 * Description:
 *      Get the RX time.
 * Input:
 *      None.
 * Output:
 *      pTimeStamp - pointer buffer of the RX time.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *      
 */
int32
rtk_time_rxTime_get(rtk_time_timeStamp_t *pTimeStamp)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.ponTod.timeStamp, pTimeStamp, sizeof(rtk_time_timeStamp_t));
    GETSOCKOPT(RTDRV_TIME_RXTIME_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pTimeStamp, &time_cfg.ponTod.timeStamp, sizeof(rtk_time_timeStamp_t));

    return RT_ERR_OK;
}   /* end of rtk_time_rxTime_get */

/* Function Name:
 *      rtk_time_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *      
 */
int32
rtk_time_ponTodTime_set(rtk_pon_tod_t ponTod)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.ponTod, &ponTod, sizeof(rtk_pon_tod_t));
    SETSOCKOPT(RTDRV_TIME_PONTOD_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_ponTodTime_set */

/* Function Name:
 *      rtk_time_gpon_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      
 */
int32
rtk_time_gpon_ponTodTime_set(rtk_pon_tod_t ponTod)
{
    ponTod.ponMode=TOD_PON_MODE_GPON;
    
    return rtk_time_ponTodTime_set(ponTod);
}   /* end of rtk_time_ponTodTime_set */

/* Function Name:
 *      rtk_time_epon_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      
 */
int32
rtk_time_epon_ponTodTime_set(rtk_pon_tod_t ponTod)
{
    ponTod.ponMode=TOD_PON_MODE_EPON;
    
    return rtk_time_ponTodTime_set(ponTod);
}   /* end of rtk_time_ponTodTime_set */

/* Function Name:
 *      rtk_time_ponTodTime_Get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None.
 * Output:
 *      pTimeStamp - pointer buffer of the PON TOD time.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *      
 */
int32
rtk_time_ponTodTime_get(rtk_pon_tod_t *pPonTod)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTod), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.ponTod, pPonTod, sizeof(rtk_pon_tod_t));
    GETSOCKOPT(RTDRV_TIME_PONTOD_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pPonTod, &time_cfg.ponTod, sizeof(rtk_pon_tod_t));

    return RT_ERR_OK;
}   /* end of rtk_time_rxTime_get */

/* Function Name:
 *      rtk_time_portPtpTxIndicator_get
 * Description:
 *      Get PTP tx indicator of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pTxIndicator - point of tx indicator
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
int32
rtk_time_portPtpTxIndicator_get(rtk_port_t port, rtk_enable_t *pTxIndicator)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTxIndicator), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_TIME_PORTPTPTXINDICATOR_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pTxIndicator, &time_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_time_portPtpTxIndicator_get */

/* Function Name:
 *      rtk_time_todEnable_set
 * Description:
 *      Set pon tod synchronization state.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_time_todEnable_set(rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_TIME_TODENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_todEnable_set */

/* Function Name:
 *      rtk_time_todEnable_get
 * Description:
 *      Get pon tod synchronizaton status.
 * Input:
 *      None
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer enable point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_todEnable_get(rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_TIME_TODENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pEnable, &time_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_time_todEnable_get */

/* Function Name:
 *      rtk_time_ppsEnable_set
 * Description:
 *      Set PTP PPS state.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_time_ppsEnable_set(rtk_enable_t enable)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_TIME_PPSENABLE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_ppsEnable_set */

/* Function Name:
 *      rtk_time_ppsEnable_get
 * Description:
 *      Get PTP PPS status.
 * Input:
 *      None
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer enable point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_ppsEnable_get(rtk_enable_t *pEnable)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_TIME_PPSENABLE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pEnable, &time_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_time_ppsEnable_get */

/* Function Name:
 *      rtk_time_ppsMode_set
 * Description:
 *      Set PTP PPS mode.
 * Input:
 *      mode - pps mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_time_ppsMode_set(rtk_time_ptpPpsMode_t mode)
{
    rtdrv_timeCfg_t time_cfg;
    memset(&time_cfg, 0x0, sizeof(rtdrv_timeCfg_t));

    /* function body */
    osal_memcpy(&time_cfg.mode, &mode, sizeof(rtk_time_ptpPpsMode_t));
    SETSOCKOPT(RTDRV_TIME_PPSMODE_SET, &time_cfg, rtdrv_timeCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_time_ppsMode_set */

/* Function Name:
 *      rtk_time_ppsMode_get
 * Description:
 *      Get PTP PPS mode.
 * Input:
 *      None
 * Output:
 *      pMode - pps mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer mode point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_ppsMode_get(rtk_time_ptpPpsMode_t *pMode)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_TIME_PPSMODE_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pMode, &time_cfg.mode, sizeof(rtk_time_ptpPpsMode_t));

    return RT_ERR_OK;
}   /* end of rtk_time_ppsMode_get */

/* Function Name:
 *      rtk_time_freeTime_get
 * Description:
 *      Get the freerun time.
 * Input:
 *      None
 * Output:
 *      pFreeTime - pointer buffer of the freerun time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pFreeTime point to NULL.
 * Note:
 *      None
 */
int32
rtk_time_freeTime_get(rtk_time_freerun_t *pFreeTime)
{
    rtdrv_timeCfg_t time_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFreeTime), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&time_cfg.freeTime, pFreeTime, sizeof(rtk_time_freerun_t));
    GETSOCKOPT(RTDRV_TIME_FREETIME_GET, &time_cfg, rtdrv_timeCfg_t, 1);
    osal_memcpy(pFreeTime, &time_cfg.freeTime, sizeof(rtk_time_freerun_t));

    return RT_ERR_OK;
}   /* end of rtk_time_freeTime_get */

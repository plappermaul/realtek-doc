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
 * $Revision$
 * $Date$
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
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/time.h> 
#include <dal/dal_mgmt.h> 

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
    int32   ret;

    if (NULL == RT_MAPPER->time_portTransparentEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portTransparentEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portTransparentEnable_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_portTransparentEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portTransparentEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portTransparentEnable_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_init)
        return RT_ERR_OK;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_init */

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
    int32   ret;

    if (NULL == RT_MAPPER->time_portPtpEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portPtpEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portPtpEnable_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_portPtpEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portPtpEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portPtpEnable_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_curTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_curTime_get( pTimeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_curTime_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_curTime_latch)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_curTime_latch();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_curTime_latch */


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
    int32   ret;
    if (NULL == RT_MAPPER->time_refTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_refTime_get( pSign, pTimeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_refTime_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_refTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_refTime_set( sign, timeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_refTime_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_frequency_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_frequency_set( freq);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_frequency_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_frequency_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_frequency_get( freq);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_frequency_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpIgrMsgAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpIgrMsgAction_set( type, igr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpIgrMsgAction_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpIgrMsgAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpIgrMsgAction_get( type, igr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpIgrMsgAction_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpEgrMsgAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpEgrMsgAction_set( type, egr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpEgrMsgAction_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_ptpEgrMsgAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpEgrMsgAction_get( type, egr_action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ptpEgrMsgAction_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_meanPathDelay_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_meanPathDelay_set( delay);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_meanPathDelay_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_meanPathDelay_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_meanPathDelay_get( delay);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_meanPathDelay_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_rxTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_rxTime_set( timeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_rxTime_set */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_rxTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_rxTime_get( pTimeStamp);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_rxTime_get */

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
    int32   ret;
    if (NULL == RT_MAPPER->time_ponTodTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ponTodTime_set( ponTod);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ponTodTime_set */


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
} /* end of rtk_time_gpon_ponTodTime_set */

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
} /* end of rtk_time_epon_ponTodTime_set */


/* Function Name:
 *      rtk_time_ponTodTime_get
 * Description:
 *      Get the PON TOD time.
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
rtk_time_ponTodTime_get(rtk_pon_tod_t *pPonTod)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ponTodTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ponTodTime_get( pPonTod);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ponTodTime_get */

/* Function Name:
 *      rtk_time_portPtpTxIndicator_get
 * Description:
 *      Get PTP TX indicator of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pTxIndicator - point of tx indicator
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rtk_time_portPtpTxIndicator_get(rtk_port_t port, rtk_enable_t *pTxIndicator)
{
    int32   ret;

    if (NULL == RT_MAPPER->time_portPtpTxIndicator_get)
        return RT_ERR_DRIVER_NOT_FOUND;
 
    RTK_API_LOCK();
    ret = RT_MAPPER->time_portPtpTxIndicator_get( port, pTxIndicator);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_portPtpTxIndicator_get */

/* Function Name:
 *      rtk_time_todEnable_get
 * Description:
 *      Get the configuration about pon tod synchronization status.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of pon tod synchronization.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of pon tod synchronization:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_time_todEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_todEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_todEnable_get( pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_todEnable_get */

/* Function Name:
 *      rtk_time_todEnable_set
 * Description:
 *      Set the configuration about pon tod synchronization state.
 * Input:
 *      enable - status of pon tod synchronization.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of pon tod synchronization:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_time_todEnable_set(rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_todEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_todEnable_set( enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_todEnable_set */

/* Function Name:
 *      rtk_time_ppsEnable_get
 * Description:
 *      Get the configuration about PTP PPS status.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of PTP PPS.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of PTP PPS:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_time_ppsEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ppsEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ppsEnable_get( pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ppsEnable_get */

/* Function Name:
 *      rtk_time_ppsEnable_set
 * Description:
 *      Set the configuration about PTP PPS state.
 * Input:
 *      enable - status of PTP PPS.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of PTP PPS:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_time_ppsEnable_set(rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ppsEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ppsEnable_set( enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ppsEnable_set */

/* Function Name:
 *      rtk_time_ppsMode_get
 * Description:
 *      Get the configuration about PTP PPS mode.
 * Input:
 *      None.
 * Output:
 *      pMode - mode of PTP PPS.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The mode of PTP PPS:
 *      - PON
 *      - PTP
 */
int32
rtk_time_ppsMode_get(rtk_time_ptpPpsMode_t *pMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ppsMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ppsMode_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ppsMode_get */

/* Function Name:
 *      rtk_time_ppsMode_set
 * Description:
 *      Set the configuration about PTP PPS mode.
 * Input:
 *      mode - mode of PTP PPS.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The mode of PTP PPS:
 *      - PON
 *      - PTP
 */
int32
rtk_time_ppsMode_set(rtk_time_ptpPpsMode_t mode)
{
    int32   ret;
    if (NULL == RT_MAPPER->time_ppsMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ppsMode_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_time_ppsMode_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->time_freeTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_freeTime_get(pFreeTime);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_time_freeTime_get */
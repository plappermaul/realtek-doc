/*
 * Copyright (C) 2023 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *           (2) PON ToD
 *           (3) 1PPS/ToD
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/time.h>
#include <rtk/rt/rt_time.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rt_time_init
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
rt_time_init(void)
{
    int32   ret = RT_ERR_OK;
#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_init)
        return RT_ERR_OK;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_init();
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_init)
        return RT_ERR_OK;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_init();
    RTK_API_UNLOCK();
#endif
    return ret;
} /* end of rt_time_init */

/* Function Name:
 *      rt_time_curTime_get
 * Description:
 *      Get the current time.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the current time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rt_time_curTime_get(rt_time_timeStamp_t *pTimeStamp)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_time_timeStamp_t rtk_timeStamp;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_curTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_curTime_get( pTimeStamp);
    RTK_API_UNLOCK();

#else
    if (NULL == RT_MAPPER->time_curTime_latch)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->time_curTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_curTime_latch();
    RTK_API_UNLOCK();

    if(ret != RT_ERR_OK)
        return ret;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_curTime_get( &rtk_timeStamp);
    RTK_API_UNLOCK();

    pTimeStamp->sec = rtk_timeStamp.sec;
    pTimeStamp->nsec = rtk_timeStamp.nsec;
#endif
    return ret;
} /* end of rt_time_curTime_get */

/* Function Name:
 *      rt_time_offsetTime_set
 * Description:
 *      Set the offset time.
 * Input:
 *      sign      - significant
 *      timeStamp - offset timestamp value
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
rt_time_offsetTime_set(rt_time_ptpSign_t sign, rt_time_timeStamp_t timeStamp)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_time_timeStamp_t rtk_timeStamp;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_offsetTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_offsetTime_set( sign, timeStamp);
    RTK_API_UNLOCK();
#else
     rtk_timeStamp.sec = timeStamp.sec;
     rtk_timeStamp.nsec = timeStamp.nsec;

     if (NULL == RT_MAPPER->time_refTime_set)
         return RT_ERR_DRIVER_NOT_FOUND;

     RTK_API_LOCK();
     ret = RT_MAPPER->time_refTime_set( sign, rtk_timeStamp);
     RTK_API_UNLOCK();
#endif

    return ret;
} /* end of rt_time_offsetTime_set */

/* Function Name:
 *      rt_time_frequency_set
 * Description:
 *      Set frequency of timer
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      setting frequency of calculating timer
 */
int32
rt_time_frequency_set(uint32 freq)
{
    int32   ret = RT_ERR_OK;

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_frequency_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_frequency_set( freq);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_frequency_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_frequency_set( freq);
    RTK_API_UNLOCK();
#endif

    return ret;
} /* end of rt_time_frequency_set */

/* Function Name:
 *      rt_time_frequency_get
 * Description:
 *      Set frequency of timer
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      getting frequency of calculating timer
 */
int32
rt_time_frequency_get(uint32 *pFreq)
{
    int32   ret = RT_ERR_OK;

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_frequency_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_frequency_get( pFreq);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_frequency_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_frequency_get( pFreq);
    RTK_API_UNLOCK();
#endif

    return ret;
} /* end of rt_time_frequency_get */

/* Function Name:
 *      rt_time_ptpPortEnable_set
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
 *      Set enabling ptp function by port
 */
int32
rt_time_ptpPortEnable_set(rt_port_t port, rt_enable_t enable)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_port_t rtk_port = 0;
    rtk_enable_t rt_enable = FALSE;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptpPortEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptpPortEnable_set( port, enable);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_portPtpEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    rtk_port = port;
    rt_enable = enable;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_portPtpEnable_set( rtk_port, rt_enable);
    RTK_API_UNLOCK();
#endif

    return ret;
} /* end of rt_time_ptpPortEnable_set */

/* Function Name:
 *      rt_time_ptpPortEnable_get
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
 * Note:
 *      Get enabling status of prp function
 */
int32
rt_time_ptpPortEnable_get(rt_port_t port, rt_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_port_t rtk_port = 0;
    rtk_enable_t rt_enable = FALSE;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptpPortEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptpPortEnable_get( port, pEnable);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_portPtpEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    rtk_port = port;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_portPtpEnable_get( rtk_port, &rt_enable);
    RTK_API_UNLOCK();

    *pEnable = rt_enable;
#endif

    return ret;
} /* end of rt_time_ptpPortEnable_get */

/* Function Name:
 *      rt_time_ptpIgrAction_set
 * Description:
 *      Set Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 *      action - ptp ingress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
int32
rt_time_ptpIgrAction_set(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t action)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_time_ptpMsgType_t msgType = PTP_MSG_TYPE_SYNC;
    rtk_time_ptpIgrMsg_action_t igr_action = PTP_IGR_ACTION_NONE;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptpIgrAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptpIgrAction_set( type, action);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_ptpIgrMsgAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(type != RT_PTP_IGR_TYPE_L2)
        return RT_ERR_INPUT;

    if (action == RT_PTP_IGR_ACTION_NONE)
        igr_action = PTP_IGR_ACTION_NONE;
    else if (action == RT_PTP_IGR_ACTION_TRAP2CPU )
        igr_action = PTP_IGR_ACTION_TRAP2CPU;
    else
        return RT_ERR_INPUT;

    for(msgType = PTP_MSG_TYPE_SYNC ;msgType < PTP_MSG_TYPE_END; msgType ++)
    {
        RTK_API_LOCK();
        ret = RT_MAPPER->time_ptpIgrMsgAction_set( msgType, igr_action);
        RTK_API_UNLOCK();
    }
#endif

     return ret;
} /* end of rt_time_ptpIgrAction_set */

/* Function Name:
 *      rt_time_ptpIgrAction_get
 * Description:
 *      Get Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 * Output:
 *      pAction - ptp ingress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
int32
rt_time_ptpIgrAction_get(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t *pAction)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_time_ptpIgrMsg_action_t igr_action = PTP_IGR_ACTION_NONE;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptpIgrAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptpIgrAction_get( type, pAction);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_ptpIgrMsgAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    if(type != RT_PTP_IGR_TYPE_L2)
        return RT_ERR_INPUT;
    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpIgrMsgAction_get( PTP_MSG_TYPE_SYNC, &igr_action);
    RTK_API_UNLOCK();
    if(igr_action == PTP_IGR_ACTION_NONE)
        *pAction = RT_PTP_IGR_ACTION_NONE;
    else if(igr_action == PTP_IGR_ACTION_TRAP2CPU)
        *pAction = PTP_IGR_ACTION_TRAP2CPU;
    else
        return RT_ERR_FAILED;
#endif

    return ret;
} /* end of rt_time_ptpIgrAction_get */

/* Function Name:
 *      rt_time_ptpEgrAction_set
 * Description:
 *      Set Type of PTP egress action .
 * Input:
 *      action - ptp engress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
  * Note:
 *      Action is no-operation or one-step or two-step
 */
int32
rt_time_ptpEgrAction_set(rt_time_ptpEgr_action_t action)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_time_ptpMsgType_t msgType = PTP_MSG_TYPE_SYNC;
    rtk_time_ptpEgrMsg_action_t egr_action = PTP_EGR_ACTION_NONE;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptpEgrAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptpEgrAction_set( action);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_ptpEgrMsgAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (action == RT_PTP_EGR_ACTION_NONE)
        egr_action = PTP_EGR_ACTION_NONE;
    else if (action == RT_PTP_EGR_ACTION_ONESTEP )
        egr_action = PTP_EGR_ACTION_LATCH_TIME;
    else if (action == RT_PTP_EGR_ACTION_TWOSTEP )
        egr_action = PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU;
    else
        return RT_ERR_INPUT;

    for(msgType = PTP_MSG_TYPE_SYNC ;msgType < PTP_MSG_TYPE_END; msgType ++)
    {
        RTK_API_LOCK();
        ret = RT_MAPPER->time_ptpEgrMsgAction_set( msgType, egr_action);
        RTK_API_UNLOCK();
    }
#endif

     return ret;
} /* end of rt_time_ptpEgrAction_set */

/* Function Name:
 *      rt_time_ptpEgrAction_get
 * Description:
 *      Get Type of PTP egress action .
 * Input:
 *      None
 * Output:
 *      pAction - ptp egress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *     Action is no-operation or one-step or two-step
 */
int32
rt_time_ptpEgrAction_get(rt_time_ptpEgr_action_t *pAction)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_time_ptpEgrMsg_action_t egr_action = PTP_EGR_ACTION_NONE;
#endif

    if(pAction == NULL)
        return RT_ERR_INPUT;

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptpEgrAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptpEgrAction_get( pAction);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_ptpIgrMsgAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_ptpEgrMsgAction_get( PTP_MSG_TYPE_SYNC, &egr_action);
    RTK_API_UNLOCK();

    if(egr_action == PTP_EGR_ACTION_NONE)
        *pAction = RT_PTP_EGR_ACTION_NONE;
    else if(egr_action == PTP_EGR_ACTION_LATCH_TIME)
        *pAction = RT_PTP_EGR_ACTION_ONESTEP;
    else if(egr_action == PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU)
        *pAction = RT_PTP_EGR_ACTION_TWOSTEP;
    else
        return RT_ERR_FAILED;
#endif

    return ret;
} /* end of rt_time_ptpEgrAction_get */

/* Function Name:
 *      rt_time_ptp_tx
 * Description:
 *      Send PTP Packet.
 * Input:
 *      port - destination Port
 *      msgLen - length of PTP Packet without CRC
 *      pMsg - content of PTP Packet without CRC
 *      type - none or L2 or UDP for informing which ptp packet is
 *      timeOffset - attach timestamp at offset of PTP packet when one step
 *      udpCSOft - informing chip UDP checksum offset for re-calculating when one step
 * Output:
 *      pTimeStamp - return recording timestamp when two step
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Type is none or L2 or UDP
 */
int32
rt_time_ptp_tx(uint8 port,uint32 msgLen,uint8 *pMsg,rt_time_ptpEgr_type_t type,uint16 timeOffset,uint16 udpCSOft,rt_time_timeStamp_t *pTimeStamp)
{
    int32   ret = RT_ERR_OK;

    if(pMsg == NULL || pTimeStamp == NULL)
        return RT_ERR_INPUT;

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptp_tx)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptp_tx(port,msgLen,pMsg,type,timeOffset,udpCSOft,pTimeStamp);
    RTK_API_UNLOCK();
#else
    //TBD
#endif

    return ret;
} /* end of rt_time_ptp_tx */

/* Function Name:
 *      rt_time_ptp_rx_callback_register
 * Description:
 *      PTP packet rx call back
 * Input:
 *      ptpRx - call back function of PTP Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      rt_time_ptp_rx_callback)(uins32* srcPort, uint32 *msgLen,uint8 *pMsg, rt_time_timeStamp_t *pTimeStamp), pTimeStamp is recording timestamp, pTimeStamp is recording timestamp
 */
int32
rt_time_ptp_rx_callback_register(rt_time_ptp_rx_callback ptpRx)
{
    int32   ret = RT_ERR_OK;
#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ptp_rx_callback_register)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ptp_rx_callback_register( ptpRx);
    RTK_API_UNLOCK();
#else
    //TBD
#endif
    return ret;
} /* end of rt_time_ptp_rx_callback_register */

/* Function Name:
 *      rt_time_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      ponTod   - pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Set corresponding superfram/localtime to update timer
 */
int32
rt_time_ponTodTime_set(rt_pon_tod_t ponTod)
{
    int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
    rtk_pon_tod_t rtk_pon_tod;
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ponTodTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ponTodTime_set( ponTod);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_ponTodTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    rtk_pon_tod.ponMode = ponTod.ponMode;
    rtk_pon_tod.startPoint.localTime = ponTod.startPoint.localTime;
    rtk_pon_tod.timeStamp.sec = ponTod.timeStamp.sec;
    rtk_pon_tod.timeStamp.nsec = ponTod.timeStamp.nsec;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_ponTodTime_set( rtk_pon_tod);
    RTK_API_UNLOCK();
#endif

    return ret;
} /* end of rt_time_ponTodTime_set */

/* Function Name:
 *      rt_time_ponTodTime_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pPonTod   - pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Get corresponding superfram/localtime to update timer
 */
int32
rt_time_ponTodTime_get(rt_pon_tod_t *pPonTod)
{
     int32   ret = RT_ERR_OK;

#if !defined(CONFIG_LUNA_G3_SERIES)
     rtk_pon_tod_t rtk_pon_tod;

     memset(&rtk_pon_tod,0,sizeof(rtk_pon_tod_t));
#endif

#if defined(CONFIG_LUNA_G3_SERIES)
     if (NULL == RT_MAPPER->rt_time_ponTodTime_get)
         return RT_ERR_DRIVER_NOT_FOUND;

     RTK_API_LOCK();
     ret = RT_MAPPER->rt_time_ponTodTime_get( pPonTod);
     RTK_API_UNLOCK();
#else
     if (NULL == RT_MAPPER->time_ponTodTime_get)
         return RT_ERR_DRIVER_NOT_FOUND;

     RTK_API_LOCK();
     ret = RT_MAPPER->time_ponTodTime_get( &rtk_pon_tod);
     RTK_API_UNLOCK();

     pPonTod->ponMode = rtk_pon_tod.ponMode;
     pPonTod->startPoint.localTime = rtk_pon_tod.startPoint.localTime;
     pPonTod->timeStamp.sec = rtk_pon_tod.timeStamp.sec;
     pPonTod->timeStamp.nsec = rtk_pon_tod.timeStamp.nsec;
#endif

     return ret;
} /* end of rt_time_ponTodTime_get */

/* Function Name:
 *      rt_time_ponTodTimeEnable_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      enable   - enable pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Enable/Disable PON ToD to update time
 */
int32
rt_time_ponTodTimeEnable_set(rt_enable_t enable)
{
    int32   ret = RT_ERR_OK;
#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ponTodTimeEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ponTodTimeEnable_set( enable);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_todEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_todEnable_set( enable);
    RTK_API_UNLOCK();
#endif
    return ret;
} /* end of rt_time_ponTodTimeEnable_set */

/* Function Name:
 *      rt_time_ponTodTimeEnable_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pEnable   - enable pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Getting status enable/eisable PON ToD to update time
 */
int32
rt_time_ponTodTimeEnable_get(rt_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;

#if defined(CONFIG_LUNA_G3_SERIES)
    if (NULL == RT_MAPPER->rt_time_ponTodTimeEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_time_ponTodTimeEnable_get( pEnable);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->time_todEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->time_todEnable_get( pEnable);
    RTK_API_UNLOCK();
#endif

    return ret;
} /* end of rt_time_ponTodTimeEnable_get */

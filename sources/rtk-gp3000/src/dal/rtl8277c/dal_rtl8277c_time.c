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
 *
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_time.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <dal/rtl8277c/dal_rtl8277c_time.h>
#include <osal/time.h>

#include "aal_ptp.h"
#include "aal_gpon.h"
#include "aal_pon.h"


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

static uint32    time_init = {INIT_NOT_COMPLETED};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl8277c_time_init
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
dal_rtl8277c_time_init(void)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    ret = aal_ptp_timer_enable_set(0,0);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    ret = aal_ptp_init(0);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    time_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl8277c_time_init */

/* Function Name:
 *      dal_rtl8277c_rt_time_curTime_get
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
dal_rtl8277c_rt_time_curTime_get(rt_time_timeStamp_t *pTimeStamp)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;
    ca_uint64_t sec;
    ca_uint32_t nsec;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    ret = aal_ptp_timer_get(0,&sec,&nsec);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    pTimeStamp->sec = sec;
    pTimeStamp->nsec = nsec;

    return ret;
} /* end of dal_rtl8277c_rt_time_curTime_get */

/* Function Name:
 *      dal_rtl8277c_rt_time_offsetTime_set
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
dal_rtl8277c_rt_time_offsetTime_set(uint32 sign, rt_time_timeStamp_t timeStamp)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_SIGN_END <= sign), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL8277C_TIME_SEC_MAX < timeStamp.sec), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL8277C_TIME_NSEC_MAX <= timeStamp.nsec), RT_ERR_INPUT);

    if(sign == RT_PTP_SIGN_POSTIVE)
    {
        val = (uint32)(timeStamp.sec & 0xFFFFFFFF);
    }
    else
    {
        if(timeStamp.nsec != 0)
        {
            val = (uint32)((~(timeStamp.sec + 1)+1) & 0xFFFFFFFFUL);
            /*bit 31~0*/

            val = (uint32)(((~(timeStamp.sec + 1)+1) >> 32) & 0xFFFF);
            /*bit 47~32*/
        }
        else
        {
            val = (uint32)((~timeStamp.sec +1) & 0xFFFFFFFFUL);
            /*bit 31~0*/

            val = (uint32)(((~timeStamp.sec +1) >> 32) & 0xFFFF);
            /*bit 47~32*/
        }
    }

    if(sign == RT_PTP_SIGN_POSTIVE)
    {
        val = (uint32)(timeStamp.nsec);
    }
    else
    {
        if(timeStamp.nsec != 0)
            val = (uint32)(1000000000 - timeStamp.nsec);
        else
            val = 0;
    }

    return ret;
} /* end of dal_rtl8277c_rt_time_offsetTime_set */

/* Function Name:
 *      dal_rtl8277c_time_frequency_set
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
dal_rtl8277c_time_frequency_set(uint32 freq)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RTL8277C_TIME_FREQUENCY_MAX < freq), RT_ERR_INPUT);

    val = freq;

    return ret;
} /* end of dal_rtl8277c_time_frequency_set */

/* Function Name:
 *      dal_rtl8277c_time_frequency_get
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
dal_rtl8277c_time_frequency_get(uint32 *freq)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == freq), RT_ERR_NULL_POINTER);


    *freq = val;

    return ret;
} /* end of dal_rtl8277c_time_frequency_get */

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpPortEnable_set
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
dal_rtl8277c_rt_time_ptpPortEnable_set(rt_port_t port, rt_enable_t enable)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    return ret;
} /* end of dal_rtl8277c_rt_time_ptpPortEnable_set */

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpPortEnable_get
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
dal_rtl8277c_rt_time_ptpPortEnable_get(rt_port_t port, rt_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    return ret;
} /* end of dal_rtl8277c_rt_time_ptpPortEnable_get */


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
dal_rtl8277c_rt_time_ptpIgrAction_set(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t action)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_IGR_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((RT_PTP_IGR_ACTION_END <= action), RT_ERR_INPUT);

    return ret;
} /* end of dal_rtl8277c_rt_time_ptpIgrAction_set */

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpIgrAction_get
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
dal_rtl8277c_rt_time_ptpIgrAction_get(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t *pAction)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_IGR_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    return ret;
} /* end of dal_rtl8277c_rt_time_ptpIgrAction_get */

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpEgrAction_set
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
dal_rtl8277c_rt_time_ptpEgrAction_set(rt_time_ptpEgr_action_t action)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_EGR_ACTION_END <= action), RT_ERR_INPUT);

    return ret;
} /* end of dal_rtl8277c_rt_time_ptpEgrAction_set */

/* Function Name:
 *      dal_rtl8277c_rt_rt_time_ptpEgrAction_get
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
dal_rtl8277c_rt_rt_time_ptpEgrAction_get(rt_time_ptpEgr_action_t *pAction)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    return ret;
} /* end of dal_rtl8277c_rt_rt_time_ptpEgrAction_get */

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTime_set
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
dal_rtl8277c_rt_time_ponTodTime_set(rt_pon_tod_t ponTod)
{
    int32   ret = RT_ERR_OK;
    ca_uint32_t pon_mode = aal_pon_mac_mode_get(0);
    ca_uint64_t sec;
    ca_uint32_t nsec;
    aal_xgpn_glb_super_frame_match_val_t val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RTL8277C_TIME_SEC_MAX < ponTod.timeStamp.sec), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL8277C_TIME_NSEC_MAX <= ponTod.timeStamp.nsec), RT_ERR_INPUT);

    switch(pon_mode)
    {
        case ONU_PON_MAC_EPON_1G:
        case ONU_PON_MAC_EPON_D10G:
        case ONU_PON_MAC_EPON_BI10G:
            RT_PARAM_CHK((0 != ponTod.ponMode), RT_ERR_INPUT);

            ret = aal_epon_onu_mpcp_timer_match_cfg_set(0,0,ponTod.startPoint.localTime);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        case ONU_PON_MAC_GPON:
            RT_PARAM_CHK((1 != ponTod.ponMode), RT_ERR_INPUT);

            ret = aal_gpon_sup_frame_value_set(0,ponTod.startPoint.superFrame);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        case ONU_PON_MAC_XGPON1:
        case ONU_PON_MAC_XGSPON:
            RT_PARAM_CHK((2 != ponTod.ponMode), RT_ERR_INPUT);

            val.sfc0 = ponTod.startPoint.xSuperFrame & 0xffffffff;
            val.sfc1 = (ponTod.startPoint.xSuperFrame>>32) & 0xffffffff;

            ret = aal_xgpon_xgpn_glb_super_frame_match_val_set(0,val);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        default:
            return RT_ERR_INPUT;
    }

    sec = ponTod.timeStamp.sec;
    nsec = ponTod.timeStamp.nsec;
    ret = aal_ptp_load_timer_set(0,sec,nsec);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl8277c_rt_time_ponTodTime_set */

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTime_get
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
dal_rtl8277c_rt_time_ponTodTime_get(rt_pon_tod_t *pPonTod)
{
    int32   ret = RT_ERR_OK;
    ca_uint32_t pon_mode = aal_pon_mac_mode_get(0);
    ca_uint64_t sec;
    ca_uint32_t nsec;
    aal_xgpn_glb_super_frame_match_val_t val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTod), RT_ERR_NULL_POINTER);

    switch(pon_mode)
    {
        case ONU_PON_MAC_EPON_1G:
        case ONU_PON_MAC_EPON_D10G:
        case ONU_PON_MAC_EPON_BI10G:
            pPonTod->ponMode = 0;

            ret = aal_epon_onu_mpcp_timer_match_cfg_get(0,0,&pPonTod->startPoint.localTime);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        case ONU_PON_MAC_GPON:
            pPonTod->ponMode = 1;

            ret = aal_gpon_sup_frame_value_get(0,&pPonTod->startPoint.superFrame);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        case ONU_PON_MAC_XGPON1:
        case ONU_PON_MAC_XGSPON:
            pPonTod->ponMode = 2;

            ret = aal_xgpon_xgpn_glb_super_frame_match_val_get(0,&val);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            pPonTod->startPoint.xSuperFrame = (ca_uint64_t)val.sfc0 + (((ca_uint64_t)val.sfc1)<<32);
            break;

        default:
            return RT_ERR_INPUT;
    }

    ret = aal_ptp_load_timer_get(0,&sec,&nsec);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    pPonTod->timeStamp.sec = sec;
    pPonTod->timeStamp.nsec = nsec;

    return ret;
} /* end of dal_rtl8277c_rt_time_ponTodTime_get */

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTimeEnable_set
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
dal_rtl8277c_rt_time_ponTodTimeEnable_set(rt_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    aal_ptp_match_load_mask_t mask;
    aal_ptp_match_load_t load;

    memset(&load,0,sizeof(aal_ptp_match_load_t));

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    mask.u32 = 0;
    mask.s.pon_load = 1;

    load.pon_load = (enable == ENABLED)?1:0;

    ret = aal_ptp_match_load_set(0,mask,load);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl8277c_rt_time_ponTodTimeEnable_set */

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTimeEnable_get
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
dal_rtl8277c_rt_time_ponTodTimeEnable_get(rt_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    aal_ptp_match_load_t load;

    memset(&load,0,sizeof(aal_ptp_match_load_t));

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    ret = aal_ptp_match_load_get(0,&load);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pEnable = (load.pon_load ==1)?ENABLED:DISABLED;

    return ret;
} /* end of dal_rtl8277c_rt_time_ponTodTimeEnable_get */

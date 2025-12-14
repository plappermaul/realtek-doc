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
 * $Revision: 45446 $
 * $Date: 2013-12-19 10:51:19 +0800 (週四, 19 十二月 2013) $
 *
 * Purpose : Definition of Port Bandwidth Control and Storm Control API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration of Ingress Port Bandwidth Control [Ingress Rate Limit]
 *           (2) Configuration of Egress  Port Bandwidth Control [Egress  Rate Limit]
 *           (3) Configuration of Storm Control
 *           (3) Configuration of meter
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/rate.h>
#include <dal/rtl9602c/dal_rtl9602c_rate.h>

/*
 * Symbol Definition
 */
#define DAL_RTL9602C_STORM_MAX_ENTRY      7

/*
 * Data Declaration
 */
static uint32    rate_init = {INIT_NOT_COMPLETED};

/*
 * Function Declaration
 */

/*
 * Function Declaration
 */
static int32
_dal_rtl9602c_rate_stromCtrl_disable_set(void)
{
    int32   ret,i;
    uint32  port;
    uint32  regList[DAL_RTL9602C_STORM_MAX_ENTRY]={RTL9602C_STORM_CTRL_UM_CTRLr,
                                                   RTL9602C_STORM_CTRL_UC_CTRLr,
                                                   RTL9602C_STORM_CTRL_MC_CTRLr,
                                                   RTL9602C_STORM_CTRL_BC_CTRLr,
                                                   RTL9602C_STORM_CTRL_ARP_CTRLr,
                                                   RTL9602C_STORM_CTRL_DHCP_CTRLr,
                                                   RTL9602C_STORM_CTRL_IGMPMLD_CTRLr};
    uint32  data;

    data = DISABLED;
    for(i=0 ; i<DAL_RTL9602C_STORM_MAX_ENTRY ; i++)
    {
        HAL_SCAN_ALL_PORT(port)
        {
            if ((ret = reg_array_field_write(regList[i], port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_rtl9602c_rate_stormPortEnReg_get(rtk_rate_storm_group_t stormType, uint32 *reg)
{
    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            *reg = RTL9602C_STORM_CTRL_UM_CTRLr;
            break;

        case STORM_GROUP_UNKNOWN_UNICAST:
            *reg = RTL9602C_STORM_CTRL_UC_CTRLr;
            break;

        case STORM_GROUP_MULTICAST:
            *reg = RTL9602C_STORM_CTRL_MC_CTRLr;
            break;

        case STORM_GROUP_BROADCAST:
            *reg = RTL9602C_STORM_CTRL_BC_CTRLr;
            break;

        case STORM_GROUP_DHCP:
            *reg = RTL9602C_STORM_CTRL_DHCP_CTRLr;
            break;

        case STORM_GROUP_ARP:
            *reg = RTL9602C_STORM_CTRL_ARP_CTRLr;
            break;

        case STORM_GROUP_IGMP_MLD:
            *reg = RTL9602C_STORM_CTRL_IGMPMLD_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;

}

static int32
_dal_rtl9602c_rate_stormPortMeterReg_get(rtk_rate_storm_group_t stormType, uint32 *reg)
{
    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_MULTICAST:
            *reg = RTL9602C_STORM_CTRL_UM_METER_IDXr;
            break;

        case STORM_GROUP_UNKNOWN_UNICAST:
            *reg = RTL9602C_STORM_CTRL_UC_METER_IDXr;
            break;

        case STORM_GROUP_MULTICAST:
            *reg = RTL9602C_STORM_CTRL_MC_METER_IDXr;
            break;

        case STORM_GROUP_BROADCAST:
            *reg = RTL9602C_STORM_CTRL_BC_METER_IDXr;
            break;

        case STORM_GROUP_DHCP:
            *reg = RTL9602C_STORM_CTRL_DHCP_METER_IDXr;
            break;

        case STORM_GROUP_ARP:
            *reg = RTL9602C_STORM_CTRL_ARP_METER_IDXr;
            break;

        case STORM_GROUP_IGMP_MLD:
            *reg = RTL9602C_STORM_CTRL_IGMPMLD_METER_IDXr;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;

}

static int32
_dal_rtl9602c_rate_bandwidthCtrl_disable_set(void)
{
    int32   ret,queue;
    uint32  port;

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = dal_rtl9602c_rate_portIgrBandwidthCtrlRate_set(port,RTL9602C_INGRESS_RATE_MAX)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        if ((ret = dal_rtl9602c_rate_portEgrBandwidthCtrlRate_set(port,RTL9602C_EGRESS_RATE_MAX)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        if(HAL_IS_PON_PORT(port))
            continue;

        for(queue=0 ; queue<HAL_MAX_NUM_OF_QUEUE() ; queue++)
        {
            if ((ret = dal_rtl9602c_rate_egrQueueBwCtrlEnable_set(port,queue,DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_rtl9602c_rate_init
 * Description:
 *      Initial the rate module.
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9602c_rate_init(void)
{
    int32   ret;
    rtk_port_t port;
    rtk_enable_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /*diable all storm control*/
    if ((ret = _dal_rtl9602c_rate_stromCtrl_disable_set()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    rate_init = INIT_COMPLETED;
    /*disable bandwidth control*/
    if ((ret = _dal_rtl9602c_rate_bandwidthCtrl_disable_set()) != RT_ERR_OK)
    {
        rate_init = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    enable = ENABLED;
    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = reg_array_field_write(RTL9602C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_MODEf, &enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }

    }


    rate_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_init */

/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of ingress port bandwidth control */




/* Function Name:
 *      dal_rtl9602c_rate_portIgrBandwidthCtrlRate_get
 * Description:
 *      Get the ingress bandwidth control rate.
 * Input:
 *      port  - port id
 * Output:
 *      pRate - ingress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      (2) The granularity of rate is 8 kbps
 */
int32
dal_rtl9602c_rate_portIgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    int32   ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RATEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    *pRate = (data << 3);
    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_portIgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_rtl9602c_rate_portIgrBandwidthCtrlRate_set
 * Description:
 *      Set the ingress bandwidth control rate.
 * Input:
 *      port - port id
 *      rate - ingress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      (2) The granularity of rate is 8 kbps
 */
int32
dal_rtl9602c_rate_portIgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{
    int32   ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTL9602C_INGRESS_RATE_MAX < rate), RT_ERR_INPUT);

    data = (rate >> 3);
    if ((ret = reg_array_field_write(RTL9602C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RATEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_portIgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_rtl9602c_rate_portIgrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      port  - port id
 * Output:
 *      pIfgInclude - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9602c_rate_portIgrBandwidthCtrlIncludeIfg_get(rtk_port_t port, rtk_enable_t *pIfgInclude)
{

    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    if(0 == data)
        *pIfgInclude = DISABLED;
    else
        *pIfgInclude = ENABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_portIgrBandwidthCtrlIncludeIfg_get */

/* Function Name:
 *      dal_rtl9602c_rate_portIgrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      port  - port id
 *      ifgInclude - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifgInclude:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9602c_rate_portIgrBandwidthCtrlIncludeIfg_set(rtk_port_t port, rtk_enable_t ifgInclude)
{

    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    if(ENABLED==ifgInclude)
        data = 1;
    else
        data = 0;

    if ((ret = reg_array_field_write(RTL9602C_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_portIgrBandwidthCtrlIncludeIfg_set */



/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      dal_rtl9602c_rate_portEgrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      port  - port id
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      (2) The granularity of rate is 8 kbps
 */
int32
dal_rtl9602c_rate_portEgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9602C_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RATEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    *pRate = (data << 3);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_portEgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_rtl9602c_rate_portEgrBandwidthCtrlRate_set
 * Description:
 *      Set the egress bandwidth control rate.
 * Input:
 *      port - port id
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      (2) The granularity of rate is 8 kbps
 */
int32
dal_rtl9602c_rate_portEgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{

    int32   ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTL9602C_EGRESS_RATE_MAX < rate), RT_ERR_INPUT);

    data = (rate >> 3);
    if ((ret = reg_array_field_write(RTL9602C_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_RATEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    if(0 == port)
    {
        data = (rate >> 4);
        if ((ret = reg_field_write(RTL9602C_LINE_RATE_2500Mr, RTL9602C_RATEf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_portEgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_rtl9602c_rate_portEgrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Per port get the status of egress bandwidth control includes IFG or not.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9602c_rate_portEgrBandwidthCtrlIncludeIfg_get(rtk_port_t port,rtk_enable_t *pIfgInclude)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(RTL9602C_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    if(1 == data)
        *pIfgInclude = ENABLED;
    else
        *pIfgInclude = DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_portEgrBandwidthCtrlIncludeIfg_get */



/* Function Name:
 *      dal_rtl9602c_rate_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of egress bandwidth control includes IFG or not.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9602c_rate_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude)
{
    int32   ret;
    uint32  data;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    port = 0;
    if ((ret = reg_array_field_read(RTL9602C_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    if(1 == data)
        *pIfgInclude = ENABLED;
    else
        *pIfgInclude = DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_egrBandwidthCtrlIncludeIfg_get */


/* Function Name:
 *      rtk_rate_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Per port set the status of egress bandwidth control includes IFG or not.
 * Input:
 *      port    - port id
 *      ifgInclude - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9602c_rate_portEgrBandwidthCtrlIncludeIfg_set(rtk_port_t port,rtk_enable_t ifgInclude)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END  <= ifgInclude), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if(ENABLED==ifgInclude)
        data = 1;
    else
        data = 0;

    if ((ret = reg_array_field_write(RTL9602C_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_egrBandwidthCtrlIncludeIfg_set */



/* Function Name:
 *      dal_rtl9602c_rate_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of egress bandwidth control includes IFG or not.
 * Input:
 *      ifgInclude - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_rtl9602c_rate_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude)
{
    int32   ret;
    uint32  data;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END  <= ifgInclude), RT_ERR_INPUT);

    if(ENABLED==ifgInclude)
        data = 1;
    else
        data = 0;

    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = reg_array_field_write(RTL9602C_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, RTL9602C_IFGf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_egrBandwidthCtrlIncludeIfg_set */


/* Function Name:
 *      dal_rtl9602c_rate_egrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of egress bandwidth control on specified queue.
 * Input:
 *      port    - port id
 *      queue   - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_egrQueueBwCtrlEnable_get(
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_APR_EN_PORT_CFGr, port, queue, RTL9602C_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_egrQueueBwCtrlEnable_get */

/* Function Name:
 *      dal_rtl9602c_rate_egrQueueBwCtrlEnable_set
 * Description:
 *      Set enable status of egress bandwidth control on specified queue.
 * Input:
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of egress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_egrQueueBwCtrlEnable_set(
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);

    if ((ret = reg_array_field_write(RTL9602C_APR_EN_PORT_CFGr, port, queue, RTL9602C_ENf, (uint32 *)&enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_egrQueueBwCtrlEnable_set */

/* Function Name:
 *      dal_rtl9602c_rate_egrQueueBwCtrlMeterIdx_get
 * Description:
 *      Get rate of egress bandwidth control on specified queue.
 * Input:
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pMeterIndex - meter index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_rtl9602c_rate_egrQueueBwCtrlMeterIdx_get(
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pMeterIndex)
{
    int ret;
    uint32 offsetIdx;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pMeterIndex), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_APR_METER_PORT_CFGr, port, queue, RTL9602C_IDXf, &offsetIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }


    *pMeterIndex = offsetIdx + ((port&0x01)*8);

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_egrQueueBwCtrlMeterIdx_get */

/* Function Name:
 *      dal_rtl9602c_rate_egrQueueBwCtrlMeterIdx_set
 * Description:
 *      Set rate of egress bandwidth control on specified queue.
 * Input:
 *      port  - port id
 *      queue - queue id
 *      meterIndex  - meter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_rtl9602c_rate_egrQueueBwCtrlMeterIdx_set(
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      meterIndex)
{
    int ret;
    uint32 offsetIdx;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= meterIndex), RT_ERR_FILTER_METER_ID);

    if (meterIndex < ((port&01)*8) ||  meterIndex > (7 + (port&01)*8))
        return RT_ERR_FILTER_METER_ID;

    offsetIdx = meterIndex - ((port&01)*8);
    if ((ret = reg_array_field_write(RTL9602C_APR_METER_PORT_CFGr, port, queue, RTL9602C_IDXf, &offsetIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_egrQueueBwCtrlMeterIdx_set */

/* Module Name    : Rate                           */
/* Sub-module Name: Configuration of storm control */

/* Function Name:
 *      dal_rtl9602c_rate_stormControlRate_get
 * Description:
 *      Get the storm control meter index.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 * Output:
 *      pIndex     - storm control meter index.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 */
int32
dal_rtl9602c_rate_stormControlMeterIdx_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  *pIndex)
{
    int32   ret;
    uint32  data;
    uint32  reg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9602c_rate_stormPortMeterReg_get(stormType, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(reg, port, REG_ARRAY_INDEX_NONE, RTL9602C_IDXf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    *pIndex = data;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_stormControlMeterIdx_get */

/* Function Name:
 *      dal_rtl9602c_rate_stormControlMeterIdx_set
 * Description:
 *      Set the storm control meter index.
 * Input:
 *      port       - port id
 *      storm_type - storm group type
 *      index       - storm control meter index.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE    - Invalid input bandwidth
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 */
int32
dal_rtl9602c_rate_stormControlMeterIdx_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  index)
{
    int32   ret;
    uint32  data;
    uint32  reg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9602c_rate_stormPortMeterReg_get(stormType, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    data = index;

    if ((ret = reg_array_field_write(reg, port, REG_ARRAY_INDEX_NONE, RTL9602C_IDXf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_stormControlMeterIdx_set */

/* Function Name:
 *      dal_rtl9602c_rate_stormControlPortEnable_get
 * Description:
 *      Get enable status of storm control on specified port.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 * Output:
 *      pEnable    - pointer to enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 */
int32
dal_rtl9602c_rate_stormControlPortEnable_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            *pEnable)
{
    int32   ret;
    uint32  data;
    uint32  reg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = _dal_rtl9602c_rate_stormPortEnReg_get(stormType, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(reg, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(data)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_stormControlPortEnable_get */

/* Function Name:
 *      dal_rtl9602c_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 *      enable     - enable status of storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 */
int32
dal_rtl9602c_rate_stormControlPortEnable_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            enable)
{
    int32   ret;
    uint32  data;
    uint32  reg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((STORM_GROUP_END <= stormType), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if ((ret = _dal_rtl9602c_rate_stormPortEnReg_get(stormType, &reg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    data = (uint32)enable;

    if ((ret = reg_array_field_write(reg, port, REG_ARRAY_INDEX_NONE, RTL9602C_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_stormControlPortEnable_set */

static int32
_dal_rtl9602c_rate_rmaStormControlBypassEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);


    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
        switch(pRmaFrame->octet[5])
        {
            case 0xCC:
                regAddr = RTL9602C_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = RTL9602C_RMA_CTRL_SSTPr;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

        switch(pRmaFrame->octet[5])
        {
            case 0x00:
                regAddr = RTL9602C_RMA_CTRL00r;
                break;
            case 0x01:
                regAddr = RTL9602C_RMA_CTRL01r;
                break;
            case 0x02:
                regAddr = RTL9602C_RMA_CTRL02r;
                break;
            case 0x03:
                regAddr = RTL9602C_RMA_CTRL03r;
                break;
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0F:
                regAddr = RTL9602C_RMA_CTRL04r;
                break;
            case 0x08:
                regAddr = RTL9602C_RMA_CTRL08r;
                break;
            case 0x0D:
                regAddr = RTL9602C_RMA_CTRL0Dr;
                break;
            case 0x0E:
                regAddr = RTL9602C_RMA_CTRL0Er;
                break;
            case 0x10:
                regAddr = RTL9602C_RMA_CTRL10r;
                break;
            case 0x11:
                regAddr = RTL9602C_RMA_CTRL11r;
                break;
            case 0x12:
                regAddr = RTL9602C_RMA_CTRL12r;
                break;
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x19:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
                regAddr = RTL9602C_RMA_CTRL13r;
                break;
            case 0x18:
                regAddr = RTL9602C_RMA_CTRL18r;
                break;
            case 0x1A:
                regAddr = RTL9602C_RMA_CTRL1Ar;
                break;
            case 0x20:
                regAddr = RTL9602C_RMA_CTRL20r;
                break;
            case 0x21:
                regAddr = RTL9602C_RMA_CTRL21r;
                break;
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
                regAddr = RTL9602C_RMA_CTRL22r;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_read(regAddr, RTL9602C_DISCARD_STORM_FILTERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of _dal_rtl9602c_rate_rmaStormControlBypassEnable_get */



static int32
_dal_rtl9602c_rate_rmaStormControlBypassEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
        switch(pRmaFrame->octet[5])
        {
            case 0xCC:
                regAddr = RTL9602C_RMA_CTRL_CDPr;
                break;
            case 0xCD:
                regAddr = RTL9602C_RMA_CTRL_SSTPr;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

        switch(pRmaFrame->octet[5])
        {
            case 0x00:
                regAddr = RTL9602C_RMA_CTRL00r;
                break;
            case 0x01:
                regAddr = RTL9602C_RMA_CTRL01r;
                break;
            case 0x02:
                regAddr = RTL9602C_RMA_CTRL02r;
                break;
            case 0x03:
                regAddr = RTL9602C_RMA_CTRL03r;
                break;
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0F:
                regAddr = RTL9602C_RMA_CTRL04r;
                break;
            case 0x08:
                regAddr = RTL9602C_RMA_CTRL08r;
                break;
            case 0x0D:
                regAddr = RTL9602C_RMA_CTRL0Dr;
                break;
            case 0x0E:
                regAddr = RTL9602C_RMA_CTRL0Er;
                break;
            case 0x10:
                regAddr = RTL9602C_RMA_CTRL10r;
                break;
            case 0x11:
                regAddr = RTL9602C_RMA_CTRL11r;
                break;
            case 0x12:
                regAddr = RTL9602C_RMA_CTRL12r;
                break;
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x19:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
                regAddr = RTL9602C_RMA_CTRL13r;
                break;
            case 0x18:
                regAddr = RTL9602C_RMA_CTRL18r;
                break;
            case 0x1A:
                regAddr = RTL9602C_RMA_CTRL1Ar;
                break;
            case 0x20:
                regAddr = RTL9602C_RMA_CTRL20r;
                break;
            case 0x21:
                regAddr = RTL9602C_RMA_CTRL21r;
                break;
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
                regAddr = RTL9602C_RMA_CTRL22r;
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_write(regAddr, RTL9602C_DISCARD_STORM_FILTERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of _dal_rtl9602c_rate_rmaStormControlBypassEnable_set */

/* Function Name:
 *      dal_rtl9602c_rate_stormBypass_set
 * Description:
 *      Set bypass storm filter control configuration.
 * Input:
 *      type    - Bypass storm filter control type.
 *      enable  - Bypass status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid IFG parameter
 * Note:
 *
 *      This API can set per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 */
int32
dal_rtl9602c_rate_stormBypass_set(rtk_storm_bypass_t type, rtk_enable_t enable)
{
    int32   ret;
    uint32  rmaTail,data;
    rtk_mac_t rmaFrame;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((BYPASS_END <= type), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    if(BYPASS_IGMP == type)
    {
        data = enable;
        if ((ret = reg_field_write(RTL9602C_IGMP_GLB_CTRLr, RTL9602C_DISC_STORM_FILTERf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        if(BYPASS_CDP == type || BYPASS_CSSTP == type)
        {
            rmaFrame.octet[0] = 0x01;
            rmaFrame.octet[1] = 0x00;
            rmaFrame.octet[2] = 0x0C;
            rmaFrame.octet[3] = 0xCC;
            rmaFrame.octet[4] = 0xCC;

            if(BYPASS_CDP == type)
                rmaFrame.octet[5] = 0xCC;
            else
                rmaFrame.octet[5] = 0xCD;
        }
        else
        {
            rmaTail = (uint32)type;

            rmaFrame.octet[0] = 0x01;
            rmaFrame.octet[1] = 0x80;
            rmaFrame.octet[2] = 0xC2;
            rmaFrame.octet[3] = 0x00;
            rmaFrame.octet[4] = 0x00;
            rmaFrame.octet[5] = rmaTail;
        }

        if (( ret = _dal_rtl9602c_rate_rmaStormControlBypassEnable_set(&rmaFrame, enable)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_stormBypass_set */


/* Function Name:
 *      dal_rtl9602c_rate_stormBypass_get
 * Description:
 *      Get bypass storm filter control configuration.
 * Input:
 *      type - Bypass storm filter control type.
 * Output:
 *      pEnable - Bypass status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 */
int32
dal_rtl9602c_rate_stormBypass_get(rtk_storm_bypass_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  rmaTail,data;
    rtk_mac_t rmaFrame;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((BYPASS_END <= type), RT_ERR_NULL_POINTER);

    if(BYPASS_IGMP == type)
    {
        if ((ret = reg_field_read(RTL9602C_IGMP_GLB_CTRLr, RTL9602C_DISC_STORM_FILTERf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
        *pEnable = (rtk_enable_t)data;
    }
    else
    {
        if(BYPASS_CDP == type || BYPASS_CSSTP == type)
        {
            rmaFrame.octet[0] = 0x01;
            rmaFrame.octet[1] = 0x00;
            rmaFrame.octet[2] = 0x0C;
            rmaFrame.octet[3] = 0xCC;
            rmaFrame.octet[4] = 0xCC;

            if(BYPASS_CDP == type)
                rmaFrame.octet[5] = 0xCC;
            else
                rmaFrame.octet[5] = 0xCD;
        }
        else
        {
            rmaTail = (uint32)type;

            rmaFrame.octet[0] = 0x01;
            rmaFrame.octet[1] = 0x80;
            rmaFrame.octet[2] = 0xC2;
            rmaFrame.octet[3] = 0x00;
            rmaFrame.octet[4] = 0x00;
            rmaFrame.octet[5] = rmaTail;
        }

        if (( ret = _dal_rtl9602c_rate_rmaStormControlBypassEnable_get(&rmaFrame, pEnable)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_stormBypass_get */



/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of meter */

/* Function Name:
 *      dal_rtl9602c_rate_shareMeter_set
 * Description:
 *      Set meter configuration
 * Input:
 *      index       - shared meter index
 *      rate        - rate of share meter
 *      ifgInclude  - include IFG or not, ENABLE:include DISABLE:exclude
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *      The API can set shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      The granularity of rate is 8 kbps. The ifg_include parameter is used
 *      for rate calculation with/without inter-frame-gap and preamble.
 */
int32
dal_rtl9602c_rate_shareMeter_set(uint32 index, uint32 rate, rtk_enable_t ifgInclude)
{
    int32   ret;
    uint32  value, mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);


    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK(((RTL9602C_METER_RATE_MAX) < rate), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    /*set IFG*/
    if(ENABLED==ifgInclude)
        value = 1;
    else
        value = 0;
    if (( ret = reg_array_field_write(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_IFGf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    /*set rate*/
    if ((ret = reg_array_field_read(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_TYPEf, &mode)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    if(0==mode)/*bit rate*/
        value = rate>>3;
    else
        value = rate;

    if (( ret = reg_array_field_write(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_RATEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_shareMeter_set */


/* Function Name:
 *      dal_rtl9602c_rate_shareMeter_get
 * Description:
 *      Get meter configuration
 * Input:
 *      index        - shared meter index
 * Output:
 *      pRate        - pointer of rate of share meter
 *      pIfgInclude  - include IFG or not, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the granularity of rate is 8 kbps.
 *      The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble
 */
int32
dal_rtl9602c_rate_shareMeter_get(uint32 index, uint32 *pRate , rtk_enable_t *pIfgInclude)
{
    int32   ret;
    uint32  value, mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    /*get include IFG status*/
    if (( ret = reg_array_field_read(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_IFGf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    if(1 == value)
        *pIfgInclude = ENABLED;
    else
        *pIfgInclude = DISABLED;

    /*get rate setting*/
    if (( ret = reg_array_field_read(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_RATEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_TYPEf, &mode)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    if(0==mode)/*bit rate*/
        *pRate = value*8;
    else
        *pRate = value;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_shareMeter_get */

/* Function Name:
 *      dal_rtl9602c_rate_shareMeterBucket_set
 * Description:
 *      Set meter Bucket Size
 * Input:
 *      index        - shared meter index
 *      bucketSize   - Bucket Size
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter bucket size.
 */
int32
dal_rtl9602c_rate_shareMeterBucket_set(uint32 index, uint32 bucketSize)
{
    uint32  value;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9602C_METER_BUCKETSIZE_MAX < bucketSize), RT_ERR_INPUT);

    value = bucketSize;
    if ((ret = reg_array_field_write(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_BUCKET_SIZEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_shareMeterBucket_set */


/* Function Name:
 *      dal_rtl9602c_rate_shareMeterBucket_get
 * Description:
 *      Get meter Bucket Size
 * Input:
 *      index        - shared meter index
 * Output:
 *      pBucketSize - Bucket Size
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter bucket size.
 */
int32
dal_rtl9602c_rate_shareMeterBucket_get(uint32 index, uint32 *pBucketSize)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBucketSize), RT_ERR_NULL_POINTER);

    if (( ret = reg_array_field_read(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_BUCKET_SIZEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    *pBucketSize = value;

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_shareMeterBucket_get */



/* Function Name:
 *      dal_rtl9602c_rate_shareMeterExceed_get
 * Description:
 *      Get exceed meter status.
 * Input:
 *      index        - shared meter index
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      - TRUE      - rate is more than configured rate.
 *      - FALSE     - rate is not over then configured rate.
 */
int32
dal_rtl9602c_rate_shareMeterExceed_get(
    uint32                  index,
    uint32                  *pIsExceed)
{
    int32   ret;
    uint32  value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    if (( ret = reg_array_field_read(RTL9602C_METER_LB_EXCEED_STSr, REG_ARRAY_INDEX_NONE, index, RTL9602C_LB_EXCEEDf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    *pIsExceed = value;

    /*clear meter exceed status*/
    if(value)
    {
        value = 1;
        if ((ret = reg_array_field_write(RTL9602C_METER_LB_EXCEED_STSr, REG_ARRAY_INDEX_NONE, index, RTL9602C_LB_EXCEEDf, &value)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }
    }


    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_shareMeterExceed_get */

/* Function Name:
 *      dal_rtl9602c_rate_shareMeterExceed_clear
 * Description:
 *      Clear share meter exceed status.
 * Input:
 *      index        - shared meter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 */
int32 dal_rtl9602c_rate_shareMeterExceed_clear(uint32 index)
{
    int32   ret;
    uint32  value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);

    /*clear meter exceed status*/
    value = 1;
    if ((ret = reg_array_field_write(RTL9602C_METER_LB_EXCEED_STSr, REG_ARRAY_INDEX_NONE, index, RTL9602C_LB_EXCEEDf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_rate_shareMeterExceed_clear */

/* Function Name:
 *      dal_rtl9602c_rate_shareMeterMode_set
 * Description:
 *      Set meter mode
 * Input:
 *      index     - shared meter index
 *      meterMode - meter mode(bit rate mode or packet rate mode)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter type.
 */
int32
dal_rtl9602c_rate_shareMeterMode_set(uint32 index, rtk_rate_metet_mode_t meterMode)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "index=%d,meterMode=%d",index, meterMode);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((METER_MODE_END <=meterMode), RT_ERR_INPUT);

    /* function body */
    if(METER_MODE_BIT_RATE == meterMode)
        value = 0;
    else
        value = 1;

    if ((ret = reg_array_field_write(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_TYPEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_rate_shareMeterMode_set */

/* Function Name:
 *      dal_rtl9602c_rate_shareMeterMode_get
 * Description:
 *      Set meter mode
 * Input:
 *      index     - shared meter index
 *      pMeterMode     - meter mode(bit rate mode or packet rate mode)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter mode.
 */
int32
dal_rtl9602c_rate_shareMeterMode_get(uint32 index, rtk_rate_metet_mode_t *pMeterMode)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMeterMode), RT_ERR_NULL_POINTER);

    /* function body */
    if ((ret = reg_array_field_read(RTL9602C_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_TYPEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    if(0==value)
        *pMeterMode = METER_MODE_BIT_RATE;
    else
        *pMeterMode = METER_MODE_PACKET_RATE;

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_rate_shareMeterMode_get */

/* Function Name:
 *      dal_rtl9602c_rate_hostIgrBwCtrlState_set
 * Description:
 *      Set state of host ingress bandwidth control
 * Input:
 *      index   - host index
 *      state   - Ingress bandwidth control state.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostIgrBwCtrlState_set(
    uint32 index,
    rtk_enable_t    state)
{
    int32   ret;
    uint32  value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    value = (uint32)state;

    if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_POLICE_SA_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_rate_hostIgrBwCtrlState_set */

/* Function Name:
 *      dal_rtl9602c_rate_hostIgrBwCtrlState_get
 * Description:
 *      Get enable status of host ingress bandwidth control
 * Input:
 *      index   - host index
 * Output:
 *      pState  - Ingress bandwidth control state.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostIgrBwCtrlState_get(
    uint32 index,
    rtk_enable_t    *pState)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);
    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_POLICE_SA_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(value)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;

} /* end of rtk_rate_hostIgrBwCtrlState_get */

/* Function Name:
 *      dal_rtl9602c_rate_hostEgrBwCtrlState_set
 * Description:
 *      Set state of host egress bandwidth control
 * Input:
 *      index   - host index
 *      state   - Egress bandwidth control state.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostEgrBwCtrlState_set(
    uint32 index,
    rtk_enable_t    state)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    value = (uint32)state;

    if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_POLICE_DA_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_rate_hostEgrBwCtrlState_set */

/* Function Name:
 *      dal_rtl9602c_rate_hostEgrBwCtrlState_get
 * Description:
 *      Get enable status of host egress bandwidth control
 * Input:
 *      index   - host index
 * Output:
 *      pState  - Egress bandwidth control state.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT            - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostEgrBwCtrlState_get(
    uint32 index,
    rtk_enable_t    *pState)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_POLICE_DA_ENf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(value)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;

} /* end of dal_rtl9602c_rate_hostEgrBwCtrlState_get */

/* Function Name:
 *      dal_rtl9602c_rate_hostBwCtrlMeterIdx_set
 * Description:
 *      Set shared meter of host bandwith control.
 * Input:
 *      index   - host index
 *      meterIndex - meter index
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostBwCtrlMeterIdx_set(
    uint32 index,
    uint32 meterIndex)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= meterIndex), RT_ERR_INPUT);

    value = (uint32)meterIndex;

    if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_POLICE_METERf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9602c_rate_hostEgrBwCtrlState_set */

/* Function Name:
 *      dal_rtl9602c_rate_hostBwCtrlMeterIdx_get
 * Description:
 *      Get shared meter of host bandwith control.
 * Input:
 *      index   - host index
 * Output:
 *      pMeterIndex - meter index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostBwCtrlMeterIdx_get(
    uint32 index,
    uint32 *pMeterIndex)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMeterIndex), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_POLICE_METERf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    *pMeterIndex = value;

    return RT_ERR_OK;

} /* end of dal_rtl9602c_rate_hostEgrBwCtrlState_get */


/* Function Name:
 *      dal_rtl9602c_rate_hostMacAddr_set
 * Description:
 *      Set MAC address of host bandwidth control.
 * Input:
 *      index   - host index
 *      pMac - MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostMacAddr_set(
    uint32 index,
    rtk_mac_t *pMac)
{
    int32   ret;
    uint32 i, tmp[6];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if((pMac->octet[0] & BITMASK_1B) == 1)
        return RT_ERR_INPUT;

    for (i=0;i<ETHER_ADDR_LEN;i++)
        tmp[i] = pMac->octet[i];


    if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_array_field_write(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     return RT_ERR_OK;

} /* end of dal_rtl9602c_rate_hostMacAddr_set */


/* Function Name:
 *       dal_rtl9602c_rate_hostMacAddr_get
 * Description:
 *      Get MAC address of host bandwidth control.
 * Input:
 *      index   - host index
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *      None
 */
int32
dal_rtl9602c_rate_hostMacAddr_get(
    uint32 index,
    rtk_mac_t *pMac)
{
    int32   ret;
    uint32 i, tmp[6];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_HOST() <=index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC5f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC4f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC3f, &tmp[2])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC2f, &tmp[3])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC1f, &tmp[4])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
     if ((ret = reg_array_field_read(RTL9602C_HOST_POLICE_CTRLr, REG_ARRAY_INDEX_NONE, index, RTL9602C_MAC0f, &tmp[5])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }

    for (i=0;i<ETHER_ADDR_LEN;i++)
        pMac->octet[i] = tmp[i];

    return RT_ERR_OK;

} /* end of dal_rtl9602c_rate_hostMacAddr_get */



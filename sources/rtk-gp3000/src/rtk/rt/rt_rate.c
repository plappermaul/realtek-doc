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
 * $Revision: 68395 $
 * $Date: 2016-05-27 16:38:35 +0800 (Fri, 27 May 2016) $
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
#include <rtk/rt/rt_port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_rate.h>

#ifdef CONFIG_EXTERNAL_SWITCH
#include <dal/dal_ext_switch_mapper.h>
#endif

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#include <rt/include/rt_ext_mapper.h>
#endif

/*
 * Symbol Definition
 */
static uint32 rate_init = INIT_NOT_COMPLETED;

/*
 * Data Declaration
 */
typedef struct rt_rateMeterStatus_s
{
    rt_enable_t state;
    rt_rate_meter_type_t type;
} rt_rateMeterStatus_t;

rt_rateMeterStatus_t rateL2MeterState[RT_RATE_HW_IDX_L2_METER_MAX_NUM];
rt_rateMeterStatus_t rateL3MeterState[RT_RATE_HW_IDX_L3_METER_MAX_NUM];
rt_rateMeterStatus_t rateSWMeterState[RT_RATE_HW_IDX_SW_METER_MAX_NUM];

typedef struct rt_rateVIndexMap_s
{
    rt_enable_t state;
    rt_rate_meter_mapping_t mapping;
} rt_rateVIndexMap_t;

rt_rateVIndexMap_t rateVIndexMap[RT_RATE_V_IDX_MAX_NUM];

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
static rt_ext_mapper_t* RT_EXT_MAPPER = NULL;
#endif

/*
 * Function Declaration
 */

static int32 _rt_rate_find_L2EmptyHwIndex(uint32* hw_index,uint32 s_idx,uint32 idx_num)
{
    uint32 hw_idx;

    *hw_index = -1;

    //for(hw_idx=0;hw_idx<RT_RATE_HW_IDX_L2_METER_MAX_NUM;hw_idx++)
    for(hw_idx=s_idx;hw_idx<(s_idx+idx_num);hw_idx++)
    {
        if(rateL2MeterState[hw_idx].state == DISABLED)
        {
            break;
        }
    }
    if(hw_idx>=(s_idx+idx_num))
    {
        return RT_ERR_FILTER_METER_ID;
    }

    *hw_index = hw_idx;

    return RT_ERR_OK;
}

static int32 _rt_rate_occupy_L2EmptyHwIndex(uint32 hw_index,rt_rate_meter_type_t type)
{
    rateL2MeterState[hw_index].state = ENABLED;
    rateL2MeterState[hw_index].type = type;

    return RT_ERR_OK;
}

static int32 _rt_rate_open_L2EmptyHwIndex(uint32 hw_index)
{
    rateL2MeterState[hw_index].state = DISABLED;
    rateL2MeterState[hw_index].type = RT_METER_TYPE_END;

    return RT_ERR_OK;
}

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
static int32 _rt_rate_find_L3EmptyHwIndex_flow(uint32* hw_index)
{
    uint32 hw_idx;

    *hw_index = -1;

    for(hw_idx=0;hw_idx<32;hw_idx++) /*flow occupy first 32 entries*/
    {
        if(rateL3MeterState[hw_idx].state == DISABLED)
        {
            break;
        }
    }
    if(hw_idx>=32) /*flow occupy first 32 entries*/
    {
        return RT_ERR_FILTER_METER_ID;
    }

    *hw_index = hw_idx;

    return RT_ERR_OK;
}

static int32 _rt_rate_find_L3EmptyHwIndex(uint32* hw_index)
{
    int32   ret = RT_ERR_OK;
    uint32 hw_idx;
    rt_internal_meterSize_t meterSize;

    *hw_index = -1;

    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeterSize_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    memset(&meterSize,0,sizeof(rt_internal_meterSize_t));
    ret = RT_EXT_MAPPER->internal_l3SwMeterSize_get(&meterSize);
    if(ret != RT_ERR_OK)
    {
        return ret;
    }

    if(meterSize.l3_meter_size>RT_RATE_HW_IDX_L3_METER_MAX_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    for(hw_idx=32;hw_idx<meterSize.l3_meter_size;hw_idx++) /*flow occupy first 32 entries*/
    {
        if(rateL3MeterState[hw_idx].state == DISABLED)
        {
            break;
        }
    }
    if(hw_idx>=meterSize.l3_meter_size)
    {
        return RT_ERR_FILTER_METER_ID;
    }

    *hw_index = hw_idx;

    return RT_ERR_OK;
}

static int32 _rt_rate_occupy_L3EmptyHwIndex(uint32 hw_index,rt_rate_meter_type_t type)
{
    rateL3MeterState[hw_index].state = ENABLED;
    rateL3MeterState[hw_index].type = type;

    return RT_ERR_OK;
}

static int32 _rt_rate_open_L3EmptyHwIndex(uint32 hw_index)
{
    rateL3MeterState[hw_index].state = DISABLED;
    rateL3MeterState[hw_index].type = RT_METER_TYPE_END;

    return RT_ERR_OK;
}

static int32 _rt_rate_find_SWEmptyHwIndex(uint32* hw_index)
{
    int32   ret = RT_ERR_OK;
    uint32 hw_idx;
    rt_internal_meterSize_t meterSize;

    *hw_index = -1;

    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeterSize_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    memset(&meterSize,0,sizeof(rt_internal_meterSize_t));
    ret = RT_EXT_MAPPER->internal_l3SwMeterSize_get(&meterSize);
    if(ret != RT_ERR_OK)
    {
        return ret;
    }

    if(meterSize.sw_meter_size>RT_RATE_HW_IDX_SW_METER_MAX_NUM)
    {
        return RT_ERR_OUT_OF_RANGE;
    }

    for(hw_idx=0;hw_idx<meterSize.sw_meter_size;hw_idx++)
    {
        if(rateSWMeterState[hw_idx].state == DISABLED)
        {
            break;
        }
    }
    if(hw_idx>=meterSize.sw_meter_size)
    {
        return RT_ERR_FILTER_METER_ID;
    }

    *hw_index = hw_idx;

    return RT_ERR_OK;
}

static int32 _rt_rate_occupy_SWEmptyHwIndex(uint32 hw_index,rt_rate_meter_type_t type)
{
    rateSWMeterState[hw_index].state = ENABLED;
    rateSWMeterState[hw_index].type = type;

    return RT_ERR_OK;
}

static int32 _rt_rate_open_SWEmptyHwIndex(uint32 hw_index)
{
    rateSWMeterState[hw_index].state = DISABLED;
    rateSWMeterState[hw_index].type = RT_METER_TYPE_END;

    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      rt_rate_init
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
rt_rate_init(void)
{
    int32   ret;
    uint32 idx;

    /* function body */
    if (NULL == RT_MAPPER->rate_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rate_init();

    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return ret;
    }
    RTK_API_UNLOCK();

    if(rate_init != INIT_COMPLETED)
    {

        for(idx=0;idx<RT_RATE_HW_IDX_L2_METER_MAX_NUM;idx++)
        {
            rateL2MeterState[idx].state = DISABLED;
            rateL2MeterState[idx].type = RT_METER_TYPE_END;
        }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
        for(idx=0;idx<RT_RATE_HW_IDX_L3_METER_MAX_NUM;idx++)
        {
            rateL3MeterState[idx].state = DISABLED;
            rateL3MeterState[idx].type = RT_METER_TYPE_END;
        }

        for(idx=0;idx<RT_RATE_HW_IDX_SW_METER_MAX_NUM;idx++)
        {
            rateSWMeterState[idx].state = DISABLED;
            rateSWMeterState[idx].type = RT_METER_TYPE_END;
        }
#endif
        for(idx=0;idx<RT_RATE_V_IDX_MAX_NUM;idx++)
        {
            rateVIndexMap[idx].state = DISABLED;
            rateVIndexMap[idx].mapping.type = RT_METER_TYPE_END;
            rateVIndexMap[idx].mapping.hw_type = RT_METER_HW_TYPE_END;
            rateVIndexMap[idx].mapping.hw_index = -1;
            rateVIndexMap[idx].mapping.hw_index_ext = -1;
        }
    }

    rate_init = INIT_COMPLETED;

    return ret;
}   /* end of rt_rate_init */

/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of ingress port bandwidth control */

/* Function Name:
 *      rt_rate_portIgrBandwidthCtrlRate_get
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
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in apollo is 8Kbps.
 */
int32
rt_rate_portIgrBandwidthCtrlRate_get(rt_port_t port, uint32 *pRate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rate_portIgrBandwidthCtrlRate_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rate_portIgrBandwidthCtrlRate_get(port, pRate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_portIgrBandwidthCtrlRate_get */

/* Function Name:
 *      rt_rate_portIgrBandwidthCtrlRate_set
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
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in apollo is 8Kbps.
 */
int32
rt_rate_portIgrBandwidthCtrlRate_set(rt_port_t port, uint32 rate)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rate_portIgrBandwidthCtrlRate_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(rate == RT_MAX_RATE)
        rate = RT_RATE_HW_IGR_RATE_MAX;

    RTK_API_LOCK();
    ret = RT_MAPPER->rate_portIgrBandwidthCtrlRate_set(port, rate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_portIgrBandwidthCtrlRate_set */


/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      rt_rate_portEgrBandwidthCtrlRate_get
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
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32
rt_rate_portEgrBandwidthCtrlRate_get(rt_port_t port, uint32 *pRate)
{
    int32   ret;
    
#if defined(CONFIG_EXTERNAL_SWITCH)
    if(IS_EXT_SWITCH_PORT(port))
    {
        /* function body */
        if (NULL == RT_EXT_SWITCH_MAPPER || NULL == RT_EXT_SWITCH_MAPPER->rate_portEgrBandwidthCtrlRate_get)
            return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_EXT_SWITCH_MAPPER->rate_portEgrBandwidthCtrlRate_get(port, pRate);
        RTK_API_UNLOCK();
        return ret;
    }
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_portEgrBandwidthCtrlRate_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rate_portEgrBandwidthCtrlRate_get(port, pRate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_portEgrBandwidthCtrlRate_get */

/* Function Name:
 *      rt_rate_portEgrBandwidthCtrlRate_set
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
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32
rt_rate_portEgrBandwidthCtrlRate_set(rt_port_t port, uint32 rate)
{
    int32   ret;

#if defined(CONFIG_EXTERNAL_SWITCH)
    if(IS_EXT_SWITCH_PORT(port))
    {
        /* function body */
        if (NULL == RT_EXT_SWITCH_MAPPER || NULL == RT_EXT_SWITCH_MAPPER->rate_portEgrBandwidthCtrlRate_set)
            return RT_ERR_DRIVER_NOT_FOUND;
        RTK_API_LOCK();
        ret = RT_EXT_SWITCH_MAPPER->rate_portEgrBandwidthCtrlRate_set(port, rate);
        RTK_API_UNLOCK();
        return ret;
    }
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_portEgrBandwidthCtrlRate_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(rate == RT_MAX_RATE)
        rate = RT_RATE_HW_EGR_RATE_MAX;

    RTK_API_LOCK();
    ret = RT_MAPPER->rate_portEgrBandwidthCtrlRate_set(port, rate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_portEgrBandwidthCtrlRate_set */

/* Module Name    : Rate                           */
/* Sub-module Name: Configuration of storm control */


/* Function Name:
 *      rt_rate_stormControlRate_get
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
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND
 */
int32
rt_rate_stormControlMeterIdx_get(
    rt_port_t              port,
    rt_rate_storm_group_t  stormType,
    uint32                 *pIndex)
{
    int32   ret=RT_ERR_OK;
    rt_rate_meter_mapping_t meterMap;
    uint32 v_idx;

    /* function body */
    if (NULL == RT_MAPPER->rate_stormControlMeterIdx_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();

    ret = RT_MAPPER->rate_stormControlMeterIdx_get(port, stormType, pIndex);
    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return ret;
    }

    meterMap.type = RT_METER_TYPE_STORM;
    meterMap.hw_index = *pIndex;

    ret = rt_rate_shareMeterMappingVirtual_get(meterMap,&v_idx);
    if(ret != RT_ERR_OK)
    {
        *pIndex = -1;
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    *pIndex = v_idx;
    RTK_API_UNLOCK();

    return ret;
}   /* end of rt_rate_stormControlMeterIdx_get */

/* Function Name:
 *      rt_rate_stormControlMeterIdx_set
 * Description:
 *      Set the storm control meter index.
 * Input:
 *      port       - port id
 *      storm_type - storm group type
 *      index      - storm control meter index.
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
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND
 */
int32
rt_rate_stormControlMeterIdx_set(
    rt_port_t              port,
    rt_rate_storm_group_t  stormType,
    uint32                 index)
{
    int32   ret=RT_ERR_OK;
    rt_rate_meter_mapping_t meterMap;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_LUNA_G3_SERIES)
    rt_enable_t ori_enable;
    uint32 ori_index;
#endif
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_stormControlMeterIdx_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_LUNA_G3_SERIES)
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }
#endif
#endif

    RTK_API_LOCK();
    ret = rt_rate_shareMeterMappingHw_get(index,&meterMap);
    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return ret;
    }

    if(meterMap.type != RT_METER_TYPE_STORM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
    if(stormType == STORM_GROUP_UNKNOWN_UNICAST)
    {
        ret = RT_EXT_MAPPER->internal_g3_l3fe_unknownDAStormCtrl_get(port,&ori_enable,&ori_index);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_EXT_MAPPER->internal_g3_l3fe_unknownDAStormCtrl_set(port,ori_enable,meterMap.hw_index_ext);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#endif
#endif
    ret = RT_MAPPER->rate_stormControlMeterIdx_set(port, stormType, meterMap.hw_index);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_stormControlMeterIdx_set */

/* Function Name:
 *      rt_rate_stormControlEnable_get
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
 *    - When global strom gruop for given strom type is disabled,
 *      API will return DISABLED
 */
int32
rt_rate_stormControlPortEnable_get(
    rt_port_t              port,
    rt_rate_storm_group_t  stormType,
    rt_enable_t            *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rate_stormControlPortEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rate_stormControlPortEnable_get(port, stormType, pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_stormControlPortEnable_get */

/* Function Name:
 *      rt_rate_stormControlPortEnable_set
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
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND
 */
int32
rt_rate_stormControlPortEnable_set(
    rt_port_t              port,
    rt_rate_storm_group_t  stormType,
    rt_enable_t            enable)
{
    int32   ret;
    uint32 v_idx = -1;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_LUNA_G3_SERIES)
    rt_rate_meter_mapping_t meterMap;
    rt_enable_t ori_enable;
    uint32 ori_index;
    rt_internal_meterConfig_t meterConf;
#endif
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_stormControlPortEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_LUNA_G3_SERIES)
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }
#endif
#endif

    RTK_API_LOCK();

    ret = rt_rate_stormControlMeterIdx_get(port,stormType,&v_idx);
    if(ret != RT_ERR_OK)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
    if(stormType == STORM_GROUP_UNKNOWN_UNICAST)
    {
        ret = RT_EXT_MAPPER->internal_g3_l3fe_unknownDAStormCtrl_get(port,&ori_enable,&ori_index);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        /*checking original index is correct*/
        ret=rt_rate_shareMeterMappingHw_get(v_idx,&meterMap);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }
        if(meterMap.hw_index_ext != ori_index)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }

        ret = RT_EXT_MAPPER->internal_g3_l3fe_unknownDAStormCtrl_set(port,enable,ori_index);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#endif
#endif
    ret = RT_MAPPER->rate_stormControlPortEnable_set(port, stormType, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_stormControlPortEnable_set */

/* Function Name:
 *      rt_rate_shareMeterType_add
 * Description:
 *      Add share meter type configuration
 * Input:
 *      type        - shared meter type

 * Output:
 *      index       - shared meter index
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *
 */
int32
rt_rate_shareMeterType_add (
    rt_rate_meter_type_t type,
    uint32* index)
{
    int32 ret = RT_ERR_OK;
    uint32 v_idx = -1;
    rt_rate_meter_hw_type_t hw_type = RT_METER_HW_TYPE_END;
    uint32 hw_idx = -1;
    uint32 hw_idx_ext = -1;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
    memset(&meterConf, 0, sizeof(rt_internal_meterConfig_t));
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    RTK_API_LOCK();

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(type == RT_METER_TYPE_FLOW)
    {
        if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
        {
            /* Search Empty Hw index*/
            ret = _rt_rate_find_L3EmptyHwIndex_flow(&hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
        else
        {
            ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add(RT_METER_TYPE_FLOW, &hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }

        /*Init Meter*/
        meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
        meterConf.ifgInclude = ENABLED;
        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
        {
            ret = _rt_rate_occupy_L3EmptyHwIndex(hw_idx,type);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }

        /*flow occupy first 32 entries and virtual index is one-one mapping*/
        v_idx = hw_idx;

        hw_type = RT_METER_HW_TYPE_L3;
    }
    else
    {
        for(v_idx=32;v_idx<RT_RATE_V_IDX_MAX_NUM;v_idx++) /*flow occupy first 32 entries*/
        {
            if(rateVIndexMap[v_idx].state == DISABLED)
            {
                break;
            }
        }
        if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }
        if(type == RT_METER_TYPE_STORM)
        {
            /* Search Empty Hw index*/
            ret = _rt_rate_find_L2EmptyHwIndex(&hw_idx,0,RT_RATE_L2_ALLOC_NUM);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            /*Init Meter*/
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = _rt_rate_occupy_L2EmptyHwIndex(hw_idx,type);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            hw_type = RT_METER_HW_TYPE_L2;

#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
            /* Search Empty Hw index*/
            ret = _rt_rate_find_L3EmptyHwIndex(&hw_idx_ext);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
            /*Init Meter*/
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
            meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
            meterConf.ifgInclude = ENABLED;
            ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_ACL,hw_idx_ext,meterConf);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = _rt_rate_occupy_L3EmptyHwIndex(hw_idx_ext,RT_METER_TYPE_ACL);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
#endif
        }
        else if(type == RT_METER_TYPE_HOST)
        {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
            /* Search Empty Hw index 128~223(96 entries=32 rate limit+32 tx+32 rx) for host policer*/
            ret = _rt_rate_find_L2EmptyHwIndex(&hw_idx,128,32);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            /*Init Meter*/
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = _rt_rate_occupy_L2EmptyHwIndex(hw_idx,type);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            hw_type = RT_METER_HW_TYPE_L2;
#else
            /* Search Empty Hw index*/
            if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
            {
                ret = _rt_rate_find_L3EmptyHwIndex(&hw_idx);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }
            else
            {
                ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add(RT_METER_TYPE_HOST, &hw_idx);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }
            /*Init Meter*/
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
            meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
            meterConf.ifgInclude = ENABLED;
            ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
            {
                ret = _rt_rate_occupy_L3EmptyHwIndex(hw_idx,type);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }

            hw_type = RT_METER_HW_TYPE_L3;
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
            /* Search Empty Hw index*/
            ret = _rt_rate_find_L2EmptyHwIndex(&hw_idx,0,RT_RATE_L2_ALLOC_NUM);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            /*Init Meter*/
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = _rt_rate_occupy_L2EmptyHwIndex(hw_idx,type);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            hw_type = RT_METER_HW_TYPE_L2;
#endif
        }
        else if (type == RT_METER_TYPE_ACL)
        {
#if defined(CONFIG_LUNA_G3_SERIES)
            /* Search Empty Hw index*/
            if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
            {
                ret = _rt_rate_find_L3EmptyHwIndex(&hw_idx);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }
            else
            {
                ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add(RT_METER_TYPE_ACL, &hw_idx);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }
            /*Init Meter*/
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
            meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
            meterConf.ifgInclude = ENABLED;
            ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
            {
                ret = _rt_rate_occupy_L3EmptyHwIndex(hw_idx,type);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }

            hw_type = RT_METER_HW_TYPE_L3;

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
            /* Search Empty Hw index*/
            ret = _rt_rate_find_L2EmptyHwIndex(&hw_idx,0,RT_RATE_L2_ALLOC_NUM);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            /*Init Meter*/
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            ret = _rt_rate_occupy_L2EmptyHwIndex(hw_idx,type);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            hw_type = RT_METER_HW_TYPE_L2;
#endif
        }
        else if (type == RT_METER_TYPE_SW)
        {
            /* Search Empty Hw index*/
            if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
            {
                ret = _rt_rate_find_SWEmptyHwIndex(&hw_idx);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }
            else
            {
                ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add(RT_METER_TYPE_SW, &hw_idx);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }

            /*Init Meter*/
            meterConf.rate = RT_RATE_HW_IDX_SW_METER_RATE_MAX;
            meterConf.bucket_size = RT_RATE_HW_IDX_SW_METER_BURST_MAX;
            meterConf.ifgInclude = ENABLED;
            ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }

            if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_add)
            {
                ret = _rt_rate_occupy_SWEmptyHwIndex(hw_idx,type);
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }
            }

            hw_type = RT_METER_HW_TYPE_SW;
        }
        else
        {
            RTK_API_UNLOCK();
            return RT_ERR_INPUT;
        }
    }
#else
    if(type == RT_METER_TYPE_STORM)
    {
        for(v_idx=32;v_idx<RT_RATE_V_IDX_MAX_NUM;v_idx++) /*flow occupy first 32 entries*/
        {
            if(rateVIndexMap[v_idx].state == DISABLED)
            {
                break;
            }
        }
        if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }
        /* Search Empty Hw index*/
        ret = _rt_rate_find_L2EmptyHwIndex(&hw_idx,0,RT_RATE_L2_ALLOC_NUM);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        /*Init Meter*/
        ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = _rt_rate_occupy_L2EmptyHwIndex(hw_idx,type);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        hw_type = RT_METER_HW_TYPE_L2;
    }
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }
#endif

    if(v_idx == -1 || v_idx >= RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }
    else
    {
        rateVIndexMap[v_idx].state = ENABLED;
        rateVIndexMap[v_idx].mapping.type = type;
        rateVIndexMap[v_idx].mapping.hw_type = hw_type;
        rateVIndexMap[v_idx].mapping.hw_index = hw_idx;
        rateVIndexMap[v_idx].mapping.hw_index_ext = hw_idx_ext;
    }

    *index = v_idx;

    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_shareMeterType_add */

/* Function Name:
 *      rt_rate_shareMeterType_del
 * Description:
 *      Delete share meter type configuration
 * Input:
 *      index       - shared meter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *
 */
int32
rt_rate_shareMeterType_del (
    uint32 index)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    uint32 hw_idx_ext = -1;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
    uint8 ifUsed = FALSE;
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeterUsedState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx >= RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;
    hw_idx_ext = rateVIndexMap[v_idx].mapping.hw_index_ext;

    if(type == RT_METER_TYPE_STORM)
    {
        uint32  port;
        rt_rate_storm_group_t gId;
        rt_enable_t enable;
        uint32  storm_index;

        /*Checking virtual index has been locked by hardware*/
        HAL_SCAN_ALL_PORT(port)
        {
            for(gId=0;gId<RT_STORM_GROUP_END;gId++)
            {
                ret = rt_rate_stormControlPortEnable_get(port,gId,&enable);
                if(ret == RT_ERR_CHIP_NOT_SUPPORTED)
                    continue;
                if(ret != RT_ERR_OK)
                {
                    RTK_API_UNLOCK();
                    return ret;
                }

                if(enable == ENABLED)
                {
                    ret = rt_rate_stormControlMeterIdx_get(port,gId,&storm_index);
                    if(ret != RT_ERR_OK)
                    {
                        RTK_API_UNLOCK();
                        return ret;
                    }

                    if(v_idx == storm_index)
                    {
                        RTK_API_UNLOCK();
                        return RT_ERR_FILTER_METER_ID;
                    }
                }

            }
        }

        /*Init Meter*/
        ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,RT_METER_MODE_BIT_RATE);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = _rt_rate_open_L2EmptyHwIndex(hw_idx);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
        /*internal_l3SwMeterUsedState_get(v_idx,&ifUsed) no use to check*/
        /*Init Meter*/
        meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
        meterConf.ifgInclude = ENABLED;
        meterConf.mode = RT_METER_MODE_BIT_RATE;
        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_ACL, hw_idx_ext,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = _rt_rate_open_L3EmptyHwIndex(hw_idx_ext);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
#endif
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
/*How to check host policer is used when meter is deleted*/
        /*Init Meter*/
        ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,RT_METER_MODE_BIT_RATE);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = _rt_rate_open_L2EmptyHwIndex(hw_idx);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeterUsedState_get(v_idx,&ifUsed);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }

        if(ifUsed == TRUE)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }
        /*Init Meter*/
        meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
        meterConf.ifgInclude = ENABLED;
        meterConf.mode = RT_METER_MODE_BIT_RATE;
        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type, hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
        if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del)
        {
            ret = _rt_rate_open_L3EmptyHwIndex(hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
        else
        {
            ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del(RT_METER_TYPE_HOST, hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_EXT_MAPPER->internal_l3SwMeterUsedState_get(v_idx,&ifUsed);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }

        if(ifUsed == TRUE)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }
        /*Init Meter*/
        ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,RT_METER_MODE_BIT_RATE);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = _rt_rate_open_L2EmptyHwIndex(hw_idx);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeterUsedState_get(v_idx,&ifUsed);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }

        if(ifUsed == TRUE)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }
#if defined(CONFIG_LUNA_G3_SERIES)
        /*Init Meter*/
        meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
        meterConf.ifgInclude = ENABLED;
        meterConf.mode = RT_METER_MODE_BIT_RATE;
        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type, hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del)
        {
            ret = _rt_rate_open_L3EmptyHwIndex(hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
        else
        {
            ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del(RT_METER_TYPE_ACL, hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        /*Init Meter*/
        ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,ENABLED);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,RT_RATE_HW_IDX_L2_METER_BURST_MAX);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,RT_METER_MODE_BIT_RATE);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        ret = _rt_rate_open_L2EmptyHwIndex(hw_idx);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeterUsedState_get(v_idx,&ifUsed);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }

        if(ifUsed == TRUE)
        {
            RTK_API_UNLOCK();
            return RT_ERR_FILTER_METER_ID;
        }
        /*Init Meter*/
        meterConf.rate = RT_RATE_HW_IDX_SW_METER_RATE_MAX;
        meterConf.bucket_size = RT_RATE_HW_IDX_SW_METER_BURST_MAX;
        meterConf.ifgInclude = ENABLED;
        meterConf.mode = RT_METER_MODE_BIT_RATE;
        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
        if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del)
        {
            ret = _rt_rate_open_SWEmptyHwIndex(hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
        else
        {
            ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del(RT_METER_TYPE_SW, hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        /*Init Meter*/
        meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        meterConf.bucket_size = RT_RATE_HW_IDX_L3_METER_BURST_MAX;
        meterConf.ifgInclude = ENABLED;
        meterConf.mode = RT_METER_MODE_BIT_RATE;
        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(NULL == RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del)
        {
            ret = _rt_rate_open_L3EmptyHwIndex(hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
        else
        {
            ret = RT_EXT_MAPPER->internal_l3SwMeterEmptyHwIdx_del(RT_METER_TYPE_FLOW, hw_idx);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
        RTK_API_UNLOCK();
        return ret; /*flow not delete realationship of virtual index and hardware index*/
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }

    if(v_idx == -1 || v_idx >= RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }
    else
    {
        rateVIndexMap[v_idx].state = DISABLED;
        rateVIndexMap[v_idx].mapping.type = RT_METER_TYPE_END;
        rateVIndexMap[v_idx].mapping.hw_type = RT_METER_HW_TYPE_END;
        rateVIndexMap[v_idx].mapping.hw_index = -1;
        rateVIndexMap[v_idx].mapping.hw_index_ext = -1;
    }

    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_shareMeterType_del */

/* Function Name:
 *      rt_rate_shareMeterType_get
 * Description:
 *      Get share meter type configuration
 * Input:
 *      index       - shared meter index

 * Output:
 *      pType       - shared meter type
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *
 */
int32
rt_rate_shareMeterType_get (
    uint32 index,
    rt_rate_meter_type_t *pType)
{
    int32 ret=RT_ERR_OK;
    uint32 v_idx = -1;

    /* function body */
    v_idx = index;

    RTK_API_LOCK();
    if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    *pType = rateVIndexMap[v_idx].mapping.type;

    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_shareMeterType_get */

/* Function Name:
 *      rt_rate_shareMeterRate_set
 * Description:
 *      Set meter rate configuration
 * Input:
 *      index       - shared meter index
 *      rate        - rate of share meter
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
rt_rate_shareMeterRate_set (
    uint32 index,
    uint32 rate)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    uint32 hw_idx_ext = -1;
    uint32 oriRate = 0;
    rt_enable_t oriIfgInclude = DISABLED;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
    memset(&meterConf, 0, sizeof(rt_internal_meterConfig_t));
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->rate_shareMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;
    hw_idx_ext = rateVIndexMap[v_idx].mapping.hw_index_ext;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,oriIfgInclude);
        else
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,rate,oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(RT_METER_TYPE_ACL,hw_idx_ext,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        else
            meterConf.rate = rate;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_ACL,hw_idx_ext,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
#endif
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,oriIfgInclude);
        else
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,rate,oriIfgInclude);

        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        else
            meterConf.rate = rate;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,oriIfgInclude);
        else
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,rate,oriIfgInclude);

        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        else
            meterConf.rate = rate;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,RT_RATE_HW_IDX_L2_METER_RATE_MAX,oriIfgInclude);
        else
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,rate,oriIfgInclude);

        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        else
            meterConf.rate = rate;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(rate == RT_MAX_RATE)
            meterConf.rate = RT_RATE_HW_IDX_L3_METER_RATE_MAX;
        else
            meterConf.rate = rate;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_FLOW,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }

    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_shareMeterRate_set */

/* Function Name:
 *      rt_rate_shareMeterRate_get
 * Description:
 *      Get meter configuration
 * Input:
 *      index        - shared meter index
 * Output:
 *      pRate        - pointer of rate of share meter
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
rt_rate_shareMeterRate_get (
    uint32  index,
    uint32* pRate)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    rt_enable_t oriIfgInclude = DISABLED;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx >= RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,pRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,pRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pRate = meterConf.rate;
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,pRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pRate = meterConf.rate;
#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,pRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pRate = meterConf.rate;
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pRate = meterConf.rate;
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }

    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_shareMeterRate_get */

/* Function Name:
 *      rt_rate_shareMeterMappingHw_get
 * Description:
 *      Get share meter hw index mapping configuration
 * Input:
 *      v_index     - shared meter index

 * Output:
 *      pMeterMap   - shared meter mapping
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *
 */
int32
rt_rate_shareMeterMappingHw_get (
    uint32 v_index,
    rt_rate_meter_mapping_t* pMeterMap)
{
    int32 ret = RT_ERR_OK;
    uint32 v_idx = -1;

    /* function body */
    v_idx = v_index;

    RTK_API_LOCK();
    if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    pMeterMap->type = rateVIndexMap[v_idx].mapping.type;
    pMeterMap->hw_type = rateVIndexMap[v_idx].mapping.hw_type;
    pMeterMap->hw_index = rateVIndexMap[v_idx].mapping.hw_index;
    pMeterMap->hw_index_ext = rateVIndexMap[v_idx].mapping.hw_index_ext;

    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_rate_shareMeterMappingHw_get */

/* Function Name:
 *      rt_rate_shareMeterMappingVirtual_get
 * Description:
 *      Get share meter virtual index mapping configuration
 * Input:
 *      pMeterMap   - shared meter mapping

 * Output:
 *      pV_index    - shared meter index
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *
 */
int32
rt_rate_shareMeterMappingVirtual_get (
    rt_rate_meter_mapping_t meterMap,
    uint32* pV_index)
{
    uint32 v_idx = -1;

    *pV_index = -1;

    /* function body */
    RTK_API_LOCK();
    for(v_idx=0;v_idx<RT_RATE_V_IDX_MAX_NUM;v_idx++)
    {
        if(rateVIndexMap[v_idx].state == ENABLED)
        {
            if((rateVIndexMap[v_idx].mapping.type == meterMap.type) && (rateVIndexMap[v_idx].mapping.hw_index == meterMap.hw_index))
            {
                break;
            }
        }
    }
    if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    *pV_index = v_idx;

    RTK_API_UNLOCK();
    return RT_ERR_OK;
}   /* end of rt_rate_shareMeterMappingVirtual_get */

/* Function Name:
 *      rt_rate_shareMeterBucket_set
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
rt_rate_shareMeterBucket_set(uint32 index, uint32 bucketSize)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    uint32 hw_idx_ext = -1;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
    memset(&meterConf, 0, sizeof(rt_internal_meterConfig_t));
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeterBucket_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;
    hw_idx_ext = rateVIndexMap[v_idx].mapping.hw_index_ext;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,bucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(RT_METER_TYPE_ACL,hw_idx_ext,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.bucket_size = bucketSize;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_ACL,hw_idx_ext,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
#endif
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,bucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.bucket_size = bucketSize;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,bucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.bucket_size = bucketSize;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterBucket_set(hw_idx,bucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.bucket_size = bucketSize;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.bucket_size = bucketSize;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_FLOW,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }

    RTK_API_UNLOCK();
    return ret;
} /* end of rt_rate_shareMeterBucket_set */

/* Function Name:
 *      rt_rate_shareMeterBucket_get
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
rt_rate_shareMeterBucket_get(uint32 index, uint32 *pBucketSize)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeterBucket_get)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx >= RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeterBucket_get(hw_idx,pBucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeterBucket_get(hw_idx,pBucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pBucketSize = meterConf.bucket_size;
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterBucket_get(hw_idx,pBucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pBucketSize = meterConf.bucket_size;
#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterBucket_get(hw_idx,pBucketSize);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pBucketSize = meterConf.bucket_size;
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pBucketSize = meterConf.bucket_size;
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }

    RTK_API_UNLOCK();
    return ret;
} /* end of rt_rate_shareMeterBucket_get */

/* Function Name:
 *      rt_rate_shareMeterIfgInclude_set
 * Description:
 *      Set meter IFG include state
 * Input:
 *      index        - shared meter index
 *      ifgInclude   - ifg include state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter IFG include state.
 */
extern int32 
rt_rate_shareMeterIfgInclude_set(uint32 index, rtk_enable_t ifgInclude)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    uint32 hw_idx_ext = -1;
    uint32 oriRate = 0;
    rt_enable_t oriIfgInclude = DISABLED;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
    memset(&meterConf, 0, sizeof(rt_internal_meterConfig_t));
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->rate_shareMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;
    hw_idx_ext = rateVIndexMap[v_idx].mapping.hw_index_ext;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(oriIfgInclude != ifgInclude)
        {
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,oriRate,ifgInclude);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(RT_METER_TYPE_ACL,hw_idx_ext,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.ifgInclude = ifgInclude;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_ACL,hw_idx_ext,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
#endif
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(oriIfgInclude != ifgInclude)
        {
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,oriRate,ifgInclude);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.ifgInclude = ifgInclude;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(oriIfgInclude != ifgInclude)
        {
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,oriRate,ifgInclude);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.ifgInclude = ifgInclude;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,&oriIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(oriIfgInclude != ifgInclude)
        {
            ret = RT_MAPPER->rate_shareMeter_set(hw_idx,oriRate,ifgInclude);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.ifgInclude = ifgInclude;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.ifgInclude = ifgInclude;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_FLOW,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }


    RTK_API_UNLOCK();
    return ret;
} /* end of rt_rate_shareMeterIfgInclude_set */

/* Function Name:
 *      rt_rate_shareMeterIfgInclude_get
 * Description:
 *      Get meter IFG include state
 * Input:
 *      index        - shared meter index
 * Output:
 *      pIfgInclude  - ifg include state
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter IFG include state.
 */
extern int32 
rt_rate_shareMeterIfgInclude_get(uint32 index, rtk_enable_t* pIfgInclude)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    uint32 oriRate = 0;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx >= RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,pIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,pIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pIfgInclude = meterConf.ifgInclude;
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,pIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pIfgInclude = meterConf.ifgInclude;

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeter_get(hw_idx,&oriRate,pIfgInclude);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pIfgInclude = meterConf.ifgInclude;
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pIfgInclude = meterConf.ifgInclude;
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }

    RTK_API_UNLOCK();
    return ret;
} /* end of rt_rate_shareMeterIfgInclude_get */

/* Function Name:
 *      rt_rate_shareMeterMode_set
 * Description:
 *      Set meter update mode
 * Input:
 *      index        - shared meter index
 *      mode         - meter mode
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter mode.
 */
extern int32 
rt_rate_shareMeterMode_set(uint32 index, rt_rate_meter_mode_t mode)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    uint32 hw_idx_ext = -1;
    rtk_rate_metet_mode_t tmpMode = METER_MODE_BIT_RATE;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeterMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_MAPPER->rate_shareMeterMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_set)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx>=RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;
    hw_idx_ext = rateVIndexMap[v_idx].mapping.hw_index_ext;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(tmpMode != mode)
        {
            tmpMode = mode;
            ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,tmpMode);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
#if defined(CONFIG_SDK_CA8279) || defined(CONFIG_SDK_RTL8198F)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(RT_METER_TYPE_ACL,hw_idx_ext,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.mode = mode;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_ACL,hw_idx_ext,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif
#endif
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(tmpMode != mode)
        {
            tmpMode = mode;
            ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,tmpMode);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.mode = mode;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(tmpMode != mode)
        {
            tmpMode = mode;
            ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,tmpMode);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.mode = mode;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        if(tmpMode != mode)
        {
            tmpMode = mode;
            ret = RT_MAPPER->rate_shareMeterMode_set(hw_idx,tmpMode);
            if(ret != RT_ERR_OK)
            {
                RTK_API_UNLOCK();
                return ret;
            }
        }
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.mode = mode;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(type,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        meterConf.mode = mode;

        ret = RT_EXT_MAPPER->internal_l3SwMeter_set(RT_METER_TYPE_FLOW,hw_idx,meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }


    RTK_API_UNLOCK();
    return ret;
} /* end of rt_rate_shareMeterMode_set */

/* Function Name:
 *      rt_rate_shareMeterMode_get
 * Description:
 *      Get meter update mode
 * Input:
 *      index        - shared meter index
 * Output:
 *      pMode        - meter mode
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter mode.
 */
extern int32 
rt_rate_shareMeterMode_get(uint32 index, rt_rate_meter_mode_t* pMode)
{
    int32 ret = RT_ERR_OK;
    rt_rate_meter_type_t type = RT_METER_TYPE_END;
    uint32 v_idx = -1;
    uint32 hw_idx = -1;
    rtk_rate_metet_mode_t tmpMode;
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    rt_internal_meterConfig_t meterConf;
    memset(&meterConf, 0, sizeof(rt_internal_meterConfig_t));
#endif

    /* function body */
    if (NULL == RT_MAPPER->rate_shareMeterMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    if(NULL == RT_EXT_MAPPER)
    {
        RT_EXT_MAPPER = rt_ext_mapper_get();
        if(RT_EXT_MAPPER == NULL)
            return RT_ERR_DRIVER_NOT_FOUND;
    }

    if (NULL == RT_EXT_MAPPER->internal_l3SwMeter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
#endif

    v_idx = index;

    RTK_API_LOCK();
    if(v_idx >= RT_RATE_V_IDX_MAX_NUM)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    if(rateVIndexMap[v_idx].state == DISABLED)
    {
        RTK_API_UNLOCK();
        return RT_ERR_FILTER_METER_ID;
    }

    type = rateVIndexMap[v_idx].mapping.type;
    hw_idx = rateVIndexMap[v_idx].mapping.hw_index;

    if(type == RT_METER_TYPE_STORM)
    {
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
        *pMode = tmpMode;
    }
#ifdef CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
    else if(type == RT_METER_TYPE_HOST)
    {
#if defined(CONFIG_LUNA_G3_SERIES)

#if defined(CONFIG_SDK_CA8279) && defined(CONFIG_RG_G3_L2FE_POL_OFFLOAD)
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
        *pMode = tmpMode;
#else
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pMode = meterConf.mode;
#endif

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
        *pMode = tmpMode;
#endif
    }
    else if(type == RT_METER_TYPE_ACL)
    {
#if defined(CONFIG_LUNA_G3_SERIES)
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pMode = meterConf.mode;

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        ret = RT_MAPPER->rate_shareMeterMode_get(hw_idx,&tmpMode);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }
        *pMode = tmpMode;
#endif
    }
    else if(type == RT_METER_TYPE_SW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pMode = meterConf.mode;
    }
    else if(type == RT_METER_TYPE_FLOW)
    {
        ret = RT_EXT_MAPPER->internal_l3SwMeter_get(type,hw_idx,&meterConf);
        if(ret != RT_ERR_OK)
        {
            RTK_API_UNLOCK();
            return ret;
        }

        *pMode = meterConf.mode;
    }
#endif
    else
    {
        RTK_API_UNLOCK();
        return RT_ERR_INPUT;
    }

    RTK_API_UNLOCK();
    return ret;
} /* end of rt_rate_shareMeterMode_get */
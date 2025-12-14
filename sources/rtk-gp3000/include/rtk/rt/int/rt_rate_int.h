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

#ifndef __RT_RATE_INT_H__
#define __RT_RATE_INT_H__


/*
 * Symbol Definition
 */

#if defined(CONFIG_SDK_CA8279)
#include <hal/chipdef/ca8279/ca8279_def.h>
#define RT_RATE_HW_IDX_L2_METER_MAX_NUM 256
#define RT_RATE_HW_IDX_L2_METER_RATE_MAX CA8279_METER_RATE_MAX
#define RT_RATE_HW_IDX_L2_METER_BURST_MAX CA8279_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_L3_METER_MAX_NUM 512
#define RT_RATE_HW_IDX_L3_METER_RATE_MAX CA8279_METER_RATE_MAX
#define RT_RATE_HW_IDX_L3_METER_BURST_MAX CA8279_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_SW_METER_MAX_NUM 128
#define RT_RATE_HW_IDX_SW_METER_RATE_MAX CA8279_METER_RATE_MAX
#define RT_RATE_HW_IDX_SW_METER_BURST_MAX 0

//224~255(32 entries) if dor ddos
#ifdef CONFIG_RG_G3_L2FE_POL_OFFLOAD
#define RT_RATE_HW_IDX_L2_RSV_FOR_DDOS 32
#define RT_RATE_HW_IDX_L2_RSV_FOR_HOST 96 //CONFIG_RG_G3_L2FE_POL_OFFLOAD 128~223(96 entries=32 rate limit+32 tx+32 rx) for host policer
#define RT_RATE_L2_ALLOC_NUM (RT_RATE_HW_IDX_L2_METER_MAX_NUM-RT_RATE_HW_IDX_L2_RSV_FOR_DDOS-RT_RATE_HW_IDX_L2_RSV_FOR_HOST)
#else
#define RT_RATE_HW_IDX_L2_RSV_FOR_DDOS 32
#define RT_RATE_L2_ALLOC_NUM (RT_RATE_HW_IDX_L2_METER_MAX_NUM-RT_RATE_HW_IDX_L2_RSV_FOR_DDOS)
#endif

#define RT_RATE_HW_IGR_RATE_MAX CA8279_INGRESS_RATE_MAX
#define RT_RATE_HW_EGR_RATE_MAX CA8279_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_RATE_MAX CA8279_PON_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_PIR_CIR_RATE_MAX CA8279_PON_PIR_CIR_RATE_MAX

#elif defined(CONFIG_SDK_CA8277B)
#include <hal/chipdef/ca8277b/ca8277b_def.h>
#define RT_RATE_HW_IDX_L2_METER_MAX_NUM 256
#define RT_RATE_HW_IDX_L2_METER_RATE_MAX CA8277B_METER_RATE_MAX
#define RT_RATE_HW_IDX_L2_METER_BURST_MAX CA8277B_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_L3_METER_MAX_NUM 512
#define RT_RATE_HW_IDX_L3_METER_RATE_MAX CA8277B_METER_RATE_MAX
#define RT_RATE_HW_IDX_L3_METER_BURST_MAX CA8277B_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_SW_METER_MAX_NUM 128
#define RT_RATE_HW_IDX_SW_METER_RATE_MAX CA8277B_METER_RATE_MAX
#define RT_RATE_HW_IDX_SW_METER_BURST_MAX 0
//224~255(32 entries) if dor ddos
#define RT_RATE_HW_IDX_L2_RSV_FOR_DDOS 32
#define RT_RATE_L2_ALLOC_NUM (RT_RATE_HW_IDX_L2_METER_MAX_NUM-RT_RATE_HW_IDX_L2_RSV_FOR_DDOS)

#define RT_RATE_HW_IGR_RATE_MAX CA8277B_INGRESS_RATE_MAX
#define RT_RATE_HW_EGR_RATE_MAX CA8277B_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_RATE_MAX CA8277B_PON_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_PIR_CIR_RATE_MAX CA8277B_PON_PIR_CIR_RATE_MAX

#elif defined(CONFIG_SDK_RTL8198F)
#include <hal/chipdef/rtl8198f/rtl8198f_def.h>
#define RT_RATE_HW_IDX_L2_METER_MAX_NUM 256
#define RT_RATE_HW_IDX_L2_METER_RATE_MAX RTL8198F_METER_RATE_MAX
#define RT_RATE_HW_IDX_L2_METER_BURST_MAX RTL8198F_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_L3_METER_MAX_NUM 128
#define RT_RATE_HW_IDX_L3_METER_RATE_MAX RTL8198F_METER_RATE_MAX
#define RT_RATE_HW_IDX_L3_METER_BURST_MAX RTL8198F_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_SW_METER_MAX_NUM 128
#define RT_RATE_HW_IDX_SW_METER_RATE_MAX RTL8198F_METER_RATE_MAX
#define RT_RATE_HW_IDX_SW_METER_BURST_MAX 0
//224~255(32 entries) if dor ddos
#define RT_RATE_HW_IDX_L2_RSV_FOR_DDOS 32
#define RT_RATE_L2_ALLOC_NUM (RT_RATE_HW_IDX_L2_METER_MAX_NUM-RT_RATE_HW_IDX_L2_RSV_FOR_DDOS)

#define RT_RATE_HW_IGR_RATE_MAX RTL8198F_INGRESS_RATE_MAX
#define RT_RATE_HW_EGR_RATE_MAX RTL8198F_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_RATE_MAX RTL8198F_PON_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_PIR_CIR_RATE_MAX RTL8198F_PON_PIR_CIR_RATE_MAX

#elif defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
#include <hal/chipdef/rtl9607c/rtl9607c_def.h>
#define RT_RATE_HW_IDX_L2_METER_MAX_NUM 48
#define RT_RATE_HW_IDX_L2_METER_RATE_MAX RTL9607C_METER_RATE_MAX
#define RT_RATE_HW_IDX_L2_METER_BURST_MAX RTL9607C_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_L3_METER_MAX_NUM 32
#define RT_RATE_HW_IDX_L3_METER_RATE_MAX RTL9607C_METER_RATE_MAX
#define RT_RATE_HW_IDX_L3_METER_BURST_MAX RTL9607C_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_SW_METER_MAX_NUM 128
#define RT_RATE_HW_IDX_SW_METER_RATE_MAX RTL9607C_METER_RATE_MAX
#define RT_RATE_HW_IDX_SW_METER_BURST_MAX 0

#define RT_RATE_L2_ALLOC_NUM RT_RATE_HW_IDX_L2_METER_MAX_NUM

#define RT_RATE_HW_IGR_RATE_MAX RTL9607C_INGRESS_RATE_MAX
#define RT_RATE_HW_EGR_RATE_MAX RTL9607C_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_RATE_MAX RTL9607C_PON_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_PIR_CIR_RATE_MAX RTL9607C_PON_PIR_CIR_RATE_MAX

#elif defined(CONFIG_SDK_RTL8277C)
#include <hal/chipdef/rtl8277c/rtl8277c_def.h>
#define RT_RATE_HW_IDX_L2_METER_MAX_NUM 23
#define RT_RATE_HW_IDX_L2_METER_RATE_MAX RTL8277C_METER_RATE_MAX
#define RT_RATE_HW_IDX_L2_METER_BURST_MAX RTL8277C_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_L3_METER_MAX_NUM 256
#define RT_RATE_HW_IDX_L3_METER_RATE_MAX RTL8277C_METER_RATE_MAX
#define RT_RATE_HW_IDX_L3_METER_BURST_MAX RTL8277C_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_SW_METER_MAX_NUM 128
#define RT_RATE_HW_IDX_SW_METER_RATE_MAX RTL8277C_METER_RATE_MAX
#define RT_RATE_HW_IDX_SW_METER_BURST_MAX 0
//224~255(32 entries) if dor ddos
#define RT_RATE_HW_IDX_L2_RSV_FOR_DDOS 0
#define RT_RATE_L2_ALLOC_NUM (RT_RATE_HW_IDX_L2_METER_MAX_NUM-RT_RATE_HW_IDX_L2_RSV_FOR_DDOS)

#define RT_RATE_HW_IGR_RATE_MAX RTL8277C_INGRESS_RATE_MAX
#define RT_RATE_HW_EGR_RATE_MAX RTL8277C_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_RATE_MAX RTL8277C_PON_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_PIR_CIR_RATE_MAX RTL8277C_PON_PIR_CIR_RATE_MAX

#elif defined(CONFIG_SDK_RTL9607F)
#include <hal/chipdef/rtl9607f/rtl9607f_def.h>
#define RT_RATE_HW_IDX_L2_METER_MAX_NUM 23
#define RT_RATE_HW_IDX_L2_METER_RATE_MAX RTL9607F_METER_RATE_MAX
#define RT_RATE_HW_IDX_L2_METER_BURST_MAX RTL9607F_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_L3_METER_MAX_NUM 256
#define RT_RATE_HW_IDX_L3_METER_RATE_MAX RTL9607F_METER_RATE_MAX
#define RT_RATE_HW_IDX_L3_METER_BURST_MAX RTL9607F_METER_BUCKETSIZE_MAX
#define RT_RATE_HW_IDX_SW_METER_MAX_NUM 128
#define RT_RATE_HW_IDX_SW_METER_RATE_MAX RTL9607F_METER_RATE_MAX
#define RT_RATE_HW_IDX_SW_METER_BURST_MAX 0
//224~255(32 entries) if dor ddos
#define RT_RATE_HW_IDX_L2_RSV_FOR_DDOS 0
#define RT_RATE_L2_ALLOC_NUM (RT_RATE_HW_IDX_L2_METER_MAX_NUM-RT_RATE_HW_IDX_L2_RSV_FOR_DDOS)

#define RT_RATE_HW_IGR_RATE_MAX RTL9607F_INGRESS_RATE_MAX
#define RT_RATE_HW_EGR_RATE_MAX RTL9607F_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_RATE_MAX RTL9607F_PON_EGRESS_RATE_MAX
#define RT_RATE_PON_EGR_PIR_CIR_RATE_MAX RTL9607F_PON_PIR_CIR_RATE_MAX
#endif

#define RT_RATE_V_IDX_MAX_NUM RT_RATE_HW_IDX_L2_METER_MAX_NUM + RT_RATE_HW_IDX_L3_METER_MAX_NUM + RT_RATE_HW_IDX_SW_METER_MAX_NUM

#define RT_MAX_RATE 0xFFFFFFFF

/*
 * Data Declaration
 */

typedef enum rt_rate_meter_hw_type_e
{
    RT_METER_HW_TYPE_L2,
    RT_METER_HW_TYPE_L3,
    RT_METER_HW_TYPE_SW,
    RT_METER_HW_TYPE_END
} rt_rate_meter_hw_type_t;

typedef struct rt_rate_meter_mapping_s
{
    uint16 type; //reference rt_rate_meter_type_t
    rt_rate_meter_hw_type_t hw_type;
    uint32 hw_index;
    uint32 hw_index_ext;
} rt_rate_meter_mapping_t;

/*
 * Function Declaration
 */

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
extern int32
rt_rate_shareMeterMappingHw_get (
    uint32 v_index,
    rt_rate_meter_mapping_t* pMeterMap);

/* Function Name:
 *      rt_rate_shareMeterMappingVirtual_get
 * Description:
 *      Get share meter virtual index mapping configuration
 * Input:
 *      pMeterMap   - shared meter mapping

 * Output:
 *      pSw_index   - shared meter index
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *
 */
extern int32
rt_rate_shareMeterMappingVirtual_get (
    rt_rate_meter_mapping_t meterMap,
    uint32* pV_index);

#endif

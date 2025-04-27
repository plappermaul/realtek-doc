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
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/switch.h>
#include <hwp/hw_profile.h>
#include <osal/print.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

#ifndef PHY_ONLY
/* Function Name:
 *      rtk_switch_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize switch module before calling any switch APIs.
 * Changes:
 *      None
 */
int32
rtk_switch_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_init(unit);
} /* end of rtk_switch_init */
#endif


#ifndef PHY_ONLY
/* Function Name:
 *      rtk_switch_deviceCapability_print
 * Description:
 *      Print device information of the specific unit for parsing the variable to testing script.
 * Input:
 *      unit     - unit id
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The routine have provided following information based on SDK hw_profile
 *      - chip id, chip revision id, chip capacity
 *      - total port number and cpu port id
 *      - detail information in each port type
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_deviceCapability_print(uint32 unit)
{
    hal_control_t *pHal_control;
    uint32 port;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    if ((pHal_control = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }

    /*HW_profile */

    osal_printf("\nCHIP_ID                       %s\n", (HWP_CHIP_FAMILY_ID(unit)==RTL8390_FAMILY_ID)?"RTL8390_FAMILY_ID":\
                                                        (HWP_CHIP_FAMILY_ID(unit)==RTL8350_FAMILY_ID)?"RTL8390_FAMILY_ID":\
                                                        (HWP_CHIP_FAMILY_ID(unit)==RTL8380_FAMILY_ID)?"RTL8380_FAMILY_ID":\
                                                        (HWP_CHIP_FAMILY_ID(unit)==RTL8330_FAMILY_ID)?"RTL8380_FAMILY_ID":\
                                                        (HWP_CHIP_FAMILY_ID(unit)==RTL9310_FAMILY_ID)?"RTL9310_FAMILY_ID":\
                                                        (HWP_CHIP_FAMILY_ID(unit)==RTL9300_FAMILY_ID)?"RTL9300_FAMILY_ID":"NULL");
    osal_printf("HWP_IDENTIFIER_NAME               %s\n", HWP_HWPROFILE(unit)->identifier.name);
    osal_printf("HWP_IDENTIFIER_ID                 %d\n", HWP_HWPROFILE(unit)->identifier.id);

    osal_printf("HWP_UNIT_VALID_LOCAL              %d\n", HWP_UNIT_VALID_LOCAL(unit));
    osal_printf("HWP_UNIT_VALID_STACK              %d\n", HWP_UNIT_VALID_STACK(unit));
    //osal_printf("HWP_CPU_EMBEDDED                  %d\n", HWP_CPU_EMBEDDED());
    osal_printf("HWP_IS_CPU_UNIT                   %d\n", HWP_IS_CPU_UNIT(unit));
    //osal_printf("HWP_MY_UNIT_ID                    %u\n", HWP_MY_UNIT_ID());
    osal_printf("HWP_UNIT_VALID_LOCAL              %d\n", HWP_UNIT_VALID_LOCAL(unit));
    osal_printf("HWP_PORT_COUNT                    %d\n", HWP_PORT_COUNT(unit) );

    osal_printf("HAL_GET_MAX_BANDWIDTH_OF_PORT                       %u\n", HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, 0));
    osal_printf("HAL_MAX_NUM_OF_MIRROR                               %u\n", HAL_MAX_NUM_OF_MIRROR(unit));
    osal_printf("HAL_MAX_NUM_OF_TRUNK                                %u\n", HAL_MAX_NUM_OF_TRUNK(unit));
    osal_printf("HAL_MAX_NUM_OF_TRUNK_IN_STACKING_MODE               %u\n", HAL_MAX_NUM_OF_TRUNK_IN_STACKING_MODE(unit));
    osal_printf("HAL_MAX_NUM_OF_STACK_TRUNK                          %u\n", HAL_MAX_NUM_OF_STACK_TRUNK(unit));
    osal_printf("HAL_MAX_NUM_OF_LOCAL_TRUNK                          %u\n", HAL_MAX_NUM_OF_LOCAL_TRUNK(unit));
    osal_printf("HAL_MAX_NUM_OF_TRUNKMEMBER                          %u\n", HAL_MAX_NUM_OF_TRUNKMEMBER(unit));
    osal_printf("HAL_MAX_NUM_OF_TRUNK_ALGO                           %u\n", HAL_MAX_NUM_OF_TRUNK_ALGO(unit));
    osal_printf("HAL_TRUNK_ALGO_SHIFT_MAX                            %u\n", HAL_TRUNK_ALGO_SHIFT_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_DUMB_TRUNKMEMBER                     %u\n", HAL_MAX_NUM_OF_DUMB_TRUNKMEMBER(unit));
    osal_printf("HAL_MAX_NUM_OF_TRUNKHASHVAL                         %u\n", HAL_MAX_NUM_OF_TRUNKHASHVAL(unit));
    osal_printf("HAL_MAX_NUM_OF_MSTI                                 %u\n", HAL_MAX_NUM_OF_MSTI(unit));
    osal_printf("HAL_MAX_NUM_OF_SVL                                  %u\n", HAL_MAX_NUM_OF_SVL(unit));
    osal_printf("HAL_MAX_NUM_OF_METERING                             %u\n", HAL_MAX_NUM_OF_METERING(unit));
    osal_printf("HAL_MAX_NUM_OF_METER_BLOCK                          %u\n", HAL_MAX_NUM_OF_METER_BLOCK(unit));
    osal_printf("HAL_RATE_OF_METER_MAX                               %u\n", HAL_RATE_OF_METER_MAX(unit));
    osal_printf("HAL_BURST_SIZE_OF_ACL_METER_MIN                     %u\n", HAL_BURST_SIZE_OF_ACL_METER_MIN(unit));
    osal_printf("HAL_BURST_SIZE_OF_ACL_METER_MAX                     %u\n", HAL_BURST_SIZE_OF_ACL_METER_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_BLOCK                            %u\n", HAL_MAX_NUM_OF_PIE_BLOCK(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_BLOCKSIZE                        %u\n", HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_COUNTER                          %u\n", HAL_MAX_NUM_OF_PIE_COUNTER(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_TEMPLATE                         %u\n", HAL_MAX_NUM_OF_PIE_TEMPLATE(unit));
    osal_printf("HAL_PIE_USER_TEMPLATE_ID_MIN                        %u\n", HAL_PIE_USER_TEMPLATE_ID_MIN(unit));
    osal_printf("HAL_PIE_USER_TEMPLATE_ID_MAX                        %u\n", HAL_PIE_USER_TEMPLATE_ID_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_FIELD_SELECTOR                       %u\n", HAL_MAX_NUM_OF_FIELD_SELECTOR(unit));
    osal_printf("HAL_MAX_OFST_OF_FIELD_SELECTOR                      %u\n", HAL_MAX_OFST_OF_FIELD_SELECTOR(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_FILTER_ID                        %u\n", HAL_MAX_NUM_OF_PIE_FILTER_ID(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_BLOCK_TEMPLATESELECTOR           %u\n", HAL_MAX_NUM_OF_PIE_BLOCK_TEMPLATESELECTOR(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_BLOCK_GRP                        %u\n", HAL_MAX_NUM_OF_PIE_BLOCK_GRP(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_BLOCK_LOGIC                      %u\n", HAL_MAX_NUM_OF_PIE_BLOCK_LOGIC(unit));
    osal_printf("HAL_MAX_NUM_OF_PIE_TEMPLATE_FIELD                   %u\n", HAL_MAX_NUM_OF_PIE_TEMPLATE_FIELD(unit));
    osal_printf("HAL_MAX_NUM_OF_RANGE_CHECK_IP                       %u\n", HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit));
    osal_printf("HAL_MAX_NUM_OF_RANGE_CHECK                          %u\n", HAL_MAX_NUM_OF_RANGE_CHECK(unit));
    osal_printf("HAL_MAX_NUM_OF_METADATA                             %u\n", HAL_MAX_NUM_OF_METADATA(unit));
    osal_printf("HAL_L2_HASHDEPTH                                    %u\n", HAL_L2_HASHDEPTH(unit));
    osal_printf("HAL_L2_HASHWIDTH                                    %u\n", HAL_L2_HASHWIDTH(unit));
    osal_printf("HAL_MAX_NUM_OF_QUEUE                                %u\n", HAL_MAX_NUM_OF_QUEUE(unit));
    osal_printf("HAL_MIN_NUM_OF_QUEUE                                %u\n", HAL_MIN_NUM_OF_QUEUE(unit));
    osal_printf("HAL_MAX_NUM_OF_CPU_QUEUE                            %u\n", HAL_MAX_NUM_OF_CPU_QUEUE(unit));
    osal_printf("HAL_MAX_NUM_OF_STACK_QUEUE                          %u\n", HAL_MAX_NUM_OF_STACK_QUEUE(unit));
    osal_printf("HAL_MAX_NUM_OF_IGR_QUEUE                            %u\n", HAL_MAX_NUM_OF_IGR_QUEUE(unit));
    osal_printf("HAL_MIN_NUM_OF_IGR_QUEUE                            %u\n", HAL_MIN_NUM_OF_IGR_QUEUE(unit));
    osal_printf("HAL_MAX_NUM_OF_CVLAN_TPID                           %u\n", HAL_MAX_NUM_OF_CVLAN_TPID(unit));
    osal_printf("HAL_MAX_NUM_OF_SVLAN_TPID                           %u\n", HAL_MAX_NUM_OF_SVLAN_TPID(unit));
    osal_printf("HAL_MAX_NUM_OF_EVLAN_TPID                           %u\n", HAL_MAX_NUM_OF_EVLAN_TPID(unit));
    osal_printf("HAL_TPID_ENTRY_IDX_MAX                              %u\n", HAL_TPID_ENTRY_IDX_MAX(unit));
    osal_printf("HAL_TPID_ENTRY_MASK_MAX                             %u\n", HAL_TPID_ENTRY_MASK_MAX(unit));
    osal_printf("HAL_PROTOCOL_VLAN_IDX_MAX                           %u\n", HAL_PROTOCOL_VLAN_IDX_MAX(unit));
    osal_printf("HAL_VLAN_FID_MAX                                    %u\n", HAL_VLAN_FID_MAX(unit));
    osal_printf("HAL_FLOWCTRL_THRESH_MAX                             %u\n", HAL_FLOWCTRL_THRESH_MAX(unit));
    osal_printf("HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_LEN_MAX            %u\n", HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_LEN_MAX(unit));
    osal_printf("HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX                %u\n", HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX(unit));
    osal_printf("HAL_PRI_OF_SELECTION_MAX                            %u\n", HAL_PRI_OF_SELECTION_MAX(unit));
    osal_printf("HAL_PRI_OF_SELECTION_MIN                            %u\n", HAL_PRI_OF_SELECTION_MIN(unit));
    osal_printf("HAL_DP_OF_SELECTION_MAX                             %u\n", HAL_DP_OF_SELECTION_MAX(unit));
    osal_printf("HAL_DP_OF_SELECTION_MIN                             %u\n", HAL_DP_OF_SELECTION_MIN(unit));
    osal_printf("HAL_PRI_SEL_GROUP_INDEX_MAX                         %u\n", HAL_PRI_SEL_GROUP_INDEX_MAX(unit));
    osal_printf("HAL_QUEUE_WEIGHT_MAX                                %u\n", HAL_QUEUE_WEIGHT_MAX(unit));
    osal_printf("HAL_IGR_QUEUE_WEIGHT_MAX                            %u\n", HAL_IGR_QUEUE_WEIGHT_MAX(unit));
    osal_printf("HAL_RATE_OF_BANDWIDTH_MAX                           %u\n", HAL_RATE_OF_BANDWIDTH_MAX(unit));
    osal_printf("HAL_RATE_OF_10G_BANDWIDTH_MAX                       %u\n", HAL_RATE_OF_10G_BANDWIDTH_MAX(unit));
    osal_printf("HAL_BURST_OF_IGR_BANDWIDTH_MAX                      %u\n", HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit));
    osal_printf("HAL_BURST_OF_BANDWIDTH_MAX                          %u\n", HAL_BURST_OF_BANDWIDTH_MAX(unit));
    osal_printf("HAL_BURST_OF_STORM_CONTROL_MAX                      %u\n", HAL_BURST_OF_STORM_CONTROL_MAX(unit));
    osal_printf("HAL_BURST_OF_STORM_PROTO_CONTROL_MAX                %u\n", HAL_BURST_OF_STORM_PROTO_CONTROL_MAX(unit));
    osal_printf("HAL_THRESH_OF_EGR_QUEUE_DROP_GROUP_IDX_MAX          %u\n", HAL_THRESH_OF_EGR_QUEUE_DROP_GROUP_IDX_MAX(unit));
    osal_printf("HAL_THRESH_OF_ETE_FC_REMOTE_PORT_GROUP_IDX_MAX      %u\n", HAL_THRESH_OF_ETE_FC_REMOTE_PORT_GROUP_IDX_MAX(unit));
    osal_printf("HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX  %u\n", HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit));
    osal_printf("HAL_THRESH_OF_IGR_BW_FLOWCTRL_MIN                   %u\n", HAL_THRESH_OF_IGR_BW_FLOWCTRL_MIN(unit));
    osal_printf("HAL_THRESH_OF_IGR_BW_FLOWCTRL_MAX                   %u\n", HAL_THRESH_OF_IGR_BW_FLOWCTRL_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_FASTPATH_OF_RATE                     %u\n", HAL_MAX_NUM_OF_FASTPATH_OF_RATE(unit));
    osal_printf("HAL_RATE_OF_STORM_CONTROL_MAX                       %u\n", HAL_RATE_OF_STORM_CONTROL_MAX(unit));
    osal_printf("HAL_RATE_OF_STORM_PROTO_CONTROL_MAX                 %u\n", HAL_RATE_OF_STORM_PROTO_CONTROL_MAX(unit));
    osal_printf("HAL_BURST_RATE_OF_STORM_CONTROL_MIN                 %u\n", HAL_BURST_RATE_OF_STORM_CONTROL_MIN(unit));
    osal_printf("HAL_BURST_RATE_OF_STORM_CONTROL_MAX                 %u\n", HAL_BURST_RATE_OF_STORM_CONTROL_MAX(unit));
    osal_printf("HAL_BURST_RATE_OF_10G_STORM_CONTROL_MIN             %u\n", HAL_BURST_RATE_OF_10G_STORM_CONTROL_MIN(unit));
    osal_printf("HAL_BURST_RATE_OF_10G_STORM_CONTROL_MAX             %u\n", HAL_BURST_RATE_OF_10G_STORM_CONTROL_MAX(unit));
    osal_printf("HAL_BURST_RATE_OF_STORM_PROTO_CONTROL_MAX           %u\n", HAL_BURST_RATE_OF_STORM_PROTO_CONTROL_MAX(unit));
    osal_printf("HAL_INTERNAL_PRIORITY_MAX                           %u\n", HAL_INTERNAL_PRIORITY_MAX(unit));
    osal_printf("HAL_DROP_PRECEDENCE_MAX                             %u\n", HAL_DROP_PRECEDENCE_MAX(unit));
    osal_printf("HAL_PRIORITY_REMAP_GROUP_IDX_MAX                    %u\n", HAL_PRIORITY_REMAP_GROUP_IDX_MAX(unit));
    osal_printf("HAL_PRIORITY_REMARK_GROUP_IDX_MAX                   %u\n", HAL_PRIORITY_REMARK_GROUP_IDX_MAX(unit));
    osal_printf("HAL_WRED_WEIGHT_MAX                                 %u\n", HAL_WRED_WEIGHT_MAX(unit));
    osal_printf("HAL_WRED_MPD_MAX                                    %u\n", HAL_WRED_MPD_MAX(unit));
    osal_printf("HAL_WRED_DROP_PROBABILITY_MAX                       %u\n", HAL_WRED_DROP_PROBABILITY_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_L2_HASH_ALGO                         %u\n", HAL_MAX_NUM_OF_L2_HASH_ALGO(unit));
    osal_printf("HAL_L2_LEARN_LIMIT_CNT_MAX                          %u\n", HAL_L2_LEARN_LIMIT_CNT_MAX(unit));
    osal_printf("HAL_L2_LEARN_LIMIT_CNT_WO_CAM_MAX                   %u\n", HAL_L2_LEARN_LIMIT_CNT_WO_CAM_MAX(unit));
    osal_printf("HAL_L2_LEARN_LIMIT_CNT_DISABLE                      %u\n", HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit));
    osal_printf("HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX                    %u\n", HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit));
    osal_printf("HAL_L2_NTFY_BP_THR_MAX                              %u\n", HAL_L2_NTFY_BP_THR_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_VRF                                  %u\n", HAL_MAX_NUM_OF_VRF(unit));
    osal_printf("HAL_MAX_NUM_OF_INTF                                 %u\n", HAL_MAX_NUM_OF_INTF(unit));
    osal_printf("HAL_MAX_NUM_OF_INTF_MTU                             %u\n", HAL_MAX_NUM_OF_INTF_MTU(unit));
    osal_printf("HAL_MAX_NUM_OF_INTF_MTU_VALUE                       %u\n", HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_ECMP                              %u\n", HAL_MAX_NUM_OF_L3_ECMP(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_ECMP_HASH_IDX                     %u\n", HAL_MAX_NUM_OF_L3_ECMP_HASH_IDX(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_ECMP_NEXTHOP                      %u\n", HAL_MAX_NUM_OF_L3_ECMP_NEXTHOP(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_NEXTHOP                           %u\n", HAL_MAX_NUM_OF_L3_NEXTHOP(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_HOST                              %u\n", HAL_MAX_NUM_OF_L3_HOST(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_ROUTE                             %u\n", HAL_MAX_NUM_OF_L3_ROUTE(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_CONFLICT_HOST                     %u\n", HAL_MAX_NUM_OF_L3_CONFLICT_HOST(unit));
    osal_printf("HAL_MAX_NUM_OF_L3_MCAST_GROUP_NEXTHOP               %u\n", HAL_MAX_NUM_OF_L3_MCAST_GROUP_NEXTHOP(unit));
    osal_printf("HAL_MAX_NUM_OF_TUNNEL                               %u\n", HAL_MAX_NUM_OF_TUNNEL(unit));
    osal_printf("HAL_MAX_NUM_OF_TUNNEL_QOS_PROFILE                   %u\n", HAL_MAX_NUM_OF_TUNNEL_QOS_PROFILE(unit));
    osal_printf("HAL_EEE_QUEUE_THRESH_MAX                            %u\n", HAL_EEE_QUEUE_THRESH_MAX(unit));
    osal_printf("HAL_SEC_MINIPV6FRAGLEN_MAX                          %u\n", HAL_SEC_MINIPV6FRAGLEN_MAX(unit));
    osal_printf("HAL_SEC_MAXPINGLEN_MAX                              %u\n", HAL_SEC_MAXPINGLEN_MAX(unit));
    osal_printf("HAL_SEC_SMURFNETMASKLEN_MAX                         %u\n", HAL_SEC_SMURFNETMASKLEN_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_IP_MAC_BIND_ENTRY                    %u\n", HAL_MAX_NUM_OF_IP_MAC_BIND_ENTRY(unit));
    osal_printf("HAL_SFLOW_RATE_MAX                                  %u\n", HAL_SFLOW_RATE_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_MCAST_FWD                            %u\n", HAL_MAX_NUM_OF_MCAST_FWD(unit));
    osal_printf("HAL_MIIM_PAGE_ID_MAX                                %u\n", HAL_MIIM_PAGE_ID_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_C2SC_ENTRY                           %u\n", HAL_MAX_NUM_OF_C2SC_ENTRY(unit));
    osal_printf("HAL_MAX_NUM_OF_C2SC_BLK_ENTRY                       %u\n", HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit));
    osal_printf("HAL_MAX_NUM_OF_C2SC_BLK                             %u\n", HAL_MAX_NUM_OF_C2SC_BLK(unit));
    osal_printf("HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_VID                 %u\n", HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_VID(unit));
    osal_printf("HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_SET                 %u\n", HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_SET(unit));
    osal_printf("HAL_MAX_NUM_OF_SC2C_ENTRY                           %u\n", HAL_MAX_NUM_OF_SC2C_ENTRY(unit));
    osal_printf("HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_VID                 %u\n", HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_VID(unit));
    osal_printf("HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_SET                 %u\n", HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_SET(unit));
    osal_printf("HAL_MAX_NUM_OF_VLAN_PROFILE                         %u\n", HAL_MAX_NUM_OF_VLAN_PROFILE(unit));
    osal_printf("HAL_MAX_ACCEPT_FRAME_LEN                            %u\n", HAL_MAX_ACCEPT_FRAME_LEN(unit));
    osal_printf("HAL_MAX_NUM_OF_MCAST_ENTRY                          %u\n", HAL_MAX_NUM_OF_MCAST_ENTRY(unit));
    osal_printf("HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY                  %u\n", HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit));
    osal_printf("HAL_MAX_NUM_OF_MPLS_ENCAP                           %u\n", 0);//HAL_MAX_NUM_OF_MPLS_ENCAP(unit));
    osal_printf("HAL_MAX_NUM_OF_MPLS_DECAP                           %u\n", 0);//HAL_MAX_NUM_OF_MPLS_DECAP(unit));
    osal_printf("HAL_MAX_NUM_OF_MPLS_DECAP_CAM                       %u\n", 0);//HAL_MAX_NUM_OF_MPLS_DECAP_CAM(unit));
    osal_printf("HAL_MAX_NUM_OF_MPLS_DECAP_ENTRY                     %u\n", 0);//HAL_MAX_NUM_OF_MPLS_DECAP_ENTRY(unit));
    osal_printf("HAL_MAX_NUM_OF_MPLS_NEXTHOP                         %u\n", 0);//HAL_MAX_NUM_OF_MPLS_NEXTHOP(unit));
    osal_printf("HAL_MPLS_HASHDEPTH                                  %u\n", 0);//HAL_MPLS_HASHDEPTH(unit));
    osal_printf("HAL_OF_BYTE_CNTR_TH_MAX                             %llu\n", HAL_OF_BYTE_CNTR_TH_MAX(unit));
    osal_printf("HAL_OF_PKT_CNTR_TH_MAX                              %llu\n", HAL_OF_PKT_CNTR_TH_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_OF_IGR_FLOWTBL                       %u\n", HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit));
    osal_printf("HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL               %u\n", HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit));
    osal_printf("HAL_OF_LOOPBACK_MAX                                 %u\n", HAL_OF_LOOPBACK_MAX(unit));
    osal_printf("HAL_OF_MAX_NUM_OF_GRP_ENTRY                         %u\n", HAL_OF_MAX_NUM_OF_GRP_ENTRY(unit));
    osal_printf("HAL_OF_MAX_NUM_OF_GRP_ENTRY_BUCKET                  %u\n", HAL_OF_MAX_NUM_OF_GRP_ENTRY_BUCKET(unit));
    osal_printf("HAL_OF_MAX_NUM_OF_ACTION_BUCKET                     %u\n", HAL_OF_MAX_NUM_OF_ACTION_BUCKET(unit));
    osal_printf("HAL_OF_MAX_NUM_OF_DMAC_ENTRY                        %u\n", HAL_OF_MAX_NUM_OF_DMAC_ENTRY(unit));
    osal_printf("HAL_MAX_NUM_OF_ROUTE_HOST_ADDR                      %u\n", HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit));
    osal_printf("HAL_MAX_NUM_OF_ROUTE_SWITCH_ADDR                    %u\n", HAL_MAX_NUM_OF_ROUTE_SWITCH_ADDR(unit));
    osal_printf("HAL_MAX_NUM_OF_NEXTHOP                              %u\n", HAL_MAX_NUM_OF_NEXTHOP(unit));
    osal_printf("HAL_MAX_NUM_OF_LED_ENTITY                           %u\n", HAL_MAX_NUM_OF_LED_ENTITY(unit));
    osal_printf("HAL_MAX_NUM_OF_DYING_GASP_PKT_CNT                   %u\n", HAL_MAX_NUM_OF_DYING_GASP_PKT_CNT(unit));
    osal_printf("HAL_DYING_GASP_SUSTAIN_TIME_MAX                     %u\n", HAL_DYING_GASP_SUSTAIN_TIME_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_RMA_USER_DEFINED                     %u\n", HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit));
    osal_printf("HAL_TIME_FREQ_MAX                                   %u\n", HAL_TIME_FREQ_MAX(unit));
    osal_printf("HAL_MAX_NUM_OF_ETHDM_RX_TIMESTAMP                   %u\n", HAL_MAX_NUM_OF_ETHDM_RX_TIMESTAMP(unit));
    osal_printf("HAL_BPE_PVID_HASHDEPTH                              %u\n", HAL_BPE_PVID_HASHDEPTH(unit));
    osal_printf("HAL_MAX_NUM_OF_STACK_PORT                           %u\n", HAL_MAX_NUM_OF_STACK_PORT(unit));


    osal_printf(" RTK_DOT1P_PRIORITY_MAX                         %u\n", RTK_DOT1P_PRIORITY_MAX);
    osal_printf(" RTK_VLAN_ID_MAX                                %u\n", RTK_VLAN_ID_MAX);
    osal_printf(" RTK_VLAN_ID_MIN                                %u\n", RTK_VLAN_ID_MIN);
    osal_printf(" RTK_DOT1P_CFI_MAX                              %u\n", RTK_DOT1P_CFI_MAX);
    osal_printf(" RTK_DOT1P_DEI_MAX                              %u\n", RTK_DOT1P_DEI_MAX);
    osal_printf(" RTK_VALUE_OF_DSCP_MAX                          %u\n", RTK_VALUE_OF_DSCP_MAX);
    osal_printf(" RTK_DROP_PRECEDENCE_MAX                        %u\n", RTK_DROP_PRECEDENCE_MAX);

#if 0
    hal_debug_show_info(unit, 0);
#endif

    for (port = 0; port < RTK_MAX_PORT_PER_UNIT; port++)
    {
        if(HWP_PORT_EXIST(unit,port))
        {
            osal_printf("HWP_PORT_EXIST(%u) %d\n", port, HWP_PORT_EXIST(unit, port));
        }
        else
        {
            osal_printf("HWP_PORT_EXIST(%u) 0\n", port);
        }
        if(HWP_IS_CPU_PORT(unit, port))
            osal_printf("HWP_CPU_PORT(%u) %d\n", port, HWP_IS_CPU_PORT(unit, port));
        if(HWP_FIBER_PORT(unit, port))
            osal_printf("HWP_FIBER_PORT(%u) %d\n", port, HWP_FIBER_PORT(unit, port));
        if(HWP_COMBO_PORT(unit, port))
            osal_printf("HWP_COMBO_PORT(%u) %d\n", port, HWP_COMBO_PORT(unit, port));

    }

    return RT_ERR_OK;
}
#endif


/* Function Name:
 *      rtk_switch_deviceInfo_get
 * Description:
 *      Get device information of the specific unit
 * Input:
 *      unit     - unit id
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The routine have provided following information based on SDK probe result
 *      - chip id, chip revision id, chip capacity
 *      - total port number and cpu port id
 *      - detail information in each port type
 * Changes:
 *      None
 */
int32
rtk_switch_deviceInfo_get(uint32 unit, rtk_switch_devInfo_t *pDevInfo)
{
#ifndef PHY_ONLY
    hal_control_t *pHal_control;
#endif
    int           i;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((NULL == pDevInfo), RT_ERR_NULL_POINTER);

#ifndef PHY_ONLY
    if ((pHal_control = hal_ctrlInfo_get(unit)) == NULL)
    {
        return RT_ERR_FAILED;
    }
#endif

    pDevInfo->chipId    = HWP_CHIP_ID(unit);
    pDevInfo->revision  = HWP_CHIP_REV(unit);
    pDevInfo->familyId  = HWP_CHIP_FAMILY_ID(unit);
    pDevInfo->port_number = HWP_PORT_COUNT(unit);
    pDevInfo->cpuPort   = HWP_CPU_MACID(unit);
    /* FE */
    pDevInfo->fe.portNum = HWP_FE_PORT_COUNT(unit);
    osal_memset(&pDevInfo->fe.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_FE_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->fe.portmask, i);
    }/* end for */
    /* GE */
    pDevInfo->ge.portNum = HWP_GE_PORT_COUNT(unit);
    osal_memset(&pDevInfo->ge.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_GE_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->ge.portmask, i);
    }/* end for */
    /* 10GE */
    pDevInfo->xge.portNum = HWP_10GE_PORT_COUNT(unit);
    osal_memset(&pDevInfo->xge.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_10GE_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->xge.portmask, i);
    }/* end for */
    /* COPPER */
    pDevInfo->copper.portNum = HWP_COPPER_PORT_COUNT(unit);
    osal_memset(&pDevInfo->copper.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_COPPER_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->copper.portmask, i);
    }/* end for */
    /* FIBER */
    pDevInfo->fiber.portNum = HWP_FIBER_PORT_COUNT(unit);
    osal_memset(&pDevInfo->fiber.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_FIBER_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->fiber.portmask, i);
    }/* end for */
    /* COMBO */
    pDevInfo->combo.portNum = HWP_COMBO_PORT_COUNT(unit);
    osal_memset(&pDevInfo->combo.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_COMBO_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->combo.portmask, i);
    }/* end for */
    /* SERDES */
    pDevInfo->serdes.portNum = HWP_SERDES_PORT_COUNT(unit);
    osal_memset(&pDevInfo->serdes.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_SERDES_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->serdes.portmask, i);
    }/* end for */
    /* ETHER */
    pDevInfo->ether.portNum = HWP_ETHER_PORT_COUNT(unit);
    osal_memset(&pDevInfo->ether.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_ETHER_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->ether.portmask, i);
    }/* end for */
    /* UPLINK */
    pDevInfo->uplink.portNum = HWP_UPLINK_PORT_COUNT(unit);
    osal_memset(&pDevInfo->uplink.portmask, 0, sizeof(rtk_portmask_t));
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        if (HWP_PORT_EXIST(unit, i) && HWP_UPLINK_PORT(unit, i))
            RTK_PORTMASK_PORT_SET(pDevInfo->uplink.portmask, i);
    }/* end for */
    /* STACK */
    {
        pDevInfo->stack.portNum = 0;
        osal_memset(&pDevInfo->stack.portmask, 0, sizeof(rtk_portmask_t));
#ifndef PHY_ONLY
        for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
            if (HAL_STACK_PORT(unit, i))
            {
                pDevInfo->stack.portNum++;
                RTK_PORTMASK_PORT_SET(pDevInfo->stack.portmask, i);
            }
        }
#endif
    }/* end for */
    /* all */
    for (i=0; i<RTK_MAX_NUM_OF_PORTS; i++) {
        pDevInfo->phyType[i] = RTK_PHYTYPE_NONE;
        if (HWP_PORT_EXIST(unit, i))
           pDevInfo->phyType[i] = HWP_PHY_MODEL_BY_PORT(unit, i);
    }/* end for */
    pDevInfo->all.portNum = HWP_PORT_COUNT(unit);
    HWP_GET_ALL_PORTMASK(unit, pDevInfo->all.portmask);

#ifndef PHY_ONLY
    osal_memcpy(&pDevInfo->capacityInfo, pHal_control->pDev_info->pCapacityInfo, sizeof(rt_register_capacity_t));
    osal_memcpy(&pDevInfo->macPpInfo, pHal_control->pDev_info->pMacPpInfo, sizeof(rt_macPpInfo_t));
#endif


    return RT_ERR_OK;
} /* end of rtk_switch_deviceInfo_get */


#ifndef PHY_ONLY
/* Function Name:
 *      rtk_switch_cpuMaxPktLen_get
 * Description:
 *      Get the max packet length setting on CPU port of the specific unit
 * Input:
 *      unit - unit id
 *      dir  - direction of packet
 * Output:
 *      pLen - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported Maximum packet lenght is 16383B.
 *      (2) Supported management frame is as following:
 *          - rtk_switch_pktDir_t \ Chip:   8380   8390   9300  9310
 *          - PKTDIR_RX                     O       O      O     O
 *          - PKTDIR_TX                     O       O      O     O
 * Changes:
 *      None
 */
int32
rtk_switch_cpuMaxPktLen_get(uint32 unit, rtk_switch_pktDir_t dir, uint32 *pLen)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_cpuMaxPktLen_get(unit, dir, pLen);
} /* end of rtk_switch_cpuMaxPktLen_get */

/* Function Name:
 *      rtk_switch_cpuMaxPktLen_set
 * Description:
 *      Set the max packet length setting on CPU port of the specific unit
 * Input:
 *      unit - unit id
 *      dir  - direction of packet
 *      len  - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid packet direction
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported Maximum packet lenght is 16383B.
 *      (2) Supported management frame is as following:
 *          - rtk_switch_pktDir_t \ Chip:   8380   8390   9300  9310
 *          - PKTDIR_RX                     O       O      O     O
 *          - PKTDIR_TX                     O       O      O     O
 * Changes:
 *      None
 */
int32
rtk_switch_cpuMaxPktLen_set(uint32 unit, rtk_switch_pktDir_t dir, uint32 len)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_cpuMaxPktLen_set(unit, dir, len);
} /* end of rtk_switch_cpuMaxPktLen_set */

/* Function Name:
 *      rtk_switch_maxPktLenLinkSpeed_get
 * Description:
 *      Get the max packet length setting of the specific speed type
 * Input:
 *      unit  - unit id
 *      speed - speed type
 * Output:
 *      pLen  - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid enum speed type
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Maximum packet length filtering is examined on both receiving and transmitting ports
 *          according to port's link speed.
 *      (2) Supported Maximum packet lenght is 16383B.
 *      (3) Max packet length setting speed type
 *          - MAXPKTLEN_LINK_SPEED_FE
 *          - MAXPKTLEN_LINK_SPEED_GE
 *      (4) Change to use rtk_switch_portMaxPktLenLinkSpeed_get(unit, port, speed, *pLen) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_switch_maxPktLenLinkSpeed_get(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_maxPktLenLinkSpeed_get(unit, speed, pLen);
} /* end of rtk_switch_maxPktLenLinkSpeed_get */

/* Function Name:
 *      rtk_switch_maxPktLenLinkSpeed_set
 * Description:
 *      Set the max packet length of the specific speed type
 * Input:
 *      unit  - unit id
 *      speed - speed type
 *      len   - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid enum speed type
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Maximum packet length filtering is examined on both receiving and transmitting ports
 *          according to port's link speed.
 *      (2) Supported Maximum packet lenght is 16383B.
 *      (3) Max packet length setting speed type
 *          - MAXPKTLEN_LINK_SPEED_FE
 *          - MAXPKTLEN_LINK_SPEED_GE
 *      (4) Change to use rtk_switch_portMaxPktLenLinkSpeed_set(unit, port, speed, len) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_switch_maxPktLenLinkSpeed_set(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_maxPktLenLinkSpeed_set(unit, speed, len);
} /* end of rtk_switch_maxPktLenLinkSpeed_set */

/* Function Name:
 *      rtk_switch_maxPktLenTagLenCntIncEnable_get
 * Description:
 *      Get include or exclude tag length state of max packet length in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Inner(4B) and outer(4B) tag length can be included or excluded to the maximum packet length filtering.
 *      (2) Change to use rtk_switch_portMaxPktLenTagLenCntIncEnable_get(unit, port, pEnable) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_switch_maxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_maxPktLenTagLenCntIncEnable_get(unit, pEnable);
} /* end of rtk_switch_maxPktLenTagLenCntIncEnable_get */

/* Function Name:
 *      rtk_switch_maxPktLenTagLenCntIncEnable_set
 * Description:
 *      Set the max packet length to include or exclude tag length in the specified device.
 * Input:
 *      unit   - unit id
 *      enable - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Inner(4B) and outer(4B) tag length can be included or excluded to the maximum packet length filtering.
 *      (2) Change to use rtk_switch_portMaxPktLenTagLenCntIncEnable_set(unit, port, enable) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_switch_maxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_maxPktLenTagLenCntIncEnable_set(unit, enable);
} /* end of rtk_switch_maxPktLenTagLenCntIncEnable_set */

/* Function Name:
 *      rtk_switch_snapMode_get
 * Description:
 *      Get the status of SNAP qualification mode.
 * Input:
 *      unit      - unit id
 * Output:
 *      pSnapMode - pointer to SNAP mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      OUI(00-00-00) comparsion can be ommited for SNAP qualification.
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 * Changes:
 *      None
 */
int32
rtk_switch_snapMode_get(uint32 unit, rtk_snapMode_t *pSnapMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_snapMode_get(unit, pSnapMode);
} /* end of rtk_switch_snapMode_get */

/* Function Name:
 *      rtk_switch_snapMode_set
 * Description:
 *      Set the status of SNAP qualification mode.
 * Input:
 *      unit     - unit id
 *      snapMode - SNAP mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      OUI(00-00-00) comparsion can be ommited for SNAP qualification.
 *      SNAP mode is as following
 *      - SNAP_MODE_AAAA03000000
 *      - SNAP_MODE_AAAA03
 * Changes:
 *      None
 */
int32
rtk_switch_snapMode_set(uint32 unit, rtk_snapMode_t snapMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_snapMode_set(unit, snapMode);
} /* end of rtk_switch_snapMode_set */

/* Function Name:
 *      rtk_switch_chksumFailAction_get
 * Description:
 *      Get forwarding action of checksum error on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      failType - checksum fail type
 * Output:
 *      pAction  - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300
 * Note:
 *      (1) Checksum fail type is as following \ Chip :     8390 9300
 *          - LAYER2_CHKSUM_FAIL                              O    O
 *      (2) Forwarding action is as following \ Chip :      8390 9300
 *          - ACTION_FORWARD                                  O    O
 *          - ACTION_DROP                                     O    O
 * Changes:
 *      None
 */
int32
rtk_switch_chksumFailAction_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_chksumFailAction_get(unit, port, failType, pAction);

} /* end of rtk_switch_chksumFailAction_get */

/* Function Name:
 *      rtk_switch_chksumFailAction_set
 * Description:
 *      Set forwarding action of checksum error on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      failType - checksum fail type
 *      action   - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid error forwarding action
 *      RT_ERR_INPUT      - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300
 * Note:
 *      (1) Checksum fail type is as following \ Chip :     8390 9300
 *          - LAYER2_CHKSUM_FAIL                              O    O
 *      (2) Forwarding action is as following \ Chip :      8390 9300
 *          - ACTION_FORWARD                                  O    O
 *          - ACTION_DROP                                     O    O
 * Changes:
 *      None
 */
int32
rtk_switch_chksumFailAction_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                action)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_chksumFailAction_set(unit, port, failType, action);

} /* end of rtk_switch_chksumFailAction_set */

/* Function Name:
 *      rtk_switch_recalcCRCEnable_get
 * Description:
 *      Get enable status of recaculate CRC on specified egress port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of recaculate CRC
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 *      When enable, only when packet is modified CRC will be recalculated.
 * Changes:
 *      None
 */
int32
rtk_switch_recalcCRCEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_recalcCRCEnable_get(unit, port, pEnable);
} /* end of rtk_switch_recalcCRCEnable_get */

/* Function Name:
 *      rtk_switch_recalcCRCEnable_set
 * Description:
 *      Set enable status of recaculate CRC on specified egress port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of recaculate CRC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      When enable, mirrored packet with bad CRC will be recaculate at mirroring port.
 *      When enable, only when packet is modified CRC will be recalculated.
 * Changes:
 *      None
 */
int32
rtk_switch_recalcCRCEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_recalcCRCEnable_set(unit, port, enable);
} /* end of rtk_switch_recalcCRCEnable_set */

/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */

/* Function Name:
 *      rtk_switch_mgmtMacAddr_get
 * Description:
 *      Get MAC address of switch.
 * Input:
 *      unit - unit id
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The MAC address is used when sending Pause frame.
 * Changes:
 *      None
 */
int32
rtk_switch_mgmtMacAddr_get(uint32 unit, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_mgmtMacAddr_get(unit, pMac);
} /* end of rtk_switch_mgmtMacAddr_get */

/* Function Name:
 *      rtk_switch_mgmtMacAddr_set
 * Description:
 *      Set MAC address of switch.
 * Input:
 *      unit - unit id
 *      pMac - MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_MAC          - invalid mac address
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The MAC address is used when sending Pause frame.
 * Changes:
 *      None
 */
int32
rtk_switch_mgmtMacAddr_set(uint32 unit, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_mgmtMacAddr_set(unit, pMac);
} /* end of rtk_switch_mgmtMacAddr_set */

/* Function Name:
 *      rtk_switch_IPv4Addr_get
 * Description:
 *      Get IPv4 address of switch.
 * Input:
 *      unit    - unit id
 * Output:
 *      pIpAddr - pointer to IPv4 address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_switch_IPv4Addr_get(uint32 unit, uint32 *pIpAddr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_IPv4Addr_get(unit, pIpAddr);
} /* end of rtk_switch_IPv4Addr_get */

/* Function Name:
 *      rtk_switch_IPv4Addr_set
 * Description:
 *      Set IPv4 address of switch.
 * Input:
 *      unit   - unit id
 *      ipAddr - IPv4 address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - invalid IPv4 address
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_switch_IPv4Addr_set(uint32 unit, uint32 ipAddr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_IPv4Addr_set(unit, ipAddr);
} /* end of rtk_switch_IPv4Addr_set */

/* Function Name:
 *      rtk_switch_pkt2CpuTypeFormat_get
 * Description:
 *      Get the configuration about content state for packet to CPU
 * Input:
 *      unit    - unit id
 *      type    - method of packet to CPU
 * Output:
 *      pFormat - type of format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The type of packet to CPU:
 *      - PKT2CPU_TYPE_FORWARD
 *      - PKT2CPU_TYPE_TRAP
 *
 *      The type of format:
 *      - TRAP_PKT_ORIGINAL
 *      - TRAP_PKT_MODIFIED
 * Changes:
 *      None
 */
int32
rtk_switch_pkt2CpuTypeFormat_get(
    uint32                      unit,
    rtk_switch_pkt2CpuType_t    type,
    rtk_pktFormat_t             *pFormat)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_pkt2CpuTypeFormat_get(unit, type, pFormat);
}   /* end of rtk_switch_pkt2CpuTypeFormat_get */

/* Function Name:
 *      rtk_switch_pkt2CpuTypeFormat_set
 * Description:
 *      Set the configuration about content state for packet to CPU
 * Input:
 *      unit   - unit id
 *      type   - method of packet to CPU
 *      format - packet format to CPU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The type of format:
 *      - TRAP_PKT_ORIGINAL
 *      - TRAP_PKT_MODIFIED
 * Changes:
 *      None
 */
int32
rtk_switch_pkt2CpuTypeFormat_set(
    uint32                      unit,
    rtk_switch_pkt2CpuType_t    type,
    rtk_pktFormat_t             format)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_pkt2CpuTypeFormat_set(unit, type, format);
}   /* end of rtk_switch_pkt2CpuTypeFormat_set */

/* Function Name:
 *      rtk_switch_pppoeIpParseEnable_get
 * Description:
 *      Get enable status of PPPoE IP-parse functionality.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of PPPoE parse functionality
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Enable the device to recognize PPPoE frame.
 *      The PPPoE frame is treated as unknown L2 frame if PPPoE IP-parse is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_pppoeIpParseEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_pppoeIpParseEnable_get(unit, pEnable);
} /* end of rtk_switch_pppoeIpParseEnable_get */

/* Function Name:
 *      rtk_switch_pppoeIpParseEnable_set
 * Description:
 *      Set enable status of PPPoE IP-parse functionality.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Enable the device to recognize PPPoE frame.
 *      The PPPoE frame is treated as unknown L2 frame if PPPoE IP-parse is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_pppoeIpParseEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_pppoeIpParseEnable_set(unit, enable);
} /* end of rtk_switch_pppoeIpParseEnable_set */

/* Function Name:
 *      rtk_switch_cpuPktTruncateEnable_get
 * Description:
 *      Get CPU port truncation function state.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation length is specified by rtk_switch_cpuPktTruncateLen_set.
 * Changes:
 *      None
 */
int32
rtk_switch_cpuPktTruncateEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_cpuPktTruncateEnable_get(unit, pEnable);
}

/* Function Name:
 *      rtk_switch_cpuPktTruncateEnable_set
 * Description:
 *      Set CPU port truncation function state.
 * Input:
 *      unit    - unit id
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation length is specified by rtk_switch_cpuPktTruncateLen_set.
 * Changes:
 *      None
 */
int32
rtk_switch_cpuPktTruncateEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_cpuPktTruncateEnable_set(unit, enable);
}

/* Function Name:
 *      rtk_switch_cpuPktTruncateLen_get
 * Description:
 *      Get the packet length for CPU port truncation function.
 * Input:
 *      unit - unit id
 * Output:
 *      pLen  - packet length to truncate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation function takes effect if rtk_switch_cpuPktTruncateEnable_set is enabled.
 * Changes:
 *      None
 */
int32
rtk_switch_cpuPktTruncateLen_get(uint32 unit, uint32 *pLen)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_cpuPktTruncateLen_get(unit, pLen);
}

/* Function Name:
 *      rtk_switch_cpuPktTruncateLen_set
 * Description:
 *      Set the packet length for CPU port truncation function.
 * Input:
 *      unit - unit id
 *      len  - packet length to truncate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - invalid truncation length
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Packet is truncated to specified length if it is trap/forward to CPU and packet length is
 *          over than the truncation length.
 *      (2) The truncation function takes effect if rtk_switch_cpuPktTruncateEnable_set is enabled.
 * Changes:
 *      None
 */
int32
rtk_switch_cpuPktTruncateLen_set(uint32 unit, uint32 len)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_cpuPktTruncateLen_set(unit, len);
}

/* Function Name:
 *      rtk_switch_portMaxPktLenLinkSpeed_get
 * Description:
 *      Get the max packet length setting of the specific speed type and port
 * Input:
 *      unit  - unit id
 *      port  - the specific port
 *      speed - speed type
 * Output:
 *      pLen  - pointer to the max packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid enum speed type
 * Applicable:
 *      9300, 9310
 * Note:
 *      Max packet length setting speed type
 *      - MAXPKTLEN_LINK_SPEED_FE
 *      - MAXPKTLEN_LINK_SPEED_GE
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_portMaxPktLenLinkSpeed_get(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_portMaxPktLenLinkSpeed_get(unit, port, speed, pLen);
}   /* end of rtk_switch_portMaxPktLenLinkSpeed_get */

/* Function Name:
 *      rtk_switch_portMaxPktLenLinkSpeed_set
 * Description:
 *      Set the max packet length of the specific speed type and port
 * Input:
 *      unit  - unit id
 *      port  - the specific port
 *      speed - speed type
 *      len   - max packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid enum speed type
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Applicable:
 *      9300, 9310
 * Note:
 *      Max packet length setting speed type
 *      - MAXPKTLEN_LINK_SPEED_FE
 *      - MAXPKTLEN_LINK_SPEED_GE
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_portMaxPktLenLinkSpeed_set(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_portMaxPktLenLinkSpeed_set(unit, port, speed, len);
}   /* end of rtk_switch_portMaxPktLenLinkSpeed_set */

/* Function Name:
 *      rtk_switch_portMaxPktLenTagLenCntIncEnable_get
 * Description:
 *      Get include or exclude tag length state of max packet length
 *      in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - the specific port
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_portMaxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_portMaxPktLenTagLenCntIncEnable_get(unit, port, pEnable);
}   /* end of rtk_switch_portMaxPktLenTagLenCntIncEnable_get */

/* Function Name:
 *      rtk_switch_portMaxPktLenTagLenCntIncEnable_set
 * Description:
 *      Set the max packet length to include or exclude tag length
 *      in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - the specific port
 *      enable  - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_portMaxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_portMaxPktLenTagLenCntIncEnable_set(unit, port, enable);
}   /* end of rtk_switch_portMaxPktLenTagLenCntIncEnable_set */

/* Function Name:
 *      rtk_switch_flexTblFmt_get
 * Description:
 *      Get the flexible table format
 * Input:
 *      unit - unit id
 * Output:
 *      pFmt - pointer to the table format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_flexTblFmt_get(uint32 unit, rtk_switch_flexTblFmt_t *pFmt)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_flexTblFmt_get(unit, pFmt);
}   /* end of rtk_switch_flexTblFmt_get */

/* Function Name:
 *      rtk_switch_flexTblFmt_set
 * Description:
 *      Set the flexible table format
 * Input:
 *      unit - unit id
 * Output:
 *      fmt  - table format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_switch_flexTblFmt_set(uint32 unit, rtk_switch_flexTblFmt_t fmt)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->switch_flexTblFmt_set(unit, fmt);
}   /* end of rtk_switch_flexTblFmt_set */
#endif /* #ifdef PHY_ONLY */


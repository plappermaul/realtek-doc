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
 * Purpose : Hardware Abstraction Layer (HAL) control structure and definition in the SDK.
 *
 * Feature : HAL control structure and definition
 *
 */

#ifndef __HAL_COMMON_HALCTRL_H__
#define __HAL_COMMON_HALCTRL_H__

/*
 * Include Files
 */
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <osal/lib.h>
#include <osal/sem.h>
#include <hwp/hw_profile.h>
#include <common/rtcore/rtcore_init.h>
#include <hal/chipdef/driver.h>
#include <hal/chipdef/chipdef.h>
#include <hal/chipdef/allmem.h>


/*
 * Symbol Definition
 */
#define HAL_CHIP_INITED     (1 << 0)    /* the data struct has been initialized */
#define HAL_CHIP_ATTACHED   (1 << 1)    /* the unit were added into system */

#define HAL_MIIM_FIX_PAGE                               0xFFFF

/*
 * Data Type Definition
 */
typedef struct hal_control_s
{
    uint32          chip_flags;
    rt_driver_t     *pChip_driver;
    rt_device_t     *pDev_info;
    rt_phyctrl_t    *pPhy_ctrl[RTK_MAX_PORT_PER_UNIT];

    /* HAL layer semaphore protection mechanism */
    osal_mutex_t    reg_sem;    /* register semaphore */
    osal_mutex_t    tbl_sem[RTK_INDIRECT_CTRL_GROUP_END];    /* table semaphore */
    osal_mutex_t    phy_sem;    /* PHY semaphore */
    osal_mutex_t    miim_sem;    /* PHY semaphore */
    osal_mutex_t    sds_sem;    /* PHY semaphore */
} hal_control_t;

extern hal_control_t hal_ctrl[];
extern rtk_portmask_t rtk_hal_stack_ports[];

/*
 * Macro Definition
 */

/* macro for unit ID and device ID information */
#define HAL_UNIT_TO_DEV_ID(unit)                    (rtk_unit2devID[unit])
#define HAL_DEV_TO_UNIT_ID(devID)                   (rtk_dev2unitID[devID])
#define HAL_STACK_PORT(unit, port)                  ((unit < RTK_MAX_NUM_OF_UNIT) ? ((RTK_PORTMASK_IS_PORT_SET(rtk_hal_stack_ports[unit], port))?1:0) : 0)
#define HAL_STACK_PORTMASK_SET(unit, pPmsk)         hal_stack_portmask_set(unit, pPmsk)
#define HAL_STACK_PORTMASK_GET(unit, pPmsk)         hal_stack_portmask_get(unit, pPmsk)
#define HAL_STACK_PORTMASK_CHK(unit, pPmsk)         (hal_stack_portmask_check(unit, pPmsk) == RT_ERR_OK)

/* macro for driver information */
#define MACDRV(pHalCtrl)                            ((pHalCtrl)->pChip_driver->pMacdrv)
#define HAL_GET_MAC_DRIVER(unit)                    (hal_ctrl[unit].pChip_driver)
#define HAL_GET_MAX_REG_IDX(unit)                   (hal_ctrl[unit].pChip_driver->reg_idx_max)
#define HAL_GET_MAX_REGFIELD_IDX(unit)              (hal_ctrl[unit].pChip_driver->regField_idx_max)
#define HAL_GET_MAX_TABLE_IDX(unit)                 (hal_ctrl[unit].pChip_driver->table_idx_max)
#define HAL_GET_REG_WORD_NUM(unit, reg)             ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset>32)?((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset+31)>>5):1)
#define HAL_IS_PORTMASK_INDEX(unit, reg)            ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->portlist_index)?1:0)
#define HAL_GET_PORTMASK_INDEX(unit, reg)           (hal_ctrl[unit].pChip_driver->pReg_list[reg]->portlist_index)


#define HAL_GET_MACPP_MIN_ADDR(unit)                (hal_ctrl[unit].pDev_info->pMacPpInfo->lowerbound_addr)
#define HAL_GET_MACPP_MAX_ADDR(unit)                (hal_ctrl[unit].pDev_info->pMacPpInfo->upperbound_addr)
#define HAL_GET_MACPP_INTERVAL(unit)                (hal_ctrl[unit].pDev_info->pMacPpInfo->interval)
#define HAL_GET_MACPP_BLK_NUM(unit)                 (hal_ctrl[unit].pDev_info->macPpInfo_blockNum)
#define HAL_GET_MACPP_BLK_MIN_ADDR(unit, blkIdx)    (hal_ctrl[unit].pDev_info->pMacPpInfo[blkIdx].lowerbound_addr)
#define HAL_GET_MACPP_BLK_MAX_ADDR(unit, blkIdx)    (hal_ctrl[unit].pDev_info->pMacPpInfo[blkIdx].upperbound_addr)
#define HAL_GET_MACPP_BLK_INTERVAL(unit, blkIdx)    (hal_ctrl[unit].pDev_info->pMacPpInfo[blkIdx].interval)


#define HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port)   (hal_portMaxBandwidth_ret(unit, port))


#define HAL_MAX_NUM_OF_MIRROR(unit)                 (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mirror)
#define HAL_MAX_NUM_OF_TRUNK(unit)                  (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_trunk)
#define HAL_MAX_NUM_OF_TRUNK_IN_STACKING_MODE(unit) (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_trunk_stacking_mode)
#define HAL_MAX_NUM_OF_STACK_TRUNK(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_stack_trunk)
#define HAL_MAX_NUM_OF_LOCAL_TRUNK(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_local_trunk)
#define HAL_MAX_NUM_OF_TRUNKMEMBER(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_trunkMember)
#define HAL_MAX_NUM_OF_TRUNK_ALGO(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_trunk_algo)
#define HAL_TRUNK_ALGO_SHIFT_MAX(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->trunk_algo_shift_max)
#define HAL_MAX_NUM_OF_DUMB_TRUNKMEMBER(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_dumb_trunkMember)
#define HAL_MAX_NUM_OF_TRUNKHASHVAL(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_trunkHashVal)
#define HAL_MAX_NUM_OF_MSTI(unit)                   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_msti)
#define HAL_MAX_NUM_OF_SVL(unit)                    (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_svl)

/* Meter */
#define HAL_MAX_NUM_OF_METERING(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_metering)
#define HAL_MAX_NUM_OF_METER_BLOCK(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_meter_block)
#define HAL_RATE_OF_METER_MAX(unit)                 (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_meter_max)
#define HAL_BURST_SIZE_OF_ACL_METER_MIN(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_size_of_acl_meter_min)
#define HAL_BURST_SIZE_OF_ACL_METER_MAX(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_size_of_acl_meter_max)

/* PIE */
#define HAL_MAX_NUM_OF_PIE_BLOCK(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_block)
#define HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit)          (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_blockSize)
#define HAL_MAX_NUM_OF_PIE_COUNTER(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_counter)
#define HAL_MAX_NUM_OF_PIE_TEMPLATE(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_template)
#define HAL_PIE_USER_TEMPLATE_ID_MIN(unit)          (hal_ctrl[unit].pDev_info->pCapacityInfo->pie_user_template_id_min)
#define HAL_PIE_USER_TEMPLATE_ID_MAX(unit)          (hal_ctrl[unit].pDev_info->pCapacityInfo->pie_user_template_id_max)
#define HAL_MAX_NUM_OF_FIELD_SELECTOR(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_field_selector)
#define HAL_MAX_OFST_OF_FIELD_SELECTOR(unit)        (hal_ctrl[unit].pDev_info->pCapacityInfo->max_offset_of_field_selector)
#define HAL_MAX_NUM_OF_PIE_FILTER_ID(unit)          (HAL_MAX_NUM_OF_PIE_BLOCK(unit) * HAL_MAX_NUM_OF_PIE_BLOCKSIZE(unit))
#define HAL_MAX_NUM_OF_PIE_BLOCK_TEMPLATESELECTOR(unit) (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_block_templateSelector)
#define HAL_MAX_NUM_OF_PIE_BLOCK_GRP(unit)          (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_block_group)
#define HAL_MAX_NUM_OF_PIE_BLOCK_LOGIC(unit)        (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_block_logic)
#define HAL_MAX_NUM_OF_PIE_TEMPLATE_FIELD(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_pie_template_field)

/* Range check */
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380))
  #define HAL_MAX_NUM_OF_RANGE_CHECK_SRCPORT(unit)  (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_range_check_srcPort)
  #define HAL_MAX_NUM_OF_RANGE_CHECK_DSTPORT(unit)  (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_range_check_dstPort)
  #define HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)      (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_range_check_vid)
  #define HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_range_check_l4Port)
  #define HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_range_check_pktLen)
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)) */
#define HAL_MAX_NUM_OF_RANGE_CHECK_IP(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_range_check_ip)
#define HAL_MAX_NUM_OF_RANGE_CHECK(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_range_check)

/* ACL */
#define HAL_MAX_NUM_OF_METADATA(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_metaData)
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380))
  #define HAL_ACL_RATE_MAX(unit)                    (hal_ctrl[unit].pDev_info->pCapacityInfo->acl_rate_max)
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)) */

#define HAL_L2_HASHDEPTH(unit)                      (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l2_hashdepth)
#define HAL_L2_HASHWIDTH(unit)                      (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l2_hashwidth)
#define HAL_MAX_NUM_OF_QUEUE(unit)                  (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_queue)
#define HAL_MIN_NUM_OF_QUEUE(unit)                  (hal_ctrl[unit].pDev_info->pCapacityInfo->min_num_of_queue)
#define HAL_MAX_NUM_OF_CPU_QUEUE(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_cpuQueue)
#define HAL_MAX_NUM_OF_STACK_QUEUE(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_stackQueue)
#define HAL_MAX_NUM_OF_IGR_QUEUE(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_igrQueue)
#define HAL_MIN_NUM_OF_IGR_QUEUE(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->min_num_of_igrQueue)
#define HAL_MAX_NUM_OF_CVLAN_TPID(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_cvlan_tpid)
#define HAL_MAX_NUM_OF_SVLAN_TPID(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_svlan_tpid)
#define HAL_MAX_NUM_OF_EVLAN_TPID(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_evlan_tpid)
#define HAL_TPID_ENTRY_IDX_MAX(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->tpid_entry_idx_max)
#define HAL_TPID_ENTRY_MASK_MAX(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->tpid_entry_mask_max)
#define HAL_PROTOCOL_VLAN_IDX_MAX(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->protocol_vlan_idx_max)
#define HAL_VLAN_FID_MAX(unit)                      (hal_ctrl[unit].pDev_info->pCapacityInfo->vlan_fid_max)
#define HAL_FLOWCTRL_THRESH_MAX(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->flowctrl_thresh_max)
#define HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_LEN_MAX(unit)  (hal_ctrl[unit].pDev_info->pCapacityInfo->flowctrl_pauseOn_page_packet_len_max)
#define HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX(unit)  (hal_ctrl[unit].pDev_info->pCapacityInfo->flowctrl_pauseOn_page_packet_max)
#define HAL_PRI_OF_SELECTION_MAX(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->pri_of_selection_max)
#define HAL_PRI_OF_SELECTION_MIN(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->pri_of_selection_min)
#define HAL_DP_OF_SELECTION_MAX(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->dp_of_selection_max)
#define HAL_DP_OF_SELECTION_MIN(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->dp_of_selection_min)
#define HAL_PRI_SEL_GROUP_INDEX_MAX(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->pri_sel_group_index_max)
#define HAL_QUEUE_WEIGHT_MAX(unit)                  (hal_ctrl[unit].pDev_info->pCapacityInfo->queue_weight_max)
#define HAL_QUEUE_WEIGHT_MIN(unit)                  (hal_ctrl[unit].pDev_info->pCapacityInfo->queue_weight_min)
#define HAL_IGR_QUEUE_WEIGHT_MAX(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->igr_queue_weight_max)
#define HAL_RATE_OF_EGR_BANDWIDTH_MIN(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_egr_bandwidth_min)
#define HAL_RATE_OF_BANDWIDTH_MAX(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_bandwidth_max)
#define HAL_RATE_OF_10G_BANDWIDTH_MAX(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_bandwidth_max_10ge_port)
#define HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)        (hal_ctrl[unit].pDev_info->pCapacityInfo->busrt_of_igr_bandwidth_max)
#define HAL_BURST_OF_BANDWIDTH_MAX(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->busrt_of_bandwidth_max)
#define HAL_BURST_OF_STORM_CONTROL_MAX(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_of_storm_control_max)
#define HAL_BURST_OF_STORM_PROTO_CONTROL_MAX(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_of_storm_proto_control_max)

#define HAL_THRESH_OF_EGR_QUEUE_DROP_GROUP_IDX_MAX(unit)  (hal_ctrl[unit].pDev_info->pCapacityInfo->flowctrl_egr_queue_drop_group_idx_max)
#define HAL_THRESH_OF_ETE_FC_REMOTE_PORT_GROUP_IDX_MAX(unit)  (hal_ctrl[unit].pDev_info->pCapacityInfo->flowctrl_e2eRemote_port_thresh_group_idx_max)
#define HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)  (hal_ctrl[unit].pDev_info->pCapacityInfo->thresh_of_igr_port_pause_congest_group_idx_max)
#define HAL_THRESH_OF_IGR_BW_FLOWCTRL_MIN(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->thresh_of_igr_bw_flowctrl_min)
#define HAL_THRESH_OF_IGR_BW_FLOWCTRL_MAX(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->thresh_of_igr_bw_flowctrl_max)
#define HAL_MAX_NUM_OF_FASTPATH_OF_RATE(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_fastPath_of_rate)
#define HAL_RATE_OF_STORM_CONTROL_MAX(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_storm_control_max)
#define HAL_RATE_OF_STORM_PROTO_CONTROL_MAX(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->rate_of_storm_proto_control_max)
#define HAL_BURST_RATE_OF_STORM_CONTROL_MIN(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_rate_of_storm_control_min)
#define HAL_BURST_RATE_OF_STORM_CONTROL_MAX(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_rate_of_storm_control_max)
#define HAL_BURST_RATE_OF_10G_STORM_CONTROL_MIN(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_rate_of_10ge_storm_control_min)
#define HAL_BURST_RATE_OF_10G_STORM_CONTROL_MAX(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_rate_of_10ge_storm_control_max)
#define HAL_BURST_RATE_OF_STORM_PROTO_CONTROL_MAX(unit) (hal_ctrl[unit].pDev_info->pCapacityInfo->burst_rate_of_storm_proto_control_max)
#define HAL_INTERNAL_PRIORITY_MAX(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->internal_priority_max)
#define HAL_DROP_PRECEDENCE_MAX(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->drop_precedence_max)
#define HAL_PRIORITY_REMAP_GROUP_IDX_MAX(unit)      (hal_ctrl[unit].pDev_info->pCapacityInfo->priority_remap_group_idx_max)
#define HAL_PRIORITY_REMARK_GROUP_IDX_MAX(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->priority_remark_group_idx_max)
#define HAL_WRED_WEIGHT_MAX(unit)                   (hal_ctrl[unit].pDev_info->pCapacityInfo->wred_weight_max)
#define HAL_WRED_MPD_MAX(unit)                      (hal_ctrl[unit].pDev_info->pCapacityInfo->wred_mpd_max)
#define HAL_WRED_DROP_PROBABILITY_MAX(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->wred_drop_probability_max)
#define HAL_MAX_NUM_OF_L2_HASH_ALGO(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l2_hash_algo)
#define HAL_L2_LEARN_LIMIT_CNT_MAX(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->l2_learn_limit_cnt_max)
#define HAL_L2_LEARN_LIMIT_CNT_WO_CAM_MAX(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->l2_learn_limit_cnt_wo_cam_max)
#define HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit)        (hal_ctrl[unit].pDev_info->pCapacityInfo->l2_learn_limit_cnt_disable)
#define HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)      (hal_ctrl[unit].pDev_info->pCapacityInfo->l2_fid_learn_limit_entry_max)
#define HAL_L2_NTFY_BP_THR_MAX(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->l2_ntfy_bp_thr_max)
#define HAL_MAX_NUM_OF_VRF(unit)                    (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_vrf)
#define HAL_MAX_NUM_OF_INTF(unit)                   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_intf)
#define HAL_MAX_NUM_OF_INTF_MTU(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_intf_mtu)
#define HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_intf_mtu_value)
#define HAL_MAX_NUM_OF_ROUTER_MAC(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_router_mac)
#define HAL_MAX_NUM_OF_L3_ECMP(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_ecmp)
#define HAL_MAX_NUM_OF_L3_ECMP_HASH_IDX(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_ecmp_hash_idx)
#define HAL_MAX_NUM_OF_L3_ECMP_NEXTHOP(unit)        (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_ecmp_nexthop)
#define HAL_MAX_NUM_OF_L3_NEXTHOP(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_nexthop)
#define HAL_MAX_NUM_OF_L3_HOST(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_host)
#define HAL_MAX_NUM_OF_L3_ROUTE(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_route)
#define HAL_MAX_NUM_OF_L3_CONFLICT_HOST(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_conflict_host)
#define HAL_MAX_NUM_OF_L3_MCAST_GROUP_NEXTHOP(unit) (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mcast_group_nexthop)
#define HAL_MAX_NUM_OF_TUNNEL(unit)                 (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_tunnel)
#define HAL_MAX_NUM_OF_TUNNEL_QOS_PROFILE(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_tunnel_qos_profile)
#define HAL_EEE_QUEUE_THRESH_MAX(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->eee_queue_thresh_max)
#define HAL_SEC_MINIPV6FRAGLEN_MAX(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->sec_minIpv6FragLen_max)
#define HAL_SEC_MAXPINGLEN_MAX(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->sec_maxPingLen_max)
#define HAL_SEC_SMURFNETMASKLEN_MAX(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->sec_smurfNetmaskLen_max)
#define HAL_MAX_NUM_OF_IP_MAC_BIND_ENTRY(unit)      (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_ip_mac_bind_entry)
#define HAL_SFLOW_RATE_MAX(unit)                    (hal_ctrl[unit].pDev_info->pCapacityInfo->sflow_rate_max)
#define HAL_MAX_NUM_OF_MCAST_FWD(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mcast_fwd)
#define HAL_MIIM_PAGE_ID_MAX(unit)                  (hal_ctrl[unit].pDev_info->pCapacityInfo->miim_page_id_max)
#define HAL_MAX_NUM_OF_C2SC_ENTRY(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_c2sc_entry)
#define HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_c2sc_blk_entry)
#define HAL_MAX_NUM_OF_C2SC_BLK(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_c2sc_blk)
#define HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_VID(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_c2sc_range_check_vid)
#define HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_SET(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_c2sc_range_check_set)
#define HAL_MAX_NUM_OF_SC2C_ENTRY(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_sc2c_entry)
#define HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_VID(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_sc2c_range_check_vid)
#define HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_SET(unit)   (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_sc2c_range_check_set)
#define HAL_MAX_NUM_OF_VLAN_PROFILE(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_vlan_prof)
#define HAL_MAX_NUM_OF_VLAN_GROUP(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_vlan_group)
#define HAL_MAX_ACCEPT_FRAME_LEN(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->max_frame_len)
#define HAL_MAX_NUM_OF_MCAST_ENTRY(unit)            (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mcast_entry)
#define HAL_MAX_NUM_OF_VLAN_PORT_ISO_ENTRY(unit)    (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_vlan_port_iso_entry)

/* MPLS */
#if defined(CONFIG_SDK_RTL8390)
  #define HAL_MAX_NUM_OF_MPLS_LIB(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mpls_lib)
#endif  /* CONFIG_SDK_RTL8390 */
#if defined(CONFIG_SDK_RTL9310)
#define HAL_MAX_NUM_OF_MPLS_ENCAP(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mpls_encap)
#define HAL_MAX_NUM_OF_MPLS_DECAP(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mpls_decap)
#define HAL_MAX_NUM_OF_MPLS_DECAP_CAM(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mpls_decap_cam)
#define HAL_MAX_NUM_OF_MPLS_DECAP_ENTRY(unit)       (HAL_MAX_NUM_OF_MPLS_DECAP(unit) + HAL_MAX_NUM_OF_MPLS_DECAP_CAM(unit))
#define HAL_MAX_NUM_OF_MPLS_NEXTHOP(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mpls_nextHop)
#define HAL_MPLS_HASHDEPTH(unit)                    (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_mpls_hashdepth)
#endif

/*OpenFlow*/
#define HAL_OF_BYTE_CNTR_TH_MAX(unit)               (hal_ctrl[unit].pDev_info->pCapacityInfo->byte_cntr_th_max)
#define HAL_OF_PKT_CNTR_TH_MAX(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->pkt_cntr_th_max)
#define HAL_MAX_NUM_OF_OF_IGR_FLOWTBL(unit)         (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_igr_flowtbl)
#define HAL_MAX_NUM_OF_OF_VIRTUAL_IGR_FLOWTBL(unit) (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_virtual_igr_flowtbl)
#define HAL_OF_LOOPBACK_MAX(unit)                   (hal_ctrl[unit].pDev_info->pCapacityInfo->loopback_time_max)
#define HAL_OF_MAX_NUM_OF_GRP_ENTRY(unit)           (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_group_entry)
#define HAL_OF_MAX_NUM_OF_GRP_ENTRY_BUCKET(unit)    (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_group_entry_bucket)
#define HAL_OF_MAX_NUM_OF_ACTION_BUCKET(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_action_bucket)
#define HAL_OF_MAX_NUM_OF_DMAC_ENTRY(unit)          (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_dmac_entry)

#define HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)        (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_route_host_addr)
#define HAL_MAX_NUM_OF_ROUTE_SWITCH_ADDR(unit)      (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_route_switch_addr)
#define HAL_MAX_NUM_OF_NEXTHOP(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l3_nexthop)
#define HAL_MAX_NUM_OF_LED_ENTITY(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_led_entity)
#define HAL_MAX_NUM_OF_DYING_GASP_PKT_CNT(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_dying_gasp_pkt_cnt)
#define HAL_DYING_GASP_SUSTAIN_TIME_MAX(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->dying_gasp_sustain_time_max)
#define HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)       (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_rma_user_defined)
#define HAL_TIME_FREQ_MAX(unit)                     (hal_ctrl[unit].pDev_info->pCapacityInfo->time_freq_max)
#define HAL_MAX_NUM_OF_ETHDM_RX_TIMESTAMP(unit)     (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_ethdm_rx_timestamp)

/* BPE */
#define HAL_BPE_PVID_HASHDEPTH(unit)                (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_bpe_pvid_hashdepth)

/* L2 Tunnel */
#define HAL_MAX_NUM_OF_L2TNL_LST(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l2tnl_lst)
#define HAL_MAX_NUM_OF_L2TNL_NODE(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_l2tnl_node)


/* Stacking */
#define HAL_MAX_NUM_OF_STACK_PORT(unit)             (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_stack_port)
#define HAL_MAX_NUM_OF_STACK_DEV(unit)              (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_stack_dev)
#define HAL_MAX_NUM_OF_DEV(unit)                    (hal_ctrl[unit].pDev_info->pCapacityInfo->max_num_of_dev)



/*
 * Function Declaration
 */


/* Function Name:
 *      hal_init
 * Description:
 *      Initialize the hal layer API.
 * Input:
 *      unit    - unit ID
 *      devID   - device ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - General Error
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_DRIVER_NOT_FOUND - The driver can not found
 * Note:
 *      Initialize the hal layer API, include get the chip id and chip revision
 *      id, get its driver id and driver revision id, then bind to its major
 *      driver. Also initialize its hal_ctrl structure for this specified unit.
 *      Before calling the function, bsps should already scan HW interface, like
 *      PCI device in all buses, or physical Lextra, and the total chip numbers,
 *      its chip id and chip revision id is known and store in database in lower
 *      layer.
 */
int32
hal_init(uint32 unit, uint32 devID);

/* Function Name:
 *      hal_ctrlInfo_get
 * Description:
 *      Find the hal control information structure for this specified unit.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of hal control information structure that found
 * Note:
 *      The function have found the exactly hal control information structure.
 */
extern hal_control_t *
hal_ctrlInfo_get(uint32 unit);


/* Function Name:
 *      hal_portMaxBandwidth_ret
 * Description:
 *      Get the max bandwith of port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      max bandwidth value
 * Note:
 *      The return value is different in FE/GE/10GE port.
 */
extern uint32
hal_portMaxBandwidth_ret(uint32 unit, rtk_port_t port);

/* Function Name:
 *      hal_stack_portmask_set
 * Description:
 *      Set stack port database
 * Input:
 *      unit - unit id
 *      pStkPorts - stack ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK - OK
 *      RT_ERR_FAILED - set fail
 * Note:
 *      None
 */
extern int32
hal_stack_portmask_set(uint32 unit, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      hal_stack_portmask_get
 * Description:
 *      Set stack port database
 * Input:
 *      unit - unit id
 * Output:
 *      pPortmask - stack ports
 * Return:
 *      RT_ERR_OK - OK
 *      RT_ERR_FAILED - set fail
 * Note:
 *      None
 */
extern int32
hal_stack_portmask_get(uint32 unit, rtk_portmask_t *pPortmask);

/* Function Name:
 *      hal_stack_portmask_check
 * Description:
 *      Check portmask is all stack port
 * Input:
 *      unit - unit id
 *      pPortmask - portmask to check
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - ports in portmask are all stack ports
 *      RT_ERR_OK  - there are non-stacking port in the portmask
 * Note:
 *      None
 */
uint32
hal_stack_portmask_check(uint32 unit, rtk_portmask_t *pPortmask);

extern void
hal_debug_show_info(uint32 unit, uint32 sequenceNo);

#endif  /* __HAL_COMMON_MALCTRL_H__ */

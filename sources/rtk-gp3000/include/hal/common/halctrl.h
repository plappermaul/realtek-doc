/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
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
#include <hal/chipdef/driver.h>
#include <hal/chipdef/chip.h>
#include <hal/chipdef/allmem.h>


/*
 * Symbol Definition
 */
#define HAL_CHIP_INITED     (1 << 0)
#define HAL_CHIP_ATTACHED   (1 << 1)

/*
 * Data Type Definition
 */
typedef struct hal_control_s
{
    uint32          chip_flags;
    uint32          chip_id;
    uint32          chip_rev_id;
    rt_driver_t     *pChip_driver;
    rt_device_t     *pDev_info;
    rt_phyctrl_t    *pPhy_ctrl[RTK_MAX_NUM_OF_PORTS];
} hal_control_t;

extern hal_control_t hal_ctrl;

/*
 * Macro Definition
 */
/* macro for driver information */
#define MACDRV(pHalCtrl)                        ((pHalCtrl)->pChip_driver->pMacdrv)
#define HAL_GET_MAX_REG_IDX()               (hal_ctrl.pChip_driver->reg_idx_max)
#define HAL_GET_MAX_REGFIELD_IDX()          (hal_ctrl.pChip_driver->regField_idx_max)
#define HAL_GET_MAX_TABLE_IDX()             (hal_ctrl.pChip_driver->table_idx_max)

/* macro for driver register information */
#define HAL_GET_REG_WORD_NUM(reg)          ((HAL_GET_REG_ARRAY_OFFSET(reg)>32)?((HAL_GET_REG_ARRAY_OFFSET(reg)+31)>>5):1)
#define HAL_GET_REG_FIELDS(reg)            (hal_ctrl.pChip_driver->pReg_list[reg].fields)
#define HAL_GET_REG_FIELD_NUM(reg)         (hal_ctrl.pChip_driver->pReg_list[reg].field_num)
#define HAL_GET_REG_LPORT(reg)             (hal_ctrl.pChip_driver->pReg_list[reg].lport)
#define HAL_GET_REG_HPORT(reg)             (hal_ctrl.pChip_driver->pReg_list[reg].hport)
#define HAL_GET_REG_LARRAY(reg)            (hal_ctrl.pChip_driver->pReg_list[reg].larray)
#define HAL_GET_REG_HARRAY(reg)            (hal_ctrl.pChip_driver->pReg_list[reg].harray)
#define HAL_GET_REG_ARRAY_OFFSET(reg)      (hal_ctrl.pChip_driver->pReg_list[reg].array_offset)
#define HAL_GET_REG_ADDR(reg)              (hal_ctrl.pChip_driver->pReg_list[reg].offset)

/* Macro for port information */
#define HAL_GET_PORTNUM()                   (hal_ctrl.pDev_info->pPortinfo->port_number)
#define HAL_IS_PORT_EXIST(port)           RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->all.portmask, port)
#define HAL_IS_ETHER_PORT(port)           RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->ether.portmask, port)
#define HAL_IS_EXT_PORT(port)             RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->ext.portmask, port)
#define HAL_IS_CPU_PORT(port)             RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->cpu.portmask, port)
#define HAL_IS_RGMII_PORT(port)           ((((int32)port == hal_ctrl.pDev_info->pPortinfo->rgmiiPort)? 1:0))
#define HAL_IS_PON_PORT(port)             ((((int32)port == hal_ctrl.pDev_info->pPortinfo->ponPort)? 1:0))
#define HAL_IS_FE_PORT(port)              RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->fe.portmask, port)
#define HAL_IS_INT_FE_PORT(port)          RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->fe_int.portmask, port)
#define HAL_IS_GE_PORT(port)              RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->ge.portmask, port)
#define HAL_IS_SERDES_PORT(port)          RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->serdes.portmask, port)
#define HAL_IS_SWPBO_PORT(port)             RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->swPbo.portmask, port)
#define HAL_IS_XE_PORT(port)              RTK_PORTMASK_IS_PORT_SET(hal_ctrl.pDev_info->pPortinfo->xe.portmask, port)
#define HAL_GET_MACPP_MIN_ADDR()            (hal_ctrl.pDev_info->pMacPpInfo->lowerbound_addr)
#define HAL_GET_MACPP_MAX_ADDR()            (hal_ctrl.pDev_info->pMacPpInfo->upperbound_addr)
#define HAL_GET_MACPP_INTERVAL()            (hal_ctrl.pDev_info->pMacPpInfo->interval)
#define HAL_GET_PORT_TYPE(port)             ((hal_ctrl.pDev_info->pPortinfo->portType[port]))
#define HAL_GET_CPU_PORT()                  ((hal_ctrl.pDev_info->pPortinfo->cpuPort))
#define HAL_GET_MAX_PORT()                  ((hal_ctrl.pDev_info->pPortinfo->all.max))
#define HAL_GET_MIN_PORT()                  ((hal_ctrl.pDev_info->pPortinfo->all.min))
#define HAL_GET_MAX_ETHER_PORT()            ((hal_ctrl.pDev_info->pPortinfo->ether.max))
#define HAL_GET_MIN_ETHER_PORT()            ((hal_ctrl.pDev_info->pPortinfo->ether.min))
#define HAL_GET_MAX_GE_PORT()               ((hal_ctrl.pDev_info->pPortinfo->ge.max))
#define HAL_GET_MIN_GE_PORT()               ((hal_ctrl.pDev_info->pPortinfo->ge.min))
#define HAL_GET_MAX_FE_PORT()               ((hal_ctrl.pDev_info->pPortinfo->fe.max))
#define HAL_GET_MIN_FE_PORT()               ((hal_ctrl.pDev_info->pPortinfo->fe.min))
#define HAL_GET_MAX_INT_FE_PORT()           ((hal_ctrl.pDev_info->pPortinfo->fe_int.max))
#define HAL_GET_MIN_INT_FE_PORT()           ((hal_ctrl.pDev_info->pPortinfo->fe_int.min))
#define HAL_GET_MAX_EXT_PORT()              ((hal_ctrl.pDev_info->pPortinfo->ext.max))
#define HAL_GET_MIN_EXT_PORT()              ((hal_ctrl.pDev_info->pPortinfo->ext.min))
#define HAL_GET_SERDES_PORTNUM()            ((hal_ctrl.pDev_info->pPortinfo->serdes.portNum))
#define HAL_GET_MAX_SERDES_PORT()           ((hal_ctrl.pDev_info->pPortinfo->serdes.max))
#define HAL_GET_MIN_SERDES_PORT()           ((hal_ctrl.pDev_info->pPortinfo->serdes.min))
#define HAL_GET_MAX_CPU_PORT()				((hal_ctrl.pDev_info->pPortinfo->cpu.max))
#define HAL_GET_MIN_CPU_PORT()				((hal_ctrl.pDev_info->pPortinfo->cpu.min))
#define HAL_GET_MAX_SWPBO_PORT()            ((hal_ctrl.pDev_info->pPortinfo->swPbo.max))
#define HAL_GET_MIN_SWPBO_PORT()            ((hal_ctrl.pDev_info->pPortinfo->swPbo.min))
#define HAL_GET_ALL_PORTMASK(dstPortmask)    RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->all.portmask))
#define HAL_GET_ALL_EXT_PORTMASK(dstPortmask)    RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->ext.portmask))
#define HAL_GET_ETHER_PORTMASK(dstPortmask)  RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->ether.portmask))
#define HAL_GET_GE_PORTMASK(dstPortmask)     RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->ge.portmask))
#define HAL_GET_FE_PORTMASK(dstPortmask)     RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->fe.portmask))
#define HAL_GET_SERDES_PORTMASK(dstPortmask)     RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->serdes.portmask))
#define HAL_GET_CPU_PORTMASK(dstPortmask)     RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->cpu.portmask))
#define HAL_GET_XE_PORTMASK(dstPortmask)     RTK_PORTMASK_ASSIGN((dstPortmask), (hal_ctrl.pDev_info->pPortinfo->xe.portmask))
#define HAL_IS_PHY_EXIST(port)            ((hal_ctrl.pPhy_ctrl[port] != NULL)? 1:0)
#define HAL_IS_PHY_8218(port)             (((hal_ctrl.pPhy_ctrl[port]->phy_model_id == PHY_MODEL_ID_RTL8218) || (hal_ctrl.pPhy_ctrl[port]->phy_model_id == PHY_MODEL_ID_RTL8218_TC))? 1:0)
#define HAL_IS_PHY_8214_8214F(port)       (((hal_ctrl.pPhy_ctrl[port]->phy_model_id == PHY_MODEL_ID_RTL8214) || (hal_ctrl.pPhy_ctrl[port]->phy_model_id == PHY_MODEL_ID_RTL8214F))? 1:0)

#define HAL_GET_PON_PORT()                  ((hal_ctrl.pDev_info->pPortinfo->ponPort))
#define HAL_GET_RGMII_PORT()                ((hal_ctrl.pDev_info->pPortinfo->rgmiiPort))
#define HAL_GET_SWPBOLB_PORT()              ((hal_ctrl.pDev_info->pPortinfo->swPboLbPort))

#define HAL_SCAN_ALL_PORT(port)             for(port = HAL_GET_MIN_PORT(); port <= HAL_GET_MAX_PORT(); port++)  if(HAL_IS_PORT_EXIST(port))
#define HAL_SCAN_ALL_ETH_PORT(port)         for(port = HAL_GET_MIN_PORT(); port <= HAL_GET_MAX_PORT(); port++)  if(HAL_IS_ETHER_PORT(port))
#define HAL_SCAN_ALL_EXT_PORT(port)         for(port = HAL_GET_MIN_EXT_PORT(); port <= HAL_GET_MAX_EXT_PORT(); port++)  if(HAL_IS_EXT_PORT(port))
#define HAL_SCAN_ALL_EXT_PORT_EX_CPU(port)  for(port = HAL_GET_MIN_EXT_PORT(); port <= HAL_GET_MAX_EXT_PORT(); port++)  if(HAL_IS_EXT_PORT(port)) if( port != HAL_GET_EXT_CPU_PORT())

/*for 4 bytes portlsit only*/
#define HAL_IS_PORTMASK_VALID(pMask)        ((((pMask.bits[0]) > (hal_ctrl.pDev_info->pPortinfo->all.portmask.bits[0]))) > 0? 0:1)
#define HAL_IS_DSLPORTMASK_VALID(dslPortmask)  ((((dslPortmask.bits[0]) > (hal_ctrl.pDev_info->pPortinfo->dsl.portmask.bits[0]))) > 0? 0:1)
#define HAL_IS_EXTPORTMASK_VALID(extPortmask)  ((((extPortmask.bits[0])> (hal_ctrl.pDev_info->pPortinfo->ext.portmask.bits[0]))) > 0? 0:1)
#define HAL_IS_PORTMASKPRT_VALID(pMask)        ((((pMask->bits[0]) > (hal_ctrl.pDev_info->pPortinfo->all.portmask.bits[0]))) > 0? 0:1)


/* Macro for Capacity */
#define HAL_MAX_NUM_OF_MIRROR()             (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_mirror)
#define HAL_MIRROR_ID_MAX()                 (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_mirror - 1)
#define HAL_MAX_NUM_OF_TRUNK()              (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_trunk)
#define HAL_TRUNK_GROUP_ID_MAX()            (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_trunk - 1)
#define HAL_MAX_NUM_OF_TRUNKMEMBER()        (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_trunkMember)
#define HAL_MAX_NUM_OF_DUMB_TRUNKMEMBER()   (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_dumb_trunkMember)
#define HAL_MAX_NUM_OF_TRUNKHASHVAL()       (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_trunkHashVal)
#define HAL_MAX_NUM_OF_MSTI()               (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_msti)
#define HAL_MAX_NUM_OF_METERING()           (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_metering)
#define HAL_MAX_NUM_OF_FIELD_SELECTOR()     (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_field_selector)
#define HAL_MAX_NUM_OF_RANGE_CHECK_SRCPORT()    (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_range_check_srcPort)
#define HAL_MAX_NUM_OF_RANGE_CHECK_IP()         (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_range_check_ip)
#define HAL_MAX_NUM_OF_RANGE_CHECK_VID()        (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_range_check_vid)
#define HAL_MAX_NUM_OF_RANGE_CHECK_L4PORT()     (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_range_check_l4Port)
#define HAL_MAX_NUM_OF_RANGE_CHECK_PKTLEN()     (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_range_check_pktLen)
#define HAL_MAX_NUM_OF_PATTERN_MATCH_DATA() (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_pattern_match_data)
#define HAL_PATTERN_MATCH_PORT_MAX()        (hal_ctrl.pDev_info->pCapacityInfo->pattern_match_port_max)
#define HAL_PATTERN_MATCH_PORT_MIN()        (hal_ctrl.pDev_info->pCapacityInfo->pattern_match_port_min)
#define HAL_L2_HASHDEPTH()                  (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_l2_hashdepth)
#define HAL_MAX_NUM_OF_QUEUE()              (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_queue)
#define HAL_MIN_NUM_OF_QUEUE()              (hal_ctrl.pDev_info->pCapacityInfo->min_num_of_queue)
#define HAL_MAX_NUM_OF_CVLAN_TPID()         (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_cvlan_tpid)
#define HAL_MAX_NUM_OF_SVLAN_TPID()         (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_svlan_tpid)
#define HAL_TPID_ENTRY_IDX_MAX()            (hal_ctrl.pDev_info->pCapacityInfo->tpid_entry_idx_max)
#define HAL_TPID_ENTRY_MASK_MAX()           (hal_ctrl.pDev_info->pCapacityInfo->tpid_entry_mask_max)
#define HAL_PROTOCOL_VLAN_IDX_MAX()         (hal_ctrl.pDev_info->pCapacityInfo->protocol_vlan_idx_max)
#define HAL_MAX_NUM_OF_VLAN_MBRCFG()        (hal_ctrl.pDev_info->pCapacityInfo->max_num_vlan_mbrcfg)
#define HAL_MAX_VLAN_MBRCFG_IDX()           (hal_ctrl.pDev_info->pCapacityInfo->max_num_vlan_mbrcfg - 1)
#define HAL_VLAN_FID_MAX()                  (hal_ctrl.pDev_info->pCapacityInfo->vlan_fid_max)
#define HAL_FLOWCTRL_THRESH_MAX()           (hal_ctrl.pDev_info->pCapacityInfo->flowctrl_thresh_max)
#define HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX()  (hal_ctrl.pDev_info->pCapacityInfo->flowctrl_pauseOn_page_packet_max)
#define HAL_PRI_OF_SELECTION_MAX()          (hal_ctrl.pDev_info->pCapacityInfo->pri_of_selection_max)
#define HAL_PRI_OF_SELECTION_MIN()          (hal_ctrl.pDev_info->pCapacityInfo->pri_of_selection_min)
#define HAL_PRI_SEL_GROUP_INDEX_MAX()       (hal_ctrl.pDev_info->pCapacityInfo->pri_sel_group_index_max)
#define HAL_PRI_SEL_WEIGHT_MAX()            (hal_ctrl.pDev_info->pCapacityInfo->pri_sel_weight_max)
#define HAL_QUEUE_WEIGHT_MAX()              (hal_ctrl.pDev_info->pCapacityInfo->queue_weight_max)
#define HAL_QUEUE0_WEIGHT_MAX()              (hal_ctrl.pDev_info->pCapacityInfo->qid0_weight_max)
#define HAL_RATE_OF_BANDWIDTH_MAX()         (hal_ctrl.pDev_info->pCapacityInfo->rate_of_bandwidth_max)
#define HAL_THRESH_OF_IGR_BW_FLOWCTRL_MAX() (hal_ctrl.pDev_info->pCapacityInfo->thresh_of_igr_bw_flowctrl_max)
#define HAL_MAX_NUM_OF_FASTPATH_OF_RATE()   (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_fastPath_of_rate)
#define HAL_RATE_OF_STORM_CONTROL_MAX()     (hal_ctrl.pDev_info->pCapacityInfo->rate_of_storm_control_max)
#define HAL_BURST_RATE_OF_STORM_CONTROL_MAX()  (hal_ctrl.pDev_info->pCapacityInfo->burst_rate_of_storm_control_max)
#define HAL_INTERNAL_PRIORITY_MAX()         (hal_ctrl.pDev_info->pCapacityInfo->internal_priority_max)
#define HAL_DROP_PRECEDENCE_MAX()           (hal_ctrl.pDev_info->pCapacityInfo->drop_precedence_max)
#define HAL_PRIORITY_REMAP_GROUP_IDX_MAX()  (hal_ctrl.pDev_info->pCapacityInfo->priority_remap_group_idx_max)
#define HAL_PRIORITY_REMARK_GROUP_IDX_MAX() (hal_ctrl.pDev_info->pCapacityInfo->priority_remark_group_idx_max)
#define HAL_PRIORITY_TO_QUEUE_GROUP_IDX_MAX() (hal_ctrl.pDev_info->pCapacityInfo->priority_to_queue_group_idx_max)
#define HAL_WRED_WEIGHT_MAX()               (hal_ctrl.pDev_info->pCapacityInfo->wred_weight_max)
#define HAL_WRED_MPD_MAX()                  (hal_ctrl.pDev_info->pCapacityInfo->wred_mpd_max)
#define HAL_MAX_NUM_OF_QUEUE()              (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_queue)
#define HAL_QUEUE_ID_MAX()                  (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_queue - 1)
#define HAL_ACL_RATE_MAX()                  (hal_ctrl.pDev_info->pCapacityInfo->acl_rate_max)
#define HAL_L2_LEARN_LIMIT_CNT_MAX()        (hal_ctrl.pDev_info->pCapacityInfo->l2_learn_limit_cnt_max)
#define HAL_L2_LEARN_4WAY_NO()              (hal_ctrl.pDev_info->pCapacityInfo->l2_learn_lut_4way_no)
#define HAL_L2_AGING_TIME_MAX()             (hal_ctrl.pDev_info->pCapacityInfo->l2_aging_time_max)
#define HAL_L2_ENTRY_AGING_MAX()            (hal_ctrl.pDev_info->pCapacityInfo->l2_entry_aging_max)
#define HAL_EEE_QUEUE_THRESH_MAX()          (hal_ctrl.pDev_info->pCapacityInfo->eee_queue_thresh_max)
#define HAL_SEC_MINIPV6FRAGLEN_MAX()        (hal_ctrl.pDev_info->pCapacityInfo->sec_minIpv6FragLen_max)
#define HAL_SEC_MAXPINGLEN_MAX()            (hal_ctrl.pDev_info->pCapacityInfo->sec_maxPingLen_max)
#define HAL_SEC_SMURFNETMASKLEN_MAX()       (hal_ctrl.pDev_info->pCapacityInfo->sec_smurfNetmaskLen_max)
#define HAL_SFLOW_RATE_MAX()                (hal_ctrl.pDev_info->pCapacityInfo->sflow_rate_max)
#define HAL_MAX_NUM_OF_MCAST_FWD()          (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_mcast_fwd)
#define HAL_MIIM_PAGE_ID_MIN()              (hal_ctrl.pDev_info->pCapacityInfo->miim_page_id_min)
#define HAL_MIIM_PAGE_ID_MAX()              (hal_ctrl.pDev_info->pCapacityInfo->miim_page_id_max)
#define HAL_MIIM_REG_ID_MAX()               (hal_ctrl.pDev_info->pCapacityInfo->miim_reg_id_max)
#define HAL_MIIM_DATA_MAX()                 (hal_ctrl.pDev_info->pCapacityInfo->miim_data_max)

#define HAL_L34_NETIF_ENTRY_MAX()           (hal_ctrl.pDev_info->pCapacityInfo->l34_netif_table_max)
#define HAL_L34_ARP_ENTRY_MAX()             (hal_ctrl.pDev_info->pCapacityInfo->l34_arp_table_max)
#define HAL_L34_EXTIP_ENTRY_MAX()           (hal_ctrl.pDev_info->pCapacityInfo->l34_extip_table_max)
#define HAL_L34_ROUTING_ENTRY_MAX()         (hal_ctrl.pDev_info->pCapacityInfo->l34_routing_table_max)
#define HAL_L34_NAPT_ENTRY_MAX()            (hal_ctrl.pDev_info->pCapacityInfo->l34_napt_table_max)
#define HAL_L34_NAPTR_ENTRY_MAX()           (hal_ctrl.pDev_info->pCapacityInfo->l34_naptr_table_max)
#define HAL_L34_NH_ENTRY_MAX()              (hal_ctrl.pDev_info->pCapacityInfo->l34_nh_table_max)
#define HAL_L34_PPPOE_ENTRY_MAX()           (hal_ctrl.pDev_info->pCapacityInfo->l34_pppoe_table_max)
#define HAL_L34_IPV6_ROUTING_ENTRY_MAX()    (hal_ctrl.pDev_info->pCapacityInfo->l34_ipv6_routing_table_max)
#define HAL_L34_IPV6_NEIGHBOR_ENTRY_MAX()   (hal_ctrl.pDev_info->pCapacityInfo->l34_ipv6_neighbor_table_max)
#define HAL_L34_BINDING_ENTRY_MAX()         (hal_ctrl.pDev_info->pCapacityInfo->l34_binding_table_max)
#define HAL_L34_WAN_TYPE_ENTRY_MAX()        (hal_ctrl.pDev_info->pCapacityInfo->l34_wan_type_table_max)
#define HAL_L34_FLOW_ROUTE_ENTRY_MAX()      (hal_ctrl.pDev_info->pCapacityInfo->l34_flow_route_table_max)
#define HAL_L34_DSLITE_ENTRY_MAX()          (hal_ctrl.pDev_info->pCapacityInfo->l34_dslite_table_max)
#define HAL_L34_DSLITE_MC_ENTRY_MAX()       (hal_ctrl.pDev_info->pCapacityInfo->l34_dslite_mc_table_max)
#define HAL_L34_IPMC_TRAN_ENTRY_MAX()       (hal_ctrl.pDev_info->pCapacityInfo->l34_ipmc_tran_table_max)


#define HAL_MAX_NUM_OF_PON_QUEUE()          (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_pon_queue)

#define HAL_MAX_NUM_OF_GPON_TCONT()         (hal_ctrl.pDev_info->pCapacityInfo->gpon_tcont_max)
#define HAL_MAX_NUM_OF_GPON_FLOW()          (hal_ctrl.pDev_info->pCapacityInfo->gpon_flow_max)

#define HAL_CLASSIFY_ENTRY_MAX()            (hal_ctrl.pDev_info->pCapacityInfo->classify_entry_max)
#define HAL_CLASSIFY_SID_NUM()              (hal_ctrl.pDev_info->pCapacityInfo->classify_sid_max)
#define HAL_CLASSIFY_L4PORT_RANGE_NUM()     (hal_ctrl.pDev_info->pCapacityInfo->classify_l4port_range_mum)
#define HAL_CLASSIFY_IP_RANGE_NUM()         (hal_ctrl.pDev_info->pCapacityInfo->classify_ip_range_mum)

#define HAL_MAX_NUM_OF_ACL_RULE_FIELD()     (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_acl_rule_field)
#define HAL_MAX_NUM_OF_ACL_TEMPLATE()       (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_acl_template)
#define HAL_MAX_NUM_OF_ACL_ACTION()         (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_acl_action)
#define HAL_MAX_NUM_OF_ACL_RULE_ENTRY()     (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_acl_rule_entry)

#define HAL_MAX_SPECIAL_CONGEST_SEC()       (hal_ctrl.pDev_info->pCapacityInfo->max_special_congest_second)
#define HAL_ENHANCED_FID_MAX()              (hal_ctrl.pDev_info->pCapacityInfo->max_enhanced_fid)
#define HAL_MAX_NUM_OF_LOG_MIB()            (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_log_mib)
#define HAL_MAX_NUM_OF_HOST()           (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_host)

#define HAL_GET_EXT_CPU_PORT()              (hal_ctrl.pDev_info->pCapacityInfo->ext_cpu_port_id)
#define HAL_PONMAC_TCONT_QUEUE_MAX()        (hal_ctrl.pDev_info->pCapacityInfo->ponmac_tcont_queue_max)
#define HAL_PONMAC_PIR_CIR_RATE_MAX()       (hal_ctrl.pDev_info->pCapacityInfo->ponmac_pir_cir_rate_max)
#define HAL_GET_MIB_LATCH_TIMER_MAX()       (hal_ctrl.pDev_info->pCapacityInfo->max_mib_latch_timer)

#define HAL_MAX_NUM_OF_NEXTHOP()            (hal_ctrl.pDev_info->pCapacityInfo->max_num_of_nexthop)

#define HAL_MAX_RGMII_TX_DELAY()            (hal_ctrl.pDev_info->pCapacityInfo->max_rgmii_tx_delay)
#define HAL_MAX_RGMII_RX_DELAY()            (hal_ctrl.pDev_info->pCapacityInfo->max_rgmii_rx_delay)
#define HAL_MAX_LLID_ENTRY()                (hal_ctrl.pDev_info->pCapacityInfo->max_llid_entry)

/*
 * Function Declaration
 */

/* Function Name:
 *      hal_init
 * Description:
 *      Initialize the hal layer API.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - General Error
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_DRIVER_NOT_FOUND - The driver can not found
 * Note:
 *      Initialize the hal layer API
 */
extern int32
hal_init(void);

/* Function Name:
 *      hal_ctrlInfo_get
 * Description:
 *      Find the hal control information structure for this specified unit.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of hal control information structure that found
 * Note:
 *      The function have found the exactly hal control information structure.
 */
extern hal_control_t *
hal_ctrlInfo_get(void);

#endif  /* __HAL_COMMON_MALCTRL_H__ */

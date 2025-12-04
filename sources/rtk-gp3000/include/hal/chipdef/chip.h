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
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

#ifndef __HAL_CHIPDEF_CHIP_H__
#define __HAL_CHIPDEF_CHIP_H__

/*
 * Include Files
 */

#include <common/rt_type.h>


/*
 * Symbol Definition
 */

/* Definition vendor ID */
#define REALTEK_VENDOR_ID   (0x10EC)

/* Definition chip ID - combine with chip family + version */
#define APOLLO_CHIP_ID          (0x03710000)
#define APOLLOMP_CHIP_ID        (0x96000001)
#define RTL9601B_CHIP_ID        (0x96010002)
#define RTL9602C_CHIP_ID        (0x96030002)
#define RTL9607C_CHIP_ID        (0x96070001)  /*TBD*/
#define LUNA_G3_CHIP_ID         (0xFFFF0001)  /*TBD*/
#define RTL8198F_CHIP_ID        (0xFFFF0001)  /*TBD*/
#define CA8279_CHIP_ID          (0xCA827900)  /*TBD*/
#define CA8277B_CHIP_ID         (0xCA8277B0)  /*TBD*/
#define RTL9603CVD_CHIP_ID      (0x96030003)
#define RTL8277C_CHIP_ID        (0x8277C000)  /*TBD*/
#define RTL9607F_CHIP_ID        (0x9607F000)  /*TBD*/


#define UNKNOWN_CHIP_ID         (0xFFFF0000)


/* Definition chip revision ID */
#define CHIP_REV_ID_0       (0x0)
#define CHIP_REV_ID_A       (0x1)
#define CHIP_REV_ID_B       (0x2)
#define CHIP_REV_ID_C       (0x3)
#define CHIP_REV_ID_D       (0x4)
#define CHIP_REV_ID_E       (0x5)
#define CHIP_REV_ID_F       (0x6)
#define CHIP_REV_ID_G       (0x7)
#define CHIP_REV_ID_MAX     (255)   /* 0xFF */


/* Definition chip attribute flags - bit-wise */
#define CHIP_AFLAG_PCI      (0x1 << 0)
#define CHIP_AFLAG_LEXRA    (0x1 << 1)


/*
 * Data Type Definition
 */

/* Definition RTL model character */
typedef enum rt_model_char_e
{
    RTL_MOEDL_CHAR_NULL = 0,
    RTL_MOEDL_CHAR_A,
    RTL_MOEDL_CHAR_B,
    RTL_MOEDL_CHAR_C,
    RTL_MOEDL_CHAR_D,
    RTL_MOEDL_CHAR_E,
    RTL_MOEDL_CHAR_F,
    RTL_MOEDL_CHAR_G,
    RTL_MOEDL_CHAR_H,
    RTL_MOEDL_CHAR_I,
    RTL_MOEDL_CHAR_J = 10,
    RTL_MOEDL_CHAR_K,
    RTL_MOEDL_CHAR_L,
    RTL_MOEDL_CHAR_M,
    RTL_MOEDL_CHAR_N,
    RTL_MOEDL_CHAR_O,
    RTL_MOEDL_CHAR_P,
    RTL_MOEDL_CHAR_Q,
    RTL_MOEDL_CHAR_R,
    RTL_MOEDL_CHAR_S,
    RTL_MOEDL_CHAR_T = 20,
    RTL_MOEDL_CHAR_U,
    RTL_MOEDL_CHAR_V,
    RTL_MOEDL_CHAR_W,
    RTL_MOEDL_CHAR_X,
    RTL_MOEDL_CHAR_Y,
    RTL_MOEDL_CHAR_Z,
    RTL_MOEDL_CHAR_END = 27
} rt_model_char_t;

/* Definition Structure & Supported Device Lists */
typedef enum rt_supported_device_e
{
#if defined(CONFIG_SDK_APOLLO)
    RT_DEVICE_APOLLO,
    RT_DEVICE_APOLLO_REV_B,
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    RT_DEVICE_APOLLOMP,
#endif
#if defined(CONFIG_SDK_RTL9601B)
    RT_DEVICE_RTL9601,
#endif
#if defined(CONFIG_SDK_RTL9602C)
    RT_DEVICE_RTL9603,
#endif
#if defined(CONFIG_SDK_RTL9607C)
    RT_DEVICE_RTL9607C,
#endif
#if defined(CONFIG_SDK_LUNA_G3)
#if defined(CONFIG_ARCH_RTL8198F)
    RT_DEVICE_RTL8198F,
#else
    RT_DEVICE_LUNA_G3,
#endif
#endif

#if defined(CONFIG_SDK_CA8279)
    RT_DEVICE_CA8279,
#endif
#if defined(CONFIG_SDK_CA8277B)
    RT_DEVICE_CA8277B,
#endif
#if defined(CONFIG_SDK_RTL8277C)
    RT_DEVICE_RTL8277C,
#endif
#if defined(CONFIG_SDK_RTL9607F)
    RT_DEVICE_RTL9607F,
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    RT_DEVICE_RTL9603CVD,
#endif

    RT_DEVICE_END
}rt_supported_device_t;

/* Definition RTL model character */
typedef enum rt_port_type_e
{
    RT_PORT_NONE = 0,
    RT_FE_PORT,
    RT_GE_PORT,
    RT_GE_COMBO_PORT,
    RT_GE_SERDES_PORT,
    RT_CPU_PORT,
    RT_INT_FE_PORT,
    RT_SWPBO_PORT,
    RT_XE_PORT,
    RT_PORT_TYPE_END
} rt_port_type_t;

/* Definition port information for this chip */
typedef struct rt_portinfo_s
{
    rt_port_type_t  portType[RTK_MAX_NUM_OF_PORTS];
    rt_portType_info_t dsl;
    rt_portType_info_t ext;
    rt_portType_info_t swPbo;
    uint32 port_number;
    int32  cpuPort; /* use (-1) for VALUE_NO_INIT */
    rt_portType_info_t cpu;
    rt_portType_info_t fe;
    rt_portType_info_t fe_int;
    rt_portType_info_t ge;
    rt_portType_info_t ge_combo;
    rt_portType_info_t serdes;
    rt_portType_info_t ether;
    rt_portType_info_t all;
    rt_portType_info_t swpbo_lb;
    rt_portType_info_t xe;
    int32  ponPort;
    int32  rgmiiPort;
	int32  swPboLbPort;
} rt_portinfo_t;

/* Define Capacity for each kind of register array */
typedef struct rt_register_capacity_s
{
    uint32  max_num_of_mirror;
    uint32  max_num_of_trunk;
    uint32  max_num_of_trunkMember;
    uint32  max_num_of_dumb_trunkMember;
    uint32  max_num_of_trunkHashVal;
    uint32  max_num_of_msti;
    uint32  max_num_of_metering;
    uint32  max_num_of_field_selector;
    uint32  max_num_of_range_check_srcPort;
    uint32  max_num_of_range_check_ip;
    uint32  max_num_of_range_check_vid;
    uint32  max_num_of_range_check_pktLen;
    uint32  max_num_of_range_check_l4Port;
    uint32  max_num_of_pattern_match_data;
    uint32  pattern_match_port_max;
    uint32  pattern_match_port_min;
    uint32  max_num_of_l2_hashdepth;
    uint32  max_num_of_queue;
    uint32  min_num_of_queue;
    uint32  max_num_of_pon_queue;
    uint32  min_num_of_pon_queue;
    uint32  max_num_of_cvlan_tpid;
    uint32  max_num_of_svlan_tpid;
    uint32  tpid_entry_idx_max;
    uint32  tpid_entry_mask_max;
    uint32  protocol_vlan_idx_max;
    uint32  max_num_vlan_mbrcfg;
    uint32  vlan_fid_max;
    uint32  flowctrl_thresh_max;
    uint32  flowctrl_pauseOn_page_packet_max;
    uint32  pri_of_selection_max;
    uint32  pri_of_selection_min;
    uint32  pri_sel_group_index_max;
    uint32  pri_sel_weight_max;
    uint32  queue_weight_max;
    uint32  qid0_weight_max;
    uint32  rate_of_bandwidth_max;
    uint32  thresh_of_igr_bw_flowctrl_max;
    uint32  rate_of_storm_control_max;
    uint32  burst_rate_of_storm_control_max;
    uint32  max_num_of_fastPath_of_rate;
    uint32  internal_priority_max;
    uint32  drop_precedence_max;
    uint32  priority_remap_group_idx_max;
    uint32  priority_remark_group_idx_max;
    uint32  priority_to_queue_group_idx_max;
    uint32  wred_weight_max;
    uint32  wred_mpd_max;
    uint32  acl_rate_max;
    uint32  l2_learn_limit_cnt_max;
    uint32  l2_learn_lut_4way_no;
    uint32  l2_aging_time_max;
    uint32  l2_entry_aging_max;
    uint32  eee_queue_thresh_max;
    uint32  sec_minIpv6FragLen_max;
    uint32  sec_maxPingLen_max;
    uint32  sec_smurfNetmaskLen_max;
    uint32  sflow_rate_max;
    uint32  max_num_of_mcast_fwd;
    uint32  miim_page_id_min;
    uint32  miim_page_id_max;
    uint32  miim_reg_id_max;
    uint32  miim_data_max;

    uint32  l34_netif_table_max;
    uint32  l34_arp_table_max;
    uint32  l34_extip_table_max;
    uint32  l34_routing_table_max;
    uint32  l34_napt_table_max;
    uint32  l34_naptr_table_max;
    uint32  l34_nh_table_max;
    uint32  l34_pppoe_table_max;
    uint32  l34_ipv6_routing_table_max;
    uint32  l34_ipv6_neighbor_table_max;
    uint32  l34_binding_table_max;
    uint32  l34_wan_type_table_max;
    uint32  l34_dslite_table_max;
    uint32  l34_dslite_mc_table_max;
    uint32  l34_flow_route_table_max;
    uint32  l34_ipmc_tran_table_max;
    uint32  gpon_tcont_max;
    uint32  gpon_flow_max;
    uint32  classify_entry_max;
    uint32  classify_sid_max;
    uint32  classify_l4port_range_mum;
    uint32  classify_ip_range_mum;

    uint32  max_num_of_acl_template;
    uint32  max_num_of_acl_rule_field;
    uint32  max_num_of_acl_action;
    uint32  max_num_of_acl_rule_entry;
    uint32  max_enhanced_fid;

    uint32  max_special_congest_second;
    uint32  max_num_of_log_mib;
    uint32  max_num_of_host;
    uint32  ext_cpu_port_id;
    uint32  ponmac_tcont_queue_max;
    uint32  ponmac_pir_cir_rate_max;
    uint32  max_mib_latch_timer;
    uint32  max_num_of_nexthop;

    uint32  max_rgmii_tx_delay;
    uint32  max_rgmii_rx_delay;

    /*EPON*/
    uint32  max_llid_entry;
} rt_register_capacity_t;

/* Define chip PER_PORT block information */
typedef struct rt_macPpInfo_s
{
    uint32 lowerbound_addr;
    uint32 upperbound_addr;
    uint32 interval;
} rt_macPpInfo_t;

/* Define chip structure */
typedef struct rt_device_s
{
    uint32 chip_id;
    uint32 chip_rev_id;
    uint32 driver_id;
    uint32 driver_rev_id;
    uint32 chip_aflags;
    rt_portinfo_t *pPortinfo;
    rt_register_capacity_t  *pCapacityInfo;
    rt_macPpInfo_t *pMacPpInfo;

} rt_device_t;


/*
 * Function Declaration
 */

/* Function Name:
 *      hal_isPpBlock_check
 * Description:
 *      Check the register is PER_PORT block or not?
 * Input:
 *      addr       - register address
 * Output:
 *      pIsPpBlock - pointer buffer of chip is PER_PORT block?
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
hal_isPpBlock_check(uint32 addr, uint32 *pIsPpBlock);


/* Function Name:
 *      hal_find_device
 * Description:
 *      Find the mac chip from SDK supported mac device lists.
 * Input:
 *      chip_id     - chip id
 *      chip_rev_id - chip revision id
 * Output:
 *      None
 * Return:
 *      NULL        - Not found
 *      Otherwise   - Pointer of mac chip structure that found
 * Note:
 *      The function have take care the forward compatible in revision.
 *      Return one recently revision if no extra match revision.
 */
extern rt_device_t *
hal_find_device(uint32 chip_id, uint32 chip_rev_id);

#endif  /* __HAL_CHIPDEF_CHIP_H__ */

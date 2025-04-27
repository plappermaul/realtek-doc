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
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

#ifndef __HAL_CHIPDEF_CHIPDEF_H__
#define __HAL_CHIPDEF_CHIPDEF_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */

/* Definition chip attribute flags - bit-wise */
#define CHIP_AFLAG_PCI      (0x1 << 0)
#define CHIP_AFLAG_LEXRA    (0x1 << 1)


#define RTL9300_UPLINK_PORT_START   24
#define RTL9300_UPLINK_PORT_END     27
#define RTL9310_UPLINK_PORT_START   52
#define RTL9310_UPLINK_PORT_END     55

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

/* Definition port type */
typedef enum rt_port_type_e
{
    RT_PORT_NONE = 0,
    RT_FE_PORT,
    RT_GE_PORT,
    RT_GE_COMBO_PORT,
    RT_GE_SERDES_PORT,
    RT_2_5GE_PORT,
    RT_2_5GE_FIBER_PORT,
    RT_2_5GE_SERDES_PORT,
    RT_5GE_PORT,
    RT_5GE_FIBER_PORT,
    RT_5GE_SERDES_PORT,
    RT_10GE_PORT,
    RT_10GE_FIBER_PORT,
    RT_10GE_SERDES_PORT,
    RT_CPU_PORT,
    RT_INT_FE_PORT,
    RT_PORT_TYPE_END
} rt_port_type_t;

typedef enum rt_serdesMode_e
{
    RTK_MII_NONE = 0,
    RTK_MII_DISABLE,
    RTK_MII_10GR,
    RTK_MII_RXAUI,
    RTK_MII_RXAUI_LITE,
    RTK_MII_RXAUISGMII_AUTO,
    RTK_MII_RXAUI1000BX_AUTO,
    RTK_MII_RSGMII_PLUS,
    RTK_MII_SGMII,
    RTK_MII_QSGMII,
    RTK_MII_1000BX_FIBER,
    RTK_MII_100BX_FIBER,
    RTK_MII_1000BX100BX_AUTO,
    RTK_MII_10GR1000BX_AUTO,
    RTK_MII_10GRSGMII_AUTO,
    RTK_MII_XAUI,
    RTK_MII_RMII,
    RTK_MII_SMII,
    RTK_MII_SSSMII,
    RTK_MII_RSGMII,
    RTK_MII_XSMII,
    RTK_MII_RS8MII = RTK_MII_XSMII,     /* RTK_MII_RS8MII is obsoleted */
    RTK_MII_XSGMII,
    RTK_MII_QHSGMII,
    RTK_MII_HISGMII,
    RTK_MII_HISGMII_5G,
    RTK_MII_DUAL_HISGMII,
    RTK_MII_2500Base_X,
    RTK_MII_RXAUI_PLUS,
    RTK_MII_USXGMII_10GSXGMII,
    RTK_MII_USXGMII_10GDXGMII,
    RTK_MII_USXGMII_10GQXGMII,
    RTK_MII_USXGMII_5GSXGMII,
    RTK_MII_USXGMII_5GDXGMII,
    RTK_MII_USXGMII_2_5GSXGMII,
    RTK_MII_USXGMII_1G,
    RTK_MII_USXGMII_100M,
    RTK_MII_USXGMII_10M,
    RTK_MII_5GBASEX,
    RTK_MII_5GR,
    RTK_MII_XFI_5G_ADAPT,
    RTK_MII_XFI_5G_CPRI,
    RTK_MII_XFI_2P5G_ADAPT,
    RTK_MII_QUSGMII,
    RTK_MII_OUSGMII,
    RTK_MII_END,
}rt_serdesMode_t;
#define rtk_serdesMode_t            rt_serdesMode_t

typedef enum stack_port_indexType_e
{
    STACK_PORT_INDEX_TYPE_START,
    STACK_PORT_INDEX_TYPE_END,
    STACK_PORT_END
} stack_port_indexType_t;


/* Definition port information for this chip */
typedef struct rt_portinfo_s
{
    rt_port_type_t  portType[RTK_MAX_NUM_OF_PORTS];
} rt_portinfo_t;


/* Define Capacity for each kind of register array */
typedef struct rt_register_capacity_s
{
    /* Mirror */
    uint32  max_num_of_mirror;
    /* Trunk */
    uint32  max_num_of_trunk;
    uint32  max_num_of_trunk_stacking_mode;
    uint32  max_num_of_local_trunk;
    uint32  max_num_of_stack_trunk;
    uint32  max_num_of_trunkMember;
    uint32  max_num_of_trunk_algo;
    uint32  trunk_algo_shift_max;
    uint32  max_num_of_dumb_trunkMember;
    uint32  max_num_of_trunkHashVal;
    /* STP */
    uint32  max_num_of_msti;
    /* Meter */
    uint32  max_num_of_metering;
    uint32  max_num_of_meter_block;
    uint32  rate_of_meter_max;
    uint32  burst_size_of_acl_meter_min;
    uint32  burst_size_of_acl_meter_max;
    /* PIE */
    uint32  max_num_of_pie_block;
    uint32  max_num_of_pie_blockSize;
    uint32  max_num_of_pie_counter;
    uint32  max_num_of_pie_template;
    uint32  pie_user_template_id_min;
    uint32  pie_user_template_id_max;
    uint32  max_num_of_field_selector;
    uint32  max_offset_of_field_selector;
    uint32  max_num_of_pie_block_templateSelector;
    uint32  max_num_of_pie_block_group;
    uint32  max_num_of_pie_block_logic;
    uint32  max_num_of_pie_template_field;
    /* Range check */
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380))
    uint32  max_num_of_range_check_srcPort;
    uint32  max_num_of_range_check_dstPort;
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)) */
    uint32  max_num_of_range_check_ip;
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380))
    uint32  max_num_of_range_check_vid;
    uint32  max_num_of_range_check_l4Port;
    uint32  max_num_of_range_check_pktLen;
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)) */
    uint32  max_num_of_range_check;
    /* ACL */
    uint32  max_num_of_metaData;
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380))
    uint32  acl_rate_max;
#endif  /* (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)) */
    /* L2 */
    uint32  max_num_of_l2_hashdepth;/* physical allocated memory view */
    uint32  max_num_of_l2_hashwidth;/* table access view */
    uint32  l2_learn_limit_cnt_max;
    uint32  l2_learn_limit_cnt_wo_cam_max;
    uint32  l2_learn_limit_cnt_disable;
    uint32  l2_fid_learn_limit_entry_max;
    uint32  l2_ntfy_bp_thr_max;
    /* L3 */
    uint32  max_num_of_vrf;
    uint32  max_num_of_intf;
    uint32  max_num_of_intf_mtu;
    uint32  max_num_of_intf_mtu_value;
    uint32  max_num_of_router_mac;
    uint32  max_num_of_l3_ecmp;
    uint32  max_num_of_l3_ecmp_hash_idx;
    uint32  max_num_of_l3_ecmp_nexthop;
    uint32  max_num_of_l3_nexthop;
    uint32  max_num_of_l3_host;
    uint32  max_num_of_l3_route;
    uint32  max_num_of_l3_conflict_host;
    uint32  max_num_of_mcast_group_nexthop;

    uint32  max_num_of_route_host_addr;
    uint32  max_num_of_route_switch_addr;
    uint32  max_num_of_next_hop;

    /* Tunnel */
    uint32  max_num_of_tunnel;
    uint32  max_num_of_tunnel_qos_profile;
    /* QoS */
    uint32  max_num_of_queue;
    uint32  min_num_of_queue;
    uint32  max_num_of_cpuQueue;
    uint32  max_num_of_stackQueue;
    uint32  max_num_of_igrQueue;
    uint32  min_num_of_igrQueue;
    uint32  dp_of_selection_max;
    uint32  dp_of_selection_min;
    uint32  pri_of_selection_max;
    uint32  pri_of_selection_min;
    uint32  pri_sel_group_index_max;
    uint32  queue_weight_max;
    uint32  queue_weight_min;
    uint32  igr_queue_weight_max;
    uint32  internal_priority_max;
    uint32  drop_precedence_max;
    uint32  priority_remap_group_idx_max;
    uint32  priority_remark_group_idx_max;
    uint32  wred_weight_max;
    uint32  wred_mpd_max;
    uint32  wred_drop_probability_max;
    /* VLAN */
    uint32  max_num_of_svl;
    uint32  max_num_of_cvlan_tpid;
    uint32  max_num_of_svlan_tpid;
    uint32  max_num_of_evlan_tpid;
    uint32  tpid_entry_idx_max;
    uint32  tpid_entry_mask_max;
    uint32  protocol_vlan_idx_max;
    uint32  vlan_fid_max;
    uint32  max_num_of_vlan_prof;
    uint32  max_num_of_vlan_group;
    uint32  max_num_of_c2sc_entry;
    uint32  max_num_of_c2sc_blk_entry;
    uint32  max_num_of_c2sc_blk;
    uint32  max_num_of_c2sc_range_check_vid;
    uint32  max_num_of_c2sc_range_check_set;
    uint32  max_num_of_sc2c_entry;
    uint32  max_num_of_sc2c_range_check_vid;
    uint32  max_num_of_sc2c_range_check_set;

    /* flow control */
    uint32  flowctrl_thresh_max;
    uint32  flowctrl_pauseOn_page_packet_len_max;
    uint32  flowctrl_pauseOn_page_packet_max;
    uint32  flowctrl_egr_queue_drop_group_idx_max;
    uint32  flowctrl_e2eRemote_port_thresh_group_idx_max;
    /* bandwidth */
    uint32  rate_of_egr_bandwidth_min;
    uint32  rate_of_bandwidth_max;
    uint32  rate_of_bandwidth_max_fe_port;
    uint32  rate_of_bandwidth_max_ge_port;
    uint32  rate_of_bandwidth_max_10ge_port;
    uint32  busrt_of_igr_bandwidth_max;
    uint32  busrt_of_bandwidth_max;
    uint32  thresh_of_igr_port_pause_congest_group_idx_max;
    uint32  thresh_of_igr_bw_flowctrl_min;
    uint32  thresh_of_igr_bw_flowctrl_max;
    /* Storm control */
    uint32  rate_of_storm_control_max;
    uint32  burst_of_storm_control_max;
    uint32  rate_of_storm_proto_control_max;
    uint32  burst_of_storm_proto_control_max;
    uint32  burst_rate_of_storm_control_min;
    uint32  burst_rate_of_storm_control_max;
    uint32  burst_rate_of_10ge_storm_control_min;
    uint32  burst_rate_of_10ge_storm_control_max;
    uint32  burst_rate_of_storm_proto_control_max;
    uint32  max_num_of_fastPath_of_rate;
    uint32  max_num_of_l2_hash_algo;

    /* EEE */
    uint32  eee_queue_thresh_max;
    /* Security */
    uint32  sec_minIpv6FragLen_max;
    uint32  sec_maxPingLen_max;
    uint32  sec_smurfNetmaskLen_max;
    uint32  max_num_of_ip_mac_bind_entry;
    /* sFlow */
    uint32  sflow_rate_max;
    /* MPLS */
    uint32  max_num_of_mpls_lib;
    uint32  max_num_of_mpls_encap;
    uint32  max_num_of_mpls_decap;
    uint32  max_num_of_mpls_decap_cam;
    uint32  max_num_of_mpls_nextHop;
    uint32  max_num_of_mpls_hashdepth;
    /*OpenFlow*/
    uint64  byte_cntr_th_max;
    uint64  pkt_cntr_th_max;
    uint32  max_num_of_igr_flowtbl;
    uint32  max_num_of_virtual_igr_flowtbl;
    uint32  loopback_time_max;
    uint32  max_num_of_group_entry;
    uint32  max_num_of_group_entry_bucket;
    uint32  max_num_of_action_bucket;
    uint32  max_num_of_dmac_entry;
    uint32  max_num_of_bpe_pvid_hashdepth;

    /* L2 tunnel */
    uint32  max_num_of_l2tnl_lst;
    uint32  max_num_of_l2tnl_node;

    /* stacking */
    uint32  max_num_of_stack_port;
    uint32  max_num_of_stack_dev;

    uint32  max_num_of_dev;

    /* XXX */
    uint32  max_num_of_mcast_fwd;
    uint32  miim_page_id_max;
    uint32  max_frame_len;
    uint32  max_num_of_mcast_entry;
    uint32  max_num_of_vlan_port_iso_entry;
    uint32  max_num_of_led_entity;
    uint32  max_num_of_dying_gasp_pkt_cnt;
    uint32  dying_gasp_sustain_time_max;
    uint32  max_num_of_rma_user_defined;
    uint32  time_freq_max;
    uint32  max_num_of_ethdm_rx_timestamp;
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
    uint32 chip_family_id;
    rt_portinfo_t *pPortinfo;
    rt_register_capacity_t  *pCapacityInfo;
    uint32 macPpInfo_blockNum;
    rt_macPpInfo_t *pMacPpInfo;
} rt_device_t;


/*
 * Function Declaration
 */

/* Function Name:
 *      hal_clone_device
 * Description:
 *      Clone pDev
 * Input:
 *      pDev        - MAC device memory structure
 * Output:
 *      None
 * Return:
 *      NULL        - clone fail
 *      Otherwise   - Pointer of mac chip structure that cloned
 * Note:
 *      The function have take care the forward compatible in revision.
 *      Return one recently revision if no extra match revision.
 */
rt_device_t *
hal_clone_device(rt_device_t *pDev);

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
 *      Pointer of mac chip structure that found
 * Note:
 *      The function have take care the forward compatible in revision.
 *      Return one recently revision if no extra match revision.
 */
extern rt_device_t *
hal_find_device(uint32 chip_id, uint32 chip_rev_id);

/* Function Name:
 *      hal_get_driver_id
 * Description:
 *      Get its driver of the mac chip based on chip id and chip revision id.
 * Input:
 *      chip_id        - chip id
 *      chip_rev_id    - chip revision id
 * Output:
 *      pDriver_id     - pointer buffer of driver id
 *      pDriver_rev_id - pointer buffer of driver revision id
 * Return:
 *      RT_ERR_OK             - OK
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_FOUND - The chip can not found
 * Note:
 *      None
 */
extern int32
hal_get_driver_id(
    uint32 chip_id,
    uint32 chip_rev_id,
    uint32 *pDriver_id,
    uint32 *pDriver_rev_id);

/* Function Name:
 *      hal_get_chip_id
 * Description:
 *      Get chip id and chip revision id.
 * Input:
 *      unit           - unit id
 * Output:
 *      pChip_id       - pointer buffer of chip id
 *      pChip_rev_id   - pointer buffer of chip revision id
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 */
extern int32
hal_get_chip_id(uint32 unit, uint32 *pChip_id, uint32 *pChip_rev_id);

/* Function Name:
 *      hal_isPpBlock_check
 * Description:
 *      Check the register is PER_PORT block or not?
 * Input:
 *      unit       - unit id
 *      addr       - register address
 * Output:
 *      pIsPpBlock - pointer buffer of chip is PER_PORT block?
 *      pPpBlockIdx - pointer buffer of PER_PORT block index
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
hal_isPpBlock_check(uint32 unit, uint32 addr, uint32 *pIsPpBlock, uint32 *pPpBlockIdx);


#endif  /* __HAL_CHIPDEF_CHIPDEF_H__ */


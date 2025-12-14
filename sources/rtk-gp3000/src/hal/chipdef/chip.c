/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <ioal/mem32.h>
#include <hal/chipdef/chip.h>
#include <hal/common/halctrl.h>

#if defined(CONFIG_SDK_APOLLO)
#include <hal/chipdef/apollo/apollo_def.h>
#endif

#if defined(CONFIG_SDK_APOLLOMP)
#include <hal/chipdef/apollomp/apollomp_def.h>
#endif

#if defined(CONFIG_SDK_RTL9601B)
#include <hal/chipdef/rtl9601b/rtl9601b_def.h>
#endif


#if defined(CONFIG_SDK_RTL9602C)
#include <hal/chipdef/rtl9602c/rtl9602c_def.h>
#endif

#if defined(CONFIG_SDK_RTL9607C)
#include <hal/chipdef/rtl9607c/rtl9607c_def.h>
#endif

#if defined(CONFIG_SDK_LUNA_G3)
#if defined(CONFIG_ARCH_RTL8198F)
#include <hal/chipdef/rtl8198f/rtl8198f_def.h>
#else
#include <hal/chipdef/luna_g3/luna_g3_def.h>
#endif
#endif

#if defined(CONFIG_SDK_CA8279)
#include <hal/chipdef/ca8279/ca8279_def.h>
#endif

#if defined(CONFIG_SDK_CA8277B)
#include <hal/chipdef/ca8277b/ca8277b_def.h>
#endif

#if defined(CONFIG_SDK_RTL8277C)
#include <hal/chipdef/rtl8277c/rtl8277c_def.h>
#endif

#if defined(CONFIG_SDK_RTL9607F)
#include <hal/chipdef/rtl9607f/rtl9607f_def.h>
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
#include <hal/chipdef/rtl9603cvd/rtl9603cvd_def.h>
#endif

/*
 * Symbol Definition
 */
#define VALUE_NO_INIT   (-1)

/*
 * Data Declaration
 */
#if defined(CONFIG_SDK_APOLLO)
static rt_portinfo_t apollo_port_info =
{
    /* Normal APOLLO Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_CPU_PORT    /*P4 */,
        RT_PORT_NONE  /*P5 */, RT_CPU_PORT   /*P6 */, RT_PORT_NONE /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE   /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	16, /*port number*/
     	15,	/*max*/
     	0,  /*min*/
     	{{0xFFFF}}/*port mask*/
     },
     /*extension port member*/
     {
     	5, /*port number*/
     	4,	/*max*/
     	0,  /*min*/
     	{{0x3F}}/*port mask*/
     },
     .serdes     = \
     {
        0,  /* port number */
        VALUE_NO_INIT,  /* max */
        VALUE_NO_INIT,  /* min */
        {{0x0}} /* port mask */
     },
     .ponPort   = 3,
     .rgmiiPort = 2,
     .swPboLbPort = -1
}; /* end of apollo_port_info */

/*
Apollo port mapping

UTP0               0
UTP1               1
RGMII              2
PON/fiber/UTP4     3
UTP2               4
UTP3               5
CPU port           6

*/



/* Normal apollo Chip Port Information */
static rt_register_capacity_t apollo_capacityInfo =
{
    .max_num_of_mirror                  = APOLLO_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = APOLLO_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = APOLLO_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = APOLLO_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = APOLLO_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = APOLLO_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = APOLLO_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = APOLLO_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = APOLLO_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = APOLLO_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = APOLLO_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = APOLLO_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = APOLLO_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = APOLLO_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = APOLLO_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = APOLLO_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = APOLLO_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = APOLLO_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = APOLLO_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = APOLLO_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = APOLLO_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = APOLLO_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = APOLLO_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = APOLLO_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = APOLLO_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = APOLLO_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = APOLLO_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = APOLLO_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = APOLLO_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = APOLLO_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = APOLLO_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = APOLLO_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = APOLLO_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = APOLLO_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = APOLLO_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = APOLLO_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = APOLLO_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = APOLLO_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = APOLLO_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = APOLLO_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = APOLLO_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = APOLLO_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = APOLLO_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = APOLLO_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = APOLLO_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = APOLLO_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = APOLLO_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = APOLLO_WRED_MPD_MAX                    ,
    .acl_rate_max                       = APOLLO_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = APOLLO_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = APOLLO_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = APOLLO_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = APOLLO_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = APOLLO_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = APOLLO_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = APOLLO_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = APOLLO_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = APOLLO_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = APOLLO_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = APOLLO_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = APOLLO_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = APOLLO_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = APOLLO_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = APOLLO_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = APOLLO_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = APOLLO_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = APOLLO_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = APOLLO_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = APOLLO_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = APOLLO_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = APOLLO_L34_PPPOE_TABLE_MAX             ,
    .gpon_tcont_max                     = APOLLO_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = APOLLO_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = APOLLO_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = APOLLO_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = APOLLO_CLASSIFY_L4PORT_RANG_NUM        ,
    .classify_ip_range_mum              = APOLLO_CLASSIFY_IP_RANG_NUM            ,
    .max_num_of_acl_template            = APOLLO_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = APOLLO_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = APOLLO_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = APOLLO_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = APOLLO_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = APOLLO_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = APOLLO_MAX_NUM_OF_LOG_MIB              ,
    .ext_cpu_port_id                    = APOLLO_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = APOLLO_PONMAC_TCONT_QUEUE_MAX          ,
    .ponmac_pir_cir_rate_max            = APOLLO_PONMAC_PIR_CIR_RATE_MAX         ,
    .max_mib_latch_timer                = APOLLO_MAX_MIB_LATCH_TIMER             ,
    .max_rgmii_tx_delay                 = APOLLO_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = APOLLO_MAX_RGMII_RX_DELAY              ,
};

/* Normal APOLLO Chip PER_PORT block information */
static rt_macPpInfo_t apollo_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x400,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_APOLLO) */



#if defined(CONFIG_SDK_APOLLOMP)
static rt_portinfo_t apollomp_port_info =
{
    /* Normal APOLLO Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT    /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT     /*P4 */,
        RT_GE_PORT    /*P5 */, RT_CPU_PORT    /*P6 */, RT_PORT_NONE  /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE   /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	16, /*port number*/
     	15,	/*max*/
     	0,  /*min*/
     	{{0xFFFF}}/*port mask*/
     },
     /*extension port member*/
     {
     	6, /*port number*/
     	5,	/*max*/
     	0,  /*min*/
     	{{0x3F}}/*port mask*/
     },
     .serdes     = \
     {
        0,  /* port number */
        VALUE_NO_INIT,  /* max */
        VALUE_NO_INIT,  /* min */
        {{0x0}} /* port mask */
     },
     .ponPort   = 4,
     .rgmiiPort = 5,
     .swPboLbPort = -1
}; /* end of apollomp_port_info */



/*
Apollo MP port mapping

UTP0               0
UTP1               1
UTP2               2
UTP3               3
PON/fiber/UTP4     4
RGMII              5
CPU port           6

*/



/* Normal apollo MP Chip Port Information */
static rt_register_capacity_t apollomp_capacityInfo =
{
    .max_num_of_mirror                  = APOLLOMP_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = APOLLOMP_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = APOLLOMP_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = APOLLOMP_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = APOLLOMP_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = APOLLOMP_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = APOLLOMP_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = APOLLOMP_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = APOLLOMP_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = APOLLOMP_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = APOLLOMP_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = APOLLOMP_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = APOLLOMP_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = APOLLOMP_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = APOLLOMP_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = APOLLOMP_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = APOLLOMP_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = APOLLOMP_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = APOLLOMP_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = APOLLOMP_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = APOLLOMP_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = APOLLOMP_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = APOLLOMP_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = APOLLOMP_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = APOLLOMP_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = APOLLOMP_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = APOLLOMP_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = APOLLOMP_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = APOLLOMP_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = APOLLOMP_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = APOLLOMP_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = APOLLOMP_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = APOLLOMP_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = APOLLOMP_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = APOLLOMP_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = APOLLOMP_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = APOLLOMP_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = APOLLOMP_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = APOLLOMP_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = APOLLOMP_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = APOLLOMP_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = APOLLOMP_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = APOLLOMP_WRED_MPD_MAX                    ,
    .acl_rate_max                       = APOLLOMP_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = APOLLOMP_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = APOLLOMP_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = APOLLOMP_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = APOLLOMP_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = APOLLOMP_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = APOLLOMP_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = APOLLOMP_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = APOLLOMP_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = APOLLOMP_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = APOLLOMP_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = APOLLOMP_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = APOLLOMP_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = APOLLOMP_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = APOLLOMP_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = APOLLOMP_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = APOLLOMP_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = APOLLOMP_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = APOLLOMP_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = APOLLOMP_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = APOLLOMP_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = APOLLOMP_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = APOLLOMP_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max       = APOLLOMP_L34_IPV6_ROUTING_TABLE_MAX,
    .l34_ipv6_neighbor_table_max    =  APOLLOMP_L34_IPV6_NBR_TABLE_MAX,
    .l34_binding_table_max	         =  APOLLOMP_L34_BINDING_TABLE_MAX,
    .l34_wan_type_table_max	  =  APOLLOMP_L34_WAN_TYPE_TABLE_MAX,
    .l34_ipmc_tran_table_max            = APOLLOMP_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     =   APOLLOMP_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = APOLLOMP_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = APOLLOMP_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = APOLLOMP_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = APOLLOMP_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = APOLLOMP_CLASSIFY_IP_RANGE_NUM            ,
    .max_num_of_acl_template            = APOLLOMP_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = APOLLOMP_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = APOLLOMP_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = APOLLOMP_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = APOLLOMP_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = APOLLOMP_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = APOLLOMP_MAX_NUM_OF_LOG_MIB              ,
    .ext_cpu_port_id                    = APOLLOMP_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = APOLLOMP_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = APOLLOMP_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = APOLLOMP_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop               	= APOLLOMP_MAX_NUM_OF_NEXTHOP			   ,
    .max_rgmii_tx_delay                 = APOLLOMP_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = APOLLOMP_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = APOLLOMP_MAX_LLID_ENTRY              ,

};


/* Normal APOLLO MP Chip PER_PORT block information */
static rt_macPpInfo_t apollomp_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x400,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_APOLLOMP) */




#if defined(CONFIG_SDK_RTL9601B)
static rt_portinfo_t rtl9601b_port_info =
{
    /* Normal RTL9601B Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_CPU_PORT   /*P2 */, RT_PORT_NONE  /*P3 */, RT_PORT_NONE     /*P4 */,
        RT_PORT_NONE  /*P5 */, RT_PORT_NONE  /*P6 */, RT_PORT_NONE  /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE  /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE  /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE   /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE  /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE  /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	16, /*port number*/
     	15,	/*max*/
     	0,  /*min*/
     	{{0xFFFF}}/*port mask*/
     },
     /*extension port member*/
     {
     	0, /*port number*/
     	0,	/*max*/
     	0,  /*min*/
     	{{0x0000}}/*port mask*/
     },
     .serdes     = \
     {
        0,  /* port number */
        VALUE_NO_INIT,  /* max */
        VALUE_NO_INIT,  /* min */
        {{0x0}} /* port mask */
     },
     .ponPort   = 1,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of rtl9601b_port_info */

/*
RTL9601B port mapping

UTP0               0
PON/fiber/UTP1     1
CPU port           2

*/



/* Normal RTL9601B Chip Port Information */
static rt_register_capacity_t rtl9601b_capacityInfo =
{
    .max_num_of_mirror                  = RTL9601B_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = RTL9601B_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = RTL9601B_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = RTL9601B_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = RTL9601B_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = RTL9601B_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = RTL9601B_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = RTL9601B_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = RTL9601B_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = RTL9601B_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = RTL9601B_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = RTL9601B_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = RTL9601B_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = RTL9601B_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = RTL9601B_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = RTL9601B_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = RTL9601B_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = RTL9601B_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = RTL9601B_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = RTL9601B_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = RTL9601B_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = RTL9601B_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = RTL9601B_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = RTL9601B_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = RTL9601B_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = RTL9601B_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = RTL9601B_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = RTL9601B_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = RTL9601B_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = RTL9601B_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = RTL9601B_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = RTL9601B_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = RTL9601B_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = RTL9601B_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = RTL9601B_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = RTL9601B_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = RTL9601B_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = RTL9601B_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = RTL9601B_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = RTL9601B_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = RTL9601B_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = RTL9601B_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = RTL9601B_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = RTL9601B_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = RTL9601B_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = RTL9601B_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = RTL9601B_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = RTL9601B_WRED_MPD_MAX                    ,
    .acl_rate_max                       = RTL9601B_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = RTL9601B_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = RTL9601B_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = RTL9601B_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = RTL9601B_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = RTL9601B_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = RTL9601B_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = RTL9601B_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = RTL9601B_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = RTL9601B_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = RTL9601B_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = RTL9601B_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = RTL9601B_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = RTL9601B_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = RTL9601B_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = RTL9601B_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = RTL9601B_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = RTL9601B_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = RTL9601B_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = RTL9601B_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = RTL9601B_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = RTL9601B_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = RTL9601B_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = RTL9601B_L34_IPV6_ROUTING_TABLE_MAX,
    .l34_ipv6_neighbor_table_max        = RTL9601B_L34_IPV6_NBR_TABLE_MAX,
    .l34_binding_table_max	            = RTL9601B_L34_BINDING_TABLE_MAX,
    .l34_wan_type_table_max	            = RTL9601B_L34_WAN_TYPE_TABLE_MAX,
    .gpon_tcont_max                     = RTL9601B_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = RTL9601B_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = RTL9601B_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = RTL9601B_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = RTL9601B_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = RTL9601B_CLASSIFY_IP_RANGE_NUM            ,
    .max_num_of_acl_template            = RTL9601B_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = RTL9601B_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = RTL9601B_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = RTL9601B_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = RTL9601B_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = RTL9601B_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = RTL9601B_MAX_NUM_OF_LOG_MIB              ,
    .ext_cpu_port_id                    = RTL9601B_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = RTL9601B_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = RTL9601B_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = RTL9601B_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop               	= RTL9601B_MAX_NUM_OF_NEXTHOP			   ,
    .max_rgmii_tx_delay                 = RTL9601B_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = RTL9601B_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = RTL9601B_MAX_LLID_ENTRY              ,

};



/* Normal RTL9601B Chip PER_PORT block information */
static rt_macPpInfo_t rtl9601b_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x400,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_RTL9601B) */




#if defined(CONFIG_SDK_RTL9602C)
static rt_portinfo_t rtl9602c_port_info =
{
    /* Normal RTL9602C Chip Port Information */
    {
        RT_FE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_CPU_PORT   /*P3 */, RT_PORT_NONE   /*P4 */,
        RT_PORT_NONE  /*P5 */, RT_PORT_NONE  /*P6 */, RT_PORT_NONE /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE   /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	0, /*port number*/
     	0,	/*max*/
     	0,  /*min*/
     	{{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
     	7, /*port number*/
     	6,	/*max*/
     	0,  /*min*/
     	{{0x7f}}/*port mask*/
     },
     .serdes     = \
     {
        0,  /* port number */
        VALUE_NO_INIT,  /* max */
        VALUE_NO_INIT,  /* min */
        {{0x0}} /* port mask */
     },
     .ponPort   = 2,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of rtl9602c_port_info */

/*
RTL9602C port mapping

UTP0               0
PON/fiber/UTP1     1
CPU port           2

*/



/* Normal RTL9602C Chip Port Information */
static rt_register_capacity_t rtl9602c_capacityInfo =
{
    .max_num_of_mirror                  = RTL9602C_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = RTL9602C_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = RTL9602C_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = RTL9602C_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = RTL9602C_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = RTL9602C_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = RTL9602C_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = RTL9602C_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = RTL9602C_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = RTL9602C_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = RTL9602C_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = RTL9602C_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = RTL9602C_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = RTL9602C_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = RTL9602C_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = RTL9602C_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = RTL9602C_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = RTL9602C_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = RTL9602C_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = RTL9602C_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = RTL9602C_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = RTL9602C_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = RTL9602C_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = RTL9602C_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = RTL9602C_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = RTL9602C_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = RTL9602C_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = RTL9602C_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = RTL9602C_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = RTL9602C_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = RTL9602C_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = RTL9602C_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = RTL9602C_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = RTL9602C_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = RTL9602C_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = RTL9602C_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = RTL9602C_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = RTL9602C_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = RTL9602C_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = RTL9602C_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = RTL9602C_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = RTL9602C_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = RTL9602C_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = RTL9602C_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = RTL9602C_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = RTL9602C_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = RTL9602C_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = RTL9602C_WRED_MPD_MAX                    ,
    .acl_rate_max                       = RTL9602C_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = RTL9602C_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = RTL9602C_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = RTL9602C_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = RTL9602C_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = RTL9602C_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = RTL9602C_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = RTL9602C_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = RTL9602C_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = RTL9602C_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = RTL9602C_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = RTL9602C_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = RTL9602C_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = RTL9602C_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = RTL9602C_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = RTL9602C_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = RTL9602C_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = RTL9602C_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = RTL9602C_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = RTL9602C_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = RTL9602C_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = RTL9602C_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = RTL9602C_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = RTL9602C_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = RTL9602C_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max	            = RTL9602C_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max	            = RTL9602C_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max	        = RTL9602C_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = RTL9602C_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = RTL9602C_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = RTL9602C_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = RTL9602C_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = RTL9602C_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = RTL9602C_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = RTL9602C_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = RTL9602C_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = RTL9602C_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = RTL9602C_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = RTL9602C_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = RTL9602C_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = RTL9602C_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = RTL9602C_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = RTL9602C_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = RTL9602C_MAX_NUM_OF_LOG_MIB              ,
	.max_num_of_host 					= RTL9602C_MAX_NUM_OF_HOST			   ,
    .ext_cpu_port_id                    = RTL9602C_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = RTL9602C_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = RTL9602C_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = RTL9602C_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop               	= RTL9602C_MAX_NUM_OF_NEXTHOP			   ,
    .max_rgmii_tx_delay                 = RTL9602C_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = RTL9602C_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = RTL9602C_MAX_LLID_ENTRY              ,

};



/* Normal RTL9602C Chip PER_PORT block information */
static rt_macPpInfo_t rtl9602c_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x400,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_RTL9602C) */


#if defined(CONFIG_SDK_RTL9607C)
static rt_portinfo_t rtl9607c_port_info =
{
    /* Normal RTL9607C Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT   /*P3 */, RT_GE_PORT   /*P4 */,
        RT_GE_PORT  /*P5 */, RT_GE_SERDES_PORT  /*P6 */, RT_GE_SERDES_PORT /*P7 */, RT_GE_PORT  /*P8 */, RT_CPU_PORT   /*P9 */,
        RT_CPU_PORT  /*P10*/, RT_SWPBO_PORT  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE   /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	0, /*port number*/
     	0,	/*max*/
     	0,  /*min*/
     	{{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
     	18, /*port number*/
     	17,	/*max*/
     	0,  /*min*/
     	{{0x3ffff}}/*port mask*/
     },
     .swPbo     = \
     {
        5,  /* port number */
        4,  /* max */
        0,  /* min */
        {{0x1f}} /* port mask */
     },
     .serdes     = \
     {
        2,  /* port number */
        7,  /* max */
        6,  /* min */
        {{0xC0}} /* port mask */
     },
     .ponPort   = 5,
     .rgmiiPort = 8,
     .swPboLbPort = 11
}; /* end of rtl9607c_port_info */

/*
RTL9607C port mapping

UTP0               0
PON/fiber/UTP1     5
CPU port           9

*/



/* Normal RTL9607C Chip Port Information */
static rt_register_capacity_t rtl9607c_capacityInfo =
{
    .max_num_of_mirror                  = RTL9607C_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = RTL9607C_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = RTL9607C_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = RTL9607C_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = RTL9607C_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = RTL9607C_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = RTL9607C_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = RTL9607C_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = RTL9607C_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = RTL9607C_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = RTL9607C_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = RTL9607C_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = RTL9607C_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = RTL9607C_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = RTL9607C_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = RTL9607C_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = RTL9607C_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = RTL9607C_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = RTL9607C_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = RTL9607C_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = RTL9607C_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = RTL9607C_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = RTL9607C_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = RTL9607C_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = RTL9607C_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = RTL9607C_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = RTL9607C_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = RTL9607C_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = RTL9607C_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = RTL9607C_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = RTL9607C_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = RTL9607C_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = RTL9607C_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = RTL9607C_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = RTL9607C_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = RTL9607C_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = RTL9607C_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = RTL9607C_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = RTL9607C_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = RTL9607C_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = RTL9607C_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = RTL9607C_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = RTL9607C_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = RTL9607C_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = RTL9607C_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = RTL9607C_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = RTL9607C_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = RTL9607C_WRED_MPD_MAX                    ,
    .acl_rate_max                       = RTL9607C_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = RTL9607C_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = RTL9607C_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = RTL9607C_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = RTL9607C_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = RTL9607C_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = RTL9607C_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = RTL9607C_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = RTL9607C_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = RTL9607C_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = RTL9607C_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = RTL9607C_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = RTL9607C_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = RTL9607C_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = RTL9607C_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = RTL9607C_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = RTL9607C_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = RTL9607C_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = RTL9607C_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = RTL9607C_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = RTL9607C_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = RTL9607C_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = RTL9607C_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = RTL9607C_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = RTL9607C_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max	            = RTL9607C_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max	            = RTL9607C_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max	        = RTL9607C_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = RTL9607C_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = RTL9607C_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = RTL9607C_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = RTL9607C_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = RTL9607C_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = RTL9607C_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = RTL9607C_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = RTL9607C_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = RTL9607C_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = RTL9607C_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = RTL9607C_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = RTL9607C_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = RTL9607C_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = RTL9607C_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = RTL9607C_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = RTL9607C_MAX_NUM_OF_LOG_MIB              ,
	.max_num_of_host 					= RTL9607C_MAX_NUM_OF_HOST			   ,
    .ext_cpu_port_id                    = RTL9607C_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = RTL9607C_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = RTL9607C_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = RTL9607C_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop               	= RTL9607C_MAX_NUM_OF_NEXTHOP			   ,
    .max_rgmii_tx_delay                 = RTL9607C_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = RTL9607C_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = RTL9607C_MAX_LLID_ENTRY              ,

};



/* Normal RTL9607C Chip PER_PORT block information */
static rt_macPpInfo_t rtl9607c_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x100,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_RTL9607C) */

#if defined(CONFIG_SDK_LUNA_G3)
#if defined(CONFIG_ARCH_RTL8198F)
static rt_portinfo_t rtl8198f_port_info =
{
    /* Normal rtl8198f Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT     /*P4 */,
        RT_XE_PORT    /*P5 */, RT_XE_PORT  /*P6 */, RT_XE_PORT /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_CPU_PORT   /*P16*/, RT_CPU_PORT  /*P17*/, RT_CPU_PORT   /*P18*/, RT_CPU_PORT    /*P19*/,
        RT_CPU_PORT   /*P20*/, RT_CPU_PORT   /*P21*/, RT_CPU_PORT  /*P22*/, RT_CPU_PORT   /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
        0, /*port number*/
        0,  /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
        0, /*port number*/
        0, /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     .swPbo     = \
     {
        0,  /* port number */
        0,  /* max */
        0,  /* min */
        {{0x0000}} /* port mask */
     },
     .ponPort   = 4,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of rtl8198f_port_info */

/*
RTL8198F port mapping

UTP0               0
PON/fiber/UTP1     5
CPU port           9

*/

/* Normal RTL8198F Chip Port Information */
static rt_register_capacity_t rtl8198f_capacityInfo =
{
    .max_num_of_mirror                  = RTL8198F_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = RTL8198F_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = RTL8198F_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = RTL8198F_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = RTL8198F_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = RTL8198F_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = RTL8198F_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = RTL8198F_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = RTL8198F_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = RTL8198F_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = RTL8198F_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = RTL8198F_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = RTL8198F_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = RTL8198F_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = RTL8198F_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = RTL8198F_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = RTL8198F_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = RTL8198F_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = RTL8198F_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = RTL8198F_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = RTL8198F_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = RTL8198F_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = RTL8198F_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = RTL8198F_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = RTL8198F_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = RTL8198F_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = RTL8198F_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = RTL8198F_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = RTL8198F_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = RTL8198F_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = RTL8198F_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = RTL8198F_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = RTL8198F_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = RTL8198F_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = RTL8198F_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = RTL8198F_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = RTL8198F_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = RTL8198F_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = RTL8198F_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = RTL8198F_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = RTL8198F_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = RTL8198F_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = RTL8198F_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = RTL8198F_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = RTL8198F_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = RTL8198F_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = RTL8198F_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = RTL8198F_WRED_MPD_MAX                    ,
    .acl_rate_max                       = RTL8198F_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = RTL8198F_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = RTL8198F_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = RTL8198F_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = RTL8198F_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = RTL8198F_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = RTL8198F_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = RTL8198F_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = RTL8198F_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = RTL8198F_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = RTL8198F_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = RTL8198F_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = RTL8198F_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = RTL8198F_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = RTL8198F_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = RTL8198F_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = RTL8198F_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = RTL8198F_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = RTL8198F_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = RTL8198F_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = RTL8198F_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = RTL8198F_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = RTL8198F_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = RTL8198F_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = RTL8198F_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max              = RTL8198F_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max             = RTL8198F_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max           = RTL8198F_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = RTL8198F_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = RTL8198F_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = RTL8198F_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = RTL8198F_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = RTL8198F_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = RTL8198F_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = RTL8198F_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = RTL8198F_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = RTL8198F_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = RTL8198F_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = RTL8198F_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = RTL8198F_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = RTL8198F_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = RTL8198F_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = RTL8198F_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = RTL8198F_MAX_NUM_OF_LOG_MIB              ,
    .max_num_of_host                    = RTL8198F_MAX_NUM_OF_HOST             ,
    .ext_cpu_port_id                    = RTL8198F_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = RTL8198F_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = RTL8198F_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = RTL8198F_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop                 = RTL8198F_MAX_NUM_OF_NEXTHOP              ,
    .max_rgmii_tx_delay                 = RTL8198F_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = RTL8198F_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = RTL8198F_MAX_LLID_ENTRY              ,

};



/* Normal RTL8198F Chip PER_PORT block information */
static rt_macPpInfo_t rtl8198f_macPpInfo =
{
    0x00000000, /* lowerbound_addr */
    0xFFFFFFFF, /* upperbound_addr */
    0x100,  /* interval */
};
#else
static rt_portinfo_t luna_g3_port_info =
{
    /* Normal LUNA_G3 Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_PORT_NONE   /*P4 */,
        RT_XE_PORT    /*P5 */, RT_XE_PORT    /*P6 */, RT_XE_PORT   /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_CPU_PORT   /*P16*/, RT_CPU_PORT  /*P17*/, RT_CPU_PORT   /*P18*/, RT_CPU_PORT    /*P19*/,
        RT_CPU_PORT   /*P20*/, RT_CPU_PORT   /*P21*/, RT_CPU_PORT  /*P22*/, RT_CPU_PORT   /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
        0, /*port number*/
        0,  /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
        0, /*port number*/
        0, /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     .swPbo     = \
     {
        0,  /* port number */
        0,  /* max */
        0,  /* min */
        {{0x0000}} /* port mask */
     },
     .serdes     = \
     {
        0,  /* port number */
        VALUE_NO_INIT,  /* max */
        VALUE_NO_INIT,  /* min */
        {{0x0}} /* port mask */
     },
     .ponPort   = 7,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of luna_g3_port_info */

/*
LUNA_G3 port mapping

UTP0               0
PON/fiber/UTP1     5
CPU port           9

*/

/* Normal LUNA_G3 Chip Port Information */
static rt_register_capacity_t luna_g3_capacityInfo =
{
    .max_num_of_mirror                  = LUNA_G3_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = LUNA_G3_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = LUNA_G3_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = LUNA_G3_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = LUNA_G3_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = LUNA_G3_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = LUNA_G3_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = LUNA_G3_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = LUNA_G3_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = LUNA_G3_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = LUNA_G3_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = LUNA_G3_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = LUNA_G3_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = LUNA_G3_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = LUNA_G3_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = LUNA_G3_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = LUNA_G3_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = LUNA_G3_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = LUNA_G3_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = LUNA_G3_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = LUNA_G3_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = LUNA_G3_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = LUNA_G3_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = LUNA_G3_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = LUNA_G3_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = LUNA_G3_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = LUNA_G3_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = LUNA_G3_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = LUNA_G3_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = LUNA_G3_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = LUNA_G3_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = LUNA_G3_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = LUNA_G3_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = LUNA_G3_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = LUNA_G3_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = LUNA_G3_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = LUNA_G3_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = LUNA_G3_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = LUNA_G3_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = LUNA_G3_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = LUNA_G3_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = LUNA_G3_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = LUNA_G3_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = LUNA_G3_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = LUNA_G3_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = LUNA_G3_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = LUNA_G3_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = LUNA_G3_WRED_MPD_MAX                    ,
    .acl_rate_max                       = LUNA_G3_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = LUNA_G3_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = LUNA_G3_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = LUNA_G3_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = LUNA_G3_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = LUNA_G3_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = LUNA_G3_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = LUNA_G3_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = LUNA_G3_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = LUNA_G3_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = LUNA_G3_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = LUNA_G3_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = LUNA_G3_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = LUNA_G3_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = LUNA_G3_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = LUNA_G3_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = LUNA_G3_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = LUNA_G3_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = LUNA_G3_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = LUNA_G3_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = LUNA_G3_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = LUNA_G3_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = LUNA_G3_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = LUNA_G3_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = LUNA_G3_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max              = LUNA_G3_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max             = LUNA_G3_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max           = LUNA_G3_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = LUNA_G3_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = LUNA_G3_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = LUNA_G3_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = LUNA_G3_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = LUNA_G3_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = LUNA_G3_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = LUNA_G3_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = LUNA_G3_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = LUNA_G3_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = LUNA_G3_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = LUNA_G3_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = LUNA_G3_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = LUNA_G3_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = LUNA_G3_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = LUNA_G3_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = LUNA_G3_MAX_NUM_OF_LOG_MIB              ,
    .max_num_of_host                    = LUNA_G3_MAX_NUM_OF_HOST             ,
    .ext_cpu_port_id                    = LUNA_G3_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = LUNA_G3_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = LUNA_G3_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = LUNA_G3_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop                 = LUNA_G3_MAX_NUM_OF_NEXTHOP              ,
    .max_rgmii_tx_delay                 = LUNA_G3_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = LUNA_G3_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = LUNA_G3_MAX_LLID_ENTRY              ,

};



/* Normal LUNA_G3 Chip PER_PORT block information */
static rt_macPpInfo_t luna_g3_macPpInfo =
{
    0x00000000, /* lowerbound_addr */
    0xFFFFFFFF, /* upperbound_addr */
    0x100,  /* interval */
};
#endif /* End of #if defined(CONFIG_ARCH_RTL8198F) */
#endif /* End of #if defined(CONFIG_SDK_LUNA_G3) */

#if defined(CONFIG_SDK_CA8279)
static rt_portinfo_t ca8279_port_info =
{
    /* Normal CA8279 Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT     /*P4 */,
        RT_XE_PORT    /*P5 */, RT_XE_PORT    /*P6 */, RT_XE_PORT   /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_CPU_PORT   /*P16*/, RT_CPU_PORT  /*P17*/, RT_CPU_PORT   /*P18*/, RT_CPU_PORT    /*P19*/,
        RT_CPU_PORT   /*P20*/, RT_CPU_PORT   /*P21*/, RT_CPU_PORT  /*P22*/, RT_CPU_PORT   /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
        0, /*port number*/
        0,  /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
        0, /*port number*/
        0, /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     .swPbo     = \
     {
        0,  /* port number */
        0,  /* max */
        0,  /* min */
        {{0x0000}} /* port mask */
     },
     .serdes     = \
     {
        0,  /* port number */
        VALUE_NO_INIT,  /* max */
        VALUE_NO_INIT,  /* min */
        {{0x0}} /* port mask */
     },
     .ponPort   = 7,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of ca8279_port_info */

/*
CA8279 port mapping

UTP0               0
PON/fiber/UTP1     5
CPU port           9

*/

/* Normal CA8279 Chip Port Information */
static rt_register_capacity_t ca8279_capacityInfo =
{
    .max_num_of_mirror                  = CA8279_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = CA8279_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = CA8279_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = CA8279_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = CA8279_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = CA8279_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = CA8279_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = CA8279_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = CA8279_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = CA8279_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = CA8279_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = CA8279_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = CA8279_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = CA8279_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = CA8279_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = CA8279_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = CA8279_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = CA8279_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = CA8279_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = CA8279_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = CA8279_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = CA8279_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = CA8279_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = CA8279_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = CA8279_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = CA8279_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = CA8279_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = CA8279_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = CA8279_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = CA8279_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = CA8279_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = CA8279_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = CA8279_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = CA8279_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = CA8279_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = CA8279_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = CA8279_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = CA8279_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = CA8279_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = CA8279_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = CA8279_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = CA8279_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = CA8279_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = CA8279_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = CA8279_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = CA8279_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = CA8279_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = CA8279_WRED_MPD_MAX                    ,
    .acl_rate_max                       = CA8279_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = CA8279_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = CA8279_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = CA8279_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = CA8279_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = CA8279_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = CA8279_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = CA8279_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = CA8279_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = CA8279_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = CA8279_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = CA8279_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = CA8279_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = CA8279_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = CA8279_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = CA8279_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = CA8279_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = CA8279_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = CA8279_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = CA8279_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = CA8279_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = CA8279_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = CA8279_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = CA8279_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = CA8279_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max              = CA8279_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max             = CA8279_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max           = CA8279_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = CA8279_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = CA8279_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = CA8279_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = CA8279_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = CA8279_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = CA8279_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = CA8279_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = CA8279_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = CA8279_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = CA8279_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = CA8279_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = CA8279_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = CA8279_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = CA8279_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = CA8279_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = CA8279_MAX_NUM_OF_LOG_MIB              ,
    .max_num_of_host                    = CA8279_MAX_NUM_OF_HOST             ,
    .ext_cpu_port_id                    = CA8279_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = CA8279_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = CA8279_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = CA8279_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop                 = CA8279_MAX_NUM_OF_NEXTHOP              ,
    .max_rgmii_tx_delay                 = CA8279_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = CA8279_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = CA8279_MAX_LLID_ENTRY              ,

};



/* Normal CA8279 Chip PER_PORT block information */
static rt_macPpInfo_t ca8279_macPpInfo =
{
    0x00000000, /* lowerbound_addr */
    0xFFFFFFFF, /* upperbound_addr */
    0x100,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_CA8279) */

#if defined(CONFIG_SDK_CA8277B)
static rt_portinfo_t ca8277b_port_info =
{
    /* Normal CA8277B Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT     /*P4 */,
        RT_XE_PORT    /*P5 */, RT_XE_PORT    /*P6 */, RT_XE_PORT   /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_CPU_PORT   /*P16*/, RT_CPU_PORT  /*P17*/, RT_CPU_PORT   /*P18*/, RT_CPU_PORT    /*P19*/,
        RT_CPU_PORT   /*P20*/, RT_CPU_PORT   /*P21*/, RT_CPU_PORT  /*P22*/, RT_CPU_PORT   /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
        0, /*port number*/
        0,  /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
        0, /*port number*/
        0, /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     .swPbo     = \
     {
        0,  /* port number */
        0,  /* max */
        0,  /* min */
        {{0x0000}} /* port mask */
     },
     .serdes     = \
     {
        4,  /* port number */
        7,  /* max */
        4,  /* min */
        {{0xf0}} /* port mask */
     },
     .ponPort   = 7,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of ca8277b_port_info */

/*
CA8277B port mapping

UTP0               0
PON/fiber/UTP1     5
CPU port           9

*/

/* Normal CA8277B Chip Port Information */
static rt_register_capacity_t ca8277b_capacityInfo =
{
    .max_num_of_mirror                  = CA8277B_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = CA8277B_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = CA8277B_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = CA8277B_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = CA8277B_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = CA8277B_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = CA8277B_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = CA8277B_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = CA8277B_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = CA8277B_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = CA8277B_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = CA8277B_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = CA8277B_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = CA8277B_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = CA8277B_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = CA8277B_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = CA8277B_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = CA8277B_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = CA8277B_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = CA8277B_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = CA8277B_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = CA8277B_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = CA8277B_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = CA8277B_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = CA8277B_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = CA8277B_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = CA8277B_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = CA8277B_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = CA8277B_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = CA8277B_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = CA8277B_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = CA8277B_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = CA8277B_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = CA8277B_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = CA8277B_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = CA8277B_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = CA8277B_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = CA8277B_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = CA8277B_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = CA8277B_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = CA8277B_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = CA8277B_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = CA8277B_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = CA8277B_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = CA8277B_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = CA8277B_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = CA8277B_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = CA8277B_WRED_MPD_MAX                    ,
    .acl_rate_max                       = CA8277B_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = CA8277B_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = CA8277B_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = CA8277B_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = CA8277B_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = CA8277B_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = CA8277B_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = CA8277B_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = CA8277B_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = CA8277B_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = CA8277B_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = CA8277B_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = CA8277B_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = CA8277B_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = CA8277B_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = CA8277B_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = CA8277B_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = CA8277B_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = CA8277B_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = CA8277B_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = CA8277B_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = CA8277B_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = CA8277B_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = CA8277B_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = CA8277B_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max              = CA8277B_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max             = CA8277B_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max           = CA8277B_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = CA8277B_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = CA8277B_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = CA8277B_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = CA8277B_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = CA8277B_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = CA8277B_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = CA8277B_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = CA8277B_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = CA8277B_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = CA8277B_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = CA8277B_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = CA8277B_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = CA8277B_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = CA8277B_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = CA8277B_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = CA8277B_MAX_NUM_OF_LOG_MIB              ,
    .max_num_of_host                    = CA8277B_MAX_NUM_OF_HOST             ,
    .ext_cpu_port_id                    = CA8277B_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = CA8277B_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = CA8277B_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = CA8277B_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop                 = CA8277B_MAX_NUM_OF_NEXTHOP              ,
    .max_rgmii_tx_delay                 = CA8277B_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = CA8277B_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = CA8277B_MAX_LLID_ENTRY              ,

};



/* Normal CA8277B Chip PER_PORT block information */
static rt_macPpInfo_t ca8277b_macPpInfo =
{
    0x00000000, /* lowerbound_addr */
    0xFFFFFFFF, /* upperbound_addr */
    0x100,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_CA8277B) */

#if defined(CONFIG_SDK_RTL8277C)
static rt_portinfo_t rtl8277c_port_info =
{
    /* Normal RTL8277C Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT   /*P4 */,
        RT_XE_PORT    /*P5 */, RT_XE_PORT    /*P6 */, RT_XE_PORT   /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_CPU_PORT   /*P16*/, RT_CPU_PORT  /*P17*/, RT_CPU_PORT   /*P18*/, RT_CPU_PORT    /*P19*/,
        RT_CPU_PORT   /*P20*/, RT_CPU_PORT   /*P21*/, RT_CPU_PORT  /*P22*/, RT_CPU_PORT   /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
        0, /*port number*/
        0,  /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
        0, /*port number*/
        0, /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     .swPbo     = \
     {
        0,  /* port number */
        0,  /* max */
        0,  /* min */
        {{0x0000}} /* port mask */
     },
     .serdes     = \
     {
        4,  /* port number */
        7,  /* max */
        4,  /* min */
        {{0xf0}} /* port mask */
     },
     .ponPort   = 7,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of rtl8277c_port_info */

/*
RTL8277C port mapping

UTP0               0
PON/fiber/UTP1     5
CPU port           9

*/

/* Normal RTL8277C Chip Port Information */
static rt_register_capacity_t rtl8277c_capacityInfo =
{
    .max_num_of_mirror                  = RTL8277C_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = RTL8277C_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = RTL8277C_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = RTL8277C_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = RTL8277C_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = RTL8277C_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = RTL8277C_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = RTL8277C_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = RTL8277C_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = RTL8277C_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = RTL8277C_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = RTL8277C_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = RTL8277C_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = RTL8277C_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = RTL8277C_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = RTL8277C_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = RTL8277C_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = RTL8277C_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = RTL8277C_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = RTL8277C_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = RTL8277C_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = RTL8277C_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = RTL8277C_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = RTL8277C_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = RTL8277C_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = RTL8277C_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = RTL8277C_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = RTL8277C_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = RTL8277C_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = RTL8277C_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = RTL8277C_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = RTL8277C_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = RTL8277C_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = RTL8277C_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = RTL8277C_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = RTL8277C_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = RTL8277C_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = RTL8277C_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = RTL8277C_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = RTL8277C_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = RTL8277C_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = RTL8277C_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = RTL8277C_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = RTL8277C_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = RTL8277C_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = RTL8277C_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = RTL8277C_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = RTL8277C_WRED_MPD_MAX                    ,
    .acl_rate_max                       = RTL8277C_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = RTL8277C_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = RTL8277C_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = RTL8277C_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = RTL8277C_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = RTL8277C_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = RTL8277C_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = RTL8277C_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = RTL8277C_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = RTL8277C_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = RTL8277C_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = RTL8277C_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = RTL8277C_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = RTL8277C_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = RTL8277C_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = RTL8277C_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = RTL8277C_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = RTL8277C_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = RTL8277C_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = RTL8277C_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = RTL8277C_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = RTL8277C_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = RTL8277C_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = RTL8277C_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = RTL8277C_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max              = RTL8277C_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max             = RTL8277C_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max           = RTL8277C_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = RTL8277C_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = RTL8277C_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = RTL8277C_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = RTL8277C_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = RTL8277C_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = RTL8277C_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = RTL8277C_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = RTL8277C_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = RTL8277C_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = RTL8277C_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = RTL8277C_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = RTL8277C_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = RTL8277C_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = RTL8277C_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = RTL8277C_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = RTL8277C_MAX_NUM_OF_LOG_MIB              ,
    .max_num_of_host                    = RTL8277C_MAX_NUM_OF_HOST             ,
    .ext_cpu_port_id                    = RTL8277C_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = RTL8277C_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = RTL8277C_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = RTL8277C_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop                 = RTL8277C_MAX_NUM_OF_NEXTHOP              ,
    .max_rgmii_tx_delay                 = RTL8277C_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = RTL8277C_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = RTL8277C_MAX_LLID_ENTRY              ,

};



/* Normal RTL8277C Chip PER_PORT block information */
static rt_macPpInfo_t rtl8277c_macPpInfo =
{
    0x00000000, /* lowerbound_addr */
    0xFFFFFFFF, /* upperbound_addr */
    0x100,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_RTL8277C) */

#if defined(CONFIG_SDK_RTL9607F)
static rt_portinfo_t rtl9607f_port_info =
{
    /* Normal RTL9607F Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT   /*P4 */,
        RT_XE_PORT    /*P5 */, RT_XE_PORT    /*P6 */, RT_XE_PORT   /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_CPU_PORT   /*P16*/, RT_CPU_PORT  /*P17*/, RT_CPU_PORT   /*P18*/, RT_CPU_PORT    /*P19*/,
        RT_CPU_PORT   /*P20*/, RT_CPU_PORT   /*P21*/, RT_CPU_PORT  /*P22*/, RT_CPU_PORT   /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
        0, /*port number*/
        0,  /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
        0, /*port number*/
        0, /*max*/
        0,  /*min*/
        {{0x0000}}/*port mask*/
     },
     .swPbo     = \
     {
        0,  /* port number */
        0,  /* max */
        0,  /* min */
        {{0x0000}} /* port mask */
     },
     .serdes     = \
     {
        4,  /* port number */
        7,  /* max */
        4,  /* min */
        {{0xf0}} /* port mask */
     },
     .ponPort   = 7,
     .rgmiiPort = -1,
     .swPboLbPort = -1
}; /* end of rtl9607f_port_info */

/*
RTL9607F port mapping

UTP0               0
PON/fiber/UTP1     5
CPU port           9

*/

/* Normal RTL9607 Chip Port Information */
static rt_register_capacity_t rtl9607f_capacityInfo =
{
    .max_num_of_mirror                  = RTL9607F_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = RTL9607F_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = RTL9607F_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = RTL9607F_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = RTL9607F_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = RTL9607F_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = RTL9607F_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = RTL9607F_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = RTL9607F_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = RTL9607F_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = RTL9607F_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = RTL9607F_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = RTL9607F_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = RTL9607F_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = RTL9607F_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = RTL9607F_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = RTL9607F_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = RTL9607F_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = RTL9607F_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = RTL9607F_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = RTL9607F_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = RTL9607F_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = RTL9607F_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = RTL9607F_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = RTL9607F_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = RTL9607F_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = RTL9607F_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = RTL9607F_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = RTL9607F_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = RTL9607F_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = RTL9607F_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = RTL9607F_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = RTL9607F_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = RTL9607F_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = RTL9607F_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = RTL9607F_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = RTL9607F_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = RTL9607F_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = RTL9607F_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = RTL9607F_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = RTL9607F_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = RTL9607F_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = RTL9607F_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = RTL9607F_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = RTL9607F_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = RTL9607F_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = RTL9607F_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = RTL9607F_WRED_MPD_MAX                    ,
    .acl_rate_max                       = RTL9607F_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = RTL9607F_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = RTL9607F_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = RTL9607F_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = RTL9607F_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = RTL9607F_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = RTL9607F_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = RTL9607F_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = RTL9607F_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = RTL9607F_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = RTL9607F_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = RTL9607F_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = RTL9607F_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = RTL9607F_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = RTL9607F_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = RTL9607F_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = RTL9607F_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = RTL9607F_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = RTL9607F_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = RTL9607F_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = RTL9607F_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = RTL9607F_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = RTL9607F_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = RTL9607F_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = RTL9607F_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max              = RTL9607F_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max             = RTL9607F_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max           = RTL9607F_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = RTL9607F_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = RTL9607F_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = RTL9607F_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = RTL9607F_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = RTL9607F_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = RTL9607F_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = RTL9607F_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = RTL9607F_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = RTL9607F_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = RTL9607F_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = RTL9607F_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = RTL9607F_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = RTL9607F_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = RTL9607F_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = RTL9607F_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = RTL9607F_MAX_NUM_OF_LOG_MIB              ,
    .max_num_of_host                    = RTL9607F_MAX_NUM_OF_HOST             ,
    .ext_cpu_port_id                    = RTL9607F_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = RTL9607F_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = RTL9607F_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = RTL9607F_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop                 = RTL9607F_MAX_NUM_OF_NEXTHOP              ,
    .max_rgmii_tx_delay                 = RTL9607F_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = RTL9607F_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = RTL9607F_MAX_LLID_ENTRY              ,

};



/* Normal RTL9607F Chip PER_PORT block information */
static rt_macPpInfo_t rtl9607f_macPpInfo =
{
    0x00000000, /* lowerbound_addr */
    0xFFFFFFFF, /* upperbound_addr */
    0x100,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_RTL9607F) */

#if defined(CONFIG_SDK_RTL9603CVD)
static rt_portinfo_t rtl9603cvd_port_info =
{
    /* Normal RTL9603CVD Chip Port Information */
    {
        RT_FE_PORT    /*P0 */, RT_FE_PORT    /*P1 */, RT_FE_PORT   /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT    /*P4 */,
        RT_CPU_PORT   /*P5 */, RT_SWPBO_PORT /*P6 */, RT_PORT_NONE /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE  /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE  /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE  /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE  /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	0, /*port number*/
     	0,	/*max*/
     	0,  /*min*/
     	{{0x0000}}/*port mask*/
     },
     /*extension port member*/
     {
     	7, /*port number*/
     	6,	/*max*/
     	0,  /*min*/
     	{{0x7f}}/*port mask*/
     },
     .swPbo     = \
     {
        4,  /* port number */
        3,  /* max */
        0,  /* min */
        {{0xf}} /* port mask */
     },
     .serdes     = \
     {
        1,  /* port number */
        2,  /* max */
        2,  /* min */
        {{0x04}} /* port mask */
     },
     .ponPort   = 4,
     .rgmiiPort = -1,
     .swPboLbPort = 6
}; /* end of rtl9603cvd_port_info */

/*
RTL9603CVD port mapping

UTP0               0
UTP1               1
UTP2               2
UTP3               3
PON/fiber          4
CPU port           5

*/



/* Normal RTL9603CVD Chip Port Information */
static rt_register_capacity_t rtl9603cvd_capacityInfo =
{
    .max_num_of_mirror                  = RTL9603CVD_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = RTL9603CVD_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = RTL9603CVD_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = RTL9603CVD_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = RTL9603CVD_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = RTL9603CVD_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = RTL9603CVD_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = RTL9603CVD_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = RTL9603CVD_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = RTL9603CVD_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = RTL9603CVD_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = RTL9603CVD_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = RTL9603CVD_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = RTL9603CVD_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = RTL9603CVD_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = RTL9603CVD_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = RTL9603CVD_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = RTL9603CVD_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = RTL9603CVD_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = RTL9603CVD_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = RTL9603CVD_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = RTL9603CVD_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = RTL9603CVD_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = RTL9603CVD_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = RTL9603CVD_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = RTL9603CVD_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = RTL9603CVD_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = RTL9603CVD_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = RTL9603CVD_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = RTL9603CVD_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = RTL9603CVD_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = RTL9603CVD_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = RTL9603CVD_PRI_SEL_GROUP_INDEX_MAX         ,
    .pri_sel_weight_max                 = RTL9603CVD_PRI_SEL_WEIGHT_MAX              ,
    .queue_weight_max                   = RTL9603CVD_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = RTL9603CVD_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = RTL9603CVD_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = RTL9603CVD_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = RTL9603CVD_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = RTL9603CVD_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = RTL9603CVD_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = RTL9603CVD_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = RTL9603CVD_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = RTL9603CVD_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = RTL9603CVD_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = RTL9603CVD_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = RTL9603CVD_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = RTL9603CVD_WRED_MPD_MAX                    ,
    .acl_rate_max                       = RTL9603CVD_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = RTL9603CVD_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = RTL9603CVD_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = RTL9603CVD_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = RTL9603CVD_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = RTL9603CVD_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = RTL9603CVD_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = RTL9603CVD_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = RTL9603CVD_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = RTL9603CVD_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = RTL9603CVD_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = RTL9603CVD_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = RTL9603CVD_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = RTL9603CVD_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = RTL9603CVD_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = RTL9603CVD_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = RTL9603CVD_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = RTL9603CVD_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = RTL9603CVD_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = RTL9603CVD_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = RTL9603CVD_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = RTL9603CVD_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = RTL9603CVD_L34_PPPOE_TABLE_MAX             ,
    .l34_ipv6_routing_table_max         = RTL9603CVD_L34_IPV6_ROUTING_TABLE_MAX      ,
    .l34_ipv6_neighbor_table_max        = RTL9603CVD_L34_IPV6_NBR_TABLE_MAX          ,
    .l34_binding_table_max	            = RTL9603CVD_L34_BINDING_TABLE_MAX           ,
    .l34_wan_type_table_max	            = RTL9603CVD_L34_WAN_TYPE_TABLE_MAX          ,
    .l34_flow_route_table_max	        = RTL9603CVD_L34_FLOW_ROUTE_TABLE_MAX        ,
    .l34_dslite_table_max               = RTL9603CVD_L34_DSLITE_INF_TABLE_MAX        ,
    .l34_dslite_mc_table_max            = RTL9603CVD_L34_DSLITE_MCAST_TABLE_MAX      ,
    .l34_ipmc_tran_table_max            = RTL9603CVD_L34_IPMC_TRAN_TABLE_MAX         ,
    .gpon_tcont_max                     = RTL9603CVD_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = RTL9603CVD_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = RTL9603CVD_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = RTL9603CVD_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = RTL9603CVD_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = RTL9603CVD_CLASSIFY_IP_RANGE_NUM           ,
    .max_num_of_acl_template            = RTL9603CVD_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = RTL9603CVD_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = RTL9603CVD_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = RTL9603CVD_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = RTL9603CVD_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = RTL9603CVD_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = RTL9603CVD_MAX_NUM_OF_LOG_MIB              ,
	.max_num_of_host 					= RTL9603CVD_MAX_NUM_OF_HOST			   ,
    .ext_cpu_port_id                    = RTL9603CVD_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = RTL9603CVD_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = RTL9603CVD_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = RTL9603CVD_MAX_MIB_LATCH_TIMER             ,
    .max_num_of_nexthop               	= RTL9603CVD_MAX_NUM_OF_NEXTHOP			   ,
    .max_rgmii_tx_delay                 = RTL9603CVD_MAX_RGMII_TX_DELAY              ,
    .max_rgmii_rx_delay                 = RTL9603CVD_MAX_RGMII_RX_DELAY              ,
    .max_llid_entry                     = RTL9603CVD_MAX_LLID_ENTRY              ,

};



/* Normal RTL9603CVD Chip PER_PORT block information */
static rt_macPpInfo_t rtl9603cvd_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x100,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_RTL9603CVD) */


/* Supported mac chip lists */
static rt_device_t supported_devices[] =
{
#if defined(CONFIG_SDK_APOLLO)
    /* RT_DEVICE_APOLLO */
    {
            APOLLO_CHIP_ID,
            CHIP_REV_ID_0,
            APOLLO_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &apollo_port_info,
            &apollo_capacityInfo,
            &apollo_macPpInfo
    },
    /* RT_DEVICE_APOLLO_REV_B */
    {
            APOLLO_CHIP_ID,
            CHIP_REV_ID_A,
            APOLLO_CHIP_ID,
            CHIP_REV_ID_A,
            CHIP_AFLAG_LEXRA,
            &apollo_port_info,
            &apollo_capacityInfo,
            &apollo_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    /* RT_DEVICE_APOLLOMP */
    {
            APOLLOMP_CHIP_ID,
            CHIP_REV_ID_A,
            APOLLOMP_CHIP_ID,
            CHIP_REV_ID_A,
            CHIP_AFLAG_LEXRA,
            &apollomp_port_info,
            &apollomp_capacityInfo,
            &apollomp_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_RTL9601B)
    /* RT_DEVICE_RTL9601B */
    {
            RTL9601B_CHIP_ID,
            CHIP_REV_ID_0,
            RTL9601B_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &rtl9601b_port_info,
            &rtl9601b_capacityInfo,
            &rtl9601b_macPpInfo
    },
#if 0
    /* RT_DEVICE_RTL9601B_REV_B */
    {
            RTL9601B_CHIP_ID,
            CHIP_REV_ID_A,
            RTL9601B_CHIP_ID,
            CHIP_REV_ID_A,
            CHIP_AFLAG_LEXRA,
            &rtl9601b_port_info,
            &rtl9601b_capacityInfo,
            &rtl9601b_macPpInfo
    },
#endif
#endif
#if defined(CONFIG_SDK_RTL9602C)
    /* RT_DEVICE_RTL9602C */
    {
            RTL9602C_CHIP_ID,
            CHIP_REV_ID_A,
            RTL9602C_CHIP_ID,
            CHIP_REV_ID_A,
            CHIP_AFLAG_LEXRA,
            &rtl9602c_port_info,
            &rtl9602c_capacityInfo,
            &rtl9602c_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_RTL9607C)
    /* RT_DEVICE_RTL9607C */
    {
            RTL9607C_CHIP_ID,
            CHIP_REV_ID_A,
            RTL9607C_CHIP_ID,
            CHIP_REV_ID_A,
            CHIP_AFLAG_LEXRA,
            &rtl9607c_port_info,
            &rtl9607c_capacityInfo,
            &rtl9607c_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_LUNA_G3)
#if defined(CONFIG_ARCH_RTL8198F)
    /* RT_DEVICE_RTL8198F */
    {
            RTL8198F_CHIP_ID,
            CHIP_REV_ID_0,
            RTL8198F_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &rtl8198f_port_info,
            &rtl8198f_capacityInfo,
            &rtl8198f_macPpInfo
    },
#else
    /* RT_DEVICE_LUNA_G3 */
    {
            LUNA_G3_CHIP_ID,
            CHIP_REV_ID_0,
            LUNA_G3_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &luna_g3_port_info,
            &luna_g3_capacityInfo,
            &luna_g3_macPpInfo
    },
#endif
#endif
#if defined(CONFIG_SDK_CA8279)
    /* RT_DEVICE_CA8279 */
    {
            CA8279_CHIP_ID,
            CHIP_REV_ID_0,
            CA8279_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &ca8279_port_info,
            &ca8279_capacityInfo,
            &ca8279_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_CA8277B)
    /* RT_DEVICE_CA8277B */
    {
            CA8277B_CHIP_ID,
            CHIP_REV_ID_0,
            CA8277B_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &ca8277b_port_info,
            &ca8277b_capacityInfo,
            &ca8277b_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_RTL8277C)
    /* RT_DEVICE_RTL8277C */
    {
            RTL8277C_CHIP_ID,
            CHIP_REV_ID_0,
            RTL8277C_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &rtl8277c_port_info,
            &rtl8277c_capacityInfo,
            &rtl8277c_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_RTL9607F)
    /* RT_DEVICE_RTL9607F */
    {
            RTL9607F_CHIP_ID,
            CHIP_REV_ID_0,
            RTL9607F_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &rtl9607f_port_info,
            &rtl9607f_capacityInfo,
            &rtl9607f_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
    /* RT_DEVICE_RTL9603CVD */
    {
            RTL9603CVD_CHIP_ID,
            CHIP_REV_ID_A,
            RTL9603CVD_CHIP_ID,
            CHIP_REV_ID_A,
            CHIP_AFLAG_LEXRA,
            &rtl9603cvd_port_info,
            &rtl9603cvd_capacityInfo,
            &rtl9603cvd_macPpInfo
    },
#endif
};


/*
 * Macro Definition
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
int32
hal_isPpBlock_check(uint32 addr, uint32 *pIsPpBlock)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pIsPpBlock), RT_ERR_NULL_POINTER);

#if 1
    if (addr < HAL_GET_MACPP_MIN_ADDR() || addr > HAL_GET_MACPP_MAX_ADDR())
        *pIsPpBlock = FALSE;
    else
        *pIsPpBlock = TRUE;
#else
    *pIsPpBlock = FALSE;
#endif


    return RT_ERR_OK;
} /* end of hal_isPpBlock_check */



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
rt_device_t *
hal_find_device(uint32 chip_id, uint32 chip_rev_id)
{
    uint32  dev_idx;
    uint32  most_rev_id = 0;
    rt_device_t *pMatchDevice = NULL;


    RT_PARAM_CHK((chip_rev_id > CHIP_REV_ID_MAX), NULL);

    /* find out appropriate supported revision from supported_devices lists
     */
    for (dev_idx = 0; dev_idx < RT_DEVICE_END; dev_idx++)
    {
        if (supported_devices[dev_idx].chip_id == chip_id)
        {
            if (supported_devices[dev_idx].chip_rev_id == chip_rev_id)
            {
                /* Match and return this MAC device */
                return (&supported_devices[dev_idx]);
            }
            else if ((supported_devices[dev_idx].chip_rev_id < chip_rev_id) &&
                     (supported_devices[dev_idx].chip_rev_id >= most_rev_id))
            {
                /* Match better candidate of MAC device */
                most_rev_id = supported_devices[dev_idx].chip_rev_id;
                pMatchDevice = &supported_devices[dev_idx];
            }
        }
    }

    return (pMatchDevice);
} /* end of hal_find_device */

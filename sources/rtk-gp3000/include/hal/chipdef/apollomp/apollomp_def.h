#ifndef _APOLLOMP_DEF_H_
#define _APOLLOMP_DEF_H_

/*chip capacity define area*/
#define    APOLLOMP_MAX_NUM_OF_MIRROR                    1
#define    APOLLOMP_MAX_NUM_OF_TRUNK                     1
#define    APOLLOMP_MAX_NUM_OF_TRUNKMEMBER               4
#define    APOLLOMP_MAX_NUM_OF_DUMB_TRUNKMEMBER          4
#define    APOLLOMP_MAX_NUM_OF_TRUNKHASHVAL              7
#define    APOLLOMP_MAX_NUM_OF_MSTI                      16
#define    APOLLOMP_MAX_NUM_OF_METERING                  32
#define    APOLLOMP_MAX_NUM_OF_FIELD_SELECTOR            16
#define    APOLLOMP_MAX_NUM_OF_RANGE_CHECK_SRCPORT       0
#define    APOLLOMP_MAX_NUM_OF_RANGE_CHECK_IP            8
#define    APOLLOMP_MAX_NUM_OF_RANGE_CHECK_VID           8
#define    APOLLOMP_MAX_NUM_OF_RANGE_CHECK_PKTLEN        8
#define    APOLLOMP_MAX_NUM_OF_RANGE_CHECK_L4PORT        16
#define    APOLLOMP_MAX_NUM_OF_PATTERN_MATCH_DATA        0
#define    APOLLOMP_PATTERN_MATCH_PORT_MAX               0
#define    APOLLOMP_PATTERN_MATCH_PORT_MIN               0
#define    APOLLOMP_MAX_NUM_OF_L2_HASHDEPTH              4
#define    APOLLOMP_MAX_NUM_OF_QUEUE                     8
#define    APOLLOMP_MIN_NUM_OF_QUEUE                     1
#define    APOLLOMP_MAX_NUM_OF_PON_QUEUE                 128
#define    APOLLOMP_MIN_NUM_OF_PON_QUEUE                 1
#define    APOLLOMP_MAX_NUM_OF_CVLAN_TPID                1
#define    APOLLOMP_MAX_NUM_OF_SVLAN_TPID                1
#define    APOLLOMP_TPID_ENTRY_IDX_MAX                   0
#define    APOLLOMP_TPID_ENTRY_MASK_MAX                  0x0
#define    APOLLOMP_PROTOCOL_VLAN_IDX_MAX                3
#define    APOLLOMP_MAX_NUM_VLAN_MBRCFG                  32
#define    APOLLOMP_VLAN_FID_MAX                         0xf
#define    APOLLOMP_FLOWCTRL_THRESH_MAX                  0x1FFF
#define    APOLLOMP_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX     0xFF
#define    APOLLOMP_PRI_OF_SELECTION_MAX                 7
#define    APOLLOMP_PRI_OF_SELECTION_MIN                 0
#define    APOLLOMP_PRI_SEL_GROUP_INDEX_MAX              2
#define    APOLLOMP_PRI_SEL_WEIGHT_MAX                   15
#define    APOLLOMP_QUEUE_WEIGHT_MAX                     127
#define    APOLLOMP_QID0_WEIGHT_MAX                      65535
#define    APOLLOMP_RATE_OF_BANDWIDTH_MAX                0xFFFF
#define    APOLLOMP_THRESH_OF_IGR_BW_FLOWCTRL_MAX        0xFF
#define    APOLLOMP_MAX_NUM_OF_FASTPATH_OF_RATE          0
#define    APOLLOMP_RATE_OF_STORM_CONTROL_MAX            0x1FFFF
#define    APOLLOMP_BURST_RATE_OF_STORM_CONTROL_MAX      0x1FFF
#define    APOLLOMP_INTERNAL_PRIORITY_MAX                7
#define    APOLLOMP_DROP_PRECEDENCE_MAX                  0
#define    APOLLOMP_PRIORITY_REMAP_GROUP_IDX_MAX         0
#define    APOLLOMP_PRIORITY_REMARK_GROUP_IDX_MAX        0
#define    APOLLOMP_PRIORITY_TO_QUEUE_GROUP_IDX_MAX      3
#define    APOLLOMP_WRED_WEIGHT_MAX                      0x0
#define    APOLLOMP_WRED_MPD_MAX                         0x0
#define    APOLLOMP_ACL_RATE_MAX                         0x1FFFF
#define    APOLLOMP_L2_LEARN_LIMIT_CNT_MAX               (2048+64)
#define    APOLLOMP_L2_LEARN_4WAY_NO                     (2048)
#define    APOLLOMP_L2_AGING_TIME_MAX                    (0x1FFFFF)
#define    APOLLOMP_L2_ENTRY_AGING_MAX                   7
#define    APOLLOMP_EEE_QUEUE_THRESH_MAX                 0xFF
#define    APOLLOMP_SEC_MINIPV6FRAGLEN_MAX               0xFFFF
#define    APOLLOMP_SEC_MAXPINGLEN_MAX                   0xFFFF
#define    APOLLOMP_SEC_SMURFNETMASKLEN_MAX              31
#define    APOLLOMP_SFLOW_RATE_MAX                       0xFFFF
#define    APOLLOMP_MAX_NUM_OF_MCAST_FWD                 (2048+64)
#define    APOLLOMP_MIIM_PAGE_ID_MIN                     0x0000
#define    APOLLOMP_MIIM_PAGE_ID_MAX                     0xFFFF
#define    APOLLOMP_MIIM_REG_ID_MAX                      (31)
#define    APOLLOMP_MIIM_DATA_MAX                        (0xFFFF)
#define    APOLLOMP_L34_NETIF_TABLE_MAX                  8
#define    APOLLOMP_L34_ARP_TABLE_MAX                    512
#define    APOLLOMP_L34_EXTIP_TABLE_MAX                  8
#define    APOLLOMP_L34_ROUTING_TABLE_MAX                8
#define    APOLLOMP_L34_NAPT_TABLE_MAX                   2048
#define    APOLLOMP_L34_NAPTR_TABLE_MAX                  1024
#define    APOLLOMP_L34_NH_TABLE_MAX                     16
#define    APOLLOMP_L34_PPPOE_TABLE_MAX                  8
#define    APOLLOMP_L34_IPMC_TRAN_TABLE_MAX              16
#define    APOLLOMP_GPON_TCONT_MAX                       32
#define    APOLLOMP_GPON_FLOW_MAX                        128
#define    APOLLOMP_CLASSIFY_ENTRY_MAX                   512
#define    APOLLOMP_CLASSIFY_SID_MAX                     128
#define    APOLLOMP_CLASSIFY_L4PORT_RANGE_NUM            8
#define    APOLLOMP_CLASSIFY_IP_RANGE_NUM                8
#define    APOLLOMP_MAX_NUM_OF_ACL_TEMPLATE              4
#define    APOLLOMP_MAX_NUM_OF_ACL_RULE_FIELD            8
#define    APOLLOMP_MAX_NUM_OF_ACL_ACTION                128
#define    APOLLOMP_MAX_NUM_OF_ACL_RULE_ENTRY            64
#define    APOLLOMP_MAX_SPECIAL_CONGEST_SEC              (15)
#define    APOLLOMP_MAX_NUM_OF_METER                     32
#define    APOLLOMP_MAX_ENHANCED_FID                     7
#define    APOLLOMP_MAX_NUM_OF_LOG_MIB                   32
#define    APOLLOMP_EXT_CPU_PORT_ID                      (0)
#define    APOLLOMP_PONMAC_TCONT_QUEUE_MAX               (32)
#define    APOLLOMP_PONMAC_PIR_CIR_RATE_MAX              (0x1FFFF)
#define    APOLLOMP_MAX_MIB_LATCH_TIMER                  (0xff)
#define    APOLLOMP_MAX_NUM_OF_NEXTHOP                  16
#define    APOLLOMP_MAX_RGMII_TX_DELAY                   (1)
#define    APOLLOMP_MAX_RGMII_RX_DELAY                   (7)

#define    APOLLOMP_MAX_LLID_ENTRY                   (8)


/*chip dependent define area*/

#define APOLLOMP_TCONT_QUEUE_MAX  32
#define APOLLOMP_SEC_FLOOD_THRESHOLD_MAX 	0x00FF

/*vlan*/
#define APOLLOMP_VIDMAX                       0xFFF
#define APOLLOMP_EVIDMAX                      0x1FFF
#define APOLLOMP_CVIDXNO                      32
#define APOLLOMP_CVIDXMAX                     (APOLLOMP_CVIDXNO-1)
#define APOLLOMP_PROTOVLAN_GIDX_MAX 3
#define APOLLOMP_PROTOVLAN_GROUPNO  4

#define APOLLOMP_EXTPORTNO                    5
#define APOLLOMP_EXTPORTNOMAX                 (APOLLOMP_EXTPORTNO-1)

#define APOLLOMP_SVLAN_ENTRY_NO       64
#define APOLLOMP_SVLAN_C2S_NO         128
#define APOLLOMP_SVLAN_MC2S_NO        8
#define APOLLOMP_SVLAN_SP2C_NO        128

#define APOLLOMP_PACEKT_LENGTH_MAX 	        0x3FFF
#define APOLLOMP_MAXLENGTH_INDEX_NUM 	        2

#define APOLLOMP_MAX_ENHANCE_VID      (0x1FFF)


#define APOLLOMP_PRI_WEIGHT_MAX    		    15
#define APOLLOMP_QOS_INBW_THRESHOLD_MAX 	0xF
#define APOLLOMP_QOS_GRANULARTY_MAX           0x1FFFF


#define APOLLOMP_METER_BUCKETSIZE_MAX         0XFFFF

#define APOLLOMP_RATE_MAX                 (0x1FFFF * 8)
#define APOLLOMP_APR_METER_MAX                 (0x8)

#define APOLLOMP_RMA_L2TAILNUM_MAX  0x2f

#define APOLLOMP_IPMC_TRANS_MAX  0x15


#define    APOLLOMP_L34_IPV6_ROUTING_TABLE_MAX            4
#define    APOLLOMP_L34_IPV6_NBR_TABLE_MAX              128
#define    APOLLOMP_L34_BINDING_TABLE_MAX                32
#define    APOLLOMP_L34_WAN_TYPE_TABLE_MAX                8

#define APOLLOMP_LED_INDEX_MAX                 (31)

#endif /*#ifndef _APOLLOMP_DEF_H_*/

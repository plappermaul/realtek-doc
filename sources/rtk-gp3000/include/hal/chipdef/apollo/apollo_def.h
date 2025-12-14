#ifndef _APOLLO_DEF_H_
#define _APOLLO_DEF_H_

/*chip capacity define area*/
#define    APOLLO_MAX_NUM_OF_MIRROR                    1
#define    APOLLO_MAX_NUM_OF_TRUNK                     1
#define    APOLLO_MAX_NUM_OF_TRUNKMEMBER               4
#define    APOLLO_MAX_NUM_OF_DUMB_TRUNKMEMBER          4
#define    APOLLO_MAX_NUM_OF_TRUNKHASHVAL              7
#define    APOLLO_MAX_NUM_OF_MSTI                      16
#define    APOLLO_MAX_NUM_OF_METERING                  32
#define    APOLLO_MAX_NUM_OF_FIELD_SELECTOR            16
#define    APOLLO_MAX_NUM_OF_RANGE_CHECK_SRCPORT       0
#define    APOLLO_MAX_NUM_OF_RANGE_CHECK_IP            8
#define    APOLLO_MAX_NUM_OF_RANGE_CHECK_VID           8
#define    APOLLO_MAX_NUM_OF_RANGE_CHECK_PKTLEN        8
#define    APOLLO_MAX_NUM_OF_RANGE_CHECK_L4PORT        16
#define    APOLLO_MAX_NUM_OF_PATTERN_MATCH_DATA        0
#define    APOLLO_PATTERN_MATCH_PORT_MAX               0
#define    APOLLO_PATTERN_MATCH_PORT_MIN               0
#define    APOLLO_MAX_NUM_OF_L2_HASHDEPTH              4
#define    APOLLO_MAX_NUM_OF_QUEUE                     8
#define    APOLLO_MIN_NUM_OF_QUEUE                     1
#define    APOLLO_MAX_NUM_OF_PON_QUEUE                 128
#define    APOLLO_MIN_NUM_OF_PON_QUEUE                 1
#define    APOLLO_MAX_NUM_OF_CVLAN_TPID                1
#define    APOLLO_MAX_NUM_OF_SVLAN_TPID                1
#define    APOLLO_TPID_ENTRY_IDX_MAX                   0
#define    APOLLO_TPID_ENTRY_MASK_MAX                  0x0
#define    APOLLO_PROTOCOL_VLAN_IDX_MAX                3
#define    APOLLO_MAX_NUM_VLAN_MBRCFG                  32
#define    APOLLO_VLAN_FID_MAX                         0xf
#define    APOLLO_FLOWCTRL_THRESH_MAX                  0x1FFF
#define    APOLLO_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX     0xFF
#define    APOLLO_PRI_OF_SELECTION_MAX                 7
#define    APOLLO_PRI_OF_SELECTION_MIN                 0
#define    APOLLO_PRI_SEL_GROUP_INDEX_MAX              1
#define    APOLLO_PRI_SEL_WEIGHT_MAX                   15
#define    APOLLO_QUEUE_WEIGHT_MAX                     127
#define    APOLLO_QID0_WEIGHT_MAX                      65535
#define    APOLLO_RATE_OF_BANDWIDTH_MAX                0xFFFF
#define    APOLLO_THRESH_OF_IGR_BW_FLOWCTRL_MAX        0xFF
#define    APOLLO_MAX_NUM_OF_FASTPATH_OF_RATE          0
#define    APOLLO_RATE_OF_STORM_CONTROL_MAX            0x1FFFF
#define    APOLLO_BURST_RATE_OF_STORM_CONTROL_MAX      0x1FFF
#define    APOLLO_INTERNAL_PRIORITY_MAX                7
#define    APOLLO_DROP_PRECEDENCE_MAX                  0
#define    APOLLO_PRIORITY_REMAP_GROUP_IDX_MAX         0
#define    APOLLO_PRIORITY_REMARK_GROUP_IDX_MAX        0
#define    APOLLO_PRIORITY_TO_QUEUE_GROUP_IDX_MAX      3
#define    APOLLO_WRED_WEIGHT_MAX                      0x0
#define    APOLLO_WRED_MPD_MAX                         0x0
#define    APOLLO_ACL_RATE_MAX                         0x1FFFF
#define    APOLLO_L2_LEARN_LIMIT_CNT_MAX               (2048+64)
#define    APOLLO_L2_LEARN_4WAY_NO                     (2048)
#define    APOLLO_L2_AGING_TIME_MAX                    (209715)
#define    APOLLO_L2_ENTRY_AGING_MAX                   7
#define    APOLLO_EEE_QUEUE_THRESH_MAX                 0xFF
#define    APOLLO_SEC_MINIPV6FRAGLEN_MAX               0xFFFF
#define    APOLLO_SEC_MAXPINGLEN_MAX                   0xFFFF
#define    APOLLO_SEC_SMURFNETMASKLEN_MAX              31
#define    APOLLO_SFLOW_RATE_MAX                       0xFFFF
#define    APOLLO_MAX_NUM_OF_MCAST_FWD                 (2048+64)
#define    APOLLO_MIIM_PAGE_ID_MIN                     0x0000
#define    APOLLO_MIIM_PAGE_ID_MAX                     0xFFFF
#define    APOLLO_MIIM_REG_ID_MAX                      (31)
#define    APOLLO_MIIM_DATA_MAX                        (0xFFFF)
#define    APOLLO_L34_NETIF_TABLE_MAX                  8
#define    APOLLO_L34_ARP_TABLE_MAX                    512
#define    APOLLO_L34_EXTIP_TABLE_MAX                  16
#define    APOLLO_L34_ROUTING_TABLE_MAX                8
#define    APOLLO_L34_NAPT_TABLE_MAX                   2048
#define    APOLLO_L34_NAPTR_TABLE_MAX                  1024
#define    APOLLO_L34_NH_TABLE_MAX                     16
#define    APOLLO_L34_PPPOE_TABLE_MAX                  8
#define    APOLLO_GPON_TCONT_MAX                       32
#define    APOLLO_GPON_FLOW_MAX                        128
#define    APOLLO_CLASSIFY_ENTRY_MAX                   512
#define    APOLLO_CLASSIFY_SID_MAX                     128
#define    APOLLO_CLASSIFY_L4PORT_RANG_NUM             8
#define    APOLLO_CLASSIFY_IP_RANG_NUM                 8
#define    APOLLO_MAX_NUM_OF_ACL_TEMPLATE              4
#define    APOLLO_MAX_NUM_OF_ACL_RULE_FIELD            8
#define    APOLLO_MAX_NUM_OF_ACL_ACTION                128
#define    APOLLO_MAX_NUM_OF_ACL_RULE_ENTRY            64
#define    APOLLO_MAX_SPECIAL_CONGEST_SEC              (15)
#define    APOLLO_MAX_ENHANCED_FID                     7
#define    APOLLO_MAX_NUM_OF_LOG_MIB                   32
#define    APOLLO_EXT_CPU_PORT_ID                      (0)
#define    APOLLO_PONMAC_TCONT_QUEUE_MAX               (32)
#define    APOLLO_PONMAC_PIR_CIR_RATE_MAX              (0x1FFFF)
#define    APOLLO_MAX_MIB_LATCH_TIMER                  (0xff)
#define    APOLLO_MAX_RGMII_TX_DELAY                   (1)
#define    APOLLO_MAX_RGMII_RX_DELAY                   (7)


/*chip dependent define area*/
#define APOLLO_TCONT_QUEUE_MAX  32
#define APOLLO_SEC_FLOOD_THRESHOLD_MAX 	0x00FF

#define APOLLO_METER_BUCKETSIZE_MAX         0XFFFF


#define APOLLO_DAL_SVLAN_ENTRY_NO       64
#define APOLLO_DAL_SVLAN_C2S_NO         128
#define APOLLO_DAL_SVLAN_MC2S_NO        8
#define APOLLO_DAL_SVLAN_SP2C_NO        128

#define APOLLO_PACEKT_LENGTH_MAX 	        0x3FFF


#define APOLLO_RATE_MAX                 (0x1FFFF * 8)
#define APOLLO_APR_METER_MAX                 (0x8)


#define APOLLO_DAL_MAX_ENHANCE_VID      (0x1FFF)


#define APOLLO_LED_INDEX_MAX                 (31)

#endif /*#ifndef _APOLLO_DEF_H_*/

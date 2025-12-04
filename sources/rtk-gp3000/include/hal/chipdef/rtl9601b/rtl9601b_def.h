#ifndef _RTL9601B_DEF_H_
#define _RTL9601B_DEF_H_

/*chip capacity define area*/
#define    RTL9601B_MAX_NUM_OF_MIRROR                    0
#define    RTL9601B_MAX_NUM_OF_TRUNK                     0
#define    RTL9601B_MAX_NUM_OF_TRUNKMEMBER               0
#define    RTL9601B_MAX_NUM_OF_DUMB_TRUNKMEMBER          0
#define    RTL9601B_MAX_NUM_OF_TRUNKHASHVAL              0
#define    RTL9601B_MAX_NUM_OF_MSTI                      16
#define    RTL9601B_MAX_NUM_OF_METERING                  8
#define    RTL9601B_MAX_NUM_OF_FIELD_SELECTOR            16
#define    RTL9601B_MAX_NUM_OF_RANGE_CHECK_SRCPORT       0
#define    RTL9601B_MAX_NUM_OF_RANGE_CHECK_IP            8
#define    RTL9601B_MAX_NUM_OF_RANGE_CHECK_VID           8
#define    RTL9601B_MAX_NUM_OF_RANGE_CHECK_PKTLEN        8
#define    RTL9601B_MAX_NUM_OF_RANGE_CHECK_L4PORT        16
#define    RTL9601B_MAX_NUM_OF_PATTERN_MATCH_DATA        0
#define    RTL9601B_PATTERN_MATCH_PORT_MAX               0
#define    RTL9601B_PATTERN_MATCH_PORT_MIN               0
#define    RTL9601B_MAX_NUM_OF_L2_HASHDEPTH              4
#define    RTL9601B_MAX_NUM_OF_QUEUE                     8
#define    RTL9601B_MIN_NUM_OF_QUEUE                     1
#define    RTL9601B_MAX_NUM_OF_PON_QUEUE                 33
#define    RTL9601B_MIN_NUM_OF_PON_QUEUE                 1
#define    RTL9601B_MAX_NUM_OF_CVLAN_TPID                1
#define    RTL9601B_MAX_NUM_OF_SVLAN_TPID                1
#define    RTL9601B_TPID_ENTRY_IDX_MAX                   0
#define    RTL9601B_TPID_ENTRY_MASK_MAX                  0x0
#define    RTL9601B_PROTOCOL_VLAN_IDX_MAX                3
#define    RTL9601B_MAX_NUM_VLAN_MBRCFG                  0
#define    RTL9601B_VLAN_FID_MAX                         0x0
#define    RTL9601B_FLOWCTRL_THRESH_MAX                  0x1FFF
#define    RTL9601B_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX     0xFF
#define    RTL9601B_PRI_OF_SELECTION_MAX                 7
#define    RTL9601B_PRI_OF_SELECTION_MIN                 0
#define    RTL9601B_PRI_SEL_GROUP_INDEX_MAX              2
#define    RTL9601B_PRI_SEL_WEIGHT_MAX                   7
#define    RTL9601B_QUEUE_WEIGHT_MAX                     127
#define    RTL9601B_QID0_WEIGHT_MAX                      65535
#define    RTL9601B_RATE_OF_BANDWIDTH_MAX                0xFFFF
#define    RTL9601B_THRESH_OF_IGR_BW_FLOWCTRL_MAX        0xFF
#define    RTL9601B_MAX_NUM_OF_FASTPATH_OF_RATE          0
#define    RTL9601B_RATE_OF_STORM_CONTROL_MAX            0x1FFFF
#define    RTL9601B_BURST_RATE_OF_STORM_CONTROL_MAX      0x1FFF
#define    RTL9601B_INTERNAL_PRIORITY_MAX                7
#define    RTL9601B_DROP_PRECEDENCE_MAX                  0
#define    RTL9601B_PRIORITY_REMAP_GROUP_IDX_MAX         0
#define    RTL9601B_PRIORITY_REMARK_GROUP_IDX_MAX        0
#define    RTL9601B_PRIORITY_TO_QUEUE_GROUP_IDX_MAX      3
#define    RTL9601B_WRED_WEIGHT_MAX                      0x0
#define    RTL9601B_WRED_MPD_MAX                         0x0
#define    RTL9601B_ACL_RATE_MAX                         0x1FFFF
#define    RTL9601B_L2_LEARN_LIMIT_CNT_MAX               (256+8)
#define    RTL9601B_L2_LEARN_4WAY_NO                     (256)
#define    RTL9601B_L2_AGING_TIME_MAX                    (0x1FFFFF)
#define    RTL9601B_L2_ENTRY_AGING_MAX                   7
#define    RTL9601B_EEE_QUEUE_THRESH_MAX                 0xFF
#define    RTL9601B_SEC_MINIPV6FRAGLEN_MAX               0xFFFF
#define    RTL9601B_SEC_MAXPINGLEN_MAX                   0xFFFF
#define    RTL9601B_SEC_SMURFNETMASKLEN_MAX              31
#define    RTL9601B_SFLOW_RATE_MAX                       0xFFFF
#define    RTL9601B_MAX_NUM_OF_MCAST_FWD                 (2048+64)
#define    RTL9601B_MIIM_PAGE_ID_MIN                     0x0000
#define    RTL9601B_MIIM_PAGE_ID_MAX                     0xFFFF
#define    RTL9601B_MIIM_REG_ID_MAX                      (31)
#define    RTL9601B_MIIM_DATA_MAX                        (0xFFFF)
#define    RTL9601B_L34_NETIF_TABLE_MAX                  0
#define    RTL9601B_L34_ARP_TABLE_MAX                    0
#define    RTL9601B_L34_EXTIP_TABLE_MAX                  0
#define    RTL9601B_L34_ROUTING_TABLE_MAX                0
#define    RTL9601B_L34_NAPT_TABLE_MAX                   0
#define    RTL9601B_L34_NAPTR_TABLE_MAX                  0
#define    RTL9601B_L34_NH_TABLE_MAX                     0
#define    RTL9601B_L34_PPPOE_TABLE_MAX                  0
#define    RTL9601B_GPON_TCONT_MAX                       9
#define    RTL9601B_GPON_FLOW_MAX                        128
#define    RTL9601B_CLASSIFY_ENTRY_MAX                   256
#define    RTL9601B_CLASSIFY_SID_MAX                     33
#define    RTL9601B_CLASSIFY_L4PORT_RANGE_NUM			 0
#define    RTL9601B_CLASSIFY_IP_RANGE_NUM                0
#define    RTL9601B_MAX_NUM_OF_ACL_TEMPLATE              4
#define    RTL9601B_MAX_NUM_OF_ACL_RULE_FIELD            8
#define    RTL9601B_MAX_NUM_OF_ACL_ACTION                64
#define    RTL9601B_MAX_NUM_OF_ACL_RULE_ENTRY            64
#define    RTL9601B_MAX_SPECIAL_CONGEST_SEC              (15)
#define    RTL9601B_MAX_NUM_OF_METER                     8
#define    RTL9601B_MAX_ENHANCED_FID                     7
#define    RTL9601B_MAX_NUM_OF_LOG_MIB                   16
#define    RTL9601B_EXT_CPU_PORT_ID                      (0)
#define    RTL9601B_PONMAC_TCONT_QUEUE_MAX               (33)
#define    RTL9601B_PONMAC_PIR_CIR_RATE_MAX              (0x3FFFF)
#define    RTL9601B_MAX_MIB_LATCH_TIMER                  (0xff)
#define    RTL9601B_MAX_NUM_OF_NEXTHOP                   0
#define    RTL9601B_MAX_RGMII_TX_DELAY                   (1)
#define    RTL9601B_MAX_RGMII_RX_DELAY                   (7)

#define    RTL9601B_MAX_LLID_ENTRY                   (2)


/*chip dependent define area*/

#define RTL9601B_TCONT_QUEUE_MAX  32
#define RTL9601B_SEC_FLOOD_THRESHOLD_MAX 	0x00FF

#define RTL9601B_QUEUENO                      8
#define RTL9601B_QIDMAX                       (RTL9601B_QUEUENO-1)


/*vlan*/
#define RTL9601B_VIDMAX                       0xFFF
#define RTL9601B_EVIDMAX                      0x1FFF
#define RTL9601B_CVIDXNO                      32
#define RTL9601B_CVIDXMAX                     (RTL9601B_CVIDXNO-1)
#define RTL9601B_PROTOVLAN_GIDX_MAX 3
#define RTL9601B_PROTOVLAN_GROUPNO  4

#define RTL9601B_EXTPORTNO                    0
#define RTL9601B_EXTPORTNOMAX                 0

#define RTL9601B_SVLAN_ENTRY_NO       64
#define RTL9601B_SVLAN_C2S_NO         128
#define RTL9601B_SVLAN_MC2S_NO        8
#define RTL9601B_SVLAN_SP2C_NO        128

#define RTL9601B_PACEKT_LENGTH_MAX 	        0x3FFF
#define RTL9601B_MAXLENGTH_INDEX_NUM 	        2

#define RTL9601B_MAX_ENHANCE_VID      (0x1FFF)


#define RTL9601B_PRI_WEIGHT_MAX    		    15
#define RTL9601B_QOS_INBW_THRESHOLD_MAX 	0xF
#define RTL9601B_QOS_GRANULARTY_MAX           0x1FFFF


#define RTL9601B_METER_BUCKETSIZE_MAX         0XFFFF

#define RTL9601B_RATE_MAX                 (0x1FFFF * 8)
#define RTL9601B_APR_METER_MAX                 (0x8)

#define RTL9601B_RMA_L2TAILNUM_MAX  0x2f

#define RTL9601B_IPMC_TRANS_MAX  0x15


#define    RTL9601B_L34_IPV6_ROUTING_TABLE_MAX            0
#define    RTL9601B_L34_IPV6_NBR_TABLE_MAX                0
#define    RTL9601B_L34_BINDING_TABLE_MAX                 0
#define    RTL9601B_L34_WAN_TYPE_TABLE_MAX                0

#define RTL9601B_LED_INDEX_MAX                 (31)

#endif /*#ifndef _RTL9601B_DEF_H_*/

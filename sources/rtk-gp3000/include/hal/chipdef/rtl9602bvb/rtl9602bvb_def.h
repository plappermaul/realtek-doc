#ifndef _RTL9602BVB_DEF_H_
#define _RTL9602BVB_DEF_H_

/*chip capacity define area*/
#define    RTL9602BVB_MAX_NUM_OF_MIRROR                    0
#define    RTL9602BVB_MAX_NUM_OF_TRUNK                     0
#define    RTL9602BVB_MAX_NUM_OF_TRUNKMEMBER               0
#define    RTL9602BVB_MAX_NUM_OF_DUMB_TRUNKMEMBER          0
#define    RTL9602BVB_MAX_NUM_OF_TRUNKHASHVAL              0
#define    RTL9602BVB_MAX_NUM_OF_MSTI                      16
#define    RTL9602BVB_MAX_NUM_OF_METERING                  16
#define    RTL9602BVB_MAX_NUM_OF_FIELD_SELECTOR            16
#define    RTL9602BVB_MAX_NUM_OF_RANGE_CHECK_SRCPORT       0
#define    RTL9602BVB_MAX_NUM_OF_RANGE_CHECK_IP            32
#define    RTL9602BVB_MAX_NUM_OF_RANGE_CHECK_VID           8
#define    RTL9602BVB_MAX_NUM_OF_RANGE_CHECK_PKTLEN        8
#define    RTL9602BVB_MAX_NUM_OF_RANGE_CHECK_L4PORT        32
#define    RTL9602BVB_MAX_NUM_OF_PATTERN_MATCH_DATA        0
#define    RTL9602BVB_PATTERN_MATCH_PORT_MAX               0
#define    RTL9602BVB_PATTERN_MATCH_PORT_MIN               0
#define    RTL9602BVB_MAX_NUM_OF_L2_HASHDEPTH              4
#define    RTL9602BVB_MAX_NUM_OF_QUEUE                     8
#define    RTL9602BVB_MIN_NUM_OF_QUEUE                     1
#define    RTL9602BVB_MAX_NUM_OF_PON_QUEUE                 65
#define    RTL9602BVB_MIN_NUM_OF_PON_QUEUE                 1
#define    RTL9602BVB_MAX_NUM_OF_CVLAN_TPID                1
#define    RTL9602BVB_MAX_NUM_OF_SVLAN_TPID                2
#define    RTL9602BVB_TPID_ENTRY_IDX_MAX                   0
#define    RTL9602BVB_TPID_ENTRY_MASK_MAX                  0x0
#define    RTL9602BVB_PROTOCOL_VLAN_IDX_MAX                3
#define    RTL9602BVB_MAX_NUM_VLAN_MBRCFG                  0
#define    RTL9602BVB_VLAN_FID_MAX                         0x1
#define    RTL9602BVB_FLOWCTRL_THRESH_MAX                  0x1FFF
#define    RTL9602BVB_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX     0xFF
#define    RTL9602BVB_PRI_OF_SELECTION_MAX                 7
#define    RTL9602BVB_PRI_OF_SELECTION_MIN                 0
#define    RTL9602BVB_PRI_SEL_GROUP_INDEX_MAX              2
#define    RTL9602BVB_PRI_SEL_WEIGHT_MAX                   7
#define    RTL9602BVB_QUEUE_WEIGHT_MAX                     127
#define    RTL9602BVB_QID0_WEIGHT_MAX                      65535
#define    RTL9602BVB_RATE_OF_BANDWIDTH_MAX                0xFFFF
#define    RTL9602BVB_THRESH_OF_IGR_BW_FLOWCTRL_MAX        0xFF
#define    RTL9602BVB_MAX_NUM_OF_FASTPATH_OF_RATE          0
#define    RTL9602BVB_RATE_OF_STORM_CONTROL_MAX            0x1FFFF
#define    RTL9602BVB_BURST_RATE_OF_STORM_CONTROL_MAX      0x1FFF
#define    RTL9602BVB_INTERNAL_PRIORITY_MAX                7
#define    RTL9602BVB_DROP_PRECEDENCE_MAX                  0
#define    RTL9602BVB_PRIORITY_REMAP_GROUP_IDX_MAX         0
#define    RTL9602BVB_PRIORITY_REMARK_GROUP_IDX_MAX        0
#define    RTL9602BVB_PRIORITY_TO_QUEUE_GROUP_IDX_MAX      3
#define    RTL9602BVB_WRED_WEIGHT_MAX                      0x0
#define    RTL9602BVB_WRED_MPD_MAX                         0x0
#define    RTL9602BVB_ACL_RATE_MAX                         0x1FFFF
#define    RTL9602BVB_L2_LEARN_LIMIT_CNT_MAX               (1024+64)
#define    RTL9602BVB_L2_LEARN_4WAY_NO                     (1024)
#define    RTL9602BVB_L2_AGING_TIME_MAX                    (0x1FFFFF)
#define    RTL9602BVB_L2_ENTRY_AGING_MAX                   7
#define    RTL9602BVB_EEE_QUEUE_THRESH_MAX                 0xFF
#define    RTL9602BVB_SEC_MINIPV6FRAGLEN_MAX               0xFFFF
#define    RTL9602BVB_SEC_MAXPINGLEN_MAX                   0xFFFF
#define    RTL9602BVB_SEC_SMURFNETMASKLEN_MAX              31
#define    RTL9602BVB_SFLOW_RATE_MAX                       0xFFFF
#define    RTL9602BVB_MAX_NUM_OF_MCAST_FWD                 (1024+64)
#define    RTL9602BVB_MIIM_PAGE_ID_MIN                     0x0000
#define    RTL9602BVB_MIIM_PAGE_ID_MAX                     0xFFFF
#define    RTL9602BVB_MIIM_REG_ID_MAX                      (31)
#define    RTL9602BVB_MIIM_DATA_MAX                        (0xFFFF)
#define    RTL9602BVB_L34_NETIF_TABLE_MAX                  16
#define    RTL9602BVB_L34_ARP_TABLE_MAX                    128
#define    RTL9602BVB_L34_EXTIP_TABLE_MAX                  8
#define    RTL9602BVB_L34_ROUTING_TABLE_MAX                16
#define    RTL9602BVB_L34_NAPT_TABLE_MAX                   4096
#define    RTL9602BVB_L34_NAPTR_TABLE_MAX                  4096
#define    RTL9602BVB_L34_NH_TABLE_MAX                     16
#define    RTL9602BVB_L34_PPPOE_TABLE_MAX                  8
#define    RTL9602BVB_L34_IPV6_ROUTING_TABLE_MAX            8
#define    RTL9602BVB_L34_IPV6_NBR_TABLE_MAX                128
#define    RTL9602BVB_L34_BINDING_TABLE_MAX                 32
#define    RTL9602BVB_L34_WAN_TYPE_TABLE_MAX                16
#define    RTL9602BVB_L34_FLOW_ROUTE_TABLE_MAX              256
#define    RTL9602BVB_L34_DSLITE_INF_TABLE_MAX              4
#define    RTL9602BVB_L34_DSLITE_MCAST_TABLE_MAX            4
#define    RTL9602BVB_L34_IPMC_TRAN_TABLE_MAX              16
#define    RTL9602BVB_GPON_TCONT_MAX                       17
#define    RTL9602BVB_GPON_FLOW_MAX                        128
#define    RTL9602BVB_CLASSIFY_ENTRY_MAX                   256
#define    RTL9602BVB_CLASSIFY_SID_MAX                     65
#define    RTL9602BVB_CLASSIFY_L4PORT_RANGE_NUM            0
#define    RTL9602BVB_CLASSIFY_IP_RANGE_NUM                0
#define    RTL9602BVB_MAX_NUM_OF_ACL_TEMPLATE              8
#define    RTL9602BVB_MAX_NUM_OF_ACL_RULE_FIELD            8
#define    RTL9602BVB_MAX_NUM_OF_ACL_ACTION                64
#define    RTL9602BVB_MAX_NUM_OF_ACL_RULE_ENTRY            64
#define    RTL9602BVB_MAX_SPECIAL_CONGEST_SEC              (15)
#define    RTL9602BVB_MAX_NUM_OF_METER                     32
#define    RTL9602BVB_MAX_ENHANCED_FID                     0
#define    RTL9602BVB_MAX_NUM_OF_LOG_MIB                   32
#define    RTL9602BVB_EXT_CPU_PORT_ID                      (0)
#define    RTL9602BVB_PONMAC_TCONT_QUEUE_MAX               (32)
#define    RTL9602BVB_PONMAC_PIR_CIR_RATE_MAX              (0x3FFFF)
#define    RTL9602BVB_MAX_MIB_LATCH_TIMER                  (0xff)
#define    RTL9602BVB_MAX_NUM_OF_NEXTHOP                   16
#define    RTL9602BVB_MAX_RGMII_TX_DELAY                   (1)
#define    RTL9602BVB_MAX_RGMII_RX_DELAY                   (7)

#define    RTL9602BVB_MAX_LLID_ENTRY                   	 (8)


#if 1
#define    RTL9602BVB_EPON_QUEUE_NUM_PER_LLID          	 (8)
#else  /*if create 8 llid only 4 queue for each LLID*/
#define    RTL9602BVB_EPON_QUEUE_NUM_PER_LLID          	 (4)
#endif

/*chip dependent define area*/

#define RTL9602BVB_TCONT_QUEUE_MAX  32
#define RTL9602BVB_SEC_FLOOD_THRESHOLD_MAX 	0x00FF

/*vlan*/
#define RTL9602BVB_VIDMAX                       0xFFF
#define RTL9602BVB_PROTOVLAN_GIDX_MAX 3
#define RTL9602BVB_PROTOVLAN_GROUPNO  4

#define RTL9602BVB_SVLAN_SP2C_NO        16

#define RTL9602BVB_PACEKT_LENGTH_MAX 	        0x3FFF
#define RTL9602BVB_MAXLENGTH_INDEX_NUM 	        2

#define RTL9602BVB_MAX_ENHANCE_VID      (0x1FFF)


#define RTL9602BVB_PRI_WEIGHT_MAX    		    15
#define RTL9602BVB_QOS_INBW_THRESHOLD_MAX 	0xF
#define RTL9602BVB_QOS_GRANULARTY_MAX           0x1FFFF


#define RTL9602BVB_METER_BUCKETSIZE_MAX         0XFFFF
#define RTL9602BVB_INGRESS_RATE_MAX                 (0x7FFFF * 8)
#define RTL9602BVB_EGRESS_RATE_MAX                 (0x7FFFF * 8)
#define RTL9602BVB_METER_RATE_MAX                 (0x7FFFF * 8)

#define RTL9602BVB_RMA_L2TAILNUM_MAX  0x2f

#define RTL9602BVB_IPMC_TRANS_MAX  15


#define RTL9602BVB_LED_INDEX_MAX                 (31)

#endif /*#ifndef _RTL9602BVB_DEF_H_*/

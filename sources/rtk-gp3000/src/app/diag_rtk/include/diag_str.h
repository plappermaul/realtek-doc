/*
 * Copyright (C) 2009-2010 Realtek Semiconductor Corp.
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
 * Purpose : Define those public diag shell string
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Diag shell string definition
 */


#ifndef __DIAG_STR_H__
#define __DIAG_STR_H__

/*
 * Include Files
 */

/*
 * Symbol Definition
 */
#define DIAG_STR_ENABLE                 "Enable"
#define DIAG_STR_DISABLE                "Disable"
#define DIAG_STR_SUCCESS                "Success"
#define DIAG_STR_FAILURE                "Failure"
#define DIAG_STR_VALID                  "Valid"
#define DIAG_STR_INVALID                "Invalid"
#define DIAG_STR_YES                    "Yes"
#define DIAG_STR_NO                     "No"
#define DIAG_STR_NOT                    "Not"
#define DIAG_STR_NONE                   "None"
#define DIAG_STR_FAULT                  "Fault"
#define DIAG_STR_All                    "All"
#define DIAG_STR_ACL                    "Acl"
#define DIAG_STR_REASON                 "Reason"

#define DIAG_STR_DOING                  "Doing"
#define DIAG_STR_FIN                    "Finish"


#define DIAG_STR_DROP                   "Drop"
#define DIAG_STR_TRAP2CPU               "Trap to CPU"
#define DIAG_STR_DROP_PON               "Drop packet to PON"
#define DIAG_STR_FORWARD                "Forward"
#define DIAG_STR_ASSIGN_SVLAN           "Assign to SVLAN"
#define DIAG_STR_ASSIGN_SVLAN_AND_KEEP  "Assign to SVLAN and Keep format"
#define DIAG_STR_USE_CVID               "Using CVID"
#define DIAG_STR_PB_SVID                "Port based SVID"
#define DIAG_STR_DISCARD                "Discard"
#define DIAG_STR_FORWARD_NOP            "Forward Nop"
#define DIAG_STR_TRAP2SLAVE             "Trap to slave CPU"

#define DIAG_STR_COPY2CPU               "Copy to CPU"
#define DIAG_STR_2GUESTVLAN             "Forward to guest VLAN"
#define DIAG_STR_FLOOD_IN_VLAN          "Flood in VLAN"
#define DIAG_STR_FLOOD_2_ALLPORT        "Flood to all ports"
#define DIAG_STR_FLOOD_2_ROUTER_PORT    "Flood to Router ports"
#define DIAG_STR_FORWARD_EX_CPU         "Forward exclude CPU port"
#define DIAG_STR_DROP_EX_RMA            "Drop exclude RMA"
#define DIAG_STR_FOLLOW_FB              "Follow FB decision"
#define DIAG_STR_COPY                   "Copy"
#define DIAG_STR_REDIRECT               "Redirect"
#define DIAG_STR_MIRROR                 "Mirror"
#define DIAG_STR_NOTSUPPORT             "Not support"
#define DIAG_STR_GUEST_VLAN             "Guest VLAN"
#define DIAG_STR_EGRESS_MEMBER          "Egress Member"
#define DIAG_STR_NORMAL_UNKNOWN_MC      "Normal unknown multicast"
#define DIAG_STR_NORMAL_ADDR            "Normal multicast address"
#define DIAG_STR_RESERVED_ADDR          "Reserved address"

#define DIAG_STR_ALWAYS                 "Always"
#define DIAG_STR_PKTCHANGE              "Packet was changed"

#define DIAG_STR_FIRSTDROP              "The 1st drop"
#define DIAG_STR_FIRSTTRAP2CPU          "The 1st trap to CPU"
#define DIAG_STR_FIRSTPASS              "The 1st pass"


#define DIAG_STR_INTERNAL_PRI           "Internal priority"
#define DIAG_STR_1Q_TAG_PRI             "Dot1q tag priority"
#define DIAG_STR_SPRI                   "SPRI of SVLAN"
#define DIAG_STR_PB_PRI                 "Port based priority"
#define DIAG_STR_USER_PRI               "User priority"
#define DIAG_STR_OUTER_TAG              "Outer tag priority"

#define DIAG_STR_MAC31_0                "MAC[31:0]"
#define DIAG_STR_IPV4_DIP               "IPv4 Dip"

#define DIAG_STR_IGMPV1                 "IGMPv1"
#define DIAG_STR_IGMPV2                 "IGMPv2"
#define DIAG_STR_IGMPV3                 "IGMPv3"
#define DIAG_STR_MLDV1                  "MLDv1"
#define DIAG_STR_MLDV2                  "MLDv2"

#define DIAG_STR_VLAN                   "VLAN"
#define DIAG_STR_MAC                    "MAC"
#define DIAG_STR_PORT                   "PORT"


#define DIAG_STR_TCP                    "TCP"
#define DIAG_STR_UDP                    "UDP"


#define DIAG_STR_NAPT                   "NAPT"
#define DIAG_STR_NAPTR                  "NAPTR"
#define DIAG_STR_NAT                    "NAT"
#define DIAG_STR_NATR                   "NATR"

#define DIAG_STR_LOCAL_PUBLIC           "LP"
#define DIAG_STR_REMOTE_PUBLIC          "RP"

#define DIAG_STR_REVISE                 "Revise"
#define DIAG_STR_NOTREVISE              "Not Revise"

#define DIAG_STR_SPORT                  "Source Port"
#define DIAG_STR_DPORT                  "Destination Port"

#define DIAG_STR_IPV4_SIP               "Ipv4 Sip"
#define DIAG_STR_IPV6_SIP               "Ipv6 Sip"
#define DIAG_STR_IPV6_DIP               "Ipv6 Dip"
#define DIAG_STR_IPV4_SIP_INNER         "Ipv4 Sip Inner"
#define DIAG_STR_IPV4_DIP_INNER         "Ipv4 Dip Inner"

#define DIAG_STR_IPV4                   "IPv4"
#define DIAG_STR_IPV6                   "IPv6"

#define DIAG_STR_CVID                   "CVID"
#define DIAG_STR_SVID                   "SVID"

#define DIAG_STR_64ENTIRES              "64 Entries"
#define DIAG_STR_128ENTIRES             "128 Entries"


#define DIAG_STR_ROUTE_TYPE_GLOBAL      "Global Route"
#define DIAG_STR_ROUTE_TYPE_LOCAL       "Local Route"

#define DIAG_STR_INGRESS_VLAN           "Ingress Vlan"
#define DIAG_STR_EGRESS_VLAN            "Egress Vlan"

#define DIAG_STR_POLICING               "Policing"
#define DIAG_STR_ACLMIB                 "Acl MIB"
#define DIAG_STR_1PREMARK               "1P Remarking"
#define DIAG_STR_DSCPREMARK             "DSCP Remarking"
#define DIAG_STR_ACLPRI                 "ACL priority"
#define DIAG_STR_SID                    "Stream ID"
#define DIAG_STR_LLID                   "LLID"
#define DIAG_STR_DSLEXT                 "DSL/EXT"
#define DIAG_STR_POLICY_ROUTE           "Policy Route"
#define DIAG_STR_BW_METERING            "BW Metering"
#define DIAG_STR_TOSREMARK				"ToS Remarking"

#define DIAG_STR_DIPONLY                "DIP only"
#define DIAG_STR_DIPSIP                 "DIP + SIP"
#define DIAG_STR_MACFID                 "MAC + FID"
#define DIAG_STR_DIPVID                 "DIP + VID"
#define DIAG_STR_DIPCVID                "DIP + CVID"
#define DIAG_STR_DIPVIDFID              "DIP + (VID or FID)"

#define DIAG_STR_IVLSVL                 "ivl-svl"
#define DIAG_STR_NOVLAN                 "no vlan"
#define DIAG_STR_ALWAYSVLAN             "always vlan"

#define DIAG_STR_STATIC                 "Static"
#define DIAG_STR_AUTO                   "Auto"
#define DIAG_STR_IVL                    "IVL"
#define DIAG_STR_SVL                    "SVL"
#define DIAG_STR_DYNAMIC                "Dynamic"
#define DIAG_STR_BOTH                   "Both"
#define DIAG_STR_IN                     "In"

#define DIAG_STR_ETHERNET               "Ethernet"
#define DIAG_STR_PPPOE                  "PPPoE"
#define DIAG_STR_SNAP                   "SNAP"
#define DIAG_STR_LLC_OTHER              "LLC Other"


#define DIAG_STR_V                      "En"
#define DIAG_STR_X                      "Dis"

#define DIAG_STR_UPSTREAM               "Upstream"
#define DIAG_STR_DOWNSTREAM             "Downstream"

#define DIAG_STR_HIT                    "Hit"
#define DIAG_STR_NOT                    "Not"


#define DIAG_STR_TRUNK_DUMB_MODE        "Dumb mode"
#define DIAG_STR_TRUNK_NORMAL_MODE      "Normal mode"

#define DIAG_STR_TRUNK_HASH_SOURCE_PORT     "Source Port"
#define DIAG_STR_TRUNK_HASH_SOURCE_MAC      "Source MAC"
#define DIAG_STR_TRUNK_HASH_DEST_MAC        "Destination MAC"
#define DIAG_STR_TRUNK_HASH_SOURCE_IP       "Source IP"
#define DIAG_STR_TRUNK_HASH_DEST_IP         "Destination IP"
#define DIAG_STR_TRUNK_HASH_SOURCE_L4PORT   "Source L4 Port"
#define DIAG_STR_TRUNK_HASH_DEST_L4PORT     "Destination L4 Port"

#define DIAG_STR_TRUNK_FLOOD_NORMAL         "Normal Flood"
#define DIAG_STR_TRUNK_FLOOD_TO_FIRST       "Forward to first port"

#define DIAG_STR_ACT_DISABLE            "Action disable"
#define DIAG_STR_NOP                    "No operation"
#define DIAG_STR_ADD                    "Add"
#define DIAG_STR_DEL                    "Delete"
#define DIAG_STR_ASSIGN                 "Assign"
#define DIAG_STR_VS_TPID                "VS_TPID"
#define DIAG_STR_VS_TPID2               "VS_TPID2"
#define DIAG_STR_TPID_8100              "TPID_8100"
#define DIAG_STR_STAG_TPID              "Stag_TPID"
#define DIAG_STR_UNTAG                  "Untagging"
#define DIAG_STR_TAG                    "Tagging"
#define DIAG_STR_C2S                    "C2S translate"
#define DIAG_STR_SP2C                   "SP2C translate"
#define DIAG_STR_TRANSPARENT            "Transparent"
#define DIAG_STR_COPY_C                 "Copy from outer tag"
#define DIAG_STR_COPY_S                 "Copy from outer tag"
#define DIAG_STR_COPY_1st               "Copy from 1st tag"
#define DIAG_STR_COPY_2nd               "Copy from 2nd tag"
#define DIAG_STR_INTER_PRI              "Assign from internal priority"
#define DIAG_STR_INTER_VID              "Assign from internal VID"
#define DIAG_STR_DSCP_PRI               "Assign from DSCP priority"
#define DIAG_STR_COPY_FROM_TOS          "Copy from TOS"

#define DIAG_STR_ASSIGN_QID             "ASSIGN to QID"
#define DIAG_STR_ASSIGN_SID             "ASSIGN to SID"
#define DIAG_STR_SWITCH_CORE            "Follow switch core"
#define DIAG_STR_LUT_LRN                "LUT learning"
#define DIAG_STR_FS_FORWARD             "Force forward"
#define DIAG_STR_ON_OPERATION           "No operation"
#define DIAG_STR_STORM_UNKN_MC          "Unknown Multicast"
#define DIAG_STR_STORM_UNKN_UC          "Unknown Unicast"
#define DIAG_STR_STORM_MC               "Multicast"
#define DIAG_STR_STORM_BC               "Broadcast"

#define DIAG_STR_CVLAN_4K               "CVLAN 4K table"
#define DIAG_STR_SVLAN_64               "SVLAN 64 entries"

#define DIAG_STR_DEFAULT                "Default"
#define DIAG_STR_STORM_ALT_ARP          "ARP-Storm"
#define DIAG_STR_STORM_ALT_DHCP         "DHCP-Storm"
#define DIAG_STR_STORM_ALT_IGMPMLD      "IGMPMLD-Storm"


#define DIAG_STR_AUTHORIZED             "Authorized"
#define DIAG_STR_UNAUTHORIZED           "Un-Authorized"

#define DIAG_STR_DOT1XOPDIR_BOTH        "BOTH"
#define DIAG_STR_DOT1XOPDIR_IN          "IN"

#define  DIAG_STR_FLOWCTRL_INGRESS   "Ingress Flow Control"
#define  DIAG_STR_FLOWCTRL_EGRESS    "Egress Flow Control"
#define  DIAG_STR_FLOWCTRL_JUMBO_3K       "3Kbytes"
#define  DIAG_STR_FLOWCTRL_JUMBO_4K       "4Kbytes"
#define  DIAG_STR_FLOWCTRL_JUMBO_6K       "6Kbytes"
#define  DIAG_STR_FLOWCTRL_JUMBO_MAX    "maximum size (8960 bytes)"
#define DIAG_STR_90PPM                  "90PPM"
#define DIAG_STR_65PPM                  "65PPM"

#define DIAG_STR_AFB_MONCOUNT_8K        "8K packets"
#define DIAG_STR_AFB_MONCOUNT_16K       "16K packets"
#define DIAG_STR_AFB_MONCOUNT_32K       "32K packets"
#define DIAG_STR_AFB_MONCOUNT_64K       "64K packets"
#define DIAG_STR_AFB_MONCOUNT_128K      "128K packets"
#define DIAG_STR_AFB_MONCOUNT_256K      "256K packets"
#define DIAG_STR_AFB_MONCOUNT_512K      "512K packets"
#define DIAG_STR_AFB_MONCOUNT_1M        "1M packets"

#define DIAG_STR_AFB_ERRCOUNT_1         "1 packet"
#define DIAG_STR_AFB_ERRCOUNT_2         "2 packets"
#define DIAG_STR_AFB_ERRCOUNT_4         "4 packets"
#define DIAG_STR_AFB_ERRCOUNT_8         "8 packets"
#define DIAG_STR_AFB_ERRCOUNT_16        "16 packets"
#define DIAG_STR_AFB_ERRCOUNT_32        "32 packets"
#define DIAG_STR_AFB_ERRCOUNT_64        "64 packets"
#define DIAG_STR_AFB_ERRCOUNT_128       "128 packets"

#define DIAG_STR_AFB_NOT_RESTORE_PL     "Not Restore Power Level"
#define DIAG_STR_AFB_RESTORE_PL         "Restore Power Level"

#define DIAG_STR_AFB_NONE_VALID_FLOW    "None Valid Flow"
#define DIAG_STR_AFB_VALID_FLOW         "Valid Flow"

#define DIAG_STR_QUEUE_STRICT           "strict"
#define DIAG_STR_QUEUE_WFQ              "WFQ"

#define DIAG_STR_SPEED_10M              "10M"
#define DIAG_STR_SPEED_100M             "100M"
#define DIAG_STR_SPEED_GIGA             "1000M"
#define DIAG_STR_SPEED_500M             "500M"
#define DIAG_STR_SPEED_2G5              "2.5G"
#define DIAG_STR_SPEED_2G5LITE          "2.5GLite"
#define DIAG_STR_SPEED_5G               "5G"
#define DIAG_STR_SPEED_5GLITE           "5GLite"
#define DIAG_STR_SPEED_10G              "10G"






#define DIAG_STR_HALF_DUPLEX            "Half"
#define DIAG_STR_FULL_DUPLEX            "Full"
#define DIAG_STR_LINK_UP                "Up"
#define DIAG_STR_LINK_DOWN              "Down"



#define DIAG_STR_RAW                    "Raw packet"
#define DIAG_STR_LLC                    "LLC packet"
#define DIAG_STR_ARP                    "Arp packet"
#define DIAG_STR_IP4HEADER              "IPv4 header"
#define DIAG_STR_IP6HEADER              "IPv6 header"
#define DIAG_STR_IPPAYLOAD              "IP payload"
#define DIAG_STR_L4PAYLOAD              "L4 payload"
#define DIAG_STR_INNER_IP4HEADER        "Inner IPv4 header"
#define DIAG_STR_INNER_IP6HEADER        "Inner IPv6 header"
#define DIAG_STR_INNER_IPPAYLOAD        "Inner IP payload"


#define  DIAG_STR_SW_VOIP_RST          "VoIP reset"
#define  DIAG_STR_SW_PCIE_PHY_RST   "PCIE PHY reset"
#define  DIAG_STR_SW_PCIE_CTRL_RST  "PCIE controller reset"
#define  DIAG_STR_SW_USB3_PHY_RST  "USB3 PHY reset"
#define  DIAG_STR_SW_USB3_CTRL_RST "USB3 controller reset"
#define  DIAG_STR_SW_USB2_PHY_RST  "USB2 PHY reset"
#define  DIAG_STR_SW_USB2_CTRL_RST "USB2 controller reset"
#define  DIAG_STR_SW_SATA_PHY_RST  "SATA PHY reset"
#define  DIAG_STR_SW_SATA_CTRL_RST "SATA controller reset"
#define  DIAG_STR_SW_GPHY_RST         "Ethernet GPHY reset"
#define  DIAG_STR_SW_GLOBAL_RST      "Global reset"
#define  DIAG_STR_SW_RSG_RST            "RSGMII reset"
#define  DIAG_STR_SW_CFG_RST            "configuration reset"
#define  DIAG_STR_SW_Q_RST               "Queue/Packet Buffer/FIFO reset"
#define  DIAG_STR_SW_NIC_RST            "NIC interface reset"
#define  DIAG_STR_CPU_MEM_RST         "CPU and Memory controller reset"
#define  DIAG_STR_WDOG_NMI_EN        "Enable WDOG_RST trigger NMI_RST"
#define  DIAG_STR_PONMAC_RST          "PON MAC reset"
#define  DIAG_STR_BACKPRESSURE_JAM     "jam mode"
#define  DIAG_STR_BACKPRESSURE_DEFER  "defer mode"

#define DIAG_STR_SEC_SYN_RST_DENY           "SYN RST Deny"
#define DIAG_STR_SEC_SYN_FIN_DENY           "SYN FIN Deny"
#define DIAG_STR_SEC_XMA_DENY               "XMA Deny"
#define DIAG_STR_SEC_NULLSCAN_DENY          "NULLSCAN Deny"
#define DIAG_STR_SEC_SYN_SPORT_L1024_DENY   "SYN Sport Less 1024 Deny"
#define DIAG_STR_SEC_TCPHDR_MIN             "TCP Header Short"
#define DIAG_STR_SEC_SMURF_DENY             "SMURF Deny"
#define DIAG_STR_SEC_ICMPV6_PING_MAX        "ICMPV6 Ping Max"
#define DIAG_STR_SEC_ICMPV4_PING_MAX        "ICMPV4 Ping Max"
#define DIAG_STR_SEC_ICMP_FRAG_DENY         "ICMP Fragment Deny"
#define DIAG_STR_SEC_IPV6_MIN_FRAG_SIZE     "IPV6 Min Fragment Size Deny"
#define DIAG_STR_SEC_POD_DENY               "POD Deny"
#define DIAG_STR_SEC_TCP_BLAT_DENY          "TCP Blat Deny"
#define DIAG_STR_SEC_UDP_BLAT_DENY          "UDP Blat_Deny"
#define DIAG_STR_SEC_BLAT_DENY              "BLAT Deny"
#define DIAG_STR_SEC_LAND_DENY              "LAND deny"
#define DIAG_STR_SEC_DAEQSA_DENY            "DAEQSA Deny"
#define DIAG_STR_SEC_TCP_FRAG_OFF_MIN       "TCP Fragment Error"
#define DIAG_STR_SEC_UDPBOMB_DENY           "UDP Bomb Deny"
#define DIAG_STR_SEC_SYNWITHDATA_DENY       "SYN With Data Deny"
#define DIAG_STR_SEC_SYN_FLOOD_DENY         "SYN Flood Deny"
#define DIAG_STR_SEC_FIN_FLOOD_DENY         "FIN Flood Deny"
#define DIAG_STR_SEC_ICMP_FLOOD_DENY        "ICMP Flood Deny"

#define DIAG_STR_CPU_APOLLO_TAG_MODE        "APOLLO mode"
#define DIAG_STR_CPU_NORMAL_TAG_MODE        "Normal mode"

#define DIAG_STR_POLARITY_HIGH              "High"
#define DIAG_STR_POLARITY_LOW               "Low"


#define     DIAG_STR_PAR_LOOPBACK           "Loop Back"
#define     DIAG_STR_MUX_CPUONLY            "CPU Only"

#define     DIAG_STR_ACT_DROP               "Drop"
#define     DIAG_STR_ACT_PERMIT             "Permit"
#define     DIAG_STR_ACT_PERMIT_NO_PON      "Permit without PON"


#define     DIAG_STR_MIRROR_ALL_PKT         "All packets"
#define     DIAG_STR_MIRROR_MIR_PKT_ONLY    "Mirrored packets only"

#define     DIAG_STR_64BITS                 "64-bits"
#define     DIAG_STR_32BITS                 "32-bits"
#define     DIAG_STR_BYTE_COUNT             "Byte count"
#define     DIAG_STR_PACKET_COUNT           "Packet count"

#define     DIAG_STR_LEARNED_DISCARDS                       "dot1dTpLearnedEntryDiscards"
#define     DIAG_STR_IF_IN_OCTETS                           "ifInOctets"
#define     DIAG_STR_IF_IN_UCAST_PKTS                       "ifInUcastPkts"
#define     DIAG_STR_F_IN_MULTICAST_PKTS                    "ifInMulticastPkts"
#define     DIAG_STR_IF_IN_BROADCAST_PKTS                   "ifInBroadcastPkts"
#define     DIAG_STR_IF_IN_DISCARDS                         "ifInDiscards"
#define     DIAG_STR_IF_OUT_OCTETS                          "ifOutOctets"
#define     DIAG_STR_IF_OUT_DISCARDS                        "ifOutDiscards"
#define     DIAG_STR_IF_OUT_UCAST_PKTS_CNT                  "ifOutUcastPkts"
#define     DIAG_STR_IF_OUT_MULTICAST_PKTS_CNT              "ifOutMulticastPkts"
#define     DIAG_STR_IF_OUT_BROADCAST_PKTS_CNT              "ifOutBroadcastPkts"
#define     DIAG_STR_DOT1D_PORT_DELAY_EXCEEDED_DISCARDS     "dot1dPortDelayExceedDiscards"
#define     DIAG_STR_DOT1D_TP_PORT_IN_DISCARDS              "dot1dTpPortInDiscards"
#define     DIAG_STR_DOT1D_TP_HC_PORT_IN_DISCARDS           "dot1dTpHcPortInDiscards"
#define     DIAG_STR_DOT3_IN_PAUSE_FRAMES                   "dot3InPauseFrames"
#define     DIAG_STR_DOT3_OUT_PAUSE_FRAMES                  "dot3OutPauseFrames"
#define     DIAG_STR_DOT3_OUT_PAUSE_ON_FRAMES               "dot3OutPauseOnFrames"
#define     DIAG_STR_DOT3_STATS_ALIGNMENT_ERRORS            "dot3StatsAlignmentErrors"
#define     DIAG_STR_DOT3_STATS_FCS_ERRORS                  "dot3StatsFcsErrors"
#define     DIAG_STR_DOT3_STATS_SINGLE_COLLISION_FRAMES     "dot3StatsSingleCollisionFrames"
#define     DIAG_STR_DOT3_STATS_MULTIPLE_COLLISION_FRAMES   "dot3StatsMultipleCollisionFrames"
#define     DIAG_STR_DOT3_STATS_DEFERRED_TRANSMISSIONS      "dot3StatsDeferredTransmissions"
#define     DIAG_STR_DOT3_STATS_LATE_COLLISIONS             "dot3StatsLateCollisions"
#define     DIAG_STR_DOT3_STATS_EXCESSIVE_COLLISIONS        "dot3StatsExcessiveCollisions"
#define     DIAG_STR_DOT3_STATS_FRAME_TOO_LONGS             "dot3StatsFrameTooLongs"
#define     DIAG_STR_DOT3_STATS_SYMBOL_ERRORS               "dot3StatsSymbolErrors"
#define     DIAG_STR_DOT3_CONTROL_IN_UNKNOWN_OPCODES        "dot3ControlInUnknownOpcodes"
#define     DIAG_STR_ETHER_STATS_DROP_EVENTS                "etherStatsDropEvents"
#define     DIAG_STR_ETHER_STATS_OCTETS                     "etherStatsOctets"
#define     DIAG_STR_ETHER_STATS_BROADCAST_PKTS             "etherStatsBroadcastPkts"
#define     DIAG_STR_ETHER_STATS_MULTICAST_PKTS             "etherStatsMulticastPkts"
#define     DIAG_STR_ETHER_STATS_UNDER_SIZE_PKTS            "etherStatsUndersizePkts"
#define     DIAG_STR_ETHER_STATS_OVERSIZE_PKTS              "etherStatsOversizePkts"
#define     DIAG_STR_ETHER_STATS_FRAGMENTS                  "etherStatsFragments"
#define     DIAG_STR_ETHER_STATS_JABBERS                    "etherStatsJabbers"
#define     DIAG_STR_ETHER_STATS_COLLISIONS                 "etherStatsCollisions"
#define     DIAG_STR_ETHER_STATS_CRC_ALIGN_ERRORS           "etherStatsCRCAlignErrors"
#define     DIAG_STR_ETHER_STATS_PKTS_64OCTETS              "etherStatsPkts64Octets"
#define     DIAG_STR_ETHER_STATS_PKTS_65TO127OCTETS         "etherStatsPkts65to127Octets"
#define     DIAG_STR_ETHER_STATS_PKTS_128TO255OCTETS        "etherStatsPkts128to255Octets"
#define     DIAG_STR_ETHER_STATS_PKTS_256TO511OCTETS        "etherStatsPkts256to511Octets"
#define     DIAG_STR_ETHER_STATS_PKTS_512TO1023OCTETS       "etherStatsPkts512to1023Octets"
#define     DIAG_STR_ETHER_STATS_PKTS_1024TO1518OCTETS      "etherStatsPkts1024to1518Octets"
#define     DIAG_STR_ETHER_STATS_PKTS_1519TOMAXOCTETS       "etherStatsPkts1519toMaxOctets"
#define     DIAG_STR_ETHER_STATS_TX_OCTETS                  "etherStatsTxOctets"
#define     DIAG_STR_ETHER_STATS_TX_UNDER_SIZE_PKTS         "etherStatsTxUndersizePkts"
#define     DIAG_STR_ETHER_STATS_TX_OVERSIZE_PKTS           "etherStatsTxOversizePkts"
#define     DIAG_STR_ETHER_STATS_TX_PKTS_64OCTETS           "etherStatsTxPkts64Octets"
#define     DIAG_STR_ETHER_STATS_TX_PKTS_65TO127OCTETS      "etherStatsTxPkts65to127Octets"
#define     DIAG_STR_ETHER_STATS_TX_PKTS_128TO255OCTETS     "etherStatsTxPkts128to255Octets"
#define     DIAG_STR_ETHER_STATS_TX_PKTS_256TO511OCTETS     "etherStatsTxPkts256to511Octets"
#define     DIAG_STR_ETHER_STATS_TX_PKTS_512TO1023OCTETS    "etherStatsTxPkts512to1023Octets"
#define     DIAG_STR_ETHER_STATS_TX_PKTS_1024TO1518OCTETS   "etherStatsTxPkts1024to1518Octets"
#define     DIAG_STR_ETHER_STATS_TX_PKTS_1519TOMAXOCTETS    "etherStatsTxPkts1519toMaxOctets"
#define     DIAG_STR_ETHER_STATS_TX_BROADCAST_PKTS          "etherStatsTxBroadcastPkts"
#define     DIAG_STR_ETHER_STATS_TX_MULTICAST_PKTS          "etherStatsTxMulticastPkts"
#define     DIAG_STR_ETHER_STATS_TX_FRAGMENTS               "etherStatsTxFragments"
#define     DIAG_STR_ETHER_STATS_TX_JABBERS                 "etherStatsTxJabbers"
#define     DIAG_STR_ETHER_STATS_TX_CRC_ALIGN_ERRORS        "etherStatsTxCRCAlignErrors"
#define     DIAG_STR_ETHER_STATS_RX_UNDER_SIZE_PKTS         "etherStatsRxUndersizePkts"
#define     DIAG_STR_ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS    "etherStatsRxUndersizeDropPkts"
#define     DIAG_STR_ETHER_STATS_RX_OVERSIZE_PKTS           "etherStatsRxOversizePkts"
#define     DIAG_STR_ETHER_STATS_RX_PKTS_64OCTETS           "etherStatsRxPkts64Octets"
#define     DIAG_STR_ETHER_STATS_RX_PKTS_65TO127OCTETS      "etherStatsRxPkts65to127Octets"
#define     DIAG_STR_ETHER_STATS_RX_PKTS_128TO255OCTETS     "etherStatsRxPkts128to255Octets"
#define     DIAG_STR_ETHER_STATS_RX_PKTS_256TO511OCTETS     "etherStatsRxPkts256to511Octets"
#define     DIAG_STR_ETHER_STATS_RX_PKTS_512TO1023OCTETS    "etherStatsRxPkts512to1023Octets"
#define     DIAG_STR_ETHER_STATS_RX_PKTS_1024TO1518OCTETS   "etherStatsRxPkts1024to1518Octets"
#define     DIAG_STR_ETHER_STATS_RX_PKTS_1519TOMAXOCTETS    "etherStatsRxPkts1519toMaxOctets"
#define     DIAG_STR_IN_OAM_PDU_PKTS                        "inOamPduPkts"
#define     DIAG_STR_OUT_OAM_PDU_PKTS                       "outOamPduPkts"
#define     DIAG_STR_IF_IN_ERR_OCTETS_INDEX                 "ifInErrOctets"
#define     DIAG_STR_IF_OUT_ERR_OCTETS_INDEX                "ifOutErrOctets"
#define     DIAG_STR_IF_ERRORS_INDEX                        "ifInErrors"

#define     DIAG_STR_COUNT_MODE_FREE        "normal free run"
#define     DIAG_STR_COUNT_MODE_TIMER       "control by MIB timer"

#define     DIAG_STR_TAG_CNT_DIR_TX         "tx-counter tag length"
#define     DIAG_STR_TAG_CNT_DIR_RX         "rx-counter tag length"

#define     DIAG_STR_EXCLUDE                "exclude"
#define     DIAG_STR_INCLUDE                "include"

#define     DIAG_STR_RST_TO_0               "all 0"
#define     DIAG_STR_RST_TO_1               "all 1"

#define     DIAG_STR_FREE_SYNC              "normal free run sync"
#define     DIAG_STR_STOP_SYNC              "stop sync"

#define     DIAG_STR_SYNFIN_DENY                "SYN FIN Deny"
#define     DIAG_STR_XMA_DENY                   "XMA Deny"
#define     DIAG_STR_NULLSCAN_DENY              "NULLSCAN Deny"
#define     DIAG_STR_SYN_SPORTL1024_DENY        "SYN Sport Less 1024 Deny"
#define     DIAG_STR_TCPHDR_MIN_CHECK           "TCP Header Short"
#define     DIAG_STR_ICMP_FRAG_PKTS_DENY        "ICMP Fragment Deny"
#define     DIAG_STR_POD_DENY                   "POD Deny"
#define     DIAG_STR_BLAT_DENY                  "Blat_Deny"
#define     DIAG_STR_LAND_DENY                  "LAND deny"
#define     DIAG_STR_DAEQSA_DENY                "DAEQSA Deny"
#define     DIAG_STR_TCP_FRAG_OFF_MIN_CHECK     "TCP Fragment Error"
#define     DIAG_STR_UDPDOMB_DENY               "UDP Domb Deny"
#define     DIAG_STR_SYNWITHDATA_DENY           "SYN With Data Deny"
#define     DIAG_STR_SYNFLOOD_DENY              "SYN Flood Deny"
#define     DIAG_STR_FINFLOOD_DENY              "FIN Flood Deny"
#define     DIAG_STR_ICMPFLOOD_DENY             "ICMP Flood Deny"
#define     DIAG_STR_SYNFLOOD_DENY_THRESHOLD    "SYN flood thershold"
#define     DIAG_STR_FINFLOOD_DENY_THRESHOLD    "FIN flood thershold"
#define     DIAG_STR_ICMPFLOOD_DENY_THRESHOLD   "ICMP flood thershold"
#define     DIAG_STR_AUTO                       "Auto"
#define     DIAG_STR_MASTER                     "Force Master"
#define     DIAG_STR_SLAVE                      "Force Slave"

#define     DIAG_STR_PONMODE_GPON               "GPON"
#define     DIAG_STR_PONMODE_EPON               "EPON"
#define     DIAG_STR_PONMODE_FIBER1G            "Fiber AN 1G"
#define     DIAG_STR_PONMODE_FIBER100M          "Fiber AN 100M"
#define     DIAG_STR_PONMODE_FIBERAUTO          "Fiber auto detect"
#define     DIAG_STR_PONMODE_FIBERFORCE1G       "Force fiber 1G"
#define     DIAG_STR_PONMODE_FIBERFORCE100M     "Force fiber 100M"

#define     DIAG_LOOKUP_MISS_IPMC           "Lookup-miss IP Multicast"
#define     DIAG_LOOKUP_MISS_UCAST          "Lookup-miss Unicast"
#define     DIAG_LOOKUP_MISS_BCAST          "Lookup-miss Broadcast"
#define     DIAG_LOOKUP_MISS_MCAST          "Lookup-miss Multicast"
#define     DIAG_LOOKUP_MISS_IP6MC          "Lookup-miss IPv6 Multicast"

#define     DIAG_STR_GPON_FSM_UNKNOW        "Unknown"
#define     DIAG_STR_GPON_FSM_O1            "Initial State(O1)"
#define     DIAG_STR_GPON_FSM_O2            "Standby State(O2)"
#define     DIAG_STR_GPON_FSM_O3            "Serial Number State(O3)"
#define     DIAG_STR_GPON_FSM_O4            "Ranging State(O4)"
#define     DIAG_STR_GPON_FSM_O5            "Operation State(O5)"
#define     DIAG_STR_GPON_FSM_O6            "POPUP State(O6)"
#define     DIAG_STR_GPON_FSM_O7            "Emergency Stop State(O7)"

#define     DIAG_STR_NGP2_ONU_STATE_O1_1    "Initial State:Off-Sync(O1.1)"
#define     DIAG_STR_NGP2_ONU_STATE_O1_2    "Initial State:Profile Leaning(O1.2)"
#define     DIAG_STR_NGP2_ONU_STATE_O2_3    "Serial Number State(O2-3)"
#define     DIAG_STR_NGP2_ONU_STATE_O4      "Ranging State(O4)"
#define     DIAG_STR_NGP2_ONU_STATE_O5_1    "Operation State:Associated(O5.1)"
#define     DIAG_STR_NGP2_ONU_STATE_O5_2    "Operation State:Pending(O5.2)"
#define     DIAG_STR_NGP2_ONU_STATE_O6      "Intermittent LODS State(6)"
#define     DIAG_STR_NGP2_ONU_STATE_O7      "Emergency Stop State(O7)"
#define     DIAG_STR_NGP2_ONU_STATE_O8_1    "DS Tuning State:Off-Sync(O8.1)"
#define     DIAG_STR_NGP2_ONU_STATE_O8_2    "DS Tuning State:Profile Leaning(O8.2)"
#define     DIAG_STR_NGP2_ONU_STATE_O9      "US Tuning State(O9)"

#define     DIAG_STR_GPON_FLOW_OMCI         "OMCI"
#define     DIAG_STR_GPON_FLOW_ETH          "ETH"
#define     DIAG_STR_GPON_FLOW_TDM          "TDM"
#define     DIAG_STR_GPON_FORCE_NOP         "Normal"
#define     DIAG_STR_GPON_FORCE_PASS        "Force pass"
#define     DIAG_STR_GPON_FORCE_DROP        "Force drop"
#define     DIAG_STR_GPON_MC_INCLUDE        "Including"
#define     DIAG_STR_GPON_MC_EXCLUDE        "Excluding"
#define     DIAG_STR_GPON_ALARM_NONE        "None"
#define     DIAG_STR_GPON_ALARM_LOS         "LOS"
#define     DIAG_STR_GPON_ALARM_LOF         "LOF"
#define     DIAG_STR_GPON_ALARM_LOM         "LOM"
#define     DIAG_STR_GPON_ALARM_SF          "SF"
#define     DIAG_STR_GPON_ALARM_SD          "SD"
#define     DIAG_STR_GPON_ALARM_TX_MISMH    "TX Mismatch"
#define     DIAG_STR_GPON_ALARM_TX_TOOLN    "TX Too Long"

#define     DIAG_STR_GPON_LASER_NORMAL      "Normal"
#define     DIAG_STR_GPON_LASER_FS_ON       "Force on"
#define     DIAG_STR_GPON_LASER_FS_OFF      "Force off"

#define     DIAG_STR_8BITW                  "8-bit width"
#define     DIAG_STR_16BITW                 "16-bit width"
#define     DIAG_STR_24BITW                 "24-bit width"
#define     DIAG_STR_32BITW                 "32-bit width"

#define     DIAG_STR_MODE0                  "mode0"
#define     DIAG_STR_MODE1                  "mode1"
#define     DIAG_STR_MODE2                  "mode2"
#define     DIAG_STR_MODE3                  "mode3"

#define     DIAG_STR_RMA00       "01-80-C2-00-00-00"
#define     DIAG_STR_RMA01       "01-80-C2-00-00-01"
#define     DIAG_STR_RMA02       "01-80-C2-00-00-02"
#define     DIAG_STR_RMA03       "01-80-C2-00-00-03"
#define     DIAG_STR_RMA04       "01-80-C2-00-00-04"
#define     DIAG_STR_RMA05       "01-80-C2-00-00-05"
#define     DIAG_STR_RMA06       "01-80-C2-00-00-06"
#define     DIAG_STR_RMA07       "01-80-C2-00-00-07"
#define     DIAG_STR_RMA08       "01-80-C2-00-00-08"
#define     DIAG_STR_RMA09       "01-80-C2-00-00-09"
#define     DIAG_STR_RMA0A       "01-80-C2-00-00-0A"
#define     DIAG_STR_RMA0B       "01-80-C2-00-00-0B"
#define     DIAG_STR_RMA0C       "01-80-C2-00-00-0C"
#define     DIAG_STR_RMA0D       "01-80-C2-00-00-0D"
#define     DIAG_STR_RMA0E       "01-80-C2-00-00-0E"
#define     DIAG_STR_RMA0F       "01-80-C2-00-00-0F"
#define     DIAG_STR_RMA10       "01-80-C2-00-00-10"
#define     DIAG_STR_RMA11       "01-80-C2-00-00-11"
#define     DIAG_STR_RMA12       "01-80-C2-00-00-12"
#define     DIAG_STR_RMA13       "01-80-C2-00-00-13"
#define     DIAG_STR_RMA14       "01-80-C2-00-00-14"
#define     DIAG_STR_RMA15       "01-80-C2-00-00-15"
#define     DIAG_STR_RMA16       "01-80-C2-00-00-16"
#define     DIAG_STR_RMA17       "01-80-C2-00-00-17"
#define     DIAG_STR_RMA18       "01-80-C2-00-00-18"
#define     DIAG_STR_RMA19       "01-80-C2-00-00-19"
#define     DIAG_STR_RMA1A       "01-80-C2-00-00-1A"
#define     DIAG_STR_RMA1B       "01-80-C2-00-00-1B"
#define     DIAG_STR_RMA1C       "01-80-C2-00-00-1C"
#define     DIAG_STR_RMA1D       "01-80-C2-00-00-1D"
#define     DIAG_STR_RMA1E       "01-80-C2-00-00-1E"
#define     DIAG_STR_RMA1F       "01-80-C2-00-00-1F"
#define     DIAG_STR_RMA20       "01-80-C2-00-00-20"
#define     DIAG_STR_RMA21       "01-80-C2-00-00-21"
#define     DIAG_STR_RMA22       "01-80-C2-00-00-22"
#define     DIAG_STR_RMA23       "01-80-C2-00-00-23"
#define     DIAG_STR_RMA24       "01-80-C2-00-00-24"
#define     DIAG_STR_RMA25       "01-80-C2-00-00-25"
#define     DIAG_STR_RMA26       "01-80-C2-00-00-26"
#define     DIAG_STR_RMA27       "01-80-C2-00-00-27"
#define     DIAG_STR_RMA28       "01-80-C2-00-00-28"
#define     DIAG_STR_RMA29       "01-80-C2-00-00-29"
#define     DIAG_STR_RMA2A       "01-80-C2-00-00-2A"
#define     DIAG_STR_RMA2B       "01-80-C2-00-00-2B"
#define     DIAG_STR_RMA2C       "01-80-C2-00-00-2C"
#define     DIAG_STR_RMA2D       "01-80-C2-00-00-2D"
#define     DIAG_STR_RMA2E       "01-80-C2-00-00-2E"
#define     DIAG_STR_RMA2F       "01-80-C2-00-00-2F"
#define     DIAG_STR_IGMP        "IGMP/MLD"
#define     DIAG_STR_IPMC        "IP Multicast"
#define     DIAG_STR_CDP         "Cisco Discovery Protocol"
#define     DIAG_STR_SSTP        "Shared Spanning Tree Protocol"
#define     DIAG_STR_ICMP        "ICMP"


#define     DIAG_STR_TIME_INCREASE          "Increase"
#define     DIAG_STR_TIME_DECREASE          "Decrease"
#define     DIAG_STR_TIME_TYPE_SYNC                  "sync"
#define     DIAG_STR_TIME_TYPE_DELAY_REQ             "delay-req"
#define     DIAG_STR_TIME_TYPE_PDELAY_REQ            "pdelay-req"
#define     DIAG_STR_TIME_TYPE_PDELAY_RESP           "pdelay-resp"
#define     DIAG_STR_TIME_TYPE_FOLLOW_UP             "follow-up"
#define     DIAG_STR_TIME_TYPE_DELAY_RESP            "delay-resp"
#define     DIAG_STR_TIME_TYPE_PDELAY_RESP_FOLLOW_UP "pdelay-resp-follow-up"
#define     DIAG_STR_TIME_TYPE_ANNOUNCE_SIGNALING    "announce-signaling"
#define     DIAG_STR_TIME_TYPE_SYNC_ONE_STEP         "sync-one-step"
#define     DIAG_STR_TIME_TYPE_PDELAY_REQ_ONE_STEP   "pdelay-req-one-step"
#define     DIAG_STR_TIME_IGR_ACT_NOP                               "No operation"
#define     DIAG_STR_TIME_IGR_ACT_TRAP_TO_CPU                       "Trap to CPU"
#define     DIAG_STR_TIME_IGR_ACT_FORWARD_TO_TRANS                  "Forward to transparent port"
#define     DIAG_STR_TIME_IGR_ACT_FORWARD_TO_TRANS_AND_MIRROR2CPU   "Forward to transparent port and Rx mirror to CPU"
#define     DIAG_STR_TIME_EGR_ACT_NOP                       "No operation"
#define     DIAG_STR_TIME_EGR_ACT_LATCH_TIME                "Latch egress timestamp"
#define     DIAG_STR_TIME_EGR_ACT_LATCH_TIME_AND_MIRROR2CPU "Latch egress timestamp and Tx mirror to CPU"
#define     DIAG_STR_TIME_EGR_ACT_MODIFY_CORRECTION         "Modify correctionField"
#define     DIAG_STR_TIME_PPS_MODE_PON         "PON"
#define     DIAG_STR_TIME_PPS_MODE_PTP         "PTP"


#define     DIAG_STR_PHY_TEST_MODE_NORMAL   "Normal"
#define     DIAG_STR_PHY_TEST_MODE_1        "Test Mode 1"
#define     DIAG_STR_PHY_TEST_MODE_2        "Test Mode 2"
#define     DIAG_STR_PHY_TEST_MODE_3        "Test Mode 3"
#define     DIAG_STR_PHY_TEST_MODE_4        "Test Mode 4"


#define     DIAG_STR_INGRESS_CVID            "Ingress CVID"
#define     DIAG_STR_INGRESS_OUTERTAG_VID    "Ingress Outer Tag VID"
#define     DIAG_STR_INTERNAL_CVID           "Internal CVID"
#define     DIAG_STR_INGRESS_CPRI            "Ingress CPRI"
#define     DIAG_STR_REMARK_CPRI             "Remark CPRI"

#define     DIAG_STR_TRAP_EXTHEADER_OVER_0      "Trap when ext header > 0"
#define     DIAG_STR_TRAP_EXTHEADER_OVER_240    "Trap when ext header > 240"

#define     DIAG_STR_HSB_DA               "HSB DA"
#define     DIAG_STR_L34_HSA_NEXTHOP      "L34 HSA nexthop"

#define     DIAG_PPPOE_ACT_NO             "No Act"
#define     DIAG_PPPOE_ACT_KEEP           "Keep/Add"
#define     DIAG_PPPOE_ACT_REMOVE         "Remove"
#define     DIAG_PPPOE_ACT_MODIFY         "Modify/Add"

#define     DIAG_STR_FLOW_OMCI  "OMCI"
#define     DIAG_STR_FLOW_ETH   "ETH"
#define     DIAG_STR_FLOW_TDM   "TDM"

#define     DIAG_STR_DSQUEUE_NORMAL0        "normal0"
#define     DIAG_STR_DSQUEUE_NORMAL1        "normal1"
#define     DIAG_STR_DSQUEUE_HIGH           "high"

#define     DIAG_STR_TRAP_HASH_SPA      "Source Port"
#define     DIAG_STR_TRAP_HASH_SMAC     "Source MAC"
#define     DIAG_STR_TRAP_HASH_DMAC     "Destination MAC"
#define     DIAG_STR_TRAP_HASH_SIP      "Source IP"
#define     DIAG_STR_TRAP_HASH_DIP      "Destination IP"
#define     DIAG_STR_TRAP_HASH_SPORT    "Source TCP/UDP Port"
#define     DIAG_STR_TRAP_HASH_DPORT    "Destination TCP/UDP Port"

/*
 * Data Declaration
 */
extern const char *diagStr_enable[];
extern const char *diagStr_valid[];
extern const char *diagStr_svlanAct[];
extern const char *diagStr_svlanAction[];
extern const char *diagStr_svlanSpriSrc[];
extern const char *diagStr_svlanFmtStr[];
extern const char *diagStr_actionStr[];
extern const char *diagStr_igmpTypeStr[];
extern const char *diagStr_aclRangeCheckLenTypeStr[];
extern const char *diagStr_aclRangeCheckPortTypeStr[];
extern const char *diagStr_aclRangeCheckIpTypeStr[];
extern const char *diagStr_aclRangeCheckVidTypeStr[];
extern const char *diagStr_aclModeStr[];

extern const char *diagStr_aclActCvlanStr[];
extern const char *diagStr_aclActSvlanStr[];
extern const char *diagStr_aclActPoliceStr[];
extern const char *diagStr_aclActFwdStr[];
extern const char *diagStr_aclActPriStr[];
extern const char *diagStr_aclActCfStr[];
extern const char *diagStr_aclActPlStr[];
extern const char *diagStr_l2IpMcHashOpStr[];
extern const char *diagStr_l2IpMcHashMethodStr[];
extern const char *diagStr_l2IpMcVlanHashMethodStr[];
extern const char *diagStr_l2LutStaticOrAutoStr[];
extern const char *diagStr_enDisplay[];
extern const char *diagStr_l2HashMethodStr[];

extern const char *diagStr_l34NexthopTypeStr[];
extern const char *diagStr_aclOper[];
extern const char *diagStr_vlanTagType[];
extern const char *diagStr_trunkMode[];
extern const char *diagStr_trunkAlgorithm[];
extern const char *diagStr_trunkFloodMode[];

extern const char *diagStr_direction[];
extern const char *diagStr_usCStagAction[];
extern const char *diagStr_usCtagAction[];
extern const char *diagStr_usVidAction[];
extern const char *diagStr_usPriAction[];
extern const char *diagStr_usSidAction[];
extern const char *diagStr_dsCStagAction[];
extern const char *diagStr_dsCtagAction[];
extern const char *diagStr_dsVidAction[];
extern const char *diagStr_dsPriAction[];
extern const char *diagStr_dsUniAction[];
extern const char *diagStr_usFwdAction[];
extern const char *diagStr_cfpriAction[];
extern const char *diagStr_dscpRemarkAction[];
extern const char *diagStr_cfRangeCheckIpTypeStr[];
extern const char *diagStr_cfRangeCheckPortTypeStr[];
extern const char *diagStr_cfTemplatePtn0VidCfg[];
extern const char *diagStr_cfTemplatePtn0PriCfg[];
extern const char *diagStr_dsUniPattern[];

extern const char *diagStr_frameType[];
extern const char *diagStr_svlanLookup[];
extern const char *diagStr_stormType[];
extern const char *diagStr_stormAltType[];
extern const char *diagStr_authstate[];
extern const char *diagStr_unAuthAct[];
extern const char *diagStr_1xOpDir[];
extern const char *diagStr_ipgCompensation[];

extern const char *diagStr_AfbMonCount[];
extern const char *diagStr_AfbErrCount[];
extern const char *diagStr_AfbRestorePL[];
extern const char *diagStr_AfbvalidFlow[];

extern const char *diagStr_queueType[];
extern const char *diagStr_portSpeed[];
extern const char *diagStr_portDuplex[];
extern const char *diagStr_portLinkStatus[];
extern const char *diagStr_portNwayFault[];

extern const char *diagStr_selectorMode[];

extern const char *diagStr_flowCtrlType[];
extern const char *diagStr_flowCtrlJumboSize[];
extern const char *diagStr_chipReset[];
extern const char *diagStr_backPressure[];

extern const char *diagStr_cpuTagFormat[];

extern const char *diagStr_polarity[];

extern const char *diagStr_oamParserAct[];
extern const char *diagStr_oamMuxAct[];
extern const char *diagStr_mirrorEgressMode[];

extern const char *diagStr_cfUnmatchAct[];
extern const char *diagStr_cfUnmatchActDs[];
extern const char *diagStr_l2flushMode[];
extern const char *diagStr_l2unknownMcAct[];
extern const char *diagStr_l2rsvdAddr[];
extern const char *diagStr_logMibMode[];
extern const char *diagStr_logMibType[];

extern const char *diagStr_globalMibCnt[];
extern const char *diagStr_mibName[];
extern const char *diagStr_mibLogTimer[];
extern const char *diagStr_mibGetTagLenDir[];
extern const char *diagStr_mibGetTagLenState[];
extern const char *diagStr_mibRstValue[];
extern const char *diagStr_mibSyncMode[];

extern const char *diagStr_secGetTypeName[];
extern const char *diagStr_secThresholdName[];
extern const char *diagStr_masterSlave[];

extern const char *diagStr_ponModeName[];

extern const char *diagStr_lookupmissType[];

extern const char *diagStr_gponFsmStatus[];
#if defined(CONFIG_COMMON_RT_API)
extern const char *diagStr_rtGponFsmStatus[];
#endif
extern const char *diagStr_ngp2OnuState[];
extern const char *diagStr_gponFlowType[];
extern const char *diagStr_gponMcForwardMode[];
extern const char *diagStr_gponMcMode[];
extern const char *diagStr_gponAlarmType[];
extern const char *diagStr_gponLaserMode[];
extern const char *diagStr_filterState[];
extern const char *diagStr_ifgState[];

extern const char *diagStr_i2cWidth[];
extern const char *diagStr_i2cEepMirrorAddrExtMode[];
extern const char *diagStr_vlanLeakyType[];
extern const char *diagStr_vlanKeepType[];

extern const char *diagStr_timeSign[];
extern const char *diagStr_timeType[];
extern const char *diagStr_timeIgrAct[];
extern const char *diagStr_timeEgrAct[];
extern const char *diagStr_timePpsMode[];

extern const char *diagStr_svlanSp2cUnmatch[];
extern const char *diagStr_testMode[];
extern const char *diagStr_txRrcType[];
extern const char *diagStr_qos1premarkSource[];

extern const char *diagStr_tcAssignOption[];
extern const char *diagStr_dsliteUnmatchAct[];
extern const char *diagStr_dsliteNhAct[];
extern const char *diagStr_dsliteFragAct[];
extern const char *diagStr_ipmcPppoeAct[];
extern const char *diagStr_routeType[];
extern const char *diagStr_l34HsbType[];

extern const char *diagStr_flowType[];
extern const char *diagStr_dsQueueType[];

extern const char *diagStr_trapHashType[];

#endif /* end of __DIAG_STR_H__ */

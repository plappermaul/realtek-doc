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
 * Purpose : Definition of TRAP API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration for traping packet to CPU
 *           (2) RMA
 *           (3) User defined RMA
 *           (4) System-wise management frame
 *           (5) System-wise user defined management frame
 *           (6) Per port user defined management frame
 *           (7) Packet with special flag or option
 *           (8) CFM and OAM packet
 *
 */

#ifndef __RTK_TRAP_H__
#define __RTK_TRAP_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/* RMA action */
typedef enum rtk_trap_rma_cmpType_e
{
    RMA_CMP_TYPE_MAC = 0,
    RMA_CMP_TYPE_ETHERTYPE,
    RMA_CMP_TYPE_MAC_ETHERTYPE,
    RMA_CMP_TYPE_END
}rtk_trap_rma_cmpType_t;

typedef enum rtk_mgmt_action_e
{
    MGMT_ACTION_FORWARD = 0,
    MGMT_ACTION_DROP,
    MGMT_ACTION_TRAP2CPU,
    MGMT_ACTION_COPY2CPU,
    MGMT_ACTION_TRAP2MASTERCPU,
    MGMT_ACTION_COPY2MASTERCPU,
    MGMT_ACTION_FLOOD_TO_ALL_PORT,
    MGMT_ACTION_END
} rtk_mgmt_action_t;

typedef enum rtk_trap_oam_action_e
{
    TRAP_OAM_ACTION_FORWARD     = ACTION_FORWARD,
    TRAP_OAM_ACTION_DROP        = ACTION_DROP,
    TRAP_OAM_ACTION_TRAP2CPU    = ACTION_TRAP2CPU,
    TRAP_OAM_ACTION_LOOPBACK    = ACTION_LOOPBACK,
    TRAP_OAM_ACTION_LINK_FAULT_DETECT = ACTION_LINK_FAULT_DETECT,
    TRAP_OAM_ACTION_END,
} rtk_trap_oam_action_t;

typedef enum rtk_trap_reason_type_e
{
    TRAP_REASON_RMA = 0,
    TRAP_REASON_IPV4IGMP,
    TRAP_REASON_IPV6MLD,
    TRAP_REASON_1XEAPOL,
    TRAP_REASON_VLANERR,
    TRAP_REASON_SLPCHANGE,
    TRAP_REASON_MULTICASTDLF,
    TRAP_REASON_CFI,
    TRAP_REASON_1XUNAUTH,
    TRAP_REASON_END,
} rtk_trap_reason_type_t;

typedef enum rtk_trap_type_e
{
    TRAP_TYPE_1XMAC_PORTCHG = 0,
    TRAP_TYPE_IPV4_IGMP,
    TRAP_TYPE_IPMC_DLF,
    TRAP_TYPE_L2MC_DLF,
    TRAP_TYPE_IPV6_MLD,
    TRAP_TYPE_CFI_1,
    TRAP_TYPE_CONTROL_END
} rtk_trap_type_t;

typedef  enum rtk_trap_rmaGroup_frameType_e
{
    RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM = 0,
    RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER,
    RMA_GROUP_TYPE_03,
    RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP,
    RMA_GROUP_TYPE_10,
    RMA_GROUP_TYPE_GMRP,
    RMA_GROUP_TYPE_GVRP,
    RMA_GROUP_TYPE_MSRP,
    RMA_GROUP_TYPE_0X,
    RMA_GROUP_TYPE_1X,
    RMA_GROUP_TYPE_2X,
    RMA_GROUP_TYPE_END
} rtk_trap_rmaGroup_frameType_t;  /* Applicable to 8380 */

/* Type of management packet */
typedef enum rtk_trap_mgmtType_e
{
    MGMT_TYPE_ARP = 0,          /* Applicable to 8380, 8390 */
    MGMT_TYPE_ARP_REQ,          /* Applicable to none */
    MGMT_TYPE_ARP_REP,          /* Applicable to none */
    MGMT_TYPE_DHCP,             /* Applicable to none */
    MGMT_TYPE_DHCP6,            /* Applicable to none */
    MGMT_GRATUITOUS_ARP,        /* Applicable to 9300, 9310 */
    MGMT_TYPE_MLD,
    MGMT_TYPE_IGMP,
    MGMT_TYPE_BPDU,
    MGMT_TYPE_PTP,              /* Applicable to 8380, 8390, 9300, 9310 */
    MGMT_TYPE_PTP_UDP,          /* Applicable to 9300, 9310 */
    MGMT_TYPE_PTP_ETH2,         /* Applicable to 9300, 9310 */
    MGMT_TYPE_LLDP,             /* Applicable to 8380, 8390, 9300, 9310 */
    MGMT_TYPE_EAPOL,            /* Applicable to 8380, 8390, 9300, 9310 */
    MGMT_TYPE_IPV6ND,           /* Applicable to 8380, 8390 */
    MGMT_TYPE_SELFMAC,          /* Applicable to 8380, 8390 */
    MGMT_TYPE_OTHER,            /* Applicable to 8380, 8390 */
    MGMT_TYPE_OAM,              /* Applicable to 8390 */
    MGMT_TYPE_CFM,              /* Applicable to 8390 */
    MGMT_TYPE_IGR_VLAN_FLTR,    /* Applicable to 8380, 8390 */
    MGMT_TYPE_VLAN_ERR,         /* Applicable to 8390 */
    MGMT_TYPE_CFI,              /* Applicable to 8380, 8390 */
    MGMT_TYPE_RMA_USR_DEF_1,    /* Applicable to 8390 */
    MGMT_TYPE_RMA_USR_DEF_2,    /* Applicable to 8390 */
    MGMT_TYPE_LACP,             /* Applicable to 8390 */
    MGMT_TYPE_UNKNOWN_DA,       /* Applicable to 8380, 8390 */
    MGMT_TYPE_RLDP_RLPP,        /* Applicable to 8380, 9300*/
    MGMT_TYPE_RMA,              /* Applicable to 8380, 8390 */
    MGMT_TYPE_SPECIAL_COPY,     /* Applicable to 8380 */
    MGMT_TYPE_SPECIAL_TRAP,     /* Applicable to 8380 */
    MGMT_TYPE_ROUT_EXCEPT,      /* Applicable to 8380 */
    MGMT_TYPE_MAC_CST_SYS,      /* Applicable to 8380 */
    MGMT_TYPE_MAC_CST_PORT,     /* Applicable to 8380 */
    MGMT_TYPE_MAC_CST_VLAN,     /* Applicable to 8380 */
    MGMT_TYPE_IPV6_HOP_POS_ERR, /* Applicable to 8380, 8390 */
    MGMT_TYPE_IPV6_HDR_UNKWN,   /* Applicable to 8380, 8390 */
    MGMT_TYPE_L2_CRC_ERR,
    MGMT_TYPE_IP4_CHKSUM_ERR,   /* Applicable to 8390, 9300 */
    MGMT_TYPE_INVALID_SA,       /* Applicable to 8390 */
    MGMT_TYPE_END
} rtk_trap_mgmtType_t;

/* Queue Type of trapped packet */
typedef enum rtk_trap_qType_e
{
    TRAP_Q_ARP_REQ_REP_GRTS,     /* Applicable to 9300, 9310 */
    TRAP_Q_DHCP_DHCP6,           /* Applicable to 9300, 9310 */
    TRAP_Q_IGMP_MLD ,            /* Applicable to 9300, 9310 */
    TRAP_Q_BPDU,                 /* Applicable to 9300, 9310 */
    TRAP_Q_PTP,                  /* Applicable to 9300, 9310*/
    TRAP_Q_LLDP,                 /* Applicable to 9300, 9310 */
    TRAP_Q_EAPOL,                /* Applicable to 9300, 9310 */
    TRAP_Q_OAM,                  /* Applicable to 9300, 9310 */
    TRAP_Q_LACP,                 /* Applicable to 9300, 9310*/
    TRAP_Q_RLDP_RLPP,            /* Applicable to 9300*/
    TRAP_Q_CFM,                  /* Applicable to 9310 */
    TRAP_Q_CFM_ETHDM,            /* Applicable to 9310 */
    TRAP_Q_RMA_USR_DEF,          /* Applicable to 9300, 9310 */
    TRAP_Q_RMA,                  /* Applicable to 9300, 9310*/
    TRAP_Q_IPV6ND,               /* Applicable to 9300, 9310 */
    TRAP_Q_IP4_IP6_HDR_ERR,      /* Applicable to 9300, 9310 */
    TRAP_Q_L2_CRC_ERR,           /* Applicable to 9300, 9310*/
    TRAP_Q_IP4_CHKSUM_ERR,       /* Applicable to 9300, 9310*/
    TRAP_Q_IP4_IP6_RSVD_ADDR,    /* Applicable to 9300, 9310*/
    TRAP_Q_IGR_VLAN_FLTR,        /* Applicable to 9300, 9310 */
    TRAP_Q_CFI,                  /* Applicable to 9300, 9310 */
    TRAP_Q_IVC,                  /* Applicable to 9300, 9310*/ // naing for ip/mac vlan ??
    TRAP_Q_INVALID_SA,           /* Applicable to 9300, 9310*/
    TRAP_Q_MAC_CST,              /* Applicable to 9300, 9310*/
    TRAP_Q_NEW_SA,               /* Applicable to 9300, 9310*/
    TRAP_Q_PMV_FORBID,           /* Applicable to 9300, 9310*/
    TRAP_Q_L2_STTC_PMV,          /* Applicable to 9300, 9310*/
    TRAP_Q_L2_DYN_PMV,           /* Applicable to 9300, 9310*/
    TRAP_Q_HASH_FULL,            /* Applicable to 9300, 9310*/
    TRAP_Q_ATK,                  /* Applicable to 9300, 9310*/
    TRAP_Q_ACL_HIT,              /* Applicable to 9300, 9310*/
    TRAP_Q_MIR_HIT,              /* Applicable to 9300, 9310*/
    TRAP_Q_CAPWAP,               /* Applicable to 9300, 9310*/
    TRAP_Q_MPLS_EXCPT,           /* Applicable to 9310 */
    TRAP_Q_OF_HIT,               /* Applicable to 9310 */
    TRAP_Q_OF_TBL_LUMIS,         /* Applicable to 9310 */
    TRAP_Q_TT_HIT,               /* Applicable to 9310 */
    TRAP_Q_SFLOW,                /* Applicable to 9310 */
    TRAP_Q_PASR_EXCPT,           /* Applicable to 9310 */
    TRAP_Q_MALFORM_PKT,          /* Applicable to 9310 */
    TRAP_Q_CPU2CPU_TALK,         /* Applicable to 9300, 9310 */

    TRAP_Q_L3_IPUC_RPF,                   /* Applicable to 9300, 9310*/
    TRAP_Q_L3_IPMC_RPF,                   /* Applicable to 9300, 9310*/ /*(ASSERT) */
    TRAP_Q_L2_UC_MC_BDG_LUMIS,            /* Applicable to 9300, 9310*/
    TRAP_Q_IP4_IP6_BDG_LUMIS,             /* Applicable to 9300, 9310*/ //only ip6 ??
    TRAP_Q_ROUTER_MAC_IF,                 /* Applicable to 9300, 9310*/
    TRAP_Q_L3_IPUC_NON_IP_PKT,            /* Applicable to 9300, 9310*/
    TRAP_Q_ROUTE_IP_CHK,                  /* Applicable to 9300, 9310*/
    TRAP_Q_L3_ROUTE_DIP_DMAC_MISMATCH,    /* Applicable to 9300, 9310*/
    TRAP_Q_IP6_UC_MC_HOP_BY_HOP,          /* Applicable to 9300, 9310*/
    TRAP_Q_IP6_UC_MC_ROUTE_HDR,           /* Applicable to 9300, 9310*/
    TRAP_Q_IP4_IP_OPT,                    /* Applicable to 9300, 9310*/
    TRAP_Q_IP4_IP6_MC_ROUTE_LUMIS,        /* Applicable to 9300, 9310*/
    TRAP_Q_L3_IPUC_NULL_ROUTE,            /* Applicable to 9300, 9310*/
    TRAP_Q_L3_IPUC_PBR_NULL_ROUTE,        /* Applicable to 9300, 9310*/
    TRAP_Q_L3_UC_HOST_ROUTE,              /* Applicable to 9300, 9310*/
    TRAP_Q_L3_UC_NET_ROUTE,               /* Applicable to 9300, 9310*/
    TRAP_Q_L3_MC_BDG_ENTRY,               /* Applicable to 9300, 9310*/
    TRAP_Q_L3_MC_ROUTE_ENTRY,             /* Applicable to 9300, 9310*/
    TRAP_Q_TTL_EXCPT,                     /* Applicable to 9310 */
    TRAP_Q_ROUTE_EXCPT_NH_AGE_OUT_ACT,    /* Applicable to 9300, 9310*/
    TRAP_Q_IP4_IP6_ICMP_REDRT,            /* Applicable to 9300, 9310*/
    TRAP_Q_IPUC_MTU,                      /* Applicable to 9300, 9310*/
    TRAP_Q_IPMC_MTU,                      /* Applicable to 9300, 9310*/
    TRAP_Q_IPUC_TTL,                      /* Applicable to 9300, 9310*/
    TRAP_Q_IPMC_TTL,                      /* Applicable to 9300, 9310*/
    TRAP_Q_IP_MAC_BINDING,                /* Applicable to 9310 */
    TRAP_Q_TUNL_MAC_IF,                   /* Applicable to 9310 */
    TRAP_Q_TUNL_IP_CHK,                   /* Applicable to 9310 */
    TRAP_Q_ROUTE_EXCPT_NH_ERR_ACT,        /* Applicable to 9310 */
    TRAP_Q_ROUTE_EXCPT_ROUTE_TO_TUNL_ACT, /* Applicable to 9310 */
    TRAP_Q_NOTIFICATION,                  /* Applicable to 9300, 9310 */

    TRAP_Q_NORMAL_FWD,           /* Applicable to 9300, 9310 */

    TRAP_Q_END

} rtk_trap_qType_t;/* Applicable to 9300,9310 */

/* Type of management IP type */
typedef enum rtk_trap_mgmtIpType_e
{
    MGMT_IP_TYPE_IPV4 = 0,
    MGMT_IP_TYPE_IPV6,
    MGMT_IP_TYPE_END
} rtk_trap_mgmtIpType_t;

/* Type of bypassing STP frame type */
typedef enum rtk_trap_bypassStpType_e
{
    BYPASS_STP_TYPE_USER_DEF_0 = 0,
    BYPASS_STP_TYPE_USER_DEF_1,
    BYPASS_STP_TYPE_USER_DEF_2,     /* Applicable to 8380 */
    BYPASS_STP_TYPE_USER_DEF_3,     /* Applicable to 8380 */
    BYPASS_STP_TYPE_USER_DEF_4,     /* Applicable to 8380 */
    BYPASS_STP_TYPE_USER_DEF_5,     /* Applicable to 8380 */
    BYPASS_STP_TYPE_SLOW_PROTO,
    BYPASS_STP_TYPE_RMA_03,             /* Applicable to 8380 */
    BYPASS_STP_TYPE_RMA_0X,
    BYPASS_STP_TYPE_EAPOL,
    BYPASS_STP_TYPE_PTP,            /* Applicable to 8390,8380 */
    BYPASS_STP_TYPE_LLDP,           /* Applicable to 8390,8380 */
    BYPASS_STP_TYPE_END
} rtk_trap_bypassStpType_t;

/* Type of bypassing VLAN drop */
typedef enum rtk_trap_bypassVlanType_e
{
    BYPASS_VLAN_TYPE_USER_DEF_0 = 0,
    BYPASS_VLAN_TYPE_USER_DEF_1,
    BYPASS_VLAN_TYPE_USER_DEF_2,      /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_USER_DEF_3,      /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_USER_DEF_4,      /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_USER_DEF_5,      /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_RMA_00,
    BYPASS_VLAN_TYPE_RMA_02,
    BYPASS_VLAN_TYPE_RMA_03,            /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_RMA_0E,
    BYPASS_VLAN_TYPE_RMA_0X,
    BYPASS_VLAN_TYPE_OAM,             /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_GMRP,            /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_GVRP,            /* Applicable to 8380 */
    BYPASS_VLAN_TYPE_EAPOL,
    BYPASS_VLAN_TYPE_PTP,
    BYPASS_VLAN_TYPE_LLDP,
    BYPASS_VLAN_TYPE_END
} rtk_trap_bypassVlanType_t;

/* Type of route exception type */
typedef enum rtk_trap_routeExceptionType_e
{
    ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED,     /* IPv6 hop limit exceed packet */
    ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR,       /* IPv6 header error packet , Applicable to 8390*/
    ROUTE_EXCEPTION_TYPE_WITH_OPT,          /* IPv4 packet has options      */
    ROUTE_EXCEPTION_TYPE_TTL_EXCEED,        /* IPv4 TTL exceed packet       */
    ROUTE_EXCEPTION_TYPE_HDR_ERR,           /* IPv4 header error packet, Applicable to 8390*/
    ROUTE_EXCEPTION_TYPE_GW_MAC_ERR,        /* DMAC isn't swith MAC         */
    ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP,    /* IPv6 packet has hop-by-hop header */
    ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT,     /* routing entry age out */
    ROUTE_EXCEPTION_TYPE_END,
} rtk_trap_routeExceptionType_t;

/* Type of CFM */
typedef enum rtk_trap_cfmType_e
{
    TRAP_CFM_TYPE_UNKN,     /* unknown CFM type */
    TRAP_CFM_TYPE_CCM,      /* Continuity Check Message */
    TRAP_CFM_TYPE_LT,       /* Link Trace */
    TRAP_CFM_TYPE_LB,       /* Loop Back */
    TRAP_CFM_TYPE_ETHDM,
    TRAP_CFM_TYPE_END,
} rtk_trap_cfmType_t;

/* Content of user defined RMA packet */
typedef struct rtk_trap_userDefinedRma_s
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_trap_rma_cmpType_t cmpType; /* 0: Compare ADDR only 1: Compare EtherType only:*/
                       /*2: Both ADDR and EtherType */
    rtk_mac_t   mac_min;
    rtk_mac_t   mac_max;
    uint32 frameValue;
#endif
    rtk_mac_t   mac;
    rtk_mac_t   macMask;
} rtk_trap_userDefinedRma_t;

/* Content of user defined management packet */
typedef struct rtk_trap_userDefinedMgmt_s
{
    rtk_enable_t    dipCheckEnable;
    rtk_enable_t    dmacCheckEnable;
    rtk_enable_t    dstL4PortCheck;
    rtk_l4Proto_t   layer4Proto;
    uint32          dstL4Port;
    uint32          mask_of_dstL4Port;
    rtk_enable_t    srcL4PortCheck;
    uint32          srcL4Port;
    uint32          mask_of_srcL4Port;
} rtk_trap_userDefinedMgmt_t;

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_trap_init
 * Description:
 *      Initial the trap module of the specified device..
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize trap module before calling any trap APIs.
 * Changes:
 *      None
 */
extern int32
rtk_trap_init(uint32 unit);

/* Module Name    : Trap                                    */
/* Sub-module Name: Configuration for traping packet to CPU */

/* Module Name    : Trap     */
/* Sub-module Name: RMA      */

/* Function Name:
 *      rtk_trap_rmaAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame.
 * Input:
 *      unit        - unit id
 *      pRma_frame  - Reserved multicast address.
 * Output:
 *      pRma_action - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaAction_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t *pRma_action);

/* Function Name:
 *      rtk_trap_rmaAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 *      rma_action - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - Invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_RMA_ADDR   - invalid RMA address
 *      RT_ERR_RMA_ACTION - Invalid RMA action
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The supported Reserved Multicast Address frame:
 *      Assignment                                                                  Address
 *      RMA_BRG_GROUP (Bridge Group Address)                                        01-80-C2-00-00-00
 *      RMA_FD_PAUSE (IEEE Std 802.3, 1988 Edition, Full Duplex PAUSE operation)    01-80-C2-00-00-01
 *      RMA_SP_MCAST (IEEE Std 802.3ad Slow Protocols-Multicast address)            01-80-C2-00-00-02
 *      RMA_1X_PAE (IEEE Std 802.1X PAE address)                                    01-80-C2-00-00-03
 *      RMA_RESERVED04 (Reserved)                                                   01-80-C2-00-00-04
 *      RMA_MEDIA_ACCESS_USE (Media Access Method Specific Use)                     01-80-C2-00-00-05
 *      RMA_RESERVED06 (Reserved)                                                   01-80-C2-00-00-06
 *      RMA_RESERVED07 (Reserved)                                                   01-80-C2-00-00-07
 *      RMA_PVD_BRG_GROUP (Provider Bridge Group Address)                           01-80-C2-00-00-08
 *      RMA_RESERVED09 (Reserved)                                                   01-80-C2-00-00-09
 *      RMA_RESERVED0A (Reserved)                                                   01-80-C2-00-00-0A
 *      RMA_RESERVED0B (Reserved)                                                   01-80-C2-00-00-0B
 *      RMA_RESERVED0C (Reserved)                                                   01-80-C2-00-00-0C
 *      RMA_MVRP (Provider Bridge MVRP Address)                                     01-80-C2-00-00-0D
 *      RMA_1ab_LL_DISCOVERY (802.1ab Link Layer Discover Protocol Address)         01-80-C2-00-00-0E
 *      RMA_RESERVED0F (Reserved)                                                   01-80-C2-00-00-0F
 *      RMA_BRG_MNGEMENT (All LANs Bridge Management Group Address)                 01-80-C2-00-00-10
 *      RMA_LOAD_SERV_GENERIC_ADDR (Load Server Generic Address)                    01-80-C2-00-00-11
 *      RMA_LOAD_DEV_GENERIC_ADDR (Loadable Device Generic Address)                 01-80-C2-00-00-12
 *      RMA_RESERVED13 (Reserved)                                                   01-80-C2-00-00-13
 *      RMA_RESERVED14 (Reserved)                                                   01-80-C2-00-00-14
 *      RMA_RESERVED15 (Reserved)                                                   01-80-C2-00-00-15
 *      RMA_RESERVED16 (Reserved)                                                   01-80-C2-00-00-16
 *      RMA_RESERVED17 (Reserved)                                                   01-80-C2-00-00-17
 *      RMA_MANAGER_STA_GENERIC_ADDR (Generic Address for All Manager Stations)     01-80-C2-00-00-18
 *      RMA_RESERVED19 (Reserved)                                                   01-80-C2-00-00-19
 *      RMA_AGENT_STA_GENERIC_ADDR (Generic Address for All Agent Stations)         01-80-C2-00-00-1A
 *      RMA_RESERVED1B (Reserved)                                                   01-80-C2-00-00-1B
 *      RMA_RESERVED1C (Reserved)                                                   01-80-C2-00-00-1C
 *      RMA_RESERVED1D (Reserved)                                                   01-80-C2-00-00-1D
 *      RMA_RESERVED1E (Reserved)                                                   01-80-C2-00-00-1E
 *      RMA_RESERVED1F (Reserved)                                                   01-80-C2-00-00-1F
 *      RMA_GMRP (GMRP Address)                                                     01-80-C2-00-00-20
 *      RMA_GVRP (GVRP address)                                                     01-80-C2-00-00-21
 *      RMA_UNDEF_GARP22~2F (Undefined GARP address)                                01-80-C2-00-00-22
 *      (2) The supported Reserved Multicast Address action:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaAction_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t rma_action);

/* Function Name:
 *      rtk_trap_rmaGroupAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame in group.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 * Output:
 *      pRma_action        - pointer buffer of RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      Change to use rtk_trap_rmaAction_get(unit, pRma_frame, pRma_action) for 9300, 9310
 *      The supported Reserved Multicast Address action:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaGroupAction_get(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t *pRma_action);

/* Function Name:
 *      rtk_trap_rmaGroupAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame in group.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 *      rma_action         - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - Invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - Invalid input parameter
 *      RT_ERR_RMA_ACTION - Invalid RMA action
 * Applicable:
 *      8380
 * Note:
 *      Change to use rtk_trap_rmaAction_set(unit, pRma_frame, rma_action) for 9300, 9310
 *      (1) The supported Reserved Multicast Address frame:
 *          - Assignment                 Address
 *          - RMA_SLOW_PROTOCOL_OAM      01-80-C2-00-00-02 & ethertype=0x8809 & subtype = 0x3
 *          - RMA_SLOW_PROTOCOL_OTHER    01-80-C2-00-00-02 except OAM
 *          - RMA_0E_EXCEPT_PTP_LLDP     01-80-C2-00-00-0E
 *          - RMA_GMRP (GMRP Address)    01-80-C2-00-00-20
 *          - RMA_GVRP (GVRP address)    01-80-C2-00-00-21
 *          - RMA_MSRP                   01-80-C2-00-00-22
 *          - RMA_0X                     01-80-C2-00-00-01~01-80-C2-00-00-0F, excluding listed above
 *          - RMA_1X                     01-80-C2-00-00-01~10-80-C2-00-00-1F, excluding listed above
 *          - RMA_2X                     01-80-C2-00-00-01~23-80-C2-00-00-2F, excluding listed above
 *      (2) The supported Reserved Multicast Address action:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaGroupAction_set(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t rma_action);

/* Function Name:
 *      rtk_trap_rmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for the RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaLearningEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_rmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for the RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 *      enable     - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaLearningEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable);

/* Function Name:
 *      rtk_trap_rmaGroupLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 * Output:
 *      pEnable            - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      Change to use rtk_trap_rmaLearningEnable_get(unit, pRma_frame, pEnable) or
 *      rtk_trap_mgmtFrameLearningEnable_get(unit, frameType, pEnable) for 9300, 9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaGroupLearningEnable_get(uint32 unit, rtk_trap_rmaGroup_frameType_t  rmaGroup_frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_rmaGroupLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 *      enable             - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      Change to use rtk_trap_rmaLearningEnable_set(unit, pRma_frame, enable) or
 *      rtk_trap_mgmtFrameLearningEnable_set(unit, frameType, enable) for 9300, 9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaGroupLearningEnable_set(uint32 unit, rtk_trap_rmaGroup_frameType_t  rmaGroup_frameType, rtk_enable_t enable);

/* Function Name:
 *      rtk_trap_bypassStp_get
 * Description:
 *      Get enable status of bypassing spanning tree ingress filtering for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 * Output:
 *      pEnable   - pointer to enable status of bypassing ingress STP check
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      - rtk_trap_bypassStpType_t \ Chip:      8390    8380    9300    9310
 *      - BYPASS_STP_TYPE_USER_DEF_0            O       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_1            O       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_2            X       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_3            X       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_4            X       O       X       X
 *      - BYPASS_STP_TYPE_USER_DEF_5            X       O       X       X
 *      - BYPASS_STP_TYPE_RMA_0X                O       O       X       X
 *      - BYPASS_STP_TYPE_SLOW_PROTO            O       O       X       X
 *      - BYPASS_STP_TYPE_EAPOL                 O       O       X       X
 *      - BYPASS_STP_TYPE_PTP                   O       O       X       X
 *      - BYPASS_STP_TYPE_LLDP                  O       O       X       X
 * Changes:
 *      None
 */
extern int32
rtk_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_bypassStp_set
 * Description:
 *      Set enable status of bypassing spanning tree ingress filtering for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 *      enable    - enable status of bypassing ingress STP check
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      - rtk_trap_bypassStpType_t \ Chip:      8390    8380    9300    9310
 *      - BYPASS_STP_TYPE_USER_DEF_0            O       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_1            O       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_2            X       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_3            X       O       O       O
 *      - BYPASS_STP_TYPE_USER_DEF_4            X       O       X       X
 *      - BYPASS_STP_TYPE_USER_DEF_5            X       O       X       X
 *      - BYPASS_STP_TYPE_RMA_0X                O       O       X       X
 *      - BYPASS_STP_TYPE_SLOW_PROTO            O       O       X       X
 *      - BYPASS_STP_TYPE_EAPOL                 O       O       X       X
 *      - BYPASS_STP_TYPE_PTP                   O       O       X       X
 *      - BYPASS_STP_TYPE_LLDP                  O       O       X       X
 * Changes:
 *      None
 */
extern int32
rtk_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      rtk_trap_bypassVlan_get
 * Description:
 *      Get enable status of bypassing ingress VLAN filtering for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 * Output:
 *      pEnable   - pointer to enable status of bypassing ingress VLAN filtering
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_VLAN_TYPE_USER_DEF_0
 *      - BYPASS_VLAN_TYPE_USER_DEF_1
 *      - BYPASS_VLAN_TYPE_USER_DEF_2 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_USER_DEF_3 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_USER_DEF_4 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_USER_DEF_5 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_RMA_00
 *      - BYPASS_VLAN_TYPE_RMA_02
 *      - BYPASS_VLAN_TYPE_RMA_0E
 *      - BYPASS_VLAN_TYPE_RMA_0X
 *      - BYPASS_VLAN_TYPE_OAM   (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_GMRP  (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_GVRP  (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_EAPOL
 *      - BYPASS_VLAN_TYPE_PTP
 *      - BYPASS_VLAN_TYPE_LLDP
 *     9300,9310 only support BYPASS_STP_TYPE_USER_DEF_0 - 3
 * Changes:
 *      None
 */
extern int32
rtk_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_bypassVlan_set
 * Description:
 *      Set enable status of bypassing ingress VLAN filtering for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 *      enable    - enable status of bypassing bypassing ingress VLAN filtering
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_VLAN_TYPE_USER_DEF_0
 *      - BYPASS_VLAN_TYPE_USER_DEF_1
 *      - BYPASS_VLAN_TYPE_USER_DEF_2 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_USER_DEF_3 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_USER_DEF_4 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_USER_DEF_5 (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_RMA_00
 *      - BYPASS_VLAN_TYPE_RMA_02
 *      - BYPASS_VLAN_TYPE_RMA_0E
 *      - BYPASS_VLAN_TYPE_RMA_0X
 *      - BYPASS_VLAN_TYPE_OAM   (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_GMRP  (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_GVRP  (Applicable to 8380)
 *      - BYPASS_VLAN_TYPE_EAPOL
 *      - BYPASS_VLAN_TYPE_PTP
 *      - BYPASS_VLAN_TYPE_LLDP
 *     9300,9310 only support BYPASS_STP_TYPE_USER_DEF_0 - 3
 * Changes:
 *      None
 */
extern int32
rtk_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable);

/* Module Name    : Trap     */
/* Sub-module Name: User defined RMA */

/* Function Name:
 *      rtk_trap_userDefineRma_get
 * Description:
 *      Get user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pUserDefinedRma - pointer to content of user defined RMA
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      rtk_trap_userDefineRma_set
 * Description:
 *      Set user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      pUserDefinedRma - to content of user defined RMA
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      rtk_trap_userDefineRmaEnable_get
 * Description:
 *      Get enable status of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pEnable        - pointer to enable status of RMA entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_userDefineRmaEnable_set
 * Description:
 *      Set enable status of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      enable         - enable status of RMA entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Function Name:
 *      rtk_trap_userDefineRmaAction_get
 * Description:
 *      Get forwarding action of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pActoin        - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 *      (4) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *          - MGMT_ACTION_TRAP2MASTERCPU    (only for 9300, 9310)
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      rtk_trap_userDefineRmaAction_set
 * Description:
 *      Set forwarding action of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      actoin         - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_FWD_ACTION   - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 *      (4) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *          - MGMT_ACTION_TRAP2MASTERCPU    (only for 9300, 9310)
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action);

/* Function Name:
 *      rtk_trap_userDefineRmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for user-defined RMA frame.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pEnable        - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_userDefineRmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this user-defined RMA frame.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      enable         - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of userDefine_idx is 0~1 in 8390
 *      (2) The valid range of userDefine_idx is 0~5 in 8380
 *      (3) The valid range of userDefine_idx is 0~3 in 9300/9310
 * Changes:
 *      None
 */
extern int32
rtk_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Module Name    : Trap                         */
/* Sub-module Name: System-wise management frame */

/* Function Name:
 *      rtk_trap_mgmtFrameAction_get
 * Description:
 *      Get forwarding action of management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pActoin   - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                          8390       8380       9300       9310
 *          - MGMT_TYPE_ARP                 (1001000)  (1001000)  (   x   )  (   x   )
 *          - MGMT_TYPE_ARP_REQ             (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_ARP_REP             (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_DHCP                (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_DHCP6               (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_MLD                 (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IGMP                (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_EAPOL               (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IPV6ND              (1001000)  (1001000)  (   x   )  (   x   )
 *          - MGMT_TYPE_SELFMAC             (1110000)  (1110000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)  (1110000)  (1110000)  (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)  (   x   )  (1110000)  (   x   )
 *          - MGMT_TYPE_RLDP_RLPP           (   x   )  (   x   )  (1010100)  (   x   )
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      rtk_trap_mgmtFrameAction_set
 * Description:
 *      Set forwarding action of management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      actoin    - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_FWD_ACTION    - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8390       8380       9300       9310
 *          - MGMT_TYPE_ARP                 (1001000)  (1001000)  (   x   )  (   x   )
 *          - MGMT_TYPE_ARP_REQ             (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_ARP_REP             (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_DHCP                (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_DHCP6               (   x   )  (   x   )  (   x   )  (   x   )
 *          - MGMT_TYPE_MLD                 (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IGMP                (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_EAPOL               (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IPV6ND              (1001000)  (1001000)  (   x   )  (   x   )
 *          - MGMT_TYPE_SELFMAC             (1110000)  (1110000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)  (1010000)  (   x   )  (   x   )
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)  (1110000)  (1110000)  (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)  (   x   )  (1110000)  (   x   )
 *          - MGMT_TYPE_RLDP_RLPP           (   x   )  (   x   )  (1010100)  (   x   )
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Function Name:
 *      rtk_trap_mgmtFramePri_get
 * Description:
 *      Get priority of trapped packet.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_trap_mgmtFrameQueue_get(unit, qType, pQid) for 9300, 9310
 *      Supported management frame is as following:
 *      - rtk_trap_mgmtType_t \ Chip:     8390    8380
 *      - MGMT_TYPE_RIP                   X       X
 *      - MGMT_TYPE_ICMP                  X       X
 *      - MGMT_TYPE_ICMPV6                X       X
 *      - MGMT_TYPE_ARP                   O       X
 *      - MGMT_TYPE_MLD                   O       X
 *      - MGMT_TYPE_IGMP                  O       X
 *      - MGMT_TYPE_BGP                   X       X
 *      - MGMT_TYPE_OSPFV2                X       X
 *      - MGMT_TYPE_OSPFV3                X       X
 *      - MGMT_TYPE_SNMP                  X       X
 *      - MGMT_TYPE_SSH                   X       X
 *      - MGMT_TYPE_FTP                   X       X
 *      - MGMT_TYPE_TFTP                  X       X
 *      - MGMT_TYPE_TELNET                X       X
 *      - MGMT_TYPE_HTTP                  X       X
 *      - MGMT_TYPE_HTTPS                 X       X
 *      - MGMT_TYPE_DHCPV6                X       X
 *      - MGMT_TYPE_DHCP                  X       X
 *      - MGMT_TYPE_DOT1X                 X       X
 *      - MGMT_TYPE_BPDU                  O       X
 *      - MGMT_TYPE_PTP                   O       X
 *      - MGMT_TYPE_LLDP                  O       X
 *      - MGMT_TYPE_EAPOL                 O       X
 *      - MGMT_TYPE_IPV6ND                O       X
 *      - MGMT_TYPE_SELFMAC               O       O
 *      - MGMT_TYPE_OTHER                 O       O
 *      - MGMT_TYPE_OAM                   O       X
 *      - MGMT_TYPE_CFM                   O       X
 *      - MGMT_TYPE_IGR_VLAN_FLTR         O       X
 *      - MGMT_TYPE_VLAN_ERR              O       X
 *      - MGMT_TYPE_CFI                   O       O
 *      - MGMT_TYPE_RMA_USR_DEF_1         O       X
 *      - MGMT_TYPE_RMA_USR_DEF_2         O       X
 *      - MGMT_TYPE_LACP                  O       X
 *      - MGMT_TYPE_RMA_0X                O       X
 *      - MGMT_TYPE_RMA_1X                O       X
 *      - MGMT_TYPE_RMA_2X                O       X
 *      - MGMT_TYPE_UNKNOWN_DA            O       O
 *      - MGMT_TYPE_RLDP_RLPP             X       O
 *      - MGMT_TYPE_RMA                   X       O
 *      - MGMT_TYPE_SPECIAL_COPY          X       O
 *      - MGMT_TYPE_SPECIAL_TRAP          X       O
 *      - MGMT_TYPE_ROUT_EXCEPT           X       O
 *      - MGMT_TYPE_MAC_CST_SYS           X       O
 *      - MGMT_TYPE_MAC_CST_PORT          X       O
 *      - MGMT_TYPE_MAC_CST_VLAN          X       O
 *      - MGMT_TYPE_IPV6_HOP_POS_ERR      O       X
 *      - MGMT_TYPE_IPV6_HDR_UNKWN        O       X
 *      - MGMT_TYPE_INVALID_SA            O       X
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFramePri_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t *pPriority);

/* Function Name:
 *      rtk_trap_mgmtFramePri_set
 * Description:
 *      Set priority of trapped packet.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      priority  - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_PRIORITY      - invalid priority value
 * Applicable:
 *      8380, 8390
 * Note:
 *      Change to use rtk_trap_mgmtFrameQueue_set(unit, qType, qid) for 9300, 9310
 *      Supported management frame is as following:
 *      - rtk_trap_mgmtType_t \ Chip:     8390    8380
 *      - MGMT_TYPE_RIP                   X       X
 *      - MGMT_TYPE_ICMP                  X       X
 *      - MGMT_TYPE_ICMPV6                X       X
 *      - MGMT_TYPE_ARP                   O       X
 *      - MGMT_TYPE_MLD                   O       X
 *      - MGMT_TYPE_IGMP                  O       X
 *      - MGMT_TYPE_BGP                   X       X
 *      - MGMT_TYPE_OSPFV2                X       X
 *      - MGMT_TYPE_OSPFV3                X       X
 *      - MGMT_TYPE_SNMP                  X       X
 *      - MGMT_TYPE_SSH                   X       X
 *      - MGMT_TYPE_FTP                   X       X
 *      - MGMT_TYPE_TFTP                  X       X
 *      - MGMT_TYPE_TELNET                X       X
 *      - MGMT_TYPE_HTTP                  X       X
 *      - MGMT_TYPE_HTTPS                 X       X
 *      - MGMT_TYPE_DHCPV6                X       X
 *      - MGMT_TYPE_DHCP                  X       X
 *      - MGMT_TYPE_DOT1X                 X       X
 *      - MGMT_TYPE_BPDU                  O       X
 *      - MGMT_TYPE_PTP                   O       X
 *      - MGMT_TYPE_LLDP                  O       X
 *      - MGMT_TYPE_EAPOL                 O       X
 *      - MGMT_TYPE_IPV6ND                O       X
 *      - MGMT_TYPE_SELFMAC               O       O
 *      - MGMT_TYPE_OTHER                 O       O
 *      - MGMT_TYPE_OAM                   O       X
 *      - MGMT_TYPE_CFM                   O       X
 *      - MGMT_TYPE_IGR_VLAN_FLTR         O       X
 *      - MGMT_TYPE_VLAN_ERR              O       X
 *      - MGMT_TYPE_CFI                   O       O
 *      - MGMT_TYPE_RMA_USR_DEF_1         O       X
 *      - MGMT_TYPE_RMA_USR_DEF_2         O       X
 *      - MGMT_TYPE_LACP                  O       X
 *      - MGMT_TYPE_RMA_0X                O       X
 *      - MGMT_TYPE_RMA_1X                O       X
 *      - MGMT_TYPE_RMA_2X                O       X
 *      - MGMT_TYPE_UNKNOWN_DA            O       O
 *      - MGMT_TYPE_RLDP_RLPP             X       O
 *      - MGMT_TYPE_RMA                   X       O
 *      - MGMT_TYPE_SPECIAL_COPY          X       O
 *      - MGMT_TYPE_SPECIAL_TRAP          X       O
 *      - MGMT_TYPE_ROUT_EXCEPT           X       O
 *      - MGMT_TYPE_MAC_CST_SYS           X       O
 *      - MGMT_TYPE_MAC_CST_PORT          X       O
 *      - MGMT_TYPE_MAC_CST_VLAN          X       O
 *      - MGMT_TYPE_IPV6_HOP_POS_ERR      O       X
 *      - MGMT_TYPE_IPV6_HDR_UNKWN        O       X
 *      - MGMT_TYPE_INVALID_SA            O       X
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFramePri_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t priority);

/* Function Name:
 *      rtk_trap_mgmtFrameQueue_get
 * Description:
 *      Get Queue Id of trapped packet.
 * Input:
 *      unit    - unit id
 *      qType   - type of trapped packet
 * Output:
 *      pQid    - pointer to queue id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_INPUT         - invalid input parameter
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_mgmtFrameQueue_get(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t *pQid);

/* Function Name:
 *      rtk_trap_mgmtFrameQueue_set
 * Description:
 *      Set queue id of trapped packet.
 * Input:
 *      unit    - unit id
 *      qType   - type of trapped  frame
 *      qid     - queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_INPUT         - invalid input parameter
 *      RT_ERR_QUEUE_ID      - invalid queue ide
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_mgmtFrameQueue_set(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t qid);

/* Function Name:
 *      rtk_trap_mgmtFrameLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for the management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pEnable   - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_BPDU (Applicable to 8380)
 *      - MGMT_TYPE_EAPOL(Applicable to 9300,9310)
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_mgmtFrameLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for the management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      enable    - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_INPUT         - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_BPDU (Applicable to 8380)
 *      - MGMT_TYPE_EAPOL(Applicable to 9300,9310)
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable);

/* Module Name    : Trap                                   */
/* Sub-module Name: Per port user defined management frame */

/* Function Name:
 *      rtk_trap_portMgmtFrameAction_get
 * Description:
 *      Get forwarding action of management frame on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      frameType - type of management frame
 * Output:
 *      pActoin   - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8390       8380       9300       9310
 *          - MGMT_TYPE_BPDU                (1110001)  (1110001)  (1110101)  (1110101)
 *          - MGMT_TYPE_PTP                 (1110000)  (1110000)  (1110100)  (1110100)
 *          - MGMT_TYPE_PTP_UDP             (  x    )  (  x    )  (1110100)  (1110100)
 *          - MGMT_TYPE_PTP_ETH2            (  x    )  (  x    )  (1110100)  (1110100)
 *          - MGMT_TYPE_LLDP                (1110001)  (1110001)  (1110101)  (1110101)
 *          - MGMT_TYPE_EAPOL               (  x    )  (  x    )  (1110101)  (1110101)
 *          - MGMT_GRATUITOUS_ARP           (  x    )  (  x    )  (1111000)  (1111000)
 *
 *          MGMT_TYPE_PTP is accepted by 9300 and 9310 only when BACKWARD-COMPATIBLE is configured.
 * Changes:
 *      None
 */
extern int32
rtk_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      rtk_trap_portMgmtFrameAction_set
 * Description:
 *      Set forwarding action of management frame on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      frameType - type of management frame
 *      actoin    - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_FWD_ACTION    - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8390       8380       9300       9310
 *          - MGMT_TYPE_BPDU                (1110001)  (1110001)  (1110101)  (1110101)
 *          - MGMT_TYPE_PTP                 (1110000)  (1110000)  (1110100)  (1110100)
 *          - MGMT_TYPE_PTP_UDP             (  x    )  (  x    )  (1110100)  (1110100)
 *          - MGMT_TYPE_PTP_ETH2            (  x    )  (  x    )  (1110100)  (1110100)
 *          - MGMT_TYPE_LLDP                (1110001)  (1110001)  (1110101)  (1110101)
 *          - MGMT_TYPE_EAPOL               (  x    )  (  x    )  (1110101)  (1110101)
 *          - MGMT_GRATUITOUS_ARP           (  x    )  (  x    )  (1111000)  (1111000)
 *
 *          MGMT_TYPE_PTP is accepted by 9300 and 9310 only when BACKWARD-COMPATIBLE is configured.
 * Changes:
 *      None
 */
extern int32
rtk_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Module Name    : Trap                               */
/* Sub-module Name: Packet with special flag or option */

/* Function Name:
 *      rtk_trap_pktWithCFIAction_get
 * Description:
 *      Get forwarding action of ethernet packet with CFI set.
 * Input:
 *      unit    - unit id
 * Output:
 *      pActoin - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_TRAP2MASTERCPU
 * Changes:
 *      None
 */
extern int32
rtk_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      rtk_trap_pktWithCFIAction_set
 * Description:
 *      Set forwarding action of ethernet packet with CFI set.
 * Input:
 *      unit   - unit id
 *      actoin - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_TRAP2MASTERCPU
 * Changes:
 *      None
 */
extern int32
rtk_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      rtk_trap_pktWithOuterCFIAction_get
 * Description:
 *      Get forwarding action of ethernet packet with outer CFI set.
 * Input:
 *      unit    - unit id
 * Output:
 *      pActoin - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_TRAP2MASTERCPU
 * Changes:
 *      None
 */
extern int32
rtk_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      rtk_trap_pktWithOuterCFIAction_set
 * Description:
 *      Set forwarding action of ethernet packet with outer CFI set.
 * Input:
 *      unit   - unit id
 *      actoin - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_TRAP2MASTERCPU
 * Changes:
 *      None
 */
extern int32
rtk_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      rtk_trap_pktWithCFIPri_get
 * Description:
 *      Get priority of packets trapped to CPU with CFI=1.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_trap_pktWithCFIPri_get(uint32 unit, rtk_pri_t *pPriority);

/* Function Name:
 *      rtk_trap_pktWithCFIPri_set
 * Description:
 *      Set priority of packets trapped to CPU with CFI=1.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_trap_pktWithCFIPri_set(uint32 unit, rtk_pri_t priority);

/* Module Name    : Trap       */
/* Sub-module Name: CFM and OAM packet */

/* Function Name:
 *      rtk_trap_cfmFrameTrapPri_get
 * Description:
 *      Get priority of CFM packets trapped to CPU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_trap_cfmFrameTrapPri_get(uint32 unit, rtk_pri_t *pPriority);

/* Function Name:
 *      rtk_trap_cfmFrameTrapPri_set
 * Description:
 *      Set priority of CFM packets trapped to CPU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_trap_cfmFrameTrapPri_set(uint32 unit, rtk_pri_t priority);

/* Function Name:
 *      rtk_trap_oamPDUAction_get
 * Description:
 *      Get forwarding action of trapped oam PDU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to forwarding action of trapped oam PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 * Changes:
 *      None
 */
extern int32
rtk_trap_oamPDUAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      rtk_trap_oamPDUAction_set
 * Description:
 *      Set forwarding action of trapped oam PDU.
 * Input:
 *      unit   - unit id
 *      action - forwarding action of trapped oam PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *      (2) When configure trap action, the non OAM PDU check OAM loopback parser action.
 *          Otherwise the non OAM PDU will be normal forward.
 * Changes:
 *      None
 */
extern int32
rtk_trap_oamPDUAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      rtk_trap_oamPDUPri_get
 * Description:
 *      Get priority of trapped OAM PDU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      The priority takes effect if 'rtk_trap_oamPDUAction_set' is set to ACTION_TRAP2CPU.
 * Changes:
 *      None
 */
extern int32
rtk_trap_oamPDUPri_get(uint32 unit, rtk_pri_t *pPriority);

/* Function Name:
 *      rtk_trap_oamPDUPri_set
 * Description:
 *      Set priority of trapped OAM PDU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      The priority takes effect if 'rtk_trap_oamPDUAction_set' is set to ACTION_TRAP2CPU.
 * Changes:
 *      None
 */
extern int32
rtk_trap_oamPDUPri_set(uint32 unit, rtk_pri_t priority);
#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
/* Function Name:
 *      rtk_trap_cfmUnknownFrameAct_get
 * Description:
 *      Get action for receive unknown type of CFM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of action for receive unknown type of CFM frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_get(unit, type, mdl, *pAction)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_UNKN
 *              mdl                     - 0
 */
extern int32
rtk_trap_cfmUnknownFrameAct_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      rtk_trap_cfmUnknownFrameAct_set
 * Description:
 *      Set action for receive unknown type of CFM frame.
 * Input:
 *      unit   - unit id
 *      action - receive unknown type of CFM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_set(unit, type, mdl, action)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_UNKN
 *              mdl                     - 0
 */
extern int32
rtk_trap_cfmUnknownFrameAct_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      rtk_trap_cfmLoopbackLinkTraceAct_get
 * Description:
 *      Get action for receive CFM loopback frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 * Output:
 *      pAction - pointer buffer of action for receive CFM loopback frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      loopback action combine with linktrace in 8390
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_get(unit, type, mdl, *pAction)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_LB
 *              mdl                     - level
 */
extern int32
rtk_trap_cfmLoopbackLinkTraceAct_get(uint32 unit, uint32 level, rtk_action_t *pAction);

/* Function Name:
 *      rtk_trap_cfmLoopbackLinkTraceAct_set
 * Description:
 *      Set action for receive CFM loopback frame.
 * Input:
 *      unit   - unit id
 *      level  - MD level
 *      action - receive CFM loopback frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      loopback action combine with linktrace in 8390
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_set(unit, type, mdl, action)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_LB
 *              mdl                     - level
 */
extern int32
rtk_trap_cfmLoopbackLinkTraceAct_set(uint32 unit, uint32 level, rtk_action_t action);

/* Function Name:
 *      rtk_trap_cfmCcmAct_get
 * Description:
 *      Get action for receive CFM CCM frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 * Output:
 *      pAction - pointer buffer of action for receive CFM CCM frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Set action to TRAP_OAM_ACTION_LINK_FAULT_DETECT to enable G.8032 link fault detection.
 *      Forwarding action is as following
 *      - TRAP_OAM_ACTION_FORWARD
 *      - TRAP_OAM_ACTION_DROP
 *      - TRAP_OAM_ACTION_TRAP2CPU
 *      - TRAP_OAM_ACTION_LINK_FAULT_DETECT
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_get(unit, type, mdl, *pAction)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_CCM
 *              mdl                     - level
 */
extern int32
rtk_trap_cfmCcmAct_get(uint32 unit, uint32 level,rtk_trap_oam_action_t *pAction);

/* Function Name:
 *      rtk_trap_cfmCcmAct_set
 * Description:
 *      Set action for receive CFM CCM frame.
 * Input:
 *      unit   - unit id
 *      level  - MD level
 *      action - receive CFM CCM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Set action to TRAP_OAM_ACTION_LINK_FAULT_DETECT to enable G.8032 link fault detection.
 *      Forwarding action is as following
 *      - TRAP_OAM_ACTION_FORWARD
 *      - TRAP_OAM_ACTION_DROP
 *      - TRAP_OAM_ACTION_TRAP2CPU
 *      - TRAP_OAM_ACTION_LINK_FAULT_DETECT
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_set(unit, type, mdl, action)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_CCM
 *              mdl                     - level
 */
extern int32
rtk_trap_cfmCcmAct_set(uint32 unit, uint32 level, rtk_trap_oam_action_t action);

/* Function Name:
 *      rtk_trap_cfmEthDmAct_get
 * Description:
 *      Get action for received CFM ETH-DM frame in specified MD level.
 * Input:
 *      unit    - unit id
 *      mdl     - MD level
 * Output:
 *      pAction - pointer to action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      The supported actions are ACTION_FORWARD, ACTION_DROP and ACTION_TRAP2CPU for 8390.
*      The supported actions are ACTION_FORWARD, ACTION_DROP, ACTION_TRAP2CPU,
*      and ACTION_TRAP2MASTERCPU for 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_get(unit, type, mdl, *pAction)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_ETHDM
 *              mdl                     - mdl
 */
extern int32
rtk_trap_cfmEthDmAct_get(uint32 unit, uint32 mdl, rtk_action_t *pAction);

/* Function Name:
 *      rtk_trap_cfmEthDmAct_set
 * Description:
 *      Set action for received CFM ETH-DM frame in specified MD level.
 * Input:
 *      unit   - unit id
 *      mdl    - MD level
 *      action - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
*      The supported actions are ACTION_FORWARD, ACTION_DROP and ACTION_TRAP2CPU for 8390.
*      The supported actions are ACTION_FORWARD, ACTION_DROP, ACTION_TRAP2CPU,
*      and ACTION_TRAP2MASTERCPU for 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_trap_cfmAct_set(unit, type, mdl, action)
 *          Parameters:
 *              type                    - TRAP_CFM_TYPE_ETHDM
 */
extern int32
rtk_trap_cfmEthDmAct_set(uint32 unit, uint32 mdl, rtk_action_t action);
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
/* Function Name:
 *      rtk_trap_portOamLoopbackParAction_get
 * Description:
 *      Get action of parser.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to parser action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      1) Parser action takes effect only if 'rtk_trap_oamPDUAction_set' is set to ACTION_TRAP2CPU.
 *      2) Forwarding action is as following
 *         - TRAP_OAM_ACTION_FORWARD
 *         - TRAP_OAM_ACTION_DROP
 *         - TRAP_OAM_ACTION_LOOPBACK
 * Changes:
 *      None
 */
extern int32
rtk_trap_portOamLoopbackParAction_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_trap_oam_action_t   *pAction);

/* Function Name:
 *      rtk_trap_portOamLoopbackParAction_set
 * Description:
 *      Set action of parser.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - parser action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      1) Parser action takes effect only if 'rtk_trap_oamPDUAction_set' is set to ACTION_TRAP2CPU.
 *      2) Forwarding action is as following
 *         - TRAP_OAM_ACTION_FORWARD
 *         - TRAP_OAM_ACTION_DROP
 *         - TRAP_OAM_ACTION_LOOPBACK
 * Changes:
 *      None
 */
extern int32
rtk_trap_portOamLoopbackParAction_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_trap_oam_action_t   action);

/* Function Name:
 *      rtk_trap_routeExceptionAction_get
 * Description:
 *      Get the configuration of routing exception operation.
 * Input:
 *      unit    - unit id
 *      type    - configure action for which route exception
 * Output:
 *      pAction - pointer to route exception operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Forwarding action is as following
 *      - TRAP_OAM_ACTION_FORWARD
 *      - TRAP_OAM_ACTION_DROP
 *      - TRAP_OAM_ACTION_TRAP2CPU
 * Changes:
 *      Please refer L3 module
 */
extern int32
rtk_trap_routeExceptionAction_get(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    *pAction);

/* Function Name:
 *      rtk_trap_routeExceptionAction_set
 * Description:
 *      Set the configuration of routing exception operation.
 * Input:
 *      unit   - unit id
 *      type   - configure action for which route exception
 *      action - route exception operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      Forwarding action is as following
 *      - TRAP_OAM_ACTION_FORWARD
 *      - TRAP_OAM_ACTION_DROP
 *      - TRAP_OAM_ACTION_TRAP2CPU
 * Changes:
 *      Please refer L3 module
 */
extern int32
rtk_trap_routeExceptionAction_set(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    action);

/* Function Name:
 *      rtk_trap_routeExceptionPri_get
 * Description:
 *      Get priority of route exception packets trapped to CPU.
 * Input:
 *      unit      - unit id
 *      type      - configure priority for which route exception
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      ROUTE_EXCEPTION_TYPE_GW_MAC_ERR and ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT share the same priority
 *      setting. That is, configure ROUTE_EXCEPTION_TYPE_GW_MAC_ERR also affect ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT.
 * Changes:
 *      Please refer L3 module
 */
extern int32
rtk_trap_routeExceptionPri_get(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_pri_t                       *pPriority);

/* Function Name:
 *      rtk_trap_routeExceptionPri_set
 * Description:
 *      Set priority of route exception packets trapped to CPU.
 * Input:
 *      unit     - unit id
 *      type     - configure priority for which route exception
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PRIORITY - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      ROUTE_EXCEPTION_TYPE_GW_MAC_ERR and ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT share the same priority
 *      setting. That is, configure ROUTE_EXCEPTION_TYPE_GW_MAC_ERR also affect ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT.
 * Changes:
 *      Please refer L3 module
 */
extern int32
rtk_trap_routeExceptionPri_set(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_pri_t                       priority);

/* Function Name:
 *      rtk_trap_mgmtFrameMgmtVlanEnable_get
 * Description:
 *      Get status of comparing forwarding VID with P-VID of CPU port.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The configuration applies to IPv6 neighbor discovery(MGMT_TYPE_IPV6ND), ARP request(MGMT_TYPE_ARP)
 *          and Switch MAC(MGMT_TYPE_SELFMAC) packets.
 *      (2) The function should be enabled for management VLAN application and disabled for unmanagement
 *          VLAN application.
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameMgmtVlanEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_mgmtFrameMgmtVlanEnable_set
 * Description:
 *      Set status of comparing forwarding VID with P-VID of CPU port.
 * Input:
 *      unit   - unit id
 *      enable - enable state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The configuration applies to IPv6 neighbor discovery(MGMT_TYPE_IPV6ND), ARP request(MGMT_TYPE_ARP)
 *          and Switch MAC(MGMT_TYPE_SELFMAC) packets.
 *      (2) The function should be enabled for management VLAN application and disabled for unmanagement
 *          VLAN application.
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameMgmtVlanEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_trap_mgmtFrameSelfARPEnable_get
 * Description:
 *      Get state of copy Self-ARP to CPU.
 * Input:
 *      unit   - unit id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The configuration only applies to ARP request(MGMT_TYPE_ARP) packet.
 *      (2) All the ARP Request packets are copied to CPU by setting rtk_trap_mgmtFrameAction_set(MGMT_TYPE_ARP).
 *          But if the function is enabled, Only ARP Request destined to switch's IP (rtk_switch_IPv4Addr_set)
 *          is copied to CPU.
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameSelfARPEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_mgmtFrameSelfARPEnable_set
 * Description:
 *      Set state of copy Self-ARP to CPU.
 * Input:
 *      unit   - unit id
 *      enable - enable state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The configuration only applies to ARP request(MGMT_TYPE_ARP) packet.
 *      (2) All the ARP Request packets are copied to CPU by setting rtk_trap_mgmtFrameAction_set(MGMT_TYPE_ARP).
 *          But if the function is enabled, Only ARP Request destined to switch's IP (rtk_switch_IPv4Addr_set)
 *          is copied to CPU.
 * Changes:
 *      None
 */
extern int32
rtk_trap_mgmtFrameSelfARPEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_trap_bpduFloodPortmask_get
 * Description:
 *      Get BPDU flooding portmask which used while BPDU action is flood to all port
 * Input:
 *      unit            - unit id
 *      pflood_portmask - BPDU flood portmask
 * Output:
 *      pEnable         - status of trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The configuration applies while BPDU packet lookup missed and action is set to flood to all port.
 * Changes:
 *      None
 */
extern int32
rtk_trap_bpduFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_bpduFloodPortmask_set
 * Description:
 *      Set BPDU flooding portmask which used while BPDU action is flood to all port
 * Input:
 *      unit            - unit id
 *      pflood_portmask - BPDU flood portmask
 * Output:
 *      pEnable         - status of trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The configuration applies while BPDU packet lookup missed and action is set to flood to all port.
 * Changes:
 *      None
 */
extern int32
rtk_trap_bpduFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_eapolFloodPortmask_get
 * Description:
 *      Get EAPOL flooding portmask which used while EAPOL action is flood to all port
 * Input:
 *      unit            - unit id
 * Output:
 *      pflood_portmask - EAPOL flood portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1)The configuration applies while EAPOL packet lookup missed and action is set to flood to all port.
 *      (2) The packet which refer this portmask will bypass egress vlan filter,
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_eapolFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_eapolFloodPortmask_set
 * Description:
 *      Set EAPOL flooding portmask which used while EAPOL action is flood to all port
 * Input:
 *      unit            - unit id
 *      pflood_portmask - EAPOL flood portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1)The configuration applies while EAPOL packet lookup missed and action is set to flood to all port.
 *      (2) The packet which refer this portmask will bypass egress vlan filter,
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_eapolFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_lldpFloodPortmask_get
 * Description:
 *      Get LLDP flooding portmask which used while LLDP action is flood to all port
 * Input:
 *      unit            - unit id
 * Output:
 *      pflood_portmask - LLDP flood portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1)The configuration applies while LLDP packet lookup missed and action is set to flood to all port.
 *      (2) The packet which refer this portmask will bypass egress vlan filter,
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_lldpFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_lldpFloodPortmask_set
 * Description:
 *      Set LLDP flooding portmask which used while LLDP action is flood to all port
 * Input:
 *      unit            - unit id
 *      pflood_portmask - LLDP flood portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1)The configuration applies while LLDP packet lookup missed and action is set to flood to all port.
 *      (2) The packet which refer this portmask will bypass egress vlan filter,
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_lldpFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_userDefineFloodPortmask_get
 * Description:
 *      Get user-defined flooding portmask which used while user-defined action is flood to all port
 * Input:
 *      unit            - unit id
 * Output:
 *      pflood_portmask - user-defined flood portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The configuration applies while packet which hit user-defined RMA entry lookup missed and action is set to flood to all port.
 *      (2) The packet which refer this portmask will bypass egress vlan filter,
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_userDefineFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_userDefineFloodPortmask_set
 * Description:
 *      Set user-defined flooding portmask which used while user-defined action is flood to all port
 * Input:
 *      unit            - unit id
 *      pflood_portmask - user-defined flood portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The configuration applies while packet which hit user-defined RMA entry lookup missed and action is set to flood to all port.
 *      (2) The packet which refer this portmask will bypass egress vlan filter,
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_userDefineFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_rmaFloodPortmask_get
 * Description:
 *      Get RMA flooding portmask which used
 * Input:
 *      unit            - unit id
 * Output:
 *      pflood_portmask - RMA flood portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) RMA flood to this portmask when the action is "forward" and L2 Table lookup miss(include BPDU,EAPOL).
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_rmaFloodPortmask_get(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_rmaFloodPortmask_set
 * Description:
 *      Set RMA flooding portmask which used
 * Input:
 *      unit            - unit id
 *      pflood_portmask - RMA flood portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) RMA flood to this portmask when the action is "forward" and L2 Table lookup miss(include BPDU,EAPOL).
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_rmaFloodPortmask_set(uint32 unit,  rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      rtk_trap_rmaCancelMirror_get
 * Description:
 *      Get RMA Cancel mirror configuration
 * Input:
 *      unit            - unit id
 * Output:
 *      pEnable        - pointer to enable status of RMA Cancel Mirror Setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_rmaCancelMirror_get(uint32 unit,  rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_rmaCancelMirror_set
 * Description:
 *      Set enable status of RMA Cancel mirror
 * Input:
 *      unit            - unit id
 *      enable        - enable status of  RMA Cancel Mirror
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_rmaCancelMirror_set(uint32 unit,  rtk_enable_t enable);

/* Function Name:
 *      rtk_trap_rmaLookupMissActionEnable_get
 * Description:
 *      Get enable status of RMA care lookup miss action.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of RMA care lookup miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Enable is care lookup miss action.
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaLookupMissActionEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_rmaLookupMissActionEnable_set
 * Description:
 *      Set enable status of RMA care lookup miss action.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of RMA care lookup miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_INPUT            - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      Enable is care lookup miss action.
 * Changes:
 *      None
 */
extern int32
rtk_trap_rmaLookupMissActionEnable_set(uint32 unit, rtk_enable_t enable);


/* Function Name:
 *      rtk_trap_cfmAct_get
 * Description:
 *      Get action for receive specified type of CFM frame.
 * Input:
 *      unit    - unit id
 *      type    - CFM frame type
 *      mdl     - MD level (for unknow type, the field is not used)
 * Output:
 *      pAction - pointer buffer of action for receive specified type of CFM frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9310
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_LINK_FAULT_DETECT (only for CCM)
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_trap_cfmCcmAct_get
 *              rtk_trap_cfmEthDmAct_get
 *              rtk_trap_cfmLoopbackLinkTraceAct_get
 *              rtk_trap_cfmUnknownFrameAct_get
 */
extern int32
rtk_trap_cfmAct_get(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t *pAction);

/* Function Name:
 *      rtk_trap_cfmAct_set
 * Description:
 *      Set action for receive specified type of CFM frame.
 * Input:
 *      unit    - unit id
 *      type    - CFM frame type
 *      mdl     - MD level (for unknow type, the field is not used)
 *      action  - receive specified type of CFM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9310
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_LINK_FAULT_DETECT (only for CCM)
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_trap_cfmCcmAct_set
 *              rtk_trap_cfmEthDmAct_set
 *              rtk_trap_cfmLoopbackLinkTraceAct_set
 *              rtk_trap_cfmUnknownFrameAct_set
 */
extern int32
rtk_trap_cfmAct_set(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t action);

/* Function Name:
 *      rtk_trap_cfmTarget_get
 * Description:
 *      Get information of trap CFM CCM, LT and LB packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of CFM trap CFM CCM, LT and LB packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      For unknown, CCM, Loopback and linktrace type.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_cfmTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      rtk_trap_cfmTarget_set
 * Description:
 *      Set information of CFM trap CFM CCM, LT and LB packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of CFM trap CFM CCM, LT and LB packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_cfmTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      rtk_trap_oamTarget_get
 * Description:
 *      Get information of OAM PDU trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of OAM PDU trap packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_oamTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      rtk_trap_oamTarget_set
 * Description:
 *      Set information of OAM PDU trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of OAM PDU trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_oamTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      rtk_trap_mgmtFrameTarget_get
 * Description:
 *      Get information of management frame trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of management frame trap
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_mgmtFrameTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      rtk_trap_mgmtFrameTarget_set
 * Description:
 *      Set information of management frame trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of management frame trap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_mgmtFrameTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      rtk_trap_capwapInvldHdr_get
 * Description:
 *      Get trap status of invalid capwap packet.
 * Input:
 *      unit        - unit id
 * Output:
 *      pEnable     - pointer to tarp status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Invalid CAPWAP packet is dropped when trap is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_capwapInvldHdr_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_trap_capwapInvldHdr_set
 * Description:
 *      Set trap status of invalid capwap packet.
 * Input:
 *      unit        - unit id
 *      enable      - enable trap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      Invalid CAPWAP packet is dropped when trap is disabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_trap_capwapInvldHdr_set(uint32 unit, rtk_enable_t enable);


#endif /* __RTK_TRAP_H__ */


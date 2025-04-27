/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public Layer3 routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Layer3 routing error handling
 */

#ifndef __RTK_L3_H__
#define __RTK_L3_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
#define RTK_L3_HOST_IP_PREFIX_LENGTH        (32)
#define RTK_L3_HOST_IP6_PREFIX_LENGTH       (128)
#define RTK_L3_ECMP_HASH_IDX_MAX            (16)    /* maximum nexthop number for all Chips */
#define RTK_L3_ECMP_NEXTHOP_CNT_MAX         (8)     /* maximum nexthop number for all Chips */
#define RTK_L3_VRRP_VRID_MAX                (256)   /* maximum VRID for VRRP */


/* rtk_l3_act_t */
typedef enum rtk_l3_act_e
{
    RTK_L3_ACT_FORWARD = 0,         /* bridging or routing */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_HARD_DROP,
    RTK_L3_ACT_END
} rtk_l3_act_t;

/* rtk_l3_intfKeyType_t */
typedef enum rtk_l3_intfKeyType_e
{
    RTK_L3_INTFKEYTYPE_NONE = 0,
    RTK_L3_INTFKEYTYPE_INTF_ID,
    RTK_L3_INTFKEYTYPE_VID,
    RTK_L3_INTFKEYTYPE_MAC_AND_VID,
} rtk_l3_intfKeyType_t;

/* L3 common flags */
typedef enum rtk_l3_flag_e
{
    RTK_L3_FLAG_NONE                = 0x00000000,   /* none */
    RTK_L3_FLAG_IPV6                = 0x00000001,   /* IPv6-specific (belong to IPv6 related entry) */
    RTK_L3_FLAG_MC_KEY_INTF         = 0x00000002,   /* for IP multicast, use interface as key (S,G,INTF) instead of VID (S,G,VID) */
    RTK_L3_FLAG_REPLACE             = 0x00000004,   /* replace the existing entry with the new info */
    RTK_L3_FLAG_NULL_INTF           = 0x00000008,   /* null interface */
    RTK_L3_FLAG_TTL_DEC_IGNORE      = 0x00000010,   /* ignore TTL decreasement */
    RTK_L3_FLAG_TTL_CHK_IGNORE      = 0x00000020,   /* ignore TTL check */
    RTK_L3_FLAG_QOS_ASSIGN          = 0x00000040,   /* assign a new internal priority */
    RTK_L3_FLAG_HIT                 = 0x00000080,   /* hit-specific (hit bit is set) */
    RTK_L3_FLAG_HIT_CLEAR           = 0x00000100,   /* action - clear the hit bit if it's set */
    RTK_L3_FLAG_NEGATE              = 0x00000200,   /* action is only applied to the unmatched entries */
    RTK_L3_FLAG_WITH_NEXTHOP        = 0x00000400,   /* assign path with nexthop entry */
    RTK_L3_FLAG_READ_HIT_IGNORE     = 0x00000800,   /* when get l3 host entry, ignore entry hit flag, only for 9300 */
    RTK_L3_FLAG_WITH_ID             = 0x00001000,   /* specify entry ID by caller */
    RTK_L3_FLAG_WITH_NH_DMAC        = 0x00002000,   /* specify nexthop DMAC address, only for 9300 */
    RTK_L3_FLAG_END
} rtk_l3_flag_t;

/* L3 VRRP flags */
typedef enum rtk_l3_vrrp_flag_e
{
    RTK_L3_VRRP_FLAG_IPV4           = 0x00000001,   /* VRID for IPV4 */
    RTK_L3_VRRP_FLAG_IPV6           = 0x00000002,   /* VRID for IPV6 */
    RTK_L3_VRRP_FLAG_END
} rtk_l3_vrrp_flag_t;

/* path ID (ECMP/nexthop) */
typedef uint32 rtk_l3_pathId_t;

/* types of ECMP hash key */
typedef enum rtk_l3_ecmpHashKey_e
{
    RTK_L3_ECMP_HASHKEY_PORT_ID     = (0x1 << 0),   /* source port */
    RTK_L3_ECMP_HASHKEY_TRUNK_ID    = (0x1 << 1),   /* trunk id */
    RTK_L3_ECMP_HASHKEY_SIP         = (0x1 << 2),   /* source IP */
    RTK_L3_ECMP_HASHKEY_DIP         = (0x1 << 3),   /* destination IP */
    RTK_L3_ECMP_HASHKEY_IP_DSCP     = (0x1 << 4),   /* IP DSCP */
    RTK_L3_ECMP_HASHKEY_IP_PROTO    = (0x1 << 5),   /* IP protocol */
    RTK_L3_ECMP_HASHKEY_IP6_LABEL   = (0x1 << 6),   /* IPv6 flow label */
    RTK_L3_ECMP_HASHKEY_SPORT       = (0x1 << 7),   /* L4 (TCP/UDP) source port */
    RTK_L3_ECMP_HASHKEY_DPORT       = (0x1 << 8),   /* L4 (TCP/UDP) destination port */
} rtk_l3_ecmpHashKey_t;

/* L3 interface control type (ICT) */
typedef enum rtk_l3_intfCtrlType_e
{
    RTK_L3_ICT_NONE = 0,                            /* Dummy */

    RTK_L3_ICT_IPUC_ROUTE_EN,                       /* rtk_enable_t (IGR: IPUC_ROUTE_EN) */
    RTK_L3_ICT_IP6UC_ROUTE_EN,                      /* rtk_enable_t (IGR: IP6UC_ROUTE_EN) */
    RTK_L3_ICT_IPMC_ROUTE_EN,                       /* rtk_enable_t (IGR: IPMC_ROUTE_EN) */
    RTK_L3_ICT_IP6MC_ROUTE_EN,                      /* rtk_enable_t (IGR: IP6MC_ROUTE_EN) */

    RTK_L3_ICT_IPUC_URPF_CHK_EN,                    /* rtk_enable_t (IGR: IP_URPF_CHK_EN) */
    RTK_L3_ICT_IPUC_URPF_DFLT_ROUTE_EN,             /* rtk_enable_t (IGR: IP_URPF_DFLT_ROUTE_EN) */
    RTK_L3_ICT_IPUC_URPF_MODE,                      /* rtk_l3_urpf_mode_t (IGR: IP_URPF_CHK_MODE) */
    RTK_L3_ICT_IPUC_URPF_FAIL_ACT,                  /* rtk_l3_act_t (IGR: IP_URPF_FAIL_ACT) */
    RTK_L3_ICT_IPUC_ICMP_REDIR_ACT,                 /* rtk_l3_act_t (EGR: IP_ICMP_REDIRECT_ACT) */
    RTK_L3_ICT_IPUC_PBR_ICMP_REDIR_ACT,             /* rtk_l3_act_t (EGR: IP_PBR_ICMP_REDIRECT_ACT) */

    RTK_L3_ICT_IP6UC_URPF_CHK_EN,                   /* rtk_enable_t (IGR: IP6_URPF_CHK_EN) */
    RTK_L3_ICT_IP6UC_URPF_DFLT_ROUTE_EN,            /* rtk_enable_t (IGR: IP6_URPF_DFLT_ROUTE_EN) */
    RTK_L3_ICT_IP6UC_URPF_MODE,                     /* rtk_l3_urpf_mode_t   (IGR: IP6_URPF_CHK_MODE) */
    RTK_L3_ICT_IP6UC_URPF_FAIL_ACT,                 /* rtk_l3_act_t (IGR: IP6_URPF_FAIL_ACT) */
    RTK_L3_ICT_IP6UC_ICMP_REDIR_ACT,                /* rtk_l3_act_t (EGR: IP6_ICMP_REDIRECT_ACT) */
    RTK_L3_ICT_IP6UC_PBR_ICMP_REDIR_ACT,            /* rtk_l3_act_t (EGR: IP6_PBR_ICMP_REDIRECT_ACT) */

    RTK_L3_ICT_IPMC_DIP_224_0_0_X_ACT,              /* rtk_l3_act_t (IGR: IPMC_ACT_224_0_0_X) */
    RTK_L3_ICT_IPMC_DIP_224_0_1_X_ACT,              /* rtk_l3_act_t (IGR: IPMC_ACT_224_0_1_X) */
    RTK_L3_ICT_IPMC_DIP_239_X_X_X_ACT,              /* rtk_l3_act_t (IGR: IPMC_ACT_239_X_X_X)*/
    RTK_L3_ICT_IPMC_ROUTE_LU_MIS_ACT,               /* rtk_l3_act_t (IGR: IPMC_ROUTE_LU_MIS_ACT) */

    RTK_L3_ICT_IP6_ND_ACT,                          /* rtk_l3_act_t (IGR: IP6_ND_ACT) */

    RTK_L3_ICT_IP6MC_DIP_FF0X_0000_XXXX_ACT,        /* rtk_l3_act_t (IGR: IP6MC_ACT_0000_00XX) */
    RTK_L3_ICT_IP6MC_DIP_FF0X_XXXX_XXXX_ACT,        /* rtk_l3_act_t (IGR: IP6_MLD_ACT_0_X_X) */
    RTK_L3_ICT_IP6MC_DIP_FF0X_DB8_0_0_ACT,          /* rtk_l3_act_t (IGR: IP6_MLD_ACT_DB8_X_X) */
    RTK_L3_ICT_IP6MC_ROUTE_LU_MIS_ACT,              /* rtk_l3_act_t (IGR: IP6MC_ROUTE_LU_MIS_ACT) */

    RTK_L3_ICT_MC_KEY_SEL,                          /* rtk_l3_ipmcKeySel_t  (IGR: MC_KEY_SEL) */

    RTK_L3_ICT_END                                  /* end of enum */
} rtk_l3_intfCtrlType_t;

/* L3 port control type (PCT) */
typedef enum rtk_l3_portCtrlType_e
{
    RTK_L3_PCT_NONE = 0,                            /* Dummy */

    RTK_L3_PCT_IPUC_URPF_CHK_EN,                    /* rtk_enable_t */
    RTK_L3_PCT_IPUC_URPF_DFLT_ROUTE_EN,             /* rtk_enable_t */
    RTK_L3_PCT_IPUC_URPF_MODE,                      /* rtk_l3_urpf_mode_t */
    RTK_L3_PCT_IPUC_URPF_FAIL_ACT,                  /* rtk_l3_act_t */

    RTK_L3_PCT_IP6UC_URPF_CHK_EN,                   /* rtk_enable_t */
    RTK_L3_PCT_IP6UC_URPF_DFLT_ROUTE_EN,            /* rtk_enable_t */
    RTK_L3_PCT_IP6UC_URPF_MODE,                     /* rtk_l3_urpf_mode_t */
    RTK_L3_PCT_IP6UC_URPF_FAIL_ACT,                 /* rtk_l3_act_t */

    RTK_L3_PCT_END                                  /* end of enum */
} rtk_l3_portCtrlType_t;

/* L3 host/route CPU target */
typedef enum rtk_l3_cpuTarget_e
{
    RTK_L3_CPUTARGET_LOCAL = 0,
    RTK_L3_CPUTARGET_MASTER,
    RTK_L3_CPUTARGET_END,
} rtk_l3_cpuTarget_t;

/* L3 global control type (GCT) */
typedef enum rtk_l3_globalCtrlType_e
{
    RTK_L3_GCT_NONE = 0,                                /* Dummy */

    /* IPv4 */
    RTK_L3_GCT_IP_HDR_ERR_ACT,                          /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_GLB_EN,                             /* rtk_enable_t */
    RTK_L3_GCT_IPUC_BAD_SIP_ACT,                        /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_BAD_DIP_ACT,                        /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_ZERO_SIP_ACT,                       /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_DMAC_BC_ACT,                        /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_DMAC_MC_ACT,                        /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_HDR_OPT_ACT,                        /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_MTU_FAIL_ACT,                       /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_TTL_FAIL_ACT,                       /* rtk_l3_act_t */
    RTK_L3_GCT_IPUC_PKT_TO_CPU_TARGET,                  /* rtk_l3_cpuTarget_t */

    /* IPv6 */
    RTK_L3_GCT_IP6_HDR_ERR_ACT,                         /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_GLB_EN,                            /* rtk_enable_t */
    RTK_L3_GCT_IP6UC_BAD_SIP_ACT,                       /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_BAD_DIP_ACT,                       /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_ZERO_SIP_ACT,                      /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT,                 /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_HBH_ACT,                           /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_HBH_ERR_ACT,                       /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT,                     /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_MTU_FAIL_ACT,                      /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_HL_FAIL_ACT,                       /* rtk_l3_act_t */
    RTK_L3_GCT_IP6UC_PKT_TO_CPU_TARGET,                 /* rtk_l3_cpuTarget_t */

    /* ECMP */
    RTK_L3_GCT_ECMP_HASH_KEY,                           /* rtk_l3_ecmpHashKey_t */
    RTK_L3_GCT_ECMP_HASH_UNIVERSAL_ID,                  /* uint32 */
    RTK_L3_GCT_ECMP_HASH_SIP_BIT_OFFSET,                /* uint32 */
    RTK_L3_GCT_ECMP_HASH_DIP_BIT_OFFSET,                /* uint32 */
    RTK_L3_GCT_ECMP_HASH_SPORT_BIT_OFFSET,              /* uint32 */
    RTK_L3_GCT_ECMP_HASH_DPORT_BIT_OFFSET,              /* uint32 */
    RTK_L3_GCT_ECMP_HASH_TBL_MANUAL,                    /* rtk_enable_t */

    /* Routing exception */
    RTK_L3_GCT_NH_ERR_ACT,                              /* rtk_l3_act_t */
    RTK_L3_GCT_NH_AGE_OUT_ACT,                          /* rtk_l3_act_t */
    RTK_L3_GCT_URPF_BASE_SEL,                           /* rtk_l3_urpf_base_t */
    RTK_L3_GCT_NON_IP_ACT,                              /* rtk_l3_act_t */

    /* Misc */
    RTK_L3_GCT_REFCNT_CHK,                              /* rtk_enable_t (reference count check) */

    RTK_L3_GCT_END                                      /* end of enum */
} rtk_l3_globalCtrlType_t;

typedef enum rtk_l3_ipmcKeySel_e
{
    RTK_L3_IPMC_KEY_SEL_VID = 0,
    RTK_L3_IPMC_KEY_SEL_INTF = 1,
} rtk_l3_ipmcKeySel_t;

typedef enum rtk_l3_urpf_base_e
{
    RTK_L3_URPF_BASE_INTF       = 0,    /* interface-based */
    RTK_L3_URPF_BASE_PORT       = 1,    /* port-based */
    RTK_L3_URPF_BASE_END,
} rtk_l3_urpf_base_t;

typedef enum rtk_l3_urpf_mode_e
{
    RTK_L3_URPF_MODE_LOOSE      = 0,    /* uRPF loose mode */
    RTK_L3_URPF_MODE_STRICT     = 1,    /* uRPF strict mode */
    RTK_L3_URPF_MODE_END,
} rtk_l3_urpf_mode_t;

typedef enum rtk_l3_intf_flag_e
{
    RTK_L3_INTF_FLAG_NONE       = 0x00000000,       /* none */
    RTK_L3_INTF_FLAG_WITH_ID    = 0x00000001,       /* specify entry ID by caller */
    RTK_L3_INTF_FLAG_MAC_MANUAL = 0x00000002,       /* (Router) MAC managed by caller */
    RTK_L3_INTF_FLAG_END
} rtk_l3_intf_flag_t;


/* L3 information */
typedef struct rtk_l3_info_s
{
    int32 mac_max;                  /* maximum number of Router MAC the chip supports */
    int32 mac_used;                 /* current number of used Router MAC */
    int32 vrf_max;                  /* maximum number of Virtual routers the chip supports (return 0 if it's not supported) */
    int32 vrf_used;                 /* current number of created virtual routers */
    int32 intf_max;                 /* maximum number of L3 interfaces the chip supports */
    int32 intf_used;                /* current number of created L3 interfaces */
    int32 intf_mtu_max;             /* maximum number of variety of MTU values the chip supports */
    int32 intf_mtu_used;            /* current used entries of MTU table */
    int32 intf_ipv6_mtu_max;        /* maximum number of variety of IPv6 MTU values the chip supports */
    int32 intf_ipv6_mtu_used;       /* current used entries of IPv6 MTU table */
    int32 ecmp_max;                 /* maximum ECMP paths the chip supports */
    int32 ecmp_used;                /* current used entries of ECMP table */
    int32 nexthop_max;              /* maximum nexthop entries the chip supports */
    int32 nexthop_used;             /* current used entries of nexthop table */
    int32 host_ipv4_max;            /* L3 host table size for IPv4 unicast */
    int32 host_ipv4_used;           /* current used entries of L3 host table */
    int32 route_ipv4_max;           /* L3 route table size for IPv4 unicast */
    int32 route_ipv4_used;          /* current used entries of L3 route table */
} rtk_l3_info_t;

/* L3 hash/search key for host/route entry */
typedef struct rtk_l3_key_s
{
    rtk_l3_flag_t       flags;      /* flags */

    rtk_vrf_id_t        vrf_id;     /* virtual router instance */
    rtk_vlan_t          vid;        /* VLAN ID */
    rtk_intf_id_t       intf_id;    /* interface ID */

    union {
        rtk_ip_addr_t   ipv4;       /* (destination) IPv4 address */
        rtk_ipv6_addr_t ipv6;       /* (destination) IPv6 address */
    } ip_addr;

    union {
        rtk_ip_addr_t   ipv4;       /* source IPv4 address */
        rtk_ipv6_addr_t ipv6;       /* source IPv6 address */
    } src_ip_addr;
} rtk_l3_key_t;

/* Layer 3 interface structure */
typedef struct rtk_l3_intf_s
{
    rtk_intf_id_t       intf_id;        /* interface ID */

    rtk_vrf_id_t        vrf_id;         /* VRF ID */
    rtk_vlan_t          vid;            /* VLAN ID */
    rtk_mac_t           mac_addr;       /* MAC address */

    uint32              mtu;            /* MTU */
    uint32              ipv6_mtu;       /* IPv6 MTU */
    uint32              ttl;            /* TTL scope (threshold) */

    rtk_l3_intf_flag_t  flags;          /* interface flags */
} rtk_l3_intf_t;

/* types of counters per L3 interface. */
typedef struct rtk_l3_intf_stats_s
{
    /* RX */
    struct {
        uint64  octets;                 /* Byte count of packets at incoming */
        uint64  unicast_pkts;           /* Packet count of unicast packets at incoming */
        uint64  multicast_pkts;         /* Packet count of multicast packets at incoming */
        uint64  broadcast_pkts;         /* Packet count of broadcast packets at incoming */
        uint64  discards;               /* Packet count of discarded packets at incoming */
    } rx;

    /* TX */
    struct {
        uint64  octets;                 /* Byte count of packets at outgoing */
        uint64  unicast_pkts;           /* Packet count of unicast packets at outgoing */
        uint64  multicast_pkts;         /* Packet count of multicast packets at outgoing */
        uint64  broadcast_pkts;         /* Packet count of broadcast packets at outgoing */
        uint64  discards;               /* Packet count of discarded packets at outgoing */
    } tx;
} rtk_l3_intf_stats_t;

/* L3 nexthop */
typedef struct rtk_l3_nextHop_s
{
    rtk_intf_id_t       intf_id;        /* interface ID (source MAC, destination VID ) */
    rtk_mac_t           mac_addr;       /* destination MAC address */
    #if defined(CONFIG_SDK_RTL9300)
    rtk_mac_t           nh_mac_addr;       /* nexthop destination MAC address, only for 9300 */
    #endif

    rtk_l3_act_t        l3_act;         /* L3 forwarding action:
                                         * RTK_L3_ACT_FORWARD        - keep forwarding
                                         * RTK_L3_ACT_DROP           - NULL interface (drop)
                                         * RTK_L3_ACT_TRAP2CPU       - trap to local CPU
                                         * RTK_L3_ACT_TRAP2MASTERCPU - trap to master CPU
                                         */
} rtk_l3_nextHop_t;

/* rtk_l3_host_act_t */
typedef enum rtk_l3_host_act_e
{
    RTK_L3_HOST_ACT_FORWARD = 0,        /* routing */
    RTK_L3_HOST_ACT_DROP,
    RTK_L3_HOST_ACT_TRAP2CPU,
    RTK_L3_HOST_ACT_COPY2CPU,
    RTK_L3_HOST_ACT_END
} rtk_l3_host_act_t;

/* L3 host entry */
typedef struct rtk_l3_host_s
{
    rtk_l3_flag_t       flags;          /* flags - RTK_L3_FLAG_XXX */

    rtk_vrf_id_t        vrf_id;         /* VRF ID */
    union
    {
        rtk_ip_addr_t   ipv4;           /* IPv4 address */
        rtk_ipv6_addr_t ipv6;           /* IPv6 address */
    } ip_addr;

    /* nexthop information */
    rtk_l3_pathId_t     path_id;        /* path ID (nexthop) */
    rtk_l3_nextHop_t    nexthop;        /* nexthop information */

    /* action */
    rtk_l3_host_act_t   fwd_act;        /* forwarding action */
    rtk_pri_t           qos_pri;        /* internal priority (available when RTK_L3_FLAG_QOS_ASSIGN set) */
} rtk_l3_host_t;

/* rtk_l3_route_act_t */
typedef enum rtk_l3_route_act_e
{
    RTK_L3_ROUTE_ACT_FORWARD = 0,       /* routing */
    RTK_L3_ROUTE_ACT_DROP,
    RTK_L3_ROUTE_ACT_TRAP2CPU,
    RTK_L3_ROUTE_ACT_COPY2CPU,
    RTK_L3_ROUTE_ACT_END
} rtk_l3_route_act_t;

/* L3 route entry */
typedef struct rtk_l3_route_s {
    rtk_l3_flag_t       flags;          /* route flags - RTK_L3_FLAG_XXX */

    rtk_vrf_id_t        vrf_id;         /* VRF ID */
    union
    {
        rtk_ip_addr_t   ipv4;           /* IPv4 address */
        rtk_ipv6_addr_t ipv6;           /* IPv6 address */
    } ip_addr;
    uint32      prefix_len;             /* length of prefix */

    #if defined(CONFIG_SDK_RTL9300)
    uint32      suffix_len;             /* length of suffix. It is used for ECMP application and specify to 0 if ECMP is not desired. */
    #endif

    /* nexthop information */
    rtk_l3_pathId_t     path_id;        /* path ID (nexthop/ECMP) */
    rtk_l3_nextHop_t    nexthop;        /* nexthop information */

    /* action */
    rtk_l3_route_act_t  fwd_act;        /* Forwarding action */
    rtk_pri_t           qos_pri;        /* Internal priority (available when RTK_L3_FLAG_QOS_ASSIGN set) */
} rtk_l3_route_t;

/* rtk_l3_hostTraverseCallbackFunction_t */
typedef int32 (*rtk_l3_hostTraverseCallback_f)(uint32 unit, int32 index, rtk_l3_host_t *pHost, void *pCookie);

/* rtk_l3_routeTraverseCallbackFunction_t */
typedef int32 (*rtk_l3_routeTraverseCallback_f)(uint32 unit, int32 index, rtk_l3_route_t *pRoute, void *pCookie);


/*
 * Data Declaration
 */

/* L3 route entry */
typedef struct rtk_l3_routeEntry_s
{
    rtk_mac_t   hostMac;    /* next hop host mac address */
#if defined(CONFIG_SDK_RTL8390)
    uint32      swMac_idx;  /* index to switch MAC address */
#endif
} rtk_l3_routeEntry_t;


/* L3 router MAC entry */
typedef struct rtk_l3_routerMacEntry_s
{
    rtk_enable_t    enable;     /* enable (valid bit) */

    /* match key */
    uint32          port_type;          /* port type
                                         * 0x0 - individual port
                                         * 0x1 - trunk port
                                         */
    uint32          port_type_mask;     /* care bitmask of port type */
    uint32          port_trunk_id;      /* Port/Trunk ID:
                                         *     Port ID  (if trunk_port is TRUE)
                                         *     Trunk ID (if trunk_port is FALSE)
                                         */
    uint32          port_trunk_id_mask; /* care bitmask of Port/Trunk ID */

    #if defined(CONFIG_SDK_RTL9300)
    rtk_vlan_t      vid;                /* VLAN ID */
    rtk_vlan_t      vid_mask;           /* care bitmask VLAN ID */
    #endif

    #if defined(CONFIG_SDK_RTL9310)
    rtk_intf_id_t   intf_id;            /* L3 interface ID */
    rtk_intf_id_t   intf_id_mask;       /* care bitmask of L3 interface ID */
    #endif

    rtk_mac_t       mac;                /* router MAC */
    rtk_mac_t       mac_mask;           /* care bitmask of router MAC */

    /* action */
    rtk_l3_act_t    l3_act;     /* L3 forwarding action:
                                 * RTK_L3_ACT_FORWARD        - do L3 routing
                                 * RTK_L3_ACT_TRAP2CPU       - trap to local CPU
                                 * RTK_L3_ACT_COPY2CPU       - copy to local CPU and do L3 routing
                                 * RTK_L3_ACT_TRAP2MASTERCPU - trap to master CPU
                                 * RTK_L3_ACT_COPY2MASTERCPU - copy to master CPU and do L3 routing
                                 */
} rtk_l3_routerMacEntry_t;


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_l3_init
 * Description:
 *      Initialize L3 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 * Changes:
 *      None
 */
extern int32
rtk_l3_init(uint32 unit);

/* Module Name    : Layer3 routing                */
/* Sub-module Name: Layer3 routing error handling */

/* Function Name:
 *      rtk_l3_routeEntry_get
 * Description:
 *      Get L3 routing entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 * Output:
 *      pEntry - L3 route entry
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
 *      (1) The route entry is indexed by L2 NextHop entry.
 *          Valid index range:
 *          - 0~2047 in 8390
 *          - 0~511 in 8380
 *      (2) L3 chip architecture be changed for 9300/9310, should reference other API
 * Changes:
 *      None
 */
extern int32
rtk_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

/* Function Name:
 *      rtk_l3_routeEntry_set
 * Description:
 *      Set L3 routing entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 *      pEntry - L3 route entry
 * Output:
 *      None
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
 *      (1) The route entry is indexed by L2 NextHop entry.
 *          Valid index range:
 *          - 0~2047 in 8390
 *          - 0~511 in 8380
 *      (2) L3 chip architecture be changed for 9300/9310, should reference other API
 * Changes:
 *      None
 */
extern int32
rtk_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

/* Function Name:
 *      rtk_l3_routeSwitchMacAddr_get
 * Description:
 *      Get routing switch MAC address.
 * Input:
 *      unit  - unit id
 *      index - index of switch MAC address
 * Output:
 *      pMac  - pointer to switch MAC address
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
 *      (1) Multiple switch MAC addresses are supported for unicast routing.
 *          Valid index value is 0~15 in 8390.
 *      (2) L3 chip architecture be changed for 9300/9310, The API implement should use 'rtk_l3_intf_get'
 * Changes:
 *      None
 */
extern int32
rtk_l3_routeSwitchMacAddr_get(uint32 unit, uint32 index, rtk_mac_t *pMac);

/* Function Name:
 *      rtk_l3_routeSwitchMacAddr_set
 * Description:
 *      Set routing switch MAC address.
 * Input:
 *      unit  - unit id
 *      index - index of switch MAC address
 *      pMac  - pointer to switch MAC address
 * Output:
 *      None
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
 *      (1) Multiple switch MAC addresses are supported for unicast routing.
 *          Valid index value is 0~15 in 8390.
 *      (2) L3 chip architecture be changed for 9300/9310,
 *           The API implement should use 'rtk_l3_intf_create' or 'rtk_l3_intf_set'
 * Changes:
 *      None
 */
extern int32
rtk_l3_routeSwitchMacAddr_set(uint32 unit, uint32 index, rtk_mac_t *pMac);

/* Function Name:
 *      rtk_l3_info_t_init
 * Description:
 *      Initialize a rtk_l3_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to L3 information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_info_t_init(rtk_l3_info_t *pInfo);

/* Function Name:
 *      rtk_l3_info_get
 * Description:
 *      Get L3-related information
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to L3 information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_info_get(uint32 unit, rtk_l3_info_t *pInfo);

/* Function Name:
 *      rtk_l3_routerMacEntry_get
 * Description:
 *      Get an router MAC entry.
 * Input:
 *      unit   - unit id
 *      index  - index of router MAC address
 * Output:
 *      pEntry - router MAC entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_l3_routerMacEntry_get(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

/* Function Name:
 *      rtk_l3_routerMacEntry_set
 * Description:
 *      Set an router MAC entry.
 * Input:
 *      unit   - unit id
 *      index  - index of router MAC address
 *      pEntry - router MAC entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) This API is used to manage Router MAC table by caller.
 *          While using this API to maintain Router MAC table,
 *          to create an L3 interface by calling rtk_l3_intf_create()
 *          SHOULD set with RTK_L3_INTF_FLAG_MAC_MANUAL flag and
 *          DO NOT use rtk_l3_vrrp_add() simultaneously.
 *      (2) MAC entry usage in rtk_l3_info_t which can get by rtk_l3_info_get()
 *          is NOT counted while using this API to add an entry.
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_l3_routerMacEntry_set(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

/* Function Name:
 *      rtk_l3_intf_t_init
 * Description:
 *      Initialize a rtk_l3_intf_t_init structure
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to L3 interface
 * Output:
 *      pIntf - pointer to L3 interface (initialized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_intf_t_init(rtk_l3_intf_t *pIntf);

/* Function Name:
 *      rtk_l3_intf_create
 * Description:
 *      Create a new L3 interface
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to L3 interface containing the input parameters
 * Output:
 *      pIntf - pointer to L3 interface (including all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_VLAN_VID                 - invalid VLAN ID
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 *      RT_ERR_MTU_EXCEED               - interface MTU is too big (over the maximum)
 *      RT_ERR_TTL_EXCEED               - interface TTL is over the maximum
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND     - vlan entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pIntf as input:
 *          mac_addr, vid. (VID=0 means not associated with a VLAN)
 *      (2) Optional: to create an L3 interface with specified interface ID
 *          call with RTK_L3_INTF_FLAG_WITH_ID set and intf_id will be refered.
 *      (3) Optional: to create an L3 interface without allocating a Router MAC entry.
 *          call with RTK_L3_INTF_FLAG_MAC_MANUTL set and
 *          using rtk_l3_routerMac_set() API to manage Router MAC entry for MAC terminate.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_intf_create(uint32 unit, rtk_l3_intf_t *pIntf);

/* Function Name:
 *      rtk_l3_intf_destroy
 * Description:
 *      Destroy an L3 interface
 * Input:
 *      unit   - unit id
 *      intfId - L3 interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_intf_destroy(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      rtk_l3_intf_destroyAll
 * Description:
 *      Destroy all L3 interfaces
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_intf_destroyAll(uint32 unit);

/* Function Name:
 *      rtk_l3_intf_get
 * Description:
 *      Get an L3 interface by interface ID/VID/MAC+VID.
 * Input:
 *      unit  - unit id
 *      type  - search key type
 *      pIntf - pointer to L3 interface (interface id, mac_addr, and vid)
 * Output:
 *      pIntf - pointer to L3 interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable types:
 *          RTK_L3_INTFKEYTYPE_INTF_ID      - identify by interface ID
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_intf_get(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);

/* Function Name:
 *      rtk_l3_intf_set
 * Description:
 *      Set an L3 interface by interface ID/VID/MAC+VID.
 * Input:
 *      unit  - unit id
 *      type  - search key type
 *      pIntf - pointer to L3 interface (interface id, mac_addr, and vid)
 * Output:
 *      pIntf - pointer to L3 interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 *      RT_ERR_MTU_EXCEED               - interface MTU is over the maximum
 *      RT_ERR_TTL_EXCEED               - interface TTL is over the maximum
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND     - vlan entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable types:
 *          RTK_L3_INTFKEYTYPE_INTF_ID      - identify by interface ID
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_intf_set(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);

/* Function Name:
 *      rtk_l3_intfStats_get
 * Description:
 *      Get statistic counters of the specified L3 interface
 * Input:
 *      unit   - unit id
 *      intfId - interface id
 * Output:
 *      pStats - pointer to the statistic data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_l3_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intf_stats_t *pStats);

/* Function Name:
 *      rtk_l3_intfStats_reset
 * Description:
 *      Reset statistic counters of the specified L3 interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_l3_intfStats_reset(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      rtk_l3_vrrp_add
 * Description:
 *      Add a VRRP MAC address to the specified VLAN
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 *      vrid - VRRP ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_EXCEEDS_CAPACITY   - exceed the hardware capacity
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) VRRP MAC address is build as { 00:00:5E:00, flags, VRID }.
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_l3_vrrp_add(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

/* Function Name:
 *      rtk_l3_vrrp_del
 * Description:
 *      Delete a VRRP MAC address from the specified VLAN
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 *      vrid - VRRP ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_l3_vrrp_del(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

/* Function Name:
 *      rtk_l3_vrrp_delAll
 * Description:
 *      Delete all VRRP MAC addresses of the specified VLAN
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_l3_vrrp_delAll(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid);

/* Function Name:
 *      rtk_l3_vrrp_get
 * Description:
 *      Get all VRIDs of the specified VLAN
 * Input:
 *      unit          - unit id
 *      vid           - VLAN ID
 *      vrIdArraySize - size of allocated entries in pVrIdArray
 * Output:
 *      pVrIdArray    - array of VRIDs
 *      pVrIdCount    - number of entries of VRID actually filled in.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_l3_vrrp_get(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrIdArraySize, uint32 *pVrIdArray, uint32 *pVrIdCount);

/* Function Name:
 *      rtk_l3_nextHop_t_init
 * Description:
 *      Initialize a rtk_l3_nextHop_t structure
 * Input:
 *      unit     - unit id
 *      pNextHop - pointer to L3 nexthop info (uninitilized)
 * Output:
 *      pNextHop - pointer to L3 nexthop info (initilized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_nextHop_t_init(rtk_l3_nextHop_t *pNextHop);

/* Function Name:
 *      rtk_l3_nextHop_create
 * Description:
 *      Create an L3 nexthop and get the returned path ID
 * Input:
 *      unit     - unit id
 *      flags    - optional flags (RTK_L3_FLAG_XXX)
 *      pNextHop - pointer to nexthop
 * Output:
 *      pPathId  - pointer to L3 path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_REPLACE
 *          RTK_L3_FLAG_WITH_ID
 *          RTK_L3_FLAG_WITH_NH_DMAC
 *      (2) If the flag RTK_L3_FLAG_REPLACE is set, then replace the existing entry
 *          with the new info based on the input path ID (nhId).
 *          Otherwise, SDK will allocate a path ID for this new nexthop entry.
 *      (3) RTK_L3_FLAG_WITH_NH_DMAC is only supported in 9300. If the flag RTK_L3_FLAG_WITH_NH_DMAC is not set, pNextHop->mac_addr for adding l2 entry and dmac entry;
 *          If the flag RTK_L3_FLAG_WITH_NH_DMAC is set, pNextHop->mac_addr for adding l2 entry,  pNextHop->nh_mac_addr for dmac entry.
 *          In microsoft NLB, Routed packet with unicast DMAC need forward to multiport.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_nextHop_create(uint32 unit, rtk_l3_flag_t flags, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pNhId);

/* Function Name:
 *      rtk_l3_nextHop_destroy
 * Description:
 *      Destroy an L3 Next-Hop
 * Input:
 *      unit   - unit id
 *      pathId - pointer to L3 path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_nextHop_destroy(uint32 unit, rtk_l3_pathId_t nhId);

/* Function Name:
 *      rtk_l3_nextHop_get
 * Description:
 *      Get an L3 Next-Hop by path ID
 * Input:
 *      unit     - unit id
 *      pathId   - L3 path ID
 * Output:
 *      pNextHop - pointer to nexthop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_nextHop_get(uint32 unit, rtk_l3_pathId_t nhId, rtk_l3_nextHop_t *pNextHop);

/* Function Name:
 *      rtk_l3_nextHopPath_find
 * Description:
 *      Find an path ID pointing to a nexthop
 * Input:
 *      unit     - unit id
 *      pNextHop - pointer to nexthop
 * Output:
 *      pPathId  - pointer to L3 path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pNhId);

/* Function Name:
 *      rtk_l3_ecmp_create
 * Description:
 *      Create an ECMP path (contains one or more nexthop paths)
 * Input:
 *      unit       - unit id
 *      flags      - optional flags (REPLACE flag for updating)
 *      pathCnt    - size of the allocated array
 *      pIntfArray - pointer to the path ID array
 *      pEcmpIntf  - pointer to the ECMP path ID
 * Output:
 *      pEcmpIntf  - pointer to the ECMP path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL        - table is full
 *      RT_ERR_EXCEED_CAPACITY - exceed the hardware capacity
 * Applicable:
 *      9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_WITH_ID
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_ecmp_create(uint32 unit, rtk_l3_flag_t flags, uint32 nhIdCnt, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId);

/* Function Name:
 *      rtk_l3_ecmp_destroy
 * Description:
 *      Destroy an ECMP path
 * Input:
 *      unit   - unit id
 *      ecmpId - ECMP path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_ecmp_destroy(uint32 unit, rtk_l3_pathId_t ecmpId);

/* Function Name:
 *      rtk_l3_ecmp_get
 * Description:
 *      Get all path IDs of an ECMP path
 * Input:
 *      unit          - unit id
 *      ecmpPathId    - ECMP path ID
 *      nhIdArraySize - size of allocated entries in pNhIdArray
 * Output:
 *      pNhIdArray    - array of ECMP path IDs
 *      pNhIdCount    - number of entries of nexthop ID actually filled in.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_ecmp_get(uint32 unit, rtk_l3_pathId_t ecmpId, uint32 nhIdArraySize, rtk_l3_pathId_t *pNhIdArray, uint32 *pNhIdCount);

/* Function Name:
 *      rtk_l3_ecmp_add
 * Description:
 *      Add a nexthop in an ECMP entry
 * Input:
 *      unit       - unit id
 *      ecmpPathId - ECMP path ID
 *      pathId     - nexthop path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_EXCEED_HW_CAPACITY - exceed the hardware capacity
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_ecmp_add(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId);

/* Function Name:
 *      rtk_l3_ecmp_del
 * Description:
 *      Delete a nexthop from an ECMP entry
 * Input:
 *      unit   - unit id
 *      ecmpId - ECMP path ID
 *      nhId   - nexthop path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_ecmp_del(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId);

/* Function Name:
 *      rtk_l3_ecmp_find
 * Description:
 *      Find a nexthop pointing to a ECMP path
 * Input:
 *      unit       - unit id
 *      nhIdCount  - number of path IDs in pIntf_array
 *      pNhIdArray - pointer to the path IDs
 * Output:
 *      pEcmpId    - ECMP path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_ecmp_find(uint32 unit, uint32 nhIdCount, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId);

/* Function Name:
 *      rtk_l3_key_t_init
 * Description:
 *      Initialize a rtk_l3_key_t_init structure
 * Input:
 *      unit - unit id
 *      pKey - pointer to L3 key (uninitialized)
 * Output:
 *      pKey - pointer to L3 key (initialized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_key_t_init(rtk_l3_key_t *pKey);

/* Function Name:
 *      rtk_l3_host_t_init
 * Description:
 *      Initialize a rtk_l3_host_t structure
 * Input:
 *      pHost - pointer to host entry info (uninitilized)
 * Output:
 *      pHost - pointer to host entry info (initilized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_host_t_init(rtk_l3_host_t *pHost);

/* Function Name:
 *      rtk_l3_host_add
 * Description:
 *      Add an entry into the L3 host routing table
 * Input:
 *      unit  - unit id
 *      pHost - pointer to rtk_l3_host_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_L3_PATH_ID_INVALID - the path ID (nexthop/ECMP) is invalid
 *      RT_ERR_TBL_FULL           - table is full
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6), and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6            - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_L3_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_L3_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_L3_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_L3_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_L3_FLAG_WITH_NEXTHOP    - assign path with nexthop entry
 *      (3) When pHost->path_id = 0, the pHost->fwd_act can only be RTK_L3_HOST_ACT_DROP
 *          or RTK_L3_HOST_ACT_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_add(uint32 unit, rtk_l3_host_t *pHost);

/* Function Name:
 *      rtk_l3_host_del
 * Description:
 *      Delete an entry from the L3 host routing table
 * Input:
 *      unit  - unit id
 *      pHost - pointer to rtk_l3_host_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_del(uint32 unit, rtk_l3_host_t *pHost);

/* Function Name:
 *      rtk_l3_host_del_byNetwork
 * Description:
 *      Delete L3 host entries based on IP prefix (network)
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to the structure containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_del_byNetwork(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      rtk_l3_host_del_byIntfId
 * Description:
 *      Delete L3 host entries that match or do not match a specified L3 interface number
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 *      (2) Not including ECMP-type host entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_del_byIntfId(uint32 unit, rtk_intf_id_t intfId, rtk_l3_flag_t flags);

/* Function Name:
 *      rtk_l3_host_delAll
 * Description:
 *      Delete all L3 host table entries
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_delAll(uint32 unit, rtk_l3_flag_t flags);

/* Function Name:
 *      rtk_l3_host_find
 * Description:
 *      Find an L3 host entry based on IP address
 * Input:
 *      unit  - unit id
 *      pHost - pointer to the structure containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6        - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set, if the bit set the flag RTK_L3_FLAG_READ_HIT_IGNORE should be clear
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_find(uint32 unit, rtk_l3_host_t *pHost);

/* Function Name:
 *      rtk_l3_hostConflict_get
 * Description:
 *      Return list of conflicts in the L3 table
 * Input:
 *      unit       - unit id
 *      pKey       - IP address to test conflict condition
 *      pHostArray - pointer to the array of conflicting addresses
 *      maxHost    - maximum number of conflicts that may be returned
 * Output:
 *      pHostArray - pointer to the array of conflicting addresses returned
 *      pHostCount - actual number of conflicting addresses returned
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *      (3) pHostArray is input buffer, user should alloc memory firstly,
 *          maxHost is the max entry number can use rtk_switch_deviceInfo_get() to get max_num_of_l3_conflict_host
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_hostConflict_get(uint32 unit, rtk_l3_key_t *pKey, rtk_l3_host_t *pHostArray, int32 maxHost, int32 *pHostCount);

/* Function Name:
 *      rtk_l3_host_age
 * Description:
 *      Run L3 host table aging
 * Input:
 *      unit    - unit id
 *      flags   - control flags (RTK_L3_FLAG_XXX)
 *      fAge    - callback function, NULL if none
 *      pCookie - user data to be passed to callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT     - if set, then clear the hit bits of the traversed entries when
 *                                their hit bits are set.
 *                                Otherwise, delete the unused entries which its hit bit is clear.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_hostTraverseCallback_f fAge, void *pCookie);

/* Function Name:
 *      rtk_l3_host_getNext
 * Description:
 *      Get the next valid L3 host entry (based on the base index)
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 *      pBase - pointer to the start number to be searched
 * Output:
 *      pBase - pointer to the index of the returned entry (-1 means not found)
 *      pHost - L3 host entry (if found)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *      (2) Use base index -1 to indicate to search from the beginging of L3 host table
 *      (3) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_host_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_host_t *pHost);

/* Function Name:
 *      rtk_l3_route_t_init
 * Description:
 *      Initialize a rtk_l3_route_t structure
 * Input:
 *      pRoute - pointer to L3 route entry info (uninitilized)
 * Output:
 *      pRoute - pointer to L3 route entry info (initilized)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_route_t_init(rtk_l3_route_t *pRoute);

/* Function Name:
 *      rtk_l3_route_add
 * Description:
 *      Add an IP route to the L3 route table
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_L3_PATH_ID_INVALID - the path ID (nexthop/ECMP) is invalid
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6), prefix_len and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6            - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_L3_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_L3_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_L3_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_L3_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_L3_FLAG_WITH_NEXTHOP    - assign path with nexthop entry
 *      (3) When pRoute->path_id = 0, the pRoute->fwd_act can only be RTK_L3_ROUTE_ACT_DROP
 *          or RTK_L3_ROUTE_ACT_TRAP2CPU
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_route_add(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      rtk_l3_route_del
 * Description:
 *      Delete an IP route from the L3 route table
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6), prefix_len and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_route_del(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      rtk_l3_route_get
 * Description:
 *      Look up a route given the network and netmask
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t specifying the basic inputs
 * Output:
 *      pRoute - pointer to returned rtk_l3_route_t info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6        - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set, if the bit set the flag RTK_L3_FLAG_READ_HIT_IGNORE should be clear
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_route_get(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      rtk_l3_route_del_byIntfId
 * Description:
 *      Delete routes based on matching or non-matching L3 interface ID
 * Input:
 *      unit   - unit id
 *      flags  - control flags (RTK_L3_FLAG_XXX)
 *      intfId - L3 interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 *      (2) Not including ECMP-type route entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_route_del_byIntfId(uint32 unit, rtk_l3_flag_t flags, rtk_intf_id_t intfId);

/* Function Name:
 *      rtk_l3_route_delAll
 * Description:
 *      Delete all L3 route table entries
 * Input:
 *      unit  - unit id
 *      flags - control flags (RTK_L3_FLAG_XXX)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_route_delAll(uint32 unit, rtk_l3_flag_t flags);

/* Function Name:
 *      rtk_l3_route_age
 * Description:
 *      Run L3 route table aging
 * Input:
 *      unit    - unit id
 *      flags   - control flags (RTK_L3_FLAG_XXX)
 *      fAge    - callback function, NULL if none
 *      pCookie - user data to be passed to callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT     - if set, then clear the hit bits of the traversed entries when
 *                                their hit bits are set.
 *                                Otherwise, delete the unused entries which its hit bit is clear.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_route_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_routeTraverseCallback_f fAge, void *pCookie);

/* Function Name:
 *      rtk_l3_route_getNext
 * Description:
 *      Get the next valid L3 route entry (based on the base index)
 * Input:
 *      unit   - unit id
 *      flags  - control flags (RTK_L3_FLAG_XXX)
 *      pBase  - pointer to the starting index number to be searched
 * Output:
 *      pBase  - pointer to the index of the returned entry (-1 means not found)
 *      pRoute - L3 route entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *      (2) Use base index -1 to indicate to search from the beginging of L3 route table
 *      (3) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_route_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_route_t *pRoute);

/* Function Name:
 *      rtk_l3_globalCtrl_get
 * Description:
 *      Get the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_globalCtrl_get(uint32 unit, rtk_l3_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      rtk_l3_globalCtrl_set
 * Description:
 *      Set the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
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
rtk_l3_globalCtrl_set(uint32 unit, rtk_l3_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      rtk_l3_intfCtrl_get
 * Description:
 *      Get the configuration of the specified control type and interface ID
 * Input:
 *      unit   - unit id
 *      intfId - L3 interface id
 *      type   - control type
 * Output:
 *      pArg   - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_intfCtrl_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 *pArg);

/* Function Name:
 *      rtk_l3_intfCtrl_set
 * Description:
 *      Set the configuration of the specified control type and interface ID
 * Input:
 *      unit   - unit id
 *      intfId - L3 interface id
 *      type   - control type
 *      arg    - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_intfCtrl_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 arg);

/* Function Name:
 *      rtk_l3_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 *pArg);

/* Function Name:
 *      rtk_l3_portCtrl_set
 * Description:
 *      Set the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l3_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 arg);

#endif /* __RTK_L3_H__ */


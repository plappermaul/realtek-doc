/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public L3 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) L3 routing
 *
 */
#ifndef __DAL_MANGO_L3_H__
#define __DAL_MANGO_L3_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/l3.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */
#define MANGO_L3_HOST_IP_PFLEN      (32)
#define MANGO_L3_HOST_IP6_PFLEN     (128)

#define DAL_MANGO_L3_CHK_IDLE_TIMES     (512)   /* maximum check times for busy waiting */

#define DAL_MANGO_L3_RESERVED_INTF_VID          (0)         /* reserved VID of interface which means NOT associated with any VLAN */
#define DAL_MANGO_L3_RESERVED_INTF_IDX          (0)         /* this interface index is reserved for all VLANs as default L2 bridging interface */
#define DAL_MANGO_L3_RESERVED_INTF_MTU_IDX      (0)         /* this MTU entry is reserved for the default interface MTU value */
#define DAL_MANGO_L3_RESERVED_NEXTHOP_IDX       (0)         /* reserved nexthop index */

#define DAL_MANGO_L3_VID_MAX            (RTK_MAX_NUM_OF_VLAN_ID)
#define DAL_MANGO_L3_VRF_MAX            (64)
#define DAL_MANGO_L3_MAC_MAX            (1024)
#define DAL_MANGO_L3_INTF_MAX           (1024)
#define DAL_MANGO_L3_IP_MTU_MAX         (16)
#define DAL_MANGO_L3_IP6_MTU_MAX        (16)
#define DAL_MANGO_L3_NEXTHOP_MAX        (8192)
#define DAL_MANGO_L3_ECMP_MAX           (256)
#define DAL_MANGO_L3_ECMP_NH_HASH_SIZE  (RTK_L3_ECMP_HASH_IDX_MAX)
#define DAL_MANGO_L3_ECMP_NH_CNT_MAX    (RTK_L3_ECMP_NEXTHOP_CNT_MAX)
#define DAL_MANGO_L3_HOST_TBL_NUM       (2)
#define DAL_MANGO_L3_HOST_TBL_HEIGHT    (1024)
#define DAL_MANGO_L3_HOST_TBL_WIDTH     (6)
#define DAL_MANGO_L3_HOST_TBL_SIZE      ((DAL_MANGO_L3_HOST_TBL_NUM)*(DAL_MANGO_L3_HOST_TBL_HEIGHT)*(DAL_MANGO_L3_HOST_TBL_WIDTH))
#if defined(CONFIG_SDK_FPGA_PLATFORM)
#define DAL_MANGO_L3_ROUTE_TBL_HEIGHT   (4)     /* only 4 for FPGA */
#else
#define DAL_MANGO_L3_ROUTE_TBL_HEIGHT   (2048)
#endif
#define DAL_MANGO_L3_ROUTE_TBL_WIDTH    (6)
#define DAL_MANGO_L3_ROUTE_TBL_SIZE     ((DAL_MANGO_L3_ROUTE_TBL_HEIGHT)*(DAL_MANGO_L3_ROUTE_TBL_WIDTH))

#define DAL_MANGO_L3_INTF_ID_ENTRY_IDX_MASK         (0xFFFFU)
#define DAL_MANGO_L3_PATH_ID_ENTRY_IDX_MASK         (0xFFFFU)
#define DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX       (0xFFFCU)   /* invalid DMAC index (L3 tunnel) */
#define DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER   (0xFFFDU)
#define DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2CPU      (0xFFFEU)
#define DAL_MANGO_L3_NEXTHOP_DMAC_IDX_DROP          (0xFFFFU)


#undef MANGO_L3_ROUTE_IPMC_DYNAMIC                  /* dynamic IPMC Route table (default: disabled),
                                                     * only available when MANGO_L3_ROUTE_IPMC_SIZE is disabled (0)
                                                     */

#define MANGO_L3_ROUTE_IPMC_SIZE            (0)     /* IPMC space size (must be 6*N, 0=disable) */

#if MANGO_L3_ROUTE_IPMC_SIZE
#define MANGO_L3_ROUTE_IPMC_HW_IDX_BASE     (0x3000)    // 12288
#define MANGO_L3_ROUTE_IPMC_WIDTH_IPV4      (2)
#define MANGO_L3_ROUTE_IPMC_WIDTH_IPV6      (6)
#endif
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
#define MANGO_L3_ROUTE_IPMC_CAM_IDX_BASE    (0x3000)    // 12288
#endif

#define MANGO_L3_ROUTE_RSVMC_ENTRY          (0)     /* LPM default reserved multicast entry (1: enable / 0: disable) */

#if MANGO_L3_ROUTE_RSVMC_ENTRY
#define MANGO_L3_RSV_RPF_ID                 (4095)  /* reserved RPF ID for reserved multicast */
#endif

#if MANGO_L3_ROUTE_RSVMC_ENTRY
#define DAL_MANGO_L3_ROUTE_TBL_SIZE_FOR_RTL9311E    (756)       /* origin: 768 ; after: 756 reserved for multicast */

#define DAL_MANGO_L3_ROUTE_TBL_RSV_V4_MIN_IDX       (12276)
#define DAL_MANGO_L3_ROUTE_TBL_RSV_V4_MAX_IDX       (12277)
#define DAL_MANGO_L3_ROUTE_TBL_RSV_V6_MIN_IDX       (12282)
#define DAL_MANGO_L3_ROUTE_TBL_RSV_V6_MAX_IDX       (12287)
#else
#define DAL_MANGO_L3_ROUTE_TBL_SIZE_FOR_RTL9311E    (768)
#endif

/* internal API flags */
typedef enum dal_mango_l3_api_flag_e
{
    DAL_MANGO_L3_API_FLAG_NONE          = (0x00000000U),
    DAL_MANGO_L3_API_FLAG_IPV6          = (0x00000001U),    /* IPv6 */
    DAL_MANGO_L3_API_FLAG_MOD_TUNNEL    = (0x00000002U),    /* called by Tunnel module */
    DAL_MANGO_L3_API_FLAG_MOD_MPLS      = (0x00000004U),    /* called by MPLS module */
    DAL_MANGO_L3_API_FLAG_MOD_OPENFLOW  = (0x00000008U),    /* called by Openflow module */
    DAL_MANGO_L3_API_FLAG_WITH_ID       = (0x00000010U),    /* specify entry ID */
    DAL_MANGO_L3_API_FLAG_MODS_MASK     = (DAL_MANGO_L3_API_FLAG_MOD_TUNNEL | DAL_MANGO_L3_API_FLAG_MOD_MPLS | DAL_MANGO_L3_API_FLAG_MOD_OPENFLOW),
} dal_mango_l3_api_flag_t;

/* interface ID flags: coding from bit16, LSB[15:0] are reserved for entry index */
typedef enum dal_mango_l3_intfId_flag_e
{
    DAL_MANGO_L3_INTF_ID_FLAG_NONE      = (0x0000U << 16),  /* nothing (initial value) */
    DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL    = (0x0001U << 16),  /* tunnel interface */
    DAL_MANGO_L3_INTF_ID_FLAG_L2_TUNNEL = (0x0002U << 16),  /* L2 tunnel */
} dal_mango_l3_intfId_flag_t;

/* L3_ENTRY_TYPE */
typedef enum dal_mango_l3_entry_type_e
{
    DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST = 0,   /* 0 */
    DAL_MANGO_L3_ENTRY_TYPE_IPV4_MULTICAST,     /* 1 */
    DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST,       /* 2 */
    DAL_MANGO_L3_ENTRY_TYPE_IPV6_MULTICAST,     /* 3 */
} dal_mango_l3_entry_type_t;

/* path ID flags: coding from bit16, LSB[15:0] are reserved for entry index */
typedef enum dal_mango_l3_pathId_flag_e
{
    DAL_MANGO_L3_PATH_ID_FLAG_ECMP      = (0x0001U << 16),  /* ECMP */
    DAL_MANGO_L3_PATH_ID_FLAG_MPLS      = (0x0002U << 16),  /* MPLS */
} dal_mango_l3_pathId_flag_t;

/* structure of L3 router MAC entry */
typedef struct dal_mango_l3_macEntry_s
{
    uint32      valid;

    uint32      port_type;
    uint32      port_id;
    uint32      intf_id;
    rtk_mac_t   mac;
    uint32      lu_phase;
    uint32      l3_intf;

    uint32      bmsk_port_type;
    uint32      bmsk_port_id;
    uint32      bmsk_intf_id;
    rtk_mac_t   bmsk_mac;
    uint32      bmsk_lu_phase;
    uint32      bmsk_l3_intf;

    uint32      act;
} dal_mango_l3_macEntry_t;

/* structure of L3 ingress interface entry */
typedef struct dal_mango_l3_intfIgrEntry_s
{
    uint32      vrf_id;
    uint32      ipuc_route_en;
    uint32      ip6uc_route_en;
    uint32      ipmc_route_en;
    uint32      ip6mc_route_en;
    uint32      ipmc_route_lu_mis_act;
    uint32      ip6mc_route_lu_mis_act;
    uint32      ipmc_act_224_0_0_x;
    uint32      ipmc_act_224_0_1_x;
    uint32      ipmc_act_239_x_x_x;
    uint32      ip6mc_act_0000_00xx;
    uint32      ip6_mld_act_0_x_x;
    uint32      ip6_mld_act_db8_x_x;
    uint32      ip6_nd_act;
    uint32      ip_urpf_fail_act;
    uint32      ip_urpf_chk_en;
    uint32      ip_urpf_dflt_route_en;
    uint32      ip_urpf_chk_mode;
    uint32      ip6_urpf_fail_act;
    uint32      ip6_urpf_chk_en;
    uint32      ip6_urpf_dflt_route_en;
    uint32      ip6_urpf_chk_mode;
    uint32      mc_key_sel;
} dal_mango_l3_intfIgrEntry_t;

/* structure of L3 egress interface entry */
typedef struct dal_mango_l3_intfEgrEntry_s
{
    uint32      dst_vid;
    rtk_mac_t   smac_addr;
    uint32      ip_mtu_idx;
    uint32      ip6_mtu_idx;
    uint32      ipmc_ttl_scope;
    uint32      ip6mc_hl_scope;
    uint32      ip_icmp_redirect_act;
    uint32      ip6_icmp_redirect_act;
    uint32      ip_pbr_icmp_redirect_act;
    uint32      ip6_pbr_icmp_redirect_act;
    uint32      tunnel_if;
    uint32      tunnel_idx;
} dal_mango_l3_intfEgrEntry_t;

/* structure of L3 interface entry (contains ingress and egress) */
typedef struct dal_mango_l3_intfEntry_s
{
    dal_mango_l3_intfIgrEntry_t igrIntf;
    dal_mango_l3_intfEgrEntry_t egrIntf;
} dal_mango_l3_intfEntry_t;

/* structure of L3 host entry */
typedef struct dal_mango_l3_hostEntry_s
{
    uint32      valid;
    uint32      fmt;                    /* IPUC, IPMC */
    uint32      entry_type;             /* IPUC, IPMC */

    uint32      ipmc_type;              /* IPMC */
    uint32      vrf_id;                 /* IPUC, IPMC */
    rtk_ip_addr_t   ip;                 /* IPUC */
    rtk_ipv6_addr_t ip6;                /* IP6UC */

    rtk_ip_addr_t   sip;                /* IPMC */
    rtk_ipv6_addr_t sip6;               /* IP6MC */
    rtk_ip_addr_t   gip;                /* IPMC */
    rtk_ipv6_addr_t gip6;               /* IP6MC */
    rtk_vlan_t  vid;                    /* IPMC, IP6MC */
    uint32      vid_cmp;                /* IPMC, IP6MC */
    uint32      mc_key_sel;             /* IPMC */

    uint32      dst_null_intf;          /* IPUC */
    uint32      act;                    /* IPUC */
    uint32      ecmp_en;                /* IPUC */
    uint32      nh_ecmp_idx;            /* IPUC */
    uint32      ttl_dec;                /* IPUC */
    uint32      ttl_chk;                /* IPUC */
    uint32      qos_en;                 /* IPUC */
    uint32      qos_pri;                /* IPUC */
    uint32      hit;                    /* IPUC, IPMC */

    uint32      l2_chk_vid;             /* IPMC */
    uint32      l2_en;                  /* IPMC */
    uint32      l2_act;                 /* IPMC */
    uint32      l2_mc_pmsk_idx;         /* IPMC */
    uint32      l2_mc_l2_tnl_lst_valid; /* IPMC */
    uint32      l2_mc_l2_tnl_lst_idx;   /* IPMC */
    uint32      l3_rpf_id;              /* IPMC */
    uint32      l3_en;                  /* IPMC */
    uint32      l3_act;                 /* IPMC */
    uint32      rpf_chk_en;             /* IPMC */
    uint32      rpf_fail_act;           /* IPMC */
    uint32      ttl_min;                /* IPMC */
    uint32      mtu_max;                /* IPMC */
    uint32      oil_idx_valid;          /* IPMC */
    uint32      oil_idx;                /* IPMC */
    uint32      stack_fwd_pmsk;         /* IPMC */
} dal_mango_l3_hostEntry_t;

/* structure of L3 prefix-route entry */
typedef struct dal_mango_l3_routeEntry_s
{
    uint32          valid;
    uint32          fmt;                    /* IPUC, IPMC */
    uint32          bmsk_fmt;
    uint32          entry_type;             /* IPUC, IPMC */
    uint32          bmsk_entry_type;

    uint32          ipmc_type;              /* IPMC */
    uint32          bmsk_ipmc_type;
    uint32          vrf_id;                 /* IPUC, IPMC */
    uint32          bmsk_vrf_id;
    rtk_ip_addr_t   ip;                     /* IPUC */
    rtk_ip_addr_t   bmsk_ip;
    rtk_ipv6_addr_t ip6;                    /* IP6UC */
    rtk_ipv6_addr_t bmsk_ip6;

    rtk_ip_addr_t   sip;                    /* IPMC */
    rtk_ip_addr_t   bmsk_sip;
    rtk_ipv6_addr_t sip6;                   /* IP6MC */
    rtk_ipv6_addr_t bmsk_sip6;
    rtk_ip_addr_t   gip;                    /* IPMC */
    rtk_ip_addr_t   bmsk_gip;
    rtk_ipv6_addr_t gip6;                   /* IP6MC */
    rtk_ipv6_addr_t bmsk_gip6;
    rtk_vlan_t      vid;                    /* IPMC, IP6MC */
    rtk_vlan_t      bmsk_vid;
    uint32          mc_key_sel;             /* IPMC */
    uint32          bmsk_mc_key_sel;

    uint32          host_route;             /* IPUC, IP6UC */
    uint32          dflt_route;             /* IPUC, IP6UC */
    uint32          round;                  /* IPMC, IP6MC */
    uint32          bmsk_round;
    uint32          dst_null_intf;          /* IPUC */
    uint32          act;                    /* IPUC */
    uint32          ecmp_en;                /* IPUC */
    uint32          nh_ecmp_idx;            /* IPUC */
    uint32          ttl_dec;                /* IPUC */
    uint32          ttl_chk;                /* IPUC */
    uint32          qos_en;                 /* IPUC */
    uint32          qos_pri;                /* IPUC */
    uint32          hit;                    /* IPUC, IPMC */

    uint32          vid_cmp;                /* IPMC */
    uint32          l2_chk_vid;             /* IPMC */
    uint32          l2_en;                  /* IPMC */
    uint32          l2_act;                 /* IPMC */
    uint32          l2_mc_pmsk_idx;         /* IPMC */
    uint32          l2_mc_l2_tnl_lst_valid; /* IPMC */
    uint32          l2_mc_l2_tnl_lst_idx;   /* IPMC */
    uint32          l3_rpf_id;              /* IPMC */
    uint32          l3_en;                  /* IPMC */
    uint32          l3_act;                 /* IPMC */
    uint32          rpf_chk_en;             /* IPMC */
    uint32          rpf_fail_act;           /* IPMC */
    uint32          ttl_min;                /* IPMC */
    uint32          mtu_max;                /* IPMC */
    uint32          oil_idx_valid;          /* IPMC */
    uint32          oil_idx;                /* IPMC */
    uint32          stack_fwd_pmsk;         /* IPMC */
} dal_mango_l3_routeEntry_t;

/* structure of L3 nexthop entry */
typedef struct dal_mango_l3_nhEntry_s
{
    uint32  dmac_idx;                   /* Nexthop destination MAC address entry index */
    uint32  l3_egr_intf_idx;            /* L3 egress interface index */
} dal_mango_l3_nhEntry_t;

/* structure of L3 ECMP entry */
typedef struct dal_mango_l3_ecmpEntry_s
{
    uint32  hash_to_nh_id[DAL_MANGO_L3_ECMP_NH_HASH_SIZE];  /* nexthop id for each hash result */
    uint32  nh_idx[DAL_MANGO_L3_ECMP_NH_CNT_MAX];           /* nexthop index array */
    uint32  meter_en;                                       /* meter enable status */
    uint32  meter_idx;                                      /* meter entry index */
} dal_mango_l3_ecmpEntry_t;

/* structure of L3 host hash */
typedef struct dal_mango_l3_hostHashKey_s
{
    uint32  alg_of_tbl[DAL_MANGO_L3_HOST_TBL_NUM];  /* algorithm of tables */

    /* Hash Key */
    rtk_vrf_id_t    vrf_id;         /* VRF ID */
    rtk_vlan_t      vid;            /* VLAN ID */
    union {
        rtk_ip_addr_t   ipv4;       /* (destination) IPv4 address */
        rtk_ipv6_addr_t ipv6;       /* (destination) IPv6 address */
    } dip;
    union {
        rtk_ip_addr_t   ipv4;       /* source IPv4 address */
        rtk_ipv6_addr_t ipv6;       /* source IPv6 address */
    } sip;
} dal_mango_l3_hostHashKey_t;

/* structure of L3 host hash index */
typedef struct dal_mango_l3_hostHashIdx_s
{
    uint32  idx_of_tbl[DAL_MANGO_L3_HOST_TBL_NUM];
} dal_mango_l3_hostHashIdx_t;

/* structure of L3 host allocation */
typedef struct dal_mango_l3_hostAlloc_s
{
    dal_mango_l3_hostHashIdx_t  hashIdx;    /* get the index for each table by calling _dal_mango_l3_hostHashIdx_get() API */
    uint32  width;                          /* width of entry (IPUC: 1, IPMC: 2, IP6UC: 3, IP6MC: 6) */
} dal_mango_l3_hostAlloc_t;

/* structure of L3 route result */
typedef struct dal_mango_l3_ipRouteResult_s
{
    uint32  lookup_hit;
    uint32  match_prefix_len;

    uint32  nh_info_valid;
    uint32  nh_dmac_idx;
    uint32  l3_egr_intf_idx;
    rtk_mac_t   nh_dmac_addr;
    rtk_mac_t   nh_smac_addr;
} dal_mango_l3_ipRouteResult_t;

/* structure of L3 nexthop pathId */
typedef struct dal_mango_l3_pathInfo_s
{
    rtk_intf_id_t   l3_intf_idx;
    rtk_mac_t       l3_intf_mac_addr;

    uint32          nh_dmac_valid;
    uint32          nh_dmac_idx;
    rtk_mac_t       nh_dmac_addr;
} dal_mango_l3_pathInfo_t;


/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define DAL_MANGO_L3_INTF_ID_ENTRY_IDX(_intfId)         ((_intfId) & DAL_MANGO_L3_INTF_ID_ENTRY_IDX_MASK)
#define DAL_MANGO_L3_INTF_ID_IS_L3(_intfId)             (0 == ((_intfId) & (DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL | DAL_MANGO_L3_INTF_ID_FLAG_L2_TUNNEL)))
#define DAL_MANGO_L3_INTF_ID_IS_TUNNEL(_intfId)         ((_intfId) & DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL)
#define DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(_intfId)      ((_intfId) & DAL_MANGO_L3_INTF_ID_FLAG_L2_TUNNEL)

#define DAL_MANGO_L3_INTF_IDX_IS_TNL(_unit, _entryIdx)      (_pL3Db[(_unit)]->intf[(_entryIdx)].intf_id & DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL)
#define DAL_MANGO_L3_INTF_ID_IS_L3_COMMON(_unit, _intfId)   (DAL_MANGO_L3_INTF_ID_IS_L3(_intfId) && !DAL_MANGO_L3_INTF_IDX_IS_TNL((_unit), (_intfId)))
#define DAL_MANGO_L3_INTF_ID_IS_L3_TUNNEL(_unit, _intfId)   (DAL_MANGO_L3_INTF_ID_IS_L3(_intfId) && DAL_MANGO_L3_INTF_IDX_IS_TNL((_unit), (_intfId)))

#define DAL_MANGO_L3_INTF_ID_ENTRY_IDX_TO_L2_TUNNEL(_idx)   ((_idx) | DAL_MANGO_L3_INTF_ID_FLAG_L2_TUNNEL)

#define DAL_MANGO_L3_ENTRY_IDX_TO_L3_INTF_ID(_idx)      (_idx)
#define DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(_idx)            ((((_idx)/6)*8) + ((_idx)%6))
#define DAL_MANGO_L3_ENTRY_ADDR_TO_IDX(_addr)           ((((_addr)/8)*6) + ((_addr)%8))

#define DAL_MANGO_L3_PATH_ID_ENTRY_IDX(_pathId)         ((_pathId) & DAL_MANGO_L3_PATH_ID_ENTRY_IDX_MASK)
#define DAL_MANGO_L3_PATH_ID_IS_NH(_pathId)             (!((_pathId) & (DAL_MANGO_L3_PATH_ID_FLAG_ECMP | DAL_MANGO_L3_PATH_ID_FLAG_MPLS)))
#define DAL_MANGO_L3_PATH_ID_IS_ECMP(_pathId)           ((_pathId) & DAL_MANGO_L3_PATH_ID_FLAG_ECMP)
#define DAL_MANGO_L3_PATH_ID_IS_MPLS(_pathId)           ((_pathId) & DAL_MANGO_L3_PATH_ID_FLAG_MPLS)
#define DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pathId)         ((_pathId) & DAL_MANGO_L3_PATH_ID_ENTRY_IDX_MASK)
#define DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pathId)       ((_pathId) & DAL_MANGO_L3_PATH_ID_ENTRY_IDX_MASK)

#define DAL_MANGO_L3_NH_IDX_TO_PATH_ID(_nhIdx)          ((_nhIdx))
#define DAL_MANGO_L3_NH_IDX_TO_MPLS_PATH_ID(_nhIdx)     ((_nhIdx) | DAL_MANGO_L3_PATH_ID_FLAG_MPLS)
#define DAL_MANGO_L3_ECMP_IDX_TO_PATH_ID(_ecmpIdx)      ((_ecmpIdx) | DAL_MANGO_L3_PATH_ID_FLAG_ECMP)


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_l3Mapper_init
 * Description:
 *      Hook l3 module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook l3 module before calling any l3 APIs.
 */
extern int32
dal_mango_l3Mapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_l3_init
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
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
dal_mango_l3_init(uint32 unit);

/* Function Name:
 *      _dal_mango_l3_intfEntry_alloc
 * Description:
 *      Allocate an interface entry.
 * Input:
 *      unit   - unit id
 *      pIndex - pointer to entry index
 *      flags  - flags of options
 * Output:
 *      pIndex - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_TBL_FULL - table is full
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfEntry_alloc(uint32 unit, uint32 *pIndex, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfEntry_free
 * Description:
 *      Release an interface entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      flags - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfEntry_free(uint32 unit, uint32 index, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfIgrEntry_get
 * Description:
 *      Get the specifed L3 ingress interface entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the interface
 *      flags  - flags of options
 * Output:
 *      pEntry - pointer to interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfIgrEntry_get(uint32 unit, uint32 index, dal_mango_l3_intfIgrEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfIgrEntry_set
 * Description:
 *      Set the specifed L3 ingress interface entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the interface
 *      flags  - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfIgrEntry_set(uint32 unit, uint32 index, dal_mango_l3_intfIgrEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfEgrEntry_get
 * Description:
 *      Get the specifed L3 egress interface entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the interface
 *      flags  - flags of options
 * Output:
 *      pEntry - pointer to interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfEgrEntry_get(uint32 unit, uint32 index, dal_mango_l3_intfEgrEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfEgrEntry_set
 * Description:
 *      Set the specifed L3 egress interface entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the interface
 *      flags  - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfEgrEntry_set(uint32 unit, uint32 index, dal_mango_l3_intfEgrEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfEntry_get
 * Description:
 *      Get the specifed L3 interface entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the interface
 *      flags  - flags of options
 * Output:
 *      pEntry - pointer to interface
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfEntry_get(uint32 unit, uint32 index, dal_mango_l3_intfEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfEntry_set
 * Description:
 *      Set the specifed L3 interface entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the interface
 *      flags  - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_intfEntry_set(uint32 unit, uint32 index, dal_mango_l3_intfEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_intfEntry_alloc
 * Description:
 *      Allocate an MTU entry index for reference.
 * Input:
 *      unit  - unit id
 *      mtu   - MTU value
 *      pIdx  - pointer to entry index
 *      flags - flags of options
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - interface MTU variety is exceed
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_mtuEntry_alloc(uint32 unit, uint32 mtu, uint32 *pIdx, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_mtuEntry_free
 * Description:
 *      Release an MTU entry index reference.
 * Input:
 *      unit  - unit id
 *      idx   - entry index
 *      flags - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_mtuEntry_free(uint32 unit, uint32 idx, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_mtuEntry_realloc
 * Description:
 *      Re-allocate an MTU entry index for reference.
 * Input:
 *      unit   - unit id
 *      newMtu - new MTU value
 *      pIdx   - pointer to current entry index
 *      flags  - flags of options
 * Output:
 *      pIdx   - pointer to new entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - interface MTU variety is exceed
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_mtuEntry_realloc(uint32 unit, uint32 newMtu, uint32 *pIdx, uint32 flags);

/* Function Name:
 *      _dal_mango_l3_nhEntry_alloc
 * Description:
 *      Allocate an L3 nexthop entry.
 * Input:
 *      unit   - unit id
 *      pIndex - pointer to entry index
 *      flags  - flags of options
 * Output:
 *      pIndex - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_TBL_FULL     - table is full
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_nhEntry_alloc(uint32 unit, uint32 *pIndex, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_nextHop_free
 * Description:
 *      Release an L3 nexthop entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      flags - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_nhEntry_free(uint32 unit, uint32 index, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_nhEntry_get
 * Description:
 *      Get an L3 nexthop entry.
 * Input:
 *      unit   - unit id
 *      index  - entry idx
 *      pEntry - pointer to entry
 *      flags  - flags of options
 * Output:
 *      pEntry - pointer to entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_nhEntry_get(uint32 unit, uint32 index, dal_mango_l3_nhEntry_t *pEntry, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_nhEntry_set
 * Description:
 *      Set an L3 nexthop entry.
 * Input:
 *      unit   - unit id
 *      index  - entry idx
 *      pEntry - pointer to entry
 *      flags  - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_nhEntry_set(uint32 unit, uint32 index, dal_mango_l3_nhEntry_t *pEntry, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_nhEntryPathId_get
 * Description:
 *      Get the corresponding path ID of a nexthop entry.
 * Input:
 *      unit    - unit id
 *      index   - entry idx
 *      pPathId - pointer to path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_nhEntryPathId_get(uint32 unit, uint32 index, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      _dal_mango_l3_hostHashIdx_get
 * Description:
 *      Get the hash index by a specified hash key.
 * Input:
 *      unit     - unit id
 *      pHashKey - pointer to hash key
 *      pHashIdx - pointer to hash index
 *      flags    - optional flags
 * Output:
 *      pHashIdx - pointer to hash index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_hostHashIdx_get(uint32 unit, dal_mango_l3_hostHashKey_t *pHashKey, dal_mango_l3_hostHashIdx_t *pHashIdx, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_hostEntry_alloc
 * Description:
 *      Allocate an L3 host entry.
 * Input:
 *      unit   - unit id
 *      pAlloc - pointer to allocation data
 *      pIndex - pointer to entry index
 *      flags  - flags of options
 * Output:
 *      pIndex - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_TBL_FULL     - table is full
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_hostEntry_alloc(uint32 unit, dal_mango_l3_hostAlloc_t *pAlloc, uint32 *pIndex, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_hostEntry_free
 * Description:
 *      Release an L3 host entry.
 * Input:
 *      unit  - unit id
 *      index - pointer to entry index
 *      flags - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_hostEntry_free(uint32 unit, uint32 index, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_hostEntry_get
 * Description:
 *      Get an L3 host entry.
 * Input:
 *      unit   - unit id
 *      index  - pointer to entry index
 *      pEntry - pointer to entry
 *      flags  - flags of options
 * Output:
 *      pEntry - pointer to entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_hostEntry_get(uint32 unit, uint32 index, dal_mango_l3_hostEntry_t *pEntry, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_hostEntry_set
 * Description:
 *      Set an L3 host entry.
 * Input:
 *      unit   - unit id
 *      index  - pointer to entry index
 *      pEntry - pointer to entry
 *      flags  - flags of options
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_hostEntry_set(uint32 unit, uint32 index, dal_mango_l3_hostEntry_t *pEntry, dal_mango_l3_api_flag_t flags);

/* Function Name:
 *      _dal_mango_l3_ipRouteResult_get
 * Description:
 *      Get the result of the unicast IP routing.
 * Input:
 *      unit    - unit id
 *      pKey    - pointer to search key
 * Output:
 *      pResult - pointer to the result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_ipRouteResult_get(uint32 unit, rtk_l3_key_t *pKey, dal_mango_l3_ipRouteResult_t *pResult);

/* Function Name:
 *      _dal_mango_l3_pathInfo_get
 * Description:
 *      Get the information of the specified path ID.
 * Input:
 *      unit      - unit id
 *      pathId    - pointer to search key
 * Output:
 *      pPathInfo - pointer to the result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      (1) Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_mango_l3_pathInfo_get(uint32 unit, rtk_l3_pathId_t pathId, dal_mango_l3_pathInfo_t *pPathInfo);

extern int32
_dal_mango_l3_pathInfo_get_byIdx(uint32 unit, uint32 nhDmacIdx, uint32 l3EgrIntfIdx, dal_mango_l3_pathInfo_t *pPathInfo);


/* Module Name    : Layer3 routing                */
/* Sub-module Name: Layer3 routing error handling */

/* Function Name:
 *      dal_mango_l3_routeEntry_get
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
 * Note:
 *      The route entry is indexed by L2 NextHop entry.
 *      Valid index range:
 *      - 0~2047 in 8390
 *      - 0~511 in 8380
 */
extern int32
dal_mango_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

/* Function Name:
 *      dal_mango_l3_routeEntry_set
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
 * Note:
 *      The route entry is indexed by L2 NextHop entry.
 *      Valid index range:
 *      - 0~2047 in 8390
 *      - 0~511 in 8380
 */
extern int32
dal_mango_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

/* Function Name:
 *      dal_mango_l3_routeSwitchMacAddr_get
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
 * Note:
 *      Multiple switch MAC addresses are supported for unicast routing.
 *      Valid index value is 0~15 in 8390.
 */
extern int32
dal_mango_l3_routeSwitchMacAddr_get(uint32 unit, uint32 index, rtk_mac_t *pMac);

/* Function Name:
 *      dal_mango_l3_routeSwitchMacAddr_set
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
 * Note:
 *      Multiple switch MAC addresses are supported for unicast routing.
 *      Valid index value is 0~15 in 8390.
 */
extern int32
dal_mango_l3_routeSwitchMacAddr_set(uint32 unit, uint32 index, rtk_mac_t *pMac);

/* Function Name:
 *      dal_mango_l3_info_get
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
 * Note:
 */
extern int32
dal_mango_l3_info_get(uint32 unit, rtk_l3_info_t *pInfo);

/* Function Name:
 *      dal_mango_l3_routerMacEntry_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l3_routerMacEntry_get(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

/* Function Name:
 *      dal_mango_l3_routerMacEntry_set
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l3_routerMacEntry_set(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

/* Function Name:
 *      dal_mango_l3_intf_create
 * Description:
 *      Create a new L3 interface
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to L3 interface containing the basic inputs
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
 * Note:
 *      (1) Basic required input parameters of the pIntf as input:
 *          mac_addr, vid. (VID=0 means not associated with a VLAN)
 *      (2) Optional: to create an L3 interface with specified interface ID
 *          call with RTK_L3_INTF_FLAG_WITH_ID set and intf_id will be refered.
 *      (3) Optional: to create an L3 interface without allocating a Router MAC entry.
 *          call with RTK_L3_INTF_FLAG_MAC_MANUTL set and
 *          using rtk_l3_routerMac_set() API to manage Router MAC entry for MAC terminate.
 */
extern int32
dal_mango_l3_intf_create(uint32 unit, rtk_l3_intf_t *pIntf);

/* Function Name:
 *      dal_mango_l3_intf_destroy
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
 * Note:
 */
extern int32
dal_mango_l3_intf_destroy(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      dal_mango_l3_intf_destroyAll
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
 * Note:
 */
extern int32
dal_mango_l3_intf_destroyAll(uint32 unit);

/* Function Name:
 *      dal_mango_l3_intf_get
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
 * Note:
 *      (1) Applicable types:
 *          RTK_L3_INTFKEYTYPE_INTF_ID      - identify by interface ID
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 */
extern int32
dal_mango_l3_intf_get(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);

/* Function Name:
 *      dal_mango_l3_intf_set
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
 * Note:
 *      (1) Applicable types:
 *          RTK_L3_INTFKEYTYPE_INTF_ID      - identify by interface ID
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 */
extern int32
dal_mango_l3_intf_set(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);

/* Function Name:
 *      dal_mango_l3_intfStats_get
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
 * Note:
 *      None
 */
extern int32
dal_mango_l3_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intf_stats_t *pStats);

/* Function Name:
 *      dal_mango_l3_intfStats_reset
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
 * Note:
 *      None
 */
extern int32
dal_mango_l3_intfStats_reset(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      dal_mango_l3_vrrp_add
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
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_EXCEEDS_CAPACITY   - exceed the hardware capacity
 * Note:
 *      (1) VRRP MAC address is build as { 00:00:5E:00, flags , VRID }.
 */
extern int32
dal_mango_l3_vrrp_add(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

/* Function Name:
 *      dal_mango_l3_vrrp_del
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
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      None
 */
extern int32
dal_mango_l3_vrrp_del(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

/* Function Name:
 *      dal_mango_l3_vrrp_delAll
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
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Note:
 *      None
 */
extern int32
dal_mango_l3_vrrp_delAll(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid);

/* Function Name:
 *      dal_mango_l3_vrrp_get
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l3_vrrp_get(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrIdArraySize, uint32 *pVrIdArray, uint32 *pVrIdCount);

/* Function Name:
 *      dal_mango_l3_nextHop_create
 * Description:
 *      Create an L3 nexthop and get the returned path ID
 * Input:
 *      unit     - unit id
 *      flags    - optional flags
 *      pNextHop - pointer to nexthop
 * Output:
 *      pPathId  - pointer to L3 path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_REPLACE
 *      (2) If the flag RTK_L3_FLAG_REPLACE is set, then replace the existing entry
 *          with the new info based on the input path ID (nhId).
 *          Otherwise, SDK will allocate a path ID for this new nexthop entry.
 */
extern int32
dal_mango_l3_nextHop_create(uint32 unit, rtk_l3_flag_t flags, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      dal_mango_l3_nextHop_destroy
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
 * Note:
 */
extern int32
dal_mango_l3_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId);

/* Function Name:
 *      dal_mango_l3_nextHop_get
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
 * Note:
 */
extern int32
dal_mango_l3_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,  rtk_l3_nextHop_t *pNextHop);

/* Function Name:
 *      dal_mango_l3_nextHopPath_find
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
 * Note:
 */
extern int32
dal_mango_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      dal_mango_l3_ecmp_create
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
 * Note:
 */
extern int32
dal_mango_l3_ecmp_create(uint32 unit, rtk_l3_flag_t flags, uint32 nhIdCnt, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      dal_mango_l3_ecmp_destroy
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
 * Note:
 */
extern int32
dal_mango_l3_ecmp_destroy(uint32 unit, rtk_l3_pathId_t pathId);

/* Function Name:
 *      dal_mango_l3_ecmp_get
 * Description:
 *      Get all path IDs of an ECMP path
 * Input:
 *      unit          - unit id
 *      ecmpPathId    - ECMP path ID
 *      nhIdArraySize - size of allocated entries in pIntf_array
 * Output:
 *      pNhIdArray    - array of ECMP path IDs
 *      pNhIdCount    - number of entries of intf_count actually filled in.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
dal_mango_l3_ecmp_get(uint32 unit, rtk_l3_pathId_t pathId, uint32 nhIdArraySize, rtk_l3_pathId_t *pNhIdArray, uint32 *pNhIdCount);

/* Function Name:
 *      dal_mango_l3_ecmp_add
 * Description:
 *      Add a nexthop in an ECMP entry
 * Input:
 *      unit   - unit id
 *      ecmpId - ECMP path ID
 *      nhId   - nexthop path ID
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
 * Note:
 */
extern int32
dal_mango_l3_ecmp_add(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId);

/* Function Name:
 *      dal_mango_l3_ecmp_del
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
 * Note:
 */
extern int32
dal_mango_l3_ecmp_del(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId);

/* Function Name:
 *      dal_mango_l3_ecmp_find
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
 * Note:
 */
extern int32
dal_mango_l3_ecmp_find(uint32 unit, uint32 nhIdCount, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId);

/* Function Name:
 *      dal_mango_l3_host_add
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
 */
extern int32
dal_mango_l3_host_add(uint32 unit, rtk_l3_host_t *pHost);

/* Function Name:
 *      dal_mango_l3_host_del
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
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 */
extern int32
dal_mango_l3_host_del(uint32 unit, rtk_l3_host_t *pHost);

/* Function Name:
 *      dal_mango_l3_host_del_byNetwork
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
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 */
extern int32
dal_mango_l3_host_del_byNetwork(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      dal_mango_l3_host_del_byIntfId
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
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 *      (2) Not including ECMP-type host entries
 */
extern int32
dal_mango_l3_host_del_byIntfId(uint32 unit, rtk_intf_id_t intfId, rtk_l3_flag_t flags);

/* Function Name:
 *      dal_mango_l3_host_delAll
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
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 */
extern int32
dal_mango_l3_host_delAll(uint32 unit, rtk_l3_flag_t flags);

/* Function Name:
 *      dal_mango_l3_host_find
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
 * Note:
 *      (1) Basic required input parameters of the pHost as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6        - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
extern int32
dal_mango_l3_host_find(uint32 unit, rtk_l3_host_t *pHost);

/* Function Name:
 *      dal_mango_l3_hostConflict_get
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
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 */
extern int32
dal_mango_l3_hostConflict_get(uint32 unit, rtk_l3_key_t *pKey, rtk_l3_host_t *pHostArray, int32 maxHost, int32 *pHostCount);

/* Function Name:
 *      dal_mango_l3_host_age
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
 * Note:
 *          RTK_L3_FLAG_HIT     - if set, then clear the hit bits of the traversed entries when
 *                                their hit bits are set.
 *                                Otherwise, delete the unused entries which its hit bit is clear.
 */
extern int32
dal_mango_l3_host_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_hostTraverseCallback_f fAge, void *pCookie);

/* Function Name:
 *      dal_mango_l3_host_getNext
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
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *      (2) Use base index -1 to indicate to search from the beginging of L3 host table
 *      (3) If the returned index is -1, means not found the next valid entry
 */
extern int32
dal_mango_l3_host_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_host_t *pHost);

/* Function Name:
 *      dal_mango_l3_route_add
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
 */
extern int32
dal_mango_l3_route_add(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      dal_mango_l3_route_del
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
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6), prefix_len and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 */
extern int32
dal_mango_l3_route_del(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      dal_mango_l3_route_get
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
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 *      (2) Applicable flags:
 *          RTK_L3_FLAG_IPV6        - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
extern int32
dal_mango_l3_route_get(uint32 unit, rtk_l3_route_t *pRoute);

/* Function Name:
 *      dal_mango_l3_route_del_byIntfId
 * Description:
 *      Delete routes based on matching or non-matching L3 interface ID
 * Input:
 *      unit   - unit id
 *      flags  - control flags
 *      intfId - L3 interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 *      (2) Not including ECMP-type route entries
 */
extern int32
dal_mango_l3_route_del_byIntfId(uint32 unit, rtk_l3_flag_t flags, rtk_intf_id_t intfId);

/* Function Name:
 *      dal_mango_l3_route_delAll
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
 * Note:
 */
extern int32
dal_mango_l3_route_delAll(uint32 unit, rtk_l3_flag_t flags);

/* Function Name:
 *      dal_mango_l3_route_age
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
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_HIT     - if set, then clear the hit bits of the traversed entries when
 *                                their hit bits are set.
 *                                Otherwise, delete the unused entries which its hit bit is clear.
 */
extern int32
dal_mango_l3_route_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_routeTraverseCallback_f fAge, void *pCookie);

/* Function Name:
 *      dal_mango_l3_route_getNext
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
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *      (2) Use base index -1 to indicate to search from the beginging of L3 route table
 *      (3) If the returned index is -1, means not found the next valid entry
 */
extern int32
dal_mango_l3_route_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_route_t *pRoute);

#if MANGO_L3_ROUTE_RSVMC_ENTRY
/* Function Name:
 *      dal_mango_l3_route_rsv4Mcast_add
 * Description:
 *      Add a default v4 multicast route to the L3 route table
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_ENTRY_EXIST        - entry already exist
 * Note:
 *      (1) add class D default entry
 */
extern int32
dal_mango_l3_route_rsv4Mcast_add(uint32 unit);

/* Function Name:
 *      dal_mango_l3_route_rsv6Mcast_add
 * Description:
 *      Add a default v6 multicast route to the L3 route table
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_ENTRY_EXIST        - entry already exist
 * Note:
 *      (1) add ff00::/8 default entry
 */
extern int32
dal_mango_l3_route_rsv6Mcast_add(uint32 unit);
#endif

#if MANGO_L3_ROUTE_IPMC_SIZE
/* Function Name:
 *      _dal_mango_l3_route_ipmcSize_get
 * Description:
 *      Get IPMC space size of L3 Route table
 * Input:
 *      unit    - unit id
 * Output:
 *      pBase   - index base
 *      pSize   - entry size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
_dal_mango_l3_route_ipmcSize_get(uint32 unit, uint32 *pBase, uint32 *pSize);

extern int32 __dal_mango_l3_routeEntry_get(uint32 unit, uint32 index, dal_mango_l3_routeEntry_t *pEntry, rtk_enable_t idxChk);
extern int32 __dal_mango_l3_routeEntry_set(uint32 unit, uint32 index, dal_mango_l3_routeEntry_t *pEntry, rtk_enable_t idxChk);
#endif

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
extern int32 __dal_mango_l3_routeEntry_get(uint32 unit, uint32 index, dal_mango_l3_routeEntry_t *pEntry, rtk_enable_t idxChk);
extern int32 __dal_mango_l3_routeEntry_set(uint32 unit, uint32 index, dal_mango_l3_routeEntry_t *pEntry, rtk_enable_t idxChk);
extern int32 __dal_mango_l3_routeIpmcEntry_alloc(uint32 unit, uint32 ipv6, uint32 *pIndex, void *pEntry);
extern int32 __dal_mango_l3_routeIpmcEntry_free(uint32 unit, uint32 index);
#endif


/* Function Name:
 *      dal_mango_l3_globalCtrl_get
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
 * Note:
 */
extern int32
dal_mango_l3_globalCtrl_get(uint32 unit, rtk_l3_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_mango_l3_globalCtrl_set
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
 * Note:
 */
extern int32
dal_mango_l3_globalCtrl_set(uint32 unit, rtk_l3_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_mango_l3_intfCtrl_get
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
 * Note:
 */
extern int32
dal_mango_l3_intfCtrl_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_mango_l3_intfCtrl_set
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
 * Note:
 */
extern int32
dal_mango_l3_intfCtrl_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_mango_l3_portCtrl_get
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
 * Note:
 */
extern int32
dal_mango_l3_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_mango_l3_portCtrl_set
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
 * Note:
 */
extern int32
dal_mango_l3_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 arg);

#endif /* __RTK_L3_H__ */


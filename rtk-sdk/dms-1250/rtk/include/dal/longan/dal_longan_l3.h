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
#ifndef __DAL_LONGAN_L3_H__
#define __DAL_LONGAN_L3_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/l3.h>
#include <dal/dal_mapper.h>
#include <hal/common/halctrl.h>
#include <hwp/hw_profile.h>


/*
 * Symbol Definition
 */
#define DEFLT_DAL_LONGAN_L3_ICMP_REDIRECT_ACT   RTK_L3_ACT_FORWARD

#define DAL_LONGAN_L3_RESERVED_INTF_MTU_IDX     (0)         /* this MTU entry is reserved for the default interface */

#define LONGAN_L3_HOST_IP_PFLEN          (32)
#define LONGAN_L3_HOST_IP6_PFLEN         (128)

#define DAL_LONGAN_L3_CHK_IDLE_TIMES      (512)   /* maximum check times for busy waiting */

#define DAL_LONGAN_L3_VID_MAX            (4096) /* may use VLAN module's info or call table_size_get() API to get */
#define DAL_LONGAN_L3_MAC_MAX            (64)
#define DAL_LONGAN_L3_INTF_MAX           (128)
#define DAL_LONGAN_L3_IP_MTU_MAX         (8)
#define DAL_LONGAN_L3_IP6_MTU_MAX        (8)
#define DAL_LONGAN_L3_NEXTHOP_MAX        (2048)
#define DAL_LONGAN_L3_SMAC_START_IDX     (2048)
#define DAL_LONGAN_L3_SMAC_IDX(index)     ((DAL_LONGAN_L3_SMAC_START_IDX)+(index))
#define DAL_LONGAN_L3_HOST_TBL_NUM       (2)
#define DAL_LONGAN_L3_HOST_TBL_HEIGHT    (512)
#define DAL_LONGAN_L3_HOST_TBL_WIDTH     (6)
#define DAL_LONGAN_L3_HOST_TBL_SIZE      ((DAL_LONGAN_L3_HOST_TBL_NUM)*(DAL_LONGAN_L3_HOST_TBL_HEIGHT)*(DAL_LONGAN_L3_HOST_TBL_WIDTH))
#define DAL_LONGAN_L3_ROUTE_TBL_HEIGHT   (64)
#define DAL_LONGAN_L3_ROUTE_TBL_WIDTH    (8)
#define DAL_LONGAN_L3_ROUTE_TBL_SIZE     ((DAL_LONGAN_L3_ROUTE_TBL_HEIGHT)*(DAL_LONGAN_L3_ROUTE_TBL_WIDTH))
#define DAL_LONGAN_L3_TBL_SIZE           ((DAL_LONGAN_L3_HOST_TBL_SIZE)+(DAL_LONGAN_L3_ROUTE_TBL_SIZE))

#define DAL_LONGAN_L3_INTF_ID_ENTRY_IDX_MASK         (0x7FU)
#define DAL_LONGAN_L3_PATH_ID_ENTRY_IDX_MASK         (0x07FFU)

#define LONGAN_L3_HOST_IP_PFLEN      (32)
#define LONGAN_L3_HOST_IP6_PFLEN     (128)

#define DAL_LONGAN_L3_CHK_IDLE_TIMES         (512)   /* maximum check times for busy waiting */
#define DAL_LONGAN_L3_ENTRY_READ_RETRY       (1000)
#define DAL_LONGAN_L3_TIMEOUT_TIMES          (3)

#define DAL_LONGAN_NULLINTF_DROP_NEXTHOP_IDX             (DAL_LONGAN_L3_NEXTHOP_MAX - 3)
#define DAL_LONGAN_NULLINTF_TRAP2CPU_NEXTHOP_IDX         (DAL_LONGAN_L3_NEXTHOP_MAX - 2)
#define DAL_LONGAN_NULLINTF_TRAP2MASTER_NEXTHOP_IDX      (DAL_LONGAN_L3_NEXTHOP_MAX - 1)
#define DAL_LONGAN_INVALID_NEXTHOP_IDX                   (0)

#define DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER   (0x7FFDU)
#define DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_TRAP2CPU      (0x7FFEU)
#define DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_DROP          (0x7FFFU)

#define LONGAN_L3_ROUTE_IPMC_SIZE            (0)     /* IPMC space size (must be 8*N, 0=disable) */

#if LONGAN_L3_ROUTE_IPMC_SIZE
#define LONGAN_L3_ROUTE_IPMC_HW_IDX_BASE     (0x200)    // 512
#define LONGAN_L3_ROUTE_IPMC_WIDTH_IPV4      (2)
#define LONGAN_L3_ROUTE_IPMC_WIDTH_IPV6      (8)
#endif

#define LONGAN_L3_ROUTE_RSVMC_ENTRY          (0)     /* LPM default reserved multicast entry (1: enable / 0: disable) */

#if LONGAN_L3_ROUTE_RSVMC_ENTRY
#define LONGAN_L3_RSV_RPF_ID                 (4095)  /* reserved RPF ID for reserved multicast */
#define DAL_LONGAN_L3_ROUTE_TBL_RSV_V4_MIN_IDX       (496)
#define DAL_LONGAN_L3_ROUTE_TBL_RSV_V4_MAX_IDX       (497)
#define DAL_LONGAN_L3_ROUTE_TBL_RSV_V6_MIN_IDX       (504)
#define DAL_LONGAN_L3_ROUTE_TBL_RSV_V6_MAX_IDX       (511)
#endif

/* internal API flags */
typedef enum dal_longan_l3_api_flag_e
{
    DAL_LONGAN_L3_API_FLAG_NONE          = (0x00000000U),
    DAL_LONGAN_L3_API_FLAG_IPV6          = (0x00000001U),    /* IPv6 */
    DAL_LONGAN_L3_API_FLAG_WITH_ID       = (0x00000002U),    /* specify entry ID */
} dal_longan_l3_api_flag_t;

typedef enum dal_longan_l3_rtmac_key_e
{
    DAL_LONGAN_L3_RTMAC_KEY_MAC_VID = 0,      /* VID, MAC */
    DAL_LONGAN_L3_RTMAC_KEY_MAC,                /* MAC */
} dal_longan_l3_rtmac_key_t;

typedef enum dal_longan_l3_pathId_findMethod_e
{
    DAL_LONGAN_L3_PATHID_METHOD_ASIC = 0,      /* ASIC find */
    DAL_LONGAN_L3_PATHID_METHOD_SW,                /* Software traverse */
} dal_longan_l3_pathId_findMethod_t;

typedef enum dal_longan_l3_alg_e
{
    DAL_LONGAN_L3_ALG0 = 0,
    DAL_LONGAN_L3_ALG1,
} dal_longan_l3_alg_t;


/* structure of L3 router MAC entry */
typedef struct dal_longan_l3_macEntry_s
{
    uint32      valid;

    uint32      port_type;
    uint32      port_id;
    uint32      intf_id;
    rtk_mac_t   mac;
    uint32      l3_intf;

    uint32      bmsk_port_type;
    uint32      bmsk_port_id;
    uint32      bmsk_intf_id;
    rtk_mac_t   bmsk_mac;
    uint32      bmsk_l3_intf;

    uint32      act;
} dal_longan_l3_macEntry_t;

/* structure of L3 egress interface entry */
typedef struct dal_longan_l3_intfEgrEntry_s
{
    uint32      dst_vid;
    rtk_mac_t   smac_addr;  //9300 get the smac_idx, then to find the smac_addr
    uint32      smac_idx;
    uint32      ip_mtu_idx;
    uint32      ip6_mtu_idx;
    uint32      ipmc_ttl_scope;
    uint32      ip6mc_hl_scope;
    uint32      ip_icmp_redirect_act;
    uint32      ip6_icmp_redirect_act;
    uint32      ip_pbr_icmp_redirect_act;
    uint32      ip6_pbr_icmp_redirect_act;
} dal_longan_l3_intfEgrEntry_t;

/* structure of L3 interface entry (contains ingress and egress) */
typedef struct dal_longan_l3_intfEntry_s
{
    dal_longan_l3_intfEgrEntry_t egrIntf;
} dal_longan_l3_intfEntry_t;

/* structure of L3 host entry */
typedef struct dal_longan_l3_hostEntry_s
{
    uint32      valid;
    uint32      entry_type;             /* IPUC, IPMC */
    uint32      ipmc_type;              /* IPMC */
    uint32      vrf_id;                 /* IPUC, IPMC */ //not used always = 0
    rtk_ip_addr_t   ip;                 /* IPUC */
    rtk_ipv6_addr_t ip6;                /* IP6UC */

    rtk_ip_addr_t   sip;                /* IPMC */
    rtk_ipv6_addr_t sip6;               /* IP6MC */
    rtk_ip_addr_t   gip;                /* IPMC */
    rtk_ipv6_addr_t gip6;               /* IP6MC */
    rtk_vlan_t  vid;                    /* IPMC, IP6MC */

    uint32      dst_null_intf;          /* IPUC */
    uint32      act;                    /* IPUC */
    uint32      nh_ecmp_idx;            /* IPUC */
    uint32      ttl_dec;                /* IPUC */
    uint32      ttl_chk;                /* IPUC */
    uint32      qos_en;                 /* IPUC */
    uint32      qos_pri;                /* IPUC */
    uint32      hit;                    /* IPUC, IPMC */

    uint32      vid_cmp;                /* IPMC */
    uint32      l2_en;                  /* IPMC */
    uint32      l2_act;                 /* IPMC */
    uint32      l2_mc_pmsk_idx;         /* IPMC */

    uint32      l3_en;                  /* IPMC */
    uint32      l3_act;                 /* IPMC */
    uint32      ttl_min;                /* IPMC */
    uint32      mtu_max_idx;            /* IPMC */
    uint32      rpf_chk_en;             /* IPMC */
    uint32      rpf_fail_act;           /* IPMC */
    uint32      oil_idx;                /* IPMC */
    uint32      oil_idx_valid;          /* IPMC */
    uint32      stack_fwd_pmsk;         /* IPMC */
} dal_longan_l3_hostEntry_t;

/* structure of L3 prefix-route entry */
typedef struct dal_longan_l3_routeEntry_s
{
    uint32          valid;
    uint32          fmt;                    /* IPUC, IPMC */
    uint32          bmsk_fmt;
    uint32          entry_type;             /* IPUC, IPMC */
    uint32          bmsk_entry_type;
    uint32          ipmc_type;              /* IPMC */
    uint32          bmsk_ipmc_type;
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

    uint32          host_route;             /* IPUC, IP6UC */
    uint32          dflt_route;             /* IPUC, IP6UC */
    uint32          round;                  /* IPMC, IP6MC */
    uint32          bmsk_round;
    uint32          dst_null_intf;          /* IPUC */
    uint32          act;                    /* IPUC */
    uint32          nh_ecmp_idx;            /* IPUC */
    uint32          ttl_dec;                /* IPUC */
    uint32          ttl_chk;                /* IPUC */
    uint32          qos_en;                 /* IPUC */
    uint32          qos_pri;                /* IPUC */
    uint32          hit;                    /* IPUC, IPMC */

    uint32          vid_cmp;                /* IPMC */
    uint32          l2_en;                  /* IPMC */
    uint32          l2_act;                 /* IPMC */
    uint32          ttl_min;                /* IPMC */
    uint32          mtu_max_idx;            /* IPMC */
    uint32          l2_mc_pmsk_idx;         /* IPMC */

    uint32          l3_en;                  /* IPMC */
    uint32          l3_act;                 /* IPMC */
    uint32          rpf_chk_en;             /* IPMC */
    uint32          rpf_fail_act;           /* IPMC */
    uint32          oil_idx;                /* IPMC */
    uint32          oil_idx_valid;          /* IPMC */
    uint32          stack_fwd_pmsk;         /* IPMC */
} dal_longan_l3_routeEntry_t;

/* structure of L3 nexthop entry */
typedef struct dal_longan_l3_nhEntry_s
{
    uint32  dmac_idx;                   /* Nexthop destination MAC address entry index */
    uint32  l3_egr_intf_idx;            /* L3 egress interface index */
} dal_longan_l3_nhEntry_t;

/* structure of L3 host hash */
typedef struct dal_longan_l3_hostHashKey_s
{
    uint32  alg_of_tbl[DAL_LONGAN_L3_HOST_TBL_NUM];  /* algorithm of tables */

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
} dal_longan_l3_hostHashKey_t;

/* structure of L3 host hash index */
typedef struct dal_longan_l3_hostHashIdx_s
{
    uint32  idx_of_tbl[DAL_LONGAN_L3_HOST_TBL_NUM];
} dal_longan_l3_hostHashIdx_t;


/* structure of L3 host allocation */
typedef struct dal_longan_l3_hostAlloc_s
{
    dal_longan_l3_hostHashIdx_t  hashIdx;    /* get the index for each table by calling _dal_longan_l3_hostHashIdx_get() API */
    uint32  width;                          /* width of entry (IPUC: 1, IPMC: 2, IP6UC: 3, IP6MC: 6) */
} dal_longan_l3_hostAlloc_t;


/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define DAL_LONGAN_L3_INTF_ID_ENTRY_IDX(_intfId)     ((_intfId) & DAL_LONGAN_L3_INTF_ID_ENTRY_IDX_MASK)
#define DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(_idx)        ((((_idx)/6)*8) + ((_idx)%6))
#define DAL_LONGAN_L3_ENTRY_ADDR_TO_IDX(_addr)       ((((_addr)/8)*6) + ((_addr)%8))

#define DAL_LONGAN_L3_PATH_ID_ENTRY_IDX(_pathId)     ((_pathId) & DAL_LONGAN_L3_PATH_ID_ENTRY_IDX_MASK)
#define DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(_pathId)     ((_pathId) & DAL_LONGAN_L3_PATH_ID_ENTRY_IDX_MASK)
#define DAL_LONGAN_L3_PATH_ID_TO_ECMP_IDX(_pathId)   ((_pathId) & DAL_LONGAN_L3_PATH_ID_ENTRY_IDX_MASK)
#define DAL_LONGAN_L3_NH_IDX_TO_PATH_ID(_nhIdx)      ((_nhIdx))


/* Function Name:
 *      dal_longan_l3Mapper_init
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
dal_longan_l3Mapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_l3_init
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
dal_longan_l3_init(uint32 unit);

/* Function Name:
 *      _dal_longan_l3_intfEntry_alloc
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
_dal_longan_l3_intfEntry_alloc(uint32 unit, uint32 *pIndex, uint32 flags);

/* Function Name:
 *      _dal_longan_l3_intfEntry_free
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
_dal_longan_l3_intfEntry_free(uint32 unit, uint32 index, uint32 flags);

/* Function Name:
 *      _dal_longan_l3_intfEgrEntry_get
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
_dal_longan_l3_intfEgrEntry_get(uint32 unit, uint32 index, dal_longan_l3_intfEgrEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_longan_l3_intfEgrEntry_set
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
_dal_longan_l3_intfEgrEntry_set(uint32 unit, uint32 index, dal_longan_l3_intfEgrEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_longan_l3_intfEntry_get
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
_dal_longan_l3_intfEntry_get(uint32 unit, uint32 index, dal_longan_l3_intfEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_longan_l3_intfEntry_set
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
_dal_longan_l3_intfEntry_set(uint32 unit, uint32 index, dal_longan_l3_intfEntry_t *pEntry, uint32 flags);

/* Function Name:
 *      _dal_longan_l3_intf_mtuEntry_alloc
 * Description:
 *      Allocate an MTU entry index for intface reference.
 * Input:
 *      unit  - unit id
 *      mtu   - MTU value
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_longan_l3_intf_mtuEntry_alloc(uint32 unit, uint32 mtu, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_intf_mtuIp6Entry_alloc
 * Description:
 *      Allocate an Ipv6 MTU entry index for intface reference.
 * Input:
 *      unit  - unit id
 *      mtu   - MTU value
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_intf_mtuIp6Entry_alloc(uint32 unit, uint32 mtu, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_ipmc_mtuEntry_alloc
 * Description:
 *      Allocate an MTU entry index for ipmc address reference.
 * Input:
 *      unit  - unit id
 *      mtu   - MTU value
 *      group   - ipmc addr entry group
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_longan_l3_ipmc_mtuEntry_alloc(uint32 unit, uint32 mtu, rtk_mcast_group_t group, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_ipmc_mtuIp6Entry_alloc
 * Description:
 *      Allocate an IPv6 MTU entry index for ipmc address reference.
 * Input:
 *      unit  - unit id
 *      mtu   - MTU value
 *      group   - ipmc addr entry group
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_longan_l3_ipmc_mtuIp6Entry_alloc(uint32 unit, uint32 mtu, rtk_mcast_group_t group, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_intf_mtuEntry_free
 * Description:
 *      Release an MTU entry index reference.
 * Input:
 *      unit  - unit id
 *      idx   - entry index
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
_dal_longan_l3_intf_mtuEntry_free(uint32 unit, uint32 idx);

/* Function Name:
 *      _dal_longan_l3_intf_mtuIp6Entry_free
 * Description:
 *      Release an IPv6 MTU entry index reference.
 * Input:
 *      unit  - unit id
 *      idx   - entry index
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
_dal_longan_l3_intf_mtuIp6Entry_free(uint32 unit, uint32 idx);

/* Function Name:
 *      _dal_longan_l3_ipmc_mtuEntry_free
 * Description:
 *      Release an MTU entry index reference.
 * Input:
 *      unit  - unit id
 *      idx   - entry index
 *      group   - ipmc addr entry group
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
_dal_longan_l3_ipmc_mtuEntry_free(uint32 unit, uint32 idx, rtk_mcast_group_t group);

/* Function Name:
 *      _dal_longan_l3_ipmc_mtuIp6Entry_free
 * Description:
 *      Release an IPv6 MTU entry index reference.
 * Input:
 *      unit  - unit id
 *      idx   - entry index
 *      group   - ipmc addr entry group
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
_dal_longan_l3_ipmc_mtuIp6Entry_free(uint32 unit, uint32 idx, rtk_mcast_group_t group);

/* Function Name:
 *      _dal_longan_l3_intf_mtuEntry_realloc
 * Description:
 *      Re-allocate an MTU entry index for reference.
 * Input:
 *      unit   - unit id
 *      newMtu - new MTU value
 *      pIdx   - pointer to current entry index
 * Output:
 *      pIdx   - pointer to new entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_longan_l3_intf_mtuEntry_realloc(uint32 unit, uint32 newMtu, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_intf_mtuIp6Entry_realloc
 * Description:
 *      Re-allocate an IPv6 MTU entry index for reference.
 * Input:
 *      unit   - unit id
 *      newMtu - new MTU value
 *      pIdx   - pointer to current entry index
 * Output:
 *      pIdx   - pointer to new entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_longan_l3_intf_mtuIp6Entry_realloc(uint32 unit, uint32 newMtu, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_ipmc_mtuEntry_realloc
 * Description:
 *      Re-allocate an MTU entry index for reference.
 * Input:
 *      unit   - unit id
 *      newMtu - new MTU value
 *      group   - ipmc addr entry group
 *      pIdx   - pointer to current entry index
 * Output:
 *      pIdx   - pointer to new entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_longan_l3_ipmc_mtuEntry_realloc(uint32 unit, uint32 newMtu, rtk_mcast_group_t group, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_ipmc_mtuIp6Entry_realloc
 * Description:
 *      Re-allocate an IPv6 MTU entry index for reference.
 * Input:
 *      unit   - unit id
 *      newMtu - new MTU value
 *      group   - ipmc addr entry group
 *      pIdx   - pointer to current entry index
 * Output:
 *      pIdx   - pointer to new entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - MTU index is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
_dal_longan_l3_ipmc_mtuIp6Entry_realloc(uint32 unit, uint32 newMtu, rtk_mcast_group_t group, uint32 *pIdx);

/* Function Name:
 *      _dal_longan_l3_nhEntry_alloc
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
_dal_longan_l3_nhEntry_alloc(uint32 unit, uint32 *pIndex, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_nextHop_free
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
_dal_longan_l3_nhEntry_free(uint32 unit, uint32 index, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_nhEntry_get
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
_dal_longan_l3_nhEntry_get(uint32 unit, uint32 index, dal_longan_l3_nhEntry_t *pEntry, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_nhEntry_set
 * Description:
 *      Get an L3 nexthop entry.
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
_dal_longan_l3_nhEntry_set(uint32 unit, uint32 index, dal_longan_l3_nhEntry_t *pEntry, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_nhEntryPathId_get
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
_dal_longan_l3_nhEntryPathId_get(uint32 unit, uint32 index, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      _dal_longan_l3_hostHashIdx_get
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
_dal_longan_l3_hostHashIdx_get(uint32 unit, dal_longan_l3_hostHashKey_t *pHashKey, dal_longan_l3_hostHashIdx_t *pHashIdx, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_hostEntry_alloc
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
_dal_longan_l3_hostEntry_alloc(uint32 unit, dal_longan_l3_hostAlloc_t *pAlloc, uint32 *pIndex, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_hostEntry_free
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
_dal_longan_l3_hostEntry_free(uint32 unit, uint32 index, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_hostEntry_get
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
_dal_longan_l3_hostEntry_get(uint32 unit, uint32 index, dal_longan_l3_hostEntry_t *pEntry, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_hostEntry_set
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
_dal_longan_l3_hostEntry_set(uint32 unit, uint32 index, dal_longan_l3_hostEntry_t *pEntry, dal_longan_l3_api_flag_t flags);

/* Function Name:
 *      _dal_longan_l3_routeEntry_alloc
 * Description:
 *      Allocate an L3 route entry.
 * Input:
 *      unit   - unit id
 *      ipv6  - ipv6 or not
 *      pfLen  - prefix length
 *      pIndex - pointer to entry index
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
_dal_longan_l3_routeEntry_alloc(uint32 unit, uint32 ipv6, uint32 pfLen, uint32 *pIndex);

/* Function Name:
 *      _dal_longan_l3_routeEntry_free
 * Description:
 *      Release an L3 route entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
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
_dal_longan_l3_routeEntry_free(uint32 unit, uint32 index);

/* Function Name:
 *      _dal_longan_l3_routeEntry_get
 * Description:
 *      Get an L3 route entry.
 * Input:
 *      unit   - unit id
 *      index  - pointer to entry index
 *      pEntry - pointer to entry
 *      idxChk - check index range or not
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
_dal_longan_l3_routeEntry_get(uint32 unit, uint32 index, dal_longan_l3_routeEntry_t *pEntry, rtk_enable_t idxChk);
/* Function Name:
 *      _dal_longan_l3_routeEntry_set
 * Description:
 *      Set an L3 route entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to entry
 *      idxChk - check index range or not
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
_dal_longan_l3_routeEntry_set(uint32 unit, uint32 index, dal_longan_l3_routeEntry_t *pEntry, rtk_enable_t idxChk);


/* Module Name    : Layer3 routing                */
/* Sub-module Name: Layer3 routing error handling */

/* Function Name:
 *      dal_longan_l3_routeEntry_get
 * Description:
 *      Get L3 rounting entry.
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
dal_longan_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

/* Function Name:
 *      dal_longan_l3_routeEntry_set
 * Description:
 *      Set L3 rounting entry.
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
dal_longan_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

/* Function Name:
 *      dal_longan_l3_info_get
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
dal_longan_l3_info_get(uint32 unit, rtk_l3_info_t *pInfo);

/* Function Name:
 *      dal_longan_l3_routerMacEntry_get
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
dal_longan_l3_routerMacEntry_get(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

/* Function Name:
 *      dal_longan_l3_routerMacEntry_set
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
dal_longan_l3_routerMacEntry_set(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry);

/* Function Name:
 *      dal_longan_l3_intf_create
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
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND     - specified vlan entry not found
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
dal_longan_l3_intf_create(uint32 unit, rtk_l3_intf_t *pIntf);


/* Function Name:
 *      dal_longan_l3_intf_destroy
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Note:
 */
extern int32
dal_longan_l3_intf_destroy(uint32 unit, rtk_intf_id_t intfId);


/* Function Name:
 *      dal_longan_l3_intf_destroyAll
 * Description:
 *      Destroy all L3 interfaces
 * Input:
 *      unit  - unit id
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
dal_longan_l3_intf_destroyAll(uint32 unit);


/* Function Name:
 *      dal_longan_l3_intf_get
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
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 */
extern int32
dal_longan_l3_intf_get(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);


/* Function Name:
 *      dal_longan_l3_intf_set
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
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND     - specified vlan entry not found
 * Note:
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 */
extern int32
dal_longan_l3_intf_set(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf);

/* Function Name:
 *      dal_longan_l3_vrrp_add
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
 *      (1) VRRP MAC address is build as { 00:00:5E:00, flags, VRID }.
 */
extern int32
dal_longan_l3_vrrp_add(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

/* Function Name:
 *      dal_longan_l3_vrrp_del
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
dal_longan_l3_vrrp_del(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId);

/* Function Name:
 *      dal_longan_l3_vrrp_delAll
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
dal_longan_l3_vrrp_delAll(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid);

/* Function Name:
 *      dal_longan_l3_vrrp_get
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
dal_longan_l3_vrrp_get(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrIdArraySize, uint32 *pVrIdArray, uint32 *pVrIdCount);

/* Function Name:
 *      dal_longan_l3_nextHop_create
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
 *          RTK_L3_FLAG_WITH_ID
 *          RTK_L3_FLAG_WITH_NH_DMAC
 *      (2) If the flag RTK_L3_FLAG_REPLACE is set, then replace the existing entry
 *          with the new info based on the input path ID (nhId).
 *          Otherwise, SDK will allocate a path ID for this new nexthop entry.
 *      (3) RTK_L3_FLAG_WITH_NH_DMAC is only supported in 9300. If the flag RTK_L3_FLAG_WITH_NH_DMAC is not set, pNextHop->mac_addr for adding l2 entry and dmac entry;
 *          If the flag RTK_L3_FLAG_WITH_NH_DMAC is set, pNextHop->mac_addr for adding l2 entry,  pNextHop->nh_mac_addr for dmac entry.
 *          In microsoft NLB, Routed packet with unicast DMAC need forward to multiport.
 */
extern int32
dal_longan_l3_nextHop_create(uint32 unit, rtk_l3_flag_t flags, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId);


/* Function Name:
 *      dal_longan_l3_nextHop_destroy
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 */
extern int32
dal_longan_l3_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId);


/* Function Name:
 *      dal_longan_l3_nextHop_get
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
dal_longan_l3_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,  rtk_l3_nextHop_t *pNextHop);


/* Function Name:
 *      dal_longan_l3_nextHopPath_find
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
dal_longan_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      dal_longan_l3_host_add
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
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6), and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *          RTK_L3_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_L3_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_L3_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_L3_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_L3_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_L3_FLAG_WITH_NEXTHOP   - assign path with nexthop entry
 *      (2) when pHost->path_id = 0, the pHost->fwd_act can only be RTK_L3_HOST_ACT_DROP
 *          or RTK_L3_HOST_ACT_TRAP2CPU
 */
extern int32
dal_longan_l3_host_add(uint32 unit, rtk_l3_host_t *pHost);


/* Function Name:
 *      dal_longan_l3_host_del
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
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 */
extern int32
dal_longan_l3_host_del(uint32 unit, rtk_l3_host_t *pHost);


/* Function Name:
 *      dal_longan_l3_host_del_byNetwork
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
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 */
extern int32
dal_longan_l3_host_del_byNetwork(uint32 unit, rtk_l3_route_t *pRoute);


/* Function Name:
 *      dal_longan_l3_host_del_byIntfId
 * Description:
 *      Delete L3 host entries that match or do not match a specified L3 interface number
 * Input:
 *      unit  - unit id
 *      flags - control flags
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 */
extern int32
dal_longan_l3_host_del_byIntfId(uint32 unit, rtk_intf_id_t intfId, rtk_l3_flag_t flags);


/* Function Name:
 *      dal_longan_l3_host_delAll
 * Description:
 *      Delete all L3 host table entries
 * Input:
 *      unit  - unit id
 *      flags - control flags
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
dal_longan_l3_host_delAll(uint32 unit, rtk_l3_flag_t flags);


/* Function Name:
 *      dal_longan_l3_host_find
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
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
extern int32
dal_longan_l3_host_find(uint32 unit, rtk_l3_host_t *pHost);


/* Function Name:
 *      dal_longan_l3_hostConflict_get
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
dal_longan_l3_hostConflict_get(uint32 unit, rtk_l3_key_t *pKey, rtk_l3_host_t *pHostArray, int32 maxHost, int32 *pHostCount);


/* Function Name:
 *      dal_longan_l3_host_age
 * Description:
 *      Run L3 host table aging
 * Input:
 *      unit    - unit id
 *      flags   - control flags
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
dal_longan_l3_host_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_hostTraverseCallback_f fAge, void *pCookie);


/* Function Name:
 *      dal_longan_l3_host_getNext
 * Description:
 *      Get the next valid L3 host entry (based on the base index)
 * Input:
 *      unit  - unit id
 *      flags - control flags
 *      pBase - pointer to the starting valid entry number to be searched
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
dal_longan_l3_host_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_host_t *pHost);


/* Function Name:
 *      dal_longan_l3_route_add
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
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_L3_PATH_ID_INVALID - the path ID (nexthop/ECMP) is invalid
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6), prefix_len and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *          RTK_L3_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_L3_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_L3_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_L3_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_L3_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_L3_FLAG_WITH_NEXTHOP   - assign path with nexthop entry
 *      (2) when pRoute->path_id = 0, the pRoute->fwd_act can only be RTK_L3_HOST_ACT_DROP
 *          or RTK_L3_HOST_ACT_TRAP2CPU
 */
extern int32
dal_longan_l3_route_add(uint32 unit, rtk_l3_route_t *pRoute);


/* Function Name:
 *      dal_longan_l3_route_del
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
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          vrf_id, ip_addr (either ipv4 or ipv6) and prefix_len.
 */
extern int32
dal_longan_l3_route_del(uint32 unit, rtk_l3_route_t *pRoute);


/* Function Name:
 *      dal_longan_l3_route_get
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
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
extern int32
dal_longan_l3_route_get(uint32 unit, rtk_l3_route_t *pRoute);


/* Function Name:
 *      dal_longan_l3_route_del_byIntfId
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Note:
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 */
extern int32
dal_longan_l3_route_del_byIntfId(uint32 unit, rtk_l3_flag_t flags, rtk_intf_id_t intfId);


/* Function Name:
 *      dal_longan_l3_route_delAll
 * Description:
 *      Delete all L3 route table entries
 * Input:
 *      unit  - unit id
 *      flags - control flags
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
dal_longan_l3_route_delAll(uint32 unit, rtk_l3_flag_t flags);


/* Function Name:
 *      dal_longan_l3_route_age
 * Description:
 *      Run L3 route table aging
 * Input:
 *      unit    - unit id
 *      flags   - control flags
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
dal_longan_l3_route_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_routeTraverseCallback_f fAge, void *pCookie);


/* Function Name:
 *      dal_longan_l3_route_getNext
 * Description:
 *      Get the next valid L3 route entry (based on the base index)
 * Input:
 *      unit   - unit id
 *      flags  - control flags (applicable flags: RTK_L3_FLAG_IPV6)
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
dal_longan_l3_route_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_route_t *pRoute);

#if LONGAN_L3_ROUTE_RSVMC_ENTRY
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

/* Function Name:
 *      dal_longan_l3_route_rsv4Mcast_add
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
dal_longan_l3_route_rsv4Mcast_add(uint32 unit);

/* Function Name:
 *      dal_longan_l3_route_rsv6Mcast_add
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
dal_longan_l3_route_rsv6Mcast_add(uint32 unit);
#endif

#if LONGAN_L3_ROUTE_IPMC_SIZE
/* Function Name:
 *      _dal_longan_l3_route_ipmcSize_get
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
_dal_longan_l3_route_ipmcSize_get(uint32 unit, uint32 *pBase, uint32 *pSize);

#endif

/* Function Name:
 *      dal_longan_l3_globalCtrl_get
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
dal_longan_l3_globalCtrl_get(uint32 unit, rtk_l3_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_longan_l3_globalCtrl_set
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
dal_longan_l3_globalCtrl_set(uint32 unit, rtk_l3_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_longan_l3_intfCtrl_get
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
dal_longan_l3_intfCtrl_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_longan_l3_intfCtrl_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Note:
 */
extern int32
dal_longan_l3_intfCtrl_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_longan_l3_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 * Output:
 *      pArg   - pointer to the argurment
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
dal_longan_l3_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_longan_l3_portCtrl_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 */
extern int32
dal_longan_l3_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 arg);

#endif /* __RTK_L3_H__ */


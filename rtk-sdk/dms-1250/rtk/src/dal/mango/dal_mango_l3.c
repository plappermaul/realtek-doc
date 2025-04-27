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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_vlan.h>
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_ipmcast.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <rtk/l3.h>

/*
 * Symbol Definition
 */
#define MANGO_L3_DBG                    (0)
#define MANGO_L3_ROUTE_HW_LU            (1)     /* H/W search for finding route entry */
#define MANGO_L3_ROUTER_MAC_ENTRY_SHADOW        /* using shadow to speed up */
#define MANGO_L3_REFCNT_CHK_DEFAULT     (ENABLED)


typedef struct dal_mango_l3_vlanIntfListNode_s
{
    uint32  intfIdx;    /* L3 interface index */

    /* node of link list */
    RTK_LIST_NODE_REF_DEF(dal_mango_l3_vlanIntfListNode_s, nodeRef);
} dal_mango_l3_vlanIntfListNode_t;


typedef struct dal_mango_l3_drvDb_s
{
    /* hardware resource (for internal APIs) */
    struct
    {
        /* Interface MTU (IPv4) */
        uint32          ip_mtu_used_count;
        struct
        {
            uint32  mtu_value;
            uint32  ref_count;
        } ip_mtu[DAL_MANGO_L3_IP_MTU_MAX];

        /* Interface MTU (IPv6) */
        uint32          ip6_mtu_used_count;
        struct
        {
            uint32  mtu_value;
            uint32  ref_count;
        } ip6_mtu[DAL_MANGO_L3_IP6_MTU_MAX];

        /* router MAC */
        rtk_bitmap_t    mac_used[BITMAP_ARRAY_CNT(DAL_MANGO_L3_MAC_MAX)];
        uint32          mac_used_count;
        struct
        {
            #ifdef MANGO_L3_ROUTER_MAC_ENTRY_SHADOW
            l3_router_mac_entry_t   mac_entry;  /* shadow */
            #endif
        } mac[DAL_MANGO_L3_MAC_MAX];

        /* interface */
        rtk_bitmap_t    intf_used[BITMAP_ARRAY_CNT(DAL_MANGO_L3_INTF_MAX)];
        uint32          intf_used_count;
        struct
        {
            uint32  flags;  /* for logging: caller and etc. */
        } intf[DAL_MANGO_L3_INTF_MAX];

        /* nexthop */
        rtk_bitmap_t    nexthop_used[BITMAP_ARRAY_CNT(DAL_MANGO_L3_NEXTHOP_MAX)];
        uint32          nexthop_used_count;
        struct
        {
            uint32  flags;  /* for logging: caller and etc. */
        } nexthop[DAL_MANGO_L3_NEXTHOP_MAX];

        /* ECMP */
        rtk_bitmap_t    ecmp_used[BITMAP_ARRAY_CNT(DAL_MANGO_L3_ECMP_MAX)];
        uint32          ecmp_used_count;
        struct
        {
            uint32  flags;  /* for logging: caller and etc. */
        } ecmp[DAL_MANGO_L3_ECMP_MAX];

        /* Host table */
        uint32          host_used_count;
        struct
        {
            /* table */
            struct
            {
                /* row */
                uint32      valid_mask;

                struct
                {
                    /* slot */
                    uint32  width;
                } slot[DAL_MANGO_L3_HOST_TBL_WIDTH];
            } row[DAL_MANGO_L3_HOST_TBL_HEIGHT];
        } hostTable[DAL_MANGO_L3_HOST_TBL_NUM];

        /* route table */
        struct
        {   /* routeTable */
            uint32  index_max;      /* maximum index (for checking parameters) */
            uint32  size;           /* route table size */

            struct
            {
                uint32  used_count;     /* used entry number by IPv4 UC entry */

                /* total entries after the specific pfLen (include self pfLen) */
                uint16  entries_after_include_pfLen[MANGO_L3_HOST_IP_PFLEN];
            } IPv4;

            struct
            {
                uint32  used_count;     /* used entry number by IPv6 UC entry */

                /* total entries before the specific pfLen (exclude self pfLen) */
                uint16  entries_before_exclude_pfLen[MANGO_L3_HOST_IP6_PFLEN];
            } IPv6;

#ifdef MANGO_L3_ROUTE_IPMC_DYNAMIC
            struct
            {
                uint16  used_count;     /* used entry number by IPv4 MC entry */
            } IPMC;

            struct
            {
                uint16  used_count;     /* used entry number by IPv6 MC entry */
            } IP6MC;

            uint16  ipuc_max;
            uint16  blank_max;
            uint16  ip6uc_max;
            uint16  ipmc_max;
            uint16  ip6mc_max;
#endif
        } routeTable;
    } HW;


    /* VLAN */
    rtk_bitmap_t    vid_valid[BITMAP_ARRAY_CNT(DAL_MANGO_L3_VID_MAX)];
    struct
    {
        uint32      intf_idx;
        uint32      intf_count;

        RTK_LIST_DEF(dal_mango_l3_vlanIntfListNode_s, intf_list);   /* associated INTFs */
    } vid[DAL_MANGO_L3_VID_MAX];
    /* VLAN interface list node */
    dal_mango_l3_vlanIntfListNode_t vlanIntfNode[DAL_MANGO_L3_INTF_MAX];


    /* Interface */
    struct
    {
        uint32      valid:1;        /* interface valid */
        uint32      macIdxValid:1;  /* router MAC entry index valid */
        uint32      macIdx:10;      /* router MAC entry index */
        uint32      reserved:4;     /* reserved */
        uint32      ref_count:16;   /* reference counter */

        /* <SHADOW>: cache to avoid reading HW entry from CHIP (enhancement) */

        rtk_intf_id_t   intf_id;
        rtk_vrf_id_t    vrf_id;     /* [SHADOW] for calculating VRF usage */

        /* [DRV] */
        uint32      ip_mtu_idx:16;  /* [SHADOW] for managing MTU resource */
        uint32      ip6_mtu_idx:16; /* [SHADOW] for managing IPv6 MTU resource */
        rtk_vlan_t  vid;            /* [SHADOW] for creating nexthop entry */
    } intf[DAL_MANGO_L3_INTF_MAX];

    /* Nexthop */
    struct
    {
        uint32      valid:1;
        uint32      mac_addr_alloc:1;   /* MAC has been allocated if set */
        uint32      reserved:14;        /* reserved */
        uint32      ref_count:16;

        /* <SHADOW>: cache to avoid reading HW entry from CHIP (enhancement) */
        uint32      intf_idx:16;        /* egress L3 interface */
        uint32      mac_addr_idx:16;    /* destination MAC address index */
        rtk_mac_t   mac_addr;           /* destination MAC address */
    } nexthop[DAL_MANGO_L3_NEXTHOP_MAX];

    /* ECMP */
    rtk_enable_t    ecmp_hash_tbl_manual;
    struct
    {
        uint32      valid:1;
        uint32      reserved:7;         /* reserved */
        uint32      nh_count:8;         /* count of valid nexthop */
        uint32      ref_count:16;

        /* <SHADOW>: cache to avoid reading HW entry from CHIP (enhancement) */
    } ecmp[DAL_MANGO_L3_ECMP_MAX];

    /* Host table */
    struct
    {
        uint32      valid:1;
        uint32      ipv6:1;
        uint32      reserved:30;    /* reserved */

        struct
        {
            rtk_l3_pathId_t path_id;
        } shadow;
    } host[DAL_MANGO_L3_HOST_TBL_SIZE];

    /* Route table */
    struct
    {
        uint32      valid:1;
        uint32      ipv6:1;
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        uint32      ipmc:1;
        uint32      reserved:29;    /* reserved */

        void        *pIpmcEntry;    /* dal_mango_ipmc_l3Entry_t */
#else
        uint32      reserved:30;    /* reserved */
#endif

        struct
        {
            rtk_l3_route_t  routeEntry;
        } shadow;
    } route[DAL_MANGO_L3_ROUTE_TBL_SIZE];

    /* reference count check state */
    rtk_enable_t    refer_cnt_chk_en;

#if MANGO_L3_ROUTE_IPMC_SIZE
    uint32          l3_route_ipmc_idx_base;
    uint32          l3_route_ipmc_size;
#endif
} dal_mango_l3_drvDb_t;

/* action */
static uint32 _actRouterMAC[] = { /* action for Router MAC entry */
    RTK_L3_ACT_FORWARD,         /* routing */
    RTK_L3_ACT_DROP,            /* bridging only */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

#if 0   /* do not use this */
static uint32 _actEntryUC[] = { /* action for Unicast (HOST/ROUTE) entry */
    RTK_L3_ACT_FORWARD,         /* routing */
    RTK_L3_ACT_DROP,            /* bridging only */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };
#endif

static uint32 _actHostUC[] = { /* action for Unicast (HOST) entry */
    RTK_L3_HOST_ACT_FORWARD,
    RTK_L3_HOST_ACT_TRAP2CPU,
    RTK_L3_HOST_ACT_COPY2CPU,
    RTK_L3_HOST_ACT_DROP,
    };

static uint32 _actRouteUC[] = { /* action for Unicast (ROUTE) entry */
    RTK_L3_ROUTE_ACT_FORWARD,
    RTK_L3_ROUTE_ACT_TRAP2CPU,
    RTK_L3_ROUTE_ACT_COPY2CPU,
    RTK_L3_ROUTE_ACT_DROP,
    };

static uint32 _actIpRouteCtrlIpHdrErr[] = { /* MANGO_L3_IP_ROUTE_CTRLr, MANGO_IP_HDR_ERR_ACTf */
    RTK_L3_ACT_HARD_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIpucRouteCtrlBadSip[] = {   /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlBadDip[] = {   /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_BAD_DIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlZeroSip[] = {  /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlDmacBc[] = { /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_DMAC_BC_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_FORWARD,         /* bridge only */
    };

static uint32 _actIpucRouteCtrlDmacMc[] = {   /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_DMAC_MC_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlHdrOpt[] = {   /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_HDR_OPT_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,         /* route */
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlMtuFail[] = {  /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlTtlFail[] = {  /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _optIpucRouteCtrlPktToCpuTarget[] = { /* MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf */
    RTK_L3_CPUTARGET_LOCAL,
    RTK_L3_CPUTARGET_MASTER,
    };

static uint32 _actIpRouteCtrlIp6HdrErr[] = {  /* MANGO_L3_IP_ROUTE_CTRLr, MANGO_IP6_HDR_ERR_ACTf */
    RTK_L3_ACT_HARD_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIp6ucRouteCtrlBadSip[] = {  /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlBadDip[] = {  /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_BAD_DIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlZeroSip[] = { /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlDmacMismatch[] = {    /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHbh[] = { /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HBH_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHbhErr[] = {  /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HBH_ERR_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHdrRoute[] = {    /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HDR_ROUTE_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlMtuFail[] = { /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHlFail[] = {  /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HL_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _optIp6ucRouteCtrlPktToCpuTarget[] = {    /* MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf */
    RTK_L3_CPUTARGET_LOCAL,
    RTK_L3_CPUTARGET_MASTER,
    };

static uint32 _actIpRouteCtrlNhErr[] = {  /* MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_ERR_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpRouteCtrlNhAgeOut[] = {   /* MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_AGE_OUT_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIpRouteCtrlNonIp[] = {  /* MANGO_L3_IP_ROUTE_CTRLr, MANGO_NON_IP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIgrIntfIpUrpfFail[] = { /* MANGO_L3_IGR_INTF_IP_URPF_FAIL_ACTtf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actEgrIntfIpIcmpRedirect[] = { /* MANGO_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actEgrIntfIpPbrIcmpRedirect[] = {  /* MANGO_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIIgrIntfIp6UrpfFail[] = {   /* MANGO_L3_IGR_INTF_IP6_URPF_FAIL_ACTtf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actEgrIntfIp6IcmpRedirect[] = {    /* MANGO_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actEgrIntfIp6PbrIcmpRedirect[] = { /* MANGO_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIgrIntfIpmcAct224_0_0_x[] = {   /* MANGO_L3_IGR_INTF_IPMC_ACT_224_0_0_Xtf */
    RTK_L3_ACT_FORWARD,     /* Follow bridging decision if found a match entry, or flood in VLAN */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIgrIntfIpmcAct224_0_1_x[] = {   /* MANGO_L3_IGR_INTF_IPMC_ACT_224_0_1_Xtf */
    RTK_L3_ACT_FORWARD,     /* Follow bridging decision if found a match entry, or flood in VLAN */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIgrIntfIpmcAct239_x_x_x[] = {   /* MANGO_L3_IGR_INTF_IPMC_ACT_239_X_X_Xtf */
    RTK_L3_ACT_FORWARD,     /* Follow bridging decision if found a match entry, or flood in VLAN */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIgrIntfIpmcRouteLuMis[] = { /* MANGO_L3_IGR_INTF_IPMC_ROUTE_LU_MIS_ACTtf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIgrIntfIp6Nd[] = {  /* MANGO_L3_IGR_INTF_IP6_ND_ACTtf */
    RTK_L3_ACT_FORWARD,     /* Follow bridging decision if found a match entry, or flood in VLAN */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIgrIntfIp6mcAct0000_00xx[] = {  /* MANGO_L3_IGR_INTF_IP6MC_ACT_0000_00XXtf */
    RTK_L3_ACT_FORWARD,     /* Follow bridging decision if found a match entry, or flood in VLAN */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIgrIntfIp6MldAct0_X_X[] = { /* MANGO_L3_IGR_INTF_IP6_MLD_ACT_0_X_Xtf */
    RTK_L3_ACT_FORWARD,     /* Follow bridging decision if found a match entry, or flood in VLAN */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIgrIntfIp6MldActDb8_X_X[] = {   /* MANGO_L3_IGR_INTF_IP6_MLD_ACT_DB8_X_Xtf */
    RTK_L3_ACT_FORWARD,     /* Follow bridging decision if found a match entry, or flood in VLAN */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    RTK_L3_ACT_DROP,
    };

static uint32 _actIgrIntfIp6mcRouteLuMis[] = {    /* MANGO_L3_IGR_INTF_IP6MC_ROUTE_LU_MIS_ACTtf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actPortIpRouteCtrlUrpfFail[] = {   /* MANGO_L3_PORT_IP_ROUTE_CTRLr, MANGO_URPF_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actPortIp6RouteCtrlUrpfFail[] = {  /* MANGO_L3_PORT_IP6_ROUTE_CTRLr, MANGO_URPF_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };


/*
 * Data Declaration
 */
static uint32               l3_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         l3_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         l3_int_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         l3_hwLU_sem[RTK_MAX_NUM_OF_UNIT];
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
static osal_mutex_t         l3_route_sem[RTK_MAX_NUM_OF_UNIT];
#endif

static dal_mango_l3_drvDb_t     *_pL3Db[RTK_MAX_NUM_OF_UNIT] = { 0 };

/*
 * Macro Declaration
 */
#define MANGO_L3_DBG_PRINTF(_level, _fmt, ...)                                      \
    do {                                                                            \
        if (MANGO_L3_DBG >= (_level))                                               \
            osal_printf("%s():L%d: " _fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
    } while (0)

/* L3 semaphore handling */
#define L3_SEM_LOCK(unit)                                                               \
    do {                                                                                \
        if (osal_sem_mutex_take(l3_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)      \
        {                                                                               \
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore lock failed");  \
            return RT_ERR_SEM_LOCK_FAILED;                                              \
        }                                                                               \
    } while(0)
#define L3_SEM_UNLOCK(unit)                                                                 \
    do {                                                                                    \
        if (osal_sem_mutex_give(l3_sem[unit]) != RT_ERR_OK)                                 \
        {                                                                                   \
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore unlock failed");  \
            return RT_ERR_SEM_UNLOCK_FAILED;                                                \
        }                                                                                   \
    } while(0)
#define L3_INT_SEM_LOCK(unit)                                                                   \
    do {                                                                                        \
        if (osal_sem_mutex_take(l3_int_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)          \
        {                                                                                       \
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "internal semaphore lock failed"); \
            return RT_ERR_SEM_LOCK_FAILED;                                                      \
        }                                                                                       \
    } while(0)
#define L3_INT_SEM_UNLOCK(unit)                                                                     \
    do {                                                                                            \
        if (osal_sem_mutex_give(l3_int_sem[unit]) != RT_ERR_OK)                                     \
        {                                                                                           \
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "internal semaphore unlock failed"); \
            return RT_ERR_SEM_UNLOCK_FAILED;                                                        \
        }                                                                                           \
    } while(0)
#define L3_HWLU_SEM_LOCK(unit)                                                                  \
    do {                                                                                        \
        if (osal_sem_mutex_take(l3_hwLU_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)         \
        {                                                                                       \
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "internal semaphore lock failed"); \
            return RT_ERR_SEM_LOCK_FAILED;                                                      \
        }                                                                                       \
    } while(0)
#define L3_HWLU_SEM_UNLOCK(unit)                                                                    \
    do {                                                                                            \
        if (osal_sem_mutex_give(l3_hwLU_sem[unit]) != RT_ERR_OK)                                    \
        {                                                                                           \
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "internal semaphore unlock failed"); \
            return RT_ERR_SEM_UNLOCK_FAILED;                                                        \
        }                                                                                           \
    } while(0)
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
#define L3_ROUTE_SEM_LOCK(unit)                                                                 \
    do {                                                                                        \
        if (osal_sem_mutex_take(l3_route_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)        \
        {                                                                                       \
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "l3-route semaphore lock failed"); \
            return RT_ERR_SEM_LOCK_FAILED;                                                      \
        }                                                                                       \
    } while(0)
#define L3_ROUTE_SEM_UNLOCK(unit)                                                                   \
    do {                                                                                            \
        if (osal_sem_mutex_give(l3_route_sem[unit]) != RT_ERR_OK)                                   \
        {                                                                                           \
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "l3-route semaphore unlock failed"); \
            return RT_ERR_SEM_UNLOCK_FAILED;                                                        \
        }                                                                                           \
    } while(0)
#endif
#define L3_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                    \
        if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)               \
        {                                                                                   \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                        \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define L3_REG_FIELD_SET_ERR_HDL(_unit, _reg, _field, _val, _data, _errMsg, _gotoErrLbl, _ret)     \
    do {                                                                                    \
        if ((_ret = reg_field_set(_unit, _reg, _field, &_val, &_data)) != RT_ERR_OK)        \
        {                                                                                   \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                        \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define L3_REG_FIELD_WRITE_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                    \
        if ((_ret = reg_field_write(_unit, _reg, _field, &_val)) != RT_ERR_OK)              \
        {                                                                                   \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                        \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define L3_REG_READ_ERR_HDL(_unit, _reg, _val, _errMsg, _gotoErrLbl, _ret)                  \
    do {                                                                                    \
        if ((_ret = reg_read(_unit, _reg, &_val)) != RT_ERR_OK)                             \
        {                                                                                   \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                        \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define L3_REG_WRITE_ERR_HDL(_unit, _reg, _val, _errMsg, _gotoErrLbl, _ret)                 \
    do {                                                                                    \
        if ((_ret = reg_write(_unit, _reg, &_val)) != RT_ERR_OK)                            \
        {                                                                                   \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                        \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define L3_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                                        \
        if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)               \
        {                                                                                                       \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                                            \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                                        \
        if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)              \
        {                                                                                                       \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                                            \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define L3_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                \
        if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)     \
        {                                                                               \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                    \
            goto _gotoErrLbl;                                                           \
        }                                                                               \
    } while(0)
#define L3_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                \
        if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)    \
        {                                                                               \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                    \
            goto _gotoErrLbl;                                                           \
        }                                                                               \
    } while(0)
#define L3_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)           \
    do {                                                                                                    \
        if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        {                                                                                                   \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                                        \
            goto _gotoErrLbl;                                                                               \
        }                                                                                                   \
    } while(0)
#define L3_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)           \
    do {                                                                                                    \
        if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        {                                                                                                   \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                                        \
            goto _gotoErrLbl;                                                                               \
        }                                                                                                   \
    } while(0)
#define L3_TABLE_FIELD_MAC_GET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)           \
    do {                                                                                                        \
        if ((_ret = table_field_mac_get(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
        {                                                                                                       \
            RT_ERR(_ret, (MOD_L3|MOD_DAL), _errMsg);                                                            \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define L3_TABLE_FIELD_MAC_SET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)           \
    do {                                                                                                        \
        if ((_ret = table_field_mac_set(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
        {                                                                                                       \
            RT_ERR(ret, (MOD_L3|MOD_DAL), _errMsg);                                                             \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define L3_TABLE_READ_FIELD_GET_ERR_HDL(_unit, _tbl, _idx, _entry, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                                        \
        L3_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret);                           \
        L3_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret);              \
    } while(0)
#define L3_TABLE_WRITE_FIELD_SET_ERR_HDL(_unit, _tbl, _idx, _entry, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                                        \
        L3_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret);                           \
        L3_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret);              \
        L3_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret);                          \
    } while(0)
#define L2_MAC_IDX_TO_INDEX_STRUCT(_idx, _struct)                       \
    do {                                                                \
        _struct.index_type = ((_idx) < 32768)? L2_IN_HASH : L2_IN_CAM;  \
        _struct.index = ((_idx) >> 2);                                  \
        _struct.hashdepth = ((_idx) & 0x3);                             \
    } while(0)
#define L3_RT_ERR_HDL_DBG(_op, _args...)                    \
    do {                                                    \
        if (RT_ERR_OK != (_op))                             \
        {                                                   \
           RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), ## _args);   \
        }                                                   \
    } while(0)
#define L3_DB_UPDATE_INTF_VALID(_unit, _intfIdx, _mtuIdx, _ip6mtuIdx)   \
    do {                                                                \
        _pL3Db[_unit]->intf[_intfIdx].ip_mtu_idx = _mtuIdx;             \
        _pL3Db[_unit]->intf[_intfIdx].ip6_mtu_idx = _ip6mtuIdx;         \
        _pL3Db[_unit]->intf[_intfIdx].valid = TRUE;                     \
    } while(0)
#define L3_DB_UPDATE_INTF_INVALID(_unit, _intfIdx)      \
    do {                                                \
        _pL3Db[_unit]->intf[_intfIdx].ip_mtu_idx = 0;   \
        _pL3Db[_unit]->intf[_intfIdx].ip6_mtu_idx = 0;  \
        _pL3Db[_unit]->intf[_intfIdx].valid = FALSE;    \
    } while(0)
#define L3_DB_INTF_IS_VALID(_unit, _intfIdx)    (_pL3Db[_unit]->intf[_intfIdx].valid)
#define L3_DB_INTF_REFCNT(_unit, _intfIdx)      (_pL3Db[_unit]->intf[_intfIdx].ref_count)
#define L3_DB_UPDATE_INTF_REFCNT_RESET(_unit, _intfIdx) \
    do {                                                \
        _pL3Db[_unit]->intf[_intfIdx].ref_count = 0;    \
        MANGO_L3_DBG_PRINTF(3, "intf[%d].ref_cnt = %d\n", _intfIdx, _pL3Db[_unit]->intf[_intfIdx].ref_count);   \
    } while(0)
#define L3_DB_UPDATE_INTF_REFCNT_INC(_unit, _intfIdx)   \
    do {                                                \
        _pL3Db[_unit]->intf[_intfIdx].ref_count += 1;   \
        MANGO_L3_DBG_PRINTF(3, "intf[%d].ref_cnt = %d\n", _intfIdx, _pL3Db[_unit]->intf[_intfIdx].ref_count);   \
    } while(0)
#define L3_DB_UPDATE_INTF_REFCNT_DEC(_unit, _intfIdx)       \
    do {                                                    \
        if (_pL3Db[_unit]->intf[_intfIdx].ref_count > 0)    \
            _pL3Db[_unit]->intf[_intfIdx].ref_count -= 1;   \
        MANGO_L3_DBG_PRINTF(3, "intf[%d].ref_cnt = %d\n", _intfIdx, _pL3Db[_unit]->intf[_intfIdx].ref_count);   \
    } while(0)
#define L3_DB_UPDATE_INTF_MAC_INDEX(_unit, _intfIdx, _macIdxValid, _macIdx) \
    do {                                                                \
        _pL3Db[_unit]->intf[(_intfIdx)].macIdxValid = (_macIdxValid);   \
        _pL3Db[_unit]->intf[(_intfIdx)].macIdx = (_macIdx);             \
    } while(0)
#define L3_DB_INTF_MAC_INDEX_VALID(_unit, _intfIdx) (_pL3Db[_unit]->intf[_intfIdx].macIdxValid)
#define L3_DB_INTF_MAC_INDEX(_unit, _intfIdx)       (_pL3Db[_unit]->intf[_intfIdx].macIdx)

#define L3_DB_UPDATE_NH_VALID(_unit, _nhIdx)            \
    do {                                                \
        _pL3Db[_unit]->nexthop[_nhIdx].valid = TRUE;    \
    } while(0)
#define L3_DB_UPDATE_NH_INVALID(_unit, _nhIdx)          \
    do {                                                \
        _pL3Db[_unit]->nexthop[_nhIdx].valid = FALSE;   \
    } while(0)
#define L3_DB_NH_IS_VALID(_unit, _nhIdx)    (_pL3Db[_unit]->nexthop[_nhIdx].valid)
#define L3_DB_NH_REFCNT(_unit, _nhIdx)      (_pL3Db[_unit]->nexthop[_nhIdx].ref_count)
#define L3_DB_UPDATE_NH_REFCNT_RESET(_unit, _nhIdx)     \
    do {                                                \
        _pL3Db[_unit]->nexthop[_nhIdx].ref_count = 0;   \
        MANGO_L3_DBG_PRINTF(3, "nexthop[%d].ref_cnt = %d\n", _nhIdx, _pL3Db[_unit]->nexthop[_nhIdx].ref_count); \
    } while(0)
#define L3_DB_UPDATE_NH_REFCNT_INC(_unit, _nhIdx)           \
    do {                                                    \
        if (DAL_MANGO_L3_RESERVED_NEXTHOP_IDX != (_nhIdx))  \
        {                                                   \
            _pL3Db[_unit]->nexthop[_nhIdx].ref_count += 1;  \
            MANGO_L3_DBG_PRINTF(3, "nexthop[%d].ref_cnt = %d\n", _nhIdx, _pL3Db[_unit]->nexthop[_nhIdx].ref_count); \
        }                                                   \
    } while(0)
#define L3_DB_UPDATE_NH_REFCNT_DEC(_unit, _nhIdx)               \
    do {                                                        \
        if (DAL_MANGO_L3_RESERVED_NEXTHOP_IDX != (_nhIdx))      \
        {                                                       \
            if (_pL3Db[_unit]->nexthop[_nhIdx].ref_count > 0)   \
                _pL3Db[_unit]->nexthop[_nhIdx].ref_count -= 1;  \
            MANGO_L3_DBG_PRINTF(3, "nexthop[%d].ref_cnt = %d\n", _nhIdx, _pL3Db[_unit]->nexthop[_nhIdx].ref_count); \
        }                                                       \
    } while(0)
#define L3_DB_UPDATE_ECMP_VALID(_unit, _ecmpIdx)    \
    do {                                            \
        _pL3Db[_unit]->ecmp[_ecmpIdx].valid = TRUE; \
    } while(0)
#define L3_DB_UPDATE_ECMP_INVALID(_unit, _ecmpIdx)      \
    do {                                                \
        _pL3Db[_unit]->ecmp[_ecmpIdx].valid = FALSE;    \
    } while(0)
#define L3_DB_ECMP_IS_VALID(_unit, _ecmpIdx)    (_pL3Db[_unit]->ecmp[_ecmpIdx].valid)
#define L3_DB_ECMP_REFCNT(_unit, _ecmpIdx)      (_pL3Db[_unit]->ecmp[_ecmpIdx].ref_count)
#define L3_DB_UPDATE_ECMP_REFCNT_RESET(_unit, _ecmpIdx) \
    do {                                                \
        _pL3Db[_unit]->ecmp[_ecmpIdx].ref_count = 0;    \
        MANGO_L3_DBG_PRINTF(3, "ecmp[%d].ref_cnt = %d\n", _ecmpIdx, _pL3Db[_unit]->ecmp[_ecmpIdx].ref_count);   \
    } while(0)
#define L3_DB_UPDATE_ECMP_REFCNT_INC(_unit, _ecmpIdx)   \
    do {                                                \
        _pL3Db[_unit]->ecmp[_ecmpIdx].ref_count += 1;   \
        MANGO_L3_DBG_PRINTF(3, "ecmp[%d].ref_cnt = %d\n", _ecmpIdx, _pL3Db[_unit]->ecmp[_ecmpIdx].ref_count);   \
    } while(0)
#define L3_DB_UPDATE_ECMP_REFCNT_DEC(_unit, _ecmpIdx)       \
    do {                                                    \
        if (_pL3Db[_unit]->ecmp[_ecmpIdx].ref_count > 0)    \
            _pL3Db[_unit]->ecmp[_ecmpIdx].ref_count -= 1;   \
        MANGO_L3_DBG_PRINTF(3, "ecmp[%d].ref_cnt = %d\n", _ecmpIdx, _pL3Db[_unit]->ecmp[_ecmpIdx].ref_count);   \
    } while(0)
#define L3_DB_UPDATE_HOST_VALID(_unit, _hostIdx)    \
    do {                                            \
        _pL3Db[_unit]->host[_hostIdx].valid = TRUE; \
    } while(0)
#define L3_DB_UPDATE_HOST_INVALID(_unit, _hostIdx)      \
    do {                                                \
        _pL3Db[_unit]->host[_hostIdx].ipv6 = FALSE;     \
        _pL3Db[_unit]->host[_hostIdx].valid = FALSE;    \
    } while(0)
#define L3_DB_HOST_IS_VALID(_unit, _hostIdx)    (_pL3Db[_unit]->host[_hostIdx].valid)
#define L3_DB_UPDATE_ROUTE_VALID(_unit, _routeIdx)      \
    do {                                                \
        _pL3Db[_unit]->route[_routeIdx].valid = TRUE;   \
    } while(0)
#define L3_DB_UPDATE_ROUTE_INVALID(_unit, _routeIdx)    \
    do {                                                \
        _pL3Db[_unit]->route[_routeIdx].ipv6 = FALSE;   \
        _pL3Db[_unit]->route[_routeIdx].valid = FALSE;  \
    } while(0)
#define L3_DB_ROUTE_IS_VALID(_unit, _routeIdx)  (_pL3Db[_unit]->route[_routeIdx].valid)

#define L3_ACTION_TO_VALUE(_actArray, _value, _action, _errMsg, _errHandle, _retval)        \
    do {                                                                                    \
        if ((_retval = RT_UTIL_ACTLIST_INDEX_GET(_actArray, _value, _action)) != RT_ERR_OK) \
        {                                                                                   \
            RT_ERR(_retval, (MOD_L3|MOD_DAL), _errMsg);                                     \
            goto _errHandle;                                                                \
        }                                                                                   \
    } while(0)
#define L3_VALUE_TO_ACTION(_actArray, _action, _value, _errMsg, _errHandle, _retval)            \
    do {                                                                                        \
        if ((_retval = RT_UTIL_ACTLIST_ACTION_GET(_actArray, _action, _value)) != RT_ERR_OK)    \
        {                                                                                       \
            RT_ERR(_retval, (MOD_L3|MOD_DAL), _errMsg);                                         \
            goto _errHandle;                                                                    \
        }                                                                                       \
    } while(0)

#define L2_MAC_ADDR_IS_EQUAL(_pMac1, _pMac2)    (0 == osal_memcmp((rtk_mac_t *)&(_pMac1), (rtk_mac_t *)&(_pMac2), sizeof(rtk_mac_t)))

#define L3_ROUTE_IDX_MIN(_unit)                 (0)
#define L3_ROUTE_IDX_MAX(_unit)                 (_pL3Db[(_unit)]->HW.routeTable.index_max)
#define L3_ROUTE_TBL_SIZE(_unit)                (_pL3Db[(_unit)]->HW.routeTable.size)
#define L3_ROUTE_TBL_IP_CNT(_unit)              (_pL3Db[(_unit)]->HW.routeTable.IPv4.used_count)
#define L3_ROUTE_TBL_IP_CNT_PFLEN(_unit, _pfl)  (_pL3Db[(_unit)]->HW.routeTable.IPv4.entries_after_include_pfLen[(_pfl)])
#define L3_ROUTE_TBL_IP6_CNT(_unit)             (_pL3Db[(_unit)]->HW.routeTable.IPv6.used_count)
#define L3_ROUTE_TBL_IP6_CNT_PFLEN(_unit, _pfl) (_pL3Db[(_unit)]->HW.routeTable.IPv6.entries_before_exclude_pfLen[(_pfl)])
#define L3_ROUTE_TBL_USED(_unit)                (L3_ROUTE_TBL_IP_CNT(_unit) + (L3_ROUTE_TBL_IP6_CNT(_unit) * 3))
#ifdef MANGO_L3_ROUTE_IPMC_DYNAMIC
#define L3_ROUTE_ENTRY_IPUC_LEN                 (1)
#define L3_ROUTE_ENTRY_IPMC_LEN                 (2)
#define L3_ROUTE_ENTRY_IP6UC_LEN                (3)
#define L3_ROUTE_ENTRY_IP6MC_LEN                (6)
#define L3_ROUTE_ENTRY_BUCKET_LEN               (6)

#define L3_ROUTE_TBL_IPMC_CNT(_unit)            (_pL3Db[(_unit)]->HW.routeTable.IPMC.used_count)
#define L3_ROUTE_TBL_IP6MC_CNT(_unit)           (_pL3Db[(_unit)]->HW.routeTable.IP6MC.used_count)
#define L3_ROUTE_TBL_USED_INC_MC(_unit)         (L3_ROUTE_TBL_IP_CNT(_unit) + (L3_ROUTE_TBL_IP6_CNT(_unit) * 3) + (L3_ROUTE_TBL_IPMC_CNT(_unit) * 2) + (L3_ROUTE_TBL_IP6MC_CNT(_unit) * 6))

#define L3_ROUTE_TBL_IPUC_MAX(_unit)            (_pL3Db[(_unit)]->HW.routeTable.ipuc_max)
#define L3_ROUTE_TBL_BLANK_MAX(_unit)           (_pL3Db[(_unit)]->HW.routeTable.blank_max)
#define L3_ROUTE_TBL_IP6UC_MAX(_unit)           (_pL3Db[(_unit)]->HW.routeTable.ip6uc_max)
#define L3_ROUTE_TBL_IPMC_MAX(_unit)            (_pL3Db[(_unit)]->HW.routeTable.ipmc_max)
#define L3_ROUTE_TBL_IP6MC_MAX(_unit)           (_pL3Db[(_unit)]->HW.routeTable.ip6mc_max)

#define L3_ROUTE_TBL_IPUC_LEFT(_unit)           (L3_ROUTE_TBL_IPUC_MAX(_unit) - L3_ROUTE_TBL_IP_CNT(_unit))
#define L3_ROUTE_TBL_BLANK_LEFT(_unit)          (L3_ROUTE_TBL_BLANK_MAX(_unit) - L3_ROUTE_TBL_IPUC_MAX(_unit))
#define L3_ROUTE_TBL_IP6UC_LEFT(_unit)          (L3_ROUTE_TBL_IP6UC_MAX(_unit) - L3_ROUTE_TBL_BLANK_MAX(_unit) - (3*L3_ROUTE_TBL_IP6_CNT(_unit)))
#define L3_ROUTE_TBL_IPMC_LEFT(_unit)           (L3_ROUTE_TBL_IPMC_MAX(_unit) - L3_ROUTE_TBL_IP6UC_MAX(_unit) - (2*L3_ROUTE_TBL_IPMC_CNT(_unit)))
#define L3_ROUTE_TBL_IP6MC_LEFT(_unit)          (0)     // always left 0, need to expand

#define L3_ROUTE_TBL_BOUNDARY_SYNC(_unit)                                                                                   \
    do {                                                                                                                    \
        /* L3_ROUTE_TBL_IP6MC_MAX() is fixed once init, not need to update */                                               \
        L3_ROUTE_TBL_IPUC_MAX(_unit)    = 6*((L3_ROUTE_TBL_IP_CNT(_unit)+5)/6);                                             \
        L3_ROUTE_TBL_IPMC_MAX(_unit)    = (L3_ROUTE_TBL_IP6MC_MAX(_unit) - (6*L3_ROUTE_TBL_IP6MC_CNT(_unit)));              \
        L3_ROUTE_TBL_IP6UC_MAX(_unit)   = (L3_ROUTE_TBL_IPMC_MAX(_unit) - (6*(((2*L3_ROUTE_TBL_IPMC_CNT(_unit))+5)/6)));    \
        L3_ROUTE_TBL_BLANK_MAX(_unit)   = (L3_ROUTE_TBL_IP6UC_MAX(_unit) - (6*(((3*L3_ROUTE_TBL_IP6_CNT(_unit))+5)/6)));    \
    } while(0)
#endif

#define L3_ABS(_x, _y)                          (((_x) > (_y))? ((_x) - (_y)) : ((_y) - (_x)))

#define L3_INTF_IDX_IS_VALID(_unit, _intfIdx)   (FALSE != (_pL3Db[(_unit)]->intf[(_intfIdx)].valid))
#define L3_INTF_IDX_IS_L3(_unit, _intfIdx)      (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[(_unit)]->intf[(_intfIdx)].intf_id))
#define L3_INTF_IDX_IS_L2_TUNNEL(_unit, _intfIdx)      (DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(_pL3Db[(_unit)]->intf[(_intfIdx)].intf_id))

#define HASH_BIT_EXTRACT(_val, _lsb, _len)   (((_val) & (((1 << (_len)) - 1) << (_lsb))) >> (_lsb))

/* TBC */
#define L3_PATH_ID_IS_VALID(_unit, _pathId) (1) /* should check BITMAP only (due to share it with MPLS) */


/*
 * Function Declaration
 */
static inline int32 l3_util_macAddr_isMcast(rtk_mac_t *pMac)
{
    if ((NULL == pMac) || (!(pMac->octet[0] & 0x01)))
        return FALSE;

    return TRUE;
}

static inline int32 l3_util_intfMtu_set(uint32 unit, uint32 mtuIdx, uint32 mtuValue)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((mtuIdx >= DAL_MANGO_L3_IP_MTU_MAX), RT_ERR_OUT_OF_RANGE);

    /* update shadow */
    _pL3Db[unit]->HW.ip_mtu[mtuIdx].mtu_value = mtuValue;

    /* write to H/W (IPv4) */
    L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_INTF_IP_MTUr, REG_ARRAY_INDEX_NONE, \
        mtuIdx, MANGO_MTUf, mtuValue, "", errHandle, ret);

errHandle:
    return ret;
}

static inline int32 l3_util_intfIp6Mtu_set(uint32 unit, uint32 mtuIdx, uint32 mtuValue)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((mtuIdx >= DAL_MANGO_L3_IP6_MTU_MAX), RT_ERR_OUT_OF_RANGE);

    /* update shadow */
    _pL3Db[unit]->HW.ip6_mtu[mtuIdx].mtu_value = mtuValue;

    /* write to H/W (IPv6) */
    L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_INTF_IP6_MTUr, REG_ARRAY_INDEX_NONE, \
        mtuIdx, MANGO_MTUf, mtuValue, "", errHandle, ret);

errHandle:
    return ret;
}


/* L3 Router MAC - (RTK) rtk_l3_routerMacEntry_t to (DAL) dal_mango_l3_macEntry_t */
static inline int32 l3_util_rtkRouterMac2macEntry(dal_mango_l3_macEntry_t *pEntry, rtk_l3_routerMacEntry_t *pMac)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));

    pEntry->valid           = (pMac->enable)? 0x1 : 0x0;

    pEntry->port_type       = (pMac->port_type)? 0x1: 0x0;
    pEntry->bmsk_port_type  = pMac->port_type_mask;

    pEntry->port_id         = pMac->port_trunk_id;
    pEntry->bmsk_port_id    = pMac->port_trunk_id_mask;

    pEntry->intf_id         = pMac->intf_id;
    pEntry->bmsk_intf_id    = pMac->intf_id_mask;

    pEntry->mac             = pMac->mac;
    pEntry->bmsk_mac        = pMac->mac_mask;

    pEntry->lu_phase        = 0;
    pEntry->bmsk_lu_phase   = 0;

    pEntry->l3_intf         = 0;
    pEntry->bmsk_l3_intf    = 0;

    L3_ACTION_TO_VALUE(_actRouterMAC, pEntry->act, pMac->l3_act, "", errHandle, ret);

errHandle:
    return ret;
}

/* L3 Router MAC - (DAL) dal_mango_l3_macEntry_t to (RTK) rtk_l3_routerMacEntry_t */
static inline int32 l3_util_macEntry2rtkRouterMac(rtk_l3_routerMacEntry_t *pMac, dal_mango_l3_macEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pMac, 0x00, sizeof(rtk_l3_routerMacEntry_t));

    pMac->enable            = (pEntry->valid)? ENABLED : DISABLED;

    pMac->port_type         = (pEntry->port_type)? 0x1 : 0x0;
    pMac->port_type_mask    = pEntry->bmsk_port_type;

    pMac->port_trunk_id         = pEntry->port_id;
    pMac->port_trunk_id_mask    = pEntry->bmsk_port_id;

    pMac->intf_id           = pEntry->intf_id;
    pMac->intf_id_mask      = pEntry->bmsk_intf_id;

    pMac->mac               = pEntry->mac;
    pMac->mac_mask          = pEntry->bmsk_mac;

    L3_VALUE_TO_ACTION(_actRouterMAC, pMac->l3_act, pEntry->act, "", errHandle, ret);

errHandle:
    return ret;
}

/* L3 UC - (RTK) rtk_l3_host_t to (DAL) dal_mango_l3_hostEntry_t */
static inline int32 l3_util_rtkHost2hostEntry(dal_mango_l3_hostEntry_t *pEntry, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
    pEntry->valid = 1;
    pEntry->fmt = 0;
    pEntry->vrf_id = pHost->vrf_id;
    if (pHost->flags & RTK_L3_FLAG_IPV6)
    {
        pEntry->entry_type = DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST;
        pEntry->ip6 = pHost->ip_addr.ipv6;
    }
    else
    {
        pEntry->entry_type = DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST;
        pEntry->ip = pHost->ip_addr.ipv4;
    }
    pEntry->dst_null_intf = (pHost->flags & RTK_L3_FLAG_NULL_INTF)? 1 : 0;
    pEntry->ttl_dec = (pHost->flags & RTK_L3_FLAG_TTL_DEC_IGNORE)? 0 : 1;
    pEntry->ttl_chk = (pHost->flags & RTK_L3_FLAG_TTL_CHK_IGNORE)? 0 : 1;
    pEntry->qos_en = (pHost->flags & RTK_L3_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pHost->qos_pri;

    if (pHost->path_id)
    {
        if (DAL_MANGO_L3_PATH_ID_IS_ECMP(pHost->path_id))
        {
            pEntry->ecmp_en = 1;
            pEntry->nh_ecmp_idx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(pHost->path_id);
        }
        else
        {
            pEntry->ecmp_en = 0;
            pEntry->nh_ecmp_idx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pHost->path_id);
        }
    }

    L3_ACTION_TO_VALUE(_actHostUC, pEntry->act, pHost->fwd_act, "", errHandle, ret);

errHandle:
    return ret;
}

/* L3 UC - (DAL) dal_mango_l3_hostEntry_t to (RTK) rtk_l3_host_t */
static inline int32 l3_util_hostEntry2rtkHost(rtk_l3_host_t *pHost, dal_mango_l3_hostEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pEntry->fmt), RT_ERR_INPUT);

    /* clear memory */
    osal_memset(pHost, 0x00, sizeof(rtk_l3_host_t));
    pHost->vrf_id = pEntry->vrf_id;
    if (pEntry->entry_type == DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST)
    {
        /* IPv6 */
        pHost->flags |= RTK_L3_FLAG_IPV6;

        pHost->ip_addr.ipv6 = pEntry->ip6;
    }
    else if (pEntry->entry_type == DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST)
    {
        /* IPv4 */
        pHost->ip_addr.ipv4 = pEntry->ip;
    }
    else
    {
        return RT_ERR_INPUT;
    }

    if (pEntry->dst_null_intf)
    {
        pHost->flags |= RTK_L3_FLAG_NULL_INTF;
    }

    if (!pEntry->ttl_dec)
    {
        pHost->flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;
    }

    if (!pEntry->ttl_chk)
    {
        pHost->flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;
    }

    if (pEntry->qos_en)
    {
        pHost->flags |= RTK_L3_FLAG_QOS_ASSIGN;
        pHost->qos_pri = pEntry->qos_pri;
    }

    if (pEntry->hit)
    {
        pHost->flags |= RTK_L3_FLAG_HIT;
    }

    pHost->path_id = (pEntry->ecmp_en)? \
        DAL_MANGO_L3_ECMP_IDX_TO_PATH_ID(pEntry->nh_ecmp_idx) : \
        DAL_MANGO_L3_NH_IDX_TO_PATH_ID(pEntry->nh_ecmp_idx);

    L3_VALUE_TO_ACTION(_actHostUC, pHost->fwd_act, pEntry->act, "", errHandle, ret);

errHandle:
    return ret;
}

/* L3 UC - (RTK) rtk_l3_route_t to (DAL) dal_mango_l3_routeEntry_t */
static inline int32 l3_util_rtkRoute2routeEntry(dal_mango_l3_routeEntry_t *pEntry, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_routeEntry_t));
    pEntry->valid = 1;
    pEntry->fmt = 0;
    pEntry->bmsk_fmt = 0x1;
    pEntry->vrf_id = pRoute->vrf_id;
    pEntry->bmsk_vrf_id = 0xFF;
    if (pRoute->flags & RTK_L3_FLAG_IPV6)
    {
        pEntry->entry_type = DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST;
        pEntry->bmsk_entry_type = 0x3;
        pEntry->ip6 = pRoute->ip_addr.ipv6;
        pEntry->bmsk_ip6 = rt_util_ip6Length2Mask_ret(pRoute->prefix_len);
        pEntry->host_route = (pRoute->prefix_len >= RTK_L3_HOST_IP6_PREFIX_LENGTH)? 1 : 0;
        pEntry->dflt_route = (pRoute->prefix_len == 0)? 1 : 0;
    }
    else
    {
        pEntry->entry_type = DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST;
        pEntry->bmsk_entry_type = 0x3;
        pEntry->ip = pRoute->ip_addr.ipv4;
        pEntry->bmsk_ip = rt_util_ipLength2Mask_ret(pRoute->prefix_len);
        pEntry->host_route = (pRoute->prefix_len >= RTK_L3_HOST_IP_PREFIX_LENGTH)? 1 : 0;
        pEntry->dflt_route = (pRoute->prefix_len == 0)? 1 : 0;
    }
    pEntry->dst_null_intf = (pRoute->flags & RTK_L3_FLAG_NULL_INTF)? 1 : 0;
    pEntry->ttl_dec = (pRoute->flags & RTK_L3_FLAG_TTL_DEC_IGNORE)? 0 : 1;
    pEntry->ttl_chk = (pRoute->flags & RTK_L3_FLAG_TTL_CHK_IGNORE)? 0 : 1;
    pEntry->qos_en = (pRoute->flags & RTK_L3_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pRoute->qos_pri;

    if (pRoute->path_id)
    {
        if (DAL_MANGO_L3_PATH_ID_IS_ECMP(pRoute->path_id))
        {
            pEntry->ecmp_en = 1;
            pEntry->nh_ecmp_idx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(pRoute->path_id);
        }
        else
        {
            pEntry->ecmp_en = 0;
            pEntry->nh_ecmp_idx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pRoute->path_id);
        }
    }

    L3_ACTION_TO_VALUE(_actRouteUC, pEntry->act, pRoute->fwd_act, "", errHandle, ret);

errHandle:
    return ret;
}

/* L3 UC - (DAL) dal_mango_l3_routeEntry_t to (RTK) rtk_l3_route_t */
static inline int32 l3_util_routeEntry2rtkRoute(rtk_l3_route_t *pRoute, dal_mango_l3_routeEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pEntry->fmt), RT_ERR_INPUT);

    /* clear memory */
    osal_memset(pRoute, 0x00, sizeof(rtk_l3_route_t));
    pRoute->vrf_id = pEntry->vrf_id;
    if (pEntry->entry_type == DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST)
    {
        /* IPv6 */
        pRoute->flags |= RTK_L3_FLAG_IPV6;
        pRoute->ip_addr.ipv6 = pEntry->ip6;
        pRoute->prefix_len = rt_util_ip6Mask2Length_ret(&pEntry->bmsk_ip6);
    }
    else if (pEntry->entry_type == DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST)
    {
        /* IPv4 */
        pRoute->ip_addr.ipv4 = pEntry->ip;
        pRoute->prefix_len = rt_util_ipMask2Length_ret(pEntry->bmsk_ip);
    }
    else
    {
        return RT_ERR_INPUT;
    }

    if (pEntry->dst_null_intf)
    {
        pRoute->flags |= RTK_L3_FLAG_NULL_INTF;
    }

    if (!pEntry->ttl_dec)
    {
        pRoute->flags |= RTK_L3_FLAG_TTL_DEC_IGNORE;
    }

    if (!pEntry->ttl_chk)
    {
        pRoute->flags |= RTK_L3_FLAG_TTL_CHK_IGNORE;
    }

    if (pEntry->qos_en)
    {
        pRoute->flags |= RTK_L3_FLAG_QOS_ASSIGN;
        pRoute->qos_pri = pEntry->qos_pri;
    }

    if (pEntry->hit)
    {
        pRoute->flags |= RTK_L3_FLAG_HIT;
    }

    pRoute->path_id = (pEntry->ecmp_en)? \
        DAL_MANGO_L3_ECMP_IDX_TO_PATH_ID(pEntry->nh_ecmp_idx) : \
        DAL_MANGO_L3_NH_IDX_TO_PATH_ID(pEntry->nh_ecmp_idx);

    L3_VALUE_TO_ACTION(_actRouteUC, pRoute->fwd_act, pEntry->act, "", errHandle, ret);

errHandle:
    return ret;
}

/* get L2 FID with VID+MAC based on its VLAN configuration */
static int32 l3_util_vlanMacFid_get(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMacAddr, rtk_fid_t *pFid)
{
    int32           ret = RT_ERR_OK;
    int32           boolMcast;
    int32           boolIVL;
    vlan_entry_t    vlan_entry;
    uint32          temp_var;
    uint32          field;

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMacAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);

    /* is mulcaist address ? */
    boolMcast = (pMacAddr->octet[0] & 0x01)? TRUE : FALSE;

    osal_memset(&vlan_entry, 0x00, sizeof(vlan_entry));

    /*** get VLAN entry from chip ***/
    if ((ret = table_read(unit, MANGO_VLANt, vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if (FALSE == boolMcast)
    {
        /* get unicast lookup mode */
        if ((ret = table_field_get(unit, MANGO_VLANt, MANGO_VLAN_L2_HKEY_UCASTtf, \
                                   &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L3|MOD_DAL), "");
            return ret;
        }

        boolIVL = (0 == temp_var)? TRUE : FALSE;
    }
    else
    {
        /* get multicast lookup mode */
        if ((ret = table_field_get(unit, MANGO_VLANt, MANGO_VLAN_L2_HKEY_MCASTtf, \
                                   &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L3|MOD_DAL), "");
            return ret;
        }

        boolIVL = (0 == temp_var)? TRUE : FALSE;
    }

    if (TRUE == boolIVL)
    {
        /* IVL mode, then use the VLAN ID */
        *pFid = vid;
    }
    else
    {
        /* get FID source configuration */
        if ((ret = reg_field_read(unit, MANGO_VLAN_CTRLr, MANGO_SVL_FID_SRCf, \
                                  &temp_var) ) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L3|MOD_DAL), "");
            return ret;
        }

        if (0 == temp_var)
        {
            /* MAC mode */
            field = (FALSE == boolMcast)? MANGO_UC_SVL_FIDf : MANGO_MC_SVL_FIDf;

            /* get FID from CHIP */
            if ((ret = reg_field_read(unit, MANGO_VLAN_CTRLr, field, pFid) ) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L3|MOD_DAL), "");
                return ret;
            }
        }
        else
        {
            /* VLAN mode */

            /* get FID from entry */
            if ((ret = table_field_get(unit, MANGO_VLANt, MANGO_VLAN_L2_TNL_LST_IDXtf, \
                                       &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L3|MOD_DAL), "");
                return ret;
            }

            *pFid = temp_var;
        }
    }

    return ret;
}

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
int32
dal_mango_l3Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->l3_init = dal_mango_l3_init;
    pMapper->l3_info_get = dal_mango_l3_info_get;
    pMapper->l3_routerMacEntry_get = dal_mango_l3_routerMacEntry_get;
    pMapper->l3_routerMacEntry_set = dal_mango_l3_routerMacEntry_set;
    pMapper->l3_intf_create = dal_mango_l3_intf_create;
    pMapper->l3_intf_destroy = dal_mango_l3_intf_destroy;
    pMapper->l3_intf_destroyAll = dal_mango_l3_intf_destroyAll;
    pMapper->l3_intf_get = dal_mango_l3_intf_get;
    pMapper->l3_intf_set = dal_mango_l3_intf_set;
    pMapper->l3_intfStats_get = dal_mango_l3_intfStats_get;
    pMapper->l3_intfStats_reset = dal_mango_l3_intfStats_reset;
    pMapper->l3_vrrp_add = dal_mango_l3_vrrp_add;
    pMapper->l3_vrrp_del = dal_mango_l3_vrrp_del;
    pMapper->l3_vrrp_delAll = dal_mango_l3_vrrp_delAll;
    pMapper->l3_vrrp_get = dal_mango_l3_vrrp_get;
    pMapper->l3_nextHop_create = dal_mango_l3_nextHop_create;
    pMapper->l3_nextHop_destroy = dal_mango_l3_nextHop_destroy;
    pMapper->l3_nextHop_get = dal_mango_l3_nextHop_get;
    pMapper->l3_nextHopPath_find = dal_mango_l3_nextHopPath_find;
    pMapper->l3_ecmp_create = dal_mango_l3_ecmp_create;
    pMapper->l3_ecmp_destroy = dal_mango_l3_ecmp_destroy;
    pMapper->l3_ecmp_get = dal_mango_l3_ecmp_get;
    pMapper->l3_ecmp_add = dal_mango_l3_ecmp_add;
    pMapper->l3_ecmp_del = dal_mango_l3_ecmp_del;
    pMapper->l3_ecmp_find = dal_mango_l3_ecmp_find;
    pMapper->l3_host_add = dal_mango_l3_host_add;
    pMapper->l3_host_del = dal_mango_l3_host_del;
    pMapper->l3_host_del_byNetwork = dal_mango_l3_host_del_byNetwork;
    pMapper->l3_host_del_byIntfId = dal_mango_l3_host_del_byIntfId;
    pMapper->l3_host_delAll = dal_mango_l3_host_delAll;
    pMapper->l3_host_find = dal_mango_l3_host_find;
    pMapper->l3_hostConflict_get = dal_mango_l3_hostConflict_get;
    pMapper->l3_host_age = dal_mango_l3_host_age;
    pMapper->l3_host_getNext = dal_mango_l3_host_getNext;
    pMapper->l3_route_add = dal_mango_l3_route_add;
    pMapper->l3_route_del = dal_mango_l3_route_del;
    pMapper->l3_route_get = dal_mango_l3_route_get;
    pMapper->l3_route_del_byIntfId = dal_mango_l3_route_del_byIntfId;
    pMapper->l3_route_delAll = dal_mango_l3_route_delAll;
    pMapper->l3_route_age = dal_mango_l3_route_age;
    pMapper->l3_route_getNext = dal_mango_l3_route_getNext;
    pMapper->l3_globalCtrl_get = dal_mango_l3_globalCtrl_get;
    pMapper->l3_globalCtrl_set = dal_mango_l3_globalCtrl_set;
    pMapper->l3_intfCtrl_get = dal_mango_l3_intfCtrl_get;
    pMapper->l3_intfCtrl_set = dal_mango_l3_intfCtrl_set;
    pMapper->l3_portCtrl_get = dal_mango_l3_portCtrl_get;
    pMapper->l3_portCtrl_set = dal_mango_l3_portCtrl_set;

    return RT_ERR_OK;
}


static int32 _dal_mango_l3_reset(uint32 unit);

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
int32
dal_mango_l3_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  value;
    uint32  host_hash_alg_0, host_hash_alg_1;
    dal_mango_l3_nhEntry_t nexthop;
    l3_nexthop_entry_t nhEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(l3_init[unit]);
    l3_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    l3_sem[unit] = osal_sem_mutex_create();
    if (0 == l3_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* create internal semaphore */
    l3_int_sem[unit] = osal_sem_mutex_create();
    if (0 == l3_int_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "internal semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* create hw-lookup semaphore */
    l3_hwLU_sem[unit] = osal_sem_mutex_create();
    if (0 == l3_hwLU_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "hwLookup semaphore create failed");
        return RT_ERR_FAILED;
    }

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    /* create l3-route table semaphore */
    l3_route_sem[unit] = osal_sem_mutex_create();
    if (0 == l3_route_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "l3-route semaphore create failed");
        return RT_ERR_FAILED;
    }
#endif

    /* allocate memory that we need */
    if ((_pL3Db[unit] = osal_alloc(sizeof(dal_mango_l3_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "out of memory");
        return RT_ERR_FAILED;
    }
    osal_memset(_pL3Db[unit], 0x00, sizeof(dal_mango_l3_drvDb_t));

    /* Initial Openflow cutline  */
    L3_ROUTE_IDX_MAX(unit) = (RTK_DEFAULT_L3_OPENFLOW_CUTLINE) - 1;

    MANGO_L3_DBG_PRINTF(1, "L3_ROUTE_IDX_MAX = %u (RTK_DEFAULT_L3_OPENFLOW_CUTLINE = %u)\n", \
        L3_ROUTE_IDX_MAX(unit), RTK_DEFAULT_L3_OPENFLOW_CUTLINE);

    /* Update L3 route table size */
    if ((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID) &&
        (L3_ROUTE_IDX_MAX(unit) >= DAL_MANGO_L3_ROUTE_TBL_SIZE_FOR_RTL9311E))
    {
        L3_ROUTE_IDX_MAX(unit) = (DAL_MANGO_L3_ROUTE_TBL_SIZE_FOR_RTL9311E) - 1;
    }

#if MANGO_L3_ROUTE_RSVMC_ENTRY
    L3_ROUTE_IDX_MAX(unit) -= 12;
#endif

#if MANGO_L3_ROUTE_IPMC_SIZE
    /* IPMC space */
    _pL3Db[unit]->l3_route_ipmc_idx_base    = L3_ROUTE_IDX_MAX(unit) - MANGO_L3_ROUTE_IPMC_SIZE + 1;
    _pL3Db[unit]->l3_route_ipmc_size        = MANGO_L3_ROUTE_IPMC_SIZE;
    L3_ROUTE_IDX_MAX(unit) -= MANGO_L3_ROUTE_IPMC_SIZE;
#endif

#if defined(CONFIG_SDK_FPGA_PLATFORM)
    if (L3_ROUTE_IDX_MAX(unit) > (4 * 6) - 1)
        L3_ROUTE_IDX_MAX(unit) = (4 * 6) - 1;
#endif

    L3_ROUTE_TBL_SIZE(unit) = L3_ROUTE_IDX_MAX(unit) - L3_ROUTE_IDX_MIN(unit) + 1;

#ifdef MANGO_L3_ROUTE_IPMC_DYNAMIC
    _pL3Db[unit]->HW.routeTable.ip6mc_max   = L3_ROUTE_TBL_SIZE(unit);
    L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
#endif

    /* RTK_DEFAULT_INTF is reserved for all VLANs as default L2 bridging interface */
    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* MTU value for IPv4 and IPv6 */
        RT_ERR_HDL(l3_util_intfMtu_set(unit, DAL_MANGO_L3_RESERVED_INTF_MTU_IDX, RTK_DEFAULT_L3_INTF_MTU), errHandle, ret);
        RT_ERR_HDL(l3_util_intfIp6Mtu_set(unit, DAL_MANGO_L3_RESERVED_INTF_MTU_IDX, RTK_DEFAULT_L3_INTF_MTU), errHandle, ret);
    }
    BITMAP_SET(_pL3Db[unit]->HW.intf_used, DAL_MANGO_L3_RESERVED_INTF_IDX);
    _pL3Db[unit]->HW.intf_used_count = 1;
    _pL3Db[unit]->HW.ip_mtu[DAL_MANGO_L3_RESERVED_INTF_MTU_IDX].ref_count = 1;
    _pL3Db[unit]->HW.ip_mtu_used_count = 1;
    _pL3Db[unit]->HW.ip6_mtu[DAL_MANGO_L3_RESERVED_INTF_MTU_IDX].ref_count = 1;
    _pL3Db[unit]->HW.ip6_mtu_used_count = 1;

    _pL3Db[unit]->intf[DAL_MANGO_L3_RESERVED_INTF_IDX].valid = TRUE;

    /* reserve nexthop index 0 */
    BITMAP_SET(_pL3Db[unit]->HW.nexthop_used, DAL_MANGO_L3_RESERVED_NEXTHOP_IDX);
    _pL3Db[unit]->HW.nexthop_used_count = 1;
    _pL3Db[unit]->nexthop[DAL_MANGO_L3_RESERVED_NEXTHOP_IDX].valid = FALSE; /* treat as invalid */

    /* write invalid nexthop entry into chip */
    osal_memset(&nexthop, 0x00, sizeof(dal_mango_l3_nhEntry_t));
    nexthop.dmac_idx = DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX;
    nexthop.l3_egr_intf_idx = DAL_MANGO_L3_RESERVED_INTF_IDX;
    osal_memset(&nhEntry, 0x00, sizeof(l3_nexthop_entry_t));
    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_NEXTHOPt, \
            MANGO_L3_NEXTHOP_DMAC_IDXtf, nexthop.dmac_idx, nhEntry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_NEXTHOPt, \
            MANGO_L3_NEXTHOP_L3_EGR_INTF_IDXtf, nexthop.l3_egr_intf_idx, nhEntry, "", errHandle, ret);
        L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_NEXTHOPt, \
            DAL_MANGO_L3_RESERVED_NEXTHOP_IDX, nhEntry, "", errHandle, ret);

        /* enable L3-related TCAM blocks */
        value = 0x1;    /* enable Router MAC */
        reg_field_write(unit, MANGO_ALE_L3_MISC_CTRLr, MANGO_ROUTER_MAC_TCAM_ENf, &value);
        value = 0x3F;   /* enable Prefix TCAM [5:0] */
        reg_field_write(unit, MANGO_ALE_L3_MISC_CTRLr, MANGO_L3_TCAM_BLK_ENf, &value);
    }

    /* configure default hash algorithm */
    host_hash_alg_0 = 0;
    host_hash_alg_1 = 1;
    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_UC_HASH_ALG_SEL_0f, host_hash_alg_0, "", errHandle, ret);
        L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_UC_HASH_ALG_SEL_1f, host_hash_alg_1, "", errHandle, ret);
    }

    // ECMP hash-table rebuild function (enable by default)
    _pL3Db[unit]->ecmp_hash_tbl_manual = ENABLED;

    /* reference count check state init */
    _pL3Db[unit]->refer_cnt_chk_en = MANGO_L3_REFCNT_CHK_DEFAULT;

    /* set init flag to complete init */
    l3_init[unit] = INIT_COMPLETED;

#if MANGO_L3_ROUTE_RSVMC_ENTRY
    RT_ERR_HDL(dal_mango_l3_route_rsv4Mcast_add(unit), errHandle, ret);
    RT_ERR_HDL(dal_mango_l3_route_rsv6Mcast_add(unit), errHandle, ret);
#endif

    return RT_ERR_OK;

errHandle:
    return ret;
}   /* end of dal_mango_l3_init */

/* Function Name:
 *      _dal_mango_l3_macEntry_get
 * Description:
 *      Get the specifed L3 MAC entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the entry
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
int32
_dal_mango_l3_macEntry_get(uint32 unit, uint32 index, dal_mango_l3_macEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_router_mac_entry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > DAL_MANGO_L3_MAC_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

#ifdef MANGO_L3_ROUTER_MAC_ENTRY_SHADOW
    osal_memcpy(&macEntry, &_pL3Db[unit]->HW.mac[index].mac_entry, sizeof(l3_router_mac_entry_t));
#else
    /* read from chip (MAC and interface are 1-to-1 mapping) */
    L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, index, macEntry, "", errHandle, ret);
#endif

    /* load data */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_VALIDtf, pEntry->valid, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_PORT_TYPEtf, pEntry->port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_PORT_IDtf, pEntry->port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_INTF_IDtf, pEntry->intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_MACtf, pEntry->mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_LU_PHASEtf, pEntry->lu_phase, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_L3_INTFtf, pEntry->l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_PORT_TYPEtf, pEntry->bmsk_port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_PORT_IDtf, pEntry->bmsk_port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_INTF_IDtf, pEntry->bmsk_intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_MACtf, pEntry->bmsk_mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_LU_PHASEtf, pEntry->bmsk_lu_phase, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_L3_INTFtf, pEntry->bmsk_l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_ACTtf, pEntry->act, macEntry, "", errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_mango_l3_macEntry_set
 * Description:
 *      Set the specifed L3 MAC entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer to the entry
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
int32
_dal_mango_l3_macEntry_set(uint32 unit, uint32 index, dal_mango_l3_macEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_router_mac_entry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > DAL_MANGO_L3_INTF_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* prepare data */
    osal_memset(&macEntry, 0x00, sizeof(l3_router_mac_entry_t));
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_VALIDtf, pEntry->valid, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_PORT_TYPEtf, pEntry->port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_PORT_IDtf, pEntry->port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_INTF_IDtf, pEntry->intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_MACtf, pEntry->mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_LU_PHASEtf, pEntry->lu_phase, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_L3_INTFtf, pEntry->l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_PORT_TYPEtf, pEntry->bmsk_port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_PORT_IDtf, pEntry->bmsk_port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_INTF_IDtf, pEntry->bmsk_intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_MACtf, pEntry->bmsk_mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_LU_PHASEtf, pEntry->bmsk_lu_phase, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_BMSK_L3_INTFtf, pEntry->bmsk_l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, \
        MANGO_L3_ROUTER_MAC_ACTtf, pEntry->act, macEntry, "", errHandle, ret);

    /* write into chip (MAC and interface are 1-to-1 mapping) */
    L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_ROUTER_MACt, index, macEntry, "", errHandle, ret);

#ifdef MANGO_L3_ROUTER_MAC_ENTRY_SHADOW
    osal_memcpy(&_pL3Db[unit]->HW.mac[index].mac_entry, &macEntry, sizeof(l3_router_mac_entry_t));
#endif

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_INPUT        - invalid input
 *      RT_ERR_ENTRY_EXIST  - entry exists
 *      RT_ERR_NULL_POINTER - null pointer
 *      RT_ERR_TBL_FULL     - table is full
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_mango_l3_intfEntry_alloc(uint32 unit, uint32 *pIndex, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pIndex=%p,flags=0x%08x", unit, pIndex, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    if (_pL3Db[unit]->HW.intf_used_count >= HAL_MAX_NUM_OF_INTF(unit))
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }

    if (flags & DAL_MANGO_L3_API_FLAG_WITH_ID)
    {
        entryIdx = *pIndex; /* caller specified */

        if ((DAL_MANGO_L3_RESERVED_INTF_IDX == entryIdx) ||
            (DAL_MANGO_L3_INTF_MAX <= entryIdx))
        {
            ret = RT_ERR_INPUT;
            goto errInput;
        }

        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, entryIdx))
        {
            _pL3Db[unit]->intf[entryIdx].intf_id = \
                (flags & DAL_MANGO_L3_API_FLAG_MOD_TUNNEL)? \
                    (entryIdx | DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL) : \
                    (entryIdx);

            BITMAP_SET(_pL3Db[unit]->HW.intf_used, entryIdx);
            _pL3Db[unit]->HW.intf_used_count += 1;

            goto errOk;
        }

        ret = RT_ERR_ENTRY_EXIST;
        goto errEntryExist;
    }

    /* search an empty entry */
    for (entryIdx=0; entryIdx<DAL_MANGO_L3_INTF_MAX; entryIdx++)
    {
        /* interface (DAL_MANGO_L3_RESERVED_INTF_IDX) which is reserved for default L2 interface */
        if (DAL_MANGO_L3_RESERVED_INTF_IDX == entryIdx)
            continue;

        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, entryIdx))
        {
            _pL3Db[unit]->intf[entryIdx].intf_id = \
                (flags & DAL_MANGO_L3_API_FLAG_MOD_TUNNEL)? \
                    (entryIdx | DAL_MANGO_L3_INTF_ID_FLAG_TUNNEL) : \
                    (entryIdx);

            BITMAP_SET(_pL3Db[unit]->HW.intf_used, entryIdx);
            _pL3Db[unit]->HW.intf_used_count += 1;
            MANGO_L3_DBG_PRINTF(3, "entryIdx = %u\n", entryIdx);

            *pIndex = entryIdx;

            goto errOk;
        }
    }

    ret = RT_ERR_TBL_FULL;

errInput:
errEntryExist:
errTblFull:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_intfEntry_free(uint32 unit, uint32 index, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,flags=0x%08x", unit, index, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, index))
    {
        /* caller check */
        if (flags & DAL_MANGO_L3_API_FLAG_MOD_TUNNEL)
        {
            /* Tunnel interface */
            if (!DAL_MANGO_L3_INTF_ID_IS_TUNNEL(_pL3Db[unit]->intf[index].intf_id))
            {
                ret = RT_ERR_INPUT; /* called by the wrong module */
                goto errInput;
            }
        }
        else if (DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(_pL3Db[unit]->intf[index].intf_id))
        {
            ret = RT_ERR_INPUT; /* called by the wrong module */
            goto errInput;
        }

        /* release entry */
        BITMAP_CLEAR(_pL3Db[unit]->HW.intf_used, index);
        _pL3Db[unit]->HW.intf_used_count -= 1;

        MANGO_L3_DBG_PRINTF(3, "index = %u\n", index);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

errInput:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_intfIgrEntry_get(uint32 unit, uint32 index, dal_mango_l3_intfIgrEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_igr_intf_entry_t igrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* read from chip */
    L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_IGR_INTFt, index, igrIntf, "", errHandle, ret);

    /* load data */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_VRF_IDtf, pEntry->vrf_id, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPUC_ROUTE_ENtf, pEntry->ipuc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6UC_ROUTE_ENtf, pEntry->ip6uc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ROUTE_ENtf, pEntry->ipmc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6MC_ROUTE_ENtf, pEntry->ip6mc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ROUTE_LU_MIS_ACTtf, pEntry->ipmc_route_lu_mis_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6MC_ROUTE_LU_MIS_ACTtf, pEntry->ip6mc_route_lu_mis_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ACT_224_0_0_Xtf, pEntry->ipmc_act_224_0_0_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ACT_224_0_1_Xtf, pEntry->ipmc_act_224_0_1_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ACT_239_X_X_Xtf, pEntry->ipmc_act_239_x_x_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6MC_ACT_0000_00XXtf, pEntry->ip6mc_act_0000_00xx, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_MLD_ACT_0_X_Xtf, pEntry->ip6_mld_act_0_x_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_MLD_ACT_DB8_X_Xtf, pEntry->ip6_mld_act_db8_x_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_ND_ACTtf, pEntry->ip6_nd_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_FAIL_ACTtf, pEntry->ip_urpf_fail_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_CHK_ENtf, pEntry->ip_urpf_chk_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_DFLT_ROUTE_ENtf, pEntry->ip_urpf_dflt_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_CHK_MODEtf, pEntry->ip_urpf_chk_mode, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_FAIL_ACTtf, pEntry->ip6_urpf_fail_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_CHK_ENtf, pEntry->ip6_urpf_chk_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_DFLT_ROUTE_ENtf, pEntry->ip6_urpf_dflt_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_CHK_MODEtf, pEntry->ip6_urpf_chk_mode, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_MC_KEY_SELtf, pEntry->mc_key_sel, igrIntf, "", errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_intfIgrEntry_set(uint32 unit, uint32 index, dal_mango_l3_intfIgrEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_igr_intf_entry_t igrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* prepare data */
    osal_memset(&igrIntf, 0x00, sizeof(l3_igr_intf_entry_t));
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_VRF_IDtf, pEntry->vrf_id, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPUC_ROUTE_ENtf, pEntry->ipuc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6UC_ROUTE_ENtf, pEntry->ip6uc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ROUTE_ENtf, pEntry->ipmc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6MC_ROUTE_ENtf, pEntry->ip6mc_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ROUTE_LU_MIS_ACTtf, pEntry->ipmc_route_lu_mis_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6MC_ROUTE_LU_MIS_ACTtf, pEntry->ip6mc_route_lu_mis_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ACT_224_0_0_Xtf, pEntry->ipmc_act_224_0_0_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ACT_224_0_1_Xtf, pEntry->ipmc_act_224_0_1_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IPMC_ACT_239_X_X_Xtf, pEntry->ipmc_act_239_x_x_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6MC_ACT_0000_00XXtf, pEntry->ip6mc_act_0000_00xx, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_MLD_ACT_0_X_Xtf, pEntry->ip6_mld_act_0_x_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_MLD_ACT_DB8_X_Xtf, pEntry->ip6_mld_act_db8_x_x, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_ND_ACTtf, pEntry->ip6_nd_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_FAIL_ACTtf, pEntry->ip_urpf_fail_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_CHK_ENtf, pEntry->ip_urpf_chk_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_DFLT_ROUTE_ENtf, pEntry->ip_urpf_dflt_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP_URPF_CHK_MODEtf, pEntry->ip_urpf_chk_mode, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_FAIL_ACTtf, pEntry->ip6_urpf_fail_act, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_CHK_ENtf, pEntry->ip6_urpf_chk_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_DFLT_ROUTE_ENtf, pEntry->ip6_urpf_dflt_route_en, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_IP6_URPF_CHK_MODEtf, pEntry->ip6_urpf_chk_mode, igrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_IGR_INTFt, \
        MANGO_L3_IGR_INTF_MC_KEY_SELtf, pEntry->mc_key_sel, igrIntf, "", errHandle, ret);

    /* write into chip */
    L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_IGR_INTFt, index, igrIntf, "", errHandle, ret);

    /* sync shadow info */
    _pL3Db[unit]->intf[index].vrf_id = pEntry->vrf_id;

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_intfEgrEntry_get(uint32 unit, uint32 index, dal_mango_l3_intfEgrEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_egr_intf_entry_t egrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* read from chip */
    L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_EGR_INTFt, index, egrIntf, "", errHandle, ret);

    /* load data */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_DST_VIDtf, pEntry->dst_vid, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_SMAC_ADDRtf, pEntry->smac_addr, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP_MTU_IDXtf, pEntry->ip_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6_MTU_IDXtf, pEntry->ip6_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IPMC_TTL_SCOPEtf, pEntry->ipmc_ttl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6MC_HL_SCOPEtf, pEntry->ip6mc_hl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, pEntry->ip_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, pEntry->ip6_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip6_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_TUNNEL_IFtf, pEntry->tunnel_if, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_TUNNEL_IDXtf, pEntry->tunnel_idx, egrIntf, "", errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_intfEgrEntry_set(uint32 unit, uint32 index, dal_mango_l3_intfEgrEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_egr_intf_entry_t egrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* prepare data */
    osal_memset(&egrIntf, 0x00, sizeof(l3_egr_intf_entry_t));
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_DST_VIDtf, pEntry->dst_vid, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_SMAC_ADDRtf, pEntry->smac_addr, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP_MTU_IDXtf, pEntry->ip_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6_MTU_IDXtf, pEntry->ip6_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IPMC_TTL_SCOPEtf, pEntry->ipmc_ttl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6MC_HL_SCOPEtf, pEntry->ip6mc_hl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, pEntry->ip_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, pEntry->ip6_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip6_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_TUNNEL_IFtf, pEntry->tunnel_if, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTFt, \
        MANGO_L3_EGR_INTF_TUNNEL_IDXtf, pEntry->tunnel_idx, egrIntf, "", errHandle, ret);

    /* write into chip */
    L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_EGR_INTFt, index, egrIntf, "", errHandle, ret);

    /* sync shadow info */
    _pL3Db[unit]->intf[index].vid           = pEntry->dst_vid;
    _pL3Db[unit]->intf[index].ip_mtu_idx    = pEntry->ip_mtu_idx;
    _pL3Db[unit]->intf[index].ip6_mtu_idx   = pEntry->ip6_mtu_idx;

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_intfEntry_get(uint32 unit, uint32 index, dal_mango_l3_intfEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    RT_ERR_HDL(_dal_mango_l3_intfIgrEntry_get(unit, index, &pEntry->igrIntf, flags), errHandle, ret);
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_get(unit, index, &pEntry->egrIntf, flags), errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:

    return ret;
}

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
int32
_dal_mango_l3_intfEntry_set(uint32 unit, uint32 index, dal_mango_l3_intfEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index > HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    RT_ERR_HDL(_dal_mango_l3_intfIgrEntry_set(unit, index, &pEntry->igrIntf, flags), errHandle, ret);
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_set(unit, index, &pEntry->egrIntf, flags), errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_mango_l3_mtuEntry_alloc
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
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_mango_l3_mtuEntry_alloc(uint32 unit, uint32 mtu, uint32 *pIdx, uint32 flags)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,mtu=%d,pIdx=%p,flags=0x%08x", unit, mtu, pIdx, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitable MTU entry */
    for (idx=0; idx<DAL_MANGO_L3_IP_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip_mtu[idx].mtu_value == mtu)
            {
                _pL3Db[unit]->HW.ip_mtu[idx].ref_count += 1;
                *pIdx = idx;
                MANGO_L3_DBG_PRINTF(3, "%s():%d idx = %u, mtu_value = %u, ref_count = %u\n", \
                    __FUNCTION__, __LINE__, idx, \
                    _pL3Db[unit]->HW.ip_mtu[idx].mtu_value, \
                    _pL3Db[unit]->HW.ip_mtu[idx].ref_count);
                goto errOk;
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new MTU entry */
    if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].mtu_value = mtu;
        _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip_mtu_used_count += 1;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, emptyEntryIdx, mtu), errExit, ret);

        *pIdx = emptyEntryIdx;
        MANGO_L3_DBG_PRINTF(3, "emptyEntry = %u, mtu_value = %u, ref_count = %u\n", emptyEntryIdx, \
            _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].mtu_value, \
            _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].ref_count);
        goto errOk;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
    }

errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_mtuEntry_free(uint32 unit, uint32 idx, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d,flags=0x%08x", unit, idx, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (idx >= DAL_MANGO_L3_IP_MTU_MAX)
    {
        ret = RT_ERR_OUT_OF_RANGE;
        goto errInput;
    }

    /* recycle the MTU entry */
    if (_pL3Db[unit]->HW.ip_mtu[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip_mtu[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip_mtu[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.ip_mtu_used_count -= 1;
        }
        MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", \
            idx, _pL3Db[unit]->HW.ip_mtu[idx].mtu_value, _pL3Db[unit]->HW.ip_mtu[idx].ref_count);
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

errEntryNotFound:
errInput:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_mango_l3_mtuEntry_realloc(uint32 unit, uint32 newMtu, uint32 *pIdx, uint32 flags)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIdx;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,newMtu=%d,pIdx=%p,flags=0x%08x", unit, newMtu, pIdx, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;
    /* search a suitable MTU entry */
    for (idx=0; idx<DAL_MANGO_L3_IP_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip_mtu[idx].mtu_value == newMtu)
            {
                _pL3Db[unit]->HW.ip_mtu[idx].ref_count += 1;
                *pIdx = idx;
                MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", idx, \
                    _pL3Db[unit]->HW.ip_mtu[idx].mtu_value, \
                    _pL3Db[unit]->HW.ip_mtu[idx].ref_count);

                /* release the old entry */
                goto freeOldIpMtuEntry;
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new MTU entry */
    if (_pL3Db[unit]->HW.ip_mtu[oldIdx].ref_count == 1)
    {
        /* just change the MTU value with the original MTU entry */
        _pL3Db[unit]->HW.ip_mtu[oldIdx].mtu_value = newMtu;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, oldIdx, newMtu), errExit, ret);

        MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", oldIdx, \
            _pL3Db[unit]->HW.ip_mtu[oldIdx].mtu_value, \
            _pL3Db[unit]->HW.ip_mtu[oldIdx].ref_count);

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].mtu_value = newMtu;
        _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip_mtu_used_count += 1;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, emptyEntryIdx, newMtu), errExit, ret);

        MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", \
            emptyEntryIdx, \
            _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].mtu_value, \
            _pL3Db[unit]->HW.ip_mtu[emptyEntryIdx].ref_count);

        *pIdx = emptyEntryIdx;
        goto freeOldIpMtuEntry;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
        goto errIntfMtuVarietyExceeds;
    }

freeOldIpMtuEntry:
    if (_pL3Db[unit]->HW.ip_mtu[oldIdx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip_mtu[oldIdx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip_mtu[oldIdx].ref_count == 0)
            _pL3Db[unit]->HW.ip_mtu_used_count -= 1;
        goto errOk;
    }

errIntfMtuVarietyExceeds:
errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_mango_l3_mtuIp6Entry_alloc
 * Description:
 *      Allocate an IPv6 MTU entry index for reference.
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
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_mango_l3_mtuIp6Entry_alloc(uint32 unit, uint32 mtu, uint32 *pIdx, uint32 flags)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,mtu=%d,pIdx=%p,flags=0x%08x", unit, mtu, pIdx, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitable MTU entry */
    for (idx=0; idx<DAL_MANGO_L3_IP6_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip6_mtu[idx].mtu_value == mtu)
            {
                _pL3Db[unit]->HW.ip6_mtu[idx].ref_count += 1;
                *pIdx = idx;
                MANGO_L3_DBG_PRINTF(3, "%s():%d idx = %u, mtu_value = %u, ref_count = %u\n", \
                    __FUNCTION__, __LINE__, idx, \
                    _pL3Db[unit]->HW.ip6_mtu[idx].mtu_value, \
                    _pL3Db[unit]->HW.ip6_mtu[idx].ref_count);
                goto errOk;
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new MTU entry */
    if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].mtu_value = mtu;
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip6_mtu_used_count += 1;

        RT_ERR_HDL(l3_util_intfIp6Mtu_set(unit, emptyEntryIdx, mtu), errExit, ret);

        *pIdx = emptyEntryIdx;
        MANGO_L3_DBG_PRINTF(3, "emptyEntry = %u, mtu_value = %u, ref_count = %u\n", emptyEntryIdx, \
            _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].mtu_value, \
            _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].ref_count);
        goto errOk;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
    }

errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_mango_l3_mtuIp6Entry_free
 * Description:
 *      Release an IPv6 MTU entry index reference.
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
int32
_dal_mango_l3_mtuIp6Entry_free(uint32 unit, uint32 idx, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d,flags=0x%08x", unit, idx, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (idx >= DAL_MANGO_L3_IP6_MTU_MAX)
    {
        ret = RT_ERR_OUT_OF_RANGE;
        goto errInput;
    }

    /* recycle the MTU entry */
    if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip6_mtu[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.ip6_mtu_used_count -= 1;
        }
        MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", \
            idx, _pL3Db[unit]->HW.ip6_mtu[idx].mtu_value, _pL3Db[unit]->HW.ip6_mtu[idx].ref_count);
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

errEntryNotFound:
errInput:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_mango_l3_mtuIp6Entry_realloc
 * Description:
 *      Re-allocate an IPv6 MTU entry index for reference.
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
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_mango_l3_mtuIp6Entry_realloc(uint32 unit, uint32 newMtu, uint32 *pIdx, uint32 flags)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIdx;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,newMtu=%d,pIdx=%p,flags=0x%08x", unit, newMtu, pIdx, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;
    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_MANGO_L3_IP6_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip6_mtu[idx].mtu_value == newMtu)
            {
                _pL3Db[unit]->HW.ip6_mtu[idx].ref_count += 1;
                *pIdx = idx;
                MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", idx, \
                    _pL3Db[unit]->HW.ip6_mtu[idx].mtu_value, \
                    _pL3Db[unit]->HW.ip6_mtu[idx].ref_count);

                /* release the old entry */
                goto freeOldIpMtuEntry;
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new MTU entry */
    if (_pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count == 1)
    {
        /* just change the MTU value with the original MTU entry */
        _pL3Db[unit]->HW.ip6_mtu[oldIdx].mtu_value = newMtu;

        RT_ERR_HDL(l3_util_intfIp6Mtu_set(unit, oldIdx, newMtu), errExit, ret);

        MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", oldIdx, \
            _pL3Db[unit]->HW.ip6_mtu[oldIdx].mtu_value, \
            _pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count);

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].mtu_value = newMtu;
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip6_mtu_used_count += 1;

        RT_ERR_HDL(l3_util_intfIp6Mtu_set(unit, emptyEntryIdx, newMtu), errExit, ret);

        MANGO_L3_DBG_PRINTF(3, "idx = %u, mtu_value = %u, ref_count = %u\n", \
            emptyEntryIdx, \
            _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].mtu_value, \
            _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].ref_count);

        *pIdx = emptyEntryIdx;
        goto freeOldIpMtuEntry;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
        goto errIntfMtuVarietyExceeds;
    }

freeOldIpMtuEntry:
    if (_pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count == 0)
            _pL3Db[unit]->HW.ip6_mtu_used_count -= 1;
        goto errOk;
    }

errIntfMtuVarietyExceeds:
errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_nhEntry_alloc(uint32 unit, uint32 *pIndex, dal_mango_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pIndex=%p,flags=0x%08x", unit, pIndex, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (flags & DAL_MANGO_L3_API_FLAG_WITH_ID)
    {
        index = *pIndex;

        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.nexthop_used, index))
        {
            BITMAP_SET(_pL3Db[unit]->HW.nexthop_used, index);

            _pL3Db[unit]->HW.nexthop_used_count += 1;
            _pL3Db[unit]->HW.nexthop[index].flags = flags;  /* logging */

            goto errOk;
        }

        ret = RT_ERR_ENTRY_EXIST;
        goto errEntryExist;
    }

    /* alloc a new interface for */
    for (index=0; index<DAL_MANGO_L3_NEXTHOP_MAX; index++)
    {
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.nexthop_used, index))
        {
            BITMAP_SET(_pL3Db[unit]->HW.nexthop_used, index);

            _pL3Db[unit]->HW.nexthop_used_count += 1;
            _pL3Db[unit]->HW.nexthop[index].flags = flags;  /* logging */

            /* update pIndex before returning */
            *pIndex = index;

            goto errOk;
        }
    }
    ret = RT_ERR_TBL_FULL;

errEntryExist:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_nhEntry_free(uint32 unit, uint32 index, dal_mango_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=0x%08x,flags=0x%08x", unit, index, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_MANGO_L3_NEXTHOP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_pL3Db[unit]->HW.nexthop_used, index))
    {
        /* check the caller */
        if ((flags & DAL_MANGO_L3_API_FLAG_MODS_MASK) != \
            (_pL3Db[unit]->HW.nexthop[index].flags & DAL_MANGO_L3_API_FLAG_MODS_MASK))
        {
            MANGO_L3_DBG_PRINTF(3, "caller is mismatched, flags = %08x, HW.nexthop[index].flags = %08x\n", \
                flags, _pL3Db[unit]->HW.nexthop[index].flags);
            ret = RT_ERR_INPUT;
            goto errInput;
        }

        _pL3Db[unit]->HW.nexthop[index].flags = flags;  /* logging */
        _pL3Db[unit]->HW.nexthop_used_count -= 1;
        BITMAP_CLEAR(_pL3Db[unit]->HW.nexthop_used, index);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

errInput:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_nhEntry_get(uint32 unit, uint32 index, dal_mango_l3_nhEntry_t *pEntry, dal_mango_l3_api_flag_t flags)
{
    int32 ret = RT_ERR_OK;
    l3_nexthop_entry_t nhEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pEntry=%p,flags=0x%08x", unit, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_MANGO_L3_NEXTHOP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* check the caller */
    if ((flags & DAL_MANGO_L3_API_FLAG_MODS_MASK) != \
        (_pL3Db[unit]->HW.nexthop[index].flags & DAL_MANGO_L3_API_FLAG_MODS_MASK))
    {
        ret = RT_ERR_INPUT;
        goto errHandle;
    }

    /* read from chip */
    L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_NEXTHOPt, index, nhEntry, "", errHandle, ret);

    /* get fields */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_NEXTHOPt, \
        MANGO_L3_NEXTHOP_DMAC_IDXtf, pEntry->dmac_idx, nhEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_NEXTHOPt, \
        MANGO_L3_NEXTHOP_L3_EGR_INTF_IDXtf, pEntry->l3_egr_intf_idx, nhEntry, "", errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u\n", index);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;

}

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
int32
_dal_mango_l3_nhEntry_set(uint32 unit, uint32 index, dal_mango_l3_nhEntry_t *pEntry, dal_mango_l3_api_flag_t flags)
{
    int32 ret = RT_ERR_OK;
    l3_nexthop_entry_t nhEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pEntry=%p,flags=0x%08x", unit, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_MANGO_L3_NEXTHOP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* check the caller */
    if ((flags & DAL_MANGO_L3_API_FLAG_MODS_MASK) != \
        (_pL3Db[unit]->HW.nexthop[index].flags & DAL_MANGO_L3_API_FLAG_MODS_MASK))
    {
        ret = RT_ERR_INPUT;
        goto errHandle;
    }

    /* set fields */
    osal_memset(&nhEntry, 0x00, sizeof(l3_nexthop_entry_t));

    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_NEXTHOPt, \
        MANGO_L3_NEXTHOP_DMAC_IDXtf, pEntry->dmac_idx, nhEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_NEXTHOPt, \
        MANGO_L3_NEXTHOP_L3_EGR_INTF_IDXtf, pEntry->l3_egr_intf_idx, nhEntry, "", errHandle, ret);

    /* write into chip */
    L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_NEXTHOPt, index, nhEntry, "", errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u, dmac_idx = %d, l3_egr_intf_idx = %d\n", \
        index, pEntry->dmac_idx, pEntry->l3_egr_intf_idx);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_nhEntryPathId_get(uint32 unit, uint32 index, rtk_l3_pathId_t *pPathId)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pPathId=%p", unit, index, pPathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index > HAL_MAX_NUM_OF_L3_NEXTHOP(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_pL3Db[unit]->HW.nexthop_used, index))
    {
        /* nhIdx (nexthop) to pathId */
        *pPathId = (_pL3Db[unit]->HW.nexthop[index].flags & DAL_MANGO_L3_API_FLAG_MOD_MPLS)? \
                    DAL_MANGO_L3_NH_IDX_TO_MPLS_PATH_ID(index) : \
                    DAL_MANGO_L3_NH_IDX_TO_PATH_ID(index);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

int32
_dal_mango_l3_vlanIntf_insert(uint32 unit, rtk_vlan_t vid, uint32 intfIdx)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_vlanIntfListNode_t *pIntfNode;
    dal_mango_l3_vlanIntfListNode_t *pIntfHead;

    if (RT_ERR_OK != _dal_mango_vlan_table_check(unit, vid))
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    if ((vid < DAL_MANGO_L3_VID_MAX) &&
        (intfIdx > 0) &&
        (intfIdx < DAL_MANGO_L3_INTF_MAX))
    {
        pIntfNode = &_pL3Db[unit]->vlanIntfNode[intfIdx];
        if (pIntfNode->intfIdx) /* normally, should NOT happen */
        {
            RTK_LIST_NODE_REMOVE(&_pL3Db[unit]->vid[vid].intf_list, pIntfNode, nodeRef);
            RTK_LIST_NODE_REF_INIT(pIntfNode, nodeRef);
        }
        pIntfNode->intfIdx = intfIdx;
        RTK_LIST_NODE_INSERT_TAIL(&_pL3Db[unit]->vid[vid].intf_list, pIntfNode, nodeRef);

        /* check the head */
        if (RTK_LIST_LENGTH(&_pL3Db[unit]->vid[vid].intf_list) >= 1)
        {
            pIntfHead = RTK_LIST_NODE_HEAD(&_pL3Db[unit]->vid[vid].intf_list);

            /* set VLAN's default interface ID */
            RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_set(unit, vid, pIntfHead->intfIdx, ENABLED), errHandle, ret);
            MANGO_L3_DBG_PRINTF(2, "Update VID=%d, default INTF_IDX=%d\n", vid, pIntfHead->intfIdx);
        }
    } else {
        ret = RT_ERR_INPUT;
    }

errHandle:

    return ret;
}

int32
_dal_mango_l3_vlanIntf_remove(uint32 unit, rtk_vlan_t vid, uint32 intfIdx)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_vlanIntfListNode_t *pIntfNode;
    dal_mango_l3_vlanIntfListNode_t *pIntfHead;

    if ((vid < DAL_MANGO_L3_VID_MAX) &&
        (intfIdx > 0) &&
        (intfIdx < DAL_MANGO_L3_INTF_MAX))
    {
        pIntfNode = &_pL3Db[unit]->vlanIntfNode[intfIdx];
        if (pIntfNode->intfIdx)
        {
            RTK_LIST_NODE_REMOVE(&_pL3Db[unit]->vid[vid].intf_list, pIntfNode, nodeRef);
            RTK_LIST_NODE_REF_INIT(pIntfNode, nodeRef);
            pIntfNode->intfIdx = 0; /* clear */

            if (RTK_LIST_LENGTH(&_pL3Db[unit]->vid[vid].intf_list) >= 1)
            {
                pIntfHead = RTK_LIST_NODE_HEAD(&_pL3Db[unit]->vid[vid].intf_list);

                /* set VLAN's default interface ID */
                RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_set(unit, vid, pIntfHead->intfIdx, DISABLED), errHandle, ret);
                MANGO_L3_DBG_PRINTF(2, "Update VID=%d, default INTF_IDX=%d\n", vid, pIntfHead->intfIdx);
            } else {
                /* empty */
                RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_set(unit, vid, DAL_MANGO_L3_RESERVED_INTF_IDX, DISABLED), errHandle, ret);
                MANGO_L3_DBG_PRINTF(2, "Update VID=%d, default INTF_IDX=%d (removed)\n", vid, DAL_MANGO_L3_RESERVED_INTF_IDX);
            }
        }
    } else {
        ret = RT_ERR_INPUT;
    }

errHandle:
    return ret;
}

int32
_dal_mango_l3_ecmpEntry_alloc(uint32 unit, uint32 *pIndex, uint32 flags)
{
    int32   ret = RT_ERR_OK;
    uint32  entryIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pIndex=%p,flags=0x%08x", unit, pIndex, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (flags & DAL_MANGO_L3_API_FLAG_WITH_ID)
    {
        entryIdx = *pIndex;

        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.ecmp_used, entryIdx))
        {
            BITMAP_SET(_pL3Db[unit]->HW.ecmp_used, entryIdx);
            _pL3Db[unit]->HW.ecmp_used_count += 1;

            goto errOk;
        }

        ret = RT_ERR_ENTRY_EXIST;
        goto errEntryExist;
    }

    /* alloc a new interface for */
    for (entryIdx=0; entryIdx<DAL_MANGO_L3_ECMP_MAX; entryIdx++)
    {
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.ecmp_used, entryIdx))
        {
            BITMAP_SET(_pL3Db[unit]->HW.ecmp_used, entryIdx);
            _pL3Db[unit]->HW.ecmp_used_count += 1;

            *pIndex = entryIdx; /* update pIndex beforce returning */

            goto errOk;
        }
    }
    ret = RT_ERR_TBL_FULL;

errEntryExist:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

int32
_dal_mango_l3_ecmpEntry_free(uint32 unit, uint32 index, uint32 flags)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=0x%08x,flags=0x%08x", unit, index, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_pL3Db[unit]->HW.ecmp_used, index))
    {
        BITMAP_CLEAR(_pL3Db[unit]->HW.ecmp_used, index);
        _pL3Db[unit]->HW.ecmp_used_count -= 1;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

int32
_dal_mango_l3_ecmpEntry_get(uint32 unit, uint32 index, dal_mango_l3_ecmpEntry_t *pEntry, uint32 flags)
{
    int32   ret = RT_ERR_OK;
    l3_ecmp_entry_t ecmpEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* read from chip */
    L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_ECMPt, index, ecmpEntry, "", errHandle, ret);

    /* get fields */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_0_NH_IDtf, pEntry->hash_to_nh_id[0], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_1_NH_IDtf, pEntry->hash_to_nh_id[1], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_2_NH_IDtf, pEntry->hash_to_nh_id[2], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_3_NH_IDtf, pEntry->hash_to_nh_id[3], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_4_NH_IDtf, pEntry->hash_to_nh_id[4], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_5_NH_IDtf, pEntry->hash_to_nh_id[5], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_6_NH_IDtf, pEntry->hash_to_nh_id[6], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_7_NH_IDtf, pEntry->hash_to_nh_id[7], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_8_NH_IDtf, pEntry->hash_to_nh_id[8], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_9_NH_IDtf, pEntry->hash_to_nh_id[9], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_10_NH_IDtf, pEntry->hash_to_nh_id[10], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_11_NH_IDtf, pEntry->hash_to_nh_id[11], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_12_NH_IDtf, pEntry->hash_to_nh_id[12], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_13_NH_IDtf, pEntry->hash_to_nh_id[13], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_14_NH_IDtf, pEntry->hash_to_nh_id[14], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_15_NH_IDtf, pEntry->hash_to_nh_id[15], ecmpEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_1tf, pEntry->nh_idx[0], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_2tf, pEntry->nh_idx[1], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_3tf, pEntry->nh_idx[2], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_4tf, pEntry->nh_idx[3], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_5tf, pEntry->nh_idx[4], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_6tf, pEntry->nh_idx[5], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_7tf, pEntry->nh_idx[6], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_8tf, pEntry->nh_idx[7], ecmpEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_METER_ENtf, pEntry->meter_en, ecmpEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_METER_IDXtf, pEntry->meter_idx, ecmpEntry, "", errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

int32
_dal_mango_l3_ecmpEntry_set(uint32 unit, uint32 index, dal_mango_l3_ecmpEntry_t *pEntry, uint32 flags)
{
    int32   ret = RT_ERR_OK;
    l3_ecmp_entry_t ecmpEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* set fields */
    osal_memset(&ecmpEntry, 0x00, sizeof(l3_ecmp_entry_t));
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_0_NH_IDtf, pEntry->hash_to_nh_id[0], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_1_NH_IDtf, pEntry->hash_to_nh_id[1], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_2_NH_IDtf, pEntry->hash_to_nh_id[2], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_3_NH_IDtf, pEntry->hash_to_nh_id[3], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_4_NH_IDtf, pEntry->hash_to_nh_id[4], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_5_NH_IDtf, pEntry->hash_to_nh_id[5], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_6_NH_IDtf, pEntry->hash_to_nh_id[6], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_7_NH_IDtf, pEntry->hash_to_nh_id[7], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_8_NH_IDtf, pEntry->hash_to_nh_id[8], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_9_NH_IDtf, pEntry->hash_to_nh_id[9], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_10_NH_IDtf, pEntry->hash_to_nh_id[10], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_11_NH_IDtf, pEntry->hash_to_nh_id[11], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_12_NH_IDtf, pEntry->hash_to_nh_id[12], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_13_NH_IDtf, pEntry->hash_to_nh_id[13], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_14_NH_IDtf, pEntry->hash_to_nh_id[14], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_HASH_15_NH_IDtf, pEntry->hash_to_nh_id[15], ecmpEntry, "", errHandle, ret);

    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_1tf, pEntry->nh_idx[0], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_2tf, pEntry->nh_idx[1], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_3tf, pEntry->nh_idx[2], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_4tf, pEntry->nh_idx[3], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_5tf, pEntry->nh_idx[4], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_6tf, pEntry->nh_idx[5], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_7tf, pEntry->nh_idx[6], ecmpEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_NH_IDX_8tf, pEntry->nh_idx[7], ecmpEntry, "", errHandle, ret);

    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_METER_ENtf, pEntry->meter_en, ecmpEntry, "", errHandle, ret);

    L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMPt, \
        MANGO_L3_ECMP_METER_IDXtf, pEntry->meter_idx, ecmpEntry, "", errHandle, ret);

    /* write into chip */
    L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_ECMPt, index, ecmpEntry, "", errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

static uint32
_dal_mango_l3_hostHash0_ret(dal_mango_l3_hostHashKey_t *pHashKey, dal_mango_l3_api_flag_t flags)
{
    uint32  ipv6 = (flags & DAL_MANGO_L3_API_FLAG_IPV6);
    uint32  hashRow[29];
    uint32  hashIdx;

    /* VRF ID */
    hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->vrf_id, 0, 5) << 5) | \
                 (HASH_BIT_EXTRACT(pHashKey->vrf_id, 5, 3) << 0);

    /* SIP */
    if (ipv6)
    {
        hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[0], 0, 7) << 3) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[1], 5, 3) << 0);
        hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[1], 0, 5) << 5) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[2], 3, 5) << 0);
        hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[2], 0, 3) << 7) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[3], 1, 7) << 0);
        hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[3], 0, 1) << 9) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[4], 0, 8) << 1) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[5], 7, 1) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[5], 0, 7) << 3) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[6], 5, 3) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[6], 0, 5) << 5) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[7], 3, 5) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[7], 0, 3) << 7) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[8], 1, 7) << 0);
        hashRow[8] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[8], 0, 1) << 9) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[9], 0, 8) << 1) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[10], 7, 1) << 0);
        hashRow[9] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[10], 0, 7) << 3) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[11], 5, 3) << 0);
        hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[11], 0, 5) << 5) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[12], 3, 5) << 0);
        hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[12], 0, 3) << 7) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[13], 1, 7) << 0);
        hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[13], 0, 1) << 9) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[14], 0, 8) << 1) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[15], 7, 1) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[15], 0, 7) << 3) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[0],  7, 1) << 0);
    } else {
        hashRow[1] = 0;
        hashRow[2] = 0;
        hashRow[3] = 0;
        hashRow[4] = 0;
        hashRow[5] = 0;
        hashRow[6] = 0;
        hashRow[7] = 0;
        hashRow[8] = 0;
        hashRow[9] = 0;
        hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4, 27,  5) << 0);
        hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4, 17, 10) << 0);
        hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4,  7, 10) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4 , 0,  7) << 3);
    }

    /* DIP */
    if (ipv6)
    {
        hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[0], 0, 8) << 0);
        hashRow[15] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[1], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 6, 2) << 0);
        hashRow[16] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 4, 4) << 0);
        hashRow[17] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[4], 2, 6) << 0);
        hashRow[18] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[4], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[5], 0, 8) << 0);
        hashRow[19] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[6], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[7], 6, 2) << 0);
        hashRow[20] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[7], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 4, 4) << 0);
        hashRow[21] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[9], 2, 6) << 0);
        hashRow[22] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[9], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[10], 0, 8) << 0);
        hashRow[23] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[11], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 6, 2) << 0);
        hashRow[24] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 4, 4) << 0);
        hashRow[25] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 2, 6) << 0);
        hashRow[26] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[15], 0, 8) << 0);
    } else {
        hashRow[14] = 0;
        hashRow[15] = 0;
        hashRow[16] = 0;
        hashRow[17] = 0;
        hashRow[18] = 0;
        hashRow[19] = 0;
        hashRow[20] = 0;
        hashRow[21] = 0;
        hashRow[22] = 0;
        hashRow[23] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 30,  2) << 0);
        hashRow[24] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 20, 10) << 0);
        hashRow[25] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 10, 10) << 0);
        hashRow[26] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4,  0, 10) << 0);
    }

    /* VID */
    hashRow[27] = (HASH_BIT_EXTRACT(pHashKey->vid, 3, 9) << 0);
    hashRow[28] = (HASH_BIT_EXTRACT(pHashKey->vid, 0, 3) << 7);

    /* result */
    hashIdx = hashRow[0]  ^ hashRow[1]  ^ hashRow[2]  ^ hashRow[3]  ^ hashRow[4]  ^ \
              hashRow[5]  ^ hashRow[6]  ^ hashRow[7]  ^ hashRow[8]  ^ hashRow[9]  ^ \
              hashRow[10] ^ hashRow[11] ^ hashRow[12] ^ hashRow[13] ^ hashRow[14] ^ \
              hashRow[15] ^ hashRow[16] ^ hashRow[17] ^ hashRow[18] ^ hashRow[19] ^ \
              hashRow[20] ^ hashRow[21] ^ hashRow[22] ^ hashRow[23] ^ hashRow[24] ^ \
              hashRow[25] ^ hashRow[26] ^ hashRow[27] ^ hashRow[28];

    return hashIdx;
}

static uint32
_dal_mango_l3_hostHash1_ret(dal_mango_l3_hostHashKey_t *pHashKey, dal_mango_l3_api_flag_t flags)
{
    uint32  ipv6 = (flags & DAL_MANGO_L3_API_FLAG_IPV6);
    uint32  sum;
    uint32  hashRow[30];
    uint32  hashIdx;

    if (ipv6)
    {
        hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[12], 6, 2) << 0);
        hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[12], 0, 6) << 4) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[13], 4, 4) << 0);
        hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[13], 0, 4) << 6) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[14], 2, 6) << 0);
        hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[14], 0, 2) << 8) | \
                     (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[15], 0, 8) << 0);

        hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 6, 2) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 0, 6) << 4) | \
                     (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 4, 4) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 0, 4) << 6) | \
                     (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 2, 6) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 0, 2) << 8) | \
                     (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[15], 0, 8) << 0);

        hashRow[8] = (HASH_BIT_EXTRACT(pHashKey->vrf_id, 0, 4) << 6) | \
                     (HASH_BIT_EXTRACT(pHashKey->vrf_id, 4, 4) << 0);
        hashRow[9] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[0], 2, 6) << 0);
        hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[0], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[1], 0, 8) << 0);
        hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[2], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[3], 6, 2) << 0);
        hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[3], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[4], 4, 4) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[4], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[5], 2, 6) << 0);
        hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[5], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[6], 0, 8) << 0);
        hashRow[15] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[7], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[8], 6, 2) << 0);
        hashRow[16] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[8], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[9], 4, 4) << 0);
        hashRow[17] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[9], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[10], 2, 6) << 0);
        hashRow[18] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[10], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->sip.ipv6.octet[11], 0, 8) << 0);

        hashRow[19] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[0], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[1], 6, 2) << 0);
        hashRow[20] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[1], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 4, 4) << 0);
        hashRow[21] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 2, 6) << 0);
        hashRow[22] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[4], 0, 8) << 0);
        hashRow[23] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[5], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[6], 6, 2) << 0);
        hashRow[24] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[6], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[7], 4, 4) << 0);
        hashRow[25] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[7], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 2, 6) << 0);
        hashRow[26] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[9], 0, 8) << 0);
        hashRow[27] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[10], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[11], 6, 2) << 0);
        hashRow[28] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[11], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->vid, 8, 4) << 0);
        hashRow[29] = (HASH_BIT_EXTRACT(pHashKey->vid, 0, 8) << 2);
    } else {
        hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4, 30,  2) << 0);
        hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4, 20, 10) << 0);
        hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4, 10, 10) << 0);
        hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->sip.ipv4,  0, 10) << 0);

        hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 30,  2) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 20, 10) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 10, 10) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4,  0, 10) << 0);

        hashRow[8] = (HASH_BIT_EXTRACT(pHashKey->vrf_id, 0, 4) << 6) | \
                     (HASH_BIT_EXTRACT(pHashKey->vrf_id, 4, 4) << 0);

        hashRow[9] = 0;
        hashRow[10] = 0;
        hashRow[11] = 0;
        hashRow[12] = 0;
        hashRow[13] = 0;
        hashRow[14] = 0;
        hashRow[15] = 0;
        hashRow[16] = 0;
        hashRow[17] = 0;
        hashRow[18] = 0;

        hashRow[19] = 0;
        hashRow[20] = 0;
        hashRow[21] = 0;
        hashRow[22] = 0;
        hashRow[23] = 0;
        hashRow[24] = 0;
        hashRow[25] = 0;
        hashRow[26] = 0;
        hashRow[27] = 0;
        hashRow[28] = (HASH_BIT_EXTRACT(pHashKey->vid, 8, 4) << 0);
        hashRow[29] = (HASH_BIT_EXTRACT(pHashKey->vid, 0, 8) << 2);

    }

    sum = hashRow[0] + hashRow[1];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[2];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[3];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[4];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[5];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[6];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);
    sum += hashRow[7];
    sum = (sum & 0x3FF) + ((sum & 0xFFFFFC00)? 1 : 0);

    /* result */
    hashIdx = sum ^ hashRow[8]  ^ hashRow[9]  ^ \
              hashRow[10] ^ hashRow[11] ^ hashRow[12] ^ hashRow[13] ^ hashRow[14] ^ \
              hashRow[15] ^ hashRow[16] ^ hashRow[17] ^ hashRow[18] ^ hashRow[19] ^ \
              hashRow[20] ^ hashRow[21] ^ hashRow[22] ^ hashRow[23] ^ hashRow[24] ^ \
              hashRow[25] ^ hashRow[26] ^ hashRow[27] ^ hashRow[28] ^ hashRow[29];

    return hashIdx;
}


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
int32
_dal_mango_l3_hostHashIdx_get(uint32 unit, dal_mango_l3_hostHashKey_t *pHashKey, dal_mango_l3_hostHashIdx_t *pHashIdx, dal_mango_l3_api_flag_t flags)
{
    uint32  tbl;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHashKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHashIdx), RT_ERR_NULL_POINTER);

    /* calculate hash index for each table */
    for (tbl=0; tbl<DAL_MANGO_L3_HOST_TBL_NUM; tbl++)
    {
        if (0 == pHashKey->alg_of_tbl[tbl])
        {
            /* Hash Algorithm 0 */
            pHashIdx->idx_of_tbl[tbl] = _dal_mango_l3_hostHash0_ret(pHashKey, flags);
        }
        else if (1 == pHashKey->alg_of_tbl[tbl])
        {
            /* Hash Algorithm 1 */
            pHashIdx->idx_of_tbl[tbl] = _dal_mango_l3_hostHash1_ret(pHashKey, flags);
        }
        else
        {
            return RT_ERR_INPUT;
        }
    }

    return RT_ERR_OK;
}

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
int32
_dal_mango_l3_hostEntry_alloc(uint32 unit, dal_mango_l3_hostAlloc_t *pAlloc, uint32 *pIndex, dal_mango_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  width, wIdx, entryMask, currentMask;
    uint32  addr, tbl, idx, cost;
    int32   bestCost = 0, bestTbl = 0, bestIdx = 0, bestSlot = 0;
    static int8  widByWidth[] = { -1, 0, 1, 2, -1, -1, 3, -1 };
    static uint8 costByMaskWidx[][4] = {
        { 12, 13, 16, 24 }, {  1,  7, 10,  0 }, {  1,  7, 10,  0 }, {  3,  7, 10,  0 },
        {  3,  4,  8,  0 }, {  1,  7,  8,  0 }, {  1,  7,  8,  0 }, {  4,  7,  8,  0 },
        {  3,  4,  8,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 },
        {  3,  4,  0,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 }, {  3,  4,  0,  0 },
        {  1,  7, 10,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 },
        {  1,  4,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  1,  7,  8,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  1,  4,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  1,  7, 10,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 },
        {  1,  4,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  1,  7,  8,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  1,  4,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  3,  7, 10,  0 }, {  1,  4,  0,  0 }, {  1,  4,  0,  0 }, {  3,  4,  0,  0 },
        {  1,  4,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  4,  7,  8,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 },
        {  3,  4,  0,  0 }, {  1,  0,  0,  0 }, {  1,  0,  0,  0 }, {  0,  0,  0,  0 }
        };
    static uint8 slotByMaskWidx[][4] = {
        { 0, 0, 0, 0 }, { 1, 2, 3, 0 }, { 0, 2, 3, 0 }, { 2, 2, 3, 0 },
        { 0, 0, 3, 0 }, { 1, 4, 3, 0 }, { 0, 4, 3, 0 }, { 3, 4, 3, 0 },
        { 4, 4, 0, 0 }, { 1, 4, 0, 0 }, { 0, 4, 0, 0 }, { 2, 4, 0, 0 },
        { 0, 0, 0, 0 }, { 1, 4, 0, 0 }, { 0, 4, 0, 0 }, { 4, 4, 0, 0 },
        { 5, 0, 0, 0 }, { 1, 2, 0, 0 }, { 0, 2, 0, 0 }, { 5, 2, 0, 0 },
        { 3, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 3, 0, 0, 0 },
        { 5, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 0, 0, 0 },
        { 5, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 5, 0, 0, 0 },
        { 4, 0, 0, 0 }, { 1, 2, 0, 0 }, { 0, 2, 0, 0 }, { 4, 2, 0, 0 },
        { 3, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 3, 0, 0, 0 },
        { 4, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 0, 0, 0 },
        { 4, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 4, 0, 0, 0 },
        { 3, 0, 0, 0 }, { 1, 2, 0, 0 }, { 0, 2, 0, 0 }, { 2, 2, 0, 0 },
        { 3, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 3, 0, 0, 0 },
        { 2, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 0, 0, 0 },
        { 0, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
        };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pAlloc=%p,pIndex=%p,flags=0x%08x", unit, pAlloc, pIndex, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAlloc), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* find a space with the specified width */
    width = pAlloc->width;
    wIdx = widByWidth[(width & 0x7)];
    entryMask = (0x1 << width) - 1;
    bestCost = 0;
    for (tbl=0; tbl<DAL_MANGO_L3_HOST_TBL_NUM; tbl++)
    {
        idx = (pAlloc->hashIdx.idx_of_tbl[tbl] % DAL_MANGO_L3_HOST_TBL_HEIGHT);
        currentMask = _pL3Db[unit]->HW.hostTable[tbl].row[idx].valid_mask;

        /* cost evaluation */
        cost = costByMaskWidx[(currentMask & 0x3F)][wIdx];

        if ((cost > 0) && ((0 == bestCost) || (cost < bestCost)))
        {
            bestCost = cost;
            bestTbl = tbl;
            bestIdx = idx;
            bestSlot = slotByMaskWidx[(currentMask & 0x3F)][wIdx];
            MANGO_L3_DBG_PRINTF(3, "best: cost = %d, tbl = %d, idx = %d, slot = %d\n",
                bestCost, bestTbl, bestIdx, bestSlot);
        }
    }

    if (bestCost)
    {
        /* take the best position for the entry */
        _pL3Db[unit]->HW.hostTable[bestTbl].row[bestIdx].slot[bestSlot].width = width;
        _pL3Db[unit]->HW.hostTable[bestTbl].row[bestIdx].valid_mask |= (entryMask << bestSlot);
        _pL3Db[unit]->HW.host_used_count += width;

        addr = (bestTbl << 13) | (bestIdx << 3) | (bestSlot << 0);
        *pIndex = DAL_MANGO_L3_ENTRY_ADDR_TO_IDX(addr);

        MANGO_L3_DBG_PRINTF(3, "tbl = %d, idx = %d, slot = %d, index = %u (0x%08x), addr = %u (0x%08x)\n",
            bestTbl, bestIdx, bestSlot, *pIndex, *pIndex, addr, addr);
    }
    else
    {
        /* Table full */
        ret = RT_ERR_TBL_FULL;
    }

    /* unlock internal semaphore */
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_hostEntry_free(uint32 unit, uint32 index, dal_mango_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  addr, tbl, idx, slot, width, entryMask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,flags=0x%08x", unit, index, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    addr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(index);
    tbl = (addr & (0x1 << 13)) >> 13;
    idx = (addr & (0x3FF << 3)) >> 3;
    slot = (addr & 0x7);
    RT_PARAM_CHK((tbl > DAL_MANGO_L3_HOST_TBL_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((idx > DAL_MANGO_L3_HOST_TBL_HEIGHT), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((slot > DAL_MANGO_L3_HOST_TBL_WIDTH), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* release the entry */
    width = _pL3Db[unit]->HW.hostTable[tbl].row[idx].slot[slot].width;
    if (0 == width)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }
    else
    {
        entryMask = (0x1 << width) - 1;

        _pL3Db[unit]->HW.hostTable[tbl].row[idx].slot[slot].width = 0;
        _pL3Db[unit]->HW.hostTable[tbl].row[idx].valid_mask &= ~(entryMask << slot);
        _pL3Db[unit]->HW.host_used_count -= width;
    }

    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_hostEntry_get(uint32 unit, uint32 index, dal_mango_l3_hostEntry_t *pEntry, dal_mango_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  addr, tbl, idx, slot, width;
    uint32  value[2];
    l3_host_route_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    addr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(index);
    tbl = (addr & (0x1 << 13)) >> 13;
    idx = (addr & (0x3FF << 3)) >> 3;
    slot = (addr & 0x7);
    RT_PARAM_CHK((tbl > DAL_MANGO_L3_HOST_TBL_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((idx > DAL_MANGO_L3_HOST_TBL_HEIGHT), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((slot > DAL_MANGO_L3_HOST_TBL_WIDTH), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    width = _pL3Db[unit]->HW.hostTable[tbl].row[idx].slot[slot].width;
    if (1 == width)
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
            MANGO_L3_HOST_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
            MANGO_L3_HOST_ROUTE_IPUC_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
    else if (2 == width)
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
            MANGO_L3_HOST_ROUTE_IPMC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
            MANGO_L3_HOST_ROUTE_IPMC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
    else if (3 == width)
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
            MANGO_L3_HOST_ROUTE_IP6UC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
            MANGO_L3_HOST_ROUTE_IP6UC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
    else if (6 == width)
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
            MANGO_L3_HOST_ROUTE_IP6MC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
            MANGO_L3_HOST_ROUTE_IP6MC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    if (0 == pEntry->fmt)
    {
        /* L3 entry */

        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
            MANGO_L3_HOST_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST == pEntry->entry_type)
        {
            /* IPv4 unicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_IPtf, pEntry->ip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_MULTICAST == pEntry->entry_type)
        {
            /* IPv4 multicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_SIPtf, pEntry->sip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);

            /* IPMC (N+1) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, (index + 1), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_GIPtf, pEntry->gip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_VIDtf, pEntry->vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST == pEntry->entry_type)
        {
            /* IPv6 unicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_IP6tf, value, entry, "", errHandle, ret);
            pEntry->ip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->ip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->ip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);

            /* IP6UC (N+1) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_1t, (index + 1), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_1t, \
                MANGO_L3_HOST_ROUTE_IP6UC_1_IP6tf, value, entry, "", errHandle, ret);
            pEntry->ip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->ip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->ip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->ip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[9] = (value[0] & 0x000000FF) >> 0;

            /* IP6UC (N+2) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_2t, (index + 2), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_2t, \
                MANGO_L3_HOST_ROUTE_IP6UC_2_IP6tf, value, entry, "", errHandle, ret);
            pEntry->ip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->ip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->ip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->ip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[15] = (value[0] & 0x000000FF) >> 0;
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_MULTICAST == pEntry->entry_type)
        {
            /* IPv6 multicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_SIP6tf, value, entry, "", errHandle, ret);
            pEntry->sip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->sip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->sip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);

            /* IP6MC (N+1) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, (index + 1), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_SIP6tf, value, entry, "", errHandle, ret);
            pEntry->sip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->sip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->sip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->sip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[9] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);

            /* IP6MC (N+2) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, (index + 2), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, \
                MANGO_L3_HOST_ROUTE_IP6MC_2_SIP6tf, value, entry, "", errHandle, ret);
            pEntry->sip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->sip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->sip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->sip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[15] = (value[0] & 0x000000FF) >> 0;

            /* IP6MC (N+3) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, (index + 3), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_GIP6tf, value, entry, "", errHandle, ret);
            pEntry->gip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->gip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->gip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_VIDtf, pEntry->vid, entry, "", errHandle, ret);

            /* IP6MC (N+4) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, (index + 4), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, \
                MANGO_L3_HOST_ROUTE_IP6MC_4_GIP6tf, value, entry, "", errHandle, ret);
            pEntry->gip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->gip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->gip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->gip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[9] = (value[0] & 0x000000FF) >> 0;

            /* IP6MC (N+5) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, (index + 5), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, \
                MANGO_L3_HOST_ROUTE_IP6MC_5_GIP6tf, value, entry, "", errHandle, ret);
            pEntry->gip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->gip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->gip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->gip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[15] = (value[0] & 0x000000FF) >> 0;
        }
        else
        {
            /* Unknown type */
            ret = RT_ERR_FAILED;
            goto errHandle;
        }
    }
    else
    {
        /* Unknown format */
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_mango_l3_hostEntry_set(uint32 unit, uint32 index, dal_mango_l3_hostEntry_t *pEntry, dal_mango_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  addr, tbl, idx, slot, width;
    uint32  value[2];
    l3_host_route_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    addr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(index);
    tbl = (addr & (0x1 << 13)) >> 13;
    idx = (addr & (0x3FF << 3)) >> 3;
    slot = (addr & 0x7);
    MANGO_L3_DBG_PRINTF(3, "index = %u, addr = %u, tbl = %u, idx = %u, slot = %u\n", index, addr, tbl, idx, slot);
    RT_PARAM_CHK((tbl > DAL_MANGO_L3_HOST_TBL_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((idx > DAL_MANGO_L3_HOST_TBL_HEIGHT), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((slot > DAL_MANGO_L3_HOST_TBL_WIDTH), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    width = _pL3Db[unit]->HW.hostTable[tbl].row[idx].slot[slot].width;
    if (0 == width)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* prepare data */
    if (0 == pEntry->fmt)
    {
        /* L3 entry */
        if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST == pEntry->entry_type)
        {
            /* IPv4 unicast */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_IPtf, pEntry->ip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, \
                MANGO_L3_HOST_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPUCt, index, entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_MULTICAST == pEntry->entry_type)
        {
            /* IPv4 multicast */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_SIPtf, pEntry->sip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, \
                MANGO_L3_HOST_ROUTE_IPMC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_0t, index, entry, "", errHandle, ret);

            /* IPMC (N+1) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_GIPtf, pEntry->gip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_VIDtf, pEntry->vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, \
                MANGO_L3_HOST_ROUTE_IPMC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IPMC_1t, (index + 1), entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST == pEntry->entry_type)
        {
            /* IPv6 unicast */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            value[0] = (pEntry->ip6.octet[0] << 24) | \
                       (pEntry->ip6.octet[1] << 16) | \
                       (pEntry->ip6.octet[2] << 8)  | \
                       (pEntry->ip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, \
                MANGO_L3_HOST_ROUTE_IP6UC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_0t, index, entry, "", errHandle, ret);

            /* IP6UC (N+1) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_1t, \
                MANGO_L3_HOST_ROUTE_IP6UC_1_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_1t, \
                MANGO_L3_HOST_ROUTE_IP6UC_1_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_1t, \
                MANGO_L3_HOST_ROUTE_IP6UC_1_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            value[1] = (pEntry->ip6.octet[4] << 8) | \
                       (pEntry->ip6.octet[5] << 0);
            value[0] = (pEntry->ip6.octet[6] << 24) | \
                       (pEntry->ip6.octet[7] << 16) | \
                       (pEntry->ip6.octet[8] << 8)  | \
                       (pEntry->ip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_1t, \
                MANGO_L3_HOST_ROUTE_IP6UC_1_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_1t, (index + 1), entry, "", errHandle, ret);

            /* IP6UC (N+2) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_2t, \
                MANGO_L3_HOST_ROUTE_IP6UC_2_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_2t, \
                MANGO_L3_HOST_ROUTE_IP6UC_2_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_2t, \
                MANGO_L3_HOST_ROUTE_IP6UC_2_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            value[1] = (pEntry->ip6.octet[10] << 8) | \
                       (pEntry->ip6.octet[11] << 0);
            value[0] = (pEntry->ip6.octet[12] << 24) | \
                       (pEntry->ip6.octet[13] << 16) | \
                       (pEntry->ip6.octet[14] << 8)  | \
                       (pEntry->ip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_2t, \
                MANGO_L3_HOST_ROUTE_IP6UC_2_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6UC_2t, (index + 2), entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_MULTICAST == pEntry->entry_type)
        {
            /* IPv6 multicast */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            value[0] = (pEntry->sip6.octet[0] << 24) | \
                       (pEntry->sip6.octet[1] << 16) | \
                       (pEntry->sip6.octet[2] << 8)  | \
                       (pEntry->sip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_SIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, \
                MANGO_L3_HOST_ROUTE_IP6MC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_0t, index, entry, "", errHandle, ret);

            /* IP6MC (N+1) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->sip6.octet[4] << 8) | \
                       (pEntry->sip6.octet[5] << 0);
            value[0] = (pEntry->sip6.octet[6] << 24) | \
                       (pEntry->sip6.octet[7] << 16) | \
                       (pEntry->sip6.octet[8] << 8)  | \
                       (pEntry->sip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_SIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, \
                MANGO_L3_HOST_ROUTE_IP6MC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_1t, (index + 1), entry, "", errHandle, ret);

            /* IP6MC (N+2) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, \
                MANGO_L3_HOST_ROUTE_IP6MC_2_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, \
                MANGO_L3_HOST_ROUTE_IP6MC_2_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, \
                MANGO_L3_HOST_ROUTE_IP6MC_2_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, \
                MANGO_L3_HOST_ROUTE_IP6MC_2_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->sip6.octet[10] << 8) | \
                       (pEntry->sip6.octet[11] << 0);
            value[0] = (pEntry->sip6.octet[12] << 24) | \
                       (pEntry->sip6.octet[13] << 16) | \
                       (pEntry->sip6.octet[14] << 8)  | \
                       (pEntry->sip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, \
                MANGO_L3_HOST_ROUTE_IP6MC_2_SIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_2t, (index + 2), entry, "", errHandle, ret);

            /* IP6MC (N+3) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[0] = (pEntry->gip6.octet[0] << 24) | \
                       (pEntry->gip6.octet[1] << 16) | \
                       (pEntry->gip6.octet[2] << 8)  | \
                       (pEntry->gip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_GIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, \
                MANGO_L3_HOST_ROUTE_IP6MC_3_VIDtf, pEntry->vid, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_3t, (index + 3), entry, "", errHandle, ret);

            /* IP6MC (N+4) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, \
                MANGO_L3_HOST_ROUTE_IP6MC_4_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, \
                MANGO_L3_HOST_ROUTE_IP6MC_4_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, \
                MANGO_L3_HOST_ROUTE_IP6MC_4_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, \
                MANGO_L3_HOST_ROUTE_IP6MC_4_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->gip6.octet[4] << 8) | \
                       (pEntry->gip6.octet[5] << 0);
            value[0] = (pEntry->gip6.octet[6] << 24) | \
                       (pEntry->gip6.octet[7] << 16) | \
                       (pEntry->gip6.octet[8] << 8)  | \
                       (pEntry->gip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, \
                MANGO_L3_HOST_ROUTE_IP6MC_4_GIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_4t, (index + 4), entry, "", errHandle, ret);

            /* IP6MC (N+5) */
            osal_memset(&entry, 0x00, sizeof(l3_host_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, \
                MANGO_L3_HOST_ROUTE_IP6MC_5_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, \
                MANGO_L3_HOST_ROUTE_IP6MC_5_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, \
                MANGO_L3_HOST_ROUTE_IP6MC_5_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, \
                MANGO_L3_HOST_ROUTE_IP6MC_5_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->gip6.octet[10] << 8) | \
                       (pEntry->gip6.octet[11] << 0);
            value[0] = (pEntry->gip6.octet[12] << 24) | \
                       (pEntry->gip6.octet[13] << 16) | \
                       (pEntry->gip6.octet[14] << 8)  | \
                       (pEntry->gip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, \
                MANGO_L3_HOST_ROUTE_IP6MC_5_GIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_HOST_ROUTE_IP6MC_5t, (index + 5), entry, "", errHandle, ret);
        }
        else
        {
            /* Unknown type */
            MANGO_L3_DBG_PRINTF(0, "Unknown type (%d), index = %d!\n", pEntry->entry_type, index);
            ret = RT_ERR_FAILED;
            goto errHandle;
        }
    }
    else
    {
        /* Unknown format */
        MANGO_L3_DBG_PRINTF(0, "Unknown format (%d), index = %d!\n", pEntry->fmt, index);
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}


static int32
__dal_mango_l3_routeEntry_hwLookup(uint32 unit, rtk_l3_route_t *pRoute, int32 *pEntryIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  value = 0, val = 0;
    uint32  ip6[4];
    uint32  chkTimes = DAL_MANGO_L3_CHK_IDLE_TIMES;

    L3_HWLU_SEM_LOCK(unit);

    /* L3_HW_LU_KEY_CTRL */
    L3_REG_READ_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, value, "", errHandle, ret);
    val = 0;
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, MANGO_VID_INTF_IDf, val, value, "", errHandle, ret);
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, MANGO_MC_KEY_SELf, val, value, "", errHandle, ret);
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, MANGO_VRFf, pRoute->vrf_id, value, "", errHandle, ret);
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, MANGO_IPMC_TYPEf, val, value, "", errHandle, ret);
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, MANGO_ROUNDf, val, value, "", errHandle, ret);
    L3_REG_WRITE_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, value, "", errHandle, ret);

    if (pRoute->flags & RTK_L3_FLAG_IPV6)
    {
        /* L3_HW_LU_KEY_SIP_CTRL */

        /* L3_HW_LU_KEY_DIP_CTRL */
        ip6[0] = (pRoute->ip_addr.ipv6.octet[0] << 24) |
                 (pRoute->ip_addr.ipv6.octet[1] << 16) |
                 (pRoute->ip_addr.ipv6.octet[2] << 8) |
                 (pRoute->ip_addr.ipv6.octet[3] << 0);
        ip6[1] = (pRoute->ip_addr.ipv6.octet[4] << 24) |
                 (pRoute->ip_addr.ipv6.octet[5] << 16) |
                 (pRoute->ip_addr.ipv6.octet[6] << 8) |
                 (pRoute->ip_addr.ipv6.octet[7] << 0);
        ip6[2] = (pRoute->ip_addr.ipv6.octet[8] << 24) |
                 (pRoute->ip_addr.ipv6.octet[9] << 16) |
                 (pRoute->ip_addr.ipv6.octet[10] << 8) |
                 (pRoute->ip_addr.ipv6.octet[11] << 0);
        ip6[3] = (pRoute->ip_addr.ipv6.octet[12] << 24) |
                 (pRoute->ip_addr.ipv6.octet[13] << 16) |
                 (pRoute->ip_addr.ipv6.octet[14] << 8) |
                 (pRoute->ip_addr.ipv6.octet[15] << 0);

        L3_REG_WRITE_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_DIP_CTRLr, ip6[0], "", errHandle, ret);

        value = DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST;
    }
    else
    {
        /* L3_HW_LU_KEY_SIP_CTRL */

        /* L3_HW_LU_KEY_DIP_CTRL */
        L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_DIP_CTRLr, MANGO_IPf, pRoute->ip_addr.ipv4, "", errHandle, ret);

        value = DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST;
    }
    L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HW_LU_KEY_CTRLr, MANGO_ENTRY_TYPEf, value, "", errHandle, ret);

    /* L3_HW_LU_CTRL */
    value = 1;
    L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HW_LU_CTRLr, MANGO_EXEC_TCAMf, value, "", errHandle, ret);

    do {
        osal_time_udelay(1);
        L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HW_LU_CTRLr, MANGO_EXEC_TCAMf, value, "", errHandle, ret);

        chkTimes--;
        if (chkTimes <= 0)
        {
            *pEntryIdx = -1;    /* RT_ERR_FAILED */
            goto errHandle;
        }
    } while (value != 0);

    /* check the result */
    L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HW_LU_CTRLr, MANGO_RESULT_TCAMf, value, "", errHandle, ret);
    if (value)
    {
        L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HW_LU_CTRLr, MANGO_ENTRY_IDX_TCAMf, value, "", errHandle, ret);
        *pEntryIdx = DAL_MANGO_L3_ENTRY_ADDR_TO_IDX(value);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

errHandle:
    L3_HWLU_SEM_UNLOCK(unit);

    return ret;
}

int32
__dal_mango_l3_routeEntry_get(uint32 unit, uint32 index, dal_mango_l3_routeEntry_t *pEntry, rtk_enable_t idxChk)
{
    int32   ret = RT_ERR_OK;
    uint32  value[2];
    l3_prefix_route_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    if (idxChk) {
        RT_PARAM_CHK((index > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    }
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
#ifndef MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_INT_SEM_LOCK(unit);
#endif

    /* pre-check */
    if (index < L3_ROUTE_TBL_IP_CNT(unit))
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
            MANGO_L3_PREFIX_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
            MANGO_L3_PREFIX_ROUTE_IPUC_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    else if ( (index >= (L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit)))) &&
                index < L3_ROUTE_TBL_IP6UC_MAX(unit))
#else
    else if ( (index >= (L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit)))) &&
                index < L3_ROUTE_TBL_SIZE(unit))
#endif
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
            MANGO_L3_PREFIX_ROUTE_IP6UC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
            MANGO_L3_PREFIX_ROUTE_IP6UC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
#if MANGO_L3_ROUTE_IPMC_SIZE
    else if ( (index >= _pL3Db[unit]->l3_route_ipmc_idx_base) &&
              (index < (_pL3Db[unit]->l3_route_ipmc_idx_base + _pL3Db[unit]->l3_route_ipmc_size)))
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
            MANGO_L3_PREFIX_ROUTE_IPMC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
            MANGO_L3_PREFIX_ROUTE_IPMC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
#endif
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    else if ((index >= L3_ROUTE_TBL_IP6UC_MAX(unit)) && (index < L3_ROUTE_TBL_IPMC_MAX(unit)))
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
            MANGO_L3_PREFIX_ROUTE_IPMC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
            MANGO_L3_PREFIX_ROUTE_IPMC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
    else if ((index >= L3_ROUTE_TBL_IPMC_MAX(unit)) && (index < L3_ROUTE_TBL_IP6MC_MAX(unit)))
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
            MANGO_L3_PREFIX_ROUTE_IP6MC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
            MANGO_L3_PREFIX_ROUTE_IP6MC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
#endif
#if MANGO_L3_ROUTE_RSVMC_ENTRY
    else if ( (index >= DAL_MANGO_L3_ROUTE_TBL_RSV_V4_MIN_IDX) &&
                (index <= DAL_MANGO_L3_ROUTE_TBL_RSV_V4_MAX_IDX) )
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
            MANGO_L3_PREFIX_ROUTE_IPMC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
            MANGO_L3_PREFIX_ROUTE_IPMC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
    else if ( (index >= DAL_MANGO_L3_ROUTE_TBL_RSV_V6_MIN_IDX) &&
                (index <= DAL_MANGO_L3_ROUTE_TBL_RSV_V6_MAX_IDX) )
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
            MANGO_L3_PREFIX_ROUTE_IP6MC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
            MANGO_L3_PREFIX_ROUTE_IP6MC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
    }
#endif
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    if (0 == pEntry->fmt)
    {
        /* L3 entry */

        L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
            MANGO_L3_PREFIX_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);

        if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST == pEntry->entry_type)
        {
            /* IPv4 unicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_IPtf, pEntry->ip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_IPtf, pEntry->bmsk_ip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_MULTICAST == pEntry->entry_type)
        {
            /* IPv4 multicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_SIPtf, pEntry->sip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_SIPtf, pEntry->bmsk_sip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_MC_KEY_SELtf, pEntry->bmsk_mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);

            /* IPMC (N+1) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, (index + 1), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_GIPtf, pEntry->gip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_VIDtf, pEntry->vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_ROUNDtf, pEntry->round, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_GIPtf, pEntry->bmsk_gip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST == pEntry->entry_type)
        {
            /* IPv6 unicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_IP6tf, value, entry, "", errHandle, ret);
            pEntry->ip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->ip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->ip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_IP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_ip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_ip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_ip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_ip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);

            /* IP6UC (N+1) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, (index + 1), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_IP6tf, value, entry, "", errHandle, ret);
            pEntry->ip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->ip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->ip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->ip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[9] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_BMSK_IP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_ip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->bmsk_ip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->bmsk_ip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_ip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_ip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_ip6.octet[9] = (value[0] & 0x000000FF) >> 0;

            /* IP6UC (N+2) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, (index + 2), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_IP6tf, value, entry, "", errHandle, ret);
            pEntry->ip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->ip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->ip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->ip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->ip6.octet[15] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_BMSK_IP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_ip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->bmsk_ip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->bmsk_ip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_ip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_ip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_ip6.octet[15] = (value[0] & 0x000000FF) >> 0;
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_MULTICAST == pEntry->entry_type)
        {
            /* IPv6 multicast */
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_SIP6tf, value, entry, "", errHandle, ret);
            pEntry->sip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->sip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->sip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_SIP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_sip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_sip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_sip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_sip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_MC_KEY_SELtf, pEntry->bmsk_mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);

            /* IP6MC (N+1) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, (index + 1), entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_SIP6tf, value, entry, "", errHandle, ret);
            pEntry->sip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->sip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->sip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->sip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[9] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_BMSK_IP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_sip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->bmsk_sip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->bmsk_sip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_sip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_sip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_sip6.octet[9] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);

            /* IP6MC (N+2) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, (index + 2), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_SIP6tf, value, entry, "", errHandle, ret);
            pEntry->sip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->sip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->sip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->sip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->sip6.octet[15] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_BMSK_IP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_sip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->bmsk_sip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->bmsk_sip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_sip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_sip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_sip6.octet[15] = (value[0] & 0x000000FF) >> 0;

            /* IP6MC (N+3) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, (index + 3), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_GIP6tf, value, entry, "", errHandle, ret);
            pEntry->gip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->gip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->gip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_VIDtf, pEntry->vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_ROUNDtf, pEntry->round, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_GIP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_gip6.octet[0] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_gip6.octet[1] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_gip6.octet[2] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_gip6.octet[3] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);

            /* IP6MC (N+4) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, (index + 4), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_GIP6tf, value, entry, "", errHandle, ret);
            pEntry->gip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->gip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->gip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->gip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[9] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_BMSK_IP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_gip6.octet[4] = (value[1] & 0x0000FF00) >> 8;
            pEntry->bmsk_gip6.octet[5] = (value[1] & 0x000000FF) >> 0;
            pEntry->bmsk_gip6.octet[6] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_gip6.octet[7] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_gip6.octet[8] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_gip6.octet[9] = (value[0] & 0x000000FF) >> 0;

            /* IP6MC (N+5) */
            L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, (index + 5), entry, "", errHandle, ret);

            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_GIP6tf, value, entry, "", errHandle, ret);
            pEntry->gip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->gip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->gip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->gip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->gip6.octet[15] = (value[0] & 0x000000FF) >> 0;
            L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_BMSK_IP6tf, value, entry, "", errHandle, ret);
            pEntry->bmsk_gip6.octet[10] = (value[1] & 0x0000FF00) >> 8;
            pEntry->bmsk_gip6.octet[11] = (value[1] & 0x000000FF) >> 0;
            pEntry->bmsk_gip6.octet[12] = (value[0] & 0xFF000000) >> 24;
            pEntry->bmsk_gip6.octet[13] = (value[0] & 0x00FF0000) >> 16;
            pEntry->bmsk_gip6.octet[14] = (value[0] & 0x0000FF00) >> 8;
            pEntry->bmsk_gip6.octet[15] = (value[0] & 0x000000FF) >> 0;
        }
        else
        {
            /* Unknown type */
            ret = RT_ERR_FAILED;
            goto errHandle;
        }
    }
    else
    {
        /* Unknown format */
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
#ifndef MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_INT_SEM_UNLOCK(unit);
#endif

    return ret;
}

int32
__dal_mango_l3_routeEntry_set(uint32 unit, uint32 index, dal_mango_l3_routeEntry_t *pEntry, rtk_enable_t idxChk)
{
    int32   ret = RT_ERR_OK;
    uint32  value[2];
    l3_prefix_route_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    if (idxChk) {
        RT_PARAM_CHK((index > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    }
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_SEM_LOCK(unit);
#else
    L3_INT_SEM_LOCK(unit);
#endif

    /* pre-check */
#if MANGO_L3_ROUTE_IPMC_SIZE
    if ( ((index >= L3_ROUTE_TBL_IP_CNT(unit) && \
         (index < (L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit)))))) &&
         ( (index < _pL3Db[unit]->l3_route_ipmc_idx_base) &&
           (index >= (_pL3Db[unit]->l3_route_ipmc_idx_base + _pL3Db[unit]->l3_route_ipmc_size)))
       )
#else
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    if ((index >= L3_ROUTE_TBL_IPUC_MAX(unit)) && (index < L3_ROUTE_TBL_BLANK_MAX(unit)))
#else
    if ((index >= L3_ROUTE_TBL_IP_CNT(unit) && \
        (index < (L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit))))))
#endif
#endif
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* prepare data */
    if (0 == pEntry->fmt)
    {
        /* L3 entry */
        if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_UNICAST == pEntry->entry_type)
        {
            /* IPv4 unicast */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_IPtf, pEntry->ip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_BMSK_IPtf, pEntry->bmsk_ip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, \
                MANGO_L3_PREFIX_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPUCt, index, entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV4_MULTICAST == pEntry->entry_type)
        {
            /* IPv4 multicast */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_SIPtf, pEntry->sip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_SIPtf, pEntry->bmsk_sip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_BMSK_MC_KEY_SELtf, pEntry->bmsk_mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_0t, index, entry, "", errHandle, ret);

            /* IPMC (N+1) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_GIPtf, pEntry->gip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_VIDtf, pEntry->vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_ROUNDtf, pEntry->round, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_GIPtf, pEntry->bmsk_gip, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, \
                MANGO_L3_PREFIX_ROUTE_IPMC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IPMC_1t, (index + 1), entry, "", errHandle, ret);

        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_UNICAST == pEntry->entry_type)
        {
            /* IPv6 unicast */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            value[0] = (pEntry->ip6.octet[0] << 24) | \
                       (pEntry->ip6.octet[1] << 16) | \
                       (pEntry->ip6.octet[2] << 8)  | \
                       (pEntry->ip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_IP6tf, value, entry, "", errHandle, ret);

            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            value[0] = (pEntry->bmsk_ip6.octet[0] << 24) | \
                       (pEntry->bmsk_ip6.octet[1] << 16) | \
                       (pEntry->bmsk_ip6.octet[2] << 8)  | \
                       (pEntry->bmsk_ip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_BMSK_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_ACTtf, pEntry->act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_ECMP_ENtf, pEntry->ecmp_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_NH_ECMP_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_0t, index, entry, "", errHandle, ret);

            /* IP6UC (N+1) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            value[1] = (pEntry->ip6.octet[4] << 8) | \
                       (pEntry->ip6.octet[5] << 0);
            value[0] = (pEntry->ip6.octet[6] << 24) | \
                       (pEntry->ip6.octet[7] << 16) | \
                       (pEntry->ip6.octet[8] << 8)  | \
                       (pEntry->ip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            value[1] = (pEntry->bmsk_ip6.octet[4] << 8) | \
                       (pEntry->bmsk_ip6.octet[5] << 0);
            value[0] = (pEntry->bmsk_ip6.octet[6] << 24) | \
                       (pEntry->bmsk_ip6.octet[7] << 16) | \
                       (pEntry->bmsk_ip6.octet[8] << 8)  | \
                       (pEntry->bmsk_ip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_1_BMSK_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_1t, (index + 1), entry, "", errHandle, ret);

            /* IP6UC (N+2) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            value[1] = (pEntry->ip6.octet[10] << 8) | \
                       (pEntry->ip6.octet[11] << 0);
            value[0] = (pEntry->ip6.octet[12] << 24) | \
                       (pEntry->ip6.octet[13] << 16) | \
                       (pEntry->ip6.octet[14] << 8)  | \
                       (pEntry->ip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_IP6tf, value, entry, "", errHandle, ret);

            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            value[1] = (pEntry->bmsk_ip6.octet[10] << 8) | \
                       (pEntry->bmsk_ip6.octet[11] << 0);
            value[0] = (pEntry->bmsk_ip6.octet[12] << 24) | \
                       (pEntry->bmsk_ip6.octet[13] << 16) | \
                       (pEntry->bmsk_ip6.octet[14] << 8)  | \
                       (pEntry->bmsk_ip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6UC_2_BMSK_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6UC_2t, (index + 2), entry, "", errHandle, ret);
        }
        else if (DAL_MANGO_L3_ENTRY_TYPE_IPV6_MULTICAST == pEntry->entry_type)
        {
            /* IPv6 multicast */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_VRF_IDtf, pEntry->vrf_id, entry, "", errHandle, ret);
            value[0] = (pEntry->sip6.octet[0] << 24) | \
                       (pEntry->sip6.octet[1] << 16) | \
                       (pEntry->sip6.octet[2] << 8)  | \
                       (pEntry->sip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_SIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_MC_KEY_SELtf, pEntry->mc_key_sel, entry, "", errHandle, ret);

            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_VRF_IDtf, pEntry->bmsk_vrf_id, entry, "", errHandle, ret);
            value[0] = (pEntry->bmsk_sip6.octet[0] << 24) | \
                       (pEntry->bmsk_sip6.octet[1] << 16) | \
                       (pEntry->bmsk_sip6.octet[2] << 8)  | \
                       (pEntry->bmsk_sip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_SIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_BMSK_MC_KEY_SELtf, pEntry->bmsk_mc_key_sel, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_TNL_LST_VALIDtf, pEntry->l2_mc_l2_tnl_lst_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_TNL_LST_IDXtf, pEntry->l2_mc_l2_tnl_lst_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_0_HITtf, pEntry->hit, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_0t, index, entry, "", errHandle, ret);

            /* IP6MC (N+1) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->sip6.octet[4] << 8) | \
                       (pEntry->sip6.octet[5] << 0);
            value[0] = (pEntry->sip6.octet[6] << 24) | \
                       (pEntry->sip6.octet[7] << 16) | \
                       (pEntry->sip6.octet[8] << 8)  | \
                       (pEntry->sip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_SIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->bmsk_sip6.octet[4] << 8) | \
                       (pEntry->bmsk_sip6.octet[5] << 0);
            value[0] = (pEntry->bmsk_sip6.octet[6] << 24) | \
                       (pEntry->bmsk_sip6.octet[7] << 16) | \
                       (pEntry->bmsk_sip6.octet[8] << 8)  | \
                       (pEntry->bmsk_sip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_BMSK_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_QOS_ENtf, pEntry->qos_en, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_L2_CHK_VIDtf, pEntry->l2_chk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_L3_RPF_IDtf, pEntry->l3_rpf_id, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_1_MTU_MAXtf, pEntry->mtu_max, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_1t, (index + 1), entry, "", errHandle, ret);

            /* IP6MC (N+2) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->sip6.octet[10] << 8) | \
                       (pEntry->sip6.octet[11] << 0);
            value[0] = (pEntry->sip6.octet[12] << 24) | \
                       (pEntry->sip6.octet[13] << 16) | \
                       (pEntry->sip6.octet[14] << 8)  | \
                       (pEntry->sip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_SIP6tf, value, entry, "", errHandle, ret);

            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->bmsk_sip6.octet[10] << 8) | \
                       (pEntry->bmsk_sip6.octet[11] << 0);
            value[0] = (pEntry->bmsk_sip6.octet[12] << 24) | \
                       (pEntry->bmsk_sip6.octet[13] << 16) | \
                       (pEntry->bmsk_sip6.octet[14] << 8)  | \
                       (pEntry->bmsk_sip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_2_BMSK_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_2t, (index + 2), entry, "", errHandle, ret);

            /* IP6MC (N+3) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[0] = (pEntry->gip6.octet[0] << 24) | \
                       (pEntry->gip6.octet[1] << 16) | \
                       (pEntry->gip6.octet[2] << 8)  | \
                       (pEntry->gip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_GIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_VIDtf, pEntry->vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_ROUNDtf, pEntry->round, entry, "", errHandle, ret);

            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            value[0] = (pEntry->bmsk_gip6.octet[0] << 24) | \
                       (pEntry->bmsk_gip6.octet[1] << 16) | \
                       (pEntry->bmsk_gip6.octet[2] << 8)  | \
                       (pEntry->bmsk_gip6.octet[3] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_GIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_3_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_3t, (index + 3), entry, "", errHandle, ret);

            /* IP6MC (N+4) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->gip6.octet[4] << 8) | \
                       (pEntry->gip6.octet[5] << 0);
            value[0] = (pEntry->gip6.octet[6] << 24) | \
                       (pEntry->gip6.octet[7] << 16) | \
                       (pEntry->gip6.octet[8] << 8)  | \
                       (pEntry->gip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_GIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->bmsk_gip6.octet[4] << 8) | \
                       (pEntry->bmsk_gip6.octet[5] << 0);
            value[0] = (pEntry->bmsk_gip6.octet[6] << 24) | \
                       (pEntry->bmsk_gip6.octet[7] << 16) | \
                       (pEntry->bmsk_gip6.octet[8] << 8)  | \
                       (pEntry->bmsk_gip6.octet[9] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_4_BMSK_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_4t, (index + 4), entry, "", errHandle, ret);

            /* IP6MC (N+5) */
            osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_FMTtf, pEntry->fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->gip6.octet[10] << 8) | \
                       (pEntry->gip6.octet[11] << 0);
            value[0] = (pEntry->gip6.octet[12] << 24) | \
                       (pEntry->gip6.octet[13] << 16) | \
                       (pEntry->gip6.octet[14] << 8)  | \
                       (pEntry->gip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_GIP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_BMSK_FMTtf, pEntry->bmsk_fmt, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
            value[1] = (pEntry->bmsk_gip6.octet[10] << 8) | \
                       (pEntry->bmsk_gip6.octet[11] << 0);
            value[0] = (pEntry->bmsk_gip6.octet[12] << 24) | \
                       (pEntry->bmsk_gip6.octet[13] << 16) | \
                       (pEntry->bmsk_gip6.octet[14] << 8)  | \
                       (pEntry->bmsk_gip6.octet[15] << 0);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, \
                MANGO_L3_PREFIX_ROUTE_IP6MC_5_BMSK_IP6tf, value, entry, "", errHandle, ret);
            L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_PREFIX_ROUTE_IP6MC_5t, (index + 5), entry, "", errHandle, ret);
        }
        else
        {
            /* Unknown type */
            ret = RT_ERR_FAILED;
            goto errHandle;
        }
    }
    else
    {
        /* Unknown format */
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

    MANGO_L3_DBG_PRINTF(3, "index = %u (0x%08x)\n", index, index);

errHandle:
errEntryNotFound:
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_SEM_UNLOCK(unit);
#else
    L3_INT_SEM_UNLOCK(unit);
#endif

    return ret;
}


/* internal APIs, called by dal_mango_l3_* APIs (should NOT lock/unlock semaphore inside) */
static inline int32 __dal_mango_l3_routeEntry_move(uint32 unit, uint32 dstIdx, uint32 srcIdx, uint32 length)
{
    int32 ret = RT_ERR_OK;
    uint32 chkTimes;
    uint32 exec;
    uint32 fromAddr, toAddr, cmd;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,dst=%d,src=%d,length=%d", unit, dstIdx, srcIdx, length);

    MANGO_L3_DBG_PRINTF(3, "unit=%d,dst=%d,src=%d,length=%d\n", unit, dstIdx, srcIdx, length);

    /* parameter check */
    RT_PARAM_CHK((dstIdx == srcIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((0 == length), RT_ERR_INPUT);
    RT_PARAM_CHK(((dstIdx + length - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((srcIdx + length - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((length > L3_ROUTE_TBL_SIZE(unit)), RT_ERR_OUT_OF_RANGE);

    /* check status until it's idle */
    chkTimes = DAL_MANGO_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes) {
            MANGO_L3_DBG_PRINTF(0, "Error: timeout chkTimes=%d\n", chkTimes);
            return RT_ERR_FAILED;
        }
    } while (exec != 0);

    /* entry address */
    if (srcIdx > dstIdx)
    {
        fromAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(srcIdx);
        toAddr   = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(dstIdx);
    } else {
        fromAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(srcIdx + length - 1);
        toAddr   = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(dstIdx + length - 1);
    }

    /* configure register */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_ENTRY_MV_PARAMr, MANGO_LENf, length, "", errHandle, ret);

    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_TOf, toAddr, value, "", errHandle, ret);
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_FROMf, fromAddr, value, "", errHandle, ret);
    cmd = 1; /* move */
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_CMDf, cmd, value, "", errHandle, ret);
    exec = 1; /* execute */
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf, exec, value, "", errHandle, ret);
    L3_REG_WRITE_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, value, "", errHandle, ret);

    /* check status until it's done */
    chkTimes = DAL_MANGO_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes) break;   /* force to leave */
    } while (exec != 0);

    MANGO_L3_DBG_PRINTF(3, "chkTimes = %d (%d)\n", chkTimes, DAL_MANGO_L3_CHK_IDLE_TIMES - chkTimes);

    ret = RT_ERR_OK;    /* success */

errHandle:
    return ret;
}

/* internal APIs, called by dal_mango_l3_* APIs (should NOT lock/unlock semaphore inside) */
static inline int32 __dal_mango_l3_routeEntry_clear(uint32 unit, uint32 baseIdx, uint32 length)
{
    int32 ret = RT_ERR_OK;
    uint32 chkTimes;
    uint32 exec;
    uint32 baseAddr, cmd;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,baseIdx=%d,length=%d", unit, baseIdx, length);

    /* parameter check */
    RT_PARAM_CHK((0 == length), RT_ERR_INPUT);
    RT_PARAM_CHK(((baseIdx + length - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((length > L3_ROUTE_TBL_SIZE(unit)), RT_ERR_OUT_OF_RANGE);

    /* check status until it's idle */
    chkTimes = DAL_MANGO_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes) {
            MANGO_L3_DBG_PRINTF(0, "Error: timeout chkTimes=%d\n", chkTimes);
            return RT_ERR_FAILED;
        }
    } while (exec != 0);

    /* entry address */
    baseAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(baseIdx);

    /* configure register */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_ENTRY_MV_PARAMr, MANGO_LENf, length, "", errHandle, ret);

    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_FROMf, baseAddr, value, "", errHandle, ret);
    cmd = 0; /* clear */
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_CMDf, cmd, value, "", errHandle, ret);
    exec = 1; /* execute */
    L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf, exec, value, "", errHandle, ret);
    L3_REG_WRITE_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, value, "", errHandle, ret);

    /* check status until it's done */
    chkTimes = DAL_MANGO_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ENTRY_MV_CTRLr, MANGO_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes) break;   /* force to leave */
    } while (exec != 0);

    MANGO_L3_DBG_PRINTF(3, "chkTimes = %d (%d)\n", chkTimes, DAL_MANGO_L3_CHK_IDLE_TIMES - chkTimes);

    ret = RT_ERR_OK;    /* success */

errHandle:
    return ret;
}


static int32
__dal_mango_l3_routeEntry_alloc(uint32 unit, uint32 ipv6, uint32 pfLen, uint32 *pIndex)
{
    int32   ret = RT_ERR_OK;
    uint32  dstIdx, srcIdx, length;
    int32   pfl;
    int32   i = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ipv6=%d,pfLen=%d,pIndex=0x%08x", unit, ipv6, pfLen, pIndex);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((TRUE == ipv6) && (pfLen > 128)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((FALSE == ipv6) && (pfLen > 32)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_SEM_LOCK(unit);
#endif

    if (TRUE == ipv6)
    {
        /* IPv6 UC route entry allocation */

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        if ((L3_ROUTE_TBL_IP6UC_LEFT(unit) + L3_ROUTE_TBL_BLANK_LEFT(unit)) < 3)
#else
        if ((L3_ROUTE_TBL_SIZE(unit) - L3_ROUTE_TBL_USED(unit)) < 3)
#endif
        {
            ret = RT_ERR_TBL_FULL;
            goto errHandle;
        }

        if (pfLen >= MANGO_L3_HOST_IP6_PFLEN)
        {
            /* use the top entry directly */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
            *pIndex = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) + 1));
#else
            *pIndex = L3_ROUTE_TBL_SIZE(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) + 1));
#endif
        } else {
            length = 3;
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
            dstIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) + 1)); /* top */
#else
            dstIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) + 1)); /* top */
#endif
            srcIdx = dstIdx;
            for (pfl=128; pfl>pfLen; pfl--)
            {
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
                srcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfl-1) + 1));
#else
                srcIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfl-1) + 1));
#endif
                if (dstIdx != srcIdx)
                {
                    /* move */
                    MANGO_L3_DBG_PRINTF(3, "v6MV0: pfl=%d, dstIdx=%d, srcIdx=%d, length=%d; pfl=%d, pfLen=%d\n", pfl, dstIdx, srcIdx, length, pfl, pfLen);
                    RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                    /* update shadow */
                    _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                    _pL3Db[unit]->route[srcIdx].valid = 0;

                    dstIdx = srcIdx;    /* new dstIdx */
                }
            }

            *pIndex = srcIdx;
        }

        for (i = pfLen; i > 0; i--)
        {
            L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, i - 1) += 1;
        }

        L3_ROUTE_TBL_IP6_CNT(unit) += 1;
    } else {
        /* IPv4 UC route entry allocation */

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        if ((L3_ROUTE_TBL_IPUC_LEFT(unit) + L3_ROUTE_TBL_BLANK_LEFT(unit)) < 1)
#else
        if ((L3_ROUTE_TBL_SIZE(unit) - L3_ROUTE_TBL_USED(unit)) < 1)
#endif
        {
            ret = RT_ERR_TBL_FULL;
            goto errHandle;
        }

        if (0 == pfLen)
        {
            /* use the bottom entry directly */
            *pIndex = L3_ROUTE_TBL_IP_CNT(unit);
        } else {
            length = 1;
            dstIdx = L3_ROUTE_TBL_IP_CNT(unit); /* bottom */
            srcIdx = dstIdx;
            for (pfl=0; pfl<pfLen; pfl++)
            {
                srcIdx = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfl);
                if (dstIdx != srcIdx)
                {
                    /* move */
                    MANGO_L3_DBG_PRINTF(3, "v4MV0: pfl=%d, dstIdx=%d, srcIdx=%d, length=%d; pfl=%d, pfLen=%d\n", pfl, dstIdx, srcIdx, length, pfl, pfLen);
                    RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                    /* update shadow */
                    _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                    _pL3Db[unit]->route[srcIdx].valid = 0;

                    dstIdx = srcIdx;    /* new dstIdx */
                }
            }

            *pIndex = srcIdx;
        }

        for (i = pfLen; i < MANGO_L3_HOST_IP_PFLEN; i++)
        {
            L3_ROUTE_TBL_IP_CNT_PFLEN(unit, i) += 1;
        }

        L3_ROUTE_TBL_IP_CNT(unit) += 1;
    }

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
#endif

    MANGO_L3_DBG_PRINTF(3, "pfLen = %d, *pIndex = %d\n", pfLen, *pIndex);

errHandle:
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_SEM_UNLOCK(unit);
#endif

    return ret;
}

/* internal APIs, called by dal_mango_l3_* APIs (should NOT lock/unlock semaphore inside) */
static inline int32 __dal_mango_l3_routeEntry_find(uint32 unit, rtk_l3_route_t *pRoute, uint32 *pRouteIdx, dal_mango_l3_routeEntry_t *pRouteEntry)
{
    int32   ret = RT_ERR_OK;
    rtk_l3_route_t   rtkRoute;
    uint32  pfLen;
    uint32  minIdx, length;
    uint32  routeIdx;
#if MANGO_L3_ROUTE_HW_LU
    int32   hwIdx;
#endif

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p,pRouteIdx=%p", unit, pRoute, pRouteIdx);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRouteIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRouteEntry), RT_ERR_NULL_POINTER);

    pfLen = pRoute->prefix_len;

#if MANGO_L3_ROUTE_HW_LU
    /* improve the performance by using H/W lookup function */
    RT_ERR_HDL(__dal_mango_l3_routeEntry_hwLookup(unit, pRoute, &hwIdx), errHandle, ret);
    MANGO_L3_DBG_PRINTF(3, "HWLU-HIT: pRoute->pfLen = %d, hwIdx = %d\n", pRoute->prefix_len, hwIdx);
#endif

    /* search H/W */
    if (pRoute->flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 */
        if (pfLen >= MANGO_L3_HOST_IP6_PFLEN)
        {
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
            minIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#else
            minIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#endif
            length = 3 * L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, (MANGO_L3_HOST_IP6_PFLEN - 1));
        }
        else if (pfLen > 0)
        {
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
            minIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen)));
#else
            minIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen)));
#endif
            length = 3 * (L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, (pfLen - 1)) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen));
        }
        else /* pfLen == 0 */
        {
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
            minIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen)));
#else
            minIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen)));
#endif
            length = 3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen));
        }

        MANGO_L3_DBG_PRINTF(3, "minIdx = %d, length = %d\n", minIdx, length);

#if MANGO_L3_ROUTE_HW_LU
        /* fast return */
        if ((hwIdx >= 0) && (hwIdx >= minIdx) && (hwIdx < (minIdx + length)))
        {
            MANGO_L3_DBG_PRINTF(3, "HWLU-HIT: fast-return, hwIdx = %d (minIdx=%d,length=%d)\n", hwIdx, minIdx, length);

            /* get entry from H/W */
            RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, hwIdx, pRouteEntry, ENABLED), errHandle, ret);

            *pRouteIdx = hwIdx;
            goto errOk;
        }
        else if ((hwIdx >= 0) && (hwIdx >= (minIdx + length)))
        {
            return RT_ERR_ENTRY_NOTFOUND;
        }
#endif

        for (routeIdx = minIdx; (routeIdx - minIdx) < length; routeIdx += 3)
        {
            /* get entry from H/W */
            RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, routeIdx, pRouteEntry, ENABLED), errHandle, ret);
            RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, pRouteEntry), errHandle, ret);

            MANGO_L3_DBG_PRINTF(3, "routeIdx = %d, pfLen = %d, rtkRoute->pfLen = %d\n", routeIdx, pfLen, rtkRoute.prefix_len);

            if ((pRoute->vrf_id == rtkRoute.vrf_id) && \
                (pfLen == rt_util_ipv6MaxMatchLength_ret(&rtkRoute.ip_addr.ipv6, &pRoute->ip_addr.ipv6, pfLen)))
            {
                /* found the entry, return hostIdx */
                *pRouteIdx = routeIdx;
                goto errOk;
            }
        }
    } else {
        /* IPv4 */
        if (pfLen >= MANGO_L3_HOST_IP_PFLEN)
        {
            minIdx = 0;
            length = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, (MANGO_L3_HOST_IP_PFLEN - 1));
        }
        else if (pfLen > 0)
        {
            minIdx = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen);
            length = L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, (pfLen - 1));
        }
        else /* pfLen == 0 */
        {
            minIdx = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen);
            length = L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen);
        }

#if MANGO_L3_ROUTE_HW_LU
        /* fast return */
        if ((hwIdx >= 0) && (hwIdx >= minIdx) && (hwIdx < (minIdx + length)))
        {
            MANGO_L3_DBG_PRINTF(3, "HWLU-HIT: fast-return, hwIdx = %d (minIdx=%d,length=%d)\n", hwIdx, minIdx, length);

            /* get entry from H/W */
            RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, hwIdx, pRouteEntry, ENABLED), errHandle, ret);

            *pRouteIdx = hwIdx;
            goto errOk;
        }
        else if ((hwIdx >= 0) && (hwIdx >= (minIdx + length)))
        {
            return RT_ERR_ENTRY_NOTFOUND;
        }
#endif

        for (routeIdx = minIdx; (routeIdx - minIdx) < length; routeIdx++)
        {
            /* get entry from H/W */
            RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, routeIdx, pRouteEntry, ENABLED), errHandle, ret);
            RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, pRouteEntry), errHandle, ret);

            if ((pRoute->vrf_id == rtkRoute.vrf_id) && \
                (pfLen == rt_util_ipMaxMatchLength_ret(rtkRoute.ip_addr.ipv4, pRoute->ip_addr.ipv4, pfLen)))
            {
                /* found the entry, return hostIdx */
                *pRouteIdx = routeIdx;
                goto errOk;
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    return ret;
}

static int32
__dal_mango_l3_routeEntry_free(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_routeEntry_t routeEntry;
    uint32  dstIdx, srcIdx, length;
    uint32  pfLen;
    int32   i = 0;
#if 0
    int32   offset;
#else
    int32   pfl, top, bottom;
#endif

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_SEM_LOCK(unit);
#endif

    if ((L3_ROUTE_TBL_IP_CNT(unit) > 0) && (index < L3_ROUTE_TBL_IP_CNT(unit)))
    {
        /* IPv4 */
        RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, index, &routeEntry, ENABLED), errHandle, ret);
        pfLen = rt_util_ipMask2Length_ret(routeEntry.bmsk_ip);

        length = 1;
        bottom = (L3_ROUTE_TBL_IP_CNT(unit) - 1);
        /* have more entries ? */
        if (index < bottom)
        {
            dstIdx = index;
            for (pfl=pfLen; pfl>0; pfl--)
            {
                srcIdx = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfl-1) - 1;
                if (dstIdx != srcIdx)
                {
                    /* move */
                    MANGO_L3_DBG_PRINTF(3, "v4MV1: pfl=%d, dstIdx=%d, srcIdx=%d, length=%d; pfl=%d, prefix_len=%d\n", pfl, dstIdx, srcIdx, length, pfl, _pL3Db[unit]->route[i].shadow.routeEntry.prefix_len);
                    RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                    /* update shadow */
                    _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                    _pL3Db[unit]->route[srcIdx].valid = 0;

                    dstIdx = srcIdx;    /* new dstIdx */
                }
            }

            if (dstIdx != bottom)
            {
                /* move the last entry */
                srcIdx = bottom;
                MANGO_L3_DBG_PRINTF(3, "v4MV2: i=%d, dstIdx=%d, srcIdx=%d, length=%d; pfl=%d, prefix_len=%d\n", i, dstIdx, srcIdx, length, pfl, _pL3Db[unit]->route[i].shadow.routeEntry.prefix_len);
                RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                /* update shadow */
                _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                _pL3Db[unit]->route[srcIdx].valid = 0;
            }
        } else {
            osal_memset(&_pL3Db[unit]->route[index], 0x00, sizeof(_pL3Db[unit]->route[index]));
        }

        /* clear the last entry */
        MANGO_L3_DBG_PRINTF(3, "v4CLR: bottom=%d, length=%d\n", bottom, length);
        RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, bottom, length), errHandle, ret);

        /* update counters */
        for (i = pfLen; i < MANGO_L3_HOST_IP_PFLEN; i++)
        {
            L3_ROUTE_TBL_IP_CNT_PFLEN(unit, i) -= 1;
        }

        L3_ROUTE_TBL_IP_CNT(unit) -= 1;
    }
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    else if ((L3_ROUTE_TBL_IP6_CNT(unit) > 0) && \
             (index >= (L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit)))))
#else
    else if ((L3_ROUTE_TBL_IP6_CNT(unit) > 0) && \
             (index >= (L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit)))))
#endif
    {
        /* IPv6 */
        if ((index % 3) != 0)
        {
            ret = RT_ERR_INPUT;
            goto errHandle;
        }

        RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, index, &routeEntry, ENABLED), errHandle, ret);
        pfLen = rt_util_ip6Mask2Length_ret(&routeEntry.bmsk_ip6);

        length = 3;
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        top = (L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit))));
#else
        top = (L3_ROUTE_TBL_SIZE(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit))));
#endif
        /* have more entries ? */
        if (index > top)
        {
            dstIdx = index;
            for (pfl=pfLen; pfl<128; pfl++)
            {
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
                srcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfl)));
#else
                srcIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * (L3_ROUTE_TBL_IP6_CNT(unit) - L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfl)));
#endif
                if (dstIdx != srcIdx)
                {
                    /* move */
                    MANGO_L3_DBG_PRINTF(3, "v6MV1: pfl=%d, dstIdx=%d, srcIdx=%d, length=%d; pfl=%d, prefix_len=%d\n", pfl, dstIdx, srcIdx, length, pfl, _pL3Db[unit]->route[i].shadow.routeEntry.prefix_len);
                    RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                    /* update shadow */
                    _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                    _pL3Db[unit]->route[srcIdx].valid = 0;

                    dstIdx = srcIdx;    /* new dstIdx */
                }
            }

            if (dstIdx != top)
            {
                /* move the last entry */
                srcIdx = top;
                MANGO_L3_DBG_PRINTF(3, "v6MV2: i=%d, dstIdx=%d, srcIdx=%d, length=%d; pfl=%d, prefix_len=%d\n", i, dstIdx, srcIdx, length, pfl, _pL3Db[unit]->route[i].shadow.routeEntry.prefix_len);
                RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                /* update shadow */
                _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                _pL3Db[unit]->route[srcIdx].valid = 0;
            }
        } else {
            osal_memset(&_pL3Db[unit]->route[index], 0x00, sizeof(_pL3Db[unit]->route[index]));
        }

        /* clear the last entry */
        MANGO_L3_DBG_PRINTF(3, "v6CLR: top=%d, length=%d\n", top, length);
        RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, top, length), errHandle, ret);

        /* update counters */
        for (i = pfLen; i > 0; i--)
        {
            L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, i - 1) -= 1;
        }

        L3_ROUTE_TBL_IP6_CNT(unit) -= 1;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
#endif

errHandle:
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_SEM_UNLOCK(unit);
#endif

    return ret;
}

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
int32
__dal_mango_l3_routeIpmcEntry_alloc(uint32 unit, uint32 ipv6, uint32 *pIndex, void *pEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  dstIdx, srcIdx, length;
    int32   i;
    uint32  dbDstIdx, dbSrcIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ipv6=%d,pIndex=0x%08x,pEntry=0x%08x", unit, ipv6, pIndex, pEntry);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);
    L3_ROUTE_SEM_LOCK(unit);

    if (TRUE == ipv6)
    {
        /* IPv6 MC route entry allocation */

        if (L3_ROUTE_TBL_BLANK_LEFT(unit) < 6)
        {
            ret = RT_ERR_TBL_FULL;
            goto errHandle;
        }

        /* expand IP6MC space since BLANK space is enough */

        /* move block (BLANK_MAX ~ IPMC_MAX-1) to (BLANK_MAX-6) */
        dstIdx = L3_ROUTE_TBL_BLANK_MAX(unit) - L3_ROUTE_ENTRY_BUCKET_LEN;
        srcIdx = L3_ROUTE_TBL_BLANK_MAX(unit);
        length = L3_ROUTE_TBL_IPMC_MAX(unit) - L3_ROUTE_TBL_BLANK_MAX(unit);

        MANGO_L3_DBG_PRINTF(3, "IP6MC_ALLOC(move block): dstIdx=%d, srcIdx=%d, length=%d\n", dstIdx, srcIdx, length);
        MANGO_L3_DBG_PRINTF(3, "(BLANK_MAX ~ IPMC_MAX-1) to (BLANK_MAX-6)\n");
        if (length > 0)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

            /* updated shadow (IP6UC, IPMC), move block upward */
            for (i=0; i<(L3_ROUTE_TBL_IP6UC_MAX(unit) - L3_ROUTE_TBL_BLANK_MAX(unit)); i+=L3_ROUTE_ENTRY_IP6UC_LEN)
            {
                dbSrcIdx = L3_ROUTE_TBL_BLANK_MAX(unit) + i;
                dbDstIdx = dbSrcIdx + (dstIdx - srcIdx);

                if (_pL3Db[unit]->route[dbSrcIdx].valid)
                {
                    MANGO_L3_DBG_PRINTF(3, "IP6UC_DB_MOVE(&CLR-SRC): %u -> %u\n", dbSrcIdx, dbDstIdx);
                    _pL3Db[unit]->route[dbDstIdx] = _pL3Db[unit]->route[dbSrcIdx];
                    osal_memset(&_pL3Db[unit]->route[dbSrcIdx], 0x00, sizeof(_pL3Db[unit]->route[dbSrcIdx]));
                }
            }
            for (i=0; i<(L3_ROUTE_TBL_IPMC_MAX(unit) - L3_ROUTE_TBL_IP6UC_MAX(unit)); i+=L3_ROUTE_ENTRY_IPMC_LEN)
            {
                dbSrcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) + i;
                dbDstIdx = dbSrcIdx + (dstIdx - srcIdx);

                if (_pL3Db[unit]->route[dbSrcIdx].valid)
                {
                    MANGO_L3_DBG_PRINTF(3, "IPMC_DB_MOVE(&CLR-SRC): %u -> %u\n", dbSrcIdx, dbDstIdx);
                    _pL3Db[unit]->route[dbDstIdx] = _pL3Db[unit]->route[dbSrcIdx];
                    osal_memset(&_pL3Db[unit]->route[dbSrcIdx], 0x00, sizeof(_pL3Db[unit]->route[dbSrcIdx]));
                    if (_pL3Db[unit]->route[dbDstIdx].ipmc) // just in case
                    {
                        RT_ERR_HDL(_dal_mango_ipmc_l3RouteIdxChange_callback(unit, _pL3Db[unit]->route[dbDstIdx].pIpmcEntry, dbDstIdx), errHandle, ret);
                    }
                }
            }
        }

        /* (NOTE) don't need to clear the bottom entry, it should be replaced by caller later */

        /* updated CNT */
        L3_ROUTE_TBL_IP6MC_CNT(unit) += 1;
        L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
        *pIndex = L3_ROUTE_TBL_IPMC_MAX(unit);  /* top entry of IP6MC space */
    } else {
        /* IPv4 MC route entry allocation */

        if (L3_ROUTE_TBL_IPMC_LEFT(unit))
        {
            /* use the left space to store the new entry */

            /* (NOTE) don't need to clear the new entry, it should be empty currently */

            /* updated CNT */
            L3_ROUTE_TBL_IPMC_CNT(unit) += 1;
            *pIndex = L3_ROUTE_TBL_IP6UC_MAX(unit) + L3_ROUTE_TBL_IPMC_LEFT(unit);
        }
        else if (L3_ROUTE_TBL_BLANK_LEFT(unit) >= 1)
        {
            /* expand IPMC space since BLANK space is enough */

            /* move block (BLANK_MAX ~ IP6UC_MAX-1) to (BLANK_MAX-6) */
            dstIdx = L3_ROUTE_TBL_BLANK_MAX(unit) - L3_ROUTE_ENTRY_BUCKET_LEN;
            srcIdx = L3_ROUTE_TBL_BLANK_MAX(unit);
            length = L3_ROUTE_TBL_IP6UC_MAX(unit) - L3_ROUTE_TBL_BLANK_MAX(unit);

            MANGO_L3_DBG_PRINTF(3, "IPMC_ALLOC(move block): dstIdx=%d, srcIdx=%d, length=%d\n", dstIdx, srcIdx, length);
            MANGO_L3_DBG_PRINTF(3, "(BLANK_MAX ~ IP6UC_MAX-1) to (BLANK_MAX-6)\n");
            if (length > 0)
            {
                RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                /* updated shadow (IP6UC) */
                /* (IP6UC shadow update) upward */
                for (i=0; i<(L3_ROUTE_TBL_IP6UC_MAX(unit) - L3_ROUTE_TBL_BLANK_MAX(unit)); i+=L3_ROUTE_ENTRY_IP6UC_LEN)
                {
                    dbSrcIdx = L3_ROUTE_TBL_BLANK_MAX(unit) + i;
                    dbDstIdx = dbSrcIdx + (dstIdx - srcIdx);

                    if (_pL3Db[unit]->route[dbSrcIdx].valid)
                    {
                        MANGO_L3_DBG_PRINTF(3, "IP6UC_DB_MOVE(&CLR-SRC): %u -> %u\n", dbSrcIdx, dbDstIdx);
                        _pL3Db[unit]->route[dbDstIdx] = _pL3Db[unit]->route[dbSrcIdx];
                        osal_memset(&_pL3Db[unit]->route[dbSrcIdx], 0x00, sizeof(_pL3Db[unit]->route[dbSrcIdx]));
                    }
                }
            }

            /* (NOTE) don't need to clear the new entry, it should be empty currently */

            /* updated CNT */
            L3_ROUTE_TBL_IPMC_CNT(unit) += 1;
            L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
            *pIndex = L3_ROUTE_TBL_IP6UC_MAX(unit) + L3_ROUTE_TBL_IPMC_LEFT(unit);
        }
        else
        {
            ret = RT_ERR_TBL_FULL;
            goto errHandle;
        }
    }

    /* update shadow */
    _pL3Db[unit]->route[(*pIndex)].valid        = TRUE;
    _pL3Db[unit]->route[(*pIndex)].ipv6         = ipv6;
    _pL3Db[unit]->route[(*pIndex)].ipmc         = TRUE;
    _pL3Db[unit]->route[(*pIndex)].pIpmcEntry   = pEntry;

    MANGO_L3_DBG_PRINTF(3, "*pIndex = %d\n", *pIndex);

errHandle:
    L3_ROUTE_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}

int32
__dal_mango_l3_routeIpmcEntry_free(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    uint32  dstIdx, srcIdx, length;
    int32   i;
    uint32  dbDstIdx, dbSrcIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);
    L3_ROUTE_SEM_LOCK(unit);

    if ((index >= L3_ROUTE_TBL_IP6UC_MAX(unit)) && (index < L3_ROUTE_TBL_IPMC_MAX(unit)))
    {
        /* IPv4 (IPMC entry) */

        if (index != (L3_ROUTE_TBL_IP6UC_MAX(unit) + L3_ROUTE_TBL_IPMC_LEFT(unit)))
        {
            /* move the top entry here */
            dstIdx = index;
            srcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) + L3_ROUTE_TBL_IPMC_LEFT(unit);
            length = L3_ROUTE_ENTRY_IPMC_LEN;

            if (_pL3Db[unit]->route[srcIdx].valid)
            {
                MANGO_L3_DBG_PRINTF(3, "IPMC_FREE(move single entry): dstIdx=%d, srcIdx=%d, length=%d\n", dstIdx, srcIdx, length);
                RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                /* update shadow (IPMC) */
                MANGO_L3_DBG_PRINTF(3, "IPMC_DB_MOVE(&CLR-SRC): %u -> %u\n", srcIdx, dstIdx);
                _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                osal_memset(&_pL3Db[unit]->route[srcIdx], 0x00, sizeof(_pL3Db[unit]->route[srcIdx]));
                if (_pL3Db[unit]->route[dstIdx].ipmc) // just in case
                {
                    RT_ERR_HDL(_dal_mango_ipmc_l3RouteIdxChange_callback(unit, _pL3Db[unit]->route[dstIdx].pIpmcEntry, dstIdx), errHandle, ret);
                }
            }
        } else {
            /* clear the target first */
            osal_memset(&_pL3Db[unit]->route[index], 0x00, sizeof(_pL3Db[unit]->route[index]));
        }

        /* updated CNT */
        L3_ROUTE_TBL_IPMC_CNT(unit) -= 1;

        if (L3_ROUTE_TBL_IPMC_LEFT(unit) >= L3_ROUTE_ENTRY_BUCKET_LEN)
        {
            /* move block (BLANK_MAX ~ IP6UC_MAX-1) to (BLANK_MAX+6) */
            dstIdx = L3_ROUTE_TBL_BLANK_MAX(unit) + L3_ROUTE_ENTRY_BUCKET_LEN;
            srcIdx = L3_ROUTE_TBL_BLANK_MAX(unit);
            length = L3_ROUTE_TBL_IP6UC_MAX(unit) - L3_ROUTE_TBL_BLANK_MAX(unit);

            MANGO_L3_DBG_PRINTF(3, "IPMC_FREE(move block): dstIdx=%d, srcIdx=%d, length=%d\n", dstIdx, srcIdx, length);
            MANGO_L3_DBG_PRINTF(3, "(BLANK_MAX ~ IP6UC_MAX-1) to (BLANK_MAX+6)\n");
            if (length > 0)
            {
                RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                /* updated shadow (IP6UC) */
                /* (IP6UC shadow update) downward */
                for (i=0; i<(L3_ROUTE_TBL_IP6UC_MAX(unit)-L3_ROUTE_TBL_BLANK_MAX(unit)); i+=L3_ROUTE_ENTRY_IP6UC_LEN)
                {
                    dbSrcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - L3_ROUTE_ENTRY_IP6UC_LEN - i;
                    dbDstIdx = dbSrcIdx + (dstIdx - srcIdx);

                    if (_pL3Db[unit]->route[dbSrcIdx].valid)
                    {
                        MANGO_L3_DBG_PRINTF(3, "IP6UC_DB_MOVE(&CLR-SRC): %u -> %u\n", dbSrcIdx, dbDstIdx);
                        _pL3Db[unit]->route[dbDstIdx] = _pL3Db[unit]->route[dbSrcIdx];
                        osal_memset(&_pL3Db[unit]->route[dbSrcIdx], 0x00, sizeof(_pL3Db[unit]->route[dbSrcIdx]));
                    } else {
                        osal_memset(&_pL3Db[unit]->route[dbDstIdx], 0x00, sizeof(_pL3Db[unit]->route[dbDstIdx]));
                    }
                }

                /* clear the original top entry of BLANK space (no need since it's cleared every move) */
            } else {
                srcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) + L3_ROUTE_TBL_IPMC_LEFT(unit) - L3_ROUTE_ENTRY_IPMC_LEN;
                MANGO_L3_DBG_PRINTF(3, "IPMC_FREE(clear entry): srcIdx=%d, length=%d\n", srcIdx, L3_ROUTE_ENTRY_IPMC_LEN);
                RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, srcIdx, L3_ROUTE_ENTRY_IPMC_LEN), errHandle, ret);
            }
        } else {
            /* clear the original top entry of IPMC space */
            srcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) + L3_ROUTE_TBL_IPMC_LEFT(unit) - L3_ROUTE_ENTRY_IPMC_LEN;
            MANGO_L3_DBG_PRINTF(3, "IPMC_FREE(clear entry): srcIdx=%d, length=%d\n", srcIdx, L3_ROUTE_ENTRY_IPMC_LEN);
            RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, srcIdx, L3_ROUTE_ENTRY_IPMC_LEN), errHandle, ret);
        }

        L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
    }
    else if ((index >= L3_ROUTE_TBL_IPMC_MAX(unit)) && (index < L3_ROUTE_TBL_IP6MC_MAX(unit)))
    {
        /* IPv6 (IP6MC entry) */

        if (index != L3_ROUTE_TBL_IPMC_MAX(unit))
        {
            /* move the top entry here */
            dstIdx = index;
            srcIdx = L3_ROUTE_TBL_IPMC_MAX(unit);
            length = L3_ROUTE_ENTRY_IP6MC_LEN;

            if (_pL3Db[unit]->route[srcIdx].valid)
            {
                MANGO_L3_DBG_PRINTF(3, "IP6MC_FREE(move single entry): dstIdx=%d, srcIdx=%d, length=%d\n", dstIdx, srcIdx, length);
                RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

                /* update shadow (IP6MC) */
                _pL3Db[unit]->route[dstIdx] = _pL3Db[unit]->route[srcIdx];
                osal_memset(&_pL3Db[unit]->route[srcIdx], 0x00, sizeof(_pL3Db[unit]->route[srcIdx]));
                if (_pL3Db[unit]->route[dstIdx].ipmc) // just in case
                {
                    RT_ERR_HDL(_dal_mango_ipmc_l3RouteIdxChange_callback(unit, _pL3Db[unit]->route[dstIdx].pIpmcEntry, dstIdx), errHandle, ret);
                }
            }
        } else {
            /* clear the target first */
            osal_memset(&_pL3Db[unit]->route[index], 0x00, sizeof(_pL3Db[unit]->route[index]));
        }

        /* updated CNT */
        L3_ROUTE_TBL_IP6MC_CNT(unit) -= 1;

        /* move block (BLANK_MAX ~ IPMC_MAX-1) to (BLANK_MAX+6) */
        dstIdx = L3_ROUTE_TBL_BLANK_MAX(unit) + L3_ROUTE_ENTRY_BUCKET_LEN;
        srcIdx = L3_ROUTE_TBL_BLANK_MAX(unit);
        length = L3_ROUTE_TBL_IPMC_MAX(unit) - L3_ROUTE_TBL_BLANK_MAX(unit);

        MANGO_L3_DBG_PRINTF(3, "IP6MC_FREE(move block): dstIdx=%d, srcIdx=%d, length=%d\n", dstIdx, srcIdx, length);
        MANGO_L3_DBG_PRINTF(3, "(BLANK_MAX ~ IPMC_MAX-1) to (BLANK_MAX+6)\n");
        if (length > 0)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_move(unit, dstIdx, srcIdx, length), errHandle, ret);

            /* updated shadow (IPMC, IP6UC) */

            /* (IPMC shadow update) downward */
            for (i=0; i<(L3_ROUTE_TBL_IPMC_MAX(unit)-L3_ROUTE_TBL_IP6UC_MAX(unit)); i+=L3_ROUTE_ENTRY_IPMC_LEN)
            {
                dbSrcIdx = L3_ROUTE_TBL_IPMC_MAX(unit) - L3_ROUTE_ENTRY_IPMC_LEN - i;
                dbDstIdx = dbSrcIdx + (dstIdx - srcIdx);

                if (_pL3Db[unit]->route[dbSrcIdx].valid)
                {
                    MANGO_L3_DBG_PRINTF(3, "IPMC_DB_MOVE(&CLR-SRC): %u -> %u\n", dbSrcIdx, dbDstIdx);
                    _pL3Db[unit]->route[dbDstIdx] = _pL3Db[unit]->route[dbSrcIdx];
                    osal_memset(&_pL3Db[unit]->route[dbSrcIdx], 0x00, sizeof(_pL3Db[unit]->route[dbSrcIdx]));
                    if (_pL3Db[unit]->route[dbDstIdx].ipmc) // just in case
                    {
                        RT_ERR_HDL(_dal_mango_ipmc_l3RouteIdxChange_callback(unit, _pL3Db[unit]->route[dbDstIdx].pIpmcEntry, dbDstIdx), errHandle, ret);
                    }
                } else {
                    osal_memset(&_pL3Db[unit]->route[dbDstIdx], 0x00, sizeof(_pL3Db[unit]->route[dbDstIdx]));
                }
            }
            /* (IP6UC shadow update) downward */
            for (i=0; i<(L3_ROUTE_TBL_IP6UC_MAX(unit)-L3_ROUTE_TBL_BLANK_MAX(unit)); i+=L3_ROUTE_ENTRY_IP6UC_LEN)
            {
                dbSrcIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - L3_ROUTE_ENTRY_IP6UC_LEN - i;
                dbDstIdx = dbSrcIdx + (dstIdx - srcIdx);

                if (_pL3Db[unit]->route[dbSrcIdx].valid)
                {
                    MANGO_L3_DBG_PRINTF(3, "IP6UC_DB_MOVE(&CLR-SRC): %u -> %u\n", dbSrcIdx, dbDstIdx);
                    _pL3Db[unit]->route[dbDstIdx] = _pL3Db[unit]->route[dbSrcIdx];
                    osal_memset(&_pL3Db[unit]->route[dbSrcIdx], 0x00, sizeof(_pL3Db[unit]->route[dbSrcIdx]));
                } else {
                    osal_memset(&_pL3Db[unit]->route[dbDstIdx], 0x00, sizeof(_pL3Db[unit]->route[dbDstIdx]));
                }
            }

            /* clear the original top entry of BLANK space (no need since it's cleared every move) */
        } else {
            /* clear the original top entry of IP6MC space */
            srcIdx = L3_ROUTE_TBL_IPMC_MAX(unit);
            MANGO_L3_DBG_PRINTF(3, "IP6MC_FREE(clear entry): srcIdx=%d, length=%d\n", srcIdx, L3_ROUTE_ENTRY_IP6MC_LEN);
            RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, srcIdx, L3_ROUTE_ENTRY_IP6MC_LEN), errHandle, ret);
        }

        L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

errHandle:
    L3_ROUTE_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}
#endif

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
int32
_dal_mango_l3_ipRouteResult_get(uint32 unit, rtk_l3_key_t *pKey, dal_mango_l3_ipRouteResult_t *pResult)
{
    MANGO_L3_DBG_PRINTF(0, "TBC\n");

    return RT_ERR_OK;
}

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
int32
_dal_mango_l3_pathInfo_get(uint32 unit, rtk_l3_pathId_t pathId, dal_mango_l3_pathInfo_t *pPathInfo)
{
    int32   ret = RT_ERR_OK;
    uint32  nhIdx;
    dal_mango_l3_nhEntry_t nhEntry;
    dal_mango_l3_intfEgrEntry_t intfEgrEntry;
    dal_mango_l2_index_t l2Index;
    dal_mango_l2_entry_t l2Entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pathId=%d", unit, pathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!(DAL_MANGO_L3_PATH_ID_IS_NH(pathId)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPathInfo), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    /* pathId to nhIdx */
    nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pathId);

    /* check validation */
    if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* for debugging purpose, we should get actual data from chip */
    RT_ERR_HDL(_dal_mango_l3_nhEntry_get(unit, nhIdx, &nhEntry, DAL_MANGO_L3_API_FLAG_NONE), errNhEntrySet, ret);

    /* L3 interface MAC address */
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_get(unit, nhEntry.l3_egr_intf_idx, &intfEgrEntry, DAL_MANGO_L3_API_FLAG_NONE), errIntfEntryGet, ret);

    /* L3 interface ID, DMAC index, DMAC address */
    pPathInfo->l3_intf_idx = nhEntry.l3_egr_intf_idx;
    pPathInfo->l3_intf_mac_addr = intfEgrEntry.smac_addr;

    /* normal nexthop entry check */
    if (TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
    {
        /* call L2 internal API to get MAC address (l2_idx to mac) */
        osal_memset(&l2Index, 0x00, sizeof(dal_mango_l2_index_t));
        //L2_MAC_IDX_TO_INDEX_STRUCT(nhEntry.dmac_idx, l2Index);
        L2_MAC_IDX_TO_INDEX_STRUCT(_pL3Db[unit]->nexthop[nhIdx].mac_addr_idx, l2Index);
        RT_ERR_HDL(_dal_mango_l2_getL2Entry(unit, &l2Index, &l2Entry), errL2EntryGet, ret);

        pPathInfo->nh_dmac_idx = nhEntry.dmac_idx;
        pPathInfo->nh_dmac_addr = (l2Entry.entry_type == L2_MULTICAST)? l2Entry.l2mcast.mac : l2Entry.unicast.mac;
        pPathInfo->nh_dmac_valid = TRUE;
    } else {
        pPathInfo->nh_dmac_valid = FALSE;
    }

errIntfEntryGet:
errL2EntryGet:
errNhEntrySet:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}

int32
_dal_mango_l3_pathInfo_get_byIdx(uint32 unit, uint32 nhDmacIdx, uint32 l3EgrIntfIdx, dal_mango_l3_pathInfo_t *pPathInfo)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_intfEgrEntry_t intfEgrEntry;
    dal_mango_l2_index_t l2Index;
    dal_mango_l2_entry_t l2Entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPathInfo), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    /* L3 interface MAC address */
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_get(unit, l3EgrIntfIdx, &intfEgrEntry, DAL_MANGO_L3_API_FLAG_NONE), errIntfEntryGet, ret);

    /* L3 interface ID, DMAC index, DMAC address */
    pPathInfo->l3_intf_idx = l3EgrIntfIdx;
    pPathInfo->l3_intf_mac_addr = intfEgrEntry.smac_addr;

    /* call L2 internal API to get MAC address (l2_idx to mac) */
    osal_memset(&l2Index, 0x00, sizeof(dal_mango_l2_index_t));
    L2_MAC_IDX_TO_INDEX_STRUCT(nhDmacIdx, l2Index);

    /* normal nexthop entry check */
    if (RT_ERR_OK == _dal_mango_l2_getL2Entry(unit, &l2Index, &l2Entry))
    {
        pPathInfo->nh_dmac_idx = nhDmacIdx;
        pPathInfo->nh_dmac_addr = (l2Entry.entry_type == L2_MULTICAST)? l2Entry.l2mcast.mac : l2Entry.unicast.mac;
        pPathInfo->nh_dmac_valid = TRUE;
    } else {
        osal_memset(&pPathInfo->nh_dmac_addr, 0x00, sizeof(rtk_mac_t));
        pPathInfo->nh_dmac_idx = 0;
        pPathInfo->nh_dmac_valid = FALSE;
    }

errIntfEntryGet:
    L3_SEM_UNLOCK(unit);

    return ret;
}



/* internal APIs, called by dal_mango_l3_* APIs (should NOT lock/unlock semaphore inside) */
static inline int32 __dal_mango_l3_hostEntry_find(uint32 unit, rtk_l3_host_t *pHost, dal_mango_l3_hostAlloc_t *pHostAlloc, uint32 *pHostIdx, dal_mango_l3_hostEntry_t *pHostEntry)
{
    int32 ret = RT_ERR_OK;
    dal_mango_l3_hostHashKey_t  hashKey;
    dal_mango_l3_hostAlloc_t    hostAlloc;
    dal_mango_l3_hostEntry_t    hostEntry;
    uint32  ipv6, flags;
    uint32  tbl, slot, addr;
    uint32  hostIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pHost=%p,pHostIdx=%p", unit, pHost, pHostIdx);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHostIdx), RT_ERR_NULL_POINTER);

    /* get hash index */
    osal_memset(&hashKey, 0x00, sizeof(dal_mango_l3_hostHashKey_t));
    osal_memset(&hostAlloc, 0x00, sizeof(dal_mango_l3_hostAlloc_t));
    L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_UC_HASH_ALG_SEL_0f, hashKey.alg_of_tbl[0], "", errHandle, ret);
    L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_UC_HASH_ALG_SEL_1f, hashKey.alg_of_tbl[1], "", errHandle, ret);
    hashKey.vrf_id = pHost->vrf_id;
    if (pHost->flags & RTK_L3_FLAG_IPV6)
    {
        ipv6 = TRUE;
        hashKey.dip.ipv6 = pHost->ip_addr.ipv6;
        flags = DAL_MANGO_L3_API_FLAG_IPV6;
        hostAlloc.width = 3;
    }
    else
    {
        ipv6 = FALSE;
        hashKey.dip.ipv4 = pHost->ip_addr.ipv4;
        flags = DAL_MANGO_L3_API_FLAG_NONE;
        hostAlloc.width = 1;
    }
    RT_ERR_HDL(_dal_mango_l3_hostHashIdx_get(unit, &hashKey, &hostAlloc.hashIdx, flags), errHandle, ret);
    if (NULL != pHostAlloc)
    {
        *pHostAlloc = hostAlloc;
    }

    /* search the entries which have the same hash-index */
    for (tbl=0; tbl<DAL_MANGO_L3_HOST_TBL_NUM; tbl++)
    {
        for (slot=0; slot<DAL_MANGO_L3_HOST_TBL_WIDTH; slot+=hostAlloc.width)
        {
            addr = ((tbl & 0x1) << 13) | ((hostAlloc.hashIdx.idx_of_tbl[tbl] & 0x3FF) << 3) | ((slot & 0x7) << 0);
            hostIdx = DAL_MANGO_L3_ENTRY_ADDR_TO_IDX(addr);

            /* ignore the entry until the hostIdx >= input-index */
            if (hostIdx < *pHostIdx)
                continue;

            if (_pL3Db[unit]->host[hostIdx].valid)
            {
                RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, hostIdx, &hostEntry, flags), errHandle, ret);

                if ((pHost->vrf_id == hostEntry.vrf_id) && \
                    (((FALSE == ipv6) && \
                      (MANGO_L3_HOST_IP_PFLEN == rt_util_ipMaxMatchLength_ret(hostEntry.ip, pHost->ip_addr.ipv4, MANGO_L3_HOST_IP_PFLEN))) || \
                     ((TRUE == ipv6) && \
                      (MANGO_L3_HOST_IP6_PFLEN == rt_util_ipv6MaxMatchLength_ret(&hostEntry.ip6, &pHost->ip_addr.ipv6, MANGO_L3_HOST_IP6_PFLEN)))))
                {
                    /* found the entry, update hostIdx */
                    *pHostIdx = hostIdx;

                    if (NULL != pHostEntry)
                    {
                        *pHostEntry = hostEntry;
                    }

                    goto errOk;
                }
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    return ret;
}

/* Function Name:
 *      _dal_mango_l3_reset
 * Description:
 *      Reset L3 module of the specified device.
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
static int32
_dal_mango_l3_reset(uint32 unit)
{
    int32 ret = RT_ERR_OK;
    uint32 hostIdx;
    uint32 routeIdx, length;
    uint32 ecmpIdx;
    uint32 idx;
    uint32 nhIdx;
    uint32 macIdx;
    uint32 intfId, intfIdx;
    dal_mango_l2_ucastNhAddr_t  l2ucEntry;
    dal_mango_l2_mcastNhAddr_t  l2mcEntry;
    dal_mango_l3_hostEntry_t    hostEntry;
    dal_mango_l3_ecmpEntry_t    ecmpEntry;
    dal_mango_l3_macEntry_t     macEntry;

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == _pL3Db[unit]), RT_ERR_NULL_POINTER);

    /* check if needs to remove the existing entries */
    if (INIT_COMPLETED == l3_init[unit])
    {
        /* scan the whole L3 host table */
        osal_memset(&hostEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
        for (hostIdx=0; hostIdx<DAL_MANGO_L3_HOST_TBL_SIZE; hostIdx++)
        {
            if (_pL3Db[unit]->host[hostIdx].valid)
            {
                /* write into chip */
                RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                /* release L3 host entry */
                RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                /* update L3 DB */
                L3_DB_UPDATE_HOST_INVALID(unit, hostIdx);

                MANGO_L3_DBG_PRINTF(3, "hostIdx = %u\n", hostIdx);
            }
        }

        /* scan the whole L3 route table */
        /* IPv6 */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        routeIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#else
        routeIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#endif
        length = 3 * L3_ROUTE_TBL_IP6_CNT(unit);
        if (length > 0)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, routeIdx, length), errHandle, ret);

            /* clear DB */
            for (routeIdx = 0; routeIdx < MANGO_L3_HOST_IP6_PFLEN; routeIdx++)
            {
                L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, routeIdx) = 0;
            }
            L3_ROUTE_TBL_IP6_CNT(unit) = 0;
        }
        /* IPv4 */
        routeIdx = 0;
        length = L3_ROUTE_TBL_IP_CNT(unit);

        if (length > 0)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, routeIdx, length), errHandle, ret);

            /* clear DB */
            for (routeIdx = 0; routeIdx < MANGO_L3_HOST_IP_PFLEN; routeIdx++)
            {
                L3_ROUTE_TBL_IP_CNT_PFLEN(unit, routeIdx) = 0;
            }
            L3_ROUTE_TBL_IP_CNT(unit) = 0;
        }

        /* clear ECMP table */
        osal_memset(&ecmpEntry, 0x00, sizeof(dal_mango_l3_ecmpEntry_t));
        for (ecmpIdx=0; ecmpIdx<DAL_MANGO_L3_ECMP_MAX; ecmpIdx++)
        {
            if (_pL3Db[unit]->ecmp[ecmpIdx].valid)
            {
                /* directly clear ref-count */
                L3_DB_ECMP_REFCNT(unit, ecmpIdx) = 0;

                RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
                for (idx=0; idx<(_pL3Db[unit]->ecmp[ecmpIdx].nh_count); idx++)
                {
                    nhIdx = ecmpEntry.nh_idx[idx];

                    /* typical nexthop entry */
                    if (L3_DB_NH_IS_VALID(unit, nhIdx))
                    {
                        L3_DB_UPDATE_NH_REFCNT_DEC(unit, nhIdx);
                    }
                    MANGO_L3_DBG_PRINTF(3, "ecmpEntry.nh_idx[%u] = %u (nhIdx)\n", idx, nhIdx);
                }

                MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->ecmp[ecmpIdx].nh_count = %u)\n", _pL3Db[unit]->ecmp[ecmpIdx].nh_count);

                RT_ERR_HDL(_dal_mango_l3_ecmpEntry_set(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                RT_ERR_HDL(_dal_mango_l3_ecmpEntry_free(unit, ecmpIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                L3_DB_UPDATE_ECMP_INVALID(unit, ecmpIdx);
            }
        }

        /* clear nexthop table */
        osal_memset(&l2mcEntry, 0x00, sizeof(dal_mango_l2_mcastNhAddr_t));
        osal_memset(&l2ucEntry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
        for (nhIdx=1; nhIdx<DAL_MANGO_L3_NEXTHOP_MAX; nhIdx++)
        {
            if (_pL3Db[unit]->nexthop[nhIdx].valid)
            {
                /* directly clear ref-count */
                L3_DB_NH_REFCNT(unit, nhIdx) = 0;

                intfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;

                /* check reference count */
                if ((_pL3Db[unit]->refer_cnt_chk_en) &&
                    (L3_DB_NH_REFCNT(unit, nhIdx) > 0))
                {
                    MANGO_L3_DBG_PRINTF(3, "FATAL: nhIdx = %u, ref_count = %u (not zero)\n", nhIdx, L3_DB_NH_REFCNT(unit, nhIdx));
                    ret = RT_ERR_ENTRY_REFERRED;
                    goto errHandle;
                }

                /* check if MAC has been allocated */
                if (TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
                {
                    /* prepare L2 entry for deleteing */
                    if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
                    {
                        RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                                   &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2mcEntry.vid), errHandle, ret);
                        l2mcEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
                        L3_RT_ERR_HDL_DBG(_dal_mango_l2_mcastNexthop_del(unit, &l2mcEntry), "");
                    }
                    else
                    {
                        RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                                   &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2ucEntry.fid), errHandle, ret);
                        l2ucEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
                        L3_RT_ERR_HDL_DBG(_dal_mango_l2_nexthop_del(unit, &l2ucEntry), "");
                    }
                }

                /* update DB */
                L3_DB_UPDATE_INTF_REFCNT_DEC(unit, intfIdx);
                osal_memset(&_pL3Db[unit]->nexthop[nhIdx].mac_addr, 0x00, sizeof(rtk_mac_t));
                _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = FALSE;
                _pL3Db[unit]->nexthop[nhIdx].intf_idx = 0;
                L3_DB_UPDATE_NH_INVALID(unit, nhIdx);

                RT_ERR_HDL(_dal_mango_l3_nhEntry_free(unit, nhIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
            }
        }

        /* clear MAC/interface entry */
        osal_memset(&macEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));
        for (macIdx=0; macIdx<DAL_MANGO_L3_INTF_MAX; macIdx++)
        {
            /* interface (DAL_MANGO_L3_RESERVED_INTF_IDX) which is reserved for default L2 interface and cannot be deleted */
            if (DAL_MANGO_L3_RESERVED_INTF_IDX == macIdx)
                continue;

            if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, macIdx))
            {
                intfId = _pL3Db[unit]->intf[macIdx].intf_id;
                intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);

                /* only remove `L3` interface (excluding `tunnel` interface) */
                if (DAL_MANGO_L3_INTF_ID_IS_L3(intfId))
                {
                    if (_pL3Db[unit]->intf[intfIdx].vid != DAL_MANGO_L3_RESERVED_INTF_VID)
                    {
                        //RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_set(unit, _pL3Db[unit]->intf[intfIdx].vid, DAL_MANGO_L3_RESERVED_INTF_IDX), errHandle, ret);
                        RT_ERR_HDL(_dal_mango_l3_vlanIntf_remove(unit, _pL3Db[unit]->intf[intfIdx].vid, intfIdx), errHandle, ret);
                    }
                    RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, intfIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errHandle, ret);
                    RT_ERR_HDL(_dal_mango_l3_intfEntry_free(unit, intfIdx, (DAL_MANGO_L3_API_FLAG_NONE)), errHandle, ret);
                    RT_ERR_HDL(_dal_mango_l3_mtuEntry_free(unit, _pL3Db[unit]->intf[intfIdx].ip_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errHandle, ret);
                    RT_ERR_HDL(_dal_mango_l3_mtuIp6Entry_free(unit, _pL3Db[unit]->intf[intfIdx].ip6_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errHandle, ret);

                    L3_DB_UPDATE_INTF_INVALID(unit, intfIdx);
                }
            }

            /* VRRP (MAC) entry */
            if (BITMAP_IS_SET(_pL3Db[unit]->HW.mac_used, macIdx))
            {
                RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, macIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errHandle, ret);

                if (BITMAP_IS_SET(_pL3Db[unit]->HW.mac_used, macIdx))
                {
                     _pL3Db[unit]->HW.mac_used_count -= 1;
                    BITMAP_CLEAR(_pL3Db[unit]->HW.mac_used, macIdx);
                }
            }
        }
    }

    return RT_ERR_OK;

errHandle:
    return ret;
}

/* Module Name    : Layer3 routing                */
/* Sub-module Name: Layer3 routing error handling */

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
int32
dal_mango_l3_info_get(uint32 unit, rtk_l3_info_t *pInfo)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  vrf_used_count = 0;
    uint32  vrf_ref_count[DAL_MANGO_L3_VRF_MAX];
#if MANGO_L3_ROUTE_IPMC_SIZE
    uint32  ipmc_lpm_ipv4_cnt;
    uint32  ipmc_lpm_ipv6_cnt;
#endif

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    /* scan all interfaces to calculate VRF usage */
    osal_memset(vrf_ref_count, 0x00, sizeof(vrf_ref_count));
    for (idx=0; idx<DAL_MANGO_L3_INTF_MAX; idx++)
    {
        if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, idx))
        {
            if (vrf_ref_count[(_pL3Db[unit]->intf[idx].vrf_id % DAL_MANGO_L3_VRF_MAX)] == 0)
            {
                vrf_used_count += 1;
            }
            vrf_ref_count[(_pL3Db[unit]->intf[idx].vrf_id % DAL_MANGO_L3_VRF_MAX)] += 1;
        }
    }

    pInfo->mac_max = HAL_MAX_NUM_OF_ROUTER_MAC(unit);
    pInfo->mac_used = _pL3Db[unit]->HW.mac_used_count;
    pInfo->vrf_max = HAL_MAX_NUM_OF_VRF(unit);
    pInfo->vrf_used = vrf_used_count;
    pInfo->intf_max = HAL_MAX_NUM_OF_INTF(unit);
    pInfo->intf_used = _pL3Db[unit]->HW.intf_used_count;
    pInfo->intf_mtu_max = HAL_MAX_NUM_OF_INTF_MTU(unit);
    pInfo->intf_mtu_used = _pL3Db[unit]->HW.ip_mtu_used_count;
    pInfo->intf_ipv6_mtu_max = HAL_MAX_NUM_OF_INTF_MTU(unit);
    pInfo->intf_ipv6_mtu_used = _pL3Db[unit]->HW.ip6_mtu_used_count;
    pInfo->ecmp_max = HAL_MAX_NUM_OF_L3_ECMP(unit);
    pInfo->ecmp_used = _pL3Db[unit]->HW.ecmp_used_count;
    pInfo->nexthop_max = HAL_MAX_NUM_OF_L3_NEXTHOP(unit);
    pInfo->nexthop_used = _pL3Db[unit]->HW.nexthop_used_count;
    pInfo->host_ipv4_max = HAL_MAX_NUM_OF_L3_HOST(unit);
    pInfo->host_ipv4_used = _pL3Db[unit]->HW.host_used_count;
#if MANGO_L3_ROUTE_IPMC_SIZE
    pInfo->route_ipv4_max = L3_ROUTE_TBL_SIZE(unit) + MANGO_L3_ROUTE_IPMC_SIZE;
#else
    pInfo->route_ipv4_max = L3_ROUTE_TBL_SIZE(unit);
#endif
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    pInfo->route_ipv4_used = L3_ROUTE_TBL_USED_INC_MC(unit);
#else
    pInfo->route_ipv4_used = L3_ROUTE_TBL_USED(unit);
#endif

#if MANGO_L3_ROUTE_IPMC_SIZE
    /* get IPMC LPM entry count and update route_ipv4_used */
    if (RT_ERR_OK == _dal_mango_ipmc_lpmRouteCnt_get(unit, &ipmc_lpm_ipv4_cnt, &ipmc_lpm_ipv6_cnt))
    {
        pInfo->route_ipv4_used += (2*ipmc_lpm_ipv4_cnt) + (6*ipmc_lpm_ipv6_cnt);
    }
#endif

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IPUC_MAX(unit)  = %u\n", L3_ROUTE_TBL_IPUC_MAX(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_BLANK_MAX(unit) = %u\n", L3_ROUTE_TBL_BLANK_MAX(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IP6UC_MAX(unit) = %u\n", L3_ROUTE_TBL_IP6UC_MAX(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IPMC_MAX(unit)  = %u\n", L3_ROUTE_TBL_IPMC_MAX(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IP6MC_MAX(unit) = %u\n", L3_ROUTE_TBL_IP6MC_MAX(unit));

    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IPUC_LEFT(unit)  = %u\n", L3_ROUTE_TBL_IPUC_LEFT(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_BLANK_LEFT(unit) = %u\n", L3_ROUTE_TBL_BLANK_LEFT(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IP6UC_LEFT(unit) = %u\n", L3_ROUTE_TBL_IP6UC_LEFT(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IPMC_LEFT(unit)  = %u\n", L3_ROUTE_TBL_IPMC_LEFT(unit));
    MANGO_L3_DBG_PRINTF(3, "L3_ROUTE_TBL_IP6MC_LEFT(unit) = %u\n", L3_ROUTE_TBL_IP6MC_LEFT(unit));
#endif

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_info_get */

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
int32
dal_mango_l3_routerMacEntry_get(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry)
{
    int32   ret;
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTER_MAC(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_l3_macEntry_get(unit, index, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntryGet, ret);
    RT_ERR_HDL(l3_util_macEntry2rtkRouterMac(pEntry, &macEntry), errMacEntryConvert, ret);

errMacEntryGet:
errMacEntryConvert:
    L3_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_l3_routerMacEntry_get */

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
int32
dal_mango_l3_routerMacEntry_set(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry)
{
    int32   ret;
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTER_MAC(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(l3_util_rtkRouterMac2macEntry(&macEntry, pEntry), errMacEntryConvert, ret);
    RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, index, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);

errMacEntryConvert:
errMacEntrySet:

    L3_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_l3_routerMacEntry_set */

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
int32
dal_mango_l3_intf_create(uint32 unit, rtk_l3_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    uint32  mtuIdx, mtuIp6Idx;
    uint32  intfIdx;
    int32   macIdx = -1;
    dal_mango_l3_intfEntry_t intfEntry;
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pIntf->vid < RTK_VLAN_ID_MIN) || (pIntf->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pIntf->mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->ipv6_mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pIntf->ttl > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    if (RT_ERR_OK != _dal_mango_vlan_table_check(unit, pIntf->vid))
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    /* function body */
    L3_SEM_LOCK(unit);

    /* check resource */
    if (_pL3Db[unit]->HW.intf_used_count >= HAL_MAX_NUM_OF_INTF(unit))
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }

    /* allocate resources  */
    if (pIntf->ipv6_mtu == 0) pIntf->ipv6_mtu = pIntf->mtu; /* same as IPv4 if it's unset */
    RT_ERR_HDL(_dal_mango_l3_mtuEntry_alloc(unit, pIntf->mtu, &mtuIdx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuAlloc, ret);
    RT_ERR_HDL(_dal_mango_l3_mtuIp6Entry_alloc(unit, pIntf->ipv6_mtu, &mtuIp6Idx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuIp6Alloc, ret);
    if (pIntf->flags & RTK_L3_INTF_FLAG_WITH_ID)
    {
        intfIdx = pIntf->intf_id;
        RT_ERR_HDL(_dal_mango_l3_intfEntry_alloc(unit, &intfIdx, (DAL_MANGO_L3_API_FLAG_WITH_ID)), errIntfAlloc, ret);
    } else {
        RT_ERR_HDL(_dal_mango_l3_intfEntry_alloc(unit, &intfIdx, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfAlloc, ret);
    }

    L3_DB_UPDATE_INTF_VALID(unit, intfIdx, mtuIdx, mtuIp6Idx);
    L3_DB_UPDATE_INTF_REFCNT_RESET(unit, intfIdx);

    /* build interface entry */
    osal_memset(&intfEntry, 0x00, sizeof(dal_mango_l3_intfEntry_t));
    intfEntry.igrIntf.vrf_id            = pIntf->vrf_id;
    intfEntry.egrIntf.dst_vid           = pIntf->vid;
    intfEntry.egrIntf.smac_addr         = pIntf->mac_addr;
    intfEntry.egrIntf.ipmc_ttl_scope    = pIntf->ttl;
    intfEntry.egrIntf.ip6mc_hl_scope    = pIntf->ttl;
    intfEntry.egrIntf.ip_mtu_idx        = mtuIdx;
    intfEntry.egrIntf.ip6_mtu_idx       = mtuIp6Idx;
    RT_ERR_HDL(_dal_mango_l3_intfIgrEntry_set(unit, intfIdx, &intfEntry.igrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfSet, ret);
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_set(unit, intfIdx, &intfEntry.egrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfSet, ret);

    if (pIntf->flags & RTK_L3_INTF_FLAG_MAC_MANUAL)
    {
        L3_DB_UPDATE_INTF_MAC_INDEX(unit, intfIdx, 0, 0);
    }
    else
    {
        /* find an empty macEntry */
        for (macIdx=1; macIdx<DAL_MANGO_L3_MAC_MAX; macIdx++)
        {
            if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.mac_used, macIdx))
            {
                /* build MAC entry */
                osal_memset(&macEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));
                macEntry.valid      = 1;
                macEntry.intf_id    = intfIdx;
                macEntry.mac        = pIntf->mac_addr;
                macEntry.bmsk_intf_id = 0x3FF;
                macEntry.act        = RTK_L3_ACT_FORWARD;
                osal_memset(&macEntry.bmsk_mac, 0xFF, sizeof(rtk_mac_t));
                RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, macIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);

                BITMAP_SET(_pL3Db[unit]->HW.mac_used, macIdx);
                _pL3Db[unit]->HW.mac_used_count += 1;
                L3_DB_UPDATE_INTF_MAC_INDEX(unit, intfIdx, 1, macIdx);  /* update L3 DB */

                break;
            }
        }

        if (macIdx >= DAL_MANGO_L3_MAC_MAX)
        {
            ret = RT_ERR_EXCEEDS_CAPACITY;
            goto errExceedsCapacity;
        }
    }

    if (pIntf->vid != DAL_MANGO_L3_RESERVED_INTF_VID)
    {
        //RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_set(unit, pIntf->vid, intfIdx), errVlanL3IntfIdxSet, ret);
        RT_ERR_HDL(_dal_mango_l3_vlanIntf_insert(unit, pIntf->vid, intfIdx), errVlanL3IntfIdxSet, ret);
    }

    /* update interface ID before returning */
    pIntf->intf_id = intfIdx;

    goto errOk;

errVlanL3IntfIdxSet:
    /* release HW mac entry */
    if (L3_DB_INTF_MAC_INDEX_VALID(unit, intfIdx))
    {
        if (BITMAP_IS_SET(_pL3Db[unit]->HW.mac_used, L3_DB_INTF_MAC_INDEX(unit, intfIdx)))
        {
            _pL3Db[unit]->HW.mac_used_count -= 1;
            BITMAP_CLEAR(_pL3Db[unit]->HW.mac_used, L3_DB_INTF_MAC_INDEX(unit, intfIdx));
        }

        L3_DB_UPDATE_INTF_MAC_INDEX(unit, intfIdx, 0, 0);
    }

errExceedsCapacity:
errMacEntrySet:
errIntfSet:
    L3_RT_ERR_HDL_DBG(_dal_mango_l3_intfEntry_free(unit, intfIdx, (DAL_MANGO_L3_API_FLAG_NONE)), "");

errIntfAlloc:
    L3_RT_ERR_HDL_DBG(_dal_mango_l3_mtuIp6Entry_free(unit, mtuIp6Idx, (DAL_MANGO_L3_API_FLAG_NONE)), "");

errMtuIp6Alloc:
    L3_RT_ERR_HDL_DBG(_dal_mango_l3_mtuEntry_free(unit, mtuIdx, (DAL_MANGO_L3_API_FLAG_NONE)), "");

errMtuAlloc:

errTblFull:

errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intf_create */


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
int32
dal_mango_l3_intf_destroy(uint32 unit, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((intfIdx == DAL_MANGO_L3_RESERVED_INTF_IDX), RT_ERR_INPUT);
    RT_PARAM_CHK((intfIdx > DAL_MANGO_L3_INTF_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(L3_INTF_IDX_IS_L2_TUNNEL(unit, intfIdx), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
    L3_SEM_LOCK(unit);

    if ( ((FALSE == _pL3Db[unit]->intf[intfIdx].valid) && (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[unit]->intf[intfIdx].intf_id))) || (DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(intfId)))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    if ((_pL3Db[unit]->refer_cnt_chk_en) &&
        (L3_DB_INTF_REFCNT(unit, intfIdx) > 0))
    {
        ret = RT_ERR_ENTRY_REFERRED;
        goto errEntryReferred;
    }

    if (_pL3Db[unit]->intf[intfIdx].vid != DAL_MANGO_L3_RESERVED_INTF_VID)
    {
        //RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_set(unit, _pL3Db[unit]->intf[intfIdx].vid, DAL_MANGO_L3_RESERVED_INTF_IDX), errVlanL3IntfIdxSet, ret);
        RT_ERR_HDL(_dal_mango_l3_vlanIntf_remove(unit, _pL3Db[unit]->intf[intfIdx].vid, intfIdx), errVlanL3IntfIdxSet, ret);
    }

    if (L3_DB_INTF_MAC_INDEX_VALID(unit, intfIdx))
    {
        /* clear MAC entry */
        osal_memset(&macEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));
        RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, L3_DB_INTF_MAC_INDEX(unit, intfIdx), &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);

        _pL3Db[unit]->HW.mac_used_count -= 1;
        BITMAP_CLEAR(_pL3Db[unit]->HW.mac_used, L3_DB_INTF_MAC_INDEX(unit, intfIdx));
        L3_DB_UPDATE_INTF_MAC_INDEX(unit, intfIdx, 0, 0);
    }

    /* release interface, MTU entry */
    RT_ERR_HDL(_dal_mango_l3_intfEntry_free(unit, intfIdx, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfFree, ret);
    RT_ERR_HDL(_dal_mango_l3_mtuIp6Entry_free(unit, _pL3Db[unit]->intf[intfIdx].ip6_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuIp6Free, ret);
    RT_ERR_HDL(_dal_mango_l3_mtuEntry_free(unit, _pL3Db[unit]->intf[intfIdx].ip_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuFree, ret);

    L3_DB_UPDATE_INTF_INVALID(unit, intfIdx);

    MANGO_L3_DBG_PRINTF(3, "unit = %u, intfIdx = %u, _pL3Db[unit]->intf[intfIdx].ip_mtu_idx = %u\n", \
        unit, intfIdx, _pL3Db[unit]->intf[intfIdx].ip_mtu_idx);
    MANGO_L3_DBG_PRINTF(3, "unit = %u, intfIdx = %u, _pL3Db[unit]->intf[intfIdx].ip6_mtu_idx = %u\n", \
        unit, intfIdx, _pL3Db[unit]->intf[intfIdx].ip6_mtu_idx);
    MANGO_L3_DBG_PRINTF(3, "unit = %u, intfId = 0x%08x\n", unit, intfId);

errMtuFree:
errMtuIp6Free:
errIntfFree:
errMacEntrySet:
errVlanL3IntfIdxSet:
errEntryReferred:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intf_destroy */


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
int32
dal_mango_l3_intf_destroyAll(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  intfIdx;
    rtk_intf_id_t intfId;
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    /* clear MAC entry */
    osal_memset(&macEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));

    for (idx=0; idx<DAL_MANGO_L3_INTF_MAX; idx++)
    {
        /* interface (DAL_MANGO_L3_RESERVED_INTF_IDX) which is reserved for default L2 interface and cannot be deleted */
        if (DAL_MANGO_L3_RESERVED_INTF_IDX == idx)
            continue;

        if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, idx))
        {
            intfId = _pL3Db[unit]->intf[idx].intf_id;
            intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);

            if (DAL_MANGO_L3_INTF_ID_IS_L3(intfId))
            {
                if (_pL3Db[unit]->intf[intfIdx].vid != DAL_MANGO_L3_RESERVED_INTF_VID)
                {
                    //RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_set(unit, _pL3Db[unit]->intf[intfIdx].vid, DAL_MANGO_L3_RESERVED_INTF_IDX), errVlanL3IntfIdxSet, ret);
                    RT_ERR_HDL(_dal_mango_l3_vlanIntf_remove(unit, _pL3Db[unit]->intf[intfIdx].vid, intfIdx), errVlanL3IntfIdxSet, ret);
                }
                if (L3_DB_INTF_MAC_INDEX_VALID(unit, intfIdx))
                {
                    RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, intfIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);
                    if (BITMAP_IS_SET(_pL3Db[unit]->HW.mac_used, L3_DB_INTF_MAC_INDEX(unit, intfIdx)))
                    {
                         _pL3Db[unit]->HW.mac_used_count -= 1;
                        BITMAP_CLEAR(_pL3Db[unit]->HW.mac_used, L3_DB_INTF_MAC_INDEX(unit, intfIdx));
                    }
                    L3_DB_UPDATE_INTF_MAC_INDEX(unit, intfIdx, 0, 0);
                }
                RT_ERR_HDL(_dal_mango_l3_intfEntry_free(unit, intfIdx, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfFree, ret);
                RT_ERR_HDL(_dal_mango_l3_mtuIp6Entry_free(unit, _pL3Db[unit]->intf[intfIdx].ip6_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuIp6Free, ret);
                RT_ERR_HDL(_dal_mango_l3_mtuEntry_free(unit, _pL3Db[unit]->intf[intfIdx].ip_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuFree, ret);

                L3_DB_UPDATE_INTF_INVALID(unit, intfIdx);
            }
        }
    }

errMtuFree:
errMtuIp6Free:
errIntfFree:
errMacEntrySet:
errVlanL3IntfIdxSet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intf_destroyAll */


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
int32
dal_mango_l3_intf_get(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    rtk_intf_id_t intfId;
    dal_mango_l3_intfEntry_t intfEntry;
    uint32  idx, intfIdx;
    uint32  ip_mtu, ip6_mtu;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    if (RTK_L3_INTFKEYTYPE_INTF_ID == type)
    {
        RT_PARAM_CHK(0 == DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pIntf->intf_id), RT_ERR_ENTRY_NOTFOUND);
        RT_PARAM_CHK(DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pIntf->intf_id) >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(L3_INTF_IDX_IS_L2_TUNNEL(unit, DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pIntf->intf_id)), RT_ERR_ENTRY_NOTFOUND);
    }

    /* function body */
    L3_SEM_LOCK(unit);

    if (RTK_L3_INTFKEYTYPE_INTF_ID == type)
    {
        intfId = pIntf->intf_id;
    }
    else if ((RTK_L3_INTFKEYTYPE_VID == type) || (RTK_L3_INTFKEYTYPE_MAC_AND_VID == type))
    {
        for (idx=0; idx<DAL_MANGO_L3_INTF_MAX; idx++)
        {
            /* interface (DAL_MANGO_L3_RESERVED_INTF_IDX) which is reserved for default L2 interface and cannot be deleted */
            if (DAL_MANGO_L3_RESERVED_INTF_IDX == idx)
                continue;

            if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, idx))
            {
                intfId = _pL3Db[unit]->intf[idx].intf_id;

                /* check VID */
                if (pIntf->vid == _pL3Db[unit]->intf[idx].vid)
                {
                    intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
                    RT_ERR_HDL(_dal_mango_l3_intfIgrEntry_get(unit, intfIdx, &intfEntry.igrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfGet, ret);
                    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_get(unit, intfIdx, &intfEntry.egrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfGet, ret);
                    /* need to check MAC address in advance ? */
                    if ((RTK_L3_INTFKEYTYPE_MAC_AND_VID == type) && (0 != rt_util_macCmp(pIntf->mac_addr.octet, intfEntry.egrIntf.smac_addr.octet)))
                    {
                        continue;
                    }

                    break;
                }
            }
        }
        if (idx >= DAL_MANGO_L3_INTF_MAX)
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }
    }
    else
    {
        ret = RT_ERR_INPUT;
        goto errType;
    }

    intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    RT_ERR_HDL(_dal_mango_l3_intfIgrEntry_get(unit, intfIdx, &intfEntry.igrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfGet, ret);
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_get(unit, intfIdx, &intfEntry.egrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfGet, ret);
    /* need to check MAC address in advance ? */
    if ((RTK_L3_INTFKEYTYPE_MAC_AND_VID == type) && (0 != rt_util_macCmp(pIntf->mac_addr.octet, intfEntry.egrIntf.smac_addr.octet)))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_L3_INTF_IP_MTUr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip_mtu_idx, \
        MANGO_MTUf, ip_mtu, "", errIpMtuGet, ret);
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_L3_INTF_IP6_MTUr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip6_mtu_idx, \
        MANGO_MTUf, ip6_mtu, "", errIpMtuGet, ret);

    /* update interface ID before returning */
    pIntf->intf_id  = intfIdx;

    pIntf->vrf_id   = intfEntry.igrIntf.vrf_id;
    pIntf->vid      = intfEntry.egrIntf.dst_vid;
    pIntf->mac_addr = intfEntry.egrIntf.smac_addr;
    pIntf->mtu      = ip_mtu;
    pIntf->ipv6_mtu = ip6_mtu;
    pIntf->ttl      = intfEntry.egrIntf.ipmc_ttl_scope;

errEntryNotFound:
errIpMtuGet:
errIntfGet:
errType:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intf_get */


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
 *      RT_ERR_MTU_EXCEED               - interface MTU is too big (over the maximum)
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND     - vlan entry not found
 * Note:
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 */
int32
dal_mango_l3_intf_set(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    rtk_intf_id_t intfId;
    dal_mango_l3_intfEntry_t intfEntry;
    dal_mango_l3_macEntry_t macEntry;
    uint32  idx, intfIdx;
    uint32  ip_mtu, ip6_mtu;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pIntf->intf_id) >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(0 == DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pIntf->intf_id), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK(L3_INTF_IDX_IS_L2_TUNNEL(unit, DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pIntf->intf_id)), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pIntf->vid < RTK_VLAN_ID_MIN) || (pIntf->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pIntf->mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->ipv6_mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pIntf->ttl > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    if (RT_ERR_OK != _dal_mango_vlan_table_check(unit, pIntf->vid))
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    /* function body */
    L3_SEM_LOCK(unit);

    if (RTK_L3_INTFKEYTYPE_INTF_ID == type)
    {
        intfId = pIntf->intf_id;
    }
    else if ((RTK_L3_INTFKEYTYPE_VID == type) || (RTK_L3_INTFKEYTYPE_MAC_AND_VID == type))
    {
        for (idx=0; idx<DAL_MANGO_L3_INTF_MAX; idx++)
        {
            /* interface (DAL_MANGO_L3_RESERVED_INTF_IDX) which is reserved for default L2 interface and cannot be deleted */
            if (DAL_MANGO_L3_RESERVED_INTF_IDX == idx)
                continue;

            if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, idx))
            {
                intfId = _pL3Db[unit]->intf[idx].intf_id;

                /* check VID */
                if (pIntf->vid == _pL3Db[unit]->intf[idx].vid)
                {
                    break;
                }
            }
        }
        if (idx >= DAL_MANGO_L3_INTF_MAX)
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }
    }
    else
    {
        ret = RT_ERR_INPUT;
        goto errType;
    }

    /* check for each change */
    intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    RT_ERR_HDL(_dal_mango_l3_intfIgrEntry_get(unit, intfIdx, &intfEntry.igrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfGet, ret);
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_get(unit, intfIdx, &intfEntry.egrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfGet, ret);
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_L3_INTF_IP_MTUr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip_mtu_idx, \
        MANGO_MTUf, ip_mtu, "", errIpMtuGet, ret);
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_L3_INTF_IP6_MTUr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip6_mtu_idx, \
        MANGO_MTUf, ip6_mtu, "", errIpMtuGet, ret);

    if (pIntf->vid != _pL3Db[unit]->intf[intfIdx].vid)
    {
        if (_pL3Db[unit]->intf[intfIdx].vid != DAL_MANGO_L3_RESERVED_INTF_VID)
        {
            RT_ERR_HDL(_dal_mango_l3_vlanIntf_remove(unit, _pL3Db[unit]->intf[intfIdx].vid, intfIdx), errVlanL3IntfIdxSet, ret);
        }

        if (pIntf->vid != DAL_MANGO_L3_RESERVED_INTF_VID)
        {
            RT_ERR_HDL(_dal_mango_l3_vlanIntf_insert(unit, pIntf->vid, intfIdx), errVlanL3IntfIdxSet, ret);
        }
    }

    if ((L3_DB_INTF_MAC_INDEX_VALID(unit, intfIdx)) &&
        (0 != rt_util_macCmp(intfEntry.egrIntf.smac_addr.octet, pIntf->mac_addr.octet)))
    {
        /* build MAC entry */
        osal_memset(&macEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));
        macEntry.valid      = 1;
        macEntry.intf_id    = intfIdx;
        macEntry.mac        = pIntf->mac_addr;
        macEntry.bmsk_intf_id = 0x3FF;
        macEntry.act        = RTK_L3_ACT_FORWARD;
        osal_memset(&macEntry.bmsk_mac, 0xFF, sizeof(rtk_mac_t));
        RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, L3_DB_INTF_MAC_INDEX(unit, intfIdx), &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);
    }

    intfEntry.igrIntf.vrf_id    = pIntf->vrf_id;
    intfEntry.egrIntf.dst_vid   = pIntf->vid;
    intfEntry.egrIntf.smac_addr = pIntf->mac_addr;
    MANGO_L3_DBG_PRINTF(3, "pIntf->mtu = %u, ip_mtu = %u, intfEntry.egrIntf.ip_mtu_idx = %u\n", pIntf->mtu, ip_mtu, intfEntry.egrIntf.ip_mtu_idx);
    MANGO_L3_DBG_PRINTF(3, "pIntf->ipv6_mtu = %u, ip6_mtu = %u, intfEntry.egrIntf.ip6_mtu_idx = %u\n", pIntf->ipv6_mtu, ip6_mtu, intfEntry.egrIntf.ip6_mtu_idx);
    if (pIntf->mtu != ip_mtu)
    {
        RT_ERR_HDL(_dal_mango_l3_mtuEntry_realloc(unit, pIntf->mtu, &intfEntry.egrIntf.ip_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuRealloc, ret);
    }
    if (pIntf->ipv6_mtu == 0) pIntf->ipv6_mtu = pIntf->mtu; /* sync with intf mtu */
    if (pIntf->ipv6_mtu != ip6_mtu)
    {
        RT_ERR_HDL(_dal_mango_l3_mtuIp6Entry_realloc(unit, pIntf->ipv6_mtu, &intfEntry.egrIntf.ip6_mtu_idx, (DAL_MANGO_L3_API_FLAG_NONE)), errMtuIp6Realloc, ret);
    }
    MANGO_L3_DBG_PRINTF(3, "pIntf->mtu = %u, ip_mtu = %u, intfEntry.egrIntf.ip_mtu_idx = %u\n", pIntf->mtu, ip_mtu, intfEntry.egrIntf.ip_mtu_idx);
    MANGO_L3_DBG_PRINTF(3, "pIntf->ipv6_mtu = %u, ip6_mtu = %u, intfEntry.egrIntf.ip6_mtu_idx = %u\n", pIntf->ipv6_mtu, ip6_mtu, intfEntry.egrIntf.ip6_mtu_idx);
    intfEntry.egrIntf.ipmc_ttl_scope    = pIntf->ttl;
    intfEntry.egrIntf.ip6mc_hl_scope    = pIntf->ttl;   /* sync with IPv4 TTL */

    RT_ERR_HDL(_dal_mango_l3_intfIgrEntry_set(unit, intfIdx, &intfEntry.igrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfSet, ret);
    RT_ERR_HDL(_dal_mango_l3_intfEgrEntry_set(unit, intfIdx, &intfEntry.egrIntf, (DAL_MANGO_L3_API_FLAG_NONE)), errIntfSet, ret);

errEntryNotFound:
errIntfSet:
errMtuIp6Realloc:
errMtuRealloc:
errMacEntrySet:
errVlanL3IntfIdxSet:
errIpMtuGet:
errIntfGet:
errType:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intf_set */

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
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Note:
 *      None
 */
int32
dal_mango_l3_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intf_stats_t *pStats)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    l3_intf_igr_cntr_t igrCntr;
    l3_intf_egr_cntr_t egrCntr;
    uint32  cntr[2];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(L3_INTF_IDX_IS_L2_TUNNEL(unit, intfIdx), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((NULL == pStats), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    if ( ((FALSE == _pL3Db[unit]->intf[intfIdx].valid) && (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[unit]->intf[intfIdx].intf_id))) || (DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(intfId)))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* load data */
    L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, intfIdx, igrCntr, "", errHandle, ret);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_IGR_INTF_CNTR_IF_IN_OCTETStf, cntr, igrCntr, "", errHandle, ret);
    pStats->rx.octets = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_IGR_INTF_CNTR_IF_IN_UCAST_PKTStf, cntr, igrCntr, "", errHandle, ret);
    pStats->rx.unicast_pkts = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_IGR_INTF_CNTR_IF_IN_MULTICAST_PKTStf, cntr, igrCntr, "", errHandle, ret);
    pStats->rx.multicast_pkts = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_IGR_INTF_CNTR_IF_IN_BROADCAST_PKTStf, cntr, igrCntr, "", errHandle, ret);
    pStats->rx.broadcast_pkts = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_IGR_INTF_CNTR_IF_IN_DISCARDStf, cntr, igrCntr, "", errHandle, ret);
    pStats->rx.discards = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    L3_TABLE_READ_ERR_HDL(unit, MANGO_L3_EGR_INTF_CNTRt, intfIdx, egrCntr, "", errHandle, ret);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_EGR_INTF_CNTR_IF_OUT_OCTETStf, cntr, egrCntr, "", errHandle, ret);
    pStats->tx.octets = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_EGR_INTF_CNTR_IF_OUT_UCAST_PKTStf, cntr, egrCntr, "", errHandle, ret);
    pStats->tx.unicast_pkts = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_EGR_INTF_CNTR_IF_OUT_MULTICAST_PKTStf, cntr, egrCntr, "", errHandle, ret);
    pStats->tx.multicast_pkts = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_EGR_INTF_CNTR_IF_OUT_BROADCAST_PKTStf, cntr, egrCntr, "", errHandle, ret);
    pStats->tx.broadcast_pkts = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);

    osal_memset(&cntr, 0x00, sizeof(cntr));
    L3_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, MANGO_L3_EGR_INTF_CNTR_IF_OUT_DISCARDStf, cntr, egrCntr, "", errHandle, ret);
    pStats->tx.discards = ((uint64)cntr[1] << 32) | ((uint64)cntr[0] << 0);


errEntryNotFound:
errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intfStats_get */

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
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Note:
 *      None
 */
int32
dal_mango_l3_intfStats_reset(uint32 unit, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    l3_intf_igr_cntr_t igrCntr;
    l3_intf_egr_cntr_t egrCntr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(L3_INTF_IDX_IS_L2_TUNNEL(unit, intfIdx), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
    L3_SEM_LOCK(unit);

    if ( ((FALSE == _pL3Db[unit]->intf[intfIdx].valid) && (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[unit]->intf[intfIdx].intf_id))) || (DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(intfId)))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* reset data */
    osal_memset(&igrCntr, 0x00, sizeof(l3_intf_igr_cntr_t));
    osal_memset(&egrCntr, 0x00, sizeof(l3_intf_egr_cntr_t));
    L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_IGR_INTF_CNTRt, intfIdx, igrCntr, "", errHandle, ret);
    L3_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_EGR_INTF_CNTRt, intfIdx, egrCntr, "", errHandle, ret);

errEntryNotFound:
errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intfStats_reset */

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
int32
dal_mango_l3_vrrp_add(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    int32   macIdx, validMacIdx;
    rtk_mac_t   vrrpMac;
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=%d,vid=%d,vrId=%d", unit, flags, vid, vrId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((flags >= RTK_L3_VRRP_FLAG_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((vid < RTK_VLAN_ID_MIN) || (vid > RTK_VLAN_ID_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((RTK_L3_VRRP_VRID_MAX <= vrId), RT_ERR_OUT_OF_RANGE);

    /* build VRRP MAC */
    vrrpMac.octet[0] = 0x00;
    vrrpMac.octet[1] = 0x00;
    vrrpMac.octet[2] = 0x5E;
    vrrpMac.octet[3] = 0x00;
    vrrpMac.octet[4] = flags;
    vrrpMac.octet[5] = vrId;

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_get(unit, vid, &intfIdx), errVlanL3IntfIdxGet, ret);

    /* scan empty entry and existence */
    validMacIdx = -1;
    for (macIdx=1; macIdx<DAL_MANGO_L3_MAC_MAX; macIdx++)
    {
        if (BITMAP_IS_SET(_pL3Db[unit]->HW.mac_used, macIdx))
        {
            RT_ERR_HDL(_dal_mango_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntryGet, ret);
            if ((macEntry.valid) &&
                (macEntry.intf_id == intfIdx) &&
                (0 == rt_util_macCmp(macEntry.mac.octet, vrrpMac.octet)))
            {
                ret = RT_ERR_INPUT; /* exist */
                goto errInput;
            }
        }
        else if (validMacIdx < 0)
        {
            validMacIdx = macIdx;   /* first empty entry */
        }
    }

    if (validMacIdx < 0)
    {
        ret = RT_ERR_EXCEEDS_CAPACITY;
        goto errExceedsCapacity;
    }
    else
    {
        osal_memset(&macEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));
        macEntry.valid      = 1;
        macEntry.intf_id    = intfIdx;
        macEntry.mac        = vrrpMac;
        macEntry.bmsk_intf_id = 0x3FF;
        macEntry.act        = RTK_L3_ACT_FORWARD;
        osal_memset(&macEntry.bmsk_mac, 0xFF, sizeof(rtk_mac_t));
        RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, validMacIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);

        BITMAP_SET(_pL3Db[unit]->HW.mac_used, validMacIdx);
        _pL3Db[unit]->HW.mac_used_count += 1;
    }

errVlanL3IntfIdxGet:
errMacEntryGet:
errInput:
errExceedsCapacity:
errMacEntrySet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_vrrp_add */

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
int32
dal_mango_l3_vrrp_del(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    int32   macIdx;
    rtk_mac_t   vrrpMac;
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,vid=%d,vrId=%d", unit, vid, vrId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((flags >= RTK_L3_VRRP_FLAG_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((vid < RTK_VLAN_ID_MIN) || (vid > RTK_VLAN_ID_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((RTK_L3_VRRP_VRID_MAX <= vrId), RT_ERR_OUT_OF_RANGE);

    /* build VRRP MAC */
    vrrpMac.octet[0] = 0x00;
    vrrpMac.octet[1] = 0x00;
    vrrpMac.octet[2] = 0x5E;
    vrrpMac.octet[3] = 0x00;
    vrrpMac.octet[4] = flags;
    vrrpMac.octet[5] = vrId;

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_get(unit, vid, &intfIdx), errVlanL3IntfIdxGet, ret);

    /* scan empty entry */
    for (macIdx=1; macIdx<DAL_MANGO_L3_MAC_MAX; macIdx++)
    {
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.mac_used, macIdx))
            continue;

        RT_ERR_HDL(_dal_mango_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntryGet, ret);
        if ((macEntry.valid) &&
            (macEntry.intf_id == intfIdx) &&
            (0 == rt_util_macCmp(macEntry.mac.octet, vrrpMac.octet)))
        {
            osal_memset(&macEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));
            RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, macIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);

            if (BITMAP_IS_SET(_pL3Db[unit]->HW.mac_used, macIdx))
            {
                 _pL3Db[unit]->HW.mac_used_count -= 1;
                BITMAP_CLEAR(_pL3Db[unit]->HW.mac_used, macIdx);
            }

            ret = RT_ERR_OK;
            goto errOk;
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errVlanL3IntfIdxGet:
errMacEntryGet:
errMacEntrySet:
errOk:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_vrrp_del */

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
int32
dal_mango_l3_vrrp_delAll(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    int32   macIdx;
    rtk_mac_t   vrrpMac;
    dal_mango_l3_macEntry_t macEntry, emptyEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((flags >= RTK_L3_VRRP_FLAG_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((vid < RTK_VLAN_ID_MIN) || (vid > RTK_VLAN_ID_MAX), RT_ERR_OUT_OF_RANGE);

    /* build VRRP MAC */
    vrrpMac.octet[0] = 0x00;
    vrrpMac.octet[1] = 0x00;
    vrrpMac.octet[2] = 0x5E;
    vrrpMac.octet[3] = 0x00;
    vrrpMac.octet[4] = flags;
    vrrpMac.octet[5] = 0x00;

    /* build an empty entry */
    osal_memset(&emptyEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_get(unit, vid, &intfIdx), errVlanL3IntfIdxGet, ret);

    /* scan empty entry and existence */
    for (macIdx=1; macIdx<DAL_MANGO_L3_MAC_MAX; macIdx++)
    {
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.mac_used, macIdx))
            continue;

        osal_memset(&emptyEntry, 0x00, sizeof(dal_mango_l3_macEntry_t));
        RT_ERR_HDL(_dal_mango_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntryGet, ret);
        if ((macEntry.valid) &&
            (macEntry.intf_id == intfIdx) &&
            (0 == osal_memcmp(&vrrpMac.octet, macEntry.mac.octet, 0x05)))   /* VRRP MAC address */
        {
            RT_ERR_HDL(_dal_mango_l3_macEntry_set(unit, macIdx, &emptyEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntrySet, ret);

            if (BITMAP_IS_SET(_pL3Db[unit]->HW.mac_used, macIdx))
            {
                 _pL3Db[unit]->HW.mac_used_count -= 1;
                BITMAP_CLEAR(_pL3Db[unit]->HW.mac_used, macIdx);
            }
        }
    }

    ret = RT_ERR_OK;

errVlanL3IntfIdxGet:
errMacEntryGet:
errMacEntrySet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_vrrp_delAll */

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
int32
dal_mango_l3_vrrp_get(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrIdArraySize, uint32 *pVrIdArray, uint32 *pVrIdCount)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    int32   macIdx;
    rtk_mac_t   vrrpMac;
    dal_mango_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,vid=%d,vrIdArraySize=%d", unit, vid, vrIdArraySize);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((flags >= RTK_L3_VRRP_FLAG_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((vid < RTK_VLAN_ID_MIN) || (vid > RTK_VLAN_ID_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pVrIdArray), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pVrIdCount), RT_ERR_NULL_POINTER);

    /* build VRRP MAC */
    vrrpMac.octet[0] = 0x00;
    vrrpMac.octet[1] = 0x00;
    vrrpMac.octet[2] = 0x5E;
    vrrpMac.octet[3] = 0x00;
    vrrpMac.octet[4] = flags;
    vrrpMac.octet[5] = 0x00;

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_vlan_l3IntfIdx_get(unit, vid, &intfIdx), errVlanL3IntfIdxGet, ret);

    /* scan empty entry and existence */
    *pVrIdCount = 0;
    for (macIdx=1; (macIdx<DAL_MANGO_L3_MAC_MAX) && (*pVrIdCount < vrIdArraySize); macIdx++)
    {
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.mac_used, macIdx))
            continue;

        RT_ERR_HDL(_dal_mango_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_MANGO_L3_API_FLAG_NONE)), errMacEntryGet, ret);
        if ((macEntry.valid) &&
            (macEntry.intf_id == intfIdx) &&
            (0 == osal_memcmp(&vrrpMac.octet, macEntry.mac.octet, 0x05)))   /* VRRP MAC address */
        {
            *(pVrIdArray + (*pVrIdCount)) = (uint32)(macEntry.mac.octet[5]);    /* VRID */
            *pVrIdCount += 1;
        }
    }

    ret = RT_ERR_OK;

errVlanL3IntfIdxGet:
errMacEntryGet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_vrrp_get */

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
 *          RTK_L3_FLAG_WITH_ID
 *      (2) If the flag RTK_L3_FLAG_REPLACE is set, then replace the existing entry
 *          with the new info based on the input path ID (nhId).
 *          Otherwise, SDK will allocate a path ID for this new nexthop entry.
 */
int32
dal_mango_l3_nextHop_create(uint32 unit, rtk_l3_flag_t flags, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIntfIdx, intfIdx, nhIdx;
    dal_mango_l2_ucastNhAddr_t l2ucEntry;
    dal_mango_l2_mcastNhAddr_t l2mcEntry;
    dal_mango_l3_nhEntry_t nhEntry;
    uint32  l2_dmac_idx = 0;
    int32   mac_addr_allocated = FALSE; /* allocation state in this operation */

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=%d,pNextHop=%p,pPathId=%p", unit, flags, pNextHop, pPathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((((RTK_L3_FLAG_REPLACE & flags) == 0) && ((RTK_L3_FLAG_WITH_ID & flags) == 0) && (RTK_L3_FLAG_NONE != flags)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((RTK_L3_FLAG_WITH_ID & flags) && (*pPathId >= DAL_MANGO_L3_NEXTHOP_MAX)), RT_ERR_OUT_OF_RANGE);

    /* intfId to intfIdx */
    intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pNextHop->intf_id);
    RT_PARAM_CHK((intfIdx > DAL_MANGO_L3_INTF_MAX), RT_ERR_OUT_OF_RANGE);

    /* initialize local variables */
    osal_memset(&l2ucEntry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
    osal_memset(&l2mcEntry, 0x00, sizeof(dal_mango_l2_mcastNhAddr_t));

    /* function body */
    L3_SEM_LOCK(unit);

    /* check interface existence */
    if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, intfIdx))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errNotFound;
    }

    if (flags & RTK_L3_FLAG_REPLACE)
    {
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(*pPathId);
        if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errNotFound;
        }

        /* check if interface has been changed */
        if (_pL3Db[unit]->nexthop[nhIdx].intf_idx != intfIdx)
        {
            oldIntfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;
            L3_DB_UPDATE_INTF_REFCNT_DEC(unit, oldIntfIdx);
            _pL3Db[unit]->nexthop[nhIdx].intf_idx = intfIdx;    /* new interface */
            L3_DB_UPDATE_INTF_REFCNT_INC(unit, intfIdx);
        } else {
            oldIntfIdx = intfIdx;
        }

        /* check if DMAC / interface index have been changed or the old DMAC entry isn't needed*/
        if ((0 != osal_memcmp(&_pL3Db[unit]->nexthop[nhIdx].mac_addr, &pNextHop->mac_addr, sizeof(rtk_mac_t))) ||
            (oldIntfIdx != intfIdx) ||
            (pNextHop->l3_act != RTK_L3_ACT_FORWARD) ||
            (DAL_MANGO_L3_INTF_ID_IS_L3_TUNNEL(unit, pNextHop->intf_id))) /* intf_id is l3IntfIdx for l3IntfId */
        {
            /* free old L2 DMAC entry first if needed */
            if (TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
            {
                /* prepare L2 entry for deleteing */
                if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
                {
                    //osal_memset(&l2mcEntry, 0x00, sizeof(dal_mango_l2_mcastNhAddr_t));
                    RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[oldIntfIdx].vid, \
                               &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2mcEntry.vid), errFidGet, ret);
                    l2mcEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;

                    L3_RT_ERR_HDL_DBG(_dal_mango_l2_mcastNexthop_del(unit, &l2mcEntry), "");
                }
                else
                {
                    //osal_memset(&l2ucEntry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
                    RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[oldIntfIdx].vid, \
                               &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2ucEntry.fid), errFidGet, ret);
                    l2ucEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;

                    L3_RT_ERR_HDL_DBG(_dal_mango_l2_nexthop_del(unit, &l2ucEntry), "");
                }

                /* Update DB */
                _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = FALSE;
                _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx = 0;
            }

            _pL3Db[unit]->nexthop[nhIdx].mac_addr = pNextHop->mac_addr;
        }

        /* typical L3 interface or L2 tunnel interface (not allocated and need) */
        if ((FALSE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc) &&
            (pNextHop->l3_act == RTK_L3_ACT_FORWARD) &&
            (DAL_MANGO_L3_INTF_ID_IS_L3_COMMON(unit, pNextHop->intf_id) ||
             DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(pNextHop->intf_id)))
        {
            /* prepare L2 entry for creating */
            if (TRUE == l3_util_macAddr_isMcast(&pNextHop->mac_addr))
            {
                //osal_memset(&l2mcEntry, 0x00, sizeof(dal_mango_l2_mcastNhAddr_t));
                RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                           &pNextHop->mac_addr, &l2mcEntry.vid), errFidGet, ret);
                l2mcEntry.mac = pNextHop->mac_addr;

                /* call L2 internal API to add a MAC entry */
                RT_ERR_HDL(_dal_mango_l2_mcastNexthop_add(unit, &l2mcEntry), errL2NhAdd, ret);

                l2_dmac_idx = l2mcEntry.l2_idx;
                MANGO_L3_DBG_PRINTF(3, "l2_dmac_idx = %d (0x%x)\n", l2_dmac_idx, l2_dmac_idx);
            }
            else
            {
                //osal_memset(&l2ucEntry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
                RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                           &pNextHop->mac_addr, &l2ucEntry.fid), errFidGet, ret);
                l2ucEntry.mac = pNextHop->mac_addr;
                l2ucEntry.add_op_flags = RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC;

                /* call L2 internal API to add a MAC entry */
                RT_ERR_HDL(_dal_mango_l2_nexthop_add(unit, &l2ucEntry), errL2NhAdd, ret);

                l2_dmac_idx = l2ucEntry.l2_idx;
            }

            /* set a flag to indicate L2 entry has been created */
            mac_addr_allocated = TRUE;

            /* Update DB */
            _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = TRUE;
            _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx = l2_dmac_idx;
        }

        /* write nexthop entry into chip */
        osal_memset(&nhEntry, 0x00, sizeof(dal_mango_l3_nhEntry_t));
        if (pNextHop->l3_act == RTK_L3_ACT_FORWARD)
        {
            nhEntry.dmac_idx = (TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)? \
                                        _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx : \
                                        DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX;
        } else {
            switch (pNextHop->l3_act)
            {
                case RTK_L3_ACT_TRAP2CPU:
                    nhEntry.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2CPU;
                    break;

                case RTK_L3_ACT_TRAP2MASTERCPU:
                    nhEntry.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER;
                    break;

                case RTK_L3_ACT_DROP:
                default:
                    nhEntry.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_DROP;
                    break;
            }
        }
        nhEntry.l3_egr_intf_idx = intfIdx;
        RT_ERR_HDL(_dal_mango_l3_nhEntry_set(unit, nhIdx, &nhEntry, DAL_MANGO_L3_API_FLAG_NONE), errL3NhSet, ret);

        MANGO_L3_DBG_PRINTF(3, "nhEntry has been changed, pathId = %u\n", *pPathId);
    }
    else
    {
        /* typical L3 interface or L2 tunnel interface */
        if ((pNextHop->l3_act == RTK_L3_ACT_FORWARD) &&
            (DAL_MANGO_L3_INTF_ID_IS_L3_COMMON(unit, pNextHop->intf_id) ||
             DAL_MANGO_L3_INTF_ID_IS_L2_TUNNEL(pNextHop->intf_id)))
        {
            /* prepare L2 entry for creating */
            if (TRUE == l3_util_macAddr_isMcast(&pNextHop->mac_addr))
            {
                //osal_memset(&l2mcEntry, 0x00, sizeof(dal_mango_l2_mcastNhAddr_t));
                RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                           &pNextHop->mac_addr, &l2mcEntry.vid), errFidGet, ret);
                l2mcEntry.mac = pNextHop->mac_addr;

                /* call L2 internal API to add a MAC entry */
                RT_ERR_HDL(_dal_mango_l2_mcastNexthop_add(unit, &l2mcEntry), errL2NhAdd, ret);

                l2_dmac_idx = l2mcEntry.l2_idx;
                MANGO_L3_DBG_PRINTF(3, "l2_dmac_idx = %d (0x%x)\n", l2_dmac_idx, l2_dmac_idx);
            }
            else
            {
                //osal_memset(&l2ucEntry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
                RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                           &pNextHop->mac_addr, &l2ucEntry.fid), errFidGet, ret);
                l2ucEntry.mac = pNextHop->mac_addr;
                l2ucEntry.add_op_flags = RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC;

                /* call L2 internal API to add a MAC entry */
                RT_ERR_HDL(_dal_mango_l2_nexthop_add(unit, &l2ucEntry), errL2NhAdd, ret);

                l2_dmac_idx = l2ucEntry.l2_idx;
            }

            /* set a flag to indicate L2 entry has been created */
            mac_addr_allocated = TRUE;
        }

        /* try to alloc an empty nexthop entry */
        if (flags & RTK_L3_FLAG_WITH_ID)
        {
            nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(*pPathId);
            RT_ERR_HDL(_dal_mango_l3_nhEntry_alloc(unit, &nhIdx, DAL_MANGO_L3_API_FLAG_WITH_ID), errL3NhAlloc, ret);
        } else {
            RT_ERR_HDL(_dal_mango_l3_nhEntry_alloc(unit, &nhIdx, DAL_MANGO_L3_API_FLAG_NONE), errL3NhAlloc, ret);
        }

        /* write nexthop entry into chip */
        osal_memset(&nhEntry, 0x00, sizeof(dal_mango_l3_nhEntry_t));
        if (pNextHop->l3_act == RTK_L3_ACT_FORWARD)
        {
            nhEntry.dmac_idx = (mac_addr_allocated == TRUE)? l2_dmac_idx : DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX;
        } else {
            switch (pNextHop->l3_act)
            {
                case RTK_L3_ACT_TRAP2CPU:
                    nhEntry.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2CPU;
                    break;

                case RTK_L3_ACT_TRAP2MASTERCPU:
                    nhEntry.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER;
                    break;

                case RTK_L3_ACT_DROP:
                default:
                    nhEntry.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_DROP;
                    break;
            }
        }
        nhEntry.l3_egr_intf_idx = intfIdx;
        RT_ERR_HDL(_dal_mango_l3_nhEntry_set(unit, nhIdx, &nhEntry, DAL_MANGO_L3_API_FLAG_NONE), errL3NhSet, ret);

        /* update DB */
        _pL3Db[unit]->nexthop[nhIdx].mac_addr = pNextHop->mac_addr;
        _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = mac_addr_allocated;
        _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx = (mac_addr_allocated == TRUE)? l2_dmac_idx : 0;
        _pL3Db[unit]->nexthop[nhIdx].intf_idx = intfIdx;
        L3_DB_UPDATE_NH_VALID(unit, nhIdx);
        L3_DB_UPDATE_INTF_REFCNT_INC(unit, intfIdx);

        /* nhIdx to pathId */
        *pPathId = DAL_MANGO_L3_NH_IDX_TO_PATH_ID(nhIdx);

        MANGO_L3_DBG_PRINTF(3, "l2_dmac_idx = %u, nhEntry.dmac_idx = %u\n", l2_dmac_idx, nhEntry.dmac_idx);
    }

    goto errOk;

errL3NhSet:
    L3_RT_ERR_HDL_DBG(_dal_mango_l3_nhEntry_free(unit, nhIdx, DAL_MANGO_L3_API_FLAG_NONE), "");

errL3NhAlloc:
    if (TRUE == mac_addr_allocated)
    {
        if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
        {
            L3_RT_ERR_HDL_DBG(_dal_mango_l2_mcastNexthop_del(unit, &l2mcEntry), "");
        }
        else
        {
            L3_RT_ERR_HDL_DBG(_dal_mango_l2_nexthop_del(unit, &l2ucEntry), "");
        }
    }

errL2NhAdd:
errFidGet:
errNotFound:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_nextHop_create */


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
int32
dal_mango_l3_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx, nhIdx;
    dal_mango_l2_ucastNhAddr_t l2ucEntry;
    dal_mango_l2_mcastNhAddr_t l2mcEntry;
    int32   mac_addr_allocated;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pathId=%d", unit, pathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_NH(pathId)), RT_ERR_INPUT);

    /* initialize local variables */
    osal_memset(&l2ucEntry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
    osal_memset(&l2mcEntry, 0x00, sizeof(dal_mango_l2_mcastNhAddr_t));

    /* function body */
    L3_SEM_LOCK(unit);

    /* pathId to nhIdx */
    nhIdx = DAL_MANGO_L3_PATH_ID_ENTRY_IDX(pathId);
    intfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;

    /* check validation */
    if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* check reference count */
    if ((_pL3Db[unit]->refer_cnt_chk_en) &&
        (L3_DB_NH_REFCNT(unit, nhIdx) > 0))
    {
        ret = RT_ERR_ENTRY_REFERRED;
        goto errEntryReferred;
    }

    /* check if MAC has been allocated */
    mac_addr_allocated = _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc;
    if (TRUE == mac_addr_allocated)
    {
        /* prepare L2 entry for deleteing */
        if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
        {
            //osal_memset(&l2mcEntry, 0x00, sizeof(dal_mango_l2_mcastNhAddr_t));
            RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                       &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2mcEntry.vid), errFidGet, ret);
            l2mcEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
        }
        else
        {
            //osal_memset(&l2ucEntry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
            RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                       &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2ucEntry.fid), errFidGet, ret);
            l2ucEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
        }
    }

    RT_ERR_HDL(_dal_mango_l3_nhEntry_free(unit, nhIdx, DAL_MANGO_L3_API_FLAG_NONE), errL3NhFree, ret);

errL3NhFree:
    if (TRUE == mac_addr_allocated)
    {
        if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
        {
            L3_RT_ERR_HDL_DBG(_dal_mango_l2_mcastNexthop_del(unit, &l2mcEntry), "");
        }
        else
        {
            L3_RT_ERR_HDL_DBG(_dal_mango_l2_nexthop_del(unit, &l2ucEntry), "");
        }
    }

    /* update DB */
    L3_DB_UPDATE_INTF_REFCNT_DEC(unit, intfIdx);
    osal_memset(&_pL3Db[unit]->nexthop[nhIdx].mac_addr, 0x00, sizeof(rtk_mac_t));
    _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = FALSE;
    _pL3Db[unit]->nexthop[nhIdx].intf_idx = 0;
    L3_DB_UPDATE_NH_INVALID(unit, nhIdx);

errFidGet:
errEntryReferred:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_nextHop_destroy */


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
int32
dal_mango_l3_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,  rtk_l3_nextHop_t *pNextHop)
{
    int32   ret = RT_ERR_OK;
    uint32  nhIdx;
    dal_mango_l3_nhEntry_t nhEntry;
    dal_mango_l2_index_t l2Index;
    dal_mango_l2_entry_t l2Entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pathId=%d", unit, pathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!(DAL_MANGO_L3_PATH_ID_IS_NH(pathId)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    if (DAL_MANGO_L3_PATH_ID_IS_NH(pathId))
    {
        /* pathId to nhIdx */
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pathId);

        /* check validation */
        if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }

        /* for debugging purpose, we should get actual data from chip */
        RT_ERR_HDL(_dal_mango_l3_nhEntry_get(unit, nhIdx, &nhEntry, DAL_MANGO_L3_API_FLAG_NONE), errNhEntrySet, ret);

        osal_memset(pNextHop, 0x00, sizeof(rtk_l3_nextHop_t));
        pNextHop->intf_id = _pL3Db[unit]->intf[(nhEntry.l3_egr_intf_idx % DAL_MANGO_L3_INTF_MAX)].intf_id;

        if (TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
        {
            /* call L2 internal API to get MAC address (l2_idx to mac) */
            osal_memset(&l2Index, 0x00, sizeof(dal_mango_l2_index_t));
            //L2_MAC_IDX_TO_INDEX_STRUCT(nhEntry.dmac_idx, l2Index);
            L2_MAC_IDX_TO_INDEX_STRUCT(_pL3Db[unit]->nexthop[nhIdx].mac_addr_idx, l2Index);
            RT_ERR_HDL(_dal_mango_l2_getL2Entry(unit, &l2Index, &l2Entry), errL2EntryGet, ret);
            pNextHop->mac_addr = (l2Entry.entry_type == L2_MULTICAST)? l2Entry.l2mcast.mac : l2Entry.unicast.mac;

            MANGO_L3_DBG_PRINTF(3, "pNextHop->intf_id = %u, MAC = %02X:%02X:%02X:%02X:%02X:%02X\n", \
                pNextHop->intf_id, \
                pNextHop->mac_addr.octet[0], \
                pNextHop->mac_addr.octet[1], \
                pNextHop->mac_addr.octet[2], \
                pNextHop->mac_addr.octet[3], \
                pNextHop->mac_addr.octet[4], \
                pNextHop->mac_addr.octet[5]);
        }

        /* action */
        switch (nhEntry.dmac_idx)
        {
        case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2CPU:
            pNextHop->l3_act = RTK_L3_ACT_TRAP2CPU;
            break;

        case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER:
            pNextHop->l3_act = RTK_L3_ACT_TRAP2MASTERCPU;
            break;

        case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_DROP:
            pNextHop->l3_act = RTK_L3_ACT_DROP;
            break;

        default:
            pNextHop->l3_act = RTK_L3_ACT_FORWARD;
            break;
        }
    }
    else if (DAL_MANGO_L3_PATH_ID_IS_MPLS(pathId))
    {
        /* pathId to nhIdx */
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pathId);

        /* check validation */
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.nexthop_used, nhIdx))
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }

        /* for debugging purpose, we should get actual data from chip (for MPLS) */
        RT_ERR_HDL(_dal_mango_l3_nhEntry_get(unit, nhIdx, &nhEntry, _pL3Db[unit]->HW.nexthop[nhIdx].flags), errNhEntrySet, ret);

        osal_memset(pNextHop, 0x00, sizeof(rtk_l3_nextHop_t));
        pNextHop->intf_id = _pL3Db[unit]->intf[(nhEntry.l3_egr_intf_idx % DAL_MANGO_L3_INTF_MAX)].intf_id;

        if (nhEntry.dmac_idx < DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX)   /* valid L2 entry index */
        {
            /* call L2 internal API to get MAC address (l2_idx to mac) */
            osal_memset(&l2Index, 0x00, sizeof(dal_mango_l2_index_t));
            //L2_MAC_IDX_TO_INDEX_STRUCT(nhEntry.dmac_idx, l2Index);
            L2_MAC_IDX_TO_INDEX_STRUCT(nhEntry.dmac_idx, l2Index);
            RT_ERR_HDL(_dal_mango_l2_getL2Entry(unit, &l2Index, &l2Entry), errL2EntryGet, ret);
            pNextHop->mac_addr = (l2Entry.entry_type == L2_MULTICAST)? l2Entry.l2mcast.mac : l2Entry.unicast.mac;

            MANGO_L3_DBG_PRINTF(3, "pNextHop->intf_id = %u, MAC = %02X:%02X:%02X:%02X:%02X:%02X\n", \
                pNextHop->intf_id, \
                pNextHop->mac_addr.octet[0], \
                pNextHop->mac_addr.octet[1], \
                pNextHop->mac_addr.octet[2], \
                pNextHop->mac_addr.octet[3], \
                pNextHop->mac_addr.octet[4], \
                pNextHop->mac_addr.octet[5]);
        }

        /* action */
        switch (nhEntry.dmac_idx)
        {
        case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2CPU:
            pNextHop->l3_act = RTK_L3_ACT_TRAP2CPU;
            break;

        case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER:
            pNextHop->l3_act = RTK_L3_ACT_TRAP2MASTERCPU;
            break;

        case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_DROP:
            pNextHop->l3_act = RTK_L3_ACT_DROP;
            break;

        default:
            pNextHop->l3_act = RTK_L3_ACT_FORWARD;
            break;
        }
    }

errL2EntryGet:
errNhEntrySet:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_nextHop_get */


static int32
_dal_mango_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId)
{
    int32   ret = RT_ERR_OK;
    uint32  nhIdx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    /* DONT NEED TO LOCK SEMAPHORE HERE (called by internal APIs) */

    /* function body */
    for (nhIdx=0; nhIdx<DAL_MANGO_L3_NEXTHOP_MAX; nhIdx++)
    {
        if (_pL3Db[unit]->nexthop[nhIdx].valid)
        {
            if ((DAL_MANGO_L3_INTF_ID_ENTRY_IDX(pNextHop->intf_id) == _pL3Db[unit]->nexthop[nhIdx].intf_idx) &&
                (L2_MAC_ADDR_IS_EQUAL(pNextHop->mac_addr, _pL3Db[unit]->nexthop[nhIdx].mac_addr)))
            {
                /* nhIdx to pathId */
                *pPathId = DAL_MANGO_L3_NH_IDX_TO_PATH_ID(nhIdx);
                goto errOk;
            }
        }
    }

    /* entry not found */
    ret = RT_ERR_ENTRY_NOTFOUND;

errOk:
    return ret;
}


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
int32
dal_mango_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_l3_nextHopPath_find(unit, pNextHop, pPathId), errHandle, ret);

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_nextHopPath_find */


/* Function Name:
 *      dal_mango_l3_ecmp_create
 * Description:
 *      Create an ECMP path (contains one or more nexthop paths)
 * Input:
 *      unit       - unit id
 *      flags      - optional flags (REPLACE flag for updating)
 *      pathCnt    - size of the allocated array
 *      pIntfArray - pointer to the path ID array
 *      pPathId    - pointer to the ECMP path ID
 * Output:
 *      pPathId    - pointer to the ECMP path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL         - table is full
 *      RT_ERR_EXCEEDS_CAPACITY - exceed the hardware capacity
 * Note:
 */
int32
dal_mango_l3_ecmp_create(uint32 unit, rtk_l3_flag_t flags, uint32 nhIdCnt, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pPathId)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  nhIdx;
    uint32  ecmpIdx;
    dal_mango_l3_ecmpEntry_t ecmpEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=%d,nhIdCnt=%d", unit, flags, nhIdCnt);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((0 == nhIdCnt), RT_ERR_INPUT);
    RT_PARAM_CHK((nhIdCnt > DAL_MANGO_L3_ECMP_NH_CNT_MAX), RT_ERR_EXCEEDS_CAPACITY);
    RT_PARAM_CHK((NULL == pNhIdArray), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    /* pre-check input data */
    for (idx=0; idx<nhIdCnt; idx++)
    {
        if (DAL_MANGO_L3_PATH_ID_IS_NH(*(pNhIdArray + idx)))
        {
            nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(*(pNhIdArray + idx));

            if (nhIdx >= DAL_MANGO_L3_NEXTHOP_MAX)
            {
                return RT_ERR_INPUT;
            }

            if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
            {
                return RT_ERR_ENTRY_NOTFOUND;
            }
        }
        else if (DAL_MANGO_L3_PATH_ID_IS_MPLS(*(pNhIdArray + idx)))
        {
            //return RT_ERR_CHIP_NOT_SUPPORTED;
            nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(*(pNhIdArray + idx));

            if (nhIdx >= DAL_MANGO_L3_NEXTHOP_MAX)
            {
                return RT_ERR_INPUT;
            }

            if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.nexthop_used, nhIdx))
            {
                return RT_ERR_ENTRY_NOTFOUND;
            }
        }
        else
        {
            return RT_ERR_INPUT;
        }
    }

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_REPLACE)
    {
        if (!DAL_MANGO_L3_PATH_ID_IS_ECMP(*pPathId))
        {
            ret = RT_ERR_INPUT;
            goto errInput;
        }

        ecmpIdx = (DAL_MANGO_L3_PATH_ID_ENTRY_IDX(*pPathId) % DAL_MANGO_L3_ECMP_MAX);
        if (!L3_DB_ECMP_IS_VALID(unit, ecmpIdx))
        {
            ret = RT_ERR_INPUT;
            goto errInput;
        }

        /* release - update old nexthop reference counter */
        RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryGet, ret);
        for (idx=0; idx<(_pL3Db[unit]->ecmp[ecmpIdx].nh_count); idx++)
        {
            nhIdx = ecmpEntry.nh_idx[idx];

            /* typical nexthop entry */
            if (L3_DB_NH_IS_VALID(unit, nhIdx))
            {
                L3_DB_UPDATE_NH_REFCNT_DEC(unit, nhIdx);
            }
        }
    }
    else
    {
        if (flags & RTK_L3_FLAG_WITH_ID)
        {
            ecmpIdx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(*pPathId);
            RT_ERR_HDL(_dal_mango_l3_ecmpEntry_alloc(unit, &ecmpIdx, DAL_MANGO_L3_API_FLAG_WITH_ID), errEcmpEntryAlloc, ret);
        } else {
            RT_ERR_HDL(_dal_mango_l3_ecmpEntry_alloc(unit, &ecmpIdx, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryAlloc, ret);
        }
    }

    _pL3Db[unit]->ecmp[ecmpIdx].nh_count = nhIdCnt;
    osal_memset(&ecmpEntry, 0x00, sizeof(dal_mango_l3_ecmpEntry_t));
    /* auto-rebuild or manual */
    if (_pL3Db[unit]->ecmp_hash_tbl_manual)
    {
        for (idx=0; idx<DAL_MANGO_L3_ECMP_NH_HASH_SIZE; idx++)
        {
            /* mod N (number of elements) */
            ecmpEntry.hash_to_nh_id[idx] = idx % nhIdCnt;
        }
    }
    for (idx=0; idx<nhIdCnt; idx++)
    {
        /* pathId to nhIdx */
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(*(pNhIdArray + idx));

        ecmpEntry.nh_idx[idx] = nhIdx;
        /* typical nexthop entry */
        if (L3_DB_NH_IS_VALID(unit, nhIdx))
        {
            L3_DB_UPDATE_NH_REFCNT_INC(unit, nhIdx);
        }

        MANGO_L3_DBG_PRINTF(3, "ecmpEntry.nh_idx[%u] = %u (nhIdx)\n", idx, nhIdx);
    }
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_set(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntrySet, ret);

    MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->ecmp[ecmpIdx].nh_count = %u\n", _pL3Db[unit]->ecmp[ecmpIdx].nh_count);

    /* update DB */
    L3_DB_UPDATE_ECMP_VALID(unit, ecmpIdx);

    /* ecmpIdx to pathId */
    *pPathId = DAL_MANGO_L3_ECMP_IDX_TO_PATH_ID(ecmpIdx);

errEcmpEntrySet:
errEcmpEntryAlloc:
errEcmpEntryGet:
errInput:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_ecmp_create */


/* Function Name:
 *      dal_mango_l3_ecmp_destroy
 * Description:
 *      Destroy an ECMP path
 * Input:
 *      unit   - unit id
 *      pathId - ECMP path ID
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
int32
dal_mango_l3_ecmp_destroy(uint32 unit, rtk_l3_pathId_t pathId)
{
    int32   ret = RT_ERR_OK;
    uint32  ecmpIdx;
    uint32  idx;
    uint32  nhIdx;
    dal_mango_l3_ecmpEntry_t ecmpEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pathId=%d", unit, pathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_ENTRY_IDX(pathId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(pathId)), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
    if (!L3_DB_ECMP_IS_VALID(unit, ecmpIdx))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* check reference count */
    if ((_pL3Db[unit]->refer_cnt_chk_en) &&
        (L3_DB_ECMP_REFCNT(unit, ecmpIdx) > 0))
    {
        ret = RT_ERR_ENTRY_REFERRED;
        goto errEntryReferred;
    }

    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryGet, ret);
    for (idx=0; idx<(_pL3Db[unit]->ecmp[ecmpIdx].nh_count); idx++)
    {
        nhIdx = ecmpEntry.nh_idx[idx];

        /* typical nexthop entry */
        if (L3_DB_NH_IS_VALID(unit, nhIdx))
        {
            L3_DB_UPDATE_NH_REFCNT_DEC(unit, nhIdx);
        }
        MANGO_L3_DBG_PRINTF(3, "ecmpEntry.nh_idx[%u] = %u (nhIdx)\n", idx, nhIdx);
    }

    MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->ecmp[ecmpIdx].nh_count = %u)\n", _pL3Db[unit]->ecmp[ecmpIdx].nh_count);

    osal_memset(&ecmpEntry, 0x00, sizeof(dal_mango_l3_ecmpEntry_t));
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_set(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntrySet, ret);

    /* update DB */
    L3_DB_UPDATE_ECMP_INVALID(unit, ecmpIdx);

    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_free(unit, ecmpIdx, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryFree, ret);

errEcmpEntryFree:
errEcmpEntrySet:
errEcmpEntryGet:
errEntryReferred:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_ecmp_destroy */


/* Function Name:
 *      dal_mango_l3_ecmp_get
 * Description:
 *      Get all path IDs of an ECMP path
 * Input:
 *      unit          - unit id
 *      pathId        - ECMP path ID
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
int32
dal_mango_l3_ecmp_get(uint32 unit, rtk_l3_pathId_t pathId, uint32 nhIdArraySize, rtk_l3_pathId_t *pNhIdArray, uint32 *pNhIdCount)
{
    int32   ret = RT_ERR_OK;
    uint32  ecmpIdx;
    dal_mango_l3_ecmpEntry_t ecmpEntry;
    uint32  nhIdCnt;
    uint32  idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pathId=%d", unit, pathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(pathId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(pathId)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNhIdArray), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pNhIdCount), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    if (0 == _pL3Db[unit]->ecmp[ecmpIdx].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* for debugging purpose, we should get from chip */
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryGet, ret);
    nhIdCnt = _pL3Db[unit]->ecmp[ecmpIdx].nh_count;
    for (idx=0; ((idx < nhIdArraySize) && (idx < nhIdCnt)); idx++)
    {
        /* nhIdx to pathId */
        if (_pL3Db[unit]->HW.nexthop[ecmpEntry.nh_idx[idx]].flags & DAL_MANGO_L3_API_FLAG_MOD_MPLS)
        {
            *(pNhIdArray + idx) = DAL_MANGO_L3_NH_IDX_TO_MPLS_PATH_ID(ecmpEntry.nh_idx[idx]);
        } else {
            *(pNhIdArray + idx) = DAL_MANGO_L3_NH_IDX_TO_PATH_ID(ecmpEntry.nh_idx[idx]);
        }

        MANGO_L3_DBG_PRINTF(3, "ecmpEntry.nh_idx[%u] = %u (nhIdx)\n", idx, ecmpEntry.nh_idx[idx]);
    }
    *pNhIdCount = idx;

    MANGO_L3_DBG_PRINTF(3, "*pNhIdCount = %u (idx)\n", idx);

errEcmpEntryGet:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_ecmp_get */


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
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be a null pointer
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 *      RT_ERR_EXCEEDS_CAPACITY - exceed the hardware capacity
 * Note:
 */
int32
dal_mango_l3_ecmp_add(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId)
{
    int32   ret = RT_ERR_OK;
    uint32  ecmpIdx;
    uint32  nhIdx;
    dal_mango_l3_ecmpEntry_t ecmpEntry;
    uint32  nhIdCnt;
    uint32  idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ecmpId=%d,nhId=%d", unit, ecmpId, nhId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(ecmpId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(ecmpId)), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_NH(nhId) && !DAL_MANGO_L3_PATH_ID_IS_MPLS(nhId)), RT_ERR_INPUT);

    /* pre-check input data */
    if (DAL_MANGO_L3_PATH_ID_IS_NH(nhId))
    {
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(nhId);

        if (nhIdx >= DAL_MANGO_L3_NEXTHOP_MAX)
        {
            return RT_ERR_INPUT;
        }

        if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
        {
            return RT_ERR_ENTRY_NOTFOUND;
        }
    }
    else if (DAL_MANGO_L3_PATH_ID_IS_MPLS(nhId))
    {
        //return RT_ERR_CHIP_NOT_SUPPORTED;
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(nhId);

        if (nhIdx >= DAL_MANGO_L3_NEXTHOP_MAX)
        {
            return RT_ERR_INPUT;
        }

        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.nexthop_used, nhIdx))
        {
            return RT_ERR_ENTRY_NOTFOUND;
        }
    }
    else
    {
        return RT_ERR_INPUT;
    }

    /* function body */
    L3_SEM_LOCK(unit);

    if (0 == _pL3Db[unit]->ecmp[ecmpIdx].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* for debugging purpose, we should get from chip */
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryGet, ret);
    nhIdCnt = _pL3Db[unit]->ecmp[ecmpIdx].nh_count;
    if (nhIdCnt < DAL_MANGO_L3_ECMP_NH_CNT_MAX)
    {
        /* pathId to nhIdx */
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(nhId);

        /* typical nexthop entry */
        if (L3_DB_NH_IS_VALID(unit, nhIdx))
        {
            L3_DB_UPDATE_NH_REFCNT_DEC(unit, nhIdx);
        }

        /* append the new pathId */
        ecmpEntry.nh_idx[nhIdCnt] = nhIdx;
        MANGO_L3_DBG_PRINTF(3, "ecmpEntry.nh_idx[%u] = %u (nhIdx)\n", nhIdCnt, ecmpEntry.nh_idx[nhIdCnt]);

        _pL3Db[unit]->ecmp[ecmpIdx].nh_count = (nhIdCnt = (nhIdCnt + 1));
    }
    else
    {
        ret = RT_ERR_EXCEEDS_CAPACITY;
        goto errExceedsCapacity;
    }

    MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->ecmp[ecmpIdx].nh_count = %u\n", _pL3Db[unit]->ecmp[ecmpIdx].nh_count);

    /* rebuild the hash map */
    if (_pL3Db[unit]->ecmp_hash_tbl_manual)
    {
        nhIdCnt = _pL3Db[unit]->ecmp[ecmpIdx].nh_count;
        for (idx=0; idx<DAL_MANGO_L3_ECMP_NH_HASH_SIZE; idx++)
        {
            /* mod N (number of elements) */
            ecmpEntry.hash_to_nh_id[idx] = idx % nhIdCnt;
        }
    }

    /* write back to chip after updating */
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_set(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntrySet, ret);

errEcmpEntrySet:
errExceedsCapacity:
errEcmpEntryGet:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_ecmp_add */


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
 *      RT_ERR_OPER_DENIED    - operation denied (an ECMP group must have at least one nexthop)
 * Note:
 */
int32
dal_mango_l3_ecmp_del(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_pathId_t nhId)
{
    int32   ret = RT_ERR_OK;
    uint32  ecmpIdx;
    dal_mango_l3_ecmpEntry_t ecmpEntry;
    uint32  nhIdCnt;
    uint32  idx;
    uint32  nhIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ecmpId=%d,nhId=%d", unit, ecmpId, nhId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_ENTRY_IDX(ecmpId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(ecmpId)), RT_ERR_INPUT);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_NH(nhId) && !DAL_MANGO_L3_PATH_ID_IS_MPLS(nhId)), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    if (0 == _pL3Db[unit]->ecmp[ecmpIdx].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* pathId to nhIdx */
    nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(nhId);

    /* for debugging purpose, we should get from chip */
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryGet, ret);
    nhIdCnt = _pL3Db[unit]->ecmp[ecmpIdx].nh_count;

    if (1 >= nhIdCnt)
    {
        ret = RT_ERR_OPER_DENIED;
        goto errOperDenied;
    }

    /* try to find out the specified pathId (nhIdx) */
    for (idx=0; idx<nhIdCnt; idx++)
    {
        if (ecmpEntry.nh_idx[idx] == nhIdx)
        {
            /* typical nexthop entry */
            if (L3_DB_NH_IS_VALID(unit, nhIdx))
            {
                L3_DB_UPDATE_NH_REFCNT_DEC(unit, nhIdx);
            }

            while ((idx + 1) < nhIdCnt)
            {
                ecmpEntry.nh_idx[idx] = ecmpEntry.nh_idx[(idx + 1)];
                idx += 1;   /* go to next */

                MANGO_L3_DBG_PRINTF(3, "ecmpEntry.nh_idx[%u] = %u (nhIdx)\n", idx, ecmpEntry.nh_idx[idx]);
            }
            ecmpEntry.nh_idx[idx] = 0;  /* clear */

            _pL3Db[unit]->ecmp[ecmpIdx].nh_count -= 1;
            break;
        }
    }

    if (nhIdCnt == (_pL3Db[unit]->ecmp[ecmpIdx].nh_count)) /* Not found */
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->ecmp[ecmpIdx].nh_count = %u\n", _pL3Db[unit]->ecmp[ecmpIdx].nh_count);

    /* rebuild the hash map */
    nhIdCnt = _pL3Db[unit]->ecmp[ecmpIdx].nh_count;
    if (_pL3Db[unit]->ecmp_hash_tbl_manual)
    {
        for (idx=0; idx<DAL_MANGO_L3_ECMP_NH_HASH_SIZE; idx++)
        {
            /* mod N (number of elements) */
            ecmpEntry.hash_to_nh_id[idx] = idx % nhIdCnt;
        }
    }

    /* write back to chip after updating */
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_set(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntrySet, ret);

errEcmpEntrySet:
errEcmpEntryGet:
errOperDenied:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_ecmp_del */


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
int32
dal_mango_l3_ecmp_find(uint32 unit, uint32 nhIdCount, rtk_l3_pathId_t *pNhIdArray, rtk_l3_pathId_t *pEcmpId)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  ecmpIdx;
    uint32  nhIdx;
    dal_mango_l3_ecmpEntry_t ecmpEntry;
    uint32  nhIdxCnt;
    uint32  nhIdxFoundMask;
    uint32  nhIdxFoundCnt;
    uint32  j;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,nhIdCount=%d,pNhIdArray=%p,pEcmpId=%p", unit, nhIdCount, pNhIdArray, pEcmpId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((0 == nhIdCount), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNhIdArray), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEcmpId), RT_ERR_NULL_POINTER);

    /* pre-check input data */
    for (idx=0; idx<nhIdCount; idx++)
    {
        nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(*(pNhIdArray + idx));

        if ((!DAL_MANGO_L3_PATH_ID_IS_NH(*(pNhIdArray + idx))) || \
            (nhIdx >= DAL_MANGO_L3_NEXTHOP_MAX) || \
            (0 == _pL3Db[unit]->nexthop[nhIdx].valid))
        {
            return RT_ERR_INPUT;
        }
    }

    /* function body */
    L3_SEM_LOCK(unit);

    for (ecmpIdx=0; ecmpIdx<DAL_MANGO_L3_ECMP_MAX; ecmpIdx++)
    {
        if (_pL3Db[unit]->ecmp[ecmpIdx].valid)
        {
            /* get entry */
            RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &ecmpEntry, DAL_MANGO_L3_API_FLAG_NONE), errEcmpEntryGet, ret);
            nhIdxCnt = _pL3Db[unit]->ecmp[ecmpIdx].nh_count;

            /* compare */
            if (nhIdCount != nhIdxCnt)   /* numbers are different */
                continue;

            nhIdxFoundMask = 0;
            nhIdxFoundCnt = 0;
            for (idx=0; idx<nhIdCount; idx++)
            {
                nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(*(pNhIdArray + idx));

                for (j=0; j<nhIdxCnt; j++)
                {
                    if ((0 == (nhIdxFoundMask & (0x1 << j))) && (ecmpEntry.nh_idx[j] == nhIdx))
                    {
                        nhIdxFoundMask |= (0x1 << j);
                        nhIdxFoundCnt++;
                        break;  /* found */
                    }
                }
                if (j >= nhIdxCnt)
                {
                    break;  /* not found */
                }
            }

            if (nhIdxFoundCnt == nhIdxCnt)  /* match */
            {
                *pEcmpId = DAL_MANGO_L3_ECMP_IDX_TO_PATH_ID(ecmpIdx);
                goto errEntryFound;
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;
    goto errEntryNotFound;

errEntryNotFound:
errEntryFound:
errEcmpEntryGet:
    L3_SEM_UNLOCK(unit);

    return ret;

}   /* end of dal_mango_l3_ecmp_find */

#if 0
/* Function Name:
 *      dal_mango_l3_ecmpHashTbl_get
 * Description:
 *      Get the hash table of a specific ECMP group
 * Input:
 *      unit        - unit id
 *      ecmpId      - ECMP path ID
 *      nhIdxCount  - number of indexes of nexthop
 * Output:
 *      pNhIdxArray - pointer to the nexthop indexes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_l3_ecmpHashTbl_get(uint32 unit, rtk_l3_pathId_t ecmpId, uint32 nhIdxCount, uint32 *pNhIdxArray)
{
    int32   ret;
    uint32  ecmpIdx;
    dal_mango_l3_ecmpEntry_t entry;
    uint32  idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ecmpId=%d,nhIdxCount=%d,pNhIdxArray=%p", unit, ecmpId, nhIdxCount, pNhIdxArray);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(ecmpId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(ecmpId)), RT_ERR_INPUT);
    RT_PARAM_CHK((nhIdxCount > HAL_MAX_NUM_OF_L3_ECMP_HASH_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pNhIdxArray), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &entry, 0), errHandle, ret);

    for (idx=0; idx<nhIdxCount; idx++)
    {
        *(pNhIdxArray + idx) = entry.hash_to_nh_id[idx];
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_l3_ecmpHashTbl_get */

/* Function Name:
 *      dal_mango_l3_ecmpHashTbl_set
 * Description:
 *      Set the hash table of a specific ECMP group
 * Input:
 *      unit        - unit id
 *      ecmpId      - ECMP path ID
 *      nhIdxCount  - number of indexes of nexthop
 *      pNhIdxArray - pointer to the nexthop indexes
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_l3_ecmpHashTbl_set(uint32 unit, rtk_l3_pathId_t ecmpId, uint32 nhIdxCount, uint32 *pNhIdxArray)
{
    int32   ret;
    uint32  ecmpIdx;
    dal_mango_l3_ecmpEntry_t entry;
    uint32  idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ecmpId=%d,nhIdxCount=%d,pNhIdxArray=%p", unit, ecmpId, nhIdxCount, pNhIdxArray);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(ecmpId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(ecmpId)), RT_ERR_INPUT);
    RT_PARAM_CHK((nhIdxCount > HAL_MAX_NUM_OF_L3_ECMP_HASH_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_get(unit, ecmpIdx, &entry, 0), errHandle, ret);
    for (idx=0; idx<nhIdxCount; idx++)
    {
        entry.hash_to_nh_id[idx] = (*(pNhIdxArray + idx)) & 0xF;    /* 4-bit field */
    }
    RT_ERR_HDL(_dal_mango_l3_ecmpEntry_set(unit, ecmpIdx, &entry, 0), errHandle, ret);

errHandle:
    L3_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_l3_ecmpHashTbl_set */

/* Function Name:
 *      dal_mango_l3_ecmpCtrl_get
 * Description:
 *      Get the ECMP group's configuration with the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_l3_ecmpCtrl_get(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_ecmpCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  ecmpIdx;
    l3_ecmp_entry_t ecmpEntry;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ecmpId=%d,type=%d,pArg=%p", unit, ecmpId, type, pArg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(ecmpId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(ecmpId)), RT_ERR_INPUT);
    RT_PARAM_CHK((type >= RTK_L3_ECT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);
    L3_INT_SEM_LOCK(unit);  /* to avoid race-condition of L3 interface entry which may be accessed by internal APIs */

    switch (type)
    {
    case RTK_L3_ECT_HASH_METER_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_ECMPt, ecmpIdx, ecmpEntry, \
                MANGO_L3_ECMP_METER_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ECT_HASH_METER_IDX:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_ECMPt, ecmpIdx, ecmpEntry, \
                MANGO_L3_ECMP_METER_IDXtf, val, "", errExit, ret);
            *pArg = (val);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    L3_INT_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_ecmpCtrl_get */

/* Function Name:
 *      dal_mango_l3_ecmpCtrl_set
 * Description:
 *      Set the ECMP group's configuration with the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_l3_ecmpCtrl_set(uint32 unit, rtk_l3_pathId_t ecmpId, rtk_l3_ecmpCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  ecmpIdx;
    l3_ecmp_entry_t ecmpEntry;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ecmpId=%d,type=%d,arg=%x", unit, ecmpId, type, arg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    ecmpIdx = DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(ecmpId);
    RT_PARAM_CHK((ecmpIdx > DAL_MANGO_L3_ECMP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_ECMP(ecmpId)), RT_ERR_INPUT);
    RT_PARAM_CHK((type >= RTK_L3_ECT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);
    L3_INT_SEM_LOCK(unit);  /* to avoid race-condition of L3 interface entry which may be accessed by internal APIs */

    switch (type)
    {
    case RTK_L3_ECT_HASH_METER_EN:
        {
            val = (arg == ENABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_ECMPt, ecmpIdx, ecmpEntry, \
                MANGO_L3_ECMP_METER_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ECT_HASH_METER_IDX:
        {
            val = (arg);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_ECMPt, ecmpIdx, ecmpEntry, \
                MANGO_L3_ECMP_METER_IDXtf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    L3_INT_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_ecmpCtrl_set */
#endif
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
 *      RT_ERR_TBL_FULL           - table is full
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_L3_PATH_ID_INVALID - the path ID (nexthop/ECMP) is invalid
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
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
int32
dal_mango_l3_host_add(uint32 unit, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_hostAlloc_t    hostAlloc;
    dal_mango_l3_hostEntry_t    hostEntry;
    uint32  hostIdx = 0;
    uint32  nhIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pHost=%p", unit, pHost);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!L3_PATH_ID_IS_VALID(unit, pHost->path_id)), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pHost->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
    if (!(pHost->flags & RTK_L3_FLAG_NULL_INTF))
    {
        if (pHost->flags & RTK_L3_FLAG_WITH_NEXTHOP)
        {
            if ((RTK_L3_HOST_ACT_FORWARD == pHost->fwd_act) ||
                (RTK_L3_HOST_ACT_COPY2CPU == pHost->fwd_act))
            {
                RT_ERR_HDL(_dal_mango_l3_nextHopPath_find(unit, &pHost->nexthop, &nhIdx), errHandle, ret);
                pHost->path_id = DAL_MANGO_L3_NH_IDX_TO_PATH_ID(nhIdx);
            }
        }
        else
        {
            /* workable for nexthop and ECMP */
            nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pHost->path_id);
        }

        /* path ID validation */
        if ((DAL_MANGO_L3_PATH_ID_IS_NH(pHost->path_id)) &&
            (DAL_MANGO_L3_RESERVED_NEXTHOP_IDX == nhIdx))
        {
            /* if the path_id = 0, can only set the action is trap/drop */
            if ((RTK_L3_HOST_ACT_TRAP2CPU != pHost->fwd_act) &&
                (RTK_L3_HOST_ACT_DROP != pHost->fwd_act))
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
        }
        else
        {
            if ((DAL_MANGO_L3_PATH_ID_IS_NH(pHost->path_id)) &&
                (0 == _pL3Db[unit]->nexthop[nhIdx].valid))
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
            else if ((DAL_MANGO_L3_PATH_ID_IS_ECMP(pHost->path_id)) &&
                     (0 == _pL3Db[unit]->ecmp[nhIdx].valid))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errEntryNotFound;
            }
        }
    }

    /* hostEntry preparation */
    RT_ERR_HDL(l3_util_rtkHost2hostEntry(&hostEntry, pHost), errHandle, ret);

    /* check if the entry (match key) has been already created in the table */
    if (pHost->flags & RTK_L3_FLAG_REPLACE)
    {
        /* find then update the entry */
        RT_ERR_HDL(__dal_mango_l3_hostEntry_find(unit, pHost, NULL, &hostIdx, NULL), errHandle, ret);

        /* update L3DB */
        if (_pL3Db[unit]->host[hostIdx].shadow.path_id != pHost->path_id)
        {
            /* old path id */
            if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->host[hostIdx].shadow.path_id))
            {
                L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                    DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
            }
            else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->host[hostIdx].shadow.path_id))
            {
                L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                    DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
            }

            /* update shadow */
            _pL3Db[unit]->host[hostIdx].shadow.path_id = pHost->path_id;

            /* new path id */
            if (DAL_MANGO_L3_PATH_ID_IS_NH(pHost->path_id))
            {
                L3_DB_UPDATE_NH_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pHost->path_id));
            }
            else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(pHost->path_id))
            {
                L3_DB_UPDATE_ECMP_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(pHost->path_id));
            }
        }
    }
    else
    {
        /* check existence */
        ret = __dal_mango_l3_hostEntry_find(unit, pHost, &hostAlloc, &hostIdx, NULL);
        if (RT_ERR_OK == ret)
        {
            ret = RT_ERR_ENTRY_EXIST;
            goto errHandle;
        }
        else if (RT_ERR_ENTRY_NOTFOUND != ret)
        {
            goto errHandle;
        }

        /* allocate L3 host entry */
        RT_ERR_HDL(_dal_mango_l3_hostEntry_alloc(unit, &hostAlloc, &hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

        /* update L3DB */
        _pL3Db[unit]->host[hostIdx].shadow.path_id = pHost->path_id;
        if (DAL_MANGO_L3_PATH_ID_IS_NH(pHost->path_id))
        {
            L3_DB_UPDATE_NH_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pHost->path_id));
        }
        else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(pHost->path_id))
        {
            L3_DB_UPDATE_ECMP_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(pHost->path_id));
        }
        _pL3Db[unit]->host[hostIdx].ipv6 = (pHost->flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;
        L3_DB_UPDATE_HOST_VALID(unit, hostIdx);
    }

    /* write into chip */
    RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

errEntryNotFound:
errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_add */


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
int32
dal_mango_l3_host_del(uint32 unit, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    uint32  hostIdx = 0;
    dal_mango_l3_hostEntry_t    hostEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pHost->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    ret = __dal_mango_l3_hostEntry_find(unit, pHost, NULL, &hostIdx, &hostEntry);
    if (RT_ERR_OK == ret)
    {
        /* write into chip */
        osal_memset(&hostEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
        RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

        /* release L3 host entry */
        RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

        /* update L3 DB */
        if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->host[hostIdx].shadow.path_id))
        {
            L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
        }
        else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->host[hostIdx].shadow.path_id))
        {
            L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
        }
        _pL3Db[unit]->host[hostIdx].shadow.path_id = DAL_MANGO_L3_RESERVED_NEXTHOP_IDX;
        L3_DB_UPDATE_HOST_INVALID(unit, hostIdx);

        MANGO_L3_DBG_PRINTF(3, "hostIdx = %u\n", hostIdx);

        goto errOk;
    }

    ret = RT_ERR_ENTRY_NOTFOUND;
    goto errEntryNotFound;

errEntryNotFound:
errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_del */


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
 */
int32
dal_mango_l3_host_del_byNetwork(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    dal_mango_l3_hostEntry_t    hostEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRoute->flags & RTK_L3_FLAG_IPV6) && (pRoute->prefix_len > 128), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!(pRoute->flags & RTK_L3_FLAG_IPV6) && (pRoute->prefix_len > 32), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pRoute->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (pRoute->flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_MANGO_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            if ((pRoute->vrf_id == hostEntry.vrf_id) && \
                (((FALSE == ipv6) && (pRoute->prefix_len == rt_util_ipMaxMatchLength_ret(hostEntry.ip, pRoute->ip_addr.ipv4, pRoute->prefix_len))) || \
                 ((TRUE == ipv6) && (pRoute->prefix_len == rt_util_ipv6MaxMatchLength_ret(&hostEntry.ip6, &pRoute->ip_addr.ipv6, pRoute->prefix_len)))))
            {
                /* write into chip */
                osal_memset(&hostEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
                RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                /* release L3 host entry */
                RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                /* update L3 DB */
                if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->host[hostIdx].shadow.path_id))
                {
                    L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                        DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
                }
                else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->host[hostIdx].shadow.path_id))
                {
                    L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                        DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
                }
                _pL3Db[unit]->host[hostIdx].shadow.path_id = DAL_MANGO_L3_RESERVED_NEXTHOP_IDX;
                L3_DB_UPDATE_HOST_INVALID(unit, hostIdx);

                MANGO_L3_DBG_PRINTF(3, "hostIdx = %u\n", hostIdx);
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_del_byNetwork */


/* Function Name:
 *      dal_mango_l3_host_del_byIntfId
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
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_L3_FLAG_NEGATE  - to indicate the action is applied to the unmatched entries
 *      (2) Not including ECMP-type host entries
 */
int32
dal_mango_l3_host_del_byIntfId(uint32 unit, rtk_intf_id_t intfId, rtk_l3_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6, negate;
    uint32  hostIdx;
    dal_mango_l3_hostEntry_t    hostEntry;
    uint32  nhIdx, intfIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((DAL_MANGO_L3_INTF_ID_IS_TUNNEL(intfId)), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;
    negate = (flags & RTK_L3_FLAG_NEGATE)? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_MANGO_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            /* currently, only remove 'nexthop' type entry */
            if (0 == hostEntry.ecmp_en)
            {
                nhIdx = hostEntry.nh_ecmp_idx;
                intfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;

                /* confirm all info */
                if ((_pL3Db[unit]->nexthop[nhIdx].valid) && \
                    (((_pL3Db[unit]->intf[intfIdx].valid) && (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[unit]->intf[intfIdx].intf_id)))) && \
                    (((FALSE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id == intfId)) || \
                     ((TRUE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id != intfId))))
                {
                    /* write into chip */
                    osal_memset(&hostEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
                    RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                    /* release L3 host entry */
                    RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                    /* update L3 DB */
                    if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->host[hostIdx].shadow.path_id))
                    {
                        L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
                    }
                    else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->host[hostIdx].shadow.path_id))
                    {
                        L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
                    }
                    _pL3Db[unit]->host[hostIdx].shadow.path_id = DAL_MANGO_L3_RESERVED_NEXTHOP_IDX;
                    L3_DB_UPDATE_HOST_INVALID(unit, hostIdx);

                    MANGO_L3_DBG_PRINTF(3, "hostIdx = %u (intfId = %u, remove)\n", hostIdx, intfId);
                }
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_del_byIntfId */


/* Function Name:
 *      dal_mango_l3_host_delAll
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
int32
dal_mango_l3_host_delAll(uint32 unit, rtk_l3_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    dal_mango_l3_hostEntry_t    hostEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x", unit, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    osal_memset(&hostEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_MANGO_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            /* write into chip */
            RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            /* release L3 host entry */
            RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            /* update L3 DB */
            if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->host[hostIdx].shadow.path_id))
            {
                L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                    DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
            }
            else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->host[hostIdx].shadow.path_id))
            {
                L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                    DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
            }
            _pL3Db[unit]->host[hostIdx].shadow.path_id = DAL_MANGO_L3_RESERVED_NEXTHOP_IDX;
            L3_DB_UPDATE_HOST_INVALID(unit, hostIdx);

            MANGO_L3_DBG_PRINTF(3, "hostIdx = %u\n", hostIdx);
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_delAll */


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
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id and ip_addr (either ipv4 or ipv6).
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
int32
dal_mango_l3_host_find(uint32 unit, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    uint32  hostIdx = 0;
    dal_mango_l3_hostEntry_t    hostEntry;
    rtk_l3_host_t   rtkHost;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pHost=%p", unit, pHost);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pHost->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    /* find the entry */
    RT_ERR_HDL(__dal_mango_l3_hostEntry_find(unit, pHost, NULL, &hostIdx, &hostEntry), errHandle, ret);
    RT_ERR_HDL(l3_util_hostEntry2rtkHost(&rtkHost, &hostEntry), errHandle, ret);

    /* optional operation */
    if ((pHost->flags & RTK_L3_FLAG_HIT_CLEAR) && (hostEntry.hit))
    {
        hostEntry.hit = 0;  /* clear hit bit */
        RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
    }

    /* return the data */
    *pHost = rtkHost;

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_find */


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
int32
dal_mango_l3_hostConflict_get(uint32 unit, rtk_l3_key_t *pKey, rtk_l3_host_t *pHostArray, int32 maxHost, int32 *pHostCount)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_hostHashKey_t  hashKey;
    dal_mango_l3_hostHashIdx_t  hashIdx;
    dal_mango_l3_hostEntry_t    hostEntry;
    uint32  ipv6, flags, entryWidth;
    uint32  tbl, slot, addr;
    uint32  hostIdx;
    uint32  numHost;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pKey=%p,pHostArray=%p,maxHost=%d,pHostCount=%p", unit, pKey, pHostArray, maxHost, pHostCount);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHostArray), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHostCount), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 >= maxHost), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    /* get hash index */
    osal_memset(&hashKey, 0x00, sizeof(dal_mango_l3_hostHashKey_t));
    L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_UC_HASH_ALG_SEL_0f, hashKey.alg_of_tbl[0], "", errHandle, ret);
    L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_UC_HASH_ALG_SEL_1f, hashKey.alg_of_tbl[1], "", errHandle, ret);
    hashKey.vrf_id = pKey->vrf_id;
    if (pKey->flags & RTK_L3_FLAG_IPV6)
    {
        ipv6 = TRUE;
        hashKey.dip.ipv6 = pKey->ip_addr.ipv6;
        flags = DAL_MANGO_L3_API_FLAG_IPV6;
        entryWidth = 3;
    }
    else
    {
        ipv6 = FALSE;
        hashKey.dip.ipv4 = pKey->ip_addr.ipv4;
        flags = DAL_MANGO_L3_API_FLAG_NONE;
        entryWidth = 1;
    }
    RT_ERR_HDL(_dal_mango_l3_hostHashIdx_get(unit, &hashKey, &hashIdx, flags), errHandle, ret);

    /* search the entries which have the same hash-index */
    numHost = 0;
    for (tbl=0; tbl<DAL_MANGO_L3_HOST_TBL_NUM; tbl++)
    {
        for (slot=0; slot<DAL_MANGO_L3_HOST_TBL_WIDTH; slot+=entryWidth)
        {
            addr = ((tbl & 0x1) << 13) | ((hashIdx.idx_of_tbl[tbl] & 0x3FF) << 3) | ((slot & 0x7) << 0);
            hostIdx = DAL_MANGO_L3_ENTRY_ADDR_TO_IDX(addr);

            if (_pL3Db[unit]->host[hostIdx].valid)
            {
                RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, hostIdx, &hostEntry, flags), errHandle, ret);

                /* ignore itself */
                if ((pKey->vrf_id == hostEntry.vrf_id) && \
                    (((FALSE == ipv6) && \
                      (MANGO_L3_HOST_IP_PFLEN == rt_util_ipMaxMatchLength_ret(hostEntry.ip, pKey->ip_addr.ipv4, MANGO_L3_HOST_IP_PFLEN))) || \
                     ((TRUE == ipv6) && \
                      (MANGO_L3_HOST_IP6_PFLEN == rt_util_ipv6MaxMatchLength_ret(&hostEntry.ip6, &pKey->ip_addr.ipv6, MANGO_L3_HOST_IP6_PFLEN)))))
                {
                    continue;   /* try to find the next */
                }

                RT_ERR_HDL(l3_util_hostEntry2rtkHost((pHostArray + numHost), &hostEntry), errHandle, ret);

                numHost += 1;
                if (numHost >= maxHost)
                    break;
            }
        }
        if (numHost >= maxHost)
            break;
    }

    *pHostCount = numHost;

    MANGO_L3_DBG_PRINTF(3, "numHost = %u\n", numHost);
    goto errOk;

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_hostConflict_get */


/* Function Name:
 *      dal_mango_l3_host_age
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
int32
dal_mango_l3_host_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_hostTraverseCallback_f fAge, void *pCookie)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    dal_mango_l3_hostEntry_t    hostEntry;
    rtk_l3_host_t rtkHost;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x,fAge=%p,pCookie=%p", unit, flags, fAge, pCookie);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((NULL == fAge) && (NULL != pCookie)), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_MANGO_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            if (flags & RTK_L3_FLAG_HIT)
            {
                /* age out entry if hit-bit is set */
                if (hostEntry.hit)
                {
                    /* callback function */
                    if (NULL != fAge)
                    {
                        RT_ERR_HDL(l3_util_hostEntry2rtkHost(&rtkHost, &hostEntry), errHandle, ret);

                        fAge(unit, hostIdx, &rtkHost, pCookie);
                    }

                    /* clear hit bit */
                    hostEntry.hit = 0;
                    RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                    MANGO_L3_DBG_PRINTF(3, "hostIdx = %u (hit=1, clear hit bit)\n", hostIdx);
                }
            }
            else
            {
                /* delete the entry since its hit bit is clear */
                if (!hostEntry.hit)
                {
                    /* write into chip */
                    osal_memset(&hostEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
                    RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                    /* release L3 host entry */
                    RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                    /* update L3 DB */
                    if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->host[hostIdx].shadow.path_id))
                    {
                        L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
                    }
                    else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->host[hostIdx].shadow.path_id))
                    {
                        L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->host[hostIdx].shadow.path_id));
                    }
                    _pL3Db[unit]->host[hostIdx].shadow.path_id = DAL_MANGO_L3_RESERVED_NEXTHOP_IDX;
                    L3_DB_UPDATE_HOST_INVALID(unit, hostIdx);

                    /* callback function */
                    if (NULL != fAge)
                    {
                        RT_ERR_HDL(l3_util_hostEntry2rtkHost(&rtkHost, &hostEntry), errHandle, ret);

                        fAge(unit, hostIdx, &rtkHost, pCookie);
                    }

                    MANGO_L3_DBG_PRINTF(3, "hostIdx = %u (hit=0, delete)\n", hostIdx);
                }
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_age */


/* Function Name:
 *      dal_mango_l3_host_getNext
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
int32
dal_mango_l3_host_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    dal_mango_l3_hostEntry_t    hostEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x,pBase=%p,pHost=%p", unit, flags, pBase, pHost);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (hostIdx=(*pBase<0)?(0):((*pBase)+1); hostIdx<DAL_MANGO_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            MANGO_L3_DBG_PRINTF(3, "hostIdx = %u\n", hostIdx);

            RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, hostIdx, &hostEntry, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
            RT_ERR_HDL(l3_util_hostEntry2rtkHost(pHost, &hostEntry), errHandle, ret);

            *pBase = hostIdx;

            goto errOk;
        }
    }
    *pBase = -1;    /* Not found any specified entry */

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_host_getNext */

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
 *      RT_ERR_TBL_FULL           - table is full
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
 *      (3) when pRoute->path_id = 0, the pRoute->fwd_act can only be RTK_L3_ROUTE_ACT_DROP
 *          or RTK_L3_ROUTE_ACT_TRAP2CPU
 */
int32
dal_mango_l3_route_add(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_routeEntry_t   routeEntry;
    uint32  ipv6;
    uint32  routeIdx;
    uint32  nhIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRoute->flags & RTK_L3_FLAG_IPV6) && (pRoute->prefix_len > 128), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!(pRoute->flags & RTK_L3_FLAG_IPV6) && (pRoute->prefix_len > 32), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((!L3_PATH_ID_IS_VALID(unit, pRoute->path_id)), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pRoute->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
    if (!(pRoute->flags & RTK_L3_FLAG_NULL_INTF))
    {
        if (pRoute->flags & RTK_L3_FLAG_WITH_NEXTHOP)
        {
            if ((RTK_L3_ROUTE_ACT_FORWARD == pRoute->fwd_act) ||
                (RTK_L3_ROUTE_ACT_COPY2CPU == pRoute->fwd_act))
            {
                RT_ERR_HDL(_dal_mango_l3_nextHopPath_find(unit, &pRoute->nexthop, &nhIdx), errHandle, ret);
                pRoute->path_id = DAL_MANGO_L3_NH_IDX_TO_PATH_ID(nhIdx);
            }
        }
        else
        {
            /* workable for nexthop and ECMP */
            nhIdx = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pRoute->path_id);
        }

        /* path ID validation */
        if ((DAL_MANGO_L3_PATH_ID_IS_NH(pRoute->path_id)) &&
            (DAL_MANGO_L3_RESERVED_NEXTHOP_IDX == nhIdx))
        {
            /* if the path_id = 0, can only set the action is trap/drop */
            if ((RTK_L3_ROUTE_ACT_TRAP2CPU != pRoute->fwd_act) &&
                (RTK_L3_ROUTE_ACT_DROP != pRoute->fwd_act))
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
        }
        else
        {
            if ((DAL_MANGO_L3_PATH_ID_IS_NH(pRoute->path_id)) &&
                (0 == _pL3Db[unit]->nexthop[nhIdx].valid))
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
            else if ((DAL_MANGO_L3_PATH_ID_IS_ECMP(pRoute->path_id)) &&
                     (0 == _pL3Db[unit]->ecmp[nhIdx].valid))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errEntryNotFound;
            }
        }
    }

    ipv6 = (pRoute->flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    /* check the action (update or create) */
    if (pRoute->flags & RTK_L3_FLAG_REPLACE)
    {
        /* find the entry first */
        if (RT_ERR_OK != __dal_mango_l3_routeEntry_find(unit, pRoute, &routeIdx, &routeEntry))
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errHandle;
        }

        /* update L3DB */
        if (_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id != pRoute->path_id)
        {
            /* old path id */
            if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
            {
                L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                    DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
            }
            else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
            {
                L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                    DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
            }

            /* new path id */
            if (DAL_MANGO_L3_PATH_ID_IS_NH(pRoute->path_id))
            {
                L3_DB_UPDATE_NH_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pRoute->path_id));
            }
            else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(pRoute->path_id))
            {
                L3_DB_UPDATE_ECMP_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(pRoute->path_id));
            }
        }
        _pL3Db[unit]->route[routeIdx].shadow.routeEntry = *pRoute;

        MANGO_L3_DBG_PRINTF(3, "(UPDATE: found routeIdx = %u\n", routeIdx);
    }
    else
    {
        if (RT_ERR_OK == __dal_mango_l3_routeEntry_find(unit, pRoute, &routeIdx, &routeEntry))
        {
            ret = RT_ERR_ENTRY_EXIST;
            goto errHandle;
        }

        /* allocate L3 route entry */
        MANGO_L3_DBG_PRINTF(3, "ipv6 = %d, prefix_len=%d\n", ipv6, pRoute->prefix_len);
        RT_ERR_HDL(__dal_mango_l3_routeEntry_alloc(unit, ipv6, pRoute->prefix_len, &routeIdx), errHandle, ret);

        /* update L3DB */
        if (DAL_MANGO_L3_PATH_ID_IS_NH(pRoute->path_id))
        {
            L3_DB_UPDATE_NH_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pRoute->path_id));
        }
        else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(pRoute->path_id))
        {
            L3_DB_UPDATE_ECMP_REFCNT_INC(unit, DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(pRoute->path_id));
        }
        _pL3Db[unit]->route[routeIdx].shadow.routeEntry = *pRoute;
        _pL3Db[unit]->route[routeIdx].ipv6 = ipv6;
        L3_DB_UPDATE_ROUTE_VALID(unit, routeIdx);
    }

    /* routeEntry preparation */
    RT_ERR_HDL(l3_util_rtkRoute2routeEntry(&routeEntry, pRoute), errHandle, ret);

    /* write data into chip */
    RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);

    MANGO_L3_DBG_PRINTF(3, "routeIdx = %u\n", routeIdx);

errEntryNotFound:
errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_add */


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
int32
dal_mango_l3_route_del(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    uint32  routeIdx;
    dal_mango_l3_routeEntry_t routeEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRoute->flags & RTK_L3_FLAG_IPV6) && (pRoute->prefix_len > 128), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!(pRoute->flags & RTK_L3_FLAG_IPV6) && (pRoute->prefix_len > 32), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pRoute->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    /* find the entry */
    if (RT_ERR_OK == __dal_mango_l3_routeEntry_find(unit, pRoute, &routeIdx, &routeEntry))
    {
        /* update L3 DB (before releasing the entry) */
        if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
        {
            L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
        }
        else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
        {
            L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
        }

        /* delete the found entry */
        RT_ERR_HDL(__dal_mango_l3_routeEntry_free(unit, routeIdx), errHandle, ret);

        goto errOk;
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_del */


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
 *          RTK_L3_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
int32
dal_mango_l3_route_get(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    uint32  routeIdx;
    dal_mango_l3_routeEntry_t routeEntry;
    rtk_l3_route_t  rtkRoute;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRoute->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    /* find the entry */
    if (RT_ERR_OK == __dal_mango_l3_routeEntry_find(unit, pRoute, &routeIdx, &routeEntry))
    {
        /* entry found */
        RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);

        /* optional operation */
        if (pRoute->flags & RTK_L3_FLAG_HIT_CLEAR)
        {
            if (routeEntry.hit)
            {
                routeEntry.hit = 0;  /* clear hit bit */
                RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);
            }
        }

        /* return the data */
        *pRoute = rtkRoute;

        MANGO_L3_DBG_PRINTF(3, "routeIdx = %u\n", routeIdx);

        goto errOk;

    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_get */


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
int32
dal_mango_l3_route_del_byIntfId(uint32 unit, rtk_l3_flag_t flags, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;
    uint32  negate;
    uint32  baseIdx, length, step;
    uint32  routeIdx;
    dal_mango_l3_routeEntry_t   routeEntry;
    uint32  nhIdx, intfIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((DAL_MANGO_L3_INTF_ID_IS_TUNNEL(intfId)), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 (scan from top to bottom) */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        baseIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#else
        baseIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#endif
        length = 3 * L3_ROUTE_TBL_IP6_CNT(unit);
        step = 3;
    } else {
        /* IPv4 (scan from bottom to top) */
        baseIdx = L3_ROUTE_TBL_IP_CNT(unit) - 1;
        length = L3_ROUTE_TBL_IP_CNT(unit);
        step = -1;
    }

    negate = (flags & RTK_L3_FLAG_NEGATE)? TRUE : FALSE;

    /* scan the whole L3 route table */
    for (routeIdx = baseIdx; L3_ABS(routeIdx, baseIdx) < length; routeIdx += step)
    {
        RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);
        MANGO_L3_DBG_PRINTF(3, "routeIdx = %u (intfId = %u, remove)\n", routeIdx, intfId);

        /* currently, only remove 'nexthop' type entry */
        if (0 == routeEntry.ecmp_en)
        {
            nhIdx = routeEntry.nh_ecmp_idx;
            intfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;
            MANGO_L3_DBG_PRINTF(3, "routeIdx = %u (intfId = %u, remove)\n", routeIdx, intfId);
            MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->nexthop[nhIdx].valid = %u\n", _pL3Db[unit]->nexthop[nhIdx].valid);
            MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->intf[intfIdx].valid = %u\n", _pL3Db[unit]->intf[intfIdx].valid);
            MANGO_L3_DBG_PRINTF(3, "_pL3Db[unit]->intf[intfIdx].intf_id = %u\n", _pL3Db[unit]->intf[intfIdx].intf_id);

            /* confirm all info */
            if ((_pL3Db[unit]->nexthop[nhIdx].valid) && \
                (((_pL3Db[unit]->intf[intfIdx].valid) && (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[unit]->intf[intfIdx].intf_id)))) && \
                (((FALSE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id == intfId)) || \
                 ((TRUE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id != intfId))))
            {
                MANGO_L3_DBG_PRINTF(3, "routeIdx = %u (intfId = %u, remove)\n", routeIdx, intfId);

                /* update L3 DB (before releasing the entry) */
                if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                {
                    L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                        DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                }
                else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                {
                    L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                        DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                }

                /* release L3 route entry */
                RT_ERR_HDL(__dal_mango_l3_routeEntry_free(unit, routeIdx), errHandle, ret);

                MANGO_L3_DBG_PRINTF(3, "routeIdx = %u (intfId = %u, remove)\n", routeIdx, intfId);
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_del_byIntfId */


/* Function Name:
 *      dal_mango_l3_route_delAll
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
int32
dal_mango_l3_route_delAll(uint32 unit, rtk_l3_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  prefixLen, index, length;
    uint32  routeIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x", unit, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 */

        /* clear H/W entries */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        index = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#else
        index = L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#endif
        length = 3 * L3_ROUTE_TBL_IP6_CNT(unit);

        if (length > 0)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, index, length), errHandle, ret);

            /* clear DB */
            for (prefixLen = 0; prefixLen < MANGO_L3_HOST_IP6_PFLEN; prefixLen++)
            {
                L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, prefixLen) = 0;
            }
            L3_ROUTE_TBL_IP6_CNT(unit) = 0;

            /* update L3 DB */
            for (routeIdx=index; routeIdx < (index + length); routeIdx+=3)
            {
                if (_pL3Db[unit]->route[routeIdx].valid)
                {
                    if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                    {
                        L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                    }
                    else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                    {
                        L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                    }

                    L3_DB_UPDATE_ROUTE_INVALID(unit, routeIdx);
                }
            }
        }
    } else {
        /* IPv4 */

        /* clear H/W entries */
        index = 0;
        length = L3_ROUTE_TBL_IP_CNT(unit);

        if (length > 0)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_clear(unit, index, length), errHandle, ret);

            /* clear DB */
            for (prefixLen = 0; prefixLen < MANGO_L3_HOST_IP_PFLEN; prefixLen++)
            {
                L3_ROUTE_TBL_IP_CNT_PFLEN(unit, prefixLen) = 0;
            }
            L3_ROUTE_TBL_IP_CNT(unit) = 0;

            /* update L3 DB */
            for (routeIdx = index; routeIdx < (index + length); routeIdx++)
            {
                if (_pL3Db[unit]->route[routeIdx].valid)
                {
                    if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                    {
                        L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                    }
                    else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                    {
                        L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                            DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                    }

                    L3_DB_UPDATE_ROUTE_INVALID(unit, routeIdx);
                }
            }
        }
    }

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    L3_ROUTE_TBL_BOUNDARY_SYNC(unit);
#endif

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_delAll */


/* Function Name:
 *      dal_mango_l3_route_age
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
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_HIT     - if set, then clear the hit bits of the traversed entries when
 *                                their hit bits are set.
 *                                Otherwise, delete the unused entries which its hit bit is clear.
 */
int32
dal_mango_l3_route_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_routeTraverseCallback_f fAge, void *pCookie)
{
    int32   ret = RT_ERR_OK;
    uint32  baseIdx, length, step;
    uint32  routeIdx;
    dal_mango_l3_routeEntry_t   routeEntry;
    rtk_l3_route_t rtkRoute;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x,fAge=%p,pCookie=%p", unit, flags, fAge, pCookie);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((NULL == fAge) && (NULL != pCookie)), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 (scan from top to bottom) */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        baseIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#else
        baseIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#endif
        length = 3 * L3_ROUTE_TBL_IP6_CNT(unit);
        step = 3;
    } else {
        /* IPv4 (scan from bottom to top) */
        baseIdx = L3_ROUTE_TBL_IP_CNT(unit) - 1;
        length = L3_ROUTE_TBL_IP_CNT(unit);
        step = -1;
    }

    for (routeIdx = baseIdx; L3_ABS(routeIdx, baseIdx) < length; routeIdx += step)
    {
        RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);

        if (flags & RTK_L3_FLAG_HIT)
        {
            /* age out entry if hit-bit is set */
            if (routeEntry.hit)
            {
                /* callback function */
                if (NULL != fAge)
                {
                    RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);

                    fAge(unit, routeIdx, &rtkRoute, pCookie);
                }

                /* clear hit bit */
                routeEntry.hit = 0;
                RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);

                MANGO_L3_DBG_PRINTF(3, "routeIdx = %u (hit=1, clear hit bit)\n", routeIdx);
            }
        }
        else
        {
            /* delete the entry since its hit bit is clear */
            if (!routeEntry.hit)
            {
                /* release L3 route entry */
                RT_ERR_HDL(__dal_mango_l3_routeEntry_free(unit, routeIdx), errHandle, ret);

                if (DAL_MANGO_L3_PATH_ID_IS_NH(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                {
                    L3_DB_UPDATE_NH_REFCNT_DEC(unit,
                        DAL_MANGO_L3_PATH_ID_TO_NH_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                }
                else if (DAL_MANGO_L3_PATH_ID_IS_ECMP(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id))
                {
                    L3_DB_UPDATE_ECMP_REFCNT_DEC(unit,
                        DAL_MANGO_L3_PATH_ID_TO_ECMP_IDX(_pL3Db[unit]->route[routeIdx].shadow.routeEntry.path_id));
                }

                L3_DB_UPDATE_ROUTE_INVALID(unit, routeIdx);

                /* callback function */
                if (NULL != fAge)
                {
                    RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);

                    fAge(unit, routeIdx, &rtkRoute, pCookie);
                }

                MANGO_L3_DBG_PRINTF(3, "routeIdx = %u (hit=0, delete)\n", routeIdx);
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_age */


/* Function Name:
 *      dal_mango_l3_route_getNext
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
int32
dal_mango_l3_route_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    int32   baseIdx, length, width;
    uint32  routeIdx;
    dal_mango_l3_routeEntry_t   routeEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x,pBase=%p,pRoute=%p", unit, flags, pBase, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 */
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        baseIdx = L3_ROUTE_TBL_IP6UC_MAX(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#else
        baseIdx = L3_ROUTE_TBL_SIZE(unit) - (3 * L3_ROUTE_TBL_IP6_CNT(unit));
#endif
        length = 3 * L3_ROUTE_TBL_IP6_CNT(unit);
        width = 3;
    } else {
        /* IPv4 */
        baseIdx = 0;
        length = L3_ROUTE_TBL_IP_CNT(unit);
        width = 1;
    }

    /* scan the whole L3 route table */
    for (routeIdx=(*pBase<baseIdx)?(baseIdx):(((*pBase)+width) - (((*pBase)+width)%width)); (routeIdx - baseIdx) < length; routeIdx += width)
    {
        MANGO_L3_DBG_PRINTF(3, "routeIdx = %u\n", routeIdx);

        RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);
        if (0 == routeEntry.valid)
            continue;

        RT_ERR_HDL(l3_util_routeEntry2rtkRoute(pRoute, &routeEntry), errHandle, ret);

        *pBase = routeIdx;

        goto errOk;
    }

    *pBase = -1;    /* Not found any specified entry */

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_getNext */

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
int32
dal_mango_l3_route_rsv4Mcast_add(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_routeEntry_t   routeEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
#if MANGO_L3_ROUTE_HW_LU
    if (RT_ERR_OK != __dal_mango_l3_routeEntry_get(unit, (DAL_MANGO_L3_ROUTE_TBL_RSV_V4_MIN_IDX), &routeEntry, DISABLED))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errHandle;
    }
#endif

    /* routeEntry preparation */
    /* 224.0.0.0 to 239.255.255.255 */
    osal_memset(&routeEntry, 0x00, sizeof(dal_mango_l3_routeEntry_t));
    routeEntry.valid = 1;
    routeEntry.fmt = 0;
    routeEntry.bmsk_fmt = 0x1;
    routeEntry.entry_type = DAL_MANGO_L3_ENTRY_TYPE_IPV4_MULTICAST;
    routeEntry.bmsk_entry_type = 0x3;
    routeEntry.ipmc_type = 1;
    routeEntry.bmsk_ipmc_type = 0x1;
    routeEntry.mc_key_sel = 0;
    routeEntry.bmsk_mc_key_sel = 0x1;
    routeEntry.l2_en = 0;
    routeEntry.l3_en = 1;
    routeEntry.l3_act = 0;
    routeEntry.rpf_chk_en = 1;
    routeEntry.rpf_fail_act = 3;
    routeEntry.ttl_min = 0;
    routeEntry.mtu_max = 1500;
    routeEntry.gip = 0xE0000000;
    routeEntry.bmsk_gip = 0xF0000000;
    routeEntry.round = 1;
    routeEntry.bmsk_round = 0x1;
    routeEntry.l3_rpf_id = MANGO_L3_RSV_RPF_ID;

    /* write data into chip */
    RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, DAL_MANGO_L3_ROUTE_TBL_RSV_V4_MIN_IDX, &routeEntry, DISABLED), errHandle, ret);
    MANGO_L3_DBG_PRINTF(3, "routeIdx = %u\n", DAL_MANGO_L3_ROUTE_TBL_RSV_V4_MIN_IDX);

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_add */

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
int32
dal_mango_l3_route_rsv6Mcast_add(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_routeEntry_t   routeEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
#if MANGO_L3_ROUTE_HW_LU
    if (RT_ERR_OK != __dal_mango_l3_routeEntry_get(unit, (DAL_MANGO_L3_ROUTE_TBL_RSV_V6_MIN_IDX), &routeEntry, DISABLED))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errHandle;
    }
#endif

    /* routeEntry preparation */
    /* ff00::/8 */
    osal_memset(&routeEntry, 0x00, sizeof(dal_mango_l3_routeEntry_t));
    routeEntry.valid = 1;
    routeEntry.entry_type = DAL_MANGO_L3_ENTRY_TYPE_IPV6_MULTICAST;
    routeEntry.bmsk_entry_type = 0x3;

    routeEntry.l3_en = 1;

    routeEntry.fmt = 0;
    routeEntry.bmsk_fmt = 0x1;

    routeEntry.ipmc_type = 1;
    routeEntry.bmsk_ipmc_type = 0x1;
    routeEntry.mc_key_sel = 0;
    routeEntry.bmsk_mc_key_sel = 0x1;
    routeEntry.l2_en = 0;

    routeEntry.l3_act = 0;
    routeEntry.rpf_chk_en = 1;
    routeEntry.rpf_fail_act = 3;
    routeEntry.ttl_min = 0;
    routeEntry.mtu_max = 1500;

    routeEntry.gip6.octet[0] = 0xFF;
    routeEntry.bmsk_gip6.octet[0] = 0xFF;

    routeEntry.round = 1;
    routeEntry.bmsk_round = 0x1;
    routeEntry.l3_rpf_id = MANGO_L3_RSV_RPF_ID;

    /* write data into chip */
    RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, DAL_MANGO_L3_ROUTE_TBL_RSV_V6_MIN_IDX, &routeEntry, DISABLED), errHandle, ret);
    MANGO_L3_DBG_PRINTF(3, "routeIdx = %u\n", DAL_MANGO_L3_ROUTE_TBL_RSV_V6_MIN_IDX);

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_route_add */
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
int32
_dal_mango_l3_route_ipmcSize_get(uint32 unit, uint32 *pBase, uint32 *pSize)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSize), RT_ERR_NULL_POINTER);

    *pBase = _pL3Db[unit]->l3_route_ipmc_idx_base;
    *pSize = _pL3Db[unit]->l3_route_ipmc_size;

    return ret;
}
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
int32
dal_mango_l3_globalCtrl_get(uint32 unit, rtk_l3_globalCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_l3_act_t act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d,pArg=%p", unit, type, pArg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_L3_GCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_L3_GCT_IP_HDR_ERR_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_IP_HDR_ERR_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlIpHdrErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_GLB_EN:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_DIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_BAD_DIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlBadDip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_ZERO_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_BC_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_DMAC_BC_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlDmacBc, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_MC_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_DMAC_MC_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlDmacMc, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_HDR_OPT_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_HDR_OPT_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlHdrOpt, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_MTU_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_TTL_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlTtlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_PKT_TO_CPU_TARGET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_optIpucRouteCtrlPktToCpuTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6_HDR_ERR_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_IP6_HDR_ERR_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlIp6HdrErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_GLB_EN:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_DIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_BAD_DIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlBadDip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_ZERO_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlDmacMismatch, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HBH_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHbh, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ERR_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HBH_ERR_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHbhErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HDR_ROUTE_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHdrRoute, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_MTU_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HL_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HL_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_PKT_TO_CPU_TARGET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_optIp6ucRouteCtrlPktToCpuTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_KEY:
        {
            uint32 hash_key = 0;
            uint32 rx_port_inc;
            uint32 trunk_id_inc;
            uint32 sip_inc;
            uint32 dip_inc;
            uint32 dscp_inc;
            uint32 ip_proto_inc;
            uint32 ip6_label_inc;
            uint32 l4_sport_inc;
            uint32 l4_dport_inc;

            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_RX_PORT_INCf, rx_port_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_TRUNK_ID_INCf, trunk_id_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_SIP_INCf, sip_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_DIP_INCf, dip_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_DSCP_INCf, dscp_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_IP6_LABEL_INCf, ip6_label_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_PROTO_INCf, ip_proto_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_SPORT_INCf, l4_sport_inc, "", errExit, ret);
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_DPORT_INCf, l4_dport_inc, "", errExit, ret);

            if (rx_port_inc)    { hash_key |= RTK_L3_ECMP_HASHKEY_PORT_ID; }
            if (trunk_id_inc)   { hash_key |= RTK_L3_ECMP_HASHKEY_TRUNK_ID; }
            if (sip_inc)        { hash_key |= RTK_L3_ECMP_HASHKEY_SIP; }
            if (dip_inc)        { hash_key |= RTK_L3_ECMP_HASHKEY_DIP; }
            if (dscp_inc)       { hash_key |= RTK_L3_ECMP_HASHKEY_IP_DSCP; }
            if (ip_proto_inc)   { hash_key |= RTK_L3_ECMP_HASHKEY_IP_PROTO; }
            if (ip6_label_inc)  { hash_key |= RTK_L3_ECMP_HASHKEY_IP6_LABEL; }
            if (l4_sport_inc)   { hash_key |= RTK_L3_ECMP_HASHKEY_SPORT; }
            if (l4_dport_inc)   { hash_key |= RTK_L3_ECMP_HASHKEY_DPORT; }

            *pArg = hash_key;
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_UNIVERSAL_ID:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_UNIVERSAL_IDf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_SIP_BIT_OFFSET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_SIP_BIT_OFFSETf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_DIP_BIT_OFFSET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_DIP_BIT_OFFSETf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_SPORT_BIT_OFFSET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_SPORT_BIT_OFFSETf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_DPORT_BIT_OFFSET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_DPORT_BIT_OFFSETf, val, "", errExit, ret);
            *pArg = val;
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_TBL_MANUAL:
        {
            *pArg = (_pL3Db[unit]->ecmp_hash_tbl_manual)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_GCT_NH_ERR_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_ERR_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlNhErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_NH_AGE_OUT_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_AGE_OUT_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlNhAgeOut, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_URPF_BASE_SEL:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_URPF_BASE_SELf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_BASE_PORT : RTK_L3_URPF_BASE_INTF;
        }
        break;

    case RTK_L3_GCT_NON_IP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_NON_IP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlNonIp, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_REFCNT_CHK:
        {
            *pArg = (_pL3Db[unit]->refer_cnt_chk_en)? ENABLED : DISABLED;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_globalCtrl_get */


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
int32
dal_mango_l3_globalCtrl_set(uint32 unit, rtk_l3_globalCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d,arg=0x%08x", unit, type, arg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_L3_GCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_L3_GCT_NONE:
        {
            /* reset L3 module */
            if ((uint32)arg == 0)
            {
                RT_ERR_HDL(_dal_mango_l3_reset(unit), errExit, ret);
            }
        }
        break;

    case RTK_L3_GCT_IP_HDR_ERR_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlIpHdrErr, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_IP_HDR_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlBadSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_DIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlBadDip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_BAD_DIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_ZERO_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlZeroSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_BC_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlDmacBc, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_DMAC_BC_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_MC_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlDmacMc, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_DMAC_MC_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_HDR_OPT_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlHdrOpt, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_HDR_OPT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_MTU_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlMtuFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_TTL_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlTtlFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_PKT_TO_CPU_TARGET:
        {
            L3_ACTION_TO_VALUE(_optIpucRouteCtrlPktToCpuTarget, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPUC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6_HDR_ERR_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlIp6HdrErr, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_IP6_HDR_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlBadSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_DIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlBadDip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_BAD_DIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_ZERO_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlZeroSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlDmacMismatch, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHbh, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HBH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ERR_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHbhErr, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HBH_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHdrRoute, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HDR_ROUTE_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_MTU_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlMtuFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HL_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHlFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_HL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_PKT_TO_CPU_TARGET:
        {
            L3_ACTION_TO_VALUE(_optIp6ucRouteCtrlPktToCpuTarget, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6UC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_KEY:
        {
            uint32 hash_key, value = 0;

            hash_key = arg;
            L3_REG_READ_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_PORT_ID)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_RX_PORT_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_TRUNK_ID)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_TRUNK_ID_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_SIP)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_SIP_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_DIP)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_DIP_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_IP_DSCP)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_DSCP_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_IP_PROTO)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_PROTO_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_IP6_LABEL)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_IP6_LABEL_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_SPORT)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_SPORT_INCf, val, value, "", errExit, ret);
            val = (hash_key & RTK_L3_ECMP_HASHKEY_DPORT)? 1 : 0;
            L3_REG_FIELD_SET_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_DPORT_INCf, val, value, "", errExit, ret);
            L3_REG_WRITE_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, value, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_UNIVERSAL_ID:
        {
            val = arg;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_UNIVERSAL_IDf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_SIP_BIT_OFFSET:
        {
            val = arg;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_SIP_BIT_OFFSETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_DIP_BIT_OFFSET:
        {
            val = arg;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_DIP_BIT_OFFSETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_SPORT_BIT_OFFSET:
        {
            val = arg;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_SPORT_BIT_OFFSETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_DPORT_BIT_OFFSET:
        {
            val = arg;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_ECMP_HASH_CTRLr, MANGO_L4_DPORT_BIT_OFFSETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_ECMP_HASH_TBL_MANUAL:
        {
            _pL3Db[unit]->ecmp_hash_tbl_manual = (arg)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_GCT_NH_ERR_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlNhErr, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_NH_AGE_OUT_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlNhAgeOut, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_NH_AGE_OUT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_URPF_BASE_SEL:
        {
            val = (((rtk_l3_urpf_base_t)arg) != RTK_L3_URPF_BASE_INTF)? 1 : 0;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_URPF_BASE_SELf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_L3_GCT_NON_IP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlNonIp, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP_ROUTE_CTRLr, MANGO_NON_IP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_REFCNT_CHK:
        {
            _pL3Db[unit]->refer_cnt_chk_en = (arg)? ENABLED : DISABLED;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_globalCtrl_set */


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
int32
dal_mango_l3_intfCtrl_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    uint32  val;
    l3_igr_intf_entry_t igrIntf;
    l3_egr_intf_entry_t egrIntf;
    rtk_l3_act_t act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d,type=%d,pArg=%p", unit, intfId, type, pArg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId) >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(0 == DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK(L3_INTF_IDX_IS_L2_TUNNEL(unit, DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId)), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((type >= RTK_L3_ICT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);
    L3_INT_SEM_LOCK(unit);  /* to avoid race-condition of L3 interface entry which may be accessed by internal APIs */

    intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    if (((FALSE == _pL3Db[unit]->intf[intfIdx].valid) && (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[unit]->intf[intfIdx].intf_id))))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errExit;
    }

    switch (type)
    {
    case RTK_L3_ICT_IPUC_ROUTE_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPUC_ROUTE_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IP6UC_ROUTE_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6UC_ROUTE_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IPMC_ROUTE_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ROUTE_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IP6MC_ROUTE_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6MC_ROUTE_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_CHK_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_CHK_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_DFLT_ROUTE_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_DFLT_ROUTE_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_MODE:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_CHK_MODEtf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_MODE_STRICT : RTK_L3_URPF_MODE_LOOSE;
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_FAIL_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_FAIL_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIpUrpfFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IPUC_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIpIcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IPUC_PBR_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIpPbrIcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_CHK_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_CHK_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_DFLT_ROUTE_EN:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_DFLT_ROUTE_ENtf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_MODE:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_CHK_MODEtf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_MODE_STRICT : RTK_L3_URPF_MODE_LOOSE;
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_FAIL_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_FAIL_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIIgrIntfIp6UrpfFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6UC_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIp6IcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6UC_PBR_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIp6PbrIcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IPMC_DIP_224_0_0_X_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ACT_224_0_0_Xtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIpmcAct224_0_0_x, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IPMC_DIP_224_0_1_X_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ACT_224_0_1_Xtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIpmcAct224_0_1_x, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IPMC_DIP_239_X_X_X_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ACT_239_X_X_Xtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIpmcAct239_x_x_x, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IPMC_ROUTE_LU_MIS_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ROUTE_LU_MIS_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIpmcRouteLuMis, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6_ND_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_ND_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIp6Nd, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6MC_DIP_FF0X_0000_XXXX_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6MC_ACT_0000_00XXtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIp6mcAct0000_00xx, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6MC_DIP_FF0X_XXXX_XXXX_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_MLD_ACT_0_X_Xtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIp6MldAct0_X_X, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6MC_DIP_FF0X_DB8_0_0_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_MLD_ACT_DB8_X_Xtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIp6MldActDb8_X_X, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6MC_ROUTE_LU_MIS_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6MC_ROUTE_LU_MIS_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIgrIntfIp6mcRouteLuMis, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_MC_KEY_SEL:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_MC_KEY_SELtf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_IPMC_KEY_SEL_INTF : RTK_L3_IPMC_KEY_SEL_VID;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    L3_INT_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intfCtrl_get */

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
int32
dal_mango_l3_intfCtrl_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    uint32  val;
    l3_igr_intf_entry_t igrIntf;
    l3_egr_intf_entry_t egrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d,type=%d,arg=0x%08x", unit, intfId, type, arg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId) >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(0 == DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK(L3_INTF_IDX_IS_L2_TUNNEL(unit, DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId)), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((type >= RTK_L3_ICT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);
    L3_INT_SEM_LOCK(unit);  /* to avoid race-condition of L3 interface entry which may be accessed by internal APIs */

    intfIdx = DAL_MANGO_L3_INTF_ID_ENTRY_IDX(intfId);
    if ( ( (FALSE == _pL3Db[unit]->intf[intfIdx].valid) && (DAL_MANGO_L3_INTF_ID_IS_L3(_pL3Db[unit]->intf[intfIdx].intf_id)) ) )
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errExit;
    }

    switch (type)
    {
    case RTK_L3_ICT_IPUC_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPUC_ROUTE_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6UC_ROUTE_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPMC_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ROUTE_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6MC_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6MC_ROUTE_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_CHK_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_CHK_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_DFLT_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_DFLT_ROUTE_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_MODE:
        {
            val = (((rtk_l3_urpf_mode_t)arg) != RTK_L3_URPF_MODE_LOOSE)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_CHK_MODEtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPUC_URPF_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIpUrpfFail, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP_URPF_FAIL_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPUC_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIpIcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPUC_PBR_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIpPbrIcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_CHK_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_CHK_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_DFLT_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_DFLT_ROUTE_ENtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_MODE:
        {
            val = (((rtk_l3_urpf_mode_t)arg) != RTK_L3_URPF_MODE_LOOSE)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_CHK_MODEtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_URPF_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIIgrIntfIp6UrpfFail, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_URPF_FAIL_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIp6IcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_PBR_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIp6PbrIcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_EGR_INTFt, intfIdx, egrIntf, \
                MANGO_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPMC_DIP_224_0_0_X_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIpmcAct224_0_0_x, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ACT_224_0_0_Xtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPMC_DIP_224_0_1_X_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIpmcAct224_0_1_x, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ACT_224_0_1_Xtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPMC_DIP_239_X_X_X_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIpmcAct239_x_x_x, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ACT_239_X_X_Xtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPMC_ROUTE_LU_MIS_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIpmcRouteLuMis, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IPMC_ROUTE_LU_MIS_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6_ND_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIp6Nd, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_ND_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6MC_DIP_FF0X_0000_XXXX_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIp6mcAct0000_00xx, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6MC_ACT_0000_00XXtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6MC_DIP_FF0X_XXXX_XXXX_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIp6MldAct0_X_X, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_MLD_ACT_0_X_Xtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6MC_DIP_FF0X_DB8_0_0_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIp6MldActDb8_X_X, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6_MLD_ACT_DB8_X_Xtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6MC_ROUTE_LU_MIS_ACT:
        {
            L3_ACTION_TO_VALUE(_actIgrIntfIp6mcRouteLuMis, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_IP6MC_ROUTE_LU_MIS_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_MC_KEY_SEL:
        {
            val = (((rtk_l3_ipmcKeySel_t)arg) != RTK_L3_IPMC_KEY_SEL_VID)? 1 : 0;
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                MANGO_L3_IGR_INTFt, intfIdx, igrIntf, \
                MANGO_L3_IGR_INTF_MC_KEY_SELtf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    L3_INT_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_intfCtrl_set */

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
int32
dal_mango_l3_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_l3_act_t act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,port=%d,type=%d,pArg=%p", unit, port, type, pArg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_L3_PCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_L3_PCT_IPUC_URPF_CHK_EN:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_DFLT_ROUTE_EN:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_MODE:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_MODEf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_MODE_STRICT : RTK_L3_URPF_MODE_LOOSE;
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_FAIL_ACT:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actPortIpRouteCtrlUrpfFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_CHK_EN:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_DFLT_ROUTE_EN:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_MODE:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_MODEf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_MODE_STRICT : RTK_L3_URPF_MODE_LOOSE;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_FAIL_ACT:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actPortIp6RouteCtrlUrpfFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_portCtrl_get */


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
int32
dal_mango_l3_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,port=%d,type=%d,arg=0x%08x", unit, port, type, arg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_L3_PCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_L3_PCT_IPUC_URPF_CHK_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_DFLT_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_MODE:
        {
            val = (((rtk_l3_urpf_mode_t)arg) != RTK_L3_URPF_MODE_LOOSE)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_MODEf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actPortIpRouteCtrlUrpfFail, val, arg, "", errExit, ret);
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_CHK_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_DFLT_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_MODE:
        {
            val = (((rtk_l3_urpf_mode_t)arg) != RTK_L3_URPF_MODE_LOOSE)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_CHK_MODEf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actPortIp6RouteCtrlUrpfFail, val, arg, "", errExit, ret);
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_URPF_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_portCtrl_set */



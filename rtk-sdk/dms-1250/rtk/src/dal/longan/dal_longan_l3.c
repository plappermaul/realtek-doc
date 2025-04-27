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
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/longan/dal_longan_vlan.h>
#include <dal/longan/dal_longan_l2.h>
#include <dal/longan/dal_longan_l3.h>
#include <dal/longan/dal_longan_ipmcast.h>
#include <dal/longan/dal_longan_mcast.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/mcast.h>

#define LONGAN_L3_DBG                    (0)
#define LONGAN_L3_ROUTE_HW_LU            (1)

#define LONGAN_L3_DBG_PRINTF(_level, _fmt, ...)                                      \
    do {                                                                            \
        if (LONGAN_L3_DBG >= (_level))                                               \
            osal_printf("%s():L%d: " _fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
    } while (0)


/* This MACRO configure the intf route mac is care VID or not.
*   when configure DAL_LONGAN_L3_RTMAC_KEY_MAC_VID the intf num is 64
*   if want to use 128 intf num should configure DAL_LONGAN_L3_RTMAC_KEY_MAC
*/
#define L3_RTMAC_KEY    DAL_LONGAN_L3_RTMAC_KEY_MAC_VID
/* L3 reference count check */
#define LONGAN_L3_REFCNT_CHK_DEFAULT     (ENABLED)

/* This MACRO configure one intf is mapped to one vlan id.SS-662
*/
#undef L3_VID_INTF_BINDING

/* This MACRO configure the nexthop id found method.
*   DAL_LONGAN_L3_PATHID_METHOD_ASIC :  use ASIC table traverse
*   DAL_LONGAN_L3_PATHID_METHOD_SW:     use SW database traverse
*/
#define L3_NEXTHOP_FIND     DAL_LONGAN_L3_PATHID_METHOD_SW
#define LONGAN_L3_HASH_IDX_ALG_NUM      (2)

/* L3 ipuc host table alg */
#define L3_TABLE0_ALG       DAL_LONGAN_L3_ALG0
#define L3_TABLE1_ALG       DAL_LONGAN_L3_ALG1

typedef struct dal_longan_l3_drvDb_s
{
    /* hardware resource (for internal APIs) */
    struct
    {
        /* Interface MTU (IPv4) */
        uint32          ip4_mtu_used_count;

        struct
        {
            uint32  mtu_value;
            uint32  ref_count;
        } ip4_mtu[DAL_LONGAN_L3_IP_MTU_MAX];

        /* Interface MTU (IPv6) */
        uint32          ip6_mtu_used_count;

        struct
        {
            uint32  mtu_value;
            uint32  ref_count;
        } ip6_mtu[DAL_LONGAN_L3_IP6_MTU_MAX];
        /* for 9300 */
        dal_longan_l3_rtmac_key_t           rtmac_key;
        uint32          mac_used_count;
        struct
        {
            rtk_mac_t  mac;
            rtk_intf_id_t    intf_id;
            uint32  ref_count;
        }route_mac[DAL_LONGAN_L3_MAC_MAX];
        struct
        {
            rtk_mac_t  mac;
            uint32  ref_count;
        }source_mac[DAL_LONGAN_L3_MAC_MAX];
        struct
        {
            rtk_mac_t  mac;
            uint32  ref_count;
        }dst_mac[DAL_LONGAN_L3_NEXTHOP_MAX];

        /* egress interface */
        rtk_bitmap_t    intf_used[BITMAP_ARRAY_CNT(DAL_LONGAN_L3_INTF_MAX)];
        uint32          intf_used_count;
        struct
        {
            uint32  flags;  /* for logging: caller and etc. */
        } intf[DAL_LONGAN_L3_INTF_MAX];

        /* nexthop */
        rtk_bitmap_t    nexthop_used[BITMAP_ARRAY_CNT(DAL_LONGAN_L3_NEXTHOP_MAX)];
        uint32          nexthop_used_count;
        struct
        {
            uint32  flags;  /* for logging: caller and etc. */
        } nexthop[DAL_LONGAN_L3_NEXTHOP_MAX];

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

                } slot[DAL_LONGAN_L3_HOST_TBL_WIDTH];
            } row[DAL_LONGAN_L3_HOST_TBL_HEIGHT];
        } hostTable[DAL_LONGAN_L3_HOST_TBL_NUM];

        /* route table */
        struct
        {
            /*routeTable*/
            uint32      index_max; /* maximum number of index that L3 module can use */
            uint32      size;           /*route table size*/

            struct
            {
                uint32  used_count;     /* used entry number by IPv4 UC entry */
                uint16  entries_after_pfLen[LONGAN_L3_HOST_IP_PFLEN];    /* total entries after the specific pfLen */
            } IPv4;

            struct
            {
                uint32  used_count;     /* used entry number by IPv6 UC entry */
                uint16  entries_before_pfLen[LONGAN_L3_HOST_IP6_PFLEN];  /* total entries before the specific pfLen */
            } IPv6;

        } routeTable;
    } HW;

#ifdef L3_VID_INTERFACE_BINDING
    /* VLAN */
    rtk_bitmap_t    vid_valid[BITMAP_ARRAY_CNT(DAL_LONGAN_L3_VID_MAX)];
#endif
    struct
    {
        uint32      intf_idx;
    } vid[DAL_LONGAN_L3_VID_MAX];

    /* egress Interface */
    struct
    {
        uint32      valid:1;
        uint32      reserved:15;    /* reserved */
        uint32      ref_count:16;

        /* <SHADOW>: cache to avoid reading HW entry from CHIP (enhancement) */

        rtk_intf_id_t   intf_id;
        rtk_vrf_id_t    vrf_id;     /* [SHADOW] for calculating VRF usage */

        /* [DRV] */
        uint32          ip_mtu_idx:16; /* [SHADOW] for managing MTU resource */
        uint32          ip6_mtu_idx:16; /* [SHADOW] for managing IPv6 MTU resource */
        rtk_vlan_t      vid;        /* [SHADOW] for creating nexthop entry */

        uint32  mac_idx;       /*only for 9300 managing source mac resource*/
        uint32  rtmac_idx;       /*only for 9300 managing route mac resource*/
        uint32  rtmac_idx_valid;       /*only for 9300 managing route mac resource*/

        rtk_l3_intf_flag_t  flags;          /* interface flags */
    } intf[DAL_LONGAN_L3_INTF_MAX];

    /* Nexthop */
    uint32      nexthop_find_method;
    struct
    {
        uint32      valid:1;
        uint32      mac_addr_alloc:1;   /* MAC has been allocated if set */
        uint32      reserved:14;    /* reserved */
        uint32      ref_count:16;

        /* <SHADOW>: cache to avoid reading HW entry from CHIP (enhancement) */
        uint32      intf_idx;       /* egress L3 interface */
        rtk_mac_t   mac_addr;       /* destination MAC address */
        uint32      mac_addr_idx;   /* destination MAC address index */
    } nexthop[DAL_LONGAN_L3_NEXTHOP_MAX];

    /* Host table */
    struct
    {
        uint32      valid:1;
        uint32      ipv6:1;
        uint32      reserved:30;    /* reserved */
        rtk_l3_host_t   l3_entry;  /*host entry shadow*/
    } host[DAL_LONGAN_L3_HOST_TBL_SIZE];

    /* Route table */
    struct
    {
        struct
        {
            rtk_l3_pathId_t     path_id;
            uint32      prefix_len:16;
            uint32      suffix_len:16;
        }shadow;

        rtk_l3_route_t  l3_entry;
    } route[DAL_LONGAN_L3_ROUTE_TBL_SIZE];

    /* reference count check state */
    rtk_enable_t    refer_cnt_chk_en;

#if LONGAN_L3_ROUTE_IPMC_SIZE
    uint32          l3_route_ipmc_idx_base;
    uint32          l3_route_ipmc_size;
#endif

} dal_longan_l3_drvDb_t;


typedef struct dal_longan_l3_hashIdx_s
{
    uint32 hashIdx_of_alg[LONGAN_L3_HASH_IDX_ALG_NUM];
} dal_longan_l3_hashIdx_t;


/*
 * Data Declaration
 */

/* action */
static uint32 _actRouterMAC[] = { /* action for Router MAC entry */
    RTK_L3_ACT_FORWARD,         /* routing */
    RTK_L3_ACT_DROP,            /* bridging only */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actHostEntryUC[] = {    /* action for Unicast HOST entry */
    RTK_L3_HOST_ACT_FORWARD,
    RTK_L3_HOST_ACT_TRAP2CPU,
    RTK_L3_HOST_ACT_COPY2CPU,
    RTK_L3_HOST_ACT_DROP,
    };

static uint32 _actRouteEntryUC[] = {    /* action for Unicast ROUTE entry */
    RTK_L3_ROUTE_ACT_FORWARD,
    RTK_L3_ROUTE_ACT_TRAP2CPU,
    RTK_L3_ROUTE_ACT_COPY2CPU,
    RTK_L3_ROUTE_ACT_DROP,
    };

static uint32 _actIpRouteCtrlIpHdrErr[] = {   /* LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_IP_HDR_ERR_ACTf */
    RTK_L3_ACT_DROP,                    /*multicast mac-base bridge*/
    RTK_L3_ACT_TRAP2CPU,                /*multicast mac-base bridge + trap*/
    RTK_L3_ACT_TRAP2MASTERCPU,          /*multicast mac-base bridge + trap*/
    RTK_L3_ACT_FORWARD,                 /*multicast mac-base bridge, unicast drop*/
    };

static uint32 _actIpucRouteCtrlBadSip[] = {   /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlBadDip[] = {   /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_BAD_DIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlZeroSip[] = {  /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlDmacBc[] = {   /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_DMAC_BC_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_FORWARD,             /*bridge*/
    };

static uint32 _actIpucRouteCtrlDmacMc[] = {   /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_DMAC_MC_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_FORWARD,             /*bridge*/
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlHdrOpt[] = {   /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_HDR_OPT_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,             /*route*/
    RTK_L3_ACT_COPY2CPU,            /*route & copy to cpu*/
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,      /*route & copy to master*/
    };

static uint32 _actIpucRouteCtrlMtuFail[] = {  /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpucRouteCtrlTtlFail[] = {  /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_TTL_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _optIpucRouteCtrlPktToCpuTarget[] = { /* LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf */
    RTK_L3_CPUTARGET_LOCAL,
    RTK_L3_CPUTARGET_MASTER,
    };

static uint32 _actIpRouteCtrlIp6HdrErr[] = {  /* LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_IP6_HDR_ERR_ACTf */
    RTK_L3_ACT_DROP,                    /*multicast mac-base bridge*/
    RTK_L3_ACT_TRAP2CPU,                /*multicast mac-base bridge + trap*/
    RTK_L3_ACT_TRAP2MASTERCPU,          /*multicast mac-base bridge + trap*/
    RTK_L3_ACT_FORWARD,                 /*multicast mac-base bridge, unicast drop*/
    };

static uint32 _actIp6ucRouteCtrlBadSip[] = {  /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlBadDip[] = {  /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_BAD_DIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlZeroSip[] = { /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlDmacMismatch[] = {    /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHbh[] = { /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HBH_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,             /* route */
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHbhErr[] = {  /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HBH_ERR_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,             /* route */
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHdrRoute[] = {    /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HDR_ROUTE_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,             /* route */
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlMtuFail[] = { /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6ucRouteCtrlHlFail[] = {  /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HL_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _optIp6ucRouteCtrlPktToCpuTarget[] = {    /* LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf */
    RTK_L3_CPUTARGET_LOCAL,
    RTK_L3_CPUTARGET_MASTER,
    };

static uint32 _actIpRouteCtrlNhAgeOut[] = {   /* LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_NH_AGE_OUT_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_FORWARD,             /* flood */
    RTK_L3_ACT_COPY2CPU,            /* flood & copy */
    RTK_L3_ACT_COPY2MASTERCPU,      /* flood & copy to master */
    };

static uint32 _actIpRouteCtrlNonIp[] = {  /* LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_NON_IP_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    };

static uint32 _actEgrIntfIpIcmpRedirect[] = { /* LONGAN_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actEgrIntfIpPbrIcmpRedirect[] = {  /* LONGAN_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actEgrIntfIp6IcmpRedirect[] = {    /* LONGAN_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actEgrIntfIp6PbrIcmpRedirect[] = { /* LONGAN_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf */
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actPortIpRouteCtrlUrpfFail[] = {   /*LONGAN_L3_PORT_IP_ROUTE_CTRLr, LONGAN_URPF_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32 _actPortIp6RouteCtrlUrpfFail[] = {  /* LONGAN_L3_PORT_IP6_ROUTE_CTRLr, LONGAN_URPF_FAIL_ACTf */
    RTK_L3_ACT_DROP,
    RTK_L3_ACT_TRAP2CPU,
    RTK_L3_ACT_FORWARD,
    RTK_L3_ACT_COPY2CPU,
    RTK_L3_ACT_TRAP2MASTERCPU,
    RTK_L3_ACT_COPY2MASTERCPU,
    };

static uint32               l3_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         l3_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         l3_int_sem[RTK_MAX_NUM_OF_UNIT];

static dal_longan_l3_drvDb_t     *_pL3Db[RTK_MAX_NUM_OF_UNIT];

static int32 width2entryType[8] = {-1, 0, 1, 2, -1, -1, 3, -1}; /*value is -1 is invalid*/

/*
 * Macro Declaration
 */

/* L3 semaphore handling */
#define L3_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(l3_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)          \
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore lock failed");          \
        return RT_ERR_SEM_LOCK_FAILED;                              \
    }\
} while(0)
#define L3_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(l3_sem[unit]) != RT_ERR_OK)                     \
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore unlock failed");      \
        return RT_ERR_SEM_UNLOCK_FAILED;                            \
    }\
} while(0)
#define L3_INT_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(l3_int_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)      \
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "internal semaphore lock failed"); \
        return RT_ERR_SEM_LOCK_FAILED;                              \
    }\
} while(0)
#define L3_INT_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(l3_int_sem[unit]) != RT_ERR_OK)                 \
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "internal semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;                            \
    }\
} while(0)


#define L3_ACTION_TO_VALUE(_actArray, _value, _action, _errMsg, _errHandle, _retval) \
do {                                                                                                                            \
    if ((_retval = rt_util_actListIndex_get(_actArray, (sizeof(_actArray)/sizeof(uint32)), &(_value), _action)) != RT_ERR_OK)   \
    {                                                                                                                           \
        RT_ERR(_retval, (MOD_L3|MOD_DAL), _errMsg);                                                                             \
        goto _errHandle;                                                                                                        \
    }                                                                                                                           \
} while(0)
#define L3_VALUE_TO_ACTION(_actArray, _action, _value, _errMsg, _errHandle, _retval) \
do {                                                                                                                            \
    if ((_retval = rt_util_actListAction_get(_actArray, (sizeof(_actArray)/sizeof(uint32)), &(_action), _value)) != RT_ERR_OK)  \
    {                                                                                                                           \
        RT_ERR(_retval, (MOD_L3|MOD_DAL), _errMsg);                                                                             \
        goto _errHandle;                                                                                                        \
    }                                                                                                                           \
} while(0)

#define L3_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                    \
    if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)               \
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
        RT_ERR(ret, (MOD_L3|MOD_DAL), _errMsg);                                     \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define L3_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                \
    if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)    \
    {                                                                               \
        RT_ERR(ret, (MOD_L3|MOD_DAL), _errMsg);                                     \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define L3_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)           \
do {                                                                                                    \
    if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(ret, (MOD_L3|MOD_DAL), _errMsg);                                                         \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)
#define L3_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)           \
do {                                                                                                    \
    if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(ret, (MOD_L3|MOD_DAL), _errMsg);                                                         \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)
#define L3_TABLE_FIELD_MAC_GET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)           \
do {                                                                                                        \
    if ((_ret = table_field_mac_get(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
    {                                                                                                       \
        RT_ERR(ret, (MOD_L3|MOD_DAL), _errMsg);                                                             \
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


#define L2_MAC_IDX_TO_INDEX_STRUCT(_idx, _struct)   \
do {                                                                \
    _struct.index_type = ((_idx) < 16384)? L2_IN_HASH : L2_IN_CAM;  \
    _struct.index = ((_idx) >> 2);                                  \
    _struct.hashdepth = ((_idx) & 0x3);                             \
} while(0)
#define L3_RT_ERR_HDL_DBG(_op, _args...) \
do {                                                                        \
    if (RT_ERR_OK != (_op))                                                 \
    {                                                                       \
       RT_LOG(LOG_DEBUG, (MOD_L3|MOD_DAL), ## _args);                       \
    }                                                                       \
} while(0)

#define L3_DB_UPDATE_INTF_VALID(_unit, _intfIdx, _mtuIdx, _macIdx, _vid, _ip6mtuIdx,_flags) \
do {                                                    \
    _pL3Db[_unit]->intf[_intfIdx].ip_mtu_idx = _mtuIdx; \
    _pL3Db[_unit]->intf[_intfIdx].ip6_mtu_idx = _ip6mtuIdx; \
    _pL3Db[_unit]->intf[_intfIdx].mac_idx = _macIdx;    \
    _pL3Db[_unit]->intf[_intfIdx].vid = _vid;           \
    _pL3Db[_unit]->intf[_intfIdx].valid = TRUE;         \
    _pL3Db[_unit]->intf[_intfIdx].flags = _flags;       \
} while(0)
#define L3_DB_UPDATE_INTF_INVALID(_unit, _intfIdx)      \
do {                                                    \
    _pL3Db[_unit]->intf[_intfIdx].ip_mtu_idx = 0;       \
    _pL3Db[_unit]->intf[_intfIdx].ip6_mtu_idx = 0;      \
    _pL3Db[_unit]->intf[_intfIdx].valid = FALSE;        \
    _pL3Db[_unit]->intf[_intfIdx].flags = 0;            \
} while(0)

#define L3_DB_UPDATE_INTF_RTMAC_INDEX(_unit, _intfIdx, _rtmacIdxValid, _rtmacIdx) \
    do {                                                                \
        _pL3Db[_unit]->intf[(_intfIdx)].rtmac_idx_valid = (_rtmacIdxValid);   \
        _pL3Db[_unit]->intf[_intfIdx].rtmac_idx = (_rtmacIdx);          \
    } while(0)
#define L3_DB_INTF_RTMAC_INDEX_VALID(_unit, _intfIdx) (_pL3Db[_unit]->intf[_intfIdx].rtmac_idx_valid)
#define L3_DB_INTF_RTMAC_INDEX(_unit, _intfIdx)       (_pL3Db[_unit]->intf[_intfIdx].rtmac_idx)
#define L3_DB_INTF_FLAGS(_unit, _intfIdx)       (_pL3Db[_unit]->intf[_intfIdx].flags)
#define L3_DB_INTF_REFCNT(_unit, _intfIdx)      (_pL3Db[_unit]->intf[_intfIdx].ref_count)
#define L3_DB_NH_REFCNT(_unit, _nhIdx)          (_pL3Db[_unit]->nexthop[_nhIdx].ref_count)

#define L3_INTF_MTU_IP6_FIX_NEW(_ip6mtu)      ((_ip6mtu > 40)? (_ip6mtu) : 0)

#define L3_INTF_IDX_IS_VALID(_unit, _intfIdx)   (FALSE != (_pL3Db[(_unit)]->intf[(_intfIdx)].valid))

#define HASH_BIT_EXTRACT(_val, _lsb, _len)   (((_val) & (((1 << (_len)) - 1) << (_lsb))) >> (_lsb))

#define IS_L3_RTMAC_CARE_VID(_unit)    (DAL_LONGAN_L3_RTMAC_KEY_MAC_VID == _pL3Db[_unit]->HW.rtmac_key)

#define L3_ROUTE_IDX_MIN(_unit)                 (0)
#define L3_ROUTE_IDX_MAX(_unit)                 (_pL3Db[(_unit)]->HW.routeTable.index_max)
#define L3_ROUTE_TBL_SIZE(_unit)                (_pL3Db[(_unit)]->HW.routeTable.size)
#define L3_ROUTE_TBL_IP_CNT(_unit)              (_pL3Db[(_unit)]->HW.routeTable.IPv4.used_count)
#define L3_ROUTE_TBL_IP_CNT_PFLEN(_unit, _pfl)  (_pL3Db[(_unit)]->HW.routeTable.IPv4.entries_after_pfLen[(_pfl)])
#define L3_ROUTE_TBL_IP6_CNT(_unit)             (_pL3Db[(_unit)]->HW.routeTable.IPv6.used_count)
#define L3_ROUTE_TBL_IP6_CNT_PFLEN(_unit, _pfl) (_pL3Db[(_unit)]->HW.routeTable.IPv6.entries_before_pfLen[(_pfl)])
#define L3_ROUTE_TBL_USED(_unit)                (L3_ROUTE_TBL_IP_CNT(_unit) + (L3_ROUTE_TBL_IP6_CNT(_unit) * 3))

#define IS_L3_ROUTE_IPV6_EMPTY(_unit)               (0 == L3_ROUTE_TBL_IP6_CNT(_unit))
#define IS_L3_ROUTE_IPV6_IDX_VALID(_idx)        ((0 == ((_idx)%8)) || (3 == ((_idx)%8)))
#define L3_ROUTE_IPV6_IDX_MAX(_unit)                 (_pL3Db[(_unit)]->HW.routeTable.index_max-4)

#define L3_ROUTE_IPV4_LAST_IDX(_unit)              ((L3_ROUTE_TBL_IP_CNT(_unit) > 0) ?  (L3_ROUTE_TBL_IP_CNT(_unit)-1) : (0))
#define L3_ROUTE_IPV6_LAST_IDX(_unit)              ((L3_ROUTE_TBL_IP6_CNT(_unit)  > 0) ? \
                                                                                (L3_ROUTE_TBL_SIZE(_unit) - ((L3_ROUTE_TBL_IP6_CNT(_unit) * 3) + ((L3_ROUTE_TBL_IP6_CNT(_unit)+1)/2 * 2))) : \
                                                                                (L3_ROUTE_IPV6_IDX_MAX(_unit)))
#define L3_ROUTE_IPV6_LEN2CNT(_len)                 (((_len) - ((((_len)+3)/8) * 2))/3)
#define L3_ROUTE_IPV6_CNT2LEN(_idx, _cnt)       (3*(_cnt) + (((_idx)%8)/3 + ((_cnt)-1)/2)*2)
#define L3_ABS(_x, _y)                          (((_x) > (_y))? ((_x) - (_y)) : ((_y) - (_x)))

#define L3_ENTRY_HOST_ADDR(_unit, _entryIdx)         (&(_pL3Db[(_unit)]->host[(_entryIdx)].l3_entry))
#define L3_ENTRY_ROUTE_ADDR(_unit, _entryIdx)         (&(_pL3Db[(_unit)]->route[(_entryIdx)].l3_entry))

//note this api is base valid ipv6 index calc index +3/-3
static inline int32 l3_longan_ipv6_valid_idx_ret(uint32 idx, uint32 next)
{
    if (IS_L3_ROUTE_IPV6_IDX_VALID(idx))
        return (idx);
    else
    {
        if (next)
            return (idx+=2);
        else
            return (idx-=2);
    }
}
#define L3_ROUTE_IPV6_IDX_PRE(_idx)     (l3_longan_ipv6_valid_idx_ret((_idx), 0))
#define L3_ROUTE_IPV6_IDX_NEXT(_idx)   (l3_longan_ipv6_valid_idx_ret((_idx), 1))


/*
 * Function Declaration
 */
static inline int32 l3_util_macAddr_isMcast(rtk_mac_t *pMac)
{
    if (NULL == pMac)
        return FALSE;
    if (pMac->octet[0] & 0x01)
        return TRUE;
    return FALSE;
}

static inline int32 ipArray2ipv6(rtk_ipv6_addr_t *ip6_addr, uint32 *ip6_array)
{
    int32 i;
    uint32 swap_addr[IP6_ADDR_LEN >> 2];

    RT_PARAM_CHK((NULL == ip6_addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == ip6_array), RT_ERR_NULL_POINTER);

    swap_addr[0] = ip6_array[3];
    swap_addr[1] = ip6_array[2];
    swap_addr[2] = ip6_array[1];
    swap_addr[3] = ip6_array[0];

    for (i = 0; i < 4; i++)
    {
        ip6_addr->octet[i * 4 + 0] = (swap_addr[i] & 0xFF000000) >> 24;
        ip6_addr->octet[i * 4 + 1] = (swap_addr[i] & 0x00FF0000) >> 16;
        ip6_addr->octet[i * 4 + 2] = (swap_addr[i] & 0x0000FF00) >> 8;
        ip6_addr->octet[i * 4 + 3] = (swap_addr[i] & 0x000000FF) >> 0;
    }

    return RT_ERR_OK;
}

static inline int32 ipv6toIpArray(uint32 *ip6_array,rtk_ipv6_addr_t *ip6_addr)
{
    int32 i;
    uint32 swap_addr[IP6_ADDR_LEN >> 2];

    RT_PARAM_CHK((NULL == ip6_addr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == ip6_array), RT_ERR_NULL_POINTER);

    for (i = 0; i < 4; i++)
    {
        swap_addr[i] =  ((ip6_addr->octet[i * 4 + 0] & 0xFF) << 24) | \
                                ((ip6_addr->octet[i * 4 + 1] & 0xFF) << 16) | \
                                ((ip6_addr->octet[i * 4 + 2] & 0xFF) << 8) | \
                                ((ip6_addr->octet[i * 4 + 3] & 0xFF) << 0);
    }

    ip6_array[3] = swap_addr[0];
    ip6_array[2] = swap_addr[1];
    ip6_array[1] = swap_addr[2];
    ip6_array[0] = swap_addr[3];

    return RT_ERR_OK;
}

static inline rtk_ipv6_addr_t ip6AddrOr_ret(rtk_ipv6_addr_t ip1, rtk_ipv6_addr_t ip2)
{
    rtk_ipv6_addr_t ip6;
    int32 i;

    osal_memset(&ip6, 0x0, sizeof(rtk_ipv6_addr_t));

    for (i = 0; i < IPV6_ADDR_LEN; i++)
    {
        ip6.octet[i] = ip1.octet[i] | ip2.octet[i];
    }

    return ip6;
}

static inline int32 l3_util_inValidBitIdx_range_get(rtk_bitmap_t *pBitmap, int32 bitSize, int32 *pStartIdx, int32 *pEndIdx)
{
    int32 wordIdx;

    RT_PARAM_CHK((NULL == pStartIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEndIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pBitmap), RT_ERR_NULL_POINTER);

    for (wordIdx = 0; wordIdx < BITMAP_ARRAY_CNT(bitSize); wordIdx++)
    {
        if (0xffffffff != pBitmap[wordIdx])
        {
            break;
        }
    }

    if (wordIdx == BITMAP_ARRAY_CNT(bitSize))
        return RT_ERR_TBL_FULL;

    *pStartIdx = wordIdx * BITMAP_WIDTH;
    *pEndIdx = bitSize < (wordIdx+1) * BITMAP_WIDTH ? bitSize : (wordIdx+1) * BITMAP_WIDTH ;

    return RT_ERR_OK;
}

static inline rtk_ipv6_addr_t l3_util_ip6sufLen2Mask_ret(uint32 suf_len)
{
    rtk_ipv6_addr_t ip6;
    uint32 i = 0;

    osal_memset(&ip6, 0x00, sizeof(rtk_ipv6_addr_t));

    if (suf_len > 128)   /* length must be in the range: 0~128 */
        suf_len = 128;

    /* step by 1 byte */
    while ((suf_len - i) > 7)
    {
        ip6.octet[(IPV6_ADDR_LEN-1-i/8)] = 0xFF;
        i += 8;
    }

    /* step by D-bit */
    if (i < suf_len)
    {
        ip6.octet[(IPV6_ADDR_LEN-1-i/8)] = (0xFF >> (8 - (suf_len - i)));
    }

    return ip6;
}

static inline rtk_ip_addr_t l3_util_ipSufLen2Mask_ret(uint32 suf_len)
{
    rtk_ip_addr_t ip;

    if (suf_len > 32)
        suf_len = 32;

    ip = (0x1 << suf_len) - 1;

    return ip;
}

static inline int32 l3_util_ipSuffixMaxMatchLength_ret(rtk_ip_addr_t ip1, rtk_ip_addr_t ip2, uint32 maxLen)
{
    int32   offset = 0, length = 0;

    /* maximum compare length is 32 */
    if (maxLen > 32)
        maxLen = 32;

    while ((length < maxLen) && \
           ((ip1 & (0x1 << offset)) == \
            (ip2 & (0x1 << offset))))
    {
        offset++;
        length++;
    }

    return length;
}

static inline int32 l3_util_ipv6SuffixMaxMatchLength_ret(rtk_ipv6_addr_t *pIp1, rtk_ipv6_addr_t *pIp2, uint32 maxLen)
{
    int32   offset = 0, length = 0;

    /* maximum compare length is 128 (IPV6_ADDR_LEN * 8) */
    if (maxLen > (IPV6_ADDR_LEN * 8))
        maxLen = (IPV6_ADDR_LEN * 8);

    while ((length < maxLen) && \
           ((pIp1->octet[IPV6_ADDR_LEN - 1- (offset >> 3)] & (0x1 << ((offset%8) & 0x7))) == \
            (pIp2->octet[IPV6_ADDR_LEN - 1- (offset >> 3)] & (0x1 << ((offset%8) & 0x7)))))
    {
        offset++;
        length++;
    }

    return length;
}

static inline int32 l3_util_intfMtu_set(uint32 unit, uint32 mtuIdx, uint32 mtuValue)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((mtuIdx >= DAL_LONGAN_L3_IP_MTU_MAX), RT_ERR_OUT_OF_RANGE);

    /* update shadow (currently,IPv4 MTU) */
    _pL3Db[unit]->HW.ip4_mtu[mtuIdx].mtu_value = mtuValue;

    /* write to H/W */
    L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
        mtuIdx, LONGAN_MTU_VALf, mtuValue, "", errHandle, ret);

errHandle:
    return ret;
}
static inline int32 l3_util_intfIp6Mtu_set(uint32 unit, uint32 mtuIdx, uint32 mtuValue)
{
    int32   ret = RT_ERR_OK;
    uint32  hwMtuIp6Value;

    /* parameter check */
    RT_PARAM_CHK((mtuIdx >= DAL_LONGAN_L3_IP_MTU_MAX), RT_ERR_OUT_OF_RANGE);

    hwMtuIp6Value = L3_INTF_MTU_IP6_FIX_NEW(mtuValue);

    /* update shadow (currently, IPv6 MTU) */
    _pL3Db[unit]->HW.ip6_mtu[mtuIdx].mtu_value = mtuValue;

    /* write to H/W */
    L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
        mtuIdx, LONGAN_MTU_VALf, hwMtuIp6Value, "", errHandle, ret);

errHandle:
    return ret;
}

/* L3 Router MAC - (RTK) rtk_l3_routerMacEntry_t to (DAL) dal_longan_l3_macEntry_t */
static inline int32 l3_util_rtkRouterMac2macEntry(dal_longan_l3_macEntry_t *pEntry, rtk_l3_routerMacEntry_t *pMac)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_longan_l3_macEntry_t));

    pEntry->valid           = (pMac->enable)? 0x1 : 0x0;

    pEntry->port_type       = (pMac->port_type)? 0x1: 0x0;
    pEntry->bmsk_port_type  = pMac->port_type_mask;

    pEntry->port_id         = pMac->port_trunk_id;
    pEntry->bmsk_port_id    = pMac->port_trunk_id_mask;

    pEntry->intf_id         = pMac->vid;
    pEntry->bmsk_intf_id    = pMac->vid_mask;

    pEntry->mac             = pMac->mac;
    pEntry->bmsk_mac        = pMac->mac_mask;

    pEntry->l3_intf         = 0;
    pEntry->bmsk_l3_intf    = 0;

    L3_ACTION_TO_VALUE(_actRouterMAC, pEntry->act, pMac->l3_act, "", errHandle, ret);

errHandle:
    return ret;
}

/* L3 Router MAC - (DAL) dal_longan_l3_macEntry_t to (RTK) rtk_l3_routerMacEntry_t */
static inline int32 l3_util_macEntry2rtkRouterMac(rtk_l3_routerMacEntry_t *pMac, dal_longan_l3_macEntry_t *pEntry)
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

    pMac->vid           = pEntry->intf_id;
    pMac->vid_mask      = pEntry->bmsk_intf_id;

    pMac->mac               = pEntry->mac;
    pMac->mac_mask          = pEntry->bmsk_mac;

    L3_VALUE_TO_ACTION(_actRouterMAC, pMac->l3_act, pEntry->act, "", errHandle, ret);

errHandle:
    return ret;
}

static inline int32 l3_util_rtkHost2hostEntry(dal_longan_l3_hostEntry_t *pEntry, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));

    pEntry->vrf_id = 0;
    pEntry->valid = 1;
    if (pHost->flags & RTK_L3_FLAG_IPV6)
    {
        pEntry->entry_type = 2;
        pEntry->ip6 = pHost->ip_addr.ipv6;
    }
    else
    {
        pEntry->entry_type = 0;
        pEntry->ip = pHost->ip_addr.ipv4;
    }
    pEntry->dst_null_intf = (pHost->flags & RTK_L3_FLAG_NULL_INTF)? 1 : 0;
    pEntry->ttl_dec = (pHost->flags & RTK_L3_FLAG_TTL_DEC_IGNORE)? 0 : 1;
    pEntry->ttl_chk = (pHost->flags & RTK_L3_FLAG_TTL_CHK_IGNORE)? 0 : 1;
    pEntry->qos_en = (pHost->flags & RTK_L3_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pHost->qos_pri;
    pEntry->hit = (pHost->flags & RTK_L3_FLAG_HIT)? 1 : 0;

    pEntry->nh_ecmp_idx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(pHost->path_id);

    L3_ACTION_TO_VALUE(_actHostEntryUC, pEntry->act, pHost->fwd_act, "", errHandle, ret);

errHandle:
    return ret;
}

static inline int32 l3_util_hostEntry2rtkHost(rtk_l3_host_t *pHost, dal_longan_l3_hostEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pHost, 0x00, sizeof(rtk_l3_host_t));
    pHost->vrf_id = 0;
    if (pEntry->entry_type >= 2)    /* IPv6 */
    {
        pHost->flags |= RTK_L3_FLAG_IPV6;

        pHost->ip_addr.ipv6 = pEntry->ip6;
    }
    else
    {
        pHost->ip_addr.ipv4 = pEntry->ip;
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

    pHost->path_id = DAL_LONGAN_L3_NH_IDX_TO_PATH_ID(pEntry->nh_ecmp_idx);

    L3_VALUE_TO_ACTION(_actHostEntryUC, pHost->fwd_act, pEntry->act, "", errHandle, ret);

errHandle:
    return ret;
}

static inline int32 l3_util_rtkRoute2routeEntry(dal_longan_l3_routeEntry_t *pEntry, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_longan_l3_routeEntry_t));

    pEntry->valid = 1;
    pEntry->bmsk_entry_type = 0x3;
    if (pRoute->flags & RTK_L3_FLAG_IPV6)
    {
        pEntry->entry_type = 2;
        pEntry->ip6 = pRoute->ip_addr.ipv6;
        pEntry->bmsk_ip6 = rt_util_ip6Length2Mask_ret(pRoute->prefix_len);
        pEntry->bmsk_ip6 = ip6AddrOr_ret(pEntry->bmsk_ip6, l3_util_ip6sufLen2Mask_ret(pRoute->suffix_len));
        pEntry->host_route = (pRoute->prefix_len >= RTK_L3_HOST_IP6_PREFIX_LENGTH)? 1 : 0;
        pEntry->dflt_route = (pRoute->prefix_len == 0)? 1 : 0;
    }
    else
    {
        pEntry->entry_type = 0;
        pEntry->ip = pRoute->ip_addr.ipv4;
        pEntry->bmsk_ip = rt_util_ipLength2Mask_ret(pRoute->prefix_len);
        pEntry->bmsk_ip |= l3_util_ipSufLen2Mask_ret(pRoute->suffix_len);
        pEntry->host_route = (pRoute->prefix_len >= RTK_L3_HOST_IP_PREFIX_LENGTH)? 1 : 0;
        pEntry->dflt_route = (pRoute->prefix_len == 0)? 1 : 0;
    }
    pEntry->dst_null_intf = (pRoute->flags & RTK_L3_FLAG_NULL_INTF)? 1 : 0;
    pEntry->ttl_dec = (pRoute->flags & RTK_L3_FLAG_TTL_DEC_IGNORE)? 0 : 1;
    pEntry->ttl_chk = (pRoute->flags & RTK_L3_FLAG_TTL_CHK_IGNORE)? 0 : 1;
    pEntry->qos_en = (pRoute->flags & RTK_L3_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pRoute->qos_pri;

    pEntry->nh_ecmp_idx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(pRoute->path_id);

    L3_ACTION_TO_VALUE(_actRouteEntryUC, pEntry->act, pRoute->fwd_act, "", errHandle, ret);

errHandle:
    return ret;
}

static inline int32 l3_util_routeEntry2rtkRoute(rtk_l3_route_t *pRoute, dal_longan_l3_routeEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pRoute, 0x00, sizeof(rtk_l3_route_t));
    pRoute->vrf_id = 0;
    if (pEntry->entry_type >= 2)    /* IPv6 */
    {
        pRoute->flags |= RTK_L3_FLAG_IPV6;
        pRoute->ip_addr.ipv6 = pEntry->ip6;
        /* pRoute->prefix_len = rt_util_ip6Mask2Length_ret(&pEntry->bmsk_ip6);  */ /*get from software database */
    }
    else
    {
        pRoute->ip_addr.ipv4 = pEntry->ip;
        /* pRoute->prefix_len = rt_util_ipMask2Length_ret(pEntry->bmsk_ip); */ /*get from software database */
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

    pRoute->path_id = DAL_LONGAN_L3_NH_IDX_TO_PATH_ID(pEntry->nh_ecmp_idx);

    L3_VALUE_TO_ACTION(_actRouteEntryUC, pRoute->fwd_act, pEntry->act, "", errHandle, ret);

errHandle:
    return ret;
}

static inline int32 l3_util_hostEntryCmp(dal_longan_l3_hostEntry_t *pEntry1, dal_longan_l3_hostEntry_t *pEntry2)
{
    if ((NULL == pEntry1) || (NULL == pEntry2))
        return RT_ERR_FAILED;

    if (pEntry1->entry_type != pEntry2->entry_type)
        return RT_ERR_FAILED;

    if (2 == pEntry1->entry_type)
    {
        if (RT_ERR_OK != rt_util_ipv6Cmp(&pEntry1->ip6, &pEntry2->ip6))
            return RT_ERR_FAILED;
    }
    else
    {
        if (pEntry1->ip != pEntry2->ip)
            return RT_ERR_FAILED;
    }

    if ((pEntry1->valid != pEntry2->valid) ||
         (pEntry1->act != pEntry2->act) ||
         (pEntry1->dst_null_intf != pEntry2->dst_null_intf) ||
         (pEntry1->nh_ecmp_idx != pEntry2->nh_ecmp_idx) ||
         (pEntry1->ttl_dec != pEntry2->ttl_dec) ||
         (pEntry1->ttl_chk != pEntry2->ttl_chk) ||
         (pEntry1->qos_en != pEntry2->qos_en) ||
         (pEntry1->qos_pri != pEntry2->qos_pri))
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

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
    if ((ret = table_read(unit, LONGAN_VLANt, vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L3|MOD_DAL), "");
        return ret;
    }

    if (FALSE == boolMcast)
    {
        /* get unicast lookup mode */
        if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_L2_HKEY_UCASTtf, \
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
        if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_L2_HKEY_MCASTtf, \
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
        /* MAC mode */
        field = (FALSE == boolMcast)? LONGAN_UC_SVL_FIDf : LONGAN_MC_SVL_FIDf;

        /* get FID from CHIP */
        if ((ret = reg_field_read(unit, LONGAN_VLAN_CTRLr, field, pFid) ) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L3|MOD_DAL), "");
            return ret;
        }
    }

    return ret;
}

/* Function Name:
 *      dal_longan_l3_nullIntf_nhEntry_init
 * Description:
 *      Allocate 3 number L3 nexthop entry for ACL null interface application.
 * Input:
 *      unit   - unit id
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
static int32
dal_longan_l3_nullIntf_nhEntry_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  nhIdxArry[][2] = {{DAL_LONGAN_NULLINTF_DROP_NEXTHOP_IDX, 0x7fff},
                                        {DAL_LONGAN_NULLINTF_TRAP2CPU_NEXTHOP_IDX, 0x7ffe},
                                        {DAL_LONGAN_NULLINTF_TRAP2MASTER_NEXTHOP_IDX, 0x7ffd}};
    int32 idx;
    dal_longan_l3_nhEntry_t nhEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    for (idx = 0; idx < sizeof(nhIdxArry)/(2*sizeof(uint32)); idx++)
    {
        BITMAP_SET(_pL3Db[unit]->HW.nexthop_used, nhIdxArry[idx][0]);
        _pL3Db[unit]->HW.nexthop_used_count += 1;

        osal_memset(&nhEntry, 0x00, sizeof(dal_longan_l3_nhEntry_t));
        nhEntry.dmac_idx = nhIdxArry[idx][1];
        RT_ERR_HDL(_dal_longan_l3_nhEntry_set(unit, nhIdxArry[idx][0], &nhEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
    }

    /* this nexthop index reserved for host/route entry trap/drop use , it can not set dst_null_intf */
    BITMAP_SET(_pL3Db[unit]->HW.nexthop_used, DAL_LONGAN_INVALID_NEXTHOP_IDX);
    _pL3Db[unit]->HW.nexthop_used_count += 1;

errHandle:

    return ret;
}

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
int32
dal_longan_l3Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->l3_init = dal_longan_l3_init;
#if CODE_TBC
    pMapper->l3_routeEntry_get = dal_longan_l3_routeEntry_get;
    pMapper->l3_routeEntry_set = dal_longan_l3_routeEntry_set;
#endif
    pMapper->l3_info_get = dal_longan_l3_info_get;
    pMapper->l3_routerMacEntry_get = dal_longan_l3_routerMacEntry_get;
    pMapper->l3_routerMacEntry_set = dal_longan_l3_routerMacEntry_set;
    pMapper->l3_intf_create = dal_longan_l3_intf_create;
    pMapper->l3_intf_destroy = dal_longan_l3_intf_destroy;
    pMapper->l3_intf_destroyAll = dal_longan_l3_intf_destroyAll;
    pMapper->l3_intf_get = dal_longan_l3_intf_get;
    pMapper->l3_intf_set = dal_longan_l3_intf_set;
    pMapper->l3_vrrp_add = dal_longan_l3_vrrp_add;
    pMapper->l3_vrrp_del = dal_longan_l3_vrrp_del;
    pMapper->l3_vrrp_delAll = dal_longan_l3_vrrp_delAll;
    pMapper->l3_vrrp_get = dal_longan_l3_vrrp_get;
    pMapper->l3_nextHop_create = dal_longan_l3_nextHop_create;
    pMapper->l3_nextHop_destroy = dal_longan_l3_nextHop_destroy;
    pMapper->l3_nextHop_get = dal_longan_l3_nextHop_get;
    pMapper->l3_nextHopPath_find = dal_longan_l3_nextHopPath_find;
    pMapper->l3_host_add = dal_longan_l3_host_add;
    pMapper->l3_host_del = dal_longan_l3_host_del;
    pMapper->l3_host_del_byNetwork = dal_longan_l3_host_del_byNetwork;
    pMapper->l3_host_del_byIntfId = dal_longan_l3_host_del_byIntfId;
    pMapper->l3_host_delAll = dal_longan_l3_host_delAll;
    pMapper->l3_host_find = dal_longan_l3_host_find;
    pMapper->l3_hostConflict_get = dal_longan_l3_hostConflict_get;
    pMapper->l3_host_age = dal_longan_l3_host_age;
    pMapper->l3_host_getNext = dal_longan_l3_host_getNext;
    pMapper->l3_route_add = dal_longan_l3_route_add;
    pMapper->l3_route_del = dal_longan_l3_route_del;
    pMapper->l3_route_get = dal_longan_l3_route_get;
    pMapper->l3_route_del_byIntfId = dal_longan_l3_route_del_byIntfId;
    pMapper->l3_route_delAll = dal_longan_l3_route_delAll;
    pMapper->l3_route_age = dal_longan_l3_route_age;
    pMapper->l3_route_getNext = dal_longan_l3_route_getNext;
    pMapper->l3_globalCtrl_get = dal_longan_l3_globalCtrl_get;
    pMapper->l3_globalCtrl_set = dal_longan_l3_globalCtrl_set;
    pMapper->l3_intfCtrl_get = dal_longan_l3_intfCtrl_get;
    pMapper->l3_intfCtrl_set = dal_longan_l3_intfCtrl_set;
    pMapper->l3_portCtrl_get = dal_longan_l3_portCtrl_get;
    pMapper->l3_portCtrl_set = dal_longan_l3_portCtrl_set;

    return RT_ERR_OK;
}


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
int32
dal_longan_l3_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  host_l3_table0_alg, host_l3_table1_alg;

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

    /* allocate memory that we need */
    if ((_pL3Db[unit] = osal_alloc(sizeof(dal_longan_l3_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "out of memory");
        return RT_ERR_FAILED;
    }
    osal_memset(_pL3Db[unit], 0x00, sizeof(dal_longan_l3_drvDb_t));

    L3_ROUTE_IDX_MAX(unit) = DAL_LONGAN_L3_ROUTE_TBL_SIZE -1;
#if LONGAN_L3_ROUTE_RSVMC_ENTRY
    L3_ROUTE_IDX_MAX(unit) -= 16;
#endif

#if LONGAN_L3_ROUTE_IPMC_SIZE
    /* IPMC space */
    _pL3Db[unit]->l3_route_ipmc_idx_base    = L3_ROUTE_IDX_MAX(unit) - LONGAN_L3_ROUTE_IPMC_SIZE + 1;
    _pL3Db[unit]->l3_route_ipmc_size        = LONGAN_L3_ROUTE_IPMC_SIZE;
    L3_ROUTE_IDX_MAX(unit) -= LONGAN_L3_ROUTE_IPMC_SIZE;
#endif

    L3_ROUTE_TBL_SIZE(unit) = L3_ROUTE_IDX_MAX(unit) - L3_ROUTE_IDX_MIN(unit) + 1;

    _pL3Db[unit]->HW.ip4_mtu[DAL_LONGAN_L3_RESERVED_INTF_MTU_IDX].mtu_value = RTK_DEFAULT_L3_INTF_MTU;
    _pL3Db[unit]->HW.ip4_mtu[DAL_LONGAN_L3_RESERVED_INTF_MTU_IDX].ref_count = 0; /*no owner used */
    _pL3Db[unit]->HW.ip4_mtu_used_count = 0;

    _pL3Db[unit]->HW.ip6_mtu[DAL_LONGAN_L3_RESERVED_INTF_MTU_IDX].mtu_value = RTK_DEFAULT_L3_INTF_MTU;
    _pL3Db[unit]->HW.ip6_mtu[DAL_LONGAN_L3_RESERVED_INTF_MTU_IDX].ref_count = 0; /*no owner used */
    _pL3Db[unit]->HW.ip6_mtu_used_count = 0;

    _pL3Db[unit]->nexthop_find_method = L3_NEXTHOP_FIND;

    _pL3Db[unit]->HW.rtmac_key = L3_RTMAC_KEY;
    if (IS_L3_RTMAC_CARE_VID(unit))
    {
        HAL_MAX_NUM_OF_INTF(unit) = 64;  /*route mac care VID, the used intf num is 64*/
        HAL_MAX_NUM_OF_L3_MCAST_GROUP_NEXTHOP(unit) = 64 + 2;  /* mcast nexthop max size is oil(bind intf) and bridge and stack nexthop*/
    }

    /* reference count check state init */
    _pL3Db[unit]->refer_cnt_chk_en = LONGAN_L3_REFCNT_CHK_DEFAULT;
    /* set init flag to complete init */
    l3_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        RT_ERR_HDL(l3_util_intfMtu_set(unit, DAL_LONGAN_L3_RESERVED_INTF_MTU_IDX, RTK_DEFAULT_L3_INTF_MTU), errHandle, ret);
        RT_ERR_HDL(l3_util_intfIp6Mtu_set(unit, DAL_LONGAN_L3_RESERVED_INTF_MTU_IDX, RTK_DEFAULT_L3_INTF_MTU), errHandle, ret);


        /* configure default hash algorithm */
        host_l3_table0_alg = L3_TABLE0_ALG;
        host_l3_table1_alg = L3_TABLE1_ALG;
        L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_0f, host_l3_table0_alg, "", errHandle, ret);
        L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_1f, host_l3_table1_alg, "", errHandle, ret);

        /*init ACL for null interface & and set reserved nhIdx*/
        RT_ERR_HDL(dal_longan_l3_nullIntf_nhEntry_init(unit), errHandle, ret);
    }

#if LONGAN_L3_ROUTE_RSVMC_ENTRY
    RT_ERR_HDL(dal_longan_l3_route_rsv4Mcast_add(unit), errHandle, ret);
    RT_ERR_HDL(dal_longan_l3_route_rsv6Mcast_add(unit), errHandle, ret);
#endif

errHandle:
    return ret;
}   /* end of dal_longan_l3_init */


/* Function Name:
 *      _dal_longan_l3_intfEgrMac_set
 * Description:
 *      Set the specifed L3 next hop DMAC entry.
 * Input:
 *      unit - unit id
 *      index - entry index
 *      mac - the egress DMAC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - invalid index
 * Note:
 */
int32
_dal_longan_l3_intfEgrMac_get(uint32 unit, uint32 index, rtk_mac_t *pMac)
{
    int32 ret = RT_ERR_OK;
    l3_egr_int_entry_t l3Egr_macEntry;

    RT_PARAM_CHK(index >= DAL_LONGAN_L3_NEXTHOP_MAX + DAL_LONGAN_L3_MAC_MAX, RT_ERR_OUT_OF_RANGE);

    osal_memset(&l3Egr_macEntry, 0x00, sizeof(l3_egr_int_entry_t));

    L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, index, l3Egr_macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, \
        LONGAN_L3_EGR_INTF_MAC_MACtf, *pMac, l3Egr_macEntry, "", errHandle, ret);


errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_l3_intfEgrMac_set
 * Description:
 *      Set the specifed L3 next hop DMAC entry.
 * Input:
 *      unit - unit id
 *      index - entry index
 *      mac - the egress DMAC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - invalid index
 * Note:
 */
int32
_dal_longan_l3_intfEgrMac_set(uint32 unit, uint32 index, rtk_mac_t mac)
{
    int32 ret = RT_ERR_OK;
    l3_egr_int_entry_t l3Egr_macEntry;

    RT_PARAM_CHK(index >= DAL_LONGAN_L3_NEXTHOP_MAX + DAL_LONGAN_L3_MAC_MAX, RT_ERR_OUT_OF_RANGE);

    osal_memset(&l3Egr_macEntry, 0x00, sizeof(l3_egr_int_entry_t));

    L3_TABLE_FIELD_MAC_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, \
        LONGAN_L3_EGR_INTF_MAC_MACtf, mac, l3Egr_macEntry, "", errHandle, ret);

    L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, index, l3Egr_macEntry, "", errHandle, ret);

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_l3_nhDmac_set
 * Description:
 *      Set the specifed L3 SMAC entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      mac - the egress SMAC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - invalid index
 * Note:
 */
int32
_dal_longan_l3_nhDmac_set(uint32 unit, uint32 index, rtk_mac_t mac)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK(index >= DAL_LONGAN_L3_NEXTHOP_MAX, RT_ERR_OUT_OF_RANGE);

    RT_ERR_HDL(_dal_longan_l3_intfEgrMac_set(unit, index, mac), errHandle, ret);

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_l3_nhDmac_set
 * Description:
 *      Set the specifed L3 SMAC entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      mac - the egress SMAC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - invalid index
 * Note:
 */
int32
_dal_longan_l3_nhDmac_get(uint32 unit, uint32 index, rtk_mac_t *pMac)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK(index >= DAL_LONGAN_L3_NEXTHOP_MAX, RT_ERR_OUT_OF_RANGE);

    RT_ERR_HDL(_dal_longan_l3_intfEgrMac_get(unit, index, pMac), errHandle, ret);

errHandle:
    return ret;
}


/* Function Name:
 *      _dal_longan_l3_smac_set
 * Description:
 *      Set the specifed L3 SMAC entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      mac - the egress SMAC
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - invalid index
 * Note:
 */
int32
_dal_longan_l3_smac_set(uint32 unit, uint32 index, rtk_mac_t mac)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK(index >= DAL_LONGAN_L3_MAC_MAX, RT_ERR_OUT_OF_RANGE);

    RT_ERR_HDL(_dal_longan_l3_intfEgrMac_set(unit, DAL_LONGAN_L3_SMAC_IDX(index), mac), errHandle, ret);

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_l3_macEntry_get
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
_dal_longan_l3_macEntry_get(uint32 unit, uint32 index, dal_longan_l3_macEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    routing_mac_entry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= DAL_LONGAN_L3_MAC_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);
#if 0
    /* pre-check */
    if (_pL3Db[unit]->HW.route_mac[index].ref_count == 0)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }
#endif
    L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, index, macEntry, "", errHandle, ret);

    /* load data */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_VALIDtf, pEntry->valid, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_PORT_TYPEtf, pEntry->port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_PORT_IDtf, pEntry->port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_INTF_IDtf, pEntry->intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_MACtf, pEntry->mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_L3_INTFtf, pEntry->l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_PORT_TYPEtf, pEntry->bmsk_port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_PORT_IDtf, pEntry->bmsk_port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_INTF_IDtf, pEntry->bmsk_intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_MACtf, pEntry->bmsk_mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_L3_INTFtf, pEntry->bmsk_l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_ACTIONtf, pEntry->act, macEntry, "", errHandle, ret);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_macEntry_set
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
_dal_longan_l3_macEntry_set(uint32 unit, uint32 index, dal_longan_l3_macEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    routing_mac_entry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= DAL_LONGAN_L3_MAC_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */

    /* prepare data */
    osal_memset(&macEntry,0x00,sizeof(routing_mac_entry_t));

    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_VALIDtf, pEntry->valid, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_PORT_TYPEtf, pEntry->port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_PORT_IDtf, pEntry->port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_INTF_IDtf, pEntry->intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_MACtf, pEntry->mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_L3_INTFtf, pEntry->l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_PORT_TYPEtf, pEntry->bmsk_port_type, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_PORT_IDtf, pEntry->bmsk_port_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_INTF_IDtf, pEntry->bmsk_intf_id, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_MAC_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_MACtf, pEntry->bmsk_mac, macEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_BMSK_L3_INTFtf, pEntry->bmsk_l3_intf, macEntry, "", errHandle, ret);

    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, \
        LONGAN_L3_ROUTER_MAC_ACTIONtf, pEntry->act, macEntry, "", errHandle, ret);

    /* write into chip (MAC and interface are 1-to-1 mapping) */
    L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_ROUTER_MACt, index, macEntry, "", errHandle, ret);

errHandle:

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_macEntry_alloc
 * Description:
 *      Allocate an route mac entry index for reference.
 * Input:
 *      unit  - unit id
 *      pMacEntry   - pointer to route mac entry
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_macEntry_alloc(uint32 unit, dal_longan_l3_macEntry_t *pMacEntry, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pMacEntry=%p,pIdx=%p", unit, pMacEntry, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitalb MAC entry */
    for (idx=0; idx<DAL_LONGAN_L3_MAC_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.route_mac[idx].ref_count > 0)
        {
            if (IS_L3_RTMAC_CARE_VID(unit))
            {
                if((RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.route_mac[idx].mac.octet, pMacEntry->mac.octet))
                &&(_pL3Db[unit]->HW.route_mac[idx].intf_id == pMacEntry->intf_id))
                {
                    _pL3Db[unit]->HW.route_mac[idx].ref_count += 1;
                    *pIdx = idx;
                    goto errOk;
                }
            }
            else
            {
                if (RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.route_mac[idx].mac.octet, pMacEntry->mac.octet))
                {
                    _pL3Db[unit]->HW.route_mac[idx].ref_count += 1;
                    *pIdx = idx;
                    goto errOk;
                }
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new router MAC entry */
    if (emptyEntryFound)
    {
        osal_memcpy(_pL3Db[unit]->HW.route_mac[emptyEntryIdx].mac.octet, pMacEntry->mac.octet,ETHER_ADDR_LEN);
        _pL3Db[unit]->HW.route_mac[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.mac_used_count += 1;
        if (IS_L3_RTMAC_CARE_VID(unit))
        {
            _pL3Db[unit]->HW.route_mac[emptyEntryIdx].intf_id = pMacEntry->intf_id;
        }

        RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, emptyEntryIdx, pMacEntry, 0),errExit, ret);

        *pIdx = emptyEntryIdx;
        goto errOk;
    }
    else
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }

errExit:
errTblFull:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_macEntry_free
 * Description:
 *      Release an MAC entry index reference.
 * Input:
 *      unit  - unit id
 *      pMacEntry   - pointer to route mac
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_macEntry_free(uint32 unit, dal_longan_l3_macEntry_t *pMacEntry)
{
    int32 ret = RT_ERR_OK;
    int32 idx;
    dal_longan_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pMacEntry=%p", unit, pMacEntry);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    for (idx=0; idx<DAL_LONGAN_L3_MAC_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.route_mac[idx].ref_count > 0)
        {
            /* only compare the all mac bit and intf_id */
            if (IS_L3_RTMAC_CARE_VID(unit))
            {
                if ((RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.route_mac[idx].mac.octet, pMacEntry->mac.octet))
                    &&(_pL3Db[unit]->HW.route_mac[idx].intf_id == pMacEntry->intf_id))
                    {
                        break;
                    }
            }
            else
            {
                if (RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.route_mac[idx].mac.octet, pMacEntry->mac.octet))
                {
                    break;
                }
            }
        }
    }

    if (idx >= DAL_LONGAN_L3_MAC_MAX)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* recycle the Mac entry */
    if (_pL3Db[unit]->HW.route_mac[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.route_mac[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.route_mac[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.mac_used_count -= 1;
            osal_memset(&macEntry, 0x00, sizeof(dal_longan_l3_macEntry_t));
            RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, idx, &macEntry, DAL_LONGAN_L3_API_FLAG_NONE), errMacEntrySet, ret);
        }
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

errEntryNotFound:
errMacEntrySet:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_macEntry_realloc
 * Description:
 *      Re-allocate an MAC entry index for reference.
 * Input:
 *      unit   - unit id
 *      pNewMacEntry - pointer to new mac entry
 *      pIdx   - pointer to current entry index
 * Output:
 *      pIdx   - pointer to new entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter is null pointer
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_macEntry_realloc(uint32 unit, dal_longan_l3_macEntry_t *pNewMacEntry, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIdx;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;
    dal_longan_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pNewMacEntry=%p,pIdx=%p", unit, pNewMacEntry, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pNewMacEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNewMacEntry->intf_id > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;

    /* search a suitalb MAC entry */
    for (idx=0; idx<DAL_LONGAN_L3_MAC_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.route_mac[idx].ref_count > 0)
        {
            if (IS_L3_RTMAC_CARE_VID(unit))
            {
            if ((RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.route_mac[idx].mac.octet, pNewMacEntry->mac.octet)) &&  \
                (_pL3Db[unit]->HW.route_mac[idx].intf_id == pNewMacEntry->intf_id))
                {
                    _pL3Db[unit]->HW.route_mac[idx].ref_count += 1;
                    *pIdx = idx;

                    /* release the old entry */
                    goto freeOldMacEntry;
                }
            }
            else
            {
                if (RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.route_mac[idx].mac.octet, pNewMacEntry->mac.octet))
                {
                    _pL3Db[unit]->HW.route_mac[idx].ref_count += 1;
                    *pIdx = idx;

                    /* release the old entry */
                    goto freeOldMacEntry;
                }
            }


        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new mac entry */
    if (_pL3Db[unit]->HW.route_mac[oldIdx].ref_count == 1)
    {
        /* just change the MTU value with the original MAC entry */
        osal_memcpy(_pL3Db[unit]->HW.route_mac[oldIdx].mac.octet, pNewMacEntry->mac.octet,ETHER_ADDR_LEN);
        _pL3Db[unit]->HW.route_mac[oldIdx].intf_id = pNewMacEntry->intf_id;

        RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, oldIdx, pNewMacEntry, 0),errExit, ret);
        *pIdx = oldIdx;

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        osal_memcpy(_pL3Db[unit]->HW.route_mac[emptyEntryIdx].mac.octet, pNewMacEntry->mac.octet,ETHER_ADDR_LEN);
        _pL3Db[unit]->HW.route_mac[emptyEntryIdx].intf_id = pNewMacEntry->intf_id;
        _pL3Db[unit]->HW.route_mac[emptyEntryIdx].ref_count = 1;

        RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, emptyEntryIdx, pNewMacEntry, 0),errExit, ret);

        *pIdx = emptyEntryIdx;
        goto freeOldMacEntry;
    }
    else
    {
        ret = RT_ERR_INPUT;
        goto errInput;
    }

freeOldMacEntry:
    if (_pL3Db[unit]->HW.route_mac[oldIdx].ref_count > 0)
    {
        _pL3Db[unit]->HW.route_mac[oldIdx].ref_count -= 1;
        if (_pL3Db[unit]->HW.route_mac[oldIdx].ref_count == 0)
        {
            _pL3Db[unit]->HW.mac_used_count -= 1;
            osal_memset(&macEntry, 0x00, sizeof(dal_longan_l3_macEntry_t));
            RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, oldIdx, &macEntry, DAL_LONGAN_L3_API_FLAG_NONE), errExit, ret);
        }
        goto errOk;
    }


errInput:
errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_smacEntry_alloc
 * Description:
 *      Allocate an source mac entry index for reference.
 * Input:
 *      unit  - unit id
 *      pMacEntry   - pointer to route mac entry
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_smacEntry_alloc(uint32 unit, rtk_mac_t mac, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pIdx=%d", unit, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (IS_L3_RTMAC_CARE_VID(unit))
    {
        emptyEntryFound = TRUE;
        emptyEntryIdx = *pIdx; /* used by adding a new entry */
    }
    else
    {
        /* search a suitalb MAC entry */
        for (idx=0; idx<DAL_LONGAN_L3_MAC_MAX; idx++)
        {
            if (_pL3Db[unit]->HW.source_mac[idx].ref_count > 0)
            {
                if (RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.source_mac[idx].mac.octet, mac.octet))
                {
                    _pL3Db[unit]->HW.source_mac[idx].ref_count += 1;
                    *pIdx = idx;
                    goto errOk;
                }
            }
            else if (FALSE == emptyEntryFound)
            {
                emptyEntryFound = TRUE;
                emptyEntryIdx = idx; /* used by adding a new entry */
            }
        }
    }


    /* try to add the new router MAC entry */
    if (emptyEntryFound)
    {
        osal_memcpy(_pL3Db[unit]->HW.source_mac[emptyEntryIdx].mac.octet, mac.octet,ETHER_ADDR_LEN);
        _pL3Db[unit]->HW.source_mac[emptyEntryIdx].ref_count = 1;

        *pIdx = emptyEntryIdx;
        goto errOk;
    }
    else
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }

errTblFull:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_macEntry_free
 * Description:
 *      Release an smac entry index reference.
 * Input:
 *      unit  - unit id
 *      pMacEntry   - pointer to route mac
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_smacEntry_free(uint32 unit, uint32 idx)
{
    int32 ret = RT_ERR_OK;
    rtk_mac_t mac;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_MAC_MAX), RT_ERR_INPUT);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* recycle the Mac entry */
    if (_pL3Db[unit]->HW.source_mac[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.source_mac[idx].ref_count -= 1;
        if(_pL3Db[unit]->HW.source_mac[idx].ref_count == 0)
        {
            osal_memset(&mac, 0x00, sizeof(rtk_mac_t));
            RT_ERR_HDL(_dal_longan_l3_intfEgrMac_set(unit, DAL_LONGAN_L3_SMAC_IDX(idx), mac), errHandle, ret);
        }
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

errEntryNotFound:
errHandle:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_smacEntry_realloc
 * Description:
 *      Reallocate an source mac entry index for reference.
 * Input:
 *      unit  - unit id
 *      pMacEntry   - pointer to route mac entry
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_smacEntry_realloc(uint32 unit, rtk_mac_t mac, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;
    rtk_mac_t null_mac;
    uint32  oldIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pIdx=%d", unit, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;

    if (IS_L3_RTMAC_CARE_VID(unit))
    {
        emptyEntryFound = TRUE;
        emptyEntryIdx = *pIdx; /* used by adding a new entry */
    }
    else
    {
        /* search a suitalb MAC entry */
        for (idx=0; idx<DAL_LONGAN_L3_MAC_MAX; idx++)
        {
            if (_pL3Db[unit]->HW.source_mac[idx].ref_count > 0)
            {
                if (RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.source_mac[idx].mac.octet, mac.octet))
                {
                    _pL3Db[unit]->HW.source_mac[idx].ref_count += 1;
                    *pIdx = idx;
                    goto freeOldMacEntry;
                }
            }
            else if (FALSE == emptyEntryFound)
            {
                emptyEntryFound = TRUE;
                emptyEntryIdx = idx; /* used by adding a new entry */
            }
        }
    }


    /* try to add the new mac entry */
    if (_pL3Db[unit]->HW.source_mac[oldIdx].ref_count == 1)
    {
        /* just change original MAC entry */
        osal_memcpy(_pL3Db[unit]->HW.source_mac[oldIdx].mac.octet, mac.octet,ETHER_ADDR_LEN);

        *pIdx = oldIdx;

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        osal_memcpy(_pL3Db[unit]->HW.source_mac[emptyEntryIdx].mac.octet, mac.octet,ETHER_ADDR_LEN);
        _pL3Db[unit]->HW.source_mac[emptyEntryIdx].ref_count = 1;

        *pIdx = emptyEntryIdx;
        goto freeOldMacEntry;
    }
    else
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }

freeOldMacEntry:
    if (_pL3Db[unit]->HW.source_mac[oldIdx].ref_count > 0)
    {
        _pL3Db[unit]->HW.source_mac[oldIdx].ref_count -= 1;
        if (_pL3Db[unit]->HW.source_mac[oldIdx].ref_count == 0)
        {
            osal_memset(&null_mac, 0x00, sizeof(rtk_mac_t));
            RT_ERR_HDL(_dal_longan_l3_intfEgrMac_set(unit, DAL_LONGAN_L3_SMAC_IDX(oldIdx), null_mac), errHandle, ret);
        }
        goto errOk;
    }

errTblFull:
errHandle:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_dmacEntry_alloc
 * Description:
 *      Allocate an destination mac entry index for reference.
 * Input:
 *      unit  - unit id
 *      pMacEntry   - pointer to route mac entry
 *      pIdx  - pointer to entry index
 * Output:
 *      pIdx  - pointer to entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NULL_POINTER   - input parameter is null pointer
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_dmacEntry_alloc(uint32 unit, rtk_mac_t mac, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pIdx=%d", unit, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitalb MAC entry */
    for (idx=0; idx<DAL_LONGAN_L3_NEXTHOP_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.dst_mac[idx].ref_count > 0)
        {
            if (RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->HW.dst_mac[idx].mac.octet, mac.octet))
            {
                _pL3Db[unit]->HW.dst_mac[idx].ref_count += 1;
                *pIdx = idx;
                goto errOk;
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new router MAC entry */
    if (emptyEntryFound)
    {
        osal_memcpy(_pL3Db[unit]->HW.dst_mac[emptyEntryIdx].mac.octet, mac.octet,ETHER_ADDR_LEN);
        _pL3Db[unit]->HW.dst_mac[emptyEntryIdx].ref_count = 1;

        *pIdx = emptyEntryIdx;
        goto errOk;
    }
    else
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }

errTblFull:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_macEntry_free
 * Description:
 *      Release an dmac entry index reference.
 * Input:
 *      unit  - unit id
 *      pMacEntry   - pointer to route mac
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_dmacEntry_free(uint32 unit, uint32 idx)
{
    int32 ret = RT_ERR_OK;
    rtk_mac_t mac;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    //RT_PARAM_CHK((idx >= DAL_LONGAN_L3_NEXTHOP_MAX), RT_ERR_INPUT);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* recycle the Mac entry */
    if (_pL3Db[unit]->HW.dst_mac[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.dst_mac[idx].ref_count -= 1;
        if(_pL3Db[unit]->HW.dst_mac[idx].ref_count == 0)
        {
            osal_memset(&mac, 0x00, sizeof(rtk_mac_t));
            RT_ERR_HDL(_dal_longan_l3_nhDmac_set(unit, (idx), mac), errHandle, ret);
        }
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

errEntryNotFound:
errHandle:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_INPUT        - invalid input
 *      RT_ERR_ENTRY_EXIST  - entry exists
 *      RT_ERR_NULL_POINTER - null pointer
 *      RT_ERR_TBL_FULL     - table is full
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
int32
_dal_longan_l3_intfEntry_alloc(uint32 unit, uint32 *pIndex, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    uint32 entryIdx;
    int32 startIdx, endIdx;

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

    if (flags & DAL_LONGAN_L3_API_FLAG_WITH_ID)
    {
        entryIdx = *pIndex; /* caller specified */

        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, entryIdx))
        {
            _pL3Db[unit]->intf[entryIdx].intf_id = entryIdx;

            BITMAP_SET(_pL3Db[unit]->HW.intf_used, entryIdx);
            _pL3Db[unit]->HW.intf_used_count += 1;

            goto errOk;
        }

        ret = RT_ERR_ENTRY_EXIST;
        goto errEntryExist;
    }

    RT_ERR_HDL(l3_util_inValidBitIdx_range_get(_pL3Db[unit]->HW.intf_used, HAL_MAX_NUM_OF_INTF(unit), &startIdx, &endIdx), errHandle,ret);

    /* search an empty entry */
    for (entryIdx=startIdx; entryIdx<endIdx; entryIdx++)
    {
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, entryIdx))
        {
            _pL3Db[unit]->intf[entryIdx].intf_id = entryIdx;

            BITMAP_SET(_pL3Db[unit]->HW.intf_used, entryIdx);
            _pL3Db[unit]->HW.intf_used_count += 1;

            *pIndex = entryIdx;

            goto errOk;
        }
    }

    ret = RT_ERR_TBL_FULL;

errEntryExist:
errTblFull:
errOk:
errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_intfEntry_free(uint32 unit, uint32 index, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,flags=0x%08x", unit, index, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, index))
    {
        /* release entry */
        BITMAP_CLEAR(_pL3Db[unit]->HW.intf_used, index);
        _pL3Db[unit]->HW.intf_used_count -= 1;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_intfEgrEntry_get(uint32 unit, uint32 index, dal_longan_l3_intfEgrEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_egr_int_entry_t egrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
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
    L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, index, egrIntf, "", errHandle, ret);

    /* load data */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_DST_VIDtf, pEntry->dst_vid, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_SMAC_IDXtf, pEntry->smac_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP_MTU_IDXtf, pEntry->ip_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6_MTU_IDXtf, pEntry->ip6_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IPMC_TTL_SCOPEtf, pEntry->ipmc_ttl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6MC_HL_SCOPEtf, pEntry->ip6mc_hl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, pEntry->ip_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, pEntry->ip6_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip6_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_intfEgrEntry_set(uint32 unit, uint32 index, dal_longan_l3_intfEgrEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;
    l3_egr_intf_entry_t egrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
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
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_DST_VIDtf, pEntry->dst_vid, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_SMAC_IDXtf, pEntry->smac_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP_MTU_IDXtf, pEntry->ip_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6_MTU_IDXtf, pEntry->ip6_mtu_idx, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IPMC_TTL_SCOPEtf, pEntry->ipmc_ttl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6MC_HL_SCOPEtf, pEntry->ip6mc_hl_scope, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, pEntry->ip_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, pEntry->ip6_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, \
        LONGAN_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, pEntry->ip6_pbr_icmp_redirect_act, egrIntf, "", errHandle, ret);

    /* write into chip */
    L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_EGR_INTFt, index, egrIntf, "", errHandle, ret);

    /* sync shadow info */
    _pL3Db[unit]->intf[index].vid        = pEntry->dst_vid;
    _pL3Db[unit]->intf[index].ip_mtu_idx = pEntry->ip_mtu_idx;
    _pL3Db[unit]->intf[index].ip6_mtu_idx = pEntry->ip6_mtu_idx;

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_intfEntry_get(uint32 unit, uint32 index, dal_longan_l3_intfEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    RT_ERR_HDL(_dal_longan_l3_intfEgrEntry_get(unit, index, &pEntry->egrIntf, flags), errHandle, ret);

errHandle:

    return ret;
}

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
int32
_dal_longan_l3_intfEntry_set(uint32 unit, uint32 index, dal_longan_l3_intfEntry_t *pEntry, uint32 flags)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    RT_ERR_HDL(_dal_longan_l3_intfEgrEntry_set(unit, index, &pEntry->egrIntf, flags), errHandle, ret);

errHandle:
    return ret;
}

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
int32
_dal_longan_l3_intf_mtuEntry_alloc(uint32 unit, uint32 mtu, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,mtu=%d,pIdx=%p", unit, mtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip4_mtu[idx].mtu_value == mtu)
            {
                _pL3Db[unit]->HW.ip4_mtu[idx].ref_count += 1;
                *pIdx = idx;
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
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].mtu_value = mtu;
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip4_mtu_used_count += 1;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, emptyEntryIdx, mtu), errExit, ret);

        *pIdx = emptyEntryIdx;
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
_dal_longan_l3_intf_mtuIp6Entry_alloc(uint32 unit, uint32 mtu, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,mtu=%d,pIdx=%p", unit, mtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip6_mtu[idx].mtu_value == mtu)
            {
                _pL3Db[unit]->HW.ip6_mtu[idx].ref_count += 1;
                *pIdx = idx;

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
int32
_dal_longan_l3_ipmc_mtuEntry_alloc(uint32 unit, uint32 mtu, rtk_mcast_group_t group, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;
    uint32  groupIdx = RTK_MCAST_GROUP_ID_IDX(group);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,mtu=%d,pIdx=%p", unit, mtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip4_mtu[idx].mtu_value == mtu)
            {
                _pL3Db[unit]->HW.ip4_mtu[idx].ref_count += 1;
                *pIdx = idx;

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
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].mtu_value = mtu;
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip4_mtu_used_count += 1;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, emptyEntryIdx, mtu), errExit, ret);

        *pIdx = emptyEntryIdx;
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
int32
_dal_longan_l3_ipmc_mtuIp6Entry_alloc(uint32 unit, uint32 mtu, rtk_mcast_group_t group, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;
    uint32  groupIdx = RTK_MCAST_GROUP_ID_IDX(group);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,mtu=%d,pIdx=%p", unit, mtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP6_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip6_mtu[idx].mtu_value == mtu)
            {
                _pL3Db[unit]->HW.ip6_mtu[idx].ref_count += 1;
                *pIdx = idx;
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
int32
_dal_longan_l3_intf_mtuEntry_free(uint32 unit, uint32 idx)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_IP_MTU_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* recycle the MTU entry */
    if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip4_mtu[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.ip4_mtu_used_count -= 1;
        }
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }


errEntryNotFound:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_intf_mtuIp6Entry_free(uint32 unit, uint32 idx)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_IP6_MTU_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* recycle the MTU entry */
    if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip6_mtu[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.ip6_mtu_used_count -= 1;
        }
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }


errEntryNotFound:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_ipmc_mtuEntry_free(uint32 unit, uint32 idx, rtk_mcast_group_t group)
{
    int32 ret = RT_ERR_OK;
    uint32  groupIdx = RTK_MCAST_GROUP_ID_IDX(group);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_IP_MTU_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* recycle the MTU entry */
    if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip4_mtu[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.ip4_mtu_used_count -= 1;
        }
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

errEntryNotFound:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_ipmc_mtuIp6Entry_free
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
int32
_dal_longan_l3_ipmc_mtuIp6Entry_free(uint32 unit, uint32 idx, rtk_mcast_group_t group)
{
    int32 ret = RT_ERR_OK;
    uint32  groupIdx = RTK_MCAST_GROUP_ID_IDX(group);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_IP6_MTU_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* recycle the MTU entry */
    if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip6_mtu[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.ip6_mtu_used_count -= 1;
        }
        goto errOk;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }
errEntryNotFound:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_mtuEntry_free
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
int32
_dal_longan_l3_mtuEntry_free(uint32 unit, uint32 idx)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (idx >= DAL_LONGAN_L3_IP_MTU_MAX)
    {
        ret = RT_ERR_OUT_OF_RANGE;
        goto errInput;
    }

    /* recycle the MTU entry */
    if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count > 0)
    {
        _pL3Db[unit]->HW.ip4_mtu[idx].ref_count -= 1;
        if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count == 0)
        {
            _pL3Db[unit]->HW.ip4_mtu_used_count -= 1;
        }
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
 *      _dal_longan_l3_mtuIp6Entry_free
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
int32
_dal_longan_l3_mtuIp6Entry_free(uint32 unit, uint32 idx)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (idx >= DAL_LONGAN_L3_IP6_MTU_MAX)
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
int32
_dal_longan_l3_intf_mtuEntry_realloc(uint32 unit, uint32 newMtu, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIdx;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,newMtu=%d,pIdx=%p", unit, newMtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip4_mtu[idx].mtu_value == newMtu)
            {
                _pL3Db[unit]->HW.ip4_mtu[idx].ref_count += 1;
                *pIdx = idx;

                /* release the old entry */
                goto freeOldIp4MtuEntry;
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new MTU entry */
    if (_pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count == 1)
    {
        /* just change the MTU value with the original MTU entry */
        _pL3Db[unit]->HW.ip4_mtu[oldIdx].mtu_value = newMtu;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, oldIdx, newMtu), errExit, ret);

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].mtu_value = newMtu;
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip4_mtu_used_count++;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, emptyEntryIdx, newMtu), errExit, ret);
        *pIdx = emptyEntryIdx;

        goto freeOldIp4MtuEntry;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
        goto errIntfMtuVarietyExceeds;
    }

freeOldIp4MtuEntry:
        if (_pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count > 0)
        {
            _pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count -= 1;
            if (_pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count == 0)
            {
                _pL3Db[unit]->HW.ip4_mtu_used_count -= 1;
            }
            goto errOk;
        }


errIntfMtuVarietyExceeds:
errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l3_intf_mtuIp6Entry_realloc
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
int32
_dal_longan_l3_intf_mtuIp6Entry_realloc(uint32 unit, uint32 newMtu, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIdx;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,newMtu=%d,pIdx=%p", unit, newMtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP6_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip6_mtu[idx].mtu_value == newMtu)
            {
                _pL3Db[unit]->HW.ip6_mtu[idx].ref_count += 1;
                *pIdx = idx;

                /* release the old entry */
                goto freeOldIp6MtuEntry;
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

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].mtu_value = newMtu;
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip6_mtu_used_count++;

        RT_ERR_HDL(l3_util_intfIp6Mtu_set(unit, emptyEntryIdx, newMtu), errExit, ret);
        *pIdx = emptyEntryIdx;

        goto freeOldIp6MtuEntry;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
        goto errIntfMtuVarietyExceeds;
    }

freeOldIp6MtuEntry:
        if (_pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count > 0)
        {
            _pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count -= 1;
            if (_pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count == 0)
            {
                _pL3Db[unit]->HW.ip6_mtu_used_count -= 1;
            }
            goto errOk;
        }


errIntfMtuVarietyExceeds:
errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_ipmc_mtuEntry_realloc(uint32 unit, uint32 newMtu, rtk_mcast_group_t group, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIdx;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;
    uint32  groupIdx = RTK_MCAST_GROUP_ID_IDX(group);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,newMtu=%d,pIdx=%p", unit, newMtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip4_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip4_mtu[idx].mtu_value == newMtu)
            {
                _pL3Db[unit]->HW.ip4_mtu[idx].ref_count += 1;
                *pIdx = idx;

                /* release the old entry */
                goto freeOldIp4MtuEntry;
            }
        }
        else if (FALSE == emptyEntryFound)
        {
            emptyEntryFound = TRUE;
            emptyEntryIdx = idx; /* used by adding a new entry */
        }
    }

    /* try to add the new MTU entry */
    if (_pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count == 1)
    {
        /* just change the MTU value with the original MTU entry */
        _pL3Db[unit]->HW.ip4_mtu[oldIdx].mtu_value = newMtu;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, oldIdx, newMtu), errExit, ret);

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].mtu_value = newMtu;
        _pL3Db[unit]->HW.ip4_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip4_mtu_used_count++;

        RT_ERR_HDL(l3_util_intfMtu_set(unit, emptyEntryIdx, newMtu), errExit, ret);
        *pIdx = emptyEntryIdx;

        goto freeOldIp4MtuEntry;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
        goto errIntfMtuVarietyExceeds;
    }

freeOldIp4MtuEntry:
        if (_pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count > 0)
        {
            _pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count -= 1;
            if (_pL3Db[unit]->HW.ip4_mtu[oldIdx].ref_count == 0)
            {
                _pL3Db[unit]->HW.ip4_mtu_used_count -= 1;
            }
            goto errOk;
        }


errIntfMtuVarietyExceeds:
errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_ipmc_mtuIp6Entry_realloc(uint32 unit, uint32 newMtu, rtk_mcast_group_t group, uint32 *pIdx)
{
    int32   ret = RT_ERR_OK;
    uint32  oldIdx;
    uint32  idx;
    uint32  emptyEntryFound = FALSE;
    uint32  emptyEntryIdx = 0;
    uint32  groupIdx = RTK_MCAST_GROUP_ID_IDX(group);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,newMtu=%d,pIdx=%p", unit, newMtu, pIdx);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    oldIdx = *pIdx;

    /* search a suitalb MTU entry */
    for (idx=0; idx<DAL_LONGAN_L3_IP6_MTU_MAX; idx++)
    {
        if (_pL3Db[unit]->HW.ip6_mtu[idx].ref_count > 0)
        {
            if (_pL3Db[unit]->HW.ip6_mtu[idx].mtu_value == newMtu)
            {
                _pL3Db[unit]->HW.ip6_mtu[idx].ref_count += 1;
                *pIdx = idx;

                /* release the old entry */
                goto freeOldIp6MtuEntry;
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

        goto errOk;
    }
    else if (emptyEntryFound)
    {
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].mtu_value = newMtu;
        _pL3Db[unit]->HW.ip6_mtu[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.ip6_mtu_used_count++;

        RT_ERR_HDL(l3_util_intfIp6Mtu_set(unit, emptyEntryIdx, newMtu), errExit, ret);
        *pIdx = emptyEntryIdx;

        goto freeOldIp6MtuEntry;
    }
    else
    {
        ret = RT_ERR_INTF_MTU_VARIETY_EXCEEDS;
        goto errIntfMtuVarietyExceeds;
    }

freeOldIp6MtuEntry:
        if (_pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count > 0)
        {
            _pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count -= 1;
            if (_pL3Db[unit]->HW.ip6_mtu[oldIdx].ref_count == 0)
            {
                _pL3Db[unit]->HW.ip6_mtu_used_count -= 1;
            }
            goto errOk;
        }


errIntfMtuVarietyExceeds:
errExit:
errOk:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}
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
int32
_dal_longan_l3_nhEntry_alloc(uint32 unit, uint32 *pIndex, dal_longan_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  index;
    int32   startIdx, endIdx;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pIndex=%p,flags=0x%08x", unit, pIndex, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (flags & DAL_LONGAN_L3_API_FLAG_WITH_ID)
    {
        index = *pIndex;

        if (DAL_LONGAN_INVALID_NEXTHOP_IDX == index)
        {
            ret = RT_ERR_INPUT;
            goto errInput;
        }

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

     /* search an empty entry */
    RT_ERR_HDL(l3_util_inValidBitIdx_range_get(_pL3Db[unit]->HW.nexthop_used, (DAL_LONGAN_L3_NEXTHOP_MAX-3), &startIdx, &endIdx), errHandle,ret);

    /* alloc a new interface for */
    for (index=startIdx; index<endIdx; index++)
    {
        if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.nexthop_used, index))
        {
            BITMAP_SET(_pL3Db[unit]->HW.nexthop_used, index);

            _pL3Db[unit]->HW.nexthop_used_count += 1;
            _pL3Db[unit]->HW.nexthop[index].flags = flags;  /* logging */

            *pIndex = index;

            goto errOk;
        }
    }

    ret = RT_ERR_TBL_FULL;

errOk:
errInput:
errEntryExist:
errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_nhEntry_free(uint32 unit, uint32 index, dal_longan_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=0x%08x,flags=0x%08x", unit, index, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_LONGAN_L3_NEXTHOP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_pL3Db[unit]->HW.nexthop_used, index))
    {
        _pL3Db[unit]->HW.nexthop[index].flags = flags;  /* logging */
        _pL3Db[unit]->HW.nexthop_used_count -= 1;
        BITMAP_CLEAR(_pL3Db[unit]->HW.nexthop_used, index);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_nhEntry_get(uint32 unit, uint32 index, dal_longan_l3_nhEntry_t *pEntry, dal_longan_l3_api_flag_t flags)
{
    int32 ret = RT_ERR_OK;
    l3_nexthop_entry_t nhEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pEntry=%p,flags=0x%08x", unit, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_LONGAN_L3_NEXTHOP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* read from chip */
    L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_NEXTHOPt, index, nhEntry, "", errHandle, ret);

    /* get fields */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_NEXTHOPt, \
        LONGAN_L3_NEXTHOP_DMAC_IDXtf, pEntry->dmac_idx, nhEntry, "", errHandle, ret);
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_NEXTHOPt, \
        LONGAN_L3_NEXTHOP_L3_EGR_INTF_IDXtf, pEntry->l3_egr_intf_idx, nhEntry, "", errHandle, ret);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;

}

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
int32
_dal_longan_l3_nhEntry_set(uint32 unit, uint32 index, dal_longan_l3_nhEntry_t *pEntry, dal_longan_l3_api_flag_t flags)
{
    int32 ret = RT_ERR_OK;
    l3_nexthop_entry_t nhEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pEntry=%p,flags=0x%08x", unit, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_LONGAN_L3_NEXTHOP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* set fields */
    osal_memset(&nhEntry, 0x00, sizeof(l3_nexthop_entry_t));
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_NEXTHOPt, \
        LONGAN_L3_NEXTHOP_DMAC_IDXtf, pEntry->dmac_idx, nhEntry, "", errHandle, ret);
    L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_NEXTHOPt, \
        LONGAN_L3_NEXTHOP_L3_EGR_INTF_IDXtf, pEntry->l3_egr_intf_idx, nhEntry, "", errHandle, ret);

    /* write into chip */
    L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_NEXTHOPt, index, nhEntry, "", errHandle, ret);

errHandle:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_nhEntryPathId_get(uint32 unit, uint32 index, rtk_l3_pathId_t *pPathId)
{
    int32 ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%u,pPathId=%p", unit, index, pPathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_L3_NEXTHOP(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    if (BITMAP_IS_SET(_pL3Db[unit]->HW.nexthop_used, index))
    {
        /* nhIdx (nexthop) to pathId */
        *pPathId = DAL_LONGAN_L3_NH_IDX_TO_PATH_ID(index);
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

    L3_INT_SEM_UNLOCK(unit);

    return ret;
}


static uint32
_dal_longan_l3_hostHash0_ret(uint32 unit, dal_longan_l3_hostHashKey_t *pHashKey, dal_longan_l3_api_flag_t flags, uint32 move_dip_0_8)
{
    uint32  ipv6 = (flags & DAL_LONGAN_L3_API_FLAG_IPV6);
    uint32  hashRow[15];
    uint32  hashIdx;

    osal_memset(hashRow, 0x0, sizeof(hashRow));
    /* DIP */
    if (ipv6)
    {
        hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[0], 6, 2) << 0);
        hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[0], 0, 6) << 3) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[1], 5, 3) << 0);
        hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[1], 0, 5) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 4, 4) << 0);
        hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 0, 4) << 5) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 3, 5) << 0);
        hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 0, 3) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[4], 2, 6) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[4], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[5], 1, 7) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[5], 0, 1) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[6], 0, 8) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[7], 0, 8) << 1) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 7, 1) << 0);
        hashRow[8] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 0, 7) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[9], 6, 2) << 0);
        hashRow[9] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[9], 0, 6) << 3) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[10], 5, 3) << 0);
        hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[10], 0, 5) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[11], 4, 4) << 0);
        hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[11], 0, 4) << 5) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 3, 5) << 0);
        hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 0, 3) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 2, 6) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 1, 7) << 0);
        if (!move_dip_0_8)
        {
            hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 0, 1) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[15], 0, 8) << 0);
        }

        /* result */
        hashIdx = hashRow[0]  ^ hashRow[1]  ^ hashRow[2]  ^ hashRow[3]  ^ hashRow[4]  ^ \
                        hashRow[5]  ^ hashRow[6]  ^ hashRow[7]  ^ hashRow[8]  ^ hashRow[9]  ^ \
                        hashRow[10] ^ hashRow[11] ^ hashRow[12] ^ hashRow[13] ^ hashRow[14];
    }
    else
    {
        hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 27, 5) << 0);
        hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 18, 9) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 9, 9) << 0);
        if (!move_dip_0_8)
        {
            hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4,  0, 9) << 0);
        }

        /* result */
        hashIdx = hashRow[11]  ^ hashRow[12]  ^ hashRow[13]  ^ hashRow[14];
    }

    return hashIdx;
}

static uint32
_dal_longan_l3_hostHash1_ret(uint32 unit, dal_longan_l3_hostHashKey_t *pHashKey, dal_longan_l3_api_flag_t flags, uint32 move_dip_0_8)
{
    uint32  ipv6 = (flags & DAL_LONGAN_L3_API_FLAG_IPV6);
    uint32  sum, sum1, preHash;
    uint32  hashRow[16];
    uint32  hashIdx;

    osal_memset(hashRow, 0x0, sizeof(hashRow));

    if (ipv6)
    {
        hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[0], 6, 2) << 0);
        hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[0], 0, 6) << 3) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[1], 5, 3) << 0);
        hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[1], 0, 5) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 4, 4) << 0);
        hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[2], 0, 4) << 5) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 3, 5) << 0);
        hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[3], 0, 3) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[4], 2, 6) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[4], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[5], 1, 7) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[5], 0, 1) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[6], 0, 8) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[7], 0, 8) << 1) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 7, 1) << 0);
        hashRow[8] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[8], 0, 7) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[9], 6, 2) << 0);
        hashRow[9] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[9], 0, 6) << 3) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[10], 5, 3) << 0);
        hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[10], 0, 5) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[11], 4, 4) << 0);
        hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[11], 0, 4) << 5);

        hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 3, 5) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[12], 0, 3) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 2, 6) << 0);
        hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[13], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 1, 7) << 0);
        if (!move_dip_0_8)
        {
            hashRow[15] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[14], 0, 1) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dip.ipv6.octet[15], 0, 8) << 0);
        }
    }
    else
    {
        hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 27, 5) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 18, 9) << 0);
        hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4, 9, 9) << 0);
        if (!move_dip_0_8)
        {
            hashRow[15] = (HASH_BIT_EXTRACT(pHashKey->dip.ipv4,  0, 9) << 0);
        }
    }

    sum = hashRow[12] + hashRow[13] + hashRow[14]; /* dip[9:0] is not calc */
    sum1 = (sum & 0x1FF) + ((sum & (0x1FF << 9)) >> 9);
    preHash = (sum1 & 0x1FF) + ((sum1 & (0x1FF << 9)) >> 9);

    /* result */
    hashIdx = hashRow[0]  ^ hashRow[1]  ^ hashRow[2]  ^ hashRow[3]  ^ hashRow[4]  ^ \
                    hashRow[5]  ^ hashRow[6]  ^ hashRow[7]  ^ hashRow[8]  ^ hashRow[9]  ^ \
                    hashRow[10] ^ hashRow[11] ^ preHash ^ hashRow[15];
    return hashIdx;

}

static int32
_dal_longan_l3_dip_update(uint32 unit, uint32 hw_entry_idx, dal_longan_l3_hostEntry_t *pHostEntry, dal_longan_l3_api_flag_t flag)
{
    int32 ret = RT_ERR_OK;
    dal_longan_l3_hashIdx_t hashIdx;
    uint32  hashIdxTbl[DAL_LONGAN_L3_HOST_TBL_NUM];
    uint32  tbl;
    uint32  dip_0_8;
    uint32  addr,  addrHashIdx;
    dal_longan_l3_hostHashKey_t hashKey;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHostEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((hw_entry_idx >= DAL_LONGAN_L3_HOST_TBL_SIZE), RT_ERR_OUT_OF_RANGE);


    osal_memset(&hashIdx, 0x0, sizeof(dal_longan_l3_hashIdx_t));
    osal_memset(&hashKey, 0x0, sizeof(dal_longan_l3_hostHashKey_t));

    L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_0f, hashKey.alg_of_tbl[0], "", errHandle, ret);
    L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_1f, hashKey.alg_of_tbl[1], "", errHandle, ret);
    hashKey.vrf_id = 0;
    if (flag & DAL_LONGAN_L3_API_FLAG_IPV6)
    {
        hashKey.dip.ipv6 = pHostEntry->ip6;
    }
    else
    {
        hashKey.dip.ipv4 = pHostEntry->ip;
    }

    hashIdx.hashIdx_of_alg[0] = _dal_longan_l3_hostHash0_ret(unit, &hashKey, flag, TRUE);
    hashIdx.hashIdx_of_alg[1] = _dal_longan_l3_hostHash1_ret(unit, &hashKey, flag, TRUE);

    /* calculate hash index for each table */
    for (tbl=0; tbl<DAL_LONGAN_L3_HOST_TBL_NUM; tbl++)
    {
        hashIdxTbl[tbl] = hashIdx.hashIdx_of_alg[(hashKey.alg_of_tbl[tbl] & 0x1)];
    }

    addr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(hw_entry_idx);
    tbl = (addr & (0x1 << 12)) >> 12;
    addrHashIdx = (addr & (0x1FF << 3)) >> 3;
    dip_0_8 = hashIdxTbl[tbl] ^ addrHashIdx;

    if (flag & DAL_LONGAN_L3_API_FLAG_IPV6)
    {
        pHostEntry->ip6.octet[15] = dip_0_8;
        pHostEntry->ip6.octet[14] = (pHostEntry->ip6.octet[14] & 0xfe) | (dip_0_8 >> 8);
    }
    else
    {
        pHostEntry->ip = (pHostEntry->ip & 0xfffffe00) | dip_0_8;
    }

errHandle:
    return ret;
}

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
int32
_dal_longan_l3_hostHashIdx_get(uint32 unit, dal_longan_l3_hostHashKey_t *pHashKey, dal_longan_l3_hostHashIdx_t *pHashIdx, dal_longan_l3_api_flag_t flags)
{
    uint32  tbl;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHashKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHashIdx), RT_ERR_NULL_POINTER);

    /* calculate hash index for each table */
    for (tbl=0; tbl<DAL_LONGAN_L3_HOST_TBL_NUM; tbl++)
    {
        if (0 == pHashKey->alg_of_tbl[tbl])
        {
            pHashIdx->idx_of_tbl[tbl] = _dal_longan_l3_hostHash0_ret(unit, pHashKey, flags, FALSE);
        }
        else if (1 == pHashKey->alg_of_tbl[tbl])
        {
            pHashIdx->idx_of_tbl[tbl] = _dal_longan_l3_hostHash1_ret(unit, pHashKey, flags, FALSE);
        }
        else
        {
            return RT_ERR_INPUT;
        }
    }

    return RT_ERR_OK;
}

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
int32
_dal_longan_l3_hostEntry_alloc(uint32 unit, dal_longan_l3_hostAlloc_t *pAlloc, uint32 *pIndex, dal_longan_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  width, entryMask, currentMask, evalMask;
    uint32  addr, tbl, idx, cost;
    int32   bestCost = 0, bestTbl = 0, bestIdx = 0, bestSlot = 0;
    int32   entryType, slot;
    static uint32 hostValidmask2Value[] = { /* value for various valid_bitmask */
        24 /* for b000000 */,   12 /* for b000001 */,   12 /* for b000010 */,   11 /* for b000011 */,
        12 /* for b000100 */,    9 /* for b000101 */,    9 /* for b000110 */,    8 /* for b000111 */,
        12 /* for b001000 */,    6 /* for b001001 */,    6 /* for b001010 */,    5 /* for b001011 */,
        8  /* for b001100 */,    5 /* for b001101 */,    5 /* for b001110 */,    4 /* for b001111 */,
        12 /* for b010000 */,    6 /* for b010001 */,    6 /* for b010010 */,    5 /* for b010011 */,
        6  /* for b010100 */,    3 /* for b010101 */,    3 /* for b010110 */,    2 /* for b010111 */,
        9  /* for b011000 */,    3 /* for b011001 */,    3 /* for b011010 */,    2 /* for b011011 */,
        5  /* for b011100 */,    2 /* for b011101 */,    2 /* for b011110 */,    1 /* for b011111 */,
        12 /* for b100000 */,    6 /* for b100001 */,    6 /* for b100010 */,    5 /* for b100011 */,
        6  /* for b100100 */,    3 /* for b100101 */,    3 /* for b100110 */,    2 /* for b100111 */,
        9  /* for b101000 */,    3 /* for b101001 */,    3 /* for b101010 */,    2 /* for b101011 */,
        5  /* for b101100 */,    2 /* for b101101 */,    2 /* for b101110 */,    1 /* for b101111 */,
        11 /* for b110000 */,    5 /* for b110001 */,    5 /* for b110010 */,    4 /* for b110011 */,
        5  /* for b110100 */,    2 /* for b110101 */,    2 /* for b110110 */,    1 /* for b110111 */,
        8  /* for b111000 */,    2 /* for b111001 */,    2 /* for b111010 */,    1 /* for b111011 */,
        4  /* for b111100 */,    1 /* for b111101 */,    1 /* for b111110 */,    0 /* for b111111 */
        };

    static int32 bestSlotIdxArray[][4]  = { /* value for index best slot posit */
        {0,0,0,0} /* for b000000 */,    {1,2,3,-1}/* for b000001 */,    {0,2,3,-1} /* for b000010 */,   {2,2,3,-1}/* for b000011 */,
        {0,0,3,-1} /* for b000100 */,    {1,4,3,-1} /* for b000101 */,    {0,4,3,-1} /* for b000110 */,    {3,4,3,-1}/* for b000111 */,
        {0,0,0,-1} /* for b001000 */,    {1,4,-1,-1} /* for b001001 */,    {0,4,-1,-1} /* for b001010 */,    {2,4,-1,-1} /* for b001011 */,
        {0,0,-1,-1}  /* for b001100 */,   {1,4,-1,-1} /* for b001101 */,    {0,4,-1,-1} /* for b001110 */,    {4,4,-1,-1} /* for b001111 */,
        {0,0,0,-1} /* for b010000 */,    {1,2,-1,-1} /* for b010001 */,    {0,2,-1,-1} /* for b010010 */,    {2,2,-1,-1} /* for b010011 */,
        {0,0,-1,-1}  /* for b010100 */,   {1,-1,-1,-1} /* for b010101 */,    {0,-1,-1,-1} /* for b010110 */,    {3,-1,-1,-1} /* for b010111 */,
        {0,0,0,-1}  /* for b011000 */,   {1,-1,-1,-1} /* for b011001 */,    {0,-1,-1,-1} /* for b011010 */,    {2,-1,-1,-1} /* for b011011 */,
        {0,0,-1,-1}  /* for b011100 */,   {1,-1,-1,-1} /* for b011101 */,    {0,-1,-1,-1} /* for b011110 */,    {5,-1,-1,-1} /* for b011111 */,
        {0,0,0,-1} /* for b100000 */,    {1,2,-1,-1} /* for b100001 */,    {0,2,-1,-1} /* for b100010 */,    {2,2,-1,-1} /* for b100011 */,
        {0,0,-1,-1}  /* for b100100 */,   {1,-1,-1,-1} /* for b100101 */,    {0,-1,-1,-1} /* for b100110 */,    {3,-1,-1,-1} /* for b100111 */,
        {0,0,0,-1}  /* for b101000 */,   {1,-1,-1,-1} /* for b101001 */,    {0,-1,-1,-1} /* for b101010 */,    {2,-1,-1,-1} /* for b101011 */,
        {0,0,-1,-1}  /* for b101100 */,   {1,-1,-1,-1} /* for b101101 */,    {0,-1,-1,-1} /* for b101110 */,    {4,-1,-1,-1} /* for b101111 */,
        {0,0,0,-1} /* for b110000 */,    {1,2,-1,-1} /* for b110001 */,    {0,2,-1,-1} /* for b110010 */,    {2,2,-1,-1} /* for b110011 */,
        {0,0,-1,-1}  /* for b110100 */,   {1,-1,-1,-1} /* for b110101 */,    {0,-1,-1,-1} /* for b110110 */,    {3,-1,-1,-1} /* for b110111 */,
        {0,0,0,-1}  /* for b111000 */,   {1,-1,-1,-1} /* for b111001 */,    {0,-1,-1,-1} /* for b111010 */,    {2,-1,-1,-1} /* for b111011 */,
        {0,0,-1,-1}  /* for b111100 */,   {1,-1,-1,-1} /* for b111101 */,    {0,-1,-1,-1} /* for b111110 */,    {-1,-1,-1,-1} /* for b111111 */
        };
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pAlloc=%p,pIndex=%p,flags=0x%08x", unit, pAlloc, pIndex, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAlloc), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
     /* find a space with the specified width */
    width = pAlloc->width;
    if (-1 == (entryType = width2entryType[width]))
        return RT_ERR_FAILED;

    /* function body */
    L3_INT_SEM_LOCK(unit);

    entryMask = (0x1 << width) - 1;

    for (tbl=0; tbl<DAL_LONGAN_L3_HOST_TBL_NUM; tbl++)
    {
        idx = (pAlloc->hashIdx.idx_of_tbl[tbl] % DAL_LONGAN_L3_HOST_TBL_HEIGHT);
        currentMask = _pL3Db[unit]->HW.hostTable[tbl].row[idx].valid_mask;

        if (-1 != (slot = bestSlotIdxArray[currentMask][entryType]))
        {
            evalMask = entryMask << slot;
            /* cost evaluation */
            evalMask |= currentMask;
            cost = hostValidmask2Value[currentMask%(0x1<<6)] - hostValidmask2Value[evalMask%(0x1<<6)];

            if ((0 == bestCost) || (cost < bestCost))
            {
                bestCost = cost;
                bestTbl = tbl;
                bestIdx = idx;
                bestSlot = slot;
            }
        }
    }

    if (bestCost)
    {
        /* take the best position for the entry */
        _pL3Db[unit]->HW.hostTable[bestTbl].row[bestIdx].slot[bestSlot].width = width;
        _pL3Db[unit]->HW.hostTable[bestTbl].row[bestIdx].valid_mask |= (entryMask << bestSlot);
        _pL3Db[unit]->HW.host_used_count += width;

        addr = (bestTbl << 12) | (bestIdx << 3) | (bestSlot << 0);
        *pIndex = DAL_LONGAN_L3_ENTRY_ADDR_TO_IDX(addr);
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
int32
_dal_longan_l3_hostEntry_free(uint32 unit, uint32 index, dal_longan_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  addr, tbl, idx, slot, width, entryMask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,flags=0x%08x", unit, index, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    addr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(index);
    tbl = (addr & (0x1 << 12)) >> 12;
    idx = (addr & (0x1FF << 3)) >> 3;
    slot = (addr & 0x7);
    RT_PARAM_CHK((tbl >= DAL_LONGAN_L3_HOST_TBL_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_HOST_TBL_HEIGHT), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((slot >= DAL_LONGAN_L3_HOST_TBL_WIDTH), RT_ERR_OUT_OF_RANGE);

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
 *      _dal_longan_l3_host_get
 * Description:
 *      Get an L3 host entry.
 * Input:
 *      unit   - unit id
 *      index  - pointer to entry index
 *      pEntry - pointer to entry
 *      pHost - pointer to host
 *      flag - l3 flag for RTK_L3_FLAG_READ_HIT_IGNORE
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
_dal_longan_l3_host_get(uint32 unit, uint32 index, rtk_l3_flag_t flag, dal_longan_l3_hostEntry_t *pEntry, rtk_l3_host_t *pHost)
{
    int32   ret;
    int32 retry = 0;
    int32 timeRetry = 0;
    dal_longan_l3_hostEntry_t tmpHostEntry;

    RT_PARAM_CHK((NULL == pEntry) || (NULL == pHost), RT_ERR_NULL_POINTER);

    if (CHIP_REV_ID_A == HWP_CHIP_REV(unit))
    {
        /* SS-432 */
        osal_memset(&tmpHostEntry, 0, sizeof(tmpHostEntry));
        RT_ERR_HDL(l3_util_rtkHost2hostEntry(&tmpHostEntry, pHost), errHandle, ret);
        if (flag & RTK_L3_FLAG_READ_HIT_IGNORE)
        {
            *pEntry = tmpHostEntry;
        }
        else
        {
            do {
                do {
                    ret = _dal_longan_l3_hostEntry_get(unit, index, pEntry, DAL_LONGAN_L3_API_FLAG_NONE);
                    if (RT_ERR_BUSYWAIT_TIMEOUT == ret)
                    {
                        if (timeRetry >= DAL_LONGAN_L3_TIMEOUT_TIMES)
                            return ret;

                        _dal_longan_ipmc_pkt_tx(unit);
                        timeRetry++;
                    }
                    }while (RT_ERR_BUSYWAIT_TIMEOUT == ret);

                    if (RT_ERR_OK != ret)
                        return ret;

                    ret = l3_util_hostEntryCmp(&tmpHostEntry, pEntry);
                    retry++;
                }while ((RT_ERR_OK != ret) && (retry < DAL_LONGAN_L3_ENTRY_READ_RETRY));
        }
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_hostEntry_get(unit, index, pEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
    }

errHandle:
    return ret;
}


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
int32
_dal_longan_l3_hostEntry_get(uint32 unit, uint32 index, dal_longan_l3_hostEntry_t *pEntry, dal_longan_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  addr, tbl, idx, slot, width;
    host_routing_entry_t entry;
    uint32 ip6[IPV6_ADDR_LEN >> 2];
    dal_longan_l3_api_flag_t    l3Flag = DAL_LONGAN_L3_API_FLAG_NONE;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    addr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(index);
    tbl = (addr & (0x1 << 12)) >> 12;
    idx = (addr & (0x1FF << 3)) >> 3;
    slot = (addr & 0x7);
    RT_PARAM_CHK((tbl >= DAL_LONGAN_L3_HOST_TBL_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_HOST_TBL_HEIGHT), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((slot >= DAL_LONGAN_L3_HOST_TBL_WIDTH), RT_ERR_OUT_OF_RANGE);
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

    osal_memset(ip6,0,sizeof(ip6));
    /* read the first entry from chip */
    RT_ERR_HDL(table_read(unit, LONGAN_L3_HOST_ROUTE_IPUCt, index, (uint32 *)&entry), errHandle, ret);

    /* load data */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
        LONGAN_L3_HOST_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);

    /* L3 entry */
    L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
        LONGAN_L3_HOST_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
    if (0 == pEntry->entry_type)
    {
        /* IPv4 unicast */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_IPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->ip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);
    }
    else if (1 == pEntry->entry_type)
    {
        /* IPv4 multicast */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, index, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_GIPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->gip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_SIPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->sip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_HITtf, pEntry->hit, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
    }
    else if (2 == pEntry->entry_type)
    {
        /* IPv6 unicast */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, index, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_IPtf, ip6[0], entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->ip6,ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_HITtf, pEntry->hit, entry, "", errHandle, ret);

        l3Flag = DAL_LONGAN_L3_API_FLAG_IPV6;
    }
    else if (3 == pEntry->entry_type)
    {
       /* IPv6 multicast */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, index, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_GIPtf, ip6[0], entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->gip6,ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_SIPtf, ip6[0], entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->sip6,ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);

        l3Flag = DAL_LONGAN_L3_API_FLAG_IPV6;
    }
    else
    {
        /* Unknown type */
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

    if (CHIP_REV_ID_A == HWP_CHIP_REV(unit))
    {
        if ((0 == pEntry->entry_type)  || (2 == pEntry->entry_type))
        {
            RT_ERR_HDL(_dal_longan_l3_dip_update(unit, index, pEntry, l3Flag), errHandle, ret);
        }
        else if ((1 == pEntry->entry_type)  || (3 == pEntry->entry_type))
        {
            RT_ERR_HDL(_dal_longan_ipmc_dip_update(unit, index, pEntry, l3Flag), errHandle, ret);
        }
    }
    else
    {
        if ((1 == pEntry->entry_type)  || (3 == pEntry->entry_type))
        {
            RT_ERR_HDL(_dal_longan_ipmc_dip_update(unit, index, pEntry, l3Flag), errHandle, ret);
        }
    }

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_hostEntry_set(uint32 unit, uint32 index, dal_longan_l3_hostEntry_t *pEntry, dal_longan_l3_api_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  addr, tbl, idx, slot, width;
    host_routing_entry_t entry;
    uint32 ip6[IPV6_ADDR_LEN >> 2];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p,flags=0x%08x", unit, index, pEntry, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    addr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(index);
    tbl = (addr & (0x1 << 12)) >> 12;
    idx = (addr & (0x1FF << 3)) >> 3;
    slot = (addr & 0x7);

    RT_PARAM_CHK((tbl >= DAL_LONGAN_L3_HOST_TBL_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((idx >= DAL_LONGAN_L3_HOST_TBL_HEIGHT), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((slot >= DAL_LONGAN_L3_HOST_TBL_WIDTH), RT_ERR_OUT_OF_RANGE);
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

    osal_memset(ip6,0,sizeof(ip6));
    /* prepare data */
     osal_memset(&entry, 0x00, sizeof(host_routing_entry_t));

    /* L3 entry */
    if (0 == pEntry->entry_type)
    {
        /* IPv4 unicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        ip6[0] = pEntry->ip;
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_IPtf, ip6[0], entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, \
            LONGAN_L3_HOST_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPUCt, index, entry, "", errHandle, ret);
    }
    else if (1 == pEntry->entry_type)
    {
        /* IPv4 multicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        ip6[0] = pEntry->sip;
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_SIPtf, ip6[0], entry, "", errHandle, ret);
        ip6[0] = pEntry->gip;
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_GIPtf, pEntry->gip, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, \
            LONGAN_L3_HOST_ROUTE_IPMC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IPMCt, index, entry, "", errHandle, ret);
    }
    else if (2 == pEntry->entry_type)
    {
        /* IPv6 unicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        ipv6toIpArray(ip6, &pEntry->ip6);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_IPtf, ip6, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, \
            LONGAN_L3_HOST_ROUTE_IP6UC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6UCt, index, entry, "", errHandle, ret);
    }
    else if (3 == pEntry->entry_type)
    {
        /* IPv6 multicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        ipv6toIpArray(ip6, &pEntry->sip6);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_SIPtf, ip6, entry, "", errHandle, ret);
        ipv6toIpArray(ip6, &pEntry->gip6);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_GIPtf, ip6, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, \
            LONGAN_L3_HOST_ROUTE_IP6MC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_ROUTE_IP6MCt, index, entry, "", errHandle, ret);
    }
    else
    {
        /* Unknown type */
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l3_host_find
 * Description:
 *      Find an L3 host entry based on IP address
 * Input:
 *      unit  - unit id
 *      pHost - pointer to the structure containing the basic inputs
 * Output:
 *      pIndex - pointer to the host index
 *      pHostEntry - pointer to the structure containing the asic host entry
 *       pHostAlloc -  pointer to the structure dal_longan_l3_hostAlloc_t entry
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
 *      *pIndex = -1 is entry not found
 *       if pHostEntry == NULL, will not return hostentry
 */
int32
_dal_longan_l3_hostEntry_find(uint32 unit, rtk_l3_host_t *pHost, dal_longan_l3_hostEntry_t* pHostEntry, dal_longan_l3_hostAlloc_t *pHostAlloc, int32 *pIndex)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    dal_longan_l3_hostEntry_t    hostEntry;
    dal_longan_l3_hostHashKey_t  hashKey;
    dal_longan_l3_hostAlloc_t    hostAlloc;
    uint32  flags;
    uint32  addr, tbl, slot;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    ipv6 = (pHost->flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
    *pIndex = -1;

    /* get hash index */
    osal_memset(&hashKey, 0x00, sizeof(dal_longan_l3_hostHashKey_t));
    osal_memset(&hostAlloc, 0x00, sizeof(dal_longan_l3_hostAlloc_t));
    L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_0f, hashKey.alg_of_tbl[0], "", errHandle, ret);
    L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_1f, hashKey.alg_of_tbl[1], "", errHandle, ret);
    hashKey.vrf_id = 0;
    if (pHost->flags & RTK_L3_FLAG_IPV6)
    {
        hashKey.dip.ipv6 = pHost->ip_addr.ipv6;
        hostAlloc.width = 3;
        flags = DAL_LONGAN_L3_API_FLAG_IPV6;
    }
    else
    {
        hashKey.dip.ipv4 = pHost->ip_addr.ipv4;
        hostAlloc.width = 1;
        flags = DAL_LONGAN_L3_API_FLAG_NONE;
    }
    RT_ERR_HDL(_dal_longan_l3_hostHashIdx_get(unit, &hashKey, &hostAlloc.hashIdx, flags), errHandle, ret);
    if (NULL != pHostAlloc)
    {
        *pHostAlloc = hostAlloc;
    }

    for (tbl=0; tbl<DAL_LONGAN_L3_HOST_TBL_NUM; tbl++)
    {
        for (slot=0; slot<DAL_LONGAN_L3_HOST_TBL_WIDTH; slot+=hostAlloc.width)
        {
            addr = ((tbl & 0x1) << 12) | ((hostAlloc.hashIdx.idx_of_tbl[tbl] & 0x1FF) << 3) | ((slot & 0x7) << 0);
            hostIdx = DAL_LONGAN_L3_ENTRY_ADDR_TO_IDX(addr);

            if ((_pL3Db[unit]->host[hostIdx].valid) && \
                (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
            {
                RT_ERR_HDL(_dal_longan_l3_host_get(unit, hostIdx, pHost->flags, &hostEntry, L3_ENTRY_HOST_ADDR(unit, hostIdx)), errHandle, ret);
                if ((((FALSE == ipv6) && \
                      (LONGAN_L3_HOST_IP_PFLEN == rt_util_ipMaxMatchLength_ret(hostEntry.ip, pHost->ip_addr.ipv4, LONGAN_L3_HOST_IP_PFLEN))) || \
                     ((TRUE == ipv6) && \
                      (LONGAN_L3_HOST_IP6_PFLEN == rt_util_ipv6MaxMatchLength_ret(&hostEntry.ip6, &pHost->ip_addr.ipv6, LONGAN_L3_HOST_IP6_PFLEN)))))
                {
                    if (NULL != pHostEntry)
                        *pHostEntry  = hostEntry;

                    /* Entry Found */
                    *pIndex = hostIdx;

                    goto errOk;
                }
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:

    return ret;
}   /* end of dal_longan_l3_host_find */


/* Function Name:
 *      _dal_longan_l3_route_get
 * Description:
 *      Get an L3 route entry.
 * Input:
 *      unit   - unit id
 *      index  - pointer to entry index
 *      pEntry - pointer to entry
 *      pRoute - pointer to route
 *      flag - l3 flag for RTK_L3_FLAG_READ_HIT_IGNORE
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
_dal_longan_l3_route_get(uint32 unit, uint32 index, rtk_l3_flag_t flag, dal_longan_l3_routeEntry_t *pEntry, rtk_l3_route_t *pRoute)
{
    int32   ret;
    dal_longan_l3_routeEntry_t tmpRouteEntry;

    RT_PARAM_CHK((NULL == pEntry) || (NULL == pRoute), RT_ERR_NULL_POINTER);

    if (CHIP_REV_ID_A == HWP_CHIP_REV(unit))
    {
        /* SS-432 */
        if (flag & RTK_L3_FLAG_READ_HIT_IGNORE)
        {
            osal_memset(&tmpRouteEntry, 0, sizeof(tmpRouteEntry));
            RT_ERR_HDL(l3_util_rtkRoute2routeEntry(&tmpRouteEntry, pRoute), errHandle, ret);
            *pEntry = tmpRouteEntry;
        }
        else
        {
            RT_ERR_HDL(_dal_longan_l3_routeEntry_get(unit, index, pEntry, ENABLED), errHandle, ret);
        }
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_routeEntry_get(unit, index, pEntry, ENABLED), errHandle, ret);
    }

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_l3_routeEntry_get
 * Description:
 *      Get an L3 route entry.
 * Input:
 *      unit   - unit id
 *      index  - entry index
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
int32
_dal_longan_l3_routeEntry_get(uint32 unit, uint32 index, dal_longan_l3_routeEntry_t *pEntry, rtk_enable_t idxChk)
{
    int32   ret = RT_ERR_OK;
    uint32 ip6[IPV6_ADDR_LEN >> 2];
    prefix_routing_entry_t entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p \n", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* parameter check */
    if (idxChk)
    {
        RT_PARAM_CHK((index > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    }
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* pre-check */
    if (index < L3_ROUTE_TBL_IP_CNT(unit))
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
    }
    else if ((index >= L3_ROUTE_IPV6_LAST_IDX(unit) && (!(IS_L3_ROUTE_IPV6_EMPTY(unit))))
        &&(index < L3_ROUTE_TBL_SIZE(unit)))
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
    }
#if LONGAN_L3_ROUTE_IPMC_SIZE
    else if ( (index >= _pL3Db[unit]->l3_route_ipmc_idx_base) &&
              (index < (_pL3Db[unit]->l3_route_ipmc_idx_base + _pL3Db[unit]->l3_route_ipmc_size)))
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
    }
#endif
#if LONGAN_L3_ROUTE_RSVMC_ENTRY
    else if ( (index >= DAL_LONGAN_L3_ROUTE_TBL_RSV_V4_MIN_IDX) &&
                (index <= DAL_LONGAN_L3_ROUTE_TBL_RSV_V4_MAX_IDX) )
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
    }
    else if ( (index >= DAL_LONGAN_L3_ROUTE_TBL_RSV_V6_MIN_IDX) &&
                (index <= DAL_LONGAN_L3_ROUTE_TBL_RSV_V6_MAX_IDX) )
    {
        /* read the first entry from chip */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, index, entry, "", errHandle, ret);

        /* load data */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
    }
#endif
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* L3 entry */
    if (0 == pEntry->entry_type)
    {
        /* IPv4 unicast */
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_BMSK_IPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->bmsk_ip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_IPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->ip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);
    }
    else if (2 == pEntry->entry_type)
    {
        /* IPv6 unicast */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, index, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_BMSK_IPtf, ip6, entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->bmsk_ip6, ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_IPtf, ip6, entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->ip6, ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_HITtf, pEntry->hit, entry, "", errHandle, ret);
    }
    else if (1 == pEntry->entry_type)
    {
        /* IPv4 multicast */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, index, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_GIPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->bmsk_gip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_GIPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->gip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_ROUNDtf, pEntry->round, entry, "", errHandle, ret);
        if (0 != pEntry->bmsk_vid)
        {
            L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
                LONGAN_L3_PREFIX_ROUTE_IPMC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        }
        else
        {
            L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
                LONGAN_L3_PREFIX_ROUTE_IPMC_RPF_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        }
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_SIPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->bmsk_sip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_SIPtf, ip6[0], entry, "", errHandle, ret);
        pEntry->sip = ip6[0];
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
    }
    else if (3 == pEntry->entry_type)
    {
        /* IPv6 multicast */
        L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, index, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_GIPtf, ip6, entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->bmsk_gip6, ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_GIPtf, ip6, entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->gip6, ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_ROUNDtf, pEntry->round, entry, "", errHandle, ret);
        if (0 != pEntry->bmsk_vid)
        {
            L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
                LONGAN_L3_PREFIX_ROUTE_IP6MC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        }
        else
        {
            L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
                LONGAN_L3_PREFIX_ROUTE_IP6MC_RPF_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        }
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_SIPtf, ip6, entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->bmsk_sip6, ip6);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_SIPtf, ip6, entry, "", errHandle, ret);
        ipArray2ipv6(&pEntry->sip6, ip6);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);

        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_HITtf, pEntry->hit, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_GET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
    }
    else
    {
        /* Unknown type */
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

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
int32
_dal_longan_l3_routeEntry_set(uint32 unit, uint32 index, dal_longan_l3_routeEntry_t *pEntry, rtk_enable_t idxChk)
{
    int32   ret = RT_ERR_OK;
    prefix_routing_entry_t entry;
    uint32 ip6[IPV6_ADDR_LEN >> 2];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p\n", unit, index, pEntry);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    if (idxChk)
    {
        RT_PARAM_CHK((index > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    }
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_INT_SEM_LOCK(unit);

    /* pre-check */
    if ((index >= L3_ROUTE_TBL_IP_CNT(unit)) && (index < L3_ROUTE_IPV6_LAST_IDX(unit)))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    osal_memset(&entry, 0x00, sizeof(l3_prefix_route_entry_t));
    osal_memset(ip6,0,sizeof(ip6));
    /* L3 entry */
    if (0 == pEntry->entry_type)
    {
        /* IPv4 unicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        ip6[0] = pEntry->bmsk_ip;
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_BMSK_IPtf, ip6[0], entry, "", errHandle, ret);
        ip6[0] = pEntry->ip;
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_IPtf, ip6[0], entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, \
            LONGAN_L3_PREFIX_ROUTE_IPUC_HITtf, pEntry->hit, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPUCt, index, entry, "", errHandle, ret);

    }
    else if (2 == pEntry->entry_type)
    {
        /* IPv6 unicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        ipv6toIpArray(ip6, &pEntry->bmsk_ip6);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_BMSK_IPtf, ip6, entry, "", errHandle, ret);
        ipv6toIpArray(ip6, &pEntry->ip6);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_IPtf, ip6, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_HOST_ROUTEtf, pEntry->host_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_DFLT_ROUTEtf, pEntry->dflt_route, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_DST_NULL_INTFtf, pEntry->dst_null_intf, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_ACTIONtf, pEntry->act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_NH_IDXtf, pEntry->nh_ecmp_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_TTL_DECtf, pEntry->ttl_dec, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_TTL_CHKtf, pEntry->ttl_chk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6UC_HITtf, pEntry->hit, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6UCt, index, entry, "", errHandle, ret);
    }
    else if (1 == pEntry->entry_type)
    {
        /* IPv4 multicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        ip6[0] = pEntry->bmsk_gip;
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_GIPtf, ip6[0], entry, "", errHandle, ret);
        ip6[0] = pEntry->gip;
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_GIPtf, ip6[0], entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_ROUNDtf, pEntry->round, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_RPF_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        if (0 == pEntry->round)
        {
            ip6[0] = pEntry->bmsk_sip;
            L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
                LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_SIPtf, ip6[0], entry, "", errHandle, ret);
            ip6[0] = pEntry->sip;
            L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
                LONGAN_L3_PREFIX_ROUTE_IPMC_SIPtf, ip6[0], entry, "", errHandle, ret);
        }
        else
        {
            ip6[0] = pEntry->bmsk_sip;
            L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
                LONGAN_L3_PREFIX_ROUTE_IPMC_BMSK_SIPtf, ip6[0], entry, "", errHandle, ret);
        }

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_HITtf, pEntry->hit, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, \
            LONGAN_L3_PREFIX_ROUTE_IPMC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IPMCt, index, entry, "", errHandle, ret);
    }
    else if (3 == pEntry->entry_type)
    {
        /* IPv6 multicast */
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_ENTRY_TYPEtf, pEntry->bmsk_entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_ENTRY_TYPEtf, pEntry->entry_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_IPMC_TYPEtf, pEntry->bmsk_ipmc_type, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_IPMC_TYPEtf, pEntry->ipmc_type, entry, "", errHandle, ret);
        ipv6toIpArray(ip6, &pEntry->bmsk_gip6);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_GIPtf, ip6, entry, "", errHandle, ret);
        ipv6toIpArray(ip6, &pEntry->gip6);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_GIPtf, ip6, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_ROUNDtf, pEntry->bmsk_round, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_ROUNDtf, pEntry->round, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_VIDtf, pEntry->bmsk_vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_RPF_VIDtf, pEntry->vid, entry, "", errHandle, ret);
        if (0 == pEntry->round)
        {
            ipv6toIpArray(ip6, &pEntry->bmsk_sip6);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
                LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_SIPtf, ip6, entry, "", errHandle, ret);
            ipv6toIpArray(ip6, &pEntry->sip6);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
                LONGAN_L3_PREFIX_ROUTE_IP6MC_SIPtf, ip6, entry, "", errHandle, ret);
        }
        else
        {
            ipv6toIpArray(ip6, &pEntry->bmsk_sip6);
            L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
                LONGAN_L3_PREFIX_ROUTE_IP6MC_BMSK_SIPtf, ip6, entry, "", errHandle, ret);
        }
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_VID_CMPtf, pEntry->vid_cmp, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_ENtf, pEntry->l2_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_ACTtf, pEntry->l2_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_MC_PMSK_IDXtf, pEntry->l2_mc_pmsk_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L3_ENtf, pEntry->l3_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_L3_ACTtf, pEntry->l3_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_RPF_CHK_ENtf, pEntry->rpf_chk_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_RPF_FAIL_ACTtf, pEntry->rpf_fail_act, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_OIL_IDXtf, pEntry->oil_idx, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_TTL_MINtf, pEntry->ttl_min, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_MTU_MAX_IDXtf, pEntry->mtu_max_idx, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_QOS_AStf, pEntry->qos_en, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_QOS_PRItf, pEntry->qos_pri, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_HITtf, pEntry->hit, entry, "", errHandle, ret);

        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_OIL_IDX_VALIDtf, pEntry->oil_idx_valid, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_STACK_FWD_PMSKtf, pEntry->stack_fwd_pmsk, entry, "", errHandle, ret);
        L3_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, \
            LONGAN_L3_PREFIX_ROUTE_IP6MC_VALIDtf, pEntry->valid, entry, "", errHandle, ret);

        L3_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_PREFIX_ROUTE_IP6MCt, index, entry, "", errHandle, ret);
    }
    else
    {
        /* Unknown type */
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

errHandle:
errEntryNotFound:
    L3_INT_SEM_UNLOCK(unit);

    return ret;
}

static int32 _dal_longan_l3_routeEntry_hw_move(uint32 unit, uint32 dstIdx, uint32 srcIdx, uint32 length)
{
    int32 ret = RT_ERR_OK;
    uint32 chkTimes;
    uint32 exec, cmd;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,dst=%d,src=%d,length=%d", unit, dstIdx, srcIdx, length);

    /* parameter check */
    RT_PARAM_CHK((dstIdx == srcIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((0 == length), RT_ERR_INPUT);
    RT_PARAM_CHK(((dstIdx + length - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((srcIdx + length - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((length >= L3_ROUTE_TBL_SIZE(unit)), RT_ERR_OUT_OF_RANGE);

    /* check status until it's idle */
    chkTimes = DAL_LONGAN_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes)
            return RT_ERR_FAILED;
    } while (exec != 0);

    /* configure register */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_PARAMr, LONGAN_LENf, length, "", errHandle, ret);
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_TOf, dstIdx, "", errHandle, ret);
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_FROMf, srcIdx, "", errHandle, ret);
    cmd = 1; /* move */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_CMDf, cmd, "", errHandle, ret);
    exec = 1; /* execute */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_EXECf, exec, "", errHandle, ret);

    /* check status until it's done */
    chkTimes = DAL_LONGAN_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes) break;   /* force to leave */
    } while (exec != 0);

    ret = RT_ERR_OK;    /* success */

errHandle:
    return ret;
}

/*NOTE:  software database move, this operation include delete entry */
static int32 _dal_longan_l3_routeEntry_sw_move(uint32 unit, uint32 ipv6, uint32 dstIdx, uint32 srcIdx, uint32 cnt)
{
    int     ret = RT_ERR_OK;
    int     i;
    uint32  fromIdx, toIdx, delIdx;
    uint32  baseCnt, delCnt = 0;
    uint32  nhIdx;
    int32 step;

    /* parameter check */
    RT_PARAM_CHK((dstIdx == srcIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((0 == cnt), RT_ERR_INPUT);

    if (ipv6)
    {
        RT_PARAM_CHK(!IS_L3_ROUTE_IPV6_IDX_VALID(dstIdx), RT_ERR_INPUT);
        RT_PARAM_CHK(!IS_L3_ROUTE_IPV6_IDX_VALID(srcIdx), RT_ERR_INPUT);
        RT_PARAM_CHK((cnt >= L3_ROUTE_TBL_SIZE(unit)/8*2), RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(((dstIdx + L3_ROUTE_IPV6_CNT2LEN(dstIdx, cnt) - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(((srcIdx + L3_ROUTE_IPV6_CNT2LEN(srcIdx, cnt) - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    }
    else
    {
        RT_PARAM_CHK((cnt >= L3_ROUTE_TBL_SIZE(unit)), RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(((dstIdx + cnt - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
        RT_PARAM_CHK(((srcIdx + cnt - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    }

    /* delete entry operation, release the path_id ref software database*/
    if ((srcIdx > dstIdx) && (!ipv6))
        delCnt = srcIdx - dstIdx;

    if ((srcIdx < dstIdx) && (ipv6))
        delCnt = L3_ROUTE_IPV6_LEN2CNT(dstIdx-srcIdx);

    for (i = 0; i < delCnt; i++)
    {
        if (ipv6)
        {
            delIdx = L3_ROUTE_IPV6_IDX_NEXT(srcIdx + L3_ROUTE_IPV6_CNT2LEN(srcIdx, cnt) + 3*i);
        }
        else
        {
            delIdx = dstIdx+i;
        }

        nhIdx = _pL3Db[unit]->route[delIdx].shadow.path_id;
        if ((!(L3_ENTRY_ROUTE_ADDR(unit, delIdx)->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
            _pL3Db[unit]->nexthop[nhIdx].ref_count--;

    }

    /*move operation*/
    /*if (srcIdx > dstIdx) for (i = 1; i <= cnt; i++) */
    /*if (srcIdx < dstIdx) for (i = cnt; i >= 1; i--) */
    baseCnt = (srcIdx > dstIdx) ? 1 : cnt;
    step = (srcIdx > dstIdx) ? 1: -1;

    toIdx = dstIdx;
    fromIdx = srcIdx;
    for (i = baseCnt; ; i+= step)
    {
        if (srcIdx > dstIdx)
        {
            if (i == cnt+1)
                break;
        }
        else
        {
            if (i == 0)
                break;
        }

        if (ipv6)
        {
            toIdx = dstIdx + 3*(i -1) + ((i -1)/2)*2;
            toIdx = L3_ROUTE_IPV6_IDX_NEXT(toIdx);
            fromIdx = srcIdx + 3*(i-1) + ((i -1)/2)*2;
            fromIdx = L3_ROUTE_IPV6_IDX_NEXT(fromIdx);
        }
        else
        {
            toIdx = dstIdx+i-1;
            fromIdx = srcIdx+i-1;
        }

        /* move entry */
        _pL3Db[unit]->route[toIdx].shadow = _pL3Db[unit]->route[fromIdx].shadow;
        _pL3Db[unit]->route[toIdx].l3_entry = _pL3Db[unit]->route[fromIdx].l3_entry;

        if (ipv6)
        {
            /* write data into chip */
            RT_ERR_HDL(_dal_longan_l3_routeEntry_hw_move(unit, toIdx, fromIdx, 3), errHandle, ret);
        }
    }

errHandle:
    return ret;
}

static int32 _dal_longan_l3_routeEntry_hw_clear(uint32 unit, uint32 baseIdx, uint32 length)
{
    int32 ret = RT_ERR_OK;
    uint32 chkTimes;
    uint32 exec, cmd;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,baseIdx=%d,length=%d", unit, baseIdx, length);

    /* parameter check */
    RT_PARAM_CHK((0 == length), RT_ERR_INPUT);
    RT_PARAM_CHK(((baseIdx + length - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((length > L3_ROUTE_TBL_SIZE(unit)), RT_ERR_OUT_OF_RANGE);

    /* check status until it's idle */
    chkTimes = DAL_LONGAN_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes)
            return RT_ERR_FAILED;
    } while (exec != 0);

    /* configure register */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_PARAMr, LONGAN_LENf, length, "", errHandle, ret);
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_FROMf, baseIdx, "", errHandle, ret);
    cmd = 0; /* clear */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_CMDf, cmd, "", errHandle, ret);
    exec = 1; /* execute */
    L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_EXECf, exec, "", errHandle, ret);

    /* check status until it's done */
    chkTimes = DAL_LONGAN_L3_CHK_IDLE_TIMES;
    do {
        L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_ENTRY_MV_CTRLr, LONGAN_EXECf, exec, "", errHandle, ret);
        chkTimes --;

        if (0 == chkTimes)
            break;   /* force to leave */
    } while (exec != 0);

    ret = RT_ERR_OK;    /* success */

errHandle:
    return ret;
}

static int32 _dal_longan_l3_routeEntry_move(uint32 unit, uint32 ipv6, uint32 dstIdx, uint32 srcIdx, uint32 cnt)
{
    int32 ret = RT_ERR_OK;

    if (ipv6)
    {
        RT_ERR_HDL(_dal_longan_l3_routeEntry_sw_move(unit, ipv6, dstIdx, srcIdx, cnt), errHandle, ret);
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_routeEntry_hw_move(unit, dstIdx, srcIdx, cnt), errHandle, ret);
        RT_ERR_HDL(_dal_longan_l3_routeEntry_sw_move(unit, ipv6, dstIdx, srcIdx, cnt), errHandle, ret);
    }

errHandle:
    return ret;
}

static int32 _dal_longan_l3_routeEntry_clear(uint32 unit, uint32 ipv6, uint32 baseIdx, uint32 cnt)
{
    int32   ret = RT_ERR_OK;
    uint32 length = 0;
    int32   i;
    uint32  routIdx;
    uint32  nhIdx;

    if (ipv6)
    {
        RT_PARAM_CHK(!IS_L3_ROUTE_IPV6_IDX_VALID(baseIdx), RT_ERR_INPUT);
        RT_PARAM_CHK(((baseIdx + L3_ROUTE_IPV6_CNT2LEN(baseIdx, cnt) - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);

        length = L3_ROUTE_IPV6_CNT2LEN(baseIdx, cnt);
    }
    else
    {
        RT_PARAM_CHK(((baseIdx + cnt - 1) > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);

        length = cnt;
    }

    RT_ERR_HDL(_dal_longan_l3_routeEntry_hw_clear(unit, baseIdx, length), errHandle, ret);

    for (i = 0; i < cnt; i++)
    {
        if (ipv6)
        {
            routIdx = L3_ROUTE_IPV6_IDX_NEXT(baseIdx + 3*i + (i/2)*2);
        }
        else
        {
            routIdx = baseIdx + i;
        }

        nhIdx = _pL3Db[unit]->route[routIdx].shadow.path_id;
        if ((!(L3_ENTRY_ROUTE_ADDR(unit, routIdx)->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
            _pL3Db[unit]->nexthop[nhIdx].ref_count--;

        osal_memset(L3_ENTRY_ROUTE_ADDR(unit, routIdx), 0x0, sizeof(rtk_l3_route_t));
    }

errHandle:
    return ret;
}


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
int32
_dal_longan_l3_routeEntry_alloc(uint32 unit, uint32 ipv6, uint32 pfLen, uint32 *pIndex)
{
    int32   ret = RT_ERR_OK;
    uint32  dstIdx, srcIdx, cnt;
    int32   i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,ipv6=%d,pfLen=%d,pIndex=0x%08x", unit, ipv6, pfLen, pIndex);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((TRUE == ipv6) && (pfLen > 128)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((FALSE == ipv6) && (pfLen > 32)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* function body */
    if (TRUE == ipv6)
    {
        /* IPv6 UC route entry allocation */
        if (IS_L3_ROUTE_IPV6_EMPTY(unit))
        {
            if (L3_ROUTE_IPV4_LAST_IDX(unit) >= L3_ROUTE_IPV6_LAST_IDX(unit))
            {
                return RT_ERR_TBL_FULL;
            }
        }
        else
        {
            if (((3 == L3_ROUTE_IPV6_LAST_IDX(unit)%8) && ((int32)(L3_ROUTE_IPV6_LAST_IDX(unit) - L3_ROUTE_TBL_IP_CNT(unit)) < 3))  ||
                 ((0 == L3_ROUTE_IPV6_LAST_IDX(unit)%8) && ((int32)(L3_ROUTE_IPV6_LAST_IDX(unit) - L3_ROUTE_TBL_IP_CNT(unit)) < 5)))
            {
                return RT_ERR_TBL_FULL;
            }
        }

        if (pfLen >= LONGAN_L3_HOST_IP6_PFLEN)
        {
            /* use the top entry directly */
            if (IS_L3_ROUTE_IPV6_EMPTY(unit))
                *pIndex = L3_ROUTE_IPV6_IDX_MAX(unit);
            else
                *pIndex = L3_ROUTE_IPV6_IDX_PRE(L3_ROUTE_IPV6_LAST_IDX(unit) - 3);
        }
        else
        {
            /* move entries for inserting a new entry */
            if (IS_L3_ROUTE_IPV6_EMPTY(unit))
                *pIndex = L3_ROUTE_IPV6_LAST_IDX(unit);
            else
            {
                srcIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
                dstIdx = L3_ROUTE_IPV6_IDX_PRE(srcIdx - 3);
                cnt = L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen);

                if (cnt > 0)
                {
                    RT_ERR_HDL(_dal_longan_l3_routeEntry_move(unit, ipv6, dstIdx, srcIdx, cnt), errHandle, ret);
                    *pIndex = L3_ROUTE_IPV6_IDX_NEXT(dstIdx + L3_ROUTE_IPV6_CNT2LEN(dstIdx, cnt));
                }
                else
                    *pIndex = dstIdx;
            }
        }

        for (i = pfLen; i > 0; i--)
        {
            L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, i - 1) += 1;
        }

        L3_ROUTE_TBL_IP6_CNT(unit) += 1;
    }
    else
    {
        /* IPv4 UC route entry allocation */
        if (IS_L3_ROUTE_IPV6_EMPTY(unit))
        {
            if (L3_ROUTE_TBL_IP_CNT(unit) == L3_ROUTE_TBL_SIZE(unit))
                return RT_ERR_TBL_FULL;
        }
        else
        {
            if ((int32)(L3_ROUTE_IPV6_LAST_IDX(unit) - L3_ROUTE_TBL_IP_CNT(unit)) < 1)
                return RT_ERR_TBL_FULL;
        }

        if (0 == pfLen)
        {
            /* use the bottom entry directly */
            *pIndex = L3_ROUTE_TBL_IP_CNT(unit);
        }
        else
        {
            /* move entries for inserting a new entry */
            srcIdx = L3_ROUTE_TBL_IP_CNT(unit) - (L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen - 1));
            dstIdx = srcIdx + 1;
            cnt = L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen - 1);
            if (cnt > 0)
            {
                RT_ERR_HDL(_dal_longan_l3_routeEntry_move(unit, ipv6, dstIdx, srcIdx, cnt), errHandle, ret);
            }

            *pIndex = srcIdx;
        }

        for (i = pfLen; i < LONGAN_L3_HOST_IP_PFLEN; i++)
        {
            L3_ROUTE_TBL_IP_CNT_PFLEN(unit, i) += 1;
        }

        L3_ROUTE_TBL_IP_CNT(unit) += 1;
    }

errHandle:

    return ret;
}

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
int32
_dal_longan_l3_routeEntry_free(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    dal_longan_l3_routeEntry_t routeEntry;
    uint32  dstIdx, srcIdx, cnt;
    uint32  pfLen;
    int32   i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index > L3_ROUTE_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    if ((L3_ROUTE_TBL_IP_CNT(unit) > 0) && (index < L3_ROUTE_TBL_IP_CNT(unit)))
    {
        /* IPv4 */
        RT_ERR_HDL(_dal_longan_l3_route_get(unit, index, RTK_L3_FLAG_READ_HIT_IGNORE, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, index)), errHandle, ret);
        pfLen = rt_util_ipMask2Length_ret(routeEntry.bmsk_ip);

        /* the bottom entry ? */
        if (index == (L3_ROUTE_TBL_IP_CNT(unit) - 1))
        {
            /* bottom entry -> remove directly */
            RT_ERR_HDL(_dal_longan_l3_routeEntry_clear(unit, FALSE, index, 1), errHandle, ret);
        }
        else
        {
            /* move other entries to here */
            srcIdx = index + 1;
            dstIdx = index;
            cnt = (L3_ROUTE_TBL_IP_CNT(unit) - 1) - index;
            RT_ERR_HDL(_dal_longan_l3_routeEntry_move(unit, FALSE, dstIdx, srcIdx, cnt), errHandle, ret);
        }

        /* update counters */
        for (i = pfLen; i < LONGAN_L3_HOST_IP_PFLEN; i++)
        {
            L3_ROUTE_TBL_IP_CNT_PFLEN(unit, i) -= 1;
        }

        L3_ROUTE_TBL_IP_CNT(unit) -= 1;
    }
    else if ((L3_ROUTE_TBL_IP6_CNT(unit) > 0) && (index >= L3_ROUTE_IPV6_LAST_IDX(unit)))
    {
        /* IPv6 */
        if (!IS_L3_ROUTE_IPV6_IDX_VALID(index))
            return RT_ERR_INPUT;

        RT_ERR_HDL(_dal_longan_l3_route_get(unit, index, RTK_L3_FLAG_READ_HIT_IGNORE, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, index)), errHandle, ret);
        pfLen = rt_util_ip6Mask2Length_ret(&routeEntry.bmsk_ip6);

        /* the top entry ? */
        if (index == L3_ROUTE_IPV6_LAST_IDX(unit))
        {
            /* top entry -> remove directly */
            RT_ERR_HDL(_dal_longan_l3_routeEntry_clear(unit, TRUE, index, 1), errHandle, ret);
        } else {
            /* move other entries to here */
            srcIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
            dstIdx = L3_ROUTE_IPV6_IDX_NEXT(srcIdx + 3);
            cnt = L3_ROUTE_IPV6_LEN2CNT(index-srcIdx);  /*the ipv6 valid length = total length - unused length*/

            RT_ERR_HDL(_dal_longan_l3_routeEntry_move(unit, TRUE, dstIdx, srcIdx, cnt), errHandle, ret);
        }

        /* update counters */
        for (i = pfLen; i > 0; i--)
        {
            L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, i - 1) -= 1;
        }

        L3_ROUTE_TBL_IP6_CNT(unit) -= 1;
    }
    else
    {
        return RT_ERR_ENTRY_NOTFOUND;
    }

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_l3_routeEntryIdx_hwFast_get
 * Description:
 *      Look up a route entry index by hw
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t specifying the basic inputs
 * Output:
 *      pIndex - pointer to found route entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 * Note:
 *      *pIndex = -1, entry is not found
 */
static int32 _dal_longan_l3_routeEntryIdx_hwFast_get(uint32 unit, rtk_l3_route_t *pRoute, int32 *pIndex)
{
    int32   ret = RT_ERR_OK;
    uint32  value;
    uint32  ipArray[IPV6_ADDR_LEN >> 2];
    rtk_ipv6_addr_t ip6Addr;
    int32   i;
    dal_longan_l3_routeEntry_t  routeEntry;
    uint32 chkTimes;

    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    *pIndex = -1;
    RT_ERR_HDL(l3_util_rtkRoute2routeEntry(&routeEntry, pRoute), errHandle,ret);

    L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_ENTRY_TYPEf, routeEntry.entry_type, "", errHandle, ret);

    if (1 == routeEntry.entry_type || 3 == routeEntry.entry_type)
    {
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_ROUNDf, routeEntry.round, "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_IPMC_TYPEf, routeEntry.ipmc_type, "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_VIDf, routeEntry.vid, "", errHandle, ret);

        //sip
        if (0 == routeEntry.round)
        {
            osal_memset(ipArray, 0x00, sizeof(ipArray));
            if (3 == routeEntry.entry_type)
                ipv6toIpArray(ipArray, &routeEntry.sip6);
            else
                ipArray[0] = routeEntry.sip;

            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IPf, ipArray[0], "", errHandle, ret);
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IP1f, ipArray[1], "", errHandle, ret);
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IP2f, ipArray[2], "", errHandle, ret);
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IP3f, ipArray[3], "", errHandle, ret);
        }

        //dip
        osal_memset(ipArray, 0x00, sizeof(ipArray));
        if (3 == routeEntry.entry_type)
            ipv6toIpArray(ipArray, &routeEntry.gip6);
        else
            ipArray[0] = routeEntry.gip;

        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_DIP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IPf, ipArray[0], "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_DIP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IP1f, ipArray[1], "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_DIP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IP2f, ipArray[2], "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_DIP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_IP3f, ipArray[3], "", errHandle, ret);
    }
    else if (0 == routeEntry.entry_type || 2 == routeEntry.entry_type)  /* IPUC/IP6UC */
    {
        osal_memset(ipArray, 0x00, sizeof(ipArray));
        if (2 == routeEntry.entry_type)
        {
            osal_memset(&ip6Addr, 0x00, sizeof(ip6Addr));
            for (i = 0; i < IPV6_ADDR_LEN; i++)
            {
                ip6Addr.octet[i] = (routeEntry.ip6.octet[i] & routeEntry.bmsk_ip6.octet[i]);
            }
            ipv6toIpArray(ipArray, &ip6Addr);
        }
        else
        {
            ipArray[0] = (routeEntry.ip & routeEntry.bmsk_ip);
        }

        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_IPf, ipArray[0], "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_IP1f, ipArray[1], "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_IP2f, ipArray[2], "", errHandle, ret);
        L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_KEY_IP_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_IP3f, ipArray[3], "", errHandle, ret);
    }
    else
    {
        ret = RT_ERR_INPUT;
        goto errHandle;
    }

    /*exec */
    value = 1;
    L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HW_LU_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_EXEC_TCAMf, value, "", errHandle, ret);

    chkTimes = DAL_LONGAN_L3_CHK_IDLE_TIMES;
    do{
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HW_LU_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
                LONGAN_EXEC_TCAMf, value, "", errHandle, ret);
            chkTimes --;

            if (0 == chkTimes)
                return RT_ERR_FAILED;
    } while (value != 0);

    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HW_LU_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
        LONGAN_RESULT_TCAMf, value, "", errHandle, ret);
    if (1 == value) /*  found */
    {
        L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HW_LU_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, \
            LONGAN_ENTRY_IDX_TCAMf, value, "", errHandle, ret);
        *pIndex = value;
    }

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_l3_routeEntryIdx_hw_get
 * Description:
 *      Look up a route entry index by hw find one by one
 * Input:
 *      unit   - unit id
 *      pRouteEntry - pointer to returned dal_longan_l3_routeEntry_t info
 *      pIndex - pointer to the begin lookup index
 * Output:
 *      pIndex - pointer to found route entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 * Note:
 *      *pIndex = -1, entry is not found
 */
static int32 _dal_longan_l3_routeEntryIdx_hw_get(uint32 unit, rtk_l3_route_t *pRoute, int32 *pIndex)
{
    int32 ret = RT_ERR_OK;
    rtk_l3_route_t   rtkRoute;
    dal_longan_l3_routeEntry_t   routeEntry;
    uint32  pfLen, sfLen;
    uint32  minIdx, length;
    uint32  routeIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p,pRouteIdx=%p", unit, pRoute, pIndex);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    pfLen = pRoute->prefix_len;
    sfLen = pRoute->suffix_len;

    /* search H/W */
    if (pRoute->flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 */
        if (pfLen >= LONGAN_L3_HOST_IP6_PFLEN)
        {
            minIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
            length =  L3_ROUTE_IPV6_CNT2LEN(minIdx, L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, (LONGAN_L3_HOST_IP_PFLEN - 1)));
        }
        else if (pfLen > 0)
        {
            minIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
            length = L3_ROUTE_IPV6_CNT2LEN(minIdx, L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen-1)) -
                            L3_ROUTE_IPV6_CNT2LEN(minIdx, L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen));
            minIdx = L3_ROUTE_IPV6_IDX_NEXT(minIdx + L3_ROUTE_IPV6_CNT2LEN(minIdx, L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen)));
        }
        else /* pfLen == 0 */
        {
            minIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
            length = L3_ROUTE_IPV6_CNT2LEN(minIdx, L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, pfLen));
            minIdx = L3_ROUTE_IPV6_IDX_NEXT(minIdx + length);
        }

        for (routeIdx = minIdx; (routeIdx - minIdx) < length; routeIdx += 3)
        {
            routeIdx = L3_ROUTE_IPV6_IDX_NEXT(routeIdx);
            if (routeIdx > L3_ROUTE_IPV6_IDX_MAX(unit))
                break;

            /* get entry from H/W */
            RT_ERR_HDL(_dal_longan_l3_route_get(unit, routeIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, routeIdx)), errHandle, ret);
            RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);

            if ((pfLen == _pL3Db[unit]->route[routeIdx].shadow.prefix_len) && \
                 (sfLen == _pL3Db[unit]->route[routeIdx].shadow.suffix_len) && \
                 (pfLen == rt_util_ipv6MaxMatchLength_ret(&rtkRoute.ip_addr.ipv6, &pRoute->ip_addr.ipv6, pfLen)) &&
                 (sfLen == l3_util_ipv6SuffixMaxMatchLength_ret(&rtkRoute.ip_addr.ipv6, &pRoute->ip_addr.ipv6, sfLen)))
            {
                /* found the entry, return hostIdx */
                *pIndex = routeIdx;
                goto errOk;
            }
        }
    }
    else
    {
        /* IPv4 */
        if (pfLen >= LONGAN_L3_HOST_IP_PFLEN)
        {
            minIdx = 0;
            length = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, (LONGAN_L3_HOST_IP_PFLEN - 1));
        }
        else if (pfLen > 0)
        {
            minIdx = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen);
            length = L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, (pfLen - 1));
        }
        else /* pfLen == 0 */
        {
            minIdx = L3_ROUTE_TBL_IP_CNT(unit) - L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen);
            //maxIdx = L3_ROUTE_TBL_IP_CNT(unit);
            length = L3_ROUTE_TBL_IP_CNT_PFLEN(unit, pfLen);
        }

        for (routeIdx = minIdx; (routeIdx - minIdx) < length; routeIdx++)
        {
            /* get entry from H/W */
            RT_ERR_HDL(_dal_longan_l3_route_get(unit, routeIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, routeIdx)), errHandle, ret);
            RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);

            if ((pfLen == _pL3Db[unit]->route[routeIdx].shadow.prefix_len) && \
                 (sfLen == _pL3Db[unit]->route[routeIdx].shadow.suffix_len) && \
                 (pfLen == rt_util_ipMaxMatchLength_ret(rtkRoute.ip_addr.ipv4, pRoute->ip_addr.ipv4, pfLen)) && \
                 (sfLen == l3_util_ipSuffixMaxMatchLength_ret(rtkRoute.ip_addr.ipv4, pRoute->ip_addr.ipv4, sfLen))
                )
            {
                /* found the entry, return hostIdx */
                *pIndex = routeIdx;
                goto errOk;
            }
        }
    }

   *pIndex = -1;
    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    return ret;
}


/* Function Name:
 *      _dal_longan_l3_routeEntry_find
 * Description:
 *      Look up a route given the network and netmask
 * Input:
 *      unit   - unit id
 *      pRoute - pointer to rtk_l3_route_t specifying the basic inputs
 * Output:
 *      pRouteEntry - pointer to returned dal_longan_l3_routeEntry_t info
 *      pIndex - pointer to returned dal_longan_l3_routeEntry_t index
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
 *      *pIndex = -1, entry is not found
 *       if pRouteEntry == NULL, not return routeEntry
 */
int32
_dal_longan_l3_routeEntry_find(uint32 unit, rtk_l3_route_t *pRoute, int32 *pIndex, dal_longan_l3_routeEntry_t *pRouteEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    rtk_l3_route_t rtkRoute;
    dal_longan_l3_routeEntry_t routeEntry;

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    ipv6 = (pRoute->flags & RTK_L3_FLAG_IPV6) ? TRUE : FALSE;

    RT_ERR_HDL(_dal_longan_l3_routeEntryIdx_hwFast_get(unit, pRoute, pIndex), errHandle, ret);
    if (-1 != *pIndex)
    {
        if (*pIndex >= L3_ROUTE_TBL_SIZE(unit))
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errHandle;
        }

        if ( (ipv6 && (*pIndex < L3_ROUTE_IPV6_LAST_IDX(unit))) ||
              (!ipv6 && (*pIndex > L3_ROUTE_IPV4_LAST_IDX(unit))) )
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errHandle;
        }

        /* get entry from H/W */
        RT_ERR_HDL(_dal_longan_l3_route_get(unit, *pIndex, pRoute->flags, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, (*pIndex))), errHandle, ret);
        RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);

        if ((pRoute->prefix_len == _pL3Db[unit]->route[*pIndex].shadow.prefix_len) && \
            (pRoute->suffix_len == _pL3Db[unit]->route[*pIndex].shadow.suffix_len) && \
            (((FALSE == ipv6) && (pRoute->prefix_len == rt_util_ipMaxMatchLength_ret(pRoute->ip_addr.ipv4, rtkRoute.ip_addr.ipv4, pRoute->prefix_len)) && \
                (pRoute->suffix_len == l3_util_ipSuffixMaxMatchLength_ret(pRoute->ip_addr.ipv4, rtkRoute.ip_addr.ipv4, pRoute->suffix_len))) || \
            ((TRUE == ipv6) && (pRoute->prefix_len == rt_util_ipv6MaxMatchLength_ret(&pRoute->ip_addr.ipv6, &rtkRoute.ip_addr.ipv6, pRoute->prefix_len)) && \
                (pRoute->suffix_len == l3_util_ipv6SuffixMaxMatchLength_ret(&pRoute->ip_addr.ipv6, &rtkRoute.ip_addr.ipv6, pRoute->suffix_len)))))
        {
             if (NULL != pRouteEntry)
                goto getEntry;

            goto errOk;
        }
        else
        {
            /* software get */
            if (RT_ERR_OK == _dal_longan_l3_routeEntryIdx_hw_get(unit, pRoute, pIndex))
            {
                if (NULL != pRouteEntry)
                    goto getEntry;

                goto errOk;
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;
    goto errOk;

getEntry:
    RT_ERR_HDL(_dal_longan_l3_route_get(unit, *pIndex, pRoute->flags, pRouteEntry, L3_ENTRY_ROUTE_ADDR(unit, *pIndex)), errHandle, ret);

errHandle:
errOk:

    return ret;
}   /* end of dal_longan_l3_route_get */


/* Module Name    : Layer3 routing                */
/* Sub-module Name: Layer3 routing error handling */

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
int32
dal_longan_l3_info_get(uint32 unit, rtk_l3_info_t *pInfo)
{
    int32   ret = RT_ERR_OK;
#if LONGAN_L3_ROUTE_IPMC_SIZE
    uint32  ipmc_lpm_ipv4_cnt;
    uint32  ipmc_lpm_ipv6_cnt;
#endif
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);
    osal_memset(pInfo,0x00,sizeof(rtk_l3_info_t));

    /* function body */
    L3_SEM_LOCK(unit);

    pInfo->vrf_max = HAL_MAX_NUM_OF_VRF(unit);
    pInfo->intf_max = HAL_MAX_NUM_OF_INTF(unit);
    pInfo->intf_used = _pL3Db[unit]->HW.intf_used_count;
    pInfo->intf_mtu_max = HAL_MAX_NUM_OF_INTF_MTU(unit);
    pInfo->intf_mtu_used = _pL3Db[unit]->HW.ip4_mtu_used_count;
    pInfo->intf_ipv6_mtu_max = HAL_MAX_NUM_OF_INTF_MTU(unit);
    pInfo->intf_ipv6_mtu_used = _pL3Db[unit]->HW.ip6_mtu_used_count;
    pInfo->nexthop_max = HAL_MAX_NUM_OF_L3_NEXTHOP(unit);
    pInfo->nexthop_used = _pL3Db[unit]->HW.nexthop_used_count;
    pInfo->host_ipv4_max = HAL_MAX_NUM_OF_L3_HOST(unit);
    pInfo->host_ipv4_used = _pL3Db[unit]->HW.host_used_count;
    pInfo->route_ipv4_max = L3_ROUTE_TBL_SIZE(unit);
    pInfo->route_ipv4_used = L3_ROUTE_TBL_USED(unit);

#if LONGAN_L3_ROUTE_IPMC_SIZE
        /* get IPMC LPM entry count and update route_ipv4_used */
        if (RT_ERR_OK == _dal_longan_ipmc_lpmRouteCnt_get(unit, &ipmc_lpm_ipv4_cnt, &ipmc_lpm_ipv6_cnt))
        {
            pInfo->route_ipv4_max = L3_ROUTE_TBL_SIZE(unit) + LONGAN_L3_ROUTE_IPMC_SIZE;
            pInfo->route_ipv4_used += (LONGAN_L3_ROUTE_IPMC_WIDTH_IPV4*ipmc_lpm_ipv4_cnt) + (LONGAN_L3_ROUTE_IPMC_WIDTH_IPV6*ipmc_lpm_ipv6_cnt);
        }
#endif
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_info_get */

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
int32
dal_longan_l3_routerMacEntry_get(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry)
{
    int32   ret;
    dal_longan_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTER_MAC(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_l3_macEntry_get(unit, index, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntryGet, ret);
    RT_ERR_HDL(l3_util_macEntry2rtkRouterMac(pEntry, &macEntry), errMacEntryConvert, ret);

errMacEntryGet:
errMacEntryConvert:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_routerMacEntry_get */

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
int32
dal_longan_l3_routerMacEntry_set(uint32 unit, uint32 index, rtk_l3_routerMacEntry_t *pEntry)
{
    int32   ret;
    dal_longan_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d", unit, index);
    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_ROUTER_MAC(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    RT_ERR_HDL(l3_util_rtkRouterMac2macEntry(&macEntry, pEntry), errMacEntryConvert, ret);
    RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, index, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntrySet, ret);

errMacEntryConvert:
errMacEntrySet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_routerMacEntry_set */

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
 *      RT_ERR_MTU_EXCEED - interface MTU is over the maximum
 *      RT_ERR_TTL_EXCEED - interface TTL is over the maximum
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
int32
dal_longan_l3_intf_create(uint32 unit, rtk_l3_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    uint32  mtuIdx, mtuIp6Idx;
    uint32  intfIdx;
    uint32  smacIdx = 0;
    uint32  rtmacIdx = 0;
    uint32  val;
    dal_longan_l3_intfEntry_t intfEntry;
    dal_longan_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pIntf->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
/*    RT_PARAM_CHK((pIntf->vid == RTK_VLAN_ID_MIN), RT_ERR_VLAN_VID); vid 0 for port interface */
    RT_PARAM_CHK((pIntf->mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->ipv6_mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->ttl > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    if (pIntf->flags & RTK_L3_INTF_FLAG_WITH_ID)
    {
        RT_PARAM_CHK((pIntf->intf_id >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_OUT_OF_RANGE);
    }

    if (RT_ERR_OK != _dal_longan_vlan_table_check(unit, pIntf->vid))
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    /* function body */
    L3_SEM_LOCK(unit);

    /* check resource */
    if (_pL3Db[unit]->HW.intf_used_count >= HAL_MAX_NUM_OF_INTF(unit))
    {
        ret = RT_ERR_TBL_FULL;
        goto errTblFull;
    }
#ifdef L3_VID_INTERFACE_BINDING
    if (BITMAP_IS_SET(_pL3Db[unit]->vid_valid, pIntf->vid))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errEntryExsit;
    }
#endif
    /* allocate resources  */
    if (pIntf->ipv6_mtu == 0)
    {
        pIntf->ipv6_mtu = pIntf->mtu; /* same as IPv4 if it's unset */
    }
    RT_ERR_HDL(_dal_longan_l3_intf_mtuEntry_alloc(unit, pIntf->mtu, &mtuIdx), errMtuAlloc, ret);
    RT_ERR_HDL(_dal_longan_l3_intf_mtuIp6Entry_alloc(unit, pIntf->ipv6_mtu, &mtuIp6Idx), errMtuIp6Alloc, ret);

    if (pIntf->flags & RTK_L3_INTF_FLAG_WITH_ID)
    {
        intfIdx = DAL_LONGAN_L3_INTF_ID_ENTRY_IDX(pIntf->intf_id);
        RT_ERR_HDL(_dal_longan_l3_intfEntry_alloc(unit, &intfIdx, (DAL_LONGAN_L3_API_FLAG_WITH_ID)), errIntfAlloc, ret);
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_intfEntry_alloc(unit, &intfIdx, (DAL_LONGAN_L3_API_FLAG_NONE)), errIntfAlloc, ret);
    }

    /* build MAC entry first*/
    osal_memset(&macEntry, 0x00, sizeof(dal_longan_l3_macEntry_t));
    macEntry.valid      = 1;
    macEntry.mac        = pIntf->mac_addr;
    osal_memset(&macEntry.bmsk_mac, 0xFF, sizeof(rtk_mac_t));


    /* search a suitalb MAC entry */
    smacIdx = intfIdx;
    RT_ERR_HDL(_dal_longan_l3_smacEntry_alloc(unit, pIntf->mac_addr, &smacIdx), errSmacEntrySet, ret);

    if (pIntf->flags & RTK_L3_INTF_FLAG_MAC_MANUAL)
    {
        L3_DB_UPDATE_INTF_RTMAC_INDEX(unit, intfIdx, FALSE, 0);
    }
    else
    {
        if (IS_L3_RTMAC_CARE_VID(unit))
        {
            /* search a suitalb MAC entry */
            macEntry.intf_id     = pIntf->vid;
            macEntry.bmsk_intf_id   = 0xfff;
            RT_ERR_HDL(_dal_longan_l3_macEntry_alloc(unit, &macEntry, &rtmacIdx), errMacEntrySet, ret);
            RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, rtmacIdx, &macEntry, 0),errMacEntrySet, ret);
        }
        else
        {
            /* mac index max is 64; intfidx max is 128; */
            RT_ERR_HDL(_dal_longan_l3_macEntry_alloc(unit, &macEntry, &rtmacIdx), errMacEntrySet, ret);
        }
        L3_DB_UPDATE_INTF_RTMAC_INDEX(unit, intfIdx, TRUE, rtmacIdx);
    }

    /* build interface entry */
    osal_memset(&intfEntry, 0x00, sizeof(dal_longan_l3_intfEntry_t));
    intfEntry.egrIntf.dst_vid           = pIntf->vid;
    intfEntry.egrIntf.smac_idx      =   smacIdx;
    intfEntry.egrIntf.ipmc_ttl_scope    = pIntf->ttl;
    intfEntry.egrIntf.ip6mc_hl_scope    = pIntf->ttl;
    intfEntry.egrIntf.ip_mtu_idx        = mtuIdx;
    intfEntry.egrIntf.ip6_mtu_idx       = mtuIp6Idx;

    L3_ACTION_TO_VALUE(_actEgrIntfIpIcmpRedirect, val, DEFLT_DAL_LONGAN_L3_ICMP_REDIRECT_ACT, "", errIntfSet, ret);
    intfEntry.egrIntf.ip_icmp_redirect_act = val;
    /*_actEgrIntfIp6IcmpRedirect /_actEgrIntfIpPbrIcmpRedirect /_actEgrIntfIp6PbrIcmpRedirect is the same to _actEgrIntfIpIcmpRedirect*/
    intfEntry.egrIntf.ip6_icmp_redirect_act = val;
    intfEntry.egrIntf.ip_pbr_icmp_redirect_act = val;
    intfEntry.egrIntf.ip6_pbr_icmp_redirect_act = val;

    RT_ERR_HDL(_dal_longan_l3_intfEgrEntry_set(unit, intfIdx, &intfEntry.egrIntf, (DAL_LONGAN_L3_API_FLAG_NONE)), errIntfSet, ret);
    RT_ERR_HDL(_dal_longan_l3_smac_set(unit, smacIdx, macEntry.mac), errIntfSet, ret);

    L3_DB_UPDATE_INTF_VALID(unit, intfIdx, mtuIdx, smacIdx, pIntf->vid, intfEntry.egrIntf.ip6_mtu_idx,pIntf->flags);

#ifdef L3_VID_INTERFACE_BINDING
    /* update vlan info */
    BITMAP_SET(_pL3Db[unit]->vid_valid, pIntf->vid);
#endif
    _pL3Db[unit]->vid[pIntf->vid].intf_idx = intfIdx;

    /* update interface ID before returning */
    pIntf->intf_id = intfIdx;

    goto errOk;

errIntfSet:
errMacEntrySet:
    L3_RT_ERR_HDL_DBG(_dal_longan_l3_macEntry_free(unit, &macEntry), "");
    L3_RT_ERR_HDL_DBG(_dal_longan_l3_smacEntry_free(unit, smacIdx), "");
errSmacEntrySet:
    L3_DB_UPDATE_INTF_RTMAC_INDEX(unit, intfIdx, 0, 0);
    L3_RT_ERR_HDL_DBG(_dal_longan_l3_intfEntry_free(unit, intfIdx, (DAL_LONGAN_L3_API_FLAG_NONE)), "");

errIntfAlloc:
    L3_RT_ERR_HDL_DBG(_dal_longan_l3_intf_mtuIp6Entry_free(unit, mtuIp6Idx), "");

errMtuIp6Alloc:
    L3_RT_ERR_HDL_DBG(_dal_longan_l3_intf_mtuEntry_free(unit, mtuIdx), "");

errMtuAlloc:
errTblFull:
#ifdef L3_VID_INTERFACE_BINDING
errEntryExsit:
#endif
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_intf_create */


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
int32
dal_longan_l3_intf_destroy(uint32 unit, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    dal_longan_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((intfId >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_OUT_OF_RANGE);

    intfIdx = DAL_LONGAN_L3_INTF_ID_ENTRY_IDX(intfId);
    /* function body */
    L3_SEM_LOCK(unit);

#ifdef L3_VID_INTERFACE_BINDING
    if (BITMAP_IS_CLEAR(_pL3Db[unit]->vid_valid, _pL3Db[unit]->intf[intfIdx].vid))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }
#endif
    if ((_pL3Db[unit]->refer_cnt_chk_en) &&
        (L3_DB_INTF_REFCNT(unit, intfIdx) > 0))
    {
        ret = RT_ERR_ENTRY_REFERRED;
        goto errEntryReferred;
    }

    /* free MAC entry */
    osal_memset(&macEntry, 0x00, sizeof(dal_longan_l3_macEntry_t));

    if(L3_DB_INTF_RTMAC_INDEX_VALID(unit, intfIdx))
    {
        RT_ERR_HDL(_dal_longan_l3_macEntry_get(unit, _pL3Db[unit]->intf[intfIdx].rtmac_idx, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntryget, ret);
        RT_ERR_HDL(_dal_longan_l3_macEntry_free(unit,&macEntry), errMacEntryFree, ret);
        L3_DB_UPDATE_INTF_RTMAC_INDEX(unit, intfIdx, 0, 0);
    }

    RT_ERR_HDL(_dal_longan_l3_smacEntry_free(unit,_pL3Db[unit]->intf[intfIdx].mac_idx), errMacEntryFree, ret);

    /* release interface, MTU entry */
    RT_ERR_HDL(_dal_longan_l3_intfEntry_free(unit, intfIdx, (DAL_LONGAN_L3_API_FLAG_NONE)), errIntfFree, ret);
    RT_ERR_HDL(_dal_longan_l3_intf_mtuEntry_free(unit, _pL3Db[unit]->intf[intfIdx].ip_mtu_idx), errMtuFree, ret);
    RT_ERR_HDL(_dal_longan_l3_intf_mtuIp6Entry_free(unit, _pL3Db[unit]->intf[intfIdx].ip6_mtu_idx), errMtuIp6Free, ret);

#ifdef L3_VID_INTERFACE_BINDING
    BITMAP_CLEAR(_pL3Db[unit]->vid_valid, _pL3Db[unit]->intf[intfIdx].vid);
#endif
    L3_DB_UPDATE_INTF_INVALID(unit, intfIdx);

errMtuFree:
errMtuIp6Free:
errIntfFree:
errMacEntryget:
errMacEntryFree:
errEntryReferred:
#ifdef L3_VID_INTERFACE_BINDING
errEntryNotFound:
#endif
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_intf_destroy */


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
int32
dal_longan_l3_intf_destroyAll(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    uint32  intfIdx;
    rtk_intf_id_t intfId;
    dal_longan_l3_macEntry_t macEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    osal_memset(&macEntry, 0x0, sizeof(dal_longan_l3_macEntry_t));

    /* function body */
    L3_SEM_LOCK(unit);

    for (idx=0; idx<HAL_MAX_NUM_OF_INTF(unit); idx++)
    {
        if (BITMAP_IS_SET(_pL3Db[unit]->HW.intf_used, idx))
        {
            intfId = _pL3Db[unit]->intf[idx].intf_id;
            intfIdx = DAL_LONGAN_L3_INTF_ID_ENTRY_IDX(intfId);

            if(L3_DB_INTF_RTMAC_INDEX_VALID(unit, intfIdx))
            {
                RT_ERR_HDL(_dal_longan_l3_macEntry_get(unit, _pL3Db[unit]->intf[intfIdx].rtmac_idx, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntryGet, ret);
                RT_ERR_HDL(_dal_longan_l3_macEntry_free(unit, &macEntry), errMacEntryFree, ret);
                L3_DB_UPDATE_INTF_RTMAC_INDEX(unit, intfIdx, 0, 0);
            }
            RT_ERR_HDL(_dal_longan_l3_smacEntry_free(unit,_pL3Db[unit]->intf[intfIdx].mac_idx), errMacEntryFree, ret);

            RT_ERR_HDL(_dal_longan_l3_intfEntry_free(unit, intfIdx, (DAL_LONGAN_L3_API_FLAG_NONE)), errIntfFree, ret);
            RT_ERR_HDL(_dal_longan_l3_intf_mtuEntry_free(unit, _pL3Db[unit]->intf[intfIdx].ip_mtu_idx), errMtuFree, ret);
            RT_ERR_HDL(_dal_longan_l3_intf_mtuIp6Entry_free(unit, _pL3Db[unit]->intf[intfIdx].ip6_mtu_idx), errMtuIp6Free, ret);
#ifdef L3_VID_INTERFACE_BINDING
            BITMAP_CLEAR(_pL3Db[unit]->vid_valid, _pL3Db[unit]->intf[intfIdx].vid);
#endif
            L3_DB_UPDATE_INTF_INVALID(unit, intfIdx);
        }
    }

errMtuFree:
errMtuIp6Free:
errIntfFree:
errMacEntryFree:
errMacEntryGet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_intf_destroyAll */


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
int32
dal_longan_l3_intf_get(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    rtk_intf_id_t intfId;
    dal_longan_l3_intfEntry_t intfEntry;
    uint32  idx;
    uint32  ip_mtu, ip6_mtu;
    routing_mac_entry_t rtMacEntry;
    rtk_mac_t   smac;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((pIntf->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* function body */
    L3_SEM_LOCK(unit);

    if (RTK_L3_INTFKEYTYPE_INTF_ID == type)
    {
        intfId = pIntf->intf_id;
    }
    else if (RTK_L3_INTFKEYTYPE_VID == type || RTK_L3_INTFKEYTYPE_MAC_AND_VID == type)
    {
        intfId = _pL3Db[unit]->vid[pIntf->vid].intf_idx;
#ifdef L3_VID_INTERFACE_BINDING
        if (!(BITMAP_IS_SET(_pL3Db[unit]->vid_valid, pIntf->vid)))
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }
#endif
        for (idx=0; idx<DAL_LONGAN_L3_INTF_MAX; idx++)
        {
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
        if(idx >= DAL_LONGAN_L3_INTF_MAX)
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }


        if (RTK_L3_INTFKEYTYPE_MAC_AND_VID == type)
        {
            L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, DAL_LONGAN_L3_SMAC_IDX(_pL3Db[unit]->intf[intfId].mac_idx), rtMacEntry, "", errMacGet, ret);
            L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, \
                LONGAN_L3_EGR_INTF_MAC_MACtf, smac, rtMacEntry, "", errMacGet, ret);
            if (RT_ERR_OK != rt_util_macCmp(smac.octet, pIntf->mac_addr.octet))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errEntryNotFound;
            }
        }
    }
    else
    {
        ret = RT_ERR_INPUT;
        goto errType;
    }

    RT_ERR_HDL(_dal_longan_l3_intfEgrEntry_get(unit, intfId, &intfEntry.egrIntf, (DAL_LONGAN_L3_API_FLAG_NONE)), errIntfGet, ret);
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_MTU_CTRLr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip_mtu_idx, \
        LONGAN_MTU_VALf, ip_mtu, "", errIpMtuGet, ret);
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6_MTU_CTRLr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip6_mtu_idx, \
        LONGAN_MTU_VALf, ip6_mtu, "", errIpMtuGet, ret);

    pIntf->vrf_id   = 0;
    pIntf->intf_id  = intfId;
    pIntf->vid      = intfEntry.egrIntf.dst_vid;
    pIntf->mtu      = ip_mtu;
    pIntf->ipv6_mtu = ip6_mtu;
    pIntf->ttl      = intfEntry.egrIntf.ipmc_ttl_scope;
    pIntf->flags    = L3_DB_INTF_FLAGS(unit, intfId);

    L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, DAL_LONGAN_L3_SMAC_IDX(_pL3Db[unit]->intf[intfId].mac_idx), rtMacEntry, "", errMacGet, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, \
        LONGAN_L3_EGR_INTF_MAC_MACtf, pIntf->mac_addr, rtMacEntry, "", errMacGet, ret);

errIpMtuGet:
errIntfGet:
errType:
errMacGet:
errEntryNotFound:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_intf_get */


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
 *      RT_ERR_MTU_EXCEED          - interface MTU is over the maximum
 *      RT_ERR_TTL_EXCEED          - interface TTL is over the maximum
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *          RTK_L3_INTFKEYTYPE_VID          - identify by VLAN ID
 *          RTK_L3_INTFKEYTYPE_MAC_AND_VID  - identify by MAC and VLAN ID
 */
int32
dal_longan_l3_intf_set(uint32 unit, rtk_l3_intfKeyType_t type, rtk_l3_intf_t *pIntf)
{
    int32   ret = RT_ERR_OK;
    rtk_intf_id_t intfId;
    dal_longan_l3_intfEntry_t intfEntry;
    uint32  ip_mtu, smacIdx = 0, rtmacIdx = 0, ip6_mtu;
    routing_mac_entry_t rtMacEntry;
    rtk_mac_t  smac;
    dal_longan_l3_macEntry_t newMacEntry;
    uint32  intfChg = FALSE;
    uint32  idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pIntf->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pIntf->mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->ipv6_mtu > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIntf->ttl > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    if (RT_ERR_OK != _dal_longan_vlan_table_check(unit, pIntf->vid))
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;

    /* function body */
    L3_SEM_LOCK(unit);

    if (RTK_L3_INTFKEYTYPE_INTF_ID == type)
    {
        intfId = pIntf->intf_id;
    }
    else if (RTK_L3_INTFKEYTYPE_VID == type || RTK_L3_INTFKEYTYPE_MAC_AND_VID == type)
    {
        RT_PARAM_CHK((pIntf->vid == RTK_VLAN_ID_MIN), RT_ERR_VLAN_VID);
        intfId = _pL3Db[unit]->vid[pIntf->vid].intf_idx;
#ifdef L3_VID_INTERFACE_BINDING
        if (!(BITMAP_IS_SET(_pL3Db[unit]->vid_valid, pIntf->vid)))
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }
#endif
        for (idx=0; idx<DAL_LONGAN_L3_INTF_MAX; idx++)
        {
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
        if(idx >= DAL_LONGAN_L3_INTF_MAX)
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
    RT_ERR_HDL(_dal_longan_l3_intfEgrEntry_get(unit, intfId, &intfEntry.egrIntf, (DAL_LONGAN_L3_API_FLAG_NONE)), errIntfGet, ret);
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_MTU_CTRLr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip_mtu_idx,
        LONGAN_MTU_VALf, ip_mtu, "", errIpMtuGet, ret);
    L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6_MTU_CTRLr, REG_ARRAY_INDEX_NONE, intfEntry.egrIntf.ip6_mtu_idx,
        LONGAN_MTU_VALf, ip6_mtu, "", errIpMtuGet, ret);

    /*check the change vid is used or not*/
    if (pIntf->vid != intfEntry.egrIntf.dst_vid)
    {
        if ((RTK_L3_INTFKEYTYPE_VID == type) || \
            (IS_L3_RTMAC_CARE_VID(unit) && (RTK_L3_INTFKEYTYPE_MAC_AND_VID == type)))
        {
            ret = RT_ERR_INPUT;
            goto errType;
        }
#ifdef L3_VID_INTERFACE_BINDING
        if (BITMAP_IS_SET(_pL3Db[unit]->vid_valid, pIntf->vid))
        {
            ret = RT_ERR_ENTRY_EXIST;
            goto errIntfSet;
        }
#endif
        intfChg = TRUE;
    }

    /*check route-mac*/
    L3_TABLE_READ_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, DAL_LONGAN_L3_SMAC_IDX(_pL3Db[unit]->intf[intfId].mac_idx), rtMacEntry, "", errMacGet, ret);
    L3_TABLE_FIELD_MAC_GET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_MACt, \
        LONGAN_L3_EGR_INTF_MAC_MACtf, smac, rtMacEntry, "", errMacGet, ret);

    if (RTK_L3_INTFKEYTYPE_MAC_AND_VID == type)
    {
        if (RT_ERR_OK != rt_util_macCmp(smac.octet, pIntf->mac_addr.octet))
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }
    }
    else
    {
        if(RT_ERR_OK != rt_util_macCmp(smac.octet, pIntf->mac_addr.octet))
        {
            intfChg = TRUE;
        }
    }

    if (pIntf->mtu != ip_mtu)
    {
        RT_ERR_HDL(_dal_longan_l3_intf_mtuEntry_realloc(unit, pIntf->mtu, &intfEntry.egrIntf.ip_mtu_idx), errMtuRealloc, ret);
    }
    if (pIntf->ipv6_mtu == 0) pIntf->ipv6_mtu = pIntf->mtu; /* sync with intf mtu */
    if (pIntf->ipv6_mtu != ip6_mtu)
    {
        RT_ERR_HDL(_dal_longan_l3_intf_mtuIp6Entry_realloc(unit, pIntf->ipv6_mtu, &intfEntry.egrIntf.ip6_mtu_idx), errMtuIp6Realloc, ret);
    }

    smacIdx = _pL3Db[unit]->intf[intfId].mac_idx;
    if (intfChg)
    {
        osal_memset(&newMacEntry, 0 , sizeof(dal_longan_l3_macEntry_t));
        newMacEntry.valid      = 1;
        newMacEntry.mac        = pIntf->mac_addr;
        osal_memset(&newMacEntry.bmsk_mac, 0xFF, sizeof(rtk_mac_t));


        if (pIntf->flags & RTK_L3_INTF_FLAG_MAC_MANUAL)
        {
            L3_DB_UPDATE_INTF_RTMAC_INDEX(unit, intfId, FALSE, 0);
        }
        else
        {
            rtmacIdx = L3_DB_INTF_RTMAC_INDEX(unit, intfId);

            if (IS_L3_RTMAC_CARE_VID(unit))
            {
                /* search a suitalb MAC entry */
                newMacEntry.intf_id     = pIntf->vid;
                newMacEntry.bmsk_intf_id    = 0xfff;
                RT_ERR_HDL(_dal_longan_l3_macEntry_realloc(unit, &newMacEntry, &rtmacIdx), errMacRealloc, ret);

                RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, rtmacIdx, &newMacEntry, DAL_LONGAN_L3_API_FLAG_NONE), errMacRealloc, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_longan_l3_macEntry_realloc(unit, &newMacEntry, &rtmacIdx), errMacRealloc, ret);
            }
            L3_DB_UPDATE_INTF_RTMAC_INDEX(unit, intfId, TRUE, rtmacIdx);
        }

        if(RT_ERR_OK != rt_util_macCmp(smac.octet, pIntf->mac_addr.octet))
        {
            /* search a suitalb MAC entry */
            RT_ERR_HDL(_dal_longan_l3_smacEntry_realloc(unit, pIntf->mac_addr, &smacIdx), errSmacSet, ret);
            RT_ERR_HDL(_dal_longan_l3_smac_set(unit, smacIdx, pIntf->mac_addr), errSmacSet, ret);
        }
    }

#ifdef L3_VID_INTERFACE_BINDING
    //update vid use database
    if (pIntf->vid != intfEntry.egrIntf.dst_vid)
    {
        BITMAP_CLEAR(_pL3Db[unit]->vid_valid, intfEntry.egrIntf.dst_vid);
        BITMAP_SET(_pL3Db[unit]->vid_valid, pIntf->vid);
    }
#endif
    intfEntry.egrIntf.dst_vid   = pIntf->vid;
    intfEntry.egrIntf.smac_idx = smacIdx;
    intfEntry.egrIntf.ipmc_ttl_scope    = pIntf->ttl;
    intfEntry.egrIntf.ip6mc_hl_scope   = pIntf->ttl;
    RT_ERR_HDL(_dal_longan_l3_intfEgrEntry_set(unit, intfId, &intfEntry.egrIntf, (DAL_LONGAN_L3_API_FLAG_NONE)), errIntfSet, ret);

    L3_DB_UPDATE_INTF_VALID(unit, intfId, intfEntry.egrIntf.ip_mtu_idx, smacIdx, pIntf->vid, intfEntry.egrIntf.ip6_mtu_idx, pIntf->flags);

    goto errOk;

errIntfSet:
errMacRealloc:
errSmacSet:
    if (pIntf->ipv6_mtu != ip6_mtu)
        L3_RT_ERR_HDL_DBG(_dal_longan_l3_mtuIp6Entry_free(unit, intfEntry.egrIntf.ip6_mtu_idx), "");
errMtuIp6Realloc:
    if (pIntf->mtu != ip_mtu)
        L3_RT_ERR_HDL_DBG(_dal_longan_l3_mtuEntry_free(unit, intfEntry.egrIntf.ip_mtu_idx), "");
errMtuRealloc:
errEntryNotFound:
errIpMtuGet:
errIntfGet:
errType:
errMacGet:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_intf_set */

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
int32
dal_longan_l3_vrrp_add(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId)
{
    int32   ret = RT_ERR_OK;
    int32   macIdx, emptyEntryIdx;
    rtk_mac_t   vrrpMac;
    dal_longan_l3_macEntry_t macEntry;

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

    emptyEntryIdx = -1;
    /* search a suitalb MAC entry */
    for (macIdx=0; macIdx<DAL_LONGAN_L3_MAC_MAX; macIdx++)
    {
        if (_pL3Db[unit]->HW.route_mac[macIdx].ref_count > 0)
        {
            RT_ERR_HDL(_dal_longan_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntryGet, ret);
            if ((macEntry.valid) &&
                (macEntry.intf_id == vid) &&
                (0 == rt_util_macCmp(macEntry.mac.octet, vrrpMac.octet)))
            {
                ret = RT_ERR_INPUT; /* exist */
                goto errInput;
            }
        }
        else if (emptyEntryIdx < 0)
        {
            emptyEntryIdx = macIdx;   /* first empty entry */
        }
    }

    if (emptyEntryIdx < 0)
    {
        ret = RT_ERR_EXCEEDS_CAPACITY;
        goto errExceedsCapacity;
    }
    else
    {
        osal_memset(&macEntry, 0x00, sizeof(dal_longan_l3_macEntry_t));
        macEntry.valid      = 1;
        macEntry.intf_id    = vid;
        macEntry.mac        = vrrpMac;
        macEntry.bmsk_intf_id = 0x3FF;
        macEntry.act        = RTK_L3_ACT_FORWARD;
        osal_memset(&macEntry.bmsk_mac, 0xFF, sizeof(rtk_mac_t));
        RT_ERR_HDL(_dal_longan_l3_macEntry_set(unit, emptyEntryIdx, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntrySet, ret);

        osal_memcpy(_pL3Db[unit]->HW.route_mac[emptyEntryIdx].mac.octet, macEntry.mac.octet,ETHER_ADDR_LEN);
        _pL3Db[unit]->HW.route_mac[emptyEntryIdx].ref_count = 1;
        _pL3Db[unit]->HW.route_mac[emptyEntryIdx].intf_id = vid;
        _pL3Db[unit]->HW.mac_used_count += 1;

    }

errMacEntryGet:
errInput:
errExceedsCapacity:
errMacEntrySet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_vrrp_add */

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
int32
dal_longan_l3_vrrp_del(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrId)
{
    int32   ret = RT_ERR_OK;
    int32   macIdx;
    rtk_mac_t   vrrpMac;
    dal_longan_l3_macEntry_t macEntry;

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

    /* scan empty entry */
    for (macIdx=0; macIdx<DAL_LONGAN_L3_MAC_MAX; macIdx++)
    {
        if (_pL3Db[unit]->HW.route_mac[macIdx].ref_count == 0)
            continue;

        RT_ERR_HDL(_dal_longan_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntryGet, ret);
        if ((macEntry.valid) &&
            (macEntry.intf_id == vid) &&
            (0 == rt_util_macCmp(macEntry.mac.octet, vrrpMac.octet)))
        {
            RT_ERR_HDL(_dal_longan_l3_macEntry_free(unit, &macEntry), errMacEntryFree, ret);
            ret = RT_ERR_OK;
            goto errOk;
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errMacEntryGet:
errMacEntryFree:
errOk:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_vrrp_del */

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
int32
dal_longan_l3_vrrp_delAll(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid)
{
    int32   ret = RT_ERR_OK;
    int32   macIdx;
    rtk_mac_t   vrrpMac;
    dal_longan_l3_macEntry_t macEntry, emptyEntry;

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
    osal_memset(&emptyEntry, 0x00, sizeof(dal_longan_l3_macEntry_t));

    /* function body */
    L3_SEM_LOCK(unit);

    /* scan empty entry and existence */
    for (macIdx=0; macIdx<DAL_LONGAN_L3_MAC_MAX; macIdx++)
    {
        if (_pL3Db[unit]->HW.route_mac[macIdx].ref_count == 0)
            continue;

        RT_ERR_HDL(_dal_longan_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntryGet, ret);
        if ((macEntry.valid) &&
            (macEntry.intf_id == vid) &&
            (0 == osal_memcmp(&vrrpMac.octet, macEntry.mac.octet, 0x05)))   /* VRRP MAC address */
        {
            RT_ERR_HDL(_dal_longan_l3_macEntry_free(unit, &macEntry), errMacEntryFree, ret);
        }
    }

    ret = RT_ERR_OK;

errMacEntryGet:
errMacEntryFree:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_vrrp_delAll */

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
int32
dal_longan_l3_vrrp_get(uint32 unit, rtk_l3_vrrp_flag_t flags, rtk_vlan_t vid, uint32 vrIdArraySize, uint32 *pVrIdArray, uint32 *pVrIdCount)
{
    int32   ret = RT_ERR_OK;
    int32   macIdx;
    rtk_mac_t   vrrpMac;
    dal_longan_l3_macEntry_t macEntry;

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

    /* scan empty entry and existence */
    *pVrIdCount = 0;
    for (macIdx=0; macIdx<DAL_LONGAN_L3_MAC_MAX; macIdx++)
    {
        if (_pL3Db[unit]->HW.route_mac[macIdx].ref_count == 0)
            continue;

        RT_ERR_HDL(_dal_longan_l3_macEntry_get(unit, macIdx, &macEntry, (DAL_LONGAN_L3_API_FLAG_NONE)), errMacEntryGet, ret);
        if ((macEntry.valid) &&
            (macEntry.intf_id == vid) &&
            (0 == osal_memcmp(&vrrpMac.octet, macEntry.mac.octet, 0x05)))   /* VRRP MAC address */
        {
            *(pVrIdArray + (*pVrIdCount)) = (uint32)(macEntry.mac.octet[5]);    /* VRID */
            *pVrIdCount += 1;
        }
    }

    ret = RT_ERR_OK;

errMacEntryGet:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_vrrp_get */

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
int32
dal_longan_l3_nextHop_create(uint32 unit, rtk_l3_flag_t flags, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx, nhIdx;
    dal_longan_l2_ucastNhAddr_t l2ucEntry;
    dal_longan_l2_mcastNhAddr_t l2mcEntry;
    dal_longan_l3_nhEntry_t nhEntry;
    uint32  oldIntfIdx = 0;
    uint32  isAllocNh = FALSE, isAddL2Nh = FALSE;
    uint32  l2_dmac_idx;
    uint32  dstIdx;
    rtk_mac_t   nh_mac_addr;       /* nexthop destination MAC address */

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=%d,pNextHop=%p,pPathId=%p", unit, flags, pNextHop, pPathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    l2_dmac_idx = 0;

    /* parameter check */
    RT_PARAM_CHK(((~(RTK_L3_FLAG_REPLACE | RTK_L3_FLAG_WITH_ID|RTK_L3_FLAG_WITH_NH_DMAC) & flags) != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNextHop->intf_id >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(!L3_INTF_IDX_IS_VALID(unit, pNextHop->intf_id), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
    L3_SEM_LOCK(unit);

    /* intfId to intfIdx */
    intfIdx = DAL_LONGAN_L3_INTF_ID_ENTRY_IDX(pNextHop->intf_id);
    if (0 == _pL3Db[unit]->intf[intfIdx].valid)
    {
        ret = RT_ERR_INTF_NOT_EXIST;
        goto errNotFound;
    }

    if (flags & RTK_L3_FLAG_WITH_NH_DMAC)
        osal_memcpy(&nh_mac_addr, &(pNextHop->nh_mac_addr), sizeof(rtk_mac_t));
    else
        osal_memcpy(&nh_mac_addr, &(pNextHop->mac_addr), sizeof(rtk_mac_t));


    if (flags & RTK_L3_FLAG_REPLACE)
    {
        /* check validation */
        nhIdx = DAL_LONGAN_L3_PATH_ID_ENTRY_IDX(*pPathId);
        if ((0 == _pL3Db[unit]->nexthop[nhIdx].valid) || (DAL_LONGAN_INVALID_NEXTHOP_IDX == nhIdx))
        {
            ret = RT_ERR_NEXTHOP_NOT_EXIST;
            goto errNotFound;
        }

        if ((RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->nexthop[nhIdx].mac_addr.octet, pNextHop->mac_addr.octet)) && \
                (_pL3Db[unit]->nexthop[nhIdx].intf_idx == intfIdx)&&(_pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc == TRUE)
                &&(pNextHop->l3_act == RTK_L3_ACT_FORWARD))
        {
            isAddL2Nh = FALSE;
            l2_dmac_idx = _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx;
        }
        else
        {
            if(pNextHop->l3_act == RTK_L3_ACT_FORWARD)
            {
                isAddL2Nh = TRUE;
            }

            /* should check reference? */
            if(TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
            {
                /* save old intf id */
                oldIntfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;

                /* free old L2 DMAC entry first, when mac or pathId changed */
                /* prepare L2 entry for deleteing */
                if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
                {
                    osal_memset(&l2mcEntry, 0x00, sizeof(dal_longan_l2_mcastNhAddr_t));
                    RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[oldIntfIdx].vid, \
                               &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2mcEntry.vid), errFidGet, ret);
                    l2mcEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;

                    L3_RT_ERR_HDL_DBG(_dal_longan_l2_mcastNexthop_del(unit, &l2mcEntry), "");
                    dstIdx = l2mcEntry.mac_idx;
                }
                else
                {
                    osal_memset(&l2ucEntry, 0x00, sizeof(dal_longan_l2_ucastNhAddr_t));
                    RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[oldIntfIdx].vid, \
                               &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2ucEntry.fid), errFidGet, ret);
                    l2ucEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;

                    L3_RT_ERR_HDL_DBG(_dal_longan_l2_nexthop_del(unit, &l2ucEntry), "");
                    dstIdx = l2ucEntry.mac_idx;
                }
                RT_ERR_HDL(_dal_longan_l3_dmacEntry_free(unit, dstIdx), errDmacEntrySet, ret);

                _pL3Db[unit]->nexthop[nhIdx].mac_addr = pNextHop->mac_addr;
                /* check if Interface has been changed */
                if (_pL3Db[unit]->nexthop[nhIdx].intf_idx != intfIdx)
                {
                    oldIntfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;
                    _pL3Db[unit]->intf[oldIntfIdx].ref_count -= 1;
                    _pL3Db[unit]->nexthop[nhIdx].intf_idx = intfIdx;    /* new interface */
                    _pL3Db[unit]->intf[intfIdx].ref_count += 1;
                }
                _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = FALSE;
            }
            else
            {
                _pL3Db[unit]->nexthop[nhIdx].mac_addr = pNextHop->mac_addr;
                /* check if Interface has been changed */
                if (_pL3Db[unit]->nexthop[nhIdx].intf_idx != intfIdx)
                {
                    oldIntfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;
                    _pL3Db[unit]->intf[oldIntfIdx].ref_count -= 1;
                    _pL3Db[unit]->nexthop[nhIdx].intf_idx = intfIdx;    /* new interface */
                    _pL3Db[unit]->intf[intfIdx].ref_count += 1;
                }
            }
        }
    }
    else
    {
        /* try to alloc an empty nexthop entry */
        if (flags & RTK_L3_FLAG_WITH_ID)
        {
            nhIdx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(*pPathId);
            RT_ERR_HDL(_dal_longan_l3_nhEntry_alloc(unit, &nhIdx, DAL_LONGAN_L3_API_FLAG_WITH_ID), errL3NhAlloc, ret);
        }
        else
        {
            RT_ERR_HDL(_dal_longan_l3_nhEntry_alloc(unit, &nhIdx, DAL_LONGAN_L3_API_FLAG_NONE), errL3NhAlloc, ret);
        }
        isAllocNh = TRUE;
        if(pNextHop->l3_act == RTK_L3_ACT_FORWARD)
        {
            isAddL2Nh = TRUE;
        }
    }

    if(TRUE == isAddL2Nh)
    {
        /* prepare L2 entry for creating */
        osal_memset(&l2mcEntry, 0x00, sizeof(dal_longan_l2_mcastNhAddr_t));
        osal_memset(&l2ucEntry, 0x00, sizeof(dal_longan_l2_ucastNhAddr_t));
        dstIdx = nhIdx;
        RT_ERR_HDL(_dal_longan_l3_dmacEntry_alloc(unit, nh_mac_addr, &dstIdx), errDmacEntrySet, ret);
        if (TRUE == l3_util_macAddr_isMcast(&pNextHop->mac_addr))
        {
            RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                       &pNextHop->mac_addr, &l2mcEntry.vid), errFidGet, ret);
            l2mcEntry.mac = pNextHop->mac_addr;
            l2mcEntry.mac_idx = dstIdx;
            l2mcEntry.add_op_flags = RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC;

            /* call L2 internal API to add a MAC entry */
            RT_ERR_HDL(_dal_longan_l2_mcastNexthop_add(unit, &l2mcEntry), errL2NhAdd, ret);
            l2_dmac_idx = l2mcEntry.l2_idx;
        }
        else
        {
            RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                       &pNextHop->mac_addr, &l2ucEntry.fid), errFidGet, ret);
            l2ucEntry.mac = pNextHop->mac_addr;
            l2ucEntry.mac_idx = dstIdx;
            l2ucEntry.add_op_flags = RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC;

            /* call L2 internal API to add a MAC entry */
            RT_ERR_HDL(_dal_longan_l2_nexthop_add(unit, &l2ucEntry), errL2NhAdd, ret);
            l2_dmac_idx = l2ucEntry.l2_idx;
        }
        RT_ERR_HDL(_dal_longan_l3_nhDmac_set(unit, dstIdx, nh_mac_addr), errL3NhSet, ret);
    }

    /* write nexthop entry into chip */
    osal_memset(&nhEntry, 0x00, sizeof(dal_longan_l3_nhEntry_t));
    nhEntry.l3_egr_intf_idx = intfIdx;
    if (pNextHop->l3_act == RTK_L3_ACT_FORWARD)
    {
        nhEntry.dmac_idx = l2_dmac_idx;
    }
    else
    {
        switch (pNextHop->l3_act)
        {
            case RTK_L3_ACT_TRAP2CPU:
                nhEntry.dmac_idx = DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_TRAP2CPU;
                break;

            case RTK_L3_ACT_TRAP2MASTERCPU:
                nhEntry.dmac_idx = DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER;
                break;

            case RTK_L3_ACT_DROP:
            default:
                nhEntry.dmac_idx = DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_DROP;
                break;
        }
    }
    RT_ERR_HDL(_dal_longan_l3_nhEntry_set(unit, nhIdx, &nhEntry, DAL_LONGAN_L3_API_FLAG_NONE), errL3NhSet, ret);

     /* nhIdx to pathId */
    *pPathId = DAL_LONGAN_L3_NH_IDX_TO_PATH_ID(nhIdx);

     /* update DB */
     if (isAllocNh)
     {
        _pL3Db[unit]->nexthop[nhIdx].valid = 1;
        _pL3Db[unit]->intf[intfIdx].ref_count += 1;
     }

    if (isAddL2Nh)
    {
        _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx = l2_dmac_idx;
        _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = TRUE;
    }
    _pL3Db[unit]->nexthop[nhIdx].mac_addr = pNextHop->mac_addr;
    _pL3Db[unit]->nexthop[nhIdx].intf_idx = intfIdx;


    goto errOk;

errL2NhAdd:
errL3NhSet:
        if (isAddL2Nh)
        {
            if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
            {
                L3_RT_ERR_HDL_DBG(_dal_longan_l2_mcastNexthop_del(unit, &l2mcEntry), "");
            }
            else
            {
                L3_RT_ERR_HDL_DBG(_dal_longan_l2_nexthop_del(unit, &l2ucEntry), "");
            }

            L3_RT_ERR_HDL_DBG(_dal_longan_l3_dmacEntry_free(unit, dstIdx) , "");
        }
errL3NhAlloc:
errDmacEntrySet:
        if (isAllocNh)
            L3_RT_ERR_HDL_DBG(_dal_longan_l3_nhEntry_free(unit, nhIdx, DAL_LONGAN_L3_API_FLAG_NONE), "");
errFidGet:
errNotFound:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_nextHop_create */


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
int32
dal_longan_l3_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx, nhIdx, isMcast = FALSE;
    dal_longan_l2_ucastNhAddr_t l2ucEntry;
    dal_longan_l2_mcastNhAddr_t l2mcEntry;
    uint32  dstIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pathId=%d", unit, pathId);

    //check the reserver pathId;
    RT_PARAM_CHK((pathId >= DAL_LONGAN_L3_NEXTHOP_MAX-3), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pathId == DAL_LONGAN_INVALID_NEXTHOP_IDX), RT_ERR_ENTRY_NOTFOUND);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* function body */
    L3_SEM_LOCK(unit);

    /* pathId to nhIdx */
    nhIdx = DAL_LONGAN_L3_PATH_ID_ENTRY_IDX(pathId);
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

    if(TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
    {
        /* prepare L2 entry for deleteing */
        osal_memset(&l2mcEntry, 0x00, sizeof(dal_longan_l2_mcastNhAddr_t));
        osal_memset(&l2ucEntry, 0x00, sizeof(dal_longan_l2_ucastNhAddr_t));
        if (TRUE == l3_util_macAddr_isMcast(&_pL3Db[unit]->nexthop[nhIdx].mac_addr))
        {
            RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                       &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2mcEntry.vid), errFidGet, ret);
            l2mcEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
            isMcast = TRUE;
        }
        else
        {
            RT_ERR_HDL(l3_util_vlanMacFid_get(unit, _pL3Db[unit]->intf[intfIdx].vid, \
                       &_pL3Db[unit]->nexthop[nhIdx].mac_addr, &l2ucEntry.fid), errFidGet, ret);
            l2ucEntry.mac = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
            isMcast = FALSE;
        }
    }

    /* update DB */
    _pL3Db[unit]->intf[intfIdx].ref_count -= 1;
    osal_memset(&_pL3Db[unit]->nexthop[nhIdx].mac_addr, 0x00, sizeof(rtk_mac_t));
    _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx = 0;
    _pL3Db[unit]->nexthop[nhIdx].intf_idx = 0;
    _pL3Db[unit]->nexthop[nhIdx].valid = 0;

    RT_ERR_HDL(_dal_longan_l3_nhEntry_free(unit, nhIdx, DAL_LONGAN_L3_API_FLAG_NONE), errL3NhFree, ret);

errL3NhFree:
    if(TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
    {
        if (TRUE == isMcast)
        {
            L3_RT_ERR_HDL_DBG(_dal_longan_l2_mcastNexthop_del(unit, &l2mcEntry), "");
            dstIdx = l2mcEntry.mac_idx;
        }
        else
        {
            L3_RT_ERR_HDL_DBG(_dal_longan_l2_nexthop_del(unit, &l2ucEntry), "");
            dstIdx = l2ucEntry.mac_idx;
        }

        RT_ERR_HDL(_dal_longan_l3_dmacEntry_free(unit, dstIdx), errDmacEntrySet, ret);
        _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc = FALSE;
    }


errFidGet:
errEntryReferred:
errEntryNotFound:
errDmacEntrySet:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_nextHop_destroy */


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
int32
dal_longan_l3_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,  rtk_l3_nextHop_t *pNextHop)
{
    int32   ret = RT_ERR_OK;
    uint32  nhIdx;
    dal_longan_l3_nhEntry_t nhEntry;
    dal_longan_l2_index_t l2Index;
    dal_longan_l2_entry_t l2Entry;
    rtk_mac_t   nh_mac_addr;       /* nexthop destination MAC address */

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pathId=%d", unit, pathId);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pathId >= DAL_LONGAN_L3_NEXTHOP_MAX-3), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pathId == DAL_LONGAN_INVALID_NEXTHOP_IDX), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
    L3_SEM_LOCK(unit);

    /* pathId to nhIdx */
    nhIdx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(pathId);

    /* check validation */
    if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    /* for debugging purpose, we should get actual data from chip */
    RT_ERR_HDL(_dal_longan_l3_nhEntry_get(unit, nhIdx, &nhEntry, DAL_LONGAN_L3_API_FLAG_NONE), errNhEntrySet, ret);

    pNextHop->intf_id = _pL3Db[unit]->intf[(nhEntry.l3_egr_intf_idx % HAL_MAX_NUM_OF_INTF(unit))].intf_id;

    if(TRUE == _pL3Db[unit]->nexthop[nhIdx].mac_addr_alloc)
    {
        /* call L2 internal API to get MAC address (l2_idx to mac) */
        osal_memset(&l2Index, 0x00, sizeof(dal_longan_l2_index_t));
        L2_MAC_IDX_TO_INDEX_STRUCT(_pL3Db[unit]->nexthop[nhIdx].mac_addr_idx, l2Index);
        RT_ERR_HDL(_dal_longan_l2_getL2Entry(unit, &l2Index, &l2Entry), errL2EntryGet, ret);
        pNextHop->mac_addr = (l2Entry.entry_type == L2_MULTICAST)? l2Entry.l2mcast.mac : l2Entry.unicast.mac;
        RT_ERR_HDL(_dal_longan_l3_nhDmac_get(unit, (l2Entry.entry_type == L2_MULTICAST)? l2Entry.l2mcast.mac_idx : l2Entry.unicast.mac_idx, &nh_mac_addr), errL2EntryGet, ret);
        pNextHop->nh_mac_addr = nh_mac_addr;
    }
    else
    {
        pNextHop->mac_addr = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
        pNextHop->nh_mac_addr = _pL3Db[unit]->nexthop[nhIdx].mac_addr;
    }

    /* action */
    switch (nhEntry.dmac_idx)
    {
    case DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_TRAP2CPU:
        pNextHop->l3_act = RTK_L3_ACT_TRAP2CPU;
        break;

    case DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER:
        pNextHop->l3_act = RTK_L3_ACT_TRAP2MASTERCPU;
        break;

    case DAL_LONGAN_L3_NEXTHOP_DMAC_IDX_DROP:
        pNextHop->l3_act = RTK_L3_ACT_DROP;
        break;

    default:
        pNextHop->l3_act = RTK_L3_ACT_FORWARD;
        break;
    }
errL2EntryGet:
errNhEntrySet:
errEntryNotFound:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_nextHop_get */


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
int32
dal_longan_l3_nextHopPath_find(uint32 unit, rtk_l3_nextHop_t *pNextHop, rtk_l3_pathId_t *pPathId)
{
    int32   ret = RT_ERR_OK;
    uint32  nhIdx;
    dal_longan_l3_nhEntry_t nhEntry;
    rtk_l2_ucastAddr_t  l2Uncast;
    rtk_l2_mcastAddr_t  l2Mcast;
    uint32  isMcast = 0, mac_idx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNextHop), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNextHop->intf_id >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L3_SEM_LOCK(unit);

    /* call L2 internal API to get L2 MAC */
    if (DAL_LONGAN_L3_PATHID_METHOD_ASIC == _pL3Db[unit]->nexthop_find_method)
    {
        if (l3_util_macAddr_isMcast(&pNextHop->mac_addr))
        {
            RT_ERR_HDL(dal_longan_l2_mcastAddr_init(unit, _pL3Db[unit]->intf[pNextHop->intf_id].vid, &pNextHop->mac_addr, &l2Mcast), errL2EntryGet, ret);
            RT_ERR_HDL(dal_longan_l2_mcastAddr_get(unit, &l2Mcast), errL2EntryGet, ret);
            isMcast = 1;
            mac_idx = l2Mcast.l2_idx;
        }
        else
        {
            RT_ERR_HDL(dal_longan_l2_addr_init(unit, _pL3Db[unit]->intf[pNextHop->intf_id].vid, &pNextHop->mac_addr, &l2Uncast), errL2EntryGet, ret);
            RT_ERR_HDL(dal_longan_l2_addr_get(unit, &l2Uncast), errL2EntryGet, ret);
            mac_idx = l2Uncast.l2_idx;
        }

        if ((isMcast && (0 == l2Mcast.nextHop)) || (!isMcast && (0 == (RTK_L2_UCAST_FLAG_NEXTHOP & l2Uncast.flags))))
        {
            ret =  RT_ERR_ENTRY_NOTFOUND;
            goto errOk;
        }
    }

    for (nhIdx=0; nhIdx<DAL_LONGAN_L3_NEXTHOP_MAX; nhIdx++)
    {
        if (_pL3Db[unit]->nexthop[nhIdx].valid)
        {
            if (DAL_LONGAN_L3_PATHID_METHOD_ASIC == _pL3Db[unit]->nexthop_find_method)
            {
                RT_ERR_HDL(_dal_longan_l3_nhEntry_get(unit, nhIdx, &nhEntry, DAL_LONGAN_L3_API_FLAG_NONE), errNhEntryGet, ret);
                if ((pNextHop->intf_id == _pL3Db[unit]->intf[(nhEntry.l3_egr_intf_idx % HAL_MAX_NUM_OF_INTF(unit))].intf_id) && \
                        (mac_idx == _pL3Db[unit]->nexthop[nhIdx].mac_addr_idx))
                {
                    /* nhIdx to pathId */
                    *pPathId = DAL_LONGAN_L3_NH_IDX_TO_PATH_ID(nhIdx);
                    goto errOk;
                }
            }
            else    /*software find*/
            {
                if ((_pL3Db[unit]->nexthop[nhIdx].intf_idx == pNextHop->intf_id) &&
                        (RT_ERR_OK == rt_util_macCmp(_pL3Db[unit]->nexthop[nhIdx].mac_addr.octet, pNextHop->mac_addr.octet)))
                {
                    /* nhIdx to pathId */
                    *pPathId = DAL_LONGAN_L3_NH_IDX_TO_PATH_ID(nhIdx);
                    goto errOk;
                }
            }
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errNhEntryGet:
errL2EntryGet:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_nextHopPath_find */

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
int32
dal_longan_l3_host_add(uint32 unit, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    dal_longan_l3_hostAlloc_t    hostAlloc;
    dal_longan_l3_hostEntry_t    hostEntry;
    uint32  hostIdx;
    uint32  nhIdx=0;
    int32    tmpHostIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pHost=%p", unit, pHost);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pHost->vrf_id), RT_ERR_INPUT);
    RT_PARAM_CHK((pHost->qos_pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((pHost->path_id >= DAL_LONGAN_L3_NEXTHOP_MAX-3), RT_ERR_INPUT);
    RT_PARAM_CHK((pHost->nexthop.intf_id >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_INPUT);

    //get nexthop id
    if (!(pHost->flags & RTK_L3_FLAG_NULL_INTF))
    {
        if (pHost->flags & RTK_L3_FLAG_WITH_NEXTHOP)
        {
            if ((RTK_L3_HOST_ACT_FORWARD == pHost->fwd_act) ||(RTK_L3_HOST_ACT_COPY2CPU == pHost->fwd_act))
            {
                RT_ERR_HDL(dal_longan_l3_nextHopPath_find(unit, &pHost->nexthop, &nhIdx), errHandleUnlock,ret);
                pHost->path_id = nhIdx;
            }
        }
        else
        {
            nhIdx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(pHost->path_id);
        }
    }

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
    if (!(pHost->flags & RTK_L3_FLAG_NULL_INTF))
    {
        if (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX)
        {
            if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
        }
        else
        {
            /* if the path_id = 0, can only set the action is trap/drop */
            if ((RTK_L3_HOST_ACT_FORWARD == pHost->fwd_act) ||(RTK_L3_HOST_ACT_COPY2CPU == pHost->fwd_act))
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
        }
    }

    /* check the entry is exsit */
    ret = _dal_longan_l3_hostEntry_find(unit, pHost, NULL, &hostAlloc, &tmpHostIdx);
    if (RT_ERR_ENTRY_NOTFOUND == ret)
    {
        if (pHost->flags & RTK_L3_FLAG_REPLACE)
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }

        /* allocate L3 host entry */
        RT_ERR_HDL(_dal_longan_l3_hostEntry_alloc(unit, &hostAlloc, &hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

        _pL3Db[unit]->host[hostIdx].valid = TRUE;
        _pL3Db[unit]->host[hostIdx].ipv6 = (pHost->flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;
        if ((!(pHost->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
            _pL3Db[unit]->nexthop[nhIdx].ref_count++;
    }
    else if (RT_ERR_OK == ret)
    {
        if (!(pHost->flags & RTK_L3_FLAG_REPLACE))
        {
            ret = RT_ERR_ENTRY_EXIST;
            goto errEntryExist;
        }

        RT_ERR_HDL(_dal_longan_l3_host_get(unit, tmpHostIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &hostEntry, L3_ENTRY_HOST_ADDR(unit, tmpHostIdx)), errHandle, ret);
        /* old nexthop ref_count--; */
        if (!hostEntry.dst_null_intf)
            _pL3Db[unit]->nexthop[hostEntry.nh_ecmp_idx].ref_count--;

        if (!(pHost->flags & RTK_L3_FLAG_NULL_INTF))
            _pL3Db[unit]->nexthop[nhIdx].ref_count++;

        hostIdx = (uint32)tmpHostIdx; //set index;
    }
    else
    {
        /* error */
        goto errHandle;
    }

     /* hostEntry preparation */
    RT_ERR_HDL(l3_util_rtkHost2hostEntry(&hostEntry, pHost), errHandle, ret);
    /* write into chip */
    RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

     _pL3Db[unit]->host[hostIdx].l3_entry = *pHost;

errEntryNotFound:
errEntryExist:
errHandle:
    L3_SEM_UNLOCK(unit);
errHandleUnlock:
    return ret;
}   /* end of dal_longan_l3_host_add */


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
 *          ip_addr (either ipv4 or ipv6).
 */
int32
dal_longan_l3_host_del(uint32 unit, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    int32   hostIdx;
    dal_longan_l3_hostEntry_t    hostEntry;
    uint32  nhIdx = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pHost->vrf_id), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    ret = _dal_longan_l3_hostEntry_find(unit, pHost, &hostEntry,NULL, &hostIdx);
    if (RT_ERR_OK == ret)
    {
         nhIdx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(hostEntry.nh_ecmp_idx);
        /* write into chip */
        osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
        RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

        /* release L3 host entry */
        RT_ERR_HDL(_dal_longan_l3_hostEntry_free(unit, hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

        _pL3Db[unit]->host[hostIdx].ipv6 = FALSE;
        _pL3Db[unit]->host[hostIdx].valid = FALSE;
        if ((!(pHost->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
            _pL3Db[unit]->nexthop[nhIdx].ref_count--;

        osal_memset(&_pL3Db[unit]->host[hostIdx].l3_entry, 0, sizeof(rtk_l3_host_t));

        goto errOk;
    }

    ret = RT_ERR_ENTRY_NOTFOUND;
    goto errEntryNotFound;

errEntryNotFound:
errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_host_del */


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
int32
dal_longan_l3_host_del_byNetwork(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    uint32  nhIdx;
    dal_longan_l3_hostEntry_t    hostEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pRoute->vrf_id), RT_ERR_INPUT);
    RT_PARAM_CHK(((0 == (pRoute->flags & RTK_L3_FLAG_IPV6)) && (pRoute->prefix_len > 32)) , RT_ERR_INPUT);
    RT_PARAM_CHK(((0 != (pRoute->flags & RTK_L3_FLAG_IPV6)) && (pRoute->prefix_len > 128)) , RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (pRoute->flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_LONGAN_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_longan_l3_host_get(unit, hostIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &hostEntry, L3_ENTRY_HOST_ADDR(unit, hostIdx)), errHandle, ret);

            if ((((FALSE == ipv6) && (pRoute->prefix_len == rt_util_ipMaxMatchLength_ret(hostEntry.ip, pRoute->ip_addr.ipv4, pRoute->prefix_len))) || \
                 ((TRUE == ipv6) && (pRoute->prefix_len == rt_util_ipv6MaxMatchLength_ret(&hostEntry.ip6, &pRoute->ip_addr.ipv6, pRoute->prefix_len)))))
            {
                /* write into chip */
                nhIdx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(hostEntry.nh_ecmp_idx);
                osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
                RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

                /* release L3 host entry */
                RT_ERR_HDL(_dal_longan_l3_hostEntry_free(unit, hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

                _pL3Db[unit]->host[hostIdx].ipv6 = FALSE;
                _pL3Db[unit]->host[hostIdx].valid = FALSE;
                if ((!(L3_ENTRY_HOST_ADDR(unit, hostIdx)->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
                    _pL3Db[unit]->nexthop[nhIdx].ref_count--;

                osal_memset(&_pL3Db[unit]->host[hostIdx].l3_entry, 0, sizeof(rtk_l3_host_t));
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_host_del_byNetwork */


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
int32
dal_longan_l3_host_del_byIntfId(uint32 unit, rtk_intf_id_t intfId, rtk_l3_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6, negate;
    uint32  hostIdx;
    dal_longan_l3_hostEntry_t    hostEntry;
    uint32  nhIdx, intfIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    RT_PARAM_CHK((intfId >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;
    negate = (flags & RTK_L3_FLAG_NEGATE)? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_LONGAN_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_longan_l3_host_get(unit, hostIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &hostEntry, L3_ENTRY_HOST_ADDR(unit, hostIdx)), errHandle, ret);

            /* currently, only remove 'nexthop' type entry */
            nhIdx = hostEntry.nh_ecmp_idx;
            intfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;

            /* confirm all info */
            if ( (_pL3Db[unit]->nexthop[nhIdx].valid) && \
                (_pL3Db[unit]->intf[intfIdx].valid) && \
                (((FALSE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id == intfId)) || \
                 ((TRUE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id != intfId))))
            {
                /* write into chip */
                osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
                RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

                /* release L3 host entry */
                RT_ERR_HDL(_dal_longan_l3_hostEntry_free(unit, hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

                _pL3Db[unit]->host[hostIdx].ipv6 = FALSE;
                _pL3Db[unit]->host[hostIdx].valid = FALSE;
                if ((!(L3_ENTRY_HOST_ADDR(unit, hostIdx)->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
                    _pL3Db[unit]->nexthop[nhIdx].ref_count--;

                osal_memset(&_pL3Db[unit]->host[hostIdx].l3_entry, 0, sizeof(rtk_l3_host_t));
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_host_del_byIntfId */


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
int32
dal_longan_l3_host_delAll(uint32 unit, rtk_l3_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    uint32  nhIdx;
    dal_longan_l3_hostEntry_t    hostEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x", unit, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_LONGAN_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_longan_l3_host_get(unit, hostIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &hostEntry, L3_ENTRY_HOST_ADDR(unit, hostIdx)), errHandle, ret);
            nhIdx = hostEntry.nh_ecmp_idx;

            /* write into chip */
            osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
            RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

            /* release L3 host entry */
            RT_ERR_HDL(_dal_longan_l3_hostEntry_free(unit, hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

            _pL3Db[unit]->host[hostIdx].ipv6 = FALSE;
            _pL3Db[unit]->host[hostIdx].valid = FALSE;
            if ((!(L3_ENTRY_HOST_ADDR(unit, hostIdx)->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
                _pL3Db[unit]->nexthop[nhIdx].ref_count--;
            osal_memset(&_pL3Db[unit]->host[hostIdx].l3_entry, 0, sizeof(rtk_l3_host_t));
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_host_delAll */


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
int32
dal_longan_l3_host_find(uint32 unit, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    int32   hostIdx;
    dal_longan_l3_hostEntry_t   hostEntry;
    rtk_l3_host_t   rtkHost;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pHost=%p", unit, pHost);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHost), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pHost->vrf_id), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
    ret = _dal_longan_l3_hostEntry_find(unit, pHost, &hostEntry,NULL, &hostIdx);
    if (RT_ERR_OK == ret)
    {
        /* return the data */
        RT_ERR_HDL(l3_util_hostEntry2rtkHost(&rtkHost, &hostEntry), errHandle, ret);
        if (pHost->flags & RTK_L3_FLAG_HIT_CLEAR)
        {
            if (pHost->flags & RTK_L3_FLAG_READ_HIT_IGNORE)
            {
                ret = RT_ERR_INPUT;
                goto errHandle;
            }

            if (hostEntry.hit)
            {
                hostEntry.hit = 0;  /* clear hit bit */
                RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
            }
        }

        /*return the data*/
        *pHost = rtkHost;
    }

errHandle:

    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_host_find */


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
 *          ip_addr (either ipv4 or ipv6).
 */
int32
dal_longan_l3_hostConflict_get(uint32 unit, rtk_l3_key_t *pKey, rtk_l3_host_t *pHostArray, int32 maxHost, int32 *pHostCount)
{
    int32   ret = RT_ERR_OK;
    dal_longan_l3_hostHashKey_t  hashKey;
    dal_longan_l3_hostHashIdx_t  hashIdx;
    dal_longan_l3_hostEntry_t    hostEntry;
    uint32  ipv6, flags;
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
    RT_PARAM_CHK((0 >= maxHost || maxHost > HAL_MAX_NUM_OF_L3_CONFLICT_HOST(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((0 != pKey->vrf_id), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    /* get hash index */
    osal_memset(&hashKey, 0x00, sizeof(dal_longan_l3_hostHashKey_t));
    L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_0f, hashKey.alg_of_tbl[0], "", errHandle, ret);
    L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_UC_HASH_ALG_SEL_1f, hashKey.alg_of_tbl[1], "", errHandle, ret);

    if (pKey->flags & RTK_L3_FLAG_IPV6)
    {
        ipv6 = TRUE;
        hashKey.dip.ipv6 = pKey->ip_addr.ipv6;
        flags = DAL_LONGAN_L3_API_FLAG_IPV6;
    }
    else
    {
        ipv6 = FALSE;
        hashKey.dip.ipv4 = pKey->ip_addr.ipv4;
        flags = DAL_LONGAN_L3_API_FLAG_NONE;
    }
    RT_ERR_HDL(_dal_longan_l3_hostHashIdx_get(unit, &hashKey, &hashIdx, flags), errHandle, ret);

    /* search the entries which have the same index */
    numHost = 0;
    for (tbl=0; tbl<DAL_LONGAN_L3_HOST_TBL_NUM; tbl++)
    {
        for (slot=0; slot<DAL_LONGAN_L3_HOST_TBL_WIDTH; slot++)
        {
            addr = ((tbl & 0x1) << 12) | ((hashIdx.idx_of_tbl[tbl] & 0x1FF) << 3) | ((slot & 0x7) << 0);
            hostIdx = DAL_LONGAN_L3_ENTRY_ADDR_TO_IDX(addr);

            if (_pL3Db[unit]->host[hostIdx].valid)
            {
                RT_ERR_HDL(_dal_longan_l3_host_get(unit, hostIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &hostEntry, L3_ENTRY_HOST_ADDR(unit, hostIdx)), errHandle, ret);

                /* ignore itself */
                if ((((FALSE == ipv6) && \
                      (LONGAN_L3_HOST_IP_PFLEN == rt_util_ipMaxMatchLength_ret(hostEntry.ip, pKey->ip_addr.ipv4, LONGAN_L3_HOST_IP_PFLEN))) || \
                     ((TRUE == ipv6) && \
                      (LONGAN_L3_HOST_IP6_PFLEN == rt_util_ipv6MaxMatchLength_ret(&hostEntry.ip6, &pKey->ip_addr.ipv6, LONGAN_L3_HOST_IP6_PFLEN)))))
                {
                    continue;   /* try to find the next */
                }

                RT_ERR_HDL(l3_util_hostEntry2rtkHost((pHostArray + numHost), &hostEntry), errHandle, ret);
                numHost += 1;
                if (numHost >= maxHost)
                    break;
            }
        }
    }

    *pHostCount = numHost;

    goto errOk;

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_hostConflict_get */


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
int32
dal_longan_l3_host_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_hostTraverseCallback_f fAge, void *pCookie)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    dal_longan_l3_hostEntry_t    hostEntry;
    rtk_l3_host_t rtkHost;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x,fAge=%p,pCookie=%p", unit, flags, fAge, pCookie);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    ipv6 = (flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (hostIdx=0; hostIdx<DAL_LONGAN_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_longan_l3_host_get(unit, hostIdx, RTK_L3_FLAG_NONE, &hostEntry, L3_ENTRY_HOST_ADDR(unit, hostIdx)), errHandle, ret);

            if (flags & RTK_L3_FLAG_HIT)
            {
                /* age out entry if hit-bit is set */
                if (hostEntry.hit)
                {
                    /* callback function */
                    if (NULL != fAge)
                    {
                        RT_ERR_HDL(l3_util_hostEntry2rtkHost(&rtkHost, &hostEntry), errHandle, ret);

                        RT_ERR_HDL(fAge(unit, hostIdx, &rtkHost, pCookie), errHandle, ret);
                    }

                    /* clear hit bit */
                    hostEntry.hit = 0;
                    RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
                }
            }
            else
            {
                /* delete the entry since its hit bit is clear */
                if (!hostEntry.hit)
                {
                    _pL3Db[unit]->host[hostIdx].ipv6 = FALSE;
                    _pL3Db[unit]->host[hostIdx].valid = FALSE;
                    if ((!(L3_ENTRY_HOST_ADDR(unit, hostIdx)->flags & RTK_L3_FLAG_NULL_INTF)) && (hostEntry.nh_ecmp_idx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
                        _pL3Db[unit]->nexthop[hostEntry.nh_ecmp_idx].ref_count--;
                    osal_memset(&_pL3Db[unit]->host[hostIdx].l3_entry, 0, sizeof(rtk_l3_host_t));

                    /* write into chip */
                    osal_memset(&hostEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
                    RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, hostIdx, &hostEntry, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

                    /* release L3 host entry */
                    RT_ERR_HDL(_dal_longan_l3_hostEntry_free(unit, hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

                    /* callback function */
                    if (NULL != fAge)
                    {
                        RT_ERR_HDL(l3_util_hostEntry2rtkHost(&rtkHost, &hostEntry), errHandle, ret);

                        RT_ERR_HDL(fAge(unit, hostIdx, &rtkHost, pCookie), errHandle, ret);
                    }
                }
            }
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_host_age */


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
int32
dal_longan_l3_host_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_host_t *pHost)
{
    int32   ret = RT_ERR_OK;
    uint32  ipv6;
    uint32  hostIdx;
    dal_longan_l3_hostEntry_t    hostEntry;

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
    for (hostIdx=(*pBase<0)?(0):((*pBase)+1); hostIdx<DAL_LONGAN_L3_HOST_TBL_SIZE; hostIdx++)
    {
        if ((_pL3Db[unit]->host[hostIdx].valid) && \
            (_pL3Db[unit]->host[hostIdx].ipv6 == ipv6))
        {
            RT_ERR_HDL(_dal_longan_l3_host_get(unit, hostIdx, flags, &hostEntry, L3_ENTRY_HOST_ADDR(unit, hostIdx)), errHandle, ret);
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
}   /* end of dal_longan_l3_host_getNext */

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
int32
dal_longan_l3_route_add(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    dal_longan_l3_routeEntry_t   routeEntry;
    uint32  ipv6;
    uint32  routeIdx;
    int32   tmpRouteIdx;
    uint32  nhIdx=0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pRoute->path_id >= DAL_LONGAN_L3_NEXTHOP_MAX-3), RT_ERR_INPUT);
    RT_PARAM_CHK((pRoute->qos_pri > HAL_INTERNAL_PRIORITY_MAX(unit)), RT_ERR_QOS_INT_PRIORITY);
    RT_PARAM_CHK((pRoute->nexthop.intf_id >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_INPUT);

    RT_PARAM_CHK((0 != pRoute->vrf_id), RT_ERR_INPUT);
    RT_PARAM_CHK(((0 != pRoute->suffix_len) && (0 == (pRoute->flags & RTK_L3_FLAG_IPV6)) && \
                                (pRoute->prefix_len + pRoute->suffix_len > 32)) , RT_ERR_INPUT);
    RT_PARAM_CHK(((0 != pRoute->suffix_len) && (0 != (pRoute->flags & RTK_L3_FLAG_IPV6)) && \
                                (pRoute->prefix_len + pRoute->suffix_len > 128)) , RT_ERR_INPUT);

    /*get nexthop id*/
    if (!(pRoute->flags & RTK_L3_FLAG_NULL_INTF))
    {
        if (pRoute->flags & RTK_L3_FLAG_WITH_NEXTHOP)
        {
            if ((RTK_L3_ROUTE_ACT_FORWARD == pRoute->fwd_act) ||(RTK_L3_ROUTE_ACT_COPY2CPU == pRoute->fwd_act))
            {
                RT_ERR_HDL(dal_longan_l3_nextHopPath_find(unit, &pRoute->nexthop, &nhIdx), errHandleUnlock,ret);
                pRoute->path_id = nhIdx;
            }
        }
        else
        {
            nhIdx = DAL_LONGAN_L3_PATH_ID_TO_NH_IDX(pRoute->path_id);
        }
    }

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
    if (!(pRoute->flags & RTK_L3_FLAG_NULL_INTF))
    {
        if (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX)
        {
            if (0 == _pL3Db[unit]->nexthop[nhIdx].valid)
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
        }
        else
        {
            /* if the path_id = 0, can only set the action is trap/drop */
            if ((RTK_L3_ROUTE_ACT_FORWARD == pRoute->fwd_act) ||(RTK_L3_ROUTE_ACT_COPY2CPU == pRoute->fwd_act))
            {
                ret = RT_ERR_NEXTHOP_NOT_EXIST;
                goto errEntryNotFound;
            }
        }
    }

    if (RT_ERR_OK != _dal_longan_l3_routeEntry_find(unit, pRoute, &tmpRouteIdx, NULL))
    {
        if (pRoute->flags & RTK_L3_FLAG_REPLACE)
        {
            ret = RT_ERR_ENTRY_NOTFOUND;
            goto errEntryNotFound;
        }
        ipv6 = (pRoute->flags & RTK_L3_FLAG_IPV6)? TRUE : FALSE;

        /* allocate L3 route entry */
        RT_ERR_HDL(_dal_longan_l3_routeEntry_alloc(unit, ipv6, pRoute->prefix_len, &routeIdx), errHandle, ret);
        _pL3Db[unit]->route[routeIdx].shadow.path_id = pRoute->path_id;
        _pL3Db[unit]->route[routeIdx].shadow.prefix_len = pRoute->prefix_len;
        _pL3Db[unit]->route[routeIdx].shadow.suffix_len = pRoute->suffix_len;
        if ((!(pRoute->flags & RTK_L3_FLAG_NULL_INTF)) && (nhIdx > DAL_LONGAN_INVALID_NEXTHOP_IDX))
        {
            _pL3Db[unit]->nexthop[nhIdx].ref_count++;
        }
    }
    else  /* entry exsit */
    {
        if (!(pRoute->flags & RTK_L3_FLAG_REPLACE))
        {
            ret = RT_ERR_ENTRY_EXIST;
            goto errEntryExist;
        }

        /* get old nh_idx */
        RT_ERR_HDL(_dal_longan_l3_route_get(unit, tmpRouteIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit,tmpRouteIdx)), errHandle, ret);
        if (1 != routeEntry.dst_null_intf)
        {
            _pL3Db[unit]->nexthop[routeEntry.nh_ecmp_idx].ref_count--;
        }

        if (!(pRoute->flags & RTK_L3_FLAG_NULL_INTF))
        {
            _pL3Db[unit]->nexthop[pRoute->path_id].ref_count++;
            _pL3Db[unit]->route[tmpRouteIdx].shadow.path_id = pRoute->path_id;
         }
        routeIdx = (uint32)tmpRouteIdx;
    }

    /* routeEntry preparation */
    RT_ERR_HDL(l3_util_rtkRoute2routeEntry(&routeEntry, pRoute), errHandle, ret);

    /* write data into chip */
    RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);

    _pL3Db[unit]->route[routeIdx].l3_entry = *pRoute;

errEntryNotFound:
errEntryExist:
errHandle:
    L3_SEM_UNLOCK(unit);
errHandleUnlock:

    return ret;
}   /* end of dal_longan_l3_route_add */


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
int32
dal_longan_l3_route_del(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    int32   routeIdx;
    dal_longan_l3_routeEntry_t routeEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pRoute->vrf_id), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

     /* find the entry */
    if (RT_ERR_OK == _dal_longan_l3_routeEntry_find(unit, pRoute, &routeIdx, &routeEntry))
    {
        /* delete the found entry */
        RT_ERR_HDL(_dal_longan_l3_routeEntry_free(unit, (uint32)routeIdx), errHandle, ret);
        goto errOk;
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_route_del */


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
int32
dal_longan_l3_route_get(uint32 unit, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    int32  routeIdx;
    dal_longan_l3_routeEntry_t routeEntry;
    rtk_l3_route_t  rtkRoute;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,pRoute=%p", unit, pRoute);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pRoute->vrf_id), RT_ERR_INPUT);

    /* function body */
    L3_SEM_LOCK(unit);

    if (RT_ERR_OK == _dal_longan_l3_routeEntry_find(unit, pRoute, &routeIdx, &routeEntry))
    {
        RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);
        if (pRoute->flags & RTK_L3_FLAG_HIT_CLEAR)
        {
            if (pRoute->flags & RTK_L3_FLAG_READ_HIT_IGNORE)
            {
                ret = RT_ERR_INPUT;
                goto errHandle;

            }

            if (routeEntry.hit)
            {
                routeEntry.hit = 0;  /* clear hit bit */
                RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);
            }
        }

        /*return data*/
        *pRoute = rtkRoute;
        /* get prefix & suffix */
        pRoute->prefix_len = _pL3Db[unit]->route[routeIdx].shadow.prefix_len;
        pRoute->suffix_len = _pL3Db[unit]->route[routeIdx].shadow.suffix_len;
        /* display original route ipaddress */
        /* pRoute->ip_addr = _pL3Db[unit]->route[routeIdx].shadow.routeEntry.ip_addr; */

        goto errOk;
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_route_get */


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
int32
dal_longan_l3_route_del_byIntfId(uint32 unit, rtk_l3_flag_t flags, rtk_intf_id_t intfId)
{
    int32   ret = RT_ERR_OK;
    uint32  negate;
    uint32  routeIdx, baseIdx, length;
    dal_longan_l3_routeEntry_t   routeEntry;
    uint32  nhIdx, intfIdx;
    int32   step;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d", unit, intfId);

    /* parameter check */
    RT_PARAM_CHK((intfId >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 (scan from top to bottom) */
        if (0 == L3_ROUTE_TBL_IP6_CNT(unit))
            goto errOk;

        baseIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
        length = L3_ROUTE_IPV6_IDX_MAX(unit) - baseIdx;
        step = 3;
    }
    else
    {
        /* IPv4 (scan from bottom to top) */
        if (0 == L3_ROUTE_TBL_IP_CNT(unit))
            goto errOk;

        baseIdx = L3_ROUTE_TBL_IP_CNT(unit) - 1;
        length = L3_ROUTE_TBL_IP_CNT(unit);
        step = -1;
    }

    negate = (flags & RTK_L3_FLAG_NEGATE)? TRUE : FALSE;

    /* scan the whole L3 route table */
    for (routeIdx = baseIdx; L3_ABS(routeIdx, baseIdx) < length; routeIdx += step)
    {
        if (3 == step)
        {
            routeIdx = L3_ROUTE_IPV6_IDX_NEXT(routeIdx);
            if (routeIdx > L3_ROUTE_IPV6_IDX_MAX(unit))
                break;
        }

        RT_ERR_HDL(_dal_longan_l3_route_get(unit, (uint32)routeIdx, RTK_L3_FLAG_READ_HIT_IGNORE, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, routeIdx)), errHandle, ret);

        /* currently, only remove 'nexthop' type entry */
        nhIdx = routeEntry.nh_ecmp_idx;
        intfIdx = _pL3Db[unit]->nexthop[nhIdx].intf_idx;

        /* confirm all info */
        if ((_pL3Db[unit]->nexthop[nhIdx].valid) && \
            (_pL3Db[unit]->intf[intfIdx].valid) && \
            (((FALSE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id == intfId)) || \
             ((TRUE == negate) && (_pL3Db[unit]->intf[intfIdx].intf_id != intfId))))
        {
            /* release L3 route entry */
            RT_ERR_HDL(_dal_longan_l3_routeEntry_free(unit, (uint32)routeIdx), errHandle, ret);
        }
    }

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_route_del_byIntfId */


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
int32
dal_longan_l3_route_delAll(uint32 unit, rtk_l3_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    uint32  routeIdx, cnt;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x", unit, flags);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 */
        /* clear H/W entries */
        routeIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
        cnt = L3_ROUTE_TBL_IP6_CNT(unit);

        if (cnt > 0)
        {
            RT_ERR_HDL(_dal_longan_l3_routeEntry_clear(unit, TRUE, routeIdx, cnt), errHandle, ret);

            /* clear DB */
            for (routeIdx = 0; routeIdx < LONGAN_L3_HOST_IP6_PFLEN; routeIdx++)
            {
                L3_ROUTE_TBL_IP6_CNT_PFLEN(unit, routeIdx) = 0;
            }
            L3_ROUTE_TBL_IP6_CNT(unit) = 0;
        }
    }
    else
    {
        /* IPv4 */

        /* clear H/W entries */
        routeIdx = 0;
        cnt = L3_ROUTE_TBL_IP_CNT(unit);

        if (cnt > 0)
        {
            RT_ERR_HDL(_dal_longan_l3_routeEntry_clear(unit, FALSE, routeIdx, cnt), errHandle, ret);

            /* clear DB */
            for (routeIdx = 0; routeIdx < LONGAN_L3_HOST_IP_PFLEN; routeIdx++)
            {
                L3_ROUTE_TBL_IP_CNT_PFLEN(unit, routeIdx) = 0;
            }
            L3_ROUTE_TBL_IP_CNT(unit) = 0;
        }
    }

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_route_delAll */


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
int32
dal_longan_l3_route_age(uint32 unit, rtk_l3_flag_t flags, rtk_l3_routeTraverseCallback_f fAge, void *pCookie)
{
    int32   ret = RT_ERR_OK;
    int32   baseIdx, step;
    uint32 routeIdx,length;
    dal_longan_l3_routeEntry_t   routeEntry;
    rtk_l3_route_t rtkRoute;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x,fAge=%p,pCookie=%p", unit, flags, fAge, pCookie);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    if (flags & RTK_L3_FLAG_IPV6)
    {
        /* IPv6 (scan from top to bottom) */
        if (0 == L3_ROUTE_TBL_IP6_CNT(unit))
            goto errOk;
        baseIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
        length = L3_ROUTE_IPV6_CNT2LEN(baseIdx, L3_ROUTE_TBL_IP6_CNT(unit));
        step = 3;
    }
    else
    {
        /* IPv4 (scan from bottom to top) */
        if (0 == L3_ROUTE_TBL_IP_CNT(unit))
            goto errOk;
        baseIdx = L3_ROUTE_TBL_IP_CNT(unit) - 1;
        length = L3_ROUTE_TBL_IP_CNT(unit);
        step = -1;
    }

    for (routeIdx = baseIdx; L3_ABS(routeIdx, baseIdx) < length; routeIdx += step)
    {
        if (3 == step)
        {
            routeIdx = L3_ROUTE_IPV6_IDX_NEXT(routeIdx);
            if (routeIdx > L3_ROUTE_IPV6_IDX_MAX(unit))
                break;
        }

        RT_ERR_HDL(_dal_longan_l3_route_get(unit, routeIdx, RTK_L3_FLAG_NONE, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, routeIdx)), errHandle, ret);
        if (flags & RTK_L3_FLAG_HIT)
        {
            /* age out entry if hit-bit is set */
            if (routeEntry.hit)
            {
                /* callback function */
                if (NULL != fAge)
                {
                    RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);
                    rtkRoute.prefix_len = _pL3Db[unit]->route[routeIdx].shadow.prefix_len;
                    rtkRoute.suffix_len = _pL3Db[unit]->route[routeIdx].shadow.suffix_len;

                    fAge(unit, routeIdx, &rtkRoute, pCookie);
                }

                /* clear hit bit */
                routeEntry.hit = 0;
                RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, routeIdx, &routeEntry, ENABLED), errHandle, ret);
            }
        }
        else
        {
            /* delete the entry since its hit bit is clear */
            if (!routeEntry.hit)
            {
                /* write into chip */
                /* release L3 route entry */
                RT_ERR_HDL(_dal_longan_l3_routeEntry_free(unit, routeIdx), errHandle, ret);

                /* callback function */
                if (NULL != fAge)
                {
                    RT_ERR_HDL(l3_util_routeEntry2rtkRoute(&rtkRoute, &routeEntry), errHandle, ret);
                    rtkRoute.prefix_len = _pL3Db[unit]->route[routeIdx].shadow.prefix_len;
                    rtkRoute.suffix_len = _pL3Db[unit]->route[routeIdx].shadow.suffix_len;

                    fAge(unit, routeIdx, &rtkRoute, pCookie);
                }
            }
        }
    }

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_route_age */


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
             RTK_L3_FLAG_READ_HIT_IGNORE - to indicate read entry ignore the hit flag
 *      (2) Use base index -1 to indicate to search from the beginging of L3 route table
 *      (3) If the returned index is -1, means not found the next valid entry
 */
int32
dal_longan_l3_route_getNext(uint32 unit, rtk_l3_flag_t flags, int32 *pBase, rtk_l3_route_t *pRoute)
{
    int32   ret = RT_ERR_OK;
    uint32  routeIdx, length, width;
    int32   baseIdx;
    dal_longan_l3_routeEntry_t   routeEntry;

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
        baseIdx = L3_ROUTE_IPV6_LAST_IDX(unit);
        length = IS_L3_ROUTE_IPV6_EMPTY(unit) ? 0 : L3_ROUTE_IPV6_CNT2LEN(baseIdx, L3_ROUTE_TBL_IP6_CNT(unit));
        width = 3;
    }
    else
    {
        /* IPv4 */
        baseIdx = 0;
        length = L3_ROUTE_TBL_IP_CNT(unit);
        width = 1;
    }

    /* scan the whole L3 route table */
    for (routeIdx=(*pBase<baseIdx)?(baseIdx):(((*pBase)+width) - ((((*pBase)+width)%8)%width)); (routeIdx - baseIdx) < length; )
    {
        if (3 == width)
        {
            routeIdx = L3_ROUTE_IPV6_IDX_NEXT(routeIdx);

            if (routeIdx > L3_ROUTE_IPV6_IDX_MAX(unit))
                break;
        }

        RT_ERR_HDL(_dal_longan_l3_route_get(unit, routeIdx, flags, &routeEntry, L3_ENTRY_ROUTE_ADDR(unit, routeIdx)), errHandle, ret);
        RT_ERR_HDL(l3_util_routeEntry2rtkRoute(pRoute, &routeEntry), errHandle, ret);

        /* get prefix & suffix */
        pRoute->prefix_len = _pL3Db[unit]->route[routeIdx].shadow.prefix_len;
        pRoute->suffix_len = _pL3Db[unit]->route[routeIdx].shadow.suffix_len;

        /* display original route ipaddress */
        /* pRoute->ip_addr = _pL3Db[unit]->route[routeIdx].shadow.routeEntry.ip_addr; */
        *pBase = routeIdx;

        goto errOk;
    }

    *pBase = -1;    /* Not found any specified entry */

errHandle:
errOk:
    L3_SEM_UNLOCK(unit);

    return ret;

}   /* end of dal_longan_l3_route_getNext */

#if LONGAN_L3_ROUTE_RSVMC_ENTRY
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
int32
dal_longan_l3_route_rsv4Mcast_add(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    dal_longan_l3_routeEntry_t   routeEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
#if LONGAN_L3_ROUTE_HW_LU
    if (RT_ERR_OK != _dal_longan_l3_routeEntry_get(unit, (DAL_LONGAN_L3_ROUTE_TBL_RSV_V4_MIN_IDX), &routeEntry, DISABLED))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errHandle;
    }
#endif

    /* routeEntry preparation */
    /* 224.0.0.0 to 239.255.255.255 */
    osal_memset(&routeEntry, 0x00, sizeof(dal_longan_l3_routeEntry_t));
    routeEntry.valid = 1;
    routeEntry.entry_type = 1;
    routeEntry.bmsk_entry_type = 0x3;
    routeEntry.ipmc_type = 1;
    routeEntry.bmsk_ipmc_type = 0x1;

    routeEntry.l2_en = 0;
    routeEntry.l3_en = 1;
    routeEntry.l3_act = 0;
    routeEntry.rpf_chk_en = 1;
    routeEntry.rpf_fail_act = 3;
    routeEntry.ttl_min = 0;
    routeEntry.mtu_max_idx = 0;
    routeEntry.gip = 0xE0000000;
    routeEntry.bmsk_gip = 0xF0000000;
    routeEntry.round = 1;
    routeEntry.bmsk_round = 0x1;
    routeEntry.vid = LONGAN_L3_RSV_RPF_ID;

    /* write data into chip */
    RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, DAL_LONGAN_L3_ROUTE_TBL_RSV_V4_MIN_IDX, &routeEntry, DISABLED), errHandle, ret);
    LONGAN_L3_DBG_PRINTF(3, "routeIdx = %u\n", DAL_LONGAN_L3_ROUTE_TBL_RSV_V4_MIN_IDX);

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_route_add */

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
int32
dal_longan_l3_route_rsv6Mcast_add(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    dal_longan_l3_routeEntry_t   routeEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */

    /* function body */
    L3_SEM_LOCK(unit);

    /* check validation */
#if LONGAN_L3_ROUTE_HW_LU
    if (RT_ERR_OK != _dal_longan_l3_routeEntry_get(unit, (DAL_LONGAN_L3_ROUTE_TBL_RSV_V6_MIN_IDX), &routeEntry, DISABLED))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errHandle;
    }
#endif

    /* routeEntry preparation */
    /* ff00::/8 */
    osal_memset(&routeEntry, 0x00, sizeof(dal_longan_l3_routeEntry_t));
    routeEntry.valid = 1;
    routeEntry.entry_type = 3;
    routeEntry.bmsk_entry_type = 0x3;

    routeEntry.ipmc_type = 1;
    routeEntry.bmsk_ipmc_type = 0x1;

    routeEntry.l2_en = 0;
    routeEntry.l3_en = 1;
    routeEntry.l3_act = 0;
    routeEntry.rpf_chk_en = 1;
    routeEntry.rpf_fail_act = 3;
    routeEntry.ttl_min = 0;
    routeEntry.mtu_max_idx = 0;

    routeEntry.gip6.octet[0] = 0xFF;
    routeEntry.bmsk_gip6.octet[0] = 0xFF;

    routeEntry.round = 1;
    routeEntry.bmsk_round = 0x1;
    routeEntry.vid= LONGAN_L3_RSV_RPF_ID;

    /* write data into chip */
    RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, DAL_LONGAN_L3_ROUTE_TBL_RSV_V6_MIN_IDX, &routeEntry, DISABLED), errHandle, ret);
    LONGAN_L3_DBG_PRINTF(3, "routeIdx = %u\n", DAL_LONGAN_L3_ROUTE_TBL_RSV_V6_MIN_IDX);

errHandle:
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_route_add */
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
int32
_dal_longan_l3_route_ipmcSize_get(uint32 unit, uint32 *pBase, uint32 *pSize)
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
int32
dal_longan_l3_globalCtrl_get(uint32 unit, rtk_l3_globalCtrlType_t type, int32 *pArg)
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
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_IP_HDR_ERR_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlIpHdrErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_GLB_EN:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_DIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_BAD_DIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlBadDip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_ZERO_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_BC_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_DMAC_BC_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlDmacBc, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_MC_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_DMAC_MC_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlDmacMc, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_HDR_OPT_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_HDR_OPT_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlHdrOpt, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_MTU_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_TTL_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_TTL_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpucRouteCtrlTtlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IPUC_PKT_TO_CPU_TARGET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_optIpucRouteCtrlPktToCpuTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6_HDR_ERR_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_IP6_HDR_ERR_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlIp6HdrErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_GLB_EN:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_DIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_BAD_DIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlBadDip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_ZERO_SIP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlDmacMismatch, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HBH_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHbh, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ERR_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HBH_ERR_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHbhErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HDR_ROUTE_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHdrRoute, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_MTU_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_HL_FAIL_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HL_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIp6ucRouteCtrlHlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_IP6UC_PKT_TO_CPU_TARGET:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_optIp6ucRouteCtrlPktToCpuTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_NH_AGE_OUT_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_NH_AGE_OUT_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actIpRouteCtrlNhAgeOut, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_GCT_URPF_BASE_SEL:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_URPF_BASE_SELf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_BASE_PORT : RTK_L3_URPF_BASE_INTF;
        }
        break;

    case RTK_L3_GCT_NON_IP_ACT:
        {
            L3_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_NONIP_ACTf, val, "", errExit, ret);
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
}   /* end of dal_longan_l3_globalCtrl_get */


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
int32
dal_longan_l3_globalCtrl_set(uint32 unit, rtk_l3_globalCtrlType_t type, int32 arg)
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
    case RTK_L3_GCT_IP_HDR_ERR_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlIpHdrErr, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_IP_HDR_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlBadSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_BAD_DIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlBadDip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_BAD_DIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_ZERO_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlZeroSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_BC_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlDmacBc, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_DMAC_BC_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_DMAC_MC_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlDmacMc, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_DMAC_MC_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_HDR_OPT_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlHdrOpt, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_HDR_OPT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_MTU_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlMtuFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_TTL_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpucRouteCtrlTtlFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_TTL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IPUC_PKT_TO_CPU_TARGET:
        {
            L3_ACTION_TO_VALUE(_optIpucRouteCtrlPktToCpuTarget, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPUC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6_HDR_ERR_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlIp6HdrErr, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_IP6_HDR_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlBadSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_BAD_DIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlBadDip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_BAD_DIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_ZERO_SIP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlZeroSip, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_DMAC_MISMATCH_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlDmacMismatch, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHbh, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HBH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HBH_ERR_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHbhErr, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HBH_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HDR_ROUTE_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHdrRoute, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HDR_ROUTE_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_MTU_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlMtuFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_HL_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actIp6ucRouteCtrlHlFail, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_HL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_IP6UC_PKT_TO_CPU_TARGET:
        {
            L3_ACTION_TO_VALUE(_optIp6ucRouteCtrlPktToCpuTarget, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6UC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_NH_AGE_OUT_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlNhAgeOut, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_NH_AGE_OUT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_URPF_BASE_SEL:
        {
            val = (((rtk_l3_urpf_base_t)arg) != RTK_L3_URPF_BASE_INTF)? 1 : 0;
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_URPF_BASE_SELf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_GCT_NON_IP_ACT:
        {
            L3_ACTION_TO_VALUE(_actIpRouteCtrlNonIp, val, arg, "", errExit, ret);
            L3_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP_ROUTE_CTRLr, LONGAN_NONIP_ACTf, val, "", errExit, ret);
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
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_globalCtrl_set */

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
int32
dal_longan_l3_intfCtrl_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    uint32  val;
    l3_egr_intf_entry_t egrIntf;
    rtk_l3_act_t act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d,type=%d,pArg=%p", unit, intfId, type, pArg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(intfId >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((type >= RTK_L3_ICT_END), RT_ERR_OUT_OF_RANGE);

    intfIdx = DAL_LONGAN_L3_INTF_ID_ENTRY_IDX(intfId);
    /* function body */
    L3_SEM_LOCK(unit);
    L3_INT_SEM_LOCK(unit);  /* to avoid race-condition of L3 interface entry which may be accessed by internal APIs */

    if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, intfIdx))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    switch (type)
    {
    case RTK_L3_ICT_IPUC_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIpIcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IPUC_PBR_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIpPbrIcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6UC_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIp6IcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_ICT_IP6UC_PBR_ICMP_REDIR_ACT:
        {
            L3_TABLE_READ_FIELD_GET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actEgrIntfIp6PbrIcmpRedirect, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errEntryNotFound:
errInput:
errExit:
    L3_INT_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}

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
int32
dal_longan_l3_intfCtrl_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_intfCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  intfIdx;
    uint32  val;
    l3_egr_intf_entry_t egrIntf;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,intfId=%d,type=%d,arg=0x%08x", unit, intfId, type, arg);

    /* check Init status */
    RT_INIT_CHK(l3_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(intfId >= HAL_MAX_NUM_OF_INTF(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((type >= RTK_L3_ICT_END), RT_ERR_OUT_OF_RANGE);

    intfIdx = DAL_LONGAN_L3_INTF_ID_ENTRY_IDX(intfId);
    /* function body */
    L3_SEM_LOCK(unit);
    L3_INT_SEM_LOCK(unit);  /* to avoid race-condition of L3 interface entry which may be accessed by internal APIs */

    if (BITMAP_IS_CLEAR(_pL3Db[unit]->HW.intf_used, intfIdx))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errEntryNotFound;
    }

    switch (type)
    {
    case RTK_L3_ICT_IPUC_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIpIcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IPUC_PBR_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIpPbrIcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIp6IcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP6_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_ICT_IP6UC_PBR_ICMP_REDIR_ACT:
        {
            L3_ACTION_TO_VALUE(_actEgrIntfIp6PbrIcmpRedirect, val, arg, "", errExit, ret);
            L3_TABLE_WRITE_FIELD_SET_ERR_HDL(unit, \
                LONGAN_L3_EGR_INTFt, intfIdx, egrIntf, \
                LONGAN_L3_EGR_INTF_IP6_PBR_ICMP_REDIRECT_ACTtf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errEntryNotFound:
errInput:
errExit:
    L3_INT_SEM_UNLOCK(unit);
    L3_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_intfCtrl_set */

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
int32
dal_longan_l3_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 *pArg)
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
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_DFLT_ROUTE_EN:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_MODE:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_MODEf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_MODE_STRICT : RTK_L3_URPF_MODE_LOOSE;
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_FAIL_ACT:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_FAIL_ACTf, val, "", errExit, ret);
            L3_VALUE_TO_ACTION(_actPortIpRouteCtrlUrpfFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_CHK_EN:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_DFLT_ROUTE_EN:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_MODE:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_MODEf, val, "", errExit, ret);
            *pArg = (val)? RTK_L3_URPF_MODE_STRICT : RTK_L3_URPF_MODE_LOOSE;
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_FAIL_ACT:
        {
            L3_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_FAIL_ACTf, val, "", errExit, ret);
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
}   /* end of dal_longan_l3_portCtrl_get */


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
int32
dal_longan_l3_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l3_portCtrlType_t type, int32 arg)
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
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_DFLT_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_MODE:
        {
            val = (((rtk_l3_urpf_mode_t)arg) != RTK_L3_URPF_MODE_LOOSE)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_MODEf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IPUC_URPF_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actPortIpRouteCtrlUrpfFail, val, arg, "", errExit, ret);
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_CHK_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_DFLT_ROUTE_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_DFLT_ROUTE_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_MODE:
        {
            val = (((rtk_l3_urpf_mode_t)arg) != RTK_L3_URPF_MODE_LOOSE)? 1 : 0;
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_CHK_MODEf, val, "", errExit, ret);
        }
        break;

    case RTK_L3_PCT_IP6UC_URPF_FAIL_ACT:
        {
            L3_ACTION_TO_VALUE(_actPortIp6RouteCtrlUrpfFail, val, arg, "", errExit, ret);
            L3_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L3_PORT_IP6_ROUTE_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_URPF_FAIL_ACTf, val, "", errExit, ret);
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
}   /* end of dal_longan_l3_portCtrl_set */



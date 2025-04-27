/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public IPMCAST APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) IPMCAST routing
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_list.h>
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
#include <rtk/ipmcast.h>

#if defined(CONFIG_SDK_DRIVER_NIC)
#include <drv/nic/nic.h>
#endif

/*
 * Symbol Definition
 */
#define LONGAN_IPMCAST_DBG                   (0)


#define IPMCAST_HASH_KEY        DAL_LONGAN_IPMC_L3TBL_HASH_KEY_SIP_GIP
#define IPMCAST_TABLE0_ALG      DAL_LONGAN_IPMC_ALG0
#define IPMCAST_TABLE1_ALG      DAL_LONGAN_IPMC_ALG0


#define LONGAN_IPMC_HASH_IDX_ALG_NUM         (2)
#define LONGAN_IPMC_STAT_MONT_ONETIME_MAX    (2)
#define LONGAN_IPMC_STAT_MONT_MODE_PKT_CNT   (1)
#define LONGAN_IPMC_STAT_MONT_MODE_BYTE_CNT  (0)

/* for SS-432 */
#define LONGAN_IPMC_RSVD_SIP             (0xc8ffffff)   /* (200.255.255.255) */
#define LONGAN_IPMC_RSVD_DIP             (0xeefefefe)   /* (238.254.254.254) */
#define LONGAN_IPMC_RSVD_VID             (4095)
#define LONGAN_IPMC_RSVD_GROUP             (0x20003FF)
#define LONGAN_IPMC_RSVD_ENTRY_TTL          (64)


typedef enum dal_longan_ipmc_l3TblLookupMode_e
{
    DAL_LONGAN_IPMC_L3TBL_LOOKUP_MODE_FORCE = 0,
    DAL_LONGAN_IPMC_L3TBL_LOOKUP_MODE_PACKET,
} dal_longan_ipmc_l3TblLookupMode_t;

typedef enum dal_longan_ipmc_l3TblHashKey_e
{
    DAL_LONGAN_IPMC_L3TBL_HASH_KEY_SIP_GIP = 0,      /* SIP, GIP */
    DAL_LONGAN_IPMC_L3TBL_HASH_KEY_SIP_GIP_VID,     /* SIP, GIP, VID */
} dal_longan_ipmc_l3TblHashKey_t;

typedef enum dal_longan_ipmc_alg_e
{
    DAL_LONGAN_IPMC_ALG0 = 0,
    DAL_LONGAN_IPMC_ALG1,
} dal_longan_ipmc_alg_t;

typedef enum dal_longan_ipmc_enum_alloc_e
{
    DAL_LONGAN_IPMC_MTU_NOT_ALLOC = 0,
    DAL_LONGAN_IPMC_MTU_ALLOC,
    DAL_LONGAN_IPMC_MTU_REALLOC,
} dal_longan_ipmc_enum_alloc_t;

/* structure of IPMCAST L3 entry */
typedef struct dal_longan_ipmc_l3Entry_s
{
    uint32  signKey;                /* hash value of signature key */

    uint32  valid:1;                /* to indicate the node is used/free */
    uint32  ipv6:1;                 /* FALSE/TRUE - IPv4/IPv6 entry */
    uint32  cam:1;                  /* FALSE/TRUE - L3 Host/Route table */

    uint32  l2_pmsk_valid:1;        /* FALSE/TRUE - l2_pmsk_idx is invalid/valid */
    uint32  l3_oil_valid:1;         /* FALSE/TRUE - l3_oil_idx is invalid/valid */

    uint32  statMont_byte_valid:1;  /* statMont byte-count valid */
    uint32  statMont_byte_idx:2;    /* statMont byte-count entry index */
    uint32  statMont_pkt_valid:1;   /* statMont packet-count valid */
    uint32  statMont_pkt_idx:2;     /* statMont packet-count entry index */

    uint32  hw_entry_idx;           /* hardware entry index */

    rtk_mcast_group_t   groupId;    /* bound multicast group ID */
    int32   l2_pmsk_idx;            /* from group: l2 portmask index (-1 also means invalid) */
    int32   l3_oil_idx;             /* from group: l3 oil index (-1 also means invalid) */

    rtk_ipmc_addr_t     ipmc_entry;     /*shadow for ipmcast entry*/

    RTK_LIST_NODE_REF_DEF(dal_longan_ipmc_l3Entry_s, nodeRef);
} dal_longan_ipmc_l3Entry_t;

typedef struct dal_longan_ipmc_drvDb_s
{
    /* L3 entry */
    RTK_LIST_DEF(dal_longan_ipmc_l3Entry_s, used_l3Entry_list);     /* list of the used L3 entries */
    RTK_LIST_DEF(dal_longan_ipmc_l3Entry_s, free_l3Entry_list);     /* list of the free L3 entries */
    dal_longan_ipmc_l3Entry_t l3Entry[DAL_LONGAN_IPMCAST_L3_ENTRY_MAX]; /* instances of L3 entries */

    struct
    {
        uint32  valid:1;        /* FALSE - free entry, TRUE - used entry */
        uint32  mode:1;         /* 0 - packet-count, 1 - byte-count */
        uint32  l3EntryIdx;     /* index of monitored l3 entry */
    } mont[DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM];

    dal_longan_ipmc_l3TblLookupMode_t    l3TblLookupMode;    /* L3 table lookup mode */
    dal_longan_ipmc_l3TblHashKey_t       l3TblHashKey;       /* L3 table hash key */


#if LONGAN_L3_ROUTE_IPMC_SIZE
    uint32  l3_route_ipmc_idx_base;
    uint32  l3_route_ipmc_size;

    uint16  l3_route_ipmc_ipv4_cnt;
    uint16  l3_route_ipmc_ipv6_cnt;

    uint32  l3_route_ipmc_ipv4_bottom_offset;
    uint32  l3_route_ipmc_ipv6_top_offset;
    dal_longan_ipmc_l3Entry_t            *pIpmcL3Entry[LONGAN_L3_ROUTE_IPMC_SIZE];
#endif
} dal_longan_ipmc_drvDb_t;

typedef struct dal_longan_ipmc_hashIdx_s
{
    uint32 hashIdx_of_alg[LONGAN_IPMC_HASH_IDX_ALG_NUM];
} dal_longan_ipmc_hashIdx_t;

typedef struct ipmc_addrIdx_s
{
    struct
    {
        uint32 valid;
        uint32 sw_idx;
    }entryIdx[DAL_LONGAN_L3_TBL_SIZE];
}ipmc_addrIdx_t;

typedef struct dal_lognan_ipmc_mtu_info_s
{
    uint32 val;
    uint32 idx;
}dal_longan_ipmc_mtu_info_t;

/*
 * Data Declaration
 */

static uint32               ipmc_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         ipmc_sem[RTK_MAX_NUM_OF_UNIT];

static dal_longan_ipmc_drvDb_t           *_pIpmcDb[RTK_MAX_NUM_OF_UNIT];

/*use for ipmc addr dump*/
static ipmc_addrIdx_t      *pIpmcAddrIdxMap[RTK_MAX_NUM_OF_UNIT];
#if defined(CONFIG_SDK_DRIVER_NIC)
/*
note : DMAC/DIP/SIP should sync with LONGAN_IPMC_RSVD_DIP/LONGAN_IPMC_RSVD_SIP
*/
static uint8 ipmcPkt[] = {
    0x01,0x00,0x5E,0x7E,0xFE,0xFE,0x00,0x00,0x07,0x08,0x09,0x0A,0x81,0x00,0x00,0x01,
    0x08,0x00,0x45,0x00,0x00,0x2E,0x00,0x00,0x00,0x00,0x40,0xFF,0xC2,0xD4,0xC8,0xFF,
    0xFF,0xFF,0xEE,0xFE,0xFE,0xFE,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
    0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
    };
#endif
static uint32 ipmc_rsv_index = 0;

static uint32 _actIpmcRouteCtrlBadSip[] = {   /* LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_DROP,             /*mac-base bridge*/
    };

static uint32 _actIpmcRouteCtrlZeroSip[] = {  /* LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_DROP,             /*mac-base bridge*/
    };

static uint32 _actIpmcRouteCtrlDmacMismatch[] = { /* LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_DROP,             /*mac-base bridge*/
    };

static uint32 _actIpmcRouteCtrlHdrOpt[] = {   /* LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_HDR_OPT_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_FORWARD,          /*route + bridge*/
    RTK_IPMC_ACT_COPY2CPU,         /*copy + route + bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_COPY2MASTERCPU,   /*copy + route + bridge*/
    RTK_IPMC_ACT_DROP,             /*bridge only*/
    };

static uint32 _actIpmcRouteCtrlMtuFail[] = {  /* LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpmcRouteCtrlTtlFail[] = {  /* LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_TTL_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _optIpmcRouteCtrlPktToCPUTarget[] = { /* LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf */
    RTK_IPMC_CPUTARGET_LOCAL,
    RTK_IPMC_CPUTARGET_MASTER,
    };

static uint32 _actIp6mcRouteCtrlBadSip[] = {  /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_DROP,             /*mac-base bridge*/
    };

static uint32 _actIp6mcRouteCtrlZeroSip[] = { /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_DROP,             /*mac-base bridge*/
    };

static uint32 _actIp6mcRouteCtrlDmacMismatch[] = {    /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_DROP,             /*mac-base bridge*/
    };

static uint32 _actIp6mcRouteCtrlHbh[] = { /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HBH_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + bridge bridge*/
    RTK_IPMC_ACT_FORWARD,          /*route + bridge*/
    RTK_IPMC_ACT_COPY2CPU,         /*copy + route + bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + bridge bridge*/
    RTK_IPMC_ACT_COPY2MASTERCPU,   /*copy + route + bridge*/
    RTK_IPMC_ACT_DROP,             /*bridge only*/
    };

static uint32 _actIp6mcRouteCtrlHbhErr[] = {  /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HBH_ERR_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + bridge*/
    RTK_IPMC_ACT_FORWARD,          /*route + bridge*/
    RTK_IPMC_ACT_COPY2CPU,         /*copy + route + bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + bridge*/
    RTK_IPMC_ACT_COPY2MASTERCPU,   /*copy + route + bridge*/
    RTK_IPMC_ACT_DROP,             /*bridge only*/
    };

static uint32 _actIp6mcRouteCtrlHdrRoute[] = {    /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HDR_ROUTE_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,         /*trap + mac-base bridge*/
    RTK_IPMC_ACT_FORWARD,          /*route + bridge*/
    RTK_IPMC_ACT_COPY2CPU,         /*copy + route + bridge*/
    RTK_IPMC_ACT_TRAP2MASTERCPU,   /*trap + mac-base bridge*/
    RTK_IPMC_ACT_COPY2MASTERCPU,   /*copy + route + bridge*/
    RTK_IPMC_ACT_DROP,             /*bridge only*/
    };

static uint32 _actIp6mcRouteCtrlMtuFail[] = { /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6mcRouteCtrlHlFail[] = {  /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HL_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _optIp6mcRouteCtrlPktToCPUTarget[] = { /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf */
    RTK_IPMC_CPUTARGET_LOCAL,
    RTK_IPMC_CPUTARGET_MASTER,
    };

static uint32 _actIpmcRouteLuMis[] = { /*LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_LU_MIS_ACTtf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIp6mcRouteLuMis[] = {    /* LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_LU_MIS_ACTf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _actEntryL2MC[] = {
    RTK_IPMC_ADDR_ACT_FORWARD,
    RTK_IPMC_ADDR_ACT_TRAP2CPU,
    RTK_IPMC_ADDR_ACT_COPY2CPU,
    RTK_IPMC_ADDR_ACT_DROP,
    };

static uint32 _actEntryL3MC[] = {
    RTK_IPMC_ADDR_ACT_FORWARD,
    RTK_IPMC_ADDR_ACT_TRAP2CPU,
    RTK_IPMC_ADDR_ACT_COPY2CPU,
    RTK_IPMC_ADDR_ACT_DROP,
    };

static uint32 _actEntryL3MCRpfFail[] = {
    RTK_IPMC_RPF_FAIL_ACT_TRAP,
    RTK_IPMC_RPF_FAIL_ACT_DROP,
    RTK_IPMC_RPF_FAIL_ACT_COPY,
    RTK_IPMC_RPF_FAIL_ACT_ASSERT_CHK,
    };

/*
 * Macro Definition
 */
#define LONGAN_IPMCAST_DBG_PRINTF(_level, _fmt, ...)                                 \
do {                                                                                \
    if (LONGAN_IPMCAST_DBG >= (_level))                                              \
        osal_printf("L%05d:%s(): " _fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__);    \
} while (0)

#define IPMCAST_SEM_LOCK(unit)                                                               \
do {\
    if (osal_sem_mutex_take(ipmc_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)         \
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore lock failed");           \
        return RT_ERR_SEM_LOCK_FAILED;                                                       \
    }\
} while(0)
#define IPMCAST_SEM_UNLOCK(unit)                                                             \
do {\
    if (osal_sem_mutex_give(ipmc_sem[unit]) != RT_ERR_OK)                                    \
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore unlock failed");       \
        return RT_ERR_SEM_UNLOCK_FAILED;                                                     \
    }\
} while(0)

#define MCAST_SEM_LOCK(_unit)       _dal_longan_mcast_sem_lock(_unit)
#define MCAST_SEM_UNLOCK(_unit)     _dal_longan_mcast_sem_unlock(_unit)

#define IPMCAST_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                    \
    if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)               \
    {                                                                                   \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                   \
        goto _gotoErrLbl;                                                               \
    }                                                                                   \
} while(0)
#define IPMCAST_REG_FIELD_WRITE_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                    \
    if ((_ret = reg_field_write(_unit, _reg, _field, &_val)) != RT_ERR_OK)              \
    {                                                                                   \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                   \
        goto _gotoErrLbl;                                                               \
    }                                                                                   \
} while(0)
#define IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                                        \
    if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)               \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                                       \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                                        \
    if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)              \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                                       \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define IPMCAST_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                \
    if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)     \
    {                                                                               \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                               \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define IPMCAST_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                \
    if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)    \
    {                                                                               \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                               \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define IPMCAST_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                    \
    if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                                   \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)
#define IPMCAST_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                    \
    if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                                   \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)
#define IPMCAST_TABLE_FIELD_MAC_GET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                        \
    if ((_ret = table_field_mac_get(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                                       \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define IPMCAST_TABLE_FIELD_MAC_SET_ERR_HDL(_unit, _tbl, _field, _mac, _entry, _errMsg, _gotoErrLbl, _ret)  \
do {                                                                                                        \
    if ((_ret = table_field_mac_set(_unit, _tbl, _field, (uint8 *)&_mac, (uint32 *)&_entry)) != RT_ERR_OK)  \
    {                                                                                                       \
        RT_ERR(ret, (MOD_IPMCAST|MOD_DAL), _errMsg);                                                        \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define IPMCAST_TABLE_READ_FIELD_GET_ERR_HDL(_unit, _tbl, _idx, _entry, _field, _val, _errMsg, _gotoErrLbl, _ret) \
do {                                                                                                        \
    IPMCAST_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret);                      \
    IPMCAST_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret);         \
} while(0)
#define IPMCAST_TABLE_WRITE_FIELD_SET_ERR_HDL(_unit, _tbl, _idx, _entry, _field, _val, _errMsg, _gotoErrLbl, _ret) \
do {                                                                                                        \
    IPMCAST_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret);                      \
    IPMCAST_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret);         \
    IPMCAST_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret);                     \
} while(0)

#define IPMCAST_RT_ERR_HDL_DBG(_op, _args...)               \
do {                                                        \
    if (RT_ERR_OK != (_op))                                 \
    {                                                       \
       RT_LOG(LOG_DEBUG, (MOD_IPMCAST|MOD_DAL), ## _args);  \
    }                                                       \
} while(0)

#define IPMCAST_ACTION_TO_VALUE(_actArray, _value, _action, _errMsg, _errHandle, _retval)   \
do {                                                                                        \
    if ((_retval = RT_UTIL_ACTLIST_INDEX_GET(_actArray, _value, _action)) != RT_ERR_OK)     \
    {                                                                                       \
        RT_ERR(_retval, (MOD_IPMCAST|MOD_DAL), _errMsg);                                    \
        goto _errHandle;                                                                    \
    }                                                                                       \
} while(0)
#define IPMCAST_VALUE_TO_ACTION(_actArray, _action, _value, _errMsg, _errHandle, _retval)   \
do {                                                                                        \
    if ((_retval = RT_UTIL_ACTLIST_ACTION_GET(_actArray, _action, _value)) != RT_ERR_OK)    \
    {                                                                                       \
        RT_ERR(_retval, (MOD_IPMCAST|MOD_DAL), _errMsg);                                    \
        goto _errHandle;                                                                    \
    }                                                                                       \
} while(0)
#define IS_IPMC_ADDR(__pMcastAddr)     ( ((__pMcastAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6) && (__pMcastAddr->dst_ip_addr.ipv6.octet[0] == 0xFF)) || \
                                         ((!(__pMcastAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)) && ((__pMcastAddr->dst_ip_addr.ipv4 & 0xF0000000) == 0xE0000000)))


#define L3_ENTRY_IDX_IS_VALID(_unit, _entryIdx) (TRUE == _pIpmcDb[(_unit)]->l3Entry[(_entryIdx)].valid)

#define L3_ENTRY_ADDR(_unit, _entryIdx)         (&_pIpmcDb[(_unit)]->l3Entry[(_entryIdx)])
#define L3_ENTRY_ADDR_TO_IDX(_unit, _pEntry)    ((_pEntry) - (&_pIpmcDb[(_unit)]->l3Entry[0]))
#define L3_ENTRY_IPMC_ADDR(_unit, _entryIdx)         (&(_pIpmcDb[(_unit)]->l3Entry[(pIpmcAddrIdxMap[(_unit)]->entryIdx[(_entryIdx)].sw_idx)].ipmc_entry))

#define L3_ENTRY_IS_VALID(_pEntry)              ((NULL != (_pEntry)) && (TRUE == (_pEntry)->valid))

#define L3_HWENTRY_IDX_IS_VALID(_unit, _hwIdx)   (TRUE == pIpmcAddrIdxMap[(_unit)]->entryIdx[(_hwIdx)].valid)
#define L3_HWENTRY_ADDR(_unit, _hwIdx)         (&_pIpmcDb[(_unit)]->l3Entry[(pIpmcAddrIdxMap[(_unit)]->entryIdx[(_hwIdx)].sw_idx)])

#define DAL_LONGAN_IPMC_HOST_IDX_WIDTH  (13)

#define L3_ENTRY_RESET(_pEntry)                                     \
do {                                                                \
    osal_memset(_pEntry, 0x00, sizeof(dal_longan_ipmc_l3Entry_t));  \
} while (0)

#define L3_ENTRY_VALIDATE(_pEntry)  \
do {                                \
    (_pEntry)->valid = TRUE;        \
} while (0)

#define L3_ENTRY_INIT(_pEntry)  \
do {                            \
} while (0)


#define MONT_ENTRY_IDX_IS_VALID(_unit, _entryIdx)   \
    (TRUE == _pIpmcDb[(_unit)]->mont[(_entryIdx)].valid)

#define L3_HW_IDX_VALID(_unit, _hwIdx, _swIdx)   \
do {                                \
    pIpmcAddrIdxMap[(_unit)]->entryIdx[_hwIdx].valid = TRUE;              \
    pIpmcAddrIdxMap[(_unit)]->entryIdx[_hwIdx].sw_idx = _swIdx;       \
} while (0)

#define L3_HW_IDX_INVALID(_unit, _hwIdx)   \
do {                                \
    pIpmcAddrIdxMap[(_unit)]->entryIdx[_hwIdx].valid = FALSE;              \
    pIpmcAddrIdxMap[(_unit)]->entryIdx[_hwIdx].sw_idx = 0;                   \
} while (0)

#define HASH_BIT_EXTRACT(_val, _lsb, _len)   (((_val) & (((1 << (_len)) - 1) << (_lsb))) >> (_lsb))

/*
 * Function Declaration
 */

extern int32 _dal_longan_ipmc_host_get(uint32 unit, uint32 index, rtk_ipmc_flag_t flag, dal_longan_l3_hostEntry_t *pEntry, rtk_ipmc_addr_t *pIpmcAddr);

static inline int32 _dal_longan_ipmc_hostEntryCmp(uint32 unit, dal_longan_l3_hostEntry_t *pEntry1, dal_longan_l3_hostEntry_t *pEntry2)
{
    if ((NULL == pEntry1) || (NULL == pEntry2))
        return RT_ERR_FAILED;

    if ((pEntry1->entry_type != pEntry2->entry_type) ||
        (pEntry1->ipmc_type != pEntry2->ipmc_type) )
        return RT_ERR_FAILED;

    if (3 == pEntry1->entry_type)
    {
        if (RT_ERR_OK != rt_util_ipv6Cmp(&pEntry1->gip6, &pEntry2->gip6))
            return RT_ERR_FAILED;

        if (RT_ERR_OK != rt_util_ipv6Cmp(&pEntry1->sip6, &pEntry2->sip6))
            return RT_ERR_FAILED;
    }
    else if (1 == pEntry1->entry_type)
    {
        if (pEntry1->gip != pEntry2->gip)
            return RT_ERR_FAILED;
    }
    else
    {
        return RT_ERR_FAILED;
    }

    if ( (pEntry1->valid != pEntry2->valid) ||
          (pEntry1->vid != pEntry2->vid) ||
          (pEntry1->qos_en != pEntry2->qos_en) ||
          (pEntry1->l2_en != pEntry2->l2_en) ||
          (pEntry1->l3_en != pEntry2->l3_en))
        return RT_ERR_FAILED;

    if (ENABLED == pEntry1->qos_en)
    {
        if (pEntry1->qos_pri != pEntry2->qos_pri)
            return RT_ERR_FAILED;
    }

    if (pEntry1->vid_cmp != pEntry2->vid_cmp)
        return RT_ERR_FAILED;

    if (ENABLED == pEntry1->l2_en)
    {
        if (pEntry1->l2_act != pEntry2->l2_act)
            return RT_ERR_FAILED;

        if (pEntry1->l2_mc_pmsk_idx != pEntry2->l2_mc_pmsk_idx)
            return RT_ERR_FAILED;
    }

    if (ENABLED == pEntry1->l3_en)
    {
        if ( (pEntry1->l3_act != pEntry2->l3_act) ||
              (pEntry1->ttl_min != pEntry2->ttl_min) ||
              /*(pEntry1->mtu_max_idx != pEntry2->mtu_max_idx) ||*/
              (pEntry1->rpf_chk_en != pEntry2->rpf_chk_en) ||
              (pEntry1->rpf_fail_act != pEntry2->rpf_fail_act) ||
              (pEntry1->oil_idx_valid != pEntry2->oil_idx_valid) ||
              (pEntry1->stack_fwd_pmsk != pEntry2->stack_fwd_pmsk))
            return RT_ERR_FAILED;

        if (ENABLED == pEntry1->oil_idx_valid)
        {
            if (pEntry1->oil_idx != pEntry2->oil_idx)
                return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

static inline int32
_dal_longan_ipmc_emptyL3HostEntry_build(
    dal_longan_l3_hostEntry_t *pEntry,
    rtk_ipmc_flag_t flags)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_longan_l3_hostEntry_t));
    if (flags & RTK_IPMC_FLAG_IPV6)
    {
        pEntry->entry_type = 0x3;
    }
    else
    {
        pEntry->entry_type = 0x1;
    }

    return RT_ERR_OK;
}

static inline int32
_dal_longan_ipmc_emptyL3RouteEntry_build(
    dal_longan_l3_routeEntry_t *pEntry,
    rtk_ipmc_flag_t flags)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_longan_l3_routeEntry_t));
    if (flags & RTK_IPMC_FLAG_IPV6)
    {
        pEntry->entry_type = 0x3;
    }
    else
    {
        pEntry->entry_type = 0x1;
    }

    return RT_ERR_OK;
}

static inline int32 _dal_longan_ipmc_mtu_get(uint32 unit, uint32 hw_index, dal_longan_ipmc_mtu_info_t *pMtu, uint32 iscam)
{
    int32 ret = RT_ERR_OK;
    dal_longan_l3_hostEntry_t   hostEntry;
    dal_longan_l3_routeEntry_t  routeEntry;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMtu), RT_ERR_NULL_POINTER);

    /* flag = RTK_IPMC_FLAG_NONE, get the entry from ASIC */
     if (!iscam)
    {
        RT_ERR_HDL(_dal_longan_ipmc_host_get(unit, hw_index, RTK_IPMC_FLAG_NONE, &hostEntry, \
                                    L3_ENTRY_IPMC_ADDR(unit, hw_index)), errHandle, ret);

        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
                hostEntry.mtu_max_idx, LONGAN_MTU_VALf, pMtu->val, "", errHandle, ret);
        pMtu->idx = hostEntry.mtu_max_idx;
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_routeEntry_get(unit, hw_index, &routeEntry, \
            DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
                routeEntry.mtu_max_idx, LONGAN_MTU_VALf, pMtu->val, "", errHandle, ret);
        pMtu->idx = routeEntry.mtu_max_idx;
    }

errHandle:
    return ret;
}

static inline int32 _dal_longan_ipmc_Ip6mtu_get(uint32 unit, uint32 hw_index, dal_longan_ipmc_mtu_info_t *pMtu, uint32 iscam)
{
    int32 ret = RT_ERR_OK;
    dal_longan_l3_hostEntry_t   hostEntry;
    dal_longan_l3_routeEntry_t  routeEntry;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMtu), RT_ERR_NULL_POINTER);

    /* flag = RTK_IPMC_FLAG_NONE, get the entry from ASIC */
    if (!iscam)
    {
        RT_ERR_HDL(_dal_longan_ipmc_host_get(unit, hw_index, RTK_IPMC_FLAG_NONE, &hostEntry, \
                                    L3_ENTRY_IPMC_ADDR(unit, hw_index)), errHandle, ret);

        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
                hostEntry.mtu_max_idx, LONGAN_MTU_VALf, pMtu->val, "", errHandle, ret);
        pMtu->idx = hostEntry.mtu_max_idx;
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_routeEntry_get(unit, hw_index, &routeEntry, \
            DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
                routeEntry.mtu_max_idx, LONGAN_MTU_VALf, pMtu->val, "", errHandle, ret);
        pMtu->idx = routeEntry.mtu_max_idx;
    }

errHandle:
    return ret;
}

static inline int32
_dal_longan_ipmc_rtkIpmcAddr2l3HostEntry(
    uint32 unit,
    dal_longan_l3_hostEntry_t *pEntry,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_ipmc_enum_alloc_t allocMtu)
{
    int32 ret = RT_ERR_OK;
    int32 l3oilIdx,l2fwdIdx;
    uint32 mtu_index;
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    pEntry->valid = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_DISABLE)? 0 : 1;
    mtu_index = pEntry->mtu_max_idx;

    if (DAL_LONGAN_IPMC_L3TBL_LOOKUP_MODE_FORCE == _pIpmcDb[unit]->l3TblLookupMode)
    {
        pEntry->ipmc_type = 0x1;
    }

    /* SS-627 */
    if (DAL_LONGAN_IPMC_L3TBL_HASH_KEY_SIP_GIP_VID == _pIpmcDb[unit]->l3TblHashKey)
    {
        pEntry->vid_cmp = 1;
    }
    else if (!(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN))
    {
        if (pIpmcAddr->src_intf.vlan_id)
            pEntry->vid_cmp = 1;
    }

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        pEntry->entry_type = 3;
        pEntry->gip6 = pIpmcAddr->dst_ip_addr.ipv6;
        osal_memset(pEntry->sip6.octet, 0xFF, IPV6_ADDR_LEN);

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
            pEntry->sip6 = pIpmcAddr->src_ip_addr.ipv6;
    }
    else
    {
        pEntry->entry_type = 1;
        pEntry->gip = pIpmcAddr->dst_ip_addr.ipv4;
        pEntry->sip = 0xFFFFFFFF;

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
            pEntry->sip = pIpmcAddr->src_ip_addr.ipv4;
    }

    pEntry->vid = pIpmcAddr->src_intf.vlan_id;
    pEntry->qos_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pIpmcAddr->qos_pri;
    pEntry->hit = 1;    /* keep HIT bit value */

    IPMCAST_ACTION_TO_VALUE(_actEntryL2MC, pEntry->l2_act, pIpmcAddr->l2_fwd_act, "", errHandle, ret);
    IPMCAST_ACTION_TO_VALUE(_actEntryL3MC, pEntry->l3_act, pIpmcAddr->l3_fwd_act, "", errHandle, ret);
    IPMCAST_ACTION_TO_VALUE(_actEntryL3MCRpfFail, pEntry->rpf_fail_act, pIpmcAddr->l3_rpf_fail_act, "", errHandle, ret);

    pEntry->ttl_min = pIpmcAddr->ttl_min;
    pEntry->rpf_chk_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN) ? 1 : 0;


    if (allocMtu == DAL_LONGAN_IPMC_MTU_REALLOC)
    {
        pEntry->mtu_max_idx = mtu_index;
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuIp6Entry_realloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
        else
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuEntry_realloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
    }
    else if (allocMtu == DAL_LONGAN_IPMC_MTU_ALLOC)
    {
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuIp6Entry_alloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
        else
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuEntry_alloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
    }

    /*should reserved fwdidx =0; for invalid l2 fwdidx; */
    RT_ERR_HDL(_dal_longan_mcast_fwdIdx_get(unit, pIpmcAddr->group,&l2fwdIdx,&l3oilIdx, &pEntry->stack_fwd_pmsk),errHandle, ret);     //should not check the group entry is not found

    if (l2fwdIdx >= 0)
    {
        pEntry->l2_mc_pmsk_idx = l2fwdIdx;
    }


    /* L2 available */
    if ((pIpmcAddr->l2_fwd_act != RTK_IPMC_ADDR_ACT_FORWARD) || (l2fwdIdx >= 0))
    {
        pEntry->l2_en = 1;
    }

    if ((l3oilIdx < 0))
    {
        pEntry->oil_idx_valid = FALSE;
    }
    else
    {
        pEntry->oil_idx_valid = TRUE;
        pEntry->oil_idx = l3oilIdx;
    }

    pEntry->l3_en = pIpmcAddr->l3_en;

errMtuAlloc:
errHandle:
    return ret;
}

static inline int32
_dal_longan_ipmc_l3HostEntry2rtkIpmcAddr(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_l3_hostEntry_t *pEntry,
    rtk_mcast_group_t groupId)
{
    int32 ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pIpmcAddr, 0x00, sizeof(rtk_ipmc_addr_t));

    pIpmcAddr->vrf_id = 0; //not support;
    pIpmcAddr->src_intf.vlan_id = pEntry->vid;

    if (!pEntry->valid)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_DISABLE;
    }

    if (0x1 == pEntry->entry_type)          /* IPMC */
    {
        if (0xFFFFFFFF != pEntry->sip)
        {
            pIpmcAddr->src_ip_addr.ipv4 = pEntry->sip;
            pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
        }

        pIpmcAddr->dst_ip_addr.ipv4 = pEntry->gip;
        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
            pEntry->mtu_max_idx, LONGAN_MTU_VALf, pIpmcAddr->mtu_max, "", errHandle, ret);
    }
    else if (0x3 == pEntry->entry_type)     /* IP6MC */
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_IPV6;

        if (!rt_util_ipv6IsAllOnes_ret(&pEntry->sip6))
        {
            pIpmcAddr->src_ip_addr.ipv6 = pEntry->sip6;
            pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
        }

        pIpmcAddr->dst_ip_addr.ipv6 = pEntry->gip6;
        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
            pEntry->mtu_max_idx, LONGAN_MTU_VALf, pIpmcAddr->mtu_max, "", errHandle, ret);
    }
    else
    {
        return RT_ERR_INPUT;
    }

    if (pEntry->qos_en)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        pIpmcAddr->qos_pri = pEntry->qos_pri;
    }

    if (pEntry->rpf_chk_en)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_RPF_EN;
    }

    if (pEntry->hit)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_HIT;
    }

    pIpmcAddr->group = groupId;
    IPMCAST_VALUE_TO_ACTION(_actEntryL2MC, pIpmcAddr->l2_fwd_act, pEntry->l2_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actEntryL3MC, pIpmcAddr->l3_fwd_act, pEntry->l3_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actEntryL3MCRpfFail, pIpmcAddr->l3_rpf_fail_act, pEntry->rpf_fail_act, "", errHandle, ret);

    pIpmcAddr->ttl_min = pEntry->ttl_min;
    pIpmcAddr->l3_en = pEntry->l3_en;

errHandle:
    return ret;
}

static inline int32
_dal_longan_ipmc_rtkIpmcAddr2l3RouteEntry(
    uint32 unit,
    dal_longan_l3_routeEntry_t *pEntry,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_ipmc_enum_alloc_t allocMtu)
{
    int32   ret = RT_ERR_OK;
    int32   l2fwdIdx,l3oilIdx;
    uint32 mtu_index;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_longan_l3_routeEntry_t));

    pEntry->valid = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_DISABLE)? 0 : 1;
    mtu_index = pEntry->mtu_max_idx;

    if (DAL_LONGAN_IPMC_L3TBL_LOOKUP_MODE_FORCE == _pIpmcDb[unit]->l3TblLookupMode)
    {
        pEntry->ipmc_type = 0x1;
        pEntry->bmsk_ipmc_type = 0x1;
    }
    /* SS-627 */
    if (DAL_LONGAN_IPMC_L3TBL_HASH_KEY_SIP_GIP_VID == _pIpmcDb[unit]->l3TblHashKey)
    {
        pEntry->vid_cmp = 1;
    }
    else if (!(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN))
    {
        if (pIpmcAddr->src_intf.vlan_id)
            pEntry->vid_cmp = 1;
    }


    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        pEntry->entry_type = 3;
        pEntry->bmsk_entry_type = 0x3;

        pEntry->gip6 = pIpmcAddr->dst_ip_addr.ipv6;
        osal_memset(pEntry->bmsk_gip6.octet, 0xff, sizeof(rtk_ipv6_addr_t));

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            pEntry->sip6 = pIpmcAddr->src_ip_addr.ipv6;
            osal_memset(pEntry->bmsk_sip6.octet, 0xff, sizeof(rtk_ipv6_addr_t));
            pEntry->round = 0;
        }
        else
        {
            pEntry->round = 1;
        }
    }
    else
    {
        pEntry->entry_type = 1;
        pEntry->bmsk_entry_type = 0x3;

        pEntry->gip = pIpmcAddr->dst_ip_addr.ipv4;
        pEntry->bmsk_gip = 0xffffffff;

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            pEntry->sip = pIpmcAddr->src_ip_addr.ipv4;
            pEntry->bmsk_sip = 0xffffffff;
            pEntry->round = 0;
        }
        else
        {
            pEntry->round = 1;
        }
    }


    pEntry->vid = pIpmcAddr->src_intf.vlan_id;
    pEntry->qos_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pIpmcAddr->qos_pri;
    pEntry->hit = 1;    /* keep HIT bit value */

    IPMCAST_ACTION_TO_VALUE(_actEntryL2MC, pEntry->l2_act, pIpmcAddr->l2_fwd_act, "", errHandle, ret);
    IPMCAST_ACTION_TO_VALUE(_actEntryL3MC, pEntry->l3_act, pIpmcAddr->l3_fwd_act, "", errHandle, ret);
    IPMCAST_ACTION_TO_VALUE(_actEntryL3MCRpfFail, pEntry->rpf_fail_act, pIpmcAddr->l3_rpf_fail_act, "", errHandle, ret);

    pEntry->ttl_min = pIpmcAddr->ttl_min;
    pEntry->rpf_chk_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN) ? 1 : 0;

    if (allocMtu == DAL_LONGAN_IPMC_MTU_REALLOC)
    {
        pEntry->mtu_max_idx = mtu_index;
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuIp6Entry_realloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
        else
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuEntry_realloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
    }
    else if (allocMtu == DAL_LONGAN_IPMC_MTU_ALLOC)
    {
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuIp6Entry_alloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
        else
        {
            RT_ERR_HDL(_dal_longan_l3_ipmc_mtuEntry_alloc(unit, pIpmcAddr->mtu_max, pIpmcAddr->group, &pEntry->mtu_max_idx), errMtuAlloc, ret);
        }
    }

    if (pEntry->vid_cmp)    /* it should consider global configuration of look-up mode */
    {
        pEntry->bmsk_vid = 0xFFF;
    }

    pEntry->bmsk_round = 0x1;


    /*should reserved fwdidx =0; for invalid l2 fwdidx; */
    RT_ERR_HDL(_dal_longan_mcast_fwdIdx_get(unit, pIpmcAddr->group,&l2fwdIdx,&l3oilIdx, &pEntry->stack_fwd_pmsk),errHandle, ret);     //should not check the group entry is not found

    if (l2fwdIdx >= 0)
    {
        pEntry->l2_mc_pmsk_idx = l2fwdIdx;
    }


    /* L2 available */
    if ((pIpmcAddr->l2_fwd_act != RTK_IPMC_ADDR_ACT_FORWARD) || (l2fwdIdx >= 0))
    {
        pEntry->l2_en = 1;
    }

    if ((l3oilIdx < 0))
    {
        pEntry->oil_idx_valid = FALSE;
    }
    else
    {
        pEntry->oil_idx_valid = TRUE;
        pEntry->oil_idx = l3oilIdx;
    }

    pEntry->l3_en = pIpmcAddr->l3_en;
    if (LONGAN_IPMCAST_DBG)
    {
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->ipmc_type = %d\n", pEntry->ipmc_type);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->vid_cmp = %d\n", pEntry->vid_cmp);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->entry_type = %d\n", pEntry->entry_type);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->round = %d\n", pEntry->round);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->vid = %d\n", pEntry->vid);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l2_en = %d\n", pEntry->l2_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l2_act = %d\n", pEntry->l2_act);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l2_mc_pmsk_idx = 0x%X (%d)\n", pEntry->l2_mc_pmsk_idx, pEntry->l2_mc_pmsk_idx);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l3_en = %d\n", pEntry->l3_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l3_act = %d\n", pEntry->l3_act);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_chk_en = %d\n", pEntry->rpf_chk_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_fail_act = %d\n", pEntry->rpf_fail_act);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx_valid = %d\n", pEntry->oil_idx_valid);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx = 0x%X (%d)\n", pEntry->oil_idx, pEntry->oil_idx);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->ttl_min = %d\n", pEntry->ttl_min);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->mtu_max = %d\n", pEntry->mtu_max_idx);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->qos_en = %d\n", pEntry->qos_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->qos_pri = %d\n", pEntry->qos_pri);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->stack_fwd_pmsk = 0x%04X\n", pEntry->stack_fwd_pmsk);
    }

errMtuAlloc:
errHandle:
    return ret;
  }

static inline int32
_dal_longan_ipmc_l3RouteEntry2rtkIpmcAddr(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_l3_routeEntry_t *pEntry,
    rtk_mcast_group_t groupId)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pIpmcAddr, 0x00, sizeof(rtk_ipmc_addr_t));

    pIpmcAddr->vrf_id = 0; //not support;
    pIpmcAddr->src_intf.vlan_id = pEntry->vid;

    if (!pEntry->valid)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_DISABLE;
    }

    if (0x1 == pEntry->entry_type)          /* IPMC */
    {


        if (0x0 == pEntry->round)
        {
            pIpmcAddr->src_ip_addr.ipv4 = pEntry->sip;
            pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
        }
        pIpmcAddr->dst_ip_addr.ipv4 = pEntry->gip;
        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
            pEntry->mtu_max_idx, LONGAN_MTU_VALf, pIpmcAddr->mtu_max, "", errHandle, ret);
    }
    else if (0x3 == pEntry->entry_type)     /* IP6MC */
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_IPV6;


        if (0x0 == pEntry->round)
        {
            pIpmcAddr->src_ip_addr.ipv6 = pEntry->sip6;
            pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
        }
        pIpmcAddr->dst_ip_addr.ipv6 = pEntry->gip6;
        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6_MTU_CTRLr, REG_ARRAY_INDEX_NONE, \
            pEntry->mtu_max_idx, LONGAN_MTU_VALf, pIpmcAddr->mtu_max, "", errHandle, ret);
    }
    else
    {
        return RT_ERR_INPUT;
    }

    if (pEntry->qos_en)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN;
        pIpmcAddr->qos_pri = pEntry->qos_pri;
    }

    if (pEntry->rpf_chk_en)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_RPF_EN;
    }

    if (pEntry->hit)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_HIT;
    }

    pIpmcAddr->group = groupId;

    IPMCAST_VALUE_TO_ACTION(_actEntryL2MC, pIpmcAddr->l2_fwd_act, pEntry->l2_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actEntryL3MC, pIpmcAddr->l3_fwd_act, pEntry->l3_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actEntryL3MCRpfFail, pIpmcAddr->l3_rpf_fail_act, pEntry->rpf_fail_act, "", errHandle, ret);

    pIpmcAddr->ttl_min = pEntry->ttl_min;
    pIpmcAddr->l3_en = pEntry->l3_en;

    if (0)
    {
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->ipmc_type = %d\n", pEntry->ipmc_type);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->vid_cmp = %d\n", pEntry->vid_cmp);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->entry_type = %d\n", pEntry->entry_type);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->vid = %d\n", pEntry->vid);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l2_en = %d\n", pEntry->l2_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l2_act = %d\n", pEntry->l2_act);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l2_mc_pmsk_idx = 0x%X (%d)\n", pEntry->l2_mc_pmsk_idx, pEntry->l2_mc_pmsk_idx);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l3_en = %d\n", pEntry->l3_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->l3_act = %d\n", pEntry->l3_act);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_chk_en = %d\n", pEntry->rpf_chk_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_fail_act = %d\n", pEntry->rpf_fail_act);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx_valid = %d\n", pEntry->oil_idx_valid);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx = 0x%X (%d)\n", pEntry->oil_idx, pEntry->oil_idx);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->ttl_min = %d\n", pEntry->ttl_min);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->mtu_max = %d\n", pEntry->mtu_max_idx);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->qos_en = %d\n", pEntry->qos_en);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->qos_pri = %d\n", pEntry->qos_pri);
        LONGAN_IPMCAST_DBG_PRINTF(1, "pEntry->stack_fwd_pmsk = 0x%04X\n", pEntry->stack_fwd_pmsk);
    }
errHandle:
    return ret;
}

static uint32
_dal_longan_ipmc_hostHash0_ret(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr, uint32 move_dip_0_8)
{
    uint32  hashRow[32];
    rtk_ipv6_addr_t     sipv6;
    rtk_ip_addr_t       sipv4;
    uint32  hashIdx = DAL_LONGAN_IPMCAST_INVALID_HASH_IDX;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    osal_memset(hashRow, 0x0, sizeof(hashRow));

    /* calculate hash index for new hash alg */
    /*hashRow[0][0~14] : sip                        hashRow[0][15~29] : dip             hashRow[0][30~31] : vid
     *hashRow[1][0~3]  : sip[31:0]                  hashRow[1][4~7] : dip[31:0]
     *hashRow[1][8~18] : sip[127:32],dip[127:128]   hashRow[1][19~29] : dip[127:32]
     *hashRow[1][30]   : pre_hash                   hashRow[1][31~32] : vid
     */
    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            osal_memcpy(&sipv6, &pIpmcAddr->src_ip_addr.ipv6, sizeof(rtk_ipv6_addr_t));
        }
        else
        {
            osal_memset(sipv6.octet, 0xFF, sizeof(rtk_ipv6_addr_t));
        }

        /* source IPv6 address */
        hashRow[0] = (HASH_BIT_EXTRACT(sipv6.octet[0], 3, 5) << 0);                /* SIP [127:123] */
        hashRow[1] = (HASH_BIT_EXTRACT(sipv6.octet[0], 0, 3) << 6) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[1], 2, 6) << 0);            /* SIP [122:114] */
        hashRow[2] = (HASH_BIT_EXTRACT(sipv6.octet[1], 0, 2) << 7) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[2], 1, 7) << 0);            /* SIP [113:105] */
        hashRow[3] = (HASH_BIT_EXTRACT(sipv6.octet[2], 0, 1) << 8) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[3], 0, 8) << 0);            /* SIP [104:96] */
        hashRow[4] = (HASH_BIT_EXTRACT(sipv6.octet[4], 0, 8) << 1) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[5], 7, 1) << 0);            /* SIP [95:87] */
        hashRow[5] = (HASH_BIT_EXTRACT(sipv6.octet[5], 0, 7) << 2) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[6], 6, 2) << 0);            /* SIP [86:78] */
        hashRow[6] =(HASH_BIT_EXTRACT(sipv6.octet[6], 0, 6) << 3) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[7], 5, 3) << 0);            /* SIP [77:69] */
        hashRow[7] = (HASH_BIT_EXTRACT(sipv6.octet[7], 0, 5) << 4) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[8], 4, 4) << 0);            /* SIP [68:60] */
        hashRow[8] = (HASH_BIT_EXTRACT(sipv6.octet[8], 0, 4) << 5) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[9], 3, 5) << 0);            /* SIP [51:42] */
        hashRow[9] = (HASH_BIT_EXTRACT(sipv6.octet[9], 0, 3) << 6) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[10], 2, 6) << 0);           /* SIP [41:33] */
        hashRow[10] = (HASH_BIT_EXTRACT(sipv6.octet[10], 0, 2) << 7) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[11], 1, 7) << 0);           /* SIP [32:24] */
        hashRow[11] = (HASH_BIT_EXTRACT(sipv6.octet[11], 0, 1) << 8) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[12], 0, 8) << 0);           /* SIP [23:15] */
        hashRow[12] = (HASH_BIT_EXTRACT(sipv6.octet[13], 0, 8) << 1) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[14], 7, 1) << 0);           /* SIP [14:6] */
        hashRow[13] = (HASH_BIT_EXTRACT(sipv6.octet[14], 0, 7) << 2) | \
                            (HASH_BIT_EXTRACT(sipv6.octet[15], 6, 2) << 0);           /* SIP [5:0] */
        hashRow[14] = (HASH_BIT_EXTRACT(sipv6.octet[15], 0, 6) << 2);

        /* destination IPv6 address */
        hashRow[15] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[0], 6, 2) << 0);           /* DIP [127:126] */
        hashRow[16] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[0], 0, 6) << 3) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[1], 5, 3) << 0);        /* DIP [125:117] */
        hashRow[17] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[1], 0, 5) << 4) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[2], 4, 4) << 0);        /* DIP [116:108] */
        hashRow[18] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[2], 0, 4) << 5) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[3], 3, 5) << 0);        /* DIP [107:99] */
        hashRow[19] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[3], 0, 3) << 6) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[4], 2, 6) << 0);        /* DIP [98:90] */
        hashRow[20] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[4], 0, 2) << 7) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[5], 1, 7) << 0);        /* DIP [89:81] */
        hashRow[21] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[5], 0, 1) << 8) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[6], 0, 8) << 0);        /* DIP [90:72] */
        hashRow[22] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[7], 0, 8) << 1) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[8], 7, 1) << 0);        /* DIP [71:63] */
        hashRow[23] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[8], 0, 7) << 2) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[9], 6, 2) << 0);        /* DIP [62:54] */
        hashRow[24] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[9], 0, 6) << 3) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[10], 5, 3) << 0);       /* DIP [53:45] */
        hashRow[25] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[10], 0, 5) << 4) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[11], 4, 4) << 0);       /* DIP [44:36] */
        hashRow[26] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[11], 0, 4) << 5) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[12], 3, 5) << 0);       /* DIP [35:27] */
        hashRow[27] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[12], 0, 3) << 6) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[13], 2, 6) << 0);       /* DIP [26:18] */
        hashRow[28] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[13], 0, 2) << 7) | \
                            (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[14], 1, 7) << 0);       /* DIP [17:9] */
        if (!move_dip_0_8)
        {
            hashRow[29] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[14], 0, 1) << 8) | \
                                (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[15], 0, 8) << 0);        /* DIP [8:0] */
        }
    }
    else  /*IPV4*/
    {
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            sipv4 = pIpmcAddr->src_ip_addr.ipv4;
        }
        else
        {
            sipv4 = 0xFFFFFFFF;
        }

        /* source IPv4 address */
        hashRow[0] = HASH_BIT_EXTRACT(sipv4, 24, 8);
        hashRow[1] = HASH_BIT_EXTRACT(sipv4, 15, 9);
        hashRow[2] = HASH_BIT_EXTRACT(sipv4, 6, 9);
        hashRow[3] = HASH_BIT_EXTRACT(sipv4, 0, 6) << 2;

        /* destination IPv4 address */
        hashRow[4] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 27, 5);
        hashRow[5] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 18, 9);
        hashRow[6] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 9, 9);
        if (!move_dip_0_8)
            hashRow[7] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 0, 9);
    }

     /* VLAN ID */
    if (DAL_LONGAN_IPMC_L3TBL_HASH_KEY_SIP_GIP_VID == _pIpmcDb[unit]->l3TblHashKey)
    {
        hashRow[30] = HASH_BIT_EXTRACT(pIpmcAddr->src_intf.vlan_id, 3, 9);
        hashRow[31] = (HASH_BIT_EXTRACT(pIpmcAddr->src_intf.vlan_id, 0, 3) << 5);
    }

    hashIdx = \
        hashRow[0]  ^ hashRow[1]  ^ hashRow[2]  ^ hashRow[3]  ^ \
        hashRow[4]  ^ hashRow[5]  ^ hashRow[6]  ^ hashRow[7]  ^ \
        hashRow[8]  ^ hashRow[9]  ^ hashRow[10] ^ hashRow[11] ^ \
        hashRow[12] ^ hashRow[13] ^ hashRow[14] ^ hashRow[15] ^ \
        hashRow[16] ^ hashRow[17] ^ hashRow[18] ^ hashRow[19] ^ \
        hashRow[20] ^ hashRow[21] ^ hashRow[22] ^ hashRow[23] ^ \
        hashRow[24] ^ hashRow[25] ^ hashRow[26] ^ hashRow[27] ^ \
        hashRow[28] ^ hashRow[29] ^ hashRow[30] ^ hashRow[31];

    return hashIdx;
}

static uint32
_dal_longan_ipmc_hostHash1_ret(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr, uint32 move_dip_0_8)
{
    uint32  hashRow[32];
    uint32  sum = 0, sum1 = 0,  preHash = 0;
    rtk_ipv6_addr_t     sipv6;
    rtk_ip_addr_t       sipv4;
    uint32  hashIdx = DAL_LONGAN_IPMCAST_INVALID_HASH_IDX;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    osal_memset(hashRow, 0x0, sizeof(hashRow));

    /* calculate hash index for new hash alg */
    /*hashRow[0][0~14] : sip                        hashRow[0][15~29] : dip             hashRow[0][30~31] : vid
    *hashRow[1][0~3]  : sip[31:0]                  hashRow[1][4~7] : dip[31:0]
    *hashRow[1][8~18] : sip[127:32],dip[127:128]   hashRow[1][19~29] : dip[127:32]
    *hashRow[1][30]   : pre_hash                   hashRow[1][31~32] : vid
    */
    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            osal_memcpy(&sipv6, &pIpmcAddr->src_ip_addr.ipv6, sizeof(rtk_ipv6_addr_t));
        }
        else
        {
            osal_memset(sipv6.octet, 0xFF, sizeof(rtk_ipv6_addr_t));
        }

        /* source IPv6 address */
        hashRow[0] = (HASH_BIT_EXTRACT(sipv6.octet[12], 2, 6) << 0);
        hashRow[1] = (HASH_BIT_EXTRACT(sipv6.octet[12], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[13], 1, 7) << 0);
        hashRow[2] = (HASH_BIT_EXTRACT(sipv6.octet[13], 0, 1) << 8) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[14], 0, 8) << 0);
        hashRow[3] = (HASH_BIT_EXTRACT(sipv6.octet[15], 0, 8) << 1);

        hashRow[4] = (HASH_BIT_EXTRACT(sipv6.octet[0], 0, 8) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(sipv6.octet[1], 0, 8) << 1) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[2], 7, 1) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(sipv6.octet[2], 0, 7) << 2) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[3], 6, 2) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(sipv6.octet[3], 0, 6) << 3) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[4], 5, 3) << 0);
        hashRow[8] = (HASH_BIT_EXTRACT(sipv6.octet[4], 0, 5) << 4) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[5], 4, 4) << 0);
        hashRow[9] = (HASH_BIT_EXTRACT(sipv6.octet[5], 0, 4) << 5) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[6], 3, 5) << 0);
        hashRow[10] = (HASH_BIT_EXTRACT(sipv6.octet[6], 0, 3) << 6) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[7], 2, 6) << 0);
        hashRow[11] = (HASH_BIT_EXTRACT(sipv6.octet[7], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[8], 1, 7) << 0);
        hashRow[12] = (HASH_BIT_EXTRACT(sipv6.octet[8], 0, 1) << 8) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[9], 0, 8) << 0);
        hashRow[13] = (HASH_BIT_EXTRACT(sipv6.octet[10], 0, 8) << 1) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[11], 7, 1) << 0);

        /* destination IPv6 address */
        hashRow[14] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[0], 6, 2) << 0) | \
                      (HASH_BIT_EXTRACT(sipv6.octet[11], 0, 7) << 2);
        hashRow[15] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[0], 0, 6) << 3) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[1], 5, 3) << 0);
        hashRow[16] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[1], 0, 5) << 4) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[2], 4, 4) << 0);
        hashRow[17] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[2], 0, 4) << 5) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[3], 3, 5) << 0);
        hashRow[18] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[3], 0, 3) << 6) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[4], 2, 6) << 0);
        hashRow[19] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[4], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[5], 1, 7) << 0);
        hashRow[20] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[5], 0, 1) << 8) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[6], 0, 8) << 0);
        hashRow[21] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[7], 0, 8) << 1) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[8], 7, 1) << 0);
        hashRow[22] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[8], 0, 7) << 2) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[9], 6, 2) << 0);
        hashRow[23] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[9], 0, 6) << 3) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[10], 5, 3) << 0);
        hashRow[24] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[10], 0, 5) << 4) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[11], 4, 4) << 0);
        hashRow[25] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[11], 0, 4) << 5);

        hashRow[26] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[12], 3, 5) << 0);
        hashRow[27] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[12], 0, 3) << 6) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[13], 2, 6) << 0);
        hashRow[28] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[13], 0, 2) << 7) | \
                      (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[14], 1, 7) << 0);
        if (!move_dip_0_8)
        {
            hashRow[29] = (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[14], 0, 1) << 8) | \
                        (HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv6.octet[15], 0, 8) << 0);
        }
    }
    else  /*IPV4*/
    {
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            sipv4 = pIpmcAddr->src_ip_addr.ipv4;
        }
        else
        {
            sipv4 = 0xFFFFFFFF;
        }

        /* source IPv4 address */
        hashRow[0] = HASH_BIT_EXTRACT(sipv4, 26, 6);
        hashRow[1] = HASH_BIT_EXTRACT(sipv4, 17, 9);
        hashRow[2] = HASH_BIT_EXTRACT(sipv4, 8, 9);
        hashRow[3] = HASH_BIT_EXTRACT(sipv4, 0, 8) << 1;

        /* destination IPv4 address */
        hashRow[26] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 27, 5);
        hashRow[27] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 18, 9);
        hashRow[28] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 9, 9);
        if (!move_dip_0_8)
            hashRow[29] = HASH_BIT_EXTRACT(pIpmcAddr->dst_ip_addr.ipv4, 0, 9);
    }

     /* VLAN ID */
    if (DAL_LONGAN_IPMC_L3TBL_HASH_KEY_SIP_GIP_VID == _pIpmcDb[unit]->l3TblHashKey)
    {
        hashRow[30] = (HASH_BIT_EXTRACT(pIpmcAddr->src_intf.vlan_id, 7, 5)) | (hashRow[25]);
        hashRow[25] = 0x0;   //reset hashRow[1][25];
        hashRow[31] = (HASH_BIT_EXTRACT(pIpmcAddr->src_intf.vlan_id, 0, 7) << 2);
    }

    /* calc pre_hash */
    sum =   hashRow[0] + hashRow[1] + hashRow[2] + hashRow[3] + \
                hashRow[26] + hashRow[27] + hashRow[28];                           /* dip[31:9] */
    sum1 = (sum & 0x1FF) + ((sum & (0x1FF << 9)) >> 9);
    preHash = (sum1 & 0x1FF) + ((sum1 & (0x1FF << 9)) >> 9);

    hashIdx = \
        hashRow[4] ^ hashRow[5] ^ hashRow[6] ^ hashRow[7] ^ \
        hashRow[8] ^ hashRow[9] ^ hashRow[10] ^ hashRow[11] ^ \
        hashRow[12] ^ hashRow[13] ^ hashRow[14] ^ hashRow[15] ^ \
        hashRow[16] ^ hashRow[17] ^ hashRow[18] ^ hashRow[19] ^ \
        hashRow[20] ^ hashRow[21] ^ hashRow[22] ^ hashRow[23] ^ \
        hashRow[24] ^ hashRow[25] ^ hashRow[29] ^ hashRow[30] ^ \
        hashRow[31] ^ preHash;

    return hashIdx;
}

int32
_dal_longan_ipmc_dip_update(uint32 unit, uint32 hw_entry_idx, dal_longan_l3_hostEntry_t *pHostEntry, dal_longan_l3_api_flag_t flag)
{
    int32 ret = RT_ERR_OK;
    dal_longan_ipmc_hashIdx_t   hashIdx;
    uint32  alg_of_tbl[DAL_LONGAN_L3_HOST_TBL_NUM];
    uint32  hashIdxTbl[DAL_LONGAN_L3_HOST_TBL_NUM];
    uint32  tbl;
    uint32  dip_0_8;
    uint32  addr,  addrHashIdx;
    rtk_ipmc_addr_t ipmcAddr;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHostEntry), RT_ERR_NULL_POINTER);

    osal_memset(&hashIdx, 0x0, sizeof(dal_longan_ipmc_hashIdx_t));
    osal_memset(&ipmcAddr, 0x0, sizeof(rtk_ipmc_addr_t));

    /* function body */
    RT_PARAM_CHK((hw_entry_idx >= DAL_LONGAN_L3_HOST_TBL_SIZE), RT_ERR_OUT_OF_RANGE);

    RT_ERR_HDL(_dal_longan_ipmc_l3HostEntry2rtkIpmcAddr(unit, &ipmcAddr, pHostEntry, 0), errHandle, ret);
    hashIdx.hashIdx_of_alg[0] = _dal_longan_ipmc_hostHash0_ret(unit, &ipmcAddr, TRUE);
    hashIdx.hashIdx_of_alg[1] = _dal_longan_ipmc_hostHash1_ret(unit, &ipmcAddr, TRUE);

    IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_MC_HASH_ALG_SEL_0f, alg_of_tbl[0], "", errHandle, ret);
    IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_MC_HASH_ALG_SEL_1f, alg_of_tbl[1], "", errHandle, ret);

    /* calculate hash index for each table */
    for (tbl=0; tbl<DAL_LONGAN_L3_HOST_TBL_NUM; tbl++)
    {
        hashIdxTbl[tbl] = hashIdx.hashIdx_of_alg[(alg_of_tbl[tbl] & 0x1)];
    }

    addr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(hw_entry_idx);
    tbl = (addr & (0x1 << 12)) >> 12;
    addrHashIdx = (addr & (0x1FF << 3)) >> 3;
    dip_0_8 = hashIdxTbl[tbl] ^ addrHashIdx;

    if (flag & DAL_LONGAN_L3_API_FLAG_IPV6)
    {
        pHostEntry->gip6.octet[15] = dip_0_8;
        pHostEntry->gip6.octet[14] = (pHostEntry->gip6.octet[14] & 0xfe) | (dip_0_8 >> 8);
    }
    else
    {
        pHostEntry->gip = (pHostEntry->gip & 0xfffffe00) | dip_0_8;
    }

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_ipmc_host_get
 * Description:
 *      Get an ipmcast host entry .
 * Input:
 *      unit   - unit id
 *      index  - pointer to entry index
 *      flag - ipmc flag for RTK_IPMC_FLAG_READ_HIT_IGNORE
 *      pEntry - pointer to entry
 *      pIpmcAddr - pointer to host ipmcast entry
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
 *      if flag = RTK_IPMC_FLAG_READ_HIT_IGNORE, the entry get from software database
 */
int32
_dal_longan_ipmc_host_get(uint32 unit, uint32 index, rtk_ipmc_flag_t flag, dal_longan_l3_hostEntry_t *pEntry, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret;
    uint32 retry = 0;
    uint32 timeRetry = 0;
    dal_longan_l3_hostEntry_t tmpHostEntry;

    RT_PARAM_CHK((NULL == pEntry) || (NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    if (CHIP_REV_ID_A == HWP_CHIP_REV(unit))
    {
        osal_memset(&tmpHostEntry, 0x0, sizeof(dal_longan_l3_hostEntry_t));
        RT_ERR_HDL(_dal_longan_ipmc_rtkIpmcAddr2l3HostEntry(unit, &tmpHostEntry, pIpmcAddr, DAL_LONGAN_IPMC_MTU_NOT_ALLOC), errHandle, ret);

        if (flag & RTK_IPMC_FLAG_READ_HIT_IGNORE)
        {
            *pEntry = tmpHostEntry;
        }
        else
        {
            do
            {
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
            {
                return ret;
            }

            if (ipmc_rsv_index != index)
                ret = _dal_longan_ipmc_hostEntryCmp(unit, &tmpHostEntry, pEntry);
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

static int32
_dal_longan_ipmc_rtkIpmcAddr_get(
    uint32 unit,
    rtk_ipmc_flag_t flags,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_ipmc_l3Entry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    dal_longan_l3_hostEntry_t    hostEntry;
#if LONGAN_L3_ROUTE_IPMC_SIZE
    dal_longan_l3_routeEntry_t   routeEntry;
#endif
    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!L3_ENTRY_IS_VALID(pEntry), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
#if LONGAN_L3_ROUTE_IPMC_SIZE
    if (FALSE == pEntry->cam)
    {
        RT_ERR_HDL(_dal_longan_ipmc_host_get(unit, pEntry->hw_entry_idx, flags, &hostEntry, \
                                        L3_ENTRY_IPMC_ADDR(unit, pEntry->hw_entry_idx)), errHandle, ret);
        RT_ERR_HDL(_dal_longan_ipmc_l3HostEntry2rtkIpmcAddr(unit, pIpmcAddr, &hostEntry, pEntry->groupId), errHandle, ret);
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_routeEntry_get(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);
        RT_ERR_HDL(_dal_longan_ipmc_l3RouteEntry2rtkIpmcAddr(unit, pIpmcAddr, &routeEntry, pEntry->groupId), errHandle, ret);
    }
#else
    RT_ERR_HDL(_dal_longan_ipmc_host_get(unit, pEntry->hw_entry_idx, flags, &hostEntry, \
                                    L3_ENTRY_IPMC_ADDR(unit, pEntry->hw_entry_idx)), errHandle, ret);
    RT_ERR_HDL(_dal_longan_ipmc_l3HostEntry2rtkIpmcAddr(unit, pIpmcAddr, &hostEntry, pEntry->groupId), errHandle, ret);
#endif

    pIpmcAddr->hw_entry_idx = pEntry->hw_entry_idx;

errHandle:
    return ret;
}

static int32
_dal_longan_ipmc_hashIdx_get(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr, dal_longan_ipmc_hashIdx_t *pHashIdx)
{
    int32 ret = RT_ERR_OK;
    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHashIdx), RT_ERR_NULL_POINTER);

    /* function body */
    pHashIdx->hashIdx_of_alg[0] = _dal_longan_ipmc_hostHash0_ret(unit, pIpmcAddr, FALSE);
    pHashIdx->hashIdx_of_alg[1] = _dal_longan_ipmc_hostHash1_ret(unit, pIpmcAddr, FALSE);

    if (DAL_LONGAN_IPMCAST_INVALID_HASH_IDX == pHashIdx->hashIdx_of_alg[0] || \
        DAL_LONGAN_IPMCAST_INVALID_HASH_IDX == pHashIdx->hashIdx_of_alg[1])
        ret = RT_ERR_FAILED;

    return ret;
}

static int32
_dal_longan_ipmc_l3HostAlloc_get(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_ipmc_hashIdx_t *pIpmcHashIdx,
    dal_longan_l3_hostAlloc_t *pHostAlloc)
{
    int32   ret = RT_ERR_OK;
    uint32  alg_of_tbl[DAL_LONGAN_L3_HOST_TBL_NUM];
    uint32  tbl;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHostAlloc), RT_ERR_NULL_POINTER);

    /* function body */
    IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_MC_HASH_ALG_SEL_0f, alg_of_tbl[0], "", errHandle, ret);
    IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_MC_HASH_ALG_SEL_1f, alg_of_tbl[1], "", errHandle, ret);

    /* calculate hash index for each table */
    for (tbl=0; tbl<DAL_LONGAN_L3_HOST_TBL_NUM; tbl++)
    {
        pHostAlloc->hashIdx.idx_of_tbl[tbl] = pIpmcHashIdx->hashIdx_of_alg[(alg_of_tbl[tbl] & 0x1)];
    }

    /* entry width: IP6MC (6), IPMC (2) */
    pHostAlloc->width = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)? 6 : 2;

errHandle:
    return ret;
}


static inline uint32
_dal_longan_ipmc_signKey_ret(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr, dal_longan_ipmc_hashIdx_t *pHashIdx)
{
    uint32  signKey = 0;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), 0);   /* return key as zero */
    RT_PARAM_CHK((NULL == pHashIdx), 0);    /* return key as zero */

    /* function body */
    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        signKey |= (0x1) << 24;
    }
    signKey |= (pHashIdx->hashIdx_of_alg[1]) << 9;
    signKey |= (pHashIdx->hashIdx_of_alg[0]) << 0;

    return signKey;
}


static inline int32 _dal_longan_ipmc_l3Entry_alloc(uint32 unit, dal_longan_ipmc_l3Entry_t **ppEntry)
{
    dal_longan_ipmc_l3Entry_t *pEntry;

    if (RTK_LIST_EMPTY(&_pIpmcDb[unit]->free_l3Entry_list))
    {
        return RT_ERR_TBL_FULL;
    }

    pEntry = RTK_LIST_NODE_HEAD(&_pIpmcDb[unit]->free_l3Entry_list);
    if (NULL == pEntry)
    {
        return RT_ERR_FAILED;   /* should not happen */
    }
    RTK_LIST_NODE_REMOVE(&_pIpmcDb[unit]->free_l3Entry_list, pEntry, nodeRef);
    RTK_LIST_NODE_REF_INIT(pEntry, nodeRef);
    L3_ENTRY_VALIDATE(pEntry);

    *ppEntry = pEntry;

    return RT_ERR_OK;
}

static inline int32 _dal_longan_ipmc_l3Entry_free(uint32 unit, dal_longan_ipmc_l3Entry_t *pEntry)
{
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        L3_ENTRY_RESET(pEntry);
        RTK_LIST_NODE_INSERT_TAIL(&_pIpmcDb[unit]->free_l3Entry_list, pEntry, nodeRef);

        return RT_ERR_OK;
    }

    return RT_ERR_ENTRY_NOTFOUND;
}

static int32
_dal_longan_ipmc_l3EntryMatch_ret(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_ipmc_l3Entry_t *pEntry,
    uint32 isRetEntry)
{
    dal_longan_l3_hostEntry_t hostEntry;
    dal_longan_l3_routeEntry_t routeEntry;
    rtk_ipmc_addr_t ipmcAddr;
    int32 ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!L3_ENTRY_IS_VALID(pEntry)), RT_ERR_INPUT);

    /* read back from H/W table */
    if (FALSE == pEntry->cam)
    {
        ret = _dal_longan_ipmc_host_get(unit, pEntry->hw_entry_idx, pIpmcAddr->ipmc_flags, &hostEntry,
                            L3_ENTRY_IPMC_ADDR(unit, pEntry->hw_entry_idx));

        ret += _dal_longan_ipmc_l3HostEntry2rtkIpmcAddr(unit, &ipmcAddr, &hostEntry, pEntry->groupId);
        if (RT_ERR_OK != ret)
        {
            return RT_ERR_FAILED;   /* fatal error */
        }
    }
    else
    {
        if ((RT_ERR_OK != _dal_longan_l3_routeEntry_get(unit, pEntry->hw_entry_idx, &routeEntry, \
                                                      DAL_LONGAN_L3_API_FLAG_NONE)) || \
            (RT_ERR_OK != _dal_longan_ipmc_l3RouteEntry2rtkIpmcAddr(unit, &ipmcAddr, &routeEntry,pEntry->groupId)))
        {
            return RT_ERR_FAILED;   /* fatal error */
        }
    }
#if LONGAN_IPMCAST_DBG
    LONGAN_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->vrf_id = %u, ipmcAddr.vrf_id = %u\n", \
        pIpmcAddr->vrf_id, ipmcAddr.vrf_id);
    LONGAN_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->ipmc_flags = %u, ipmcAddr.ipmc_flags = %u\n", \
        pIpmcAddr->ipmc_flags, ipmcAddr.ipmc_flags);
    LONGAN_IPMCAST_DBG_PRINTF(3, "(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID) = %u, (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID) = %u\n", \
        (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID), (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID));
    LONGAN_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->src_intf.intf_id = %u, ipmcAddr.src_intf.intf_id = %u\n", \
        pIpmcAddr->src_intf.intf_id, ipmcAddr.src_intf.intf_id);
    LONGAN_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->src_ip_addr.ipv4 = 0x%08X, ipmcAddr.src_ip_addr.ipv4 = 0x%08X\n", \
        pIpmcAddr->src_ip_addr.ipv4, ipmcAddr.src_ip_addr.ipv4);
    LONGAN_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->dst_ip_addr.ipv4 = 0x%08X, ipmcAddr.dst_ip_addr.ipv4 = 0x%08X\n", \
        pIpmcAddr->dst_ip_addr.ipv4, ipmcAddr.dst_ip_addr.ipv4);


    LONGAN_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->src_ip_addr.ipv6[15] = 0x%02X, ipmcAddr.src_ip_addr.ipv6[15] = 0x%02X\n", \
        pIpmcAddr->src_ip_addr.ipv6.octet[15], ipmcAddr.src_ip_addr.ipv6.octet[15]);
    LONGAN_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->dst_ip_addr.ipv6[15] = 0x%02X, ipmcAddr.dst_ip_addr.ipv6[15] = 0x%02X\n", \
        pIpmcAddr->dst_ip_addr.ipv6.octet[15], ipmcAddr.dst_ip_addr.ipv6.octet[15]);
#endif

    /* compare entries */
    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID)
    {
        if (!(ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID))
            return RT_ERR_FAILED;

        if (pIpmcAddr->src_intf.intf_id != ipmcAddr.src_intf.intf_id)
            return RT_ERR_FAILED;
    }
    else
    {
        if (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID)
            return RT_ERR_FAILED;

        if (pIpmcAddr->src_intf.vlan_id != ipmcAddr.src_intf.vlan_id)
            return RT_ERR_FAILED;
    }

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        if (!(ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_IPV6))
            return RT_ERR_FAILED;

        if (rt_util_ipv6IsAllOnes_ret(&pIpmcAddr->src_ip_addr.ipv6))
        {
            if (!rt_util_ipv6IsZero_ret(&ipmcAddr.src_ip_addr.ipv6))
                return RT_ERR_FAILED;
        }
        else
        {
            if (RT_ERR_OK != rt_util_ipv6Cmp(&pIpmcAddr->src_ip_addr.ipv6, &ipmcAddr.src_ip_addr.ipv6))
                return RT_ERR_FAILED;
         }

        if (RT_ERR_OK != rt_util_ipv6Cmp(&pIpmcAddr->dst_ip_addr.ipv6, &ipmcAddr.dst_ip_addr.ipv6))
            return RT_ERR_FAILED;
    }
    else
    {
        if (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_IPV6)
            return RT_ERR_FAILED;

        if (0xFFFFFFFF == pIpmcAddr->src_ip_addr.ipv4)
        {
            if (0x0 != ipmcAddr.src_ip_addr.ipv4)
                return RT_ERR_FAILED;
        }
        else
        {
            if (pIpmcAddr->src_ip_addr.ipv4 != ipmcAddr.src_ip_addr.ipv4)
                return RT_ERR_FAILED;
        }

        if (pIpmcAddr->dst_ip_addr.ipv4 != ipmcAddr.dst_ip_addr.ipv4)
            return RT_ERR_FAILED;
    }

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_HIT_CLEAR)
    {
        if (FALSE == pEntry->cam)
        {
            if (hostEntry.hit)
            {
                hostEntry.hit = 0;
                if (RT_ERR_OK !=_dal_longan_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                                            DAL_LONGAN_L3_API_FLAG_NONE))
                {
                    return RT_ERR_FAILED;
                }
            }
        }
        else
        {
            if (routeEntry.hit)
            {
                routeEntry.hit = 0;
                if (RT_ERR_OK != _dal_longan_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, \
                                                      DAL_LONGAN_L3_API_FLAG_NONE))
                {
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    if (isRetEntry)
    {
        pIpmcAddr->ipmc_flags = ipmcAddr.ipmc_flags;
        pIpmcAddr->l2_fwd_act = ipmcAddr.l2_fwd_act;
        pIpmcAddr->l3_fwd_act = ipmcAddr.l3_fwd_act;
        pIpmcAddr->l3_en = ipmcAddr.l3_en;
        pIpmcAddr->l3_rpf_fail_act = ipmcAddr.l3_rpf_fail_act;
        pIpmcAddr->ttl_min = ipmcAddr.ttl_min;
        pIpmcAddr->mtu_max = ipmcAddr.mtu_max;
        pIpmcAddr->qos_pri = ipmcAddr.qos_pri;
    }

    return RT_ERR_OK;   /* match */
}


static int32
_dal_longan_ipmc_l3Entry_find_withSignKey(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_longan_ipmc_l3Entry_t **ppEntry,
    uint32 signKey,
    uint32 isRetAddrEntry)
{
    dal_longan_ipmc_l3Entry_t *pEntry;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == ppEntry), RT_ERR_NULL_POINTER);

    /* function body */
    RTK_LIST_FOREACH(&_pIpmcDb[unit]->used_l3Entry_list, pEntry, nodeRef)
    {
        if (pEntry->signKey > signKey)
        {
            break;
        }

        if (pEntry->signKey == signKey)
        {
            /* check further */
            if (RT_ERR_OK == _dal_longan_ipmc_l3EntryMatch_ret(unit, pIpmcAddr, pEntry, isRetAddrEntry))
            {
                *ppEntry = pEntry;
                return RT_ERR_OK;   /* entry found */
            }
        }
    }

    *ppEntry = NULL;    /* entry not found, but return OK */

    return RT_ERR_OK;
}


static inline int32
_dal_longan_ipmc_l3EntryUsedList_insert(uint32 unit, dal_longan_ipmc_l3Entry_t *pEntry)
{
    dal_longan_ipmc_l3Entry_t *pPrev = NULL;
    dal_longan_ipmc_l3Entry_t *pTemp;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    RTK_LIST_FOREACH(&_pIpmcDb[unit]->used_l3Entry_list, pTemp, nodeRef)
    {
        if (pTemp->signKey > pEntry->signKey)
            break;

        pPrev = pTemp;
    }

    if (NULL == pPrev)
        RTK_LIST_NODE_INSERT_HEAD(&_pIpmcDb[unit]->used_l3Entry_list, pEntry, nodeRef);
    else
        RTK_LIST_NODE_INSERT_AFTER(&_pIpmcDb[unit]->used_l3Entry_list, pPrev, pEntry, nodeRef);

    if (FALSE == pEntry->cam)
        L3_HW_IDX_VALID(unit, pEntry->hw_entry_idx, L3_ENTRY_ADDR_TO_IDX(unit, pEntry));
    else
        L3_HW_IDX_VALID(unit, pEntry->hw_entry_idx + DAL_LONGAN_L3_HOST_TBL_SIZE, L3_ENTRY_ADDR_TO_IDX(unit, pEntry));
    ;

    return RT_ERR_OK;
}

static inline int32
_dal_longan_ipmc_l3EntryUsedList_remove(uint32 unit, dal_longan_ipmc_l3Entry_t *pEntry)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    if (FALSE == pEntry->cam)
        L3_HW_IDX_INVALID(unit, pEntry->hw_entry_idx);
    else
        L3_HW_IDX_INVALID(unit, pEntry->hw_entry_idx + DAL_LONGAN_L3_HOST_TBL_SIZE);

    RTK_LIST_NODE_REMOVE(&_pIpmcDb[unit]->used_l3Entry_list, pEntry, nodeRef);

    return RT_ERR_OK;
}

static inline int32
_dal_longan_ipmc_l3EntryStatMont_remove(uint32 unit, dal_longan_ipmc_l3Entry_t *pEntry)
{
    uint32 l3EntryIdx, montIdx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* calculation of l3EntryIdx */
    l3EntryIdx = pEntry->hw_entry_idx;
    l3EntryIdx |= (TRUE == pEntry->cam)? (0x1 << 13) : 0;

    if (TRUE == pEntry->statMont_byte_valid)
    {
        /* confirm the relationship */
        montIdx = pEntry->statMont_byte_idx;
        if (l3EntryIdx == _pIpmcDb[unit]->mont[montIdx].l3EntryIdx)
        {
            _pIpmcDb[unit]->mont[montIdx].valid = FALSE;
            _pIpmcDb[unit]->mont[montIdx].mode = 0;
            _pIpmcDb[unit]->mont[montIdx].l3EntryIdx = 0;

            pEntry->statMont_byte_valid = FALSE;
            pEntry->statMont_byte_idx = 0;

            /* don't have to sync to H/W here (not necessary) */
        }
    }
    if (TRUE == pEntry->statMont_pkt_valid)
    {
        /* confirm the relationship */
        montIdx = pEntry->statMont_pkt_idx;
        if (l3EntryIdx == _pIpmcDb[unit]->mont[montIdx].l3EntryIdx)
        {
            _pIpmcDb[unit]->mont[montIdx].valid = FALSE;
            _pIpmcDb[unit]->mont[montIdx].mode = 0;
            _pIpmcDb[unit]->mont[montIdx].l3EntryIdx = 0;

            pEntry->statMont_pkt_valid = FALSE;
            pEntry->statMont_pkt_idx = 0;

            /* don't have to sync to H/W here (not necessary) */
        }
    }

    return RT_ERR_OK;
}

static inline int32
_dal_longan_ipmc_statKey2rtkIpmcAddr(rtk_ipmc_addr_t *pIpmcAddr, rtk_ipmc_statKey_t *pStatKey)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pStatKey), RT_ERR_NULL_POINTER);

    osal_memset(pIpmcAddr, 0, sizeof(rtk_ipmc_addr_t));
    pIpmcAddr->vrf_id = 0;
    if (pStatKey->flags & RTK_IPMC_FLAG_IPV6)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_IPV6;
        pIpmcAddr->src_ip_addr.ipv6 = pStatKey->src_ip_addr.ipv6;
        pIpmcAddr->dst_ip_addr.ipv6 = pStatKey->dst_ip_addr.ipv6;
    }
    else
    {
        pIpmcAddr->src_ip_addr.ipv4 = pStatKey->src_ip_addr.ipv4;
        pIpmcAddr->dst_ip_addr.ipv4 = pStatKey->dst_ip_addr.ipv4;
    }

    if (pStatKey->flags & RTK_IPMC_FLAG_SIP)
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;

    pIpmcAddr->src_intf.vlan_id = pStatKey->src_intf.vlan_id;

    return RT_ERR_OK;
}

static inline int32
_dal_longan_ipmc_statMont_sync(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    uint32  value;

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM), RT_ERR_OUT_OF_RANGE);

    value = _pIpmcDb[unit]->mont[index].mode;
    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_COUNTER_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, LONGAN_L3_ENTRY_CNTR_MODEf, value, "", errHandle, ret);

    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_COUNTER_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, LONGAN_L3_ENTRY_IDXf, _pIpmcDb[unit]->mont[index].l3EntryIdx, "", errHandle, ret);

errHandle:
    return ret;
}

static inline int32
_dal_longan_ipmc_statMont_reset(uint32 unit, uint32 index)
{
    int32 ret = RT_ERR_OK;
    uint32 value = 1;

    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_COUNTER_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, LONGAN_RESETf, value, "", errHandle, ret);

    /* check  status */
    do {
        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_ENTRY_COUNTER_CTRLr, REG_ARRAY_INDEX_NONE, \
            index, LONGAN_RESETf, value, "", errHandle, ret);
    } while (0 != value);

errHandle:
    return ret;
}


static inline int32
_dal_longan_ipmc_statMontCntr_get(uint32 unit, uint32 index, rtk_ipmc_statCntr_t *pCntr)
{
    int32   ret = RT_ERR_OK;
    uint32  value, cnt_hi, cnt_lo;

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    value = 1;
    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_ENTRY_COUNTER_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, LONGAN_CNTR_LATCHf, value, "", errHandle, ret);

    /* get high/low counter */
    IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_ENTRY_COUNTER_DATAr, REG_ARRAY_INDEX_NONE, \
        index, LONGAN_VAL_HIf, cnt_hi, "", errHandle, ret);
    IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, LONGAN_L3_ENTRY_COUNTER_DATAr, REG_ARRAY_INDEX_NONE, \
        index, LONGAN_VAL_LOf, cnt_lo, "", errHandle, ret);

    /* [FIXME] should consider overflow issue (polling by a thread?) */
    if (LONGAN_IPMC_STAT_MONT_MODE_BYTE_CNT == _pIpmcDb[unit]->mont[index].mode)
    {
        pCntr->rxByte = (((uint64)cnt_hi) << 32) | ((uint64)cnt_lo);
    }
    else
    {
        pCntr->rxPacket = (((uint64)cnt_hi) << 32) | ((uint64)cnt_lo);
    }

errHandle:
    return ret;
}


#if LONGAN_L3_ROUTE_IPMC_SIZE
int32
_dal_longan_ipmc_lpmRouteCnt_get(uint32 unit, uint32 *pIpv4Cnt, uint32 *pIpv6Cnt)
{
    int32 ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((pIpv4Cnt == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pIpv6Cnt == NULL), RT_ERR_NULL_POINTER);

    /* function body */
    *pIpv4Cnt = _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt;
    *pIpv6Cnt = _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt;

    LONGAN_IPMCAST_DBG_PRINTF(3, "*pIpv4Cnt = %d, *pIpv6Cnt = %d\n", *pIpv4Cnt, *pIpv6Cnt);

    return ret;
}
#endif
int32
_dal_longan_ipmc_l2PmskIdx_update(uint32 unit, uint32 l3EntryIdx, int32 l2PmskIdx)
{
    int32 ret = RT_ERR_OK;
    uint32 tbl, tfL2En, tfIdx, valIdx = 0, valL2En;
    dal_longan_ipmc_l3Entry_t *pEntry;
    dal_longan_l3_hostEntry_t hostEntry;
    dal_longan_l3_routeEntry_t routeEntry;

    /* parameter check */
    RT_PARAM_CHK((l3EntryIdx >= DAL_LONGAN_IPMCAST_L3_ENTRY_MAX), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    pEntry = L3_ENTRY_ADDR(unit, l3EntryIdx);
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        if (l2PmskIdx < 0)
        {
            pEntry->l2_pmsk_valid = FALSE;
            pEntry->l2_pmsk_idx = 0;
            valL2En = 0;
        }
        else
        {
            pEntry->l2_pmsk_valid = TRUE;
            pEntry->l2_pmsk_idx = l2PmskIdx;
            valL2En = 1;
            valIdx = (uint32)pEntry->l2_pmsk_idx;
        }

        /* sync to H/W */
        if (FALSE == pEntry->cam)
        {
            /* host table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = LONGAN_L3_HOST_ROUTE_IP6MCt;
                tfIdx = LONGAN_L3_HOST_ROUTE_IP6MC_L2_MC_PMSK_IDXtf;
                tfL2En = LONGAN_L3_HOST_ROUTE_IP6MC_L2_ENtf;
            }
            else
            {
                tbl = LONGAN_L3_HOST_ROUTE_IPMCt;
                tfIdx = LONGAN_L3_HOST_ROUTE_IPMC_L2_MC_PMSK_IDXtf;
                tfL2En = LONGAN_L3_HOST_ROUTE_IPMC_L2_ENtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfIdx, valIdx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL2En, valL2En, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, hostEntry, "", errHandle, ret);
        }
        else
        {
            /* route table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = LONGAN_L3_PREFIX_ROUTE_IP6MCt;
                tfIdx = LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_MC_PMSK_IDXtf;
                tfL2En = LONGAN_L3_PREFIX_ROUTE_IP6MC_L2_ENtf;
            }
            else
            {
                tbl = LONGAN_L3_PREFIX_ROUTE_IPMCt;
                tfIdx = LONGAN_L3_PREFIX_ROUTE_IPMC_L2_MC_PMSK_IDXtf;
                tfL2En = LONGAN_L3_PREFIX_ROUTE_IPMC_L2_ENtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfIdx, valIdx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL2En, valL2En, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, routeEntry, "", errHandle, ret);
        }
    }

errHandle:
    IPMCAST_SEM_UNLOCK(unit);
    return ret;
}

int32
_dal_longan_ipmc_stkPmsk_update(uint32 unit, uint32 l3EntryIdx, uint32 pmskVal)
{
    int32 ret = RT_ERR_OK;
    uint32 tbl, tfStk, tfL3Act, valL3Act;
    dal_longan_ipmc_l3Entry_t *pEntry;
    dal_longan_l3_hostEntry_t hostEntry;
    dal_longan_l3_routeEntry_t routeEntry;

    /* parameter check */
    RT_PARAM_CHK((l3EntryIdx >= DAL_LONGAN_IPMCAST_L3_ENTRY_MAX), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    pEntry = L3_ENTRY_ADDR(unit, l3EntryIdx);
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        /* sync to H/W */
        if (FALSE == pEntry->cam)
        {
            /* host table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = LONGAN_L3_HOST_ROUTE_IP6MCt;
                tfStk = LONGAN_L3_HOST_ROUTE_IP6MC_STACK_FWD_PMSKtf;
                tfL3Act = LONGAN_L3_HOST_ROUTE_IP6MC_L3_ACTtf;
            }
            else
            {
                tbl = LONGAN_L3_HOST_ROUTE_IPMCt;
                tfStk = LONGAN_L3_HOST_ROUTE_IPMC_STACK_FWD_PMSKtf;
                tfL3Act = LONGAN_L3_HOST_ROUTE_IPMC_L3_ACTtf;
            }

            IPMCAST_TABLE_FIELD_GET_ERR_HDL(unit, tbl, tfL3Act, valL3Act, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfStk, pmskVal, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, hostEntry, "", errHandle, ret);
        }
        else
        {
            /* route table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = LONGAN_L3_PREFIX_ROUTE_IP6MCt;
                tfStk = LONGAN_L3_PREFIX_ROUTE_IP6MC_STACK_FWD_PMSKtf;
            }
            else
            {
                tbl = LONGAN_L3_PREFIX_ROUTE_IPMCt;
                tfStk = LONGAN_L3_PREFIX_ROUTE_IPMC_STACK_FWD_PMSKtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfStk, pmskVal, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, routeEntry, "", errHandle, ret);
        }
    }

errHandle:
    IPMCAST_SEM_UNLOCK(unit);
    return ret;
}

int32
_dal_longan_ipmc_l3OilIdx_update(uint32 unit, uint32 l3EntryIdx, int32 l3OilIdx)
{
    int32 ret = RT_ERR_OK;
    uint32 tbl, tfIdx, tfL3Oilvalid, valIdx, valL3Oil;
    dal_longan_ipmc_l3Entry_t *pEntry;
    dal_longan_l3_hostEntry_t hostEntry;
    rtk_portmask_t  stkPmsk;
    dal_longan_l3_routeEntry_t routeEntry;

    /* parameter check */
    RT_PARAM_CHK((l3EntryIdx >= DAL_LONGAN_IPMCAST_L3_ENTRY_MAX), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);
    RTK_PORTMASK_RESET(stkPmsk);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    pEntry = L3_ENTRY_ADDR(unit, l3EntryIdx);
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        if (l3OilIdx < 0)
        {
            pEntry->l3_oil_valid = FALSE;
            pEntry->l3_oil_idx = 0;
            valL3Oil = 0;
            valIdx = 0;
        }
        else
        {
            pEntry->l3_oil_valid = TRUE;
            pEntry->l3_oil_idx = l3OilIdx;
            valL3Oil = 1;
            valIdx = (uint32)pEntry->l3_oil_idx;
        }

        /* sync to H/W */
        if (FALSE == pEntry->cam)
        {
            /* host table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = LONGAN_L3_HOST_ROUTE_IP6MCt;
                tfL3Oilvalid = LONGAN_L3_HOST_ROUTE_IP6MC_OIL_IDX_VALIDtf;
                tfIdx = LONGAN_L3_HOST_ROUTE_IP6MC_OIL_IDXtf;
            }
            else
            {
                tbl = LONGAN_L3_HOST_ROUTE_IPMCt;
                tfL3Oilvalid = LONGAN_L3_HOST_ROUTE_IPMC_OIL_IDX_VALIDtf;
                tfIdx = LONGAN_L3_HOST_ROUTE_IPMC_OIL_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL3Oilvalid, valL3Oil, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfIdx, valIdx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, hostEntry, "", errHandle, ret);
        }
        else
        {
            /* route table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = LONGAN_L3_PREFIX_ROUTE_IP6MCt;
                tfL3Oilvalid = LONGAN_L3_PREFIX_ROUTE_IP6MC_OIL_IDX_VALIDtf;
                tfIdx = LONGAN_L3_PREFIX_ROUTE_IP6MC_OIL_IDXtf;
            }
            else
            {
                tbl = LONGAN_L3_PREFIX_ROUTE_IPMCt;
                tfL3Oilvalid = LONGAN_L3_PREFIX_ROUTE_IPMC_OIL_IDX_VALIDtf;
                tfIdx = LONGAN_L3_PREFIX_ROUTE_IPMC_OIL_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL3Oilvalid, valL3Oil, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfIdx, valIdx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, pEntry->hw_entry_idx, routeEntry, "", errHandle, ret);
        }
    }

errHandle:
    IPMCAST_SEM_UNLOCK(unit);
    return ret;
}


/* SS-432 */
int32
_dal_longan_ipmc_entry_init(uint32 unit)
{

    int32 ret;
    host_routing_entry_t entry;
    rtk_ipmc_addr_t ipMcAddr;
    rtk_l2_mcastAddr_t l2McAddr;
    rtk_mcast_group_t group = LONGAN_IPMC_RSVD_GROUP;
    int32 base;
    uint32 val;

    RT_ERR_HDL(dal_longan_mcast_group_create(unit, RTK_MCAST_FLAG_WITH_ID, RTK_MCAST_TYPE_IP, &group), errHandle, ret);
    osal_memset(&ipMcAddr, 0, sizeof(ipMcAddr));
    ipMcAddr.src_intf.vlan_id = LONGAN_IPMC_RSVD_VID;
    ipMcAddr.ipmc_flags = RTK_IPMC_FLAG_SIP;
    ipMcAddr.dst_ip_addr.ipv4 = LONGAN_IPMC_RSVD_DIP;
    ipMcAddr.src_ip_addr.ipv4 = LONGAN_IPMC_RSVD_SIP;
    ipMcAddr.group = group;
    ipMcAddr.ttl_min = LONGAN_IPMC_RSVD_ENTRY_TTL;
    ipMcAddr.mtu_max = RTK_DEFAULT_L3_INTF_MTU;
    RT_ERR_HDL(dal_longan_ipmc_addr_add(unit, &ipMcAddr), errHandle, ret);

    osal_memset(&l2McAddr, 0, sizeof(l2McAddr));
    l2McAddr.rvid = LONGAN_IPMC_RSVD_VID;
    l2McAddr.mac.octet[0] = 0x01;
    l2McAddr.mac.octet[1] = 0x00;
    l2McAddr.mac.octet[2] = 0x5e;
    l2McAddr.mac.octet[3] = (LONGAN_IPMC_RSVD_DIP & 0x007f0000) >> 16;
    l2McAddr.mac.octet[4] = (LONGAN_IPMC_RSVD_DIP & 0x0000ff00) >> 8;
    l2McAddr.mac.octet[5] = (LONGAN_IPMC_RSVD_DIP & 0x000000ff) >> 0;
    l2McAddr.portmask.bits[0] = 0x0;
    RT_ERR_HDL(dal_longan_l2_mcastAddr_add(unit, &l2McAddr), errHandle, ret);

    base = -1;
    RT_ERR_HDL(dal_longan_ipmc_addr_getNext(unit, RTK_IPMC_FLAG_NONE, &base, &ipMcAddr), errHandle, ret);
    if (base >= 0)
    {
        if (!((ipMcAddr.dst_ip_addr.ipv4 == LONGAN_IPMC_RSVD_DIP) &&
             (ipMcAddr.src_ip_addr.ipv4 == LONGAN_IPMC_RSVD_SIP) &&
             (ipMcAddr.src_intf.vlan_id == LONGAN_IPMC_RSVD_VID)))
            return RT_ERR_ENTRY_NOTFOUND;

        RT_ERR_HDL(table_read(unit, LONGAN_L3_HOST_ROUTE_IPMCt, base, (uint32 *)&entry), errHandle, ret);
        val = 1; /* enable */
        RT_ERR_HDL(table_field_set(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_L2_ENtf, &val, (uint32 *)&entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_L3_ENtf, &val, (uint32 *)&entry), errHandle, ret);
        val = 0x3; /* action drop, pmsk 24,25 */
        RT_ERR_HDL(table_field_set(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_STACK_FWD_PMSKtf, &val, (uint32 *)&entry), errHandle, ret);
        RT_ERR_HDL(table_field_set(unit, LONGAN_L3_HOST_ROUTE_IPMCt, LONGAN_L3_HOST_ROUTE_IPMC_L2_ACTtf, &val, (uint32 *)&entry), errHandle, ret);
        RT_ERR_HDL(table_write(unit, LONGAN_L3_HOST_ROUTE_IPMCt, base, (uint32 *)&entry), errHandle, ret);

        ipmc_rsv_index = base;
    }
    else
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
    }

errHandle:
    return ret;
}


static int32
_dal_longan_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 *pArg)
{
    int32 ret;
    uint32 val;
    rtk_action_t act;

     /* parameter check */
    RT_PARAM_CHK((type >= RTK_IPMC_GCT_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    switch (type)
    {

    case RTK_IPMC_GCT_IPMC_GLB_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IPMC_BAD_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_ZERO_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlDmacMismatch, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_HDR_OPT_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_HDR_OPT_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlHdrOpt, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_SRC_VLAN_FLTR_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IPMC_MTU_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_TTL_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_TTL_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlTtlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_optIpmcRouteCtrlPktToCPUTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

   case  RTK_IPMC_GCT_IPMC_LU_MIS_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_LU_MIS_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteLuMis, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    /*IPV6*/
    case RTK_IPMC_GCT_IP6MC_GLB_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_BAD_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_ZERO_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlDmacMismatch, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HBH_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHbh, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ERR_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HBH_ERR_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHbhErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HDR_ROUTE_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HDR_ROUTE_ACTf, val, "", errExit, ret);\
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHdrRoute, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_SRC_VLAN_FLTR_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_MTU_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HL_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HL_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_optIp6mcRouteCtrlPktToCPUTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

     case  RTK_IPMC_GCT_IP6MC_LU_MIS_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_LU_MIS_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteLuMis, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
    }

errExit:
    return ret;
}



static int32
_dal_longan_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_IPMC_GCT_END), RT_ERR_OUT_OF_RANGE);

    switch (type)
    {
    case RTK_IPMC_GCT_IPMC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_BAD_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlBadSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_ZERO_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlZeroSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlDmacMismatch, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_HDR_OPT_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlHdrOpt, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_HDR_OPT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_SRC_VLAN_FLTR_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_MTU_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlMtuFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_TTL_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlTtlFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_TTL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_ACTION_TO_VALUE(_optIpmcRouteCtrlPktToCPUTarget, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

    case  RTK_IPMC_GCT_IPMC_LU_MIS_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteLuMis, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IPMC_ROUTE_CTRLr, LONGAN_LU_MIS_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_GLB_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_BAD_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlBadSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_ZERO_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlZeroSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlDmacMismatch, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHbh, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HBH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ERR_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHbhErr, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HBH_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HDR_ROUTE_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHdrRoute, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HDR_ROUTE_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_SRC_VLAN_FLTR_ENf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_MTU_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlMtuFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HL_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHlFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_HL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_ACTION_TO_VALUE(_optIp6mcRouteCtrlPktToCPUTarget, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

     case  RTK_IPMC_GCT_IP6MC_LU_MIS_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteLuMis, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_IP6MC_ROUTE_CTRLr, LONGAN_LU_MIS_ACTf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
    }

errExit:
    return ret;
}



/* Function Name:
 *      _dal_longan_ipmc_pkt_tx
 * Description:
 *      when L3 host table busy loop, CPU tx ipmc packet to exit busy loop
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
int32 _dal_longan_ipmc_pkt_tx(uint32 unit)
{
    int32   ret = RT_ERR_OK;

#if defined(CONFIG_SDK_DRIVER_NIC)
    drv_nic_pkt_t *pPacket;
    uint32  len = sizeof(ipmcPkt);
    int32   glob_enable[2];
    rtk_enable_t profile_enable[2];
    uint32  profile_idx[2];
    rtk_vlan_profile_t vlanProfile[2];
    rtk_vlan_ifilter_t   ifilter;
    rtk_vlan_t  vid = LONGAN_IPMC_RSVD_VID;
    uint32  unitId;
    uint32  mstUnitId = HWP_MY_UNIT_ID();
    uint32  slvUnitId = HWP_CASCADE_SLAVE_UNIT_ID();

    if (HWP_CASCADE_MODE())
        RT_PARAM_CHK((unit != mstUnitId && unit != slvUnitId), RT_ERR_UNIT_ID);
    else
        RT_PARAM_CHK((unit != mstUnitId), RT_ERR_UNIT_ID);

    osal_memset(glob_enable, 0, sizeof(glob_enable));
    osal_memset(profile_enable, 0, sizeof(profile_enable));
    osal_memset(profile_idx, 0, sizeof(profile_idx));
    osal_memset(vlanProfile, 0, sizeof(vlanProfile));

    if (RT_ERR_OK != drv_nic_pkt_alloc(mstUnitId, len, 0, &pPacket))
    {
        osal_printf("[%s]: Alloc packet failed.\n", __FUNCTION__);
        return RT_ERR_FAILED;
    }

    //enable ipmcast route
    RT_ERR_HDL(dal_longan_vlan_portIgrFilter_get(mstUnitId, HWP_CPU_MACID(mstUnitId), &ifilter), errHandle, ret);
    for (unitId = 0; unitId < 2; unitId++)
    {
        if ((mstUnitId == unit) && (slvUnitId == unitId))
            break;

        RT_ERR_HDL(_dal_longan_ipmc_globalCtrl_get(unitId, RTK_IPMC_GCT_IPMC_GLB_EN, &glob_enable[unitId]), errHandle, ret);
        RT_ERR_HDL(_dal_longan_vlan_profileIdx_get(unitId, vid, &profile_idx[unitId]), errHandle, ret);
        RT_ERR_HDL(dal_longan_vlan_profile_get(unitId, profile_idx[unitId], &vlanProfile[unitId]), errHandle, ret);
        profile_enable[unitId] = vlanProfile[unitId].ip4_mRoute;

        vlanProfile[unitId].ip4_mRoute = ENABLED;
        RT_ERR_HDL(dal_longan_vlan_profile_set(unitId, profile_idx[unitId], &vlanProfile[unitId]), errHandle, ret);
        RT_ERR_HDL(_dal_longan_ipmc_globalCtrl_set(unitId, RTK_IPMC_GCT_IPMC_GLB_EN, ENABLED), errHandle, ret);
    }
    RT_ERR_HDL(dal_longan_vlan_portIgrFilter_set(mstUnitId, HWP_CPU_MACID(mstUnitId), INGRESS_FILTER_FWD), errHandle, ret);

    osal_memcpy(pPacket->head, ipmcPkt, len);

    pPacket->length = len;
    pPacket->data = pPacket->head;
    pPacket->tail = pPacket->data + pPacket->length;
    pPacket->end = pPacket->tail;
    pPacket->as_txtag               = TRUE;
    pPacket->tx_tag.fwd_type        = NIC_FWD_TYPE_ALE;

    pPacket->tx_tag.as_priority   = 0x1;
    pPacket->tx_tag.priority    = 31;
    pPacket->tx_tag.fvid_en    = 1;
    pPacket->tx_tag.fvid  =     vid;
    pPacket->tx_tag.l3_act      = 0x1;
    pPacket->tx_tag.bp_fltr     = 0x1;
    pPacket->tx_tag.bp_stp      = 0x1;
    pPacket->tx_tag.bp_vlan_egr = 0x1;

    ret = drv_nic_pkt_tx(mstUnitId, pPacket, NULL, NULL);
    if (ret)
    {
        drv_nic_pkt_free(mstUnitId, pPacket);
        goto errHandle;
    }

errHandle:
    for (unitId = 0; unitId < 2; unitId++)
    {
        if ((mstUnitId == unit) && (slvUnitId == unitId))
            break;

        vlanProfile[unitId].ip4_mRoute = profile_enable[unitId];
        RT_ERR_HDL(dal_longan_vlan_profile_set(unitId, profile_idx[unitId], &vlanProfile[unitId]), errHandle, ret);
        RT_ERR_HDL(_dal_longan_ipmc_globalCtrl_set(unitId, RTK_IPMC_GCT_IPMC_GLB_EN, glob_enable[unitId]), errHandle, ret);
    }
    RT_ERR_HDL(dal_longan_vlan_portIgrFilter_set(mstUnitId, HWP_CPU_MACID(mstUnitId), ifilter), errHandle, ret);
#endif

    return ret;

}

#if LONGAN_L3_ROUTE_IPMC_SIZE
/*
 * table map:
 *
 *                       +----------+
 *         idx_base ---> | B + 0    | <-- ipv4_top (the first posion)
 *                       | B + 1    |
 *                       | B + 2    |
 *                       | . . .    |
 *                       |          | <-- ipv4_bottom = B+(2*n), n=ipv4_cnt
 *                       | . . .    |
 *                       |          | <-- ipv6_top = (B+size)-(3*n), n=ipv6_cnt
 *                       | . . .    |
 *                       |          |
 * (idx_base + size) --> +----------+ <-- ipv6_bottom (unable to use)
 *
 */
static inline int32 _dal_longan_ipmc_l3RouteEntry_alloc(uint32 unit, uint32 ipv6, uint32 *pIndex, dal_longan_ipmc_l3Entry_t *pEntry)
{
    uint32  ipv4_size;
    uint32  ipv6_size;
    uint32  free_size;
    uint32  offset_idx;     /* 0 ~ (LONGAN_L3_ROUTE_IPMC_SIZE - 1) */

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    ipv4_size = (_pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt * LONGAN_L3_ROUTE_IPMC_WIDTH_IPV4);
    ipv6_size = (_pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt * LONGAN_L3_ROUTE_IPMC_WIDTH_IPV6);
    free_size = (_pIpmcDb[unit]->l3_route_ipmc_size - ipv4_size - ipv6_size);

    if (ipv6)
    {
        /* IPv6 */
        if (free_size < LONGAN_L3_ROUTE_IPMC_WIDTH_IPV6)
        {
            return RT_ERR_TBL_FULL;
        }

        offset_idx = (_pIpmcDb[unit]->l3_route_ipmc_size) - ipv6_size - LONGAN_L3_ROUTE_IPMC_WIDTH_IPV6;

        _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset = offset_idx;
        _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt += 1;

        LONGAN_IPMCAST_DBG_PRINTF(3, "[ALLOC] ipv6_top_offset = %d, ipv6_cnt = %d, offset_idx = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt, offset_idx);
    } else {
        /* IPv4 */
        if (free_size < LONGAN_L3_ROUTE_IPMC_WIDTH_IPV4)
        {
            return RT_ERR_TBL_FULL;
        }

        offset_idx = (ipv4_size);

        _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset = offset_idx;
        _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt += 1;

        LONGAN_IPMCAST_DBG_PRINTF(3, "[ALLOC] ipv4_bottom_offset = %d, ipv4_cnt = %d, offset_idx = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt, offset_idx);
    }

    _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = pEntry;
        LONGAN_IPMCAST_DBG_PRINTF(3, "[FREE] offset_idx = %d, %p,%p\n", offset_idx, _pIpmcDb[unit]->pIpmcL3Entry[offset_idx], pEntry);

    /* entry index */
    *pIndex = (_pIpmcDb[unit]->l3_route_ipmc_idx_base + offset_idx);

    LONGAN_IPMCAST_DBG_PRINTF(3, "[ALLOC] *pIndex = %d\n", (*pIndex));

    return RT_ERR_OK;
}


static inline int32 _dal_longan_ipmc_l3RouteEntry_free(uint32 unit, dal_longan_ipmc_l3Entry_t    *pEntry)
{
    int32   ret = RT_ERR_OK;
    uint32 offset_idx;
    dal_longan_ipmc_l3Entry_t    *pEntry1;
    uint32 base_hw_idx;
    uint32 dst_hw_idx;
    uint32 src_hw_idx;
    dal_longan_l3_routeEntry_t   routeEntry;
    uint32 index = 0;

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    index = pEntry->hw_entry_idx;
    RT_PARAM_CHK((index < (_pIpmcDb[unit]->l3_route_ipmc_idx_base)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((index >= (_pIpmcDb[unit]->l3_route_ipmc_idx_base + _pIpmcDb[unit]->l3_route_ipmc_size)), RT_ERR_OUT_OF_RANGE);


    base_hw_idx = _pIpmcDb[unit]->l3_route_ipmc_idx_base;

    offset_idx = (index - _pIpmcDb[unit]->l3_route_ipmc_idx_base);
    pEntry1 = _pIpmcDb[unit]->pIpmcL3Entry[offset_idx];

    if ((0 == pEntry) || (0 == pEntry->cam))
    {
        return RT_ERR_INPUT;
    }

    if (pEntry->ipv6)
    {
        /* IPv6 */

        /* move the top entry to this place if need */
        dst_hw_idx = pEntry->hw_entry_idx;
        src_hw_idx = (_pIpmcDb[unit]->l3_route_ipmc_idx_base + _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset);
        if (dst_hw_idx != src_hw_idx)
        {
            RT_ERR_HDL(_dal_longan_l3_routeEntry_get(unit, src_hw_idx, &routeEntry, DISABLED), errHandle, ret);
            RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

            /* updated hw_entry_idx of the moved entry */
            _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)]->hw_entry_idx = dst_hw_idx;

            _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)];

            L3_HW_IDX_VALID(unit, dst_hw_idx + DAL_LONGAN_L3_HOST_TBL_SIZE, L3_ENTRY_ADDR_TO_IDX(unit, _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)]));

            offset_idx = src_hw_idx - base_hw_idx;

            pEntry->hw_entry_idx = src_hw_idx;

            dst_hw_idx = src_hw_idx;
        }

        RT_ERR_HDL(_dal_longan_ipmc_emptyL3RouteEntry_build(&routeEntry, RTK_IPMC_FLAG_IPV6), errHandle, ret);
        RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

        _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset += LONGAN_L3_ROUTE_IPMC_WIDTH_IPV6;
        _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt -= 1;

        LONGAN_IPMCAST_DBG_PRINTF(3, "[FREE] ipv6_top_offset = %d, ipv6_cnt = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt);
    } else {
        /* IPv4 */

        /* move the bottom entry to this place if need */
        dst_hw_idx = pEntry->hw_entry_idx;
        src_hw_idx = (_pIpmcDb[unit]->l3_route_ipmc_idx_base + _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset);
        LONGAN_IPMCAST_DBG_PRINTF(3, "[FREE] dst_hw_idx = %d, src_hw_idx = %d\n", dst_hw_idx, src_hw_idx);
        if (dst_hw_idx != src_hw_idx)
        {
            RT_ERR_HDL(_dal_longan_l3_routeEntry_get(unit, src_hw_idx, &routeEntry, DISABLED), errHandle, ret);
            RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

            /* updated hw_entry_idx of the moved entry */
            _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)]->hw_entry_idx = dst_hw_idx;

            _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)];

            L3_HW_IDX_VALID(unit, dst_hw_idx + DAL_LONGAN_L3_HOST_TBL_SIZE, L3_ENTRY_ADDR_TO_IDX(unit, _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)]));
            offset_idx = src_hw_idx - base_hw_idx;

            pEntry->hw_entry_idx = src_hw_idx;
            dst_hw_idx = src_hw_idx;
        LONGAN_IPMCAST_DBG_PRINTF(3, "[FREE] dst_hw_idx = %d, src_hw_idx = %d new hwindex %d\n", dst_hw_idx, src_hw_idx,pEntry->hw_entry_idx);
        }

        RT_ERR_HDL(_dal_longan_ipmc_emptyL3RouteEntry_build(&routeEntry, RTK_IPMC_FLAG_NONE), errHandle, ret);
        RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

        _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset -= LONGAN_L3_ROUTE_IPMC_WIDTH_IPV4;
        _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt -= 1;

        LONGAN_IPMCAST_DBG_PRINTF(3, "[FREE] ipv4_bottom_offset = %d, ipv4_cnt = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt);
    }

    /* release entry point */
    _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = 0;   // NULL

errHandle:
    return ret;
}
#endif

/* Function Name:
 *      dal_longan_ipmcMapper_init
 * Description:
 *      Hook ipmc module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook ipmc module before calling any ipmc APIs.
 */
int32
dal_longan_ipmcMapper_init(dal_mapper_t *pMapper)
{
    pMapper->ipmc_init = dal_longan_ipmc_init;
    pMapper->ipmc_addr_add = dal_longan_ipmc_addr_add;
    pMapper->ipmc_addr_find = dal_longan_ipmc_addr_find;
    pMapper->ipmc_addr_del = dal_longan_ipmc_addr_del;
    pMapper->ipmc_addr_delAll = dal_longan_ipmc_addr_delAll;
    pMapper->ipmc_nextValidAddr_get = dal_longan_ipmc_addr_getNext;
    pMapper->ipmc_statMont_create = dal_longan_ipmc_statMont_create;
    pMapper->ipmc_statMont_destroy = dal_longan_ipmc_statMont_destroy;
    pMapper->ipmc_statCntr_reset = dal_longan_ipmc_statCntr_reset;
    pMapper->ipmc_statCntr_get = dal_longan_ipmc_statCntr_get;
    pMapper->ipmc_globalCtrl_set = dal_longan_ipmc_globalCtrl_set;
    pMapper->ipmc_globalCtrl_get = dal_longan_ipmc_globalCtrl_get;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_ipmc_init
 * Description:
 *      Initialize ipmcast module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize ipmcast module before calling any ipmcast APIs.
 */
int32
dal_longan_ipmc_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    dal_longan_ipmc_l3Entry_t *pEntry;
    uint32 host_ipmc_table0_alg, host_ipmc_table1_alg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(ipmc_init[unit]);
    ipmc_init[unit] = INIT_NOT_COMPLETED;

    /* should check L2/L3 modele init finished; */

    /* create semaphore */
    ipmc_sem[unit] = osal_sem_mutex_create();
    if (0 == ipmc_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* allocate memory that we need */
    if ((_pIpmcDb[unit] = osal_alloc(sizeof(dal_longan_ipmc_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "out of memory");
        return RT_ERR_FAILED;
    }

    /* initialize drvDb */
    osal_memset(_pIpmcDb[unit], 0x00, sizeof(dal_longan_ipmc_drvDb_t));

    if ((pIpmcAddrIdxMap[unit] = osal_alloc(sizeof(ipmc_addrIdx_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "out of memory");
        return RT_ERR_FAILED;
    }
    osal_memset(pIpmcAddrIdxMap[unit], 0x00, sizeof(ipmc_addrIdx_t));

    /* used/free l3Entry list initialize */
    RTK_LIST_INIT(&_pIpmcDb[unit]->used_l3Entry_list);
    RTK_LIST_INIT(&_pIpmcDb[unit]->free_l3Entry_list);
    for (idx=0; idx<DAL_LONGAN_IPMCAST_L3_ENTRY_MAX; idx++)
    {
        pEntry = L3_ENTRY_ADDR(unit, idx);
        L3_ENTRY_RESET(pEntry);
        RTK_LIST_NODE_REF_INIT(pEntry, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pIpmcDb[unit]->free_l3Entry_list, pEntry, nodeRef);
    }

    /* set init flag to complete init */
    ipmc_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {

        /* L3 table lookup mode and hash key */
        _pIpmcDb[unit]->l3TblLookupMode = DAL_LONGAN_IPMC_L3TBL_LOOKUP_MODE_FORCE;

        IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_LU_MODE_SELf, \
            _pIpmcDb[unit]->l3TblLookupMode, "", errHandle, ret);

        _pIpmcDb[unit]->l3TblHashKey = IPMCAST_HASH_KEY;

        /*if HASH_KEY_SEL = 1, the lookupMode should be MODE_FORCE*/
        IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_LU_FORCE_MODE_HASH_KEY_SELf, \
            _pIpmcDb[unit]->l3TblHashKey, "", errHandle, ret);

        /* configure default hash algorithm */
        host_ipmc_table0_alg = IPMCAST_TABLE0_ALG;
        host_ipmc_table1_alg = IPMCAST_TABLE1_ALG;
        IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_MC_HASH_ALG_SEL_0f, host_ipmc_table0_alg, "", errHandle, ret);
        IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, LONGAN_L3_HOST_TBL_CTRLr, LONGAN_MC_HASH_ALG_SEL_1f, host_ipmc_table1_alg, "", errHandle, ret);

        /* SS-432 */
        if (CHIP_REV_ID_A == HWP_CHIP_REV(unit))
        {
            RT_ERR_HDL(_dal_longan_ipmc_entry_init(unit), errHandle, ret);
        }
    }

#if LONGAN_L3_ROUTE_IPMC_SIZE
    _dal_longan_l3_route_ipmcSize_get(unit, &(_pIpmcDb[unit]->l3_route_ipmc_idx_base), &(_pIpmcDb[unit]->l3_route_ipmc_size));
#endif

errHandle:
    return ret;
}   /* end of dal_longan_l3mc_init */


/* Function Name:
 *      dal_longan_ipmc_addr_add
 * Description:
 *      Add an ipmcast entry into the L3 host or route table
 * Input:
 *      unit  - unit id
 *      pMcastAddr - pointer to rtk_ipmc_addr_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_IPMC_ADDR          - invalid multicast ip address
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6), and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *          RTK_IPMC_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_IPMC_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                        (path ID will not be referred)
 *          RTK_IPMC_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_IPMC_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_IPMC_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_IPMC_FLAG_SIP         - entry is dip + sip
 */
int32
dal_longan_ipmc_addr_add(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    int32   l3EntryAllocated = FALSE;
    int32   hostEntryAllocated = FALSE;
    uint32  signKey;
    uint32  hostIdx;
    dal_longan_ipmc_hashIdx_t hashIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;
    dal_longan_l3_hostAlloc_t hostAlloc;
    dal_longan_l3_hostEntry_t hostEntry;
    dal_longan_ipmc_mtu_info_t mtu;
    dal_longan_ipmc_enum_alloc_t allocMtu = DAL_LONGAN_IPMC_MTU_ALLOC;

#if LONGAN_L3_ROUTE_IPMC_SIZE
    int32   routeEntryAllocated = FALSE;
    uint32  routeIdx = 0;
    dal_longan_l3_routeEntry_t routeEntry;
#endif
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pIpmcAddr=%p", unit, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!IS_IPMC_ADDR(pIpmcAddr), RT_ERR_IPMC_ADDR);
    RT_PARAM_CHK((pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID), RT_ERR_INPUT);
    RT_PARAM_CHK((pIpmcAddr->src_intf.vlan_id > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((0 != pIpmcAddr->vrf_id), RT_ERR_INPUT);
    RT_PARAM_CHK((pIpmcAddr->mtu_max > HAL_MAX_NUM_OF_INTF_MTU_VALUE(unit)), RT_ERR_MTU_EXCEED);
    RT_PARAM_CHK((pIpmcAddr->ttl_min > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    osal_memset(&hostEntry, 0x0, sizeof(dal_longan_l3_hostEntry_t));

    /* function body */
    RT_ERR_CHK(_dal_longan_ipmc_hashIdx_get(unit, pIpmcAddr, &hashIdx), ret);
    signKey = _dal_longan_ipmc_signKey_ret(unit, pIpmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    /* bindable check */
    //RT_ERR_HDL(_dal_longan_mcast_ipmc_bindState_get(unit, pIpmcAddr->group, &groupBind, &entryIdx), errHandle, ret);

    /* try to find the L3 entry */
    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_find_withSignKey(unit, pIpmcAddr, &pEntry, signKey, FALSE), errHandle, ret);
    if (NULL != pEntry)
    {
        /* already exists */
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_REPLACE)
        {
            if(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
            {
                RT_ERR_HDL(_dal_longan_ipmc_Ip6mtu_get(unit, pEntry->hw_entry_idx, &mtu, pEntry->cam), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_longan_ipmc_mtu_get(unit, pEntry->hw_entry_idx, &mtu, pEntry->cam), errHandle, ret);
            }
            if (mtu.val != pIpmcAddr->mtu_max)
            {
                allocMtu = DAL_LONGAN_IPMC_MTU_REALLOC;
            }
            else
            {
                allocMtu = DAL_LONGAN_IPMC_MTU_NOT_ALLOC;
            }

            if (pEntry->groupId != pIpmcAddr->group)
            {
                #if 0
                /* check the new group if it's being used */
                if (TRUE == groupBind)
                {
                    /* the group is being used */
                    ret =  RT_ERR_ENTRY_REFERRED;
                    goto errHandle;
                }
                #endif
                /* unbind the old group */
                RT_ERR_HDL(_dal_longan_mcast_ipmc_unbind(unit, pEntry->groupId, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);
                if(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
                {
                    RT_ERR_HDL(_dal_longan_l3_ipmc_mtuIp6Entry_free(unit, mtu.idx, pEntry->groupId), errHandle, ret);
                }
                else
                {
                    RT_ERR_HDL(_dal_longan_l3_ipmc_mtuEntry_free(unit, mtu.idx, pEntry->groupId), errHandle, ret);
                }
            }

            if (FALSE == pEntry->cam)
            {
                hostEntry.mtu_max_idx = mtu.idx;
                RT_ERR_HDL(_dal_longan_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr, allocMtu), errHandle, ret);
                RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                    DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
            }
#if LONGAN_L3_ROUTE_IPMC_SIZE
            else
            {
                routeEntry.mtu_max_idx = mtu.idx;
                RT_ERR_HDL(_dal_longan_ipmc_rtkIpmcAddr2l3RouteEntry(unit, &routeEntry, pIpmcAddr, allocMtu), errHandle, ret);
                RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, \
                    DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
            }
#endif
            /* update entry info */
            pEntry->groupId = pIpmcAddr->group;
            /* call mcast API to update bind info */
            RT_ERR_HDL(_dal_longan_mcast_ipmc_bind(unit, pEntry->groupId, pIpmcAddr, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);
        }
        else
        {
            ret = RT_ERR_ENTRY_EXIST;
            goto errHandle;
        }
    }
    else
    {
#if 0
        if (TRUE == groupBind)
        {
           ret =  RT_ERR_ENTRY_REFERRED;
           goto errHandle;
        }
#endif
        /* alloc a new l3Entry */
        RT_ERR_HDL(_dal_longan_ipmc_l3Entry_alloc(unit, &pEntry), errHandle, ret);
        l3EntryAllocated = TRUE;

        pEntry->ipv6 = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)? TRUE : FALSE;

        /* try to allocate an empty L3 host entry (SRAM) */
        RT_ERR_HDL(_dal_longan_ipmc_l3HostAlloc_get(unit, pIpmcAddr, &hashIdx, &hostAlloc), errHandle, ret);
#if LONGAN_L3_ROUTE_IPMC_SIZE
        ret = _dal_longan_l3_hostEntry_alloc(unit, &hostAlloc, &hostIdx, DAL_LONGAN_L3_API_FLAG_NONE);
        if (RT_ERR_OK == ret)
        {
            pEntry->cam = FALSE;
            hostEntryAllocated = TRUE;
            pEntry->hw_entry_idx = hostIdx;

            /* write into L3 Host table here */
            RT_ERR_HDL(_dal_longan_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr, allocMtu), errHandle, ret);
            RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
        }
        else
        {
            LONGAN_IPMCAST_DBG_PRINTF(3, "Add in tcam\n");
            /* try to allocate an emptry L3 Route entry (TCAM) */
            RT_ERR_HDL(_dal_longan_ipmc_l3RouteEntry_alloc(unit, pEntry->ipv6, &routeIdx, pEntry), errHandle, ret);
            pEntry->cam = TRUE;
            pEntry->hw_entry_idx = routeIdx;

            routeEntryAllocated = TRUE;


            /* write into L3 Route table here */
            RT_ERR_HDL(_dal_longan_ipmc_rtkIpmcAddr2l3RouteEntry(unit, &routeEntry, pIpmcAddr, allocMtu), errHandle, ret);
            RT_ERR_HDL(_dal_longan_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, \
                DISABLED), errHandle, ret);
        }
#else
        RT_ERR_HDL(_dal_longan_l3_hostEntry_alloc(unit, &hostAlloc, &hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
        pEntry->cam = FALSE;
        hostEntryAllocated = TRUE;
        pEntry->hw_entry_idx = hostIdx;

        /* write into L3 Host table here */
        RT_ERR_HDL(_dal_longan_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr, allocMtu), errHandle, ret);
        RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
#endif
        /* update entry info */
        pEntry->groupId = pIpmcAddr->group;
        /* check the goup bind first, call mcast API to update bind info */
        RT_ERR_HDL(_dal_longan_mcast_ipmc_bind(unit, pEntry->groupId, pIpmcAddr, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);

        /* insert the l3Entry into used_l3Entry_list */
        pEntry->signKey = signKey;

        RT_ERR_HDL(_dal_longan_ipmc_l3EntryUsedList_insert(unit, pEntry), errHandle, ret);
    }

    pEntry->ipmc_entry = *pIpmcAddr;

    goto errOk;

errHandle:
#if LONGAN_L3_ROUTE_IPMC_SIZE
    if (TRUE == routeEntryAllocated)
        IPMCAST_RT_ERR_HDL_DBG(_dal_longan_ipmc_l3RouteEntry_free(unit, pEntry), "");
#endif
    if (TRUE == hostEntryAllocated)
        IPMCAST_RT_ERR_HDL_DBG(_dal_longan_l3_hostEntry_free(unit, hostIdx, DAL_LONGAN_L3_API_FLAG_NONE), "");

    if (TRUE == l3EntryAllocated)
        IPMCAST_RT_ERR_HDL_DBG(_dal_longan_ipmc_l3Entry_free(unit, pEntry), "");

errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_longan_ipmc_addr_add */


/* Function Name:
 *      dal_longan_ipmc_find
 * Description:
 *      Find an ipmcast entry based on IP address
 * Input:
 *      unit  - unit id
 *      pMcastAddr - pointer to the structure containing the basic inputs
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
 *      (1) Basic required input parameters of the pMcastAddras input keys:
 *          vrf_id and dst_ip_addr or with src_ip_addr (either ipv4 or ipv6).
 *          RTK_IPMC_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 *          RTK_IPMC_FLAG_READ_HIT_IGNORE - get entry ignore the hit flag
 *           and get entry from software database
 */
int32
dal_longan_ipmc_addr_find(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    dal_longan_ipmc_hashIdx_t hashIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pIpmcAddr=%p", unit, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!IS_IPMC_ADDR(pIpmcAddr), RT_ERR_IPMC_ADDR);
    RT_PARAM_CHK((0 != pIpmcAddr->vrf_id), RT_ERR_INPUT);
    RT_PARAM_CHK(((pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_READ_HIT_IGNORE) &&
                                (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_HIT_CLEAR)), RT_ERR_INPUT);

    /* function body */
    RT_ERR_CHK(_dal_longan_ipmc_hashIdx_get(unit, pIpmcAddr, &hashIdx), ret);
    signKey = _dal_longan_ipmc_signKey_ret(unit, pIpmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_find_withSignKey(unit, pIpmcAddr, &pEntry, signKey, TRUE), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* return information (groupId) and flags */
    pIpmcAddr->group = pEntry->groupId;
    pIpmcAddr->hw_entry_idx = pEntry->hw_entry_idx;

errHandle:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      dal_longan_ipmc_addr_del
 * Description:
 *      Delete an entry from the L3 host and route table
 * Input:
 *      unit  - unit id
 *      pMcastAddr - pointer to rtk_ipmc_addr_t containing the basic inputs
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
 *      (1) Basic required input parameters of the pMcastAddr as input keys:
 *          dst_ip_addr and src_ip_addr(either ipv4 or ipv6).
 */
int32
dal_longan_ipmc_addr_del(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    dal_longan_ipmc_hashIdx_t hashIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;
    dal_longan_l3_hostEntry_t hostEntry;
    dal_longan_ipmc_mtu_info_t mtu;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pIpmcAddr=%p", unit, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!IS_IPMC_ADDR(pIpmcAddr), RT_ERR_IPMC_ADDR);
    RT_PARAM_CHK((0 != pIpmcAddr->vrf_id), RT_ERR_INPUT);

    /* function body */
    RT_ERR_CHK(_dal_longan_ipmc_hashIdx_get(unit, pIpmcAddr, &hashIdx), ret);
    signKey = _dal_longan_ipmc_signKey_ret(unit, pIpmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    /* try to find the L3 entry */
    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_find_withSignKey(unit, pIpmcAddr, &pEntry, signKey, FALSE), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    if(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        RT_ERR_HDL(_dal_longan_ipmc_Ip6mtu_get(unit, pEntry->hw_entry_idx, &mtu, pEntry->cam), errHandle, ret);
    }
    else
    {
        RT_ERR_HDL(_dal_longan_ipmc_mtu_get(unit, pEntry->hw_entry_idx, &mtu, pEntry->cam), errHandle, ret);
    }

    if (FALSE == pEntry->cam)
    {
        RT_ERR_HDL(_dal_longan_ipmc_emptyL3HostEntry_build(&hostEntry, pIpmcAddr->ipmc_flags), errHandle, ret);
        RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

        RT_ERR_HDL(_dal_longan_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
            DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
    }
#if LONGAN_L3_ROUTE_IPMC_SIZE
    else
    {
        RT_ERR_HDL(_dal_longan_ipmc_l3RouteEntry_free(unit, pEntry), errHandle, ret);
    }
#endif
    /*remove mtu ref*/
    if(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        RT_ERR_HDL(_dal_longan_l3_ipmc_mtuIp6Entry_free(unit, mtu.idx, pEntry->groupId), errHandle, ret);
    }
    else
    {
        RT_ERR_HDL(_dal_longan_l3_ipmc_mtuEntry_free(unit, mtu.idx, pEntry->groupId), errHandle, ret);
    }


    /* remove the monitors if it has */
    RT_ERR_HDL(_dal_longan_ipmc_l3EntryStatMont_remove(unit, pEntry), errHandle, ret);
    /* unbind multicast group */
    RT_ERR_HDL(_dal_longan_mcast_ipmc_unbind(unit, pEntry->groupId, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);

    /* remove the l3Entry into used_l3Entry_list */
    RT_ERR_HDL(_dal_longan_ipmc_l3EntryUsedList_remove(unit, pEntry), errHandle, ret);
    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_free(unit, pEntry), errHandle, ret);

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_host_del */


/* Function Name:
 *      dal_longan_ipmc_addr_delAll
 * Description:
 *      Delete all ipmcast entry in host and route table entries
 * Input:
 *      unit  - unit id
 *      flag - configure flag
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
dal_longan_ipmc_addr_delAll(uint32 unit, rtk_ipmc_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    int32   ipv6;
    //uint32  l3EntryIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;
    dal_longan_ipmc_l3Entry_t *pTemp;
    dal_longan_l3_hostEntry_t hostEntry;
    dal_longan_ipmc_mtu_info_t mtu;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,flags=0x%08x", unit, flags);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* function body */
    ipv6 = (flags & RTK_IPMC_FLAG_IPV6)? TRUE : FALSE;

    RT_ERR_HDL(_dal_longan_ipmc_emptyL3HostEntry_build(&hostEntry, flags), errHandle, ret);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    RTK_LIST_FOREACH_SAFE(&_pIpmcDb[unit]->used_l3Entry_list, pEntry, pTemp, nodeRef)
    {
        if (pEntry->ipv6 == ipv6)
        {
            if(pEntry->ipv6)
            {
                RT_ERR_HDL(_dal_longan_ipmc_Ip6mtu_get(unit, pEntry->hw_entry_idx, &mtu, pEntry->cam), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_longan_ipmc_mtu_get(unit, pEntry->hw_entry_idx, &mtu, pEntry->cam), errHandle, ret);
            }

            if (FALSE == pEntry->cam)
            {
                RT_ERR_HDL(_dal_longan_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                    DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);

                RT_ERR_HDL(_dal_longan_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
                    DAL_LONGAN_L3_API_FLAG_NONE), errHandle, ret);
            }
#if LONGAN_L3_ROUTE_IPMC_SIZE
            else
            {
                RT_ERR_HDL(_dal_longan_ipmc_l3RouteEntry_free(unit, pEntry), errHandle, ret);
            }
#endif
            /*remove mtu ref*/
            if(pEntry->ipv6)
            {
                RT_ERR_HDL(_dal_longan_l3_ipmc_mtuIp6Entry_free(unit, mtu.idx, pEntry->groupId), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_longan_l3_ipmc_mtuEntry_free(unit, mtu.idx, pEntry->groupId), errHandle, ret);
            }

            /* remove the monitors if it has */
            RT_ERR_HDL(_dal_longan_ipmc_l3EntryStatMont_remove(unit, pEntry), errHandle, ret);
            /* unbind multicast group */
            RT_ERR_HDL(_dal_longan_mcast_ipmc_unbind(unit, pEntry->groupId, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);

            /* remove the l3Entry into used_l3Entry_list */
            RT_ERR_HDL(_dal_longan_ipmc_l3EntryUsedList_remove(unit, pEntry), errHandle, ret);
            RT_ERR_HDL(_dal_longan_ipmc_l3Entry_free(unit, pEntry), errHandle, ret);
        }
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_longan_ipmc_addr_delAll */

/* Function Name:
 *      dal_longan_ipmc_addr_getNext
 * Description:
 *      Get the next valid ipmcast  entry (based on the base index)
 * Input:
 *      unit  - unit id
 *      flags - control flags
 *      pBase - pointer to the starting valid entry number to be searched
 * Output:
 *      pBase - pointer to the index of the returned entry (-1 means not found)
 *      pMcastAddr - ipmcast entry (if found)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) Applicable flags:
 *          RTK_IPMC_FLAG_IPV6  - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_IPMC_FLAG_READ_HIT_IGNORE - get entry ignore the hit flag
 *          and get entry from software database
 *      (2) Use base index -1 to indicate to search from the beginging of L3 host table
 *      (3) If the returned index is -1, means not found the next valid entry
 */
int32
dal_longan_ipmc_addr_getNext(uint32 unit, rtk_ipmc_flag_t flags, int32 *pBase, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    int32   ipv6;
    int32   idx;
    dal_longan_ipmc_l3Entry_t    *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,flags=0x%08x,pBase=%p,pHost=%p", unit, flags, pBase, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_READ_HIT_IGNORE) &&
                                (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_HIT_CLEAR)), RT_ERR_INPUT);

    /* function body */
    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    ipv6 = (flags & RTK_IPMC_FLAG_IPV6) ? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (idx=(*pBase<0)?(0):((*pBase)+1); idx<DAL_LONGAN_L3_TBL_SIZE; idx++)
    {
        if (L3_HWENTRY_IDX_IS_VALID(unit, idx))
        {
            /*Get the reserved entry*/
            if (CHIP_REV_ID_A == HWP_CHIP_REV(unit))
            {
                if (ipmc_rsv_index == idx)
                {
                    osal_memset(pIpmcAddr, 0x00, sizeof(rtk_ipmc_addr_t));
                    pIpmcAddr->ipmc_flags = RTK_IPMC_FLAG_SIP;
                    pIpmcAddr->group = LONGAN_IPMC_RSVD_GROUP;
                    pIpmcAddr->src_ip_addr.ipv4 = LONGAN_IPMC_RSVD_SIP;
                    pIpmcAddr->dst_ip_addr.ipv4 = LONGAN_IPMC_RSVD_DIP;
                    pIpmcAddr->src_intf.vlan_id = LONGAN_IPMC_RSVD_VID;
                    pIpmcAddr->mtu_max = RTK_DEFAULT_L3_INTF_MTU;
                    pIpmcAddr->ttl_min =    LONGAN_IPMC_RSVD_ENTRY_TTL;
                    *pBase = idx;
                    goto errOk;
                }
            }

            pEntry = L3_HWENTRY_ADDR(unit, idx);

            if (ipv6 != pEntry->ipv6)
                continue;

            RT_ERR_HDL(_dal_longan_ipmc_rtkIpmcAddr_get(unit, flags, pIpmcAddr, pEntry), errHandle, ret);
            *pBase = idx;

            goto errOk;
        }
    }

    *pBase = -1;    /* Not found any specified entry */

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_ipmc_statMont_create
 * Description:
 *      Add an ipmc monitor counter entry for ipmcast entry
 * Input:
 *      unit  - unit id
 *      pStatMont - pointer to rtk_ipmc_statMont_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_TBL_FULL           - table is full
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 *          pStatMont->mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET wil use two counter entry
 */
int32
dal_longan_ipmc_statMont_create(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
{
    int32   ret = RT_ERR_OK;
    int32   pktMontExist = FALSE;
    int32   byteMontExist = FALSE;
    uint32  signKey;
    uint32  l3EntryAddr;
    uint32  idx;
    uint32  montReqNum = 0;
    uint32  montUseNum = 0;
    uint32  montFreeNum = 0;
    uint32  montFreeIdx[LONGAN_IPMC_STAT_MONT_ONETIME_MAX];
    uint32  montEntryIdx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_longan_ipmc_hashIdx_t hashIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatMont=%p", unit, pStatMont);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatMont), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pStatMont->key.vrf_id), RT_ERR_INPUT);

    /* function body */
    RT_ERR_CHK(_dal_longan_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, &pStatMont->key), ret);
    RT_ERR_CHK(_dal_longan_ipmc_hashIdx_get(unit,&ipmcAddr, &hashIdx), ret);
    signKey = _dal_longan_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey, FALSE), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryIdx */
    if (FALSE == pEntry->cam)
        l3EntryAddr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    else
        l3EntryAddr = (0x1 << DAL_LONGAN_IPMC_HOST_IDX_WIDTH) |pEntry->hw_entry_idx;

    /* estimate the required number of statMonitor */
    if (RTK_IPMCAST_STAT_MONT_FLAG_BYTE & pStatMont->mont_flags)
    {
        montReqNum += 1;
    }
    if (RTK_IPMCAST_STAT_MONT_FLAG_PACKET & pStatMont->mont_flags)
    {
        montReqNum += 1;
    }

    /* check current status */
    for (idx=0; idx<DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryIdx == l3EntryAddr)
            {
                if (LONGAN_IPMC_STAT_MONT_MODE_PKT_CNT == _pIpmcDb[unit]->mont[idx].mode)
                {
                    /* packet count */
                    pktMontExist = TRUE;
                    if (RTK_IPMCAST_STAT_MONT_FLAG_PACKET & pStatMont->mont_flags)
                    {
                        montReqNum -= 1;
                    }
                }
                else
                {
                    /* byte count */
                    byteMontExist = TRUE;
                    if (RTK_IPMCAST_STAT_MONT_FLAG_BYTE & pStatMont->mont_flags)
                    {
                        montReqNum -= 1;
                    }
                }
            }
        }
        else
        {
            if (montFreeNum < LONGAN_IPMC_STAT_MONT_ONETIME_MAX)
            {
                montFreeIdx[montFreeNum] = idx;
                montFreeNum++;
            }
        }
    }

    /* directly return if all required monitors are already existing */
    if (0 == montReqNum)
        goto errOk;

    /* check if have enough resource to create required entries */
    if (montFreeNum < montReqNum)
    {
        ret = RT_ERR_TBL_FULL;
        goto errHandle;
    }

    /* create byte monitor if required */
    if ((pStatMont->mont_flags & RTK_IPMCAST_STAT_MONT_FLAG_BYTE) && (FALSE == byteMontExist))
    {
        montEntryIdx = montFreeIdx[montUseNum];
        _pIpmcDb[unit]->mont[montEntryIdx].valid = TRUE;
        _pIpmcDb[unit]->mont[montEntryIdx].mode = LONGAN_IPMC_STAT_MONT_MODE_BYTE_CNT;
        _pIpmcDb[unit]->mont[montEntryIdx].l3EntryIdx = l3EntryAddr;
        montUseNum += 1;

        pEntry->statMont_byte_valid = TRUE;
        pEntry->statMont_byte_idx = montEntryIdx;

        /* sync to H/W, and reset the counter */
        RT_ERR_HDL(_dal_longan_ipmc_statMont_sync(unit, montEntryIdx), errHandle, ret);
        RT_ERR_HDL(_dal_longan_ipmc_statMont_reset(unit, montEntryIdx), errHandle, ret);
    }

    /* create packet monitor if required */
    if ((pStatMont->mont_flags & RTK_IPMCAST_STAT_MONT_FLAG_PACKET) && (FALSE == pktMontExist))
    {
        montEntryIdx = montFreeIdx[montUseNum];
        _pIpmcDb[unit]->mont[montEntryIdx].valid = TRUE;
        _pIpmcDb[unit]->mont[montEntryIdx].mode = LONGAN_IPMC_STAT_MONT_MODE_PKT_CNT;
        _pIpmcDb[unit]->mont[montEntryIdx].l3EntryIdx = l3EntryAddr;
        montUseNum += 1;

        pEntry->statMont_pkt_valid = TRUE;
        pEntry->statMont_pkt_idx = montFreeNum;

        /* sync to H/W, and reset the counter */
        RT_ERR_HDL(_dal_longan_ipmc_statMont_sync(unit, montEntryIdx), errHandle, ret);
        RT_ERR_HDL(_dal_longan_ipmc_statMont_reset(unit, montEntryIdx), errHandle, ret);
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      dal_longan_ipmc_statMont_destroy
 * Description:
 *      Delete an ipmc monitor counter entry for ipmcast entry
 * Input:
 *      unit  - unit id
 *      pStatMont - pointer to rtk_ipmc_statMont_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 *      RT_ERR_INPUT              - invalid input parameter

 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 *          pStatMont->mont_flags = 0,will also remove the counter entry
 *          delete an ipmcast entry,should destroy the monitor entry
 */
int32
dal_longan_ipmc_statMont_destroy(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    uint32  l3EntryAddr;
    uint32  idx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_longan_ipmc_hashIdx_t hashIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatMont=%p", unit, pStatMont);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatMont), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pStatMont->key.vrf_id), RT_ERR_INPUT);

    /* function body */
    RT_ERR_CHK(_dal_longan_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, &pStatMont->key), ret);
    RT_ERR_CHK(_dal_longan_ipmc_hashIdx_get(unit,&ipmcAddr, &hashIdx), ret);
    signKey = _dal_longan_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey, FALSE), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    if ((FALSE == pEntry->statMont_byte_valid) && (FALSE == pEntry->statMont_pkt_valid))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryIdx */
    if (FALSE == pEntry->cam)
        l3EntryAddr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    else
        l3EntryAddr = (0x1 << DAL_LONGAN_IPMC_HOST_IDX_WIDTH) |pEntry->hw_entry_idx;

    /* remove the existing monitor */
    for (idx=0; idx<DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryIdx == l3EntryAddr)
            {
                if (LONGAN_IPMC_STAT_MONT_MODE_PKT_CNT == _pIpmcDb[unit]->mont[idx].mode)
                {
                    /* packet count */
                    if (RTK_IPMCAST_STAT_MONT_FLAG_PACKET & pStatMont->mont_flags)
                    {
                        /* remove the monitor */
                        _pIpmcDb[unit]->mont[idx].valid = FALSE;
                        _pIpmcDb[unit]->mont[idx].mode = 0;
                        _pIpmcDb[unit]->mont[idx].l3EntryIdx = 0;

                        pEntry->statMont_pkt_valid = FALSE;
                        pEntry->statMont_pkt_idx = 0;

                         /* sync to H/W */
                        RT_ERR_HDL(_dal_longan_ipmc_statMont_sync(unit, idx), errHandle, ret);
                    }
                }
                else
                {
                    /* byte count */
                    if (RTK_IPMCAST_STAT_MONT_FLAG_BYTE & pStatMont->mont_flags)
                    {
                        /* remove the monitor */
                        _pIpmcDb[unit]->mont[idx].valid = FALSE;
                        _pIpmcDb[unit]->mont[idx].mode = 0;
                        _pIpmcDb[unit]->mont[idx].l3EntryIdx = 0;

                        pEntry->statMont_byte_valid = FALSE;
                        pEntry->statMont_byte_idx = 0;

                         /* sync to H/W */
                        RT_ERR_HDL(_dal_longan_ipmc_statMont_sync(unit, idx), errHandle, ret);
                    }
                }
            }
        }
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_ipmc_statCntr_reset
 * Description:
 *      reset the ipmc monitor counter
 * Input:
 *      unit  - unit id
 *      pKey - pointer to rtk_ipmc_statKey_t of ipmcast entry key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 * Note:
 *
 */
int32
dal_longan_ipmc_statCntr_reset(uint32 unit, rtk_ipmc_statKey_t *pStatKey)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    uint32  l3EntryAddr;
    uint32  idx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_longan_ipmc_hashIdx_t hashIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatKey=%p", unit, pStatKey);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pStatKey->vrf_id), RT_ERR_INPUT);

    /* function body */
    RT_ERR_CHK(_dal_longan_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, pStatKey), ret);
    RT_ERR_CHK(_dal_longan_ipmc_hashIdx_get(unit,&ipmcAddr, &hashIdx), ret);
    signKey = _dal_longan_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey, FALSE), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    if ((FALSE == pEntry->statMont_byte_valid) && (FALSE == pEntry->statMont_pkt_valid))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryIdx */
    if (FALSE == pEntry->cam)
        l3EntryAddr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    else
        l3EntryAddr = (0x1 << DAL_LONGAN_IPMC_HOST_IDX_WIDTH) |pEntry->hw_entry_idx;

    /* remove the existing monitor */
    for (idx=0; idx<DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryIdx == l3EntryAddr)
            {
                RT_ERR_HDL(_dal_longan_ipmc_statMont_reset(unit, idx), errHandle, ret);
            }
        }
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_longan_ipmc_statCntr_get
 * Description:
 *      reset the ipmc monitor counter
 * Input:
 *      unit  - unit id
 *      pKey - pointer to rtk_ipmc_statKey_t of ipmcast entry key
 * Output:
 *      pCntr - pointer to counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found (when try to replace)
 * Note:
 *
 */
int32
dal_longan_ipmc_statCntr_get(uint32 unit, rtk_ipmc_statKey_t *pStatKey, rtk_ipmc_statCntr_t *pCntr)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    uint32  l3EntryAddr;
    uint32  idx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_longan_ipmc_hashIdx_t hashIdx;
    dal_longan_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatKey=%p", unit, pStatKey);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pStatKey->vrf_id), RT_ERR_INPUT);

    osal_memset(pCntr, 0x00, sizeof(rtk_ipmc_statCntr_t));

    /* function body */
    RT_ERR_CHK(_dal_longan_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, pStatKey), ret);
    RT_ERR_CHK(_dal_longan_ipmc_hashIdx_get(unit,&ipmcAddr, &hashIdx), ret);
    signKey = _dal_longan_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey, FALSE), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    if ((FALSE == pEntry->statMont_byte_valid) && (FALSE == pEntry->statMont_pkt_valid))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryIdx */
    if (FALSE == pEntry->cam)
        l3EntryAddr = DAL_LONGAN_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    else
        l3EntryAddr = (0x1 << DAL_LONGAN_IPMC_HOST_IDX_WIDTH) |pEntry->hw_entry_idx;

    /* remove the existing monitor */
    for (idx=0; idx<DAL_LONGAN_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryIdx == l3EntryAddr)
            {
                RT_ERR_HDL(_dal_longan_ipmc_statMontCntr_get(unit, idx, pCntr), errHandle, ret);
            }
        }
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_longan_ipmc_globalCtrl_get
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
dal_longan_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d,pArg=%p", unit, type, pArg);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_IPMC_GCT_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_ipmc_globalCtrl_get(unit, type, pArg), errExit, ret);

errExit:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_globalCtrl_get */


/* Function Name:
 *      dal_longan_ipmc_globalCtrl_set
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
dal_longan_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,type=%d,arg=0x%08x", unit, type, arg);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_IPMC_GCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_longan_ipmc_globalCtrl_set(unit, type, arg), errExit, ret);

errExit:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_l3_globalCtrl_set */



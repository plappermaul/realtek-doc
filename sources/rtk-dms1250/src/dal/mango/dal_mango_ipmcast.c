/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public IP multicast APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) IP multicast routing
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_vlan.h>
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_ipmcast.h>
#include <dal/mango/dal_mango_mcast.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/ipmcast.h>

/*
 * Symbol Definition
 */
#define MANGO_IPMCAST_DBG                   (0)

#define MANGO_IPMC_HASH_IDX_ALG_NUM         (2)
#define MANGO_IPMC_STAT_MONT_ONETIME_MAX    (2)
#define MANGO_IPMC_STAT_MONT_MODE_PKT_CNT   (0)
#define MANGO_IPMC_STAT_MONT_MODE_BYTE_CNT  (1)

//#define DAL_MANGO_MCAST_GROUPID_GET(unit,idx)       (_pIpmcDb[unit]->ipmcast[idx].groupID)

typedef enum dal_mango_ipmc_l3TblLookupMode_e
{
    DAL_MANGO_IPMC_L3TBL_LOOKUP_MODE_FORCE = 0,
    DAL_MANGO_IPMC_L3TBL_LOOKUP_MODE_PACKET,
} dal_mango_ipmc_l3TblLookupMode_t;

typedef enum dal_mango_ipmc_l3TblHashKey_e
{
    DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP = 0,      /* VRF_ID, SIP, GIP */
    DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP_INTF,     /* VRF_ID, SIP, GIP, VID/INTF */
} dal_mango_ipmc_l3TblHashKey_t;


/* structure of IPMCAST L3 entry */
typedef struct dal_mango_ipmc_l3Entry_s
{
    uint32  signKey;                /* hash value of signature key */

    uint32  valid:1;                /* to indicate the node is used/free */
    uint32  ipv6:1;                 /* FALSE/TRUE - IPv4/IPv6 entry */
    uint32  cam:1;                  /* FALSE/TRUE - L3 Host/Route table */

    uint32  l2_pmsk_valid:1;        /* FALSE/TRUE - l2_pmsk_idx is invalid/valid */
    uint32  l2_tnl_lst_valid:1;     /* FALSE/TRUE - l2_tnl_lst_idx is invalid/valid */
    uint32  l3_oil_valid:1;         /* FALSE/TRUE - l3_oil_idx is invalid/valid */

    uint32  statMont_byte_valid:1;  /* statMont byte-count valid */
    uint32  statMont_byte_idx:2;    /* statMont byte-count entry index */
    uint32  statMont_pkt_valid:1;   /* statMont packet-count valid */
    uint32  statMont_pkt_idx:2;     /* statMont packet-count entry index */

    uint32  hw_entry_idx;           /* hardware entry index */

    rtk_mcast_group_t   groupId;    /* bound multicast group ID */
    int32   l2_pmsk_idx;            /* from group: l2 portmask index (-1 also means invalid) */
    int32   l2_tnl_lst_idx;         /* from group: l2 tunnel list index (-1 also means invalid) */
    int32   l3_oil_idx;             /* from group: l3 oil index (-1 also means invalid) */

    RTK_LIST_NODE_REF_DEF(dal_mango_ipmc_l3Entry_s, nodeRef);
} dal_mango_ipmc_l3Entry_t;

typedef struct dal_mango_ipmc_drvDb_s
{
    /* table info */
    uint32  tblBase;
    uint32  tblSize;

    /* L3 entry */
    RTK_LIST_DEF(dal_mango_ipmc_l3Entry_s, used_l3Entry_list);  /* list of the used L3 entries */
    RTK_LIST_DEF(dal_mango_ipmc_l3Entry_s, free_l3Entry_list);  /* list of the free L3 entries */
    dal_mango_ipmc_l3Entry_t l3Entry[DAL_MANGO_IPMCAST_L3_ENTRY_MAX];   /* instances of L3 entries */

    dal_mango_ipmc_l3TblLookupMode_t    l3TblLookupMode;    /* L3 table lookup mode */
    dal_mango_ipmc_l3TblHashKey_t       l3TblHashKey;       /* L3 table hash key */

    struct
    {
        uint32  valid:1;        /* FALSE - free entry, TRUE - used entry */
        uint32  mode:1;         /* 0 - packet-count, 1 - byte-count */
        uint32  l3EntryAddr;    /* address of monitored l3 entry */
    } mont[DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM];

#if MANGO_L3_ROUTE_IPMC_SIZE
    uint32  l3_route_ipmc_idx_base;
    uint32  l3_route_ipmc_size;

    uint16  l3_route_ipmc_ipv4_cnt;
    uint16  l3_route_ipmc_ipv6_cnt;

    uint32  l3_route_ipmc_ipv4_bottom_offset;
    uint32  l3_route_ipmc_ipv6_top_offset;
    dal_mango_ipmc_l3Entry_t            *pIpmcL3Entry[MANGO_L3_ROUTE_IPMC_SIZE];
#endif
} dal_mango_ipmc_drvDb_t;

typedef struct dal_mango_ipmc_hashIdx_s
{
    uint32 hashIdx_of_alg[MANGO_IPMC_HASH_IDX_ALG_NUM];
} dal_mango_ipmc_hashIdx_t;



/*
 * Data Declaration
 */
static uint32               ipmc_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         ipmc_sem[RTK_MAX_NUM_OF_UNIT];

static dal_mango_ipmc_drvDb_t           *_pIpmcDb[RTK_MAX_NUM_OF_UNIT];


static uint32 _actIpmcRouteCtrlBadSip[] = {   /* MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    RTK_IPMC_ACT_DROP,
    };

static uint32 _actIpmcRouteCtrlZeroSip[] = {  /* MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    RTK_IPMC_ACT_DROP,
    };

static uint32 _actIpmcRouteCtrlDmacMismatch[] = { /* MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpmcRouteCtrlHdrOpt[] = {   /* MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_HDR_OPT_ACTf */
    RTK_IPMC_ACT_HARD_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_FORWARD,           /* route and bridge */
    RTK_IPMC_ACT_COPY2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    RTK_IPMC_ACT_COPY2MASTERCPU,
    RTK_IPMC_ACT_DROP,
    };

static uint32 _actIpmcRouteCtrlMtuFail[] = {  /* MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _actIpmcRouteCtrlTtlFail[] = {  /* MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,
    RTK_IPMC_ACT_TRAP2CPU,
    RTK_IPMC_ACT_TRAP2MASTERCPU,
    };

static uint32 _optIpmcRouteCtrlPktToCPUTarget[] = { /* MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf */
    RTK_IPMC_CPUTARGET_LOCAL,
    RTK_IPMC_CPUTARGET_MASTER,
    };

static uint32 _actIp6mcRouteCtrlBadSip[] = {  /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,         /* drop L2+L3 */
    RTK_IPMC_ACT_TRAP2CPU,          /* [BUG] trap to local CPU */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* [BUG] trap to master CPU */
    RTK_IPMC_ACT_DROP,              /* bridge only */
    };

static uint32 _actIp6mcRouteCtrlZeroSip[] = { /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf */
    RTK_IPMC_ACT_HARD_DROP,         /* drop L2+L3 */
    RTK_IPMC_ACT_TRAP2CPU,          /* [BUG] trap to local CPU */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* [BUG] trap to master CPU */
    RTK_IPMC_ACT_DROP,              /* bridge only */
    };

static uint32 _actIp6mcRouteCtrlDmacMismatch[] = {    /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf */
    RTK_IPMC_ACT_HARD_DROP,         /* drop L2+L3 */
    RTK_IPMC_ACT_DROP,              /* bridge only */
    RTK_IPMC_ACT_TRAP2CPU,          /* bridge + to local CPU */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* bridge + to master CPU */
    };

static uint32 _actIp6mcRouteCtrlHbh[] = { /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HBH_ACTf */
    RTK_IPMC_ACT_HARD_DROP,         /* drop L2+L3 */
    RTK_IPMC_ACT_TRAP2CPU,          /* [BUG] trap to local CPU (should be bridge + to local CPU) */
    RTK_IPMC_ACT_FORWARD,           /* route and bridge */
    RTK_IPMC_ACT_COPY2CPU,          /* route and bridge + to local CPU */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* [BUG] trap to master CPU (should be bridge + to master CPU) */
    RTK_IPMC_ACT_COPY2MASTERCPU,    /* route and bridge + to master CPU */
    RTK_IPMC_ACT_DROP,              /* bridge only */
    };

static uint32 _actIp6mcRouteCtrlHbhErr[] = {  /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HBH_ERR_ACTf */
    RTK_IPMC_ACT_HARD_DROP,         /* drop L2+L3 */
    RTK_IPMC_ACT_TRAP2CPU,          /* [BUG] trap to local CPU (should be bridge + to local CPU) */
    RTK_IPMC_ACT_FORWARD,           /* [BUG] bridge only (SHOULD BE route and bridge) */
    RTK_IPMC_ACT_COPY2CPU,          /* [BUG] bridge + to local CPU (SHOULD BE L2/L3 +to local CPU) */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* [BUG] trap to master CPU (should be bridge + to master CPU) */
    RTK_IPMC_ACT_COPY2MASTERCPU,    /* [BUG] bridge + to master CPU (SHOULD BE L2/L3 +to master CPU) */
    RTK_IPMC_ACT_DROP,              /* bridge only */
    };

static uint32 _actIp6mcRouteCtrlHdrRoute[] = {    /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HDR_ROUTE_ACTf */
    RTK_IPMC_ACT_HARD_DROP,         /* drop L2+L3 */
    RTK_IPMC_ACT_TRAP2CPU,          /* [BUG] trap to local CPU (should be bridge + to local CPU) */
    RTK_IPMC_ACT_FORWARD,           /* route and bridge */
    RTK_IPMC_ACT_COPY2CPU,          /* route and bridge + to local CPU */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* [BUG] to master CPU (SHOULD BE bridge + to master CPU) */
    RTK_IPMC_ACT_COPY2MASTERCPU,    /* route and bridge + to master CPU */
    RTK_IPMC_ACT_DROP,              /* [BUG] route+bridge (SHOULD BE bridge only) */
    };

static uint32 _actIp6mcRouteCtrlMtuFail[] = { /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,              /* Drop L3 */
    RTK_IPMC_ACT_TRAP2CPU,          /* [BUG] drop L3 (SHOULD BE trapped to local CPU) */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* [BUG] drop L3 (SHOULD BE trapped to master CPU) */
    };

static uint32 _actIp6mcRouteCtrlHlFail[] = {  /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HL_FAIL_ACTf */
    RTK_IPMC_ACT_DROP,              /* Drop L3 */
    RTK_IPMC_ACT_TRAP2CPU,          /* [BUG] drop L3 (SHOULD BE trapped to local CPU) */
    RTK_IPMC_ACT_TRAP2MASTERCPU,    /* [BUG] drop L3 (SHOULD BE trapped to master CPU) */
    };

static uint32 _optIp6mcRouteCtrlPktToCPUTarget[] = {    /* MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf */
    RTK_IPMC_CPUTARGET_LOCAL,
    RTK_IPMC_CPUTARGET_MASTER,
    };

static uint32 _actIpmcEntryL2Action[] = {
    RTK_IPMC_ADDR_ACT_FORWARD,       /* bridge */
    RTK_IPMC_ADDR_ACT_TRAP2CPU,
    RTK_IPMC_ADDR_ACT_COPY2CPU,
    RTK_IPMC_ADDR_ACT_DROP,
    };

static uint32 _actIpmcEntryL3Action[] = {
    RTK_IPMC_ADDR_ACT_FORWARD,       /* route */
    RTK_IPMC_ADDR_ACT_TRAP2CPU,
    RTK_IPMC_ADDR_ACT_COPY2CPU,
    RTK_IPMC_ADDR_ACT_DROP,
    };

static uint32 _actIpmcEntryL3RpfAction[] = {
    RTK_IPMC_RPF_FAIL_ACT_TRAP,
    RTK_IPMC_RPF_FAIL_ACT_DROP,
    RTK_IPMC_RPF_FAIL_ACT_COPY,
    RTK_IPMC_RPF_FAIL_ACT_ASSERT_CHK,
    };



/*
 * Macro Definition
 */
#define MANGO_IPMCAST_DBG_PRINTF(_level, _fmt, ...)                                 \
do {                                                                                \
    if (MANGO_IPMCAST_DBG >= (_level))                                              \
        osal_printf("L%05d:%s(): " _fmt, __LINE__, __FUNCTION__, ##__VA_ARGS__);    \
} while (0)

#define IPMCAST_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(ipmc_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_IPMCAST), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define IPMCAST_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(ipmc_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_IPMCAST), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)
#define MCAST_SEM_LOCK(_unit)       _dal_mango_mcast_sem_lock(_unit)
#define MCAST_SEM_UNLOCK(_unit)     _dal_mango_mcast_sem_unlock(_unit)
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

#define L3_ENTRY_IDX_IS_VALID(_unit, _entryIdx) (TRUE == _pIpmcDb[(_unit)]->l3Entry[(_entryIdx)].valid)

#define L3_ENTRY_ADDR(_unit, _entryIdx)         (&_pIpmcDb[(_unit)]->l3Entry[(_entryIdx)])
#define L3_ENTRY_ADDR_TO_IDX(_unit, _pEntry)    ((_pEntry) - (&_pIpmcDb[(_unit)]->l3Entry[0]))

#define L3_ENTRY_IS_VALID(_pEntry)              ((NULL != (_pEntry)) && (TRUE == (_pEntry)->valid))

#define L3_ENTRY_RESET(_pEntry)                                     \
do {                                                                \
    osal_memset(_pEntry, 0x00, sizeof(dal_mango_ipmc_l3Entry_t));   \
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

#define HASH_BIT_EXTRACT(_val, _lsb, _len)   (((_val) & (((1 << (_len)) - 1) << (_lsb))) >> (_lsb))



/*
 * Function Declaration
 */

static inline int32
_dal_mango_ipmc_emptyL3HostEntry_build(
    dal_mango_l3_hostEntry_t *pEntry,
    rtk_ipmc_flag_t flags)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
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
_dal_mango_ipmc_emptyL3RouteEntry_build(
    dal_mango_l3_routeEntry_t *pEntry,
    rtk_ipmc_flag_t flags)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_routeEntry_t));
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
_dal_mango_ipmc_rtkIpmcAddr2l3HostEntry(
    uint32 unit,
    dal_mango_l3_hostEntry_t *pEntry,
    rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    int32   l2fwdIdx;
    int32   l2LstIdx;
    int32   oilIdx;
    uint32  stkPmsk;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_hostEntry_t));
    pEntry->valid = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_DISABLE)? 0 : 1;

    pEntry->fmt = 0;
    if (DAL_MANGO_IPMC_L3TBL_LOOKUP_MODE_FORCE == _pIpmcDb[unit]->l3TblLookupMode)
    {
        pEntry->ipmc_type = 0x1;
    }

    if (DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP_INTF == _pIpmcDb[unit]->l3TblHashKey)
    {
        pEntry->vid_cmp = 1;
    }
    else if (!(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN))
    {
        if ((pIpmcAddr->src_intf.intf_id) ||
            (pIpmcAddr->src_intf.vlan_id))
            pEntry->vid_cmp = 1;
    }

    pEntry->vrf_id = pIpmcAddr->vrf_id;
    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        pEntry->entry_type = 0x3;
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            pEntry->sip6 = pIpmcAddr->src_ip_addr.ipv6;
        }
        else
        {
            /* set to all-1s */
            osal_memset(pEntry->sip6.octet, 0xFF, IPV6_ADDR_LEN);
        }
        pEntry->gip6 = pIpmcAddr->dst_ip_addr.ipv6;
    }
    else
    {
        pEntry->entry_type = 0x1;
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            pEntry->sip = pIpmcAddr->src_ip_addr.ipv4;
        }
        else
        {
            /* set to all-1s */
            pEntry->sip = 0xFFFFFFFF;
        }
        pEntry->gip = pIpmcAddr->dst_ip_addr.ipv4;
    }

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID)
    {
        pEntry->mc_key_sel = 0x1;
        pEntry->vid = pIpmcAddr->src_intf.intf_id;
        pEntry->l2_chk_vid = 0;     /* not supprt VLAN */
        pEntry->l3_rpf_id = pIpmcAddr->src_intf.intf_id;
    }
    else
    {
        pEntry->mc_key_sel = 0x0;
        pEntry->vid = pIpmcAddr->src_intf.vlan_id;
        pEntry->l2_chk_vid = pIpmcAddr->src_intf.vlan_id;
        pEntry->l3_rpf_id = pIpmcAddr->src_intf.vlan_id;
    }

    RT_ERR_HDL(_dal_mango_mcast_fwdIdx_get(unit, pIpmcAddr->group, &l2fwdIdx, &l2LstIdx, &oilIdx, &stkPmsk), errHandle, ret);

    MANGO_IPMCAST_DBG_PRINTF(1, "l2fwdIdx=%d, l2LstIdx=%d, oilIdx=%d, stkPmsk=0x%08X\n", l2fwdIdx, l2LstIdx, oilIdx, stkPmsk);

    IPMCAST_ACTION_TO_VALUE(_actIpmcEntryL2Action, pEntry->l2_act, pIpmcAddr->l2_fwd_act, "", errHandle, ret);
    if (l2fwdIdx >= 0)
    {
        pEntry->l2_mc_pmsk_idx = l2fwdIdx;
    }

    if (l2LstIdx >= 0)
    {
        pEntry->l2_mc_l2_tnl_lst_valid = 1;
        pEntry->l2_mc_l2_tnl_lst_idx = l2LstIdx;
    }

    /* L2 available */
    if ((pEntry->l2_act != 0) || (l2fwdIdx >= 0) || (l2LstIdx >= 0))
    {
        pEntry->l2_en = 1;
    }

    IPMCAST_ACTION_TO_VALUE(_actIpmcEntryL3Action, pEntry->l3_act, pIpmcAddr->l3_fwd_act, "", errHandle, ret);
    pEntry->rpf_chk_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN)? 1 : 0;
    IPMCAST_ACTION_TO_VALUE(_actIpmcEntryL3RpfAction, pEntry->rpf_fail_act, pIpmcAddr->l3_rpf_fail_act, "", errHandle, ret);
    if (oilIdx >= 0)
    {
        pEntry->oil_idx_valid = 1;
        pEntry->oil_idx = oilIdx;
    }

    pEntry->ttl_min = pIpmcAddr->ttl_min;
    pEntry->mtu_max = pIpmcAddr->mtu_max;

    pEntry->qos_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pIpmcAddr->qos_pri;
    pEntry->stack_fwd_pmsk = stkPmsk;

    pEntry->l3_en = pIpmcAddr->l3_en;

    pEntry->hit = 1;    /* keep HIT bit value */

    if (MANGO_IPMCAST_DBG)
    {
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->ipmc_type = %d\n", pEntry->ipmc_type);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->vid_cmp = %d\n", pEntry->vid_cmp);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->vrf_id = %d\n", pEntry->vrf_id);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->entry_type = %d\n", pEntry->entry_type);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->mc_key_sel = %d\n", pEntry->mc_key_sel);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->vid = %d\n", pEntry->vid);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_chk_vid = %d\n", pEntry->l2_chk_vid);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l3_rpf_id = %d\n", pEntry->l3_rpf_id);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_en = %d\n", pEntry->l2_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_act = %d\n", pEntry->l2_act);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_mc_pmsk_idx = 0x%X (%d)\n", pEntry->l2_mc_pmsk_idx, pEntry->l2_mc_pmsk_idx);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l3_en = %d\n", pEntry->l3_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l3_act = %d\n", pEntry->l3_act);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_chk_en = %d\n", pEntry->rpf_chk_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_fail_act = %d\n", pEntry->rpf_fail_act);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx_valid = %d\n", pEntry->oil_idx_valid);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx = 0x%X (%d)\n", pEntry->oil_idx, pEntry->oil_idx);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->ttl_min = %d\n", pEntry->ttl_min);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->mtu_max = %d\n", pEntry->mtu_max);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->qos_en = %d\n", pEntry->qos_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->qos_pri = %d\n", pEntry->qos_pri);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->stack_fwd_pmsk = 0x%04X\n", pEntry->stack_fwd_pmsk);
    }

errHandle:

    MANGO_IPMCAST_DBG_PRINTF(1, "l2_en=%d (act=%d), l3_en=%d (act=%d)\n", pEntry->l2_en, pEntry->l2_act, pEntry->l3_en, pEntry->l3_act);

    return ret;
}

static inline int32
_dal_mango_ipmc_l3HostEntry2rtkIpmcAddr(
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_mango_l3_hostEntry_t *pEntry,
    rtk_mcast_group_t groupId)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pEntry->fmt), RT_ERR_ENTRY_NOTFOUND);

    /* clear memory */
    osal_memset(pIpmcAddr, 0x00, sizeof(rtk_ipmc_addr_t));

    if (!pEntry->valid)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_DISABLE;
    }

    pIpmcAddr->vrf_id = pEntry->vrf_id;

    if (0x1 == pEntry->entry_type)          /* IPMC */
    {
        if (0xFFFFFFFF != pEntry->sip)
        {
            pIpmcAddr->src_ip_addr.ipv4 = pEntry->sip;
            pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
        }

        pIpmcAddr->dst_ip_addr.ipv4 = pEntry->gip;
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
    }
    else
    {
        MANGO_IPMCAST_DBG_PRINTF(1, "FATAL: unknown pEntry->entry_type = %u\n", pEntry->entry_type);
        return RT_ERR_INPUT;
    }

    if (0x1 == pEntry->mc_key_sel)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;

        pIpmcAddr->src_intf.intf_id = pEntry->vid;
    }
    else
    {
        pIpmcAddr->src_intf.vlan_id = pEntry->vid;
    }

    /* IPMC flags */
    if (pEntry->rpf_chk_en) { pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_RPF_EN; }
    if (pEntry->qos_en)     { pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN; }
    if (pEntry->hit)        { pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_HIT; }

    /* groupId */
    pIpmcAddr->group = groupId;

    /* actions */
    IPMCAST_VALUE_TO_ACTION(_actIpmcEntryL2Action, pIpmcAddr->l2_fwd_act, pEntry->l2_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actIpmcEntryL3Action, pIpmcAddr->l3_fwd_act, pEntry->l3_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actIpmcEntryL3RpfAction, pIpmcAddr->l3_rpf_fail_act, pEntry->rpf_fail_act, "", errHandle, ret);

    /* options */
    pIpmcAddr->ttl_min = pEntry->ttl_min;
    pIpmcAddr->mtu_max = pEntry->mtu_max;
    pIpmcAddr->qos_pri = pEntry->qos_pri;
    pIpmcAddr->l3_en = pEntry->l3_en;

errHandle:

    return ret;
}

static inline int32
_dal_mango_ipmc_rtkIpmcAddr2l3RouteEntry(
    uint32 unit,
    dal_mango_l3_routeEntry_t *pEntry,
    rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    int32   l2fwdIdx;
    int32   l2LstIdx;
    int32   oilIdx;
    uint32  stkPmsk;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEntry, 0x00, sizeof(dal_mango_l3_routeEntry_t));
    pEntry->valid = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_DISABLE)? 0 : 1;

    pEntry->fmt = 0;
    pEntry->bmsk_fmt = 0x1;

    if (DAL_MANGO_IPMC_L3TBL_LOOKUP_MODE_FORCE == _pIpmcDb[unit]->l3TblLookupMode)
    {
        pEntry->ipmc_type = 0x1;
        pEntry->bmsk_ipmc_type = 0x1;
    }

    if (DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP_INTF == _pIpmcDb[unit]->l3TblHashKey)
    {
        pEntry->vid_cmp = 1;
    }
    else if (!(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN))
    {
        if ((pIpmcAddr->src_intf.intf_id) ||
            (pIpmcAddr->src_intf.vlan_id))
            pEntry->vid_cmp = 1;
    }

    pEntry->vrf_id = pIpmcAddr->vrf_id;
    pEntry->bmsk_vrf_id = 0xFF;

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        pEntry->entry_type = 0x3;
        pEntry->bmsk_entry_type = 0x3;

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            pEntry->sip6 = pIpmcAddr->src_ip_addr.ipv6;
            osal_memset(pEntry->bmsk_sip6.octet, 0xFF, IPV6_ADDR_LEN);
        }
        else
        {
            pEntry->round = 1;
        }
        pEntry->gip6 = pIpmcAddr->dst_ip_addr.ipv6;
        osal_memset(pEntry->bmsk_gip6.octet, 0xFF, IPV6_ADDR_LEN);
    }
    else
    {
        pEntry->entry_type = 0x1;
        pEntry->bmsk_entry_type = 0x3;

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            pEntry->sip = pIpmcAddr->src_ip_addr.ipv4;
            pEntry->bmsk_sip = 0xFFFFFFFF;
        }
        else
        {
            pEntry->round = 1;
        }
        pEntry->gip = pIpmcAddr->dst_ip_addr.ipv4;
        pEntry->bmsk_gip = 0xFFFFFFFF;
    }

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID)
    {
        pEntry->mc_key_sel = 0x1;
        pEntry->vid = pIpmcAddr->src_intf.intf_id;
        pEntry->l2_chk_vid = 0;     /* not supprt VLAN */
        pEntry->l3_rpf_id = pIpmcAddr->src_intf.intf_id;
    }
    else
    {
        pEntry->mc_key_sel = 0x0;
        pEntry->vid = pIpmcAddr->src_intf.vlan_id;
        pEntry->l2_chk_vid = pIpmcAddr->src_intf.vlan_id;
        pEntry->l3_rpf_id = pIpmcAddr->src_intf.vlan_id;
    }

    if (pEntry->vid_cmp)    /* it should consider global configuration of look-up mode */
    {
        pEntry->bmsk_mc_key_sel = 0x1;
        pEntry->bmsk_vid = 0xFFF;
    }

    pEntry->bmsk_round = 0x1;

    RT_ERR_HDL(_dal_mango_mcast_fwdIdx_get(unit, pIpmcAddr->group, &l2fwdIdx, &l2LstIdx, &oilIdx, &stkPmsk), errHandle, ret);

    IPMCAST_ACTION_TO_VALUE(_actIpmcEntryL2Action, pEntry->l2_act, pIpmcAddr->l2_fwd_act, "", errHandle, ret);
    if (l2fwdIdx >= 0)
    {
        pEntry->l2_mc_pmsk_idx = l2fwdIdx;
    }

    if (l2LstIdx >= 0)
    {
        pEntry->l2_mc_l2_tnl_lst_valid = 1;
        pEntry->l2_mc_l2_tnl_lst_idx = l2LstIdx;
    }

    /* L2 available */
    if ((pEntry->l2_act != 0) || (l2fwdIdx >= 0) || (l2LstIdx >= 0))
    {
        pEntry->l2_en = 1;
    }

    IPMCAST_ACTION_TO_VALUE(_actIpmcEntryL3Action, pEntry->l3_act, pIpmcAddr->l3_fwd_act, "", errHandle, ret);
    pEntry->rpf_chk_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_RPF_EN)? 1 : 0;
    IPMCAST_ACTION_TO_VALUE(_actIpmcEntryL3RpfAction, pEntry->rpf_fail_act, pIpmcAddr->l3_rpf_fail_act, "", errHandle, ret);
    if (oilIdx >= 0)
    {
        pEntry->oil_idx_valid = 1;
        pEntry->oil_idx = oilIdx;
    }

    pEntry->ttl_min = pIpmcAddr->ttl_min;
    pEntry->mtu_max = pIpmcAddr->mtu_max;

    pEntry->qos_en = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_QOS_ASSIGN)? 1 : 0;
    pEntry->qos_pri = pIpmcAddr->qos_pri;
    pEntry->stack_fwd_pmsk = stkPmsk;

    pEntry->l3_en = pIpmcAddr->l3_en;

    pEntry->hit = 1;    /* keep HIT bit value */

    if (MANGO_IPMCAST_DBG)
    {
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->ipmc_type = %d\n", pEntry->ipmc_type);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->vid_cmp = %d\n", pEntry->vid_cmp);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->vrf_id = %d\n", pEntry->vrf_id);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->entry_type = %d\n", pEntry->entry_type);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->round = %d\n", pEntry->round);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->mc_key_sel = %d\n", pEntry->mc_key_sel);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->vid = %d\n", pEntry->vid);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_chk_vid = %d\n", pEntry->l2_chk_vid);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l3_rpf_id = %d\n", pEntry->l3_rpf_id);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_en = %d\n", pEntry->l2_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_act = %d\n", pEntry->l2_act);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l2_mc_pmsk_idx = 0x%X (%d)\n", pEntry->l2_mc_pmsk_idx, pEntry->l2_mc_pmsk_idx);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l3_en = %d\n", pEntry->l3_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->l3_act = %d\n", pEntry->l3_act);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_chk_en = %d\n", pEntry->rpf_chk_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->rpf_fail_act = %d\n", pEntry->rpf_fail_act);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx_valid = %d\n", pEntry->oil_idx_valid);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->oil_idx = 0x%X (%d)\n", pEntry->oil_idx, pEntry->oil_idx);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->ttl_min = %d\n", pEntry->ttl_min);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->mtu_max = %d\n", pEntry->mtu_max);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->qos_en = %d\n", pEntry->qos_en);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->qos_pri = %d\n", pEntry->qos_pri);
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->stack_fwd_pmsk = 0x%04X\n", pEntry->stack_fwd_pmsk);
    }

errHandle:

    MANGO_IPMCAST_DBG_PRINTF(1, "l2_en=%d (act=%d), l3_en=%d (act=%d)\n", pEntry->l2_en, pEntry->l2_act, pEntry->l3_en, pEntry->l3_act);

    return ret;
}

static inline int32
_dal_mango_ipmc_l3RouteEntry2rtkIpmcAddr(
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_mango_l3_routeEntry_t *pEntry,
    rtk_mcast_group_t groupId)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((0 != pEntry->fmt), RT_ERR_ENTRY_NOTFOUND);

    /* clear memory */
    osal_memset(pIpmcAddr, 0x00, sizeof(rtk_ipmc_addr_t));

    if (!pEntry->valid)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_DISABLE;
    }

    pIpmcAddr->vrf_id = pEntry->vrf_id;

    if (0x1 == pEntry->entry_type)          /* IPMC */
    {
        if (0 == pEntry->round)
        {
            pIpmcAddr->src_ip_addr.ipv4 = pEntry->sip;
            pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
        }

        pIpmcAddr->dst_ip_addr.ipv4 = pEntry->gip;
    }
    else if (0x3 == pEntry->entry_type)     /* IP6MC */
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_IPV6;

        if (0 == pEntry->round)
        {
            pIpmcAddr->src_ip_addr.ipv6 = pEntry->sip6;
            pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
        }

        pIpmcAddr->dst_ip_addr.ipv6 = pEntry->gip6;
    }
    else
    {
        MANGO_IPMCAST_DBG_PRINTF(1, "FATAL: unknown pEntry->entry_type = %u\n", pEntry->entry_type);
        return RT_ERR_INPUT;
    }

    if (0x1 == pEntry->mc_key_sel)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;

        /* using L3 RPF ID instead of vid/intf_id (due to it may not be able to read back) */
        pIpmcAddr->src_intf.intf_id = pEntry->l3_rpf_id;
    }
    else
    {
        /* using L3 RPF ID instead of vid/intf_id (due to it may not be able to read back) */
        pIpmcAddr->src_intf.vlan_id = pEntry->l3_rpf_id;
    }

    /* IPMC flags */
    if (pEntry->rpf_chk_en) { pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_RPF_EN; }
    if (pEntry->qos_en)     { pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_QOS_ASSIGN; }
    if (pEntry->hit)        { pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_HIT; }

    /* groupId */
    pIpmcAddr->group = groupId;

    /* actions */
    IPMCAST_VALUE_TO_ACTION(_actIpmcEntryL2Action, pIpmcAddr->l2_fwd_act, pEntry->l2_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actIpmcEntryL3Action, pIpmcAddr->l3_fwd_act, pEntry->l3_act, "", errHandle, ret);
    IPMCAST_VALUE_TO_ACTION(_actIpmcEntryL3RpfAction, pIpmcAddr->l3_rpf_fail_act, pEntry->rpf_fail_act, "", errHandle, ret);

    /* options */
    pIpmcAddr->ttl_min = pEntry->ttl_min;
    pIpmcAddr->mtu_max = pEntry->mtu_max;
    pIpmcAddr->qos_pri = pEntry->qos_pri;
    pIpmcAddr->l3_en   = pEntry->l3_en;

errHandle:

    return ret;
}

static int32
_dal_mango_ipmc_rtkIpmcAddr_get(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_mango_ipmc_l3Entry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    dal_mango_l3_hostEntry_t    hostEntry;
#if MANGO_L3_ROUTE_IPMC_SIZE
    dal_mango_l3_routeEntry_t   routeEntry;
#endif
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    dal_mango_l3_routeEntry_t   routeEntry;
#endif
    rtk_ipmc_flag_t     origFlags;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!L3_ENTRY_IS_VALID(pEntry), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
#if MANGO_L3_ROUTE_IPMC_SIZE
    if (FALSE == pEntry->cam)
    {
        origFlags = pIpmcAddr->ipmc_flags;

        RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
        RT_ERR_HDL(_dal_mango_ipmc_l3HostEntry2rtkIpmcAddr(pIpmcAddr, &hostEntry, pEntry->groupId), errHandle, ret);
        pIpmcAddr->hw_entry_idx = pEntry->hw_entry_idx;

        /* optional operation */
        if ((origFlags & RTK_IPMC_FLAG_HIT_CLEAR) && (hostEntry.hit))
        {
            hostEntry.hit = 0;  /* clear hit bit */
            RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
        }
    }
    else
    {
        origFlags = pIpmcAddr->ipmc_flags;

        RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);
        RT_ERR_HDL(_dal_mango_ipmc_l3RouteEntry2rtkIpmcAddr(pIpmcAddr, &routeEntry, pEntry->groupId), errHandle, ret);
        pIpmcAddr->hw_entry_idx = (MANGO_L3_ROUTE_IPMC_HW_IDX_BASE + pEntry->hw_entry_idx);

        /* optional operation */
        if ((origFlags & RTK_IPMC_FLAG_HIT_CLEAR) && (routeEntry.hit))
        {
            routeEntry.hit = 0;  /* clear hit bit */
            RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);
        }
    }
#else
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    if (FALSE == pEntry->cam)
    {
        origFlags = pIpmcAddr->ipmc_flags;

        RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
        RT_ERR_HDL(_dal_mango_ipmc_l3HostEntry2rtkIpmcAddr(pIpmcAddr, &hostEntry, pEntry->groupId), errHandle, ret);
        pIpmcAddr->hw_entry_idx = pEntry->hw_entry_idx;

        /* optional operation */
        if ((origFlags & RTK_IPMC_FLAG_HIT_CLEAR) && (hostEntry.hit))
        {
            hostEntry.hit = 0;  /* clear hit bit */
            RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
        }
    }
    else
    {
        origFlags = pIpmcAddr->ipmc_flags;

        RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);
        RT_ERR_HDL(_dal_mango_ipmc_l3RouteEntry2rtkIpmcAddr(pIpmcAddr, &routeEntry, pEntry->groupId), errHandle, ret);
        pIpmcAddr->hw_entry_idx = (MANGO_L3_ROUTE_IPMC_CAM_IDX_BASE + pEntry->hw_entry_idx);    /* for displaying */

        /* optional operation */
        if ((origFlags & RTK_IPMC_FLAG_HIT_CLEAR) && (routeEntry.hit))
        {
            routeEntry.hit = 0;  /* clear hit bit */
            RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);
        }
    }
#else
    origFlags = pIpmcAddr->ipmc_flags;

    RT_ERR_HDL(_dal_mango_l3_hostEntry_get(unit, pEntry->hw_entry_idx, &hostEntry, \
        DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
    RT_ERR_HDL(_dal_mango_ipmc_l3HostEntry2rtkIpmcAddr(pIpmcAddr, &hostEntry, pEntry->groupId), errHandle, ret);
    pIpmcAddr->hw_entry_idx = pEntry->hw_entry_idx;

    /* optional operation */
    if ((origFlags & RTK_IPMC_FLAG_HIT_CLEAR) && (hostEntry.hit))
    {
        hostEntry.hit = 0;  /* clear hit bit */
        RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
    }
#endif  /* else of MANGO_L3_ROUTE_IPMC_DYNAMIC */
#endif

errHandle:
    return ret;
}


static uint32
_dal_mango_ipmc_hostHash0_ret(uint32 unit, rtk_ipmc_addr_t *pHashKey)
{
    uint32  ipv6 = (pHashKey->ipmc_flags & RTK_IPMC_FLAG_IPV6);
    uint32  vlan_intf_id;
    uint32  hashRow[29];
    uint32  hashIdx;

    vlan_intf_id = (pHashKey->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID)? \
        pHashKey->src_intf.intf_id : pHashKey->src_intf.vlan_id;

    if (DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP == _pIpmcDb[unit]->l3TblHashKey)
    {
        vlan_intf_id = 0;   /* masked */
    }

    /* VRF ID */
    hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->vrf_id, 0, 5) << 5) | \
                 (HASH_BIT_EXTRACT(pHashKey->vrf_id, 5, 3) << 0);

    /* SIP */
    if (ipv6)
    {
        if (pHashKey->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[0], 0, 7) << 3) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[1], 5, 3) << 0);
            hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[1], 0, 5) << 5) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[2], 3, 5) << 0);
            hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[2], 0, 3) << 7) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[3], 1, 7) << 0);
            hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[3], 0, 1) << 9) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[4], 0, 8) << 1) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[5], 7, 1) << 0);
            hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[5], 0, 7) << 3) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[6], 5, 3) << 0);
            hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[6], 0, 5) << 5) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[7], 3, 5) << 0);
            hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[7], 0, 3) << 7) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[8], 1, 7) << 0);
            hashRow[8] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[8], 0, 1) << 9) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[9], 0, 8) << 1) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[10], 7, 1) << 0);
            hashRow[9] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[10], 0, 7) << 3) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[11], 5, 3) << 0);
            hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[11], 0, 5) << 5) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[12], 3, 5) << 0);
            hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[12], 0, 3) << 7) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[13], 1, 7) << 0);
            hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[13], 0, 1) << 9) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[14], 0, 8) << 1) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[15], 7, 1) << 0);
            hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[15], 0, 7) << 3) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[0],  7, 1) << 0);
        } else {
            hashRow[1] = (HASH_BIT_EXTRACT(0xFFU, 0, 7) << 3) | \
                         (HASH_BIT_EXTRACT(0xFFU, 5, 3) << 0);
            hashRow[2] = (HASH_BIT_EXTRACT(0xFFU, 0, 5) << 5) | \
                         (HASH_BIT_EXTRACT(0xFFU, 3, 5) << 0);
            hashRow[3] = (HASH_BIT_EXTRACT(0xFFU, 0, 3) << 7) | \
                         (HASH_BIT_EXTRACT(0xFFU, 1, 7) << 0);
            hashRow[4] = (HASH_BIT_EXTRACT(0xFFU, 0, 1) << 9) | \
                         (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 1) | \
                         (HASH_BIT_EXTRACT(0xFFU, 7, 1) << 0);
            hashRow[5] = (HASH_BIT_EXTRACT(0xFFU, 0, 7) << 3) | \
                         (HASH_BIT_EXTRACT(0xFFU, 5, 3) << 0);
            hashRow[6] = (HASH_BIT_EXTRACT(0xFFU, 0, 5) << 5) | \
                         (HASH_BIT_EXTRACT(0xFFU, 3, 5) << 0);
            hashRow[7] = (HASH_BIT_EXTRACT(0xFFU, 0, 3) << 7) | \
                         (HASH_BIT_EXTRACT(0xFFU, 1, 7) << 0);
            hashRow[8] = (HASH_BIT_EXTRACT(0xFFU, 0, 1) << 9) | \
                         (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 1) | \
                         (HASH_BIT_EXTRACT(0xFFU, 7, 1) << 0);
            hashRow[9] = (HASH_BIT_EXTRACT(0xFFU, 0, 7) << 3) | \
                         (HASH_BIT_EXTRACT(0xFFU, 5, 3) << 0);
            hashRow[10] = (HASH_BIT_EXTRACT(0xFFU, 0, 5) << 5) | \
                          (HASH_BIT_EXTRACT(0xFFU, 3, 5) << 0);
            hashRow[11] = (HASH_BIT_EXTRACT(0xFFU, 0, 3) << 7) | \
                          (HASH_BIT_EXTRACT(0xFFU, 1, 7) << 0);
            hashRow[12] = (HASH_BIT_EXTRACT(0xFFU, 0, 1) << 9) | \
                          (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 1) | \
                          (HASH_BIT_EXTRACT(0xFFU, 7, 1) << 0);
            hashRow[13] = (HASH_BIT_EXTRACT(0xFFU, 0, 7) << 3) | \
                          (HASH_BIT_EXTRACT(0xFFU,  7, 1) << 0);
        }
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
        if (pHashKey->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4, 27,  5) << 0);
            hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4, 17, 10) << 0);
            hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4,  7, 10) << 0);
            hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4,  0,  7) << 3);
        } else {
            hashRow[10] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL, 27,  5) << 0);
            hashRow[11] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL, 17, 10) << 0);
            hashRow[12] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL,  7, 10) << 0);
            hashRow[13] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL,  0,  7) << 3);
        }
    }

    /* DIP */
    if (ipv6)
    {
        hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[0], 0, 8) << 0);
        hashRow[15] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[1], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[2], 6, 2) << 0);
        hashRow[16] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[2], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[3], 4, 4) << 0);
        hashRow[17] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[3], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[4], 2, 6) << 0);
        hashRow[18] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[4], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[5], 0, 8) << 0);
        hashRow[19] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[6], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[7], 6, 2) << 0);
        hashRow[20] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[7], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[8], 4, 4) << 0);
        hashRow[21] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[8], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[9], 2, 6) << 0);
        hashRow[22] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[9], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[10], 0, 8) << 0);
        hashRow[23] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[11], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[12], 6, 2) << 0);
        hashRow[24] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[12], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[13], 4, 4) << 0);
        hashRow[25] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[13], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[14], 2, 6) << 0);
        hashRow[26] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[14], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[15], 0, 8) << 0);
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
        hashRow[23] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4, 30,  2) << 0);
        hashRow[24] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4, 20, 10) << 0);
        hashRow[25] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4, 10, 10) << 0);
        hashRow[26] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4,  0, 10) << 0);
    }

    /* VLAN/INTF ID */
    hashRow[27] = (HASH_BIT_EXTRACT(vlan_intf_id, 3, 9) << 0);
    hashRow[28] = (HASH_BIT_EXTRACT(vlan_intf_id, 0, 3) << 7);

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
_dal_mango_ipmc_hostHash1_ret(uint32 unit, rtk_ipmc_addr_t *pHashKey)
{
    uint32  ipv6 = (pHashKey->ipmc_flags & RTK_IPMC_FLAG_IPV6);
    uint32  vlan_intf_id;
    uint32  sum;
    uint32  hashRow[30];
    uint32  hashIdx;

    vlan_intf_id = (pHashKey->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID)? \
        pHashKey->src_intf.intf_id : pHashKey->src_intf.vlan_id;

    if (DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP == _pIpmcDb[unit]->l3TblHashKey)
    {
        vlan_intf_id = 0;   /* masked */
    }

    if (ipv6)
    {
        if (pHashKey->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[12], 6, 2) << 0);
            hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[12], 0, 6) << 4) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[13], 4, 4) << 0);
            hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[13], 0, 4) << 6) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[14], 2, 6) << 0);
            hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[14], 0, 2) << 8) | \
                         (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[15], 0, 8) << 0);

            hashRow[9] =  (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[0], 2, 6) << 0);
            hashRow[10] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[0], 0, 2) << 8) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[1], 0, 8) << 0);
            hashRow[11] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[2], 0, 8) << 2) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[3], 6, 2) << 0);
            hashRow[12] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[3], 0, 6) << 4) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[4], 4, 4) << 0);
            hashRow[13] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[4], 0, 4) << 6) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[5], 2, 6) << 0);
            hashRow[14] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[5], 0, 2) << 8) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[6], 0, 8) << 0);
            hashRow[15] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[7], 0, 8) << 2) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[8], 6, 2) << 0);
            hashRow[16] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[8], 0, 6) << 4) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[9], 4, 4) << 0);
            hashRow[17] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[9], 0, 4) << 6) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[10], 2, 6) << 0);
            hashRow[18] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[10], 0, 2) << 8) | \
                          (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv6.octet[11], 0, 8) << 0);
        } else {
            hashRow[0] = (HASH_BIT_EXTRACT(0xFFU, 6, 2) << 0);
            hashRow[1] = (HASH_BIT_EXTRACT(0xFFU, 0, 6) << 4) | \
                         (HASH_BIT_EXTRACT(0xFFU, 4, 4) << 0);
            hashRow[2] = (HASH_BIT_EXTRACT(0xFFU, 0, 4) << 6) | \
                         (HASH_BIT_EXTRACT(0xFFU, 2, 6) << 0);
            hashRow[3] = (HASH_BIT_EXTRACT(0xFFU, 0, 2) << 8) | \
                         (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 0);

            hashRow[9] =  (HASH_BIT_EXTRACT(0xFFU, 2, 6) << 0);
            hashRow[10] = (HASH_BIT_EXTRACT(0xFFU, 0, 2) << 8) | \
                          (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 0);
            hashRow[11] = (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 2) | \
                          (HASH_BIT_EXTRACT(0xFFU, 6, 2) << 0);
            hashRow[12] = (HASH_BIT_EXTRACT(0xFFU, 0, 6) << 4) | \
                          (HASH_BIT_EXTRACT(0xFFU, 4, 4) << 0);
            hashRow[13] = (HASH_BIT_EXTRACT(0xFFU, 0, 4) << 6) | \
                          (HASH_BIT_EXTRACT(0xFFU, 2, 6) << 0);
            hashRow[14] = (HASH_BIT_EXTRACT(0xFFU, 0, 2) << 8) | \
                          (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 0);
            hashRow[15] = (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 2) | \
                          (HASH_BIT_EXTRACT(0xFFU, 6, 2) << 0);
            hashRow[16] = (HASH_BIT_EXTRACT(0xFFU, 0, 6) << 4) | \
                          (HASH_BIT_EXTRACT(0xFFU, 4, 4) << 0);
            hashRow[17] = (HASH_BIT_EXTRACT(0xFFU, 0, 4) << 6) | \
                          (HASH_BIT_EXTRACT(0xFFU, 2, 6) << 0);
            hashRow[18] = (HASH_BIT_EXTRACT(0xFFU, 0, 2) << 8) | \
                          (HASH_BIT_EXTRACT(0xFFU, 0, 8) << 0);
        }

        hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[12], 6, 2) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[12], 0, 6) << 4) | \
                     (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[13], 4, 4) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[13], 0, 4) << 6) | \
                     (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[14], 2, 6) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[14], 0, 2) << 8) | \
                     (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[15], 0, 8) << 0);

        hashRow[8] = (HASH_BIT_EXTRACT(pHashKey->vrf_id, 0, 4) << 6) | \
                     (HASH_BIT_EXTRACT(pHashKey->vrf_id, 4, 4) << 0);

        hashRow[19] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[0], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[1], 6, 2) << 0);
        hashRow[20] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[1], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[2], 4, 4) << 0);
        hashRow[21] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[2], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[3], 2, 6) << 0);
        hashRow[22] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[3], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[4], 0, 8) << 0);
        hashRow[23] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[5], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[6], 6, 2) << 0);
        hashRow[24] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[6], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[7], 4, 4) << 0);
        hashRow[25] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[7], 0, 4) << 6) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[8], 2, 6) << 0);
        hashRow[26] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[8], 0, 2) << 8) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[9], 0, 8) << 0);
        hashRow[27] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[10], 0, 8) << 2) | \
                      (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[11], 6, 2) << 0);
        hashRow[28] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv6.octet[11], 0, 6) << 4) | \
                      (HASH_BIT_EXTRACT(vlan_intf_id, 8, 4) << 0);
        hashRow[29] = (HASH_BIT_EXTRACT(vlan_intf_id, 0, 8) << 2);
    } else {
        if (pHashKey->ipmc_flags & RTK_IPMC_FLAG_SIP)
        {
            hashRow[0] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4, 30,  2) << 0);
            hashRow[1] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4, 20, 10) << 0);
            hashRow[2] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4, 10, 10) << 0);
            hashRow[3] = (HASH_BIT_EXTRACT(pHashKey->src_ip_addr.ipv4,  0, 10) << 0);
        } else {
            hashRow[0] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL, 30,  2) << 0);
            hashRow[1] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL, 20, 10) << 0);
            hashRow[2] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL, 10, 10) << 0);
            hashRow[3] = (HASH_BIT_EXTRACT(0xFFFFFFFFUL,  0, 10) << 0);
        }

        hashRow[4] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4, 30,  2) << 0);
        hashRow[5] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4, 20, 10) << 0);
        hashRow[6] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4, 10, 10) << 0);
        hashRow[7] = (HASH_BIT_EXTRACT(pHashKey->dst_ip_addr.ipv4,  0, 10) << 0);

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
        hashRow[28] = (HASH_BIT_EXTRACT(vlan_intf_id, 8, 4) << 0);
        hashRow[29] = (HASH_BIT_EXTRACT(vlan_intf_id, 0, 8) << 2);
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

static int32
_dal_mango_ipmc_hashIdx_get(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr, dal_mango_ipmc_hashIdx_t *pHashIdx)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHashIdx), RT_ERR_NULL_POINTER);

    /* function body */
    pHashIdx->hashIdx_of_alg[0] = _dal_mango_ipmc_hostHash0_ret(unit, pIpmcAddr);
    pHashIdx->hashIdx_of_alg[1] = _dal_mango_ipmc_hostHash1_ret(unit, pIpmcAddr);

    MANGO_IPMCAST_DBG_PRINTF(1, "hashIdx_of_alg[0] = %u, hashIdx_of_alg[1] = %u\n", \
        pHashIdx->hashIdx_of_alg[0], pHashIdx->hashIdx_of_alg[1]);

    return RT_ERR_OK;
}

static int32
_dal_mango_ipmc_l3HostAlloc_get(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_mango_ipmc_hashIdx_t *pIpmcHashIdx,
    dal_mango_l3_hostAlloc_t *pHostAlloc)
{
    int32   ret = RT_ERR_OK;
    uint32  alg_of_tbl[DAL_MANGO_L3_HOST_TBL_NUM];
    uint32  tbl;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHostAlloc), RT_ERR_NULL_POINTER);

    /* function body */
    IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_MC_HASH_ALG_SEL_0f, alg_of_tbl[0], "", errHandle, ret);
    IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_MC_HASH_ALG_SEL_1f, alg_of_tbl[1], "", errHandle, ret);

    /* calculate hash index for each table */
    for (tbl=0; tbl<DAL_MANGO_L3_HOST_TBL_NUM; tbl++)
    {
        pHostAlloc->hashIdx.idx_of_tbl[tbl] = pIpmcHashIdx->hashIdx_of_alg[(alg_of_tbl[tbl] & 0x1)];
    }

    /* entry width: IP6MC (6), IPMC (2) */
    pHostAlloc->width = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)? 6 : 2;

    MANGO_IPMCAST_DBG_PRINTF(1, "idx_of_tbl[0] = %u, idx_of_tbl[1] = %u, width = %u\n", \
        pHostAlloc->hashIdx.idx_of_tbl[0], pHostAlloc->hashIdx.idx_of_tbl[1], pHostAlloc->width);

errHandle:
    return ret;
}


static inline uint32
_dal_mango_ipmc_signKey_ret(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr, dal_mango_ipmc_hashIdx_t *pHashIdx)
{
    uint32  signKey;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), 0);   /* return key as zero */
    RT_PARAM_CHK((NULL == pHashIdx), 0);    /* return key as zero */

    /* function body */
    signKey = (pIpmcAddr->vrf_id & 0xFF) << 24;

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
    {
        signKey |= (0x1) << 23;
    }

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID)
    {
        signKey |= (0x1) << 22;
    }

    //signKey |= (pHashIdx->hashIdx_of_alg[1]) << 12;   /* don't have enough space */
    signKey |= (pHashIdx->hashIdx_of_alg[0]) << 0;

    MANGO_IPMCAST_DBG_PRINTF(1, "signKey = 0x%08X (%u, %u, %u, %u, %u)\n", \
        signKey, pIpmcAddr->vrf_id, \
        (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6), \
        (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID), \
        pHashIdx->hashIdx_of_alg[1], pHashIdx->hashIdx_of_alg[0]);

    return signKey;
}


static inline int32 _dal_mango_ipmc_l3Entry_alloc(uint32 unit, dal_mango_ipmc_l3Entry_t **ppEntry)
{
    dal_mango_ipmc_l3Entry_t *pEntry;

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

static inline int32 _dal_mango_ipmc_l3Entry_free(uint32 unit, dal_mango_ipmc_l3Entry_t *pEntry)
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
_dal_mango_ipmc_l3EntryMatch_ret(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_mango_ipmc_l3Entry_t *pEntry)
{
    dal_mango_l3_hostEntry_t hostEntry;
#if MANGO_L3_ROUTE_IPMC_SIZE
    dal_mango_l3_routeEntry_t routeEntry;
#endif
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    dal_mango_l3_routeEntry_t routeEntry;
#endif
    rtk_ipmc_addr_t ipmcAddr;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!L3_ENTRY_IS_VALID(pEntry)), RT_ERR_INPUT);

    /* read back from H/W table */
#if MANGO_L3_ROUTE_IPMC_SIZE
    if (FALSE == pEntry->cam)
    {
        if ((RT_ERR_OK != _dal_mango_l3_hostEntry_get(unit, pEntry->hw_entry_idx, &hostEntry, \
                                                      DAL_MANGO_L3_API_FLAG_NONE)) || \
            (RT_ERR_OK != _dal_mango_ipmc_l3HostEntry2rtkIpmcAddr(&ipmcAddr, &hostEntry, pEntry->groupId)))
        {
            return RT_ERR_FAILED;   /* fatal error */
        }
    }
    else
    {
        if ((RT_ERR_OK != __dal_mango_l3_routeEntry_get(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED)) || \
            (RT_ERR_OK != _dal_mango_ipmc_l3RouteEntry2rtkIpmcAddr(&ipmcAddr, &routeEntry, pEntry->groupId)))
        {
            return RT_ERR_FAILED;   /* fatal error */
        }
    }
#else
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    if (FALSE == pEntry->cam)
    {
        if ((RT_ERR_OK != _dal_mango_l3_hostEntry_get(unit, pEntry->hw_entry_idx, &hostEntry, \
                                                      DAL_MANGO_L3_API_FLAG_NONE)) || \
            (RT_ERR_OK != _dal_mango_ipmc_l3HostEntry2rtkIpmcAddr(&ipmcAddr, &hostEntry, pEntry->groupId)))
        {
            return RT_ERR_FAILED;   /* fatal error */
        }
    }
    else
    {
        if ((RT_ERR_OK != __dal_mango_l3_routeEntry_get(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED)) || \
            (RT_ERR_OK != _dal_mango_ipmc_l3RouteEntry2rtkIpmcAddr(&ipmcAddr, &routeEntry, pEntry->groupId)))
        {
            return RT_ERR_FAILED;   /* fatal error */
        }
    }
#else
    if ((RT_ERR_OK != _dal_mango_l3_hostEntry_get(unit, pEntry->hw_entry_idx, &hostEntry, \
                                                  DAL_MANGO_L3_API_FLAG_NONE)) || \
        (RT_ERR_OK != _dal_mango_ipmc_l3HostEntry2rtkIpmcAddr(&ipmcAddr, &hostEntry, pEntry->groupId)))
    {
        return RT_ERR_FAILED;   /* fatal error */
    }
#endif  /* else of MANGO_L3_ROUTE_IPMC_DYNAMIC */
#endif

    /* compare entries */
#if 0
    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->vrf_id = %u, ipmcAddr.vrf_id = %u\n", \
        pIpmcAddr->vrf_id, ipmcAddr.vrf_id);
    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->ipmc_flags = %u, ipmcAddr.ipmc_flags = %u\n", \
        pIpmcAddr->ipmc_flags, ipmcAddr.ipmc_flags);
    MANGO_IPMCAST_DBG_PRINTF(3, "(pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID) = %u, (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID) = %u\n", \
        (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID), (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SRC_INTF_ID));
    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->src_intf.intf_id = %u, ipmcAddr.src_intf.intf_id = %u\n", \
        pIpmcAddr->src_intf.intf_id, ipmcAddr.src_intf.intf_id);
    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->src_ip_addr.ipv4 = 0x%08X, ipmcAddr.src_ip_addr.ipv4 = 0x%08X\n", \
        pIpmcAddr->src_ip_addr.ipv4, ipmcAddr.src_ip_addr.ipv4);
    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->dst_ip_addr.ipv4 = 0x%08X, ipmcAddr.dst_ip_addr.ipv4 = 0x%08X\n", \
        pIpmcAddr->dst_ip_addr.ipv4, ipmcAddr.dst_ip_addr.ipv4);


    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->src_ip_addr.ipv6[15] = 0x%02X, ipmcAddr.src_ip_addr.ipv6[15] = 0x%02X\n", \
        pIpmcAddr->src_ip_addr.ipv6.octet[15], ipmcAddr.src_ip_addr.ipv6.octet[15]);
    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcAddr->dst_ip_addr.ipv6[15] = 0x%02X, ipmcAddr.dst_ip_addr.ipv6[15] = 0x%02X\n", \
        pIpmcAddr->dst_ip_addr.ipv6.octet[15], ipmcAddr.dst_ip_addr.ipv6.octet[15]);
#endif
    if (pIpmcAddr->vrf_id != ipmcAddr.vrf_id)
        return RT_ERR_FAILED;

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

    if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_SIP)
    {
        if (!(ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SIP))
            return RT_ERR_FAILED;

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
        {
            if (!(ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_IPV6))
                return RT_ERR_FAILED;

            if (RT_ERR_OK != rt_util_ipv6Cmp(&pIpmcAddr->src_ip_addr.ipv6, &ipmcAddr.src_ip_addr.ipv6))
                return RT_ERR_FAILED;

            if (RT_ERR_OK != rt_util_ipv6Cmp(&pIpmcAddr->dst_ip_addr.ipv6, &ipmcAddr.dst_ip_addr.ipv6))
                return RT_ERR_FAILED;
        }
        else
        {
            if (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_IPV6)
                return RT_ERR_FAILED;

            if (pIpmcAddr->src_ip_addr.ipv4 != ipmcAddr.src_ip_addr.ipv4)
                return RT_ERR_FAILED;

            if (pIpmcAddr->dst_ip_addr.ipv4 != ipmcAddr.dst_ip_addr.ipv4)
                return RT_ERR_FAILED;
        }
    }
    else
    {
        if (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_SIP)
            return RT_ERR_FAILED;

        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)
        {
            if (!(ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_IPV6))
                return RT_ERR_FAILED;

            if (RT_ERR_OK != rt_util_ipv6Cmp(&pIpmcAddr->dst_ip_addr.ipv6, &ipmcAddr.dst_ip_addr.ipv6))
                return RT_ERR_FAILED;
        }
        else
        {
            if (ipmcAddr.ipmc_flags & RTK_IPMC_FLAG_IPV6)
                return RT_ERR_FAILED;

            if (pIpmcAddr->dst_ip_addr.ipv4 != ipmcAddr.dst_ip_addr.ipv4)
                return RT_ERR_FAILED;
        }

    }

    return RT_ERR_OK;   /* match */
}


static int32
_dal_mango_ipmc_l3Entry_find_withSignKey(
    uint32 unit,
    rtk_ipmc_addr_t *pIpmcAddr,
    dal_mango_ipmc_l3Entry_t **ppEntry,
    uint32 signKey)
{
    dal_mango_ipmc_l3Entry_t *pEntry;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == ppEntry), RT_ERR_NULL_POINTER);

    /* function body */
    RTK_LIST_FOREACH(&_pIpmcDb[unit]->used_l3Entry_list, pEntry, nodeRef)
    {
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry->signKey = 0x%08X, signKey = 0x%08X\n", pEntry->signKey, signKey);
        if (pEntry->signKey > signKey)
            break;

        if (pEntry->signKey == signKey)
        {
            /* check further */
            if (RT_ERR_OK == _dal_mango_ipmc_l3EntryMatch_ret(unit, pIpmcAddr, pEntry))
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
_dal_mango_ipmc_l3EntryUsedList_insert(uint32 unit, dal_mango_ipmc_l3Entry_t *pEntry)
{
    dal_mango_ipmc_l3Entry_t *pPrev = NULL;
    dal_mango_ipmc_l3Entry_t *pTemp;

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

    MANGO_IPMCAST_DBG_PRINTF(3, "RTK_LIST_LENGTH(used_l3Entry_list) = %u\n", \
        RTK_LIST_LENGTH(&_pIpmcDb[unit]->used_l3Entry_list));

    return RT_ERR_OK;
}

static inline int32
_dal_mango_ipmc_l3EntryUsedList_remove(uint32 unit, dal_mango_ipmc_l3Entry_t *pEntry)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    RTK_LIST_NODE_REMOVE(&_pIpmcDb[unit]->used_l3Entry_list, pEntry, nodeRef);

    MANGO_IPMCAST_DBG_PRINTF(3, "RTK_LIST_LENGTH(&_pIpmcDb[unit]->used_l3Entry_list) = %u\n", \
        RTK_LIST_LENGTH(&_pIpmcDb[unit]->used_l3Entry_list));

    return RT_ERR_OK;
}

static inline int32
_dal_mango_ipmc_l3EntryStatMont_remove(uint32 unit, dal_mango_ipmc_l3Entry_t *pEntry)
{
    uint32 l3EntryAddr, montIdx;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* calculation of l3EntryAddr */
    l3EntryAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    l3EntryAddr |= (TRUE == pEntry->cam)? (0x1 << 14) : 0;

    if (TRUE == pEntry->statMont_byte_valid)
    {
        /* confirm the relationship */
        montIdx = pEntry->statMont_byte_idx;
        if (l3EntryAddr == _pIpmcDb[unit]->mont[montIdx].l3EntryAddr)
        {
            _pIpmcDb[unit]->mont[montIdx].valid = FALSE;
            _pIpmcDb[unit]->mont[montIdx].mode = 0;
            _pIpmcDb[unit]->mont[montIdx].l3EntryAddr = 0;

            pEntry->statMont_byte_valid = FALSE;
            pEntry->statMont_byte_idx = 0;

            /* don't have to sync to H/W here (not necessary) */
        }
    }
    if (TRUE == pEntry->statMont_pkt_valid)
    {
        /* confirm the relationship */
        montIdx = pEntry->statMont_pkt_idx;
        if (l3EntryAddr == _pIpmcDb[unit]->mont[montIdx].l3EntryAddr)
        {
            _pIpmcDb[unit]->mont[montIdx].valid = FALSE;
            _pIpmcDb[unit]->mont[montIdx].mode = 0;
            _pIpmcDb[unit]->mont[montIdx].l3EntryAddr = 0;

            pEntry->statMont_pkt_valid = FALSE;
            pEntry->statMont_pkt_idx = 0;

            /* don't have to sync to H/W here (not necessary) */
        }
    }

    return RT_ERR_OK;
}

static inline int32
_dal_mango_ipmc_statKey2rtkIpmcAddr(rtk_ipmc_addr_t *pIpmcAddr, rtk_ipmc_statKey_t *pStatKey)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pStatKey), RT_ERR_NULL_POINTER);

    osal_memset(pIpmcAddr, 0, sizeof(rtk_ipmc_addr_t));
    pIpmcAddr->vrf_id = pStatKey->vrf_id;
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
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SIP;
    }
    if (pStatKey->flags & RTK_IPMC_FLAG_SRC_INTF_ID)
    {
        pIpmcAddr->ipmc_flags |= RTK_IPMC_FLAG_SRC_INTF_ID;
        pIpmcAddr->src_intf.intf_id = pStatKey->src_intf.intf_id;
    }
    else
    {
        pIpmcAddr->src_intf.vlan_id = pStatKey->src_intf.vlan_id;
    }

    return RT_ERR_OK;
}

static inline int32
_dal_mango_ipmc_statMont_sync(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    uint32  value;

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM), RT_ERR_OUT_OF_RANGE);

    value = _pIpmcDb[unit]->mont[index].mode;
    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_MONT_CNTR_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, MANGO_MODEf, value, "", errHandle, ret);

    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_MONT_CNTR_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, MANGO_INDEXf, _pIpmcDb[unit]->mont[index].l3EntryAddr, "", errHandle, ret);

    MANGO_IPMCAST_DBG_PRINTF(1, "idx = %u, mode = %u, l3EntryAddr = %u\n", index, \
        _pIpmcDb[unit]->mont[index].mode, \
        _pIpmcDb[unit]->mont[index].l3EntryAddr);

errHandle:
    return ret;
}

static inline int32
_dal_mango_ipmc_statMont_reset(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    uint32  rstValue = 1;

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM), RT_ERR_OUT_OF_RANGE);

    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_MONT_CNTR_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, MANGO_RSTf, rstValue, "", errHandle, ret);

    MANGO_IPMCAST_DBG_PRINTF(1, "idx = %u\n", index);

errHandle:
    return ret;
}

static inline int32
_dal_mango_ipmc_statMontCntr_get(uint32 unit, uint32 index, rtk_ipmc_statCntr_t *pCntr)
{
    int32   ret = RT_ERR_OK;
    uint32  value, cnt_hi, cnt_lo;

    /* parameter check */
    RT_PARAM_CHK((index >= DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    value = 1;
    IPMCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_MONT_CNTR_CTRLr, REG_ARRAY_INDEX_NONE, \
        index, MANGO_LATCHf, value, "", errHandle, ret);

    /* check busy status */
    do {
        IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_L3_MONT_CNTR_CTRLr, REG_ARRAY_INDEX_NONE, \
            index, MANGO_LATCHf, value, "", errHandle, ret);
    } while (0 != value);

    /* get high/low counter */
    IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_L3_MONT_CNTR_DATAr, REG_ARRAY_INDEX_NONE, \
        index, MANGO_CNT_HIf, cnt_hi, "", errHandle, ret);
    IPMCAST_REG_ARRAY_FIELD_READ_ERR_HDL(unit, MANGO_L3_MONT_CNTR_DATAr, REG_ARRAY_INDEX_NONE, \
        index, MANGO_CNT_LOf, cnt_lo, "", errHandle, ret);

    if (MANGO_IPMC_STAT_MONT_MODE_BYTE_CNT == _pIpmcDb[unit]->mont[index].mode)
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

#if MANGO_L3_ROUTE_IPMC_SIZE
int32
_dal_mango_ipmc_lpmRouteCnt_get(uint32 unit, uint32 *pIpv4Cnt, uint32 *pIpv6Cnt)
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

    MANGO_IPMCAST_DBG_PRINTF(3, "*pIpv4Cnt = %d, *pIpv6Cnt = %d\n", *pIpv4Cnt, *pIpv6Cnt);

    return ret;
}
#endif

int32
_dal_mango_ipmc_l2PmskIdx_update(uint32 unit, uint32 l3EntryIdx, int32 l2PmskIdx)
{
    int32 ret = RT_ERR_OK;
    uint32 tbl, tfL2Valid, tfL2PmskIdx, valL2Valid, valL2PmskIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;
    l3_host_route_entry_t hostEntry;
    l3_prefix_route_entry_t routeEntry;

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((l3EntryIdx >= DAL_MANGO_IPMCAST_L3_ENTRY_MAX), RT_ERR_NULL_POINTER);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    pEntry = L3_ENTRY_ADDR(unit, l3EntryIdx);
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        if (l2PmskIdx < 0)
        {
            pEntry->l2_pmsk_valid = FALSE;
            pEntry->l2_pmsk_idx = 0;

            valL2Valid = 0;
            valL2PmskIdx = 0;
        }
        else
        {
            pEntry->l2_pmsk_valid = TRUE;
            pEntry->l2_pmsk_idx = l2PmskIdx;

            valL2Valid = 1;
            valL2PmskIdx = (uint32)pEntry->l2_pmsk_idx;
        }

        /* sync to H/W */
        MANGO_IPMCAST_DBG_PRINTF(1, "[DBG] pEntry->hw_entry_idx = %d, valL2Valid = %d, valL2PmskIdx = %d\n", pEntry->hw_entry_idx, valL2Valid, valL2PmskIdx);
        if (FALSE == pEntry->cam)
        {
            /* host table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_HOST_ROUTE_IP6MC_0t;
                tfL2Valid = MANGO_L3_HOST_ROUTE_IP6MC_0_L2_ENtf;
                tfL2PmskIdx = MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_PMSK_IDXtf;
            }
            else
            {
                tbl = MANGO_L3_HOST_ROUTE_IPMC_0t;
                tfL2Valid = MANGO_L3_HOST_ROUTE_IPMC_0_L2_ENtf;
                tfL2PmskIdx = MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_PMSK_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL2Valid, valL2Valid, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL2PmskIdx, valL2PmskIdx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), hostEntry, "", errHandle, ret);
        }
        else
        {
            /* route table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IP6MC_0t;
                tfL2Valid = MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_ENtf;
                tfL2PmskIdx = MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_PMSK_IDXtf;
            }
            else
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IPMC_0t;
                tfL2Valid = MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_ENtf;
                tfL2PmskIdx = MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_PMSK_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL2Valid, valL2Valid, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfL2PmskIdx, valL2PmskIdx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), routeEntry, "", errHandle, ret);
        }
    }

errHandle:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

int32
_dal_mango_ipmc_l2ListIdx_update(uint32 unit, uint32 l3EntryIdx, int32 l2ListIdx)
{
    int32 ret = RT_ERR_OK;
    uint32 tbl, tfListValid, tfListIdx, valListValid, valListIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;
    l3_host_route_entry_t hostEntry;
    l3_prefix_route_entry_t routeEntry;

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((l3EntryIdx >= DAL_MANGO_IPMCAST_L3_ENTRY_MAX), RT_ERR_NULL_POINTER);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    pEntry = L3_ENTRY_ADDR(unit, l3EntryIdx);
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        if (l2ListIdx < 0)
        {
            pEntry->l2_tnl_lst_valid = FALSE;
            pEntry->l2_tnl_lst_idx = 0;

            valListValid = 0;
            valListIdx = 0;
        }
        else
        {
            pEntry->l2_tnl_lst_valid = TRUE;
            pEntry->l2_tnl_lst_idx = l2ListIdx;

            valListValid = 1;
            valListIdx = (uint32)pEntry->l2_tnl_lst_idx;
        }

        /* sync to H/W */
        MANGO_IPMCAST_DBG_PRINTF(1, "[DBG] pEntry->hw_entry_idx = %d, valListValid = %d, valListIdx = %d\n", pEntry->hw_entry_idx, valListValid, valListIdx);
        if (FALSE == pEntry->cam)
        {
            /* host table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_HOST_ROUTE_IP6MC_0t;
                tfListValid = MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_TNL_LST_VALIDtf;
                tfListIdx = MANGO_L3_HOST_ROUTE_IP6MC_0_L2_MC_TNL_LST_IDXtf;
            }
            else
            {
                tbl = MANGO_L3_HOST_ROUTE_IPMC_0t;
                tfListValid = MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_TNL_LST_VALIDtf;
                tfListIdx = MANGO_L3_HOST_ROUTE_IPMC_0_L2_MC_TNL_LST_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListValid, valListValid, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListIdx, valListIdx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), hostEntry, "", errHandle, ret);
        }
        else
        {
            /* route table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IP6MC_0t;
                tfListValid = MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_TNL_LST_VALIDtf;
                tfListIdx = MANGO_L3_PREFIX_ROUTE_IP6MC_0_L2_MC_TNL_LST_IDXtf;
            }
            else
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IPMC_0t;
                tfListValid = MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_TNL_LST_VALIDtf;
                tfListIdx = MANGO_L3_PREFIX_ROUTE_IPMC_0_L2_MC_TNL_LST_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListValid, valListValid, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListIdx, valListIdx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), routeEntry, "", errHandle, ret);
        }
    }

errHandle:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}


int32
_dal_mango_ipmc_stkPmsk_update(uint32 unit, uint32 l3EntryIdx, uint32 pmskVal)
{
    int32 ret = RT_ERR_OK;
    uint32 tbl, tfStk;
    dal_mango_ipmc_l3Entry_t *pEntry;
    dal_mango_l3_hostEntry_t hostEntry;
    dal_mango_l3_routeEntry_t routeEntry;

    /* parameter check */
    RT_PARAM_CHK((l3EntryIdx >= DAL_MANGO_IPMCAST_L3_ENTRY_MAX), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    MANGO_IPMCAST_DBG_PRINTF(3, "l3EntryIdx = %x, pmskVal = %x\n", l3EntryIdx, pmskVal);

    pEntry = L3_ENTRY_ADDR(unit, l3EntryIdx);
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        /* sync to H/W */
        if (FALSE == pEntry->cam)
        {
            /* host table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_HOST_ROUTE_IP6MC_1t;
                tfStk = MANGO_L3_HOST_ROUTE_IP6MC_1_STACK_FWD_PMSKtf;
            }
            else
            {
                tbl = MANGO_L3_HOST_ROUTE_IPMC_1t;
                tfStk = MANGO_L3_HOST_ROUTE_IPMC_1_STACK_FWD_PMSKtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 1), hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfStk, pmskVal, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 1), hostEntry, "", errHandle, ret);

        }
        else
        {
            /* route table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IP6MC_1t;
                tfStk = MANGO_L3_PREFIX_ROUTE_IP6MC_1_STACK_FWD_PMSKtf;
            }
            else
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IPMC_1t;
                tfStk = MANGO_L3_PREFIX_ROUTE_IPMC_1_STACK_FWD_PMSKtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 1), routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfStk, pmskVal, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 1), routeEntry, "", errHandle, ret);
        }
    }

errHandle:
    IPMCAST_SEM_UNLOCK(unit);
    return ret;
}

int32
_dal_mango_ipmc_l3OilIdx_update(uint32 unit, uint32 l3EntryIdx, int32 l3OilIdx)
{
    int32 ret = RT_ERR_OK;
    uint32 tbl, tfListValid, tfListIdx, valListValid, valListIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;
    l3_host_route_entry_t hostEntry;
    l3_prefix_route_entry_t routeEntry;

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((l3EntryIdx >= DAL_MANGO_IPMCAST_L3_ENTRY_MAX), RT_ERR_NULL_POINTER);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    pEntry = L3_ENTRY_ADDR(unit, l3EntryIdx);
    if (L3_ENTRY_IS_VALID(pEntry))
    {
        if (l3OilIdx < 0)
        {
            pEntry->l3_oil_valid = FALSE;
            pEntry->l3_oil_idx = 0;

            valListValid = 0;
            valListIdx = 0;
        }
        else
        {
            pEntry->l3_oil_valid = TRUE;
            pEntry->l3_oil_idx = l3OilIdx;

            valListValid = 1;
            valListIdx = (uint32)pEntry->l3_oil_idx;
        }

        /* sync to H/W */
        if (FALSE == pEntry->cam)
        {
            /* host table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_HOST_ROUTE_IP6MC_0t;
                tfListValid = MANGO_L3_HOST_ROUTE_IP6MC_0_OIL_IDX_VALIDtf;
                tfListIdx = MANGO_L3_HOST_ROUTE_IP6MC_0_OIL_IDXtf;
            }
            else
            {
                tbl = MANGO_L3_HOST_ROUTE_IPMC_0t;
                tfListValid = MANGO_L3_HOST_ROUTE_IPMC_0_OIL_IDX_VALIDtf;
                tfListIdx = MANGO_L3_HOST_ROUTE_IPMC_0_OIL_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListValid, valListValid, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListIdx, valListIdx, hostEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), hostEntry, "", errHandle, ret);

        }
        else
        {
            /* route table */
            if (TRUE == pEntry->ipv6)
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IP6MC_0t;
                tfListValid = MANGO_L3_PREFIX_ROUTE_IP6MC_0_OIL_IDX_VALIDtf;
                tfListIdx = MANGO_L3_PREFIX_ROUTE_IP6MC_0_OIL_IDXtf;
            }
            else
            {
                tbl = MANGO_L3_PREFIX_ROUTE_IPMC_0t;
                tfListValid = MANGO_L3_PREFIX_ROUTE_IPMC_0_OIL_IDX_VALIDtf;
                tfListIdx = MANGO_L3_PREFIX_ROUTE_IPMC_0_OIL_IDXtf;
            }

            IPMCAST_TABLE_READ_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListValid, valListValid, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_FIELD_SET_ERR_HDL(unit, tbl, tfListIdx, valListIdx, routeEntry, "", errHandle, ret);
            IPMCAST_TABLE_WRITE_ERR_HDL(unit, tbl, (pEntry->hw_entry_idx + 0), routeEntry, "", errHandle, ret);
        }
    }

errHandle:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

#if MANGO_L3_ROUTE_IPMC_SIZE
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
static inline int32 _dal_mango_ipmc_l3RouteEntry_alloc(uint32 unit, uint32 ipv6, uint32 *pIndex, dal_mango_ipmc_l3Entry_t *pEntry)
{
    uint32  ipv4_size;
    uint32  ipv6_size;
    uint32  free_size;
    uint32  offset_idx;     /* 0 ~ (MANGO_L3_ROUTE_IPMC_SIZE - 1) */

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    ipv4_size = (_pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt * MANGO_L3_ROUTE_IPMC_WIDTH_IPV4);
    ipv6_size = (_pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt * MANGO_L3_ROUTE_IPMC_WIDTH_IPV6);
    free_size = (_pIpmcDb[unit]->l3_route_ipmc_size - ipv4_size - ipv6_size);

    if (ipv6)
    {
        /* IPv6 */
        if (free_size < MANGO_L3_ROUTE_IPMC_WIDTH_IPV6)
        {
            return RT_ERR_TBL_FULL;
        }

        offset_idx = (_pIpmcDb[unit]->l3_route_ipmc_size) - ipv6_size - MANGO_L3_ROUTE_IPMC_WIDTH_IPV6;

        _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset = offset_idx;
        _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt += 1;

        MANGO_IPMCAST_DBG_PRINTF(3, "[ALLOC] ipv6_top_offset = %d, ipv6_cnt = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt);
    } else {
        /* IPv4 */
        if (free_size < MANGO_L3_ROUTE_IPMC_WIDTH_IPV4)
        {
            return RT_ERR_TBL_FULL;
        }

        offset_idx = (ipv4_size);

        _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset = offset_idx;
        _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt += 1;

        MANGO_IPMCAST_DBG_PRINTF(3, "[ALLOC] ipv4_bottom_offset = %d, ipv4_cnt = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt);
    }

    _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = pEntry;

    /* entry index */
    *pIndex = (_pIpmcDb[unit]->l3_route_ipmc_idx_base + offset_idx);

    MANGO_IPMCAST_DBG_PRINTF(3, "[ALLOC] *pIndex = %d\n", (*pIndex));

    return RT_ERR_OK;
}

static inline int32 _dal_mango_ipmc_l3RouteEntry_free(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    uint32 offset_idx;
    dal_mango_ipmc_l3Entry_t    *pEntry;
    uint32 base_hw_idx;
    uint32 dst_hw_idx;
    uint32 src_hw_idx;
    dal_mango_l3_routeEntry_t   routeEntry;

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index < (_pIpmcDb[unit]->l3_route_ipmc_idx_base)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((index >= (_pIpmcDb[unit]->l3_route_ipmc_idx_base + _pIpmcDb[unit]->l3_route_ipmc_size)), RT_ERR_OUT_OF_RANGE);

    base_hw_idx = _pIpmcDb[unit]->l3_route_ipmc_idx_base;

    offset_idx = (index - _pIpmcDb[unit]->l3_route_ipmc_idx_base);
    pEntry = _pIpmcDb[unit]->pIpmcL3Entry[offset_idx];
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
        MANGO_IPMCAST_DBG_PRINTF(3, "[FREE] dst_hw_idx = %d, src_hw_idx = %d\n", dst_hw_idx, src_hw_idx);
        if (dst_hw_idx != src_hw_idx)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, src_hw_idx, &routeEntry, DISABLED), errHandle, ret);
            RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

            /* updated hw_entry_idx of the moved entry */
            _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)]->hw_entry_idx = dst_hw_idx;

            _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)];

            offset_idx = (src_hw_idx - base_hw_idx);

            dst_hw_idx = src_hw_idx;
        }

        RT_ERR_HDL(_dal_mango_ipmc_emptyL3RouteEntry_build(&routeEntry, RTK_IPMC_FLAG_IPV6), errHandle, ret);
        RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

        _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset += MANGO_L3_ROUTE_IPMC_WIDTH_IPV6;
        _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt -= 1;

        MANGO_IPMCAST_DBG_PRINTF(3, "[FREE] ipv6_top_offset = %d, ipv6_cnt = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_top_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv6_cnt);
    } else {
        /* IPv4 */

        /* move the bottom entry to this place if need */
        dst_hw_idx = pEntry->hw_entry_idx;
        src_hw_idx = (_pIpmcDb[unit]->l3_route_ipmc_idx_base + _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset);
        MANGO_IPMCAST_DBG_PRINTF(3, "[FREE] dst_hw_idx = %d, src_hw_idx = %d\n", dst_hw_idx, src_hw_idx);
        if (dst_hw_idx != src_hw_idx)
        {
            RT_ERR_HDL(__dal_mango_l3_routeEntry_get(unit, src_hw_idx, &routeEntry, DISABLED), errHandle, ret);
            RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

            /* updated hw_entry_idx of the moved entry */
            _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)]->hw_entry_idx = dst_hw_idx;

            _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = _pIpmcDb[unit]->pIpmcL3Entry[(src_hw_idx - base_hw_idx)];

            offset_idx = (src_hw_idx - base_hw_idx);

            dst_hw_idx = src_hw_idx;
        }

        RT_ERR_HDL(_dal_mango_ipmc_emptyL3RouteEntry_build(&routeEntry, RTK_IPMC_FLAG_NONE), errHandle, ret);
        RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, dst_hw_idx, &routeEntry, DISABLED), errHandle, ret);

        _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset -= MANGO_L3_ROUTE_IPMC_WIDTH_IPV4;
        _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt -= 1;

        MANGO_IPMCAST_DBG_PRINTF(3, "[FREE] ipv4_bottom_offset = %d, ipv4_cnt = %d\n",
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_bottom_offset,
            _pIpmcDb[unit]->l3_route_ipmc_ipv4_cnt);
    }

    /* release entry point */
    _pIpmcDb[unit]->pIpmcL3Entry[offset_idx] = 0;   // NULL

errHandle:
    return ret;
}
#endif

#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
int32
_dal_mango_ipmc_l3RouteIdxChange_callback(uint32 unit, void *pIpmcEntry, uint32 hwEntryIdx)
{
    dal_mango_ipmc_l3Entry_t *pEntry = (dal_mango_ipmc_l3Entry_t *)pIpmcEntry;

    MANGO_IPMCAST_DBG_PRINTF(3, "pIpmcEntry = 0x%p, hw_entry_idx: 0x%04X (%u) -> 0x%04X (%u)\n", \
        pEntry, pEntry->hw_entry_idx, pEntry->hw_entry_idx, hwEntryIdx, hwEntryIdx);

    /* updated hw_entry_idx */
    if (pEntry->cam == FALSE)
    {
        MANGO_IPMCAST_DBG_PRINTF(0, "FATAL error: pEntry(0x%p) is NOT a CAM entry!\n", pIpmcEntry);
        return RT_ERR_FAILED;
    }

    pEntry->hw_entry_idx = hwEntryIdx;

    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      dal_mango_ipmcMapper_init
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
dal_mango_ipmcMapper_init(dal_mapper_t *pMapper)
{
    pMapper->ipmc_init = dal_mango_ipmc_init;
    pMapper->ipmc_addr_add = dal_mango_ipmc_addr_add;
    pMapper->ipmc_addr_find = dal_mango_ipmc_addr_find;
    pMapper->ipmc_addr_del = dal_mango_ipmc_addr_del;
    pMapper->ipmc_addr_delAll = dal_mango_ipmc_addr_delAll;
    pMapper->ipmc_nextValidAddr_get = dal_mango_ipmc_addr_getNext;
    pMapper->ipmc_statMont_create = dal_mango_ipmc_statMont_create;
    pMapper->ipmc_statMont_destroy = dal_mango_ipmc_statMont_destroy;
    pMapper->ipmc_statCntr_reset = dal_mango_ipmc_statCntr_reset;
    pMapper->ipmc_statCntr_get = dal_mango_ipmc_statCntr_get;
    pMapper->ipmc_globalCtrl_set = dal_mango_ipmc_globalCtrl_set;
    pMapper->ipmc_globalCtrl_get = dal_mango_ipmc_globalCtrl_get;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_ipmc_init
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
dal_mango_ipmc_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  idx;
    dal_mango_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(ipmc_init[unit]);
    ipmc_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    ipmc_sem[unit] = osal_sem_mutex_create();
    if (0 == ipmc_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_IPMCAST), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* allocate memory that we need */
    if ((_pIpmcDb[unit] = osal_alloc(sizeof(dal_mango_ipmc_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_IPMCAST), "out of memory");
        return RT_ERR_FAILED;
    }

    /* initialize drvDb */
    osal_memset(_pIpmcDb[unit], 0x00, sizeof(dal_mango_ipmc_drvDb_t));

    /* used/free l3Entry list initialize */
    RTK_LIST_INIT(&_pIpmcDb[unit]->used_l3Entry_list);
    RTK_LIST_INIT(&_pIpmcDb[unit]->free_l3Entry_list);
    for (idx=0; idx<DAL_MANGO_IPMCAST_L3_ENTRY_MAX; idx++)
    {
        pEntry = L3_ENTRY_ADDR(unit, idx);
        L3_ENTRY_RESET(pEntry);
        RTK_LIST_NODE_REF_INIT(pEntry, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pIpmcDb[unit]->free_l3Entry_list, pEntry, nodeRef);
    }

#ifdef MANGO_IPMCAST_DBG
    RTK_LIST_FOREACH(&_pIpmcDb[unit]->free_l3Entry_list, pEntry, nodeRef)
    {
        MANGO_IPMCAST_DBG_PRINTF(3, "Entry-Node: idx = %ld (L3_ENTRY_ADDR_TO_IDX), pNode = %p, pPrev = %p, pNext = %p\n", \
            (long)L3_ENTRY_ADDR_TO_IDX(unit, pEntry), pEntry, pEntry->nodeRef.pPrev, pEntry->nodeRef.pNext);
    }
#endif

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* L3 table lookup mode and hash key */
        _pIpmcDb[unit]->l3TblLookupMode = DAL_MANGO_IPMC_L3TBL_LOOKUP_MODE_FORCE;
        IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_LU_MODE_SELf, \
            _pIpmcDb[unit]->l3TblLookupMode, "", errHandle, ret);
        _pIpmcDb[unit]->l3TblHashKey = DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP; //DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP_INTF;
        IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_LU_FORCE_MODE_HASH_KEY_SELf, \
            _pIpmcDb[unit]->l3TblHashKey, "", errHandle, ret);
    }

#if MANGO_L3_ROUTE_IPMC_SIZE
    _dal_mango_l3_route_ipmcSize_get(unit, &(_pIpmcDb[unit]->l3_route_ipmc_idx_base), &(_pIpmcDb[unit]->l3_route_ipmc_size));
#endif

    /* set init flag to complete init */
    ipmc_init[unit] = INIT_COMPLETED;

errHandle:

    return ret;
}   /* end of dal_mango_l3mc_init */


/* Function Name:
 *      dal_mango_ipmc_addr_add
 * Description:
 *      Add an ipmcast entry into the L3 host or route table
 * Input:
 *      unit      - unit id
 *      pIpmcAddr - pointer to rtk_ipmc_addr_t containing the basic inputs
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
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, ip_addr (either ipv4 or ipv6), and pathId (if RTK_L3_FLAG_NULL_INTF isn't set).
 *          RTK_IPMC_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_IPMC_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                          (path ID will not be referred)
 *          RTK_IPMC_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_IPMC_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_IPMC_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_IPMC_FLAG_SIP             - entry is dip + sip
 */
int32
dal_mango_ipmc_addr_add(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    int32   l3EntryAllocated = FALSE;
    int32   hostEntryAllocated = FALSE;
#if MANGO_L3_ROUTE_IPMC_SIZE
    int32   routeEntryAllocated = FALSE;
#endif
    uint32  signKey;
    uint32  hostIdx;
#if MANGO_L3_ROUTE_IPMC_SIZE
    uint32  routeIdx = 0;
#endif
    dal_mango_ipmc_hashIdx_t hashIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;
    dal_mango_l3_hostAlloc_t hostAlloc;
    dal_mango_l3_hostEntry_t hostEntry;
#if MANGO_L3_ROUTE_IPMC_SIZE
    dal_mango_l3_routeEntry_t routeEntry;
#endif
#ifdef MANGO_L3_ROUTE_IPMC_DYNAMIC
    int32   routeEntryAllocated = FALSE;
    uint32  routeIdx = 0;
    dal_mango_l3_routeEntry_t routeEntry;
#endif
    //rtk_enable_t groupBind;
    //uint32  entryIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pIpmcAddr=%p", unit, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pIpmcAddr->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((pIpmcAddr->ttl_min > RTK_IP_TTL_MAX), RT_ERR_TTL_EXCEED);

    /* function body */
    RT_ERR_CHK(_dal_mango_ipmc_hashIdx_get(unit, pIpmcAddr, &hashIdx), ret);
    signKey = _dal_mango_ipmc_signKey_ret(unit, pIpmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    /* bindable check */
    //RT_ERR_HDL(_dal_mango_mcast_ipmc_bindState_get(unit, pIpmcAddr->group, &groupBind, &entryIdx), errHandle, ret);

    /* try to find the L3 entry */
    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_find_withSignKey(unit, pIpmcAddr, &pEntry, signKey), errHandle, ret);
    if (NULL != pEntry)
    {
        /* already exists */
        if (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_REPLACE)
        {
            if (pIpmcAddr->group != pEntry->groupId)
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
                RT_ERR_HDL(_dal_mango_mcast_ipmc_unbind(unit, pEntry->groupId, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);
            }

#if MANGO_L3_ROUTE_IPMC_SIZE
            if (FALSE == pEntry->cam)
            {
                RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr), errHandle, ret);
                RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                    DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3RouteEntry(unit, &routeEntry, pIpmcAddr), errHandle, ret);
                RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);
            }
#else
#ifdef MANGO_L3_ROUTE_IPMC_DYNAMIC
            if (FALSE == pEntry->cam)
            {
                RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr), errHandle, ret);
                RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                    DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3RouteEntry(unit, &routeEntry, pIpmcAddr), errHandle, ret);
                RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);
            }
#else
            RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr), errHandle, ret);
            RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
#endif  /* else of MANGO_L3_ROUTE_IPMC_DYNAMIC */
#endif

            /* update entry info */
            pEntry->groupId = pIpmcAddr->group;

            /* bind the new group */
            RT_ERR_HDL(_dal_mango_mcast_ipmc_bind(unit, pEntry->groupId, pIpmcAddr, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);
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
        /* check group's binding state */
        if (TRUE == groupBind)
        {
            /* the group is being used */
            ret =  RT_ERR_ENTRY_REFERRED;
            goto errHandle;
        }
#endif

        /* alloc a new l3Entry */
        RT_ERR_HDL(_dal_mango_ipmc_l3Entry_alloc(unit, &pEntry), errHandle, ret);
        l3EntryAllocated = TRUE;

        pEntry->ipv6 = (pIpmcAddr->ipmc_flags & RTK_IPMC_FLAG_IPV6)? TRUE : FALSE;

        osal_memset(&hostAlloc, 0x00, sizeof(dal_mango_l3_hostAlloc_t));
        /* try to allocate an empty L3 host entry (SRAM) */
        RT_ERR_HDL(_dal_mango_ipmc_l3HostAlloc_get(unit, pIpmcAddr, &hashIdx, &hostAlloc), errHandle, ret);

#if MANGO_L3_ROUTE_IPMC_SIZE
        ret = _dal_mango_l3_hostEntry_alloc(unit, &hostAlloc, &hostIdx, DAL_MANGO_L3_API_FLAG_NONE);
        if (RT_ERR_OK == ret)
        {
            hostEntryAllocated = TRUE;

            pEntry->cam = FALSE;
            pEntry->hw_entry_idx = hostIdx;

            /* write into L3 Host table here */
            RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr), errHandle, ret);
            RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            MANGO_IPMCAST_DBG_PRINTF(3, "IPMC host set - pEntry->hw_entry_idx = %u\n", pEntry->hw_entry_idx);
        }
        else
        {
            /* try to allocate an emptry L3 Route entry (TCAM) */
            RT_ERR_HDL(_dal_mango_ipmc_l3RouteEntry_alloc(unit, pEntry->ipv6, &routeIdx, pEntry), errHandle, ret);
            routeEntryAllocated = TRUE;

            pEntry->cam = TRUE;
            pEntry->hw_entry_idx = routeIdx;

            /* write into L3 Route table here */
            RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3RouteEntry(unit, &routeEntry, pIpmcAddr), errHandle, ret);
            RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);

            MANGO_IPMCAST_DBG_PRINTF(3, "IPMC route set - pEntry->hw_entry_idx = %u\n", pEntry->hw_entry_idx);
        }
#else
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        ret = _dal_mango_l3_hostEntry_alloc(unit, &hostAlloc, &hostIdx, DAL_MANGO_L3_API_FLAG_NONE);
        if (RT_ERR_OK == ret)
        {
            hostEntryAllocated = TRUE;

            pEntry->cam = FALSE;
            pEntry->hw_entry_idx = hostIdx;

            /* write into L3 Host table here */
            RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr), errHandle, ret);
            RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            MANGO_IPMCAST_DBG_PRINTF(3, "IPMC host set - pEntry->hw_entry_idx = %u\n", pEntry->hw_entry_idx);
        }
        else
        {
            /* try to allocate an emptry L3 Route entry (TCAM) */
            RT_ERR_HDL(__dal_mango_l3_routeIpmcEntry_alloc(unit, pEntry->ipv6, &routeIdx, pEntry), errHandle, ret);
            routeEntryAllocated = TRUE;

            pEntry->cam = TRUE;
            pEntry->hw_entry_idx = routeIdx;

            /* write into L3 Route table here */
            RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3RouteEntry(unit, &routeEntry, pIpmcAddr), errHandle, ret);
            RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);

            MANGO_IPMCAST_DBG_PRINTF(3, "IPMC route set - pEntry->hw_entry_idx = %u\n", pEntry->hw_entry_idx);
        }
#else
        RT_ERR_HDL(_dal_mango_l3_hostEntry_alloc(unit, &hostAlloc, &hostIdx, DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
        hostEntryAllocated = TRUE;
        pEntry->cam = FALSE;
        pEntry->hw_entry_idx = hostIdx;

        /* write into L3 Host table here */
        RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr2l3HostEntry(unit, &hostEntry, pIpmcAddr), errHandle, ret);
        RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

        MANGO_IPMCAST_DBG_PRINTF(1, "IPMC host set - pEntry->hw_entry_idx = %u\n", pEntry->hw_entry_idx);
#endif  /* else of MANGO_L3_ROUTE_IPMC_DYNAMIC */
#endif

        /* insert the l3Entry into used_l3Entry_list */
        pEntry->signKey = signKey;
        RT_ERR_HDL(_dal_mango_ipmc_l3EntryUsedList_insert(unit, pEntry), errHandle, ret);

        /* update entry info */
        pEntry->groupId = pIpmcAddr->group;

        /* call mcast API to update bind info */
        RT_ERR_HDL(_dal_mango_mcast_ipmc_bind(unit, pEntry->groupId, pIpmcAddr, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);
    }

    goto errOk;

errHandle:
#if MANGO_L3_ROUTE_IPMC_SIZE
    if (TRUE == routeEntryAllocated)
        IPMCAST_RT_ERR_HDL_DBG(_dal_mango_ipmc_l3RouteEntry_free(unit, routeIdx), "");
#endif
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
        if (TRUE == routeEntryAllocated)
            IPMCAST_RT_ERR_HDL_DBG(__dal_mango_l3_routeIpmcEntry_free(unit, routeIdx), "");
#endif
    if (TRUE == hostEntryAllocated)
        IPMCAST_RT_ERR_HDL_DBG(_dal_mango_l3_hostEntry_free(unit, hostIdx, DAL_MANGO_L3_API_FLAG_NONE), "");

    if (TRUE == l3EntryAllocated)
        IPMCAST_RT_ERR_HDL_DBG(_dal_mango_ipmc_l3Entry_free(unit, pEntry), "");

errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_ipmc_find
 * Description:
 *      Find an ipmcast entry based on IP address
 * Input:
 *      unit      - unit id
 *      pIpmcAddr - pointer to the structure containing the basic inputs
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
 *      (1) Basic required input parameters of the pIpmcAddr as input keys:
 *          vrf_id and dst_ip_addr or with src_ip_addr (either ipv4 or ipv6).
 *          RTK_IPMC_FLAG_HIT_CLEAR   - clear the hit bit if it's set
 */
int32
dal_mango_ipmc_addr_find(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    dal_mango_ipmc_hashIdx_t hashIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pIpmcAddr=%p", unit, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pIpmcAddr->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    RT_ERR_CHK(_dal_mango_ipmc_hashIdx_get(unit, pIpmcAddr, &hashIdx), ret);
    signKey = _dal_mango_ipmc_signKey_ret(unit, pIpmcAddr, &hashIdx);

    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_find_withSignKey(unit, pIpmcAddr, &pEntry, signKey), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* return information (groupId) and flags */
    //pIpmcAddr->group = pEntry->groupId;
    RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr_get(unit, pIpmcAddr, pEntry), errHandle, ret);

errHandle:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_ipmc_addr_del
 * Description:
 *      Delete an entry from the L3 host and route table
 * Input:
 *      unit      - unit id
 *      pIpmcAddr - pointer to rtk_ipmc_addr_t containing the basic inputs
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
 *      (1) Basic required input parameters of the pIpmcAddr as input keys:
 *          dst_ip_addr and src_ip_addr(either ipv4 or ipv6).
 */
int32
dal_mango_ipmc_addr_del(uint32 unit, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    dal_mango_ipmc_hashIdx_t hashIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;
    dal_mango_l3_hostEntry_t hostEntry;
#if MANGO_L3_ROUTE_IPMC_SIZE
    //dal_mango_l3_routeEntry_t routeEntry;
#endif

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pIpmcAddr=%p", unit, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pIpmcAddr->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    RT_ERR_CHK(_dal_mango_ipmc_hashIdx_get(unit, pIpmcAddr, &hashIdx), ret);
    signKey = _dal_mango_ipmc_signKey_ret(unit, pIpmcAddr, &hashIdx);

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    /* try to find the L3 entry */
    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_find_withSignKey(unit, pIpmcAddr, &pEntry, signKey), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

#if MANGO_L3_ROUTE_IPMC_SIZE
    if (FALSE == pEntry->cam)
    {
        RT_ERR_HDL(_dal_mango_ipmc_emptyL3HostEntry_build(&hostEntry, pIpmcAddr->ipmc_flags), errHandle, ret);
        RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

        RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
    }
    else
    {
        //RT_ERR_HDL(_dal_mango_ipmc_emptyL3RouteEntry_build(&routeEntry, pIpmcAddr->ipmc_flags), errHandle, ret);
        //RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);

        RT_ERR_HDL(_dal_mango_ipmc_l3RouteEntry_free(unit, pEntry->hw_entry_idx), errHandle, ret);
    }
#else
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
    if (FALSE == pEntry->cam)
    {
        RT_ERR_HDL(_dal_mango_ipmc_emptyL3HostEntry_build(&hostEntry, pIpmcAddr->ipmc_flags), errHandle, ret);
        RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

        RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
            DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
    }
    else
    {
        //RT_ERR_HDL(_dal_mango_ipmc_emptyL3RouteEntry_build(&routeEntry, pIpmcAddr->ipmc_flags), errHandle, ret);
        //RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);

        RT_ERR_HDL(__dal_mango_l3_routeIpmcEntry_free(unit, pEntry->hw_entry_idx), errHandle, ret);
    }
#else
    RT_ERR_HDL(_dal_mango_ipmc_emptyL3HostEntry_build(&hostEntry, pIpmcAddr->ipmc_flags), errHandle, ret);
    RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
        DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

    RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
        DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
#endif  /* else of MANGO_L3_ROUTE_IPMC_DYNAMIC */
#endif

    /* remove the monitors if it has */
    RT_ERR_HDL(_dal_mango_ipmc_l3EntryStatMont_remove(unit, pEntry), errHandle, ret);

    /* unbind multicast group */
    RT_ERR_HDL(_dal_mango_mcast_ipmc_unbind(unit, pEntry->groupId, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);

    /* remove the l3Entry into used_l3Entry_list */
    RT_ERR_HDL(_dal_mango_ipmc_l3EntryUsedList_remove(unit, pEntry), errHandle, ret);
    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_free(unit, pEntry), errHandle, ret);

    goto errOk;

errHandle:

errOk:
    IPMCAST_SEM_UNLOCK(unit);
    MCAST_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_mango_ipmc_addr_delAll
 * Description:
 *      Delete all ipmcast entry in host and route table entries
 * Input:
 *      unit - unit id
 *      flag - configure flag
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
dal_mango_ipmc_addr_delAll(uint32 unit, rtk_ipmc_flag_t flags)
{
    int32   ret = RT_ERR_OK;
    int32   ipv6;
    dal_mango_ipmc_l3Entry_t *pEntry;
    dal_mango_ipmc_l3Entry_t *pTemp;
    dal_mango_l3_hostEntry_t hostEntry;
#if MANGO_L3_ROUTE_IPMC_SIZE
    //dal_mango_l3_routeEntry_t routeEntry;
#endif

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,flags=0x%08x", unit, flags);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* function body */
    ipv6 = (flags & RTK_IPMC_FLAG_IPV6)? TRUE : FALSE;

    RT_ERR_HDL(_dal_mango_ipmc_emptyL3HostEntry_build(&hostEntry, flags), errHandle, ret);
#if MANGO_L3_ROUTE_IPMC_SIZE
    //RT_ERR_HDL(_dal_mango_ipmc_emptyL3RouteEntry_build(&routeEntry, flags), errHandle, ret);
#endif

    MCAST_SEM_LOCK(unit);
    IPMCAST_SEM_LOCK(unit);

    RTK_LIST_FOREACH_SAFE(&_pIpmcDb[unit]->used_l3Entry_list, pEntry, pTemp, nodeRef)
    {
        MANGO_IPMCAST_DBG_PRINTF(1, "pEntry = %p (valid = %u, ipv6 = %u)\n", pEntry, pEntry->valid, pEntry->ipv6);

        if (pEntry->ipv6 == ipv6)
        {
#if MANGO_L3_ROUTE_IPMC_SIZE
            if (FALSE == pEntry->cam)
            {
                RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                    DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
                    DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
            }
            else
            {
                //RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);

                RT_ERR_HDL(_dal_mango_ipmc_l3RouteEntry_free(unit, pEntry->hw_entry_idx), errHandle, ret);
            }
#else
#ifdef  MANGO_L3_ROUTE_IPMC_DYNAMIC
            if (FALSE == pEntry->cam)
            {
                RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                    DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

                RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
                    DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
            }
            else
            {
                //RT_ERR_HDL(__dal_mango_l3_routeEntry_set(unit, pEntry->hw_entry_idx, &routeEntry, DISABLED), errHandle, ret);

                RT_ERR_HDL(__dal_mango_l3_routeIpmcEntry_free(unit, pEntry->hw_entry_idx), errHandle, ret);
            }
#else
            RT_ERR_HDL(_dal_mango_l3_hostEntry_set(unit, pEntry->hw_entry_idx, &hostEntry, \
                DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);

            RT_ERR_HDL(_dal_mango_l3_hostEntry_free(unit, pEntry->hw_entry_idx, \
                DAL_MANGO_L3_API_FLAG_NONE), errHandle, ret);
#endif
#endif

            /* remove the monitors if it has */
            RT_ERR_HDL(_dal_mango_ipmc_l3EntryStatMont_remove(unit, pEntry), errHandle, ret);

            /* unbind multicast group */
            RT_ERR_HDL(_dal_mango_mcast_ipmc_unbind(unit, pEntry->groupId, L3_ENTRY_ADDR_TO_IDX(unit, pEntry)), errHandle, ret);

            /* remove the l3Entry into used_l3Entry_list */
            RT_ERR_HDL(_dal_mango_ipmc_l3EntryUsedList_remove(unit, pEntry), errHandle, ret);
            RT_ERR_HDL(_dal_mango_ipmc_l3Entry_free(unit, pEntry), errHandle, ret);

            MANGO_IPMCAST_DBG_PRINTF(1, "remove OK, ret = %d\n", ret);
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
 *      dal_mango_ipmc_addr_getNext
 * Description:
 *      Get the next valid ipmcast  entry (based on the base index)
 * Input:
 *      unit      - unit id
 *      flags     - control flags
 *      pBase     - pointer to the starting valid entry number to be searched
 * Output:
 *      pBase     - pointer to the index of the returned entry (-1 means not found)
 *      pIpmcAddr - ipmcast entry (if found)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) Applicable flags:
 *          RTK_IPMC_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *      (2) Use base index -1 to indicate to search from the beginging of L3 host table
 *      (3) If the returned index is -1, means not found the next valid entry
 */
int32
dal_mango_ipmc_addr_getNext(uint32 unit, rtk_ipmc_flag_t flags, int32 *pBase, rtk_ipmc_addr_t *pIpmcAddr)
{
    int32   ret = RT_ERR_OK;
    int32   ipv6;
    int32   idx;
    dal_mango_ipmc_l3Entry_t    *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,flags=0x%08x,pBase=%p,pHost=%p", unit, flags, pBase, pIpmcAddr);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    ipv6 = (flags & RTK_IPMC_FLAG_IPV6) ? TRUE : FALSE;

    /* scan the whole L3 host table */
    for (idx=(*pBase<0)?(0):((*pBase)+1); idx<DAL_MANGO_IPMCAST_L3_ENTRY_MAX; idx++)
    {
        if (L3_ENTRY_IDX_IS_VALID(unit, idx))
        {
            pEntry = L3_ENTRY_ADDR(unit, idx);

            if (ipv6 != pEntry->ipv6)
                continue;

            RT_ERR_HDL(_dal_mango_ipmc_rtkIpmcAddr_get(unit, pIpmcAddr, pEntry), errHandle, ret);

            *pBase = idx;

            goto errOk;
        }
    }

    *pBase = -1;    /* Not found any specified entry */

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_ipmc_statMont_create
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
 *      RT_ERR_TBL_FULL                 - table is full
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 *          pStatMont->mont_flags = RTK_IPMCAST_STAT_MONT_FLAG_BYTE_AND_PACKET wil use two counter entry
 */
int32
dal_mango_ipmc_statMont_create(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
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
    uint32  montFreeIdx[MANGO_IPMC_STAT_MONT_ONETIME_MAX];
    uint32  montEntryIdx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_mango_ipmc_hashIdx_t hashIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatMont=%p", unit, pStatMont);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatMont), RT_ERR_NULL_POINTER);

    /* function body */
    RT_ERR_CHK(_dal_mango_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, &pStatMont->key), ret);
    RT_ERR_CHK(_dal_mango_ipmc_hashIdx_get(unit, &ipmcAddr, &hashIdx), ret);
    signKey = _dal_mango_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryAddr */
    l3EntryAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    l3EntryAddr |= (TRUE == pEntry->cam)? (0x1 << 14) : 0;

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
    for (idx=0; idx<DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryAddr == l3EntryAddr)
            {
                if (MANGO_IPMC_STAT_MONT_MODE_PKT_CNT == _pIpmcDb[unit]->mont[idx].mode)
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
            if (montFreeNum < MANGO_IPMC_STAT_MONT_ONETIME_MAX)
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
        MANGO_IPMCAST_DBG_PRINTF(1, "montFreeNum = %u, montReqNum = %u\n", montFreeNum, montReqNum);
        ret = RT_ERR_TBL_FULL;
        goto errHandle;
    }

    /* create byte monitor if required */
    if ((pStatMont->mont_flags & RTK_IPMCAST_STAT_MONT_FLAG_BYTE) && (FALSE == byteMontExist))
    {
        montEntryIdx = montFreeIdx[montUseNum];
        _pIpmcDb[unit]->mont[montEntryIdx].valid = TRUE;
        _pIpmcDb[unit]->mont[montEntryIdx].mode = 1;
        _pIpmcDb[unit]->mont[montEntryIdx].l3EntryAddr = l3EntryAddr;
        montUseNum += 1;

        pEntry->statMont_byte_valid = TRUE;
        pEntry->statMont_byte_idx = montEntryIdx;

        /* sync to H/W, and reset the counter */
        RT_ERR_HDL(_dal_mango_ipmc_statMont_sync(unit, montEntryIdx), errHandle, ret);
        RT_ERR_HDL(_dal_mango_ipmc_statMont_reset(unit, montEntryIdx), errHandle, ret);
    }

    /* create packet monitor if required */
    if ((pStatMont->mont_flags & RTK_IPMCAST_STAT_MONT_FLAG_PACKET) && (FALSE == pktMontExist))
    {
        montEntryIdx = montFreeIdx[montUseNum];
        _pIpmcDb[unit]->mont[montEntryIdx].valid = TRUE;
        _pIpmcDb[unit]->mont[montEntryIdx].mode = 0;
        _pIpmcDb[unit]->mont[montEntryIdx].l3EntryAddr = l3EntryAddr;
        montUseNum += 1;

        pEntry->statMont_pkt_valid = TRUE;
        pEntry->statMont_pkt_idx = montFreeNum;

        /* sync to H/W, and reset the counter */
        RT_ERR_HDL(_dal_mango_ipmc_statMont_sync(unit, montEntryIdx), errHandle, ret);
        RT_ERR_HDL(_dal_mango_ipmc_statMont_reset(unit, montEntryIdx), errHandle, ret);
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_ipmc_statMont_destroy
 * Description:
 *      Delete an ipmc monitor counter entry for ipmcast entry
 * Input:
 *      unit      - unit id
 *      pStatMont - pointer to rtk_ipmc_statMont_t containing the basic inputs
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found (when try to replace)
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 *          pStatMont->mont_flags = 0,will also remove the counter entry
 *          delete an ipmcast entry,should destroy the monitor entry
 */
int32
dal_mango_ipmc_statMont_destroy(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    uint32  l3EntryAddr;
    uint32  idx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_mango_ipmc_hashIdx_t hashIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatMont=%p", unit, pStatMont);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatMont), RT_ERR_NULL_POINTER);

    /* function body */
    RT_ERR_CHK(_dal_mango_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, &pStatMont->key), ret);
    RT_ERR_CHK(_dal_mango_ipmc_hashIdx_get(unit, &ipmcAddr, &hashIdx), ret);
    signKey = _dal_mango_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryAddr */
    l3EntryAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    l3EntryAddr |= (TRUE == pEntry->cam)? (0x1 << 14) : 0;

    /* remove the existing monitor */
    for (idx=0; idx<DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryAddr == l3EntryAddr)
            {
                if (MANGO_IPMC_STAT_MONT_MODE_PKT_CNT == _pIpmcDb[unit]->mont[idx].mode)
                {
                    /* packet count */
                    if (RTK_IPMCAST_STAT_MONT_FLAG_PACKET & pStatMont->mont_flags)
                    {
                        /* remove the monitor */
                        _pIpmcDb[unit]->mont[idx].valid = FALSE;
                        _pIpmcDb[unit]->mont[idx].mode = 0;
                        _pIpmcDb[unit]->mont[idx].l3EntryAddr = 0;

                        pEntry->statMont_pkt_valid = FALSE;
                        pEntry->statMont_pkt_idx = 0;

                        /* sync to H/W */
                        RT_ERR_HDL(_dal_mango_ipmc_statMont_sync(unit, idx), errHandle, ret);
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
                        _pIpmcDb[unit]->mont[idx].l3EntryAddr = 0;

                        pEntry->statMont_byte_valid = FALSE;
                        pEntry->statMont_byte_idx = 0;

                        /* sync to H/W */
                        RT_ERR_HDL(_dal_mango_ipmc_statMont_sync(unit, idx), errHandle, ret);
                    }
                }
            }
        }
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_ipmc_statCntr_reset
 * Description:
 *      reset the ipmc monitor counter
 * Input:
 *      unit - unit id
 *      pKey - pointer to rtk_ipmc_statKey_t of ipmcast entry key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found (when try to replace)
 * Note:
 *
 */
int32
dal_mango_ipmc_statCntr_reset(uint32 unit, rtk_ipmc_statKey_t *pStatKey)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    uint32  l3EntryAddr;
    uint32  idx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_mango_ipmc_hashIdx_t hashIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatKey=%p", unit, pStatKey);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pStatKey->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    RT_ERR_CHK(_dal_mango_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, pStatKey), ret);
    RT_ERR_CHK(_dal_mango_ipmc_hashIdx_get(unit, &ipmcAddr, &hashIdx), ret);
    signKey = _dal_mango_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryAddr */
    l3EntryAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    l3EntryAddr |= (TRUE == pEntry->cam)? (0x1 << 14) : 0;

    /* remove the existing monitor */
    for (idx=0; idx<DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryAddr == l3EntryAddr)
            {
                /* reset the count */
                RT_ERR_HDL(_dal_mango_ipmc_statMont_reset(unit, idx), errHandle, ret);
            }
        }
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_ipmc_statCntr_get
 * Description:
 *      Get counter and parameter of L3 counter
 * Input:
 *      unit  - unit id
 *      pKey  - pointer to rtk_ipmc_statKey_t of ipmcast entry key
 * Output:
 *      pCntr - pointer to counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found (when try to get)
 * Note:
 *
 */
int32
dal_mango_ipmc_statCntr_get(uint32 unit, rtk_ipmc_statKey_t *pStatKey, rtk_ipmc_statCntr_t *pCntr)
{
    int32   ret = RT_ERR_OK;
    uint32  signKey;
    uint32  l3EntryAddr;
    uint32  idx;
    rtk_ipmc_addr_t ipmcAddr;
    dal_mango_ipmc_hashIdx_t hashIdx;
    dal_mango_ipmc_l3Entry_t *pEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,pStatKey=%p", unit, pStatKey);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pStatKey->vrf_id >= HAL_MAX_NUM_OF_VRF(unit)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    RT_ERR_CHK(_dal_mango_ipmc_statKey2rtkIpmcAddr(&ipmcAddr, pStatKey), ret);
    RT_ERR_CHK(_dal_mango_ipmc_hashIdx_get(unit, &ipmcAddr, &hashIdx), ret);
    signKey = _dal_mango_ipmc_signKey_ret(unit, &ipmcAddr, &hashIdx);

    IPMCAST_SEM_LOCK(unit);

    RT_ERR_HDL(_dal_mango_ipmc_l3Entry_find_withSignKey(unit, &ipmcAddr, &pEntry, signKey), errHandle, ret);
    if (NULL == pEntry)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* calculation of l3EntryAddr */
    l3EntryAddr = DAL_MANGO_L3_ENTRY_IDX_TO_ADDR(pEntry->hw_entry_idx);
    l3EntryAddr |= (TRUE == pEntry->cam)? (0x1 << 14) : 0;

    /* remove the existing monitor */
    for (idx=0; idx<DAL_MANGO_IPMCAST_STAT_MONT_ENTRY_NUM; idx++)
    {
        if (MONT_ENTRY_IDX_IS_VALID(unit, idx))
        {
            if (_pIpmcDb[unit]->mont[idx].l3EntryAddr == l3EntryAddr)
            {
                /* get the byte/packet count */
                RT_ERR_HDL(_dal_mango_ipmc_statMontCntr_get(unit, idx, pCntr), errHandle, ret);
            }
        }
    }

    goto errOk;

errHandle:
errOk:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_ipmc_globalCtrl_get
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
dal_mango_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_ipmc_act_t act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,type=%d,pArg=%p", unit, type, pArg);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_IPMC_GCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_IPMC_GCT_LU_HASH_WT_INTF_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_LU_FORCE_MODE_HASH_KEY_SELf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IPMC_GLB_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IPMC_BAD_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_ZERO_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlDmacMismatch, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_HDR_OPT_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_HDR_OPT_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlHdrOpt, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_SRC_INTF_FLTR_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IPMC_MTU_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_TTL_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIpmcRouteCtrlTtlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IPMC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_optIpmcRouteCtrlPktToCPUTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    /* IPV6 */
    case RTK_IPMC_GCT_IP6MC_GLB_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_BAD_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlBadSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_ZERO_SIP_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlZeroSip, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlDmacMismatch, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HBH_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHbh, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ERR_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HBH_ERR_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHbhErr, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HDR_ROUTE_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HDR_ROUTE_ACTf, val, "", errExit, ret);\
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHdrRoute, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_SRC_INTF_FLTR_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_MTU_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlMtuFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HL_FAIL_ACT:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HL_FAIL_ACTf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_actIp6mcRouteCtrlHlFail, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_REG_FIELD_READ_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
            IPMCAST_VALUE_TO_ACTION(_optIp6mcRouteCtrlPktToCPUTarget, act, val, "", errExit, ret);
            *pArg = act;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_globalCtrl_get */


/* Function Name:
 *      dal_mango_ipmc_globalCtrl_set
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
dal_mango_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_IPMCAST), "unit=%d,type=%d,arg=0x%08x", unit, type, arg);

    /* check Init status */
    RT_INIT_CHK(ipmc_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_IPMC_GCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    IPMCAST_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_IPMC_GCT_LU_HASH_WT_INTF_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_HOST_TBL_CTRLr, MANGO_LU_FORCE_MODE_HASH_KEY_SELf, val, "", errExit, ret);
            _pIpmcDb[unit]->l3TblHashKey = (val)? DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP_INTF : DAL_MANGO_IPMC_L3TBL_HASH_KEY_VRF_SIP_GIP;
            goto errOk;
        }
        break;

    case RTK_IPMC_GCT_IPMC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_IPMC_GCT_IPMC_BAD_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlBadSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_ZERO_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlZeroSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlDmacMismatch, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_HDR_OPT_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlHdrOpt, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_HDR_OPT_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_SRC_INTF_FLTR_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_SRC_INTF_FLTR_ENf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_IPMC_GCT_IPMC_MTU_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlMtuFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_TTL_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIpmcRouteCtrlTtlFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_TTL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IPMC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_ACTION_TO_VALUE(_optIpmcRouteCtrlPktToCPUTarget, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IPMC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_GLB_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_GLB_ENf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_BAD_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlBadSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_BAD_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_ZERO_SIP_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlZeroSip, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_ZERO_SIP_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_DMAC_MISMATCH_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlDmacMismatch, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_DMAC_MISMATCH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHbh, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HBH_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HBH_ERR_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHbhErr, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HBH_ERR_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HDR_ROUTE_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHdrRoute, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HDR_ROUTE_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_SRC_INTF_FLTR_EN:
        {
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_SRC_INTF_FLTR_ENf, val, "", errExit, ret);
            goto errOk;
        }
        break;

    case RTK_IPMC_GCT_IP6MC_MTU_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlMtuFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_MTU_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_HL_FAIL_ACT:
        {
            IPMCAST_ACTION_TO_VALUE(_actIp6mcRouteCtrlHlFail, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_HL_FAIL_ACTf, val, "", errExit, ret);
        }
        break;

    case RTK_IPMC_GCT_IP6MC_PKT_TO_CPU_TARGET:
        {
            IPMCAST_ACTION_TO_VALUE(_optIp6mcRouteCtrlPktToCPUTarget, val, arg, "", errExit, ret);
            IPMCAST_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L3_IP6MC_ROUTE_CTRLr, MANGO_PKT_TO_CPU_TARGETf, val, "", errExit, ret);
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errInput;
    }

errInput:
errExit:
errOk:
    IPMCAST_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_l3_globalCtrl_set */



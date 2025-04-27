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
 *            1) mcast
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/util/rt_list.h>
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
#include <dal/longan/dal_longan_mcast.h>
#include <dal/longan/dal_longan_ipmcast.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/mcast.h>
/*
 * Symbol Definition
 */
typedef struct dal_longan_mcast_group_s
{
        rtk_mcast_group_t   groupId;
        uint32  valid:1;        /* Group state: TRUE/FALSE */
#if 0
        uint32  ipmc_bind:1;    /* IPMC binding state: TRUE/FALSE */
        uint32  ipmc_entryIdx;     /* IPMC entry index (available when it's bound to an IPMC entry) */
        uint32  ipmc_flags;     /* IPMC flags (available when it's bound to an IPMC entry) */
#endif
        int32   l2_fwdIndex;    /* fowrding portmask index of L2 bridging (-1 means invalid) */
        rtk_portmask_t  stk_pmsk;   /*L3  stacking forward portmask*/

        /* associated entries */
        RTK_LIST_DEF(dal_longan_mcast_l3Node_s, l3_entry_list);  /* associated L3 entries */

        RTK_LIST_DEF(dal_longan_mcast_oil_s, oil_list);

        RTK_LIST_NODE_REF_DEF(dal_longan_mcast_group_s, nodeRef);
}dal_longan_mcast_group_t;

typedef struct dal_longan_mcast_oil_s
{
    uint32 sortKey;
    uint32 valid :1;
    uint32 egrif_type:4;
    uint32 bridge_mode:1;  //for TTL_DEC = 0, TTL_CHK = 0, SA_REPLACE = 0
    uint32 l3_egr_intf_idx :7;  //9300 128 entry

    int32   fwdIndex;
    RTK_LIST_NODE_REF_DEF(dal_longan_mcast_oil_s, nodeRef);
} dal_longan_mcast_oil_t;

/* L3MC node */
typedef struct dal_longan_mcast_l3Node_s
{
    uint32  ipmc_entryIdx;  /* IPMC entry index */
    uint32  ipmc_flags;     /* IPMC flags  */

    /* node of link list */
    RTK_LIST_NODE_REF_DEF(dal_longan_mcast_l3Node_s, nodeRef);
} dal_longan_mcast_l3Node_t;

typedef struct dal_longan_mcast_drvDb_s
{
    struct
    {
        /* hardware resource (for internal APIs) */
        uint32 oil_used_count;
        RTK_LIST_DEF(dal_longan_mcast_oil_s, free_oil_list);
        dal_longan_mcast_oil_t   oil[DAL_LONGAN_MCAST_OIL_MAX];

        /* IPMC entry node */
        uint32 l3IpmcNode_used_count;
        RTK_LIST_DEF(dal_longan_mcast_l3Node_s, free_l3Node_list);
        dal_longan_mcast_l3Node_t       l3Node[DAL_LONGAN_MCAST_L3_NODE_MAX];
    } HW;

    RTK_LIST_DEF(dal_longan_mcast_group_s, free_group_list);
    dal_longan_mcast_group_t group[DAL_LONGAN_MCAST_GROUP_MAX];
} dal_longan_mcast_drvDb_t;


/* structure of multicast OIL entry */
typedef struct dal_mcast_oilEntry_s {
    int32       oil_next;               /* index of the next oil */
    uint32      eol;                    /* end of list */
    uint32      ttl_dec;                /* TTL decrease */
    uint32      ttl_chk;                /* TTl check */
    uint32      sa_replace;             /* SMAC replacement */
    uint32      mc_pmsk_idx;            /* (multicast portmask) idnex */
    uint32      l3_egr_intf_idx;        /* L3 egress inteface */
} dal_longan_mcast_oilEntry_t;


#define MCAST_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(mcast_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define MCAST_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(mcast_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L3), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define MCAST_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                    \
    if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)               \
    {                                                                                   \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                     \
        goto _gotoErrLbl;                                                               \
    }                                                                                   \
} while(0)
#define MCAST_REG_FIELD_WRITE_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                    \
    if ((_ret = reg_field_write(_unit, _reg, _field, &_val)) != RT_ERR_OK)              \
    {                                                                                   \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                     \
        goto _gotoErrLbl;                                                               \
    }                                                                                   \
} while(0)
#define MCAST_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                                        \
    if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)               \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                                         \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define MCAST_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                                        \
    if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)              \
    {                                                                                                       \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                                         \
        goto _gotoErrLbl;                                                                                   \
    }                                                                                                       \
} while(0)
#define MCAST_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                \
    if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)     \
    {                                                                               \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                 \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define MCAST_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret)   \
do {                                                                                \
    if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK)    \
    {                                                                               \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                 \
        goto _gotoErrLbl;                                                           \
    }                                                                               \
} while(0)
#define MCAST_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                                    \
    if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                                     \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)
#define MCAST_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret)    \
do {                                                                                                    \
    if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
    {                                                                                                   \
        RT_ERR(_ret, (MOD_MCAST|MOD_DAL), _errMsg);                                                     \
        goto _gotoErrLbl;                                                                               \
    }                                                                                                   \
} while(0)

#define MCAST_RT_ERR_HDL_DBG(_op, _args...) \
do {                                                        \
    if (RT_ERR_OK != (_op))                                 \
    {                                                       \
       RT_LOG(LOG_DEBUG, (MOD_MCAST|MOD_DAL), ## _args);    \
    }                                                       \
} while(0)

#define IS_GROUP_VALID(_unit, _grpIdx)    (TRUE == _pMcastDb[_unit]->group[_grpIdx].valid)

#define GROUP_VALID_SET(_unit, _grpIdx)         \
do { \
    _pMcastDb[_unit]->group[_grpIdx].valid = TRUE; \
} while (0)
#define GROUP_VALID_CLEAR(_unit, _grpIdx)       \
do { \
    _pMcastDb[_unit]->group[_grpIdx].valid = FALSE; \
} while (0)


#define GROUP_ENTRY_RESET(_pGrp)                                \
do {                                                            \
    osal_memset(_pGrp, 0x00, sizeof(dal_longan_mcast_group_t));  \
} while (0)

#define GROUP_ENTRY_VALIDATE(_pGrp) \
do {                                \
    (_pGrp)->valid = TRUE;          \
} while (0)

#define GROUP_ENTRY_INVALIDATE(_pGrp)   \
do {                                    \
    (_pGrp)->valid = FALSE;             \
} while (0)

#define GROUP_ENTRY_IS_VALID(_pGrp)             ((NULL != (_pGrp)) && (TRUE == (_pGrp)->valid))

#define GROUP_ENTRY_RESET_BY_ID(_unit, _grpIdx)   \
do {                                                                                        \
    osal_memset(&_pMcastDb[_unit]->group[_grpIdx], 0x00, sizeof(dal_longan_mcast_group_t));  \
    /*_pMcastDb[_unit]->group[_grpIdx].groupIdx = _grpIdx;*/                                \
} while (0)

#define GROUP_ENTRY_ADDR(_unit, _grpIdx)        (&_pMcastDb[_unit]->group[_grpIdx])
#define GROUP_ENTRY_ADDR_TO_IDX(_unit, _pGrp)   ((_pGrp) - (&_pMcastDb[_unit]->group[0]))
#define GROUP_IS_BIND_IPMC(_unit, _grpIdx)      (TRUE == _pMcastDb[_unit]->group[_grpIdx].bind_ipmc)

#define OIL_ENTRY_ADDR(_unit, _oilIdx)          (&_pMcastDb[_unit]->HW.oil[_oilIdx])
#define OIL_ENTRY_ADDR_TO_IDX(_unit, _pOil)     ((_pOil) - (&_pMcastDb[_unit]->HW.oil[0]))
#define OIL_ENTRY_IS_VALID(_pOil)               ((NULL != (_pOil)) && (TRUE == (_pOil)->valid))

#define OIL_ENTRY_RESET(_pOil)                                  \
do {                                                            \
    osal_memset(_pOil, 0x00, sizeof(dal_longan_mcast_oil_t));    \
} while (0)

#define OIL_ENTRY_VALIDATE(_pOil)   \
do {                                \
    (_pOil)->valid = TRUE;          \
} while (0)

#define L3_NODE_ADDR(_unit, _nodeIdx)           (&_pMcastDb[(_unit)]->HW.l3Node[(_nodeIdx)])
#define L3_NODE_ADDR_TO_IDX(_unit, _pL3Node)    ((_pL3Node) - (&_pMcastDb[(_unit)]->HW.l3Node[0]))
//#define L3_NODE_IS_VALID(_pL3Node)            ((NULL != (_pL2Node)) && (TRUE == (_pL2Node)->valid))
#define L3_NODE_RESET(_pL3Node)                                   \
    do {                                                                \
        osal_memset(_pL3Node, 0x00, sizeof(dal_longan_mcast_l3Node_t));  \
    } while (0)

/*
 * Data Declaration
 */

static uint32                   mcast_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         mcast_sem[RTK_MAX_NUM_OF_UNIT];
static uint32                   mcast_extSemLocked[RTK_MAX_NUM_OF_UNIT];

static dal_longan_mcast_drvDb_t         *_pMcastDb[RTK_MAX_NUM_OF_UNIT];


static inline int32
mcast_util_dalOil2oilEntry(uint32 unit, dal_longan_mcast_oilEntry_t *pEntry, dal_longan_mcast_oil_t *pOil)
{
    int32   ret = RT_ERR_OK;
    dal_longan_mcast_oil_t *pOilNext;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOil), RT_ERR_NULL_POINTER);

    /* get info of the next node */
    pOilNext = RTK_LIST_NODE_NEXT(pOil, nodeRef);
    if (NULL == pOilNext)
    {
        pEntry->oil_next = 0;
        pEntry->eol = 1;
    }
    else
    {
        pEntry->oil_next = OIL_ENTRY_ADDR_TO_IDX(unit, pOilNext);
        pEntry->eol = 0;
    }

    if (pOil->bridge_mode)
    {
        pEntry->ttl_dec = 0;
        pEntry->ttl_chk = 0;
        pEntry->sa_replace = 0;
    }
    else
    {
        pEntry->ttl_dec = 1;
        pEntry->ttl_chk = 1;
        pEntry->sa_replace = 1;
    }

    switch (pOil->egrif_type)
    {
        case RTK_MCAST_EGRIF_TYPE_L2:
            return RT_ERR_INPUT;

        case RTK_MCAST_EGRIF_TYPE_L3:
            {
                pEntry->l3_egr_intf_idx = pOil->l3_egr_intf_idx;
                pEntry->mc_pmsk_idx = pOil->fwdIndex;
            }
            break;

        case RTK_MCAST_EGRIF_TYPE_TUNNEL:
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return ret;
}


static inline int32
mcast_util_dalOil2rtkMcNh(uint32 unit, rtk_mcast_egrif_t *pEgrIf, dal_longan_mcast_oil_t *pOil)
{
    int32 ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOil), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEgrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    pEgrIf->type = pOil->egrif_type;

    switch (pEgrIf->type)
    {
    case RTK_MCAST_EGRIF_TYPE_L2:
        {
            ;
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_L3:
        {
            pEgrIf->l3.intf_id  = pOil->l3_egr_intf_idx;
            pEgrIf->l3.fwdIndex = pOil->fwdIndex;

            if (TRUE == pOil->bridge_mode)
                pEgrIf->flags = RTK_MCAST_EGRIF_FLAG_BRIDGE;

            RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pOil->fwdIndex, &pEgrIf->l3.portmask), errHandle, ret);
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_TUNNEL:
    default:
        {
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            goto errHandle;
        }
    }

    goto errOk;

errHandle:
errOk:

    return ret;
}


static inline int32 _dal_longan_mcast_group_alloc(uint32 unit, dal_longan_mcast_group_t **ppGroup)
{
    dal_longan_mcast_group_t *pGroup;

    if (RTK_LIST_EMPTY(&_pMcastDb[unit]->free_group_list))
    {
        return RT_ERR_TBL_FULL;
    }

    pGroup = RTK_LIST_NODE_HEAD(&_pMcastDb[unit]->free_group_list);
    if (NULL == pGroup)
    {
        return RT_ERR_FAILED;   /* should not happen */
    }
    RTK_LIST_NODE_REMOVE(&_pMcastDb[unit]->free_group_list, pGroup, nodeRef);
    RTK_LIST_NODE_REF_INIT(pGroup, nodeRef);
    GROUP_ENTRY_VALIDATE(pGroup);

    *ppGroup = pGroup;

    return RT_ERR_OK;
}

static inline int32 _dal_longan_mcast_group_free(uint32 unit, dal_longan_mcast_group_t *pGroup)
{
    if (GROUP_ENTRY_IS_VALID(pGroup))
    {
        GROUP_ENTRY_RESET(pGroup);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->free_group_list, pGroup, nodeRef);

        return RT_ERR_OK;
    }

    return RT_ERR_ENTRY_NOTFOUND;
}

static inline int32 _dal_longan_mcast_group_alloc_byIdx(uint32 unit, dal_longan_mcast_group_t **ppGroup, uint32 index)
{
    dal_longan_mcast_group_t *pGroup;

    if (GROUP_ENTRY_IS_VALID(&_pMcastDb[unit]->group[index]))
    {
        return RT_ERR_ENTRY_EXIST;
    }

    pGroup = &_pMcastDb[unit]->group[index];
    RTK_LIST_NODE_REMOVE(&_pMcastDb[unit]->free_group_list, pGroup, nodeRef);
    GROUP_ENTRY_VALIDATE(pGroup);

    *ppGroup = pGroup;

    return RT_ERR_OK;
}

/* the input param valid check before
*groupIdx:12bit
*grouptype:2bit
*nhTYpe:2bit
*intf_id 10:bit
*isbrdg 1:bit
*/
/* to get the unique sort key */
static inline uint32
_dal_longan_mcast_oil_sortKey_ret(rtk_mcast_group_t groupId,rtk_mcast_egrif_t *pEgrIf)
{
    uint32 sortKey = 0;
    uint32                  groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);

    RT_PARAM_CHK((NULL == pEgrIf), 0); /* return key as zero */

    sortKey =   (((pEgrIf->type) & 0xFF) << 24) | \
                      (((pEgrIf->l3.intf_id) & 0xFFF) << 12) | \
                      (((groupIdx) & 0x7FF) << 1) | \
                      ((pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_BRIDGE)? 0x0 : 0x1);

    return sortKey;
}

static inline int32 _dal_longan_mcast_oil_alloc(uint32 unit, dal_longan_mcast_oil_t **ppOil)
{
    dal_longan_mcast_oil_t *pOil;

    if (RTK_LIST_EMPTY(&_pMcastDb[unit]->HW.free_oil_list))
    {
        return RT_ERR_TBL_FULL;
    }

    pOil = RTK_LIST_NODE_HEAD(&_pMcastDb[unit]->HW.free_oil_list);
    if (NULL == pOil)
    {
        return RT_ERR_FAILED;   /* should not happen */
    }
    RTK_LIST_NODE_REMOVE(&_pMcastDb[unit]->HW.free_oil_list, pOil, nodeRef);
    RTK_LIST_NODE_REF_INIT(pOil, nodeRef);
    OIL_ENTRY_VALIDATE(pOil);

    *ppOil = pOil;

    return RT_ERR_OK;
}

static inline int32 _dal_longan_mcast_oil_free(uint32 unit, dal_longan_mcast_oil_t *pOil)
{
    if (OIL_ENTRY_IS_VALID(pOil))
    {
        OIL_ENTRY_RESET(pOil);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_oil_list, pOil, nodeRef);

        return RT_ERR_OK;
    }

    return RT_ERR_ENTRY_NOTFOUND;
}

static int32
_dal_longan_mcast_oilEntry_set(uint32 unit, uint32 index,dal_longan_mcast_oilEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    l3_egr_iol_entry_t oilEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,index=%d,pEntry=%p", unit, index, pEntry);

    /* parameter check */
    RT_PARAM_CHK(index >= DAL_LONGAN_MCAST_OIL_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */

    osal_memset(&oilEntry, 0x00, sizeof(l3_egr_iol_entry_t));
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, \
        LONGAN_L3_EGR_INTF_LIST_TTL_DECtf, pEntry->ttl_dec, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, \
        LONGAN_L3_EGR_INTF_LIST_TTL_CHKtf, pEntry->ttl_chk, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, \
        LONGAN_L3_EGR_INTF_LIST_SA_REPLACEtf, pEntry->sa_replace, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, \
        LONGAN_L3_EGR_INTF_LIST_OIL_NEXTtf, pEntry->oil_next, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, \
        LONGAN_L3_EGR_INTF_LIST_EOLtf, pEntry->eol, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, \
        LONGAN_L3_EGR_INTF_LIST_MC_PMSK_IDXtf, pEntry->mc_pmsk_idx, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, \
        LONGAN_L3_EGR_INTF_LIST_L3_EGR_INTF_BSSID_IDXtf, pEntry->l3_egr_intf_idx, oilEntry, "", errHandle, ret);

    MCAST_TABLE_WRITE_ERR_HDL(unit, LONGAN_L3_EGR_INTF_LISTt, index, oilEntry, "", errHandle, ret);

errHandle:

    return ret;
}


static inline int32
_dal_longan_mcast_ipEgrIfOil_insert(uint32 unit, dal_longan_mcast_group_t *pGroup, dal_longan_mcast_oil_t *pOil, dal_longan_mcast_oil_t *pOilPrev)
{
    int32   ret = RT_ERR_OK;
    int32   oilIdx;
    dal_longan_mcast_oilEntry_t oilEntry;
    dal_longan_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOil), RT_ERR_NULL_POINTER);

    if (NULL == pOilPrev)
    {
        /* add to head */
        RTK_LIST_NODE_INSERT_HEAD(&pGroup->oil_list, pOil, nodeRef);

        /* sync to H/W */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOil), errHandle, ret);
        RT_ERR_HDL(_dal_longan_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);

#if 0
        /* update to IPMCAST if group is bound */
        if (TRUE == pGroup->ipmc_bind)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l3OilIdx_update(unit, pGroup->ipmc_entryIdx, oilIdx), errHandle, ret);
        }
#else
        RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l3OilIdx_update(unit, pL3Node->ipmc_entryIdx, oilIdx), errHandle, ret);
        }
#endif
    }
    else
    {
        /* add new_oil to the list:
         * I think it should add to the last of the list
         * so the traffic in current OIL list will not break.)
         */
        RTK_LIST_NODE_INSERT_AFTER(&pGroup->oil_list, pOilPrev, pOil, nodeRef);

        /* sync to H/W */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOil), errHandle, ret);
        RT_ERR_HDL(_dal_longan_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOilPrev);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOilPrev), errHandle, ret);
        RT_ERR_HDL(_dal_longan_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
    }

errHandle:
    return ret;
}

static inline int32
_dal_longan_mcast_ipEgrIfOil_remove(uint32 unit, dal_longan_mcast_group_t *pGroup, dal_longan_mcast_oil_t *pOil)
{
    int32   ret = RT_ERR_OK;
    int32   oilIdx;
    dal_longan_mcast_oil_t *pOilPrev;
    dal_longan_mcast_oil_t *pOilNext;
    dal_longan_mcast_oilEntry_t oilEntry;
    dal_longan_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOil), RT_ERR_NULL_POINTER);

    if (pOil->fwdIndex >= 0)
        RT_ERR_HDL(_dal_longan_l2_mcastFwdIndex_free(unit, pOil->fwdIndex), errHandle, ret);

    /* update OIL node */
    pOil->fwdIndex = DAL_LONGAN_MCAST_INVALID_FWD_IDX;

    /* update OIL list */
    if (pOil == RTK_LIST_NODE_HEAD(&pGroup->oil_list))
    {
        /* need to change the head */
        pOilNext = RTK_LIST_NODE_NEXT(pOil, nodeRef);
        oilIdx = (NULL != pOilNext)? OIL_ENTRY_ADDR_TO_IDX(unit, pOilNext) : DAL_LONGAN_MCAST_INVALID_OIL_IDX;

#if 0
        /* update IPMC entry if it's bound */
        if (TRUE == pGroup->ipmc_bind)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l3OilIdx_update(unit, pGroup->ipmc_entryIdx, oilIdx), errHandle, ret);
        }
#else
        RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l3OilIdx_update(unit, pL3Node->ipmc_entryIdx, oilIdx), errHandle, ret);
        }
#endif

        /* remove OIL node from oil_list */
        RTK_LIST_NODE_REMOVE(&pGroup->oil_list, pOil, nodeRef);
        RT_ERR_HDL(_dal_longan_mcast_oil_free(unit, pOil), errHandle, ret);
    }
    else
    {
        /* need to update the previous node */
        pOilPrev = RTK_LIST_NODE_PREV(pOil, nodeRef);

        /* remove OIL node from oil_list */
        RTK_LIST_NODE_REMOVE(&pGroup->oil_list, pOil, nodeRef);
        RT_ERR_HDL(_dal_longan_mcast_oil_free(unit, pOil), errHandle, ret);

        /* sync to H/W */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOilPrev);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOilPrev), errHandle, ret);
        RT_ERR_HDL(_dal_longan_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
    }

errHandle:
    return ret;
}



int32
_dal_longan_mcast_ipEgrIfL2_add(uint32 unit, dal_longan_mcast_group_t  *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    int32 old_pmsk_idx = DAL_LONGAN_MCAST_INVALID_FWD_IDX;
    rtk_portmask_t fwd_pmsk;
    int32 fwdIndexAllocated = FALSE;
    dal_longan_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pEgrIf->l2.fwdIndex < 0) && !HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask)), RT_ERR_PORT_MASK);

       /* caller has specified a fwd-pmsk index */
    if (pEgrIf->l2.fwdIndex >= 0)
    {
        if (pEgrIf->l2.fwdIndex != pGroup->l2_fwdIndex)
        {
            old_pmsk_idx = pGroup->l2_fwdIndex;
            /* just update the index, we don't update the portmask
             * because we expect user already update the portmask.
             */
            pGroup->l2_fwdIndex = pEgrIf->l2.fwdIndex;
            RT_ERR_HDL(_dal_longan_l2_mcastFwdIndex_alloc(unit, &pEgrIf->l2.fwdIndex), errHandle, ret);
            fwdIndexAllocated = TRUE;
        }
    }
    else
    {
        /* if portmask is not allocated yet, allocate a new one */
        old_pmsk_idx = pGroup->l2_fwdIndex;
        if (old_pmsk_idx < 0)
        {
            RT_ERR_HDL(_dal_longan_l2_mcastFwdIndex_alloc(unit, &pEgrIf->l2.fwdIndex), errHandle, ret);
            fwdIndexAllocated = TRUE;
            pGroup->l2_fwdIndex = pEgrIf->l2.fwdIndex;
        }
        /* if RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE is set or the portmask is newly created,
         * assign ports from user assigned value
         */
        if ((TRUE == fwdIndexAllocated) || (pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE))
        {
            /* Use input port_mask replace to HW_port_mask: */
            RTK_PORTMASK_ASSIGN(fwd_pmsk, pEgrIf->l2.portmask);
        }
        else
        {
            /* add ports in port_mask to HW: */
            RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pGroup->l2_fwdIndex, &fwd_pmsk), errHandle, ret);
            RTK_PORTMASK_OR(fwd_pmsk, pEgrIf->l2.portmask);
        }
        /* write hw_portmask to PORTMASK table of pmsk_idx */
        RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_set(unit, pGroup->l2_fwdIndex, &fwd_pmsk), errHandle, ret);
    }

    if (TRUE == fwdIndexAllocated)
    {
#if 0
        /* sync to H/W if it's bound */
        if (TRUE == pGroup->ipmc_bind)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l2PmskIdx_update(unit, pGroup->ipmc_entryIdx, pEgrIf->l2.fwdIndex), errHandle, ret);
        }
#else
        RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l2PmskIdx_update(unit, pL3Node->ipmc_entryIdx, pEgrIf->l2.fwdIndex), errHandle, ret);
        }
#endif

        /* free the old index if the new one has been created */
        if (old_pmsk_idx >= 0)
        {
            MCAST_RT_ERR_HDL_DBG(_dal_longan_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
        }
    }

    goto errOk;

errHandle:
    if (fwdIndexAllocated)
        MCAST_RT_ERR_HDL_DBG(_dal_longan_l2_mcastFwdIndex_free(unit, pEgrIf->l2.fwdIndex), "");

errOk:
    return ret;
}

int32
_dal_longan_mcast_ipEgrIfL3_add(uint32 unit, dal_longan_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   oilFound = FALSE;
    int32   oilCreated = FALSE;
    int32   old_pmsk_idx = DAL_LONGAN_MCAST_INVALID_FWD_IDX;
    int32   fwdIndexAllocated = FALSE;
    uint32  sortKey, oilIdx;
    dal_longan_mcast_oil_t *pOilPrev = NULL;
    dal_longan_mcast_oil_t *pOil;
    dal_longan_mcast_oilEntry_t oilEntry;
    rtk_portmask_t fwd_pmsk;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pEgrIf->l3.intf_id >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(((pEgrIf->l3.fwdIndex < 0) && !HWP_PMSK_EXIST(unit, &pEgrIf->l3.portmask)), RT_ERR_PORT_MASK);

    /* function body */
    sortKey = _dal_longan_mcast_oil_sortKey_ret(pGroup->groupId, pEgrIf);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->oil_list, pOil, nodeRef)
    {
        if (pOil->sortKey > sortKey)
            break;

        if (pOil->sortKey == sortKey)
        {
            oilFound = TRUE;
            old_pmsk_idx = pOil->fwdIndex;
            break;
        }

        pOilPrev = pOil;    /* candidate of the previous node for inserting a new node */
    }

    /* create a new oil node if entry does not exist yet */
    if (FALSE == oilFound)
    {
        RT_ERR_HDL(_dal_longan_mcast_oil_alloc(unit, &pOil), errHandle, ret);
        oilCreated = TRUE;

        /* initialize the new oil information, MC_PMSK_IDX must set to invalid first */
        pOil->sortKey = sortKey;
        pOil->egrif_type = RTK_MCAST_EGRIF_TYPE_L3;
        pOil->bridge_mode = (pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_BRIDGE)? TRUE : FALSE;
        pOil->l3_egr_intf_idx = pEgrIf->l3.intf_id;

        /* if the node is newly create, there must be no port mask index here. */
        old_pmsk_idx = DAL_LONGAN_MCAST_INVALID_FWD_IDX;
    }

    /* caller has specified a fwd-pmsk index */
    if (pEgrIf->l3.fwdIndex >= 0)
    {
        /* just update the index, we don't update the portmask
         * because we expect user already update the portmask.
         */
        RT_ERR_HDL(_dal_longan_l2_mcastFwdIndex_alloc(unit, &pEgrIf->l3.fwdIndex), errHandle, ret);
        fwdIndexAllocated = TRUE;

        pOil->fwdIndex = pEgrIf->l3.fwdIndex;
    }
    else
    {
        /* for the new allocated oil_node, portmask has not allocated yet, allocate a new one: */
        if (FALSE == oilFound)
        {
            RT_ERR_HDL(_dal_longan_l2_mcastFwdIndex_alloc(unit, &pEgrIf->l3.fwdIndex), errHandle, ret);
            fwdIndexAllocated = TRUE;

            pOil->fwdIndex = pEgrIf->l3.fwdIndex;
        }

        /* if RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE is set or the portmask is newly created,
         * assign ports from user assigned value
         */
        if ((TRUE == fwdIndexAllocated) || (pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE))
        {
            /* use input port_mask to replace HW_port_mask */
            RTK_PORTMASK_ASSIGN(fwd_pmsk, pEgrIf->l3.portmask);
        }
        else
        {
            /* add ports in port_mask to HW: */
            RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pOil->fwdIndex, &fwd_pmsk), errHandle, ret);
            RTK_PORTMASK_OR(fwd_pmsk, pEgrIf->l3.portmask);
        }

        /* write hw_port_mask to PORTMASK table of oil_node.MC_PMSK_IDX */
        RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_set(unit, pOil->fwdIndex, &fwd_pmsk), errHandle, ret);
    }

    /* Update linked list and sync to H/W entry */
    if (FALSE == oilFound)
    {
        /* The following codes that add node to list can integrate to a API for all add functions to call. */
        /* add new oil to the list */
        RT_ERR_HDL(_dal_longan_mcast_ipEgrIfOil_insert(unit, pGroup, pOil, pOilPrev), errHandle, ret);
    }
    else
    {
        /* update the OIL entry to H/W entry */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOil), errHandle, ret);
        RT_ERR_HDL(_dal_longan_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
    }

    /* free the old fwd-index if new one is created */
    if ((TRUE == fwdIndexAllocated) && (old_pmsk_idx >= 0))
    {
        MCAST_RT_ERR_HDL_DBG(_dal_longan_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
    }

    goto errOk;

errHandle:
    /* free the created fwdIndex */
    if (fwdIndexAllocated)
        MCAST_RT_ERR_HDL_DBG(_dal_longan_l2_mcastFwdIndex_free(unit, pEgrIf->l3.fwdIndex), "");

    /* free the created OIL node */
    if (oilCreated)
        MCAST_RT_ERR_HDL_DBG(_dal_longan_mcast_oil_free(unit, pOil), "");

errOk:
    return ret;
}

int32
_dal_longan_mcast_ipEgrIfStk_add(uint32 unit, dal_longan_mcast_group_t  *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    uint32 val = 0;
    dal_longan_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->stk.portmask), RT_ERR_PORT_MASK);

    if (pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE)
    {
        RTK_PORTMASK_ASSIGN(pGroup->stk_pmsk, pEgrIf->stk.portmask);
    }
    else
    {
        RTK_PORTMASK_OR(pGroup->stk_pmsk, pEgrIf->stk.portmask);
    }

#if 0
    if (TRUE == pGroup->ipmc_bind)
    {
        rt_util_upinkPort_mask2Reg(unit, &pGroup->stk_pmsk, &val);

        RT_ERR_HDL(_dal_longan_ipmc_stkPmsk_update(unit, pGroup->ipmc_entryIdx, val), errHandle, ret);
    }
#else
    RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
    {
        rt_util_upinkPort_mask2Reg(unit, &pGroup->stk_pmsk, &val);
        RT_ERR_HDL(_dal_longan_ipmc_stkPmsk_update(unit, pL3Node->ipmc_entryIdx, val), errHandle, ret);
    }
#endif

errHandle:
    return ret;
}

int32
_dal_longan_mcast_ipEgrIfL2_del(uint32 unit, dal_longan_mcast_group_t  *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    int32   needFreeL2PmskIdx = FALSE;
    rtk_portmask_t fwd_pmsk;
    dal_longan_mcast_l3Node_t *pL3Node;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask), RT_ERR_PORT_MASK);

    if (pEgrIf->l2.fwdIndex >= 0)
    {
        if(pGroup->l2_fwdIndex != pEgrIf->l2.fwdIndex)
        {
            //index no match, return error
            return RT_ERR_INPUT;
        }

        needFreeL2PmskIdx = TRUE;
    }
    else
    {
        /* delete ports */
        if(pGroup->l2_fwdIndex < 0)
        {
            /* portmask is never allocated, simply return ok */
            goto errOk;
        }

        RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pGroup->l2_fwdIndex, &fwd_pmsk), errHandle,ret);
        /* remove user specified ports: */
        RTK_PORTMASK_REMOVE(fwd_pmsk, pEgrIf->l2.portmask);

        /* if all ports are zero, free the port_mask; otherwise, update to PORTMASK table */
        if (0 == RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk))
        {
            /* release the portmask from ipmc table */
            needFreeL2PmskIdx = TRUE;
        }
        else
        {
            /* write hwp_port_mask to PORTMASK table of pGrpDb->l2_fwdIndex */
            RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_set(unit, pGroup->l2_fwdIndex, &fwd_pmsk), errHandle, ret);
            goto errOk;
        }
    }

    /* free the portmask index if need */
    if (TRUE == needFreeL2PmskIdx)
    {
#if 0
        /* first, sync to H/W entry if need */
        if (TRUE == pGroup->ipmc_bind)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l2PmskIdx_update(unit, pGroup->ipmc_entryIdx, DAL_LONGAN_MCAST_INVALID_FWD_IDX), errHandle, ret);
        }
#else
        RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
        {
            RT_ERR_HDL(_dal_longan_ipmc_l2PmskIdx_update(unit, pL3Node->ipmc_entryIdx, DAL_LONGAN_MCAST_INVALID_FWD_IDX), errHandle, ret);
        }
#endif

        /* free the fwd-index */
        RT_ERR_HDL(_dal_longan_l2_mcastFwdIndex_free(unit, pGroup->l2_fwdIndex), errHandle, ret);
        pGroup->l2_fwdIndex = DAL_LONGAN_MCAST_INVALID_FWD_IDX;
    }

    goto errOk;

errHandle:
errOk:
    return ret;
}

int32
_dal_longan_mcast_ipEgrIfL3_del(uint32 unit, dal_longan_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   oilFound = FALSE;
    int32   needFreeFwdIndex = FALSE;
    uint32  sortKey;
    dal_longan_mcast_oil_t *pOil;
    rtk_portmask_t fwd_pmsk;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pEgrIf->l3.intf_id >= HAL_MAX_NUM_OF_INTF(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l3.portmask), RT_ERR_PORT_MASK);

    /* function body */
    sortKey = _dal_longan_mcast_oil_sortKey_ret(pGroup->groupId, pEgrIf);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->oil_list, pOil, nodeRef)
    {
        if (pOil->sortKey > sortKey)
            break;

        if (pOil->sortKey == sortKey)
        {
            oilFound = TRUE;
            break;
        }
    }

    /* return error if entry is not found */
    if (FALSE == oilFound)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* caller has specified a fwd-pmsk index */
    if (pEgrIf->l3.fwdIndex >= 0)
    {
        if (pOil->fwdIndex != pEgrIf->l3.fwdIndex)
        {
            /* index no match, return error */
            return RT_ERR_INPUT;
        }
        /*user should free fwdindex*/
        needFreeFwdIndex = TRUE;
        pOil->fwdIndex = DAL_LONGAN_MCAST_INVALID_FWD_IDX;
    }
    else
    {
        /* delete ports */
        if (pOil->fwdIndex < 0)
        {
            /* portmask is never allocated, simply return ok */
            goto errOk;
        }

        RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pOil->fwdIndex, &fwd_pmsk), errHandle,ret);
        /* remove user specified ports: */
        RTK_PORTMASK_REMOVE(fwd_pmsk, pEgrIf->l3.portmask);

        /* if all ports are zero, free the port_mask; otherwise, update to PORTMASK table */
        if (0 == RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk))
        {
            needFreeFwdIndex = TRUE;
        }
        else
        {
            /* write hwp_port_mask to PORTMASK table of oil[delIdx].fwdIdx */
            RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_set(unit, pOil->fwdIndex, &fwd_pmsk), errHandle, ret);
            goto errOk;
        }

    }

    /* free the OIL if need to free the fwd-index */
    if (TRUE == needFreeFwdIndex)
    {
        RT_ERR_HDL(_dal_longan_mcast_ipEgrIfOil_remove(unit, pGroup, pOil), errHandle, ret);
    }

    goto errOk;

errHandle:
errOk:
    return ret;
}

int32
_dal_longan_mcast_ipEgrIfStk_del(uint32 unit, dal_longan_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    uint32 val = 0;
    dal_longan_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->stk.portmask), RT_ERR_PORT_MASK);

    RTK_PORTMASK_REMOVE(pGroup->stk_pmsk, pEgrIf->stk.portmask);

#if 0
    if (TRUE == pGroup->ipmc_bind)
    {
        rt_util_upinkPort_mask2Reg(unit, &pGroup->stk_pmsk, &val);

        RT_ERR_HDL(_dal_longan_ipmc_stkPmsk_update(unit, pGroup->ipmc_entryIdx, val), errHandle, ret);
    }
#else
    RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
    {
        rt_util_upinkPort_mask2Reg(unit, &pGroup->stk_pmsk, &val);
        RT_ERR_HDL(_dal_longan_ipmc_stkPmsk_update(unit, pL3Node->ipmc_entryIdx, val), errHandle, ret);
    }
#endif

errHandle:
    return ret;
}


static int32
_dal_longan_mcast_ipEgrIf_add(uint32 unit, dal_longan_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pEgrIf->type >= RTK_MCAST_EGRIF_TYPE_END), RT_ERR_INPUT);

    switch (pEgrIf->type)
    {
        case RTK_MCAST_EGRIF_TYPE_L2:
            ret = _dal_longan_mcast_ipEgrIfL2_add(unit, pGroup, pEgrIf);
            break;
        case RTK_MCAST_EGRIF_TYPE_L3:
            ret = _dal_longan_mcast_ipEgrIfL3_add(unit, pGroup, pEgrIf);
            break;
        case RTK_MCAST_EGRIF_TYPE_STK:
            ret = _dal_longan_mcast_ipEgrIfStk_add(unit, pGroup, pEgrIf);
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return ret;
}

static int32
_dal_longan_mcast_ipEgrIf_del(uint32 unit, dal_longan_mcast_group_t  *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pEgrIf->type >= RTK_MCAST_EGRIF_TYPE_END), RT_ERR_INPUT);

    switch (pEgrIf->type)
    {
        case RTK_MCAST_EGRIF_TYPE_L2:
            ret = _dal_longan_mcast_ipEgrIfL2_del(unit, pGroup, pEgrIf);
            break;
        case RTK_MCAST_EGRIF_TYPE_L3:
            ret = _dal_longan_mcast_ipEgrIfL3_del(unit, pGroup, pEgrIf);
            break;
        case RTK_MCAST_EGRIF_TYPE_STK:
            ret = _dal_longan_mcast_ipEgrIfStk_del(unit, pGroup, pEgrIf);
            break;
        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return ret;
}

/* Function Name:
 *      _dal_longan_mcast_fwdIdx_get
 * Description:
 *      get the group l2 forward table index and oil index
 * Input:
 *      unit  - unit id
 *      group  - mcast group
 * Output:
 *      pL2fwdIdx  - pointer of the l2 forward index
 *      pOilIdx  - pointer of the l3 oil index
 *      pStkPmskVal - pointer of stacking port mask value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      if not found, or invalid. *pL2fwdIdx = -1, *pOilIdx = -1
 */
int32
_dal_longan_mcast_fwdIdx_get(uint32 unit, rtk_mcast_group_t groupId, int32 *pL2fwdIdx, int32 *pOilIdx, uint32 *pStkPmskVal)
{
    int32   ret = RT_ERR_OK;
    int32   groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;
    dal_longan_mcast_oil_t *pOil;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,group=%d,pL2fwdIdx=%p,pOilIdx=%p", unit, groupId, pL2fwdIdx, pOilIdx);

    /* check external semaphore luck status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

    /* parameter check */
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pL2fwdIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOilIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pStkPmskVal), RT_ERR_NULL_POINTER);

    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);
    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret = RT_ERR_MCAST_GROUP_NOT_FOUND;
        goto errHandle;
    }

    *pL2fwdIdx = pGroup->l2_fwdIndex;
    rt_util_upinkPort_mask2Reg(unit, &pGroup->stk_pmsk, pStkPmskVal);

     pOil = RTK_LIST_NODE_HEAD(&pGroup->oil_list);
    if (OIL_ENTRY_IS_VALID(pOil))
    {
        *pOilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
    }
    else
    {
        *pOilIdx = DAL_LONGAN_MCAST_INVALID_OIL_IDX;
    }

errHandle:
    return ret;
}
#if 0
/* Function Name:
 *      _dal_longan_mcast_ipmc_bindState_get
 * Description:
 *      get the binding state of the group to ipmc entry
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group
 *      pBind     - binding state
 *      pEntryIdx - ipmc entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32
_dal_longan_mcast_ipmc_bindState_get(uint32 unit, rtk_mcast_group_t groupId, rtk_enable_t *pBind, uint32 *pEntryIdx)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = RTK_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;

    /* check external semaphore luck status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_IP), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pBind), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntryIdx), RT_ERR_NULL_POINTER);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* return the group info */
    *pBind = (pGroup->ipmc_bind)? TRUE : FALSE;
    *pEntryIdx = pGroup->ipmc_entryIdx;

errHandle:
    return ret;
}
#endif
/* Function Name:
 *      _dal_longan_mcast_ipmc_bind
 * Description:
 *      bind the group to ipmc entry
 * Input:
 *      unit  - unit id
 *      groupId - mcast group
 *      pMcastAddr - pointer to the ipmc addr
 *      entryIdx - pointer to the l3 interface entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *
 */
int32
_dal_longan_mcast_ipmc_bind(uint32 unit,  rtk_mcast_group_t groupId, rtk_ipmc_addr_t *pMcastAddr, uint32 entryIdx)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = RTK_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;
    dal_longan_mcast_l3Node_t *pL3Node;

    /* check external semaphore luck status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_IP), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);

    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

#if 0
    if ((TRUE == pGroup->ipmc_bind)  && \
        (pGroup->ipmc_entryIdx != entryIdx))
    {
        ret =  RT_ERR_ENTRY_REFERRED;
        goto errHandle;
    }

    /* update group entry */
    pGroup->ipmc_flags = pMcastAddr->ipmc_flags;
    pGroup->ipmc_entryIdx = entryIdx;
    pGroup->ipmc_bind = TRUE;
#else
    RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
    {
        if (pL3Node->ipmc_entryIdx == entryIdx) /* update exsting node */
        {
            pL3Node->ipmc_flags = pMcastAddr->ipmc_flags;
            goto errOk;
        }
    }

    pL3Node = RTK_LIST_NODE_HEAD(&_pMcastDb[unit]->HW.free_l3Node_list);
    if (NULL == pL3Node)
    {
        ret = RT_ERR_FAILED;   /* should not happen */
        goto errHandle;
    }
    RTK_LIST_NODE_REMOVE(&_pMcastDb[unit]->HW.free_l3Node_list, pL3Node, nodeRef);
    RTK_LIST_NODE_REF_INIT(pL3Node, nodeRef);
    RTK_LIST_NODE_INSERT_TAIL(&pGroup->l3_entry_list, pL3Node, nodeRef);

    /* update node */
    pL3Node->ipmc_entryIdx  = entryIdx;
    pL3Node->ipmc_flags     = pMcastAddr->ipmc_flags;
#endif

errHandle:
errOk:
    return ret;
}



/* Function Name:
 *      _dal_longan_mcast_ipmc_unbind
 * Description:
 *      un-bind the group to ipmc entry
 * Input:
 *      unit  - unit id
 *      group - mcast group
 *      pEntryIdx - pointer to the ipmc addr table index
 * Output:
 *      pEntryIdx - pointer to the ipmc addr table index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - the module is not initial
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *
 */
int32
_dal_longan_mcast_ipmc_unbind(uint32 unit, rtk_mcast_group_t groupId, uint32 entryIdx)
{
    int32 ret = RT_ERR_OK;
    rtk_mcast_type_t    type = RTK_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;
    dal_longan_mcast_l3Node_t *pL3Node;

    /* check external semaphore luck status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

     /* parameter check */
    RT_PARAM_CHK((type >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((entryIdx >= DAL_LONGAN_IPMCAST_L3_ENTRY_MAX), RT_ERR_OUT_OF_RANGE);

    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

#if 0
    /* check binding status */
    if (FALSE == pGroup->ipmc_bind)
    {
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

    /* return hardward entry index */
    *pEntryIdx = pGroup->ipmc_entryIdx;

    /* clear IPMC binding info */
    pGroup->ipmc_entryIdx = 0;
    pGroup->ipmc_bind = FALSE;
#else
    RTK_LIST_FOREACH(&pGroup->l3_entry_list, pL3Node, nodeRef)
    {
        if (pL3Node->ipmc_entryIdx == entryIdx)
        {
            RTK_LIST_NODE_REMOVE(&pGroup->l3_entry_list, pL3Node, nodeRef);
            RTK_LIST_NODE_REF_INIT(pL3Node, nodeRef);
            RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_l3Node_list, pL3Node, nodeRef);

            goto errOk;
        }
    }

    /* not found and do nothing */
    osal_printf("%s():%d - unbind (%u) failed!\n", __func__, __LINE__, entryIdx);
#endif

errHandle:
errOk:

    return ret;
}

/* Function Name:
 *      _dal_longan_mcast_sem_lock
 * Description:
 *      lock multicast APIs' semaphore
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      This is an API called by sub-modules (IPMCAST).
 */
int32
_dal_longan_mcast_sem_lock(uint32 unit)
{
    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    MCAST_SEM_LOCK(unit);

    mcast_extSemLocked[unit] = TRUE;

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_mcast_sem_unlock
 * Description:
 *      unlock multicast APIs' semaphore
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      This is an API called by sub-modules (IPMCAST).
 */
int32
_dal_longan_mcast_sem_unlock(uint32 unit)
{
    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    mcast_extSemLocked[unit] = FALSE;

    MCAST_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_mcastMapper_init
 * Description:
 *      Hook mcast module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook mcast module before calling any mcast APIs.
 */
int32
dal_longan_mcastMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mcast_init = dal_longan_mcast_init;
    pMapper->mcast_group_create = dal_longan_mcast_group_create;
    pMapper->mcast_group_destroy = dal_longan_mcast_group_destroy;
    pMapper->mcast_group_getNext = dal_longan_mcast_group_getNext;
    pMapper->mcast_nextHop_get = dal_longan_mcast_egrIf_get;
    pMapper->mcast_nextHop_add = dal_longan_mcast_egrIf_add;
    pMapper->mcast_nextHop_del = dal_longan_mcast_egrIf_del;
    pMapper->mcast_nextHop_delAll = dal_longan_mcast_egrIf_delAll;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_mcast_init
 * Description:
 *      Initialize mcast module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 */
int32
dal_longan_mcast_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    int32   idx;
    dal_longan_mcast_oil_t *pOil;
    dal_longan_mcast_group_t *pGroup;
    dal_longan_mcast_l3Node_t *pL3Node;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(mcast_init[unit]);
    mcast_init[unit] = INIT_NOT_COMPLETED;

    /*should check L2/L3 model init finished
    *init mcast before init ipmcast  */

     /* clear external semaphore luck status */
    mcast_extSemLocked[unit] = FALSE;

    /* create semaphore */
    mcast_sem[unit] = osal_sem_mutex_create();
    if (0 == mcast_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* allocate memory that we need */
    if ((_pMcastDb[unit] = osal_alloc(sizeof(dal_longan_mcast_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L3), "out of memory");
        ret = RT_ERR_FAILED;
        goto errInit;
    }

    osal_memset(_pMcastDb[unit], 0x00, sizeof(dal_longan_mcast_drvDb_t));
    /* free OIL list initialize */
    RTK_LIST_INIT(&_pMcastDb[unit]->HW.free_oil_list);
    for (idx=0; idx<DAL_LONGAN_MCAST_OIL_MAX; idx++)
    {
        pOil = OIL_ENTRY_ADDR(unit, idx);
        OIL_ENTRY_RESET(pOil);
        RTK_LIST_NODE_REF_INIT(pOil, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_oil_list, pOil, nodeRef);
    }
    /* free group list initialize */
    RTK_LIST_INIT(&_pMcastDb[unit]->free_group_list);
    for (idx=0; idx<DAL_LONGAN_MCAST_GROUP_MAX; idx++)
    {
        pGroup = GROUP_ENTRY_ADDR(unit, idx);
        GROUP_ENTRY_RESET(pGroup);
        RTK_LIST_NODE_REF_INIT(pGroup, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->free_group_list, pGroup, nodeRef);
    }

    /* free l3Node initialize */
    for (idx=0; idx<DAL_LONGAN_MCAST_L3_NODE_MAX; idx++)
    {
        pL3Node = L3_NODE_ADDR(unit, idx);
        L3_NODE_RESET(pL3Node);
        RTK_LIST_NODE_REF_INIT(pL3Node, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_l3Node_list, pL3Node, nodeRef);
    }

    /* set init flag to complete init */
    mcast_init[unit] = INIT_COMPLETED;
    goto errOk;


errInit:
    osal_sem_mutex_destroy(mcast_sem[unit]);

errOk:
    return ret;
}   /* end of dal_longan_l3mc_init */


/* Function Name:
 *      dal_longan_mcast_group_create
 * Description:
 *      Create a multicast group
 * Input:
 *      unit  - unit id
 *      flags - RTK_MCAST_FLAG_WITH_ID
 *      type  - multicast group type
 * Output:
 *      pGroupId- pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *
 */
int32
dal_longan_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroupId)
{
    int32   ret = RT_ERR_OK;
    uint32 groupIdx;
    dal_longan_mcast_group_t *pGroup;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,flags=0x%08x,type=%d,pGroup=%p", unit, flags, type, pGroupId);
    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroupId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_MCAST_TYPE_END <= type), RT_ERR_INPUT);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_MCAST_TYPE_IP:
            if (flags & RTK_MCAST_FLAG_WITH_ID)
            {
                if (RTK_MCAST_GROUP_ID_TYPE(*pGroupId) != RTK_MCAST_TYPE_IP)
                {
                    ret = RT_ERR_INPUT;
                    goto errType;
                }

                groupIdx = RTK_MCAST_GROUP_ID_IDX(*pGroupId);
                if (groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX)
                {
                    ret = RT_ERR_OUT_OF_RANGE;
                    goto errOutOfRange;
                }

                RT_ERR_HDL(_dal_longan_mcast_group_alloc_byIdx(unit, &pGroup, groupIdx), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_longan_mcast_group_alloc(unit, &pGroup), errHandle, ret);
            }

            groupIdx = GROUP_ENTRY_ADDR_TO_IDX(unit, pGroup);
            pGroup->groupId = RTK_MCAST_GROUP_ID(type, groupIdx);
            pGroup->l2_fwdIndex = DAL_LONGAN_MCAST_INVALID_FWD_IDX;

            *pGroupId = pGroup->groupId;

            goto errOk;
            break;
        case RTK_MCAST_TYPE_MAC:
                ret = RT_ERR_CHIP_NOT_SUPPORTED;
                goto errType;
                break;
        default :
            ret = RT_ERR_FAILED;
            goto errUnkownType;
            break;
    }


errType:
errOutOfRange:
errUnkownType:
errHandle:
errOk:
    MCAST_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      dal_longan_mcast_group_destroy
 * Description:
 *      Destrory the multicast group
 * Input:
 *      unit  - unit id
 *      groupId - multicast group
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OPER_DENIED  - action not allowed to destroy
 * Note:
 */
int32
dal_longan_mcast_group_destroy(uint32 unit, rtk_mcast_group_t groupId)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t        type = RTK_MCAST_GROUP_ID_TYPE(groupId);
    uint32                  groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,group=%d", unit, groupId);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_MCAST_TYPE_IP:
            {
                pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

                if (!GROUP_ENTRY_IS_VALID(pGroup))
                {
                    ret = RT_ERR_ENTRY_NOTFOUND;
                    goto errHandle;
                }

#if 0
                if (TRUE == pGroup->ipmc_bind)
                {
                    ret = RT_ERR_ENTRY_REFERRED;
                    goto errHandle;
                }
#else
                if(RTK_LIST_LENGTH(&(pGroup->l3_entry_list))>0)
                {
                    ret = RT_ERR_ENTRY_REFERRED;
                    goto errHandle;
                }
#endif

                if ((pGroup->l2_fwdIndex >= 0) || !RTK_LIST_EMPTY(&pGroup->oil_list))
                {
                    /* resource not free yet, return error. */
                    ret = RT_ERR_ENTRY_REFERRED;
                    goto errHandle;
                }

                RT_ERR_HDL(_dal_longan_mcast_group_free(unit, pGroup), errHandle, ret);
            }
            break;
        case RTK_MCAST_TYPE_MAC:
                ret = RT_ERR_CHIP_NOT_SUPPORTED;
                goto errHandle;
                break;

        default :
            ret = RT_ERR_INPUT;
            goto errHandle;
            break;

    }

    goto errOk;

errHandle:
errOk:
    MCAST_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      dal_longan_mcast_group_getNext
 * Description:
 *      Get next multicast group
 * Input:
 *      unit  - unit id
 *      type - mcast type
 *      pBase- start index
 * Output:
 *      pGroup - pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *  *pBase = -1 indicate get first valid group
 *  type = 0 indicate include all type
 */
int32
dal_longan_mcast_group_getNext(uint32 unit, rtk_mcast_type_t  type, int32 *pBase, rtk_mcast_group_t *pGroup)
{
    int32   ret = RT_ERR_OK;
    int32   idx;
    rtk_mcast_group_t groupId;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_MCAST_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);

    /* function body */
    MCAST_SEM_LOCK(unit);

    /* scan the whole L3 host table */
    for (idx=(*pBase<0)?(0):((*pBase)+1); idx<DAL_LONGAN_MCAST_GROUP_MAX; idx++)
    {
        if (IS_GROUP_VALID(unit,idx))
        {
            groupId = _pMcastDb[unit]->group[idx].groupId;
            if ((0 != type) && (type !=  RTK_MCAST_GROUP_ID_TYPE(groupId)))
                continue;

            *pGroup =  groupId;
            *pBase = idx;

            goto errOk;
        }
    }

   *pBase = -1;

errOk:
    MCAST_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_longan_mcast_group_getNext */

/* Function Name:
 *      dal_longan_mcast_nh_get
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit  - unit id
 *      groupId - multicast group
 *      egrIf_max - number of rtk_mcast_egrif_t entry in pEgrIf buffer
 *      pEgrIf - buffer pointer
 * Output:
 *      pEgrIf_count - returned rtk_mcast_egrif_t entry number
 *      pEgrIf - pointer to the next hop information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 *      (1) should input enough buffer of pEgrIf
 */
int32
dal_longan_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t groupId, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = RTK_MCAST_GROUP_ID_TYPE(groupId);
    uint32              groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    int32   idx;
    dal_longan_mcast_group_t *pGroup;
    dal_longan_mcast_oil_t *pOil;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d,egrIf_max=%d,pNh=%p,pNum=%p", unit, groupId, egrIf_max, pEgrIf, pEgrIf_count);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEgrIf_count), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    MCAST_SEM_LOCK(unit);

    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    idx = 0;

    /* reset the returned number */
    *pEgrIf_count = 0;

    /* L2 nexthop */
    if ((idx < egrIf_max) && (pGroup->l2_fwdIndex >= 0))
    {
        pEgrIf[idx].type = RTK_MCAST_EGRIF_TYPE_L2;
        pEgrIf[idx].l2.fwdIndex = pGroup->l2_fwdIndex;
        RT_ERR_HDL(_dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pGroup->l2_fwdIndex, &pEgrIf[idx].l2.portmask), errHandle, ret);

        *pEgrIf_count = (++idx);
    }

    /*STK nexthop*/
    if ((idx < egrIf_max) && (0 != RTK_PORTMASK_GET_PORT_COUNT(pGroup->stk_pmsk)))
    {
        pEgrIf[idx].type = RTK_MCAST_EGRIF_TYPE_STK;
        pEgrIf[idx].stk.portmask = pGroup->stk_pmsk;

        *pEgrIf_count = (++idx);
    }

    RTK_LIST_FOREACH(&pGroup->oil_list, pOil, nodeRef)
    {
        if (idx >= egrIf_max)
            break;

        RT_ERR_HDL(mcast_util_dalOil2rtkMcNh(unit, &pEgrIf[idx], pOil), errHandle, ret);

        *pEgrIf_count = (++idx);
    }

errHandle:
    MCAST_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      dal_longan_mcast_egrIf_add
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit  - unit id
 *      group - multicast group
 *      pEgrIf - buffer pointer of mcast nh
 * Output:
*       None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
int32
dal_longan_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t groupId,rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = RTK_MCAST_GROUP_ID_TYPE(groupId);
    uint32              groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d,pOif=%p", unit, groupId, pEgrIf);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (groupType)
    {
    case RTK_MCAST_TYPE_IP:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            RT_ERR_HDL(_dal_longan_mcast_ipEgrIf_add(unit, pGroup, pEgrIf), errHandle, ret);
        }
        break;

    case RTK_MCAST_TYPE_MAC:
        {
            /* these 2 types are not supported yet,
             * will need further discussion with module PIC to see if they need to use this API
             */
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            goto errHandle;
        }
        break;

    default :
        {
            ret = RT_ERR_INPUT;
            goto errHandle;
        }
        break;
    }

    goto errOk;

errHandle:
errOk:
    MCAST_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      dal_longan_mcast_egrIf_del
 * Description:
 *      Delete a NH from multicast group replication list
 * Input:
 *      unit  - unit id
 *      groupId - multicast group
 *      pEgrIf  - pointer to multicast next hop information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
int32
dal_longan_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t groupId, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t        type = RTK_MCAST_GROUP_ID_TYPE(groupId);
    uint32                  groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,group=%d,pOif=%p", unit, groupId,pEgrIf);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    MCAST_SEM_LOCK(unit);

    //check oil sw, find the continue number of oil entry to compare the flag;
    switch (type)
    {
        case RTK_MCAST_TYPE_IP:
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);
            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_MCAST_GROUP_NOT_FOUND;
                goto errHandle;
            }
            RT_ERR_HDL(_dal_longan_mcast_ipEgrIf_del(unit,pGroup, pEgrIf), errHandle,ret);
            break;
        case RTK_MCAST_TYPE_MAC:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            goto errHandle;

        default :
            ret = RT_ERR_INPUT;
            goto errHandle;
    }
    goto errOk;

errHandle:
errOk:
    MCAST_SEM_UNLOCK(unit);
    return ret;
}

/* Function Name:
 *      rtk_mcast_egrIf_delAll
 * Description:
 *      Delete all NHs from multicast group replication list
 * Input:
 *      unit  - unit id
 *      groupId - multicast group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 */

int32
dal_longan_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t groupId)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_egrif_t  nhEntry;
    rtk_mcast_type_t        groupType = RTK_MCAST_GROUP_ID_TYPE(groupId);
    uint32                  groupIdx = RTK_MCAST_GROUP_ID_IDX(groupId);
    dal_longan_mcast_group_t *pGroup;
    dal_longan_mcast_oil_t *pOil;
    dal_longan_mcast_oil_t *pTemp;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L3), "unit=%d,group=%d", unit, groupId);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_LONGAN_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (groupType)
        {
         case RTK_MCAST_TYPE_IP:
            {
                pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);
                if (!GROUP_ENTRY_IS_VALID(pGroup))
                {
                    ret = RT_ERR_ENTRY_NOTFOUND;
                    goto errHandle;
                }

                /* l2 bridge */
                if (pGroup->l2_fwdIndex >= 0)
                {
                    nhEntry.type = RTK_MCAST_EGRIF_TYPE_L2;
                    nhEntry.l2.fwdIndex = pGroup->l2_fwdIndex;
                    /* remove all ports */
                    HWP_GET_ALL_PORTMASK(unit, nhEntry.l2.portmask);
                    RT_ERR_HDL(_dal_longan_mcast_ipEgrIfL2_del(unit, pGroup, &nhEntry), errHandle, ret);
                }

                if (0 != RTK_PORTMASK_GET_PORT_COUNT(pGroup->stk_pmsk))
                {
                    nhEntry.type = RTK_MCAST_EGRIF_TYPE_STK;
                    nhEntry.stk.portmask = pGroup->stk_pmsk;

                    RT_ERR_HDL(_dal_longan_mcast_ipEgrIfStk_del(unit, pGroup, &nhEntry), errHandle, ret);
                }

                RTK_LIST_FOREACH_REVERSE_SAFE(&pGroup->oil_list, pOil, pTemp, nodeRef)
                {
                    RT_ERR_HDL(_dal_longan_mcast_ipEgrIfOil_remove(unit, pGroup, pOil), errHandle, ret);
                }
            }
            break;

        case RTK_MCAST_TYPE_MAC:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            goto errHandle;

        default :
            ret = RT_ERR_INPUT;
            goto errHandle;
       }

        goto errOk;

errHandle:
errOk:
    MCAST_SEM_UNLOCK(unit);
    return ret;
}


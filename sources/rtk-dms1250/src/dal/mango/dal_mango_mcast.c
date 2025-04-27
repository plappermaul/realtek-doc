/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public multicast APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Multicast group
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
#include <dal/mango/dal_mango_switch.h>
#include <dal/mango/dal_mango_vlan.h>
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_tunnel.h>
#include <dal/mango/dal_mango_mcast.h>
#include <dal/mango/dal_mango_ipmcast.h>
#include <dal/mango/dal_mango_bpe.h>
#include <dal/mango/dal_mango_stack.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/mcast.h>

/*
 * Symbol Definition
 */
#define MANGO_MCAST_DBG                     (0)

#define L2TUNNEL_NODE_END_OF_LIST           (0)

typedef struct dal_mango_mcast_group_s
{
    rtk_mcast_group_t   groupId;

    uint32  valid:1;        /* Group state: TRUE/FALSE, to indicate the node is used/free  */

    union {
        struct {
            /* associated entries */
            RTK_LIST_DEF(dal_mango_mcast_vlanNode_s, vlan_list);    /* associated VLANs */
            RTK_LIST_DEF(dal_mango_mcast_l2Node_s, l2_entry_list);  /* associated L2 entries */

            /* members information */
            int32   l2_fwdIndex;    /* forwarding portmask index of L2 bridging (-1 means invalid) */
            //int32   l2_lstIndex;    /* forwarding list index of L2 bridging (-1 means invalid) */
            RTK_LIST_DEF(dal_mango_mcast_l2TnlNode_s, l2_tunnel_list);
        } mac;

        struct {
#if 0
            uint32  ipmc_bind:1;    /* IPMC binding state: TRUE/FALSE */
            uint32  ipmc_entryIdx;  /* IPMC entry index (available when it's bound to an IPMC entry) */
            uint32  ipmc_flags;     /* IPMC flags (available when it's bound to an IPMC entry) */
#endif

            /* associated entries */
            RTK_LIST_DEF(dal_mango_mcast_l3Node_s, l3_entry_list);  /* associated L3 entries */

            /* members information */
            int32   l2_fwdIndex;    /* forwarding portmask index of L2 bridging (-1 means invalid) */
            int32   l2_lstIndex;    /* forwarding list index of L2 bridging (-1 means invalid) */
            rtk_portmask_t  stk_pmsk;   /* L3 stacking forward portmask */
            RTK_LIST_DEF(dal_mango_mcast_oil_s, oil_list);
        } ip;
    };

    RTK_LIST_NODE_REF_DEF(dal_mango_mcast_group_s, nodeRef);
} dal_mango_mcast_group_t;

typedef struct dal_mango_mcast_oil_s
{
    uint64  sortKey;    /* unique key for sorting */

    uint32  valid:1;
    uint32  egrif_type:4;
    uint32  bridge_mode:1;          /* TTL_DEC = 0, TTL_CHK = 0, SA_REPLACE = 0 */
    uint32  l3_egr_intf_idx:10;

    int32   fwdIndex;   /* forwarding portmask index (-1 means invalid) */

    rtk_bpe_ecid_t  ecid;   /* ECID of PE-TAG */

    /* list references */
    RTK_LIST_NODE_REF_DEF(dal_mango_mcast_oil_s, nodeRef);
} dal_mango_mcast_oil_t;

typedef struct dal_mango_mcast_l2TnlNode_s
{
    uint32  sortKey;    /* unique key for sorting */

    uint32  valid:1;
    uint32  egrif_type:4;
    uint32  port_type:1; /* 0: port_info is PMSK_IDX[11:0]; 1: port_info is OFFSET[10:8] and PMSK[7:0]*/

    uint32  tnl_data;   /*for vxlan: L2_TNL_IDX; for BPE: port_info; for MPLS: MPLS_ENCAP_IDX*/

    /* list references */
    RTK_LIST_NODE_REF_DEF(dal_mango_mcast_l2TnlNode_s, nodeRef);
} dal_mango_mcast_l2TnlNode_t;

typedef struct dal_mango_mcast_drvDb_s
{
    /* hardware resource (for internal APIs) */
    struct
    {
        /* OIL */
        uint32  oil_used_count;
        RTK_LIST_DEF(dal_mango_mcast_oil_s, free_oil_list);
        dal_mango_mcast_oil_t   oil[DAL_MANGO_MCAST_OIL_MAX];

        /* L2 tunnel node */
        uint32 l2TunnelNode_used_count;
        RTK_LIST_DEF(dal_mango_mcast_l2TnlNode_s, free_l2TunnelNode_list);
        dal_mango_mcast_l2TnlNode_t    l2TunnelNode[DAL_MANGO_MCAST_L2TUNNEL_NODE_MAX];

        /* IPMC entry node */
        uint32 l3IpmcNode_used_count;
        RTK_LIST_DEF(dal_mango_mcast_l3Node_s, free_l3Node_list);
        dal_mango_mcast_l3Node_t       l3Node[DAL_MANGO_MCAST_L3_NODE_MAX];
    } HW;

    RTK_LIST_DEF(dal_mango_mcast_group_s, free_group_list);
    dal_mango_mcast_group_t group[DAL_MANGO_MCAST_GROUP_MAX];
} dal_mango_mcast_drvDb_t;

/* structure of multicast OIL entry */
typedef struct dal_mango_mcast_oilEntry_s
{
    uint32      oil_next;               /* index of the next oil */
    uint32      ttl_dec;                /* TTL decrease */
    uint32      ttl_chk;                /* TTl check */
    uint32      sa_replace;             /* SMAC replacement */
    uint32      l3_egr_intf_idx;        /* L3 egress inteface */
    uint32      l2_tnl_en;              /* L2 tunnel */
    uint32      mc_pmsk_tnl_idx;        /* (multicast portmask/tunnel) idnex */
    uint32      dst_port_type;          /* destination port type */
    uint32      ecid;                   /* ECID */
} dal_mango_mcast_oilEntry_t;


/*
 * Data Declaration
 */

static uint32                   mcast_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t             mcast_sem[RTK_MAX_NUM_OF_UNIT];
static uint32                   mcast_extSemLocked[RTK_MAX_NUM_OF_UNIT];

static dal_mango_mcast_drvDb_t  *_pMcastDb[RTK_MAX_NUM_OF_UNIT];



/*
 * Macro Declaration
 */
#define MANGO_MCAST_DBG_PRINTF(_level, fmt, ...)                                            \
do {                                                                                        \
    if (MANGO_MCAST_DBG >= (_level))                                                        \
        osal_printf("%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);   \
} while (0)

#define MCAST_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(mcast_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_MCAST), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define MCAST_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(mcast_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_MCAST), "semaphore unlock failed");\
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

#define DAL_MANGO_GROUP_NUM_GET(_unit,_grpIdx)  (_pMcastDb[_unit]->group[_grpIdx].l3mc.num)

#define IS_OIL_VALID(__pOilEntry)               (TRUE == __pOilEntry->valid)
#define GROUP_IS_VALID(__unit, __grpIdx)        (TRUE == _pMcastDb[__unit]->group[__grpIdx].valid)
#define OIL_USED_NUM(__unit)                    (_pMcastDb[__unit]->HW.oil_used_count)

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
    osal_memset(_pGrp, 0x00, sizeof(dal_mango_mcast_group_t));  \
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
    osal_memset(&_pMcastDb[_unit]->group[_grpIdx], 0x00, sizeof(dal_mango_mcast_group_t));  \
    /*_pMcastDb[_unit]->group[_grpIdx].groupIdx = _grpIdx;*/                                \
} while (0)

#define GROUP_ENTRY_ADDR(_unit, _grpIdx)        (&_pMcastDb[(_unit)]->group[(_grpIdx)])
#define GROUP_ENTRY_ADDR_TO_IDX(_unit, _pGrp)   ((_pGrp) - (&_pMcastDb[(_unit)]->group[0]))
#define GROUP_IS_BIND_IPMC(_unit, _grpIdx)      (TRUE == _pMcastDb[(_unit)]->group[(_grpIdx)].bind_ipmc)

#define OIL_ENTRY_ADDR(_unit, _oilIdx)          (&_pMcastDb[(_unit)]->HW.oil[(_oilIdx)])
#define OIL_ENTRY_ADDR_TO_IDX(_unit, _pOil)     ((_pOil) - (&_pMcastDb[(_unit)]->HW.oil[0]))

#define OIL_ENTRY_IS_VALID(_pOil)               ((NULL != (_pOil)) && (TRUE == (_pOil)->valid))

#define OIL_ENTRY_RESET(_pOil)                                  \
do {                                                            \
    osal_memset(_pOil, 0x00, sizeof(dal_mango_mcast_oil_t));    \
} while (0)

#define OIL_ENTRY_VALIDATE(_pOil)   \
do {                                \
    (_pOil)->valid = TRUE;          \
} while (0)

#define OIL_ENTRY_INIT(_pOil)   \
do {                            \
    /*(_pOil)-> = TRUE;*/    \
} while (0)


#define L2TUNNEL_NODE_ADDR(_unit, _nodeIdx)         (&_pMcastDb[(_unit)]->HW.l2TunnelNode[(_nodeIdx)])
#define L2TUNNEL_NODE_ADDR_TO_IDX(_unit, _pL2Node)  ((_pL2Node) - (&_pMcastDb[(_unit)]->HW.l2TunnelNode[0]))
#define L2TUNNEL_NODE_IS_VALID(_pL2Node)            ((NULL != (_pL2Node)) && (TRUE == (_pL2Node)->valid))
#define L2TUNNEL_NODE_RESET(_pL2Node)                                   \
    do {                                                                \
        osal_memset(_pL2Node, 0x00, sizeof(dal_mango_mcast_l2TnlNode_t));  \
    } while (0)
#define L2TUNNEL_NODE_VALIDATE(_pL2Node)   \
    do {                                    \
        (_pL2Node)->valid = TRUE;           \
    } while (0)

#define L3_NODE_ADDR(_unit, _nodeIdx)           (&_pMcastDb[(_unit)]->HW.l3Node[(_nodeIdx)])
#define L3_NODE_ADDR_TO_IDX(_unit, _pL3Node)    ((_pL3Node) - (&_pMcastDb[(_unit)]->HW.l3Node[0]))
//#define L3_NODE_IS_VALID(_pL3Node)            ((NULL != (_pL2Node)) && (TRUE == (_pL2Node)->valid))
#define L3_NODE_RESET(_pL3Node)                                   \
    do {                                                                \
        osal_memset(_pL3Node, 0x00, sizeof(dal_mango_mcast_l3Node_t));  \
    } while (0)
#if 0
#define L3_NODE_VALIDATE(_pL3Node)   \
    do {                                    \
        (_pL3Node)->valid = TRUE;           \
    } while (0)
#endif

#define FLEX_TABLE_FMT_CHECK(_unit, _fmt) \
    do { \
        int32 _retval; \
        uint32 _value; \
        if ((_retval = dal_mango_switch_flexTblFmt_get(_unit, (rtk_switch_flexTblFmt_t *)&_value)) != RT_ERR_OK) \
        { \
            RT_ERR(_retval, (MOD_BPE|MOD_DAL), "get table format failed"); \
            return _retval; \
        } \
        if (_fmt != _value) \
        { \
            _retval = RT_ERR_NOT_ALLOWED; \
            RT_ERR(_retval, (MOD_BPE|MOD_DAL), "table format not allowed"); \
            return _retval; \
        } \
    } while(0)



/*
 * Function Declaration
 */

static inline int32
mcast_util_dalOil2oilEntry(uint32 unit, dal_mango_mcast_oilEntry_t *pEntry, dal_mango_mcast_oil_t *pOil)
{
    int32   ret = RT_ERR_OK;
    dal_mango_mcast_oil_t *pOilNext;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOil), RT_ERR_NULL_POINTER);

    osal_memset(pEntry, 0x00, sizeof(dal_mango_mcast_oilEntry_t));

    /* get info of the next node */
    pOilNext = RTK_LIST_NODE_NEXT(pOil, nodeRef);
    if (NULL == pOilNext)
    {
        pEntry->oil_next = 0;
    }
    else
    {
        pEntry->oil_next = OIL_ENTRY_ADDR_TO_IDX(unit, pOilNext);
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
            pEntry->mc_pmsk_tnl_idx = pOil->fwdIndex;
            pEntry->dst_port_type = 0;
            pEntry->ecid = pOil->ecid;
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_TUNNEL:
        {
            pEntry->l3_egr_intf_idx = pOil->l3_egr_intf_idx;
        }
        break;

    default:
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return ret;
}

static inline int32
mcast_util_dalOil2rtkMcNh(uint32 unit, rtk_mcast_egrif_t *pEgrIf, dal_mango_mcast_oil_t *pOil)
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
            MANGO_MCAST_DBG_PRINTF(3, "some this error for type is NH_TYPE_L2\n");
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_L3:
        {
            pEgrIf->l3.intf_id  = pOil->l3_egr_intf_idx;
            pEgrIf->l3.ecid     = pOil->ecid;
            pEgrIf->l3.fwdIndex = pOil->fwdIndex;

            if (TRUE == pOil->bridge_mode)
                pEgrIf->flags = RTK_MCAST_EGRIF_FLAG_BRIDGE;

            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pOil->fwdIndex, &pEgrIf->l3.portmask), errHandle, ret);
            MANGO_MCAST_DBG_PRINTF(3, "pEgrIf->l3.intf_id = %u\n", pEgrIf->l3.intf_id);
            MANGO_MCAST_DBG_PRINTF(3, "pEgrIf->l3.fwdIndex = %d\n", pEgrIf->l3.fwdIndex);
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_TUNNEL:
        {
            pEgrIf->l3.intf_id  = pOil->l3_egr_intf_idx;

            MANGO_MCAST_DBG_PRINTF(3, "pEgrIf->l3.intf_id = %u\n", pEgrIf->l3.intf_id);
        }
        break;
    case RTK_MCAST_EGRIF_TYPE_BPE:
    case RTK_MCAST_EGRIF_TYPE_VXLAN:
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

static inline int32
mcast_util_rtkPmsk2dalEcidPmsk(uint32 unit, rtk_portmask_t  *pPortmask, dal_mango_mcast_l2TnlNode_t *pNode)
{
    int32   ret = RT_ERR_OK;
    int32   offset = -1;
    int32   new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    uint32  tmp_base;
    uint32  tmp_bit;
    uint32  tmp_pmsk = 0;
    rtk_port_t  port;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);

    pNode->port_type = BPE_OFFSET_PMSK;

    if (RTK_PORTMASK_GET_PORT_COUNT(*pPortmask) != 0)
    {
        /* translate portmask as offset and pmsk (partial portmask) */
        /* pmsk represents port  0~7   when offset = 0 */
        /* pmsk represents port  8~15  when offset = 1 */
        /* pmsk represents port 16~23  when offset = 2 */
        /* ...                                         */
        RTK_PORTMASK_SCAN(*pPortmask, port)
        {
            tmp_base    = port/8;
            tmp_bit     = port%8;

            if (offset == -1)
            {
                /* init base offset value */
                offset = (int32)tmp_base;
                tmp_pmsk += (0x1 << tmp_bit);
            }
            else
            {
                if ((uint32)offset == tmp_base)
                {
                    /* port is belong to the same group, update the partial portmask*/
                    tmp_pmsk += (0x1 << tmp_bit);
                }
                else
                {
                    /* different base offset, out of partial pmsk range */
                    /* alloc forwarding index*/
                    pNode->port_type = BPE_PMSK_INDEX;
                    RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), errHandle, ret);
                    RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, new_pmsk_idx, pPortmask), errHandle, ret);

                    RT_LOG(LOG_DEBUG, (MOD_BPE|MOD_MCAST|MOD_DAL), "out of offset pmsk range");
                    break;
                }
            }
        }
    }
    else
    {
        offset  = 0;
        tmp_pmsk = 0;
    }

    if (BPE_OFFSET_PMSK == pNode->port_type)
    {
        /* tnl_data[31:11] = reserved
         * tnl_data[10:8]  = offset
         * tnl_data[7:0]   = pmsk
         */
        pNode->tnl_data = (((offset & 0x7) << 8) | (tmp_pmsk & 0xFFUL));

        RT_LOG(LOG_DEBUG, (MOD_MCAST|MOD_BPE|MOD_DAL), "BPE_OFFSET_PMSK 0x%x: offset=%u, pmsk=0x%x",
            pNode->tnl_data, offset, tmp_pmsk);
    }
    else
    {
        /* tnl_data[31:12] = reserved
         * tnl_data[11:0]  = pmsk_index
         */
        pNode->tnl_data = new_pmsk_idx;

        RT_LOG(LOG_DEBUG, (MOD_MCAST|MOD_BPE|MOD_DAL), "BPE_PMSK_INDEX: fwdIndex=%u, bitmap0=0x%x, bitmap1=0x%x",
             pNode->tnl_data, pPortmask->bits[0], pPortmask->bits[1]);
    }

    goto errOK;

errHandle:
    if (new_pmsk_idx != DAL_MANGO_MCAST_INVALID_FWD_IDX)
    {
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, new_pmsk_idx), "");
    }
errOK:
    return ret;

}


static inline int32
mcast_util_dalEcidPmsk2rtkPmsk(uint32 unit, dal_mango_mcast_l2TnlNode_t *pNode, rtk_portmask_t  *pPortmask)
{
    int32   ret = RT_ERR_OK;
    int32   idx = -1;
    uint32  tmp_base;
    uint32  tmp_pmsk;
    rtk_port_t  port;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);


    if(BPE_PMSK_INDEX == pNode->port_type)
    {
       RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pNode->tnl_data, pPortmask), errHandle, ret);
    }
    else
    {
        tmp_base = (((pNode->tnl_data) >> 8) & 0x7UL);
        tmp_pmsk = ((pNode->tnl_data) & 0xFFUL);

       for (idx = 0; idx< 8; idx++)
       {
           if (tmp_pmsk & 0x1)
           {
               port = tmp_base*8 + idx;
               RTK_PORTMASK_PORT_SET(*pPortmask, port);
           }
           tmp_pmsk = (tmp_pmsk >> 1);
       }
    }

errHandle:
    return ret;

}


static inline int32
mcast_util_dalTunnel2rtkMcNh(uint32 unit, dal_mango_mcast_l2TnlNode_t *pNode, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    dal_mango_bpe_ecidPmsk_entry_t ecidPmskEntry;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);

    /* clear memory */
    osal_memset(pEgrIf, 0x00, sizeof(rtk_mcast_egrif_t));
    osal_memset(&ecidPmskEntry, 0x00, sizeof(dal_mango_bpe_ecidPmsk_entry_t));

    RT_LOG(LOG_DEBUG, (MOD_MCAST|MOD_BPE|MOD_DAL), "pNode->type=%d, pNode->sortKey=0x%x, pNode->tnl_data=0x%x",
         pNode->egrif_type, pNode->sortKey, pNode->tnl_data);

    pEgrIf->type = pNode->egrif_type;
    switch (pEgrIf->type)
    {
        case RTK_MCAST_EGRIF_TYPE_VXLAN:
            {
                pEgrIf->vxlan.entry_idx = pNode->tnl_data;
            }
            break;

        case RTK_MCAST_EGRIF_TYPE_BPE:
            {
                RT_ERR_HDL(mcast_util_dalEcidPmsk2rtkPmsk(unit, pNode, &pEgrIf->bpe.portmask), errHandle, ret);

                pEgrIf->bpe.ecid = ((pNode->sortKey) & 0x3FFFFFUL);
                RT_LOG(LOG_DEBUG, (MOD_MCAST|MOD_BPE|MOD_DAL), "RTK PORTMASK: bitmap0=0x%x, bitmap1=0x%x",
                    pEgrIf->bpe.portmask.bits[0], pEgrIf->bpe.portmask.bits[1]);
            }
            break;

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

static inline int32 _dal_mango_mcast_group_alloc(uint32 unit, dal_mango_mcast_group_t **ppGroup)
{
    dal_mango_mcast_group_t *pGroup;

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

static inline int32 _dal_mango_mcast_group_free(uint32 unit, dal_mango_mcast_group_t *pGroup)
{
    if (GROUP_ENTRY_IS_VALID(pGroup))
    {
        GROUP_ENTRY_RESET(pGroup);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->free_group_list, pGroup, nodeRef);

        return RT_ERR_OK;
    }

    return RT_ERR_ENTRY_NOTFOUND;
}

static inline int32 _dal_mango_mcast_group_alloc_byIdx(uint32 unit, dal_mango_mcast_group_t **ppGroup, uint32 index)
{
    dal_mango_mcast_group_t *pGroup;

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

/* to get the unique sort key */
static inline uint64
_dal_mango_mcast_oil_sortKey_ret(rtk_mcast_egrif_t *pEgrIf)
{
    uint64  sortKey;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEgrIf), 0ULL);   /* return key as zero */

    switch (pEgrIf->type)
    {
    case RTK_MCAST_EGRIF_TYPE_L2:
        {
            /* bit[63..56] = egrif_type
             * bit[55..0]  = reserved
             */
            sortKey = (((pEgrIf->type) & 0xFFULL) << 56);
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_L3:
        {
            /* bit[63..56] = egrif_type
             * bit[55..40] = intf_id
             * bit[39..33] = reserved
             * bit[32]     = bridge mode state
             * bit[31..22] = reserved
             * bit[21..0]  = ECID
             */
            sortKey = (((pEgrIf->type) & 0xFFULL) << 56) | \
                      (((pEgrIf->l3.intf_id) & 0xFFFFULL) << 40) | \
                      ((pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_BRIDGE)? (0x0ULL << 32) : (0x1ULL << 32)) | \
                      (((pEgrIf->l3.ecid) & 0x3FFFFFULL) << 0);
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_TUNNEL:  /* IP tunnel */
        {
            /* bit[63..56] = egrif_type
             * bit[55..48] = reserved
             * bit[47..32] = intf_id
             * bit[31..0]  = reserved
             */
            sortKey = (((pEgrIf->type) & 0xFFULL) << 56) | \
                      (((pEgrIf->tunnel.intf_id) & 0xFFFFULL) << 32);
        }
        break;

    default:
        return 0;   /* should not happen */
    }

    MANGO_MCAST_DBG_PRINTF(3, "[DBG] sortKey = 0x%016llX\n", sortKey);

    return sortKey;
}


static inline int32 _dal_mango_mcast_oil_alloc(uint32 unit, dal_mango_mcast_oil_t **ppOil)
{
    dal_mango_mcast_oil_t *pOil;

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

static inline int32 _dal_mango_mcast_oil_free(uint32 unit, dal_mango_mcast_oil_t *pOil)
{
    if (OIL_ENTRY_IS_VALID(pOil))
    {
        OIL_ENTRY_RESET(pOil);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_oil_list, pOil, nodeRef);

        return RT_ERR_OK;
    }

    return RT_ERR_ENTRY_NOTFOUND;
}

#if 0
static int32
_dal_mango_mcast_oilEntry_get(uint32 unit, uint32 index, dal_mango_mcast_oilEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    l3_egr_intf_list_entry_t oilEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,index=%d,pEntry=%p", unit, index, pEntry);

    /* parameter check */
    RT_PARAM_CHK(index >= DAL_MANGO_MCAST_OIL_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */

    /* read from chip */
    MCAST_TABLE_READ_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, index, oilEntry, "", errHandle, ret);

    /* load data */
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
     MANGO_L3_EGR_INTF_LIST_OIL_NEXTtf, pEntry->oil_next, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
     MANGO_L3_EGR_INTF_LIST_EOLtf, pEntry->eol, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_TTL_DECtf, pEntry->ttl_dec, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_TTL_CHKtf, pEntry->ttl_chk, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_SA_REPLACEtf, pEntry->sa_replace, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_DST_TYPEtf, pEntry->dst_type, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_L2_TNL_VALIDtf, pEntry->l2_tnl_en, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_MC_PMSK_L2_TNL_IDXtf, pEntry->mc_pmsk_tnl_idx, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_MC_PMSK_IDXtf, pEntry->mc_pmsk_idx, oilEntry, "", errHandle, ret);

    MANGO_MCAST_DBG("%s():%d - debug, index = %u (0x%08x)\n", __FUNCTION__, __LINE__, index, index);

errHandle:
    return ret;
}
#endif

static int32
_dal_mango_mcast_oilEntry_set(uint32 unit, uint32 index, dal_mango_mcast_oilEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    l3_egr_intf_list_entry_t oilEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,index=%d,pEntry=%p", unit, index, pEntry);

    /* parameter check */
    RT_PARAM_CHK(index >= DAL_MANGO_MCAST_OIL_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&oilEntry, 0x00, sizeof(l3_egr_intf_list_entry_t));
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_OIL_NEXTtf, pEntry->oil_next, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_TTL_DECtf, pEntry->ttl_dec, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_TTL_CHKtf, pEntry->ttl_chk, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_SA_REPLACEtf, pEntry->sa_replace, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_L3_EGR_INTF_IDXtf, pEntry->l3_egr_intf_idx, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_L2_TNL_VALIDtf, pEntry->l2_tnl_en, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_MC_PMSK_L2_TNL_IDXtf, pEntry->mc_pmsk_tnl_idx, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_DST_PORT_TYPEtf, pEntry->dst_port_type, oilEntry, "", errHandle, ret);
    MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, \
        MANGO_L3_EGR_INTF_LIST_ECIDtf, pEntry->ecid, oilEntry, "", errHandle, ret);

    MCAST_TABLE_WRITE_ERR_HDL(unit, MANGO_L3_EGR_INTF_LISTt, index, oilEntry, "", errHandle, ret);

    MANGO_MCAST_DBG_PRINTF(2, "idx = %u (0x%08x), oil_next = %u\n", index, index, pEntry->oil_next);

    if (MANGO_MCAST_DBG)
    {
        MANGO_MCAST_DBG_PRINTF(1, "index = %d\n", index);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->oil_next = %d\n", pEntry->oil_next);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->ttl_dec = %d\n", pEntry->ttl_dec);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->ttl_chk = %d\n", pEntry->ttl_chk);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->sa_replace = %d\n", pEntry->sa_replace);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->l3_egr_intf_idx = 0x%X (%d)\n",
            pEntry->l3_egr_intf_idx, pEntry->l3_egr_intf_idx);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->l2_tnl_en = %d\n", pEntry->l2_tnl_en);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->mc_pmsk_tnl_idx = 0x%X (%d)\n",
            pEntry->mc_pmsk_tnl_idx, pEntry->mc_pmsk_tnl_idx);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->dst_port_type = %d\n", pEntry->dst_port_type);
        MANGO_MCAST_DBG_PRINTF(1, "pEntry->ecid = 0x%X\n", pEntry->ecid);
    }

errHandle:

    return ret;
}

static inline int32
_dal_mango_mcast_ipEgrIfOil_insert(uint32 unit, dal_mango_mcast_group_t *pGroup, dal_mango_mcast_oil_t *pOil, dal_mango_mcast_oil_t *pOilPrev)
{
    int32   ret = RT_ERR_OK;
    int32   oilIdx;
    dal_mango_mcast_oilEntry_t oilEntry;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOil), RT_ERR_NULL_POINTER);

    if (NULL == pOilPrev)
    {
        /* add to head */
        RTK_LIST_NODE_INSERT_HEAD(&pGroup->ip.oil_list, pOil, nodeRef);

        /* sync to H/W */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOil), errHandle, ret);
        RT_ERR_HDL(_dal_mango_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);

        /* update to IPMCAST if group is bound */
#if 0
        if (TRUE == pGroup->ip.ipmc_bind)
        {
            RT_ERR_HDL(_dal_mango_ipmc_l3OilIdx_update(unit, pGroup->ip.ipmc_entryIdx, oilIdx), errHandle, ret);
            MANGO_MCAST_DBG_PRINTF(2, "update IPMCAST: ipmc_entryIdx = %u, oilIdx = %d\n", pGroup->ip.ipmc_entryIdx, oilIdx);
        }
#else
        RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
        {
            RT_ERR_HDL(_dal_mango_ipmc_l3OilIdx_update(unit, pL3Node->ipmc_entryIdx, oilIdx), errHandle, ret);
            MANGO_MCAST_DBG_PRINTF(2, "update IPMCAST: ipmc_entryIdx = %u, oilIdx = %d\n", pL3Node->ipmc_entryIdx, oilIdx);
        }
#endif
    }
    else
    {
        /* add new_oil to the list:
         * I think it should add to the last of the list
         * so the traffic in current OIL list will not break.)
         */
        RTK_LIST_NODE_INSERT_AFTER(&pGroup->ip.oil_list, pOilPrev, pOil, nodeRef);

        /* sync to H/W */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOil), errHandle, ret);
        RT_ERR_HDL(_dal_mango_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOilPrev);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOilPrev), errHandle, ret);
        RT_ERR_HDL(_dal_mango_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
    }

errHandle:
    return ret;
}

static inline int32
_dal_mango_mcast_ipEgrIfOil_remove(uint32 unit, dal_mango_mcast_group_t *pGroup, dal_mango_mcast_oil_t *pOil)
{
    int32   ret = RT_ERR_OK;
    int32   oilIdx;
    dal_mango_mcast_oil_t *pOilPrev;
    dal_mango_mcast_oil_t *pOilNext;
    dal_mango_mcast_oilEntry_t oilEntry;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOil), RT_ERR_NULL_POINTER);

    if (pOil->fwdIndex >= 0)
        RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_free(unit, pOil->fwdIndex), errHandle, ret);

    /* update OIL node */
    pOil->fwdIndex = DAL_MANGO_MCAST_INVALID_FWD_IDX;

    /* update OIL list */
    MANGO_MCAST_DBG_PRINTF(3, "\n");
    if (pOil == RTK_LIST_NODE_HEAD(&pGroup->ip.oil_list))
    {
        /* need to change the head */
        pOilNext = RTK_LIST_NODE_NEXT(pOil, nodeRef);
        oilIdx = (NULL != pOilNext)? OIL_ENTRY_ADDR_TO_IDX(unit, pOilNext) : DAL_MANGO_MCAST_INVALID_OIL_IDX;

        /* update IPMC entry if it's bound */
#if 0
        if (TRUE == pGroup->ip.ipmc_bind)
        {
            MANGO_MCAST_DBG_PRINTF(3, "\n");
            RT_ERR_HDL(_dal_mango_ipmc_l3OilIdx_update(unit, pGroup->ip.ipmc_entryIdx, oilIdx), errHandle, ret);
        }
#else
        RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
        {
            MANGO_MCAST_DBG_PRINTF(3, "\n");
            RT_ERR_HDL(_dal_mango_ipmc_l3OilIdx_update(unit, pL3Node->ipmc_entryIdx, oilIdx), errHandle, ret);
        }
#endif

        /* remove OIL node from oil_list */
        RTK_LIST_NODE_REMOVE(&pGroup->ip.oil_list, pOil, nodeRef);
        RT_ERR_HDL(_dal_mango_mcast_oil_free(unit, pOil), errHandle, ret);
    }
    else
    {
        /* need to update the previous node */
        pOilPrev = RTK_LIST_NODE_PREV(pOil, nodeRef);

        /* remove OIL node from oil_list */
        RTK_LIST_NODE_REMOVE(&pGroup->ip.oil_list, pOil, nodeRef);
        RT_ERR_HDL(_dal_mango_mcast_oil_free(unit, pOil), errHandle, ret);

        /* sync to H/W */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOilPrev);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOilPrev), errHandle, ret);
        RT_ERR_HDL(_dal_mango_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
    }

errHandle:
    return ret;
}


static int32
_dal_mango_mcast_ipEgrIfL2_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    int32 old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    int32 new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    rtk_portmask_t fwd_pmsk;
    int32 fwdIndexAllocated = FALSE;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask), RT_ERR_PORT_MASK);

    /* caller has specified a fwd-pmsk index */
    if (pEgrIf->l2.fwdIndex >= 0)
    {
        if (pEgrIf->l2.fwdIndex != pGroup->ip.l2_fwdIndex)
        {
            old_pmsk_idx = pGroup->ip.l2_fwdIndex;

            /* just update the index, we don't update the portmask
             * because we expect user already update the portmask.
             */
            new_pmsk_idx = pEgrIf->l2.fwdIndex;
            RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), errHandle, ret);
            fwdIndexAllocated = TRUE;
        }
    }
    else
    {
        /* if portmask is not allocated yet, allocate a new one */
        old_pmsk_idx = pGroup->ip.l2_fwdIndex;
        if (old_pmsk_idx < 0)
        {
            new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
            RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), errHandle, ret);
            fwdIndexAllocated = TRUE;
        }
        else
        {
            new_pmsk_idx = old_pmsk_idx;
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
            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, new_pmsk_idx, &fwd_pmsk), errHandle, ret);
            RTK_PORTMASK_OR(fwd_pmsk, pEgrIf->l2.portmask);
        }

        /* write hw_portmask to PORTMASK table of pGroup->mc_pmsk_idx */
        RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, new_pmsk_idx, &fwd_pmsk), errHandle, ret);
    }

    if (TRUE == fwdIndexAllocated)
    {
        pGroup->ip.l2_fwdIndex = new_pmsk_idx;

        /* sync to H/W if it's bound */
#if 0
        if (TRUE == pGroup->ip.ipmc_bind)
        {
            MANGO_MCAST_DBG_PRINTF(3, "[DBG] sync to IPMCAST entry (idx=%d), new_pmsk_idx=%d\n", pGroup->ip.ipmc_entryIdx, new_pmsk_idx);
            RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pGroup->ip.ipmc_entryIdx, new_pmsk_idx), errHandle, ret);
        }
#else
        RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
        {
            MANGO_MCAST_DBG_PRINTF(3, "[DBG] sync to IPMCAST entry (idx=%d), new_pmsk_idx=%d\n", pL3Node->ipmc_entryIdx, new_pmsk_idx);
            RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pL3Node->ipmc_entryIdx, new_pmsk_idx), errHandle, ret);
        }
#endif

        /* free the old index if the new one has been created */
        if (old_pmsk_idx >= 0)
        {
            MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
        }
    }

    goto errOk;

errHandle:
    if (fwdIndexAllocated)
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, new_pmsk_idx), "");

errOk:
    return ret;
}

static int32
_dal_mango_mcast_ipEgrIfL2Ecid_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    int32 old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    int32 new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    int32 tmp_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    dal_mango_bpe_ecidPmsk_entry_t  bpe_pmsklist_entry;
    rtk_portmask_t tmp_pmsk;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask), RT_ERR_PORT_MASK);

    osal_memset(&tmp_pmsk, 0, sizeof(rtk_portmask_t));
    osal_memset(&bpe_pmsklist_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    osal_memcpy(&bpe_pmsklist_entry.ecid, &(pEgrIf->bpe.ecid), sizeof(bpe_pmsklist_entry.ecid));

    /* find original portmask index */
    if (pGroup->ip.l2_lstIndex != DAL_MANGO_MCAST_INVALID_LST_IDX)
    {
        if ((ret = dal_mango_bpe_ecidPmskEntry_get(unit, pGroup->ip.l2_lstIndex, &bpe_pmsklist_entry)) == RT_ERR_OK)
        {
            if(bpe_pmsklist_entry.port_type == BPE_PMSK_INDEX)
            {
                old_pmsk_idx = bpe_pmsklist_entry.portmask.pmsk_index;
            }
        }
    }

    if (pEgrIf->l2.fwdIndex == DAL_MANGO_MCAST_INVALID_FWD_IDX)
    {
        /* auto alloc forwarding portmask entry */
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), "");


        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, new_pmsk_idx, &pEgrIf->l2.portmask), "");

        bpe_pmsklist_entry.portmask.pmsk_index = new_pmsk_idx;

        RT_LOG(LOG_DEBUG, (MOD_BPE|MOD_DAL), "new fwdIndex=%u, bitmap0=0x%x, bitmap1=0x%x",
            new_pmsk_idx, pEgrIf->l2.portmask.bits[0], pEgrIf->l2.portmask.bits[1]);
    }
    else
    {
        /* manual assign a specific portmask entry*/
        bpe_pmsklist_entry.portmask.pmsk_index = pEgrIf->l2.fwdIndex;

        if (pEgrIf->l2.fwdIndex == old_pmsk_idx)
        {
            /* reuse original forwarding portmask entry, don't free it */
            old_pmsk_idx = -1;
        }
    }

    if (pGroup->ip.l2_lstIndex == DAL_MANGO_MCAST_INVALID_LST_IDX)
    {
        /* the first ecid_pmsk list index, create it */
        MCAST_RT_ERR_HDL_DBG(dal_mango_bpe_ecidPmskEntry_add(unit, BPE_PMSKLIST_INVALID, &bpe_pmsklist_entry), "");
        if (bpe_pmsklist_entry.curr_index != BPE_PMSKLIST_INVALID)
        {
            /* assign this index as the header of group */
            pGroup->ip.l2_lstIndex = bpe_pmsklist_entry.curr_index;
        }

        if (pGroup->ip.l2_fwdIndex == DAL_MANGO_MCAST_INVALID_FWD_IDX)
        {
            /* if this group doesn't support normal mcast bridging, allocate an empty portmask as dummy entry */
            MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_alloc(unit, &tmp_pmsk_idx), "");
            MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, tmp_pmsk_idx, &tmp_pmsk), "");
            pGroup->ip.l2_fwdIndex = tmp_pmsk_idx;
        }
    }
    else
    {
        MCAST_RT_ERR_HDL_DBG(dal_mango_bpe_ecidPmskEntry_add(unit, pGroup->ip.l2_lstIndex, &bpe_pmsklist_entry), "");
    }

    /* sync to H/W if it's bound */
#if 0
    if (TRUE == pGroup->ip.ipmc_bind)
    {
        MANGO_MCAST_DBG_PRINTF(3, "sync to IPMCAST entry (idx=%d), l2_pmsk_idx=%d\n",pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_fwdIndex);
        RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_fwdIndex), errHandle, ret);

        MANGO_MCAST_DBG_PRINTF(3, "sync to IPMCAST entry (idx=%d), ecid_pmsk_idx=%d\n",pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_lstIndex);
        RT_ERR_HDL(_dal_mango_ipmc_l2ListIdx_update(unit, pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_lstIndex), errHandle, ret);
    }
#else
    RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
    {
        MANGO_MCAST_DBG_PRINTF(3, "sync to IPMCAST entry (idx=%d), l2_pmsk_idx=%d\n", pL3Node->ipmc_entryIdx, pGroup->ip.l2_fwdIndex);
        RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pL3Node->ipmc_entryIdx, pGroup->ip.l2_fwdIndex), errHandle, ret);

        MANGO_MCAST_DBG_PRINTF(3, "sync to IPMCAST entry (idx=%d), ecid_pmsk_idx=%d\n", pL3Node->ipmc_entryIdx, pGroup->ip.l2_lstIndex);
        RT_ERR_HDL(_dal_mango_ipmc_l2ListIdx_update(unit, pL3Node->ipmc_entryIdx, pGroup->ip.l2_lstIndex), errHandle, ret);
    }
#endif

    goto errOk;

errHandle:
    /* free the old index if the new one has been created */
    if (old_pmsk_idx >= 0)
    {
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
    }

errOk:
    return ret;
}


static int32
_dal_mango_mcast_ipEgrIfL3_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   oilFound = FALSE;
    int32   oilCreated = FALSE;
    int32   old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    int32   new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    int32   fwdIndexAllocated = FALSE;
    uint64  sortKey;
    uint32  oilIdx;
    dal_mango_mcast_oil_t *pOilPrev = NULL;
    dal_mango_mcast_oil_t *pOil;
    dal_mango_mcast_oilEntry_t oilEntry;
    rtk_portmask_t fwd_pmsk;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l3.portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pEgrIf->l3.intf_id >= DAL_MANGO_L3_INTF_MAX), RT_ERR_INPUT);

    /* function body */
    sortKey = _dal_mango_mcast_oil_sortKey_ret(pEgrIf);
    MANGO_MCAST_DBG_PRINTF(3, "sortKey = 0x%llX\n", sortKey);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->ip.oil_list, pOil, nodeRef)
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
        RT_ERR_HDL(_dal_mango_mcast_oil_alloc(unit, &pOil), errHandle, ret);
        oilCreated = TRUE;

        /* initialize the new oil information, MC_PMSK_IDX must set to invalid first */
        pOil->sortKey = sortKey;
        pOil->egrif_type = RTK_MCAST_EGRIF_TYPE_L3;
        pOil->bridge_mode = (pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_BRIDGE)? TRUE : FALSE;
        pOil->l3_egr_intf_idx = pEgrIf->l3.intf_id;
        pOil->ecid = pEgrIf->l3.ecid;

        /* if the node is newly create, there must be no port mask index here. */
        old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    }

    /* caller has specified a fwd-pmsk index */
    if (pEgrIf->l3.fwdIndex >= 0)
    {
        /* just update the index, we don't update the portmask
         * because we expect user already update the portmask.
         */
        new_pmsk_idx = pEgrIf->l3.fwdIndex;
        RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), errHandle, ret);
        fwdIndexAllocated = TRUE;

        pOil->fwdIndex = new_pmsk_idx;
    }
    else
    {
        /* for the new allocated oil_node, portmask has not allocated yet, allocate a new one: */
        if (FALSE == oilFound)
        {
            new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
            RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), errHandle, ret);
            fwdIndexAllocated = TRUE;

            pOil->fwdIndex = new_pmsk_idx;
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
            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pOil->fwdIndex, &fwd_pmsk), errHandle, ret);
            RTK_PORTMASK_OR(fwd_pmsk, pEgrIf->l3.portmask);
        }

        /* write hw_port_mask to PORTMASK table of oil_node.MC_PMSK_IDX */
        RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, pOil->fwdIndex, &fwd_pmsk), errHandle, ret);
    }

    /* Update linked list and sync to H/W entry */
    if (FALSE == oilFound)
    {
        /* The following codes that add node to list can integrate to a API for all add functions to call. */
        /* add new oil to the list */
        RT_ERR_HDL(_dal_mango_mcast_ipEgrIfOil_insert(unit, pGroup, pOil, pOilPrev), errHandle, ret);
    }
    else
    {
        /* update the OIL entry to H/W entry */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOil), errHandle, ret);
        RT_ERR_HDL(_dal_mango_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
    }

    /* free the old fwd-index if new one is created */
    if ((TRUE == fwdIndexAllocated) && (old_pmsk_idx >= 0))
    {
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
    }

    MANGO_MCAST_DBG_PRINTF(2, "RTK_LIST_LENGTH(&pGroup->oil_list) = %u\n", RTK_LIST_LENGTH(&pGroup->ip.oil_list));
    if (!RTK_LIST_EMPTY(&pGroup->ip.oil_list))
    {
        MANGO_MCAST_DBG_PRINTF(2, "pGroup->oil_list's OIL_IDX = %ld\n", (long)OIL_ENTRY_ADDR_TO_IDX(unit, RTK_LIST_NODE_HEAD(&pGroup->ip.oil_list)));
    }

    goto errOk;

errHandle:
    /* free the created fwdIndex */
    if (fwdIndexAllocated)
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, new_pmsk_idx), "");

    /* free the created OIL node */
    if (oilCreated)
        MCAST_RT_ERR_HDL_DBG(_dal_mango_mcast_oil_free(unit, pOil), "");

errOk:
    return ret;
}

static int32
_dal_mango_mcast_ipEgrIfTunnel_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   oilFound = FALSE;
    int32   oilCreated = FALSE;
    uint64  sortKey;
    uint32  oilIdx;
    uint32  tunnelIntfIdx = 0;
    dal_mango_mcast_oil_t *pOilPrev = NULL;
    dal_mango_mcast_oil_t *pOil;
    dal_mango_mcast_oilEntry_t oilEntry;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pEgrIf->l3.intf_id >= DAL_MANGO_L3_INTF_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, pEgrIf->l3.intf_id, &tunnelIntfIdx), errOk, ret);

    /* function body */
    sortKey = _dal_mango_mcast_oil_sortKey_ret(pEgrIf);
    MANGO_MCAST_DBG_PRINTF(3, "sortKey = 0x%llX\n", sortKey);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->ip.oil_list, pOil, nodeRef)
    {
        if (pOil->sortKey > sortKey)
            break;

        if (pOil->sortKey == sortKey)
        {
            oilFound = TRUE;
            break;
        }

        pOilPrev = pOil;    /* candidate of the previous node for inserting a new node */
    }

    /* create a new oil node if entry does not exist yet */
    if (FALSE == oilFound)
    {
        RT_ERR_HDL(_dal_mango_mcast_oil_alloc(unit, &pOil), errHandle, ret);
        oilCreated = TRUE;

        /* initialize the new oil information, MC_PMSK_IDX must set to invalid first */
        pOil->sortKey = sortKey;
        pOil->egrif_type = RTK_MCAST_EGRIF_TYPE_TUNNEL;
        pOil->l3_egr_intf_idx = pEgrIf->l3.intf_id;
        pOil->fwdIndex = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    }

    /* Update linked list and sync to H/W entry */
    if (FALSE == oilFound)
    {
        /* The following codes that add node to list can integrate to a API for all add functions to call. */
        /* add new oil to the list */
        RT_ERR_HDL(_dal_mango_mcast_ipEgrIfOil_insert(unit, pGroup, pOil, pOilPrev), errHandle, ret);
    }
    else
    {
        /* update the OIL entry to H/W entry */
        oilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
        RT_ERR_HDL(mcast_util_dalOil2oilEntry(unit, &oilEntry, pOil), errHandle, ret);
        RT_ERR_HDL(_dal_mango_mcast_oilEntry_set(unit, oilIdx, &oilEntry), errHandle, ret);
    }

    MANGO_MCAST_DBG_PRINTF(2, "RTK_LIST_LENGTH(&pGroup->oil_list) = %u\n", RTK_LIST_LENGTH(&pGroup->ip.oil_list));
    if (!RTK_LIST_EMPTY(&pGroup->ip.oil_list))
    {
        MANGO_MCAST_DBG_PRINTF(2, "pGroup->oil_list's OIL_IDX = %u\n", OIL_ENTRY_ADDR_TO_IDX(unit, RTK_LIST_NODE_HEAD(&pGroup->ip.oil_list)));
    }

    goto errOk;

errHandle:
    /* free the created OIL node */
    if (oilCreated)
        MCAST_RT_ERR_HDL_DBG(_dal_mango_mcast_oil_free(unit, pOil), "");

errOk:
    return ret;
}

static int32
_dal_mango_mcast_ipEgrIfStk_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    uint32 portmask_val = 0;
    rtk_port_t port;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->stk.portmask), RT_ERR_PORT_MASK);

    if (pEgrIf->flags & RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE)
    {
        RTK_PORTMASK_ASSIGN(pGroup->ip.stk_pmsk, pEgrIf->stk.portmask);
    }
    else
    {
        RTK_PORTMASK_OR(pGroup->ip.stk_pmsk, pEgrIf->stk.portmask);
    }

    /* sync to H/W if it's bound */
#if 0
    if (TRUE == pGroup->ip.ipmc_bind)
    {
        /* stacking logical ports to stacking 16-bit portmask */
        RTK_PORTMASK_SCAN(pGroup->ip.stk_pmsk, port)
        {
            if (stackPortId2Idx[unit][port] < 16)
                portmask_val |= (0x1 << stackPortId2Idx[unit][port]);
        }

        RT_ERR_HDL(_dal_mango_ipmc_stkPmsk_update(unit, pGroup->ip.ipmc_entryIdx, portmask_val), errHandle, ret);
    }
#else
    /* stacking logical ports to stacking 16-bit portmask */
    RTK_PORTMASK_SCAN(pGroup->ip.stk_pmsk, port)
    {
        if (stackPortId2Idx[unit][port] < 16)
            portmask_val |= (0x1 << stackPortId2Idx[unit][port]);
    }

    RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
    {
        RT_ERR_HDL(_dal_mango_ipmc_stkPmsk_update(unit, pL3Node->ipmc_entryIdx, portmask_val), errHandle, ret);
    }
#endif

errHandle:
    return ret;
}


static int32
_dal_mango_mcast_ipEgrIfL2_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   needFreeL2PmskIdx = FALSE;
    rtk_portmask_t fwd_pmsk;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask), RT_ERR_PORT_MASK);

    /* function body */
    if (pEgrIf->l2.fwdIndex >= 0)
    {
        if (pGroup->ip.l2_fwdIndex != pEgrIf->l2.fwdIndex)
        {
            /* index no match, return error */
            return RT_ERR_INPUT;
        }

        needFreeL2PmskIdx = TRUE;
    }
    else
    {
        /* delete ports */
        if (pGroup->ip.l2_fwdIndex < 0)
        {
            /* portmask is never allocated, simply return ok */
            goto errOk;
        }

        RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pGroup->ip.l2_fwdIndex, &fwd_pmsk), errHandle,ret);
        /* remove user specified ports: */
        RTK_PORTMASK_REMOVE(fwd_pmsk, pEgrIf->l2.portmask);

        /* if all ports are zero, free the port_mask; otherwise, update to PORTMASK table */
        MANGO_MCAST_DBG_PRINTF(2, "RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk) = %d\n", RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk));
        if (0 == RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk))
        {
            /* release the portmask from ipmc table */
            needFreeL2PmskIdx = TRUE;
        }
        else
        {
            /* write hwp_port_mask to PORTMASK table of pGrpDb->l2_fwdIndex */
            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, pGroup->ip.l2_fwdIndex, &fwd_pmsk), errHandle, ret);
            goto errOk;
        }
    }

    /* free the portmask index if need */
    if (TRUE == needFreeL2PmskIdx)
    {
        MANGO_MCAST_DBG_PRINTF(2, "[DBG] needFreeL2PmskIdx = TRUE, pGroup->ip.l3_entry_list = %p\n", &pGroup->ip.l3_entry_list);

        /* first, sync to H/W entry if need */
#if 0
        if (TRUE == pGroup->ip.ipmc_bind)
        {
            RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pGroup->ip.ipmc_entryIdx, DAL_MANGO_MCAST_INVALID_FWD_IDX), errHandle, ret);
        }
#else
        RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
        {
            RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pL3Node->ipmc_entryIdx, DAL_MANGO_MCAST_INVALID_FWD_IDX), errHandle, ret);
        }
#endif

        /* free the fwd-index */
        RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_free(unit, pGroup->ip.l2_fwdIndex), errHandle, ret);

        pGroup->ip.l2_fwdIndex = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    }

    goto errOk;

errHandle:
errOk:
    return ret;
}

static int32
_dal_mango_mcast_ipEgrIfL2Ecid_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    dal_mango_bpe_ecidPmsk_entry_t  bpe_pmsklist_entry;
    rtk_portmask_t  tmp_pmsk;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask), RT_ERR_PORT_MASK);

    osal_memset(&tmp_pmsk, 0, sizeof(rtk_portmask_t));
    osal_memset(&bpe_pmsklist_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    osal_memcpy(&bpe_pmsklist_entry.ecid, &(pEgrIf->bpe.ecid), sizeof(bpe_pmsklist_entry.ecid));

    RT_ERR_HDL(dal_mango_bpe_ecidPmskEntry_get(unit, pGroup->ip.l2_lstIndex, &bpe_pmsklist_entry), errHandle,ret);

    /* function body */
    if (bpe_pmsklist_entry.port_type == BPE_PMSK_INDEX)
    {
        _dal_mango_l2_mcastFwdIndex_free(unit, bpe_pmsklist_entry.portmask.pmsk_index);
    }

    /* free ecid pmsk list table */
    RT_ERR_HDL(dal_mango_bpe_ecidPmskEntry_del(unit, pGroup->ip.l2_lstIndex, &bpe_pmsklist_entry), errHandle,ret);

    /* update the group and ipmc entry if the deleted entry is the header index */
    if (pGroup->ip.l2_lstIndex == bpe_pmsklist_entry.curr_index)
    {
        if (bpe_pmsklist_entry.next_index == BPE_PMSKLIST_INVALID)
        {
            /* no next valid list index */
            pGroup->ip.l2_lstIndex = DAL_MANGO_MCAST_INVALID_LST_IDX;

            /* check if l2 fwd portmask is all zero, free this dummy entry */
            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pGroup->ip.l2_fwdIndex, &tmp_pmsk), errHandle, ret);
            if (RTK_PORTMASK_IS_ALL_ZERO(tmp_pmsk))
            {
                _dal_mango_l2_mcastFwdIndex_free(unit, pGroup->ip.l2_fwdIndex);
                pGroup->ip.l2_fwdIndex = DAL_MANGO_MCAST_INVALID_FWD_IDX;
            }
        }
        else
        {
            /* assign header as the next valid index */
            pGroup->ip.l2_lstIndex = bpe_pmsklist_entry.next_index;
        }

#if 0
        /* sync to H/W entry if need */
        if (TRUE == pGroup->ip.ipmc_bind)
        {
            MANGO_MCAST_DBG_PRINTF(2, "sync to IPMCAST entry (idx=%d), l2_pmsk_idx=%d\n",pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_fwdIndex);
            RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_fwdIndex), errHandle, ret);

            MANGO_MCAST_DBG_PRINTF(2, "sync to IPMCAST entry (idx=%d), ecid_pmsk_idx=%d\n",pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_lstIndex);
            RT_ERR_HDL(_dal_mango_ipmc_l2ListIdx_update(unit, pGroup->ip.ipmc_entryIdx, pGroup->ip.l2_lstIndex), errHandle, ret);
        }
#else
        /* sync to H/W entry if need */
        RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
        {
            MANGO_MCAST_DBG_PRINTF(2, "sync to IPMCAST entry (idx=%d), l2_pmsk_idx=%d\n", pL3Node->ipmc_entryIdx, pGroup->ip.l2_fwdIndex);
            RT_ERR_HDL(_dal_mango_ipmc_l2PmskIdx_update(unit, pL3Node->ipmc_entryIdx, pGroup->ip.l2_fwdIndex), errHandle, ret);

            MANGO_MCAST_DBG_PRINTF(2, "sync to IPMCAST entry (idx=%d), ecid_pmsk_idx=%d\n", pL3Node->ipmc_entryIdx, pGroup->ip.l2_lstIndex);
            RT_ERR_HDL(_dal_mango_ipmc_l2ListIdx_update(unit, pL3Node->ipmc_entryIdx, pGroup->ip.l2_lstIndex), errHandle, ret);
        }
#endif
    }

    goto errOk;

errHandle:
errOk:
    return ret;
}

static int32
_dal_mango_mcast_ipEgrIfL3_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   oilFound = FALSE;
    int32   needFreeFwdIndex = FALSE;
    uint64  sortKey;
    dal_mango_mcast_oil_t *pOil;
    rtk_portmask_t fwd_pmsk;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l3.portmask), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pEgrIf->l3.intf_id >= DAL_MANGO_L3_INTF_MAX), RT_ERR_INPUT);

    /* function body */
    sortKey = _dal_mango_mcast_oil_sortKey_ret(pEgrIf);
    MANGO_MCAST_DBG_PRINTF(3, "sortKey = 0x%llX\n", sortKey);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->ip.oil_list, pOil, nodeRef)
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

        needFreeFwdIndex = TRUE;
    }
    else
    {
        /* delete ports */
        if (pOil->fwdIndex < 0)
        {
            /* portmask is never allocated, simply return ok */
            goto errOk;
        }

        RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pOil->fwdIndex, &fwd_pmsk), errHandle,ret);
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
            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, pOil->fwdIndex, &fwd_pmsk), errHandle, ret);
            goto errOk;
        }

    }

    /* free the OIL if need to free the fwd-index */
    if (TRUE == needFreeFwdIndex)
    {
        RT_ERR_HDL(_dal_mango_mcast_ipEgrIfOil_remove(unit, pGroup, pOil), errHandle, ret);
    }

    MANGO_MCAST_DBG_PRINTF(2, "RTK_LIST_LENGTH(&pGroup->oil_list) = %u\n", RTK_LIST_LENGTH(&pGroup->ip.oil_list));

    goto errOk;

errHandle:
errOk:
    return ret;
}

static int32
_dal_mango_mcast_ipEgrIfTunnel_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   oilFound = FALSE;
    uint64  sortKey;
    uint32  tunnelIntfIdx = 0;
    dal_mango_mcast_oil_t *pOil;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    RT_PARAM_CHK((pEgrIf->l3.intf_id >= DAL_MANGO_L3_INTF_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID), RT_ERR_CHIP_NOT_SUPPORTED);

    RT_ERR_HDL(_dal_mango_tunnel_tunnelIntfEntry_find(unit, pEgrIf->l3.intf_id, &tunnelIntfIdx), errOk, ret);

    /* function body */
    sortKey = _dal_mango_mcast_oil_sortKey_ret(pEgrIf);
    MANGO_MCAST_DBG_PRINTF(3, "sortKey = 0x%llX\n", sortKey);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->ip.oil_list, pOil, nodeRef)
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

    RT_ERR_HDL(_dal_mango_mcast_ipEgrIfOil_remove(unit, pGroup, pOil), errHandle, ret);

    MANGO_MCAST_DBG_PRINTF(2, "RTK_LIST_LENGTH(&pGroup->oil_list) = %u\n", RTK_LIST_LENGTH(&pGroup->ip.oil_list));

    goto errOk;

errHandle:
errOk:
    return ret;
}

static int32
_dal_mango_mcast_ipEgrIfStk_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    uint32 portmask_val = 0;
    rtk_port_t port;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->stk.portmask), RT_ERR_PORT_MASK);

    RTK_PORTMASK_REMOVE(pGroup->ip.stk_pmsk, pEgrIf->stk.portmask);

#if 0
    /* sync to H/W if it's bound */
    if (TRUE == pGroup->ip.ipmc_bind)
    {
        /* stacking logical ports to stacking 16-bit portmask */
        RTK_PORTMASK_SCAN(pGroup->ip.stk_pmsk, port)
        {
            if (stackPortId2Idx[unit][port] < 16)
                portmask_val |= (0x1 << stackPortId2Idx[unit][port]);
        }

        RT_ERR_HDL(_dal_mango_ipmc_stkPmsk_update(unit, pGroup->ip.ipmc_entryIdx, portmask_val), errHandle, ret);
    }
#else
    /* stacking logical ports to stacking 16-bit portmask */
    RTK_PORTMASK_SCAN(pGroup->ip.stk_pmsk, port)
    {
        if (stackPortId2Idx[unit][port] < 16)
            portmask_val |= (0x1 << stackPortId2Idx[unit][port]);
    }

    RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
    {
        RT_ERR_HDL(_dal_mango_ipmc_stkPmsk_update(unit, pL3Node->ipmc_entryIdx, portmask_val), errHandle, ret);
    }
#endif

errHandle:
    return ret;
}


static int32
_dal_mango_mcast_ipEgrIf_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    switch (pEgrIf->type)
    {
    case RTK_MCAST_EGRIF_TYPE_L2:
        ret = _dal_mango_mcast_ipEgrIfL2_add(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_L3:
        ret = _dal_mango_mcast_ipEgrIfL3_add(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_TUNNEL:
        ret = _dal_mango_mcast_ipEgrIfTunnel_add(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_BPE:
        ret = _dal_mango_mcast_ipEgrIfL2Ecid_add(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_STK:
        ret = _dal_mango_mcast_ipEgrIfStk_add(unit, pGroup, pEgrIf);
        break;

    default:
        ret = RT_ERR_INPUT;
        break;
    }

    return ret;
}

static int32
_dal_mango_mcast_ipEgrIf_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    switch (pEgrIf->type)
    {
    case RTK_MCAST_EGRIF_TYPE_L2:
        ret = _dal_mango_mcast_ipEgrIfL2_del(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_L3:
        ret = _dal_mango_mcast_ipEgrIfL3_del(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_TUNNEL:
        ret = _dal_mango_mcast_ipEgrIfTunnel_del(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_BPE:
        ret = _dal_mango_mcast_ipEgrIfL2Ecid_del(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_STK:
        ret = _dal_mango_mcast_ipEgrIfStk_del(unit, pGroup, pEgrIf);
        break;

    default:
        ret = RT_ERR_INPUT;
        break;
    }

    return ret;
}



static int32
_dal_mango_mcast_vlan_update(uint32 unit, rtk_vlan_t vid, dal_mango_mcast_l2TnlNode_t *pNode)
{
    int32   ret = RT_ERR_OK;
    vlan_entry_t vlanEntry;
    uint32  tunnel_lst_valid;
    uint32  tunnel_lst_idx;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,vid=%d,pNode=%p", unit, vid, pNode);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    /* function body */
    /* [SEM_LOCK] VLAN table */
    RT_ERR_CHK(_dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_VLAN), ret);

    if ((ret = table_read(unit, MANGO_VLANt, vid, (uint32 *)&vlanEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_MCAST|MOD_DAL), "");
        goto errTableRead;
    }

    if (NULL != pNode)
    {
        tunnel_lst_valid = 1;
        tunnel_lst_idx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNode);
    } else {
        tunnel_lst_valid = 0;
        tunnel_lst_idx = 0;
    }

    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_L2_TNL_LST_VALIDtf, &tunnel_lst_valid, (uint32 *)&vlanEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_MCAST|MOD_DAL), "");
        goto errTableFieldSet;
    }

    if ((ret = table_field_set(unit, MANGO_VLANt, MANGO_VLAN_L2_TNL_LST_IDXtf, &tunnel_lst_idx, (uint32 *)&vlanEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_MCAST|MOD_DAL), "");
        goto errTableFieldSet;
    }

    /* write back to the chip */
    if ((ret = table_write(unit, MANGO_VLANt, vid, (uint32 *)&vlanEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_MCAST|MOD_DAL), "");
        goto errTableWrite;
    }

    MANGO_MCAST_DBG_PRINTF(1, "[UPDATE] VID = %u, tunnel_lst_valid = %u, tunnel_lst_idx = %u\n", vid, tunnel_lst_valid, tunnel_lst_idx);

errTableWrite:
errTableFieldSet:
errTableRead:
    /* [SEM_UNLOCK] VLAN table */
    RT_ERR_CHK(_dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_VLAN), ret);

    return ret;
}

/* Function Name:
 *      _dal_mango_mcast_vlan_bind
 * Description:
 *      Bind the group to an VLAN entry
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group
 *      pNode     - pointer to the VLAN link-list node
 * Output:
 *      pVlanInfo - pointer to the VLAN info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32
_dal_mango_mcast_vlan_bind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_vlanNode_t *pNode, dal_mango_mcast_vlanInfo_t *pVlanInfo)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_MAC), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pVlanInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_MCAST_TYPE_MAC != RTK_MCAST_GROUP_ID_TYPE(groupId)), RT_ERR_INPUT);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* insert the node into the group */
    RTK_LIST_NODE_INSERT_TAIL(&pGroup->mac.vlan_list, pNode, nodeRef);

    MANGO_MCAST_DBG_PRINTF(2, "mcast group - VLAN count = %d\n", RTK_LIST_LENGTH(&pGroup->mac.vlan_list));

    /* return L2 tunnel list index */
    osal_memset(pVlanInfo, 0x00, sizeof(dal_mango_mcast_vlanInfo_t));
    if (RTK_LIST_LENGTH(&pGroup->mac.l2_tunnel_list) > 0)
    {
        pVlanInfo->tunnel_lst_valid = 1;
        pVlanInfo->tunnel_lst_idx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list));
    }

    MANGO_MCAST_DBG_PRINTF(2, "VLAN bind: groupId=0x%08X, vid=%d, tunnel=%d,%d\n",
        groupId, pNode->vid, pVlanInfo->tunnel_lst_valid, pVlanInfo->tunnel_lst_idx);

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_mango_mcast_vlan_unbind
 * Description:
 *      Unbind the group from an VLAN entry
 * Input:
 *      unit    - unit id
 *      groupId - mcast group ID
 *      pNode   - pointer to the VLAN link-list node
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
_dal_mango_mcast_vlan_unbind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_vlanNode_t *pNode)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_MAC), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* remove the node from the group */
    RTK_LIST_NODE_REMOVE(&pGroup->mac.vlan_list, pNode, nodeRef);

    MANGO_MCAST_DBG_PRINTF(2, "mcast group - VLAN count = %d (removed vid %u)\n", RTK_LIST_LENGTH(&pGroup->mac.vlan_list), pNode->vid);

    MANGO_MCAST_DBG_PRINTF(2, "VLAN unbind: groupId = 0x%08X, vid = %d\n", groupId, pNode->vid);

errHandle:
    return ret;
}

static int32
_dal_mango_mcast_l2mc_update(uint32 unit, dal_mango_l2_index_t l2Idx, int32 l2_fwdIndex, dal_mango_mcast_l2TnlNode_t *pNode)
{
    int32   ret = RT_ERR_OK;
    l2_entry_t l2_entry;
    uint32  l2_local_fwd_valid;
    uint32  l2_local_fwd_idx;
    uint32  l2_remote_fwd_valid;
    uint32  l2_tunnel_lst_idx;
    uint32  l2_physical_idx = 0;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,type=%d,index=%d,l2_fwdIndex=%d", unit, l2Idx.index_type, l2Idx.index, l2_fwdIndex);

    /* parameter check */
    // unnecessary RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);

    /* function body */
    /* [SEM_LOCK] L2 table */
    RT_ERR_CHK(_dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC), ret);

    _dal_mango_l2_hashPhyIndex_get(l2Idx.index, &l2_physical_idx);

    /* read entry from chip */
    if (l2Idx.index_type == L2_IN_HASH)
    {
        if ((ret = table_read(unit, MANGO_L2_MCt, ((l2_physical_idx << 2) | (l2Idx.hashdepth)), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            goto errTableRead;
        }
    }
    else
    {
        if ((ret = table_read(unit, MANGO_L2_CAM_MCt, (l2Idx.index), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            goto errTableRead;
        }
    }

    /* local fwd */
    l2_local_fwd_valid = (l2_fwdIndex >= 0)? 1 : 0;
    l2_local_fwd_idx = (l2_fwdIndex >= 0)? l2_fwdIndex : 0;

    /* remote fwd */
    if (NULL != pNode)
    {
        l2_remote_fwd_valid = 1;
        l2_tunnel_lst_idx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNode);
    } else {
        l2_remote_fwd_valid = 0;
        l2_tunnel_lst_idx = 0;
    }

    if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_LOCAL_FWDtf, &l2_local_fwd_valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        goto errTableFieldSet;
    }
    if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_MC_PMSK_IDXtf, &l2_local_fwd_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        goto errTableFieldSet;
    }
    if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_REMOTE_FWDtf, &l2_remote_fwd_valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        goto errTableFieldSet;
    }
    if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_L2_TNL_LST_IDXtf, &l2_tunnel_lst_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        goto errTableFieldSet;
    }

    /* write back to the chip */
    if (l2Idx.index_type == L2_IN_HASH)
    {
        if ((ret = table_write(unit, MANGO_L2_MCt, ((l2_physical_idx << 2) | (l2Idx.hashdepth)), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            goto errTableWrite;
        }
    }
    else
    {
        if ((ret = table_write(unit, MANGO_L2_CAM_MCt, (l2Idx.index), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            goto errTableWrite;
        }
    }

    MANGO_MCAST_DBG_PRINTF(1, "\n-----------------------------------------------------------------\n"
                              " l2Idx.index = %u, local_fwd = %u / %u, remote_fwd = %u / %u\n"
                              "-----------------------------------------------------------------\n",
        l2_physical_idx, l2_local_fwd_valid, l2_local_fwd_idx, l2_remote_fwd_valid, l2_tunnel_lst_idx);

errTableWrite:
errTableFieldSet:
errTableRead:
    /* [SEM_UNLOCK] L2 table */
    RT_ERR_CHK(_dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC), ret);

    return ret;
}

/* Function Name:
 *      _dal_mango_mcast_l2mc_bind
 * Description:
 *      Bind the group to an L2MC entry
 * Input:
 *      unit    - unit id
 *      groupId - mcast group
 *      pNode   - pointer to the L2 link-list node
 * Output:
 *      pL2Info - pointer to the L2 information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32
_dal_mango_mcast_l2mc_bind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_l2Node_t *pNode, dal_mango_mcast_l2Info_t *pL2Info)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;

    MANGO_MCAST_DBG_PRINTF(1, "L2MC bind: groupId = 0x%08X, l2Type=%d,l2Index=%d\n", groupId, pNode->l2Idx.index_type, pNode->l2Idx.index);

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_MAC), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2Info), RT_ERR_NULL_POINTER);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* insert the node into the group */
    RTK_LIST_NODE_INSERT_TAIL(&pGroup->mac.l2_entry_list, pNode, nodeRef);

    MANGO_MCAST_DBG_PRINTF(2, "mcast group - L2 entry count = %d\n", RTK_LIST_LENGTH(&pGroup->mac.l2_entry_list));

    /* return L2 tunnel list index */
    osal_memset(pL2Info, 0x00, sizeof(dal_mango_mcast_l2Info_t));
    pL2Info->fwdIndex = pGroup->mac.l2_fwdIndex;
    if (RTK_LIST_LENGTH(&pGroup->mac.l2_tunnel_list) > 0)
    {
        pL2Info->l2_tnl_lst_valid = 1;
        pL2Info->l2_tnl_lst_idx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list));
    }

errHandle:
    return ret;
}



/* Function Name:
 *      _dal_mango_mcast_l2mc_unbind
 * Description:
 *      Unbind the group from an L2MC entry
 * Input:
 *      unit    - unit id
 *      groupId - mcast group ID
 *      pNode   - pointer to the L2 link-list node
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
_dal_mango_mcast_l2mc_unbind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_l2Node_t *pNode)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;

    MANGO_MCAST_DBG_PRINTF(1, "L2MC unbind: groupId = 0x%08X, l2Type=%d,l2Index=%d\n", groupId, pNode->l2Idx.index_type, pNode->l2Idx.index);

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_MAC), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* remove the node from the group */
    RTK_LIST_NODE_REMOVE(&pGroup->mac.l2_entry_list, pNode, nodeRef);

    MANGO_MCAST_DBG_PRINTF(2, "mcast group - L2 entry count = %d\n", RTK_LIST_LENGTH(&pGroup->mac.l2_entry_list));

errHandle:
    return ret;
}

/* to get the unique sort key */
static inline uint32
_dal_mango_mcast_l2Tunnel_sortKey_ret(rtk_mcast_egrif_t *pEgrIf)
{
    uint32  sortKey;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEgrIf), 0ULL);   /* return key as zero */

    switch (pEgrIf->type)
    {
    case RTK_MCAST_EGRIF_TYPE_L2:
        {
            /* bit[31..24] = egrif_type
             * bit[23..0]  = reserved
             */
            sortKey = (((pEgrIf->type) & 0xFFUL) << 24);
        }
        break;

    case RTK_MCAST_EGRIF_TYPE_BPE:  /* BPE */
        {
            /* bit[31..24] = egrif_type
             * bit[23..22] = reserved
             * bit[21..0]  = ECID
             */
            sortKey = (((pEgrIf->type) & 0xFFUL) << 24) | \
                      (((pEgrIf->bpe.ecid) & 0x3FFFFFUL) << 0);
        }
        break;


    case RTK_MCAST_EGRIF_TYPE_VXLAN:  /* VXLAN */
        {
            /* bit[31..24] = egrif_type
             * bit[23..0]  = VXLAN entry idx
             */
            sortKey = (((pEgrIf->type) & 0xFFULL) << 24) | \
                      (((pEgrIf->vxlan.entry_idx) & 0xFFFFFFUL) << 0);
        }
        break;

    default:
        return 0;   /* should not happen */
    }

    MANGO_MCAST_DBG_PRINTF(3, "[DBG] sortKey = 0x%08X\n", sortKey);

    return sortKey;
}


static inline int32 _dal_mango_mcast_l2TnlNode_alloc(uint32 unit, dal_mango_mcast_l2TnlNode_t **ppTunnel)
{
    dal_mango_mcast_l2TnlNode_t *pTunnel;

    if (RTK_LIST_EMPTY(&_pMcastDb[unit]->HW.free_l2TunnelNode_list))
    {
        return RT_ERR_TBL_FULL;
    }

    pTunnel = RTK_LIST_NODE_HEAD(&_pMcastDb[unit]->HW.free_l2TunnelNode_list);
    if (NULL == pTunnel)
    {
        return RT_ERR_FAILED;   /* should not happen */
    }
    RTK_LIST_NODE_REMOVE(&_pMcastDb[unit]->HW.free_l2TunnelNode_list, pTunnel, nodeRef);
    RTK_LIST_NODE_REF_INIT(pTunnel, nodeRef);
    L2TUNNEL_NODE_VALIDATE(pTunnel);

    *ppTunnel = pTunnel;

    return RT_ERR_OK;
}

static inline int32 _dal_mango_mcast_l2TnlNode_free(uint32 unit, dal_mango_mcast_l2TnlNode_t *pTunnel)
{
    if (L2TUNNEL_NODE_IS_VALID(pTunnel))
    {
        L2TUNNEL_NODE_RESET(pTunnel);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_l2TunnelNode_list, pTunnel, nodeRef);

        return RT_ERR_OK;
    }

    return RT_ERR_ENTRY_NOTFOUND;
}

static int32
_dal_mango_mcast_l2TnlNode_set(uint32 unit, uint32 nodeIdx, uint32 nextNodeIdx, dal_mango_mcast_l2TnlNode_t *pNode)
{
    int32 ret = RT_ERR_OK;
    l2_tnl_lst_t        l2NodeEntry;
    uint32 ecid;
    uint32 port_type;
    uint32 egrIfType;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,index=%d,nextNodeIdx=%d",
        unit, nodeIdx, nextNodeIdx);

    /* parameter check */
    RT_PARAM_CHK(nodeIdx >= HAL_MAX_NUM_OF_L2TNL_LST(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(nextNodeIdx >= HAL_MAX_NUM_OF_L2TNL_LST(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pNode, RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&l2NodeEntry, 0x00, sizeof(l2_tnl_lst_t));

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "pNode->sortKey=0x%x, pNode->egrif_type=%d, pNode->tnl_data=0x%x",
        pNode->sortKey, pNode->egrif_type, pNode->tnl_data);

    egrIfType = pNode->egrif_type;
    switch (egrIfType)
    {
        case RTK_MCAST_EGRIF_TYPE_VXLAN:
            {
                MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_LSTt, \
                    MANGO_L2_TNL_LST_L2_TNL_IDXtf, pNode->tnl_data, l2NodeEntry, "", errHandle, ret);
                MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_L2_TNL_LSTt, \
                    MANGO_L2_TNL_LST_NEXT_IDXtf, nextNodeIdx, l2NodeEntry, "", errHandle, ret);
                MCAST_TABLE_WRITE_ERR_HDL(unit, MANGO_L2_TNL_LSTt, nodeIdx, l2NodeEntry, "", errHandle, ret);
            }
            break;

        case RTK_MCAST_EGRIF_TYPE_BPE:
            {
                ecid = ((pNode->sortKey) & 0x3FFFFFUL);
                MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
                    MANGO_ECID_PMSK_LIST_ECIDtf, ecid, l2NodeEntry, "", errHandle, ret);
                port_type = pNode->port_type;
                MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
                    MANGO_ECID_PMSK_LIST_PORT_TYPEtf, port_type, l2NodeEntry, "", errHandle, ret);
                MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
                    MANGO_ECID_PMSK_LIST_PMSK_IDXtf, pNode->tnl_data, l2NodeEntry, "", errHandle, ret);
                MCAST_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
                    MANGO_ECID_PMSK_LIST_NEXT_IDXtf, nextNodeIdx, l2NodeEntry, "", errHandle, ret);
                MCAST_TABLE_WRITE_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, nodeIdx, l2NodeEntry, "", errHandle, ret);
            }
            break;

        default:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

errHandle:

    return ret;
}

static inline int32
_dal_mango_mcast_macEgrIfTunnel_insert(uint32 unit, dal_mango_mcast_group_t *pGroup, dal_mango_mcast_l2TnlNode_t *pNode, dal_mango_mcast_l2TnlNode_t *pNodePrev)
{
    int32   ret = RT_ERR_OK;
    uint32  nodeIdx;
    uint32  nextNodeIdx;
    dal_mango_mcast_l2TnlNode_t *pNextNode;
    dal_mango_mcast_vlanNode_t  *pVlanNode;
    dal_mango_mcast_l2Node_t    *pL2Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);

    if (NULL == pNodePrev)
    {
        /* add new node as the head of tunnel list*/
        RTK_LIST_NODE_INSERT_HEAD(&pGroup->mac.l2_tunnel_list, pNode, nodeRef);

        /* update the bound entries */
        RTK_LIST_FOREACH(&pGroup->mac.vlan_list, pVlanNode, nodeRef)
        {
            _dal_mango_mcast_vlan_update(unit, pVlanNode->vid, RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list));
        }
        RTK_LIST_FOREACH(&pGroup->mac.l2_entry_list, pL2Node, nodeRef)
        {
            _dal_mango_mcast_l2mc_update(unit, pL2Node->l2Idx, pGroup->mac.l2_fwdIndex, RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list));
        }
    }
    else
    {
        /* add new node in the tunnel list */
        RTK_LIST_NODE_INSERT_AFTER(&pGroup->mac.l2_tunnel_list, pNodePrev, pNode, nodeRef);

        /* update the next index of previous node as current node */
        nodeIdx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNodePrev);
        nextNodeIdx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNode);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "set previous nodeIdx=%d, nextNodeIdx=%d",
            nodeIdx, nextNodeIdx);

        /* sync to H/W */
        RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_set(unit, nodeIdx, nextNodeIdx, pNodePrev), errHandle, ret);
    }

    /* update content (including next index) of this new node*/
    pNextNode = RTK_LIST_NODE_NEXT(pNode, nodeRef);
    nodeIdx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNode);
    nextNodeIdx = (NULL != pNextNode)? L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNextNode) : L2TUNNEL_NODE_END_OF_LIST;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "update current nodeIdx=%d with nextNodeIdx=%d", nodeIdx, nextNodeIdx);

    /* sync to H/W */
    RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_set(unit, nodeIdx, nextNodeIdx, pNode), errHandle, ret);

errHandle:
    return ret;
}

static inline int32
_dal_mango_mcast_macEgrIfTunnel_remove(uint32 unit, dal_mango_mcast_group_t *pGroup, dal_mango_mcast_l2TnlNode_t *pNode)
{
    int32   ret = RT_ERR_OK;
    uint32  nodeIdx;
    uint32  nextNodeIdx;
    dal_mango_mcast_l2TnlNode_t *pNodePrev;
    dal_mango_mcast_l2TnlNode_t *pNodeNext;
    dal_mango_mcast_vlanNode_t  *pVlanNode;
    dal_mango_mcast_l2Node_t    *pL2Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pNode), RT_ERR_NULL_POINTER);

    pNodeNext = RTK_LIST_NODE_NEXT(pNode, nodeRef);
    nextNodeIdx = (NULL != pNodeNext)? L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNodeNext) : L2TUNNEL_NODE_END_OF_LIST;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "nextNodeIdx=%d", nextNodeIdx);

    /* update the node list */
    if (pNode == RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list))
    {
        /* the removed node is the head, update new head for bound entries*/
        RTK_LIST_FOREACH(&pGroup->mac.vlan_list, pVlanNode, nodeRef)
        {
            _dal_mango_mcast_vlan_update(unit, pVlanNode->vid, pNodeNext);
        }
        RTK_LIST_FOREACH(&pGroup->mac.l2_entry_list, pL2Node, nodeRef)
        {
            _dal_mango_mcast_l2mc_update(unit, pL2Node->l2Idx, pGroup->mac.l2_fwdIndex, pNodeNext);
        }
    }
    else
    {
        /* the removed node is not the head, update the next index of the previous node */
        pNodePrev = RTK_LIST_NODE_PREV(pNode, nodeRef);
        nodeIdx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNodePrev);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "set previous nodeIdx=%d,nextNodeIdx=%d",
            nodeIdx, nextNodeIdx);

        /* sync to H/W */
        RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_set(unit, nodeIdx, nextNodeIdx, pNodePrev), errHandle, ret);
    }

    /* remove L2TNL node from l2_tunnel_list */
    RTK_LIST_NODE_REMOVE(&pGroup->mac.l2_tunnel_list, pNode, nodeRef);
    RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_free(unit, pNode), errHandle, ret);


errHandle:
    return ret;
}

static int32
_dal_mango_mcast_macEgrIfL2_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;
    int32 old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    int32 new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    rtk_portmask_t fwd_pmsk;
    int32 fwdIndexAllocated = FALSE;
    dal_mango_mcast_l2Node_t *pL2Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask), RT_ERR_PORT_MASK);

    /* caller has specified a fwd-pmsk index */
    if (pEgrIf->l2.fwdIndex >= 0)
    {
        if (pEgrIf->l2.fwdIndex != pGroup->mac.l2_fwdIndex)
        {
            old_pmsk_idx = pGroup->mac.l2_fwdIndex;

            /* just update the index, we don't update the portmask
             * because we expect user already update the portmask.
             */
            new_pmsk_idx = pEgrIf->l2.fwdIndex;
            RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), errHandle, ret);
            fwdIndexAllocated = TRUE;
        }
    }
    else
    {
        /* if portmask is not allocated yet, allocate a new one */
        old_pmsk_idx = pGroup->mac.l2_fwdIndex;
        if (old_pmsk_idx < 0)
        {
            new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
            RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_alloc(unit, &new_pmsk_idx), errHandle, ret);
            fwdIndexAllocated = TRUE;
        }
        else
        {
            new_pmsk_idx = old_pmsk_idx;
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
            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, new_pmsk_idx, &fwd_pmsk), errHandle, ret);
            RTK_PORTMASK_OR(fwd_pmsk, pEgrIf->l2.portmask);
        }

        /* write hw_portmask to PORTMASK table of pGroup->mc_pmsk_idx */
        RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, new_pmsk_idx, &fwd_pmsk), errHandle, ret);
    }

    if (TRUE == fwdIndexAllocated)
    {
        MANGO_MCAST_DBG_PRINTF(3, "[DBG] fwdIndexAllocated = TRUE\n");

        pGroup->mac.l2_fwdIndex = new_pmsk_idx;

        /* sync to all associated L2 entries */
        RTK_LIST_FOREACH(&pGroup->mac.l2_entry_list, pL2Node, nodeRef)
        {
            /* update L2 entry */
            _dal_mango_mcast_l2mc_update(unit, pL2Node->l2Idx, pGroup->mac.l2_fwdIndex, RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list));
        }

        /* free the old index if the new one has been created */
        if (old_pmsk_idx >= 0)
        {
            MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
        }
    }

    goto errOk;

errHandle:
    if (fwdIndexAllocated)
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, new_pmsk_idx), "");

errOk:
    return ret;
}

static int32
_dal_mango_mcast_macEgrIfL2_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   needFreeL2PmskIdx = FALSE;
    rtk_portmask_t fwd_pmsk;
    dal_mango_mcast_l2Node_t *pL2Node;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEgrIf->l2.portmask), RT_ERR_PORT_MASK);

    /* function body */
    if (pEgrIf->l2.fwdIndex >= 0)
    {
        if(pGroup->mac.l2_fwdIndex != pEgrIf->l2.fwdIndex)
        {
            /* index no match, return error */
            return RT_ERR_INPUT;
        }

        needFreeL2PmskIdx = TRUE;
    }
    else
    {
        /* delete ports */
        if(pGroup->mac.l2_fwdIndex < 0)
        {
            /* portmask is never allocated, simply return ok */
            goto errOk;
        }

        RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pGroup->mac.l2_fwdIndex, &fwd_pmsk), errHandle,ret);
        /* remove user specified ports: */
        RTK_PORTMASK_REMOVE(fwd_pmsk, pEgrIf->l2.portmask);

        /* if all ports are zero, free the port_mask; otherwise, update to PORTMASK table */
        MANGO_MCAST_DBG_PRINTF(3, "RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk) = %d\n", RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk));
        if (0 == RTK_PORTMASK_GET_PORT_COUNT(fwd_pmsk))
        {
            /* release the portmask from associated entries */
            needFreeL2PmskIdx = TRUE;
        }
        else
        {
            /* write hwp_port_mask to PORTMASK table of pGrpDb->l2_fwdIndex */
            RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, pGroup->mac.l2_fwdIndex, &fwd_pmsk), errHandle, ret);
            goto errOk;
        }
    }

    /* free the portmask index if need */
    if (TRUE == needFreeL2PmskIdx)
    {
        MANGO_MCAST_DBG_PRINTF(3, "[DBG] needFreeL2PmskIdx = TRUE\n");

        /* sync to all the associated L2 entries */
        RTK_LIST_FOREACH(&pGroup->mac.l2_entry_list, pL2Node, nodeRef)
        {
            /* update L2 entry */
            _dal_mango_mcast_l2mc_update(unit, pL2Node->l2Idx, DAL_MANGO_MCAST_INVALID_FWD_IDX, RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list));
        }

        /* free the fwd-index */
        RT_ERR_HDL(_dal_mango_l2_mcastFwdIndex_free(unit, pGroup->mac.l2_fwdIndex), errHandle, ret);

        pGroup->mac.l2_fwdIndex = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    }

    goto errOk;

errHandle:
errOk:
    return ret;
}

static int32
_dal_mango_mcast_macEgrIfL2Ecid_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   nodeFound = FALSE;
    int32   nodeCreated = FALSE;
    uint32  sort_key;
    uint32  idx_curr;
    uint32  idx_next;
    int32   new_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    int32   old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    dal_mango_mcast_l2TnlNode_t *pNodePrev = NULL;
    dal_mango_mcast_l2TnlNode_t *pNode;
    dal_mango_mcast_l2TnlNode_t *pNextNode;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    FLEX_TABLE_FMT_CHECK(unit, RTK_SWITCH_FLEX_TBL_FMT_PE_ECID);

    /* function body */
    sort_key = _dal_mango_mcast_l2Tunnel_sortKey_ret(pEgrIf);

    /* try to find the corresponding tunnel node */
    RTK_LIST_FOREACH(&pGroup->mac.l2_tunnel_list, pNode, nodeRef)
    {
        if (pNode->sortKey > sort_key)
        {
            /* out of boundary, not found!*/
            break;
        }

        if (pNode->sortKey == sort_key)
        {
            nodeFound = TRUE;
            break;
        }

        /* candidate of the previous node for inserting a new node */
        pNodePrev = pNode;
    }

    if(nodeFound)
    {
        /* get the original forwarding portmask index*/
        if(BPE_PMSK_INDEX == pNode->port_type)
        {
            old_pmsk_idx = pNode->tnl_data;
        }
    }
    else
    {
        /* node not found, create a new tunnel node*/
        RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_alloc(unit, &pNode), errHandle, ret);
        nodeCreated = TRUE;
        pNode->sortKey = sort_key;
        pNode->egrif_type = RTK_MCAST_EGRIF_TYPE_BPE;
    }

    /* set forwarding port infomation of this node*/
    if (DAL_MANGO_MCAST_INVALID_FWD_IDX == pEgrIf->bpe.fwdIndex)
    {
        /* auto alloc */
        /* translate as offset and partial portmask at first */
        /* if portlist is out of range and translation failed, alloc new forwarding index*/
        RT_ERR_HDL(mcast_util_rtkPmsk2dalEcidPmsk(unit, &pEgrIf->bpe.portmask, pNode), errHandle, ret);

        if(BPE_PMSK_INDEX == pNode->port_type)
        {
            new_pmsk_idx = pNode->tnl_data;
        }
    }
    else
    {
        /* manual assign forwarding index*/
        pNode->port_type = BPE_PMSK_INDEX;
        pNode->tnl_data = pEgrIf->bpe.fwdIndex;

        if (pEgrIf->bpe.fwdIndex == old_pmsk_idx)
        {
            /* reuse the original forwarding index, reinit and don't free it */
            old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
        }
    }

    /* sync to H/W */
    if (!nodeFound)
    {
        /* insert new tunnel node in the tunnel list */
        RT_ERR_HDL(_dal_mango_mcast_macEgrIfTunnel_insert(unit, pGroup, pNode, pNodePrev), errHandle, ret);
    }
    else
    {
        /* update tunnel node */
        pNextNode = RTK_LIST_NODE_NEXT(pNode, nodeRef);
        idx_next = (NULL != pNextNode)? L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNextNode) : L2TUNNEL_NODE_END_OF_LIST;
        idx_curr = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNode);
        RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_set(unit, idx_curr, idx_next, pNode), errHandle, ret);
    }

    goto errOk;

errHandle:
    /* free the created tunnel node */
    if (nodeCreated)
    {
        MCAST_RT_ERR_HDL_DBG(_dal_mango_mcast_l2TnlNode_free(unit, pNode), "");
    }
    if (DAL_MANGO_MCAST_INVALID_FWD_IDX != new_pmsk_idx)
    {
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, new_pmsk_idx), "");
    }

errOk:
    if (DAL_MANGO_MCAST_INVALID_FWD_IDX != old_pmsk_idx)
    {
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
    }

    return ret;
}

static int32
_dal_mango_mcast_macEgrIfL2Ecid_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   nodeFound = FALSE;
    uint32  sort_key;
    int32   old_pmsk_idx = DAL_MANGO_MCAST_INVALID_FWD_IDX;
    dal_mango_mcast_l2TnlNode_t *pNode;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);
    FLEX_TABLE_FMT_CHECK(unit, RTK_SWITCH_FLEX_TBL_FMT_PE_ECID);

    /* function body */
    sort_key = _dal_mango_mcast_l2Tunnel_sortKey_ret(pEgrIf);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->mac.l2_tunnel_list, pNode, nodeRef)
    {
        if (pNode->sortKey > sort_key)
            break;

        if (pNode->sortKey == sort_key)
        {
            nodeFound = TRUE;
            break;
        }
    }

    /* return error if entry is not found */
    if (FALSE == nodeFound)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }
    else
    {
        if (BPE_PMSK_INDEX == pNode->port_type)
        {
            old_pmsk_idx = pNode->tnl_data;
        }
    }

    /* free the node entry */
    RT_ERR_HDL(_dal_mango_mcast_macEgrIfTunnel_remove(unit, pGroup, pNode), errHandle, ret);

    goto errOk;

errHandle:
errOk:
    if (DAL_MANGO_MCAST_INVALID_FWD_IDX != old_pmsk_idx)
    {
        MCAST_RT_ERR_HDL_DBG(_dal_mango_l2_mcastFwdIndex_free(unit, old_pmsk_idx), "");
    }

    return ret;
}




static int32
_dal_mango_mcast_macEgrIfVxlan_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   nodeFound = FALSE;
    int32   nodeCreated = FALSE;
    uint32  sortKey;
    uint32  nodeIdx;
    uint32  nextNodeIdx;
    dal_mango_mcast_l2TnlNode_t *pNodePrev = NULL;
    dal_mango_mcast_l2TnlNode_t *pNode;
    dal_mango_mcast_l2TnlNode_t *pNextNode;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
    sortKey = _dal_mango_mcast_l2Tunnel_sortKey_ret(pEgrIf);
    MANGO_MCAST_DBG_PRINTF(3, "sortKey = 0x%08X\n", sortKey);

    /* try to find the corresponding tunnel node */
    RTK_LIST_FOREACH(&pGroup->mac.l2_tunnel_list, pNode, nodeRef)
    {
        if (pNode->sortKey > sortKey)
            break;

        if (pNode->sortKey == sortKey)
        {
            nodeFound = TRUE;
            break;
        }

        pNodePrev = pNode;    /* candidate of the previous node for inserting a new node */
    }

    /* create a new tunnel node if entry does not exist yet */
    if (FALSE == nodeFound)
    {
        RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_alloc(unit, &pNode), errHandle, ret);
        nodeCreated = TRUE;

        /* initialize the new oil information, MC_PMSK_IDX must set to invalid first */
        pNode->sortKey = sortKey;
        pNode->egrif_type = RTK_MCAST_EGRIF_TYPE_VXLAN;
        pNode->tnl_data = pEgrIf->vxlan.entry_idx;
    }

    /* Update linked list and sync to H/W entry */
    if (FALSE == nodeFound)
    {
        /* The following codes that add node to list can integrate to a API for all add functions to call. */
        /* add new tunnel to the list */
        RT_ERR_HDL(_dal_mango_mcast_macEgrIfTunnel_insert(unit, pGroup, pNode, pNodePrev), errHandle, ret);
    }
    else
    {
        pNextNode = RTK_LIST_NODE_NEXT(pNode, nodeRef);

        /* update the node entry to H/W entry */
        nodeIdx = L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNode);
        nextNodeIdx = (NULL != pNextNode)? L2TUNNEL_NODE_ADDR_TO_IDX(unit, pNextNode) : L2TUNNEL_NODE_END_OF_LIST;
        RT_ERR_HDL(_dal_mango_mcast_l2TnlNode_set(unit, nodeIdx, nextNodeIdx, pNode), errHandle, ret);
    }

    MANGO_MCAST_DBG_PRINTF(3, "RTK_LIST_LENGTH(&pGroup->mac.l2_tunnel_list) = %u\n", RTK_LIST_LENGTH(&pGroup->mac.l2_tunnel_list));
    if (!RTK_LIST_EMPTY(&pGroup->mac.l2_tunnel_list))
    {
        MANGO_MCAST_DBG_PRINTF(3, "pGroup's head_node_idx = %ld\n", (long)L2TUNNEL_NODE_ADDR_TO_IDX(unit, RTK_LIST_NODE_HEAD(&pGroup->mac.l2_tunnel_list)));
    }

    goto errOk;

errHandle:
    /* free the created tunnel node */
    if (nodeCreated)
        MCAST_RT_ERR_HDL_DBG(_dal_mango_mcast_l2TnlNode_free(unit, pNode), "");

errOk:
    return ret;
}

static int32
_dal_mango_mcast_macEgrIfVxlan_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    int32   nodeFound = FALSE;
    uint32  sortKey;
    dal_mango_mcast_l2TnlNode_t *pNode;

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!GROUP_ENTRY_IS_VALID(pGroup), RT_ERR_ENTRY_NOTFOUND);

    /* function body */
    sortKey = _dal_mango_mcast_l2Tunnel_sortKey_ret(pEgrIf);
    MANGO_MCAST_DBG_PRINTF(3, "sortKey = 0x%08X\n", sortKey);

    /* try to find the corresponding oil node */
    RTK_LIST_FOREACH(&pGroup->mac.l2_tunnel_list, pNode, nodeRef)
    {
        if (pNode->sortKey > sortKey)
            break;

        if (pNode->sortKey == sortKey)
        {
            nodeFound = TRUE;
            break;
        }
    }

    /* return error if entry is not found */
    if (FALSE == nodeFound)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    /* free the node entry */
    RT_ERR_HDL(_dal_mango_mcast_macEgrIfTunnel_remove(unit, pGroup, pNode), errHandle, ret);

    MANGO_MCAST_DBG_PRINTF(3, "RTK_LIST_LENGTH(&pGroup->mac.l2_tunnel_list) = %u\n", RTK_LIST_LENGTH(&pGroup->mac.l2_tunnel_list));

    goto errOk;

errHandle:
errOk:
    return ret;
}

static int32
_dal_mango_mcast_macEgrIf_add(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    switch (pEgrIf->type)
    {
    case RTK_MCAST_EGRIF_TYPE_L2:
        ret = _dal_mango_mcast_macEgrIfL2_add(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_BPE:
        ret = _dal_mango_mcast_macEgrIfL2Ecid_add(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_VXLAN:
        ret = _dal_mango_mcast_macEgrIfVxlan_add(unit, pGroup, pEgrIf);
        break;

    default:
        ret = RT_ERR_INPUT;
        break;
    }

    return ret;
}

static int32
_dal_mango_mcast_macEgrIf_del(uint32 unit, dal_mango_mcast_group_t *pGroup, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;

    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    switch (pEgrIf->type)
    {
    case RTK_MCAST_EGRIF_TYPE_L2:
        ret = _dal_mango_mcast_macEgrIfL2_del(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_BPE:
        ret = _dal_mango_mcast_macEgrIfL2Ecid_del(unit, pGroup, pEgrIf);
        break;

    case RTK_MCAST_EGRIF_TYPE_VXLAN:
        ret = _dal_mango_mcast_macEgrIfVxlan_del(unit, pGroup, pEgrIf);
        break;

    default:
        ret = RT_ERR_INPUT;
        break;
    }

    return ret;
}



/* Function Name:
 *      dal_mango_mcastMapper_init
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
dal_mango_mcastMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mcast_init = dal_mango_mcast_init;
    pMapper->mcast_group_create = dal_mango_mcast_group_create;
    pMapper->mcast_group_destroy = dal_mango_mcast_group_destroy;
    pMapper->mcast_group_getNext = dal_mango_mcast_group_getNext;
    pMapper->mcast_nextHop_get = dal_mango_mcast_egrIf_get;
    pMapper->mcast_nextHop_add = dal_mango_mcast_egrIf_add;
    pMapper->mcast_nextHop_del = dal_mango_mcast_egrIf_del;
    pMapper->mcast_nextHop_delAll = dal_mango_mcast_egrIf_delAll;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_mcast_init
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
dal_mango_mcast_init(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    int32   idx;
    dal_mango_mcast_oil_t *pOil;
    dal_mango_mcast_group_t *pGroup;
    dal_mango_mcast_l2TnlNode_t *pTunnel;
    dal_mango_mcast_l3Node_t *pIpmc;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(mcast_init[unit]);
    mcast_init[unit] = INIT_NOT_COMPLETED;

    /* clear external semaphore luck status */
    mcast_extSemLocked[unit] = FALSE;

    /* create semaphore */
    mcast_sem[unit] = osal_sem_mutex_create();
    if (0 == mcast_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_MCAST), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* allocate memory that we need */
    if ((_pMcastDb[unit] = osal_alloc(sizeof(dal_mango_mcast_drvDb_t))) == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_MCAST), "out of memory");
        ret = RT_ERR_FAILED;
        goto errInit;
    }

    /* initialize drvDb */
    osal_memset(_pMcastDb[unit], 0x00, sizeof(dal_mango_mcast_drvDb_t));

    /* free OIL list initialize (don't use index 0, reserve it) */
    RTK_LIST_INIT(&_pMcastDb[unit]->HW.free_oil_list);
    for (idx=1; idx<DAL_MANGO_MCAST_OIL_MAX; idx++)
    {
        pOil = OIL_ENTRY_ADDR(unit, idx);
        OIL_ENTRY_RESET(pOil);
        RTK_LIST_NODE_REF_INIT(pOil, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_oil_list, pOil, nodeRef);
    }
    /* free group list initialize */
    RTK_LIST_INIT(&_pMcastDb[unit]->free_group_list);
    for (idx=0; idx<DAL_MANGO_MCAST_GROUP_MAX; idx++)
    {
        pGroup = GROUP_ENTRY_ADDR(unit, idx);
        GROUP_ENTRY_RESET(pGroup);
        RTK_LIST_NODE_REF_INIT(pGroup, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->free_group_list, pGroup, nodeRef);
    }
    /* free l2TunnelNode list initialize */
    RTK_LIST_INIT(&_pMcastDb[unit]->HW.free_l2TunnelNode_list);
    for (idx=1; idx<DAL_MANGO_MCAST_L2TUNNEL_NODE_MAX; idx++)
    {
        pTunnel = L2TUNNEL_NODE_ADDR(unit, idx);
        L2TUNNEL_NODE_RESET(pTunnel);
        RTK_LIST_NODE_REF_INIT(pTunnel, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_l2TunnelNode_list, pTunnel, nodeRef);
    }
    /* free l3Node initialize */
    for (idx=0; idx<DAL_MANGO_MCAST_L3_NODE_MAX; idx++)
    {
        pIpmc = L3_NODE_ADDR(unit, idx);
        L3_NODE_RESET(pIpmc);
        RTK_LIST_NODE_REF_INIT(pIpmc, nodeRef);
        RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_l3Node_list, pIpmc, nodeRef);
    }

#if MANGO_MCAST_DBG
    RTK_LIST_FOREACH(&_pMcastDb[unit]->free_group_list, pGroup, nodeRef)
    {
        MANGO_MCAST_DBG_PRINTF(3, "Group-Node: idx = %u (GROUP_ENTRY_ADDR_TO_IDX), pNode = %p, pPrev = %p, pNext = %p\n", \
            GROUP_ENTRY_ADDR_TO_IDX(unit, pGroup), pGroup, pGroup->nodeRef.pPrev, pGroup->nodeRef.pNext);
    }
#endif

    /* set init flag to complete init */
    mcast_init[unit] = INIT_COMPLETED;

    goto errOk;

errInit:
    osal_sem_mutex_destroy(mcast_sem[unit]);

errOk:
    return ret;
} /* end of dal_mango_mcast_init */

/* Function Name:
 *      _dal_mango_mcast_sem_lock
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
_dal_mango_mcast_sem_lock(uint32 unit)
{
    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    MCAST_SEM_LOCK(unit);

    mcast_extSemLocked[unit] = TRUE;

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_mcast_sem_unlock
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
_dal_mango_mcast_sem_unlock(uint32 unit)
{
    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    mcast_extSemLocked[unit] = FALSE;

    MCAST_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_mcast_fwdIdx_get
 * Description:
 *      get the group l2 forward table index and oil index
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group ID
 * Output:
 *      pL2fwdIdx - pointer of the l2 forward index
 *      pL2LstIdx - pointer of the l2 MC/TNL index
 *      pOilIdx   - pointer of the l3 oil index
 *      pStkPmsk  - pointer of the l3 stack forward portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      if not found, or invalid. *pL2fwdIdx = -1, *pL2LstIdx = -1, *pOilIdx = -1
 */
int32
_dal_mango_mcast_fwdIdx_get(uint32 unit, rtk_mcast_group_t groupId, int32 *pL2fwdIdx, int32 *pL2LstIdx, int32 *pOilIdx, uint32 *pStkPmsk)
{
    int32   ret = RT_ERR_OK;
    uint32  groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;
    dal_mango_mcast_oil_t *pOil;
    rtk_port_t port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d,pL2fwdIdx=%p,pOilIdx=%p", unit, groupId, pL2fwdIdx, pOilIdx);

    /* check external semaphore lock status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

    /* parameter check */
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pL2fwdIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2LstIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pOilIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pStkPmsk), RT_ERR_NULL_POINTER);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret = RT_ERR_MCAST_GROUP_NOT_FOUND;
        goto errHandle;
    }

    *pL2fwdIdx = pGroup->ip.l2_fwdIndex;

    *pL2LstIdx = pGroup->ip.l2_lstIndex;

    pOil = RTK_LIST_NODE_HEAD(&pGroup->ip.oil_list);
    if (OIL_ENTRY_IS_VALID(pOil))
    {
        *pOilIdx = OIL_ENTRY_ADDR_TO_IDX(unit, pOil);
    }
    else
    {
        *pOilIdx = DAL_MANGO_MCAST_INVALID_OIL_IDX;
    }


    /* stacking logical ports to stacking 16-bit portmask */
    *pStkPmsk = 0;
    RTK_PORTMASK_SCAN(pGroup->ip.stk_pmsk, port)
    {
        MANGO_MCAST_DBG_PRINTF(1, "[pGroup->ip.stk_pmsk] port = %d, stackPortId2Idx=%d\n",
            port, stackPortId2Idx[unit][port]);

        if (stackPortId2Idx[unit][port] < 16)
            *pStkPmsk |= (0x1 << stackPortId2Idx[unit][port]);
    }

errHandle:
    return ret;
}

#if 0
/* Function Name:
 *      _dal_mango_mcast_ipmc_bindState_get
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
_dal_mango_mcast_ipmc_bindState_get(uint32 unit, rtk_mcast_group_t groupId, rtk_enable_t *pBind, uint32 *pEntryIdx)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;
    //dal_mango_mcast_l3Node_t *pL3Node;

    /* check external semaphore lock status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_IP), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
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
#if 0
    *pBind = (pGroup->ip.ipmc_bind)? TRUE : FALSE;
    *pEntryIdx = pGroup->ip.ipmc_entryIdx;
#else
#if 0
    RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
    {
        if (pL3Node->ipmc_entryIdx == entryIdx)
        {
            *pBind = TRUE;
            *pEntryIdx = pL3Node->ipmc_entryIdx;
            goto errOk;
        }
    }
#endif
    *pBind = FALSE;
    *pEntryIdx = 0;
#endif

errHandle:
//errOk:
    return ret;
}
#endif

/* Function Name:
 *      _dal_mango_mcast_ipmc_bind
 * Description:
 *      Bind the group to an IPMC entry
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group
 *      pIpmcAddr - pointer to the ipmc addr
 *      entryIdx  - ipmc entry index
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
_dal_mango_mcast_ipmc_bind(uint32 unit, rtk_mcast_group_t groupId, rtk_ipmc_addr_t *pIpmcAddr, uint32 entryIdx)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* check external semaphore lock status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

    /* parameter check */
    RT_PARAM_CHK((groupType != RTK_MCAST_TYPE_IP), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pIpmcAddr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((entryIdx >= DAL_MANGO_IPMCAST_L3_ENTRY_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

#if 0
    if ((TRUE == pGroup->ip.ipmc_bind) && \
        (pGroup->ip.ipmc_entryIdx != entryIdx))
    {
        ret =  RT_ERR_ENTRY_REFERRED;
        goto errHandle;
    }

    /* update group entry */
    pGroup->ip.ipmc_flags = pIpmcAddr->ipmc_flags;
    pGroup->ip.ipmc_entryIdx = entryIdx;
    pGroup->ip.ipmc_bind = TRUE;
#else
    RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
    {
        if (pL3Node->ipmc_entryIdx == entryIdx) /* update exsting node */
        {
            pL3Node->ipmc_flags = pIpmcAddr->ipmc_flags;
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
    RTK_LIST_NODE_INSERT_TAIL(&pGroup->ip.l3_entry_list, pL3Node, nodeRef);

    /* update node */
    pL3Node->ipmc_entryIdx  = entryIdx;
    pL3Node->ipmc_flags     = pIpmcAddr->ipmc_flags;

    MANGO_MCAST_DBG_PRINTF(3, "pL3Node->ipmc_entryIdx = %d, entryIdx = %d\n", pL3Node->ipmc_entryIdx, entryIdx);
#endif

errHandle:
errOk:
    return ret;
}

/* Function Name:
 *      _dal_mango_mcast_ipmc_unbind
 * Description:
 *      Unbind the group from an IPMC entry
 * Input:
 *      unit     - unit id
 *      groupId  - mcast group ID
 *      entryIdx - pointer to the ipmc addr table index
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
_dal_mango_mcast_ipmc_unbind(uint32 unit, rtk_mcast_group_t groupId, uint32 entryIdx)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    int32               groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;
    dal_mango_mcast_l3Node_t *pL3Node;

    /* check external semaphore lock status */
    RT_PARAM_CHK((FALSE == mcast_extSemLocked[unit]), RT_ERR_FAILED);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    //RT_PARAM_CHK((NULL == pEntryIdx), RT_ERR_NULL_POINTER);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret =  RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

#if 0
    /* check binding status */
    if (FALSE == pGroup->ip.ipmc_bind)
    {
        ret = RT_ERR_FAILED;
        goto errHandle;
    }

    /* return hardward entry index */
    *pEntryIdx = pGroup->ip.ipmc_entryIdx;

    /* clear IPMC binding info */
    pGroup->ip.ipmc_bind = FALSE;
    pGroup->ip.ipmc_entryIdx = 0;
#else
    RTK_LIST_FOREACH(&pGroup->ip.l3_entry_list, pL3Node, nodeRef)
    {
        MANGO_MCAST_DBG_PRINTF(3, "pL3Node->ipmc_entryIdx = %d, entryIdx = %d\n", pL3Node->ipmc_entryIdx, entryIdx);
        if (pL3Node->ipmc_entryIdx == entryIdx)
        {
            RTK_LIST_NODE_REMOVE(&pGroup->ip.l3_entry_list, pL3Node, nodeRef);
            RTK_LIST_NODE_REF_INIT(pL3Node, nodeRef);
            RTK_LIST_NODE_INSERT_TAIL(&_pMcastDb[unit]->HW.free_l3Node_list, pL3Node, nodeRef);

            goto errOk;
        }
    }

    /* not found and do nothing */
    MANGO_MCAST_DBG_PRINTF(0, "unbind (%u) failed!\n", entryIdx);
    ret =  RT_ERR_FAILED;
#endif

errHandle:
errOk:
    return ret;
}

/* Function Name:
 *      dal_mango_mcast_group_create
 * Description:
 *      Create a multicast group
 * Input:
 *      unit     - unit id
 *      flags    - RTK_MCAST_FLAG_WITH_ID
 *      type     - multicast group type
 * Output:
 *      pGroupId - pointer to multicast group ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *
 */
int32
dal_mango_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroupId)
{
    int32   ret = RT_ERR_OK;
    uint32  groupIdx;
    dal_mango_mcast_group_t *pGroup;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,flags=0x%08x,type=%d,pGroup=%p", unit, flags, type, pGroupId);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_MCAST_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pGroupId), RT_ERR_NULL_POINTER);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (type)
    {
    case RTK_MCAST_TYPE_MAC:
        {
            /* caller has specified a group ID */
            if (flags & RTK_MCAST_FLAG_WITH_ID)
            {
                if (DAL_MANGO_MCAST_GROUP_ID_TYPE(*pGroupId) != RTK_MCAST_TYPE_MAC)
                {
                    ret = RT_ERR_INPUT;
                    goto errType;
                }

                groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(*pGroupId);
                if (groupIdx >= DAL_MANGO_MCAST_GROUP_MAX)
                {
                    ret = RT_ERR_OUT_OF_RANGE;
                    goto errOutOfRange;
                }

                RT_ERR_HDL(_dal_mango_mcast_group_alloc_byIdx(unit, &pGroup, groupIdx), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_mango_mcast_group_alloc(unit, &pGroup), errHandle, ret);
            }

            groupIdx = GROUP_ENTRY_ADDR_TO_IDX(unit, pGroup);
            pGroup->groupId = DAL_MANGO_MCAST_GROUP_ID(type, groupIdx);
            pGroup->mac.l2_fwdIndex = DAL_MANGO_MCAST_INVALID_FWD_IDX;
            //pGroup->mac.l2_lstIndex = DAL_MANGO_MCAST_INVALID_LST_IDX;

            *pGroupId = pGroup->groupId;

            goto errOk;
        }
        break;

    case RTK_MCAST_TYPE_IP:
        {
            /* caller has specified a group ID */
            if (flags & RTK_MCAST_FLAG_WITH_ID)
            {
                if (DAL_MANGO_MCAST_GROUP_ID_TYPE(*pGroupId) != RTK_MCAST_TYPE_IP)
                {
                    ret = RT_ERR_INPUT;
                    goto errType;
                }

                groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(*pGroupId);
                if (groupIdx >= DAL_MANGO_MCAST_GROUP_MAX)
                {
                    ret = RT_ERR_OUT_OF_RANGE;
                    goto errOutOfRange;
                }

                RT_ERR_HDL(_dal_mango_mcast_group_alloc_byIdx(unit, &pGroup, groupIdx), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(_dal_mango_mcast_group_alloc(unit, &pGroup), errHandle, ret);
            }

            groupIdx = GROUP_ENTRY_ADDR_TO_IDX(unit, pGroup);
            pGroup->groupId = DAL_MANGO_MCAST_GROUP_ID(type, groupIdx);
            pGroup->ip.l2_fwdIndex = DAL_MANGO_MCAST_INVALID_FWD_IDX;
            pGroup->ip.l2_lstIndex = DAL_MANGO_MCAST_INVALID_LST_IDX;

            *pGroupId = pGroup->groupId;

            goto errOk;
        }
        break;

    default :
        {
            ret = RT_ERR_FAILED;
            goto errUnkownType;
        }
        break;
    }


errType:
errUnkownType:
errOutOfRange:
errHandle:
errOk:
    MCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_mcast_group_destroy
 * Description:
 *      Destrory the multicast group
 * Input:
 *      unit    - unit id
 *      groupId - multicast group ID
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
dal_mango_mcast_group_destroy(uint32 unit, rtk_mcast_group_t groupId)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    uint32              groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d", unit, groupId);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (groupType)
    {
    case RTK_MCAST_TYPE_MAC:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            if (!RTK_LIST_EMPTY(&pGroup->mac.vlan_list))
            {
                ret = RT_ERR_ENTRY_REFERRED;
                goto errHandle;
            }

            if (!RTK_LIST_EMPTY(&pGroup->mac.l2_entry_list))
            {
                ret = RT_ERR_ENTRY_REFERRED;
                goto errHandle;
            }

            if ((pGroup->mac.l2_fwdIndex >= 0) || (RTK_LIST_LENGTH(&pGroup->mac.l2_tunnel_list) > 0))
            {
                /* resource not free yet, return error. */
                ret = RT_ERR_ENTRY_REFERRED;
                goto errHandle;
            }

            RT_ERR_HDL(_dal_mango_mcast_group_free(unit, pGroup), errHandle, ret);
        }
        break;

    case RTK_MCAST_TYPE_IP:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            //if (TRUE == pGroup->ip.ipmc_bind)
            if (RTK_LIST_LENGTH(&pGroup->ip.l3_entry_list) > 0)
            {
                ret = RT_ERR_ENTRY_REFERRED;
                goto errHandle;
            }

            if ((pGroup->ip.l2_fwdIndex >= 0) || !RTK_LIST_EMPTY(&pGroup->ip.oil_list))
            {
                /* resource not free yet, return error. */
                ret = RT_ERR_ENTRY_REFERRED;
                goto errHandle;
            }

            RT_ERR_HDL(_dal_mango_mcast_group_free(unit, pGroup), errHandle, ret);
        }
        break;

    default:
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
 *      dal_mango_mcast_group_getNext
 * Description:
 *      Get next multicast group
 * Input:
 *      unit     - unit id
 *      type     - mcast type
 *      pBase    - start index
 * Output:
 *      pGroupId - pointer to multicast group ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) use type = 0 to indicate include all type
 *      (2) use pBase = -1 to indicate get first valid group
 */
int32
dal_mango_mcast_group_getNext(uint32 unit, rtk_mcast_type_t type, int32 *pBase, rtk_mcast_group_t *pGroupId)
{
    int32   ret = RT_ERR_OK;
    int32   groupIdx;
    rtk_mcast_group_t groupId;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_MCAST_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pGroupId), RT_ERR_NULL_POINTER);

    /* function body */
    MCAST_SEM_LOCK(unit);

    /* scan the whole group table */
    for (groupIdx=(*pBase<0)?(0):((*pBase)+1); groupIdx<DAL_MANGO_MCAST_GROUP_MAX; groupIdx++)
    {
        if (GROUP_IS_VALID(unit, groupIdx))
        {
            MANGO_MCAST_DBG_PRINTF(3, "groupIdx = %d\n", groupIdx);

            groupId = _pMcastDb[unit]->group[groupIdx].groupId;

            if ((0 != type) && (type !=  DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId)))
                continue;

            *pGroupId = groupId;
            *pBase = groupIdx;

            goto errOk;
        }
    }

    if (groupIdx >= DAL_MANGO_MCAST_GROUP_MAX)
        *pBase = -1;

errOk:
    MCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_mcast_egrIf_get
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit         - unit id
 *      groupId      - multicast group ID
 *      egrIf_max    - number of rtk_mcast_egrif_t entry in pEgrIf buffer
 *      pEgrIf       - buffer pointer
 * Output:
 *      pEgrIf_count - returned rtk_mcast_egrif_t entry number
 *      pEgrIf       - pointer to the next hop information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE   - partition value is out of range
 * Note:
 *      (1) should input enough buffer of pEgrIf
 */
int32
dal_mango_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t groupId, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    uint32              groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    int32   idx;
    dal_mango_mcast_group_t *pGroup;
    dal_mango_mcast_oil_t *pOil;
    dal_mango_mcast_l2TnlNode_t *pNode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d,pNh=%p,pNum=%p", unit, groupId, pEgrIf, pEgrIf_count);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEgrIf_count), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (groupType)
    {
    case RTK_MCAST_TYPE_MAC:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);
            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            idx = 0;

            /* reset the returned number */
            *pEgrIf_count = 0;

            /* L2 nexthop */
            if ((idx < egrIf_max) && (pGroup->mac.l2_fwdIndex >= 0))
            {
                pEgrIf[idx].type = RTK_MCAST_EGRIF_TYPE_L2;
                pEgrIf[idx].l2.fwdIndex = pGroup->mac.l2_fwdIndex;
                RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pGroup->mac.l2_fwdIndex, &pEgrIf[idx].l2.portmask), errHandle, ret);

                *pEgrIf_count = (++idx);
            }

            RTK_LIST_FOREACH(&pGroup->mac.l2_tunnel_list, pNode, nodeRef)
            {
                if (idx >= egrIf_max)
                    break;

                RT_ERR_HDL(mcast_util_dalTunnel2rtkMcNh(unit, pNode, &pEgrIf[idx]), errHandle, ret);

                *pEgrIf_count = (++idx);
            }
        }
        break;

    case RTK_MCAST_TYPE_IP:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);
            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            idx = 0;

            /* reset the returned number */
            *pEgrIf_count = 0;

            /* L2 nexthop */
            if ((idx < egrIf_max) && (pGroup->ip.l2_fwdIndex >= 0))
            {
                pEgrIf[idx].type = RTK_MCAST_EGRIF_TYPE_L2;
                pEgrIf[idx].l2.fwdIndex = pGroup->ip.l2_fwdIndex;
                RT_ERR_HDL(_dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pGroup->ip.l2_fwdIndex, &pEgrIf[idx].l2.portmask), errHandle, ret);

                *pEgrIf_count = (++idx);
            }

            /* STK nexthop */
            if ((idx < egrIf_max) && (0 != RTK_PORTMASK_GET_PORT_COUNT(pGroup->ip.stk_pmsk)))
            {
                pEgrIf[idx].type = RTK_MCAST_EGRIF_TYPE_STK;
                pEgrIf[idx].stk.portmask = pGroup->ip.stk_pmsk;

                *pEgrIf_count = (++idx);
            }

            RTK_LIST_FOREACH(&pGroup->ip.oil_list, pOil, nodeRef)
            {
                if (idx >= egrIf_max)
                    break;

                RT_ERR_HDL(mcast_util_dalOil2rtkMcNh(unit, &pEgrIf[idx], pOil), errHandle, ret);

                *pEgrIf_count = (++idx);
            }
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        goto errHandle;
    }

errHandle:
    MCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_mcast_egrIf_add
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit    - unit id
 *      groupId - multicast group ID
 *      pEgrIf  - buffer pointer of mcast nh
 * Output:
*       None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_PORT_MASK          - invalid portmask
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found
 *      RT_ERR_OUT_OF_RANGE       - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
int32
dal_mango_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t groupId, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    uint32              groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d,pOif=%p", unit, groupId, pEgrIf);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

    /* lock semaphore */
    MCAST_SEM_LOCK(unit);

    /* check existent */
    if (!GROUP_ENTRY_IS_VALID(pGroup))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    switch (groupType)
    {
    case RTK_MCAST_TYPE_MAC:
        {
            RT_ERR_HDL(_dal_mango_mcast_macEgrIf_add(unit, pGroup, pEgrIf), errHandle, ret);
        }
        break;

    case RTK_MCAST_TYPE_IP:
        {
            RT_ERR_HDL(_dal_mango_mcast_ipEgrIf_add(unit, pGroup, pEgrIf), errHandle, ret);
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
    /* unlock semaphore */
    MCAST_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_mcast_egrIf_del
 * Description:
 *      Delete a NH from multicast group replication list
 * Input:
 *      unit    - unit id
 *      groupId - multicast group ID
 *      pEgrIf  - pointer to multicast next hop information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_PORT_MASK          - invalid portmask
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found
 *      RT_ERR_OUT_OF_RANGE       - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
int32
dal_mango_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t groupId, rtk_mcast_egrif_t *pEgrIf)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    uint32              groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d,pEgrIf=%p", unit, groupId, pEgrIf);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (groupType)
    {
    case RTK_MCAST_TYPE_MAC:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            /* if the pmcNh is not exist, it will stop */
            RT_ERR_HDL(_dal_mango_mcast_macEgrIf_del(unit, pGroup, pEgrIf), errHandle, ret);
        }
        break;

    case RTK_MCAST_TYPE_IP:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            /* if the pmcNh is not exist, it will stop */
            RT_ERR_HDL(_dal_mango_mcast_ipEgrIf_del(unit, pGroup, pEgrIf), errHandle, ret);
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
 *      rtk_mcast_egrIf_delAll
 * Description:
 *      Delete all NHs from multicast group replication list
 * Input:
 *      unit    - unit id
 *      groupId - multicast group ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 */

int32
dal_mango_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t groupId)
{
    int32   ret = RT_ERR_OK;
    rtk_mcast_egrif_t  nhEntry;
    rtk_mcast_type_t    groupType = DAL_MANGO_MCAST_GROUP_ID_TYPE(groupId);
    uint32              groupIdx = DAL_MANGO_MCAST_GROUP_ID_IDX(groupId);
    dal_mango_mcast_group_t *pGroup;
    dal_mango_mcast_oil_t *pOil;
    dal_mango_mcast_oil_t *pTemp;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MCAST), "unit=%d,groupId=%d", unit, groupId);

    /* check Init status */
    RT_INIT_CHK(mcast_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((groupType >= RTK_MCAST_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((groupIdx >= DAL_MANGO_MCAST_GROUP_MAX), RT_ERR_OUT_OF_RANGE);

    /* function body */
    MCAST_SEM_LOCK(unit);

    switch (groupType)
    {
    case RTK_MCAST_TYPE_MAC:
        {
            /* these 2 types are not supported yet,
             * will need further discussion with module PIC to see if they need to use this API
             */
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            goto errHandle;
        }
        break;

    case RTK_MCAST_TYPE_IP:
        {
            pGroup = GROUP_ENTRY_ADDR(unit, groupIdx);

            MANGO_MCAST_DBG_PRINTF(3, "pGroup = %p\n", pGroup);
            if (!GROUP_ENTRY_IS_VALID(pGroup))
            {
                ret = RT_ERR_ENTRY_NOTFOUND;
                goto errHandle;
            }

            /* l2 bridge */
            if (pGroup->ip.l2_fwdIndex >= 0)
            {
                osal_memset(&nhEntry, 0x00, sizeof(rtk_mcast_egrif_t));
                nhEntry.type = RTK_MCAST_EGRIF_TYPE_L2;
                nhEntry.l2.fwdIndex = pGroup->ip.l2_fwdIndex;
                /* remove all ports */
                HWP_GET_ALL_PORTMASK(unit, nhEntry.l2.portmask);

                RT_ERR_HDL(_dal_mango_mcast_ipEgrIfL2_del(unit, pGroup, &nhEntry), errHandle, ret);
            }

            if (0 != RTK_PORTMASK_GET_PORT_COUNT(pGroup->ip.stk_pmsk))
            {
                osal_memset(&nhEntry, 0x00, sizeof(rtk_mcast_egrif_t));
                nhEntry.type = RTK_MCAST_EGRIF_TYPE_STK;
                nhEntry.stk.portmask = pGroup->ip.stk_pmsk;

                RT_ERR_HDL(_dal_mango_mcast_ipEgrIfStk_del(unit, pGroup, &nhEntry), errHandle, ret);
            }

            RTK_LIST_FOREACH_REVERSE_SAFE(&pGroup->ip.oil_list, pOil, pTemp, nodeRef)
            {
                MANGO_MCAST_DBG_PRINTF(3, "pOil = %p, oilIdx = %ld\n", pOil, (long)OIL_ENTRY_ADDR_TO_IDX(unit, pOil));

                RT_ERR_HDL(_dal_mango_mcast_ipEgrIfOil_remove(unit, pGroup, pOil), errHandle, ret);
#if 0   /* old method - lower performance */
                RT_ERR_HDL(mcast_util_dalOil2rtkMcNh(unit, &nhEntry, pOil), errHandle, ret);
                RT_ERR_HDL(_dal_mango_mcast_ipEgrIf_del(unit, pGroup, &nhEntry), errHandle, ret);
#endif
            }

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


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
 * Purpose : Definition those public vlan APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) BPE
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_bpe.h>
#include <rtk/default.h>
#include <rtk/bpe.h>
#include <dal/mango/dal_mango_switch.h>
#include <dal/mango/dal_mango_l2.h>


/*
 * Symbol Definition
 */
#define MANGO_BPE_DBG      (0)

static uint32 _typeFwdMode[] = {
    RTK_BPE_FWDMODE_CTRL_BRIDGE,
    RTK_BPE_FWDMODE_PORT_EXTENDER,
    };

static uint32 _typeEcidEqPcidAct[] = {
    ACTION_FORWARD,
    ACTION_DROP,
    ACTION_TRAP2CPU,
    ACTION_COPY2CPU,
    };

static uint32 _typeEcidRpfFailAct[] = {
    ACTION_FORWARD,
    ACTION_DROP,
    ACTION_TRAP2CPU,
    ACTION_COPY2CPU,
    ACTION_TRAP2MASTERCPU,
    ACTION_COPY2MASTERCPU,
    };

static uint32 _typeEgrTagSts[] = {
    RTK_BPE_TAGSTS_UNTAGGED,
    RTK_BPE_TAGSTS_TAGGED,
    RTK_BPE_TAGSTS_PCID_CHK_MCAST_UNTAGGED,
    RTK_BPE_TAGSTS_PCID_CHK_MCAST_TAGGED,
    };

static uint32 _typeEgrVlanTagSts[] = {
    RTK_BPE_VTAGSTS_NORMAL,
    RTK_BPE_VTAGSTS_MCAST_TAGGED,
    };



/*
 * Data Declaration
 */
static uint32               bpe_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         bpe_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         ecidPsmk_sem[RTK_MAX_NUM_OF_UNIT];
static uint32               pvidTable_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               pvidTable_cam_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               ecidPmskTable_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               peFwdTable_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               peFwdTable_cam_size[RTK_MAX_NUM_OF_UNIT] = {0};


static dal_mango_bpe_pmskList_t   free_pmsk_list[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
#define MANGO_BPE_DBG_PRINTF(fmt, ...) \
do { \
    if (MANGO_BPE_DBG) \
        osal_printf("%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
} while (0)

/* bpe semaphore handling */
#define BPE_SEM_LOCK(unit) \
do { \
    if (osal_sem_mutex_take(bpe_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK) \
    { \
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_BPE|MOD_DAL), "semaphore lock failed"); \
        return RT_ERR_SEM_LOCK_FAILED; \
    } \
} while(0)

#define BPE_SEM_UNLOCK(unit) \
do { \
    if (osal_sem_mutex_give(bpe_sem[unit]) != RT_ERR_OK) \
    { \
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_BPE|MOD_DAL), "semaphore unlock failed"); \
        return RT_ERR_SEM_UNLOCK_FAILED; \
    } \
} while(0)

/* ECID_PSMK semaphore handling */
#define ECID_PSMK_SEM_LOCK(unit) \
    do { \
        if (osal_sem_mutex_take(ecidPsmk_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK) \
        { \
            RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_BPE|MOD_DAL), "semaphore lock failed"); \
            return RT_ERR_SEM_LOCK_FAILED; \
        } \
    } while(0)

#define ECID_PSMK_SEM_UNLOCK(unit) \
    do { \
        if (osal_sem_mutex_give(ecidPsmk_sem[unit]) != RT_ERR_OK) \
        { \
            RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_BPE|MOD_DAL), "semaphore unlock failed"); \
            return RT_ERR_SEM_UNLOCK_FAILED; \
        } \
    } while(0)

#define PE_FWD_SEM_LOCK(unit) \
do { \
    if (_dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC) != RT_ERR_OK) \
    { \
        return RT_ERR_SEM_LOCK_FAILED; \
    } \
} while(0)

#define PE_FWD_SEM_UNLOCK(unit) \
do { \
    if (_dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC) != RT_ERR_OK) \
    { \
        return RT_ERR_SEM_UNLOCK_FAILED; \
    } \
} while(0)



#define BPE_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret) \
do { \
    if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK) \
    { \
        RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
        goto _gotoErrLbl; \
    } \
} while(0)
#define BPE_REG_FIELD_WRITE_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret) \
do { \
    if ((_ret = reg_field_write(_unit, _reg, _field, &_val)) != RT_ERR_OK) \
    { \
        RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
        goto _gotoErrLbl;\
    } \
} while(0)

#define BPE_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret) \
    do { \
        if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK) \
        { \
            RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
            goto _gotoErrLbl;\
        } \
    } while(0)
#define BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret) \
    do { \
        if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK) \
        { \
            RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
            goto _gotoErrLbl;\
        } \
    } while(0)

#define BPE_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret) \
    do { \
        if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK) \
        { \
            RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
            goto _gotoErrLbl; \
        } \
    } while(0)
#define BPE_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret) \
    do { \
        if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK) \
        { \
            RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
            goto _gotoErrLbl; \
        } \
    } while(0)
#define BPE_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret) \
    do { \
        if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        { \
            RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
            goto _gotoErrLbl; \
        } \
    } while(0)
#define BPE_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret) \
    do { \
        if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
        { \
            RT_ERR(_ret, (MOD_BPE|MOD_DAL), _errMsg); \
            goto _gotoErrLbl; \
        } \
    } while(0)

#define BPE_TYPE_TO_VALUE(_actArray, _value, _type, _errMsg, _errHandle, _retval) \
do { \
    if ((_retval = RT_UTIL_ACTLIST_INDEX_GET(_actArray, _value, _type)) != RT_ERR_OK) \
    { \
        RT_ERR(_retval, (MOD_BPE|MOD_DAL), _errMsg); \
        goto _errHandle; \
    } \
} while(0)
#define BPE_VALUE_TO_TYPE(_actArray, _type, _value, _errMsg, _errHandle, _retval) \
do { \
    if ((_retval = RT_UTIL_ACTLIST_ACTION_GET(_actArray, _type, _value)) != RT_ERR_OK) \
    { \
        RT_ERR(_retval, (MOD_BPE|MOD_DAL), _errMsg); \
        goto _errHandle; \
    } \
} while(0)

#define BPE_TABLE_FMT_CHECK(_unit) \
do { \
    int32 _retval; \
    uint32 _value; \
    if ((_retval = dal_mango_switch_flexTblFmt_get(_unit, (rtk_switch_flexTblFmt_t *)&_value)) != RT_ERR_OK) \
    { \
        RT_ERR(_retval, (MOD_BPE|MOD_DAL), "get table format failed"); \
        return _retval; \
    } \
    if (RTK_SWITCH_FLEX_TBL_FMT_PE_ECID != _value) \
    { \
        _retval = RT_ERR_NOT_ALLOWED; \
        RT_ERR(_retval, (MOD_BPE|MOD_DAL), "table format is not for ECID"); \
        return _retval; \
    } \
} while(0)



/*
 * Function Declaration
 */

/* for ECID-PVID table*/
static int32
_dal_mango_bpe_pvidEntryToHash(uint32 unit, dal_mango_bpe_pvid_entry_t *pPvid_entry, uint32 *pHashVal)
{
    uint32 ecid;
    uint32 nsg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pPvid_entry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pHashVal), RT_ERR_NULL_POINTER);

    ecid = pPvid_entry->ecid;
    nsg = pPvid_entry->nsg;

    /* hash algorith according to spec*/
    *pHashVal = ((nsg >> 2) & 0xff) ^
                (((nsg << 6) & 0xff) | ((ecid >> 16) & 0xff)) ^
                ((ecid >> 8) & 0xff) ^
                (ecid & 0xff);

    return RT_ERR_OK;
}

static int32
_dal_mango_bpe_pvidEntry_compare(dal_mango_bpe_pvid_entry_t *pSrcEntry,
    dal_mango_bpe_pvid_entry_t *pDstEntry)
{
    if ((osal_memcmp(&pSrcEntry->ecid, &pDstEntry->ecid, sizeof(pSrcEntry->ecid))) ||
        (osal_memcmp(&pSrcEntry->nsg, &pDstEntry->nsg, sizeof(pSrcEntry->nsg))))
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

static int32
_dal_mango_bpe_pvidEntry_get(uint32 unit, dal_mango_bpe_pvid_index_t *pIndex,
    dal_mango_bpe_pvid_entry_t *pEntry)
{
    int32               ret = RT_ERR_OK;
    ecid_pvid_entry_t   raw_entry;
    uint32              valid;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    osal_memset(&raw_entry, 0, sizeof(ecid_pvid_entry_t));
    osal_memset(pEntry, 0, sizeof(dal_mango_bpe_pvid_entry_t));

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "type=%u, index=%u",
         pIndex->index_type, pIndex->index);


    /* ECID_PVID table */
    if (pIndex->index_type == BPE_IN_HASH)
    {
        BPE_TABLE_READ_ERR_HDL(unit, MANGO_ECID_PVIDt, ((pIndex->index << 3) | (pIndex->hashdepth)), raw_entry, "", errHandler, ret);
    }
    else
    {
        BPE_TABLE_READ_ERR_HDL(unit, MANGO_ECID_PVID_CAMt, (pIndex->index), raw_entry, "", errHandler, ret);
    }

    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_VALIDtf, valid, raw_entry, "", errHandler, ret);

    /* Check entry valid */
    if (!valid)
    {
        pEntry->is_entry_valid = 0;
        return RT_ERR_OK;
    }
    pEntry->is_entry_valid = 1;

    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_NSGtf, pEntry->nsg, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_ECIDtf, pEntry->ecid, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_VIDtf, pEntry->vid, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_PORT_TYPEtf, pEntry->is_trk, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_PORT_IDtf, pEntry->port_id, raw_entry, "", errHandler, ret);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "get nsg=0x%x, ecid=0x%x, vid=%u",
         pEntry->nsg, pEntry->ecid, pEntry->vid);

    errHandler:
    return ret;
}


static int32
_dal_mango_bpe_pvidEntry_set(uint32 unit, dal_mango_bpe_pvid_index_t *pIndex,
    dal_mango_bpe_pvid_entry_t *pEntry)
{
    int32               ret = RT_ERR_OK;
    ecid_pvid_entry_t   raw_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    osal_memset(&raw_entry, 0, sizeof(ecid_pvid_entry_t));

    /* ECID_PVID table */
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_VALIDtf, pEntry->valid, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_NSGtf, pEntry->nsg, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_ECIDtf, pEntry->ecid, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_VIDtf, pEntry->vid, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_PORT_TYPEtf, pEntry->is_trk, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PVIDt, \
            MANGO_ECID_PVID_PORT_IDtf, pEntry->port_id, raw_entry, "", errHandler, ret);

    if (pIndex->index_type == BPE_IN_HASH)
    {
        BPE_TABLE_WRITE_ERR_HDL(unit, MANGO_ECID_PVIDt, ((pIndex->index << 3) | (pIndex->hashdepth)), raw_entry, "", errHandler, ret);
    }
    else
    {
        BPE_TABLE_WRITE_ERR_HDL(unit, MANGO_ECID_PVID_CAMt, (pIndex->index), raw_entry, "", errHandler, ret);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "set type=%u, index=%u, nsg=0x%x, ecid=0x%x, vid=%u",
         pIndex->index_type, pIndex->index, pEntry->nsg, pEntry->ecid, pEntry->vid);
    errHandler:
    return ret;
}

static int32
_dal_mango_bpe_pvidEntry_getExistOrFree(uint32 unit, dal_mango_bpe_pvid_entry_t *pEntry,
    dal_mango_bpe_getMethod_t get_method, dal_mango_bpe_pvid_index_t *pIndex)
{
    int32           ret;
    uint32          hash_depth;
    uint32          hashVal;
    uint32          found_free = FALSE;
    dal_mango_bpe_pvid_index_t     hash_index;
    dal_mango_bpe_pvid_index_t     cam_index;
    dal_mango_bpe_pvid_entry_t     pvid_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&pvid_entry, 0, sizeof(dal_mango_bpe_pvid_entry_t));

    hash_depth = HAL_BPE_PVID_HASHDEPTH(unit);

    /* calculate hash key */
    if ((ret = _dal_mango_bpe_pvidEntryToHash(unit, pEntry, &hashVal)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "can't caculate hash value");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "get_method=%u, nsg=0x%x, ecid=0x%x, hashVal=%u",
         get_method, pEntry->nsg, pEntry->ecid, hashVal);

    /* found entry in the hash table */
    hash_index.index_type   = BPE_IN_HASH;
    hash_index.index        = hashVal;
    for (hash_index.hashdepth = 0; hash_index.hashdepth < hash_depth; hash_index.hashdepth++)
    {
        if ((ret = _dal_mango_bpe_pvidEntry_get(unit, &hash_index, &pvid_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
            return ret;
        }

        if (pvid_entry.is_entry_valid)
        {
            if (RT_ERR_OK == _dal_mango_bpe_pvidEntry_compare(pEntry, &pvid_entry))
            {
                pEntry->is_entry_exist      = TRUE;
                pvid_entry.is_entry_exist  = TRUE;
                break;
            }
        }
        else
        {
            if (BPE_GET_FREE_ONLY == get_method)
            {
                pIndex->index_type  = BPE_IN_HASH;
                pIndex->index       = hashVal;
                pIndex->hashdepth   = hash_index.hashdepth;
                return RT_ERR_OK;
            }
            else if ((BPE_GET_EXIST_OR_FREE == get_method) && FALSE == found_free)
            {
                found_free = TRUE;
                pIndex->index_type  = BPE_IN_HASH;
                pIndex->index       = hashVal;
                pIndex->hashdepth   = hash_index.hashdepth;
            }
        }
    }

    if (pvid_entry.is_entry_exist)
    {
        if (get_method == BPE_GET_EXIST_ONLY || get_method == BPE_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pIndex, &hash_index, sizeof(dal_mango_bpe_pvid_index_t));
            osal_memcpy(pEntry, &pvid_entry, sizeof(dal_mango_bpe_pvid_entry_t));
            return RT_ERR_OK;
        }
    }

    /* if not in the hash table, found this entry in CAM */
    cam_index.index_type = BPE_IN_CAM;
    for (cam_index.index = 0; cam_index.index < pvidTable_cam_size[unit]; cam_index.index++)
    {
        if ((ret = _dal_mango_bpe_pvidEntry_get(unit, &cam_index, &pvid_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
            return ret;
        }

        if (pvid_entry.is_entry_valid)
        {
            if (RT_ERR_OK == _dal_mango_bpe_pvidEntry_compare(pEntry, &pvid_entry))
            {
                pEntry->is_entry_exist      = TRUE;
                pvid_entry.is_entry_exist  = TRUE;
                break;
            }
        }
        else
        {
            if (BPE_GET_FREE_ONLY == get_method)
            {
                pIndex->index_type  = BPE_IN_CAM;
                pIndex->index       = cam_index.index;
                pIndex->hashdepth   = 0;
                return RT_ERR_OK;
            }
            else if ((BPE_GET_EXIST_OR_FREE == get_method) && FALSE == found_free)
            {
                found_free = TRUE;
                pIndex->index_type  = BPE_IN_CAM;
                pIndex->index       = cam_index.index;
                pIndex->hashdepth   = 0;
            }
        }
    }


    if (pvid_entry.is_entry_exist)
    {
        if (get_method == BPE_GET_EXIST_ONLY || get_method == BPE_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pIndex, &cam_index, sizeof(dal_mango_bpe_pvid_index_t));
            osal_memcpy(pEntry, &pvid_entry, sizeof(dal_mango_bpe_pvid_entry_t));
            return RT_ERR_OK;
        }
    }

    if (get_method == BPE_GET_EXIST_ONLY)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((get_method == BPE_GET_EXIST_OR_FREE) && (TRUE == found_free))
        return RT_ERR_OK;

    return RT_ERR_ENTRY_EXIST;
}


static int32
_dal_mango_bpe_fwdEntryToHash(uint32 unit, dal_mango_bpe_fwd_entry_t *pEntry,
    dal_mango_bpe_fwd_hashIdx_t *pIdx)
{
    int32   ret;
    uint32  base;
    uint32  ext;
    uint32  grp;
    uint32  nsg_reverse;
    uint32  ext_reverse;
    uint32  algo_hash_val[2];
    uint32  blk_algo_type[2];

    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    grp = BPE_ECID_GRP(pEntry->ecid);
    ext = BPE_ECID_EXT(pEntry->ecid);
    base = BPE_ECID_BASE(pEntry->ecid);

    /* Algo 0 */
    algo_hash_val[0] =  (uint32) (((pEntry->nsg) & BITMASK_12B) ^
                            (grp & BITMASK_12B) ^
                            (ext & BITMASK_12B) ^
                            (base & BITMASK_12B));

    /* Algo 1 */
    /* seperate nsg and ext as first part (bit 0-5) and Second part (bit 6-11) */
    /* swap the first and the second part of nsg and ext seperately */
    nsg_reverse = ((pEntry->nsg << 6) | (pEntry->nsg >> 6));
    ext_reverse = ((ext << 6) | (ext >> 6));

    algo_hash_val[1] =  (uint32) ((nsg_reverse & BITMASK_12B) ^
                            (grp & BITMASK_12B) ^
                            (ext_reverse & BITMASK_12B) ^
                            (base & BITMASK_12B));

    /* Get HASH algo */
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_L2_HASH_ALGO_BLK0f, &blk_algo_type[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_L2_HASH_ALGO_BLK1f, &blk_algo_type[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d, algo_hash_val[%u,%u], blk_algo_type[%u,%u]",
        unit,
        algo_hash_val[0],
        algo_hash_val[1],
        blk_algo_type[0],
        blk_algo_type[1]);

    if (blk_algo_type[0] == 0)
        pIdx->blk0_hashIdx = algo_hash_val[0];
    else
        pIdx->blk0_hashIdx = algo_hash_val[1];

    if (blk_algo_type[1] == 0)
        pIdx->blk1_hashIdx = algo_hash_val[0] + ((peFwdTable_size[unit]/2) >> 2);
    else
        pIdx->blk1_hashIdx = algo_hash_val[1] + ((peFwdTable_size[unit]/2) >> 2);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d, block0 hashIdx=%u, block1 hashIdx=%u",
        unit,
        pIdx->blk0_hashIdx,
        pIdx->blk1_hashIdx);

    return RT_ERR_OK;
}


static int32
_dal_mango_bpe_fwdEntry_compare(dal_mango_bpe_fwd_entry_t *pSrcEntry,
    dal_mango_bpe_fwd_entry_t *pDstEntry)
{
    if ((osal_memcmp(&pSrcEntry->ecid, &pDstEntry->ecid, sizeof(pSrcEntry->ecid))) ||
        (osal_memcmp(&pSrcEntry->nsg, &pDstEntry->nsg, sizeof(pSrcEntry->nsg))))
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

static int32
_dal_mango_bpe_fwdEntry_get(uint32 unit, dal_mango_bpe_fwd_index_t index,
    dal_mango_bpe_fwd_entry_t *pEntry)
{
    int32               ret = RT_ERR_OK;
    l2_entry_t          raw_entry;
    uint32              value;

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&raw_entry, 0, sizeof(l2_entry_t));
    osal_memset(pEntry, 0, sizeof(dal_mango_bpe_fwd_entry_t));


    /* ECID_PVID table */
    if (index.index_type == BPE_IN_HASH)
    {
        BPE_TABLE_READ_ERR_HDL(unit, MANGO_PE_FWDt, ((index.index << BPE_FWD_HASH_BITS) | (index.hashdepth)), raw_entry, "", errHandler, ret);
    }
    else
    {
        BPE_TABLE_READ_ERR_HDL(unit, MANGO_PE_FWD_CAMt, (index.index), raw_entry, "", errHandler, ret);
    }

    /* Check entry valid */
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_VALIDtf, value, raw_entry, "", errHandler, ret);
    if (!value)
    {
        pEntry->is_entry_valid = 0;
        return RT_ERR_OK;
    }
    pEntry->is_entry_valid = 1;

    /* Check entry format */
    /*  fmt = 0: L2 */
    /*  fmt = 1: OpenFlow */
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_FMTtf, value, raw_entry, "", errHandler, ret);
    if (value != 0)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "non-l2 entry");
        pEntry->format = L2_TYPE_OPENFLOW;
        return RT_ERR_OK;
    }
    pEntry->format = L2_TYPE_L2;

    /* Check entry type */
    /* entry_type = 0: L2 unicast or multicast */
    /* entry_type = 1: PE forwarding entry */
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_ENTRY_TYPEtf, value, raw_entry, "", errHandler, ret);
    if (value != L2_BPE_PE)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "non-pe-fwd entry");
        pEntry->entry_type = L2_BPE_CB;
        return RT_ERR_OK;
    }
    pEntry->entry_type = L2_BPE_PE;


    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_NSGtf, pEntry->nsg, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_ECIDtf, pEntry->ecid, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_DPMtf, pEntry->portmask, raw_entry, "", errHandler, ret);

    errHandler:
    return ret;
}


static int32
_dal_mango_bpe_fwdEntry_set(uint32 unit, dal_mango_bpe_fwd_index_t index,
    dal_mango_bpe_fwd_entry_t *pEntry)
{
    int32       ret = RT_ERR_OK;
    l2_entry_t  raw_entry;

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    if (pEntry->entry_type != L2_BPE_PE)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "non-pe-fwd entry");
        return RT_ERR_OK;
    }

    osal_memset(&raw_entry, 0, sizeof(l2_entry_t));

    /* ECID_PVID table */
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_VALIDtf, pEntry->valid, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_FMTtf, pEntry->format, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_ENTRY_TYPEtf, pEntry->entry_type, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_NSGtf, pEntry->nsg, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_DPMtf, pEntry->portmask, raw_entry, "", errHandler, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_PE_FWDt, \
            MANGO_PE_FWD_ECIDtf, pEntry->ecid, raw_entry, "", errHandler, ret);

    if (index.index_type == BPE_IN_HASH)
    {
        BPE_TABLE_WRITE_ERR_HDL(unit, MANGO_PE_FWDt, ((index.index << 2) | (index.hashdepth)), raw_entry, "", errHandler, ret);
    }
    else
    {
        BPE_TABLE_WRITE_ERR_HDL(unit, MANGO_PE_FWD_CAMt, (index.index), raw_entry, "", errHandler, ret);
    }

    errHandler:
    return ret;
}

static int32
_dal_mango_bpe_fwdEntry_getExistOrFree(uint32 unit, dal_mango_bpe_fwd_entry_t *pEntry,
    dal_mango_bpe_getMethod_t get_method, dal_mango_bpe_fwd_index_t *pIndex)
{
    int32 ret;
    uint32 hash_depth;
    uint32 value;
    rtk_enable_t camEbl;

    uint32 blk0_entryCnt =0;
    uint32 blk1_entryCnt = 0;
    uint32 blk0_freeDepth = 0xffffffff;
    uint32 blk1_freeDepth = 0xffffffff;
    uint32 cam_freeIndex = 0xffffffff;

    dal_mango_bpe_fwd_hashIdx_t  hashIdx;
    dal_mango_bpe_fwd_entry_t    fwd_entry;
    dal_mango_bpe_fwd_index_t    blk0_index;
    dal_mango_bpe_fwd_index_t    blk1_index;
    dal_mango_bpe_fwd_index_t    cam_index;

    hash_depth = HAL_L2_HASHDEPTH(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,nsg=0x%x,ecid=0x%x,method=%u",
        unit,
        pEntry->nsg,
        pEntry->ecid,
        get_method);

    /* Initialization */
    osal_memset(&cam_index, 0, sizeof(dal_mango_bpe_fwd_index_t));
    osal_memset(&hashIdx, 0, sizeof(dal_mango_bpe_fwd_hashIdx_t));

    /* calculate hash index */
    if ((ret = _dal_mango_bpe_fwdEntryToHash(unit, pEntry, &hashIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }

    blk0_index.index_type = BPE_IN_HASH;
    blk0_index.index = hashIdx.blk0_hashIdx;
    for (blk0_index.hashdepth = 0; blk0_index.hashdepth < hash_depth; blk0_index.hashdepth++)
    {
        if ((ret = _dal_mango_bpe_fwdEntry_get(unit, blk0_index, &fwd_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
            return ret;
        }

        if (fwd_entry.is_entry_valid)
        {
            blk0_entryCnt++;
            if (RT_ERR_OK == _dal_mango_bpe_fwdEntry_compare(pEntry, &fwd_entry))
            {
                pEntry->is_entry_exist = TRUE;
                fwd_entry.is_entry_exist = TRUE;
                break;
            }
        }
        else
        {
            if (blk0_freeDepth == 0xffffffff)
                blk0_freeDepth = blk0_index.hashdepth;
        }
    }

    if (fwd_entry.is_entry_exist)
    {
        if (get_method == BPE_GET_EXIST_ONLY || get_method == BPE_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pIndex, &blk0_index, sizeof(dal_mango_bpe_fwd_index_t));
            osal_memcpy(pEntry, &fwd_entry, sizeof(dal_mango_bpe_fwd_entry_t));
            return RT_ERR_OK;
        }
    }

    blk1_index.index_type = BPE_IN_HASH;
    blk1_index.index = hashIdx.blk1_hashIdx;
    for (blk1_index.hashdepth = 0; blk1_index.hashdepth < hash_depth; blk1_index.hashdepth++)
    {
        if ((ret = _dal_mango_bpe_fwdEntry_get(unit, blk1_index, &fwd_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
            return ret;
        }

        if (fwd_entry.is_entry_valid)
        {
            blk1_entryCnt++;
            if (RT_ERR_OK == _dal_mango_bpe_fwdEntry_compare(pEntry, &fwd_entry))
            {
                pEntry->is_entry_exist = TRUE;
                fwd_entry.is_entry_exist = TRUE;
                break;
            }
        }
        else
        {
            if (blk1_freeDepth == 0xffffffff)
                blk1_freeDepth = blk1_index.hashdepth;
        }
    }

    if (fwd_entry.is_entry_exist)
    {
        if (get_method == BPE_GET_EXIST_ONLY || get_method == BPE_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pIndex, &blk1_index, sizeof(dal_mango_bpe_fwd_index_t));
            osal_memcpy(pEntry, &fwd_entry, sizeof(dal_mango_bpe_fwd_entry_t));
            return RT_ERR_OK;
        }
    }

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value))!=RT_ERR_OK)
        return ret;
    camEbl = value ? ENABLED : DISABLED;

    if (camEbl == ENABLED)
    {
        cam_index.index_type = BPE_IN_CAM;
        for (cam_index.index = 0; cam_index.index < peFwdTable_cam_size[unit]; cam_index.index++)
        {
            if ((ret = _dal_mango_bpe_fwdEntry_get(unit, cam_index, &fwd_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
                return ret;
            }

            if (fwd_entry.is_entry_valid)
            {
                if (RT_ERR_OK == _dal_mango_bpe_fwdEntry_compare(pEntry, &fwd_entry))
                {
                    pEntry->is_entry_exist = TRUE;
                    fwd_entry.is_entry_exist = TRUE;
                    break;
                }
            }
            else
            {
                if (cam_freeIndex == 0xffffffff)
                    cam_freeIndex = cam_index.index;
            }
        }

        if (fwd_entry.is_entry_exist)
        {
            if (get_method == BPE_GET_EXIST_ONLY || get_method == BPE_GET_EXIST_OR_FREE)
            {
                osal_memcpy(pIndex, &cam_index, sizeof(dal_mango_bpe_fwd_index_t));
                osal_memcpy(pEntry, &fwd_entry, sizeof(dal_mango_bpe_fwd_entry_t));
                return RT_ERR_OK;
            }
        }
    }


    if (get_method == BPE_GET_EXIST_ONLY)
        return RT_ERR_ENTRY_NOTFOUND;

    if (get_method == BPE_GET_EXIST_OR_FREE || get_method == BPE_GET_FREE_ONLY)
    {
        if (blk0_freeDepth != 0xffffffff && blk1_freeDepth == 0xffffffff)
        {
            blk0_index.hashdepth = blk0_freeDepth;
            osal_memcpy(pIndex, &blk0_index, sizeof(dal_mango_bpe_fwd_index_t));
            return RT_ERR_OK;
        }
        else if (blk0_freeDepth == 0xffffffff && blk1_freeDepth != 0xffffffff)
        {
            blk1_index.hashdepth = blk1_freeDepth;
            osal_memcpy(pIndex, &blk1_index, sizeof(dal_mango_bpe_fwd_index_t));
            return RT_ERR_OK;
        }
        else if (blk0_freeDepth != 0xffffffff && blk1_freeDepth != 0xffffffff)
        {
            if (blk0_entryCnt <= blk1_entryCnt)
            {
                blk0_index.hashdepth = blk0_freeDepth;
                osal_memcpy(pIndex, &blk0_index, sizeof(dal_mango_bpe_fwd_index_t));
                return RT_ERR_OK;
            }
            else
            {
                blk1_index.hashdepth = blk1_freeDepth;
                osal_memcpy(pIndex, &blk1_index, sizeof(dal_mango_bpe_fwd_index_t));
                return RT_ERR_OK;
            }
        }
        else
        {
            if (camEbl == ENABLED && cam_freeIndex != 0xffffffff)
            {
                cam_index.index = cam_freeIndex;
                osal_memcpy(pIndex, &cam_index, sizeof(dal_mango_bpe_fwd_index_t));
                return RT_ERR_OK;
            }
            else
                return RT_ERR_BPE_NO_EMPTY_ENTRY;
        }
    }

    return RT_ERR_OK;
}



static int32
_dal_mango_bpe_pmskEntry_compare(dal_mango_bpe_ecidPmsk_entry_t *pSrcEntry,
    dal_mango_bpe_ecidPmsk_entry_t *pDstEntry)
{

    if(pSrcEntry->curr_index == BPE_PMSKLIST_INVALID)
    {
        /* compare ecid if source index is not specdified */
        if(osal_memcmp(&pSrcEntry->ecid, &pDstEntry->ecid, sizeof(pSrcEntry->ecid)))
        {
            return RT_ERR_FAILED;
        }
    }
    else if (pSrcEntry->curr_index != pDstEntry->curr_index)
    {
        /* compare index if source index is specdified */
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int32
_dal_mango_bpe_pmskEntry_get(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry)
{
    int32               ret = RT_ERR_OK;
    uint32              val = 0;
    ecid_pmsk_entry_t   raw_entry;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%u, index=%u", unit, index);

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&raw_entry, 0, sizeof(ecid_pmsk_entry_t));

    /* ECID_PMSK_LIST table */
    BPE_TABLE_READ_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, index, raw_entry, "", errHandle, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
            MANGO_ECID_PMSK_LIST_NEXT_IDXtf, pEntry->next_index, raw_entry, "", errHandle, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
            MANGO_ECID_PMSK_LIST_ECIDtf, pEntry->ecid, raw_entry, "", errHandle, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
            MANGO_ECID_PMSK_LIST_PORT_TYPEtf, pEntry->port_type, raw_entry, "", errHandle, ret);
    BPE_TABLE_FIELD_GET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
            MANGO_ECID_PMSK_LIST_PMSK_IDXtf, val, raw_entry, "", errHandle, ret);

    /* For Port ID when PORT_TYPE=0:
     *   bit[11]: IS_TRK
     *   bit[9:4]: PORT ID if IS_TRK=0
     *   bit[10:4]: TRUNK ID if IS_TRK=1
     * For Portmask when PORT_TYPE=1:
     *   bit[11:0]: Portmask Index
     */
    if(pEntry->port_type == BPE_OFFSET_PMSK)
    {
        /* offset and partial portmask */
        pEntry->port_offset.offset = (uint32)((val & 0x700) >> 8);
        pEntry->port_offset.pmsk = (uint32)(val & 0x0FF);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "offset=%u, pmsk=0x%x",
            pEntry->port_offset.offset,
            pEntry->port_offset.pmsk);
    }
    else
    {
        /* portmask index */
        pEntry->portmask.pmsk_index = (uint32)(val & 0xFFF);
    }

    pEntry->curr_index = index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "get entry idx %u with next_idx=%u,ecid=0x%x,port_type=%u,portmask_idx=%u",
        pEntry->curr_index, pEntry->next_index,
        pEntry->ecid, pEntry->port_type, pEntry->portmask.pmsk_index);

    errHandle:
    return ret;
}


static int32
_dal_mango_bpe_pmskEntry_set(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry)
{
    int32               ret = RT_ERR_OK;
    ecid_pmsk_entry_t   raw_entry;
    uint32              val = 0;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%u, ecid_pmsk_idx=%u", unit, index);

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&raw_entry, 0, sizeof(ecid_pmsk_entry_t));

    /* PORT_TYPE = BPE_OFFSET_PMSK:
     *   bit[11]: reserved
     *   bit[10:8]: offset
     *   bit[7:0]: portmask (partial, continuously 8 ports)
     * PORT_TYPE = BPE_PMSK_INDEX:
     *   bit[11:0]: Portmask Index
     */
    if(pEntry->port_type == BPE_OFFSET_PMSK)
    {
        /* offset and partial portmask */
        val = (uint32)((pEntry->port_offset.offset << 8) | (pEntry->port_offset.pmsk & 0x0FF));
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "offset=%u, pmsk=0x%x, val=0x%x",
            pEntry->port_offset.offset,
            pEntry->port_offset.pmsk,
            val);
    }
    else
    {
        val = (uint32)(pEntry->portmask.pmsk_index & 0xFFF);
    }

    /* ECID_PMSK_LIST table */
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
                MANGO_ECID_PMSK_LIST_NEXT_IDXtf, pEntry->next_index, raw_entry, "", errHandle, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
            MANGO_ECID_PMSK_LIST_ECIDtf, pEntry->ecid, raw_entry, "", errHandle, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
            MANGO_ECID_PMSK_LIST_PORT_TYPEtf, pEntry->port_type, raw_entry, "", errHandle, ret);
    BPE_TABLE_FIELD_SET_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, \
            MANGO_ECID_PMSK_LIST_PMSK_IDXtf, val, raw_entry, "", errHandle, ret);

    BPE_TABLE_WRITE_ERR_HDL(unit, MANGO_ECID_PMSK_LISTt, index, raw_entry, "", errHandle, ret);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "set ecid_pmsk_list with cur_idx=%u,next_idx=%u,ecid=0x%x",
        pEntry->curr_index, pEntry->next_index, pEntry->ecid);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "port info with port_type=%u,fwd_pmsk_idx=%u",
        pEntry->port_type, pEntry->portmask.pmsk_index);

    errHandle:
    return ret;
}

static int32
_dal_mango_bpe_pmskList_alloc(uint32 unit, uint32 *pPmskIndex)
{
    int16   free_idx;

    RT_PARAM_CHK(NULL == pPmskIndex, RT_ERR_NULL_POINTER);

    /* 1. found the first/head entry in the free pmsk list */
    /* 2. remove this entry from the free pmsk list */
    while (free_pmsk_list[unit].free_index_head != BPE_PMSKLIST_INVALID)
    {
        free_idx = free_pmsk_list[unit].free_index_head;
        free_pmsk_list[unit].free_index_head = free_pmsk_list[unit].pPmskList_entry[free_idx].next_index;
        free_pmsk_list[unit].pPmskList_entry[free_idx].next_index = BPE_PMSKLIST_INVALID;

        *pPmskIndex = free_idx;
        free_pmsk_list[unit].free_entry_count--;
        return RT_ERR_OK;
    }

    return RT_ERR_TBL_FULL;
}



static int32
_dal_mango_bpe_pmsklist_free(uint32 unit, uint32 pmskIndex)
{
    RT_PARAM_CHK(pmskIndex >= ecidPmskTable_size[unit], RT_ERR_ENTRY_INDEX);

    /* 1. initialize this entry */
    /* 2. add this entry back to the head of free pmsk list */
    osal_memset(&free_pmsk_list[unit].pPmskList_entry[pmskIndex], 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    free_pmsk_list[unit].pPmskList_entry[pmskIndex].next_index  = free_pmsk_list[unit].free_index_head;
    free_pmsk_list[unit].free_index_head = pmskIndex;
    free_pmsk_list[unit].free_entry_count++;

    return RT_ERR_OK;
}

int32
_dal_mango_bpe_pmskEntry_offset_cal(rtk_portmask_t  portmask, uint32 *offset, uint32 *pmsk)
{
    if ((offset == NULL) || (pmsk == NULL))
    {
        return RT_ERR_NULL_POINTER;
    }

    return RT_ERR_OK;
}

/* Module Name : BPE */

/* Function Name:
 *      dal_mango_bpeMapper_init
 * Description:
 *      Hook BPE module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook vlan module before calling any BPE APIs.
 */
int32
dal_mango_bpeMapper_init(dal_mapper_t *pMapper)
{
    pMapper->bpe_init = dal_mango_bpe_init;
    pMapper->bpe_portFwdMode_get = dal_mango_bpe_portFwdMode_get;
    pMapper->bpe_portFwdMode_set = dal_mango_bpe_portFwdMode_set;
    pMapper->bpe_portEcidNameSpaceGroupId_get = dal_mango_bpe_portEcidNameSpaceGroupId_get;
    pMapper->bpe_portEcidNameSpaceGroupId_set = dal_mango_bpe_portEcidNameSpaceGroupId_set;
    pMapper->bpe_portPcid_get = dal_mango_bpe_portPcid_get;
    pMapper->bpe_portPcid_set = dal_mango_bpe_portPcid_set;
    pMapper->bpe_portPcidAct_get = dal_mango_bpe_portPcidAct_get;
    pMapper->bpe_portPcidAct_set = dal_mango_bpe_portPcidAct_set;
    pMapper->bpe_portEgrTagSts_get = dal_mango_bpe_portEgrTagSts_get;
    pMapper->bpe_portEgrTagSts_set = dal_mango_bpe_portEgrTagSts_set;
    pMapper->bpe_portEgrVlanTagSts_get = dal_mango_bpe_portEgrVlanTagSts_get;
    pMapper->bpe_portEgrVlanTagSts_set = dal_mango_bpe_portEgrVlanTagSts_set;
    pMapper->bpe_pvidEntry_add = dal_mango_bpe_pvidEntry_add;
    pMapper->bpe_pvidEntry_del = dal_mango_bpe_pvidEntry_del;
    pMapper->bpe_pvidEntry_get = dal_mango_bpe_pvidEntry_get;
    pMapper->bpe_pvidEntryNextValid_get = dal_mango_bpe_pvidEntryNextValid_get;
    pMapper->bpe_fwdEntry_add = dal_mango_bpe_fwdEntry_add;
    pMapper->bpe_fwdEntry_del = dal_mango_bpe_fwdEntry_del;
    pMapper->bpe_fwdEntry_get = dal_mango_bpe_fwdEntry_get;
    pMapper->bpe_fwdEntryNextValid_get = dal_mango_bpe_fwdEntryNextValid_get;
    pMapper->bpe_globalCtrl_get = dal_mango_bpe_globalCtrl_get;
    pMapper->bpe_globalCtrl_set = dal_mango_bpe_globalCtrl_set;
    pMapper->bpe_portCtrl_get = dal_mango_bpe_portCtrl_get;
    pMapper->bpe_portCtrl_set = dal_mango_bpe_portCtrl_set;
    pMapper->bpe_priRemarking_get = dal_mango_bpe_priRemarking_get;
    pMapper->bpe_priRemarking_set = dal_mango_bpe_priRemarking_set;


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_bpe_init
 * Description:
 *      Initialize BPE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize BPE module before calling any BPE APIs.
 */
int32
dal_mango_bpe_init(uint32 unit)
{
    int32       ret = RT_ERR_OK;
    uint32      index;

    RT_LOG(LOG_DEBUG, (MOD_BPE|MOD_DAL), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(bpe_init[unit]);
    bpe_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    bpe_sem[unit] = osal_sem_mutex_create();
    if (0 == bpe_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_BPE|MOD_DAL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    ecidPsmk_sem[unit] = osal_sem_mutex_create();
    if (0 == ecidPsmk_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_BPE|MOD_DAL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* get table size*/
    if ((ret = table_size_get(unit, MANGO_ECID_PVIDt, &pvidTable_size[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }
    if ((ret = table_size_get(unit, MANGO_ECID_PVID_CAMt, &pvidTable_cam_size[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }
    if ((ret = table_size_get(unit, MANGO_ECID_PMSK_LISTt, &ecidPmskTable_size[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }

    if ((ret = table_size_get(unit, MANGO_PE_FWDt, &peFwdTable_size[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }
    if ((ret = table_size_get(unit, MANGO_PE_FWD_CAMt, &peFwdTable_cam_size[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }


    /* allocate memory for free index */
    free_pmsk_list[unit].pPmskList_entry = (dal_mango_bpe_ecidPmsk_entry_t *)osal_alloc(ecidPmskTable_size[unit] * sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    if (!free_pmsk_list[unit].pPmskList_entry)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "memory allocate failed");
        return ret;
    }
    osal_memset(free_pmsk_list[unit].pPmskList_entry, 0, (ecidPmskTable_size[unit] * sizeof(dal_mango_bpe_ecidPmsk_entry_t)));

    free_pmsk_list[unit].free_index_head = 1;   /* index 0 is reserved */
    free_pmsk_list[unit].free_entry_count = ecidPmskTable_size[unit] - 1;

    /* create free link-list for all entry, from 0 ~ max index - 2 */
    for (index = 0; index < (ecidPmskTable_size[unit] - 1); index++)
    {
        free_pmsk_list[unit].pPmskList_entry[index].next_index = index + 1;
    }
    free_pmsk_list[unit].pPmskList_entry[ecidPmskTable_size[unit] - 1].next_index = BPE_PMSKLIST_INVALID;

    /* set init flag to complete init */
    bpe_init[unit] = INIT_COMPLETED;

    return ret;
}

/* Function Name:
 *      dal_mango_bpe_portFwdMode_get
 * Description:
 *      Get the forwarding mode of a specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to the forwarding mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portFwdMode_get(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t *pMode)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_bpe_fwdMode_t type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_LU_KEY_SELf, val, "", errExit, ret);
    BPE_VALUE_TO_TYPE(_typeFwdMode, type, val, "", errExit, ret);
    *pMode = type;

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portFwdMode_get */

/* Function Name:
 *      dal_mango_bpe_portFwdMode_set
 * Description:
 *      Set the forwarding mode of a specified port
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - forwarding mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portFwdMode_set(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t mode)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,mode=%d", unit, port, mode);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((mode >= RTK_BPE_FWDMODE_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_TYPE_TO_VALUE(_typeFwdMode, val, mode, "", errExit, ret);
    BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit,  \
        MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_LU_KEY_SELf, val, "", errExit, ret);
    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portFwdMode_set */

/* Function Name:
 *      dal_mango_bpe_portEcidNameSpaceGroupId_get
 * Description:
 *      Get the E-CID naming space group ID of a specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGroupId - pointer to the group id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portEcidNameSpaceGroupId_get(uint32 unit, rtk_port_t port, uint32 *pGroupId)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pGroupId), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_ECID_NSGf, val, "", errExit, ret);
    *pGroupId = val;

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portEcidNameSpaceGroupId_get */

/* Function Name:
 *      dal_mango_bpe_portEcidNameSpaceGroupId_set
 * Description:
 *      Set the E-CID naming space group ID of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      groupId - group id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portEcidNameSpaceGroupId_set(uint32 unit, rtk_port_t port, uint32 groupId)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,groupId=%d", unit, port, groupId);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((groupId > BPE_ECID_NSG_MAX_NUM), RT_ERR_OUT_OF_RANGE);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
        MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_ECID_NSGf, groupId, "", errExit, ret);

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portEcidNameSpaceGroupId_set */

/* Function Name:
 *      dal_mango_bpe_portPcid_get
 * Description:
 *      Get the PCID of a specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      type  - ecid configutation
 * Output:
 *      pPcid - pointer to PCID (Port's E-CID)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portPcid_get(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t *pPcid)
{
    int32   ret = RT_ERR_OK;
    uint32  base = 0;
    uint32  ext = 0;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,type=%d", unit, port, type);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPcid), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);

    if(type & RTK_BPE_PCFG_EXT_ONLY)
    {
        /* extension bits*/
        BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
            MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
            MANGO_PCID_EXTf, ext, "", errExit, ret);
    }

    if(type & RTK_BPE_PCFG_BASE_ONLY)
    {
        /* base bits*/
        BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
            MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
            MANGO_PCID_BASEf, base, "", errExit, ret);
    }

    /* E-CID is combined extension (bit 19-12) and base (bit 11-0) */
    val = BPE_ECID_PCID(ext, base);
    *pPcid = val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "pcid=0x%x(type=%u,base=0x%x,extension=0x%x)",
        type, *pPcid, base, ext);

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portPcid_get */

/* Function Name:
 *      dal_mango_bpe_portPcid_set
 * Description:
 *      Set the PCID of a specified port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - ecid configutation
 *      pcid - PCID (Port's E-CID)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portPcid_set(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t pcid)
{
    int32   ret = RT_ERR_OK;
    uint32  base = 0;
    uint32  ext = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,type=%d,pcid=%d", unit, port, type, pcid);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);
    base = BPE_ECID_BASE(pcid);
    ext = BPE_ECID_EXT(pcid);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((base > BPE_ECID_BASE_MAX_NUM), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((ext > BPE_ECID_EXT_MAX_NUM), RT_ERR_OUT_OF_RANGE);

    /* function body */
    BPE_SEM_LOCK(unit);

    if(type & RTK_BPE_PCFG_EXT_ONLY)
    {
        /* extension bits */
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "set extension 0x%x", ext);
        BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
            MANGO_PCID_EXTf, ext, "", errExit, ret);
        BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
            MANGO_PCID_EXTf, ext, "", errExit, ret);
    }

    if(type & RTK_BPE_PCFG_BASE_ONLY)
    {
        /* base bits */
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "set base 0x%x", base);
        BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
            MANGO_PCID_BASEf, base, "", errExit, ret);
        BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
            MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
            MANGO_PCID_BASEf, base, "", errExit, ret);
    }

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portPcid_set */

/* Function Name:
 *      dal_mango_bpe_portPcidAct_get
 * Description:
 *      Get the action of PCID packet of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portPcidAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_action_t type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_ECID_EQ_PCID_ACTf, val, "", errExit, ret);
    BPE_VALUE_TO_TYPE(_typeEcidEqPcidAct, type, val, "", errExit, ret);
    *pAction = type;

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portPcidAct_get */

/* Function Name:
 *      dal_mango_bpe_portPcidAct_set
 * Description:
 *      Set the action of PCID packet of a specified port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portPcidAct_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,action=%d", unit, port, action);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* XXX add range check for action if necessary */

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_TYPE_TO_VALUE(_typeEcidEqPcidAct, val, action, "", errExit, ret);
    BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit,  \
        MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_ECID_EQ_PCID_ACTf, val, "", errExit, ret);

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portPcidAct_set */

/* Function Name:
 *      dal_mango_bpe_portEgrTagSts_get
 * Description:
 *      Get the egress PE-tag status configuration of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus - pointer to the tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_bpe_tagSts_t *pStatus)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_bpe_tagSts_t type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_TAG_STS_MODEf, val, "", errExit, ret);
    BPE_VALUE_TO_TYPE(_typeEgrTagSts, type, val, "", errExit, ret);
    *pStatus = type;

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portEgrTagSts_get */

/* Function Name:
 *      dal_mango_bpe_portEgrTagSts_set
 * Description:
 *      Set the egress PE-tag status configuration of a specified port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      status - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portEgrTagSts_set(uint32 unit, rtk_port_t port,
    rtk_bpe_tagSts_t status)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,status=%d", unit, port, status);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((status >= RTK_BPE_TAGSTS_END), RT_ERR_INPUT);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_TYPE_TO_VALUE(_typeEgrTagSts, val, status, "", errExit, ret);
    BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit,  \
        MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_TAG_STS_MODEf, val, "", errExit, ret);

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portEgrTagSts_set */

/* Function Name:
 *      dal_mango_bpe_portEgrVlanTagSts_get
 * Description:
 *      Get the egress VLAN tag status configuration of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pVlan_status - pointer to the tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portEgrVlanTagSts_get(uint32 unit, rtk_port_t port,
    rtk_bpe_vlanTagSts_t *pVlan_status)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    rtk_bpe_vlanTagSts_t type;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pVlan_status), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
        MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_VLAN_TAG_STS_MODEf, val, "", errExit, ret);
    BPE_VALUE_TO_TYPE(_typeEgrVlanTagSts, type, val, "", errExit, ret);
    *pVlan_status = type;

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;

}   /* end of dal_mango_bpe_portEgrVlanTagSts_get */

/* Function Name:
 *      dal_mango_bpe_portEgrVlanTagSts_set
 * Description:
 *      Set the egress VLAN tag status configuration of a specified port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      vlan_status - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
int32
dal_mango_bpe_portEgrVlanTagSts_set(uint32 unit, rtk_port_t port,
    rtk_bpe_vlanTagSts_t vlan_status)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,vlan_status=%d", unit, port, vlan_status);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((vlan_status >= RTK_BPE_VTAGSTS_END), RT_ERR_INPUT);

    /* function body */
    BPE_SEM_LOCK(unit);
    BPE_TYPE_TO_VALUE(_typeEgrVlanTagSts, val, vlan_status, "", errExit, ret);
    BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit,  \
        MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
        MANGO_VLAN_TAG_STS_MODEf, val, "", errExit, ret);

    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portEgrVlanTagSts_set */


/* Function Name:
 *      dal_mango_bpe_pvidEntry_add
 * Description:
 *      Add an extended port's PVID entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the PVID entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_bpe_pvidEntry_add(uint32 unit, rtk_bpe_pvidEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    dal_mango_bpe_pvid_entry_t  pvid_entry;
    dal_mango_bpe_pvid_index_t  pvid_index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    BPE_TABLE_FMT_CHECK(unit);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    /* XXX range check for each field of pEntry */

    osal_memset(&pvid_entry, 0, sizeof(dal_mango_bpe_pvid_entry_t));
    osal_memset(&pvid_index, 0, sizeof(dal_mango_bpe_pvid_index_t));
    osal_memcpy(&pvid_entry.nsg, &(pEntry->nsg_id), sizeof(pvid_entry.nsg));
    osal_memcpy(&pvid_entry.ecid, &(pEntry->ecid), sizeof(pvid_entry.ecid));

    /* function body */
    BPE_SEM_LOCK(unit);

    ret = _dal_mango_bpe_pvidEntry_getExistOrFree(unit, &pvid_entry, BPE_GET_EXIST_OR_FREE, &pvid_index);
    if (RT_ERR_OK != ret)
    {
        BPE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }

    pvid_entry.valid = TRUE;
    osal_memcpy(&pvid_entry.vid, &pEntry->pvid, sizeof(pvid_entry.vid));
    if (pEntry->flags & RTK_BPE_PVID_FLAG_TRUNK_PORT)
    {
        pvid_entry.is_trk   = 1;
        pvid_entry.port_id  = pEntry->trunk_id;
    }
    else
    {
        pvid_entry.is_trk   = 0;
        pvid_entry.port_id  = pEntry->port_id;
    }

    ret = _dal_mango_bpe_pvidEntry_set(unit, &pvid_index, &pvid_entry);
    if (pvid_index.index_type == BPE_IN_HASH)
        pEntry->entry_idx = (pvid_index.index << BPE_PVID_HASH_BITS) | pvid_index.hashdepth;
    else
        pEntry->entry_idx = pvidTable_size[unit] + pvid_index.index;

    BPE_SEM_UNLOCK(unit);

    return ret;
}/* end of dal_mango_bpe_pvidEntry_add */


/* Function Name:
 *      dal_mango_bpe_pvidEntry_del
 * Description:
 *      Delete an extended port's PVID entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the PVID entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_bpe_pvidEntry_del(uint32 unit, rtk_bpe_pvidEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    dal_mango_bpe_pvid_entry_t     pvid_entry;
    dal_mango_bpe_pvid_index_t     pvid_index;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    BPE_TABLE_FMT_CHECK(unit);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&pvid_entry, 0, sizeof(dal_mango_bpe_pvid_entry_t));
    osal_memset(&pvid_index, 0, sizeof(dal_mango_bpe_pvid_index_t));
    osal_memcpy(&pvid_entry.nsg, &(pEntry->nsg_id), sizeof(pvid_entry.nsg));
    osal_memcpy(&pvid_entry.ecid, &(pEntry->ecid), sizeof(pvid_entry.ecid));

    /* function body */
    BPE_SEM_LOCK(unit);

    if ((ret = _dal_mango_bpe_pvidEntry_getExistOrFree(unit, &pvid_entry, BPE_GET_EXIST_ONLY,
        &pvid_index)) != RT_ERR_OK)
    {
        BPE_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "ret=%d", ret);
        return ret;
    }

    osal_memset(&pvid_entry, 0, sizeof(dal_mango_bpe_pvid_entry_t));
    ret = _dal_mango_bpe_pvidEntry_set(unit, &pvid_index, &pvid_entry);

    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_pvidEntry_del */


/* Function Name:
 *      dal_mango_bpe_pvidEntry_get
 * Description:
 *      Get an extended port's PVID entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the PVID entry
 * Output:
 *      pEntry - pointer to the returned PVID entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_bpe_pvidEntry_get(uint32 unit, rtk_bpe_pvidEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    dal_mango_bpe_pvid_entry_t pvid_entry;
    dal_mango_bpe_pvid_index_t pvid_index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    BPE_TABLE_FMT_CHECK(unit);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "input pEntry: nsg=0x%x,ecid=0x%x,vid=%u,idx=%u",
        pEntry->nsg_id, pEntry->ecid, pEntry->pvid, pEntry->entry_idx);

    osal_memset(&pvid_entry, 0, sizeof(dal_mango_bpe_pvid_entry_t));
    osal_memset(&pvid_index, 0, sizeof(dal_mango_bpe_pvid_index_t));
    osal_memcpy(&pvid_entry.nsg, &(pEntry->nsg_id), sizeof(pvid_entry.nsg));
    osal_memcpy(&pvid_entry.ecid, &(pEntry->ecid), sizeof(pvid_entry.ecid));

    /* function body */
    BPE_SEM_LOCK(unit);

    if ((ret = _dal_mango_bpe_pvidEntry_getExistOrFree(unit, &pvid_entry, BPE_GET_EXIST_ONLY,
        &pvid_index)) != RT_ERR_OK)
    {
        /* Return Fail if not found */
        BPE_SEM_UNLOCK(unit);
        return ret;
    }

    BPE_SEM_UNLOCK(unit);

    /* fill content */
    pEntry->pvid = pvid_entry.vid;
    if (pvid_entry.is_trk)
    {
        pEntry->flags |= RTK_BPE_PVID_FLAG_TRUNK_PORT;
        osal_memcpy(&pEntry->trunk_id, &(pvid_entry.port_id), sizeof(pEntry->trunk_id));
    }
    else
    {
        osal_memcpy(&pEntry->port_id, &(pvid_entry.port_id), sizeof(pEntry->port_id));
    }


    if (pvid_index.index_type == BPE_IN_HASH)
        pEntry->entry_idx = (pvid_index.index << BPE_PVID_HASH_BITS) | pvid_index.hashdepth;
    else
        pEntry->entry_idx = pvidTable_size[unit] + pvid_index.index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "ouput pEntry: nsg=0x%x,ecid=0x%x,vid=%u,idx=%u",
        pEntry->nsg_id, pEntry->ecid, pEntry->pvid, pEntry->entry_idx);

    return RT_ERR_OK;
}   /* end of dal_mango_bpe_pvidEntry_get */

/* Function Name:
 *      dal_mango_bpe_pvidEntryNextValid_get
 * Description:
 *      Get a next valid PVID entry from PVID table.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of PVID table to get next
 * Output:
 *      pScan_idx - returned found scan index (-1 means not found)
 *      pEntry    - pointer to PVID entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOT_EXIST  - entry is not existed
 * Note:
 *      None
 */
int32
dal_mango_bpe_pvidEntryNextValid_get(uint32 unit, int32 *pScan_idx, rtk_bpe_pvidEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  index;
    uint32  is_found = 0;
    dal_mango_bpe_pvid_entry_t     pvid_entry;
    dal_mango_bpe_pvid_index_t     pvid_index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    BPE_TABLE_FMT_CHECK(unit);
    RT_PARAM_CHK((NULL == pScan_idx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&pvid_entry, 0, sizeof(dal_mango_bpe_pvid_entry_t));

    if(*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 1;

    if (*pScan_idx >= (pvidTable_size[unit] + pvidTable_cam_size[unit]))
    {
        return RT_ERR_INPUT;
    }

    /* function body */
    BPE_SEM_LOCK(unit);

    if (*pScan_idx < pvidTable_size[unit])
    {
        pvid_index.index_type = BPE_IN_HASH;
        for (index = *pScan_idx; index < pvidTable_size[unit]; index++)
        {
            pvid_index.index       = index >> BPE_PVID_HASH_BITS;
            pvid_index.hashdepth   = index & 0x7;

            if ((ret = _dal_mango_bpe_pvidEntry_get(unit, &pvid_index, &pvid_entry)) != RT_ERR_OK)
            {
                BPE_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "ret=%d", ret);
                return ret;
            }

            if (pvid_entry.is_entry_valid == 1)
            {
                is_found = 1;
                break;
            }
        }
    }

    if (is_found == 0)
    {
        pvid_index.index_type = BPE_IN_CAM;
        for (index = (*pScan_idx < pvidTable_size[unit]) ? 0 : (*pScan_idx - pvidTable_size[unit]);
            index < pvidTable_cam_size[unit]; index++)
        {
            pvid_index.index = index;
            if ((ret = _dal_mango_bpe_pvidEntry_get(unit, &pvid_index, &pvid_entry)) != RT_ERR_OK)
            {
                BPE_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "ret=%d", ret);
                return ret;
            }

            if (pvid_entry.is_entry_valid == 1)
            {
                is_found = 1;
                break;
            }
        }
    }

    BPE_SEM_UNLOCK(unit);

    if (is_found == 0)
        return RT_ERR_ENTRY_NOTFOUND;

    /* fill content */
    osal_memcpy(&pEntry->nsg_id, &(pvid_entry.nsg), sizeof(pEntry->nsg_id));
    osal_memcpy(&pEntry->ecid, &(pvid_entry.ecid), sizeof(pEntry->ecid));
    osal_memcpy(&pEntry->pvid, &(pvid_entry.vid), sizeof(pEntry->pvid));
    if (pvid_entry.is_trk)
    {
        pEntry->flags |= RTK_BPE_PVID_FLAG_TRUNK_PORT;
        osal_memcpy(&pEntry->trunk_id, &(pvid_entry.port_id), sizeof(pEntry->trunk_id));
    }
    else
    {
        osal_memcpy(&pEntry->port_id, &(pvid_entry.port_id), sizeof(pEntry->port_id));
    }

    if (pvid_index.index_type == BPE_IN_HASH)
        pEntry->entry_idx = (pvid_index.index << BPE_PVID_HASH_BITS) | pvid_index.hashdepth;
    else
        pEntry->entry_idx = pvidTable_size[unit] + pvid_index.index;

    *pScan_idx = pEntry->entry_idx;

    return ret;
}   /* end of dal_mango_bpe_pvidEntryNextValid_get */

/* Function Name:
 *      dal_mango_bpe_fwdEntry_add
 * Description:
 *      Add an E-CID forwarding entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the E-CID forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_bpe_fwdEntry_add(uint32 unit, rtk_bpe_fwdEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    dal_mango_bpe_fwd_entry_t  fwd_entry;
    dal_mango_bpe_fwd_index_t  fwd_index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pEntry->portmask), RT_ERR_PORT_MASK);
    /* XXX range check for each field of pEntry */

    osal_memset(&fwd_entry, 0, sizeof(dal_mango_bpe_fwd_entry_t));
    osal_memset(&fwd_index, 0, sizeof(dal_mango_bpe_fwd_index_t));
    fwd_entry.nsg = pEntry->nsg_id;
    osal_memcpy(&fwd_entry.ecid, &pEntry->ecid, sizeof(fwd_entry.ecid));
    osal_memcpy(&fwd_entry.portmask, &pEntry->portmask, sizeof(fwd_entry.portmask));

    /* function body */
    PE_FWD_SEM_LOCK(unit);

    ret = _dal_mango_bpe_fwdEntry_getExistOrFree(unit, &fwd_entry, BPE_GET_EXIST_OR_FREE, &fwd_index);
    if (RT_ERR_OK != ret)
    {
        PE_FWD_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "");
        return ret;
    }

    /* assign other fields */
    fwd_entry.valid = TRUE;
    fwd_entry.format = L2_TYPE_L2;
    fwd_entry.entry_type = BPE_FWD_TYPE_PE;
    osal_memcpy(&fwd_entry.portmask, &pEntry->portmask, sizeof(fwd_entry.portmask));

    ret = _dal_mango_bpe_fwdEntry_set(unit, fwd_index, &fwd_entry);
    if (fwd_index.index_type == BPE_IN_HASH)
        pEntry->entry_idx = (fwd_index.index << BPE_FWD_HASH_BITS) | fwd_index.hashdepth;
    else
        pEntry->entry_idx = peFwdTable_size[unit] + fwd_index.index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "idx=%d,nsg=0x%x,ecid=0x%x,portmask0=0x%x,portmask1=0x%x",
        pEntry->entry_idx,
        pEntry->nsg_id,
        pEntry->ecid,
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 0),
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 1));

    PE_FWD_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_fwdEntry_add */


/* Function Name:
 *      dal_mango_bpe_fwdEntry_del
 * Description:
 *      Delete an E-CID forwarding entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the E-CID forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_bpe_fwdEntry_del(uint32 unit, rtk_bpe_fwdEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    dal_mango_bpe_fwd_entry_t     fwd_entry;
    dal_mango_bpe_fwd_index_t     fwd_index;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    /* XXX range check for each field of pEntry */

    osal_memset(&fwd_entry, 0, sizeof(dal_mango_bpe_fwd_entry_t));
    osal_memset(&fwd_index, 0, sizeof(dal_mango_bpe_fwd_index_t));
    fwd_entry.nsg = pEntry->nsg_id;
    osal_memcpy(&fwd_entry.ecid, &(pEntry->ecid), sizeof(fwd_entry.ecid));

    /* function body */
    PE_FWD_SEM_LOCK(unit);
    if ((ret = _dal_mango_bpe_fwdEntry_getExistOrFree(unit, &fwd_entry, BPE_GET_EXIST_ONLY,
        &fwd_index)) != RT_ERR_OK)
    {
        PE_FWD_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "ret=%d", ret);
        return ret;
    }

    osal_memset(&fwd_entry, 0, sizeof(dal_mango_bpe_pvid_entry_t));
    fwd_entry.entry_type = BPE_FWD_TYPE_PE;
    ret = _dal_mango_bpe_fwdEntry_set(unit, fwd_index, &fwd_entry);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "idx=%d", pEntry->entry_idx);

    PE_FWD_SEM_UNLOCK(unit);
    return ret;
}   /* end of dal_mango_bpe_fwdEntry_del */


/* Function Name:
 *      dal_mango_bpe_fwdEntry_get
 * Description:
 *      Get an E-CID forwarding entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the E-CID forwarding entry
 * Output:
 *      pEntry - pointer to the returned E-CID forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_bpe_fwdEntry_get(uint32 unit, rtk_bpe_fwdEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    dal_mango_bpe_fwd_entry_t fwd_entry;
    dal_mango_bpe_fwd_index_t fwd_index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    /* XXX range check for each field of pEntry */

    osal_memset(&fwd_entry, 0, sizeof(dal_mango_bpe_fwd_entry_t));
    osal_memset(&fwd_index, 0, sizeof(dal_mango_bpe_fwd_index_t));
    fwd_entry.nsg = pEntry->nsg_id;
    osal_memcpy(&fwd_entry.ecid, &(pEntry->ecid), sizeof(fwd_entry.ecid));

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "nsg=0x%x,ecid=0x%x", fwd_entry.nsg, fwd_entry.ecid);

    /* function body */
    PE_FWD_SEM_LOCK(unit);
    if ((ret = _dal_mango_bpe_fwdEntry_getExistOrFree(unit, &fwd_entry, BPE_GET_EXIST_ONLY,
        &fwd_index)) != RT_ERR_OK)
    {
        /* Return Fail if not found */
        PE_FWD_SEM_UNLOCK(unit);
        return ret;
    }

    PE_FWD_SEM_UNLOCK(unit);

    /* fill content */
    osal_memcpy(&(pEntry->portmask), &(fwd_entry.portmask), sizeof(pEntry->portmask));

    if (fwd_index.index_type == BPE_IN_HASH)
        pEntry->entry_idx = (fwd_index.index << BPE_FWD_HASH_BITS) | fwd_index.hashdepth;
    else
        pEntry->entry_idx = peFwdTable_size[unit] + fwd_index.index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "idx=%d,nsg=0x%x,ecid=0x%x,portmask0=0x%x,portmask1=0x%x",
        pEntry->entry_idx,
        pEntry->nsg_id,
        pEntry->ecid,
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 0),
        RTK_PORTMASK_WORD_GET(pEntry->portmask, 1));

    return ret;
}   /* end of dal_mango_bpe_fwdEntry_get */

/* Function Name:
 *      dal_mango_bpe_fwdEntryNextValid_get
 * Description:
 *      Get a next valid E-CID forwarding entry on the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of E-CID forwarding table to get next.
 * Output:
 *      pScan_idx - returned found scan index (-1 means not found)
 *      pEntry    - pointer to E-CID forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOT_EXIST  - entry is not existed
 * Note:
 *      None
 */
int32
dal_mango_bpe_fwdEntryNextValid_get(uint32 unit, int32 *pScan_idx,
    rtk_bpe_fwdEntry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    rtk_enable_t camEbl;
    uint32 index;
    uint32 value;
    uint32 is_found = 0;
    dal_mango_bpe_fwd_entry_t     fwd_entry;
    dal_mango_bpe_fwd_index_t     fwd_index;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pScan_idx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    osal_memset(&fwd_entry, 0, sizeof(dal_mango_bpe_fwd_entry_t));
    osal_memset(&fwd_index, 0, sizeof(dal_mango_bpe_fwd_index_t));

    /* function body */
    PE_FWD_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value)) != RT_ERR_OK)
    {
        PE_FWD_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "ret=%d", ret);
        return ret;
    }
    camEbl = value ? ENABLED : DISABLED;

    if(*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 1;

    if ((*pScan_idx >= (peFwdTable_size[unit] + peFwdTable_cam_size[unit])) ||
        ((*pScan_idx >= peFwdTable_size[unit]) && (camEbl == DISABLED)))
    {
        ret = RT_ERR_INPUT;
        PE_FWD_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "ret=%d", ret);

        return ret;
    }

    if (*pScan_idx < peFwdTable_size[unit])
    {
        fwd_index.index_type = BPE_IN_HASH;
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "start search in hash table %d", *pScan_idx);

        for (index = *pScan_idx; index < peFwdTable_size[unit]; index++)
        {
            fwd_index.index       = index >> BPE_FWD_HASH_BITS;
            fwd_index.hashdepth   = index & 0x3;
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "%d", index);

            if ((ret = _dal_mango_bpe_fwdEntry_get(unit, fwd_index, &fwd_entry)) != RT_ERR_OK)
            {
                PE_FWD_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_BPE), "ret=%d", ret);
                return ret;
            }

            if ((fwd_entry.is_entry_valid == 1) && (fwd_entry.entry_type == L2_BPE_PE))
            {
                is_found = 1;
                break;
            }
        }
    }

    if (camEbl == ENABLED && is_found == 0)
    {
        fwd_index.index_type = BPE_IN_CAM;
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "start search in CAM table %d", *pScan_idx);

        for (index = (*pScan_idx < peFwdTable_size[unit]) ? 0 : (*pScan_idx - peFwdTable_size[unit]);
            index < peFwdTable_cam_size[unit]; index++)
        {
            fwd_index.index = index;

            if ((ret = _dal_mango_bpe_fwdEntry_get(unit, fwd_index, &fwd_entry)) != RT_ERR_OK)
            {
                PE_FWD_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_BPE), "ret=%d", ret);
                return ret;
            }

            if ((fwd_entry.is_entry_valid == 1) && (fwd_entry.entry_type == L2_BPE_PE))
            {
                is_found = 1;
                break;
            }
        }
    }

    PE_FWD_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "end search");

    if (is_found == 0)
        return RT_ERR_ENTRY_NOTFOUND;

    /* fill content */
    osal_memcpy(&pEntry->nsg_id, &(fwd_entry.nsg), sizeof(pEntry->nsg_id));
    osal_memcpy(&pEntry->ecid, &(fwd_entry.ecid), sizeof(pEntry->ecid));
    osal_memcpy(&pEntry->portmask, &(fwd_entry.portmask), sizeof(pEntry->portmask));

    if (fwd_index.index_type == BPE_IN_HASH)
        pEntry->entry_idx = (fwd_index.index << BPE_FWD_HASH_BITS) | fwd_index.hashdepth;
    else
        pEntry->entry_idx = peFwdTable_size[unit] + fwd_index.index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "current valid idx=%u, cam enable=%u", *pScan_idx, camEbl);

    *pScan_idx = (int32)pEntry->entry_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "next valid idx=%u", *pScan_idx);

    return ret;
}   /* end of dal_mango_bpe_fwdEntryNextValid_get */


/* Function Name:
 *      dal_mango_bpe_globalCtrl_get
 * Description:
 *      Get the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argument
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_mango_bpe_globalCtrl_get(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;
    uint32  action;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_BPE_GCT_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);
    switch (type)
    {
        case RTK_BPE_GCT_ETAG_CPU_KEEP:
            BPE_REG_FIELD_READ_ERR_HDL(unit, MANGO_PE_ETAG_CTRLr, MANGO_CPU_KEEPf,\
                                       val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            break;

        case RTK_BPE_GCT_ETAG_TPID:
            BPE_REG_FIELD_READ_ERR_HDL(unit, MANGO_PE_ETAG_MAC_CTRLr, MANGO_TPIDf,\
                                       val, "", errExit, ret);
            *pArg = val;
            break;

        case RTK_BPE_GCT_ECID_RPF_FAIL_ACT:
            BPE_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_CTRLr, MANGO_ECID_RPF_FAIL_ACTf,\
                                       val, "", errExit, ret);
            BPE_VALUE_TO_TYPE(_typeEcidRpfFailAct, action, val, "", errExit, ret);
            *pArg = (int32)action;
            break;

        case RTK_BPE_GCT_PE_LM_FTLR:
            BPE_REG_FIELD_READ_ERR_HDL(unit, MANGO_L2_CTRLr, MANGO_PE_LM_FLTRf,\
                                       val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            break;

        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

    errInput:
    errExit:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_globalCtrl_get */

/* Function Name:
 *      dal_mango_bpe_globalCtrl_set
 * Description:
 *      Set the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argument
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
dal_mango_bpe_globalCtrl_set(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,type=%d,arg=%d", unit, type, arg);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_BPE_GCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    BPE_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_BPE_GCT_ETAG_CPU_KEEP:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_PE_ETAG_CTRLr, MANGO_CPU_KEEPf,
                                        val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_GCT_ETAG_TPID:
            val = (uint32)arg;
            BPE_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_PE_ETAG_MAC_CTRLr, MANGO_TPIDf,
                                        val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_GCT_ECID_RPF_FAIL_ACT:
            BPE_TYPE_TO_VALUE(_typeEcidRpfFailAct, val, arg, "", errExit, ret);
            BPE_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_CTRLr, MANGO_ECID_RPF_FAIL_ACTf,
                                        val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_GCT_PE_LM_FTLR:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_FIELD_WRITE_ERR_HDL(unit, MANGO_L2_CTRLr, MANGO_PE_LM_FLTRf,
                                        val, "", errExit, ret);
            goto errOk;
            break;

        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

errInput:
errExit:
errOk:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_globalCtrl_set */

/* Function Name:
 *      dal_mango_bpe_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 * Output:
 *      pArg - pointer to the argument
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
dal_mango_bpe_portCtrl_get(uint32 unit, rtk_port_t port, rtk_bpe_portCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,type=%d", unit, port, type);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= RTK_BPE_PCT_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_BPE_PCT_PETAG_PARSE_EN:
            BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_MAC_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_ETAG_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_IGR_RSVD_ECID_FLTR_EN:
            BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_RSVD_ECID_FLTRf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_IGR_MC_ECID_FLTR_EN:
            BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_MC_ECID_FLTRf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_EGR_TAG_DEI_RMK_EN:
            BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_TAG_DEI_RMK_ENf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_EGR_TAG_PRI_KEEP_EN:
            BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_TAG_PRI_KEEPf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_EGR_SRC_PORT_FLTR_EN:
            BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_PE_SRC_PORT_FLTRf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_USE_DEFAULT:
             BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_USE_DEFAULTf, val, "", errExit, ret);
             *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_BASE_PE:
             BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_BASE_PEf, val, "", errExit, ret);
             *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        case RTK_BPE_PCT_IGR_ECID_RPF_CHK_EN:
            BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_ECID_RPF_CHKf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            goto errOk;
            break;

        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

    errInput:
    errExit:
    errOk:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portCtrl_get */

/* Function Name:
 *      dal_mango_bpe_portCtrl_set
 * Description:
 *      Set the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 *      arg  - argument
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_mango_bpe_portCtrl_set(uint32 unit, rtk_port_t port, rtk_bpe_portCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,port=%d,type=%d,arg=%d", unit, port, type, arg);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= RTK_BPE_PCT_END), RT_ERR_OUT_OF_RANGE);;

    /* function body */
    BPE_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_BPE_PCT_PETAG_PARSE_EN:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_MAC_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_ETAG_ENf,val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_PCT_IGR_RSVD_ECID_FLTR_EN:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_RSVD_ECID_FLTRf,val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_PCT_IGR_MC_ECID_FLTR_EN:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_MC_ECID_FLTRf, val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_PCT_EGR_TAG_DEI_RMK_EN:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_TAG_DEI_RMK_ENf, val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_PCT_EGR_TAG_PRI_KEEP_EN:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_TAG_PRI_KEEPf, val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_PCT_EGR_SRC_PORT_FLTR_EN:
           val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_PE_SRC_PORT_FLTRf, val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_PCT_USE_DEFAULT:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_USE_DEFAULTf, val, "", errExit, ret);
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_USE_DEFAULTf, val, "", errExit, ret);
            goto errOk;
            break;


        case RTK_BPE_PCT_BASE_PE:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_PCID_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_BASE_PEf, val, "", errExit, ret);
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_EGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_BASE_PEf, val, "", errExit, ret);
            goto errOk;
            break;

        case RTK_BPE_PCT_IGR_ECID_RPF_CHK_EN:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_PE_PORT_ETAG_IGR_CTRLr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_ECID_RPF_CHKf, val, "", errExit, ret);
            goto errOk;
            break;

        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

    errInput:
    errExit:
    errOk:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_portCtrl_set */

/* Function Name:
 *      dal_mango_bpe_priRemarking_get
 * Description:
 *      Get the remarked priority (3 bits) of a specified source.
 * Input:
 *      unit - unit id
 *      src  - remark source
 *      val  - remark source value
 * Output:
 *      pPri - pointer to the remarked priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_bpe_priRmkSrc_t \ Chip:           9310
 *      - PETAG_PRI_RMK_SRC_INT_PRI             O
 */
int32
dal_mango_bpe_priRemarking_get(uint32 unit, rtk_bpe_priRmkSrc_t src,
    rtk_bpe_priRmkVal_t val, rtk_pri_t *pPri)
{
    int32   ret = RT_ERR_OK;
    uint32  priority = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,src=%d,val=%d", unit, src, val);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((src >= PETAG_PRI_RMK_SRC_END), RT_ERR_INPUT);
    /* XXX add range check for val according to the source selection*/
    RT_PARAM_CHK((NULL == pPri), RT_ERR_NULL_POINTER);

    /* function body */
    BPE_SEM_LOCK(unit);

    switch (src)
    {
        case PETAG_PRI_RMK_SRC_INT_PRI:
             BPE_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                 MANGO_PE_ETAG_RMK_CTRLr, REG_ARRAY_INDEX_NONE, val.pri.val, \
                 MANGO_PRIf, priority, "", errExit, ret);
            *pPri = priority;
            goto errOk;
            break;
        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

    errInput:
    errExit:
    errOk:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_priRemarking_get */

/* Function Name:
 *      dal_mango_bpe_priRemarking_set
 * Description:
 *      Set the remarked priority (3 bits) of a specified source.
 * Input:
 *      unit - unit id
 *      src  - remark source
 *      val  - remark source value
 *      pri  - remarked priority value (range from 0 ~ 7)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid priority
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_bpe_priRmkSrc_t \ Chip:           9310
 *      - PETAG_PRI_RMK_SRC_INT_PRI             O
 */
int32
dal_mango_bpe_priRemarking_set(uint32 unit, rtk_bpe_priRmkSrc_t src,
    rtk_bpe_priRmkVal_t val, rtk_pri_t pri)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,src=%d,val=%d,pri=%d", unit, src, val, pri);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((src >= PETAG_PRI_RMK_SRC_END), RT_ERR_INPUT);
    /* XXX add range check for val according to the source selection*/
    RT_PARAM_CHK((pri >= RTK_MAX_NUM_OF_PRIORITY), RT_ERR_INPUT);


    /* function body */
    BPE_SEM_LOCK(unit);
    switch (src)
    {
        case PETAG_PRI_RMK_SRC_INT_PRI:
             BPE_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                 MANGO_PE_ETAG_RMK_CTRLr, REG_ARRAY_INDEX_NONE, val.pri.val, \
                 MANGO_PRIf, pri, "", errExit, ret);
            goto errOk;
            break;
        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

    errInput:
    errExit:
    errOk:
    BPE_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_bpe_priRemarking_set */

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_add
 * Description:
 *      Add an ECID_PMSK entry on the specified device.
 * Input:
 *      unit    - unit id
 *      index   - index of ECID_PMSK_LIST table
 * Output:
 *      pEntry  - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
int32
dal_mango_bpe_ecidPmskEntry_add(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  curr_idx = BPE_PMSKLIST_INVALID;
    uint32  prev_idx = BPE_PMSKLIST_INVALID;
    uint32  ecid_pmsk_idx = BPE_PMSKLIST_INVALID;

    dal_mango_bpe_ecidPmsk_entry_t prev_entry;
    dal_mango_bpe_ecidPmsk_entry_t curr_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%u, index=%u", unit, index);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    BPE_TABLE_FMT_CHECK(unit);

    /* function body */
    ECID_PSMK_SEM_LOCK(unit);

    /* parameter initialization*/
    osal_memset(&curr_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    osal_memset(&prev_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));

    /* initialize current index as the index from input (l2_CB_MC) */
    curr_idx = index;

    while (curr_idx != BPE_PMSKLIST_INVALID)
    {
        /* find the existed ecid_pmsk entry */
        if(RT_ERR_OK == _dal_mango_bpe_pmskEntry_get(unit, curr_idx, &curr_entry))
        {
            if (RT_ERR_OK == _dal_mango_bpe_pmskEntry_compare(pEntry, &curr_entry))
            {
                /* found the pmsk entry with the same ecid and nsg, stop finding the next entry */
                ecid_pmsk_idx = curr_idx;
                curr_idx = BPE_PMSKLIST_INVALID;
            }
            else
            {
                /* find the next one, and backup the current entry as previous one */
                osal_memcpy(&prev_entry, &curr_entry, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
                prev_idx = curr_idx;
                curr_idx = curr_entry.next_index;
            }
        }
    }

    if (ecid_pmsk_idx == BPE_PMSKLIST_INVALID)
    {
        /* alloc ecid pmsk list entry */
        if((ret = _dal_mango_bpe_pmskList_alloc(unit, &ecid_pmsk_idx))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_BPE), "alloc failed! ret=%d", ret);
            goto errHandler;
        }

        if (prev_idx != BPE_PMSKLIST_INVALID)
        {
            /* always add the new pmsk entry in the tail of list */
            /* the next index of the previous entry is this new pmsk entry */
            prev_entry.next_index = ecid_pmsk_idx;
            if((ret = _dal_mango_bpe_pmskEntry_set(unit, prev_idx, &prev_entry))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_BPE), "update previous falied! ret=%d", ret);
                goto errHandler;
            }
        }
    }

    /* configure ecid pmsk list entry */
    if((ret = _dal_mango_bpe_pmskEntry_set(unit, ecid_pmsk_idx, pEntry))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "set entry failed! ret=%d", ret);
        goto errHandler;
    }
    pEntry->curr_index = ecid_pmsk_idx;

    errHandler:
    ECID_PSMK_SEM_UNLOCK(unit);
    return ret;
}/* end of dal_mango_bpe_ecidPmskEntry_add */

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_del
 * Description:
 *      Delete an ECID_PMSK entry on the specified device.
 * Input:
 *      unit     - unit id
 *      index    - index of ECID_PMSK_LIST table
 * Output:
 *      pEntry   - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
int32 dal_mango_bpe_ecidPmskEntry_del(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  curr_idx = BPE_PMSKLIST_INVALID;
    uint32  prev_idx = BPE_PMSKLIST_INVALID;
    uint32  ecid_pmsk_index = BPE_PMSKLIST_INVALID;

    dal_mango_bpe_ecidPmsk_entry_t prev_entry;
    dal_mango_bpe_ecidPmsk_entry_t curr_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);
    BPE_TABLE_FMT_CHECK(unit);

    /* parameter check */
    // RT_PARAM_CHK((index >= XXX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "curr_index=%d,ecid=0x%x", pEntry->curr_index, pEntry->ecid);

    osal_memset(&curr_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    osal_memset(&prev_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));

    /* function body */
    ECID_PSMK_SEM_LOCK(unit);

    /* initialize current index as the index from l2 bpe entry */
    curr_idx = index;

    /* find the pmsk entry */
    while (curr_idx != BPE_PMSKLIST_INVALID)
    {
        if(RT_ERR_OK == _dal_mango_bpe_pmskEntry_get(unit, curr_idx, &curr_entry))
        {
            if (RT_ERR_OK == _dal_mango_bpe_pmskEntry_compare(pEntry, &curr_entry))
            {
                /* found the pmsk entry with the same ecid and nsg */
                ecid_pmsk_index = curr_idx;
                curr_idx = BPE_PMSKLIST_INVALID;
            }
            else
            {
                /* find the next, and backup the current entry as previous one */
                osal_memcpy(&prev_entry, &curr_entry, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
                prev_idx = curr_idx;
                curr_idx = curr_entry.next_index;

            }
        }
    }

    if (ecid_pmsk_index == BPE_PMSKLIST_INVALID)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "delete enrtry not found", ret);
        goto errHandler;
    }

    if (prev_idx != BPE_PMSKLIST_INVALID)
    {
        /* Update the next index of the previous entry */
        prev_entry.next_index = curr_entry.next_index;
        if((ret = _dal_mango_bpe_pmskEntry_set(unit, prev_idx, &prev_entry))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_BPE), "", ret);
            goto errHandler;
        }
    }

    /* set curr_entry as empty */
    osal_memset(&curr_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    if((ret = _dal_mango_bpe_pmskEntry_set(unit, ecid_pmsk_index, &curr_entry))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "", ret);
        goto errHandler;
    }

    /* free ecid pmsk list entry */
    _dal_mango_bpe_pmsklist_free(unit, ecid_pmsk_index);

    errHandler:
    ECID_PSMK_SEM_UNLOCK(unit);
    return ret;
}/* end of dal_mango_bpe_ecidPmskEntry_del */

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_get
 * Description:
 *      Get an ECID_PMSK entry on the specified device.
 * Input:
 *      unit    - unit id
 *      index   - index of ECID_PMSK_LIST table
 * Output:
 *      pEntry  - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
int32 dal_mango_bpe_ecidPmskEntry_get(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    uint32  search_idx = BPE_PMSKLIST_INVALID;
    uint32  ecid_pmsk_index = BPE_PMSKLIST_INVALID;
    dal_mango_bpe_ecidPmsk_entry_t curr_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    // RT_PARAM_CHK((head_index >= XXX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    BPE_TABLE_FMT_CHECK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "get entry with ecid=0x%x", pEntry->ecid);


    osal_memset(&curr_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));

    /* function body */
    ECID_PSMK_SEM_LOCK(unit);

    /* initialize search index */
    search_idx = index;

    /* get the ecid_pmsk entry by ecid*/
    while (search_idx != BPE_PMSKLIST_INVALID)
    {
        if(RT_ERR_OK == _dal_mango_bpe_pmskEntry_get(unit, search_idx, &curr_entry))
        {
            if (RT_ERR_OK == _dal_mango_bpe_pmskEntry_compare(pEntry, &curr_entry))
            {
                /* found the pmsk entry with the same ecid and nsg */
                ecid_pmsk_index = search_idx;
                break;
            }
            else
            {
                /* find the next */
                search_idx = curr_entry.next_index;
            }
        }
    }

    if (ecid_pmsk_index == BPE_PMSKLIST_INVALID)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandler;
    }

    /* return the found entry */
    osal_memcpy(pEntry, &curr_entry, sizeof(dal_mango_bpe_ecidPmsk_entry_t));
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "get entry idx %u with next_idx=%u, port_type=%u, portmask_idx=%u",
        pEntry->curr_index, pEntry->next_index,
        pEntry->port_type,
        pEntry->portmask.pmsk_index);

    errHandler:
    ECID_PSMK_SEM_UNLOCK(unit);
    return ret;
}/* end of dal_mango_bpe_ecidPmskEntry_get */

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_set
 * Description:
 *      Set an ECID_PMSK entry on the specified device.
 * Input:
 *      unit   - unit id
 *      index  - index of ECID_PMSK_LIST table
 *      pEntry - pointer to the ECID_PMSK_LIST table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
int32 dal_mango_bpe_ecidPmskEntry_set(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    dal_mango_bpe_ecidPmsk_entry_t curr_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    // RT_PARAM_CHK((index >= XXX), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    BPE_TABLE_FMT_CHECK(unit);

    osal_memset(&curr_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));

    /* function body */
    ECID_PSMK_SEM_LOCK(unit);
    /* find the pmsk entry */
    if ((ret = _dal_mango_bpe_pmskEntry_get(unit, index, &curr_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "", ret);
        goto errHandler;
    }

    if ((ret = _dal_mango_bpe_pmskEntry_compare(pEntry, &curr_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "", ret);
        goto errHandler;
    }

    osal_memcpy(&curr_entry, pEntry, sizeof(curr_entry));

    if ((ret = _dal_mango_bpe_pmskEntry_set(unit, index, &curr_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "", ret);
        goto errHandler;
    }

    pEntry->curr_index = index;

    errHandler:
    ECID_PSMK_SEM_UNLOCK(unit);
    return ret;
}/* end of dal_mango_bpe_ecidPmskEntry_set */

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntryNextValid_get
 * Description:
 *      Get a next valid ECID_PMSK entry on the specified device.
 * Input:
 *      unit     - unit id
 *      index    - current index of ECID_PMSK_LIST table to get next
 * Output:
 *      pEntry   - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
int32 dal_mango_bpe_ecidPmskEntryNextValid_get(uint32 unit, uint32 index,
    dal_mango_bpe_ecidPmsk_entry_t *pEntry)
{
    int32   ret = RT_ERR_OK;
    dal_mango_bpe_ecidPmsk_entry_t curr_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "unit=%d,index=%u", unit, index);

    /* check Init status */
    RT_INIT_CHK(bpe_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    BPE_TABLE_FMT_CHECK(unit);
    RT_PARAM_CHK((BPE_PMSKLIST_INVALID == index), RT_ERR_NULL_POINTER);

    osal_memset(&curr_entry, 0, sizeof(dal_mango_bpe_ecidPmsk_entry_t));

    /* function body */
    ECID_PSMK_SEM_LOCK(unit);

    /* find the current pmsk entry */
    if ((ret = _dal_mango_bpe_pmskEntry_get(unit, index, &curr_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "", ret);
        goto errHandler;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_BPE), "curr index=%u, next index =%u",
        curr_entry.curr_index, curr_entry.next_index);

    if (curr_entry.next_index == BPE_PMSKLIST_INVALID)
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandler;
    }

    /* find the next pmsk entry */
    if ((ret = _dal_mango_bpe_pmskEntry_get(unit, curr_entry.next_index, pEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_BPE), "", ret);
        goto errHandler;
    }

    errHandler:
    ECID_PSMK_SEM_UNLOCK(unit);

    return ret;
}/* end of dal_mango_bpe_ecidPmskEntryNextValid_get */



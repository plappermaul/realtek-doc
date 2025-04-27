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
 * $Revision: 102860 $
 * $Date: 2019-12-19 21:46:19 +0800 (Thu, 19 Dec 2019) $
 *
 * Purpose : Definition those public L2 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) l2 address table
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <osal/cache.h>
#include <osal/isr.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
//#include <dal/longan/dal_longan_trunk.h>
#include <dal/longan/dal_longan_l2.h>
//#include <dal/longan/dal_longan_l3.h>
//#include <dal/longan/dal_longan_trap.h>
//#include <dal/longan/dal_longan_vlan.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/trap.h>
#ifdef CONFIG_SDK_DRIVER_L2NTFY
  #include <private/drv/l2ntfy/l2ntfy_rtl9300.h>
#endif
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               l2_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         l2_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         l2_pmsk_tbl_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         l2_sem_2[RTK_MAX_NUM_OF_UNIT];

static uint32 hashTable_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32 cam_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32 mcast_tableSize[RTK_MAX_NUM_OF_UNIT] = {0};

static uint32               blk0AlgoType[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               blk1AlgoType[RTK_MAX_NUM_OF_UNIT] = {0};

static uint16               *nh_used_cnt[RTK_MAX_NUM_OF_UNIT];

static uint32               l2_lrn[ARB_MAX][RTK_MAX_NUM_OF_PORTS];
static uint32               gAging_time[ARB_MAX];

/* Multicast database */
static dal_longan_mcast_index_pool_t    mcast_idx_pool[RTK_MAX_NUM_OF_UNIT];
static dal_longan_l2_mcastFwdShadow_t  *mcast_fwdTable_shadow = NULL;

/* Multicast database */
/*
 * Macro Definition
 */
/* semaphore handling */
#define L2_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(l2_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L2), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define L2_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(l2_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L2), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define L2_SEM_LOCK_2(unit)    \
do {\
    if (osal_sem_mutex_take(l2_sem_2[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L2), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define L2_SEM_UNLOCK_2(unit)   \
do {\
    if (osal_sem_mutex_give(l2_sem_2[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L2), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define L2_PMSK_TBL_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(l2_pmsk_tbl_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L2), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define L2_PMSK_TBL_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(l2_pmsk_tbl_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L2), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

#define L2_REG_FIELD_READ_ERR_HDL(_unit, _reg, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                    \
        if ((_ret = reg_field_read(_unit, _reg, _field, &_val)) != RT_ERR_OK)               \
        {                                                                                   \
            RT_ERR(_ret, (MOD_L2|MOD_DAL), _errMsg);                                        \
            goto _gotoErrLbl;                                                               \
        }                                                                                   \
    } while(0)
#define L2_REG_ARRAY_FIELD_READ_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)    \
    do {                                                                                                        \
        if ((_ret = reg_array_field_read(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)               \
        {                                                                                                       \
            RT_ERR(_ret, (MOD_L2|MOD_DAL), _errMsg);                                                            \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)
#define L2_REG_ARRAY_FIELD_WRITE_ERR_HDL(_unit, _reg, _idx1, _idx2, _field, _val, _errMsg, _gotoErrLbl, _ret)   \
    do {                                                                                                        \
        if ((_ret = reg_array_field_write(_unit, _reg, _idx1, _idx2, _field, &_val)) != RT_ERR_OK)              \
        {                                                                                                       \
            RT_ERR(_ret, (MOD_L2|MOD_DAL), _errMsg);                                                            \
            goto _gotoErrLbl;                                                                                   \
        }                                                                                                       \
    } while(0)

#define L2_BPE_TABLE_READ_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret) \
            do { \
                if ((_ret = table_read(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK) \
                { \
                    RT_ERR(_ret, (MOD_BPE|MOD_L2|MOD_DAL), _errMsg); \
                    goto _gotoErrLbl; \
                } \
            } while(0)
#define L2_BPE_TABLE_WRITE_ERR_HDL(_unit, _tbl, _idx, _entry, _errMsg, _gotoErrLbl, _ret) \
            do { \
                if ((_ret = table_write(_unit, _tbl, _idx, (uint32 *)&_entry)) != RT_ERR_OK) \
                { \
                    RT_ERR(_ret, (MOD_BPE|MOD_L2|MOD_DAL), _errMsg); \
                    goto _gotoErrLbl; \
                } \
            } while(0)
#define L2_BPE_TABLE_FIELD_GET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret) \
            do { \
                if ((_ret = table_field_get(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
                { \
                    RT_ERR(_ret, (MOD_BPE|MOD_L2|MOD_DAL), _errMsg); \
                    goto _gotoErrLbl; \
                } \
            } while(0)
#define L2_BPE_TABLE_FIELD_SET_ERR_HDL(_unit, _tbl, _field, _val, _entry, _errMsg, _gotoErrLbl, _ret) \
            do { \
                if ((_ret = table_field_set(_unit, _tbl, _field, (uint32 *)&_val, (uint32 *)&_entry)) != RT_ERR_OK) \
                { \
                    RT_ERR(_ret, (MOD_BPE|MOD_L2|MOD_DAL), _errMsg); \
                    goto _gotoErrLbl; \
                } \
            } while(0)

//static int32 _dal_longan_l2_allocMcastIdx(uint32 unit, int32 *pMcastIdx);
//static int32 _dal_longan_l2_isMcastIdxUsed(uint32 unit, int32 mcastIdx);
//static int32 _dal_longan_l2_freeMcastIdx(uint32 unit, int32 mcastIdx);
//static int32 _dal_longan_l2_compareEntry(dal_longan_l2_entry_t *pSrcEntry, dal_longan_l2_entry_t *pDstEntry);
//static int32 _dal_longan_l2_getExistOrFreeL2Entry(uint32 unit, dal_longan_l2_entry_t *pL2_entry, dal_longan_l2_getMethod_t get_method, dal_longan_l2_index_t *pL2_index);
//static int32 _dal_longan_l2_nextValidAddr_get(uint32 unit,int32 *pScan_idx, uint32 type, uint32 include_static, dal_longan_l2_entry_t *pL2_addr);
//static int32 _dal_longan_l2_init_config(uint32 unit);
//static int32 _dal_longan_l2_mcastFwdPortmaskEntryRefCnt_get(uint32 unit, int32 index, uint32* pRefCnt);
static int32 _dal_longan_l2_entryToHashKey(uint32 unit, dal_longan_l2_entry_t *pL2_entry, dal_longan_l2_hashKey_t *pKey);

static int32 _dal_longan_l2_setL2Entry(uint32 unit, dal_longan_l2_index_t * pIndex, dal_longan_l2_entry_t *pL2_entry);
static int32 _dal_longan_l2_compareEntry(dal_longan_l2_entry_t *pSrcEntry, dal_longan_l2_entry_t *pDstEntry);
static int32 _dal_longan_l2_getExistOrFreeL2Entry(uint32 unit, dal_longan_l2_entry_t *pL2_entry, dal_longan_l2_getMethod_t get_method, dal_longan_l2_index_t *pL2_index);
static int32 _dal_longan_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac, uint32 sram_if);
// int32 _dal_longan_l2_mcastFwdPortmaskEntry_set(uint32 unit, int32 index, rtk_portmask_t  *pPortmask);

int32 _dal_longan_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex)
{
    int16   free_idx;
    int32   ret = RT_ERR_OK;

    RT_PARAM_CHK(NULL == pFwdIndex, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(*pFwdIndex >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);

    L2_PMSK_TBL_SEM_LOCK(unit);

    if (*pFwdIndex >= 0)
    {
        mcast_idx_pool[unit].pMcast_index_pool[*pFwdIndex].ref_count++;
        goto errOk;
    }

    while(mcast_idx_pool[unit].free_index_head != END_OF_MCAST_IDX)
    {

        free_idx = mcast_idx_pool[unit].free_index_head;

        mcast_idx_pool[unit].free_index_head = mcast_idx_pool[unit].pMcast_index_pool[free_idx].next_index;
        mcast_idx_pool[unit].pMcast_index_pool[free_idx].next_index = MCAST_IDX_ALLOCATED;

        *pFwdIndex = free_idx;
        mcast_idx_pool[unit].pMcast_index_pool[free_idx].ref_count++;

        mcast_idx_pool[unit].free_entry_count--;

        goto errOk;
    }

    ret = RT_ERR_L2_INDEXTBL_FULL;

errOk:
    L2_PMSK_TBL_SEM_UNLOCK(unit);
    return ret;
}

int32 _dal_longan_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index)
{
    int32 ret = RT_ERR_OK;
    rtk_portmask_t portmask;

    RT_PARAM_CHK(index >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK(index < 0, RT_ERR_L2_MULTI_FWD_INDEX);

    if(mcast_idx_pool[unit].pMcast_index_pool[index].ref_count==0)
        return RT_ERR_OK;

    L2_PMSK_TBL_SEM_LOCK(unit);

    mcast_idx_pool[unit].pMcast_index_pool[index].ref_count--;

    if (0 == mcast_idx_pool[unit].pMcast_index_pool[index].ref_count)
    {
        mcast_idx_pool[unit].pMcast_index_pool[index].next_index = mcast_idx_pool[unit].free_index_head;
        mcast_idx_pool[unit].free_index_head = index;
        mcast_idx_pool[unit].free_entry_count++;

        osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
        if((ret=_dal_longan_l2_mcastFwdPortmaskEntry_set(unit, index, &portmask))!= RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
            goto errExit;
        }
    }

errExit:
    L2_PMSK_TBL_SEM_UNLOCK(unit);
    return ret;
}

int32 _dal_longan_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret = RT_ERR_OK;
    multicast_index_entry_t mcast_entry;

    if ((ret = table_read(unit, LONGAN_MC_PORTMASKt, index, (uint32 *)&mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        goto errExit;
    }

    osal_memset(pPortmask, 0, sizeof(rtk_portmask_t));
    if ((ret = table_field_get(unit, LONGAN_MC_PORTMASKt, LONGAN_MC_PORTMASK_PORTMASKtf, &(pPortmask->bits[0]), (uint32 *) &mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        goto errExit;
    }

errExit:
    return ret;
}

int32 _dal_longan_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret;
    multicast_index_entry_t mcast_entry;

    osal_memset(&mcast_entry, 0, sizeof(mcast_entry));

    if ((ret = table_field_set(unit, LONGAN_MC_PORTMASKt, LONGAN_MC_PORTMASK_PORTMASKtf, &(pPortmask->bits[0]), (uint32 *) &mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        goto errExit;
    }

    if ((ret = table_write(unit, LONGAN_MC_PORTMASKt, index, (uint32 *)&mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        goto errExit;
    }

errExit:
    return ret;
}

/*
static int32 _dal_longan_l2_mcastFwdPortmaskEntryRefCnt_get(uint32 unit, int32 index, uint32* pRefCnt)
{
    *pRefCnt = mcast_idx_pool[unit].pMcast_index_pool[index].ref_count;
    return RT_ERR_OK;
}
*/

static int32 _dal_longan_l2_entryToHashKey(uint32 unit, dal_longan_l2_entry_t *pL2_entry, dal_longan_l2_hashKey_t *pKey)
{
    uint64  hashSeed = 0;
    uint32 hash3, hash1;
    uint32 hash0_key;
    uint32 hash1_key;


    if(pL2_entry->entry_type== L2_UNICAST)
    {
        hashSeed = (((uint64)pL2_entry->unicast.fid<<48) |
                    ((uint64)pL2_entry->unicast.mac.octet[0] << 40) |
                    ((uint64)pL2_entry->unicast.mac.octet[1] << 32) |
                    ((uint64)pL2_entry->unicast.mac.octet[2] << 24) |
                    ((uint64)pL2_entry->unicast.mac.octet[3] << 16) |
                    ((uint64)pL2_entry->unicast.mac.octet[4] << 8) |
                    ((uint64)pL2_entry->unicast.mac.octet[5]));
    }
    else
    {
        hashSeed = (((uint64)pL2_entry->l2mcast.fid<<48) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[0] << 40) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[1] << 32) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[2] << 24) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[3] << 16) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[4] << 8) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[5]));
    }

    hash0_key =  (uint32) (((hashSeed >> 55) & BITMASK_5B)
                        ^ ((hashSeed >> 44) & BITMASK_11B)
                        ^ ((hashSeed >> 33) & BITMASK_11B)
                        ^ ((hashSeed >> 22) & BITMASK_11B)
                        ^ ((hashSeed >> 11) & BITMASK_11B)
                        ^ (hashSeed  & BITMASK_11B));

    hash1 = (hashSeed>>11) & BITMASK_11B;
    hash1 = ((hash1 & BITMASK_5B) <<6) | ((hash1>>5) & BITMASK_6B);

    hash3 = (hashSeed>>33) & BITMASK_11B;
    hash3 = ((hash3 & BITMASK_6B) <<5) | ((hash3>>6) & BITMASK_5B);

    hash1_key = (uint32) ((hashSeed >> 55) & BITMASK_5B)
                    ^ ((hashSeed >> 44) & BITMASK_11B)
                    ^ hash3
                    ^ ((hashSeed >> 22) & BITMASK_11B)
                    ^ hash1
                    ^ (hashSeed  & BITMASK_11B);

    if(blk0AlgoType[unit]==0)
        pKey->blk0_hashKey = hash0_key;
    else
        pKey->blk0_hashKey = hash1_key;

    if(blk1AlgoType[unit]==0)
        pKey->blk1_hashKey = hash0_key+((hashTable_size[unit]/2)>>2);
    else
        pKey->blk1_hashKey = hash1_key+((hashTable_size[unit]/2)>>2);

    return RT_ERR_OK;
}

int32 _dal_longan_l2_getL2Entry(uint32 unit, dal_longan_l2_index_t * pIndex, dal_longan_l2_entry_t *pL2_entry)
{
    int32 ret;
    l2_entry_t  l2_entry;

    uint32 is_l2mcast;
    rtk_fid_t fid;
    uint32  mac_uint32[2];
    uint32 next_hop;
    uint32 is_valid;
    uint32 spa;

    osal_memset(pL2_entry, 0, sizeof(dal_longan_l2_entry_t));

    /* read entry from chip */
    if(pIndex->index_type==L2_IN_HASH)
    {
        if ((ret = table_read(unit, LONGAN_L2_UCt, ((pIndex->index<<2) | (pIndex->hashdepth)), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    else
    {
        if ((ret = table_read(unit, LONGAN_L2_CAM_UCt, (pIndex->index), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_VALIDtf, &is_valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    pL2_entry->is_entry_valid = is_valid;
    if(is_valid==0)
        return RT_ERR_OK;

    if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_FID_RVIDtf, &fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_MACtf, &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    is_l2mcast = (mac_uint32[1] >> 8) & BITMASK_1B;

    if(is_l2mcast)
    {
        pL2_entry->entry_type = L2_MULTICAST;

        pL2_entry->l2mcast.fid = fid;
        pL2_entry->l2mcast.mac.octet[0] = (uint8)(mac_uint32[1] >> 8);
        pL2_entry->l2mcast.mac.octet[1] = (uint8)(mac_uint32[1]);
        pL2_entry->l2mcast.mac.octet[2] = (uint8)(mac_uint32[0] >> 24);
        pL2_entry->l2mcast.mac.octet[3] = (uint8)(mac_uint32[0] >> 16);
        pL2_entry->l2mcast.mac.octet[4] = (uint8)(mac_uint32[0] >> 8);
        pL2_entry->l2mcast.mac.octet[5] = (uint8)(mac_uint32[0]);

        if ((ret = table_field_get(unit, LONGAN_L2_MCt, LONGAN_L2_MC_NEXT_HOPtf, &next_hop, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        pL2_entry->l2mcast.nh = next_hop;

        if ((ret = table_field_get(unit, LONGAN_L2_MCt, LONGAN_L2_MC_MC_PMSK_IDXtf, &pL2_entry->l2mcast.index, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if(next_hop)
        {
            if ((ret = table_field_get(unit, LONGAN_L2_MCt, LONGAN_L2_MC_MAC_IDXtf, &pL2_entry->l2mcast.mac_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
    }
    else
    {
        pL2_entry->entry_type = L2_UNICAST;

        pL2_entry->unicast.fid = fid;
        pL2_entry->unicast.mac.octet[0] = (uint8)(mac_uint32[1] >> 8);
        pL2_entry->unicast.mac.octet[1] = (uint8)(mac_uint32[1]);
        pL2_entry->unicast.mac.octet[2] = (uint8)(mac_uint32[0] >> 24);
        pL2_entry->unicast.mac.octet[3] = (uint8)(mac_uint32[0] >> 16);
        pL2_entry->unicast.mac.octet[4] = (uint8)(mac_uint32[0] >> 8);
        pL2_entry->unicast.mac.octet[5] = (uint8)(mac_uint32[0]);

        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_NEXT_HOPtf, &next_hop, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        pL2_entry->unicast.nh = next_hop;

        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_IS_TRKtf, &pL2_entry->unicast.is_trk, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_SPAtf, &spa, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if(pL2_entry->unicast.is_trk==0)
        {
            pL2_entry->unicast.devID = (spa>>6) & 0xf;
            pL2_entry->unicast.port = spa & 0x3f;
        }
        else
        {
            pL2_entry->unicast.devID = (spa>>9) & 0x1;
            pL2_entry->unicast.trunk = spa & 0x3f;
        }

        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_AGEtf, &pL2_entry->unicast.aging, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_SA_BLKtf, &pL2_entry->unicast.sablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_DA_BLKtf, &pL2_entry->unicast.dablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_STATICtf, &pL2_entry->unicast.is_static, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_SUSPENDtf, &pL2_entry->unicast.suspending, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(next_hop)
        {
            if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_VIDtf, &pL2_entry->unicast.mac_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
        else
        {
            if ((ret = table_field_get(unit, LONGAN_L2_UCt, LONGAN_L2_UC_VIDtf, &pL2_entry->unicast.agg_vid, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_longan_l2_setL2Entry(uint32 unit, dal_longan_l2_index_t * pIndex, dal_longan_l2_entry_t *pL2_entry)
{
    int32 ret;
    l2_entry_t  l2_entry;
    uint32  mac_uint32[2] ={0,0};
    uint32 spa=0;

    osal_memset(&l2_entry, 0, sizeof(l2_entry));

    if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_VALIDtf, &pL2_entry->is_entry_valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if(pL2_entry->entry_type == L2_UNICAST)
    {
        mac_uint32[1] = ((uint32)pL2_entry->unicast.mac.octet[0]) << 8;
        mac_uint32[1] |= ((uint32)pL2_entry->unicast.mac.octet[1]);
        mac_uint32[0] = ((uint32)pL2_entry->unicast.mac.octet[2]) << 24;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[3]) << 16;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[4]) << 8;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[5]);

        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_MACtf, &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_FID_RVIDtf, &pL2_entry->unicast.fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_IS_TRKtf, &pL2_entry->unicast.is_trk, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(pL2_entry->unicast.is_trk==0)
        {
            spa = (pL2_entry->unicast.devID<<6) | pL2_entry->unicast.port;
        }
        else
        {
            spa = (pL2_entry->unicast.devID<<9) | pL2_entry->unicast.trunk;
        }

        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_SPAtf, &spa, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_AGEtf, &pL2_entry->unicast.aging, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_SA_BLKtf, &pL2_entry->unicast.sablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_DA_BLKtf, &pL2_entry->unicast.dablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_STATICtf, &pL2_entry->unicast.is_static, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_SUSPENDtf, &pL2_entry->unicast.suspending, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_NEXT_HOPtf, &pL2_entry->unicast.nh, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(pL2_entry->unicast.nh)
        {
            if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_VIDtf, &pL2_entry->unicast.mac_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
        else
        {
            if ((ret = table_field_set(unit, LONGAN_L2_UCt, LONGAN_L2_UC_VIDtf, &pL2_entry->unicast.agg_vid, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
    }
    else
    {
        mac_uint32[1] = ((uint32)pL2_entry->l2mcast.mac.octet[0]) << 8;
        mac_uint32[1] |= ((uint32)pL2_entry->l2mcast.mac.octet[1]);
        mac_uint32[0] = ((uint32)pL2_entry->l2mcast.mac.octet[2]) << 24;
        mac_uint32[0] |= ((uint32)pL2_entry->l2mcast.mac.octet[3]) << 16;
        mac_uint32[0] |= ((uint32)pL2_entry->l2mcast.mac.octet[4]) << 8;
        mac_uint32[0] |= ((uint32)pL2_entry->l2mcast.mac.octet[5]);

        if ((ret = table_field_set(unit, LONGAN_L2_MCt, LONGAN_L2_MC_MACtf, &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if ((ret = table_field_set(unit, LONGAN_L2_MCt, LONGAN_L2_MC_FID_RVIDtf, &pL2_entry->l2mcast.fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if ((ret = table_field_set(unit, LONGAN_L2_MCt, LONGAN_L2_MC_MC_PMSK_IDXtf, &pL2_entry->l2mcast.index, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, LONGAN_L2_MCt, LONGAN_L2_MC_NEXT_HOPtf, &pL2_entry->l2mcast.nh, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if(pL2_entry->l2mcast.nh)
        {
            if ((ret = table_field_set(unit, LONGAN_L2_MCt, LONGAN_L2_MC_MAC_IDXtf, &pL2_entry->l2mcast.mac_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
    }

    if(pIndex->index_type==L2_IN_HASH)
    {
        if ((ret = table_write(unit, LONGAN_L2_UCt, ((pIndex->index<<2) | (pIndex->hashdepth)), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    else
    {
        if ((ret = table_write(unit, LONGAN_L2_CAM_UCt, (pIndex->index), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_longan_l2_compareEntry(dal_longan_l2_entry_t *pSrcEntry, dal_longan_l2_entry_t *pDstEntry)
{
    RT_LOG(LOG_TRACE, (MOD_DAL|MOD_L2), "entry_type=%d", pSrcEntry->entry_type);

    if (pSrcEntry->entry_type != pDstEntry->entry_type)
        return RT_ERR_FAILED;

    switch (pSrcEntry->entry_type)
    {
        case L2_UNICAST:
            RT_LOG(LOG_TRACE, (MOD_DAL|MOD_L2), "srcfid=%d, dstfid=%d, srcMac=%x-%x-%x-%x-%x-%x,\
                   dstMac=%x-%x-%x-%x-%x-%x", pSrcEntry->unicast.fid, pDstEntry->unicast.fid,
                   pSrcEntry->unicast.mac.octet[0], pSrcEntry->unicast.mac.octet[1], pSrcEntry->unicast.mac.octet[2],
                   pSrcEntry->unicast.mac.octet[3], pSrcEntry->unicast.mac.octet[4], pSrcEntry->unicast.mac.octet[5],
                   pDstEntry->unicast.mac.octet[0], pDstEntry->unicast.mac.octet[1], pDstEntry->unicast.mac.octet[2],
                   pDstEntry->unicast.mac.octet[3], pDstEntry->unicast.mac.octet[4], pDstEntry->unicast.mac.octet[5]);

            if ((osal_memcmp(&pSrcEntry->unicast.mac, &pDstEntry->unicast.mac, sizeof(rtk_mac_t)))
               || (pSrcEntry->unicast.fid != pDstEntry->unicast.fid))
            {
                return RT_ERR_FAILED;
            }

            return RT_ERR_OK;

        case L2_MULTICAST:
            RT_LOG(LOG_TRACE, (MOD_DAL|MOD_L2), "srcrvid=%d, dstrvid=%d, srcMac=%x-%x-%x-%x-%x-%x,\
                   dstMac=%x-%x-%x-%x-%x-%x", pSrcEntry->l2mcast.fid, pDstEntry->l2mcast.fid,
                   pSrcEntry->l2mcast.mac.octet[0], pSrcEntry->l2mcast.mac.octet[1], pSrcEntry->l2mcast.mac.octet[2],
                   pSrcEntry->l2mcast.mac.octet[3], pSrcEntry->l2mcast.mac.octet[4], pSrcEntry->l2mcast.mac.octet[5],
                   pDstEntry->l2mcast.mac.octet[0], pDstEntry->l2mcast.mac.octet[1], pDstEntry->l2mcast.mac.octet[2],
                   pDstEntry->l2mcast.mac.octet[3], pDstEntry->l2mcast.mac.octet[4], pDstEntry->l2mcast.mac.octet[5]);

            if ((osal_memcmp(&pSrcEntry->l2mcast.mac, &pDstEntry->l2mcast.mac, sizeof(rtk_mac_t)))
               || (pSrcEntry->l2mcast.fid != pDstEntry->l2mcast.fid))
            {
                return RT_ERR_FAILED;
            }

            return RT_ERR_OK;
       default:
            return RT_ERR_FAILED;
    }
}

static int32 _dal_longan_l2_getExistOrFreeL2Entry(uint32 unit, dal_longan_l2_entry_t *pL2_entry, dal_longan_l2_getMethod_t get_method, dal_longan_l2_index_t *pL2_index)
{
    int32 ret;
    uint32 hash_depth;
    uint32 value;

    rtk_enable_t l2CamEbl;

    uint32 is_found = 0;

    uint32 blk0_entryCnt =0;
    uint32 blk1_entryCnt = 0;
    uint32 blk0_freeDepth = 0xffffffff;
    uint32 blk1_freeDepth = 0xffffffff;
    uint32 cam_freeIndex = 0xffffffff;

    dal_longan_l2_hashKey_t hashkey;
    dal_longan_l2_entry_t l2_entry;
    dal_longan_l2_index_t blk0_index;
    dal_longan_l2_index_t blk1_index;
    dal_longan_l2_index_t cam_index;

    hash_depth = HAL_L2_HASHDEPTH(unit);

    /* calculate hash key */
    if ((ret = _dal_longan_l2_entryToHashKey(unit, pL2_entry, &hashkey)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blk0_index.index_type = L2_IN_HASH;
    blk0_index.index = hashkey.blk0_hashKey;
    for(blk0_index.hashdepth=0; blk0_index.hashdepth<hash_depth; blk0_index.hashdepth++)
    {
        if ((ret = _dal_longan_l2_getL2Entry(unit, &blk0_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(l2_entry.is_entry_valid)
        {
            blk0_entryCnt++;
            if(RT_ERR_OK == _dal_longan_l2_compareEntry(pL2_entry, &l2_entry))
            {
                is_found = 1; break;
            }
        }
        else
        {
            if(blk0_freeDepth == 0xffffffff)
                blk0_freeDepth = blk0_index.hashdepth;
        }
    }

    if(is_found==1)
    {
        if(get_method == L2_GET_EXIST_ONLY || get_method == L2_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pL2_index, &blk0_index, sizeof(dal_longan_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_longan_l2_entry_t));
            pL2_entry->is_entry_exist = 1;
            return RT_ERR_OK;
        }
        else
            return RT_ERR_L2_ENTRY_EXIST;
    }

    blk1_index.index_type = L2_IN_HASH;
    blk1_index.index = hashkey.blk1_hashKey;
    for(blk1_index.hashdepth=0; blk1_index.hashdepth<hash_depth; blk1_index.hashdepth++)
    {
        if ((ret = _dal_longan_l2_getL2Entry(unit, &blk1_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(l2_entry.is_entry_valid)
        {
            blk1_entryCnt++;
            if(RT_ERR_OK == _dal_longan_l2_compareEntry(pL2_entry, &l2_entry))
            {
                is_found = 1; break;
            }
        }
        else
        {
            if(blk1_freeDepth == 0xffffffff)
                blk1_freeDepth = blk1_index.hashdepth;
        }
    }

    if(is_found==1)
    {
        if(get_method == L2_GET_EXIST_ONLY || get_method == L2_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pL2_index, &blk1_index, sizeof(dal_longan_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_longan_l2_entry_t));
            pL2_entry->is_entry_exist = 1;
            return RT_ERR_OK;
        }
        else
            return RT_ERR_L2_ENTRY_EXIST;
    }

    if((ret=reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LUTCAM_ENf, &value))!=RT_ERR_OK)
        return ret;
    l2CamEbl = value==1 ? ENABLED : DISABLED;

    if(l2CamEbl==ENABLED)
    {
        cam_index.index_type = L2_IN_CAM;
        for(cam_index.index=0; cam_index.index<cam_size[unit]; cam_index.index++)
        {
            if ((ret = _dal_longan_l2_getL2Entry(unit, &cam_index, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if(l2_entry.is_entry_valid)
            {
                if(RT_ERR_OK == _dal_longan_l2_compareEntry(pL2_entry, &l2_entry))
                {
                    is_found = 1; break;
                }
            }
            else
            {
                if(cam_freeIndex == 0xffffffff)
                    cam_freeIndex = cam_index.index;
            }
        }

        if(is_found==1)
        {
            if(get_method == L2_GET_EXIST_ONLY || get_method == L2_GET_EXIST_OR_FREE)
            {
                osal_memcpy(pL2_index, &cam_index, sizeof(dal_longan_l2_index_t));
                osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_longan_l2_entry_t));
                pL2_entry->is_entry_exist = 1;
                return RT_ERR_OK;
            }
            else
                return RT_ERR_L2_ENTRY_EXIST;
        }
    }

    pL2_entry->is_entry_exist = 0;

    if(get_method == L2_GET_EXIST_ONLY)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    if(get_method == L2_GET_EXIST_OR_FREE || get_method == L2_GET_FREE_ONLY)
    {
        if(blk0_freeDepth != 0xffffffff && blk1_freeDepth == 0xffffffff)
        {
            blk0_index.hashdepth = blk0_freeDepth;
            osal_memcpy(pL2_index, &blk0_index, sizeof(dal_longan_l2_index_t));
            return RT_ERR_OK;
        }
        else if(blk0_freeDepth == 0xffffffff && blk1_freeDepth != 0xffffffff)
        {
            blk1_index.hashdepth = blk1_freeDepth;
            osal_memcpy(pL2_index, &blk1_index, sizeof(dal_longan_l2_index_t));
            return RT_ERR_OK;
        }
        else if(blk0_freeDepth != 0xffffffff && blk1_freeDepth != 0xffffffff)
        {
            if(blk0_entryCnt<=blk1_entryCnt)
            {
                blk0_index.hashdepth = blk0_freeDepth;
                osal_memcpy(pL2_index, &blk0_index, sizeof(dal_longan_l2_index_t));
                return RT_ERR_OK;
            }
            else
            {
                blk1_index.hashdepth = blk1_freeDepth;
                osal_memcpy(pL2_index, &blk1_index, sizeof(dal_longan_l2_index_t));
                return RT_ERR_OK;
            }
        }
        else
        {
            if(l2CamEbl==ENABLED && cam_freeIndex != 0xffffffff)
            {
                cam_index.index= cam_freeIndex;
                osal_memcpy(pL2_index, &cam_index, sizeof(dal_longan_l2_index_t));
                return RT_ERR_OK;
            }
            else
                return RT_ERR_L2_NO_EMPTY_ENTRY;
        }
    }

    return RT_ERR_OK;
}


static int32 _dal_longan_l2_getFirstDynamicEntry(uint32 unit, dal_longan_l2_entry_t *pL2_entry, dal_longan_l2_index_t *pL2_index)
{
    int32 ret;
    uint32 hash_depth;
    rtk_enable_t l2CamEbl;
    uint32 value;

    dal_longan_l2_hashKey_t hashkey;
    dal_longan_l2_entry_t l2_entry;
    dal_longan_l2_index_t blk0_index;
    dal_longan_l2_index_t blk1_index;
    dal_longan_l2_index_t cam_index;

    hash_depth = HAL_L2_HASHDEPTH(unit);

    /* calculate hash key */
    if ((ret = _dal_longan_l2_entryToHashKey(unit, pL2_entry, &hashkey)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blk0_index.index_type = L2_IN_HASH;
    blk0_index.index = hashkey.blk0_hashKey;
    for(blk0_index.hashdepth=0; blk0_index.hashdepth<hash_depth; blk0_index.hashdepth++)
    {
        if ((ret = _dal_longan_l2_getL2Entry(unit, &blk0_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(l2_entry.is_entry_valid &&
            l2_entry.entry_type==L2_UNICAST &&
            l2_entry.unicast.sablock==0 &&
            l2_entry.unicast.dablock==0 &&
            l2_entry.unicast.is_static==0 &&
            l2_entry.unicast.nh==0
            )
        {
            osal_memcpy(pL2_index, &blk0_index, sizeof(dal_longan_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_longan_l2_entry_t));
            return RT_ERR_OK;
        }
    }

    blk1_index.index_type = L2_IN_HASH;
    blk1_index.index = hashkey.blk1_hashKey;
    for(blk1_index.hashdepth=0; blk1_index.hashdepth<hash_depth; blk1_index.hashdepth++)
    {
        if ((ret = _dal_longan_l2_getL2Entry(unit, &blk1_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(l2_entry.is_entry_valid &&
            l2_entry.entry_type==L2_UNICAST &&
            l2_entry.unicast.sablock==0 &&
            l2_entry.unicast.dablock==0 &&
            l2_entry.unicast.is_static==0 &&
            l2_entry.unicast.nh==0
            )
        {
            osal_memcpy(pL2_index, &blk1_index, sizeof(dal_longan_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_longan_l2_entry_t));
            return RT_ERR_OK;
        }
    }

    if((ret=reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LUTCAM_ENf, &value))!=RT_ERR_OK)
        return ret;
    l2CamEbl = value==1 ? ENABLED : DISABLED;

    if(l2CamEbl==ENABLED)
    {
        cam_index.index_type = L2_IN_CAM;
        for(cam_index.index=0; cam_index.index<cam_size[unit]; cam_index.index++)
        {
            if ((ret = _dal_longan_l2_getL2Entry(unit, &cam_index, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if(l2_entry.is_entry_valid &&
                l2_entry.entry_type==L2_UNICAST &&
                l2_entry.unicast.sablock==0 &&
                l2_entry.unicast.dablock==0 &&
                l2_entry.unicast.is_static==0 &&
                l2_entry.unicast.nh==0
                )
            {
                osal_memcpy(pL2_index, &cam_index, sizeof(dal_longan_l2_index_t));
                osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_longan_l2_entry_t));
                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_L2_ENTRY_NOTFOUND;
}

/* Function Name:
 *      dal_longan_l2Mapper_init
 * Description:
 *      Hook l2 module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook l2 module before calling any l2 APIs.
 */
int32
dal_longan_l2Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->l2_init = dal_longan_l2_init;
    pMapper->l2_flushLinkDownPortAddrEnable_get = dal_longan_l2_flushLinkDownPortAddrEnable_get;
    pMapper->l2_flushLinkDownPortAddrEnable_set = dal_longan_l2_flushLinkDownPortAddrEnable_set;
    pMapper->l2_ucastAddr_flush = dal_longan_l2_ucastAddr_flush;
    pMapper->l2_macLearningCnt_get = dal_longan_l2_macLearningCnt_get;
    pMapper->l2_limitLearningNum_get = dal_longan_l2_limitLearningNum_get;
    pMapper->l2_limitLearningNum_set = dal_longan_l2_limitLearningNum_set;
    pMapper->l2_limitLearningAction_get = dal_longan_l2_limitLearningAction_get;
    pMapper->l2_limitLearningAction_set = dal_longan_l2_limitLearningAction_set;
    pMapper->l2_fidLimitLearningEntry_get = dal_longan_l2_fidLimitLearningEntry_get;
    pMapper->l2_fidLimitLearningEntry_set = dal_longan_l2_fidLimitLearningEntry_set;
    pMapper->l2_fidLearningCnt_reset = dal_longan_l2_fidLearningCnt_reset;
    pMapper->l2_agingTime_get = dal_longan_l2_agingTime_get;
    pMapper->l2_agingTime_set = dal_longan_l2_agingTime_set;
    pMapper->l2_portAgingEnable_get = dal_longan_l2_portAgingEnable_get;
    pMapper->l2_portAgingEnable_set = dal_longan_l2_portAgingEnable_set;
    pMapper->l2_trkAgingEnable_get = dal_longan_l2_trkAgingEnable_get;
    pMapper->l2_trkAgingEnable_set = dal_longan_l2_trkAgingEnable_set;
    pMapper->l2_hashAlgo_get = dal_longan_l2_hashAlgo_get;
    pMapper->l2_hashAlgo_set = dal_longan_l2_hashAlgo_set;
    pMapper->l2_bucketHashAlgo_get = dal_longan_l2_bucketHashAlgo_get;
    pMapper->l2_bucketHashAlgo_set = dal_longan_l2_bucketHashAlgo_set;
    pMapper->l2_learningFullAction_get = dal_longan_l2_learningFullAction_get;
    pMapper->l2_learningFullAction_set = dal_longan_l2_learningFullAction_set;
    pMapper->l2_portNewMacOp_get = dal_longan_l2_portNewMacOp_get;
    pMapper->l2_portNewMacOp_set = dal_longan_l2_portNewMacOp_set;
    pMapper->l2_addr_init = dal_longan_l2_addr_init;
    pMapper->l2_addr_add = dal_longan_l2_addr_add;
    pMapper->l2_addr_del = dal_longan_l2_addr_del;
    pMapper->l2_addr_get = dal_longan_l2_addr_get;
    pMapper->l2_addr_set = dal_longan_l2_addr_set;
    pMapper->l2_addr_delAll = dal_longan_l2_addr_delAll;
    pMapper->l2_nextValidAddr_get = dal_longan_l2_nextValidAddr_get;
    pMapper->l2_mcastAddr_init = dal_longan_l2_mcastAddr_init;
    pMapper->l2_mcastAddr_add = dal_longan_l2_mcastAddr_add;
    pMapper->l2_mcastAddr_del = dal_longan_l2_mcastAddr_del;
    pMapper->l2_mcastAddr_get = dal_longan_l2_mcastAddr_get;
    pMapper->l2_mcastAddr_set = dal_longan_l2_mcastAddr_set;
    pMapper->l2_mcastAddr_addByIndex = dal_longan_l2_mcastAddr_addByIndex;
    pMapper->l2_nextValidMcastAddr_get = dal_longan_l2_nextValidMcastAddr_get;
    pMapper->l2_mcastFwdIndex_alloc = dal_longan_l2_mcastFwdIndex_alloc;
    pMapper->l2_mcastFwdIndex_free = dal_longan_l2_mcastFwdIndex_free;
    pMapper->l2_mcastFwdPortmaskEntry_get = dal_longan_l2_mcastFwdPortmaskEntry_get;
    pMapper->l2_mcastFwdPortmaskEntry_set = dal_longan_l2_mcastFwdPortmaskEntry_set;
    pMapper->l2_cpuMacAddr_add = dal_longan_l2_cpuMacAddr_add;
    pMapper->l2_cpuMacAddr_del = dal_longan_l2_cpuMacAddr_del;
    pMapper->l2_portMoveAction_get = dal_longan_l2_portMoveAction_get;
    pMapper->l2_portMoveAction_set = dal_longan_l2_portMoveAction_set;
    pMapper->l2_portMoveLearn_get = dal_longan_l2_portMoveLearn_get;
    pMapper->l2_portMoveLearn_set = dal_longan_l2_portMoveLearn_set;
    pMapper->l2_lookupMissFloodPortMask_get = dal_longan_l2_lookupMissFloodPortMask_get;
    pMapper->l2_lookupMissFloodPortMask_set = dal_longan_l2_lookupMissFloodPortMask_set;
    pMapper->l2_lookupMissFloodPortMask_add = dal_longan_l2_lookupMissFloodPortMask_add;
    pMapper->l2_lookupMissFloodPortMask_del = dal_longan_l2_lookupMissFloodPortMask_del;
    pMapper->l2_portLookupMissAction_get = dal_longan_l2_portLookupMissAction_get;
    pMapper->l2_portLookupMissAction_set = dal_longan_l2_portLookupMissAction_set;
    pMapper->l2_portUcastLookupMissAction_get = dal_longan_l2_portUcastLookupMissAction_get;
    pMapper->l2_portUcastLookupMissAction_set = dal_longan_l2_portUcastLookupMissAction_set;
    pMapper->l2_srcPortEgrFilterMask_get = dal_longan_l2_srcPortEgrFilterMask_get;
    pMapper->l2_srcPortEgrFilterMask_set = dal_longan_l2_srcPortEgrFilterMask_set;
    pMapper->l2_srcPortEgrFilterMask_add = dal_longan_l2_srcPortEgrFilterMask_add;
    pMapper->l2_srcPortEgrFilterMask_del = dal_longan_l2_srcPortEgrFilterMask_del;
    pMapper->l2_exceptionAddrAction_get = dal_longan_l2_exceptionAddrAction_get;
    pMapper->l2_exceptionAddrAction_set = dal_longan_l2_exceptionAddrAction_set;
    pMapper->l2_addrEntry_get = dal_longan_l2_addrEntry_get;
    pMapper->l2_conflictAddr_get = dal_longan_l2_conflictAddr_get;
    pMapper->l2_zeroSALearningEnable_get = dal_longan_l2_zeroSALearningEnable_get;
    pMapper->l2_zeroSALearningEnable_set = dal_longan_l2_zeroSALearningEnable_set;
    pMapper->l2_portDynamicPortMoveForbidEnable_get = dal_longan_l2_portDynamicPortMoveForbidEnable_get;
    pMapper->l2_portDynamicPortMoveForbidEnable_set = dal_longan_l2_portDynamicPortMoveForbidEnable_set;
    pMapper->l2_trkDynamicPortMoveForbidEnable_get = dal_longan_l2_trkDynamicPortMoveForbidEnable_get;
    pMapper->l2_trkDynamicPortMoveForbidEnable_set = dal_longan_l2_trkDynamicPortMoveForbidEnable_set;
    pMapper->l2_portMacFilterEnable_get = dal_longan_l2_portMacFilterEnable_get;
    pMapper->l2_portMacFilterEnable_set = dal_longan_l2_portMacFilterEnable_set;
    pMapper->l2_hwNextValidAddr_get = dal_longan_l2_hwNextValidAddr_get;
    pMapper->l2_stkLearningEnable_get = dal_longan_l2_stkLearningEnable_get;
    pMapper->l2_stkLearningEnable_set = dal_longan_l2_stkLearningEnable_set;
    pMapper->l2_stkKeepUcastEntryValid_get = dal_longan_l2_stkKeepUcastEntryValid_get;
    pMapper->l2_stkKeepUcastEntryValid_set = dal_longan_l2_stkKeepUcastEntryValid_set;
    pMapper->l2_portCtrl_get = dal_longan_l2_portCtrl_get;
    pMapper->l2_portCtrl_set = dal_longan_l2_portCtrl_set;
    pMapper->l2_status_get = dal_longan_l2_status_get;
    pMapper->l2_entryCnt_get = dal_longan_l2_entryCnt_get;
    pMapper->l2_hashIdx_get = dal_longan_l2_hashIdx_get;
    pMapper->l2_addr_delByMac = dal_longan_l2_addr_delByMac;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
dal_longan_l2_init(uint32 unit)
{
    int32 ret;
    uint32 index;
    rtk_portmask_t portmask;

    RT_INIT_REENTRY_CHK(l2_init[unit]);
    l2_init[unit] = INIT_NOT_COMPLETED;

#ifndef __BOOTLOADER__
  #ifdef CONFIG_SDK_DRIVER_L2NTFY_R9300
    /* register callback function if it's my unit id */
    if (unit == HWP_MY_UNIT_ID())
    {
        RT_ERR_CHK(r9300_l2ntfy_l2_cb_register(unit, _dal_longan_l2_portNewMacOp_set, _dal_longan_l2_agingTime_set), ret);
    }
  #endif /* CONFIG_SDK_DRIVER_L2NTFY_R9300 */
#endif
    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    /* create semaphore */
    l2_sem[unit] = osal_sem_mutex_create();
    if (0 == l2_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    l2_sem_2[unit] = osal_sem_mutex_create();
    if (0 == l2_sem_2[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* create semaphore */
    l2_pmsk_tbl_sem[unit] = osal_sem_mutex_create();
    if (0 == l2_pmsk_tbl_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "semaphore create failed");
        return RT_ERR_FAILED;
    }


    L2_SEM_LOCK(unit);

    if ((ret = table_size_get(unit, LONGAN_L2_UCt, &hashTable_size[unit])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = table_size_get(unit, LONGAN_L2_CAM_UCt, &cam_size[unit])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = table_size_get(unit, LONGAN_MC_PORTMASKt, &mcast_tableSize[unit])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    /* allocate memory for free multicast index */
    mcast_idx_pool[unit].pMcast_index_pool = (dal_longan_mcast_index_t *)osal_alloc(mcast_tableSize[unit] * sizeof(dal_longan_mcast_index_t));
    if (NULL==mcast_idx_pool[unit].pMcast_index_pool)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    mcast_idx_pool[unit].size_of_mcast_fwd_index = mcast_tableSize[unit];
    mcast_idx_pool[unit].free_index_head = 0;
    mcast_idx_pool[unit].free_entry_count = mcast_tableSize[unit];

    /* create free link-list for all entry, from 0 ~ max index - 2 */
    for (index = 0; index < (mcast_tableSize[unit] - 1); index++)
    {
        mcast_idx_pool[unit].pMcast_index_pool[index].next_index = index + 1;
        mcast_idx_pool[unit].pMcast_index_pool[index].ref_count = 0;
    }
    mcast_idx_pool[unit].pMcast_index_pool[mcast_tableSize[unit] - 1].next_index = END_OF_MCAST_IDX;
    mcast_idx_pool[unit].pMcast_index_pool[mcast_tableSize[unit] - 1].ref_count = 0;

    /* allocate memory for fwdTable shadow */
    mcast_fwdTable_shadow = osal_alloc(mcast_tableSize[unit] * sizeof(dal_longan_l2_mcastFwdShadow_t));
    if (NULL==mcast_fwdTable_shadow)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "memory allocate failed");
        osal_free(mcast_idx_pool[unit].pMcast_index_pool);
        mcast_idx_pool[unit].pMcast_index_pool = NULL;
        return RT_ERR_FAILED;
    }
    osal_memset(mcast_fwdTable_shadow, 0, mcast_tableSize[unit] * sizeof(dal_longan_l2_mcastFwdShadow_t));

    nh_used_cnt[unit] = osal_alloc((hashTable_size[unit] + cam_size[unit]) * sizeof(uint16));
    if (NULL==nh_used_cnt[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "memory allocate failed");
        osal_free(mcast_idx_pool[unit].pMcast_index_pool);
        mcast_idx_pool[unit].pMcast_index_pool = NULL;
        osal_free(mcast_fwdTable_shadow);
        mcast_fwdTable_shadow = NULL;
        return RT_ERR_FAILED;
    }
    osal_memset(nh_used_cnt[unit], 0, (hashTable_size[unit] + cam_size[unit]) * sizeof(uint16));

    L2_SEM_LOCK(unit);
    if((ret=reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_L2_HASH_ALGO_BLK0f, &blk0AlgoType[unit]))!=RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        osal_free(mcast_idx_pool[unit].pMcast_index_pool);
        mcast_idx_pool[unit].pMcast_index_pool = NULL;
        osal_free(mcast_fwdTable_shadow);
        mcast_fwdTable_shadow = NULL;
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if((ret=reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_L2_HASH_ALGO_BLK1f, &blk1AlgoType[unit]))!=RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        osal_free(mcast_idx_pool[unit].pMcast_index_pool);
        mcast_idx_pool[unit].pMcast_index_pool = NULL;
        osal_free(mcast_fwdTable_shadow);
        mcast_fwdTable_shadow = NULL;
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    for (index = 0; index < ARB_MAX; index++)
        gAging_time[index] = 489;

    /* set init flag to complete init */
    l2_init[unit] = INIT_COMPLETED;

    HWP_GET_ALL_PORTMASK(unit, portmask);
    if ((ret = dal_longan_l2_lookupMissFloodPortMask_set(unit, DLF_TYPE_UCAST, &portmask)) != RT_ERR_OK)
    {
        osal_free(mcast_idx_pool[unit].pMcast_index_pool);
        mcast_idx_pool[unit].pMcast_index_pool = NULL;
        osal_free(mcast_fwdTable_shadow);
        mcast_fwdTable_shadow = NULL;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "UC LUT miss portmask init failed/n");
        return ret;
    }
    if ((ret = dal_longan_l2_lookupMissFloodPortMask_set(unit, DLF_TYPE_BCAST, &portmask)) != RT_ERR_OK)
    {
        osal_free(mcast_idx_pool[unit].pMcast_index_pool);
        mcast_idx_pool[unit].pMcast_index_pool = NULL;
        osal_free(mcast_fwdTable_shadow);
        mcast_fwdTable_shadow = NULL;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "BC LUT miss portmask init failed/n");
        return ret;
    }

    return RT_ERR_OK;
}

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      dal_longan_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush dynamic mac entries corresponding to linkdown port configuration.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_longan_l2_flushLinkDownPortAddrEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LINK_DOWN_P_INVLDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush dynamic mac entries corresponding to linkdown port configuration.
 * Input:
 *      unit   - unit id
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_longan_l2_flushLinkDownPortAddrEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_L2_CTRLr, LONGAN_LINK_DOWN_P_INVLDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      unit    - unit id
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      The L2 unicast entries in L2 table which meet the specified criteria are flushed by HW.
 */
int32
dal_longan_l2_ucastAddr_flush(uint32 unit, rtk_l2_flushCfg_t *pConfig)
{
    int32   ret;
    uint32 value;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pConfig->act >= FLUSH_ACT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->flushByVid >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((ENABLED == pConfig->flushByVid && ((pConfig->vid < RTK_VLAN_ID_MIN) || (pConfig->vid > RTK_VLAN_ID_MAX))), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pConfig->flushByPort >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->portOrTrunk>= RTK_ENABLE_END), RT_ERR_INPUT);
    if(pConfig->flushByPort==ENABLED && pConfig->portOrTrunk==ENABLED)
    {
        RT_PARAM_CHK(pConfig->devID>=HAL_MAX_NUM_OF_DEV(unit), RT_ERR_DEV_ID);
        RT_PARAM_CHK(((HAL_UNIT_TO_DEV_ID(unit)==pConfig->devID) && !HWP_PORT_EXIST(unit, pConfig->port)), RT_ERR_PORT_ID);
    }
    else if(pConfig->flushByPort==ENABLED && pConfig->portOrTrunk==DISABLED)
        RT_PARAM_CHK((pConfig->trunk>=HAL_MAX_NUM_OF_TRUNK(unit)) , RT_ERR_TRUNK_ID);
    if(pConfig->act == FLUSH_ACT_REPLACE_ALL_UC || pConfig->act == FLUSH_ACT_REPLACE_DYNM_ONLY)
    {
        RT_PARAM_CHK((pConfig->replacePortOrTrunk>= RTK_ENABLE_END), RT_ERR_INPUT);
        RT_PARAM_CHK((ENABLED == pConfig->replacePortOrTrunk && (pConfig->replacing_devID>=HAL_MAX_NUM_OF_DEV(unit))), RT_ERR_DEV_ID);
        RT_PARAM_CHK((ENABLED == pConfig->replacePortOrTrunk && ((HAL_UNIT_TO_DEV_ID(unit)==pConfig->replacing_devID) && !HWP_PORT_EXIST(unit, pConfig->replacingPort))), RT_ERR_PORT_ID);
        RT_PARAM_CHK((DISABLED == pConfig->replacePortOrTrunk && pConfig->replacingTrk>=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    }
    if(pConfig->act == FLUSH_ACT_CLEAR_AGG_VID)
    {
        RT_PARAM_CHK(((pConfig->flushFlag & RTK_L2_FLUSH_BY_AGGVID) && (pConfig->agg_vid > RTK_VLAN_ID_MAX)), RT_ERR_INPUT);
    }


    switch(pConfig->act)
    {
        case FLUSH_ACT_FLUSH_ALL_UC:
        case FLUSH_ACT_REPLACE_ALL_UC:
            value = 0x0;break;
        case FLUSH_ACT_FLUSH_DYNM_ONLY:
        case FLUSH_ACT_REPLACE_DYNM_ONLY:
            value = 0x1;break;
        case FLUSH_ACT_CLEAR_NEXTHOP:
            value = 0x2;break;
        case FLUSH_ACT_CLEAR_AGG_VID:
            value = 0x3;break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_ENTRY_TYPEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if(pConfig->act  == FLUSH_ACT_REPLACE_DYNM_ONLY || pConfig->act  == FLUSH_ACT_REPLACE_ALL_UC)
        value = 1;
    else
        value = 0;
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_ACTf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }


    if(pConfig->flushByVid==ENABLED)
        value = 1;
    else
        value = 0;
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_FVID_CMPf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if((pConfig->act == FLUSH_ACT_CLEAR_AGG_VID || pConfig->act == FLUSH_ACT_FLUSH_ALL_UC || pConfig->act == FLUSH_ACT_FLUSH_DYNM_ONLY) &&
       (pConfig->flushFlag & RTK_L2_FLUSH_BY_AGGVID))
        value = 1;
    else
        value = 0;
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_AGG_VID_CMPf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }


    if(pConfig->flushByPort==ENABLED)
        value = 1;
    else
        value = 0;
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_PORT_CMPf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }



    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_FVIDf, &pConfig->vid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_AGG_VIDf, &pConfig->agg_vid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    value = (pConfig->portOrTrunk==ENABLED) ? ((pConfig->devID<<6) | pConfig->port) : ((1<<10)|pConfig->trunk);
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_PORT_IDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    value = (pConfig->replacePortOrTrunk==ENABLED) ? ((pConfig->replacing_devID <<6) | pConfig->replacingPort) : ((1<<10)|pConfig->replacingTrk);
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_REPLACING_PORT_IDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    value = 1;
    if ((ret = reg_field_write(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_STSf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    while(1)
    {
        if((ret = reg_field_read(unit, LONGAN_L2_TBL_FLUSH_CTRLr, LONGAN_STSf, &value)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if(value==0)
            break;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

/* Function Name:
 *      dal_longan_l2_macLearningCnt_get
 * Description:
 *      Get number of learned MAC addresses of specified type.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      pNum             - number of learned MAC addresses
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 */
int32
dal_longan_l2_macLearningCnt_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLimitCnt), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pLimitCnt->portTrkCnt.id), RT_ERR_PORT_ID);break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.id >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);break;
        case L2_MAC_LIMIT_FID:
            RT_PARAM_CHK((pLimitCnt->fidCnt.entryId >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);break;
        default:
            break;
    }

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if((ret = reg_field_read(unit, LONGAN_L2_LRN_CONSTRT_CNTr, LONGAN_LRN_CNTf, &pLimitCnt->glbCnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_PORT:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_PORT_CONSTRT_CNTr, pLimitCnt->portTrkCnt.id, REG_ARRAY_INDEX_NONE, LONGAN_LRN_CNTf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_TRUNK:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_TRK_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, pLimitCnt->portTrkCnt.id, LONGAN_LRN_CNTf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_FID:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, pLimitCnt->fidCnt.entryId, LONGAN_LRN_CNTf, &pLimitCnt->fidCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_limitLearningNum_get
 * Description:
 *      Get the mac limit learning counts of specified device.
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pLimitCnt - pointer ro mac limit parameters
 * Output:
 *      pLimitCnt - pointer of mac limit learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
dal_longan_l2_limitLearningNum_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLimitCnt), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pLimitCnt->portTrkCnt.id), RT_ERR_PORT_ID);break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.id >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);break;
        case L2_MAC_LIMIT_FID:
            RT_PARAM_CHK((pLimitCnt->fidCnt.entryId >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);break;
        default:
            break;
    }

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if((ret = reg_field_read(unit, LONGAN_L2_LRN_CONSTRT_CTRLr, LONGAN_CONSTRT_NUMf, &pLimitCnt->glbCnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (pLimitCnt->glbCnt == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
                pLimitCnt->glbCnt = L2_MAC_CST_DISABLE;
            break;
        case L2_MAC_LIMIT_PORT:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_PORT_CONSTRT_CTRLr, pLimitCnt->portTrkCnt.id, REG_ARRAY_INDEX_NONE, LONGAN_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (pLimitCnt->portTrkCnt.cnt == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
                pLimitCnt->portTrkCnt.cnt = L2_MAC_CST_DISABLE;
            break;
        case L2_MAC_LIMIT_TRUNK:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pLimitCnt->portTrkCnt.id, LONGAN_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (pLimitCnt->portTrkCnt.cnt == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
                pLimitCnt->portTrkCnt.cnt = L2_MAC_CST_DISABLE;
            break;
        case L2_MAC_LIMIT_FID:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, pLimitCnt->fidCnt.entryId, LONGAN_CONSTRT_NUMf, &pLimitCnt->fidCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (pLimitCnt->fidCnt.cnt == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
                pLimitCnt->fidCnt.cnt = L2_MAC_CST_DISABLE;
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_l2_limitLearningNum_set
 * Description:
 *      Set the mac limit learning counts of specified device.
 * Input:
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pLimitCnt - pointer ro mac limit parameters and mac limit learning counts
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
dal_longan_l2_limitLearningNum_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLimitCnt), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if (pLimitCnt->glbCnt == L2_MAC_CST_DISABLE)
                pLimitCnt->glbCnt = HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit);
            RT_PARAM_CHK((pLimitCnt->glbCnt > HAL_L2_LEARN_LIMIT_CNT_MAX(unit)) && (pLimitCnt->glbCnt != HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit)),RT_ERR_LIMITED_L2ENTRY_NUM);
            break;
        case L2_MAC_LIMIT_PORT:
            if (pLimitCnt->portTrkCnt.cnt == L2_MAC_CST_DISABLE)
                pLimitCnt->portTrkCnt.cnt = HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit);
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pLimitCnt->portTrkCnt.id), RT_ERR_PORT_ID);
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.cnt > HAL_L2_LEARN_LIMIT_CNT_MAX(unit)) && (pLimitCnt->portTrkCnt.cnt != HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit)), RT_ERR_LIMITED_L2ENTRY_NUM);
            break;
        case L2_MAC_LIMIT_TRUNK:
            if (pLimitCnt->portTrkCnt.cnt == L2_MAC_CST_DISABLE)
                pLimitCnt->portTrkCnt.cnt = HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit);
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.id >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.cnt > HAL_L2_LEARN_LIMIT_CNT_MAX(unit)) && (pLimitCnt->portTrkCnt.cnt != HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit)), RT_ERR_LIMITED_L2ENTRY_NUM);
            break;
        case L2_MAC_LIMIT_FID:
            if (pLimitCnt->fidCnt.cnt == L2_MAC_CST_DISABLE)
                pLimitCnt->fidCnt.cnt = HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit);
            RT_PARAM_CHK((pLimitCnt->fidCnt.entryId >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);
            RT_PARAM_CHK((pLimitCnt->fidCnt.cnt > HAL_L2_LEARN_LIMIT_CNT_MAX(unit)) && (pLimitCnt->fidCnt.cnt != HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit)), RT_ERR_LIMITED_L2ENTRY_NUM);
            break;
        default:
            break;
    }

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if((ret = reg_field_write(unit, LONGAN_L2_LRN_CONSTRT_CTRLr, LONGAN_CONSTRT_NUMf, &pLimitCnt->glbCnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_PORT:
            if((ret = reg_array_field_write(unit, LONGAN_L2_LRN_PORT_CONSTRT_CTRLr, pLimitCnt->portTrkCnt.id, REG_ARRAY_INDEX_NONE, LONGAN_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_TRUNK:
            if((ret = reg_array_field_write(unit, LONGAN_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pLimitCnt->portTrkCnt.id, LONGAN_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_FID:
            if((ret = reg_array_field_write(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, pLimitCnt->fidCnt.entryId, LONGAN_CONSTRT_NUMf, &pLimitCnt->fidCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_limitLearningAction_get
 * Description:
 *      Get the mac limit action of specified device.
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters
 * Output:
 *      pAction - pointer of mac limit action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
dal_longan_l2_limitLearningAction_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    int32   ret;
    uint32 value;
    rtk_action_t *pAction_temp = NULL;


    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pAction->portTrkAct.id), RT_ERR_PORT_ID);break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pAction->portTrkAct.id >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);break;
        default:
            break;
    }

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if((ret = reg_field_read(unit, LONGAN_L2_LRN_CONSTRT_CTRLr, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            pAction_temp = &pAction->glbAct;
            break;
        case L2_MAC_LIMIT_PORT:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_PORT_CONSTRT_CTRLr, pAction->portTrkAct.id, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            pAction_temp = &pAction->portTrkAct.act;
            break;
        case L2_MAC_LIMIT_TRUNK:
            if((ret = reg_array_field_read(unit, LONGAN_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pAction->portTrkAct.id, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            pAction_temp = &pAction->portTrkAct.act;
            break;
        case L2_MAC_LIMIT_FID:
            if((ret = reg_field_read(unit, LONGAN_L2_VLAN_CONSTRT_CTRLr, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            pAction_temp = &pAction->fidAct.act;
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    switch(value)
    {
        case 0:
            *pAction_temp = ACTION_FORWARD;break;
        case 1:
            *pAction_temp = ACTION_DROP;break;
        case 2:
            *pAction_temp = ACTION_TRAP2CPU;break;
        case 3:
            *pAction_temp = ACTION_COPY2CPU;break;
        case 4:
            *pAction_temp = ACTION_TRAP2MASTERCPU;break;
        case 5:
            *pAction_temp = ACTION_COPY2MASTERCPU;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_limitLearningAction_set
 * Description:
 *      Set the mac limit action of specified device.
 * Input:
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters and mac limit action
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
dal_longan_l2_limitLearningAction_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    int32   ret;
    uint32 value;
    rtk_action_t action = 0;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pAction->portTrkAct.id), RT_ERR_PORT_ID);break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pAction->portTrkAct.id >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);break;
        default:
            break;
    }

    switch(type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            action = pAction->glbAct;break;
        case L2_MAC_LIMIT_PORT:
        case L2_MAC_LIMIT_TRUNK:
            action= pAction->portTrkAct.act;break;
        case L2_MAC_LIMIT_FID:
            action = pAction->fidAct.act;break;
        default:
            break;
    }

    switch(action)
    {
        case ACTION_FORWARD:
            value=0;break;
        case ACTION_DROP:
            value=1;break;
        case ACTION_TRAP2CPU:
            value=2;break;
        case ACTION_COPY2CPU:
            value=3;break;
        case ACTION_TRAP2MASTERCPU:
            value=4;break;
        case ACTION_COPY2MASTERCPU:
            value=5;break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if((ret = reg_field_write(unit, LONGAN_L2_LRN_CONSTRT_CTRLr, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_PORT:
            if((ret = reg_array_field_write(unit, LONGAN_L2_LRN_PORT_CONSTRT_CTRLr, pAction->portTrkAct.id, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_TRUNK:
            if((ret = reg_array_field_write(unit, LONGAN_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pAction->portTrkAct.id, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_FID:
            if((ret = reg_field_write(unit, LONGAN_L2_VLAN_CONSTRT_CTRLr, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_fidLimitLearningEntry_get
 * Description:
 *      Get the specified VLAN MAC limit configuration.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of VLAN MAC limit entry
 * Output:
 *      pFidMacLimitEntry - pointer to MAC limit configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 *      (2) Forwarding action is as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
dal_longan_l2_fidLimitLearningEntry_get(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    int32   ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, fid_macLimit_idx=%d", unit, fid_macLimit_idx);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((fid_macLimit_idx >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pFidMacLimitEntry), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, LONGAN_FVIDf, &pFidMacLimitEntry->fid)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    if ((ret = reg_array_field_read(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, LONGAN_PORT_IDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, LONGAN_CONSTRT_NUMf, &pFidMacLimitEntry->maxNum)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    if(value & (1<<6))
    {
        pFidMacLimitEntry->portOrTrunk = DISABLED;
        pFidMacLimitEntry->trunk = value & 0x3f;
    }
    else
    {
        pFidMacLimitEntry->portOrTrunk = ENABLED;
        pFidMacLimitEntry->port = value & 0x3f;
    }

    if (pFidMacLimitEntry->maxNum == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
        pFidMacLimitEntry->maxNum = L2_MAC_CST_DISABLE;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_fidLimitLearningEntry_set
 * Description:
 *      Set the specified VLAN MAC limit configuration.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of VLAN MAC limit entry
 *      pFidMacLimitEntry - MAC limit configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 *      (2) Forwarding action is as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 *      (3) This API will flush all MACs learnt on specified VLAN before this entry takes effect.
 *          If the fid of an entry changes from one to another, this API will flush MACs learnt on these two VLANs.
 */
int32
dal_longan_l2_fidLimitLearningEntry_set(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    int32   ret;
    uint32 value=0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, fid_macLimit_idx=%d", unit, fid_macLimit_idx);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((fid_macLimit_idx >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pFidMacLimitEntry), RT_ERR_NULL_POINTER);
    if (pFidMacLimitEntry->maxNum == L2_MAC_CST_DISABLE)
        pFidMacLimitEntry->maxNum = HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit);
    RT_PARAM_CHK((pFidMacLimitEntry->maxNum > HAL_L2_LEARN_LIMIT_CNT_MAX(unit)) && (pFidMacLimitEntry->maxNum != HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit)) , RT_ERR_LIMITED_L2ENTRY_NUM);
    RT_PARAM_CHK((pFidMacLimitEntry->fid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pFidMacLimitEntry->portOrTrunk >= RTK_ENABLE_END) , RT_ERR_INPUT);
    RT_PARAM_CHK(((pFidMacLimitEntry->portOrTrunk == ENABLED) && !(HWP_PORT_EXIST(unit, pFidMacLimitEntry->port) || pFidMacLimitEntry->port==63)) , RT_ERR_PORT_ID);
    RT_PARAM_CHK(((pFidMacLimitEntry->portOrTrunk == DISABLED) && (pFidMacLimitEntry->trunk >= HAL_MAX_NUM_OF_TRUNK(unit))) , RT_ERR_TRUNK_ID);


    if(pFidMacLimitEntry->portOrTrunk == ENABLED)
        value = (0<<6) |pFidMacLimitEntry->port;
    else
        value = (1<<6) |pFidMacLimitEntry->trunk;

    L2_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, LONGAN_FVIDf, &pFidMacLimitEntry->fid)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    if ((ret = reg_array_field_write(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, LONGAN_PORT_IDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, LONGAN_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, LONGAN_CONSTRT_NUMf, &pFidMacLimitEntry->maxNum)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_fidLearningCnt_reset
 * Description:
 *      Reset number of learned MAC addresses on specified VLAN MAC limit entry.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 */
int32
dal_longan_l2_fidLearningCnt_reset(uint32 unit, uint32 fid_macLimit_idx)
{
    int32   ret;
    uint32 value=0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, fid_macLimit_idx=%d", unit, fid_macLimit_idx);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((fid_macLimit_idx >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    L2_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, LONGAN_LRN_CNTf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */



/* Function Name:
 *      _dal_longan_l2_agingTime_set
 * Description:
 *      Set the aging time of suspending entry from the specified device.
 * Input:
 *      unit       - unit id
 *      type       - age time type
 *      aging_time - aging time
 *      arb_id     - id for arbiter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The unit is second.
 */
int32
_dal_longan_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time, uint32 arb_id)
{
    int32   ret;
    uint32  value = 0;
    uint32  ageUnit = 0, i;
    uint32  aging_time_tmp;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_AGE_TIME_END), RT_ERR_INPUT);


    aging_time_tmp      = aging_time;
    gAging_time[arb_id] = aging_time;
    if (aging_time != 0) //only when no one says "not learn", can learning be enabled
    {
        for (i = 0; i < ARB_MAX; i++)
        {
            if (gAging_time[i] == 0)
            {
                aging_time_tmp = 0;  //not aging
                break;
            }
        }
    }

    if (aging_time_tmp != 0)
        aging_time_tmp = gAging_time[ARB_NORMAL_API];

    if (type == L2_AGE_TIME_NORMAL)
    {
        ageUnit = (uint32)((aging_time_tmp * 10 + 6)/ 7);

        if (ageUnit > 0x1fffff)
            return RT_ERR_INPUT;

        L2_SEM_LOCK(unit);
        if ((ret = reg_field_write(unit, LONGAN_L2_AGE_CTRLr, LONGAN_AGE_UNITf, &ageUnit)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);
    }
    else
    {
        L2_SEM_LOCK(unit);
        if ((ret = reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_AGE_UNITf, &ageUnit)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);

        if (aging_time_tmp == 0 || ageUnit == 0)
            value = 1;
        else if ((uint32)((aging_time_tmp * 10 + 6) / ageUnit) == 0)
            value = 1;
        else
            value = (uint32)((aging_time_tmp * 10 + 6) / ageUnit);

        if (value > 7)
            return RT_ERR_INPUT;

        L2_SEM_LOCK(unit);
        if ((ret = reg_field_write(unit, LONGAN_L2_AGE_CTRLr, LONGAN_SUS_AGE_MAXf, &value)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_agingTime_get
 * Description:
 *      Get the aging time of from the specified device.
 * Input:
 *      unit        - unit id
 *      type       - age time type
 * Output:
 *      pAging_time - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit is second.
 */
int32
dal_longan_l2_agingTime_get(uint32 unit, rtk_l2_ageTimeType_t type, uint32 *pAging_time)
{
    int32   ret;
    uint32  value = 0;
    uint32  suspendMax = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_AGE_TIME_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAging_time), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_AGE_UNITf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_SUS_AGE_MAXf, &suspendMax)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    if (type == L2_AGE_TIME_NORMAL)
        *pAging_time = (uint32)(value*7/ 10);
    else
    {
        if(value==0)
            *pAging_time = 0;
        else if((value*suspendMax)<10)
            *pAging_time = 1;
        else
            *pAging_time = (uint32)((value*suspendMax)/10);
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pAging_time=%ld", *pAging_time);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_l2_agingTime_set
 * Description:
 *      Set the aging time of suspending entry from the specified device.
 * Input:
 *      unit       - unit id
 *      type       - age time type
 *      aging_time - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The unit is second.
 */
int32
dal_longan_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_AGE_TIME_END), RT_ERR_INPUT);

    ret = _dal_longan_l2_agingTime_set(unit, type, aging_time, ARB_NORMAL_API);

    return ret;
}


/* Function Name:
 *      dal_longan_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out state configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_portAgingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_L2_PORT_AGE_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_AGE_ENf, &enable)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out state configuration of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_l2_portAgingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_L2_PORT_AGE_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_AGE_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_trkAgingEnable_get
 * Description:
 *      Get the dynamic address aging out state configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      trunk   - trunk id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_trkAgingEnable_get(uint32 unit, rtk_trk_t trunk, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, trunk=%d", unit, trunk);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((trunk >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_L2_TRK_AGE_CTRLr, REG_ARRAY_INDEX_NONE, trunk, LONGAN_AGE_ENf, &enable)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_trkAgingEnable_set
 * Description:
 *      Set the dynamic address aging out state configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      trunk   - trunk id
 *      enable  - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_l2_trkAgingEnable_set(uint32 unit, rtk_trk_t trunk, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, trunk=%d, enable=%d", unit, trunk, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((trunk >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_L2_TRK_AGE_CTRLr, REG_ARRAY_INDEX_NONE, trunk, LONGAN_AGE_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_hashAlgo_get
 * Description:
 *      Get hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none.
 */
int32
dal_longan_l2_hashAlgo_get(uint32 unit, uint32 *pHash_algo)
{
    return dal_longan_l2_bucketHashAlgo_get(unit, 0, pHash_algo);
}

/* Function Name:
 *      dal_longan_l2_hashAlgo_set
 * Description:
 *      Set hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none.
 */
int32
dal_longan_l2_hashAlgo_set(uint32 unit, uint32 hash_algo)
{
    int32 ret;
    ret = dal_longan_l2_bucketHashAlgo_set(unit, 0, hash_algo);
    if(ret!= RT_ERR_OK)
        return ret;
    else
        return dal_longan_l2_bucketHashAlgo_set(unit, 1, hash_algo);
}


/* Function Name:
 *      dal_longan_l2_bucketHashAlgo_get
 * Description:
 *      Get bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      bucket can be 0 to 1.
 */
int32
dal_longan_l2_bucketHashAlgo_get(uint32 unit, uint32 bucket, uint32 *pHash_algo)
{
    int32   ret;
    uint32  value = 0;
    uint32 regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHash_algo), RT_ERR_NULL_POINTER);

    if(bucket==0)
        regField = LONGAN_L2_HASH_ALGO_BLK0f;
    else if(bucket==1)
        regField = LONGAN_L2_HASH_ALGO_BLK1f;
    else
        return RT_ERR_INPUT;

    L2_SEM_LOCK(unit);

    if((ret = reg_field_read(unit, LONGAN_L2_CTRLr, regField, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    *pHash_algo = value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pHash_algo=%d", *pHash_algo);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_bucketHashAlgo_set
 * Description:
 *      Set bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 *      hash_algo   - hash algorithm of layer2 switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      The valid bucket is 0 to 1, and hash_algo is 0 and 1
 */
int32
dal_longan_l2_bucketHashAlgo_set(uint32 unit, uint32 bucket, uint32 hash_algo)
{
    int32   ret;
    uint32 regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d  hash_algo=%d", unit, hash_algo);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    if(bucket==0)
        regField = LONGAN_L2_HASH_ALGO_BLK0f;
    else if(bucket==1)
        regField = LONGAN_L2_HASH_ALGO_BLK1f;
    else
        return RT_ERR_INPUT;
    RT_PARAM_CHK((hash_algo >= HAL_MAX_NUM_OF_L2_HASH_ALGO(unit)), RT_ERR_INPUT);

    L2_SEM_LOCK(unit);

    if((ret = reg_field_write(unit, LONGAN_L2_CTRLr, regField, &hash_algo)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    if(bucket==0)
        blk0AlgoType[unit] = hash_algo;
    else
        blk1AlgoType[unit] = hash_algo;

    return RT_ERR_OK;
}

/* Module Name    : L2      */
/* Sub-module Name: Unicast */

/* Function Name:
 *      dal_longan_l2_learningFullAction_get
 * Description:
 *      Get the forwarding action which is taken when SA learning full.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_learningFullAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  act;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if((ret = reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_HASH_FULL_ACTf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* translate register value to action */
    switch(act)
    {
        case 0:
            *pAction = ACTION_FORWARD;break;
        case 1:
            *pAction = ACTION_DROP;break;
        case 2:
            *pAction = ACTION_TRAP2CPU;break;
        case 3:
            *pAction = ACTION_COPY2CPU;break;
        case 4:
            *pAction = ACTION_TRAP2MASTERCPU;break;
        case 5:
            *pAction = ACTION_COPY2MASTERCPU;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pAction=%d", *pAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_learningFullAction_set
 * Description:
 *      Set the forwarding action which is taken when SA learning full.
 * Input:
 *      unit   - unit id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_l2_learningFullAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  act;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    switch(action)
    {
        case ACTION_FORWARD:
            act=0;break;
        case ACTION_DROP:
            act=1;break;
        case ACTION_TRAP2CPU:
            act=2;break;
        case ACTION_COPY2CPU:
            act=3;break;
        case ACTION_TRAP2MASTERCPU:
            act=4;break;
        case ACTION_COPY2MASTERCPU:
            act=5;break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if((ret = reg_field_write(unit, LONGAN_L2_CTRLr, LONGAN_HASH_FULL_ACTf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_l2_portNewMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pMode      - pointer to learning mode
 *      pAct       - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
_dal_longan_l2_portNewMacOp_get(uint32 unit, rtk_port_t port, uint32 *pMode, uint32 *pAct)
{
    int32   ret;
    uint32  mode = 0, act = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAct), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(unit, LONGAN_L2_PORT_SALRNr, port, REG_ARRAY_INDEX_NONE, LONGAN_SALRNf, &mode)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }
    if ((ret = reg_array_field_read(unit, LONGAN_L2_PORT_NEW_SA_FWDr, port, REG_ARRAY_INDEX_NONE, LONGAN_NEW_SA_FWDf, &act)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    *pMode  = mode;
    *pAct   = act;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMode=%d  pAct:%d", *pMode, *pAct);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_l2_portNewMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      mode      - learning mode value
 *      act       - forwarding action value
 *      arb_id    - id for arbiter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. This is a arbiter API, only when all caller are agree enable learning, can SALRN be enabled
 *         and uses the setting storing in the l2_lrn[ARB_NORMAL_API] to restore
 *      2. Only dal_longan_l2_portNewMacOp_set (ARB_NORMAL_API) can set NEW_SA_FWD action
 */
int32
_dal_longan_l2_portNewMacOp_set(uint32 unit, rtk_port_t port, uint32 mode, uint32 act, uint32 arb_id)
{
    int32   ret;
    uint32  i, lrn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, mode=%d, act=%d", unit, port, mode, act);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    L2_SEM_LOCK_2(unit);

    lrn                     = mode;
    l2_lrn[arb_id][port]    = mode;

    if (mode != 0x2) //only when no one says "not learn", can learning be enabled
    {
        for (i = 0; i < ARB_MAX; i++)
        {
            if (l2_lrn[i][port] == 0x2)
            {
                lrn = 0x2;  //not learn
                break;
            }
        }
    }

    if (lrn != 0x2)
        lrn = l2_lrn[ARB_NORMAL_API][port];

    if ((ret = reg_array_field_write(unit, LONGAN_L2_PORT_SALRNr, port, REG_ARRAY_INDEX_NONE, LONGAN_SALRNf, &lrn)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK_2(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    if (arb_id == ARB_NORMAL_API)
    {
        if ((ret = reg_array_field_write(unit, LONGAN_L2_PORT_NEW_SA_FWDr, port, REG_ARRAY_INDEX_NONE, LONGAN_NEW_SA_FWDf, &act)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK_2(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
            return ret;
        }
    }

    L2_SEM_UNLOCK_2(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portNewMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *          - ACTION_TRAP2MASTERCPU
 *          - ACTION_COPY2MASTERCPU
 *
 *      (2) Learning mode is as following
 *          - HARDWARE_LEARNING
 *          - SOFTWARE_LEARNING
 *          - NOT_LEARNING
 */
int32
dal_longan_l2_portNewMacOp_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction)
{
    int32   ret;
    uint32  mode, act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLrnMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);


    if ((ret = _dal_longan_l2_portNewMacOp_get(unit, port, &mode, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    switch (mode)
    {
        case 0:
            *pLrnMode = HARDWARE_LEARNING;
            break;
        case 1:
            *pLrnMode = SOFTWARE_LEARNING;
            break;
        case 2:
            *pLrnMode = NOT_LEARNING;
            break;
        default:
            return RT_ERR_FAILED;
    }

    switch(act)
    {
        case 0:
            *pFwdAction = ACTION_FORWARD;break;
        case 1:
            *pFwdAction = ACTION_DROP;break;
        case 2:
            *pFwdAction = ACTION_TRAP2CPU;break;
        case 3:
            *pFwdAction = ACTION_COPY2CPU;break;
        case 4:
            *pFwdAction = ACTION_TRAP2MASTERCPU;break;
        case 5:
            *pFwdAction = ACTION_COPY2MASTERCPU;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pLrnMode=%d  pFwdAction:%d", *pLrnMode, *pFwdAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portNewMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *          - ACTION_TRAP2MASTERCPU
 *          - ACTION_COPY2MASTERCPU
 *
 *      (2) Learning mode is as following
 *          - HARDWARE_LEARNING
 *          - SOFTWARE_LEARNING
 *          - NOT_LEARNING
 */
int32
dal_longan_l2_portNewMacOp_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    int32   ret;
    uint32  mode, act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, lrnMode=%d, fwdAction=%d", unit, port, lrnMode, fwdAction);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(lrnMode >= LEARNING_MODE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(fwdAction >= ACTION_END, RT_ERR_INPUT);

    switch (lrnMode)
    {
        case HARDWARE_LEARNING:
            mode = 0;
            break;
        case SOFTWARE_LEARNING:
            mode = 1;
            break;
        case NOT_LEARNING:
            mode = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch(fwdAction)
    {
        case ACTION_FORWARD:
            act=0;break;
        case ACTION_DROP:
            act=1;break;
        case ACTION_TRAP2CPU:
            act=2;break;
        case ACTION_COPY2CPU:
            act=3;break;
        case ACTION_TRAP2MASTERCPU:
            act=4;break;
        case ACTION_COPY2MASTERCPU:
            act=5;break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    if ((ret = _dal_longan_l2_portNewMacOp_set(unit, port, mode, act, ARB_NORMAL_API)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      dal_longan_l2_addr_init
 * Description:
 *      Initialize content of buffer of L2 entry.
 *      Will fill vid ,MAC address and reset other field of L2 entry.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 *      pMac     - MAC address
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
dal_longan_l2_addr_init(
    uint32              unit,
    rtk_vlan_t          vid,
    rtk_mac_t           *pMac,
    rtk_l2_ucastAddr_t  *pL2_addr)
{
    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pL2_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(pL2_addr, 0, sizeof(rtk_l2_ucastAddr_t));
    pL2_addr->vid = vid;
    pL2_addr->age = 7;
    osal_memcpy(&pL2_addr->mac.octet[0], &pMac->octet[0], sizeof(rtk_mac_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      unit     - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API can supported add by port or trunk-id view both.
 *          - If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->trk_gid is valid and pL2_addr->port is invalid.
 *          - If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 *      (2) When adding a entry with RTK_L2_UCAST_FLAG_SA_BLOCK or RTK_L2_UCAST_FLAG_SA_SECURE attribute,
 *          pL2_addr->port should be set to 0x3f in 8390 and 9310, and 0x1f in 8380 and 9300 if the specified MAC address
 *          would come from nowhere.
 */
int32
dal_longan_l2_addr_add(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    int32               ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;
    uint32 age_valid;
    uint32 myDevId;

    uint32 suspend_age;

    uint32 age_val;

    uint32 oriNh = 0;
    uint32 ori_macIdx = 0;
    rtk_port_t              lPort;

    //RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pL2_addr=%x", unit, pL2_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    if (pL2_addr->port == L2_PORT_DONT_CARE)
        lPort = PORT_DONT_CARE_9300;
    else if (pL2_addr->port == L2_PORT_BLACK_HOLE)
        lPort = PORT_BLACK_HOLE_9300;
    else
        lPort = pL2_addr->port;

    /* parameter check */
    RT_PARAM_CHK(NULL == pL2_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    if(pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
        RT_PARAM_CHK((pL2_addr->trk_gid>=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    else
    {
        if( ((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)==0) && (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC))
             RT_PARAM_CHK((((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)) && (lPort != PORT_BLACK_HOLE_9300)), RT_ERR_PORT_ID);
        else if(((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)) && !(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC))
            RT_PARAM_CHK((((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)) && lPort!=PORT_DONT_CARE_9300), RT_ERR_PORT_ID);
        else
            RT_PARAM_CHK(((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)), RT_ERR_PORT_ID);
    }
    RT_PARAM_CHK((((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)) && (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND)) , RT_ERR_INPUT);
    RT_PARAM_CHK((pL2_addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) && pL2_addr->route_idx >= HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((!(pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) && pL2_addr->agg_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pL2_addr->devID >= HAL_MAX_NUM_OF_DEV(unit)), RT_ERR_DEV_ID);
    RT_PARAM_CHK((pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET) && (pL2_addr->age > 7), RT_ERR_INPUT);

    L2_SEM_LOCK(unit);
    if((ret=reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_KEEP_AGE_OUT_ENTRY_VALIDf, &age_valid)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if((ret=reg_field_read(unit, LONGAN_STK_GLB_CTRLr, LONGAN_MY_DEV_IDf, &myDevId)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if((ret=reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_SUS_AGE_MAXf, &suspend_age)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    if(pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET)
        age_val = pL2_addr->age;
    else
        age_val = 7;

    if(age_val==0 && age_valid==0)
    {
        if  (
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)==0 &&
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) == 0 &&
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) == 0 &&
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) == 0
            )
            return RT_ERR_INPUT;
    }
    else if(age_val==0 && age_valid==1)
    {
        if((pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)==0)
        {
            if  (
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)==0 &&
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) == 0 &&
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) == 0 &&
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) == 0
                )
            {
                if(pL2_addr->devID==myDevId)
                    return RT_ERR_INPUT;
            }
        }
    }

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &(pL2_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (ret == RT_ERR_OK && l2_entry.is_entry_exist==1 && (pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_NOT_UPDATE_EXIST))
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
        if(index_entry.index_type == L2_IN_HASH)
            pL2_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
        else
            pL2_addr->l2_idx = hashTable_size[unit] + index_entry.index;
        return RT_ERR_L2_ENTRY_EXIST;
    }
    else if (ret == RT_ERR_L2_NO_EMPTY_ENTRY && (pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC) != 0)
    {
        ret = _dal_longan_l2_getFirstDynamicEntry(unit, &l2_entry, &index_entry);
        if (ret != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_NO_EMPTY_ENTRY;
        }

        osal_memcpy(&pL2_addr->macInfo.mac, &l2_entry.unicast.mac, ETHER_ADDR_LEN);
        pL2_addr->macInfo.devID      = l2_entry.unicast.devID;
        pL2_addr->macInfo.port      = l2_entry.unicast.port;
        pL2_addr->macInfo.vid       = l2_entry.unicast.fid;
        pL2_addr->macInfo.flags     = l2_entry.unicast.is_trk ? RTK_L2_UCAST_FLAG_TRUNK_PORT : 0;
        pL2_addr->macInfo.state     = l2_entry.unicast.suspending ? RTK_L2_UCAST_STATE_SUSPEND : 0;
        pL2_addr->macInfo.trk_gid   = l2_entry.unicast.trunk;
        pL2_addr->macInfo.age       = l2_entry.unicast.aging;
    }
    else if (RT_ERR_OK != ret)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (l2_entry.is_entry_exist)
    {
        oriNh       = l2_entry.unicast.nh;
        ori_macIdx  = l2_entry.unicast.mac_idx;
    }

    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &(pL2_addr->mac), sizeof(rtk_mac_t));
    if(pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
    {
        l2_entry.unicast.is_trk = 1;
        l2_entry.unicast.devID = (age_val==0 && age_valid) ? 1 : 0;
        l2_entry.unicast.trunk = pL2_addr->trk_gid;
    }
    else
    {
        l2_entry.unicast.devID = pL2_addr->devID;
        l2_entry.unicast.port = lPort;
    }

    l2_entry.unicast.aging = age_val;

    l2_entry.unicast.sablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.dablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.is_static  = (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC) ? TRUE: FALSE;
    l2_entry.unicast.suspending = (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND) ? TRUE: FALSE;
    l2_entry.unicast.nh         = (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? TRUE: oriNh;
    if (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)
        l2_entry.unicast.mac_idx    = pL2_addr->route_idx;
    else if(oriNh)
        l2_entry.unicast.mac_idx    = ori_macIdx;
    else
        l2_entry.unicast.agg_vid    = pL2_addr->agg_vid;

    if(l2_entry.unicast.aging==0 && l2_entry.unicast.is_static==0)
    {
        if(age_valid==0 || (l2_entry.unicast.is_trk==0 && l2_entry.unicast.devID==myDevId))
        {
            l2_entry.unicast.is_trk     = 0;
            l2_entry.unicast.devID    = 0;
            l2_entry.unicast.port       = PORT_DONT_CARE_9300;
            l2_entry.unicast.suspending = 0;
            l2_entry.unicast.agg_vid    = 0;
        }
        else
        {
            l2_entry.unicast.devID = l2_entry.unicast.is_trk ? 1 : l2_entry.unicast.devID;
        }
    }

    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);
    if(index_entry.index_type == L2_IN_HASH)
        pL2_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
    else
        pL2_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      VID is same as FID in IVL mode.
 */
int32
dal_longan_l2_addr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32 ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    /* search exist or free entry */
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = vid;
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    /* fill content */
    if (l2_entry.unicast.nh)
    {
        l2_entry.unicast.aging      = 0;
        l2_entry.unicast.is_trk     = 0;
        l2_entry.unicast.devID    = 0;
        l2_entry.unicast.port       = PORT_DONT_CARE_9300;
        l2_entry.unicast.sablock    = 0;
        l2_entry.unicast.dablock    = 0;
        l2_entry.unicast.is_static  = 0;
        l2_entry.unicast.suspending = 0;
        l2_entry.unicast.agg_vid    = 0;
    }
    else
    {
        osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
        l2_entry.entry_type = L2_UNICAST;
    }



    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      unit     - unit id
 * Output:
 *      pL2_addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) VID is same as FID in IVL mode.
 *      (2) The pL2_addr.vid and pL2_addr.mac is input key
 *      (3) The pL2_addr.port, pL2_addr.auth, pL2_addr.sa_block,
 *          pL2_addr.da_block and pL2_addr.is_static is output.
 *      (4) If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag,
 *          mean the pL2_addr->trk_gid is valid and pL2_addr->port is valid also.
 *          The pL2_addr->port value is the represent port of pL2_addr->trk_gid.
 *      (5) If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag,
 *          mean the pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 */
int32
dal_longan_l2_addr_get(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    int32 ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d", unit, pL2_addr->vid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pL2_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    RT_PARAM_CHK((pL2_addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMac=%x-%x-%x-%x-%x-%x",
           pL2_addr->mac.octet[0], pL2_addr->mac.octet[1], pL2_addr->mac.octet[2],
           pL2_addr->mac.octet[3], pL2_addr->mac.octet[4], pL2_addr->mac.octet[5]);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type     = L2_UNICAST;
    l2_entry.unicast.fid    = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &pL2_addr->mac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        /* Return Fail if not found */
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* fill content */
    osal_memcpy(&pL2_addr->mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
    pL2_addr->vid       = l2_entry.unicast.fid;
    pL2_addr->flags     = 0;    /* Clear data */
    pL2_addr->state     = 0;    /* Clear data */
    if(l2_entry.unicast.is_trk)
    {
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
        pL2_addr->devID = l2_entry.unicast.devID;   /*when trunk, devID indicate if entry is notified*/
        pL2_addr->trk_gid = l2_entry.unicast.trunk;
    }
    else
    {
        pL2_addr->devID = l2_entry.unicast.devID;
        pL2_addr->port = l2_entry.unicast.port;
    }

    if(l2_entry.unicast.sablock)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
    if(l2_entry.unicast.dablock)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
    if(l2_entry.unicast.is_static)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_STATIC;
    if(l2_entry.unicast.nh)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_NEXTHOP;
    if(l2_entry.unicast.suspending)
        pL2_addr->state |= RTK_L2_UCAST_STATE_SUSPEND;
    if(l2_entry.unicast.aging == 0)
        pL2_addr->isAged = TRUE;
    else
        pL2_addr->isAged = FALSE;
    pL2_addr->age = l2_entry.unicast.aging;
    pL2_addr->route_idx     = l2_entry.unicast.mac_idx;
    pL2_addr->agg_vid       = l2_entry.unicast.agg_vid;

    if(index_entry.index_type == L2_IN_HASH)
        pL2_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
    else
        pL2_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "vid=%d, port=%d, sa_block=%d, da_block=%d, is_static=%d, nh=%d, suspend=%d, agg_vid=:%d",
        pL2_addr->vid, pL2_addr->port, l2_entry.unicast.sablock, l2_entry.unicast.dablock, l2_entry.unicast.is_static,
        l2_entry.unicast.nh, l2_entry.unicast.suspending, l2_entry.unicast.agg_vid);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_addr_set
 * Description:
 *      Update content of L2 entry.
 * Input:
 *      unit     - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API can support adding entry by port or trunk-id view both.
 *          - If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->trk_gid is valid and pL2_addr->port is invalid.
 *          - If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 */
int32
dal_longan_l2_addr_set(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    int32               ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;
    uint32 age_valid;
    uint32 myDevId;

    uint32 suspend_age;

    uint32 age_val;

    uint32 oriNh = 0;
    uint32 ori_macIdx = 0;
    rtk_port_t              lPort;

    //RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pL2_addr=%x", unit, pL2_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    if (pL2_addr->port == L2_PORT_DONT_CARE)
        lPort = PORT_DONT_CARE_9300;
    else if (pL2_addr->port == L2_PORT_BLACK_HOLE)
        lPort = PORT_BLACK_HOLE_9300;
    else
        lPort = pL2_addr->port;

    /* parameter check */
    RT_PARAM_CHK(NULL == pL2_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    if(pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
        RT_PARAM_CHK((pL2_addr->trk_gid>=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    else
    {
        if( ((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)==0) && (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC) )
             RT_PARAM_CHK((((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)) && (lPort != PORT_BLACK_HOLE_9300)), RT_ERR_PORT_ID);
        else if(((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)) && !(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC))
            RT_PARAM_CHK((((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)) && lPort!=PORT_DONT_CARE_9300), RT_ERR_PORT_ID);
        else
            RT_PARAM_CHK(((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)), RT_ERR_PORT_ID);
    }
    RT_PARAM_CHK((((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)) && (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND)) , RT_ERR_INPUT);
    RT_PARAM_CHK((pL2_addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) && pL2_addr->route_idx >= HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((!(pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) && pL2_addr->agg_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pL2_addr->devID >= HAL_MAX_NUM_OF_DEV(unit)), RT_ERR_DEV_ID);
    RT_PARAM_CHK((pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET) && (pL2_addr->age > 7), RT_ERR_INPUT);

    L2_SEM_LOCK(unit);
    if((ret=reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_KEEP_AGE_OUT_ENTRY_VALIDf, &age_valid)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if((ret=reg_field_read(unit, LONGAN_STK_GLB_CTRLr, LONGAN_MY_DEV_IDf, &myDevId)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if((ret=reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_SUS_AGE_MAXf, &suspend_age)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    if(pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET)
        age_val = pL2_addr->age;
    else
        age_val = 7;

    if(age_val==0 && age_valid==0)
    {
        if  (
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)==0 &&
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) == 0 &&
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) == 0 &&
                (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) == 0
            )
            return RT_ERR_INPUT;
    }
    else if(age_val==0 && age_valid==1)
    {
        if((pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)==0)
        {
            if  (
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)==0 &&
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) == 0 &&
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) == 0 &&
                    (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) == 0
                )
            {
                if(pL2_addr->devID==myDevId)
                    return RT_ERR_INPUT;
            }
        }
    }

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &(pL2_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry);
    if (RT_ERR_OK != ret)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (l2_entry.is_entry_exist)
    {
        oriNh       = l2_entry.unicast.nh;
        ori_macIdx  = l2_entry.unicast.mac_idx;
    }

    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &(pL2_addr->mac), sizeof(rtk_mac_t));
    if(pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
    {
        l2_entry.unicast.is_trk = 1;
        l2_entry.unicast.devID = (age_val==0 && age_valid) ? 1 : 0;
        l2_entry.unicast.trunk = pL2_addr->trk_gid;
    }
    else
    {
        l2_entry.unicast.devID = pL2_addr->devID;
        l2_entry.unicast.port = lPort;
    }

    l2_entry.unicast.aging = age_val;

    l2_entry.unicast.sablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.dablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.is_static  = (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC) ? TRUE: FALSE;
    l2_entry.unicast.suspending = (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND) ? TRUE: FALSE;
    l2_entry.unicast.nh         = (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? TRUE: oriNh;
    if (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)
        l2_entry.unicast.mac_idx    = pL2_addr->route_idx;
    else if(oriNh)
        l2_entry.unicast.mac_idx    = ori_macIdx;
    else
        l2_entry.unicast.agg_vid    = pL2_addr->agg_vid;

    if(l2_entry.unicast.aging==0 && l2_entry.unicast.is_static==0)
    {
        if(age_valid==0 || (l2_entry.unicast.is_trk==0 && l2_entry.unicast.devID==myDevId))
        {
            l2_entry.unicast.is_trk     = 0;
            l2_entry.unicast.devID    = 0;
            l2_entry.unicast.port       = PORT_DONT_CARE_9300;
            l2_entry.unicast.suspending = 0;
            l2_entry.unicast.agg_vid    = 0;
        }
        else
        {
            l2_entry.unicast.devID = l2_entry.unicast.is_trk ? 1 : l2_entry.unicast.devID;
        }
    }

    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);
    if(index_entry.index_type == L2_IN_HASH)
        pL2_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
    else
        pL2_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    L2_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_longan_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry from the specified device.
 * Input:
 *      unit           - unit id
 *      include_static - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_l2_addr_delAll(uint32 unit, uint32 include_static)
{
    int32 ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;
    uint32 index;
    rtk_enable_t l2CamEbl;
    uint32 value;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* search exist or free entry */
    l2_entry.entry_type = L2_UNICAST;
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));

    L2_SEM_LOCK(unit);

    index_entry.index_type = L2_IN_HASH;
    for(index=0; index<hashTable_size[unit]; index++)
    {
        index_entry.index = (index>>2);
        index_entry.hashdepth = index&0x3;

        if ((ret = _dal_longan_l2_getL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
            return ret;
        }

        if(l2_entry.entry_type==L2_UNICAST && l2_entry.is_entry_valid==1 && (include_static==1 || l2_entry.unicast.is_static==0))
        {
            if (l2_entry.unicast.nh)
            {
                l2_entry.unicast.aging      = 0;
                l2_entry.unicast.is_trk     = 0;
                l2_entry.unicast.devID    = 0;
                l2_entry.unicast.port       = PORT_DONT_CARE_9300;
                l2_entry.unicast.sablock    = 0;
                l2_entry.unicast.dablock    = 0;
                l2_entry.unicast.is_static  = 0;
                l2_entry.unicast.suspending = 0;
                l2_entry.unicast.agg_vid    = 0;
            }
            else
                osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
            if ((ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                return ret;
            }
        }
    }

    if((ret=reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LUTCAM_ENf, &value))!=RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    l2CamEbl = value==1 ? ENABLED : DISABLED;

    if(l2CamEbl==ENABLED)
    {
        index_entry.index_type = L2_IN_CAM;
        for(index=0; index<cam_size[unit]; index++)
        {
            index_entry.index = index;

            if ((ret = _dal_longan_l2_getL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                return ret;
            }

            if(l2_entry.entry_type==L2_UNICAST && l2_entry.is_entry_valid==1 && (include_static==1 || l2_entry.unicast.is_static==0))
            {
                if (l2_entry.unicast.nh)
                {
                    l2_entry.unicast.aging      = 0;
                    l2_entry.unicast.is_trk     = 0;
                    l2_entry.unicast.devID    = 0;
                    l2_entry.unicast.port       = PORT_DONT_CARE_9300;
                    l2_entry.unicast.sablock    = 0;
                    l2_entry.unicast.dablock    = 0;
                    l2_entry.unicast.is_static  = 0;
                    l2_entry.unicast.suspending = 0;
                    l2_entry.unicast.agg_vid    = 0;
                }
                else
                    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));

                if ((ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
                {
                    L2_SEM_UNLOCK(unit);
                    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                    return ret;
                }
            }
        }
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      unit           - unit id
 *      pScan_idx      - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2_addr       - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          returned next valid L2 unicast address which based on index order of l2 table.
 *      (2) Input -1 for getting the first entry of l2 table.
 *      (3) The pScan_idx is both the input and output argument.
 */
int32
dal_longan_l2_nextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    uint32              include_static,
    rtk_l2_ucastAddr_t  *pL2_addr)
{
    int32 ret;
    rtk_l2_entry_t          entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);
    RT_PARAM_CHK(NULL == pScan_idx, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2_addr), RT_ERR_NULL_POINTER);

    osal_memset(&entry, 0, sizeof(entry));

    if(*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 1;

    if (include_static)
        ret = dal_longan_l2_hwNextValidAddr_get(unit, pScan_idx, L2_NEXT_VALID_TYPE_UC, &entry);
    else
        ret = dal_longan_l2_hwNextValidAddr_get(unit, pScan_idx, L2_NEXT_VALID_TYPE_AUTO_UC, &entry);

    osal_memcpy(pL2_addr, &entry.unicast, sizeof(rtk_l2_ucastAddr_t));
    return ret;
}

/* Function Name:
 *      _dal_longan_l2_nexthop_add
 * Description:
 *      Add L2 nexthop entry to ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_longan_l2_ucastNhAddr_t  - nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will add a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just set the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will add an entry with nexthop bit set to 1 and the portId be 0x3f.
 */
int32
_dal_longan_l2_nexthop_add(uint32 unit, dal_longan_l2_ucastNhAddr_t *pNexthop_addr)
{
    int32                   ret;
    dal_longan_l2_entry_t    l2_entry;
    dal_longan_l2_index_t    index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pNexthop_addr=%x", unit, pNexthop_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pNexthop_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNexthop_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);

    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pNexthop_addr->fid;
    osal_memcpy(&l2_entry.unicast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (ret == RT_ERR_OK && l2_entry.is_entry_exist == 0)
    {
        l2_entry.unicast.port = PORT_DONT_CARE_9300;
        l2_entry.unicast.aging = 0; /*PORT_DONT_CARE_9300 should set aging = 0*/
    }
    else if (ret == RT_ERR_L2_NO_EMPTY_ENTRY && (pNexthop_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC) != 0)
    {
        ret = _dal_longan_l2_getFirstDynamicEntry(unit, &l2_entry, &index_entry);
        if (ret != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_NO_EMPTY_ENTRY;
        }

        osal_memcpy(&pNexthop_addr->macInfo.mac, &l2_entry.unicast.mac, ETHER_ADDR_LEN);
        pNexthop_addr->macInfo.devID      = l2_entry.unicast.devID;
        pNexthop_addr->macInfo.port      = l2_entry.unicast.port;;
        pNexthop_addr->macInfo.vid       = l2_entry.unicast.fid;
        pNexthop_addr->macInfo.flags     = l2_entry.unicast.is_trk ? RTK_L2_UCAST_FLAG_TRUNK_PORT : 0;
        pNexthop_addr->macInfo.trk_gid   = l2_entry.unicast.trunk;
        pNexthop_addr->macInfo.age       = l2_entry.unicast.aging;

        l2_entry.entry_type = L2_UNICAST;
        l2_entry.unicast.fid = pNexthop_addr->fid;
        osal_memcpy(&l2_entry.unicast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));
        l2_entry.unicast.port = PORT_DONT_CARE_9300;
        l2_entry.unicast.aging = 0; /*PORT_DONT_CARE_9300 should set aging = 0*/
    }
    else if (RT_ERR_OK != ret)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    l2_entry.unicast.valid = TRUE;
    l2_entry.unicast.nh = 1;
    l2_entry.unicast.mac_idx = pNexthop_addr->mac_idx;
    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);
    if (index_entry.index_type == L2_IN_HASH)
        pNexthop_addr->l2_idx = (index_entry.index << 2) | index_entry.hashdepth;
    else
        pNexthop_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    nh_used_cnt[unit][pNexthop_addr->l2_idx]++;

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_l2_nexthop_del
 * Description:
 *      Delete L2 nexthop entry from ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_longan_l2_ucastNhAddr_t  - nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will delete a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just clear the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will set this entry invalid directly.
 */
int32
_dal_longan_l2_nexthop_del(uint32 unit, dal_longan_l2_ucastNhAddr_t *pNexthop_addr)
{
    int32   ret;
    uint32  index;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d, pMac=%x-%x-%x-%x-%x-%x",
                                        unit, pNexthop_addr->fid,
                                        pNexthop_addr->mac.octet[0], pNexthop_addr->mac.octet[1], pNexthop_addr->mac.octet[2],
                                        pNexthop_addr->mac.octet[3], pNexthop_addr->mac.octet[4], pNexthop_addr->mac.octet[5]);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pNexthop_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNexthop_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    RT_PARAM_CHK((pNexthop_addr->fid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);


    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pNexthop_addr->fid;
    osal_memcpy(&l2_entry.unicast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    /* fill content */
    if (l2_entry.unicast.nh)
    {
        /* retrun to caller */
        pNexthop_addr->mac_idx = l2_entry.unicast.mac_idx;

        if (index_entry.index_type == L2_IN_HASH)
            index = (index_entry.index << 2) | index_entry.hashdepth;
        else
            index = hashTable_size[unit] + index_entry.index;

        if (nh_used_cnt[unit][index] > 0)
            nh_used_cnt[unit][index]--;

        if (nh_used_cnt[unit][index] == 0)
        {
            l2_entry.unicast.nh = 0;
            if (l2_entry.unicast.port == PORT_DONT_CARE_9300 &&
                l2_entry.unicast.sablock == 0 &&
                l2_entry.unicast.dablock == 0 &&
                l2_entry.unicast.is_static == 0)
            {
                osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
                l2_entry.entry_type = L2_UNICAST;
            }
            if ((ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                return ret;
            }
        }
    }
    else
    {
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_NEXTHOP_NOT_EXIST;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      dal_longan_l2_mcastAddr_init
 * Description:
 *      Initialize content of buffer of L2 multicast entry.
 *      Will fill vid ,MAC address and reset other field of L2 multicast entry.
 * Input:
 *      unit        - unit id
 *      vid         - vlan id
 *      pMac        - MAC address
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
int32
dal_longan_l2_mcastAddr_init(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac, rtk_l2_mcastAddr_t *pMcast_addr)
{
    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pMcast_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(pMcast_addr, 0, sizeof(rtk_l2_mcastAddr_t));
    pMcast_addr->rvid = vid;
    osal_memcpy(&pMcast_addr->mac.octet[0], &pMac->octet[0], sizeof(rtk_mac_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Don't need to configure pMcast_addr->fwdIndex because driver automatically allocates a
 *          free portmask entry index and return it back to pMcast_addr->fwdIndex.
 *      (2) pMcast_addr->portmask is used to configure the allocated portmask entry.
 */
int32
dal_longan_l2_mcastAddr_add(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    pMcast_addr->fwdIndex = -1; /* for automatically allocate */

    ret = dal_longan_l2_mcastAddr_addByIndex(unit, pMcast_addr);

    return ret;
}


/* Function Name:
 *      dal_longan_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - multicast mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_VLAN_VID       - invalid vlan id
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_MAC            - invalid mac address
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      The corresponding portmask entry is cleared only if its reference count reaches 0.
 */
int32
dal_longan_l2_mcastAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32 ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;
    int32 fwdTableIdx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    /* search exist or free entry */
    l2_entry.entry_type = L2_MULTICAST;
    l2_entry.l2mcast.fid = vid;
    osal_memcpy(&l2_entry.l2mcast.mac, pMac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    if ((ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    if (l2_entry.l2mcast.nh)
    {
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_NEXTHOP_EXIST;
    }

    fwdTableIdx = l2_entry.l2mcast.index;

    /* fill content */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type = L2_MULTICAST;
    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);

    if(ret == RT_ERR_OK)
    {
        _dal_longan_l2_mcastFwdIndex_free(unit, fwdTableIdx);
    }

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_mcastAddr_get
 * Description:
 *      Get L2 multicast entry based on specified VID and MAC address.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_mcastAddr_get(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    int32 ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d", unit, pMcast_addr->rvid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMcast_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcast_addr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pMcast_addr->rvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.fid    = pMcast_addr->rvid;
    osal_memcpy(&l2_entry.l2mcast.mac, &pMcast_addr->mac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        /* Return Fail if not found */
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    /* fill content */
    osal_memcpy(&pMcast_addr->mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
    pMcast_addr->rvid       = l2_entry.l2mcast.fid;
    pMcast_addr->fwdIndex = l2_entry.l2mcast.index;
    pMcast_addr->nextHop = l2_entry.l2mcast.nh;
    pMcast_addr->mac_idx = l2_entry.l2mcast.mac_idx;

    if ((ret = _dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pMcast_addr->fwdIndex, &pMcast_addr->portmask)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);


    if(index_entry.index_type == L2_IN_HASH)
        pMcast_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
    else
        pMcast_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_mcastAddr_set
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      unit        - unit id
 * Output:
 *      pMcast_addr - pointer to L2 multicast entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_mcastAddr_set(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    int32               ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;
    int32 mcastFwdIdx=-1;
    rtk_portmask_t portmask;
    int32 old_mcastFwdIdx=-1;

    //RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pMcast_addr=%x", unit, pMcast_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMcast_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcast_addr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pMcast_addr->rvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pMcast_addr->nextHop && pMcast_addr->mac_idx >= HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pMcast_addr->portmask), RT_ERR_PORT_MASK);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type = L2_MULTICAST;
    l2_entry.l2mcast.fid = pMcast_addr->rvid;
    osal_memcpy(&l2_entry.l2mcast.mac, &(pMcast_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry);
    if (RT_ERR_OK != ret)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
    if((ret = _dal_longan_l2_mcastFwdPortmaskEntry_get(unit, (int32)l2_entry.l2mcast.index, &portmask)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }


    if (RTK_PORTMASK_COMPARE(portmask, pMcast_addr->portmask) != 0)
    {
        if((ret = _dal_longan_l2_mcastFwdIndex_alloc(unit, &mcastFwdIdx)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        old_mcastFwdIdx = l2_entry.l2mcast.index;
        l2_entry.l2mcast.index = mcastFwdIdx;

        if((ret = _dal_longan_l2_mcastFwdPortmaskEntry_set(unit, mcastFwdIdx, &pMcast_addr->portmask)) != RT_ERR_OK)
        {
            _dal_longan_l2_mcastFwdIndex_free(unit, mcastFwdIdx);
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);
    if(index_entry.index_type == L2_IN_HASH)
        pMcast_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
    else
        pMcast_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    if (RTK_PORTMASK_COMPARE(portmask, pMcast_addr->portmask) != 0)
        _dal_longan_l2_mcastFwdIndex_free(unit, (RT_ERR_OK==ret)?old_mcastFwdIdx:mcastFwdIdx);

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_mcastAddr_addByIndex
 * Description:
 *      Add L2 multicast entry to ASIC with specific forward portmask index.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Specific pMcast_addr->fwdIndex.
 *      (2) pMcast_addr->portmask is used to configure the allocated portmask entry.
 */
int32
dal_longan_l2_mcastAddr_addByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    int32               ret, fwdIndex;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;
    int32 mcastFwdIdx=-1;
    int32 oldFwdIdx=-1;

    //RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pMcast_addr=%x", unit, pMcast_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMcast_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcast_addr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pMcast_addr->rvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(pMcast_addr->fwdIndex >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK((pMcast_addr->nextHop && pMcast_addr->mac_idx >= HAL_MAX_NUM_OF_ROUTE_HOST_ADDR(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, &pMcast_addr->portmask), RT_ERR_PORT_MASK);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type = L2_MULTICAST;
    l2_entry.l2mcast.fid = pMcast_addr->rvid;
    osal_memcpy(&l2_entry.l2mcast.mac, &(pMcast_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (ret == RT_ERR_OK && l2_entry.is_entry_exist==1 && (pMcast_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_NOT_UPDATE_EXIST))
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
        if(index_entry.index_type == L2_IN_HASH)
            pMcast_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
        else
            pMcast_addr->l2_idx = hashTable_size[unit] + index_entry.index;
        return RT_ERR_L2_ENTRY_EXIST;
    }
    else if (ret == RT_ERR_L2_NO_EMPTY_ENTRY && (pMcast_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC) != 0)
    {
        ret = _dal_longan_l2_getFirstDynamicEntry(unit, &l2_entry, &index_entry);
        if (ret != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_NO_EMPTY_ENTRY;
        }

        osal_memcpy(&pMcast_addr->macInfo.mac, &l2_entry.unicast.mac, ETHER_ADDR_LEN);
        pMcast_addr->macInfo.devID      = l2_entry.unicast.devID;
        pMcast_addr->macInfo.port      = l2_entry.unicast.port;
        pMcast_addr->macInfo.vid       = l2_entry.unicast.fid;
        pMcast_addr->macInfo.flags     = l2_entry.unicast.is_trk ? RTK_L2_UCAST_FLAG_TRUNK_PORT : 0;
        pMcast_addr->macInfo.state     = l2_entry.unicast.suspending ? RTK_L2_UCAST_STATE_SUSPEND : 0;
        pMcast_addr->macInfo.trk_gid   = l2_entry.unicast.trunk;
        pMcast_addr->macInfo.age       = l2_entry.unicast.aging;
    }
    else if (RT_ERR_OK != ret)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (ret == RT_ERR_OK && l2_entry.is_entry_exist==1)
    {
        oldFwdIdx = (int32)(l2_entry.l2mcast.index);
    }

    fwdIndex = pMcast_addr->fwdIndex;
    if((ret = _dal_longan_l2_mcastFwdIndex_alloc(unit, &pMcast_addr->fwdIndex)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    mcastFwdIdx = pMcast_addr->fwdIndex;

    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type = L2_MULTICAST;
    l2_entry.l2mcast.fid = pMcast_addr->rvid;
    osal_memcpy(&l2_entry.l2mcast.mac, &(pMcast_addr->mac), sizeof(rtk_mac_t));
    l2_entry.l2mcast.index = mcastFwdIdx;
    l2_entry.l2mcast.mac_idx = pMcast_addr->mac_idx;

    if (fwdIndex < 0)
    {
        if((ret = _dal_longan_l2_mcastFwdPortmaskEntry_set(unit, mcastFwdIdx, &pMcast_addr->portmask)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    if(index_entry.index_type == L2_IN_HASH)
        pMcast_addr->l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
    else
        pMcast_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    l2_entry.l2mcast.nh = nh_used_cnt[unit][pMcast_addr->l2_idx] ? 1 : 0;

    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);


    if(ret !=RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if(oldFwdIdx != -1)
    {
        _dal_longan_l2_mcastFwdIndex_free(unit, oldFwdIdx);
    }

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of l2 table to get next.
 * Output:
 *      pMcast_addr  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          returned next valid L2 multicast address which based on index order of l2 table.
 *      (2) Input -1 for getting the first entry of l2 table.
 *      (3) The pScan_idx is both the input and output argument.
 */
int32
dal_longan_l2_nextValidMcastAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_mcastAddr_t  *pMcast_addr)
{
    int32 ret;
    rtk_l2_entry_t          entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);
    RT_PARAM_CHK(NULL == pScan_idx, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMcast_addr), RT_ERR_NULL_POINTER);

    osal_memset(&entry, 0, sizeof(entry));

    if(*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 1;

    osal_memset(&entry, 0, sizeof(entry));
    ret = dal_longan_l2_hwNextValidAddr_get(unit, pScan_idx, L2_NEXT_VALID_TYPE_MC, &entry);

    osal_memcpy(pMcast_addr, &entry.l2mcast, sizeof(rtk_l2_mcastAddr_t));
    return ret;
}

/* Function Name:
 *      _dal_longan_l2_mcastNexthop_add
 * Description:
 *      Add L2 nexthop entry to ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_longan_l2_mcastNhAddr_t  - nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will add a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just set the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
_dal_longan_l2_mcastNexthop_add(uint32 unit, dal_longan_l2_mcastNhAddr_t *pNexthop_addr)
{
    int32                   ret, index;
    dal_longan_l2_entry_t    l2_entry;
    dal_longan_l2_index_t    index_entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pNexthop_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNexthop_addr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pNexthop_addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d, pMac=%x-%x-%x-%x-%x-%x",
        unit, pNexthop_addr->vid,
        pNexthop_addr->mac.octet[0], pNexthop_addr->mac.octet[1], pNexthop_addr->mac.octet[2],
        pNexthop_addr->mac.octet[3], pNexthop_addr->mac.octet[4], pNexthop_addr->mac.octet[5]);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    osal_memcpy(&l2_entry.l2mcast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.fid    = pNexthop_addr->vid;
    l2_entry.l2mcast.mac_idx    = pNexthop_addr->mac_idx;

    L2_SEM_LOCK(unit);

    ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (ret == RT_ERR_OK && l2_entry.is_entry_exist == 0)
    {
        ;
    }
    else if (ret == RT_ERR_L2_NO_EMPTY_ENTRY && (pNexthop_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC) != 0)
    {
        ret = _dal_longan_l2_getFirstDynamicEntry(unit, &l2_entry, &index_entry);
        if (ret != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_NO_EMPTY_ENTRY;
        }

        osal_memcpy(&pNexthop_addr->macInfo.mac, &l2_entry.unicast.mac, ETHER_ADDR_LEN);
        pNexthop_addr->macInfo.devID      = l2_entry.unicast.devID;
        pNexthop_addr->macInfo.port      = l2_entry.unicast.port;;
        pNexthop_addr->macInfo.vid       = l2_entry.unicast.fid;
        pNexthop_addr->macInfo.flags     = l2_entry.unicast.is_trk ? RTK_L2_UCAST_FLAG_TRUNK_PORT : 0;
        pNexthop_addr->macInfo.trk_gid   = l2_entry.unicast.trunk;
        pNexthop_addr->macInfo.age       = l2_entry.unicast.aging;

        l2_entry.is_entry_valid = 1;
        l2_entry.entry_type = L2_MULTICAST;
        l2_entry.l2mcast.fid = pNexthop_addr->vid;
        osal_memcpy(&l2_entry.l2mcast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));
        l2_entry.l2mcast.mac_idx    = pNexthop_addr->mac_idx;
    }
    else if (RT_ERR_OK != ret)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (FALSE == l2_entry.is_entry_exist)
    {
        index = -1;
        if ((ret = _dal_longan_l2_mcastFwdIndex_alloc(unit, &index)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            return ret;
        }
        l2_entry.l2mcast.index = index;
    }

    l2_entry.l2mcast.nh = 1;
    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);
    if (index_entry.index_type == L2_IN_HASH)
        pNexthop_addr->l2_idx = (index_entry.index << 2) | index_entry.hashdepth;
    else
        pNexthop_addr->l2_idx = hashTable_size[unit] + index_entry.index;

    nh_used_cnt[unit][pNexthop_addr->l2_idx]++;

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_longan_l2_mcastNexthop_del
 * Description:
 *      Delete L2 multicast nexthop entry from ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_longan_l2_mcastNhAddr_t  - nexthop entry
 * Output:
 *      dal_longan_l2_mcastNhAddr_t  - nexthop entry mac_index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will delete a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just clear the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
_dal_longan_l2_mcastNexthop_del(uint32 unit, dal_longan_l2_mcastNhAddr_t *pNexthop_addr)
{
    int32                   ret;
    uint32                  index;
    dal_longan_l2_entry_t    l2_entry;
    dal_longan_l2_index_t    index_entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pNexthop_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNexthop_addr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pNexthop_addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d, pMac=%x-%x-%x-%x-%x-%x",
        unit, pNexthop_addr->vid,
        pNexthop_addr->mac.octet[0], pNexthop_addr->mac.octet[1], pNexthop_addr->mac.octet[2],
        pNexthop_addr->mac.octet[3], pNexthop_addr->mac.octet[4], pNexthop_addr->mac.octet[5]);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    osal_memcpy(&l2_entry.l2mcast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.fid    = pNexthop_addr->vid;


    L2_SEM_LOCK(unit);

    ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry);
    if (RT_ERR_OK != ret)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (l2_entry.l2mcast.nh)
    {
        /* retrun to caller */
        pNexthop_addr->mac_idx = l2_entry.l2mcast.mac_idx;

        if (index_entry.index_type == L2_IN_HASH)
            index = (index_entry.index << 2) | index_entry.hashdepth;
        else
            index = hashTable_size[unit] + index_entry.index;

        if (nh_used_cnt[unit][index] > 0)
            nh_used_cnt[unit][index]--;
        if (nh_used_cnt[unit][index] == 0)
        {
            l2_entry.l2mcast.nh = 0;
            ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);
        }
    }
    else
    {
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_NEXTHOP_NOT_EXIST;
    }

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Module Name    : L2                         */
/* Sub-module Name: Multicast forwarding table */

/* Function Name:
 *      dal_longan_l2_mcastFwdIndex_alloc
 * Description:
 *      Allocate index for multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      pFwdIndex - pointer to index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX       - invalid index of multicast forwarding entry
 *      RT_ERR_L2_MCAST_FWD_ENTRY_EXIST - Mcast forwarding entry already exist
 *      RT_ERR_L2_INDEXTBL_FULL         - L2 index table is full
 * Note:
 *      (1) If pFwdIndex is larger than or equal to 0, will use pFwdIndex as multicast index.
 *      (2) If pFwdIndex is smaller than 0, will allocate a free index and return it.
 *      (3) The reference count corresponds to the pFwdIndex is increased after a successfully allocation.
 */
int32
dal_longan_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pFwdIndex, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(*pFwdIndex >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);

    L2_SEM_LOCK(unit);
    if ((ret = _dal_longan_l2_mcastFwdIndex_alloc(unit, pFwdIndex)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_mcastFwdIndex_free
 * Description:
 *      Free index for multicast forwarding entry
 * Input:
 *      unit  - unit id
 *      index - index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                      - invalid unit id
 *      RT_ERR_NOT_INIT                     - The module is not initial
 *      RT_ERR_L2_MULTI_FWD_INDEX           - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_MCAST_FWD_ENTRY_NOT_EXIST - index of forwarding entry is not exist
 * Note:
 *      (1) The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 *      (2) The reference count corresponds to the pFwdIndex is decreased after a successfully free.
 */
int32
dal_longan_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(index >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK(index < 0, RT_ERR_L2_MULTI_FWD_INDEX);

    L2_SEM_LOCK(unit);
    if ((ret = _dal_longan_l2_mcastFwdIndex_free(unit, index)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_mcastFwdPortmaskEntry_get
 * Description:
 *      Get portmask of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      index      - index of multicast forwarding portmask
 * Output:
 *      pPortmask  - pointer buffer of multicast ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 */
int32
dal_longan_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pPortmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(index >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);

    osal_memset(pPortmask, 0, sizeof(rtk_portmask_t));

    L2_SEM_LOCK(unit);
    if ((ret = _dal_longan_l2_mcastFwdPortmaskEntry_get(unit, index, pPortmask)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pPortmask=0x%x", pPortmask->bits[0]);


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_mcastFwdPortmaskEntry_set
 * Description:
 *      Set portmask of multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      index     - index of multicast forwarding portmask
 *      pPortmask - pointer buffer of multicast ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 */
int32
dal_longan_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pPortmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(index >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pPortmask), RT_ERR_PORT_MASK);

    L2_SEM_LOCK(unit);
    if ((ret = _dal_longan_l2_mcastFwdPortmaskEntry_set(unit, index, pPortmask)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2              */
/* Sub-module Name: CPU MAC address */

/* Function Name:
 *      dal_longan_l2_cpuMacAddr_add
 * Description:
 *      Add a CPU mac address entry to the lookup table.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The packet destined to the CPU MAC is then forwarded to CPU port.
 */
int32
dal_longan_l2_cpuMacAddr_add(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32               ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = vid;
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.is_entry_valid = 1;
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = vid;
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));
    l2_entry.unicast.is_trk = 0;
    l2_entry.unicast.devID = HAL_UNIT_TO_DEV_ID(unit);
    l2_entry.unicast.port = HWP_CPU_MACID(unit);
    l2_entry.unicast.aging = 7;
    l2_entry.unicast.is_static  = 1;

    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_cpuMacAddr_del
 * Description:
 *      Delete a CPU mac address entry from the lookup table.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_cpuMacAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32 ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    /* search exist or free entry */
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = vid;
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_longan_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    /* fill content */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    ret = _dal_longan_l2_setL2Entry(unit, &index_entry, &l2_entry);

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Module Name    : L2        */
/* Sub-module Name: Port move */

/* Function Name:
 *      dal_longan_l2_portMoveAction_get
 * Description:
 *      Get forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_portMoveAction_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction)
{
    int32   ret;
    uint32 value;
    rtk_action_t *pAction_temp=NULL;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_PORT_MOVE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type==L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pAction->dynAct.port)), RT_ERR_PORT_ID);

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if((ret = reg_array_field_read(unit, LONGAN_L2_DYN_PORT_MV_ACTr, pAction->dynAct.port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            pAction_temp = &pAction->dynAct.act;
            break;
        case L2_PORT_MOVE_STATIC:
            if((ret = reg_field_read(unit, LONGAN_L2_GLB_STT_PORT_MV_ACTr, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            pAction_temp = &pAction->sttAct.act;
            break;
        case L2_PORT_MOVE_FORBID:
            if((ret = reg_field_read(unit, LONGAN_L2_PORT_MV_FORBID_CTRLr, LONGAN_FORBID_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            pAction_temp = &pAction->forbidAct.act;
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    switch(value)
    {
        case 0:
            *pAction_temp = ACTION_FORWARD;break;
        case 1:
            *pAction_temp = ACTION_DROP;break;
        case 2:
            *pAction_temp = ACTION_TRAP2CPU;break;
        case 3:
            *pAction_temp = ACTION_COPY2CPU;break;
        case 4:
            *pAction_temp = ACTION_TRAP2MASTERCPU;break;
        case 5:
            *pAction_temp = ACTION_COPY2MASTERCPU;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portMoveAction_set
 * Description:
 *      Set forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter and action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      None
 */
int32
dal_longan_l2_portMoveAction_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction)
{
    int32   ret;
    uint32 value;
    rtk_action_t action = 0;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_PORT_MOVE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type==L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pAction->dynAct.port)), RT_ERR_PORT_ID);

    switch(type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            action= pAction->dynAct.act;break;
        case L2_PORT_MOVE_STATIC:
            action= pAction->sttAct.act;break;
        case L2_PORT_MOVE_FORBID:
            action= pAction->forbidAct.act;break;
        default:
            break;
    }

    switch(action)
    {
        case ACTION_FORWARD:
            value=0;break;
        case ACTION_DROP:
            value=1;break;
        case ACTION_TRAP2CPU:
            value=2;break;
        case ACTION_COPY2CPU:
            value=3;break;
        case ACTION_TRAP2MASTERCPU:
            value=4;break;
        case ACTION_COPY2MASTERCPU:
            value=5;break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if((ret = reg_array_field_write(unit, LONGAN_L2_DYN_PORT_MV_ACTr, pAction->dynAct.port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_STATIC:
            if((ret = reg_field_write(unit, LONGAN_L2_GLB_STT_PORT_MV_ACTr, LONGAN_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_FORBID:
            if((ret = reg_field_write(unit, LONGAN_L2_PORT_MV_FORBID_CTRLr, LONGAN_FORBID_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portMoveLearn_get
 * Description:
 *      Get learning action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pLearn  - pointer to portmove parameter
 * Output:
 *      pLearn - pointer to learning action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_portMoveLearn_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    int32   ret;
    uint32 value;
    rtk_enable_t lrn;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLearn), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type==L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pLearn->dynLrn.port)), RT_ERR_PORT_ID);

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if((ret = reg_array_field_read(unit, LONGAN_L2_DYN_PORT_MV_LRNr, pLearn->dynLrn.port, REG_ARRAY_INDEX_NONE, LONGAN_LRNf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_STATIC:
            if((ret = reg_field_read(unit, LONGAN_L2_GLB_STT_PORT_MV_LRNr, LONGAN_LRNf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    L2_SEM_UNLOCK(unit);

    switch(value)
    {
        case 0:
            lrn = DISABLED;break;
        case 1:
            lrn = ENABLED;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    if(type==L2_PORT_MOVE_DYNAMIC)
        pLearn->dynLrn.enable = lrn;
    else
        pLearn->sttLrn.enable =lrn;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portMoveLearn_set
 * Description:
 *      Set learning action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pLearn  - pointer to portmove parameter and learning action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      None
 */
int32
dal_longan_l2_portMoveLearn_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    int32   ret;
    uint32 value;
    rtk_enable_t lrn = 0;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_PORT_MOVE_FORBID), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLearn), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type==L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pLearn->dynLrn.port)), RT_ERR_PORT_ID);

    switch(type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            lrn = pLearn->dynLrn.enable;break;
        case L2_PORT_MOVE_STATIC:
            lrn = pLearn->sttLrn.enable;break;
        default:
            break;
    }

    switch(lrn)
    {
        case DISABLED:
            value = 0;break;
        case ENABLED:
            value = 1;break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    switch(type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if((ret = reg_array_field_write(unit, LONGAN_L2_DYN_PORT_MV_LRNr, pLearn->dynLrn.port, REG_ARRAY_INDEX_NONE, LONGAN_LRNf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_STATIC:
            if((ret = reg_field_write(unit, LONGAN_L2_GLB_STT_PORT_MV_LRNr, LONGAN_LRNf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        default:
            L2_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;

    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2                        */
/* Sub-module Name: Parameter for lookup miss */

/* Function Name:
 *      dal_longan_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask which limits the lookup missed flooding domain.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8390, 8380, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
int32
dal_longan_l2_lookupMissFloodPortMask_get(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pFlood_portmask, RT_ERR_NULL_POINTER);

    switch (type)
    {
        case DLF_TYPE_UCAST:
            reg_idx = LONGAN_L2_UNKN_UC_FLD_PMSKr;
            break;
        case DLF_TYPE_BCAST:
            reg_idx = LONGAN_L2_BC_FLD_PMSKr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, reg_idx, LONGAN_PORTMASKf, &pFlood_portmask->bits[0])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "flood_portmask=0x%x", pFlood_portmask->bits[0]);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
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
 *      (1) In 9300 and 9310, only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
int32
dal_longan_l2_lookupMissFloodPortMask_set(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pFlood_portmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pFlood_portmask), RT_ERR_PORT_MASK);

    switch (type)
    {
        case DLF_TYPE_UCAST:
            reg_idx = LONGAN_L2_UNKN_UC_FLD_PMSKr;
            break;
        case DLF_TYPE_BCAST:
            reg_idx = LONGAN_L2_BC_FLD_PMSKr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, reg_idx, LONGAN_PORTMASKf, &pFlood_portmask->bits[0])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_longan_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to the lookup missed flooding port mask.
 * Input:
 *      unit       - unit id
 *      type       - type of lookup miss
 *      flood_port - flooding port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8390, 8380, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
int32
dal_longan_l2_lookupMissFloodPortMask_add(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32 ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, flood_port=%d",
           unit, flood_port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, flood_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_longan_l2_lookupMissFloodPortMask_get(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, flood_port);

    ret = dal_longan_l2_lookupMissFloodPortMask_set(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_lookupMissFloodPortMask_del
 * Description:
 *      Delete one port member from the lookup missed flooding port mask.
 * Input:
 *      unit       - unit id
 *      type       - type of lookup miss
 *      flood_port - flooding port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8390, 8380, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
int32
dal_longan_l2_lookupMissFloodPortMask_del(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32 ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, flood_port=%d",
           unit, flood_port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, flood_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_longan_l2_lookupMissFloodPortMask_get(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, flood_port);

    ret = dal_longan_l2_lookupMissFloodPortMask_set(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_IP6MC
 *      - DLF_TYPE_UCAST (9300 and 9310 only support DLF_TYPE_UCAST)
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
int32
dal_longan_l2_portLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    RT_PARAM_CHK((type != DLF_TYPE_UCAST), RT_ERR_INPUT);

    return dal_longan_l2_portUcastLookupMissAction_get(unit, port, pAction);
}

/* Function Name:
 *      dal_longan_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_IP6MC
 *      - DLF_TYPE_UCAST (9300 and 9310 only support DLF_TYPE_UCAST)
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
int32
dal_longan_l2_portLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    RT_PARAM_CHK((type != DLF_TYPE_UCAST), RT_ERR_INPUT);

    return dal_longan_l2_portUcastLookupMissAction_set(unit, port, action);
}

/* Function Name:
 *      dal_longan_l2_portUcastLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
int32
dal_longan_l2_portUcastLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    int32   ret;
    uint32  act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);
    if((ret = reg_array_field_read(unit, LONGAN_L2_PORT_UC_LM_ACTr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    switch(act)
    {
        case 0:
            *pAction = ACTION_FORWARD;break;
        case 1:
            *pAction = ACTION_DROP;break;
        case 2:
            *pAction = ACTION_TRAP2CPU;break;
        case 3:
            *pAction = ACTION_COPY2CPU;break;
        case 4:
            *pAction = ACTION_TRAP2MASTERCPU;break;
        case 5:
            *pAction = ACTION_COPY2MASTERCPU;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pAction:%d",  *pAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portUcastLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
int32
dal_longan_l2_portUcastLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    int32   ret;
    uint32  act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(action >= ACTION_END, RT_ERR_INPUT);

    switch(action)
    {
        case ACTION_FORWARD:
            act=0;break;
        case ACTION_DROP:
            act=1;break;
        case ACTION_TRAP2CPU:
            act=2;break;
        case ACTION_COPY2CPU:
            act=3;break;
        case ACTION_TRAP2MASTERCPU:
            act=4;break;
        case ACTION_COPY2MASTERCPU:
            act=5;break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);
    if((ret = reg_array_field_write(unit, LONGAN_L2_PORT_UC_LM_ACTr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */

/* Function Name:
 *      dal_longan_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 * Output:
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
int32
dal_longan_l2_srcPortEgrFilterMask_get(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    int32       ret = RT_ERR_FAILED;
    uint32      value;
    rtk_port_t  port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pFilter_portmask, RT_ERR_NULL_POINTER);

    osal_memset(pFilter_portmask, 0, sizeof(rtk_portmask_t));

    L2_SEM_LOCK(unit);

    HWP_PORT_TRAVS(unit, port)
    {
        if ((ret = reg_array_field_read(unit, LONGAN_L2_SRC_P_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_SRC_FLTR_ENf, &value)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
            return ret;
        }

        if (value)
        {
            RTK_PORTMASK_PORT_SET(*pFilter_portmask, port);
        }
    }

    L2_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "filter_portmask=0x%x", pFilter_portmask->bits[0]);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
int32
dal_longan_l2_srcPortEgrFilterMask_set(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    int32       ret = RT_ERR_FAILED;
    uint32      value;
    rtk_port_t  port;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pFilter_portmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pFilter_portmask), RT_ERR_PORT_MASK);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, filter_portmask=0x%x", unit, pFilter_portmask->bits[0]);

    L2_SEM_LOCK(unit);

    HWP_PORT_TRAVS(unit, port)
    {
        value = (RTK_PORTMASK_IS_PORT_SET(*pFilter_portmask, port) ? 1 : 0);
        if ((ret = reg_array_field_write(unit, LONGAN_L2_SRC_P_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_SRC_FLTR_ENf, &value)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
            return ret;
        }
    }

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_srcPortEgrFilterMask_add
 * Description:
 *      Enable the loopback filtering function on specified port.
 * Input:
 *      unit        - unit id
 *      filter_port - ports which turn on loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
int32
dal_longan_l2_srcPortEgrFilterMask_add(uint32 unit, rtk_port_t filter_port)
{
    int32 ret;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, filter_port=%d",
           unit, filter_port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, filter_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_longan_l2_srcPortEgrFilterMask_get(unit, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, filter_port);

    ret = dal_longan_l2_srcPortEgrFilterMask_set(unit, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_srcPortEgrFilterMask_del
 * Description:
 *      Disable the loopback filtering function on specified port.
 * Input:
 *      unit        - unit id
 *      filter_port - ports which turn off loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
int32
dal_longan_l2_srcPortEgrFilterMask_del(uint32 unit, rtk_port_t filter_port)
{
    int32 ret = RT_ERR_FAILED;
    rtk_portmask_t  portmask;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, filter_port=%d",
           unit, filter_port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, filter_port), RT_ERR_PORT_ID);

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    ret = dal_longan_l2_srcPortEgrFilterMask_get(unit, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, filter_port);

    ret = dal_longan_l2_srcPortEgrFilterMask_set(unit, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}

/*
 * MISC
 */

/* Function Name:
 *      dal_longan_l2_exceptionAddrAction_get
 * Description:
 *      Get forwarding action of packet with exception source MAC address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 * Output:
 *      pAction    - pointer to forward action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE - invalid exception address type
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Note:
 *      For 8390, 8380, 9300 and 9310, Exception address type is as following
 *      - SA_IS_BCAST_OR_MCAST
 *      - SA_IS_ZERO
 */
int32
dal_longan_l2_exceptionAddrAction_get(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    *pAction)
{
    int32   ret;
    uint32 act;
    uint32  reg_filed;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((exceptType >= EXCEPT_ADDR_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pAction, RT_ERR_NULL_POINTER);

    switch (exceptType)
    {
        case SA_IS_ZERO:
            reg_filed = LONGAN_ZERO_SA_ACTf;
            break;
        case SA_IS_BCAST_OR_MCAST:
            reg_filed = LONGAN_MC_BC_SA_ACTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, LONGAN_L2_CTRLr, reg_filed, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    switch(act)
    {
        case 0:
            *pAction = ACTION_FORWARD;break;
        case 1:
            *pAction = ACTION_DROP;break;
        case 2:
            *pAction = ACTION_TRAP2CPU;break;
        case 3:
            *pAction = ACTION_TRAP2MASTERCPU;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    return ret;
}

/* Function Name:
 *      dal_longan_l2_exceptionAddrAction_set
 * Description:
 *      Set forwarding action of packet with exception source MAC address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 *      action     - forward action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE - invalid exception address type
 *      RT_ERR_INPUT               - invalid input parameter
 * Note:
 *      For 8390, 8380, 9300 and 9310, Exception address type is as following
 *      - SA_IS_BCAST_OR_MCAST
 *      - SA_IS_ZERO
 */
int32
dal_longan_l2_exceptionAddrAction_set(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    action)
{
    int32   ret;
    uint32 act;
    uint32  reg_filed;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((exceptType >= EXCEPT_ADDR_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(action >= ACTION_END, RT_ERR_INPUT);

    switch(action)
    {
        case ACTION_FORWARD:
            act=0;break;
        case ACTION_DROP:
            act=1;break;
        case ACTION_TRAP2CPU:
            act=2;break;
        case ACTION_TRAP2MASTERCPU:
            act=3;break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    switch (exceptType)
    {
        case SA_IS_ZERO:
            reg_filed = LONGAN_ZERO_SA_ACTf;
            break;
        case SA_IS_BCAST_OR_MCAST:
            reg_filed = LONGAN_MC_BC_SA_ACTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, LONGAN_L2_CTRLr, reg_filed, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_longan_l2_addrEntry_get
 * Description:
 *      Get the L2 table entry by index of the specified unit.
 * Input:
 *      unit      - unit id
 *      index     - l2 table index
 * Output:
 *      pL2_entry - pointer buffer of l2 table entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) The index valid range is from 0 to (L2 hash table size - 1)
 *         - 0 ~ (L2 hash table size - 1) entry in L2 hash table
 *      2) The output entry have 2 variables (valid and entry_type) and its detail data structure
 *         - valid: 1 mean the entry is valid; 0: invalid
 *         - entry_type: FLOW_TYPE_UNICAST, FLOW_TYPE_L2_MULTI, FLOW_TYPE_IP4_MULTI and FLOW_TYPE_IP6_MULTI
 *                       the field is ignored if valid field is 0.
 *         - detail data structure is ignored if valid is 0, and its field meanings are depended
 *           on the entry_type value.
 *      3) If pL2_entry->flags have enabled the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *         pL2_entry->unicast.trk_gid value is valid trunk id value.
 */
int32
dal_longan_l2_addrEntry_get(uint32 unit, uint32 index, rtk_l2_entry_t *pL2_entry)
{
    int32 ret;
    dal_longan_l2_entry_t  l2_entry;
    dal_longan_l2_index_t  index_entry;
    rtk_enable_t l2CamEbl;
    uint32 value;

    RT_PARAM_CHK((NULL == pL2_entry), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);
    if((ret=reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LUTCAM_ENf, &value))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    l2CamEbl = value==1 ? ENABLED : DISABLED;

    if((index >= (hashTable_size[unit]+cam_size[unit])) || ((index >= hashTable_size[unit])&&(l2CamEbl == DISABLED)))
        return RT_ERR_INPUT;

    if(index<hashTable_size[unit])
    {
        index_entry.index_type = L2_IN_HASH;
        index_entry.index = index>>2;
        index_entry.hashdepth = index & 0x3;
    }
    else
    {
        index_entry.index_type = L2_IN_CAM;
        index_entry.index = index - hashTable_size[unit];
    }

    L2_SEM_LOCK(unit);
    if ((ret = _dal_longan_l2_getL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    pL2_entry->valid = l2_entry.is_entry_valid;

    if(!pL2_entry->valid)
        return RT_ERR_OK;

    switch(l2_entry.entry_type)
    {
        case L2_UNICAST:
            pL2_entry->entry_type = FLOW_TYPE_UNICAST;break;
        case L2_MULTICAST:
            pL2_entry->entry_type = FLOW_TYPE_L2_MULTI;break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    if(pL2_entry->entry_type == FLOW_TYPE_UNICAST)
    {
        /* fill content */
        osal_memcpy(&pL2_entry->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
        pL2_entry->unicast.vid       = l2_entry.unicast.fid;
        pL2_entry->unicast.flags     = 0;    /* Clear data */
        pL2_entry->unicast.state     = 0;    /* Clear data */
        if(l2_entry.unicast.is_trk)
        {
            pL2_entry->unicast.flags |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
            pL2_entry->unicast.trk_gid = l2_entry.unicast.trunk;
        }
        else
        {
            pL2_entry->unicast.devID = l2_entry.unicast.devID;
            pL2_entry->unicast.port = l2_entry.unicast.port;
        }

        if(l2_entry.unicast.sablock)
            pL2_entry->unicast.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
        if(l2_entry.unicast.dablock)
            pL2_entry->unicast.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
        if(l2_entry.unicast.is_static)
            pL2_entry->unicast.flags |= RTK_L2_UCAST_FLAG_STATIC;
        if(l2_entry.unicast.nh)
            pL2_entry->unicast.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;
        if(l2_entry.unicast.suspending)
            pL2_entry->unicast.state |= RTK_L2_UCAST_STATE_SUSPEND;
        if(l2_entry.unicast.aging == 0)
            pL2_entry->unicast.isAged = TRUE;
        else
            pL2_entry->unicast.isAged = FALSE;
        pL2_entry->unicast.route_idx     = l2_entry.unicast.mac_idx;
        pL2_entry->unicast.agg_vid       = l2_entry.unicast.agg_vid;

        if(index_entry.index_type == L2_IN_HASH)
            pL2_entry->unicast.l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
        else
            pL2_entry->unicast.l2_idx = hashTable_size[unit] + index_entry.index;
    }

    if(pL2_entry->entry_type == FLOW_TYPE_L2_MULTI)
    {
        /* fill content */
        osal_memcpy(&pL2_entry->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
        pL2_entry->l2mcast.rvid       = l2_entry.l2mcast.fid;
        pL2_entry->l2mcast.fwdIndex = l2_entry.l2mcast.index;
        pL2_entry->l2mcast.nextHop = l2_entry.l2mcast.nh;
        pL2_entry->l2mcast.mac_idx = l2_entry.l2mcast.mac_idx;

        L2_SEM_LOCK(unit);
        if ((ret = _dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pL2_entry->l2mcast.fwdIndex, &pL2_entry->l2mcast.portmask)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);

        if(index_entry.index_type == L2_IN_HASH)
            pL2_entry->l2mcast.l2_idx = (index_entry.index<<2) | index_entry.hashdepth;
        else
            pL2_entry->l2mcast.l2_idx = hashTable_size[unit] + index_entry.index;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_conflictAddr_get
 * Description:
 *      Get the conflict L2 table entry from one given L2 address in the specified unit.
 * Input:
 *      unit            - unit id
 *      pL2Addr         - l2 address to find its conflict entries
 *      cfAddrList_size - buffer size of the pCfAddrList
 * Output:
 *      pCfAddrList     - pointer buffer of the conflict l2 table entry list
 *      pCf_retCnt      - return number of find conflict l2 table entry list
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The function can be used if add l2 entry return RT_ERR_L2_NO_EMPTY_ENTRY.
 *          Input the pL2Addr->entry_type and its hash key to get conflict entry information.
 *      (2) User want to prepare the return buffer pCfAddrList and via. cfAddrList_size argument
 *          tell driver its size.
 *      (3) The function will return valid L2 hash entry from the same bucket and the return number
 *          is filled in pCf_retCnt, entry data is filled in pCfAddrList.
 */
int32
dal_longan_l2_conflictAddr_get(
    uint32          unit,
    rtk_l2_entry_t  *pL2Addr,
    rtk_l2_entry_t  *pCfAddrList,
    uint32          cfAddrList_size,
    uint32          *pCf_retCnt)
{
    int32 ret;

    uint32 cf_num=0;

    dal_longan_l2_hashKey_t hashkey;
    dal_longan_l2_entry_t l2_entry;
    dal_longan_l2_index_t blk0_index;
    dal_longan_l2_index_t blk1_index;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* check input parameters */
    RT_PARAM_CHK(NULL == pL2Addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pCfAddrList, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(0 == cfAddrList_size, RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pCf_retCnt, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->entry_type != FLOW_TYPE_UNICAST && pL2Addr->entry_type != FLOW_TYPE_L2_MULTI), RT_ERR_OUT_OF_RANGE);

    if(pL2Addr->entry_type == FLOW_TYPE_UNICAST)
    {
        RT_PARAM_CHK((pL2Addr->unicast.mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
        RT_PARAM_CHK((pL2Addr->unicast.vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    }
    else
    {
        RT_PARAM_CHK((pL2Addr->l2mcast.mac.octet[0] & BITMASK_1B) != 0x1, RT_ERR_MAC);
        RT_PARAM_CHK((pL2Addr->l2mcast.rvid> RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    }

    if (pL2Addr->entry_type == FLOW_TYPE_UNICAST)
    {   /* FLOW_TYPE_UNICAST */
        l2_entry.entry_type = FLOW_TYPE_UNICAST;
        l2_entry.unicast.fid = pL2Addr->unicast.vid;
        osal_memcpy(&l2_entry.unicast.mac, &pL2Addr->unicast.mac, sizeof(rtk_mac_t));
    }
    else if (pL2Addr->entry_type == FLOW_TYPE_L2_MULTI)
    {   /* FLOW_TYPE_L2_MULTI */
        l2_entry.entry_type = FLOW_TYPE_L2_MULTI;
        l2_entry.l2mcast.fid = pL2Addr->l2mcast.rvid;
        osal_memcpy(&l2_entry.l2mcast.mac, &pL2Addr->l2mcast.mac, sizeof(rtk_mac_t));
    }
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* calculate hash key */
    if ((ret = _dal_longan_l2_entryToHashKey(unit, &l2_entry, &hashkey)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blk0_index.index_type = L2_IN_HASH;
    blk0_index.index = hashkey.blk0_hashKey;

    L2_SEM_LOCK(unit);
    cf_num=0;
    for (blk0_index.hashdepth= 0; blk0_index.hashdepth < HAL_L2_HASHDEPTH(unit) && cf_num < cfAddrList_size; blk0_index.hashdepth++)
    {
        if ((ret=_dal_longan_l2_getL2Entry(unit, &blk0_index, &l2_entry))!= RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            return ret;
        }

        if(l2_entry.is_entry_valid==1)
        {
            (pCfAddrList + cf_num)->valid = l2_entry.is_entry_valid;
            if(l2_entry.entry_type == L2_UNICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_UNICAST;
                osal_memcpy(&(pCfAddrList + cf_num)->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
                 (pCfAddrList + cf_num)->unicast.vid       = l2_entry.unicast.fid;
                 (pCfAddrList + cf_num)->unicast.flags     = 0;    /* Clear data */
                 (pCfAddrList + cf_num)->unicast.state     = 0;    /* Clear data */
                if(l2_entry.unicast.is_trk)
                {
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
                     (pCfAddrList + cf_num)->unicast.trk_gid = l2_entry.unicast.trunk;
                }
                else
                {
                     (pCfAddrList + cf_num)->unicast.devID = l2_entry.unicast.devID;
                     (pCfAddrList + cf_num)->unicast.port = l2_entry.unicast.port;
                }

                if(l2_entry.unicast.sablock)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
                if(l2_entry.unicast.dablock)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
                if(l2_entry.unicast.is_static)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_STATIC;
                if(l2_entry.unicast.nh)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;
                if(l2_entry.unicast.suspending)
                     (pCfAddrList + cf_num)->unicast.state |= RTK_L2_UCAST_STATE_SUSPEND;
                if(l2_entry.unicast.aging == 0)
                     (pCfAddrList + cf_num)->unicast.isAged = TRUE;
                else
                     (pCfAddrList + cf_num)->unicast.isAged = FALSE;
                 (pCfAddrList + cf_num)->unicast.route_idx     = l2_entry.unicast.mac_idx;
                 (pCfAddrList + cf_num)->unicast.agg_vid       = l2_entry.unicast.agg_vid;

                 (pCfAddrList + cf_num)->unicast.l2_idx = (blk0_index.index<<2) | blk0_index.hashdepth;
            }
            else if(l2_entry.entry_type == L2_MULTICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_L2_MULTI;
                osal_memcpy(&(pCfAddrList + cf_num)->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
                (pCfAddrList + cf_num)->l2mcast.rvid       = l2_entry.l2mcast.fid;
                (pCfAddrList + cf_num)->l2mcast.fwdIndex = l2_entry.l2mcast.index;
                (pCfAddrList + cf_num)->l2mcast.nextHop = l2_entry.l2mcast.nh;
                (pCfAddrList + cf_num)->l2mcast.mac_idx = l2_entry.l2mcast.mac_idx;

                if ((ret = _dal_longan_l2_mcastFwdPortmaskEntry_get(unit, (pCfAddrList + cf_num)->l2mcast.fwdIndex, &(pCfAddrList + cf_num)->l2mcast.portmask)) != RT_ERR_OK)
                {
                    L2_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }

                (pCfAddrList + cf_num)->l2mcast.l2_idx = (blk0_index.index<<2) | blk0_index.hashdepth;
            }
            else
            {
                L2_SEM_UNLOCK(unit);
                return RT_ERR_OUT_OF_RANGE;
            }

            cf_num++;

        }
    }

    blk1_index.index_type = L2_IN_HASH;
    blk1_index.index = hashkey.blk1_hashKey;
    for (blk1_index.hashdepth= 0; blk1_index.hashdepth < HAL_L2_HASHDEPTH(unit) && cf_num < cfAddrList_size; blk1_index.hashdepth++)
    {
        if ((ret=_dal_longan_l2_getL2Entry(unit, &blk1_index, &l2_entry))!= RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            return ret;
        }

        if(l2_entry.is_entry_valid==1)
        {
            (pCfAddrList + cf_num)->valid = l2_entry.is_entry_valid;
            if(l2_entry.entry_type == L2_UNICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_UNICAST;
                osal_memcpy(&(pCfAddrList + cf_num)->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
                 (pCfAddrList + cf_num)->unicast.vid       = l2_entry.unicast.fid;
                 (pCfAddrList + cf_num)->unicast.flags     = 0;    /* Clear data */
                 (pCfAddrList + cf_num)->unicast.state     = 0;    /* Clear data */
                if(l2_entry.unicast.is_trk)
                {
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
                     (pCfAddrList + cf_num)->unicast.trk_gid = l2_entry.unicast.trunk;
                }
                else
                {
                     (pCfAddrList + cf_num)->unicast.devID = l2_entry.unicast.devID;
                     (pCfAddrList + cf_num)->unicast.port = l2_entry.unicast.port;
                }

                if(l2_entry.unicast.sablock)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
                if(l2_entry.unicast.dablock)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
                if(l2_entry.unicast.is_static)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_STATIC;
                if(l2_entry.unicast.nh)
                     (pCfAddrList + cf_num)->unicast.flags |= RTK_L2_UCAST_FLAG_NEXTHOP;
                if(l2_entry.unicast.suspending)
                     (pCfAddrList + cf_num)->unicast.state |= RTK_L2_UCAST_STATE_SUSPEND;
                if(l2_entry.unicast.aging == 0)
                     (pCfAddrList + cf_num)->unicast.isAged = TRUE;
                else
                     (pCfAddrList + cf_num)->unicast.isAged = FALSE;
                 (pCfAddrList + cf_num)->unicast.route_idx     = l2_entry.unicast.mac_idx;
                 (pCfAddrList + cf_num)->unicast.agg_vid       = l2_entry.unicast.agg_vid;

                 (pCfAddrList + cf_num)->unicast.l2_idx = (blk1_index.index<<2) | blk1_index.hashdepth;
            }
            else if(l2_entry.entry_type == L2_MULTICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_L2_MULTI;
                osal_memcpy(&(pCfAddrList + cf_num)->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
                (pCfAddrList + cf_num)->l2mcast.rvid       = l2_entry.l2mcast.fid;
                (pCfAddrList + cf_num)->l2mcast.fwdIndex = l2_entry.l2mcast.index;
                (pCfAddrList + cf_num)->l2mcast.nextHop = l2_entry.l2mcast.nh;
                (pCfAddrList + cf_num)->l2mcast.mac_idx = l2_entry.l2mcast.mac_idx;

                if ((ret = _dal_longan_l2_mcastFwdPortmaskEntry_get(unit, (pCfAddrList + cf_num)->l2mcast.fwdIndex, &(pCfAddrList + cf_num)->l2mcast.portmask)) != RT_ERR_OK)
                {
                    L2_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }

                (pCfAddrList + cf_num)->l2mcast.l2_idx = (blk1_index.index<<2) | blk1_index.hashdepth;
            }
            else
            {
                L2_SEM_UNLOCK(unit);
                return RT_ERR_OUT_OF_RANGE;
            }

            cf_num++;

        }
    }

    (*pCf_retCnt) = cf_num;

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_zeroSALearningEnable_get
 * Description:
 *      Get enable status of all-zero-SA learning.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_zeroSALearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_ZERO_SA_LRNf, &enable)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_zeroSALearningEnable_set
 * Description:
 *      Set enable status of all-zero-SA learning.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_l2_zeroSALearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, enable=%d",
           unit, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    L2_SEM_LOCK(unit);

    /* programming value to CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_L2_CTRLr, LONGAN_ZERO_SA_LRNf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portDynamicPortMoveForbidEnable_get
 * Description:
 *      Get the port moveforbiddance configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
int32
dal_longan_l2_portDynamicPortMoveForbidEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP */
    if((ret = reg_array_field_read(unit, LONGAN_L2_PORT_MV_FORBIDr, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &enable)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portDynamicPortMoveForbidEnable_set
 * Description:
 *      Set the port move forbiddance configuration of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
int32
dal_longan_l2_portDynamicPortMoveForbidEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    L2_SEM_LOCK(unit);

    /* program value to CHIP */
    if((ret = reg_array_field_write(unit, LONGAN_L2_PORT_MV_FORBIDr, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_trkDynamicPortMoveForbidEnable_get
 * Description:
 *      Get the port moveforbiddance configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled trunk is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
int32
dal_longan_l2_trkDynamicPortMoveForbidEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, tid=%d", unit, tid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP */
    if((ret = reg_array_field_read(unit, LONGAN_L2_TRK_MV_FORBIDr, REG_ARRAY_INDEX_NONE, tid, LONGAN_ENf, &enable)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_trkDynamicPortMoveForbidEnable_set
 * Description:
 *      Set the port move forbiddance configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled trunk is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
int32
dal_longan_l2_trkDynamicPortMoveForbidEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, tid=%d, enable=%d", unit, tid, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >=HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    L2_SEM_LOCK(unit);

    /* program value to CHIP */
    if((ret = reg_array_field_write(unit, LONGAN_L2_TRK_MV_FORBIDr, REG_ARRAY_INDEX_NONE, tid, LONGAN_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portMacFilterEnable_get
 * Description:
 *      Get the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_portMacFilterEnable_get(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  reg;
    uint32  enable;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, blockMode=%d", unit, port, filterMode);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(filterMode)
    {
        case MAC_FILTER_MODE_SA:
           reg = LONGAN_L2_PORT_SABLK_CTRLr;break;
        case MAC_FILTER_MODE_DA:
           reg = LONGAN_L2_PORT_DABLK_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    /* get value from CHIP */
    if((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &enable)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == enable)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portMacFilterEnable_set
 * Description:
 *      Set the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 *      enable      - drop procedence assignment status for trapped packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_portMacFilterEnable_set(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t enable)
{
    int32   ret;
    uint32  reg;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, blockMode=%d, enable=%d", unit, port, filterMode, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    switch(filterMode)
    {
        case MAC_FILTER_MODE_SA:
           reg = LONGAN_L2_PORT_SABLK_CTRLr;break;
        case MAC_FILTER_MODE_DA:
           reg = LONGAN_L2_PORT_DABLK_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    /* program value to CHIP */
    if((ret = reg_array_field_write(unit, reg, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_hwNextValidAddr_get
 * Description:
 *      get next valid entry with specific method.
 * Input:
 *      unit        - unit id
 *      pScan_idx   - the index which starting search from
 *      rtk_l2_nextValidType_t  - search Method
 * Output:
 *      pScan_idx           - the next valid entry index
 *      rtk_l2_entry_t      - the next valid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_l2_hwNextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_nextValidType_t type,
    rtk_l2_entry_t  *pEntry)
{

    int32 ret;
    dal_longan_l2_entry_t    l2_entry;
    uint32                  is_found = 0;
    rtk_enable_t            l2CamEbl;
    uint32                  value = 0, data = 0;
    dal_longan_l2_index_t    index_entry;

    uint32 scanType;
    uint32 startIdx;
    uint32 findIdx;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);
    RT_PARAM_CHK(NULL == pScan_idx, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type > L2_NEXT_VALID_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_longan_l2_entry_t));

    L2_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LUTCAM_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    l2CamEbl = value == 1 ? ENABLED : DISABLED;

    if ((*pScan_idx >= (int32)(hashTable_size[unit] + cam_size[unit])) || ((*pScan_idx >= (int32)hashTable_size[unit]) && (l2CamEbl == DISABLED)))
        return RT_ERR_INPUT;


    if (L2_NEXT_VALID_TYPE_UC == type)
        scanType = 0;
    else if (L2_NEXT_VALID_TYPE_AUTO_UC == type)
        scanType = 1;
    else if (L2_NEXT_VALID_TYPE_SOFTWARE_UC == type)
        scanType = 2;
    else if (L2_NEXT_VALID_TYPE_UC_NH == type)
        scanType = 3;
    else if (L2_NEXT_VALID_TYPE_MC == type)
        scanType = 4;
    else if (L2_NEXT_VALID_TYPE_MC_NH == type)
        scanType = 5;
    else
    {
        return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    data=0;

    value = 1;
    if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_EXECf, &value, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_METHODf, &scanType, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    value=(*pScan_idx < hashTable_size[unit]) ? 0 : 1;
    if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_SRAM_IFf, &value, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    startIdx = (*pScan_idx < hashTable_size[unit]) ? *pScan_idx : (*pScan_idx - hashTable_size[unit]);
    if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_START_ADDRf, &startIdx, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    if ((ret = reg_write(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }


    while(1)
    {
        if ((ret = reg_field_read(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_EXECf, &value)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if (value == 0)
            break;
    }


    if ((ret = reg_read(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL2r, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }


    if ((ret = reg_field_get(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL2r, LONGAN_HITf, &is_found, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if ((ret = reg_field_get(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL2r, LONGAN_HIT_ADDRf, &findIdx, &data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }


    if (is_found == 0 || (startIdx > findIdx))
    {
        L2_SEM_UNLOCK(unit);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    if(*pScan_idx < hashTable_size[unit])
    {
        index_entry.index_type = L2_IN_HASH;
        index_entry.index       = findIdx >> 2;
        index_entry.hashdepth   = findIdx & 0x3;
    }
    else
    {
        index_entry.index_type  = L2_IN_CAM;
        index_entry.index       = findIdx;
    }


    if ((ret=_dal_longan_l2_getL2Entry(unit, &index_entry, &l2_entry))!= RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    if(*pScan_idx < hashTable_size[unit])
    {
        *pScan_idx = findIdx;
    }
    else
    {
        *pScan_idx = findIdx + hashTable_size[unit];
    }

    pEntry->valid = l2_entry.is_entry_valid;
    pEntry->entry_type = l2_entry.entry_type;
    /* fill content */
    if (l2_entry.entry_type==L2_UNICAST)
    {
        osal_memcpy(&pEntry->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
        pEntry->unicast.vid     = l2_entry.unicast.fid;
        pEntry->unicast.flags   = 0;    /* Clear data */
        pEntry->unicast.state   = 0;    /* Clear data */
        if (l2_entry.unicast.is_trk)
        {
            pEntry->unicast.flags     |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
            pEntry->unicast.devID = l2_entry.unicast.devID;
            pEntry->unicast.trk_gid   = l2_entry.unicast.trunk;
        }
        else
        {
            pEntry->unicast.devID   = l2_entry.unicast.devID;
            pEntry->unicast.port      = l2_entry.unicast.port;
        }

        pEntry->unicast.flags     |= l2_entry.unicast.sablock ? RTK_L2_UCAST_FLAG_SA_BLOCK : 0;
        pEntry->unicast.flags     |= l2_entry.unicast.dablock ? RTK_L2_UCAST_FLAG_DA_BLOCK : 0;
        pEntry->unicast.flags     |= l2_entry.unicast.is_static ? RTK_L2_UCAST_FLAG_STATIC : 0;
        pEntry->unicast.state     |= l2_entry.unicast.suspending ? RTK_L2_UCAST_STATE_SUSPEND : 0;
        pEntry->unicast.flags     |= l2_entry.unicast.nh ? RTK_L2_UCAST_FLAG_NEXTHOP : 0;
        pEntry->unicast.age        = l2_entry.unicast.aging;
        pEntry->unicast.isAged     = l2_entry.unicast.aging==0 ? TRUE : FALSE;
        pEntry->unicast.agg_vid = l2_entry.unicast.agg_vid;
        pEntry->unicast.route_idx    = l2_entry.unicast.mac_idx;

        pEntry->unicast.l2_idx = *pScan_idx;
    }
    else if (l2_entry.entry_type==L2_MULTICAST)
    {
        osal_memcpy(&pEntry->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
        pEntry->l2mcast.rvid        = l2_entry.l2mcast.fid;
        pEntry->l2mcast.fwdIndex    = l2_entry.l2mcast.index;
        pEntry->l2mcast.nextHop     = l2_entry.l2mcast.nh;
        pEntry->l2mcast.mac_idx = l2_entry.l2mcast.mac_idx;

        pEntry->l2mcast.l2_idx = *pScan_idx;

        L2_SEM_LOCK(unit);
        if ((ret = _dal_longan_l2_mcastFwdPortmaskEntry_get(unit, pEntry->l2mcast.fwdIndex, &pEntry->l2mcast.portmask)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);
    }
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_longan_l2_stkLearningEnable_get
 * Description:
 *      Get the enable status of stacking system auto-learning mode.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to the enable status of stacking system auto-learning mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When it is enabled, switch will learn the MAC and source ingress port of packet which received from stacking port.
 */
int32
dal_longan_l2_stkLearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_STK_AUTO_LRNf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_stkLearningEnable_set
 * Description:
 *      Set the enable status of stacking system auto-learning mode.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of stacking system auto-learning mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      When it is enabled, switch will learn the MAC and source ingress port of packet which received from stacking port.
 */
int32
dal_longan_l2_stkLearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_L2_CTRLr, LONGAN_STK_AUTO_LRNf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_stkKeepUcastEntryValid_get
 * Description:
 *      Get the enable status of keeping-entry-valid in stacking system.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to the enable status of distributed lookup
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When it is enabled, switch will keep an aging-out/link-down-flush unicast entry valid if it was not learnt on this switch.
 */
int32
dal_longan_l2_stkKeepUcastEntryValid_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_L2_AGE_CTRLr, LONGAN_KEEP_AGE_OUT_ENTRY_VALIDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_stkKeepUcastEntryValid_set
 * Description:
 *      Set the enable status of keeping-entry-valid in stacking system.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of distributed lookup
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      When it is enabled, switch will keep an aging-out/link-down-flush unicast entry valid if it was not learnt on this switch.
 */
int32
dal_longan_l2_stkKeepUcastEntryValid_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_L2_AGE_CTRLr, LONGAN_KEEP_AGE_OUT_ENTRY_VALIDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - control type
 * Output:
 *      pArg    - pointer to the argurment
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
dal_longan_l2_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d,port=%d,type=%d,pArg=%p", unit, port, type, pArg);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= RTK_L2_PCT_END), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    L2_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_L2_PCT_SA_ACT_REF:
            L2_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L2_SA_ACT_REFr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_SRCf, val, "", errExit, ret);
            *pArg = (val)? ENABLED : DISABLED;
            break;

        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

errInput:
errExit:
    L2_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_longan_l2_portCtrl_set
 * Description:
 *      Set the configuration of the specified control type and port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - control type
 *      arg     - argurment
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
dal_longan_l2_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d,port=%d,type=%d,arg=0x%08x", unit, port, type, arg);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= RTK_L2_PCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L2_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_L2_PCT_SA_ACT_REF:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L2_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                LONGAN_L2_SA_ACT_REFr, port, REG_ARRAY_INDEX_NONE, \
                LONGAN_SRCf, val, "", errExit, ret);
            break;

        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

errInput:
errExit:
    L2_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_longan_l2_status_get
 * Description:
 *      Get the status or counter of the specified feature
 * Input:
 *      unit    - unit id
 *      type    - status type
 *      index   - index
 * Output:
 *      pArg    - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      "index" may be only valid in some rtk_l2_stsType_t types.
 */
int32
dal_longan_l2_status_get(uint32 unit, rtk_l2_stsType_t type, uint32 index, uint32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;
    rtk_lrnAge_shadow_t *pLrnAge = NULL;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d,type=%d,index=%d,pArg=%p", unit, type, index, pArg);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_L2_STS_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L2_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_L2_STS_HASHFULL_CNT:
            L2_REG_FIELD_READ_ERR_HDL(unit, \
                LONGAN_L2_HASH_FULL_CNTr, LONGAN_CNTf, val, "", errExit, ret);
            *pArg = val;
            break;

        case RTK_L2_STS_DBG_CNT:
            pLrnAge = (rtk_lrnAge_shadow_t*)pArg;
            osal_memcpy(pLrnAge->l2_lrn, l2_lrn, sizeof(l2_lrn));
            osal_memcpy(pLrnAge->aging_time, gAging_time, sizeof(gAging_time));
            break;

        default:
            ret = RT_ERR_INPUT;
            goto errInput;
    }

errInput:
errExit:
    L2_SEM_UNLOCK(unit);

    return ret;
}



/* Function Name:
 *      dal_longan_l2_entryCnt_get
 * Description:
 *      Get l2 entry counter
 * Input:
 *      unit    - unit id
 *      type    - l2 entry type
 * Output:
 *      pCnt    - pointer to the entry counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT        - invalid input
 * Note:
 */
int32
dal_longan_l2_entryCnt_get(uint32 unit, rtk_l2_entryType_t type, uint32 *pCnt)
{
    int32 ret;
    rtk_l2_entry_t l2_entry;
    rtk_enable_t l2CamEbl;
    uint32 index;
    uint32 value;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);
    RT_PARAM_CHK((type >= L2_ENTRY_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);
    if((ret=reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LUTCAM_ENf, &value))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }
    l2CamEbl = value==1 ? ENABLED : DISABLED;
    L2_SEM_UNLOCK(unit);

    value = 0;
    for(index=0; index<hashTable_size[unit]+(l2CamEbl==ENABLED ? cam_size[unit] : 0); index++)
    {
        osal_memset(&l2_entry, 0, sizeof(rtk_l2_entry_t));
        if((ret=dal_longan_l2_addrEntry_get(unit, index, &l2_entry))!=RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        switch(type)
        {
            case L2_ENTRY_ALL:
                if(l2_entry.valid==1)
                    value++;
                break;
            case L2_ENTRY_ALL_SOFTWARE_CFG:
                if  (
                    l2_entry.valid==1 &&
                    (
                        l2_entry.entry_type==FLOW_TYPE_L2_MULTI ||
                        (l2_entry.entry_type==FLOW_TYPE_UNICAST &&  (
                                                                        (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_STATIC)!=0 ||
                                                                        (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_SA_BLOCK)!=0 ||
                                                                        (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_DA_BLOCK)!=0 ||
                                                                        (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_NEXTHOP)!=0
                                                                    )
                        )
                    )
                    )
                    value++;
                break;

            case L2_ENTRY_MC:
                if(l2_entry.valid==1 && l2_entry.entry_type==FLOW_TYPE_L2_MULTI)
                    value++;
                break;
            case L2_ENTRY_UC_SOFTWARE_CFG:
                if  (
                        l2_entry.valid==1 && l2_entry.entry_type==FLOW_TYPE_UNICAST &&
                        (
                            (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_STATIC)!=0 ||
                            (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_SA_BLOCK)!=0 ||
                            (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_DA_BLOCK)!=0 ||
                            (l2_entry.unicast.flags & RTK_L2_UCAST_FLAG_NEXTHOP)!=0
                        )
                    )
                    value++;
                break;

            default:
                return ret;


        }
    }

    *pCnt = value;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_hashIdx_get
 * Description:
 *      Get L2 hash index based on specified vid and MAC address
 * Input:
 *      unit        - unit id
 *      pMacHashIdx - pointer to vid and mac
 * Output:
 *      pMacHashIdx - pointer to hash indexes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      (1) VID is same as FID in IVL mode.
 *      (2) The pMacHashIdx.vid and pMacHashIdx.mac is input key
 *      (3) validBlk and validAlgo fields in pMacHashIdx specify how many blocks and hash-algo number
 *          the target unit supports.
 */
int32
dal_longan_l2_hashIdx_get(uint32 unit, rtk_l2_macHashIdx_t *pMacHashIdx)
{
    uint64  hashSeed = 0;
    uint32  hash1, hash3;
    uint32  hash0_idx;
    uint32  hash1_idx;


    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMacHashIdx, RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d,vid=%p,pMac=%x-%x-%x-%x-%x-%x", unit, pMacHashIdx->vid,
           pMacHashIdx->mac.octet[0], pMacHashIdx->mac.octet[1], pMacHashIdx->mac.octet[2],
           pMacHashIdx->mac.octet[3], pMacHashIdx->mac.octet[4], pMacHashIdx->mac.octet[5]);



    hashSeed = (((uint64)pMacHashIdx->vid << 48) |
                    ((uint64)pMacHashIdx->mac.octet[0] << 40) |
                    ((uint64)pMacHashIdx->mac.octet[1] << 32) |
                    ((uint64)pMacHashIdx->mac.octet[2] << 24) |
                    ((uint64)pMacHashIdx->mac.octet[3] << 16) |
                    ((uint64)pMacHashIdx->mac.octet[4] << 8) |
                    ((uint64)pMacHashIdx->mac.octet[5]));

    /* Algo 0 */
    hash0_idx =  (uint32) (((hashSeed >> 55) & BITMASK_5B)
                        ^ ((hashSeed >> 44) & BITMASK_11B)
                        ^ ((hashSeed >> 33) & BITMASK_11B)
                        ^ ((hashSeed >> 22) & BITMASK_11B)
                        ^ ((hashSeed >> 11) & BITMASK_11B)
                        ^ (hashSeed  & BITMASK_11B));



    /* Algo 1 */
    hash1 = (hashSeed>>11) & BITMASK_11B;
    hash1 = ((hash1 & BITMASK_5B) <<6) | ((hash1>>5) & BITMASK_6B);

    hash3 = (hashSeed>>33) & BITMASK_11B;
    hash3 = ((hash3 & BITMASK_6B) <<5) | ((hash3>>6) & BITMASK_5B);

    hash1_idx = (uint32) ((hashSeed >> 55) & BITMASK_5B)
                    ^ ((hashSeed >> 44) & BITMASK_11B)
                    ^ hash3
                    ^ ((hashSeed >> 22) & BITMASK_11B)
                    ^ hash1
                    ^ (hashSeed  & BITMASK_11B);



    pMacHashIdx->idx[0][0] = hash0_idx;
    pMacHashIdx->idx[1][0] = hash0_idx + ((hashTable_size[unit]/2)>>2);
    pMacHashIdx->idx[0][1] = hash1_idx;
    pMacHashIdx->idx[1][1] = hash1_idx + ((hashTable_size[unit]/2)>>2);
    pMacHashIdx->validBlk = 2;
    pMacHashIdx->validAlgo = 2;

    return RT_ERR_OK;
}

static int32
_dal_longan_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac, uint32 sram_if)
{
    int32 ret;

    uint32 value;
    uint32 is_mc;
    uint32 findIdx;
    uint32 tbl_idx;
    uint32 tbl_size;
    uint32 scanType;
    uint32 is_found;
    uint32 next_hop;
    uint32 tbl_type;
    uint32 fwdTableIdx;

    uint32 data0 = 0;
    uint32 data2 = 0;
    uint32 scan_idx = 0;
    uint32 is_static = 0;
    uint32 mac_uint32[2];
    uint32 tbl_name[4] = {LONGAN_L2_UCt, LONGAN_L2_MCt, LONGAN_L2_CAM_UCt, LONGAN_L2_CAM_MCt};
    uint32 tbl_nh_field[4] = {LONGAN_L2_UC_NEXT_HOPtf, LONGAN_L2_MC_NEXT_HOPtf, LONGAN_L2_CAM_UC_NEXT_HOPtf, LONGAN_L2_CAM_MC_NEXT_HOPtf};
    uint32 tbl_mac_field[4] = {LONGAN_L2_UC_MACtf, LONGAN_L2_MC_MACtf, LONGAN_L2_CAM_UC_MACtf, LONGAN_L2_CAM_MC_MACtf};

    rtk_fid_t fid;
    rtk_mac_t l2_mac;
    l2_entry_t  l2_entry;
    dal_longan_l2_index_t index_entry;

    is_mc = ((pMac->octet[0] & BITMASK_1B)>0)?1:0;
    scanType = (is_mc == 1)?4:0;
    tbl_size = (sram_if == 0)?((int32)cam_size[unit]):((int32)hashTable_size[unit]);
    tbl_type = ((!sram_if)<<1) | (is_mc);

    value = (sram_if == 1)?0:1;
    if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_SRAM_IFf, &value, &data0)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }
    if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_METHODf, &scanType, &data0)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    while(scan_idx < tbl_size)
    {
        value = 1;
        if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_EXECf, &value, &data0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
            return ret;
        }
        if ((ret = reg_field_set(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_START_ADDRf, &scan_idx, &data0)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
            return ret;
        }
        L2_SEM_LOCK(unit);
        if ((ret = reg_write(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, &data0)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        while(1)
        {
            if ((ret = reg_field_read(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL0r, LONGAN_EXECf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (value == 0)
                break;
        }
        if ((ret = reg_read(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL2r, &data2)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);

        if ((ret = reg_field_get(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL2r, LONGAN_HITf, &is_found, &data2)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = reg_field_get(unit, LONGAN_TBL_ACCESS_L2_METHOD_CTRL2r, LONGAN_HIT_ADDRf, &findIdx, &data2)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if (is_found == 0 || (scan_idx > findIdx))
        {
            return RT_ERR_OK;
        }
        if(sram_if)
        {
            index_entry.index_type  = L2_IN_HASH;
            index_entry.index       = findIdx >> 2;
            index_entry.hashdepth   = findIdx & 0x3;
            tbl_idx                 = ((index_entry.index<<2) | (index_entry.hashdepth));
        }
        else
        {
            index_entry.index_type  = L2_IN_CAM;
            index_entry.index       = findIdx;
            tbl_idx                 = index_entry.index;
        }

        L2_SEM_LOCK(unit);
        if ((ret = table_read(unit,  tbl_name[tbl_type], tbl_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);
        if ((ret = table_field_get(unit, tbl_name[tbl_type], tbl_nh_field[tbl_type], &next_hop, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit,  tbl_name[tbl_type], tbl_mac_field[tbl_type], &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        l2_mac.octet[0] = (uint8)(mac_uint32[1] >> 8);
        l2_mac.octet[1] = (uint8)(mac_uint32[1]);
        l2_mac.octet[2] = (uint8)(mac_uint32[0] >> 24);
        l2_mac.octet[3] = (uint8)(mac_uint32[0] >> 16);
        l2_mac.octet[4] = (uint8)(mac_uint32[0] >> 8);
        l2_mac.octet[5] = (uint8)(mac_uint32[0]);

        if(0 == is_mc)
        {
            if ((ret = table_field_get(unit, tbl_name[tbl_type], ((sram_if == 1)?LONGAN_L2_UC_STATICtf:LONGAN_L2_CAM_UC_STATICtf), &is_static, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
        if(!osal_memcmp(pMac, &l2_mac, sizeof(rtk_mac_t)) && ((0 == next_hop)||(0 == is_mc)) && ((0 == is_static)||(1 == include_static)))
        {
            if(1 == next_hop)
            {
                if ((ret = table_field_get(unit, tbl_name[tbl_type], ((sram_if == 1)?LONGAN_L2_UC_FID_RVIDtf:LONGAN_L2_CAM_UC_FID_RVIDtf), &fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                osal_memset(&l2_entry, 0, sizeof(l2_entry_t));
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1)?LONGAN_L2_UC_MACtf:LONGAN_L2_CAM_UC_MACtf), &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1)?LONGAN_L2_UC_FID_RVIDtf:LONGAN_L2_CAM_UC_FID_RVIDtf), &fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                value = PORT_DONT_CARE_9300;
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1)?LONGAN_L2_UC_SPAtf:LONGAN_L2_CAM_UC_SPAtf), &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                value = 1;
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1)?LONGAN_L2_UC_NEXT_HOPtf:LONGAN_L2_CAM_UC_NEXT_HOPtf), &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1)?LONGAN_L2_UC_VALIDtf:LONGAN_L2_CAM_UC_VALIDtf), &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
            }
            else
            {
                if(is_mc)
                {
                    if ((ret = table_field_get(unit, tbl_name[tbl_type],  ((sram_if == 1)?LONGAN_L2_MC_MC_PMSK_IDXtf:LONGAN_L2_CAM_MC_MC_PMSK_IDXtf), &fwdTableIdx, (uint32 *)&l2_entry)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                        return ret;
                    }
                    L2_SEM_LOCK(unit);
                    _dal_longan_l2_mcastFwdIndex_free(unit, fwdTableIdx);
                    L2_SEM_UNLOCK(unit);
                }
                osal_memset(&l2_entry, 0, sizeof(l2_entry_t));
            }

            L2_SEM_LOCK(unit);
            if ((ret = table_write(unit, tbl_name[tbl_type], tbl_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            L2_SEM_UNLOCK(unit);
        }
        scan_idx = findIdx + 1;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_l2_addr_delByMac
 * Description:
 *      Delete all L2 entries with specific MAC
 * Input:
 *      unit            - unit id
 *      include_static  - include static mac or not
 *      pMac            - mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input
 * Note:
 */
int32
dal_longan_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac)
{
    int32 ret;
    uint32 value;
    uint32 sram_if = 1;
    rtk_enable_t l2CamEbl;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((0x0 != include_static) && (0x1 != include_static)), RT_ERR_INPUT);

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, LONGAN_L2_CTRLr, LONGAN_LUTCAM_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);
    l2CamEbl = (value == 1)?ENABLED:DISABLED;

    if((ret = _dal_longan_l2_addr_delByMac(unit, include_static, pMac, sram_if)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if(ENABLED == l2CamEbl)
    {
        sram_if = 0;
        if((ret = _dal_longan_l2_addr_delByMac(unit, include_static, pMac, sram_if)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}


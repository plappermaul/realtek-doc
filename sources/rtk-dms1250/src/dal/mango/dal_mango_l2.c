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
 * $Revision: 103274 $
 * $Date: 2020-01-03 18:05:35 +0800 (Fri, 03 Jan 2020) $
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
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <hal/mac/drv/drv_rtl9310.h>
//#include <dal/mango/dal_mango_trunk.h>
#include <dal/mango/dal_mango_switch.h>
#include <dal/mango/dal_mango_l2.h>
//#include <dal/mango/dal_mango_l3.h>
//#include <dal/mango/dal_mango_trap.h>
//#include <dal/mango/dal_mango_vlan.h>
#include <dal/mango/dal_mango_mcast.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/trap.h>
#ifdef CONFIG_SDK_DRIVER_L2NTFY
  #include <drv/l2ntfy/l2ntfy.h>
  #include <private/drv/l2ntfy/l2ntfy_rtl9310.h>
#endif
#include <common/debug/rt_log.h>


/*
 * Symbol Definition
 */
#define PORT_DONT_CARE_9310      (0X3F)


typedef struct dal_mango_l2DrvDb_entry_s
{
    rtk_mcast_group_t           groupId;        /* associated group id */
    dal_mango_mcast_l2Node_t    mcastNode;      /* mcast link-list node */
} dal_mango_l2DrvDb_entry_t;

typedef struct dal_mango_l2_drvDb_s
{
    dal_mango_l2DrvDb_entry_t *pEntry;
} dal_mango_l2_drvDb_t;

/*
 * Data Declaration
 */
static uint32               l2_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         l2_sem[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         l2_sem_2[RTK_MAX_NUM_OF_UNIT];
static osal_mutex_t         l2_pmsk_tbl_sem[RTK_MAX_NUM_OF_UNIT];
static uint32               hashTable_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               cam_size[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               mcast_tableSize[RTK_MAX_NUM_OF_UNIT] = {0};
static uint32               gRTL9311E;
static uint32               l2_lrn[ARB_MAX][RTK_MAX_NUM_OF_PORTS];
static uint32               gAging_time[ARB_MAX];
static uint32				ld_patch_en;


/* shadow database */
static uint32               blkAlgoType[LUT_TBL_BLK_NUM][RTK_MAX_NUM_OF_UNIT];
static uint16               *nh_used_cnt[RTK_MAX_NUM_OF_UNIT];
dal_mango_l2_drvDb_t        l2Db[RTK_MAX_NUM_OF_UNIT];

/* Multicast database */
static dal_mango_mcast_index_pool_t    mcast_idx_pool[RTK_MAX_NUM_OF_UNIT];


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
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_L2), "portmask table semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define L2_PMSK_TBL_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(l2_pmsk_tbl_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_L2), "portmask table semaphore unlock failed");\
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

#define L2_ENTRY_UNIQUE_IDX(_unit, _idxEntry)  (((_idxEntry).index_type == L2_IN_HASH)? (((_idxEntry).index << 2) | (_idxEntry).hashdepth) : (hashTable_size[(_unit)] + (_idxEntry).index))
#define TBL_SIZE_XLATE_P(size) ((gRTL9311E ? ((size) * 2) : (size)))
#define HASH_IDX_XLATE_P(index_l) ((gRTL9311E && (index_l) >= 2048 && (index_l) < 4096 ? ((index_l) + 2048) : (index_l)))
#define TBL_IDX_XLATE_L(index_p) ((gRTL9311E && (index_p) >= 16384 ? (gRTL9311E && (index_p) >= 32768 ? ((index_p) - 16384) : ((index_p) - 8192)) : (index_p)))
#define TBL_IDX_XLATE_P(index_l) (gRTL9311E && (index_l) >= 8192 && (index_l) < 16384 ? (index_l + 8192) : (index_l))


/*
 * Function Declaration
 */
static int32 _dal_mango_l2_init_config(uint32 unit);
static int32 _dal_mango_l2_getExistOrFreeL2Entry(uint32 unit, dal_mango_l2_entry_t *pL2_entry, dal_mango_l2_getMethod_t get_method, dal_mango_l2_index_t *pL2_index);
static int32 _dal_mango_l2_getFirstDynamicEntry(uint32 unit, dal_mango_l2_entry_t *pL2_entry, dal_mango_l2_index_t *pL2_index);
static int32 _dal_mango_l2_compareEntry(dal_mango_l2_entry_t *pSrcEntry, dal_mango_l2_entry_t *pDstEntry);
static int32 _dal_mango_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac, uint32 sram_if);


int32 _dal_mango_l2_semLock(uint32 unit)
{
    L2_SEM_LOCK(unit);
    return RT_ERR_OK;
}

int32 _dal_mango_l2_semUnlock(uint32 unit)
{
    L2_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

int32 _dal_mango_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex)
{
    int16   free_idx;
    int32   ret = RT_ERR_OK;

    RT_PARAM_CHK(NULL == pFwdIndex, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(*pFwdIndex >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);


    if (*pFwdIndex >= 0)
    {
        mcast_idx_pool[unit].pMcast_index_pool[*pFwdIndex].ref_count++;
        goto errOk;
    }

    while (mcast_idx_pool[unit].free_index_head != END_OF_MCAST_IDX)
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
    return ret;
}

int32 _dal_mango_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index)
{
    int32 ret = RT_ERR_OK;
    rtk_portmask_t portmask;

    RT_PARAM_CHK(index >= mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK(index < 0, RT_ERR_L2_MULTI_FWD_INDEX);


    if(mcast_idx_pool[unit].pMcast_index_pool[index].ref_count==0)
        return RT_ERR_OK;

    mcast_idx_pool[unit].pMcast_index_pool[index].ref_count--;

    if (0 == mcast_idx_pool[unit].pMcast_index_pool[index].ref_count)
    {
        mcast_idx_pool[unit].pMcast_index_pool[index].next_index = mcast_idx_pool[unit].free_index_head;
        mcast_idx_pool[unit].free_index_head = index;
        mcast_idx_pool[unit].free_entry_count++;

        osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
        if((ret=_dal_mango_l2_mcastFwdPortmaskEntry_set(unit, index, &portmask))!= RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
            goto errExit;
        }
    }

errExit:
    return ret;
}

int32 _dal_mango_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret;
    multicast_index_entry_t mcast_entry;

    osal_memset(&mcast_entry, 0, sizeof(mcast_entry));

    if ((ret = table_read(unit, MANGO_MC_PMSKt, index, (uint32 *)&mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_MC_PMSKt, MANGO_MC_PMSK_PMSKtf, &(pPortmask->bits[0]), (uint32 *) &mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x", ret);
        return ret;
    }

    return RT_ERR_OK;
}

int32 _dal_mango_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret;
    multicast_index_entry_t mcast_entry;

    osal_memset(&mcast_entry, 0, sizeof(mcast_entry));

    if ((ret = table_field_set(unit, MANGO_MC_PMSKt, MANGO_MC_PMSK_PMSKtf, &(pPortmask->bits[0]), (uint32 *) &mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x", ret);
        return ret;
    }

    if ((ret = table_write(unit, MANGO_MC_PMSKt, index, (uint32 *)&mcast_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x", ret);
        return ret;
    }

    return RT_ERR_OK;
}

int32 _dal_mango_l2_entryToHashIdx(uint32 unit, dal_mango_l2_entry_t *pL2_entry, dal_mango_l2_hashIdx_t *pIdx)
{
    uint64  hashSeed = 0;
    uint32  hash0, hash1, hash2, hash3, hash4;
    uint32  hash0_idx;
    uint32  hash1_idx;


    if (pL2_entry->entry_type == L2_UNICAST)
    {
        hashSeed = (((uint64)pL2_entry->unicast.fid << 48) |
                    ((uint64)pL2_entry->unicast.mac.octet[0] << 40) |
                    ((uint64)pL2_entry->unicast.mac.octet[1] << 32) |
                    ((uint64)pL2_entry->unicast.mac.octet[2] << 24) |
                    ((uint64)pL2_entry->unicast.mac.octet[3] << 16) |
                    ((uint64)pL2_entry->unicast.mac.octet[4] << 8) |
                    ((uint64)pL2_entry->unicast.mac.octet[5]));
    }
    else    /* L2_MULTICAST */
    {
        hashSeed = (((uint64)pL2_entry->l2mcast.fid << 48) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[0] << 40) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[1] << 32) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[2] << 24) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[3] << 16) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[4] << 8) |
                    ((uint64)pL2_entry->l2mcast.mac.octet[5]));
    }

    /* Algo 0 */
    hash0 = (hashSeed) & BITMASK_12B;
    hash1 = (hashSeed >> 12) & BITMASK_12B;
    hash2 = (hashSeed >> 24) & BITMASK_12B;
    hash3 = (hashSeed >> 36) & BITMASK_12B;
    hash4 = (hashSeed >> 48) & BITMASK_12B;
    hash4 = ((hash4 & BITMASK_3B) << 9) | ((hash4 >> 3) & BITMASK_9B);
    hash0_idx = (uint32)(hash4 ^ hash3 ^ hash2 ^ hash1 ^ hash0);
    if (gRTL9311E)
        hash0_idx &= 0x7ff;


    /* Algo 1 */
    hash0 = (hashSeed) & BITMASK_12B;
    hash0 = ((hash0 & BITMASK_9B) << 3) | ((hash0 >> 9) & BITMASK_3B);
    hash1 = (hashSeed >> 12) & BITMASK_12B;
    hash1 = ((hash1 & BITMASK_6B) << 6) | ((hash1 >> 6) & BITMASK_6B);
    hash2 = (hashSeed >> 24) & BITMASK_12B;
    hash3 = (hashSeed >> 36) & BITMASK_12B;
    hash3 = ((hash3 & BITMASK_6B) << 6) | ((hash3 >> 6) & BITMASK_6B);
    hash4 = (hashSeed >> 48) & BITMASK_12B;
    hash1_idx = (uint32)(hash4 ^ hash3 ^ hash2 ^ hash1 ^ hash0);
    if (gRTL9311E)
        hash1_idx &= 0x7ff;

    if (blkAlgoType[0][unit]==0)
        pIdx->blk0_hashIdx = hash0_idx;
    else
        pIdx->blk0_hashIdx = hash1_idx;

    if (blkAlgoType[1][unit]==0)
        pIdx->blk1_hashIdx = hash0_idx + ((hashTable_size[unit]/2)>>2);
    else
        pIdx->blk1_hashIdx = hash1_idx + ((hashTable_size[unit]/2)>>2);

    return RT_ERR_OK;
}

int32 _dal_mango_l2_hashIdxToVid(uint32 unit, dal_mango_l2_entry_t *pL2_entry, dal_mango_l2_index_t *pIndex)
{
    uint32                  idx = pIndex->index;
    uint32                  blkId;
    dal_mango_l2_hashIdx_t  partialkey1;


    osal_memset(&partialkey1, 0, sizeof(dal_mango_l2_hashIdx_t));

    if (pIndex->index < (hashTable_size[unit] >> 3))
        blkId = 0;
    else
        blkId = 1;

    if (0 == blkId)
    {
        switch (pL2_entry->entry_type)
        {
            case L2_UNICAST:
                if (blkAlgoType[0][unit] == 0)
                    pL2_entry->unicast.fid = ((idx & 0x1ff) << 3) | ((idx) >> 9);
                else
                    pL2_entry->unicast.fid = idx;
                _dal_mango_l2_entryToHashIdx(unit, pL2_entry, &partialkey1);
                if (blkAlgoType[0][unit] == 0)
                    pL2_entry->unicast.fid = ((partialkey1.blk0_hashIdx & 0x1ff) << 3) | ((partialkey1.blk0_hashIdx & 0xfff) >> 9);
                else
                    pL2_entry->unicast.fid = partialkey1.blk0_hashIdx;
                break;

            case L2_MULTICAST:
                if (blkAlgoType[0][unit] == 0)
                    pL2_entry->l2mcast.fid = ((idx & 0x1ff) << 3) | ((idx) >> 9);
                else
                    pL2_entry->l2mcast.fid = idx;
                _dal_mango_l2_entryToHashIdx(unit, pL2_entry, &partialkey1);
                if (blkAlgoType[0][unit] == 0)
                    pL2_entry->l2mcast.fid = ((partialkey1.blk0_hashIdx & 0x1ff) << 3) | ((partialkey1.blk0_hashIdx & 0xfff) >> 9);
                else
                    pL2_entry->l2mcast.fid = partialkey1.blk0_hashIdx;
                break;

            default:
                return RT_ERR_FAILED;
        }
    }
    else if (1 == blkId)
    {
        idx = idx - 4096;
        switch (pL2_entry->entry_type)
        {
            case L2_UNICAST:
                if (blkAlgoType[1][unit] == 0)
                    pL2_entry->unicast.fid = ((idx & 0x1ff) << 3) | ((idx & 0xfff) >> 9);
                else
                    pL2_entry->unicast.fid = idx;
                _dal_mango_l2_entryToHashIdx(unit, pL2_entry, &partialkey1);
                if (blkAlgoType[1][unit] == 0)
                    pL2_entry->unicast.fid = ((partialkey1.blk1_hashIdx & 0xff) << 3) | ((partialkey1.blk1_hashIdx & 0xfff) >> 9);
                else
                    pL2_entry->unicast.fid = partialkey1.blk1_hashIdx - ((hashTable_size[unit]/2)>>2);
                break;

            case L2_MULTICAST:
                if (blkAlgoType[1][unit] == 0)
                    pL2_entry->l2mcast.fid = ((idx & 0x1ff) << 3) | ((idx & 0xfff) >> 9);
                else
                    pL2_entry->l2mcast.fid = idx;
                _dal_mango_l2_entryToHashIdx(unit, pL2_entry, &partialkey1);
                if (blkAlgoType[1][unit] == 0)
                    pL2_entry->l2mcast.fid = ((partialkey1.blk1_hashIdx & 0xff) << 3) | ((partialkey1.blk1_hashIdx & 0xfff) >> 9);
                else
                    pL2_entry->l2mcast.fid = partialkey1.blk1_hashIdx - ((hashTable_size[unit]/2)>>2);
                break;

            default:
                return RT_ERR_FAILED;
        }
    }
    else
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

int32 _dal_mango_l2_getL2Entry(uint32 unit, dal_mango_l2_index_t * pIndex, dal_mango_l2_entry_t *pL2_entry)
{
    int32       ret;
    uint32      is_l2mcast;
    uint32      mac_uint32[2];
    uint32      next_hop = 0;
    rtk_fid_t   fid = 0;
    uint32      valid = 0, fmt = 0, nTo1Ebl = 0;
    uint32      entry_type = 0;
    l2_entry_t  l2_entry;

    osal_memset(pL2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memset(&l2_entry, 0, sizeof(l2_entry_t));

    /* read entry from chip */
    if (pIndex->index_type == L2_IN_HASH)
    {
        if ((ret = table_read(unit, MANGO_L2_UCt, ((HASH_IDX_XLATE_P(pIndex->index) << 2) | (pIndex->hashdepth)), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    else
    {
        if ((ret = table_read(unit, MANGO_L2_CAM_UCt, (pIndex->index), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_VALIDtf, &valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* Check entry valid */
    if (!valid)
    {
        pL2_entry->is_entry_valid = 0;
        return RT_ERR_OK;
    }
    pL2_entry->is_entry_valid = 1;

    if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_FMTtf, &fmt, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* fmt = 0: L2 */
    /* fmt = 1: OpenFlow */
    if (fmt == 1)
    {
        pL2_entry->entry_type = ENTRY_OPENFLOW;
        return RT_ERR_OK;
    }

    if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_ENTRY_TYPEtf, &entry_type, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* entry_type = 0: L2UC or L2MC */
    /* entry_type = 1: PE forwarding */
    if (entry_type != 0)
    {
        pL2_entry->entry_type = ENTRY_PE_FWD;
        return RT_ERR_OK;
    }

    if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_FIDtf, &fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_MACtf, &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_NEXT_HOPtf, &next_hop, (uint32 *)&l2_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    is_l2mcast = (uint8) ((mac_uint32[1] >> 8) & BITMASK_1B);

    if (is_l2mcast)
    {
        if ((ret = table_field_get(unit, MANGO_L2_MCt, MANGO_L2_MC_LOCAL_FWDtf, &pL2_entry->l2mcast.local_fwd, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_MCt, MANGO_L2_MC_REMOTE_FWDtf, &pL2_entry->l2mcast.remote_fwd, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        pL2_entry->entry_type = L2_MULTICAST;

        pL2_entry->l2mcast.valid        = TRUE;
        pL2_entry->l2mcast.mac.octet[0] = (uint8)(mac_uint32[1] >> 8);
        pL2_entry->l2mcast.mac.octet[1] = (uint8)(mac_uint32[1]);
        pL2_entry->l2mcast.mac.octet[2] = (uint8)(mac_uint32[0] >> 24);
        pL2_entry->l2mcast.mac.octet[3] = (uint8)(mac_uint32[0] >> 16);
        pL2_entry->l2mcast.mac.octet[4] = (uint8)(mac_uint32[0] >> 8);
        pL2_entry->l2mcast.mac.octet[5] = (uint8)(mac_uint32[0]);
        pL2_entry->l2mcast.nh           = next_hop;
        if (IF_CHIP_TYPE_1(unit) && pIndex->index_type == L2_IN_HASH)
            _dal_mango_l2_hashIdxToVid(unit, pL2_entry, pIndex);
        else
            pL2_entry->l2mcast.fid = fid;

        if ((ret = table_field_get(unit, MANGO_L2_MCt, MANGO_L2_MC_MC_PMSK_IDXtf, &pL2_entry->l2mcast.index, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_MCt, MANGO_L2_MC_L2_TNL_LST_IDXtf, &pL2_entry->l2mcast.tunnel_list_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    else
    {
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_L2_TNLtf, &pL2_entry->unicast.l2_tnl_if, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        pL2_entry->entry_type = L2_UNICAST;
        pL2_entry->unicast.valid        = TRUE;
        pL2_entry->unicast.mac.octet[0] = (uint8)(mac_uint32[1] >> 8);
        pL2_entry->unicast.mac.octet[1] = (uint8)(mac_uint32[1]);
        pL2_entry->unicast.mac.octet[2] = (uint8)(mac_uint32[0] >> 24);
        pL2_entry->unicast.mac.octet[3] = (uint8)(mac_uint32[0] >> 16);
        pL2_entry->unicast.mac.octet[4] = (uint8)(mac_uint32[0] >> 8);
        pL2_entry->unicast.mac.octet[5] = (uint8)(mac_uint32[0]);
        if (IF_CHIP_TYPE_1(unit) && pIndex->index_type == L2_IN_HASH)
            _dal_mango_l2_hashIdxToVid(unit, pL2_entry, pIndex);
        else
            pL2_entry->unicast.fid = fid;
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_IS_TRKtf, &pL2_entry->unicast.is_trk, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_SPAtf, &pL2_entry->unicast.slp, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_AGEtf, &pL2_entry->unicast.aging, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_SA_BLKtf, &pL2_entry->unicast.sablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_DA_BLKtf, &pL2_entry->unicast.dablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_STATICtf, &pL2_entry->unicast.is_static, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_SUSPENDtf, &pL2_entry->unicast.suspending, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        pL2_entry->unicast.nh = next_hop;

        if (pL2_entry->unicast.l2_tnl_if)
        {
            /* L2 tunnel index (share the field with AGG_VID) */
            if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_AGG_VIDtf, &pL2_entry->unicast.l2_tnl_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        } else {
            if ((ret = reg_field_read(unit, MANGO_VLAN_L2TBL_CNVT_CTRLr, MANGO_GLB_VID_CNVT_ENf, &nTo1Ebl)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if (nTo1Ebl)
            {
                if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_TAGSTStf, &pL2_entry->unicast.tagSts, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_AGG_PRItf, &pL2_entry->unicast.agg_pri, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_get(unit, MANGO_L2_UCt, MANGO_L2_UC_AGG_VIDtf, &pL2_entry->unicast.agg_vid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
            }
            else
            {
                if ((ret = table_field_get(unit, MANGO_L2_CB_UCt, MANGO_L2_CB_UC_ECIDtf, &pL2_entry->unicast.ecid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
            }
        }
    }

    return RT_ERR_OK;
}

int32 _dal_mango_l2_setL2Entry(uint32 unit, dal_mango_l2_index_t * pIndex, dal_mango_l2_entry_t *pL2_entry)
{
    int32 ret;
    l2_entry_t  l2_entry;
    uint32  mac_uint32[2] ={0, 0};
    uint32  value = 0, nTo1Ebl = 0, hash_algo = 0;
    uint32  regField;

    osal_memset(&l2_entry, 0, sizeof(l2_entry));
    if (((HASH_IDX_XLATE_P(pIndex->index) << 2) | (pIndex->hashdepth)) < 16384)
        regField = MANGO_L2_HASH_ALGO_BLK0f;
    else
        regField = MANGO_L2_HASH_ALGO_BLK1f;

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, regField, &hash_algo)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (pL2_entry->entry_type == L2_UNICAST)
    {
        /* L2_UNICAST */
        mac_uint32[1] = ((uint32)pL2_entry->unicast.mac.octet[0]) << 8;
        mac_uint32[1] |= ((uint32)pL2_entry->unicast.mac.octet[1]);
        mac_uint32[0] = ((uint32)pL2_entry->unicast.mac.octet[2]) << 24;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[3]) << 16;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[4]) << 8;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[5]);

        if (pIndex->index_type == L2_IN_HASH)
        {
            if (hash_algo == 0)
                value = (pL2_entry->unicast.fid >> 2) & 0x1;
            else
                value = (pL2_entry->unicast.fid >> 11) & 0x1;


            if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_HASH_MSBtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_FIDtf, &pL2_entry->unicast.fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_MACtf, &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_VALIDtf, &pL2_entry->unicast.valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        value = 0;
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_FMTtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        value = 0;
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_ENTRY_TYPEtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_L2_TNLtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_IS_TRKtf, &pL2_entry->unicast.is_trk, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_SPAtf, &pL2_entry->unicast.slp, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_AGEtf, &pL2_entry->unicast.aging, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_SA_BLKtf, &pL2_entry->unicast.sablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_DA_BLKtf, &pL2_entry->unicast.dablock, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_STATICtf, &pL2_entry->unicast.is_static, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_SUSPENDtf, &pL2_entry->unicast.suspending, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_NEXT_HOPtf, &pL2_entry->unicast.nh, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if (pL2_entry->unicast.l2_tnl_if)
        {
            /* L2 tunnel bit */
            if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_L2_TNLtf, &pL2_entry->unicast.l2_tnl_if, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            /* L2 tunnel index (share the field with AGG_VID) */
            if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_AGG_VIDtf, &pL2_entry->unicast.l2_tnl_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        } else {
            if ((ret = reg_field_read(unit, MANGO_VLAN_L2TBL_CNVT_CTRLr, MANGO_GLB_VID_CNVT_ENf, &nTo1Ebl)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if (nTo1Ebl)
            {
                if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_TAGSTStf, &pL2_entry->unicast.tagSts, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_AGG_PRItf, &pL2_entry->unicast.agg_pri, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_AGG_VIDtf, &pL2_entry->unicast.agg_vid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
            }
            else
            {
                if ((ret = table_field_set(unit, MANGO_L2_CB_UCt, MANGO_L2_CB_UC_ECIDtf, &pL2_entry->unicast.ecid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
            }
        }
    }
    else if (pL2_entry->entry_type == L2_MULTICAST)
    {
        /* L2_MULTICAST */
        mac_uint32[1] = ((uint32)pL2_entry->l2mcast.mac.octet[0]) << 8;
        mac_uint32[1] |= ((uint32)pL2_entry->l2mcast.mac.octet[1]);
        mac_uint32[0] = ((uint32)pL2_entry->l2mcast.mac.octet[2]) << 24;
        mac_uint32[0] |= ((uint32)pL2_entry->l2mcast.mac.octet[3]) << 16;
        mac_uint32[0] |= ((uint32)pL2_entry->l2mcast.mac.octet[4]) << 8;
        mac_uint32[0] |= ((uint32)pL2_entry->l2mcast.mac.octet[5]);

        if (pIndex->index_type == L2_IN_HASH)
        {
            if (hash_algo == 0)
                value = (pL2_entry->l2mcast.fid >> 2) & 0x1;
            else
                value = (pL2_entry->l2mcast.fid >> 11) & 0x1;


            if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_HASH_MSBtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_FIDtf, &pL2_entry->l2mcast.fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_MACtf, &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_VALIDtf, &pL2_entry->l2mcast.valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        value = 0; /* 0 for L2 entry */
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_FMTtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        value = 0; /* 0 for L2 entry */
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_ENTRY_TYPEtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_LOCAL_FWDtf, &pL2_entry->l2mcast.local_fwd, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_MC_PMSK_IDXtf, &pL2_entry->l2mcast.index, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_NEXT_HOPtf, &pL2_entry->l2mcast.nh, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_REMOTE_FWDtf, &pL2_entry->l2mcast.remote_fwd, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_L2_TNL_LST_IDXtf, &pL2_entry->l2mcast.tunnel_list_idx, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    else
    {
        /* patch for link-down flush  */
        mac_uint32[1] = ((uint32)pL2_entry->unicast.mac.octet[0]) << 8;
        mac_uint32[1] |= ((uint32)pL2_entry->unicast.mac.octet[1]);
        mac_uint32[0] = ((uint32)pL2_entry->unicast.mac.octet[2]) << 24;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[3]) << 16;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[4]) << 8;
        mac_uint32[0] |= ((uint32)pL2_entry->unicast.mac.octet[5]);

        value = 1;
        if ((ret = table_field_set(unit, MANGO_L2_MCt, MANGO_L2_MC_FMTtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_ENTRY_TYPEtf, &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_FIDtf, &pL2_entry->unicast.fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_MACtf, &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_L2_UCt, MANGO_L2_UC_VALIDtf, &pL2_entry->unicast.valid, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    if (pIndex->index_type == L2_IN_HASH)
    {
        if ((ret = table_write(unit, MANGO_L2_UCt, ((HASH_IDX_XLATE_P(pIndex->index) << 2) | (pIndex->hashdepth)), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    else
    {
        if ((ret = table_write(unit, MANGO_L2_CAM_UCt, (pIndex->index), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_mango_l2_compareEntry(dal_mango_l2_entry_t *pSrcEntry, dal_mango_l2_entry_t *pDstEntry)
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

static int32 _dal_mango_l2_getExistOrFreeL2Entry(uint32 unit, dal_mango_l2_entry_t *pL2_entry, dal_mango_l2_getMethod_t get_method, dal_mango_l2_index_t *pL2_index)
{
    int32           ret;
    uint32          hash_depth;
    uint32          value = 0;
    rtk_enable_t    l2CamEbl;

    uint32 blk0_entryCnt =0;
    uint32 blk1_entryCnt = 0;
    uint32 blk0_freeDepth = 0xffffffff;
    uint32 blk1_freeDepth = 0xffffffff;
    uint32 cam_freeIndex = 0xffffffff;

    dal_mango_l2_hashIdx_t  hashIdx;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    blk0_index;
    dal_mango_l2_index_t    blk1_index;
    dal_mango_l2_index_t    cam_index;

    hash_depth = HAL_L2_HASHDEPTH(unit);
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memset(&hashIdx, 0, sizeof(dal_mango_l2_hashIdx_t));

    /* calculate hash index */
    if ((ret = _dal_mango_l2_entryToHashIdx(unit, pL2_entry, &hashIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blk0_index.index_type = L2_IN_HASH;
    blk0_index.index = hashIdx.blk0_hashIdx;
    for (blk0_index.hashdepth = 0; blk0_index.hashdepth < hash_depth; blk0_index.hashdepth++)
    {
        if ((ret = _dal_mango_l2_getL2Entry(unit, &blk0_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if (l2_entry.is_entry_valid)
        {
            blk0_entryCnt++;
            if (RT_ERR_OK == _dal_mango_l2_compareEntry(pL2_entry, &l2_entry))
            {
                pL2_entry->is_entry_exist = TRUE;
                l2_entry.is_entry_exist = TRUE;
                break;
            }
        }
        else
        {
            if (blk0_freeDepth == 0xffffffff)
                blk0_freeDepth = blk0_index.hashdepth;
        }
    }

    if (l2_entry.is_entry_exist)
    {
        if (get_method == L2_GET_EXIST_ONLY || get_method == L2_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pL2_index, &blk0_index, sizeof(dal_mango_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_mango_l2_entry_t));
            return RT_ERR_OK;
        }
    }

    blk1_index.index_type = L2_IN_HASH;
    blk1_index.index = hashIdx.blk1_hashIdx;
    for (blk1_index.hashdepth = 0; blk1_index.hashdepth < hash_depth; blk1_index.hashdepth++)
    {
        if ((ret = _dal_mango_l2_getL2Entry(unit, &blk1_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if (l2_entry.is_entry_valid)
        {
            blk1_entryCnt++;
            if (RT_ERR_OK == _dal_mango_l2_compareEntry(pL2_entry, &l2_entry))
            {
                pL2_entry->is_entry_exist = TRUE;
                l2_entry.is_entry_exist = TRUE;
                break;
            }
        }
        else
        {
            if (blk1_freeDepth == 0xffffffff)
                blk1_freeDepth = blk1_index.hashdepth;
        }
    }

    if (l2_entry.is_entry_exist)
    {
        if (get_method == L2_GET_EXIST_ONLY || get_method == L2_GET_EXIST_OR_FREE)
        {
            osal_memcpy(pL2_index, &blk1_index, sizeof(dal_mango_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_mango_l2_entry_t));
            return RT_ERR_OK;
        }
    }

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value))!=RT_ERR_OK)
        return ret;
    l2CamEbl = value ? ENABLED : DISABLED;

    if (l2CamEbl == ENABLED && ld_patch_en == FALSE)
    {
        cam_index.index_type = L2_IN_CAM;
        for (cam_index.index = 0; cam_index.index < cam_size[unit]; cam_index.index++)
        {
            if ((ret = _dal_mango_l2_getL2Entry(unit, &cam_index, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if (l2_entry.is_entry_valid)
            {
                if (RT_ERR_OK == _dal_mango_l2_compareEntry(pL2_entry, &l2_entry))
                {
                    pL2_entry->is_entry_exist = TRUE;
                    l2_entry.is_entry_exist = TRUE;
                    break;
                }
            }
            else
            {
                if (cam_freeIndex == 0xffffffff)
                    cam_freeIndex = cam_index.index;
            }
        }

        if (l2_entry.is_entry_exist)
        {
            if (get_method == L2_GET_EXIST_ONLY || get_method == L2_GET_EXIST_OR_FREE)
            {
                osal_memcpy(pL2_index, &cam_index, sizeof(dal_mango_l2_index_t));
                osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_mango_l2_entry_t));
                return RT_ERR_OK;
            }
        }
    }


    if (get_method == L2_GET_EXIST_ONLY)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    if (get_method == L2_GET_EXIST_OR_FREE || get_method == L2_GET_FREE_ONLY)
    {
        if (blk0_freeDepth != 0xffffffff && blk1_freeDepth == 0xffffffff)
        {
            blk0_index.hashdepth = blk0_freeDepth;
            osal_memcpy(pL2_index, &blk0_index, sizeof(dal_mango_l2_index_t));
            return RT_ERR_OK;
        }
        else if (blk0_freeDepth == 0xffffffff && blk1_freeDepth != 0xffffffff)
        {
            blk1_index.hashdepth = blk1_freeDepth;
            osal_memcpy(pL2_index, &blk1_index, sizeof(dal_mango_l2_index_t));
            return RT_ERR_OK;
        }
        else if (blk0_freeDepth != 0xffffffff && blk1_freeDepth != 0xffffffff)
        {
            if (blk0_entryCnt <= blk1_entryCnt)
            {
                blk0_index.hashdepth = blk0_freeDepth;
                osal_memcpy(pL2_index, &blk0_index, sizeof(dal_mango_l2_index_t));
                return RT_ERR_OK;
            }
            else
            {
                blk1_index.hashdepth = blk1_freeDepth;
                osal_memcpy(pL2_index, &blk1_index, sizeof(dal_mango_l2_index_t));
                return RT_ERR_OK;
            }
        }
        else
        {
            if (l2CamEbl == ENABLED && cam_freeIndex != 0xffffffff)
            {
                cam_index.index = cam_freeIndex;
                osal_memcpy(pL2_index, &cam_index, sizeof(dal_mango_l2_index_t));
                return RT_ERR_OK;
            }
            else
                return RT_ERR_L2_NO_EMPTY_ENTRY;
        }
    }

    return RT_ERR_OK;
}

static int32 _dal_mango_l2_getFirstDynamicEntry(uint32 unit, dal_mango_l2_entry_t *pL2_entry, dal_mango_l2_index_t *pL2_index)
{
    int32           ret;
    uint32          hash_depth;
    rtk_enable_t    l2CamEbl;
    uint32          value = 0;

    dal_mango_l2_hashIdx_t hashIdx;
    dal_mango_l2_entry_t l2_entry;
    dal_mango_l2_index_t blk0_index;
    dal_mango_l2_index_t blk1_index;
    dal_mango_l2_index_t cam_index;

    hash_depth = HAL_L2_HASHDEPTH(unit);
    osal_memset(&hashIdx, 0, sizeof(dal_mango_l2_hashIdx_t));
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));

    /* calculate hash index */
    if ((ret = _dal_mango_l2_entryToHashIdx(unit, pL2_entry, &hashIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blk0_index.index_type = L2_IN_HASH;
    blk0_index.index = hashIdx.blk0_hashIdx;
    for (blk0_index.hashdepth = 0; blk0_index.hashdepth < hash_depth; blk0_index.hashdepth++)
    {
        if ((ret = _dal_mango_l2_getL2Entry(unit, &blk0_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if (l2_entry.is_entry_valid &&
            l2_entry.entry_type==L2_UNICAST &&
            l2_entry.unicast.sablock==0 &&
            l2_entry.unicast.dablock==0 &&
            l2_entry.unicast.is_static==0 &&
            l2_entry.unicast.nh==0
            )
        {
            osal_memcpy(pL2_index, &blk0_index, sizeof(dal_mango_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_mango_l2_entry_t));
            return RT_ERR_OK;
        }
    }

    blk1_index.index_type = L2_IN_HASH;
    blk1_index.index = hashIdx.blk1_hashIdx;
    for (blk1_index.hashdepth = 0; blk1_index.hashdepth < hash_depth; blk1_index.hashdepth++)
    {
        if ((ret = _dal_mango_l2_getL2Entry(unit, &blk1_index, &l2_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if (l2_entry.is_entry_valid &&
            l2_entry.entry_type==L2_UNICAST &&
            l2_entry.unicast.sablock==0 &&
            l2_entry.unicast.dablock==0 &&
            l2_entry.unicast.is_static==0 &&
            l2_entry.unicast.nh==0
            )
        {
            osal_memcpy(pL2_index, &blk1_index, sizeof(dal_mango_l2_index_t));
            osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_mango_l2_entry_t));
            return RT_ERR_OK;
        }
    }

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value)) != RT_ERR_OK)
        return ret;
    l2CamEbl = value == 1 ? ENABLED : DISABLED;

    if (l2CamEbl == ENABLED && ld_patch_en == FALSE)
    {
        cam_index.index_type = L2_IN_CAM;
        for (cam_index.index = 0; cam_index.index < cam_size[unit]; cam_index.index++)
        {
            if ((ret = _dal_mango_l2_getL2Entry(unit, &cam_index, &l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if (l2_entry.is_entry_valid &&
                l2_entry.entry_type==L2_UNICAST &&
                l2_entry.unicast.sablock==0 &&
                l2_entry.unicast.dablock==0 &&
                l2_entry.unicast.is_static==0 &&
                l2_entry.unicast.nh==0
                )
            {
                osal_memcpy(pL2_index, &cam_index, sizeof(dal_mango_l2_index_t));
                osal_memcpy(pL2_entry, &l2_entry, sizeof(dal_mango_l2_entry_t));
                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_L2_ENTRY_NOTFOUND;
}

/* Function Name:
 *      _dal_mango_l2_init_config
 * Description:
 *      Initialize config of l2 module for the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling this API.
 */
static int32
_dal_mango_l2_init_config(uint32 unit)
{
    int32           ret;
    rtk_portmask_t  portmask;
    rtk_l2_flushCfg_t   flushCfg;

    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));
    osal_memset(&flushCfg, 0, sizeof(rtk_l2_flushCfg_t));

    /* Aging time */
    if ((ret = dal_mango_l2_agingTime_set(unit, L2_AGE_TIME_NORMAL, 300)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blkAlgoType[0][unit] = 0;
    blkAlgoType[1][unit] = 1;
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, MANGO_L2_HASH_ALGO_BLK0f, &blkAlgoType[0][unit])) != RT_ERR_OK)
        return ret;
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, MANGO_L2_HASH_ALGO_BLK1f, &blkAlgoType[1][unit])) != RT_ERR_OK)
        return ret;

    /* Set source port filter based on valid portmask */
    HWP_GET_ALL_PORTMASK(unit, portmask);
    dal_mango_l2_srcPortEgrFilterMask_set(unit, &portmask);

    flushCfg.act = FLUSH_ACT_FLUSH_ALL_UC;
    dal_mango_l2_ucastAddr_flush(unit, &flushCfg);

    return RT_ERR_OK;
}

int32 _dal_mango_l2_tablePhyIndex_get(uint32 index_l, uint32 *index_p)
{
    if (gRTL9311E && (index_l) >= 8192 && (index_l) < 16384)
        *index_p = index_l + 8192;
    else
        *index_p = index_l;

    return RT_ERR_OK;
}

int32 _dal_mango_l2_tableLogIndex_get(uint32 index_p, uint32 *index_l)
{
    *index_l = TBL_IDX_XLATE_L(index_p);

    return RT_ERR_OK;
}

int32 _dal_mango_l2_hashPhyIndex_get(uint32 index_p, uint32 *index_l)
{
    *index_l = HASH_IDX_XLATE_P(index_p);

    return RT_ERR_OK;
}

/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      dal_mango_l2Mapper_init
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
dal_mango_l2Mapper_init(dal_mapper_t *pMapper)
{
    pMapper->l2_init = dal_mango_l2_init;
    pMapper->l2_flushLinkDownPortAddrEnable_get = dal_mango_l2_flushLinkDownPortAddrEnable_get;
    pMapper->l2_flushLinkDownPortAddrEnable_set = dal_mango_l2_flushLinkDownPortAddrEnable_set;
    pMapper->l2_ucastAddr_flush = dal_mango_l2_ucastAddr_flush;
    pMapper->l2_macLearningCnt_get = dal_mango_l2_macLearningCnt_get;
    pMapper->l2_limitLearningNum_get = dal_mango_l2_limitLearningNum_get;
    pMapper->l2_limitLearningNum_set = dal_mango_l2_limitLearningNum_set;
    pMapper->l2_limitLearningAction_get = dal_mango_l2_limitLearningAction_get;
    pMapper->l2_limitLearningAction_set = dal_mango_l2_limitLearningAction_set;
    pMapper->l2_fidLimitLearningEntry_get = dal_mango_l2_fidLimitLearningEntry_get;
    pMapper->l2_fidLimitLearningEntry_set = dal_mango_l2_fidLimitLearningEntry_set;
    pMapper->l2_fidLearningCnt_reset = dal_mango_l2_fidLearningCnt_reset;
    pMapper->l2_agingTime_get = dal_mango_l2_agingTime_get;
    pMapper->l2_agingTime_set = dal_mango_l2_agingTime_set;
    pMapper->l2_portAgingEnable_get = dal_mango_l2_portAgingEnable_get;
    pMapper->l2_portAgingEnable_set = dal_mango_l2_portAgingEnable_set;
    pMapper->l2_trkAgingEnable_get = dal_mango_l2_trkAgingEnable_get;
    pMapper->l2_trkAgingEnable_set = dal_mango_l2_trkAgingEnable_set;
    pMapper->l2_hashAlgo_get = dal_mango_l2_hashAlgo_get;
    pMapper->l2_hashAlgo_set = dal_mango_l2_hashAlgo_set;
    pMapper->l2_bucketHashAlgo_get = dal_mango_l2_bucketHashAlgo_get;
    pMapper->l2_bucketHashAlgo_set = dal_mango_l2_bucketHashAlgo_set;
    pMapper->l2_learningFullAction_get = dal_mango_l2_learningFullAction_get;
    pMapper->l2_learningFullAction_set = dal_mango_l2_learningFullAction_set;
    pMapper->l2_portNewMacOp_get = dal_mango_l2_portNewMacOp_get;
    pMapper->l2_portNewMacOp_set = dal_mango_l2_portNewMacOp_set;
    pMapper->l2_addr_init = dal_mango_l2_addr_init;
    pMapper->l2_addr_add = dal_mango_l2_addr_add;
    pMapper->l2_addr_del = dal_mango_l2_addr_del;
    pMapper->l2_addr_get = dal_mango_l2_addr_get;
    pMapper->l2_addr_set = dal_mango_l2_addr_set;
    pMapper->l2_addr_delAll = dal_mango_l2_addr_delAll;
    pMapper->l2_nextValidAddr_get = dal_mango_l2_nextValidAddr_get;
    pMapper->l2_mcastAddr_init = dal_mango_l2_mcastAddr_init;
    pMapper->l2_mcastAddr_add = dal_mango_l2_mcastAddr_add;
    pMapper->l2_mcastAddr_del = dal_mango_l2_mcastAddr_del;
    pMapper->l2_mcastAddr_get = dal_mango_l2_mcastAddr_get;
    pMapper->l2_mcastAddr_set = dal_mango_l2_mcastAddr_set;
    pMapper->l2_mcastAddr_addByIndex = dal_mango_l2_mcastAddr_addByIndex;
    pMapper->l2_nextValidMcastAddr_get = dal_mango_l2_nextValidMcastAddr_get;
    pMapper->l2_mcastFwdIndex_alloc = dal_mango_l2_mcastFwdIndex_alloc;
    pMapper->l2_mcastFwdIndex_free = dal_mango_l2_mcastFwdIndex_free;
    pMapper->l2_mcastFwdPortmaskEntry_get = dal_mango_l2_mcastFwdPortmaskEntry_get;
    pMapper->l2_mcastFwdPortmaskEntry_set = dal_mango_l2_mcastFwdPortmaskEntry_set;
    pMapper->l2_cpuMacAddr_add = dal_mango_l2_cpuMacAddr_add;
    pMapper->l2_cpuMacAddr_del = dal_mango_l2_cpuMacAddr_del;
    pMapper->l2_portMoveAction_get = dal_mango_l2_portMoveAction_get;
    pMapper->l2_portMoveAction_set = dal_mango_l2_portMoveAction_set;
    pMapper->l2_portMoveLearn_get = dal_mango_l2_portMoveLearn_get;
    pMapper->l2_portMoveLearn_set = dal_mango_l2_portMoveLearn_set;
    pMapper->l2_lookupMissFloodPortMask_get = dal_mango_l2_lookupMissFloodPortMask_get;
    pMapper->l2_lookupMissFloodPortMask_set = dal_mango_l2_lookupMissFloodPortMask_set;
    pMapper->l2_lookupMissFloodPortMask_add = dal_mango_l2_lookupMissFloodPortMask_add;
    pMapper->l2_lookupMissFloodPortMask_del = dal_mango_l2_lookupMissFloodPortMask_del;
    pMapper->l2_portLookupMissAction_get = dal_mango_l2_portLookupMissAction_get;
    pMapper->l2_portLookupMissAction_set = dal_mango_l2_portLookupMissAction_set;
    pMapper->l2_portUcastLookupMissAction_get = dal_mango_l2_portUcastLookupMissAction_get;
    pMapper->l2_portUcastLookupMissAction_set = dal_mango_l2_portUcastLookupMissAction_set;
    pMapper->l2_srcPortEgrFilterMask_get = dal_mango_l2_srcPortEgrFilterMask_get;
    pMapper->l2_srcPortEgrFilterMask_set = dal_mango_l2_srcPortEgrFilterMask_set;
    pMapper->l2_srcPortEgrFilterMask_add = dal_mango_l2_srcPortEgrFilterMask_add;
    pMapper->l2_srcPortEgrFilterMask_del = dal_mango_l2_srcPortEgrFilterMask_del;
    pMapper->l2_exceptionAddrAction_get = dal_mango_l2_exceptionAddrAction_get;
    pMapper->l2_exceptionAddrAction_set = dal_mango_l2_exceptionAddrAction_set;
    pMapper->l2_addrEntry_get = dal_mango_l2_addrEntry_get;
    pMapper->l2_conflictAddr_get = dal_mango_l2_conflictAddr_get;
    pMapper->l2_zeroSALearningEnable_get = dal_mango_l2_zeroSALearningEnable_get;
    pMapper->l2_zeroSALearningEnable_set = dal_mango_l2_zeroSALearningEnable_set;
    pMapper->l2_portDynamicPortMoveForbidEnable_get = dal_mango_l2_portDynamicPortMoveForbidEnable_get;
    pMapper->l2_portDynamicPortMoveForbidEnable_set = dal_mango_l2_portDynamicPortMoveForbidEnable_set;
    pMapper->l2_trkDynamicPortMoveForbidEnable_get = dal_mango_l2_trkDynamicPortMoveForbidEnable_get;
    pMapper->l2_trkDynamicPortMoveForbidEnable_set = dal_mango_l2_trkDynamicPortMoveForbidEnable_set;
    pMapper->l2_hwNextValidAddr_get = dal_mango_l2_hwNextValidAddr_get;
    pMapper->l2_portMacFilterEnable_get = dal_mango_l2_portMacFilterEnable_get;
    pMapper->l2_portMacFilterEnable_set = dal_mango_l2_portMacFilterEnable_set;
    pMapper->l2_portCtrl_get = dal_mango_l2_portCtrl_get;
    pMapper->l2_portCtrl_set = dal_mango_l2_portCtrl_set;
    pMapper->l2_status_get = dal_mango_l2_status_get;
    pMapper->l2_stkLearningEnable_get = dal_mango_l2_stkLearningEnable_get;
    pMapper->l2_stkLearningEnable_set = dal_mango_l2_stkLearningEnable_set;
    pMapper->l2_stkKeepUcastEntryValid_get = dal_mango_l2_stkKeepUcastEntryValid_get;
    pMapper->l2_stkKeepUcastEntryValid_set = dal_mango_l2_stkKeepUcastEntryValid_set;
    pMapper->l2_entryCnt_get = dal_mango_l2_entryCnt_get;
    pMapper->l2_hashIdx_get = dal_mango_l2_hashIdx_get;
    pMapper->l2_addr_delByMac = dal_mango_l2_addr_delByMac;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
dal_mango_l2_init(uint32 unit)
{
    int32   ret;
#ifndef __BOOTLOADER__
    uint32  index, value, i;
    dal_mango_l2_entry_t l2_entry;
    dal_mango_l2_index_t index_entry;
#endif  /* __BOOTLOADER__ */
    RT_INIT_REENTRY_CHK(l2_init[unit]);
    l2_init[unit] = INIT_NOT_COMPLETED;

#ifndef __BOOTLOADER__
  #ifdef CONFIG_SDK_DRIVER_L2NTFY_R9310
    /* register callback function if it's my unit id */
    if (unit == HWP_MY_UNIT_ID())
    {
        RT_ERR_CHK(r9310_l2ntfy_l2_cb_register(unit, _dal_mango_l2_portNewMacOp_set, _dal_mango_l2_agingTime_set), ret);
    }
  #endif /* CONFIG_SDK_DRIVER_L2NTFY_R9310 */
#endif

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
#ifndef __BOOTLOADER__

    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    mcast_idx_pool[unit].pMcast_index_pool = NULL;
    l2Db[unit].pEntry   = NULL;
    nh_used_cnt[unit]   = NULL;

    if ((HWP_CHIP_ID(unit) == RTL9311E_CHIP_ID))
        gRTL9311E = 1;

    l2_pmsk_tbl_sem[unit] = osal_sem_mutex_create();
    if (0 == l2_pmsk_tbl_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "l2 portmask table semaphore create failed");
        return RT_ERR_FAILED;
    }

    if ((ret = table_size_get(unit, MANGO_L2_UCt, &hashTable_size[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if (gRTL9311E)
        hashTable_size[unit] /= 2;

    if ((ret = table_size_get(unit, MANGO_L2_CAM_UCt, &cam_size[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = table_size_get(unit, MANGO_MC_PMSKt, &mcast_tableSize[unit])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }


    /* allocate memory for free multicast index */
    mcast_idx_pool[unit].pMcast_index_pool = (dal_mango_mcast_index_t *)osal_alloc(mcast_tableSize[unit] * sizeof(dal_mango_mcast_index_t));
    if (NULL == mcast_idx_pool[unit].pMcast_index_pool)
    {
        ret = RT_ERR_MEM_ALLOC;
        RT_ERR(ret, (MOD_DAL|MOD_L2), "memory allocate failed");
        goto errExit;
    }
    mcast_idx_pool[unit].size_of_mcast_fwd_index = mcast_tableSize[unit];
    mcast_idx_pool[unit].free_entry_count = mcast_tableSize[unit];
    /* first free index is 0 */
    mcast_idx_pool[unit].free_index_head = 0;

    /* create free link-list for all entry, from 0 ~ max index - 2 */
    for (index = 0; index < (mcast_tableSize[unit] - 1); index++)
    {
        mcast_idx_pool[unit].pMcast_index_pool[index].next_index = index + 1;
        mcast_idx_pool[unit].pMcast_index_pool[index].ref_count = 0;
    }
    mcast_idx_pool[unit].pMcast_index_pool[mcast_tableSize[unit] - 1].next_index = END_OF_MCAST_IDX;
    mcast_idx_pool[unit].pMcast_index_pool[mcast_tableSize[unit] - 1].ref_count = 0;


    /* init shadow database */
    l2Db[unit].pEntry = osal_alloc((hashTable_size[unit] + cam_size[unit]) * sizeof(dal_mango_l2DrvDb_entry_t));
    if (NULL == l2Db[unit].pEntry)
    {
        ret = RT_ERR_MEM_ALLOC;
        RT_ERR(ret, (MOD_DAL|MOD_L2), "memory allocate failed");
        goto errExit;
    }
    osal_memset(l2Db[unit].pEntry, 0, (hashTable_size[unit] + cam_size[unit]) * sizeof(dal_mango_l2DrvDb_entry_t));
    for (index = 0; index < (hashTable_size[unit] + cam_size[unit]); index++)
    {
        /* set the L2 entry index for each mcast link-list node */
        if (index < hashTable_size[unit])
        {
            l2Db[unit].pEntry[index].mcastNode.l2Idx.index_type = L2_IN_HASH;
            l2Db[unit].pEntry[index].mcastNode.l2Idx.index = (index >> 2);
            l2Db[unit].pEntry[index].mcastNode.l2Idx.hashdepth = (index & 0x3);
        } else {
            l2Db[unit].pEntry[index].mcastNode.l2Idx.index_type = L2_IN_CAM;
            l2Db[unit].pEntry[index].mcastNode.l2Idx.index = (index - hashTable_size[unit]);
            l2Db[unit].pEntry[index].mcastNode.l2Idx.hashdepth = 0;
        }
    }

    nh_used_cnt[unit] = osal_alloc((hashTable_size[unit] + cam_size[unit]) * sizeof(uint16));
    if (NULL == nh_used_cnt[unit])
    {
        ret = RT_ERR_MEM_ALLOC;
        RT_ERR(ret, (MOD_DAL|MOD_L2), "memory allocate failed");
        goto errExit;
    }
    osal_memset(nh_used_cnt[unit], 0, (hashTable_size[unit] + cam_size[unit]) * sizeof(uint16));

    /* patch for link-down flush */
    ld_patch_en = value = 1;
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value)) != RT_ERR_OK)
        goto errExit;
    l2_entry.entry_type = ENTRY_OPENFLOW;
    l2_entry.unicast.valid = 1;
    l2_entry.unicast.fid = 4095;
    l2_entry.unicast.mac.octet[0] = 0xff;
    l2_entry.unicast.mac.octet[1] = 0xff;
    l2_entry.unicast.mac.octet[2] = 0xff;
    l2_entry.unicast.mac.octet[3] = 0xff;
    l2_entry.unicast.mac.octet[4] = 0xff;
    l2_entry.unicast.mac.octet[5] = 0xff;
    index_entry.index_type = L2_IN_CAM;
    for (i = 0; i < cam_size[unit]; i++)
    {
        index_entry.index = i;
        _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
    }

#endif  /* __BOOTLOADER__ */

    for (i = 0; i < ARB_MAX; i++)
        gAging_time[i] = 300;

    /* set init flag to complete init */
    l2_init[unit] = INIT_COMPLETED;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((ret = _dal_mango_l2_init_config(unit)) != RT_ERR_OK)
        {
            l2_init[unit] = INIT_NOT_COMPLETED;
            #ifndef __BOOTLOADER__
            osal_free(mcast_idx_pool[unit].pMcast_index_pool);
            mcast_idx_pool[unit].pMcast_index_pool = 0;
            mcast_idx_pool[unit].free_index_head = 0;
            mcast_idx_pool[unit].free_entry_count = 0;
            mcast_idx_pool[unit].size_of_mcast_fwd_index = 0;
            #endif  /* __BOOTLOADER__ */
            RT_ERR(ret, (MOD_DAL|MOD_L2), "L2 default config initialize failed");
            goto errExit;
        }
    }

    return RT_ERR_OK;

errExit:
    if (NULL != mcast_idx_pool[unit].pMcast_index_pool)
        osal_free(mcast_idx_pool[unit].pMcast_index_pool);
    if (NULL != l2Db[unit].pEntry)
        osal_free(l2Db[unit].pEntry);
    if (NULL != nh_used_cnt[unit])
        osal_free(nh_used_cnt[unit]);

    return ret;
}



/* Function Name:
 *      dal_mango_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pEnable  - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_l2_flushLinkDownPortAddrEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LINK_DOWN_P_INVLDf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit   - unit id
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_l2_flushLinkDownPortAddrEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, enable=%d", unit, enable);

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
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, MANGO_LINK_DOWN_P_INVLDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_ucastAddr_flush
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      None
 */
int32
dal_mango_l2_ucastAddr_flush(uint32 unit, rtk_l2_flushCfg_t *pConfig)
{
    int32   ret;
    uint32  value = 0;
    uint32  data[2] = {0};
    uint32  stk_age_vld_en, ori_ageTime = 300;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pConfig->act >= FLUSH_ACT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->flushByVid >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->flushByVid && ((pConfig->vid < RTK_VLAN_ID_MIN) || (pConfig->vid > RTK_VLAN_ID_MAX))), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pConfig->flushByPort >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pConfig->portOrTrunk>= RTK_ENABLE_END), RT_ERR_INPUT);
    if (pConfig->flushByPort && pConfig->portOrTrunk == ENABLED)
        RT_PARAM_CHK((rtk_unit2devID[unit]==pConfig->devID) && !HWP_PORT_EXIST(unit, pConfig->port), RT_ERR_PORT_ID);
    else if (pConfig->flushByPort == ENABLED && pConfig->portOrTrunk == DISABLED)
        RT_PARAM_CHK((pConfig->trunk >= HAL_MAX_NUM_OF_TRUNK(unit)) , RT_ERR_TRUNK_ID);
    if (pConfig->act == FLUSH_ACT_REPLACE_ALL_UC || pConfig->act == FLUSH_ACT_REPLACE_DYNM_ONLY)
    {
        RT_PARAM_CHK((pConfig->replacePortOrTrunk >= RTK_ENABLE_END), RT_ERR_INPUT);
        RT_PARAM_CHK((ENABLED == pConfig->replacePortOrTrunk && (rtk_unit2devID[unit]==pConfig->replacing_devID) && !HWP_PORT_EXIST(unit, pConfig->replacingPort)), RT_ERR_PORT_ID);
        RT_PARAM_CHK((DISABLED == pConfig->replacePortOrTrunk && pConfig->replacingTrk >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    }
    if (pConfig->act == FLUSH_ACT_CLEAR_AGG_VID)
    {
        RT_PARAM_CHK(((pConfig->flushFlag & RTK_L2_FLUSH_BY_AGGVID) && ((pConfig->agg_vid < RTK_VLAN_ID_MIN) || (pConfig->agg_vid > RTK_VLAN_ID_MAX))), RT_ERR_INPUT);
    }
    if (pConfig->flushFlag & RTK_L2_FLUSH_BY_AGGVID)
    {
        if ((ret = reg_field_read(unit, MANGO_VLAN_L2TBL_CNVT_CTRLr, MANGO_GLB_VID_CNVT_ENf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        RT_PARAM_CHK(value == 0, RT_ERR_L2_N_TO_1_NOT_ENABLED);
    }
    if (pConfig->flushFlag & RTK_L2_FLUSH_BY_ECID)
    {
        if ((ret = reg_field_read(unit, MANGO_VLAN_L2TBL_CNVT_CTRLr, MANGO_GLB_VID_CNVT_ENf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        RT_PARAM_CHK(value != 0, RT_ERR_L2_PE_NOT_ENABLED);
    }


    if (pConfig->act == FLUSH_ACT_REPLACE_ALL_UC || pConfig->act == FLUSH_ACT_REPLACE_DYNM_ONLY)
    {
        value = 1;
        if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_ACTf, &value, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    if (pConfig->flushByVid == ENABLED)
    {
        value = 1;
        if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_FVID_CMPf, &value, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    if((pConfig->act == FLUSH_ACT_CLEAR_AGG_VID || pConfig->act == FLUSH_ACT_FLUSH_ALL_UC || pConfig->act == FLUSH_ACT_FLUSH_DYNM_ONLY) &&
       (pConfig->flushFlag & RTK_L2_FLUSH_BY_AGGVID))
    {
        value = 1;
        if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_AGG_VID_CMPf, &value, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    if (pConfig->flushByPort == ENABLED)
    {
        value = 1;
        if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_PORT_CMPf, &value, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    if (pConfig->flushFlag & RTK_L2_FLUSH_BY_ECID)
    {
        value = 1;
        if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_AGG_VID_CMPf, &value, data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    switch (pConfig->act)
    {
        case FLUSH_ACT_FLUSH_ALL_UC:
        case FLUSH_ACT_REPLACE_ALL_UC:
            value = 0x0;
            break;
        case FLUSH_ACT_FLUSH_DYNM_ONLY:
        case FLUSH_ACT_REPLACE_DYNM_ONLY:
            value = 0x1;
            break;
        case FLUSH_ACT_CLEAR_NEXTHOP:
            value = 0x2;
            break;
        case FLUSH_ACT_CLEAR_AGG_VID:
            value = 0x3;
            break;
        default:
            return RT_ERR_INPUT;
    }
    if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_ENTRY_TYPEf, &value, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_FVIDf, &pConfig->vid, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* Set AGG_VID or ECID */
    if (pConfig->flushFlag & RTK_L2_FLUSH_BY_AGGVID)
        value = pConfig->agg_vid;
    else
        value = pConfig->ecid;
    if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_AGG_VIDf, &value, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    value = (pConfig->portOrTrunk == ENABLED) ? ((pConfig->devID << 6) | pConfig->port) : ((1 << 10) | pConfig->trunk);
    if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_PORT_IDf, &value, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    value = (pConfig->replacePortOrTrunk == ENABLED) ? ((pConfig->replacing_devID << 6) | pConfig->replacingPort) : ((1 << 10) | pConfig->replacingTrk);
    if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_REPLACING_PORT_IDf, &value, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    value = 0;      /* trigger later*/
    if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_STSf, &value, data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_L2_AGE_CTRLr, MANGO_KEEP_AGE_OUT_ENTRY_VALIDf, &stk_age_vld_en)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if (stk_age_vld_en)
    {
        value = 0;
        ori_ageTime = gAging_time[ARB_NORMAL_API];
        _dal_mango_l2_agingTime_set(unit, L2_AGE_TIME_NORMAL, 0, ARB_NORMAL_API);

        value = 0;
        if ((ret = reg_field_write(unit, MANGO_L2_AGE_CTRLr, MANGO_KEEP_AGE_OUT_ENTRY_VALIDf, &value)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    /* Step 1: program value to CHIP*/
    if ((ret = reg_write(unit, MANGO_L2_TBL_FLUSH_CTRLr, data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* Step 2: trigger flush/replace */
    value = 1;
    if ((ret = reg_field_set(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_STSf, &value, data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_write(unit, MANGO_L2_TBL_FLUSH_CTRLr, data)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    while(1)
    {
        if ((ret = reg_field_read(unit, MANGO_L2_TBL_FLUSH_CTRLr, MANGO_STSf, &value)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        if (value == 0)
            break;
    }


    if (stk_age_vld_en)
    {
        if ((ret = reg_field_write(unit, MANGO_L2_AGE_CTRLr, MANGO_KEEP_AGE_OUT_ENTRY_VALIDf, &stk_age_vld_en)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        _dal_mango_l2_agingTime_set(unit, L2_AGE_TIME_NORMAL, ori_ageTime, ARB_NORMAL_API);
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_macLearningCnt_get
 * Description:
 *      Get the total mac learning counts of the whole specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pMac_cnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. The valid range of rtk_l2_macCnt_t.fidCnt.entryId is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 */
int32
dal_mango_l2_macLearningCnt_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLimitCnt), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pLimitCnt->portTrkCnt.id), RT_ERR_PORT_ID);
            break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.id >= STANDALONE_TRUNK_ID_MAX), RT_ERR_TRUNK_ID);
            break;
        case L2_MAC_LIMIT_FID:
            RT_PARAM_CHK((pLimitCnt->fidCnt.entryId >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);
            break;
        default:
            break;
    }

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if ((ret = reg_field_read(unit, MANGO_L2_LRN_CONSTRT_CNTr, MANGO_LRN_CNTf, &pLimitCnt->glbCnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_PORT:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_PORT_CONSTRT_CNTr, pLimitCnt->portTrkCnt.id, REG_ARRAY_INDEX_NONE, MANGO_LRN_CNTf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_TRUNK:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_TRK_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, pLimitCnt->portTrkCnt.id, MANGO_LRN_CNTf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_FID:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, pLimitCnt->fidCnt.entryId, MANGO_LRN_CNTf, &pLimitCnt->fidCnt.cnt)) != RT_ERR_OK)
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
 *      dal_mango_l2_limitLearningNum_get
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
dal_mango_l2_limitLearningNum_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLimitCnt), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pLimitCnt->portTrkCnt.id), RT_ERR_PORT_ID);
            break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.id >= STANDALONE_TRUNK_ID_MAX), RT_ERR_TRUNK_ID);
            break;
        case L2_MAC_LIMIT_FID:
            RT_PARAM_CHK((pLimitCnt->fidCnt.entryId >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);
            break;
        default:
            break;
    }

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if ((ret = reg_field_read(unit, MANGO_L2_LRN_CONSTRT_CTRLr, MANGO_CONSTRT_NUMf, &pLimitCnt->glbCnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (pLimitCnt->glbCnt == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
                pLimitCnt->glbCnt = L2_MAC_CST_DISABLE;
            break;
        case L2_MAC_LIMIT_PORT:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_PORT_CONSTRT_CTRLr, pLimitCnt->portTrkCnt.id, REG_ARRAY_INDEX_NONE, MANGO_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (pLimitCnt->portTrkCnt.cnt == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
                pLimitCnt->portTrkCnt.cnt = L2_MAC_CST_DISABLE;
            break;
        case L2_MAC_LIMIT_TRUNK:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pLimitCnt->portTrkCnt.id, MANGO_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            if (pLimitCnt->portTrkCnt.cnt == HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit))
                pLimitCnt->portTrkCnt.cnt = L2_MAC_CST_DISABLE;
            break;
        case L2_MAC_LIMIT_FID:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, pLimitCnt->fidCnt.entryId, MANGO_CONSTRT_NUMf, &pLimitCnt->fidCnt.cnt)) != RT_ERR_OK)
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
 *      dal_mango_l2_limitLearningNum_set
 * Description:
 *      Set the mac limit learning counts of specified device.
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
dal_mango_l2_limitLearningNum_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLimitCnt), RT_ERR_NULL_POINTER);

    switch (type)
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
            RT_PARAM_CHK((pLimitCnt->portTrkCnt.id >= STANDALONE_TRUNK_ID_MAX), RT_ERR_TRUNK_ID);
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

    switch (type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if ((ret = reg_field_write(unit, MANGO_L2_LRN_CONSTRT_CTRLr, MANGO_CONSTRT_NUMf, &pLimitCnt->glbCnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_PORT:
            if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_PORT_CONSTRT_CTRLr, pLimitCnt->portTrkCnt.id, REG_ARRAY_INDEX_NONE, MANGO_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_TRUNK:
            if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pLimitCnt->portTrkCnt.id, MANGO_CONSTRT_NUMf, &pLimitCnt->portTrkCnt.cnt)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_FID:
            if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, pLimitCnt->fidCnt.entryId, MANGO_CONSTRT_NUMf, &pLimitCnt->fidCnt.cnt)) != RT_ERR_OK)
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
 *      dal_mango_l2_limitLearningAction_get
 * Description:
 *      Get the mac limit action of specified device.
 * Input:
 *      unit    - unit id
 *      type    - mac limit type
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
dal_mango_l2_limitLearningAction_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    int32   ret;
    uint32  value = 0;
    rtk_action_t action;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pAction->portTrkAct.id), RT_ERR_PORT_ID);
            break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pAction->portTrkAct.id >= STANDALONE_TRUNK_ID_MAX), RT_ERR_TRUNK_ID);
            break;
        default:
            break;
    }

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if ((ret = reg_field_read(unit, MANGO_L2_LRN_CONSTRT_CTRLr, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_PORT:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_PORT_CONSTRT_CTRLr, pAction->portTrkAct.id, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_TRUNK:
            if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pAction->portTrkAct.id, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_FID:
            if ((ret = reg_field_read(unit, MANGO_L2_VLAN_CONSTRT_CTRLr, MANGO_ACTf, &value)) != RT_ERR_OK)
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

    switch (value)
    {
        case 0:
            action = ACTION_FORWARD;
            break;
        case 1:
            action = ACTION_DROP;
            break;
        case 2:
            action = ACTION_TRAP2CPU;
            break;
        case 3:
            action = ACTION_COPY2CPU;
            break;
        case 4:
            action = ACTION_TRAP2MASTERCPU;
            break;
        case 5:
            action = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    switch (type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            pAction->glbAct = action;
            break;
        case L2_MAC_LIMIT_PORT:
        case L2_MAC_LIMIT_TRUNK:
            pAction->portTrkAct.act = action;
            break;
        case L2_MAC_LIMIT_FID:
            pAction->fidAct.act = action;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_limitLearningAction_set
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
dal_mango_l2_limitLearningAction_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    int32   ret;
    uint32  value = 0;
    rtk_action_t action;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_MAC_LIMIT_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case L2_MAC_LIMIT_PORT:
            RT_PARAM_CHK(!HWP_PORT_EXIST(unit, pAction->portTrkAct.id), RT_ERR_PORT_ID);break;
        case L2_MAC_LIMIT_TRUNK:
            RT_PARAM_CHK((pAction->portTrkAct.id >= STANDALONE_TRUNK_ID_MAX), RT_ERR_TRUNK_ID);break;
        default:
            break;
    }

    switch (type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            action = pAction->glbAct;
            break;
        case L2_MAC_LIMIT_PORT:
        case L2_MAC_LIMIT_TRUNK:
            action= pAction->portTrkAct.act;
            break;
        case L2_MAC_LIMIT_FID:
            action = pAction->fidAct.act;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (action)
    {
        case ACTION_FORWARD:
            value = 0;
            break;
        case ACTION_DROP:
            value = 1;
            break;
        case ACTION_TRAP2CPU:
            value = 2;
            break;
        case ACTION_COPY2CPU:
            value = 3;
            break;
        case ACTION_TRAP2MASTERCPU:
            value=4;
            break;
        case ACTION_COPY2MASTERCPU:
            value=5;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_MAC_LIMIT_GLOBAL:
            if ((ret = reg_field_write(unit, MANGO_L2_LRN_CONSTRT_CTRLr, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_PORT:
            if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_PORT_CONSTRT_CTRLr, pAction->portTrkAct.id, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_TRUNK:
            if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_TRK_CONSTRT_CTRLr, REG_ARRAY_INDEX_NONE, pAction->portTrkAct.id, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_MAC_LIMIT_FID:
            if ((ret = reg_field_write(unit, MANGO_L2_VLAN_CONSTRT_CTRLr, MANGO_ACTf, &value)) != RT_ERR_OK)
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
 *      dal_mango_l2_fidLimitLearningEntry_get
 * Description:
 *      Get FID MAC limit entry.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of FID MAC limit entry
 * Output:
 *      pFidMacLimitEntry - pointer to MAC limit entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
dal_mango_l2_fidLimitLearningEntry_get(uint32 unit, uint32 fid_macLimit_idx, rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    int32   ret;
    uint32  value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, fid_macLimit_idx=%d", unit, fid_macLimit_idx);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((fid_macLimit_idx >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pFidMacLimitEntry), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, MANGO_FVIDf, &pFidMacLimitEntry->fid)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, MANGO_PORT_IDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, MANGO_CONSTRT_NUMf, &pFidMacLimitEntry->maxNum)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    if (value & (1 << 6))
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
 *      dal_mango_l2_fidLimitLearningEntry_set
 * Description:
 *      Set FID MAC limit entry.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of FID MAC limit entry
 *      pFidMacLimitEntry - MAC limit entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_VLAN_VID         - invalid vlan id
 * Note:
 *      Forwarding action is as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
dal_mango_l2_fidLimitLearningEntry_set(uint32 unit, uint32 fid_macLimit_idx, rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    int32   ret;
    uint32  value = 0;
    rtk_port_t  lPort;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, fid_macLimit_idx=%d", unit, fid_macLimit_idx);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);


    if (pFidMacLimitEntry->port == L2_PORT_DONT_CARE)
        lPort = PORT_DONT_CARE_9310;
    else
        lPort = pFidMacLimitEntry->port;

    /* parameter check */
    RT_PARAM_CHK((fid_macLimit_idx >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pFidMacLimitEntry), RT_ERR_NULL_POINTER);
    if (pFidMacLimitEntry->maxNum == L2_MAC_CST_DISABLE)
        pFidMacLimitEntry->maxNum = HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit);
    RT_PARAM_CHK((pFidMacLimitEntry->maxNum > HAL_L2_LEARN_LIMIT_CNT_MAX(unit)) && (pFidMacLimitEntry->maxNum != HAL_L2_LEARN_LIMIT_CNT_DISABLE(unit)) , RT_ERR_LIMITED_L2ENTRY_NUM);
    RT_PARAM_CHK((pFidMacLimitEntry->fid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pFidMacLimitEntry->portOrTrunk >= RTK_ENABLE_END) , RT_ERR_INPUT);
    RT_PARAM_CHK(((pFidMacLimitEntry->portOrTrunk == ENABLED) && !(HWP_PORT_EXIST(unit, lPort) || lPort == PORT_DONT_CARE_9310)) , RT_ERR_PORT_ID);
    RT_PARAM_CHK(((pFidMacLimitEntry->portOrTrunk == DISABLED) && (pFidMacLimitEntry->trunk >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit))) , RT_ERR_TRUNK_ID);


    if (pFidMacLimitEntry->portOrTrunk == ENABLED)
        value = (0 << 6) |lPort;
    else
        value = (1 << 6) |pFidMacLimitEntry->trunk;

    L2_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, MANGO_FVIDf, &pFidMacLimitEntry->fid)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, MANGO_PORT_IDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_ENTRYr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, MANGO_CONSTRT_NUMf, &pFidMacLimitEntry->maxNum)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_fidLearningCnt_reset
 * Description:
 *      Reset number of learned MAC addresses on specified entry of fid MAC limit.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of FID MAC limit entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_mango_l2_fidLearningCnt_reset(uint32 unit, uint32 fid_macLimit_idx)
{
    int32       ret;
    uint32      value = 0;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((fid_macLimit_idx >= HAL_L2_FID_LEARN_LIMIT_ENTRY_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, fid_macLimit_idx=%d", unit, fid_macLimit_idx);

    L2_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_L2_LRN_VLAN_CONSTRT_CNTr, REG_ARRAY_INDEX_NONE, fid_macLimit_idx, MANGO_LRN_CNTf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_mango_l2_agingTime_set
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
_dal_mango_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time, uint32 arb_id)
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
        ageUnit = (uint32)((aging_time_tmp * 10 + 7)/ 8);

        if (ageUnit > 0x1fffff)
            return RT_ERR_INPUT;

        if ((ret = reg_field_write(unit, MANGO_L2_AGE_CTRLr, MANGO_AGE_UNITf, &ageUnit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_read(unit, MANGO_L2_AGE_CTRLr, MANGO_AGE_UNITf, &ageUnit)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }


        if (aging_time_tmp == 0 || ageUnit == 0)
            value = 0;
        else if ((uint32)(aging_time_tmp * 10 / ageUnit) == 0)
            value = 0;
        else
            value = (uint32)(aging_time_tmp * 10 / ageUnit) - 1;

        if (value > 8)
            return RT_ERR_INPUT;

        if ((ret = reg_field_write(unit, MANGO_L2_AGE_CTRLr, MANGO_SUS_AGE_MAXf, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}
/* Function Name:
 *      dal_mango_l2_agingTime_get
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
dal_mango_l2_agingTime_get(uint32 unit, rtk_l2_ageTimeType_t type, uint32 *pAging_time)
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
    if ((ret = reg_field_read(unit, MANGO_L2_AGE_CTRLr, MANGO_AGE_UNITf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_L2_AGE_CTRLr, MANGO_SUS_AGE_MAXf, &suspendMax)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    if (type == L2_AGE_TIME_NORMAL)
        *pAging_time = (uint32)(value * 8 / 10);
    else
        *pAging_time = (uint32)(value * (suspendMax + 1)/10);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pAging_time=%ld", *pAging_time);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_agingTime_set
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
dal_mango_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_AGE_TIME_END), RT_ERR_INPUT);

    ret = _dal_mango_l2_agingTime_set(unit, type, aging_time, ARB_NORMAL_API);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out configuration of the specified port to the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_l2_portAgingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_L2_PORT_AGE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_AGE_ENf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port to the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_l2_portAgingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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

    /* program value from CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_L2_PORT_AGE_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_AGE_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_trkAgingEnable_get
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
dal_mango_l2_trkAgingEnable_get(uint32 unit, rtk_trk_t trunk, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, trunk=%d", unit, trunk);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((trunk >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_L2_TRK_AGE_CTRLr, REG_ARRAY_INDEX_NONE, trunk, MANGO_AGE_ENf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_trkAgingEnable_set
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
dal_mango_l2_trkAgingEnable_set(uint32 unit, rtk_trk_t trunk, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, trunk=%d, enable=%d", unit, trunk, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((trunk >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
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
    if ((ret = reg_array_field_write(unit, MANGO_L2_TRK_AGE_CTRLr, REG_ARRAY_INDEX_NONE, trunk, MANGO_AGE_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_hashAlgo_get
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
dal_mango_l2_hashAlgo_get(uint32 unit, uint32 *pHash_algo)
{
    return dal_mango_l2_bucketHashAlgo_get(unit, 0, pHash_algo);
}

/* Function Name:
 *      dal_mango_l2_hashAlgo_set
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
dal_mango_l2_hashAlgo_set(uint32 unit, uint32 hash_algo)
{
    int32 ret;
    ret = dal_mango_l2_bucketHashAlgo_set(unit, 0, hash_algo);
    if(ret!= RT_ERR_OK)
        return ret;
    else
        return dal_mango_l2_bucketHashAlgo_set(unit, 1, hash_algo);
}

/* Function Name:
 *      dal_mango_l2_bucketHashAlgo_get
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
dal_mango_l2_bucketHashAlgo_get(uint32 unit, uint32 bucket, uint32 *pHash_algo)
{
    int32   ret;
    uint32  value = 0;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHash_algo), RT_ERR_NULL_POINTER);

    if (bucket == 0)
        regField = MANGO_L2_HASH_ALGO_BLK0f;
    else if (bucket == 1)
        regField = MANGO_L2_HASH_ALGO_BLK1f;
    else
        return RT_ERR_INPUT;

    L2_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, regField, &value)) != RT_ERR_OK)
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
 *      dal_mango_l2_bucketHashAlgo_set
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
dal_mango_l2_bucketHashAlgo_set(uint32 unit, uint32 bucket, uint32 hash_algo)
{
    int32   ret;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d  hash_algo=%d", unit, hash_algo);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);
    RT_PARAM_CHK((bucket >= LUT_TBL_BLK_NUM), RT_ERR_INPUT);

    /* parameter check */
    if (bucket == 0)
        regField = MANGO_L2_HASH_ALGO_BLK0f;
    else if (bucket == 1)
        regField = MANGO_L2_HASH_ALGO_BLK1f;
    RT_PARAM_CHK((hash_algo >= HAL_MAX_NUM_OF_L2_HASH_ALGO(unit)), RT_ERR_INPUT);

    L2_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, regField, &hash_algo)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blkAlgoType[bucket][unit] = hash_algo;

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_learningFullAction_get
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
dal_mango_l2_learningFullAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  act = 0;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_HASH_FULL_ACTf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    /* translate register value to action */
    switch (act)
    {
        case 0:
            *pAction = ACTION_FORWARD;
            break;
        case 1:
            *pAction = ACTION_DROP;
            break;
        case 2:
            *pAction = ACTION_TRAP2CPU;
            break;
        case 3:
            *pAction = ACTION_COPY2CPU;
            break;
        case 4:
            *pAction = ACTION_TRAP2MASTERCPU;
            break;
        case 5:
            *pAction = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pAction=%d", *pAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_learningFullAction_set
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
dal_mango_l2_learningFullAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  act;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    switch (action)
    {
        case ACTION_FORWARD:
            act = 0;
            break;
        case ACTION_DROP:
            act = 1;
            break;
        case ACTION_TRAP2CPU:
            act = 2;
            break;
        case ACTION_COPY2CPU:
            act = 3;
            break;
        case ACTION_TRAP2MASTERCPU:
            act = 4;
            break;
        case ACTION_COPY2MASTERCPU:
            act = 5;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, MANGO_HASH_FULL_ACTf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_l2_portNewMacOp_get
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
_dal_mango_l2_portNewMacOp_get(uint32 unit, rtk_port_t port, uint32 *pMode, uint32 *pAct)
{
    int32   ret;
    uint32  mode = 0, act = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pAct), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(unit, MANGO_L2_PORT_SALRNr, port, REG_ARRAY_INDEX_NONE, MANGO_SALRNf, &mode)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MANGO_L2_PORT_NEW_SA_FWDr, port, REG_ARRAY_INDEX_NONE, MANGO_NEW_SA_FWDf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    *pMode  = mode;
    *pAct   = act;


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMode=%d  pAct:%d", *pMode, *pAct);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_l2_portNewMacOp_set
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
 *      2. Only dal_mango_l2_portNewMacOp_set (ARB_NORMAL_API) can set NEW_SA_FWD action
 */
int32
_dal_mango_l2_portNewMacOp_set(uint32 unit, rtk_port_t port, uint32 mode, uint32 act, uint32 arb_id)
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

    if ((ret = reg_array_field_write(unit, MANGO_L2_PORT_SALRNr, port, REG_ARRAY_INDEX_NONE, MANGO_SALRNf, &lrn)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK_2(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    if (arb_id == ARB_NORMAL_API)
    {
        if ((ret = reg_array_field_write(unit, MANGO_L2_PORT_NEW_SA_FWDr, port, REG_ARRAY_INDEX_NONE, MANGO_NEW_SA_FWDf, &act)) != RT_ERR_OK)
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
 *      dal_mango_l2_portNewMacOp_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
dal_mango_l2_portNewMacOp_get(uint32 unit, rtk_port_t port, rtk_l2_newMacLrnMode_t *pLrnMode, rtk_action_t *pFwdAction)
{
    int32   ret;
    uint32  mode = 0, act = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pLrnMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    if ((ret = _dal_mango_l2_portNewMacOp_get(unit, port, &mode, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
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

    switch (act)
    {
        case 0:
            *pFwdAction = ACTION_FORWARD;
            break;
        case 1:
            *pFwdAction = ACTION_DROP;
            break;
        case 2:
            *pFwdAction = ACTION_TRAP2CPU;
            break;
        case 3:
            *pFwdAction = ACTION_COPY2CPU;
            break;
        case 4:
            *pFwdAction = ACTION_TRAP2MASTERCPU;
            break;
        case 5:
            *pFwdAction = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pLrnMode=%d  pFwdAction:%d", *pLrnMode, *pFwdAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portNewMacOp_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
dal_mango_l2_portNewMacOp_set(uint32 unit, rtk_port_t port, rtk_l2_newMacLrnMode_t lrnMode, rtk_action_t fwdAction)
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

    switch (fwdAction)
    {
        case ACTION_FORWARD:
            act = 0;
            break;
        case ACTION_DROP:
            act = 1;
            break;
        case ACTION_TRAP2CPU:
            act = 2;
            break;
        case ACTION_COPY2CPU:
            act = 3;
            break;
        case ACTION_TRAP2MASTERCPU:
            act = 4;
            break;
        case ACTION_COPY2MASTERCPU:
            act = 5;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    if ((ret = _dal_mango_l2_portNewMacOp_set(unit, port, mode, act, ARB_NORMAL_API)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2      */
/* Sub-module Name: Unicast */

/* Function Name:
 *      dal_mango_l2_addr_init
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
dal_mango_l2_addr_init(
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
 *      dal_mango_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      unit        - unit id
 *      pL2_addr    - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
dal_mango_l2_addr_add(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    uint8                   oriNh = 0;
    int32                   ret;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;
    rtk_port_t              lPort;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pL2_addr=%x", unit, pL2_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    if ((pL2_addr->flags & RTK_L2_UCAST_FLAG_L2_TUNNEL) || \
        (pL2_addr->port == L2_PORT_DONT_CARE))
        lPort = PORT_DONT_CARE_9310;
    else if (pL2_addr->port == L2_PORT_BLACK_HOLE)
        lPort = PORT_BLACK_HOLE_9310;
    else
        lPort = pL2_addr->port;

    /* parameter check */
    RT_PARAM_CHK(NULL == pL2_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    if (!(pL2_addr->flags & RTK_L2_UCAST_FLAG_L2_TUNNEL))
    {
        if (pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
            RT_PARAM_CHK((pL2_addr->trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
        else
        {
            if(((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)==0) && (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC))
                RT_PARAM_CHK((((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)) && (lPort != PORT_BLACK_HOLE_9310)), RT_ERR_PORT_ID);
            else if (((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)) && !(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC))
                RT_PARAM_CHK(((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort) && lPort != PORT_DONT_CARE_9310), RT_ERR_PORT_ID);
            else
                RT_PARAM_CHK(((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)), RT_ERR_PORT_ID);
        }
    }
    RT_PARAM_CHK((((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)) && (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND)) , RT_ERR_INPUT);
    RT_PARAM_CHK((pL2_addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pL2_addr->agg_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pL2_addr->devID > RTK_MAX_NUM_OF_UNIT), RT_ERR_UNIT_ID);
    RT_PARAM_CHK((pL2_addr->agg_pri > RTK_DOT1P_PRIORITY_MAX), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK(pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP , RT_ERR_INPUT);       /* Nexthop bit should be operated by L3 API */
    RT_PARAM_CHK((pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET) && (pL2_addr->age > 7), RT_ERR_INPUT);


    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &(pL2_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (ret == RT_ERR_OK && l2_entry.is_entry_exist && (pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_NOT_UPDATE_EXIST))
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
        pL2_addr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
        return RT_ERR_L2_ENTRY_EXIST;
    }
    else if (ret == RT_ERR_L2_NO_EMPTY_ENTRY && (pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC) != 0)
    {
        ret = _dal_mango_l2_getFirstDynamicEntry(unit, &l2_entry, &index_entry);
        if (ret != RT_ERR_OK)
        {
            _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_NO_EMPTY_ENTRY;
        }

        osal_memcpy(&pL2_addr->macInfo.mac, &l2_entry.unicast.mac, ETHER_ADDR_LEN);
        pL2_addr->macInfo.devID      = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
        pL2_addr->macInfo.port      = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
        pL2_addr->macInfo.vid       = l2_entry.unicast.fid;
        pL2_addr->macInfo.flags     = l2_entry.unicast.is_trk ? RTK_L2_UCAST_FLAG_TRUNK_PORT : 0;
        pL2_addr->macInfo.trk_gid   = l2_entry.unicast.is_trk ? (l2_entry.unicast.slp & 0xff) : 0;
    }
    else if (RT_ERR_OK != ret)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (l2_entry.is_entry_exist)
        oriNh = l2_entry.unicast.nh;
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.valid = TRUE;
    l2_entry.unicast.fid = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &(pL2_addr->mac), sizeof(rtk_mac_t));
    if (pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
    {
        l2_entry.unicast.is_trk = 1;
        l2_entry.unicast.slp = pL2_addr->trk_gid;
    }
    else
        l2_entry.unicast.slp = (pL2_addr->devID << RTL9310_L2ENTRY_DEVI_ID_OFFSET) | lPort;
    if (!(pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT) && lPort == PORT_DONT_CARE_9310)
        l2_entry.unicast.aging = 0;
    else
    {
        if (pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET)
            l2_entry.unicast.aging = pL2_addr->age;
        else
            l2_entry.unicast.aging = 7;
    }
    l2_entry.unicast.sablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.dablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.is_static  = (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC) ? TRUE: FALSE;
    l2_entry.unicast.suspending = (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND) ? TRUE: FALSE;
    l2_entry.unicast.nh         = (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP) ? TRUE: oriNh;
    l2_entry.unicast.tagSts     = (pL2_addr->flags & RTK_L2_UCAST_FLAG_TAG_STS) ? TRUE: FALSE;
    l2_entry.unicast.agg_pri    = pL2_addr->agg_pri;
    l2_entry.unicast.agg_vid    = pL2_addr->agg_vid;
    l2_entry.unicast.ecid       = pL2_addr->ecid;
    l2_entry.unicast.l2_tnl_if  = (pL2_addr->flags & RTK_L2_UCAST_FLAG_L2_TUNNEL)? TRUE : FALSE;
    l2_entry.unicast.l2_tnl_idx = pL2_addr->l2_tunnel_idx;

    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
    pL2_addr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);


    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_mango_l2_addr_del
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
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      vid is same as fid in IVL mode.
 */
int32
dal_mango_l2_addr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32 ret;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pMac=%x-%x-%x-%x-%x-%x",
           pMac->octet[0], pMac->octet[1], pMac->octet[2],
           pMac->octet[3], pMac->octet[4], pMac->octet[5]);

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = vid;
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    /* fill content */
    if (l2_entry.unicast.nh)
    {
        l2_entry.unicast.aging      = 0;
        l2_entry.unicast.is_trk     = 0;
        l2_entry.unicast.slp        = PORT_DONT_CARE_9310;
        l2_entry.unicast.sablock    = 0;
        l2_entry.unicast.dablock    = 0;
        l2_entry.unicast.is_static  = 0;
        l2_entry.unicast.suspending = 0;
        l2_entry.unicast.tagSts     = 0;
        l2_entry.unicast.agg_pri    = 0;
        l2_entry.unicast.agg_vid    = 0;
        l2_entry.unicast.ecid       = 0;
        l2_entry.unicast.l2_tnl_if  = 0;
        l2_entry.unicast.l2_tnl_idx = 0;
    }
    else
    {
        osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
        l2_entry.entry_type = L2_UNICAST;
    }
    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_mango_l2_addr_get
 * Description:
 *      Get a L2 unicast address entry from the specified device.
 * Input:
 *      unit     - unit id
 *      pL2_addr - structure of l2 address data
 * Output:
 *      pL2_addr - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vid
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
dal_mango_l2_addr_get(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    int32 ret;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;

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

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type     = L2_UNICAST;
    l2_entry.unicast.fid    = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &pL2_addr->mac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
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
    if (l2_entry.unicast.is_trk)
    {
        pL2_addr->flags     |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
        pL2_addr->trk_gid   = l2_entry.unicast.slp & 0xff;
    }
    else
    {
        pL2_addr->devID     = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
        pL2_addr->port      = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
    }

    if (l2_entry.unicast.sablock)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
    if (l2_entry.unicast.dablock)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
    if (l2_entry.unicast.is_static)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_STATIC;
    if (l2_entry.unicast.suspending)
        pL2_addr->state |= RTK_L2_UCAST_STATE_SUSPEND;
    if (l2_entry.unicast.nh)
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_NEXTHOP;
    if (l2_entry.unicast.aging == 0 && l2_entry.unicast.is_trk == 0 && l2_entry.unicast.slp == PORT_DONT_CARE_9310)
        pL2_addr->isAged = TRUE;
    else
        pL2_addr->isAged = FALSE;
    if (l2_entry.unicast.tagSts)
        pL2_addr->flags    |= RTK_L2_UCAST_FLAG_TAG_STS;
    pL2_addr->agg_pri   = l2_entry.unicast.agg_pri;
    pL2_addr->agg_vid   = l2_entry.unicast.agg_vid;
    pL2_addr->age       = l2_entry.unicast.aging;
    pL2_addr->ecid      = l2_entry.unicast.ecid;

    if (l2_entry.unicast.l2_tnl_if)
    {
        pL2_addr->flags |= RTK_L2_UCAST_FLAG_L2_TUNNEL;
        pL2_addr->l2_tunnel_idx = l2_entry.unicast.l2_tnl_idx;
    } else {
        pL2_addr->l2_tunnel_idx = 0;
    }

    pL2_addr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "vid=%d, unit=%d, port=%d, sa_block=%d, da_block=%d, is_static=%d, nh=%d, suspend=%d, agg_pri=:%d, agg_vid=:%d, ecid=0x%x",
        pL2_addr->vid, pL2_addr->devID, pL2_addr->port, l2_entry.unicast.sablock, l2_entry.unicast.dablock, l2_entry.unicast.is_static,
        l2_entry.unicast.nh, l2_entry.unicast.suspending, l2_entry.unicast.agg_pri, l2_entry.unicast.agg_vid,
        l2_entry.unicast.ecid);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_l2_addr_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_l2_addr_set(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    int32                   ret;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;
    rtk_port_t              lPort;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pL2_addr=%x", unit, pL2_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    if (pL2_addr->port == L2_PORT_DONT_CARE)
        lPort = PORT_DONT_CARE_9310;
    else if (pL2_addr->port == L2_PORT_BLACK_HOLE)
        lPort = PORT_BLACK_HOLE_9310;
    else
        lPort = pL2_addr->port;

    /* parameter check */
    RT_PARAM_CHK(NULL == pL2_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    if (pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
        RT_PARAM_CHK((pL2_addr->trk_gid >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    else
    {
        if(((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)==0) && ((pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)==0) && (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC))
            RT_PARAM_CHK((((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)) && (lPort != PORT_BLACK_HOLE_9310)), RT_ERR_PORT_ID);
        else if (((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) || (pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP)) && !(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC))
            RT_PARAM_CHK(((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort) && lPort != PORT_DONT_CARE_9310), RT_ERR_PORT_ID);
        else
            RT_PARAM_CHK(((rtk_unit2devID[unit]==pL2_addr->devID) && !HWP_PORT_EXIST(unit, lPort)), RT_ERR_PORT_ID);
    }
    RT_PARAM_CHK((((pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK)||(pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC)) && (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND)) , RT_ERR_INPUT);
    RT_PARAM_CHK((pL2_addr->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pL2_addr->agg_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(pL2_addr->flags & RTK_L2_UCAST_FLAG_NEXTHOP , RT_ERR_INPUT);       /* Nexthop bit should be operated by L2 API */
    RT_PARAM_CHK((pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET) && (pL2_addr->age > 7), RT_ERR_INPUT);


    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pL2_addr->vid;
    osal_memcpy(&l2_entry.unicast.mac, &(pL2_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry);
    if (RT_ERR_OK != ret)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT)
    {
        l2_entry.unicast.is_trk = 1;
        l2_entry.unicast.slp    = pL2_addr->trk_gid;
    }
    else
        l2_entry.unicast.slp    = (pL2_addr->devID << RTL9310_L2ENTRY_DEVI_ID_OFFSET) | lPort;

    if (!(pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT) && lPort == PORT_DONT_CARE_9310)
        l2_entry.unicast.aging  = 0;
    else
    {
        if (pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET)
            l2_entry.unicast.aging = pL2_addr->age;
        else
            l2_entry.unicast.aging = 7;
    }
    l2_entry.unicast.sablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.dablock    = (pL2_addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? TRUE: FALSE;
    l2_entry.unicast.is_static  = (pL2_addr->flags & RTK_L2_UCAST_FLAG_STATIC) ? TRUE: FALSE;
    l2_entry.unicast.suspending = (pL2_addr->state & RTK_L2_UCAST_STATE_SUSPEND) ? TRUE: FALSE;
    l2_entry.unicast.tagSts     = (pL2_addr->flags & RTK_L2_UCAST_FLAG_TAG_STS) ? TRUE: FALSE;
    l2_entry.unicast.agg_pri    = pL2_addr->agg_pri;
    l2_entry.unicast.agg_vid    = pL2_addr->agg_vid;
    l2_entry.unicast.ecid       = pL2_addr->ecid;
    l2_entry.unicast.l2_tnl_if  = (pL2_addr->flags & RTK_L2_UCAST_FLAG_L2_TUNNEL) ? TRUE: FALSE;
    l2_entry.unicast.l2_tnl_idx = pL2_addr->l2_tunnel_idx;
    if (!(pL2_addr->flags & RTK_L2_UCAST_FLAG_TRUNK_PORT) && lPort == PORT_DONT_CARE_9310)
        l2_entry.unicast.aging = 0;
    else
    {
        if (pL2_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_AGE_SET)
            l2_entry.unicast.aging = pL2_addr->age;
        else
            l2_entry.unicast.aging = 7;
    }

    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
    pL2_addr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_addr_delAll
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_l2_addr_delAll(uint32 unit, uint32 include_static)
{
    int32                   ret;
    uint32                  value = 0, index = 0;
    rtk_enable_t            l2CamEbl;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* search exist or free entry */
    l2_entry.entry_type = L2_UNICAST;
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    index_entry.index_type = L2_IN_HASH;
    for (index = 0; index < hashTable_size[unit]; index++)
    {
        index_entry.index = index >> 2;
        index_entry.hashdepth = index & 0x3;

        if ((ret = _dal_mango_l2_getL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
        {
            _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
            return ret;
        }

        if (l2_entry.entry_type == L2_UNICAST && l2_entry.is_entry_valid == 1 && (include_static || l2_entry.unicast.is_static == 0))
        {
            if (l2_entry.unicast.nh)
            {
                l2_entry.unicast.aging      = 0;
                l2_entry.unicast.is_trk     = 0;
                l2_entry.unicast.slp        = PORT_DONT_CARE_9310;
                l2_entry.unicast.sablock    = 0;
                l2_entry.unicast.dablock    = 0;
                l2_entry.unicast.is_static  = 0;
                l2_entry.unicast.suspending = 0;
                l2_entry.unicast.tagSts     = 0;
                l2_entry.unicast.agg_pri    = 0;
                l2_entry.unicast.agg_vid    = 0;
                l2_entry.unicast.ecid       = 0;
                l2_entry.unicast.l2_tnl_if  = 0;
                l2_entry.unicast.l2_tnl_idx = 0;
            }
            else
                osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
            if ((ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                return ret;
            }
        }
    }

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    l2CamEbl = value == 1 ? ENABLED : DISABLED;

    if (l2CamEbl == ENABLED && ld_patch_en == FALSE)
    {
        index_entry.index_type = L2_IN_CAM;
        for (index=0; index < cam_size[unit]; index++)
        {
            index_entry.index = index;

            if ((ret = _dal_mango_l2_getL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                return ret;
            }

            if (l2_entry.entry_type == L2_UNICAST && l2_entry.is_entry_valid == 1 && (include_static || l2_entry.unicast.is_static == 0))
            {
                if (l2_entry.unicast.nh)
                {
                    l2_entry.unicast.aging      = 0;
                    l2_entry.unicast.is_trk     = 0;
                    l2_entry.unicast.slp        = PORT_DONT_CARE_9310;
                    l2_entry.unicast.sablock    = 0;
                    l2_entry.unicast.dablock    = 0;
                    l2_entry.unicast.is_static  = 0;
                    l2_entry.unicast.suspending = 0;
                    l2_entry.unicast.tagSts     = 0;
                    l2_entry.unicast.agg_pri    = 0;
                    l2_entry.unicast.agg_vid    = 0;
                    l2_entry.unicast.ecid       = 0;
                    l2_entry.unicast.l2_tnl_if  = 0;
                    l2_entry.unicast.l2_tnl_idx = 0;
                }
                else
                    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
                if ((ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
                {
                    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                    L2_SEM_UNLOCK(unit);
                    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                    return ret;
                }
            }
        }
    }

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_l2_nextValidAddr_get
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
dal_mango_l2_nextValidAddr_get(
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


    if(*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 1;

    osal_memset(&entry, 0, sizeof(entry));
    if (include_static)
        ret = dal_mango_l2_hwNextValidAddr_get(unit, pScan_idx, L2_NEXT_VALID_TYPE_UC, &entry);
    else
        ret = dal_mango_l2_hwNextValidAddr_get(unit, pScan_idx, L2_NEXT_VALID_TYPE_AUTO_UC, &entry);

    osal_memcpy(pL2_addr, &entry.unicast, sizeof(rtk_l2_ucastAddr_t));
    pL2_addr->l2_idx = *pScan_idx;
    return ret;
}

/* Function Name:
 *      _dal_mango_l2_nexthop_add
 * Description:
 *      Add L2 nexthop entry to ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_ucastNhAddr_t  - nexthop entry
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
 *          this function will add an entry with nexthop bit set to 1 and the portId be PORT_DONT_CARE_9310.
 */
int32
_dal_mango_l2_nexthop_add(uint32 unit, dal_mango_l2_ucastNhAddr_t *pNexthop_addr)
{
    int32                   ret;
    uint32                  l2_idx_l;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, pNexthop_addr=%x", unit, pNexthop_addr);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pNexthop_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNexthop_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pNexthop_addr->fid;
    osal_memcpy(&l2_entry.unicast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (ret == RT_ERR_OK && l2_entry.is_entry_exist == 0)
    {
        l2_entry.unicast.slp = PORT_DONT_CARE_9310;
    }
    else if (ret == RT_ERR_L2_NO_EMPTY_ENTRY && (pNexthop_addr->add_op_flags & RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC) != 0)
    {
        ret = _dal_mango_l2_getFirstDynamicEntry(unit, &l2_entry, &index_entry);
        if (ret != RT_ERR_OK)
        {
            _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_NO_EMPTY_ENTRY;
        }

        osal_memcpy(&pNexthop_addr->macInfo.mac, &l2_entry.unicast.mac, ETHER_ADDR_LEN);
        pNexthop_addr->macInfo.devID      = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
        pNexthop_addr->macInfo.port      = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
        pNexthop_addr->macInfo.vid       = l2_entry.unicast.fid;
        pNexthop_addr->macInfo.flags     = l2_entry.unicast.is_trk ? RTK_L2_UCAST_FLAG_TRUNK_PORT : 0;
        pNexthop_addr->macInfo.trk_gid   = l2_entry.unicast.is_trk ? (l2_entry.unicast.slp & 0xff) : 0;

        l2_entry.entry_type = L2_UNICAST;
        l2_entry.unicast.fid = pNexthop_addr->fid;
        osal_memcpy(&l2_entry.unicast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));
        l2_entry.unicast.slp = PORT_DONT_CARE_9310;
    }
    else if (RT_ERR_OK != ret)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    l2_entry.unicast.valid = TRUE;
    l2_entry.unicast.nh = 1;
    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
    l2_idx_l = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
    pNexthop_addr->l2_idx = TBL_IDX_XLATE_P(l2_idx_l);  /* l2_idx is physical index in this internal API */

    nh_used_cnt[unit][l2_idx_l]++;

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_l2_nexthop_del
 * Description:
 *      Delete L2 nexthop entry from ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_ucastNhAddr_t  - nexthop entry
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
_dal_mango_l2_nexthop_del(uint32 unit, dal_mango_l2_ucastNhAddr_t *pNexthop_addr)
{
    int32   ret;
    uint32  index;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, fid=%d, pMac=%x-%x-%x-%x-%x-%x",
                                        unit, pNexthop_addr->fid,
                                        pNexthop_addr->mac.octet[0], pNexthop_addr->mac.octet[1], pNexthop_addr->mac.octet[2],
                                        pNexthop_addr->mac.octet[3], pNexthop_addr->mac.octet[4], pNexthop_addr->mac.octet[5]);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pNexthop_addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pNexthop_addr->mac.octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    RT_PARAM_CHK((pNexthop_addr->fid > RTK_FID_MAX), RT_ERR_L2_FID);


    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = pNexthop_addr->fid;
    osal_memcpy(&l2_entry.unicast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    /* fill content */
    if (l2_entry.unicast.nh)
    {
        index = L2_ENTRY_UNIQUE_IDX(unit, index_entry); /* logical index */

        if (nh_used_cnt[unit][index] > 0)
            nh_used_cnt[unit][index]--;
        if (nh_used_cnt[unit][index] == 0)
        {
            l2_entry.unicast.nh = 0;
            if (l2_entry.unicast.slp == PORT_DONT_CARE_9310 &&
                l2_entry.unicast.sablock == 0 &&
                l2_entry.unicast.dablock == 0 &&
                l2_entry.unicast.is_static == 0)
            {
                osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
                l2_entry.entry_type = L2_UNICAST;
            }
            if ((ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                return ret;
            }
        }
    }
    else
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_NEXTHOP_NOT_EXIST;
    }

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_l2_nexthop_del_byIdx
 * Description:
 *      Delete L2 nexthop entry from ASIC based on index.
 * Input:
 *      unit    - unit id
 *      index   - index to nexthop entry (should be physical index)
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
_dal_mango_l2_nexthop_del_byIdx(uint32 unit, uint32 index)
{
    int32   ret;
    uint32                l2_idx_l;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    l2_idx_l = TBL_IDX_XLATE_L(index);
    RT_PARAM_CHK((l2_idx_l >= hashTable_size[unit] + cam_size[unit]), RT_ERR_INPUT);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));

    if (l2_idx_l < hashTable_size[unit])
    {
        index_entry.index_type  = L2_IN_HASH;
        index_entry.index       = l2_idx_l >> 2;
        index_entry.hashdepth   = l2_idx_l & 0x3;
    }
    else
    {
        index_entry.index_type  = L2_IN_CAM;
        index_entry.index       = l2_idx_l - hashTable_size[unit];
    }

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = _dal_mango_l2_getL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* fill content */
    if (l2_entry.unicast.nh)
    {
        nh_used_cnt[unit][l2_idx_l]--;
        if (nh_used_cnt[unit][l2_idx_l] == 0)
        {
            l2_entry.unicast.nh = 0;
            if (l2_entry.unicast.slp == PORT_DONT_CARE_9310 &&
                l2_entry.unicast.sablock == 0 &&
                l2_entry.unicast.dablock == 0 &&
                l2_entry.unicast.is_static == 0 &&
                l2_entry.unicast.suspending == 0)
            {
                osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
                l2_entry.entry_type = L2_UNICAST;
            }
            if ((ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
                return ret;
            }
        }
    }
    else
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_NEXTHOP_NOT_EXIST;
    }

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      dal_mango_l2_mcastAddr_init
 * Description:
 *      Initialize content of buffer of L2 multicast entry.
 *      Will fill vid ,MAC address and reset other field of L2 multicast entry.
 * Input:
 *      unit        - unit id
 *      vid         - vlan id
 *      pMac        - MAC address
 *      pMcastAddr - L2 multicast entry
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
dal_mango_l2_mcastAddr_init(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac, rtk_l2_mcastAddr_t *pMcastAddr)
{
    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pMcastAddr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(pMcastAddr, 0, sizeof(rtk_l2_mcastAddr_t));
    pMcastAddr->rvid = vid;
    osal_memcpy(&pMcastAddr->mac.octet[0], &pMac->octet[0], sizeof(rtk_mac_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      unit        - unit id
 *      pMcastAddr - L2 multicast entry
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
 *      (1) Don't need to configure pMcastAddr->fwdIndex because driver automatically allocates a
 *          free portmask entry index and return it back to pMcastAddr->fwdIndex.
 *      (2) pMcastAddr->portmask is used to configure the allocated portmask entry.
 */
int32
dal_mango_l2_mcastAddr_add(uint32 unit, rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    pMcastAddr->fwdIndex = -1; /* for automatically allocate */

    ret = dal_mango_l2_mcastAddr_addByIndex(unit, pMcastAddr);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_mcastAddr_del
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
dal_mango_l2_mcastAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32 ret;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;
    int32 fwdTableIdx = -1;
    uint32 l2_index;

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

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    l2_entry.entry_type = L2_MULTICAST;
    l2_entry.l2mcast.fid = vid;
    osal_memcpy(&l2_entry.l2mcast.mac, pMac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    if (l2_entry.l2mcast.nh)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_NEXTHOP_EXIST;
    }

    /* traditional API (if groupId == 0) */
    if (0 == l2Db[unit].pEntry[L2_ENTRY_UNIQUE_IDX(unit, index_entry)].groupId)
    {
        if (l2_entry.l2mcast.local_fwd)
            fwdTableIdx = l2_entry.l2mcast.index;

        /* fill content */
        if (l2_entry.l2mcast.remote_fwd)
        {
            /* only clear normal ports if ECID list is activated*/
            l2_entry.l2mcast.local_fwd  = 0;
            l2_entry.l2mcast.index      = 0;
        }
        else
        {
            osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
        }

        l2_entry.entry_type = L2_MULTICAST;
        ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);

        if (ret == RT_ERR_OK && fwdTableIdx != -1)
            _dal_mango_l2_mcastFwdIndex_free(unit, fwdTableIdx);
    }
    else
    {
        /* unbind the group */
        l2_index = L2_ENTRY_UNIQUE_IDX(unit, index_entry);

        ret = _dal_mango_mcast_l2mc_unbind(unit, l2Db[unit].pEntry[l2_index].groupId, &l2Db[unit].pEntry[l2_index].mcastNode);
        if (ret != RT_ERR_OK)
        {
            _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        l2Db[unit].pEntry[l2_index].groupId = 0;

        osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
        l2_entry.entry_type = L2_MULTICAST;
        ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
    }

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_mcastAddr_get
 * Description:
 *      Get L2 multicast entry based on specified VID and MAC address.
 * Input:
 *      unit        - unit id
 *      pMcastAddr - L2 multicast entry
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
dal_mango_l2_mcastAddr_get(uint32 unit, rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32 ret;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d", unit, pMcastAddr->rvid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMcastAddr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcastAddr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->rvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.fid    = pMcastAddr->rvid;
    osal_memcpy(&l2_entry.l2mcast.mac, &pMcastAddr->mac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);

    if ((ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        /* Return Fail if not found */
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if (l2_entry.l2mcast.local_fwd == 0)
    {
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_ENTRY_NOTFOUND;
    }

    /* fill content */
    pMcastAddr->fwdIndex   = l2_entry.l2mcast.index;
    pMcastAddr->nextHop    = l2_entry.l2mcast.nh;

    if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pMcastAddr->fwdIndex, &pMcastAddr->portmask)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    pMcastAddr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
    pMcastAddr->groupId = l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_mcastAddr_set
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      unit        - unit id
 * Output:
 *      pMcastAddr - pointer to L2 multicast entry
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
dal_mango_l2_mcastAddr_set(uint32 unit, rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32               ret;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;
    int32 mcastFwdIdx = -1;
    rtk_portmask_t portmask;
    int32 old_mcastFwdIdx = -1;
    uint32                      ori_group;
    dal_mango_mcast_l2Info_t    mcastL2Info;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, vid=%d, pMac=%x-%x-%x-%x-%x-%x \
           pmsk0=0x%x, pmsk1=0x%x", unit, pMcastAddr->rvid, pMcastAddr->mac.octet[0], pMcastAddr->mac.octet[1], pMcastAddr->mac.octet[2],
           pMcastAddr->mac.octet[3], pMcastAddr->mac.octet[4], pMcastAddr->mac.octet[5],
           RTK_PORTMASK_WORD_GET(pMcastAddr->portmask, 0),
           RTK_PORTMASK_WORD_GET(pMcastAddr->portmask, 1));

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMcastAddr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcastAddr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->rvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, &pMcastAddr->portmask), RT_ERR_PORT_MASK);
    /* [FIXME] check group id if it's valid here (if groupId != 0) */

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    osal_memset(&portmask, 0, sizeof(rtk_portmask_t));

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memcpy(&l2_entry.l2mcast.mac, &(pMcastAddr->mac), sizeof(rtk_mac_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.fid    = pMcastAddr->rvid;

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    /* check entry existence */
    ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry);
    if (RT_ERR_OK != ret)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* group id */
    pMcastAddr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);

    if (pMcastAddr->nextHop || nh_used_cnt[unit][pMcastAddr->l2_idx] == 0)
        l2_entry.l2mcast.nh = pMcastAddr->nextHop;

    /* group process */
    if ((pMcastAddr->groupId != l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId) || (0 == pMcastAddr->groupId))
    {
        /* unbind the old group if it exists */
        if (l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId > 0)
        {
            ret = _dal_mango_mcast_l2mc_unbind(unit, l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId, &l2Db[unit].pEntry[pMcastAddr->l2_idx].mcastNode);
            if (ret != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }

        /* update database */
        ori_group = l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId;
        l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId = pMcastAddr->groupId;
        if (l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId > 0)
        {
            /* bind to the new group id */
            ret = _dal_mango_mcast_l2mc_bind(unit, l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId, &l2Db[unit].pEntry[pMcastAddr->l2_idx].mcastNode, &mcastL2Info);
            if (ret != RT_ERR_OK)
            {
                l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId = ori_group;
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if (mcastL2Info.fwdIndex >= 0)
            {
                l2_entry.l2mcast.local_fwd = 1;
                l2_entry.l2mcast.index = mcastL2Info.fwdIndex;
            } else {
                l2_entry.l2mcast.local_fwd = 0;
                l2_entry.l2mcast.index = 0;
            }

            l2_entry.l2mcast.remote_fwd = mcastL2Info.l2_tnl_lst_valid;
            l2_entry.l2mcast.tunnel_list_idx = mcastL2Info.l2_tnl_lst_idx;
        }
        else
        {
            /* traditional API (groupId = 0) */
            if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, (int32)l2_entry.l2mcast.index, &portmask)) != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_LOG(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            l2_entry.l2mcast.local_fwd = 1;
            if (RTK_PORTMASK_COMPARE(portmask, pMcastAddr->portmask) != 0)
            {
                if ((ret = _dal_mango_l2_mcastFwdIndex_alloc(unit, &mcastFwdIdx)) != RT_ERR_OK)
                {
                    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                    L2_SEM_UNLOCK(unit);
                    RT_LOG(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }

                old_mcastFwdIdx = l2_entry.l2mcast.index;
                l2_entry.l2mcast.index = mcastFwdIdx;

                if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_set(unit, mcastFwdIdx, &pMcastAddr->portmask)) != RT_ERR_OK)
                {
                    _dal_mango_l2_mcastFwdIndex_free(unit, mcastFwdIdx);
                    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                    L2_SEM_UNLOCK(unit);
                    RT_LOG(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
            }

            if (RTK_PORTMASK_COMPARE(portmask, pMcastAddr->portmask) != 0)
                _dal_mango_l2_mcastFwdIndex_free(unit, (RT_ERR_OK == ret) ? old_mcastFwdIdx : mcastFwdIdx);

            l2_entry.l2mcast.remote_fwd = 0;
            l2_entry.l2mcast.tunnel_list_idx = 0;
        }
    }

    /* write the entry to update data */
    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_mcastAddr_addByIndex
 * Description:
 *      Add L2 multicast entry to ASIC with specific forward portmask index.
 * Input:
 *      unit        - unit id
 *      pMcastAddr  - L2 multicast entry
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
 *      (1) Specific pMcastAddr->fwdIndex.
 *      (2) pMcastAddr->portmask is used to configure the allocated portmask entry.
 */
int32
dal_mango_l2_mcastAddr_addByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32                   ret, fwdIndex;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;
    int32                   oldFwdIdx = -1;
    uint32                  ori_group;
    dal_mango_mcast_l2Info_t    mcastL2Info;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMcastAddr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMcastAddr->mac.octet[0] & BITMASK_1B) == 0, RT_ERR_MAC);
    RT_PARAM_CHK((pMcastAddr->rvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(pMcastAddr->fwdIndex >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, &pMcastAddr->portmask), RT_ERR_PORT_MASK);
    /* [FIXME] check group id if it exists */

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.valid  = TRUE;
    l2_entry.l2mcast.fid    = pMcastAddr->rvid;
    osal_memcpy(&l2_entry.l2mcast.mac, &(pMcastAddr->mac), sizeof(rtk_mac_t));


    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (ret == RT_ERR_OK && l2_entry.is_entry_exist && (pMcastAddr->add_op_flags & RTK_L2_ADD_OP_FLAG_NOT_UPDATE_EXIST))
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
        pMcastAddr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
        return RT_ERR_L2_ENTRY_EXIST;
    }
    else if (ret == RT_ERR_L2_NO_EMPTY_ENTRY && (pMcastAddr->add_op_flags & RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC) != 0)
    {
        ret = _dal_mango_l2_getFirstDynamicEntry(unit, &l2_entry, &index_entry);
        if (ret != RT_ERR_OK)
        {
            _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "");
            return RT_ERR_L2_NO_EMPTY_ENTRY;
        }

        osal_memcpy(&pMcastAddr->macInfo.mac, &l2_entry.unicast.mac, ETHER_ADDR_LEN);
        pMcastAddr->macInfo.devID      = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
        pMcastAddr->macInfo.port      = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
        pMcastAddr->macInfo.vid       = l2_entry.unicast.fid;
        pMcastAddr->macInfo.flags     = l2_entry.unicast.is_trk ? RTK_L2_UCAST_FLAG_TRUNK_PORT : 0;
        pMcastAddr->macInfo.state     = l2_entry.unicast.suspending ? RTK_L2_UCAST_STATE_SUSPEND : 0;
        pMcastAddr->macInfo.trk_gid   = l2_entry.unicast.is_trk ? (l2_entry.unicast.slp & 0xff) : 0;
    }
    else if (RT_ERR_OK != ret)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }


    /* traditional API (if groupId == 0) */
    if (0 == pMcastAddr->groupId)
    {
        if (ret == RT_ERR_OK && l2_entry.is_entry_exist && l2_entry.l2mcast.local_fwd)
        {
            oldFwdIdx = (int32)(l2_entry.l2mcast.index);
        }

        fwdIndex = pMcastAddr->fwdIndex;
        if ((ret = _dal_mango_l2_mcastFwdIndex_alloc(unit, &pMcastAddr->fwdIndex)) != RT_ERR_OK)
        {
            _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
            L2_SEM_UNLOCK(unit);
            return ret;
        }

        if (fwdIndex < 0)
        {
            if((ret = _dal_mango_l2_mcastFwdPortmaskEntry_set(unit, pMcastAddr->fwdIndex, &pMcastAddr->portmask)) != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_LOG(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }
    }

    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.valid  = TRUE;
    l2_entry.l2mcast.fid    = pMcastAddr->rvid;
    osal_memcpy(&l2_entry.l2mcast.mac, &(pMcastAddr->mac), sizeof(rtk_mac_t));

    pMcastAddr->l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
    l2_entry.l2mcast.nh = nh_used_cnt[unit][pMcastAddr->l2_idx] ? 1 : 0;

    /* group process */
    if ((l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId != pMcastAddr->groupId) || (0 == pMcastAddr->groupId))
    {
        /* release old group if it exists */
        if (l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId > 0)
        {
            /* call mcast API to unbind the group */
            ret = _dal_mango_mcast_l2mc_unbind(unit, l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId, &l2Db[unit].pEntry[pMcastAddr->l2_idx].mcastNode);
            if (ret != RT_ERR_OK)
            {
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }

        /* update database */
        ori_group = l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId;
        l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId = pMcastAddr->groupId;
        if (l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId > 0)
        {
            /* call mcast API to bind the group with the L2 entry */
            ret = _dal_mango_mcast_l2mc_bind(unit, l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId, &l2Db[unit].pEntry[pMcastAddr->l2_idx].mcastNode, &mcastL2Info);
            if (ret != RT_ERR_OK)
            {
                l2Db[unit].pEntry[pMcastAddr->l2_idx].groupId = ori_group;
                _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }

            if (mcastL2Info.fwdIndex >= 0)
            {
                l2_entry.l2mcast.local_fwd = 1;
                l2_entry.l2mcast.index = mcastL2Info.fwdIndex;
            } else {
                l2_entry.l2mcast.local_fwd = 0;
                l2_entry.l2mcast.index = 0;
            }

            l2_entry.l2mcast.remote_fwd = mcastL2Info.l2_tnl_lst_valid;
            l2_entry.l2mcast.tunnel_list_idx = mcastL2Info.l2_tnl_lst_idx;
        } else {
            /* traditional API (groupId = 0) */
            l2_entry.l2mcast.index      = (uint32)pMcastAddr->fwdIndex;
            l2_entry.l2mcast.local_fwd  = 1;
            l2_entry.l2mcast.remote_fwd = 0;
            l2_entry.l2mcast.tunnel_list_idx = 0;
        }
    }

    /* write the L2 entry to update data */
    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
    if (ret != RT_ERR_OK)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    /* traditional API (if groupId == 0) */
    if (0 == pMcastAddr->groupId)
    {
        if (oldFwdIdx != -1)
        {
            _dal_mango_l2_mcastFwdIndex_free(unit, oldFwdIdx);
        }
    }

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of l2 table to get next.
 * Output:
 *      pMcastAddr  - structure of l2 address data
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
dal_mango_l2_nextValidMcastAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_mcastAddr_t  *pMcastAddr)
{
    int32 ret;
    rtk_l2_entry_t  entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);
    RT_PARAM_CHK(NULL == pScan_idx, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);


    if (*pScan_idx < 0)
        *pScan_idx = 0;
    else
        *pScan_idx += 1;

    osal_memset(&entry, 0, sizeof(entry));
    ret = dal_mango_l2_hwNextValidAddr_get(unit, pScan_idx, L2_NEXT_VALID_TYPE_MC, &entry);

    osal_memcpy(pMcastAddr, &entry.l2mcast, sizeof(rtk_l2_mcastAddr_t));
    pMcastAddr->l2_idx = TBL_IDX_XLATE_L(*pScan_idx);
    return ret;
}

/* Function Name:
 *      _dal_mango_l2_mcastNexthop_add
 * Description:
 *      Add L2 nexthop entry to ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_mcastNhAddr_t  - nexthop entry
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
_dal_mango_l2_mcastNexthop_add(uint32 unit, dal_mango_l2_mcastNhAddr_t *pNexthop_addr)
{
    int32                   ret, index;
    uint32                  l2_idx_l;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;

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

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memcpy(&l2_entry.l2mcast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.fid    = pNexthop_addr->vid;


    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry);
    if (RT_ERR_OK != ret)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (FALSE == l2_entry.is_entry_exist)
    {
        index = -1;
        if ((ret = _dal_mango_l2_mcastFwdIndex_alloc(unit, &index)) != RT_ERR_OK)
        {
            _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
            L2_SEM_UNLOCK(unit);
            return ret;
        }
        l2_entry.l2mcast.index = index;
    }

    l2_entry.l2mcast.nh = 1;
    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
    l2_idx_l = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
    pNexthop_addr->l2_idx = TBL_IDX_XLATE_P(l2_idx_l);  /* l2_idx is physical index in this internal API */

    nh_used_cnt[unit][l2_idx_l]++;

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      _dal_mango_l2_mcastNexthop_del
 * Description:
 *      Delete L2 multicast nexthop entry from ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_mcastNhAddr_t  - nexthop entry
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
 *          this function will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
_dal_mango_l2_mcastNexthop_del(uint32 unit, dal_mango_l2_mcastNhAddr_t *pNexthop_addr)
{
    int32                   ret;
    uint32                  index;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;

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

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memcpy(&l2_entry.l2mcast.mac, &(pNexthop_addr->mac), sizeof(rtk_mac_t));
    l2_entry.entry_type     = L2_MULTICAST;
    l2_entry.l2mcast.fid    = pNexthop_addr->vid;


    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry);
    if (RT_ERR_OK != ret)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if (l2_entry.l2mcast.nh)
    {
        index = L2_ENTRY_UNIQUE_IDX(unit, index_entry); /* logical index */

        if (nh_used_cnt[unit][index] > 0)
            nh_used_cnt[unit][index]--;
        if (nh_used_cnt[unit][index] == 0)
        {
            l2_entry.l2mcast.nh = 0;
            ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);
        }
    }
    else
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        return RT_ERR_L2_NEXTHOP_NOT_EXIST;
    }

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Module Name    : L2                         */
/* Sub-module Name: Multicast forwarding table */

/* Function Name:
 *      dal_mango_l2_mcastFwdIndex_alloc
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
dal_mango_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pFwdIndex, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(*pFwdIndex >= (int32)mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);

    L2_SEM_LOCK(unit);
    if ((ret = _dal_mango_l2_mcastFwdIndex_alloc(unit, pFwdIndex)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_mcastFwdIndex_free
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
dal_mango_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(index >= mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK(index < 0, RT_ERR_L2_MULTI_FWD_INDEX);

    L2_SEM_LOCK(unit);
    if ((ret = _dal_mango_l2_mcastFwdIndex_free(unit, index)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_mcastFwdPortmaskEntry_get
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
dal_mango_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pPortmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(index >= mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);

    L2_SEM_LOCK(unit);
    if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, index, pPortmask)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "return failed ret value = %x",ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pPortmask, 0),
        RTK_PORTMASK_WORD_GET(*pPortmask, 1));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_mcastFwdPortmaskEntry_set
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
dal_mango_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    RT_PARAM_CHK(NULL == pPortmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(index >= mcast_tableSize[unit], RT_ERR_L2_MULTI_FWD_INDEX);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pPortmask), RT_ERR_PORT_MASK);

    L2_SEM_LOCK(unit);
    if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_set(unit, index, pPortmask)) != RT_ERR_OK)
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
 *      dal_mango_l2_cpuMacAddr_add
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
dal_mango_l2_cpuMacAddr_add(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32               ret;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pMac->octet[0] & BITMASK_1B) != 0, RT_ERR_MAC);
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));
    l2_entry.entry_type     = L2_UNICAST;
    l2_entry.unicast.fid    = vid;

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_OR_FREE, &index_entry)) != RT_ERR_OK)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));
    l2_entry.entry_type         = L2_UNICAST;
    l2_entry.unicast.valid      = TRUE;
    l2_entry.unicast.fid        = vid;
    l2_entry.unicast.is_trk     = 0;
    l2_entry.unicast.slp        = HWP_CPU_MACID(unit);
    l2_entry.unicast.aging      = 7;
    l2_entry.unicast.is_static  = 1;

    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_cpuMacAddr_del
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
dal_mango_l2_cpuMacAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    int32 ret;
    dal_mango_l2_entry_t  l2_entry;
    dal_mango_l2_index_t  index_entry;

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

    osal_memset(&index_entry, 0, sizeof(dal_mango_l2_index_t));
    /* search exist or free entry */
    l2_entry.entry_type = L2_UNICAST;
    l2_entry.unicast.fid = vid;
    osal_memcpy(&l2_entry.unicast.mac, pMac, sizeof(rtk_mac_t));

    L2_SEM_LOCK(unit);
    /* [SEM_LOCK] L2 table */
    if ((ret = _dal_mango_switch_tblSemaphore_lock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    if ((ret = _dal_mango_l2_getExistOrFreeL2Entry(unit, &l2_entry, L2_GET_EXIST_ONLY, &index_entry)) != RT_ERR_OK)
    {
        _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d", ret);
        return ret;
    }

    /* fill content */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));
    l2_entry.entry_type = L2_UNICAST;
    ret = _dal_mango_l2_setL2Entry(unit, &index_entry, &l2_entry);

    /* [SEM_UNLOCK] L2 table */
    _dal_mango_switch_tblSemaphore_unlock(unit, DAL_MANGO_SWITCH_TBL_L2_MAC);
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Module Name    : L2        */
/* Sub-module Name: Port move */

/* Function Name:
 *      dal_mango_l2_portMoveAction_get
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
dal_mango_l2_portMoveAction_get(
    uint32                  unit,
    rtk_l2_portMoveType_t   type,
    rtk_l2_portMoveAct_t    *pAction)
{
    int32           ret;
    uint32          value = 0;
    rtk_action_t    action = 0;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_PORT_MOVE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type == L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pAction->dynAct.port)), RT_ERR_PORT_ID);

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if ((ret = reg_array_field_read(unit, MANGO_L2_PORT_DYN_MV_ACTr, pAction->dynAct.port, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_STATIC:
            if ((ret = reg_field_read(unit, MANGO_L2_GLB_STT_PORT_MV_ACTr, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_FORBID:
            if ((ret = reg_field_read(unit, MANGO_L2_PORT_MV_FORBID_CTRLr, MANGO_FORBID_ACTf, &value)) != RT_ERR_OK)
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

    switch (value)
    {
        case 0:
            action = ACTION_FORWARD;
            break;
        case 1:
            action = ACTION_DROP;
            break;
        case 2:
            action = ACTION_TRAP2CPU;
            break;
        case 3:
            action = ACTION_COPY2CPU;
            break;
        case 4:
            action = ACTION_TRAP2MASTERCPU;
            break;
        case 5:
            action = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            pAction->dynAct.act = action;
            break;
        case L2_PORT_MOVE_STATIC:
            pAction->sttAct.act = action;
            break;
        case L2_PORT_MOVE_FORBID:
            pAction->forbidAct.act = action;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portMoveAction_set
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
dal_mango_l2_portMoveAction_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction)
{
    int32   ret;
    uint32  value;
    rtk_action_t action;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_PORT_MOVE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type == L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pAction->dynAct.port)), RT_ERR_PORT_ID);

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            action= pAction->dynAct.act;
            break;
        case L2_PORT_MOVE_STATIC:
            action= pAction->sttAct.act;
            break;
        case L2_PORT_MOVE_FORBID:
            action= pAction->forbidAct.act;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (action)
    {
        case ACTION_FORWARD:
            value = 0;
            break;
        case ACTION_DROP:
            value = 1;
            break;
        case ACTION_TRAP2CPU:
            value = 2;
            break;
        case ACTION_COPY2CPU:
            value = 3;
            break;
        case ACTION_TRAP2MASTERCPU:
            if (L2_PORT_MOVE_FORBID == type)
                return RT_ERR_FWD_ACTION;
            value = 4;
            break;
        case ACTION_COPY2MASTERCPU:
            if (L2_PORT_MOVE_FORBID == type)
                return RT_ERR_FWD_ACTION;
            value = 5;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if ((ret = reg_array_field_write(unit, MANGO_L2_PORT_DYN_MV_ACTr, pAction->dynAct.port, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_STATIC:
            if ((ret = reg_field_write(unit, MANGO_L2_GLB_STT_PORT_MV_ACTr, MANGO_ACTf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_FORBID:
            if ((ret = reg_field_write(unit, MANGO_L2_PORT_MV_FORBID_CTRLr, MANGO_FORBID_ACTf, &value)) != RT_ERR_OK)
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
 *      dal_mango_l2_portMoveLearn_get
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
dal_mango_l2_portMoveLearn_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    int32   ret;
    uint32  value = 0;
    rtk_enable_t lrn;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_PORT_MOVE_FORBID), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLearn), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type == L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pLearn->dynLrn.port)), RT_ERR_PORT_ID);

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if ((ret = reg_array_field_read(unit, MANGO_L2_PORT_DYN_MV_LRNr, pLearn->dynLrn.port, REG_ARRAY_INDEX_NONE, MANGO_LRNf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_STATIC:
            if ((ret = reg_field_read(unit, MANGO_L2_GLB_STT_PORT_MV_LRNr, MANGO_LRNf, &value)) != RT_ERR_OK)
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

    switch (value)
    {
        case 0:
            lrn = DISABLED;
            break;
        case 1:
            lrn = ENABLED;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            pLearn->dynLrn.enable = lrn;
            break;
        case L2_PORT_MOVE_STATIC:
            pLearn->sttLrn.enable = lrn;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portMoveLearn_set
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
dal_mango_l2_portMoveLearn_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    int32   ret;
    uint32  value;
    rtk_enable_t lrn;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= L2_PORT_MOVE_FORBID), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLearn), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type == L2_PORT_MOVE_DYNAMIC && !HWP_PORT_EXIST(unit, pLearn->dynLrn.port)), RT_ERR_PORT_ID);

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            lrn = pLearn->dynLrn.enable;
            break;
        case L2_PORT_MOVE_STATIC:
            lrn = pLearn->sttLrn.enable;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (lrn)
    {
        case DISABLED:
            value = 0;
            break;
        case ENABLED:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    switch (type)
    {
        case L2_PORT_MOVE_DYNAMIC:
            if ((ret = reg_array_field_write(unit, MANGO_L2_PORT_DYN_MV_LRNr, pLearn->dynLrn.port, REG_ARRAY_INDEX_NONE, MANGO_LRNf, &value)) != RT_ERR_OK)
            {
                L2_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
            break;
        case L2_PORT_MOVE_STATIC:
            if ((ret = reg_field_write(unit, MANGO_L2_GLB_STT_PORT_MV_LRNr, MANGO_LRNf, &value)) != RT_ERR_OK)
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
 *      dal_mango_l2_lookupMissFloodPortMask_get
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
dal_mango_l2_lookupMissFloodPortMask_get(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pFlood_portmask, RT_ERR_NULL_POINTER);

    switch (type)
    {
        case DLF_TYPE_UCAST:
            reg_idx = MANGO_L2_UNKN_UC_FLD_PMSKr;
            break;
        case DLF_TYPE_BCAST:
            reg_idx = MANGO_L2_BC_FLD_PMSKr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, reg_idx, MANGO_PMSK_0f, &pFlood_portmask->bits[0])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_field_read(unit, reg_idx, MANGO_PMSK_1f, &pFlood_portmask->bits[1])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "flood pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pFlood_portmask, 0),
        RTK_PORTMASK_WORD_GET(*pFlood_portmask, 1));

    return ret;
}

/* Function Name:
 *      dal_mango_l2_lookupMissFloodPortMask_set
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
dal_mango_l2_lookupMissFloodPortMask_set(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, type=%d, flood pmsk0=0x%x, pmsk1=0x%x", unit, type,
        RTK_PORTMASK_WORD_GET(*pFlood_portmask, 0),
        RTK_PORTMASK_WORD_GET(*pFlood_portmask, 1));

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= DLF_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pFlood_portmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pFlood_portmask), RT_ERR_PORT_MASK);

    switch (type)
    {
        case DLF_TYPE_UCAST:
            reg_idx = MANGO_L2_UNKN_UC_FLD_PMSKr;
            break;
        case DLF_TYPE_BCAST:
            reg_idx = MANGO_L2_BC_FLD_PMSKr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, reg_idx, MANGO_PMSK_0f, &pFlood_portmask->bits[0])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, reg_idx, MANGO_PMSK_1f, &pFlood_portmask->bits[1])) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}


/* Function Name:
 *      dal_mango_l2_lookupMissFloodPortMask_add
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
dal_mango_l2_lookupMissFloodPortMask_add(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
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

    ret = dal_mango_l2_lookupMissFloodPortMask_get(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, flood_port);

    ret = dal_mango_l2_lookupMissFloodPortMask_set(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_lookupMissFloodPortMask_del
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
dal_mango_l2_lookupMissFloodPortMask_del(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
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

    ret = dal_mango_l2_lookupMissFloodPortMask_get(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, flood_port);

    ret = dal_mango_l2_lookupMissFloodPortMask_set(unit, type, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portLookupMissAction_get
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
dal_mango_l2_portLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    RT_PARAM_CHK((type != DLF_TYPE_UCAST), RT_ERR_INPUT);

    return dal_mango_l2_portUcastLookupMissAction_get(unit, port, pAction);
}

/* Function Name:
 *      dal_mango_l2_portLookupMissAction_set
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
dal_mango_l2_portLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    RT_PARAM_CHK((type != DLF_TYPE_UCAST), RT_ERR_INPUT);

    return dal_mango_l2_portUcastLookupMissAction_set(unit, port, action);
}

/* Function Name:
 *      dal_mango_l2_portUcastLookupMissAction_get
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
dal_mango_l2_portUcastLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    int32   ret;
    uint32  act = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MANGO_L2_PORT_UC_LM_ACTr, port, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "ret=%d",ret);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    switch (act)
    {
        case 0:
            *pAction = ACTION_FORWARD;
            break;
        case 1:
            *pAction = ACTION_DROP;
            break;
        case 2:
            *pAction = ACTION_TRAP2CPU;
            break;
        case 3:
            *pAction = ACTION_COPY2CPU;
            break;
        case 4:
            *pAction = ACTION_TRAP2MASTERCPU;
            break;
        case 5:
            *pAction = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pAction:%d",  *pAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portUcastLookupMissAction_set
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
dal_mango_l2_portUcastLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    int32   ret;
    uint32  act;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, action=%d", unit, port, action);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(action >= ACTION_END, RT_ERR_INPUT);

    switch (action)
    {
        case ACTION_FORWARD:
            act = 0;
            break;
        case ACTION_DROP:
            act = 1;
            break;
        case ACTION_TRAP2CPU:
            act = 2;
            break;
        case ACTION_COPY2CPU:
            act = 3;
            break;
        case ACTION_TRAP2MASTERCPU:
            act = 4;
            break;
        case ACTION_COPY2MASTERCPU:
            act = 5;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MANGO_L2_PORT_UC_LM_ACTr, port, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &act)) != RT_ERR_OK)
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
 *      dal_mango_l2_srcPortEgrFilterMask_get
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
dal_mango_l2_srcPortEgrFilterMask_get(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    int32       ret = RT_ERR_FAILED;
    uint32      value = 0;
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
        if ((ret = reg_array_field_read(unit, MANGO_L2_SRC_P_FLTRr, port, REG_ARRAY_INDEX_NONE, MANGO_SRC_FLTR_ENf, &value)) != RT_ERR_OK)
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

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "filter pmsk0=0x%x, pmsk1=0x%x",
        RTK_PORTMASK_WORD_GET(*pFilter_portmask, 0),
        RTK_PORTMASK_WORD_GET(*pFilter_portmask, 1));

    return ret;
}

/* Function Name:
 *      dal_mango_l2_srcPortEgrFilterMask_set
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
dal_mango_l2_srcPortEgrFilterMask_set(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    int32       ret = RT_ERR_FAILED;
    uint32      value;
    rtk_port_t  port;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pFilter_portmask, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pFilter_portmask), RT_ERR_PORT_MASK);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, filter pmsk0=0x%x, pmsk1=0x%x", unit,
        RTK_PORTMASK_WORD_GET(*pFilter_portmask, 0),
        RTK_PORTMASK_WORD_GET(*pFilter_portmask, 1));

    L2_SEM_LOCK(unit);

    HWP_PORT_TRAVS(unit, port)
    {
        value = (RTK_PORTMASK_IS_PORT_SET(*pFilter_portmask, port) ? 1 : 0);
        if ((ret = reg_array_field_write(unit, MANGO_L2_SRC_P_FLTRr, port, REG_ARRAY_INDEX_NONE, MANGO_SRC_FLTR_ENf, &value)) != RT_ERR_OK)
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
 *      dal_mango_l2_srcPortEgrFilterMask_add
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
dal_mango_l2_srcPortEgrFilterMask_add(uint32 unit, rtk_port_t filter_port)
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

    ret = dal_mango_l2_srcPortEgrFilterMask_get(unit, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    RTK_PORTMASK_PORT_SET(portmask, filter_port);

    ret = dal_mango_l2_srcPortEgrFilterMask_set(unit, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_srcPortEgrFilterMask_del
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
dal_mango_l2_srcPortEgrFilterMask_del(uint32 unit, rtk_port_t filter_port)
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

    ret = dal_mango_l2_srcPortEgrFilterMask_get(unit, &portmask);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    RTK_PORTMASK_PORT_CLEAR(portmask, filter_port);

    ret = dal_mango_l2_srcPortEgrFilterMask_set(unit, &portmask);
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
 *      dal_mango_l2_exceptionAddrAction_get
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
dal_mango_l2_exceptionAddrAction_get(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    *pAction)
{
    int32   ret;
    uint32  act = 0;
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
            reg_filed = MANGO_ZERO_SA_ACTf;
            break;
        case SA_IS_BCAST_OR_MCAST:
            reg_filed = MANGO_MC_BC_SA_ACTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, reg_filed, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    switch (act)
    {
        case 0:
            *pAction = ACTION_FORWARD;
            break;
        case 1:
            *pAction = ACTION_DROP;
            break;
        case 2:
            *pAction = ACTION_TRAP2CPU;
            break;
        case 3:
            *pAction = ACTION_TRAP2MASTERCPU;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    return ret;
}

/* Function Name:
 *      dal_mango_l2_exceptionAddrAction_set
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
dal_mango_l2_exceptionAddrAction_set(
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

    switch (action)
    {
        case ACTION_FORWARD:
            act = 0;
            break;
        case ACTION_DROP:
            act = 1;
            break;
        case ACTION_TRAP2CPU:
            act = 2;
            break;
        case ACTION_TRAP2MASTERCPU:
            act = 3;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    switch (exceptType)
    {
        case SA_IS_ZERO:
            reg_filed = MANGO_ZERO_SA_ACTf;
            break;
        case SA_IS_BCAST_OR_MCAST:
            reg_filed = MANGO_MC_BC_SA_ACTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, reg_filed, &act)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return ret;
}

/* Function Name:
 *      dal_mango_l2_addrEntry_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The index valid range is from 0 to (L2 hash table size - 1)
 *          - 0 ~ (L2 hash table size - 1) entry in L2 hash table
 *      (2) The output entry have 2 variables (valid and entry_type) and its detail data structure
 *          - valid: 1 mean the entry is valid, 0: invalid
 *          - entry_type: FLOW_TYPE_UNICAST, FLOW_TYPE_L2_MULTI and FLOW_TYPE_IP_MULTI
 *                        the field is ignored if valid field is 0.
 *          - detail data structure is ignored if valid is 0, and its filed meanings is depended
 *            on the entry_type value.
 *      (3) If pL2_entry->flags have enabled the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *          pL2_entry->unicast.trk_gid is valid trunk id value.
 */
int32
dal_mango_l2_addrEntry_get(uint32 unit, uint32 index, rtk_l2_entry_t *pL2_entry)
{
    int32 ret;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    index_entry;
    rtk_enable_t            l2CamEbl;
    uint32                  value = 0;

    RT_PARAM_CHK((NULL == pL2_entry), RT_ERR_NULL_POINTER);

    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));

    if ((ret=reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value))!=RT_ERR_OK)
        return ret;
    l2CamEbl = value == 1 ? ENABLED : DISABLED;

    if ((index >= (hashTable_size[unit] + cam_size[unit])) || ((index >= hashTable_size[unit]) && ((l2CamEbl == DISABLED) || (ld_patch_en == TRUE))))
        return RT_ERR_INPUT;

    if (index < hashTable_size[unit])
    {
        index_entry.index_type = L2_IN_HASH;
        index_entry.index = index >> 2;
        index_entry.hashdepth = index & 0x3;
    }
    else
    {
        index_entry.index_type = L2_IN_CAM;
        index_entry.index = index - hashTable_size[unit];
    }

    L2_SEM_LOCK(unit);
    if ((ret = _dal_mango_l2_getL2Entry(unit, &index_entry, &l2_entry)) != RT_ERR_OK)
    {
        RT_LOG(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    pL2_entry->valid = l2_entry.is_entry_valid;

    if (!pL2_entry->valid)
        return RT_ERR_OK;

    switch (l2_entry.entry_type)
    {
        case L2_UNICAST:
            pL2_entry->entry_type = FLOW_TYPE_UNICAST;
            break;
        case L2_MULTICAST:
            pL2_entry->entry_type = FLOW_TYPE_L2_MULTI;
            break;
        case ENTRY_OPENFLOW:
            pL2_entry->entry_type = FLOW_TYPE_END;
            return RT_ERR_L2_INVALID_FLOWTYPE;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    if (pL2_entry->entry_type == FLOW_TYPE_UNICAST)
    {
        /* fill content */
        osal_memcpy(&pL2_entry->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
        pL2_entry->unicast.vid          = l2_entry.unicast.fid;
        pL2_entry->unicast.flags        = 0;    /* Clear data */
        pL2_entry->unicast.state        = 0;    /* Clear data */
        if (l2_entry.unicast.is_trk)
        {
            pL2_entry->unicast.flags    |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
            pL2_entry->unicast.trk_gid  = l2_entry.unicast.slp & 0xff;
        }
        else
        {
            pL2_entry->unicast.devID    = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
            pL2_entry->unicast.port     = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
        }

        if (l2_entry.unicast.sablock)
            pL2_entry->unicast.flags    |= RTK_L2_UCAST_FLAG_SA_BLOCK;
        if (l2_entry.unicast.dablock)
            pL2_entry->unicast.flags    |= RTK_L2_UCAST_FLAG_DA_BLOCK;
        if (l2_entry.unicast.is_static)
            pL2_entry->unicast.flags    |= RTK_L2_UCAST_FLAG_STATIC;
        if (l2_entry.unicast.suspending)
            pL2_entry->unicast.state    |= RTK_L2_UCAST_STATE_SUSPEND;
        if (l2_entry.unicast.nh)
            pL2_entry->unicast.flags    |= RTK_L2_UCAST_FLAG_NEXTHOP;
        if (l2_entry.unicast.aging == 0 && l2_entry.unicast.is_trk == 0 && l2_entry.unicast.slp == PORT_DONT_CARE_9310)
            pL2_entry->unicast.isAged   = TRUE;
        else
            pL2_entry->unicast.isAged   = FALSE;
        if (l2_entry.unicast.tagSts)
            pL2_entry->unicast.flags    |= RTK_L2_UCAST_FLAG_TAG_STS;
        pL2_entry->unicast.agg_pri      = l2_entry.unicast.agg_pri;
        pL2_entry->unicast.agg_vid      = l2_entry.unicast.agg_vid;
        pL2_entry->unicast.ecid         = l2_entry.unicast.ecid;
        if (l2_entry.unicast.l2_tnl_if)
            pL2_entry->unicast.flags    |= RTK_L2_UCAST_FLAG_L2_TUNNEL;
        pL2_entry->unicast.l2_tunnel_idx = l2_entry.unicast.l2_tnl_idx;
        pL2_entry->unicast.age          = l2_entry.unicast.aging;

        pL2_entry->unicast.l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
    }

    if (pL2_entry->entry_type == FLOW_TYPE_L2_MULTI)
    {
        /* fill content */
        osal_memcpy(&pL2_entry->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
        pL2_entry->l2mcast.rvid     = l2_entry.l2mcast.fid;
        pL2_entry->l2mcast.fwdIndex = l2_entry.l2mcast.index;
        pL2_entry->l2mcast.nextHop  = l2_entry.l2mcast.nh;

        L2_SEM_LOCK(unit);
        if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pL2_entry->l2mcast.fwdIndex, &pL2_entry->l2mcast.portmask)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);

        pL2_entry->l2mcast.l2_idx = L2_ENTRY_UNIQUE_IDX(unit, index_entry);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_conflictAddr_get
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
dal_mango_l2_conflictAddr_get(
    uint32          unit,
    rtk_l2_entry_t  *pL2Addr,
    rtk_l2_entry_t  *pCfAddrList,
    uint32          cfAddrList_size,
    uint32          *pCf_retCnt)
{
    int32   ret;
    uint32  cf_num = 0;

    dal_mango_l2_hashIdx_t  hashIdx;
    dal_mango_l2_entry_t    l2_entry;
    dal_mango_l2_index_t    blk0_index;
    dal_mango_l2_index_t    blk1_index;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* check input parameters */
    RT_PARAM_CHK(NULL == pL2Addr, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pCfAddrList, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(0 == cfAddrList_size, RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pCf_retCnt, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pL2Addr->entry_type != FLOW_TYPE_UNICAST && pL2Addr->entry_type != FLOW_TYPE_L2_MULTI), RT_ERR_OUT_OF_RANGE);

    osal_memset(&hashIdx, 0, sizeof(dal_mango_l2_hashIdx_t));
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));

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

    /* calculate hash idx */
    if ((ret = _dal_mango_l2_entryToHashIdx(unit, &l2_entry, &hashIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    blk0_index.index_type = L2_IN_HASH;
    blk0_index.index = hashIdx.blk0_hashIdx;

    L2_SEM_LOCK(unit);
    cf_num = 0;
    for (blk0_index.hashdepth = 0; blk0_index.hashdepth < HAL_L2_HASHDEPTH(unit) && cf_num < cfAddrList_size; blk0_index.hashdepth++)
    {
        if ((ret=_dal_mango_l2_getL2Entry(unit, &blk0_index, &l2_entry))!= RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            return ret;
        }

        if (l2_entry.is_entry_valid == 1)
        {
            (pCfAddrList + cf_num)->valid = l2_entry.is_entry_valid;
            if (l2_entry.entry_type == L2_UNICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_UNICAST;
                osal_memcpy(&(pCfAddrList + cf_num)->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
                (pCfAddrList + cf_num)->unicast.vid         = l2_entry.unicast.fid;
                (pCfAddrList + cf_num)->unicast.flags       = 0;    /* Clear data */
                (pCfAddrList + cf_num)->unicast.state       = 0;    /* Clear data */
                if (l2_entry.unicast.is_trk)
                {
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
                    (pCfAddrList + cf_num)->unicast.trk_gid = l2_entry.unicast.slp & 0xff;
                }
                else
                {
                    (pCfAddrList + cf_num)->unicast.devID   = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
                    (pCfAddrList + cf_num)->unicast.port    = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
                }

                if (l2_entry.unicast.sablock)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_SA_BLOCK;
                if (l2_entry.unicast.dablock)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_DA_BLOCK;
                if (l2_entry.unicast.is_static)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_STATIC;
                if (l2_entry.unicast.suspending)
                    (pCfAddrList + cf_num)->unicast.state   |= RTK_L2_UCAST_STATE_SUSPEND;
                if (l2_entry.unicast.nh)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_NEXTHOP;
                if (l2_entry.unicast.aging == 0 && l2_entry.unicast.is_trk == 0 && l2_entry.unicast.slp == PORT_DONT_CARE_9310)
                    (pCfAddrList + cf_num)->unicast.isAged  = TRUE;
                else
                    (pCfAddrList + cf_num)->unicast.isAged  = FALSE;
                if (l2_entry.unicast.tagSts)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_TAG_STS;
                (pCfAddrList + cf_num)->unicast.agg_pri     = l2_entry.unicast.agg_pri;
                (pCfAddrList + cf_num)->unicast.agg_vid     = l2_entry.unicast.agg_vid;
                (pCfAddrList + cf_num)->unicast.ecid        = l2_entry.unicast.ecid;
                if (l2_entry.unicast.l2_tnl_if)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_L2_TUNNEL;
                (pCfAddrList + cf_num)->unicast.l2_tunnel_idx = l2_entry.unicast.l2_tnl_idx;
                (pCfAddrList + cf_num)->unicast.age         = l2_entry.unicast.aging;

                (pCfAddrList + cf_num)->unicast.l2_idx      = (blk0_index.index << 2) | blk0_index.hashdepth;
            }
            else if (l2_entry.entry_type == L2_MULTICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_L2_MULTI;
                osal_memcpy(&(pCfAddrList + cf_num)->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
                (pCfAddrList + cf_num)->l2mcast.rvid       = l2_entry.l2mcast.fid;
                (pCfAddrList + cf_num)->l2mcast.fwdIndex = l2_entry.l2mcast.index;
                (pCfAddrList + cf_num)->l2mcast.nextHop = l2_entry.l2mcast.nh;

                if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, (pCfAddrList + cf_num)->l2mcast.fwdIndex, &(pCfAddrList + cf_num)->l2mcast.portmask)) != RT_ERR_OK)
                {
                    L2_SEM_UNLOCK(unit);
                    RT_LOG(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }

                (pCfAddrList + cf_num)->l2mcast.l2_idx = (blk0_index.index << 2) | blk0_index.hashdepth;

                /* group id */
                (pCfAddrList + cf_num)->l2mcast.groupId = l2Db[unit].pEntry[((pCfAddrList + cf_num)->l2mcast.l2_idx)].groupId;
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
    blk1_index.index = hashIdx.blk1_hashIdx;
    for (blk1_index.hashdepth = 0; blk1_index.hashdepth < HAL_L2_HASHDEPTH(unit) && cf_num < cfAddrList_size; blk1_index.hashdepth++)
    {
        if ((ret=_dal_mango_l2_getL2Entry(unit, &blk1_index, &l2_entry))!= RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            return ret;
        }

        if (l2_entry.is_entry_valid == 1)
        {
            (pCfAddrList + cf_num)->valid = l2_entry.is_entry_valid;
            if (l2_entry.entry_type == L2_UNICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_UNICAST;
                osal_memcpy(&(pCfAddrList + cf_num)->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
                (pCfAddrList + cf_num)->unicast.vid         = l2_entry.unicast.fid;
                (pCfAddrList + cf_num)->unicast.flags       = 0;    /* Clear data */
                (pCfAddrList + cf_num)->unicast.state       = 0;    /* Clear data */
                if (l2_entry.unicast.is_trk)
                {
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
                    (pCfAddrList + cf_num)->unicast.trk_gid = l2_entry.unicast.slp & 0xff;
                }
                else
                {
                    (pCfAddrList + cf_num)->unicast.devID   = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
                    (pCfAddrList + cf_num)->unicast.port    = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
                }

                if (l2_entry.unicast.sablock)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_SA_BLOCK;
                if (l2_entry.unicast.dablock)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_DA_BLOCK;
                if (l2_entry.unicast.is_static)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_STATIC;
                if (l2_entry.unicast.suspending)
                    (pCfAddrList + cf_num)->unicast.state   |= RTK_L2_UCAST_STATE_SUSPEND;
                if (l2_entry.unicast.nh)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_NEXTHOP;
                if (l2_entry.unicast.aging == 0 && l2_entry.unicast.is_trk == 0 && l2_entry.unicast.slp == PORT_DONT_CARE_9310)
                    (pCfAddrList + cf_num)->unicast.isAged  = TRUE;
                else
                    (pCfAddrList + cf_num)->unicast.isAged  = FALSE;
                if (l2_entry.unicast.tagSts)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_TAG_STS;
                (pCfAddrList + cf_num)->unicast.agg_pri     = l2_entry.unicast.agg_pri;
                (pCfAddrList + cf_num)->unicast.agg_vid     = l2_entry.unicast.agg_vid;
                (pCfAddrList + cf_num)->unicast.ecid        = l2_entry.unicast.ecid;
                if (l2_entry.unicast.l2_tnl_if)
                    (pCfAddrList + cf_num)->unicast.flags   |= RTK_L2_UCAST_FLAG_L2_TUNNEL;
                (pCfAddrList + cf_num)->unicast.l2_tunnel_idx = l2_entry.unicast.l2_tnl_idx;
                (pCfAddrList + cf_num)->unicast.age         = l2_entry.unicast.aging;
                (pCfAddrList + cf_num)->unicast.l2_idx      = (blk1_index.index << 2) | blk1_index.hashdepth;
            }
            else if (l2_entry.entry_type == L2_MULTICAST)
            {
                /* fill content */
                (pCfAddrList + cf_num)->entry_type = FLOW_TYPE_L2_MULTI;
                osal_memcpy(&(pCfAddrList + cf_num)->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
                (pCfAddrList + cf_num)->l2mcast.rvid        = l2_entry.l2mcast.fid;
                (pCfAddrList + cf_num)->l2mcast.fwdIndex    = l2_entry.l2mcast.index;
                (pCfAddrList + cf_num)->l2mcast.nextHop     = l2_entry.l2mcast.nh;

                if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, (pCfAddrList + cf_num)->l2mcast.fwdIndex, &(pCfAddrList + cf_num)->l2mcast.portmask)) != RT_ERR_OK)
                {
                    L2_SEM_UNLOCK(unit);
                    RT_LOG(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }

                (pCfAddrList + cf_num)->l2mcast.l2_idx = (blk1_index.index << 2) | blk1_index.hashdepth;

                /* group id */
                (pCfAddrList + cf_num)->l2mcast.groupId = l2Db[unit].pEntry[((pCfAddrList + cf_num)->l2mcast.l2_idx)].groupId;
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
 *      dal_mango_l2_zeroSALearningEnable_get
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
dal_mango_l2_zeroSALearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_ZERO_SA_LRNf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_zeroSALearningEnable_set
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
dal_mango_l2_zeroSALearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, enable=%d", unit, enable);

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
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, MANGO_ZERO_SA_LRNf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portDynamicPortMoveForbidEnable_get
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
dal_mango_l2_portDynamicPortMoveForbidEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_L2_PORT_MV_FORBIDr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_portDynamicPortMoveForbidEnable_set
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
dal_mango_l2_portDynamicPortMoveForbidEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(unit, MANGO_L2_PORT_MV_FORBIDr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_trkDynamicPortMoveForbidEnable_get
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
dal_mango_l2_trkDynamicPortMoveForbidEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, tid=%d", unit, tid);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >= STANDALONE_TRUNK_ID_MAX), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_L2_TRK_MV_FORBIDr, REG_ARRAY_INDEX_NONE, tid, MANGO_ENf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_trkDynamicPortMoveForbidEnable_set
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
dal_mango_l2_trkDynamicPortMoveForbidEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, tid=%d, enable=%d", unit, tid, enable);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >= STANDALONE_TRUNK_ID_MAX), RT_ERR_TRUNK_ID);
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
    if ((ret = reg_array_field_write(unit, MANGO_L2_TRK_MV_FORBIDr, REG_ARRAY_INDEX_NONE, tid, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portMacFilterEnable_get
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
dal_mango_l2_portMacFilterEnable_get(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  reg;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, port=%d, blockMode=%d", unit, port, filterMode);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (filterMode)
    {
        case MAC_FILTER_MODE_SA:
           reg = MANGO_L2_PORT_SABLK_CTRLr;
           break;
        case MAC_FILTER_MODE_DA:
           reg = MANGO_L2_PORT_DABLK_CTRLr;
           break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_portMacFilterEnable_set
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
dal_mango_l2_portMacFilterEnable_set(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t enable)
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

    switch (filterMode)
    {
        case MAC_FILTER_MODE_SA:
           reg = MANGO_L2_PORT_SABLK_CTRLr;
           break;
        case MAC_FILTER_MODE_DA:
           reg = MANGO_L2_PORT_DABLK_CTRLr;
           break;
        default:
            return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, reg, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_hwNextValidAddr_get
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
dal_mango_l2_hwNextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_nextValidType_t type,
    rtk_l2_entry_t  *pEntry)
{
    int32 ret;
    dal_mango_l2_entry_t    l2_entry;
    uint32                  index = 0;
    uint32                  is_found = 0;
    uint32                  index_type = L2_IN_HASH;
    rtk_enable_t            l2CamEbl;
    uint32                  value = 0;
    dal_mango_l2_index_t    index_entry;
    uint32                  *ptr;

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);
    RT_PARAM_CHK(NULL == pScan_idx, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((type > L2_NEXT_VALID_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* search exist or free entry */
    osal_memset(&l2_entry, 0, sizeof(dal_mango_l2_entry_t));

    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value)) != RT_ERR_OK)
        return ret;
    l2CamEbl = value == 1 ? ENABLED : DISABLED;

    if ((*pScan_idx >= (hashTable_size[unit] + cam_size[unit])) || ((*pScan_idx >= hashTable_size[unit]) && (l2CamEbl == DISABLED)))
        return RT_ERR_INPUT;


    if (L2_NEXT_VALID_TYPE_UC == type)
        value = 1;
    else if (L2_NEXT_VALID_TYPE_AUTO_UC == type)
        value = 2;
    else if (L2_NEXT_VALID_TYPE_SOFTWARE_UC == type)
        value = 3;
    else if (L2_NEXT_VALID_TYPE_UC_NH == type)
        value = 4;
    else if (L2_NEXT_VALID_TYPE_MC == type)
        value = 5;
    else if (L2_NEXT_VALID_TYPE_MC_NH == type)
        value = 6;
    else
    {
        return RT_ERR_INPUT;
    }

    L2_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_TBL_ACCESS_L2_METHOD_CTRLr, MANGO_METHODf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }

    if (*pScan_idx < hashTable_size[unit])
    {
        if ((ret = table_read(unit, TBL_L2_SRAM_FIND_NEXT, HASH_IDX_XLATE_P(*pScan_idx >> 2) << 2 | (*pScan_idx & 0x3), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        ptr = (uint32 *)&l2_entry;

        is_found = ptr[3] >> 15;
        if (is_found)
            index = ptr[3] & 0x7fff;
    }

    if (l2CamEbl == ENABLED && is_found == 0)
    {
        index_type = L2_IN_CAM;
        value = 1;
        index = (*pScan_idx < (int32)hashTable_size[unit]) ? 0 : *pScan_idx - hashTable_size[unit];
        if ((ret = table_read(unit, TBL_L2_BCAM_FIND_NEXT, index, (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        ptr = (uint32 *)&l2_entry;

        is_found = ptr[3] >> 15;
        if (is_found)
            index = ptr[3] & 0x7fff;
    }

    /* reset METHOD bit to normal mode */
    value = 0;
    if ((ret = reg_field_write(unit, MANGO_TBL_ACCESS_L2_METHOD_CTRLr, MANGO_METHODf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }


    if (is_found == 0)
    {
        L2_SEM_UNLOCK(unit);
        return RT_ERR_ENTRY_NOTFOUND;
    }


    if (index_type == L2_IN_HASH)
    {
        index_entry.index_type  = L2_IN_HASH;
        index_entry.index       = TBL_IDX_XLATE_L(index) >> 2;
        index_entry.hashdepth   = TBL_IDX_XLATE_L(index) & 0x3;
    }
    else
    {
        index_entry.index_type  = L2_IN_CAM;
        index_entry.index       = index;
    }

    if ((ret=_dal_mango_l2_getL2Entry(unit, &index_entry, &l2_entry))!= RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        return ret;
    }

    L2_SEM_UNLOCK(unit);


    if (index_type == L2_IN_HASH)
        *pScan_idx  = TBL_IDX_XLATE_L(index);
    else
        *pScan_idx  = hashTable_size[unit] + index;

    pEntry->valid       = l2_entry.is_entry_valid;
    pEntry->entry_type  = l2_entry.entry_type;
    /* fill content */
    if (type < L2_NEXT_VALID_TYPE_MC)
    {
        osal_memcpy(&pEntry->unicast.mac, &l2_entry.unicast.mac, sizeof(rtk_mac_t));
        pEntry->unicast.vid     = l2_entry.unicast.fid;
        pEntry->unicast.flags   = 0;    /* Clear data */
        pEntry->unicast.state   = 0;    /* Clear data */
        if (l2_entry.unicast.is_trk)
        {
            pEntry->unicast.flags     |= RTK_L2_UCAST_FLAG_TRUNK_PORT;
            pEntry->unicast.trk_gid   = l2_entry.unicast.slp & 0xff;
        }
        else
        {
            pEntry->unicast.devID     = l2_entry.unicast.slp >> RTL9310_L2ENTRY_DEVI_ID_OFFSET;
            pEntry->unicast.port      = l2_entry.unicast.slp & RTL9310_L2ENTRY_PORT_MASK;
        }
        if (l2_entry.unicast.sablock)
            pEntry->unicast.flags     |= RTK_L2_UCAST_FLAG_SA_BLOCK;
        if (l2_entry.unicast.dablock)
            pEntry->unicast.flags     |= RTK_L2_UCAST_FLAG_DA_BLOCK;
        if (l2_entry.unicast.is_static)
            pEntry->unicast.flags     |= RTK_L2_UCAST_FLAG_STATIC;
        if (l2_entry.unicast.suspending)
            pEntry->unicast.state     |= RTK_L2_UCAST_STATE_SUSPEND;
        if (l2_entry.unicast.nh)
            pEntry->unicast.flags     |= RTK_L2_UCAST_FLAG_NEXTHOP;
        if (l2_entry.unicast.aging == 0 && l2_entry.unicast.is_trk == 0 && l2_entry.unicast.slp == PORT_DONT_CARE_9310)
            pEntry->unicast.isAged    = TRUE;
        else
            pEntry->unicast.isAged    = FALSE;
        if (l2_entry.unicast.tagSts)
            pEntry->unicast.flags     |= RTK_L2_UCAST_FLAG_TAG_STS;
        pEntry->unicast.agg_pri = l2_entry.unicast.agg_pri;
        pEntry->unicast.agg_vid = l2_entry.unicast.agg_vid;
        pEntry->unicast.ecid    = l2_entry.unicast.ecid;
        if (l2_entry.unicast.l2_tnl_if)
            pEntry->unicast.flags |= RTK_L2_UCAST_FLAG_L2_TUNNEL;
        pEntry->unicast.l2_tunnel_idx = l2_entry.unicast.l2_tnl_idx;
        pEntry->unicast.age     = l2_entry.unicast.aging;
        pEntry->unicast.l2_idx  = *pScan_idx;
    }
    else
    {
        osal_memcpy(&pEntry->l2mcast.mac, &l2_entry.l2mcast.mac, sizeof(rtk_mac_t));
        pEntry->l2mcast.rvid        = l2_entry.l2mcast.fid;
        pEntry->l2mcast.fwdIndex    = l2_entry.l2mcast.index;
        pEntry->l2mcast.nextHop     = l2_entry.l2mcast.nh;
        pEntry->l2mcast.l2_idx      = *pScan_idx;
        pEntry->l2mcast.groupId     = l2Db[unit].pEntry[(pEntry->l2mcast.l2_idx)].groupId;

        L2_SEM_LOCK(unit);
        if ((ret = _dal_mango_l2_mcastFwdPortmaskEntry_get(unit, pEntry->l2mcast.fwdIndex, &pEntry->l2mcast.portmask)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
        L2_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_portCtrl_get
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
dal_mango_l2_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 *pArg)
{
    int32   ret = RT_ERR_OK;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d,port=%d,type=%d,pArg=%p", unit, port, type, pArg);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_L2_PCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L2_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_L2_PCT_SA_ACT_REF:
            L2_REG_ARRAY_FIELD_READ_ERR_HDL(unit, \
                MANGO_L2_SA_ACT_REFr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_SRCf, val, "", errExit, ret);
            *pArg = (val)? RTK_L2_SA_REF_VLAN_PROFILE : RTK_L2_SA_REF_PORT;
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
 *      dal_mango_l2_portCtrl_set
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
dal_mango_l2_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 arg)
{
    int32   ret = RT_ERR_OK;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d,port=%d,type=%d,arg=0x%08x", unit, port, type, arg);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= RTK_L2_PCT_END), RT_ERR_OUT_OF_RANGE);

    /* function body */
    L2_SEM_LOCK(unit);

    switch (type)
    {
        case RTK_L2_PCT_SA_ACT_REF:
            val = (((rtk_enable_t)arg) != DISABLED)? 1 : 0;
            L2_REG_ARRAY_FIELD_WRITE_ERR_HDL(unit, \
                MANGO_L2_SA_ACT_REFr, port, REG_ARRAY_INDEX_NONE, \
                MANGO_SRCf, val, "", errExit, ret);
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
 *      dal_mango_l2_status_get
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
dal_mango_l2_status_get(uint32 unit, rtk_l2_stsType_t type, uint32 index, uint32 *pArg)
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
                MANGO_L2_HASH_FULL_CNTr, MANGO_CNTf, val, "", errExit, ret);
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
 *      dal_mango_l2_stkLearningEnable_get
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
dal_mango_l2_stkLearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_STK_AUTO_LRNf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_stkLearningEnable_set
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When it is enabled, switch will learn the MAC and source ingress port of packet which received from stacking port.
 */
int32
dal_mango_l2_stkLearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, enable=%d", unit, enable);

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
    if ((ret = reg_field_write(unit, MANGO_L2_CTRLr, MANGO_STK_AUTO_LRNf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_stkKeepUcastEntryValid_get
 * Description:
 *      Get the setting about entry validity after aging period.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_l2_stkKeepUcastEntryValid_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  enable = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(l2_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    L2_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_L2_AGE_CTRLr, MANGO_KEEP_AGE_OUT_ENTRY_VALIDf, &enable)) != RT_ERR_OK)
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
 *      dal_mango_l2_stkKeepUcastEntryValid_set
 * Description:
 *      Set the entry validity after aging period.
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When it is enabled, entry will keep valid after its aging period.
 */
int32
dal_mango_l2_stkKeepUcastEntryValid_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "unit=%d, enable=%d", unit, enable);

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
    if ((ret = reg_field_write(unit, MANGO_L2_AGE_CTRLr, MANGO_KEEP_AGE_OUT_ENTRY_VALIDf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_entryCnt_get
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
dal_mango_l2_entryCnt_get(uint32 unit, rtk_l2_entryType_t type, uint32 *pCnt)
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
    if((ret=reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        L2_SEM_UNLOCK(unit);
        return ret;
    }
    l2CamEbl = value==1 ? ENABLED : DISABLED;
    L2_SEM_UNLOCK(unit);

    value = 0;
    for(index = 0; index < hashTable_size[unit] + ((l2CamEbl==ENABLED && ld_patch_en == FALSE) ? cam_size[unit] : 0); index++)
    {
        osal_memset(&l2_entry, 0, sizeof(rtk_l2_entry_t));
        ret = dal_mango_l2_addrEntry_get(unit, index, &l2_entry);
        if((ret != RT_ERR_OK) && (ret != RT_ERR_L2_INVALID_FLOWTYPE))
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        switch(type)
        {
            case L2_ENTRY_ALL:
                if(l2_entry.valid && l2_entry.entry_type != FLOW_TYPE_END)
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
 *      dal_mango_l2_hashIdx_get
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
dal_mango_l2_hashIdx_get(uint32 unit, rtk_l2_macHashIdx_t *pMacHashIdx)
{
    uint64  hashSeed = 0;
    uint32  hash0, hash1, hash2, hash3, hash4;
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
    hash0 = (hashSeed) & BITMASK_12B;
    hash1 = (hashSeed >> 12) & BITMASK_12B;
    hash2 = (hashSeed >> 24) & BITMASK_12B;
    hash3 = (hashSeed >> 36) & BITMASK_12B;
    hash4 = (hashSeed >> 48) & BITMASK_12B;
    hash4 = ((hash4 & BITMASK_3B) << 9) | ((hash4 >> 3) & BITMASK_9B);
    hash0_idx = (uint32)(hash4 ^ hash3 ^ hash2 ^ hash1 ^ hash0);
    if (gRTL9311E)
        hash0_idx &= 0x7ff;


    /* Algo 1 */
    hash0 = (hashSeed) & BITMASK_12B;
    hash0 = ((hash0 & BITMASK_9B) << 3) | ((hash0 >> 9) & BITMASK_3B);
    hash1 = (hashSeed >> 12) & BITMASK_12B;
    hash1 = ((hash1 & BITMASK_6B) << 6) | ((hash1 >> 6) & BITMASK_6B);
    hash2 = (hashSeed >> 24) & BITMASK_12B;
    hash3 = (hashSeed >> 36) & BITMASK_12B;
    hash3 = ((hash3 & BITMASK_6B) << 6) | ((hash3 >> 6) & BITMASK_6B);
    hash4 = (hashSeed >> 48) & BITMASK_12B;
    hash1_idx = (uint32)(hash4 ^ hash3 ^ hash2 ^ hash1 ^ hash0);
    if (gRTL9311E)
        hash1_idx &= 0x7ff;

    pMacHashIdx->idx[0][0] = hash0_idx;
    pMacHashIdx->idx[1][0] = hash0_idx + ((hashTable_size[unit]/2)>>2);
    pMacHashIdx->idx[0][1] = hash1_idx;
    pMacHashIdx->idx[1][1] = hash1_idx + ((hashTable_size[unit]/2)>>2);
    pMacHashIdx->validBlk = 2;
    pMacHashIdx->validAlgo = 2;

    return RT_ERR_OK;
}

static int32
_dal_mango_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac, uint32 sram_if)
{
    int32 ret;

    uint32 value;
    uint32 is_mc;
    uint32 findIdx = 0;
    uint32 tbl_idx;
    uint32 tbl_size;
    uint32 scanType;
    uint32 is_found;
    uint32 next_hop;
    uint32 tbl_type, find_tbl;
    uint32 fwdTableIdx;


    uint32 scan_idx = 0;
    uint32 is_static = 0;
    uint32 mac_uint32[2];
    uint32 l2_tnl   = 0;
    uint32 find_tbl_name[2] = {TBL_L2_SRAM_FIND_NEXT, TBL_L2_BCAM_FIND_NEXT};
    uint32 tbl_name[4] = {MANGO_L2_UCt, MANGO_L2_MCt, MANGO_L2_CAM_UCt, MANGO_L2_CAM_MCt};
    uint32 tbl_nh_field[4] = {MANGO_L2_UC_NEXT_HOPtf, MANGO_L2_MC_NEXT_HOPtf, MANGO_L2_CAM_UC_NEXT_HOPtf, MANGO_L2_CAM_MC_NEXT_HOPtf};
    uint32 tbl_mac_field[4] = {MANGO_L2_UC_MACtf, MANGO_L2_MC_MACtf, MANGO_L2_CAM_UC_MACtf, MANGO_L2_CAM_MC_MACtf};
    uint32 tbl_l2tnl_field[4] = {MANGO_L2_UC_L2_TNLtf, MANGO_L2_MC_REMOTE_FWDtf, MANGO_L2_CAM_UC_L2_TNLtf, MANGO_L2_CAM_MC_REMOTE_FWDtf};

    rtk_fid_t fid;
    rtk_mac_t l2_mac;
    l2_entry_t  l2_entry;
    uint32                  *ptr;

    is_mc    = (pMac->octet[0] & BITMASK_1B) ? 1 : 0;
    scanType = (is_mc == 1) ? 5 : 1;
    tbl_size = (sram_if == 0) ? ((int32)cam_size[unit]) : ((int32)hashTable_size[unit]);
    tbl_type = ((!sram_if) << 1) | (is_mc);
    find_tbl = !sram_if;


    while(scan_idx < tbl_size)
    {
        L2_SEM_LOCK(unit);

        if ((ret = reg_field_write(unit, MANGO_TBL_ACCESS_L2_METHOD_CTRLr, MANGO_METHODf, &scanType)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
            return ret;
        }

        if ((ret = table_read(unit, find_tbl_name[find_tbl], HASH_IDX_XLATE_P(scan_idx >> 2) << 2 | (scan_idx & 0x3), (uint32 *)&l2_entry)) != RT_ERR_OK)
        {
            L2_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }

        ptr = (uint32 *)&l2_entry;

        is_found = ptr[3] >> 15;
        if (is_found)
            findIdx = ptr[3] & 0x7fff;

        L2_SEM_UNLOCK(unit);

        if (is_found == 0)
        {
            goto exit;
        }


        tbl_idx = findIdx;

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
        if ((ret = table_field_get(unit,  tbl_name[tbl_type], tbl_l2tnl_field[tbl_type], &l2_tnl, (uint32 *)&l2_entry)) != RT_ERR_OK)
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
            if ((ret = table_field_get(unit, tbl_name[tbl_type], ((sram_if == 1)? MANGO_L2_UC_STATICtf : MANGO_L2_CAM_UC_STATICtf), &is_static, (uint32 *)&l2_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return ret;
            }
        }

        if (!osal_memcmp(pMac, &l2_mac, sizeof(rtk_mac_t)) && (l2_tnl == 0) && ((0 == next_hop) || (0 == is_mc)) && ((0 == is_static) || (1 == include_static)))
        {
            if (1 == next_hop)
            {
                if ((ret = table_field_get(unit, tbl_name[tbl_type], ((sram_if == 1) ? MANGO_L2_UC_FIDtf : MANGO_L2_CAM_UC_FIDtf), &fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                osal_memset(&l2_entry, 0, sizeof(l2_entry_t));
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1) ? MANGO_L2_UC_MACtf : MANGO_L2_CAM_UC_MACtf), &mac_uint32[0], (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1) ? MANGO_L2_UC_FIDtf : MANGO_L2_CAM_UC_FIDtf), &fid, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                value = PORT_DONT_CARE_9310;
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1) ? MANGO_L2_UC_SPAtf : MANGO_L2_CAM_UC_SPAtf), &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                value = 1;
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1) ? MANGO_L2_UC_NEXT_HOPtf : MANGO_L2_CAM_UC_NEXT_HOPtf), &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
                if ((ret = table_field_set(unit, tbl_name[tbl_type], ((sram_if == 1) ? MANGO_L2_UC_VALIDtf : MANGO_L2_CAM_UC_VALIDtf), &value, (uint32 *)&l2_entry)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return ret;
                }
            }
            else
            {
                if(is_mc)
                {
                    if ((ret = table_field_get(unit, tbl_name[tbl_type],  ((sram_if == 1) ? MANGO_L2_MC_MC_PMSK_IDXtf : MANGO_L2_CAM_MC_MC_PMSK_IDXtf), &fwdTableIdx, (uint32 *)&l2_entry)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                        return ret;
                    }
                    L2_SEM_LOCK(unit);
                    _dal_mango_l2_mcastFwdIndex_free(unit, fwdTableIdx);
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

exit:
    /* reset METHOD bit to normal mode */
    value = 0;
    if ((ret = reg_field_write(unit, MANGO_TBL_ACCESS_L2_METHOD_CTRLr, MANGO_METHODf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "err ret = %d", ret);
        return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_l2_addr_delByMac
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
dal_mango_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac)
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
    if ((ret = reg_field_read(unit, MANGO_L2_CTRLr, MANGO_LUTCAM_ENf, &value)) != RT_ERR_OK)
    {
        L2_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }
    L2_SEM_UNLOCK(unit);

    l2CamEbl = (value == 1) ? ENABLED : DISABLED;

    if((ret = _dal_mango_l2_addr_delByMac(unit, include_static, pMac, sram_if)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return ret;
    }

    if(ENABLED == l2CamEbl && ld_patch_en == FALSE)
    {
        sram_if = 0;
        if((ret = _dal_mango_l2_addr_delByMac(unit, include_static, pMac, sram_if)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}


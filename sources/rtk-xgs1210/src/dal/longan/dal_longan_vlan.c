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
 * $Revision: 98162 $
 * $Date: 2019-06-25 11:39:47 +0800 (Tue, 25 Jun 2019) $
 *
 * Purpose : Definition those public vlan APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) vlan
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
#include <rtk/default.h>
#include <rtk/vlan.h>

/*
 * Symbol Definition
 */
#define RTK_VLAN_GROUP_NUM   8
#define LONGAN_VLAN_DBG      (0)

typedef enum dal_longan_vlan_action_type_e
{
    LONGAN_VLAN_ACTION_BGD = 0,
    LONGAN_VLAN_ACTION_RSVD_MC,
    LONGAN_VLAN_ACTION_URPF_FAIL,
    LONGAN_VLAN_ACTION_END
}dal_longan_vlan_action_type_t;

/*
 * Data Declaration
 */

/* vlan information structure */
typedef struct dal_longan_vlan_info_s
{
    uint32          count;           /* count of valid vlan number    */
    rtk_bitmap_t    *pValid_lists;   /* valid bit for this table      */
    uint16          *pVid2tblindex;  /* table index of vid, 0:invalid */

    /* hardware resource (for internal APIs) */
    struct
    {
        /* mac-based Table */
        struct
        {
            uint32  startIndex;     /* recored mac-based Table start index */
            uint32  size;           /* mac-based Table size */
            uint32  used_count;     /* used entry number by mac-based Table entry */
            uint16  entries_after_pfLen[LONGAN_VLAN_MAC_PFLEN];    /* total entries after the specific pfLen */
        } macTable;

        /* ipSubnet-based Table */
        struct
        {
            uint32  startIndex;     /* recored ipSubnet-based Table start index */
            uint32  size;           /* ipSubnet-based Table size */
            uint32  used_count;     /* used entry number by ipSubnet-based Table entry */
            uint16  entries_after_pfLen[LONGAN_VLAN_IP_PFLEN];    /* total entries after the specific pfLen */
        } ipSubnetTable;
    } HW;
} dal_longan_vlan_info_t;

/* vlan entry*/
typedef struct dal_longan_vlan_data_s
{
    rtk_vlan_t  vid;
    rtk_stg_t   msti;                   /*multiple spanning tree instance*/
    rtk_vlan_l2LookupMode_t ucast_mode;/*L2 lookup mode for unicast traffic*/
    rtk_vlan_l2LookupMode_t mcast_mode;/*L2 lookup mode for L2/IP mulicast traffic*/
    uint32      profile_idx;            /*VLAN profile index*/
    rtk_portmask_t  member_portmask;
    rtk_portmask_t  untag_portmask;
    rtk_vlan_groupMask_t groupMask;
} dal_longan_vlan_data_t;

typedef struct dal_longan_vlan_shadow_s
{
    rtk_vlan_t  vid;
    rtk_fid_t   msti;                   /*filtering db and multiple spanning tree instance*/
    rtk_l2_ucastLookupMode_t ucast_mode;/*L2 lookup mode for unicast traffic*/
    rtk_l2_ucastLookupMode_t mcast_mode;/*L2 lookup mode for L2/IP mulicast traffic*/
    uint32      profile_idx;            /*VLAN profile index*/
    rtk_portmask_t  member_portmask;
    rtk_vlan_groupMask_t groupMask;
} dal_longan_vlan_shadow_t;

typedef struct dal_longan_vlan_shadowUntag_s
{
    rtk_portmask_t  untag_portmask;
} dal_longan_vlan_shadowUntag_t;

/*
 * Data Declaration
 */
static uint32               vlan_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         vlan_sem[RTK_MAX_NUM_OF_UNIT];
static dal_longan_vlan_info_t  *pDal_longan_vlan_info[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
#define LONGAN_VLAN_DBG_PRINTF(_level, fmt, ...)                                             \
do {                                                                                        \
    if (LONGAN_VLAN_DBG >= (_level))                                                         \
        osal_printf("%s():L%d: " fmt, __FUNCTION__,  __LINE__, ##__VA_ARGS__);   \
} while (0)

/* vlan semaphore handling */
#define VLAN_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(vlan_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_VLAN|MOD_DAL), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define VLAN_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(vlan_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_VLAN|MOD_DAL), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


#define VLANINFO_VALID_IS_SET(unit, vid)    BITMAP_IS_SET(pDal_longan_vlan_info[unit]->pValid_lists, vid)
#define VLANINFO_VALID_IS_CLEAR(unit, vid)  BITMAP_IS_CLEAR(pDal_longan_vlan_info[unit]->pValid_lists, vid)
#define VLANINFO_VALID_SET(unit, vid)       BITMAP_SET(pDal_longan_vlan_info[unit]->pValid_lists, vid)
#define VLANINFO_VALID_CLEAR(unit, vid)     BITMAP_CLEAR(pDal_longan_vlan_info[unit]->pValid_lists, vid)

#define VLANDB_IPSUBNET_TBL_SIZE(unit)          (pDal_longan_vlan_info[unit]->HW.ipSubnetTable.size)
#define VLANDB_IPSUBNET_TBL_IDX_MIN(unit)       (pDal_longan_vlan_info[unit]->HW.ipSubnetTable.startIndex)
#define VLANDB_IPSUBNET_TBL_IDX_MAX(unit)       VLANDB_IPSUBNET_TBL_IDX_MIN(unit) + VLANDB_IPSUBNET_TBL_SIZE(unit)
#define VLANDB_IPSUBNET_TBL_BOTTOM(unit)        VLANDB_IPSUBNET_TBL_IDX_MIN(unit) + VLANDB_IPSUBNET_TBL_USED(unit)
#define VLANDB_IPSUBNET_TBL_USED(unit)          (pDal_longan_vlan_info[unit]->HW.ipSubnetTable.used_count)
#define VLANDB_IPSUBNET_PFLEN_CNT(unit, _pfl)   (pDal_longan_vlan_info[unit]->HW.ipSubnetTable.entries_after_pfLen[(_pfl)])

#define VLANDB_MAC_TBL_SIZE(unit)               (pDal_longan_vlan_info[unit]->HW.macTable.size)
#define VLANDB_MAC_TBL_IDX_MIN(unit)            (pDal_longan_vlan_info[unit]->HW.macTable.startIndex)
#define VLANDB_MAC_TBL_IDX_MAX(unit)            VLANDB_MAC_TBL_IDX_MIN(unit) + VLANDB_MAC_TBL_SIZE(unit)
#define VLANDB_MAC_TBL_BOTTOM(unit)             VLANDB_MAC_TBL_IDX_MIN(unit) + VLANDB_MAC_TBL_USED(unit)
#define VLANDB_MAC_TBL_USED(unit)               (pDal_longan_vlan_info[unit]->HW.macTable.used_count)
#define VLANDB_MAC_PFLEN_CNT(unit, _pfl)        (pDal_longan_vlan_info[unit]->HW.macTable.entries_after_pfLen[(_pfl)])

/*
 * Function Declaration
 */
static int32 _dal_longan_setVlan(uint32 unit, dal_longan_vlan_data_t *pVlan_entry);
static int32 _dal_longan_getVlan(uint32 unit, dal_longan_vlan_data_t *pVlan_entry);
static int32 _dal_longan_vlan_init_config(uint32 unit);
static int32 _dal_longan_vlan_actionXlateRtk(uint32 value,dal_longan_vlan_action_type_t type,rtk_action_t *pAction);
static int32 _dal_longan_vlan_actionXlateDal(rtk_action_t action, dal_longan_vlan_action_type_t type,uint32 *pValue);

/* Module Name : vlan */

/* Function Name:
 *      dal_longan_vlanMapper_init
 * Description:
 *      Hook vlan module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook vlan module before calling any vlan APIs.
 */
int32
dal_longan_vlanMapper_init(dal_mapper_t *pMapper)
{
    pMapper->vlan_init = dal_longan_vlan_init;
    pMapper->vlan_create = dal_longan_vlan_create;
    pMapper->vlan_destroy = dal_longan_vlan_destroy;
    pMapper->vlan_destroyAll = dal_longan_vlan_destroyAll;
    pMapper->vlan_port_add = dal_longan_vlan_port_add;
    pMapper->vlan_port_del = dal_longan_vlan_port_del;
    pMapper->vlan_port_get = dal_longan_vlan_port_get;
    pMapper->vlan_port_set = dal_longan_vlan_port_set;
    pMapper->vlan_stg_get = dal_longan_vlan_stg_get;
    pMapper->vlan_stg_set = dal_longan_vlan_stg_set;
    pMapper->vlan_l2LookupSvlFid_get = dal_longan_vlan_l2LookupSvlFid_get;
    pMapper->vlan_l2LookupSvlFid_set = dal_longan_vlan_l2LookupSvlFid_set;
    pMapper->vlan_l2LookupMode_get = dal_longan_vlan_l2LookupMode_get;
    pMapper->vlan_l2LookupMode_set = dal_longan_vlan_l2LookupMode_set;
    pMapper->vlan_groupMask_get = dal_longan_vlan_groupMask_get;
    pMapper->vlan_groupMask_set = dal_longan_vlan_groupMask_set;
    pMapper->vlan_profileIdx_get = dal_longan_vlan_profileIdx_get;
    pMapper->vlan_profileIdx_set = dal_longan_vlan_profileIdx_set;
    pMapper->vlan_profile_get = dal_longan_vlan_profile_get;
    pMapper->vlan_profile_set = dal_longan_vlan_profile_set;
    pMapper->vlan_portFwdVlan_get = dal_longan_vlan_portFwdVlan_get;
    pMapper->vlan_portFwdVlan_set = dal_longan_vlan_portFwdVlan_set;
    pMapper->vlan_portAcceptFrameType_get = dal_longan_vlan_portAcceptFrameType_get;
    pMapper->vlan_portAcceptFrameType_set = dal_longan_vlan_portAcceptFrameType_set;
    pMapper->vlan_portIgrFilter_get = dal_longan_vlan_portIgrFilter_get;
    pMapper->vlan_portIgrFilter_set = dal_longan_vlan_portIgrFilter_set;
    pMapper->vlan_portEgrFilterEnable_get = dal_longan_vlan_portEgrFilterEnable_get;
    pMapper->vlan_portEgrFilterEnable_set = dal_longan_vlan_portEgrFilterEnable_set;
    pMapper->vlan_mcastLeakyEnable_get = dal_longan_vlan_mcastLeakyEnable_get;
    pMapper->vlan_mcastLeakyEnable_set = dal_longan_vlan_mcastLeakyEnable_set;
    pMapper->vlan_portPvidMode_get = dal_longan_vlan_portPvidMode_get;
    pMapper->vlan_portPvidMode_set = dal_longan_vlan_portPvidMode_set;
    pMapper->vlan_portPvid_get = dal_longan_vlan_portPvid_get;
    pMapper->vlan_portPvid_set = dal_longan_vlan_portPvid_set;
    pMapper->vlan_protoGroup_get = dal_longan_vlan_protoGroup_get;
    pMapper->vlan_protoGroup_set = dal_longan_vlan_protoGroup_set;
    pMapper->vlan_portProtoVlan_get = dal_longan_vlan_portProtoVlan_get;
    pMapper->vlan_portProtoVlan_set = dal_longan_vlan_portProtoVlan_set;
    pMapper->vlan_portMacBasedVlanEnable_get = dal_longan_vlan_portMacBasedVlanEnable_get;
    pMapper->vlan_portMacBasedVlanEnable_set = dal_longan_vlan_portMacBasedVlanEnable_set;
    pMapper->vlan_macBasedVlanEntry_get = dal_longan_vlan_macBasedVlanEntry_get;
    pMapper->vlan_macBasedVlanEntry_set = dal_longan_vlan_macBasedVlanEntry_set;
    pMapper->vlan_macBasedVlanEntry_add = dal_longan_vlan_macBasedVlanEntry_add;
    pMapper->vlan_macBasedVlanEntry_del = dal_longan_vlan_macBasedVlanEntry_del;
    pMapper->vlan_portIpSubnetBasedVlanEnable_get = dal_longan_vlan_portIpSubnetBasedVlanEnable_get;
    pMapper->vlan_portIpSubnetBasedVlanEnable_set = dal_longan_vlan_portIpSubnetBasedVlanEnable_set;
    pMapper->vlan_ipSubnetBasedVlanEntry_get = dal_longan_vlan_ipSubnetBasedVlanEntry_get;
    pMapper->vlan_ipSubnetBasedVlanEntry_set = dal_longan_vlan_ipSubnetBasedVlanEntry_set;
    pMapper->vlan_ipSubnetBasedVlanEntry_add = dal_longan_vlan_ipSubnetBasedVlanEntry_add;
    pMapper->vlan_ipSubnetBasedVlanEntry_del = dal_longan_vlan_ipSubnetBasedVlanEntry_del;
    pMapper->vlan_tpidEntry_get = dal_longan_vlan_tpidEntry_get;
    pMapper->vlan_tpidEntry_set = dal_longan_vlan_tpidEntry_set;
    pMapper->vlan_portIgrTpid_get = dal_longan_vlan_portIgrTpid_get;
    pMapper->vlan_portIgrTpid_set = dal_longan_vlan_portIgrTpid_set;
    pMapper->vlan_portEgrTpid_get = dal_longan_vlan_portEgrTpid_get;
    pMapper->vlan_portEgrTpid_set = dal_longan_vlan_portEgrTpid_set;
    pMapper->vlan_portEgrTpidSrc_get = dal_longan_vlan_portEgrTpidSrc_get;
    pMapper->vlan_portEgrTpidSrc_set = dal_longan_vlan_portEgrTpidSrc_set;
    pMapper->vlan_portIgrExtraTagEnable_get = dal_longan_vlan_portIgrExtraTagEnable_get;
    pMapper->vlan_portIgrExtraTagEnable_set = dal_longan_vlan_portIgrExtraTagEnable_set;
    pMapper->vlan_portEgrTagSts_get = dal_longan_vlan_portEgrTagSts_get;
    pMapper->vlan_portEgrTagSts_set = dal_longan_vlan_portEgrTagSts_set;
    pMapper->vlan_portIgrVlanTransparentEnable_get = dal_longan_vlan_portIgrVlanTransparentEnable_get;
    pMapper->vlan_portIgrVlanTransparentEnable_set = dal_longan_vlan_portIgrVlanTransparentEnable_set;
    pMapper->vlan_portEgrVlanTransparentEnable_get = dal_longan_vlan_portEgrVlanTransparentEnable_get;
    pMapper->vlan_portEgrVlanTransparentEnable_set = dal_longan_vlan_portEgrVlanTransparentEnable_set;
    pMapper->vlan_igrVlanCnvtBlkMode_get = dal_longan_vlan_igrVlanCnvtBlkMode_get;
    pMapper->vlan_igrVlanCnvtBlkMode_set = dal_longan_vlan_igrVlanCnvtBlkMode_set;
    pMapper->vlan_igrVlanCnvtEntry_get = dal_longan_vlan_igrVlanCnvtEntry_get;
    pMapper->vlan_igrVlanCnvtEntry_set = dal_longan_vlan_igrVlanCnvtEntry_set;
    pMapper->vlan_portIgrVlanCnvtEnable_get = dal_longan_vlan_portIgrVlanCnvtEnable_get;
    pMapper->vlan_portIgrVlanCnvtEnable_set = dal_longan_vlan_portIgrVlanCnvtEnable_set;
    pMapper->vlan_egrVlanCnvtEntry_get = dal_longan_vlan_egrVlanCnvtEntry_get;
    pMapper->vlan_egrVlanCnvtEntry_set = dal_longan_vlan_egrVlanCnvtEntry_set;
    pMapper->vlan_portEgrVlanCnvtEnable_get = dal_longan_vlan_portEgrVlanCnvtEnable_get;
    pMapper->vlan_portEgrVlanCnvtEnable_set = dal_longan_vlan_portEgrVlanCnvtEnable_set;
    pMapper->vlan_portVlanAggrEnable_get = dal_longan_vlan_portVlanAggrEnable_get;
    pMapper->vlan_portVlanAggrEnable_set = dal_longan_vlan_portVlanAggrEnable_set;
    pMapper->vlan_portVlanAggrCtrl_get = dal_longan_vlan_portVlanAggrCtrl_get;
    pMapper->vlan_portVlanAggrCtrl_set = dal_longan_vlan_portVlanAggrCtrl_set;
    pMapper->vlan_leakyStpFilter_get = dal_longan_vlan_leakyStpFilter_get;
    pMapper->vlan_leakyStpFilter_set = dal_longan_vlan_leakyStpFilter_set;
    pMapper->vlan_portIgrVlanCnvtLuMisAct_get = dal_longan_vlan_portIgrVlanCnvtLuMisAct_get;
    pMapper->vlan_portIgrVlanCnvtLuMisAct_set = dal_longan_vlan_portIgrVlanCnvtLuMisAct_set;
    pMapper->vlan_portEgrVlanCnvtLuMisAct_get = dal_longan_vlan_portEgrVlanCnvtLuMisAct_get;
    pMapper->vlan_portEgrVlanCnvtLuMisAct_set = dal_longan_vlan_portEgrVlanCnvtLuMisAct_set;
    pMapper->vlan_igrVlanCnvtEntry_delAll = dal_longan_vlan_igrVlanCnvtEntry_delAll;
    pMapper->vlan_egrVlanCnvtEntry_delAll = dal_longan_vlan_egrVlanCnvtEntry_delAll;
    pMapper->vlan_igrVlanCnvtHitIndication_get = dal_longan_vlan_igrVlanCnvtHitIndication_get;
    pMapper->vlan_egrVlanCnvtHitIndication_get = dal_longan_vlan_egrVlanCnvtHitIndication_get;
    pMapper->vlan_portIgrVlanCnvtRangeCheckSet_get = dal_longan_vlan_portIgrVlanCnvtRangeCheckSet_get;
    pMapper->vlan_portIgrVlanCnvtRangeCheckSet_set = dal_longan_vlan_portIgrVlanCnvtRangeCheckSet_set;
    pMapper->vlan_igrVlanCnvtRangeCheckEntry_get = dal_longan_vlan_igrVlanCnvtRangeCheckEntry_get;
    pMapper->vlan_igrVlanCnvtRangeCheckEntry_set = dal_longan_vlan_igrVlanCnvtRangeCheckEntry_set;
    pMapper->vlan_portEgrVlanCnvtRangeCheckSet_get = dal_longan_vlan_portEgrVlanCnvtRangeCheckSet_get;
    pMapper->vlan_portEgrVlanCnvtRangeCheckSet_set = dal_longan_vlan_portEgrVlanCnvtRangeCheckSet_set;
    pMapper->vlan_egrVlanCnvtRangeCheckEntry_get = dal_longan_vlan_egrVlanCnvtRangeCheckEntry_get;
    pMapper->vlan_egrVlanCnvtRangeCheckEntry_set = dal_longan_vlan_egrVlanCnvtRangeCheckEntry_set;
    pMapper->vlan_trkVlanAggrEnable_get = dal_longan_vlan_trkVlanAggrEnable_get;
    pMapper->vlan_trkVlanAggrEnable_set = dal_longan_vlan_trkVlanAggrEnable_set;
    pMapper->vlan_trkVlanAggrCtrl_get = dal_longan_vlan_trkVlanAggrCtrl_get;
    pMapper->vlan_trkVlanAggrCtrl_set = dal_longan_vlan_trkVlanAggrCtrl_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_init
 * Description:
 *      Initialize vlan module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 */
int32
dal_longan_vlan_init(uint32 unit)
{
    int32       ret;
    uint32      vlan_tableSize;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    RT_INIT_REENTRY_CHK(vlan_init[unit]);
    vlan_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    vlan_sem[unit] = osal_sem_mutex_create();
    if (0 == vlan_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_VLAN|MOD_DAL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    vlan_tableSize = 0;
    if ((ret = table_size_get(unit, LONGAN_VLANt, &vlan_tableSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "table size get failed");
        return ret;
    }

    VLAN_SEM_LOCK(unit);

    /* allocate memory for each database and initilize database */
    pDal_longan_vlan_info[unit] = (dal_longan_vlan_info_t *)osal_alloc(sizeof(dal_longan_vlan_info_t));
    if (0 == pDal_longan_vlan_info[unit])
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_FAILED, (MOD_VLAN|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pDal_longan_vlan_info[unit], 0, sizeof(dal_longan_vlan_info_t));

    pDal_longan_vlan_info[unit]->pValid_lists = (rtk_bitmap_t *)osal_alloc(BITMAP_ARRAY_SIZE(vlan_tableSize));
    if (0 == pDal_longan_vlan_info[unit]->pValid_lists)
    {
        osal_free(pDal_longan_vlan_info[unit]);
        pDal_longan_vlan_info[unit] = 0;
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_FAILED, (MOD_VLAN|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pDal_longan_vlan_info[unit]->pValid_lists, 0, BITMAP_ARRAY_SIZE(vlan_tableSize));

    pDal_longan_vlan_info[unit]->pVid2tblindex = (uint16 *)osal_alloc(vlan_tableSize * sizeof(uint16));
    if (0 == pDal_longan_vlan_info[unit]->pVid2tblindex)
    {
        osal_free(pDal_longan_vlan_info[unit]->pValid_lists);
        pDal_longan_vlan_info[unit]->pValid_lists = 0;
        osal_free(pDal_longan_vlan_info[unit]);
        pDal_longan_vlan_info[unit] = 0;
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_FAILED, (MOD_VLAN|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pDal_longan_vlan_info[unit]->pVid2tblindex, 0, (vlan_tableSize * sizeof(uint16)));
    VLAN_SEM_UNLOCK(unit);

    /* set init flag to complete init */
    vlan_init[unit] = INIT_COMPLETED;

    if (( ret = _dal_longan_vlan_init_config(unit)) != RT_ERR_OK)
    {
        vlan_init[unit] = INIT_NOT_COMPLETED;
        osal_free(pDal_longan_vlan_info[unit]->pVid2tblindex);
        pDal_longan_vlan_info[unit]->pVid2tblindex = 0;
        osal_free(pDal_longan_vlan_info[unit]->pValid_lists);
        pDal_longan_vlan_info[unit]->pValid_lists = 0;
        osal_free(pDal_longan_vlan_info[unit]);
        pDal_longan_vlan_info[unit] = 0;

        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "Init default vlan config failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_longan_vlan_init */

/* Function Name:
 *      _dal_longan_vlan_table_check
 * Description:
 *      Check specific vid vlan table exist or not
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 * Output:
 *      Nonte
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - vlan entry not found
 * Note:
 *
 */
extern int32
_dal_longan_vlan_table_check(uint32 unit, rtk_vlan_t vid)
{
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);
    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_vlan_igrVlanCnvtBlkMode_get
 * Description:
 *      Get the operation mode of ingress VLAN conversion table block.
 * Input:
 *      unit    - unit id
 *      blk_idx - block index
 * Output:
 *      pMode   - operation mode of ingress VLAN conversion block
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. Valid block index ranges from 0 to 7.
 *      2. Ingress VLAN conversion table block can be used for doing ingress VLAN conversion
 *         or MAC-basd VLAN or IP-Subnet-based VLAN.
 */
int32
_dal_longan_vlan_igrVlanCnvtBlkMode_get(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t *pMode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((blk_idx >= HAL_MAX_NUM_OF_C2SC_BLK(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pMode, RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_IVC_BLK_CTRLr, REG_ARRAY_INDEX_NONE, blk_idx, LONGAN_MODEf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pMode = CONVERSION_MODE_C2SC;
            break;
        case 1:
            *pMode = CONVERSION_MODE_MAC_BASED;
            break;
        case 2:
            *pMode = CONVERSION_MODE_IP_SUBNET_BASED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pMode=%d", *pMode);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_vlan_tblDb_update
 * Description:
 *      Update IP-subnet / MAC based vlan table database
 * Input:
 *      unit    - unit id
 *      blk_idx - block id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *
 */
int32
_dal_longan_vlan_tblDb_update(uint32 unit)
{
    int32   ret = RT_ERR_OK;
    uint32  ip_hit = FALSE, mac_hit = FALSE;
    uint32  blk_idx = 0;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode = CONVERSION_MODE_DISABLED;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    for (blk_idx = 0; blk_idx < HAL_MAX_NUM_OF_C2SC_BLK(unit); blk_idx++)
    {
        RT_ERR_HDL(_dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, blk_idx, &blk_mode), errHandle, ret);

        if (CONVERSION_MODE_IP_SUBNET_BASED == blk_mode)
        {
            if (FALSE == ip_hit)
            {
                ip_hit = TRUE;
                VLANDB_IPSUBNET_TBL_IDX_MIN(unit) = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit) * blk_idx;
                VLANDB_IPSUBNET_TBL_SIZE(unit) = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
            }
            else
                VLANDB_IPSUBNET_TBL_SIZE(unit) += HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
        }
        else if (CONVERSION_MODE_MAC_BASED == blk_mode)
        {
            if (FALSE == mac_hit)
            {
                mac_hit = TRUE;
                VLANDB_MAC_TBL_IDX_MIN(unit) = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit) * blk_idx;
                VLANDB_MAC_TBL_SIZE(unit) = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
            }
            else
                VLANDB_MAC_TBL_SIZE(unit) += HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
        }
        else
            continue;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, ip_start=%d[%d], mac_start=%d[%d]",
                unit, VLANDB_IPSUBNET_TBL_IDX_MIN(unit), VLANDB_IPSUBNET_TBL_SIZE(unit),
                        VLANDB_MAC_TBL_IDX_MIN(unit), VLANDB_MAC_TBL_SIZE(unit));

    LONGAN_VLAN_DBG_PRINTF(3, "unit=%d, ip_start=%d[%d], mac_start=%d[%d]\n",
                        unit, VLANDB_IPSUBNET_TBL_IDX_MIN(unit), VLANDB_IPSUBNET_TBL_SIZE(unit),
                              VLANDB_MAC_TBL_IDX_MIN(unit), VLANDB_MAC_TBL_SIZE(unit));

errHandle:
    return ret;
}

/* internal APIs, called by dal_longan_vlan_* APIs (should NOT lock/unlock semaphore inside) */
static inline int32 __dal_longan_vlan_ipSubEntry_move(uint32 unit, uint32 dstIdx, uint32 srcIdx)
{
    int32 ret = RT_ERR_OK;
    rtk_vlan_ipSubnetVlanEntry_t ipSubnetVlan_entry;

    osal_memset(&ipSubnetVlan_entry, 0, sizeof(ipSubnetVlan_entry));

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,dst=%d,src=%d", unit, dstIdx, srcIdx);

    LONGAN_VLAN_DBG_PRINTF(3, "unit=%d,dst=%d,src=%d\n", unit, dstIdx, srcIdx);

    /* parameter check */
    RT_PARAM_CHK((dstIdx == srcIdx), RT_ERR_INPUT);
    RT_PARAM_CHK(((dstIdx) < VLANDB_IPSUBNET_TBL_IDX_MIN(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((srcIdx) < VLANDB_IPSUBNET_TBL_IDX_MIN(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((dstIdx) > VLANDB_IPSUBNET_TBL_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((srcIdx) > VLANDB_IPSUBNET_TBL_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    // 1. cp srcIdx / 2. write to dstIdx / 3. remove srcIdx
    RT_ERR_HDL(dal_longan_vlan_ipSubnetBasedVlanEntry_get(unit, srcIdx, &ipSubnetVlan_entry), errHandle, ret);
    RT_ERR_HDL(dal_longan_vlan_ipSubnetBasedVlanEntry_set(unit, dstIdx, &ipSubnetVlan_entry), errHandle, ret);
    osal_memset(&ipSubnetVlan_entry, 0, sizeof(ipSubnetVlan_entry));
    RT_ERR_HDL(dal_longan_vlan_ipSubnetBasedVlanEntry_set(unit, srcIdx, &ipSubnetVlan_entry), errHandle, ret);

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_vlan_ipSubEntry_alloc
 * Description:
 *      Output ip-subnet-based vlan table index
 * Input:
 *      unit    - unit id
 *      pfLen   - prefix lengh (from mask)
 * Output:
 *      pIndex  - entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_TBL_FULL       - table full
 * Note:
 *
 */
static int32
_dal_longan_vlan_ipSubEntry_alloc(uint32 unit, uint32 pfLen, uint32 *pIndex)
{
    int32   ret = RT_ERR_OK;
    uint32  dstIdx = 0, srcIdx = 0;
    int32   pfl = 0;
    int32   i = 0;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,pfLen=%d,pIndex=0x%08x", unit, pfLen, pIndex);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((pfLen > 32)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* ip-subnet-based vlan table entry allocation */
    if ((VLANDB_IPSUBNET_TBL_SIZE(unit) - VLANDB_IPSUBNET_TBL_USED(unit)) < 1)
    {
        return RT_ERR_TBL_FULL;
    }

    if (0 == pfLen)
    {
        /* use the bottom entry directly */
        *pIndex = VLANDB_IPSUBNET_TBL_BOTTOM(unit);
    } else {
        dstIdx = VLANDB_IPSUBNET_TBL_BOTTOM(unit);
        srcIdx = dstIdx;
        for (pfl = 0; pfl < pfLen; pfl++)
        {
            srcIdx = VLANDB_IPSUBNET_TBL_BOTTOM(unit) - VLANDB_IPSUBNET_PFLEN_CNT(unit, pfl);
            if (dstIdx != srcIdx)
            {
                /* move */
                LONGAN_VLAN_DBG_PRINTF(3, "ipSubMV0: pfl=%d, dstIdx=%d, srcIdx=%d; pfl=%d, pfLen=%d\n", pfl, dstIdx, srcIdx, pfl, pfLen);
                RT_ERR_HDL(__dal_longan_vlan_ipSubEntry_move(unit, dstIdx, srcIdx), errHandle, ret);

                dstIdx = srcIdx;    /* new dstIdx */
            }
        }

        *pIndex = srcIdx;
    }

    for (i = pfLen; i < LONGAN_VLAN_IP_PFLEN; i++)
    {
        VLANDB_IPSUBNET_PFLEN_CNT(unit, i) += 1;
    }

    VLANDB_IPSUBNET_TBL_USED(unit) += 1;

    LONGAN_VLAN_DBG_PRINTF(3, "pfLen = %d, *pIndex = %d\n", pfLen, *pIndex);

errHandle:

    return ret;
}

/* Function Name:
 *      _dal_longan_vlan_ipSubEntry_free
 * Description:
 *      Rearrange ip-subnet-based table
 * Input:
 *      unit    - unit id
 *      pfLen   - prefix lengh (from mask)
 *      index   - entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_TBL_FULL       - table full
 * Note:
 *
 */
static int32
_dal_longan_vlan_ipSubEntry_free(uint32 unit, uint32 pfLen, uint32 index)
{
    int32   ret = RT_ERR_OK;
    uint32  dstIdx = 0, srcIdx = 0, bottom = 0;
    int32   pfl = 0;
    int32   i = 0;
    rtk_vlan_ipSubnetVlanEntry_t ipSubnetVlan_entry;

    osal_memset(&ipSubnetVlan_entry, 0, sizeof(ipSubnetVlan_entry));

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,pfLen=%d,index=0x%08x", unit, pfLen, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((pfLen > 32)), RT_ERR_OUT_OF_RANGE);

    /* function body */
    if ( (VLANDB_IPSUBNET_TBL_USED(unit) > 0) && (index < VLANDB_IPSUBNET_TBL_BOTTOM(unit)) )
    {
        /* ip-subnet-based vlan */
        bottom = (VLANDB_IPSUBNET_TBL_BOTTOM(unit) - 1);
        /* have more entries ? */
        if (index < bottom)
        {
            dstIdx = index;
            for (pfl = pfLen; pfl > 0; pfl--)
            {
                srcIdx = VLANDB_IPSUBNET_TBL_BOTTOM(unit) - VLANDB_IPSUBNET_PFLEN_CNT(unit, pfl-1) - 1;
                if (dstIdx != srcIdx)
                {
                    /* move */
                    LONGAN_VLAN_DBG_PRINTF(3, "ipSubMV0: pfl=%d, dstIdx=%d, srcIdx=%d; pfl=%d, pfLen=%d\n", pfl, dstIdx, srcIdx, pfl, pfLen);
                    RT_ERR_HDL(__dal_longan_vlan_ipSubEntry_move(unit, dstIdx, srcIdx), errHandle, ret);

                    dstIdx = srcIdx;    /* new dstIdx */
                }
            }

            if (dstIdx != bottom)
            {
                /* move the last entry */
                srcIdx = bottom;
                LONGAN_VLAN_DBG_PRINTF(3, "ipSubMV0: pfl=%d, dstIdx=%d, srcIdx=%d; pfl=%d, pfLen=%d\n", pfl, dstIdx, srcIdx, pfl, pfLen);
                RT_ERR_HDL(__dal_longan_vlan_ipSubEntry_move(unit, dstIdx, srcIdx), errHandle, ret);
            }
        }

        /* clear the last entry */
        LONGAN_VLAN_DBG_PRINTF(3, "ipSubCLR: bottom=%d\n", bottom);
        RT_ERR_HDL(dal_longan_vlan_ipSubnetBasedVlanEntry_set(unit, bottom, &ipSubnetVlan_entry), errHandle, ret);

        /* update counters */
        for (i = pfLen; i < LONGAN_VLAN_IP_PFLEN; i++)
        {
            VLANDB_IPSUBNET_PFLEN_CNT(unit, i) -= 1;
        }

        VLANDB_IPSUBNET_TBL_USED(unit) -= 1;
    }

errHandle:
    return ret;
}

int32
_dal_longan_vlan_ipSubEntry_find(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry, uint32 *pIndex)
{
    int32 ret = RT_ERR_OK;
    uint32 index = 0;
    rtk_vlan_ipSubnetVlanEntry_t ipSubnetVlan_entry;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    for (index = VLANDB_IPSUBNET_TBL_IDX_MIN(unit); index < (VLANDB_IPSUBNET_TBL_BOTTOM(unit)); index++)
    {
        osal_memset(&ipSubnetVlan_entry, 0, sizeof(ipSubnetVlan_entry));
        RT_ERR_HDL(dal_longan_vlan_ipSubnetBasedVlanEntry_get(unit, index, &ipSubnetVlan_entry), errHandle, ret);

        if ( (pEntry->sip == ipSubnetVlan_entry.sip) &&
                (pEntry->sip_care== ipSubnetVlan_entry.sip_care) ){

            *pIndex = index;
            goto errOk;
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    return ret;
}

/* Function Name:
 *      _dal_longan_vlan_maskValid_check
 * Description:
 *      Check valid netmask
 * Input:
 *      mask    - ipv4 netmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static ipaddr_t
_dal_longan_vlan_maskValid_check(ipaddr_t mask)
{
    uint32 tmpMask = ~mask;

    if ( 0 == (tmpMask & (tmpMask + 1)) )
        return RT_ERR_OK;

    return RT_ERR_FAILED;
}

/* Function Name:
 *      __dal_longan_vlan_macBasedVlanEntry_get
 * Description:
 *      Get MAC-based vlan.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pEntry    - pointer buffer of mac based vlan entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to dal_longan_vlan_igrVlanCnvtBlkMode_set.
 */
int32
__dal_longan_vlan_macBasedVlanEntry_get(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry)
{
    int32   ret = RT_ERR_FAILED;
    vlan_igrcnvt_entry_t cnvt_entry;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;
    uint32  mac_uint32[2],temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_ERR_HDL(_dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_MAC_BASED, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pEntry, RT_ERR_NULL_POINTER);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*** get entry from chip ***/
    if ((ret = table_read(unit, LONGAN_VLAN_MAC_BASEDt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*Search Key*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VALIDtf,\
        &pEntry->valid, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PORT_TYPEtf,\
        &pEntry->port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PORT_IDtf,\
        &pEntry->port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_PORT_IDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x3F)
        pEntry->port_care = 1;
    else
        pEntry->port_care = 0;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_IVID_VALIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(0 == temp_var)
        pEntry->ivid_aft = ACCEPT_FRAME_TYPE_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_IVID_VALIDtf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(0 == temp_var)
            pEntry->ivid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
        else
            pEntry->ivid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_OVID_VALIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(0 == temp_var)
        pEntry->ovid_aft = ACCEPT_FRAME_TYPE_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_OVID_VALIDtf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(0 == temp_var)
            pEntry->ovid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
        else
            pEntry->ovid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_SMACtf,\
        &mac_uint32[0], (uint32 *) &cnvt_entry), errHandle, ret);
    pEntry->mac.octet[0] = (uint8)(mac_uint32[1] >> 8);
    pEntry->mac.octet[1] = (uint8)(mac_uint32[1]);
    pEntry->mac.octet[2] = (uint8)(mac_uint32[0] >> 24);
    pEntry->mac.octet[3] = (uint8)(mac_uint32[0] >> 16);
    pEntry->mac.octet[4] = (uint8)(mac_uint32[0] >> 8);
    pEntry->mac.octet[5] = (uint8)(mac_uint32[0]);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_SMACtf,\
        &mac_uint32[0], (uint32 *) &cnvt_entry), errHandle, ret);
    pEntry->mac_care.octet[0] = (uint8)(mac_uint32[1] >> 8);
    pEntry->mac_care.octet[1] = (uint8)(mac_uint32[1]);
    pEntry->mac_care.octet[2] = (uint8)(mac_uint32[0] >> 24);
    pEntry->mac_care.octet[3] = (uint8)(mac_uint32[0] >> 16);
    pEntry->mac_care.octet[4] = (uint8)(mac_uint32[0] >> 8);
    pEntry->mac_care.octet[5] = (uint8)(mac_uint32[0]);

    /*Action*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_FWD_ACTtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pEntry->fwd_action = ACTION_DROP;
            break;
        case 1:
            pEntry->fwd_action = ACTION_FORWARD;
            break;
        case 2:
            pEntry->fwd_action = ACTION_TRAP2CPU;
            break;
        case 3:
            pEntry->fwd_action = ACTION_COPY2CPU;
            break;
        case 4:
            pEntry->fwd_action = ACTION_TRAP2MASTERCPU;
            break;
        case 5:
            pEntry->fwd_action = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_IGNORE_IGR_FLTRtf,\
        &pEntry->igrvlanfilter_ignore, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VLAN_TYPEtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(0 == temp_var)
        pEntry->vlan_type = INNER_VLAN;
    else if ( 1 == temp_var)
        pEntry->vlan_type = OUTER_VLAN;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TPID_ASSIGNtf,\
        &pEntry->tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TPID_IDXtf,\
        &pEntry->tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TAG_STS_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(0 == temp_var)
        pEntry->sts_action = VLAN_TAG_STATUS_ACT_NONE;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TAG_STStf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
        if(0 == temp_var)
            pEntry->sts_action = VLAN_TAG_STATUS_ACT_UNTAGGED;
        else
            pEntry->sts_action = VLAN_TAG_STATUS_ACT_TAGGED;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PRI_ASSIGNtf,\
        &pEntry->pri_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PRItf,\
        &pEntry->pri_new, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VID_ASSIGNtf,\
        &pEntry->vid_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VIDtf,\
        &pEntry->vid_new, (uint32 *) &cnvt_entry), errHandle, ret);

    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      __dal_longan_vlan_macBasedVlanEntry_set
 * Description:
 *      Set MAC-based vlan.
 * Input:
 *      unit     - unit id
 *      index    - entry index
 * Output:
 *      pEntry    -  pointer buffer of Mac-based VLAN entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_QOS_1P_PRIORITY         - invalid priority value
*       RT_ERR_VLAN_TPID_INDEX        - invalid tpid index
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to dal_longan_vlan_igrVlanCnvtBlkMode_set.
 */
int32
__dal_longan_vlan_macBasedVlanEntry_set(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry)
{
    int32   ret = RT_ERR_FAILED;
    vlan_igrcnvt_entry_t cnvt_entry;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;
    uint32  mac_uint32[2];
    uint32  port_mask,portType_mask;
    uint32  temp_var1,temp_var2;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d",unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_ERR_HDL(_dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_MAC_BASED, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->port_care),RT_ERR_INPUT);
    RT_PARAM_CHK((1 < pEntry->port_type), RT_ERR_INPUT);
    RT_PARAM_CHK(((pEntry->port_care != 0) && (0 == pEntry->port_type) && !HWP_PORT_EXIST(unit, pEntry->port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK(((pEntry->port_care != 0) && (0 != pEntry->port_type) && (pEntry->port >= HAL_MAX_NUM_OF_TRUNK(unit))), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ivid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ovid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_COPY2MASTERCPU < pEntry->fwd_action),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->igrvlanfilter_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TYPE_END <= pEntry->vlan_type),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->pri_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pEntry->pri_new), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->vid_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vid_new > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pEntry->tpid_action),RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_CVLAN_TPID(unit) <= pEntry->tpid_idx), RT_ERR_VLAN_TPID_INDEX);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VALIDtf,\
        &pEntry->valid, (uint32 *) &cnvt_entry), errHandle, ret);

    mac_uint32[1] = ((uint32)pEntry->mac.octet[0]) << 8;
    mac_uint32[1] |= ((uint32)pEntry->mac.octet[1]);
    mac_uint32[0] = ((uint32)pEntry->mac.octet[2]) << 24;
    mac_uint32[0] |= ((uint32)pEntry->mac.octet[3]) << 16;
    mac_uint32[0] |= ((uint32)pEntry->mac.octet[4]) << 8;
    mac_uint32[0] |= ((uint32)pEntry->mac.octet[5]);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_SMACtf,\
        &mac_uint32[0], (uint32 *) &cnvt_entry), errHandle, ret);

    mac_uint32[1] = ((uint32)pEntry->mac_care.octet[0]) << 8;
    mac_uint32[1] |= ((uint32)pEntry->mac_care.octet[1]);
    mac_uint32[0] = ((uint32)pEntry->mac_care.octet[2]) << 24;
    mac_uint32[0] |= ((uint32)pEntry->mac_care.octet[3]) << 16;
    mac_uint32[0] |= ((uint32)pEntry->mac_care.octet[4]) << 8;
    mac_uint32[0] |= ((uint32)pEntry->mac_care.octet[5]);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_SMACtf,\
        &mac_uint32[0], (uint32 *) &cnvt_entry), errHandle, ret);

    if (pEntry->port_care != 0)
    {
        port_mask = 0x3F;
        portType_mask = 0x1;
    }
    else
    {
        port_mask = 0;
        portType_mask = 0;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PORT_IDtf,\
        &pEntry->port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_PORT_IDtf,\
        &port_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PORT_TYPEtf,\
        &pEntry->port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_PORT_TYPEtf,\
        &portType_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pEntry->ivid_aft)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            temp_var1 = 0;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_IVID_VALIDtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_IVID_VALIDtf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pEntry->ovid_aft)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            temp_var1 = 0;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_OVID_VALIDtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_BMSK_OVID_VALIDtf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pEntry->fwd_action)
    {
        case ACTION_DROP:
            temp_var1 = 0;
            break;
        case ACTION_FORWARD:
            temp_var1 = 1;
            break;
        case ACTION_TRAP2CPU:
            temp_var1 = 2;
            break;
        case ACTION_COPY2CPU:
            temp_var1 = 3;
            break;
        case ACTION_TRAP2MASTERCPU:
            temp_var1 = 4;
            break;
        case ACTION_COPY2MASTERCPU:
            temp_var1 = 5;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_FWD_ACTtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_IGNORE_IGR_FLTRtf,\
        &pEntry->igrvlanfilter_ignore, (uint32 *) &cnvt_entry), errHandle, ret);

    temp_var1 = (pEntry->vlan_type==INNER_VLAN) ? 0 : 1;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VLAN_TYPEtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pEntry->sts_action)
    {
        case VLAN_TAG_STATUS_ACT_NONE:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_UNTAGGED:
            temp_var1 = 1;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_TAGGED:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TAG_STS_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TAG_STStf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TPID_ASSIGNtf,\
        &pEntry->tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_TPID_IDXtf,\
        &pEntry->tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PRI_ASSIGNtf,\
        &pEntry->pri_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_PRItf,\
        &pEntry->pri_new, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VID_ASSIGNtf,\
        &pEntry->vid_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_MAC_BASED_VIDtf,\
        &pEntry->vid_new, (uint32 *) &cnvt_entry), errHandle, ret);

    /* programming entry to chip */
    if ((ret = table_write(unit, LONGAN_VLAN_MAC_BASEDt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}
/* internal APIs, called by dal_longan_vlan_* APIs (should NOT lock/unlock semaphore inside) */
static inline int32 _dal_longan_vlan_macEntry_move(uint32 unit, uint32 dstIdx, uint32 srcIdx)
{
    int32 ret = RT_ERR_OK;
    rtk_vlan_macVlanEntry_t macVlan_entry;

    osal_memset(&macVlan_entry, 0, sizeof(macVlan_entry));

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,dst=%d,src=%d", unit, dstIdx, srcIdx);

    LONGAN_VLAN_DBG_PRINTF(3, "unit=%d,dst=%d,src=%d\n", unit, dstIdx, srcIdx);

    /* parameter check */
    RT_PARAM_CHK((dstIdx == srcIdx), RT_ERR_INPUT);
    RT_PARAM_CHK(((dstIdx) < VLANDB_MAC_TBL_IDX_MIN(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((srcIdx) < VLANDB_MAC_TBL_IDX_MIN(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((dstIdx) > VLANDB_MAC_TBL_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((srcIdx) > VLANDB_MAC_TBL_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    // 1. cp srcIdx / 2. write to dstIdx / 3. remove srcIdx
    RT_ERR_HDL(__dal_longan_vlan_macBasedVlanEntry_get(unit, srcIdx, &macVlan_entry), errHandle, ret);
    RT_ERR_HDL(__dal_longan_vlan_macBasedVlanEntry_set(unit, dstIdx, &macVlan_entry), errHandle, ret);
    osal_memset(&macVlan_entry, 0, sizeof(macVlan_entry));
    RT_ERR_HDL(__dal_longan_vlan_macBasedVlanEntry_set(unit, srcIdx, &macVlan_entry), errHandle, ret);

errHandle:
    return ret;
}

/* Function Name:
 *      _dal_longan_vlan_macEntry_alloc
 * Description:
 *      Output mac-based vlan table index
 * Input:
 *      unit    - unit id
 * Output:
 *      pIndex  - entry index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_TBL_FULL       - table full
 * Note:
 *
 */
static int32
_dal_longan_vlan_macEntry_alloc(uint32 unit, uint32 *pIndex)
{
    int32   ret = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,pIndex=0x%08x", unit, pIndex);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    /* mac-based vlan table entry allocation */
    if ((VLANDB_MAC_TBL_SIZE(unit) - VLANDB_MAC_TBL_USED(unit)) < 1)
    {
        return RT_ERR_TBL_FULL;
    }

    /* use the bottom entry directly */
    *pIndex = VLANDB_MAC_TBL_BOTTOM(unit);

    VLANDB_MAC_TBL_USED(unit) += 1;

    LONGAN_VLAN_DBG_PRINTF(3, "*pIndex = %d\n", *pIndex);

    return ret;
}

/* Function Name:
 *      _dal_longan_vlan_macEntry_free
 * Description:
 *      Rearrange mac-based table
 * Input:
 *      unit    - unit id
 *      index   - entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_TBL_FULL       - table full
 * Note:
 *
 */
static int32
_dal_longan_vlan_macEntry_free(uint32 unit, uint32 index)
{
    int32   ret = RT_ERR_OK;
    uint32  bottom = 0;
    rtk_vlan_macVlanEntry_t macEntry;

    osal_memset(&macEntry, 0, sizeof(macEntry));

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d,index=0x%08x", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */

    /* function body */
    bottom = (VLANDB_MAC_TBL_BOTTOM(unit) - 1);

    if ( (VLANDB_MAC_TBL_USED(unit) > 0) && (index < (VLANDB_MAC_TBL_BOTTOM(unit))) )
    {
        /* mac-based vlan */
        LONGAN_VLAN_DBG_PRINTF(3, "macMV0: dstIdx=%d, srcIdx=%d;\n", index, bottom);
        if (bottom == index)
            RT_ERR_HDL(__dal_longan_vlan_macBasedVlanEntry_set(unit, bottom, &macEntry), errHandle, ret);
        else
            RT_ERR_HDL(_dal_longan_vlan_macEntry_move(unit, index, bottom), errHandle, ret);

        /* update counters */
        VLANDB_MAC_TBL_USED(unit) -= 1;
    }

errHandle:
    return ret;
}

int32
_dal_longan_vlan_macEntry_find(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry, uint32 *pIndex)
{
    int32 ret = RT_ERR_OK;
    uint32 index = 0;
    rtk_vlan_macVlanEntry_t macEntry;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);

    for (index = VLANDB_MAC_TBL_IDX_MIN(unit); index < (VLANDB_MAC_TBL_BOTTOM(unit)); index++)
    {
        osal_memset(&macEntry, 0, sizeof(macEntry));
        RT_ERR_HDL(__dal_longan_vlan_macBasedVlanEntry_get(unit, index, &macEntry), errHandle, ret);

        if ( 0 == (osal_memcmp( &pEntry->mac, &macEntry.mac, sizeof(rtk_mac_t) )) ){
            *pIndex = index;
            goto errOk;
        }
    }

    ret = RT_ERR_ENTRY_NOTFOUND;

errHandle:
errOk:
    return ret;
}

/* Function Name:
 *      dal_longan_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_VLAN_VID   - invalid vid
 *      RT_ERR_VLAN_EXIST - vlan is exist
 * Note:
 *      1. Default STG is assigned to CIST after vlan creation and
 *         STG can be re-assigned later by dal_longan_vlan_stg_set
 *      2. Default lookup mode for L2 unicast and L2/IP multicast is assigned
 *         to be based on VID
 */
int32
dal_longan_vlan_create(uint32 unit, rtk_vlan_t vid)
{
    int32   ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(VLANINFO_VALID_IS_SET(unit, vid), RT_ERR_VLAN_EXIST);

    /* create vlan in CHIP*/
    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid         = vid;
    vlan_data_entry.msti    = RTK_DEFAULT_MSTI;

    VLAN_SEM_LOCK(unit);

    /* write entry to CHIP*/
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* Set vlan valid bit in software */
    VLANINFO_VALID_SET(unit, vid);
    pDal_longan_vlan_info[unit]->count++;

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_create */

/* Function Name:
 *      dal_longan_vlan_destroy
 * Description:
 *      Destroy the vlan in the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
int32
dal_longan_vlan_destroy(uint32 unit, rtk_vlan_t vid)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid     = vid;

    VLAN_SEM_LOCK(unit);
    /* write entry to CHIP*/
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* Unset vlan valid bit in software */
    VLANINFO_VALID_CLEAR(unit, vid);
    pDal_longan_vlan_info[unit]->count--;

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longanw_vlan_destroy */


/* Function Name:
 *      dal_longan_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan in the specified device.
 *      If restore_default_vlan is enable, default vlan will be restored.
 * Input:
 *      unit                 - unit id
 *      restore_default_vlan - keep or restore default vlan setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
int32
dal_longan_vlan_destroyAll(uint32 unit, uint32 keep_and_restore_default_vlan)
{
    int32       ret;
    dal_longan_vlan_data_t vlan_data_entry;
    rtk_vlan_t  vid;
    uint32      vlan_tableSize;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, keep_and_restore_default_vlan=%d",
           unit, keep_and_restore_default_vlan);

    RT_INIT_CHK(vlan_init[unit]);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    if ((ret = table_size_get(unit, LONGAN_VLANt, &vlan_tableSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "table size get failed");
        return ret;
    }

    for (vid = 0; vid < vlan_tableSize; vid++)
    {
        if (VLANINFO_VALID_IS_SET(unit, vid))
        {
            if ((ret = dal_longan_vlan_destroy(unit, vid)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
                return ret;
            }
        }
    }

    VLAN_SEM_LOCK(unit);
    if (1 == keep_and_restore_default_vlan)
    {
        /* configure vlan data entry */
        vlan_data_entry.vid         = RTK_DEFAULT_VLAN_ID;
        vlan_data_entry.msti    = RTK_DEFAULT_MSTI;
        HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.member_portmask);
        HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.untag_portmask);

        if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }

        /* Set vlan valid bit in software */
        VLANINFO_VALID_SET(unit, RTK_DEFAULT_VLAN_ID);
        pDal_longan_vlan_info[unit]->count++;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_destroyAll */

/* Function Name:
 *      dal_longan_vlan_port_add
 * Description:
 *      Add one vlan member to the specified device.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 *      port     - port id for add
 *      is_untag - untagged or tagged member
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_VLAN_PORT_MBR_EXIST  - member port exist in the specified vlan
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      1. The valid value of is_untag are {0: tagged, 1: untagged}
 */
int32
dal_longan_vlan_port_add(uint32 unit, rtk_vlan_t vid, rtk_port_t port, uint32 is_untag)
{
    int32           ret;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, port=%d, is_untag=%d",
           unit, vid, port, is_untag);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(is_untag > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid     = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if (RTK_PORTMASK_IS_PORT_SET(vlan_data_entry.member_portmask, port))
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return RT_ERR_VLAN_PORT_MBR_EXIST;
    }

    /* set member_portmask and untag_portmask */
    RTK_PORTMASK_PORT_SET(vlan_data_entry.member_portmask, port);
    if (1 == is_untag)
    {
        RTK_PORTMASK_PORT_SET(vlan_data_entry.untag_portmask, port);
    }

    /* programming to chip */
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_port_add */


/* Function Name:
 *      dal_longan_vlan_port_del
 * Description:
 *      Delete one vlan member from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      port - port id for delete
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
int32
dal_longan_vlan_port_del(uint32 unit, rtk_vlan_t vid, rtk_port_t port)
{
    int32           ret;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, port=%d",
           unit, vid, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid     = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* clear member_portmask and untag_portmask */
    RTK_PORTMASK_PORT_CLEAR(vlan_data_entry.member_portmask, port);
    RTK_PORTMASK_PORT_CLEAR(vlan_data_entry.untag_portmask, port);

    /* programming to chip */
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_port_del */


/* Function Name:
 *      dal_longan_vlan_port_get
 * Description:
 *      Get the vlan members from the specified device.
 * Input:
 *      unit              - unit id
 *      vid               - vlan id
 * Output:
 *      pMember_portmask  - pointer buffer of member ports
 *      pUntag_portmask   - pointer buffer of untagged member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_port_get(
    uint32         unit,
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32   ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((NULL == pMember_portmask) || (NULL == pUntag_portmask)), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);


    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid     = vid;

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RTK_PORTMASK_ASSIGN(*pMember_portmask, vlan_data_entry.member_portmask);
    RTK_PORTMASK_ASSIGN(*pUntag_portmask, vlan_data_entry.untag_portmask);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%u, vid=%u, pMember_portmask=0x%X, pUntag_portmask=0x%X",
           unit, vid, (*pMember_portmask).bits[0], (*pUntag_portmask).bits[0]);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_port_get */


/* Function Name:
 *      dal_longan_vlan_port_set
 * Description:
 *      Replace the vlan members in the specified device.
 * Input:
 *      unit              - unit id
 *      vid               - vlan id
 *      pMember_portmask  - member ports
 *      pUntag_portmask   - untagged member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Replace with new specified VLAN member regardless of original setting
 */
int32
dal_longan_vlan_port_set(
    uint32         unit,
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32           ret;
    dal_longan_vlan_data_t vlan_data_entry;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d",
           unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((NULL == pMember_portmask) || (NULL == pUntag_portmask)), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid     = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* copy new portmask to vlan data entry */
    RTK_PORTMASK_ASSIGN(vlan_data_entry.member_portmask, *pMember_portmask);
    RTK_PORTMASK_ASSIGN(vlan_data_entry.untag_portmask, *pUntag_portmask);

    /* programming to chip */
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_port_set */

/* Function Name:
 *      dal_longan_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan from the specified device.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 * Output:
 *      pStg  - pointer buffer of spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_stg_get(uint32 unit, rtk_vlan_t vid, rtk_stg_t *pStg)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pStg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    /* set fid/msti to new fid/msti */
    *pStg = vlan_data_entry.msti;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pStg=%d", *pStg);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_stg_get */


/* Function Name:
 *      dal_longan_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      stg  - spanning tree group instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_MSTI                 - invalid msti
 * Note:
 *      None
 */
int32
dal_longan_vlan_stg_set(uint32 unit, rtk_vlan_t vid, rtk_stg_t stg)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, stg=%d", unit, vid, stg);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((stg >= HAL_MAX_NUM_OF_MSTI(unit)), RT_ERR_MSTI);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid     = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set msti to new msti */
    vlan_data_entry.msti = stg;

    /* programming to chip */
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_stg_set */

/* Function Name:
 *      dal_longan_vlan_l2LookupSvlFid_get
 * Description:
 *      Get the filtering database id in the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      type - mac type
 * Output:
 *      pFid - pointer buffer of filtering database id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_l2LookupSvlFid_get(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t *pFid)
{
    int32 ret = RT_ERR_FAILED;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, mac type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= VLAN_L2_MAC_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case VLAN_L2_MAC_TYPE_UC:
            field = LONGAN_UC_SVL_FIDf;
            break;
        case VLAN_L2_MAC_TYPE_MC:
            field = LONGAN_MC_SVL_FIDf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_VLAN_CTRLr, field,pFid) ) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pFid=%d", *pFid);

    return  RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_l2LookupSvlFid_set
 * Description:
 *      Set the filter id  to the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      None
 */
int32
dal_longan_vlan_l2LookupSvlFid_set(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t fid)
{
    int32 ret = RT_ERR_FAILED;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, mac type=%d, fid=%d", unit, type,fid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((fid > RTK_FID_MAX) , RT_ERR_FID);
    RT_PARAM_CHK(type >= VLAN_L2_MAC_TYPE_END, RT_ERR_INPUT);

    switch(type)
    {
        case VLAN_L2_MAC_TYPE_UC:
            field = LONGAN_UC_SVL_FIDf;
            break;
        case VLAN_L2_MAC_TYPE_MC:
            field = LONGAN_MC_SVL_FIDf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* programming to chip */
    if ((ret = reg_field_write(unit, LONGAN_VLAN_CTRLr,field,&fid) ) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_l2LookupMode_get
 * Description:
 *      Get L2 lookup mode for L2  traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      type - dmac type
 * Output:
 *      pMode - lookup mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Each VLAN can have its own lookup mode for L2  traffic
 */
int32
dal_longan_vlan_l2LookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t *pMode)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX) , RT_ERR_VLAN_VID);
    RT_PARAM_CHK(type >= VLAN_L2_MAC_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    if(VLAN_L2_MAC_TYPE_UC == type)
        *pMode = vlan_data_entry.ucast_mode;
    else
        *pMode = vlan_data_entry.mcast_mode;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pMode=%d", *pMode);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_l2LookupMode_get */

/* Function Name:
 *      dal_longan_vlan_l2LookupMode_set
 * Description:
 *      Set L2 lookup mode for L2  traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      type - dmac type
 *      mode  - lookup mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_INPUT                - Invalid input parameter
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      Each VLAN can have its own lookup mode for L2 unicast traffic
 */
int32
dal_longan_vlan_l2LookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t mode)
{
    int32 ret = RT_ERR_FAILED;
    uint32 value;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, mode=%d", unit, vid, mode);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(type >= VLAN_L2_MAC_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(mode >= VLAN_L2_LOOKUP_MODE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* translate to chip's definition */
    switch (mode)
    {
        case VLAN_L2_LOOKUP_MODE_VID:
            value = 0;
            break;
        case VLAN_L2_LOOKUP_MODE_FID:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* configure vlan data entry */
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if(VLAN_L2_MAC_TYPE_UC == type)
        vlan_data_entry.ucast_mode = value;
    else
        vlan_data_entry.mcast_mode = value;

    /* programming to chip */
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_l2LookupMode_set */

/* Function Name:
 *      dal_longan_vlan_groupMask_get
 * Description:
 *      Get specific VLAN Groupmask.
 * Input:
 *      unit     - unit id
 *      vid   - vlan id
 * Output:
 *      pGroupmask - VLAN groupmask configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - profile index is out of range
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *     (1) ACL can refer the bits as search key for VLAN based application
 *     (2) Bits[0-5] used for special Application Packet(IGMP/MLD/DHCP/DHCPV6/ARP/ARP_REPLY),
 *      refer the following API
 *          -dal_longan_trap_portMgmtFrameAction_get/set
 *     (3) Priority : ACL > Application Packet Trap
 */
int32
dal_longan_vlan_groupMask_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pGroupmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    /* get vlan group mask  */
    osal_memcpy(pGroupmask->bits, vlan_data_entry.groupMask.bits, sizeof(rtk_vlan_groupMask_t));

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pGroupmask bit=0x%x", pGroupmask->bits[0]);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_groupMask_set
 * Description:
 *      Set specific VLAN Groupmask.
 * Input:
 *      unit     - unit id
 *      vid   - vlan id
 *      pGroupmask - VLAN Groupmask configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *     (1) ACL can refer the bits as search key for VLAN based application
 *     (2) Bits[0-5] used for special Application Packet(IGMP/MLD/DHCP/DHCPV6/ARP/ARP_REPLY),
 *      refer the following API
 *          -dal_longan_trap_portMgmtFrameAction_get/set
 *     (3) Priority : ACL > Application Packet Trap
 */
int32
dal_longan_vlan_groupMask_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, pGroupmask bit=0x%x", unit, vid, pGroupmask->bits[0]);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);
    /*RT_PARAM_CHK((pGroupmask->bits[0] >= (1<<HAL_MAX_NUM_OF_VLAN_GROUP(unit))), RT_ERR_INPUT);*/

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid     = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set msti to new msti */
    osal_memcpy(vlan_data_entry.groupMask.bits,pGroupmask->bits, sizeof(rtk_vlan_groupMask_t));

    /* programming to chip */
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}
/* Function Name:
 *      dal_longan_vlan_profileIdx_get
 * Description:
 *      Get VLAN profile index of specified VLAN.
 * Input:
 *      unit    - unit id
 *      vid     - vlan id
 * Output:
 *      pIdx    - VLAN profile index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Each VLAN can map to a VLAN profile (0~7)
 */
int32
dal_longan_vlan_profileIdx_get(uint32 unit, rtk_vlan_t vid, uint32 *pIdx)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    *pIdx = vlan_data_entry.profile_idx;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pIdx=%d", *pIdx);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_profileIdx_get */

/* Function Name:
 *      _dal_longan_vlan_profileIdx_get
 * Description:
 *      Get VLAN profile index of specified VLAN, skip the vid exist check.
 * Input:
 *      unit    - unit id
 *      vid     - vlan id
 * Output:
 *      pIdx    - VLAN profile index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Each VLAN can map to a VLAN profile (0~7)
 *      This API skip the vid exist check
 */
int32
_dal_longan_vlan_profileIdx_get(uint32 unit, rtk_vlan_t vid, uint32 *pIdx)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pIdx), RT_ERR_NULL_POINTER);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    *pIdx = vlan_data_entry.profile_idx;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pIdx=%d", *pIdx);

    return RT_ERR_OK;
} /* end of _dal_longan_vlan_profileIdx_get */

/* Function Name:
 *      dal_longan_vlan_profileIdx_set
 * Description:
 *      Set VLAN profile index of specified VLAN.
 * Input:
 *      unit    - unit id
 *      vid     - vlan id
 *      idx     - VLAN profile index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_OUT_OF_RANGE         - input value is out of range
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      Each VLAN can map to a VLAN profile (0~7)
 */
int32
dal_longan_vlan_profileIdx_set(uint32 unit, rtk_vlan_t vid, uint32 idx)
{
    int32 ret = RT_ERR_FAILED;
    dal_longan_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, idx=%d", unit, vid, idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(idx >= HAL_MAX_NUM_OF_VLAN_PROFILE(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_longan_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    vlan_data_entry.profile_idx = idx;

    /* programming to chip */
    if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_profileIdx_set */

/* Function Name:
 *      dal_longan_vlan_profile_get
 * Description:
 *      Get specific VLAN profile.
 * Input:
 *      unit        - unit id
 *      idx         - VLAN profile index
 * Output:
 *      pProfile    - VLAN profile configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - profile index is out of range
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Totally 8 profile (0~7) are supported
 */
int32
dal_longan_vlan_profile_get(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile)
{
    int32 ret = RT_ERR_FAILED;
    uint32 temp_var;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(idx >= HAL_MAX_NUM_OF_VLAN_PROFILE(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pProfile), RT_ERR_NULL_POINTER);

    osal_memset(pProfile,0x00, sizeof(rtk_vlan_profile_t));

    VLAN_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_LRN_ENf, &pProfile->lrnMode), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_NEW_SA_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_BGD,
                          &pProfile->l2_newMacAct), errHandle, ret);

    /*routing fields */
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_UC_RT_ENf, &pProfile->ip4_uRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_UC_RT_ENf, &pProfile->ip6_uRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_RT_ENf, &pProfile->ip4_mRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_RT_ENf, &pProfile->ip6_mRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_CHK_ENf, &pProfile->ip4_uRPF_chk), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_CHK_ENf, &pProfile->ip6_uRPF_chk), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_DFLT_ROUTE_ENf, &pProfile->ip4_uRPF_dfltRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_DFLT_ROUTE_ENf, &pProfile->ip6_uRPF_dfltRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_CHK_MODEf, &pProfile->ip4_uRPF_chkMode), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_CHK_MODEf, &pProfile->ip6_uRPF_chkMode), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_FAIL_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_URPF_FAIL,
                          &pProfile->ip4_uRPF_failAct), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_FAIL_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_URPF_FAIL,
                          &pProfile->ip6_uRPF_failAct), errHandle, ret);

    /*Rsvd MC Action*/
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_ACT_224_0_0f, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_RSVD_MC,
                          &pProfile->ipmc_224_0_act), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_ACT_224_0_1f, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_RSVD_MC,
                          &pProfile->ipmc_224_1_act), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_ACT_239_0_0f, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_RSVD_MC,
                          &pProfile->ipmc_239_0_act), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_ACT_0_Xf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_RSVD_MC,
                          &pProfile->ip6mc_00xx_act), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_ACT_0_X_Xf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_RSVD_MC,
                          &pProfile->ip6mc_mld_0x_act), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_ACT_DB8_X_Xf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_RSVD_MC,
                          &pProfile->ip6mc_mld_db8_act), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_ND_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_RSVD_MC,
                          &pProfile->ip6_nd_act), errHandle, ret);


    /*L2 Bridge Action*/
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_BDG_MODEf, &pProfile->ipmcBdgMode), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_BDG_MODEf, &pProfile->ip6mcBdgMode), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_MC_BDG_LU_MIS_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_BGD,
                          &pProfile->l2mc_bdgLuMisAct), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_L2BDG_LU_MIS_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_BGD,
                          &pProfile->ipmc_bdgLuMisAct), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_L2BDG_LU_MIS_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateRtk(temp_var,LONGAN_VLAN_ACTION_BGD,
                          &pProfile->ip6mc_bdgLuMisAct), errHandle, ret);

    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_UNKN_MC_FLD_PMSKf, &pProfile->l2mc_unknFloodPm.bits[0]), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_UNKN_MC_FLD_PMSKf, &pProfile->ipmc_unknFloodPm.bits[0]), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_UNKN_MC_FLD_PMSKf, &pProfile->ip6mc_unknFloodPm.bits[0]), errHandle, ret);

    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "learning=%d", pProfile->lrnMode);

    return RT_ERR_OK;

errHandle:
    VLAN_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
} /* end of dal_longan_vlan_profile_get */

/* Function Name:
 *      dal_longan_vlan_profile_set
 * Description:
 *      Set specific VLAN profile.
 * Input:
 *      unit     - unit id
 *      idx      - VLAN profile index
 *      pProfile - VLAN profile configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - profile index is out of range
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Totally 8 profile (0~7) are supported
 */
int32
dal_longan_vlan_profile_set(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile)
{
    int32 ret = RT_ERR_FAILED;
    uint32 temp_var;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(idx >= HAL_MAX_NUM_OF_VLAN_PROFILE(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pProfile), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((LEARNING_MODE_END <= pProfile->lrnMode), RT_ERR_INPUT);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "learning=%d", pProfile->lrnMode);

    VLAN_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_LRN_ENf, &pProfile->lrnMode), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->l2_newMacAct,\
                          LONGAN_VLAN_ACTION_BGD, &temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_NEW_SA_ACTf, &temp_var), errHandle, ret);
    /*routing fields */
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_UC_RT_ENf, &pProfile->ip4_uRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_UC_RT_ENf, &pProfile->ip6_uRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_RT_ENf, &pProfile->ip4_mRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_RT_ENf, &pProfile->ip6_mRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_CHK_ENf, &pProfile->ip4_uRPF_chk), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_CHK_ENf, &pProfile->ip6_uRPF_chk), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_DFLT_ROUTE_ENf, &pProfile->ip4_uRPF_dfltRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_DFLT_ROUTE_ENf, &pProfile->ip6_uRPF_dfltRoute), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_CHK_MODEf, &pProfile->ip4_uRPF_chkMode), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_CHK_MODEf, &pProfile->ip6_uRPF_chkMode), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ip4_uRPF_failAct,\
                          LONGAN_VLAN_ACTION_URPF_FAIL,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_URPF_FAIL_ACTf, &temp_var), errHandle, ret);
    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ip6_uRPF_failAct,\
                          LONGAN_VLAN_ACTION_URPF_FAIL,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_URPF_FAIL_ACTf, &temp_var), errHandle, ret);
    /*Rsvd MC Action*/
    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ipmc_224_0_act,\
                          LONGAN_VLAN_ACTION_RSVD_MC,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_ACT_224_0_0f, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ipmc_224_1_act,\
                          LONGAN_VLAN_ACTION_RSVD_MC,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_ACT_224_0_1f, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ipmc_239_0_act,\
                          LONGAN_VLAN_ACTION_RSVD_MC,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_ACT_239_0_0f, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ip6mc_00xx_act,\
                          LONGAN_VLAN_ACTION_RSVD_MC,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_ACT_0_Xf, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ip6mc_mld_0x_act,\
                          LONGAN_VLAN_ACTION_RSVD_MC,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_ACT_0_X_Xf, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ip6mc_mld_db8_act,\
                          LONGAN_VLAN_ACTION_RSVD_MC,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_ACT_DB8_X_Xf, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ip6_nd_act,\
                          LONGAN_VLAN_ACTION_RSVD_MC,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_ND_ACTf, &temp_var), errHandle, ret);

    /*L2 Bridge Action*/
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_BDG_MODEf, &pProfile->ipmcBdgMode), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_BDG_MODEf, &pProfile->ip6mcBdgMode), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->l2mc_bdgLuMisAct,\
                          LONGAN_VLAN_ACTION_BGD,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_MC_BDG_LU_MIS_ACTf, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ipmc_bdgLuMisAct,\
                          LONGAN_VLAN_ACTION_BGD,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_MC_L2BDG_LU_MIS_ACTf, &temp_var), errHandle, ret);

    RT_ERR_HDL(_dal_longan_vlan_actionXlateDal(pProfile->ip6mc_bdgLuMisAct,\
                          LONGAN_VLAN_ACTION_BGD,&temp_var), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_MC_L2BDG_LU_MIS_ACTf, &temp_var), errHandle, ret);

    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_L2_UNKN_MC_FLD_PMSKf, &pProfile->l2mc_unknFloodPm.bits[0]), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP4_UNKN_MC_FLD_PMSKf, &pProfile->ipmc_unknFloodPm.bits[0]), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, LONGAN_VLAN_PROFILE_SETr, REG_ARRAY_INDEX_NONE, idx,\
                          LONGAN_IP6_UNKN_MC_FLD_PMSKf, &pProfile->ip6mc_unknFloodPm.bits[0]), errHandle, ret);
    VLAN_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    VLAN_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
} /* end of dal_longan_vlan_profile_set */

/* Function Name:
 *      dal_longan_vlan_portFwdVlan_get
 * Description:
 *      Get forwarding vlan(inner/outer vlan)  on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tagType    - packet tag type
 * Output:
 *      pVlanType -  pointer to inner/outer vlan
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
dal_longan_vlan_portFwdVlan_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_vlan_pktTagMode_t        tagMode,
    rtk_vlanType_t            *pVlanType)
{
    int32  ret;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, tagType= %d", unit, port,tagMode);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(tagMode >= VLAN_PKT_TAG_MODE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanType), RT_ERR_NULL_POINTER);

    switch(tagMode)
    {
        case VLAN_PKT_TAG_MODE_UNTAG:
            field = LONGAN_UNTAGf;
            break;
        case VLAN_PKT_TAG_MODE_INNERTAG:
            field = LONGAN_ITAGf;
            break;
        case VLAN_PKT_TAG_MODE_OUTERTAG:
            field = LONGAN_OTAGf;
            break;
        case VLAN_PKT_TAG_MODE_DBLTAG:
            field = LONGAN_DBL_TAGf;
            break;
        case VLAN_PKT_TAG_MODE_ALL:
            field = LONGAN_UNTAGf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                         LONGAN_VLAN_PORT_FWD_CTRLr,
                         port,
                         REG_ARRAY_INDEX_NONE,
                         field,
                         pVlanType)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pVlanType=%x", *pVlanType);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portFwdVlan_set
 * Description:
 *      Set forwarding vlan(inner/outer vlan)  on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tagType    - packet tag type
 *      vlanType - inner/outer vlan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
dal_longan_vlan_portFwdVlan_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_vlan_pktTagMode_t        tagMode,
    rtk_vlanType_t             vlanType)
{
    int32  ret;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, tagType= %d", unit, port,tagMode);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_PKT_TAG_MODE_END <= tagMode), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TYPE_END <= vlanType), RT_ERR_INPUT);

    switch(tagMode)
    {
        case VLAN_PKT_TAG_MODE_UNTAG:
            field = LONGAN_UNTAGf;
            break;
        case VLAN_PKT_TAG_MODE_INNERTAG:
            field = LONGAN_ITAGf;
            break;
        case VLAN_PKT_TAG_MODE_OUTERTAG:
            field = LONGAN_OTAGf;
            break;
        case VLAN_PKT_TAG_MODE_DBLTAG:
            field = LONGAN_DBL_TAGf;
            break;
        case VLAN_PKT_TAG_MODE_ALL:
            field = LONGAN_UNTAGf;/*not be used*/
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if(VLAN_PKT_TAG_MODE_ALL == tagMode)
    {
        if ((ret = reg_array_field_write(unit,
                             LONGAN_VLAN_PORT_FWD_CTRLr,
                             port,
                             REG_ARRAY_INDEX_NONE,
                             LONGAN_UNTAGf,
                          &vlanType)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                             LONGAN_VLAN_PORT_FWD_CTRLr,
                             port,
                             REG_ARRAY_INDEX_NONE,
                             LONGAN_ITAGf,
                          &vlanType)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                             LONGAN_VLAN_PORT_FWD_CTRLr,
                             port,
                             REG_ARRAY_INDEX_NONE,
                             LONGAN_OTAGf,
                          &vlanType)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
        if ((ret = reg_array_field_write(unit,
                             LONGAN_VLAN_PORT_FWD_CTRLr,
                             port,
                             REG_ARRAY_INDEX_NONE,
                             LONGAN_DBL_TAGf,
                          &vlanType)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_write(unit,
                             LONGAN_VLAN_PORT_FWD_CTRLr,
                             port,
                             REG_ARRAY_INDEX_NONE,
                             field,
                          &vlanType)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      dal_longan_vlan_portAcceptFrameType_get
 * Description:
 *      Get vlan accept frame type of the port from the specified device.
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      type               - vlan type
 * Output:
 *      pAccept_frame_type - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used get accept frame type for 802.1Q or 802.1ad VLAN
 */
int32
dal_longan_vlan_portAcceptFrameType_get(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlanType_t             type,
    rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field1,field2;
    uint32  accept_tag = TRUE, accept_untag = TRUE;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAccept_frame_type), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field1 = LONGAN_ITAG_ACCEPTf;
            field2 = LONGAN_ITAG_UNTAG_ACCEPTf;
            break;
        case OUTER_VLAN:
            field1 = LONGAN_OTAG_ACCEPTf;
            field2 = LONGAN_OTAG_UNTAG_ACCEPTf;
            break;
        default:
            return RT_ERR_FAILED;
    }
    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                         LONGAN_VLAN_PORT_AFTr,
                         port,
                         REG_ARRAY_INDEX_NONE,
                         field1,
                         &accept_tag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                         LONGAN_VLAN_PORT_AFTr,
                         port,
                         REG_ARRAY_INDEX_NONE,
                         field2,
                         &accept_untag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (accept_tag == TRUE && accept_untag == TRUE)
        *pAccept_frame_type = ACCEPT_FRAME_TYPE_ALL;
    else if (accept_tag == TRUE)
        *pAccept_frame_type = ACCEPT_FRAME_TYPE_TAG_ONLY;
    else if (accept_untag == TRUE)
        *pAccept_frame_type = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
    else
         *pAccept_frame_type = ACCEPT_FRAME_TYPE_END;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pAccept_frame_type=%x", *pAccept_frame_type);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portAcceptFrameType_get */

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_set
 * Description:
 *      Set vlan accept frame type of the port to the specified device.
 * Input:
 *      unit              - unit id
 *      port              - port id
 *      accept_frame_type - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used to set accept frame type for 802.1Q or 802.1ad VLAN
 */
int32
dal_longan_vlan_portAcceptFrameType_set(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlanType_t             type,
    rtk_vlan_acceptFrameType_t accept_frame_type)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field1,field2;
    uint32  accept_tag, accept_untag;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, accept_frame_type=%d",
           unit, port, accept_frame_type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((accept_frame_type >= ACCEPT_FRAME_TYPE_END), RT_ERR_VLAN_ACCEPT_FRAME_TYPE);

    /* translate to chip's definition */
    switch (accept_frame_type)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            accept_tag = TRUE;
            accept_untag = TRUE;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            accept_tag = TRUE;
            accept_untag = FALSE;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            accept_tag = FALSE;
            accept_untag = TRUE;
            break;
        default:
            return RT_ERR_VLAN_ACCEPT_FRAME_TYPE;
    }

    switch(type)
    {
        case INNER_VLAN:
            field1 = LONGAN_ITAG_ACCEPTf;
            field2 = LONGAN_ITAG_UNTAG_ACCEPTf;
            break;
        case OUTER_VLAN:
            field1 = LONGAN_OTAG_ACCEPTf;
            field2 = LONGAN_OTAG_UNTAG_ACCEPTf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                         LONGAN_VLAN_PORT_AFTr,
                         port,
                         REG_ARRAY_INDEX_NONE,
                         field1,
                         &accept_tag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                         LONGAN_VLAN_PORT_AFTr,
                         port,
                         REG_ARRAY_INDEX_NONE,
                         field2,
                         &accept_untag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portAcceptFrameType_set */

/* Function Name:
 *      dal_longan_vlan_portIgrFilter_get
 * Description:
 *      Get vlan ingress filter configuration of the port from the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pIgr_filter - pointer buffer of ingress filter configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portIgrFilter_get(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t *pIgr_filter)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIgr_filter), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_IGR_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_IGR_FLTR_ACTf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pIgr_filter = INGRESS_FILTER_FWD;
            break;
        case 1:
            *pIgr_filter = INGRESS_FILTER_DROP;
            break;
        case 2:
            *pIgr_filter = INGRESS_FILTER_TRAP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pIgr_filter=%d", *pIgr_filter);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portIgrFilter_set
 * Description:
 *      Set vlan ingress filter configuration of the port from the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      igr_filter  - ingress filter configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_vlan_portIgrFilter_set(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t igr_filter)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, igr_filter=%d", unit, port, igr_filter);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((igr_filter >= INGRESS_FILTER_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (igr_filter)
    {
        case INGRESS_FILTER_FWD:
            value = 0;
            break;
        case INGRESS_FILTER_DROP:
            value = 1;
            break;
        case INGRESS_FILTER_TRAP:
            value = 2;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_IGR_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_IGR_FLTR_ACTf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrFilterEnable_get
 * Description:
 *      Get enable status of egress filtering on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of egress filtering
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portEgrFilterEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_EGR_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_EGR_FLTR_ENf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pEnable = DISABLED;
            break;
        case 1:
            *pEnable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=%x", *pEnable);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portEgrFilterEnable_get */

/* Function Name:
 *      dal_longan_vlan_portEgrFilterEnable_set
 * Description:
 *      Set enable status of egress filtering on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of egress filtering
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_vlan_portEgrFilterEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (enable)
    {
        case DISABLED:
            value = 0;
            break;
        case ENABLED:
            value = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_EGR_FLTRr, port, REG_ARRAY_INDEX_NONE, LONGAN_EGR_FLTR_ENf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portEgrFilterEnable_set */

/* Function Name:
 *      dal_longan_vlan_mcastLeakyEnable_get
 * Description:
 *      Get vlan egress leaky status of the system from the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pLeaky  - pointer buffer of vlan leaky of egress
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Enable leaky function to allow L2/IP multicast traffic to across VLAN.
 *      That is, egress VLAN filtering is bypassed by L2/IP multicast traffic.
 */
int32
dal_longan_vlan_mcastLeakyEnable_get(uint32 unit, rtk_enable_t *pLeaky)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLeaky), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_VLAN_CTRLr, LONGAN_LEAK_ENf, pLeaky)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pLeaky=%d", *pLeaky);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_mcastLeakyEnable_set
 * Description:
 *      Set vlan egress leaky configure of the system to the specified device.
 * Input:
 *      unit  - unit id
 *      leaky - vlan leaky of egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Enable leaky function to allow L2/IP multicast traffic to across VLAN.
 *      That is, egress VLAN filtering is bypassed by L2/IP multicast traffic.
 */
int32
dal_longan_vlan_mcastLeakyEnable_set(uint32 unit, rtk_enable_t leaky)
{
    int32   ret = RT_ERR_FAILED;
    uint32  en_leaky;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, leaky=%d", unit, leaky);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((leaky >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (ENABLED == leaky)
    {
        en_leaky = 1;
    }
    else
    {
        en_leaky = 0;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, LONGAN_VLAN_CTRLr, LONGAN_LEAK_ENf, &en_leaky)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portPvidMode_get
 * Description:
 *      Get the configuration of  port-based VLAN mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      type - vlan type
 * Output:
 *      pMode - pointer to port-based VLAN mode configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Port-based VLAN can apply on different tag format, refer to rtk_vlan_pbVlan_mode_t
 */
int32
dal_longan_vlan_portPvidMode_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t *pMode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field,value;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pMode, RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_IPVID_FMTf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OPVID_FMTf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          LONGAN_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          field,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "ret=0x%x", ret);
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pMode = PBVLAN_MODE_UNTAG_AND_PRITAG;
            break;
        case 1:
            *pMode = PBVLAN_MODE_UNTAG_ONLY;
            break;
        case 2:
            *pMode = PBVLAN_MODE_ALL_PKT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pMode=%d", *pMode);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portPvidMode_set
 * Description:
 *      Set the configuration of port-based VLAN mode.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      mode - inner port-based VLAN mode configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Port-based VLAN can apply on different tag format, refer to rtk_vlan_pbVlan_mode_t
 */
int32
dal_longan_vlan_portPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t mode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field,value;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(mode >= PBVLAN_MODE_END, RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (mode)
    {
        case PBVLAN_MODE_UNTAG_AND_PRITAG:
            value = 0;
            break;
        case PBVLAN_MODE_UNTAG_ONLY:
            value = 1;
            break;
        case PBVLAN_MODE_ALL_PKT:
            value = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_IPVID_FMTf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OPVID_FMTf;
            break;
        default:
            return RT_ERR_FAILED;
    }
    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          field,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "ret=0x%x", ret);
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portPvid_get
 * Description:
 *      Get port default vlan id from the specified device.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type - vlan type
 * Output:
 *      pPvid  - pointer buffer of port default inner vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portPvid_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 *pPvid)
{
    int32 ret = RT_ERR_FAILED;
    uint32 field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_IPVIDf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OPVIDf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          LONGAN_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          field,
                          pPvid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "ret=0x%x", ret);
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pPvid=%d", *pPvid);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portPvid_get */


/* Function Name:
 *      dal_longan_vlan_portPvid_set
 * Description:
 *      Set port default vlan id to the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      pvid - port default inner vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
dal_longan_vlan_portPvid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 pvid)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, inner pvid=%d",
           unit, port, pvid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((pvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_IPVIDf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OPVIDf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          field,
                          &pvid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "ret=0x%x", ret);
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portPvid_set */

/* Function Name:
 *      dal_longan_vlan_protoGroup_get
 * Description:
 *      Get protocol group for protocol based vlan.
 * Input:
 *      unit            - unit id
 *      protoGroup_idx  - protocol group index
 * Output:
 *      pProtoGroup     - pointer to protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - protocol group index is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_protoGroup_get(
    uint32                  unit,
    uint32                  protoGroup_idx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, protoGroup_idx=%d", unit, protoGroup_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((protoGroup_idx > HAL_PROTOCOL_VLAN_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pProtoGroup), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          LONGAN_VLAN_PPB_VLAN_SETr,
                          REG_ARRAY_INDEX_NONE,
                          protoGroup_idx,
                          LONGAN_FRAME_TYPEf,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                         LONGAN_VLAN_PPB_VLAN_SETr,
                         REG_ARRAY_INDEX_NONE,
                         protoGroup_idx,
                         LONGAN_FRAME_TYPE_VALUEf,
                         &(pProtoGroup->framevalue))) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            pProtoGroup->frametype = FRAME_TYPE_ETHERNET;
            break;
        case 1:
            pProtoGroup->frametype = FRAME_TYPE_LLCOTHER;
            break;
        case 2:
            pProtoGroup->frametype = FRAME_TYPE_RFC1042;
            break;
        default:
            return FRAME_TYPE_END;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "frametype=%d, framevalue=%x", \
        pProtoGroup->frametype, pProtoGroup->framevalue);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_protoGroup_get */

/* Function Name:
 *      dal_longan_vlan_protoGroup_set
 * Description:
 *      Set protocol group for protocol based vlan.
 * Input:
 *      unit           - unit id
 *      protoGroup_idx - protocol group index
 *      protoGroup     - protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_OUT_OF_RANGE    - protocol group index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_VLAN_FRAME_TYPE - Error frame type
 * Note:
 *      None
 */
int32
dal_longan_vlan_protoGroup_set(
    uint32                  unit,
    uint32                  protoGroup_idx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, protoGroup_idx=%d, frametype=%d, framevalue=%x",\
        unit, protoGroup_idx,pProtoGroup->frametype, pProtoGroup->framevalue);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((protoGroup_idx > HAL_PROTOCOL_VLAN_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pProtoGroup), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pProtoGroup->frametype > RTK_ETHERTYPE_MAX), RT_ERR_VLAN_FRAME_TYPE);

    /* translate to chip's definition */
    switch (pProtoGroup->frametype)
    {
        case FRAME_TYPE_ETHERNET:
            value = 0;
            break;
        case FRAME_TYPE_LLCOTHER:
            value = 1;
            break;
        case FRAME_TYPE_RFC1042:
            value = 2;
            break;
        default:
            return RT_ERR_VLAN_FRAME_TYPE;
    }

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit,
                          LONGAN_VLAN_PPB_VLAN_SETr,
                          REG_ARRAY_INDEX_NONE,
                          protoGroup_idx,
                          LONGAN_FRAME_TYPEf,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          LONGAN_VLAN_PPB_VLAN_SETr,
                          REG_ARRAY_INDEX_NONE,
                          protoGroup_idx,
                          LONGAN_FRAME_TYPE_VALUEf,
                          &pProtoGroup->framevalue)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_protoGroup_set */

/* Function Name:
 *      dal_longan_vlan_portProtoVlan_get
 * Description:
 *      Get vlan of specificed protocol group on specified port.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      protoGroup_idx  - protocol group index
 * Output:
 *      pVlan_cfg       - pointer to vlan configuration of protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_OUT_OF_RANGE     - protocol group index is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portProtoVlan_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  protoGroup_idx,
    rtk_vlan_protoVlanCfg_t *pVlan_cfg)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, protoGroup_idx=%d", unit, port, protoGroup_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((protoGroup_idx > HAL_PROTOCOL_VLAN_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pVlan_cfg), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_VLAN_TYPEf, &pVlan_cfg->vlan_type)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_VID_ASf, &pVlan_cfg->vid_assign)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_VIDf, &pVlan_cfg->vid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_PRI_ASf, &pVlan_cfg->pri_assign)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_PRIf, &pVlan_cfg->pri)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pVlan_cfg->vlan_type=%d, pVlan_cfg->vid_assign=%d, pVlan_cfg->vid=%d, pVlan_cfg->pri_assign=%d, pVlan_cfg->pri",\
        pVlan_cfg->vlan_type, pVlan_cfg->vid_assign,pVlan_cfg->vid,pVlan_cfg->pri_assign,pVlan_cfg->pri);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portProtoVlan_get */

/* Function Name:
 *      dal_longan_vlan_portProtoVlan_set
 * Description:
 *      Set vlan of specificed protocol group on specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      protoGroup_idx - protocol group index
 *      pVlan_cfg      - vlan configuration of protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_OUT_OF_RANGE    - protocol group index is out of range
 *      RT_ERR_VLAN_VID        - invalid vlan id
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portProtoVlan_set(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  protoGroup_idx,
    rtk_vlan_protoVlanCfg_t *pVlan_cfg)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, protoGroup_idx=%d, pVlan_cfg->vlan_type=%d,\
        pVlan_cfg->vid_assign=%d, pVlan_cfg->vid=%d, pVlan_cfg->pri_assign=%d, pVlan_cfg->pri=%d",
        unit, port, protoGroup_idx, pVlan_cfg->vlan_type, pVlan_cfg->vid_assign,pVlan_cfg->vid,\
        pVlan_cfg->pri_assign,pVlan_cfg->pri);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((protoGroup_idx > HAL_PROTOCOL_VLAN_IDX_MAX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pVlan_cfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((VLAN_TYPE_END <= pVlan_cfg->vlan_type), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pVlan_cfg->vid_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((pVlan_cfg->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= pVlan_cfg->pri_assign), RT_ERR_INPUT);
    RT_PARAM_CHK(RTK_DOT1P_PRIORITY_MAX < pVlan_cfg->pri  , RT_ERR_QOS_1P_PRIORITY);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_VLAN_TYPEf, &pVlan_cfg->vlan_type)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_VID_ASf, &pVlan_cfg->vid_assign)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_VIDf, &pVlan_cfg->vid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_PRI_ASf, &pVlan_cfg->pri_assign)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_PPB_VLAN_SETr, port, protoGroup_idx, LONGAN_PRIf, &pVlan_cfg->pri)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portProtoVlan_set */

/* Function Name:
 *      dal_longan_vlan_portMacBasedVlanEnable_get
 * Description:
 *      Get enable status of MAC-based VLAN on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to enable status of MAC-based VLAN
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_longan_vlan_portMacBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                    LONGAN_VLAN_IVC_CTRLr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    LONGAN_MAC_VLAN_ENf,
                    pEnable)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* dal_longan_vlan_portMacBasedVlanEnable_get */

/* Function Name:
 *      dal_longan_vlan_portMacBasedVlanEable_set
 * Description:
 *      Set enable status of MAC-based VLAN on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of MAC-based VLAN
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
dal_longan_vlan_portMacBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit,
                    LONGAN_VLAN_IVC_CTRLr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    LONGAN_MAC_VLAN_ENf,
                    &enable)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* dal_longan_vlan_portMacBasedVlanEnable_set */

/* Function Name:
 *      dal_longan_vlan_macBasedVlanEntry_get
 * Description:
 *      Get MAC-based vlan.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pEntry    - pointer buffer of mac based vlan entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to dal_longan_vlan_igrVlanCnvtBlkMode_set.
 */
int32
dal_longan_vlan_macBasedVlanEntry_get(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry)
{
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_ERR_HDL(dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_MAC_BASED, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pEntry, RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if (RT_ERR_OK != __dal_longan_vlan_macBasedVlanEntry_get(unit, index, pEntry))
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      rtk_vlan_macBasedVlanEntry_set
 * Description:
 *      Set MAC-based vlan.
 * Input:
 *      unit     - unit id
 *      index    - entry index
 * Output:
 *      pEntry    -  pointer buffer of Mac-based VLAN entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_QOS_1P_PRIORITY         - invalid priority value
*       RT_ERR_VLAN_TPID_INDEX        - invalid tpid index
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to dal_longan_vlan_igrVlanCnvtBlkMode_set.
 */
int32
dal_longan_vlan_macBasedVlanEntry_set(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry)
{
    int32   ret = RT_ERR_FAILED;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d",unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_ERR_HDL(dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_MAC_BASED, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->port_care),RT_ERR_INPUT);
    RT_PARAM_CHK((1 < pEntry->port_type), RT_ERR_INPUT);
    RT_PARAM_CHK(((pEntry->port_care != 0) && (0 == pEntry->port_type) && !HWP_PORT_EXIST(unit, pEntry->port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK(((pEntry->port_care != 0) && (0 != pEntry->port_type) && (pEntry->port >= HAL_MAX_NUM_OF_TRUNK(unit))), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ivid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ovid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_COPY2MASTERCPU < pEntry->fwd_action),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->igrvlanfilter_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TYPE_END <= pEntry->vlan_type),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->pri_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pEntry->pri_new), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->vid_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vid_new > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pEntry->tpid_action),RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_CVLAN_TPID(unit) <= pEntry->tpid_idx), RT_ERR_VLAN_TPID_INDEX);

    /* programming entry to chip */
    VLAN_SEM_LOCK(unit);
    if (RT_ERR_OK != __dal_longan_vlan_macBasedVlanEntry_set(unit, index, pEntry))
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      dal_longan_vlan_macBasedVlanEntry_add
 * Description:
 *      add a mac-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of Mac-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_TBL_FULL             - the table is full
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Note:
 *      1. Cannot update the entry
 */
int32
dal_longan_vlan_macBasedVlanEntry_add(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    uint32 index = 0, i = 0;
    rtk_vlan_macVlanEntry_t macEntry;

    osal_memset(&macEntry, 0, sizeof(rtk_vlan_macVlanEntry_t));

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pEntry->pri_new), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((pEntry->vid_new > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    VLAN_SEM_LOCK(unit);

    if (RT_ERR_OK == _dal_longan_vlan_macEntry_find(unit, pEntry, &index))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errHandle;
    }

    macEntry.valid = TRUE;
    osal_memcpy(&macEntry.mac, &pEntry->mac, sizeof(rtk_mac_t));
    for (i = 0; i < ETHER_ADDR_LEN; i++)
        macEntry.mac_care.octet[i] = 0xff;
    macEntry.pri_assign = ENABLED;
    macEntry.pri_new = pEntry->pri_new;
    macEntry.vid_assign = ENABLED;
    macEntry.vid_new = pEntry->vid_new;

    RT_ERR_HDL(_dal_longan_vlan_macEntry_alloc(unit, &index), errHandle, ret);

    RT_ERR_HDL(__dal_longan_vlan_macBasedVlanEntry_set(unit, index, &macEntry), errSet, ret);

    VLAN_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errSet:
    ret = _dal_longan_vlan_macEntry_free(unit, index);
    if (RT_ERR_OK != ret)
    {
        goto errHandle;
    }

errHandle:
    VLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_vlan_macBasedVlanEntry_add */

/* Function Name:
 *      dal_longan_vlan_macBasedVlanEntry_del
 * Description:
 *      delete a mac-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of Mac-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Note:
 *      None
 */
int32
dal_longan_vlan_macBasedVlanEntry_del(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    uint32 index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if (RT_ERR_OK != _dal_longan_vlan_macEntry_find(unit, pEntry, &index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    RT_ERR_HDL(_dal_longan_vlan_macEntry_free(unit, index), errHandle, ret);

errHandle:
    VLAN_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_longan_vlan_macBasedVlanEntry_del */

/* Function Name:
 *      dal_longan_vlan_portIpSubnetBasedVlanEnable_get
 * Description:
 *      Get enable status of IPSubet-based VLAN on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable   - pointer to enable status of IPSubet-based VLAN
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
dal_longan_vlan_portIpSubnetBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                    LONGAN_VLAN_IVC_CTRLr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    LONGAN_IP_VLAN_ENf,
                    pEnable)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portIpSubnetBasedVlanEnable_set
 * Description:
 *      Set enable status of IPSubet-based VLAN on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of IPSubet-based VLAN
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
dal_longan_vlan_portIpSubnetBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit,
                    LONGAN_VLAN_IVC_CTRLr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    LONGAN_IP_VLAN_ENf,
                    &enable)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_ipSubnetBasedVlanEntry_get
 * Description:
 *      Get IP-Subnet-based vlan.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pEntry    -  pointer buffer of ipSubnet-based VLAN entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to dal_longan_vlan_igrVlanCnvtBlkMode_set.
 */
int32
dal_longan_vlan_ipSubnetBasedVlanEntry_get(uint32 unit, uint32 index, rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    int32   ret = RT_ERR_FAILED;
    vlan_igrcnvt_entry_t cnvt_entry;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;
    uint32 temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_ERR_HDL(dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_IP_SUBNET_BASED, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pEntry, RT_ERR_NULL_POINTER);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*** get entry from chip ***/
    VLAN_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_VLAN_IP_BASEDt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /*Search Key*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VALIDtf,\
        &pEntry->valid, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PORT_TYPEtf,\
        &pEntry->port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PORT_IDtf,\
        &pEntry->port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_PORT_IDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x3F)
        pEntry->port_care = 1;
    else
        pEntry->port_care = 0;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_IVID_VALIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

    if(0 == temp_var)
        pEntry->ivid_aft = ACCEPT_FRAME_TYPE_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_IVID_VALIDtf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(0 == temp_var)
            pEntry->ivid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
        else
            pEntry->ivid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_OVID_VALIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(0 == temp_var)
        pEntry->ovid_aft = ACCEPT_FRAME_TYPE_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_OVID_VALIDtf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(0 == temp_var)
            pEntry->ovid_aft = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
        else
            pEntry->ovid_aft = ACCEPT_FRAME_TYPE_TAG_ONLY;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_SIPtf,\
        &pEntry->sip, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_SIPtf,\
        &pEntry->sip_care, (uint32 *) &cnvt_entry), errHandle, ret);

    /*Action*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_FWD_ACTtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pEntry->fwd_action = ACTION_DROP;
            break;
        case 1:
            pEntry->fwd_action = ACTION_FORWARD;
            break;
        case 2:
            pEntry->fwd_action = ACTION_TRAP2CPU;
            break;
        case 3:
            pEntry->fwd_action = ACTION_COPY2CPU;
            break;
        case 4:
            pEntry->fwd_action = ACTION_TRAP2MASTERCPU;
            break;
        case 5:
            pEntry->fwd_action = ACTION_COPY2MASTERCPU;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_IGNORE_IGR_FLTRtf,\
        &pEntry->igrvlanfilter_ignore, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VLAN_TYPEtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(0 == temp_var)
        pEntry->vlan_type = INNER_VLAN;
    else if ( 1 == temp_var)
        pEntry->vlan_type = OUTER_VLAN;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TPID_ASSIGNtf,\
        &pEntry->tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TPID_IDXtf,\
        &pEntry->tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TAG_STS_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(0 == temp_var)
        pEntry->sts_action = VLAN_TAG_STATUS_ACT_NONE;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TAG_STStf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
        if(0 == temp_var)
            pEntry->sts_action = VLAN_TAG_STATUS_ACT_UNTAGGED;
        else
            pEntry->sts_action = VLAN_TAG_STATUS_ACT_TAGGED;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PRI_ASSIGNtf,\
        &pEntry->pri_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PRItf,\
        &pEntry->pri_new, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VID_ASSIGNtf,\
        &pEntry->vid_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VIDtf,\
        &pEntry->vid_new, (uint32 *) &cnvt_entry), errHandle, ret);

    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      dal_longan_vlan_ipSubnetBasedVlanEntry_set
 * Description:
 *      Set IP-Subnet-based vlan.
 * Input:
 *      unit     - unit id
 *      index    - entry index
 *      pEntry    -  pointer buffer of ipSubnet-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_QOS_1P_PRIORITY             - invalid priority value
 *      RT_ERR_VLAN_TPID_INDEX            - invalid tipid index
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to dal_longan_vlan_igrVlanCnvtBlkMode_set.
 */
int32
dal_longan_vlan_ipSubnetBasedVlanEntry_set(uint32 unit, uint32 index, rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    int32   ret = RT_ERR_FAILED;
    vlan_igrcnvt_entry_t cnvt_entry;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;
    uint32  port_mask,portType_mask;
    uint32  temp_var1,temp_var2;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d",unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_ERR_HDL(dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_IP_SUBNET_BASED, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->port_care),RT_ERR_INPUT);
    RT_PARAM_CHK((1 < pEntry->port_type), RT_ERR_INPUT);
    RT_PARAM_CHK(((pEntry->port_care != 0) && (0 == pEntry->port_type) && !HWP_PORT_EXIST(unit, pEntry->port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK(((pEntry->port_care != 0) && (0 != pEntry->port_type) && (pEntry->port >= HAL_MAX_NUM_OF_TRUNK(unit))), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ivid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ovid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_COPY2MASTERCPU < pEntry->fwd_action),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->igrvlanfilter_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TYPE_END <= pEntry->vlan_type),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->pri_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pEntry->pri_new), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->vid_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vid_new > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pEntry->tpid_action),RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_CVLAN_TPID(unit) <= pEntry->tpid_idx), RT_ERR_VLAN_TPID_INDEX);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VALIDtf,\
        &pEntry->valid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_SIPtf,\
        &pEntry->sip, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_SIPtf,\
        &pEntry->sip_care, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pEntry->port_care != 0)
    {
        port_mask = 0x7F;
        portType_mask = 0x1;
    }
    else
    {
        port_mask = 0;
        portType_mask = 0;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PORT_IDtf,\
        &pEntry->port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_PORT_IDtf,\
        &port_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PORT_TYPEtf,\
        &pEntry->port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_PORT_TYPEtf,\
        &portType_mask, (uint32 *) &cnvt_entry), errHandle, ret);


    switch(pEntry->ivid_aft)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            temp_var1 = 0;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_IVID_VALIDtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_IVID_VALIDtf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pEntry->ovid_aft)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            temp_var1 = 0;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_OVID_VALIDtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_BMSK_OVID_VALIDtf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pEntry->fwd_action)
    {
        case ACTION_DROP:
            temp_var1 = 0;
            break;
        case ACTION_FORWARD:
            temp_var1 = 1;
            break;
        case ACTION_TRAP2CPU:
            temp_var1 = 2;
            break;
        case ACTION_COPY2CPU:
            temp_var1 = 3;
            break;
        case ACTION_TRAP2MASTERCPU:
            temp_var1 = 4;
            break;
        case ACTION_COPY2MASTERCPU:
            temp_var1 = 5;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_MAC_BASEDt, LONGAN_VLAN_IP_BASED_FWD_ACTtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_IGNORE_IGR_FLTRtf,\
        &pEntry->igrvlanfilter_ignore, (uint32 *) &cnvt_entry), errHandle, ret);

    temp_var1 = (pEntry->vlan_type==INNER_VLAN) ? 0 : 1;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VLAN_TYPEtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pEntry->sts_action)
    {
        case VLAN_TAG_STATUS_ACT_NONE:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_UNTAGGED:
            temp_var1 = 1;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_TAGGED:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TAG_STS_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TAG_STStf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TPID_ASSIGNtf,\
        &pEntry->tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_TPID_IDXtf,\
        &pEntry->tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PRI_ASSIGNtf,\
        &pEntry->pri_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_PRItf,\
        &pEntry->pri_new, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VID_ASSIGNtf,\
        &pEntry->vid_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IP_BASEDt, LONGAN_VLAN_IP_BASED_VIDtf,\
        &pEntry->vid_new, (uint32 *) &cnvt_entry), errHandle, ret);

    /* programming entry to chip */
    VLAN_SEM_LOCK(unit);
    if ((ret = table_write(unit, LONGAN_VLAN_IP_BASEDt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      dal_longan_vlan_ipSubnetBasedVlanEntry_add
 * Description:
 *      add an IP-subnet-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of ipSubnet-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_TBL_FULL - the table is full
 *      RT_ERR_MAC      - invalid mac address
 *      RT_ERR_VLAN_VID - invalid vlan id
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      1. Cannot update the entry
 *      2. Valid bit will be always true
 *      3. Allow valid netmask only
 */
int32
dal_longan_vlan_ipSubnetBasedVlanEntry_add(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    uint32 pfLen = 0, index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ivid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACCEPT_FRAME_TYPE_END <= pEntry->ovid_aft),RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_COPY2MASTERCPU < pEntry->fwd_action),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->igrvlanfilter_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TYPE_END <= pEntry->vlan_type),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->pri_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pEntry->pri_new), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->vid_assign), RT_ERR_INPUT);
    RT_PARAM_CHK((pEntry->vid_new > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pEntry->tpid_action),RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_CVLAN_TPID(unit) <= pEntry->tpid_idx), RT_ERR_VLAN_TPID_INDEX);

    // valid netmask check
    if (RT_ERR_OK != _dal_longan_vlan_maskValid_check(pEntry->sip_care))
        return RT_ERR_INPUT;

    // assign back to ip from (ip & netmask)
    pEntry->sip = (pEntry->sip & pEntry->sip_care);

    if (RT_ERR_OK == _dal_longan_vlan_ipSubEntry_find(unit, pEntry, &index))
    {
        ret = RT_ERR_ENTRY_EXIST;
        goto errHandle;
    }

    pEntry->valid = TRUE;

    pfLen = rt_util_ipMask2Length_ret(pEntry->sip_care);
    RT_ERR_HDL(_dal_longan_vlan_ipSubEntry_alloc(unit, pfLen, &index), errHandle, ret);

    RT_ERR_HDL(dal_longan_vlan_ipSubnetBasedVlanEntry_set(unit, index, pEntry), errHandle, ret);

errHandle:
    return ret;
}   /* end of dal_longan_vlan_ipSubnetBasedVlanEntry_add */

/* Function Name:
 *      dal_longan_vlan_ipSubnetBasedVlanEntry_del
 * Description:
 *      delete an IP-subnet-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of Mac-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
dal_longan_vlan_ipSubnetBasedVlanEntry_del(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    int32 ret = RT_ERR_OK;
    uint32 pfLen = 0, index = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    // valid netmask check
    if (RT_ERR_OK != _dal_longan_vlan_maskValid_check(pEntry->sip_care))
        return RT_ERR_INPUT;

    // assign back to ip from (ip & netmask)
    pEntry->sip = (pEntry->sip & pEntry->sip_care);

    if (RT_ERR_OK != _dal_longan_vlan_ipSubEntry_find(unit, pEntry, &index))
    {
        ret = RT_ERR_ENTRY_NOTFOUND;
        goto errHandle;
    }

    pfLen = rt_util_ipMask2Length_ret(pEntry->sip_care);
    RT_ERR_HDL(_dal_longan_vlan_ipSubEntry_free(unit, pfLen, index), errHandle, ret);

errHandle:
    return ret;
}   /* end of dal_longan_vlan_ipSubnetBasedVlanEntry_del */

/* Function Name:
 *      dal_longan_vlan_portIgrVlanTransparentEnable_get
 * Description:
 *      Get enable status of keep tag format at ingress.
 * Input:
 *      unit       - unit id
 *      port      - port id
 *      type      - vlan type
 * Output:
 *      pEnable - enable status of keep tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointers
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_longan_vlan_portIgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value, field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d", unit, port,type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= VLAN_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_IGR_ITAG_KEEPf;
            break;
        case OUTER_VLAN:
            field = LONGAN_IGR_OTAG_KEEPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_VLAN_PORT_TAG_STS_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          field,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pEnable = DISABLED;
            break;
        case 1:
            *pEnable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=%s", (*pEnable)?"ENABLED":"DISABLED");

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portIgrTagKeepEnable_get */

/* Function Name:
 *      dal_longan_vlan_portIgrVlanTransparentEnable_set
 * Description:
 *      Set enable status of keep tag format at ingress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      type      - vlan type
 *      enable  -  enable
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
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_longan_vlan_portIgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d, enable=%d",unit, port, type, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= VLAN_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (enable)
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

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_IGR_ITAG_KEEPf;
            break;
        case OUTER_VLAN:
            field = LONGAN_IGR_OTAG_KEEPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_VLAN_PORT_TAG_STS_CTRLr,
                          port, REG_ARRAY_INDEX_NONE,
                          field,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portIgrVlanTransparentEnable_set */

/* Function Name:
 *      dal_longan_vlan_portEgrVlanTransparentEnable_get
 * Description:
 *      Get enable status of keep tag format at egress.
 * Input:
 *      unit       - unit id
 *      port      - port id
 *      type      - vlan type
 * Output:
 *      pEnable - enable status of keep tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointers
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_longan_vlan_portEgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value, field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d", unit, port,type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= VLAN_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_EGR_ITAG_KEEPf;
            break;
        case OUTER_VLAN:
            field = LONGAN_EGR_OTAG_KEEPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_VLAN_PORT_TAG_STS_CTRLr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          field,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pEnable = DISABLED;
            break;
        case 1:
            *pEnable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=%s", (*pEnable)?"ENABLED":"DISABLED");

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portEgrTagKeepEnable_get */

/* Function Name:
 *      dal_longan_vlan_portEgrVlanTransparentEnable_set
 * Description:
 *      Set enable status of keep tag format at egress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      type      - vlan type
 *      enable  -  enable
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
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_longan_vlan_portEgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d, enable=%d",unit, port, type, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((type >= VLAN_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (enable)
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

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_EGR_ITAG_KEEPf;
            break;
        case OUTER_VLAN:
            field = LONGAN_EGR_OTAG_KEEPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_VLAN_PORT_TAG_STS_CTRLr,
                          port, REG_ARRAY_INDEX_NONE,
                          field,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portEgrTagKeepEnable_set */

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtBlkMode_get
 * Description:
 *      Get the operation mode of ingress VLAN conversion table block.
 * Input:
 *      unit    - unit id
 *      blk_idx - block index
 * Output:
 *      pMode   - operation mode of ingress VLAN conversion block
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. Valid block index ranges from 0 to 7.
 *      2. Ingress VLAN conversion table block can be used for doing ingress VLAN conversion
 *         or MAC-basd VLAN or IP-Subnet-based VLAN.
 */
int32
dal_longan_vlan_igrVlanCnvtBlkMode_get(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t *pMode)
{
    int32   ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((blk_idx >= HAL_MAX_NUM_OF_C2SC_BLK(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(NULL == pMode, RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = _dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, blk_idx, pMode)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtBlkMode_set
 * Description:
 *      Get the operation mode of ingress VLAN conversion table block.
 * Input:
 *      unit    - unit id
 *      blk_idx - block index
 *      mode    - operation mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      1. Valid block index ranges from 0 to 7.
 *      2. Ingress VLAN conversion table block can be used for doing ingress VLAN conversion
 *         or MAC-basd VLAN or IP-Subnet-based VLAN.
 *
 *      Limitation for block mode set (Using rtk_vlan_ipSubnetBasedVlanEntry_add /
 *          rtk_vlan_ipSubnetBasedVlanEntry_delete / rtk_vlan_macBasedVlanEntry_add /
 *          rtk_vlan_macBasedVlanEntry_delete):
 *      (a) Cannot modify an in-used block mode
 *      (b) Must allocate a sequential block for IVC / MAC-based / IP-Subnet-based VLAN
 */
int32
dal_longan_vlan_igrVlanCnvtBlkMode_set(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t mode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value, ori_val;
    uint32  i, start, end;
    vlan_igrcnvt_entry_t data;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((blk_idx >= HAL_MAX_NUM_OF_C2SC_BLK(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((mode >= CONVERSION_MODE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (mode)
    {
        case CONVERSION_MODE_C2SC:
            value = 0;
            break;
        case CONVERSION_MODE_MAC_BASED:
            value = 1;
            break;
        case CONVERSION_MODE_IP_SUBNET_BASED:
            value = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_IVC_BLK_CTRLr, REG_ARRAY_INDEX_NONE, blk_idx, LONGAN_MODEf, &ori_val)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if (ori_val == value)
    {
        VLAN_SEM_UNLOCK(unit);
        return RT_ERR_OK;
    }

    osal_memset(&data, 0, sizeof(vlan_igrcnvt_entry_t));

    start = HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit) * blk_idx;
    end = start + HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit);
    for (i = start; i < end; ++i)
    {
        if ((ret = table_write(unit, LONGAN_VLAN_IGR_CNVTt, i, (uint32 *) &data)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }

    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_IVC_BLK_CTRLr, REG_ARRAY_INDEX_NONE, blk_idx, LONGAN_MODEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    _dal_longan_vlan_tblDb_update(unit);

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtEntry_get
 * Description:
 *      Get ingress VLAN conversion (C2SC) entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of ingress VLAN conversion (C2SC) entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - entry index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_igrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData)
{
    int32   ret = RT_ERR_FAILED;
    vlan_igrcnvt_entry_t cnvt_entry;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;
    uint32 temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit)), RT_ERR_OUT_OF_RANGE);
    RT_ERR_HDL(dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_C2SC, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*** get entry from chip ***/
    VLAN_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_VLAN_IGR_CNVTt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);
    /* search key*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_VALIDtf,\
        &pData->valid, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_ITAG_IFtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_ITAG_IFtf,\
            &pData->match_key.inner_tag_sts, (uint32 *) &cnvt_entry), errHandle, ret);
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_OTAG_IFtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_OTAG_IFtf,\
            &pData->match_key.outer_tag_sts, (uint32 *) &cnvt_entry), errHandle, ret);
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_IVIDtf,\
        &pData->match_key.inner_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_IVIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0xFFF)
        pData->match_key.inner_vid_ignore = 0;
    else
        pData->match_key.inner_vid_ignore = 1;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_OVIDtf,\
        &pData->match_key.outer_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_OVIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0xFFF)
        pData->match_key.outer_vid_ignore = 0;
    else
        pData->match_key.outer_vid_ignore = 1;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_IPRItf,\
        &pData->match_key.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_IPRItf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x7)
        pData->match_key.inner_priority_ignore = 0;
    else
        pData->match_key.inner_priority_ignore = 1;
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_OPRItf,\
        &pData->match_key.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_OPRItf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x7)
        pData->match_key.outer_priority_ignore = 0;
    else
        pData->match_key.outer_priority_ignore = 1;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_PORT_TYPEtf,\
        &pData->match_key.port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_PORT_IDtf,\
        &pData->match_key.port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_PORT_IDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x3F)
        pData->match_key.port_ignore = 0;
    else
        pData->match_key.port_ignore = 1;

    /*action*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITAG_STS_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.inner_tag_sts = VLAN_TAG_STATUS_ACT_NONE;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITAG_STStf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(temp_var == 0)
            pData->action.inner_tag_sts = VLAN_TAG_STATUS_ACT_UNTAGGED;
        else
            pData->action.inner_tag_sts = VLAN_TAG_STATUS_ACT_TAGGED;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTAG_STS_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.outer_tag_sts = VLAN_TAG_STATUS_ACT_NONE;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTAG_STStf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(temp_var == 0)
            pData->action.outer_tag_sts = VLAN_TAG_STATUS_ACT_UNTAGGED;
        else
            pData->action.outer_tag_sts = VLAN_TAG_STATUS_ACT_TAGGED;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IVID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        case 1:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 2:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 3:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_OUTER;
            break;
        default:
            return VLAN_TAG_VID_ACT_NONE;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IVIDtf,\
        &pData->action.inner_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OVID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        case 1:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 2:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 3:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_INNER;
            break;
        default:
            return VLAN_TAG_VID_ACT_NONE;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OVIDtf,\
        &pData->action.outer_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IPRI_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pData->action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;
            break;
        case 1:
            pData->action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
            break;
        default:
            return VLAN_TAG_PRIORITY_ACT_NONE;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IPRItf,\
        &pData->action.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OPRI_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pData->action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;
            break;
        case 1:
            pData->action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
            break;
        case 2:
            pData->action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_COPY_FROM_INNER;
            break;
        default:
            return VLAN_TAG_PRIORITY_ACT_NONE;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OPRItf,\
        &pData->action.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITPID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.inner_tpid_action = VLAN_TAG_TPID_ACT_NONE;
    else
        pData->action.inner_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITPID_IDXtf,\
        &pData->action.inner_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTPID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.outer_tpid_action = VLAN_TAG_TPID_ACT_NONE;
    else
        pData->action.outer_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTPID_IDXtf,\
        &pData->action.outer_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
} /* end of dal_longan_vlan_igrVlanCnvtEntry_get */

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtEntry_set
 * Description:
 *      Set ingress VLAN conversion (C2SC) entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of ingress VLAN conversion (C2SC) entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OUT_OF_RANGE         - entry index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to add a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_QOS_1P_PRIORITY      - Invalid 802.1p priority
 *      RT_ERR_VLAN_TPID_INDEX      - Invalid TPID index
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_vlan_igrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData)
{
    int32   ret = RT_ERR_FAILED;
    vlan_igrcnvt_entry_t cnvt_entry;
    rtk_vlan_igrVlanCnvtBlk_mode_t blk_mode;
    uint32 care_tagSts,vid_mask,pri_mask,port_mask,portType_mask;
    uint32 temp_var1,temp_var2,tagSts = 0;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_C2SC_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_ERR_HDL(dal_longan_vlan_igrVlanCnvtBlkMode_get(unit, index/HAL_MAX_NUM_OF_C2SC_BLK_ENTRY(unit),\
        &blk_mode), errHandle, ret);
    RT_PARAM_CHK(blk_mode != CONVERSION_MODE_C2SC, RT_ERR_VLAN_C2SC_BLOCK_MODE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pData->valid != 0 && pData->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_CNVT_TAG_STS_END <= pData->match_key.inner_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_CNVT_TAG_STS_END <= pData->match_key.outer_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.inner_vid_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.outer_vid_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((pData->match_key.inner_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pData->match_key.outer_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.inner_priority_ignore), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.outer_priority_ignore), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->match_key.inner_priority), RT_ERR_VLAN_PRIORITY);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->match_key.outer_priority), RT_ERR_VLAN_PRIORITY);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.port_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((1 < pData->match_key.port_type), RT_ERR_INPUT);
    RT_PARAM_CHK(((pData->match_key.port_ignore != 1) && (pData->match_key.port_type == 0) && !HWP_PORT_EXIST (unit, pData->match_key.port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK(((pData->match_key.port_ignore != 1) && (pData->match_key.port_type != 0) && (pData->match_key.port >= HAL_MAX_NUM_OF_TRUNK(unit))), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((VLAN_TAG_STATUS_ACT_END <= pData->action.inner_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_STATUS_ACT_END <= pData->action.outer_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_VID_ACT_END <= pData->action.inner_vid_action), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_VID_ACT_END <= pData->action.outer_vid_action), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->action.inner_vid_value > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pData->action.outer_vid_value > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_TAG_PRIORITY_ACT_END <= pData->action.inner_priority_action), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_PRIORITY_ACT_END <= pData->action.outer_priority_action), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->action.inner_priority), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->action.outer_priority), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pData->action.inner_tpid_action), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pData->action.outer_tpid_action), RT_ERR_INPUT);
    if (VLAN_TAG_TPID_ACT_ASSIGN == pData->action.inner_tpid_action)
            RT_PARAM_CHK(pData->action.inner_tpid_idx>= HAL_MAX_NUM_OF_CVLAN_TPID(unit), RT_ERR_VLAN_TPID_INDEX);
    if (VLAN_TAG_TPID_ACT_ASSIGN == pData->action.outer_tpid_action)
            RT_PARAM_CHK(pData->action.outer_tpid_idx>= HAL_MAX_NUM_OF_SVLAN_TPID(unit), RT_ERR_VLAN_TPID_INDEX);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*search key*/
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_VALIDtf,\
        &pData->valid, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->match_key.inner_tag_sts)
    {
        case VLAN_CNVT_TAG_STS_UNTAGGED:
            care_tagSts = 0x1;
            tagSts = 0;
            break;
        case VLAN_CNVT_TAG_STS_TAGGED:
            care_tagSts = 0x1;
            tagSts = 1;
            break;
        case VLAN_CNVT_TAG_STS_ALL:
            care_tagSts = 0x0;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_ITAG_IFtf,\
        &tagSts, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_ITAG_IFtf,\
        &care_tagSts, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->match_key.outer_tag_sts)
    {
        case VLAN_CNVT_TAG_STS_UNTAGGED:
            care_tagSts = 0x1;
            tagSts = 0;
            break;
        case VLAN_CNVT_TAG_STS_TAGGED:
            care_tagSts = 0x1;
            tagSts = 1;
            break;
        case VLAN_CNVT_TAG_STS_ALL:
            care_tagSts = 0x0;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_OTAG_IFtf,\
        &tagSts, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_OTAG_IFtf,\
        &care_tagSts, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.inner_vid_ignore == 1)
        vid_mask = 0;
    else
        vid_mask = 0xFFF;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_IVIDtf,\
        &pData->match_key.inner_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_IVIDtf,\
        &vid_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.outer_vid_ignore == 1)
        vid_mask = 0;
    else
        vid_mask = 0xFFF;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_OVIDtf,\
        &pData->match_key.outer_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_OVIDtf,\
        &vid_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.inner_priority_ignore== 1)
        pri_mask = 0;
    else
        pri_mask = 0x7;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_IPRItf,\
        &pData->match_key.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_IPRItf,\
        &pri_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.outer_priority_ignore== 1)
        pri_mask = 0;
    else
        pri_mask = 0x7;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ORI_OPRItf,\
        &pData->match_key.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_ORI_OPRItf,\
        &pri_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.port_ignore == 1)
    {
        port_mask = 0;
        portType_mask = 0;
    }
    else
    {
        port_mask = 0x3F;
        portType_mask = 0x1;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_PORT_IDtf,\
        &pData->match_key.port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_PORT_IDtf,\
        &port_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_PORT_TYPEtf,\
        &pData->match_key.port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_BMSK_PORT_TYPEtf,\
        &portType_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    /*action*/
    switch(pData->action.inner_tag_sts)
    {
        case VLAN_TAG_STATUS_ACT_NONE:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_UNTAGGED:
            temp_var1 = 1;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_TAGGED:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITAG_STS_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITAG_STStf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.outer_tag_sts)
    {
        case VLAN_TAG_STATUS_ACT_NONE:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_UNTAGGED:
            temp_var1 = 1;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_TAGGED:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTAG_STS_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTAG_STStf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);


    switch(pData->action.inner_vid_action)
    {
        case VLAN_TAG_VID_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_VID_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        case VLAN_TAG_VID_ACT_SHIFT:
            temp_var1 = 2;
            break;
        case VLAN_TAG_VID_ACT_COPY_FROM_OUTER:
            temp_var1 = 3;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IVID_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IVIDtf,\
        &pData->action.inner_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.outer_vid_action)
    {
        case VLAN_TAG_VID_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_VID_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        case VLAN_TAG_VID_ACT_SHIFT:
            temp_var1 = 2;
            break;
        case VLAN_TAG_VID_ACT_COPY_FROM_INNER:
            temp_var1 = 3;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OVID_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OVIDtf,\
        &pData->action.outer_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.inner_priority_action)
    {
        case VLAN_TAG_PRIORITY_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_PRIORITY_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IPRI_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_IPRItf,\
        &pData->action.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.outer_priority_action)
    {
        case VLAN_TAG_PRIORITY_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_PRIORITY_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        case VLAN_TAG_PRIORITY_ACT_COPY_FROM_INNER:
            temp_var1 = 2;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OPRI_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OPRItf,\
        &pData->action.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);


    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITPID_ASSIGNtf,\
        &pData->action.inner_tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_ITPID_IDXtf,\
        &pData->action.inner_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTPID_ASSIGNtf,\
        &pData->action.outer_tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_IGR_CNVTt, LONGAN_VLAN_IGR_CNVT_OTPID_IDXtf,\
        &pData->action.outer_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    /* programming entry to chip */
    VLAN_SEM_LOCK(unit);
    if ((ret = table_write(unit, LONGAN_VLAN_IGR_CNVTt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
} /* end of dal_longan_vlan_igrVlanCnvtEntry_set */

/* Function Name:
 *      dal_longan_vlan_portIgrVlanCnvtEnable_get
 * Description:
 *      Get enable status of ingress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable   - pointer to enable status of ingress VLAN conversion
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
dal_longan_vlan_portIgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    VLAN_SEM_LOCK(unit);
    if((ret = reg_array_field_read(unit, LONGAN_VLAN_IVC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_IVC_ENf, pEnable)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_vlan_portIgrVlanCnvtEnable_get */

/* Function Name:
 *      dal_longan_vlan_portIgrVlanCnvtEnable_set
 * Description:
 *      Set enable status of ingress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of ingress VLAN conversion
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
dal_longan_vlan_portIgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d,port=%d,enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    VLAN_SEM_LOCK(unit);
    if((ret = reg_array_field_write(unit, LONGAN_VLAN_IVC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_IVC_ENf, &enable)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_vlan_portIgrVlanCnvtEnable_set */

/* Function Name:
 *      dal_longan_vlan_egrVlanCnvtEntry_get
 * Description:
 *      Get egress VLAN conversion (SC2C) entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion (SC2C) entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - entry index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_egrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData)
{
    int32   ret = RT_ERR_FAILED;
    vlan_egrcnvt_entry_t cnvt_entry;
    uint32  temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_SC2C_ENTRY(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*** get entry from chip ***/
    VLAN_SEM_LOCK(unit);
    if ((ret = table_read(unit, LONGAN_VLAN_EGR_CNVTt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

     /* search key*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_VALIDtf,\
        &pData->valid, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_ITAG_IFtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->match_key.inner_tag_sts = VLAN_CNVT_TAG_STS_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_ITAG_IFtf,\
            &pData->match_key.inner_tag_sts, (uint32 *) &cnvt_entry), errHandle, ret);
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_OTAG_IFtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->match_key.outer_tag_sts = VLAN_CNVT_TAG_STS_ALL;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_OTAG_IFtf,\
            &pData->match_key.outer_tag_sts, (uint32 *) &cnvt_entry), errHandle, ret);
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_IVIDtf,\
        &pData->match_key.inner_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_IVIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0xFFF)
        pData->match_key.inner_vid_ignore = 0;
    else
        pData->match_key.inner_vid_ignore = 1;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_OVIDtf,\
        &pData->match_key.outer_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_OVIDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0xFFF)
        pData->match_key.outer_vid_ignore = 0;
    else
        pData->match_key.outer_vid_ignore = 1;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_IPRItf,\
        &pData->match_key.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_IPRItf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x7)
        pData->match_key.inner_priority_ignore = 0;
    else
        pData->match_key.inner_priority_ignore = 1;
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_OPRItf,\
        &pData->match_key.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_OPRItf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x7)
        pData->match_key.outer_priority_ignore = 0;
    else
        pData->match_key.outer_priority_ignore = 1;

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_PORT_TYPEtf,\
        &pData->match_key.port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_PORT_IDtf,\
        &pData->match_key.port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_PORT_IDtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x3F)
        pData->match_key.port_ignore = 0;
    else
        pData->match_key.port_ignore = 1;

    /*action*/
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITAG_STS_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.inner_tag_sts = VLAN_TAG_STATUS_ACT_NONE;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITAG_STStf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(temp_var == 0)
            pData->action.inner_tag_sts = VLAN_TAG_STATUS_ACT_UNTAGGED;
        else
            pData->action.inner_tag_sts = VLAN_TAG_STATUS_ACT_TAGGED;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTAG_STS_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.outer_tag_sts = VLAN_TAG_STATUS_ACT_NONE;
    else
    {
        RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTAG_STStf,\
            &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

        if(temp_var == 0)
            pData->action.outer_tag_sts = VLAN_TAG_STATUS_ACT_UNTAGGED;
        else
            pData->action.outer_tag_sts = VLAN_TAG_STATUS_ACT_TAGGED;
    }

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IVID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        case 1:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 2:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 3:
            pData->action.inner_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_OUTER;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IVIDtf,\
        &pData->action.inner_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OVID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_NONE;
            break;
        case 1:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_ASSIGN;
            break;
        case 2:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_SHIFT;
            break;
        case 3:
            pData->action.outer_vid_action = VLAN_TAG_VID_ACT_COPY_FROM_INNER;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OVIDtf,\
        &pData->action.outer_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IPRI_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(temp_var)
    {
        case 0:
            pData->action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;
            break;
        case 1:
            pData->action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
            break;
        case 2:
            pData->action.inner_priority_action = VLAN_TAG_PRIORITY_ACT_COPY_FROM_OUTER;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IPRItf,\
        &pData->action.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OPRI_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    switch(temp_var)
    {
        case 0:
            pData->action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_NONE;
            break;
        case 1:
            pData->action.outer_priority_action = VLAN_TAG_PRIORITY_ACT_ASSIGN;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OPRItf,\
        &pData->action.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITPID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.inner_tpid_action = VLAN_TAG_TPID_ACT_NONE;
    else
        pData->action.inner_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITPID_IDXtf,\
        &pData->action.inner_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTPID_ASSIGNtf,\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if(temp_var == 0)
        pData->action.outer_tpid_action = VLAN_TAG_TPID_ACT_NONE;
    else
        pData->action.outer_tpid_action = VLAN_TAG_TPID_ACT_ASSIGN;
    RT_ERR_HDL(table_field_get(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTPID_IDXtf,\
        &pData->action.outer_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      dal_longan_vlan_egrVlanCnvtEntry_set
 * Description:
 *      Set egress VLAN conversion (SC2C) entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion (SC2C) entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_OUT_OF_RANGE    - entry index is out of range
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_VLAN_VID        - invalid vid
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_VLAN_TPID_INDEX - Invalid TPID index
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_vlan_egrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData)
{
    int32   ret = RT_ERR_FAILED;
    vlan_egrcnvt_entry_t cnvt_entry;
    uint32 care_tagSts,vid_mask,pri_mask,port_mask,portType_mask;
    uint32 temp_var1,temp_var2,tagSts = 0;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

      /* parameter check */
    RT_PARAM_CHK(index >= HAL_MAX_NUM_OF_SC2C_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pData->valid != 0 && pData->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_CNVT_TAG_STS_END <= pData->match_key.inner_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_CNVT_TAG_STS_END <= pData->match_key.outer_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.inner_vid_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.outer_vid_ignore),RT_ERR_INPUT);
    RT_PARAM_CHK((pData->match_key.inner_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pData->match_key.outer_vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.inner_priority_ignore), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.outer_priority_ignore), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->match_key.inner_priority), RT_ERR_VLAN_PRIORITY);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->match_key.outer_priority), RT_ERR_VLAN_PRIORITY);
    RT_PARAM_CHK((RTK_ENABLE_END <= pData->match_key.port_ignore), RT_ERR_INPUT);
    RT_PARAM_CHK((1 < pData->match_key.port_type), RT_ERR_INPUT);
    RT_PARAM_CHK(((pData->match_key.port_ignore != 1) && (pData->match_key.port_type == 0) && !HWP_PORT_EXIST (unit, pData->match_key.port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK(((pData->match_key.port_ignore != 1) && (pData->match_key.port_type != 0) && (pData->match_key.port >= HAL_MAX_NUM_OF_TRUNK(unit))), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((VLAN_TAG_STATUS_ACT_END <= pData->action.inner_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_STATUS_ACT_END <= pData->action.outer_tag_sts), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_VID_ACT_END <= pData->action.inner_vid_action), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_VID_ACT_END <= pData->action.outer_vid_action), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->action.inner_vid_value > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pData->action.outer_vid_value > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((VLAN_TAG_PRIORITY_ACT_END <= pData->action.inner_priority_action), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_PRIORITY_ACT_END <= pData->action.outer_priority_action), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->action.inner_priority), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < pData->action.outer_priority), RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pData->action.inner_tpid_action), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TAG_TPID_ACT_END <= pData->action.outer_tpid_action), RT_ERR_INPUT);
    if (VLAN_TAG_TPID_ACT_ASSIGN == pData->action.inner_tpid_action)
            RT_PARAM_CHK(pData->action.inner_tpid_idx>= HAL_MAX_NUM_OF_CVLAN_TPID(unit), RT_ERR_VLAN_TPID_INDEX);
    if (VLAN_TAG_TPID_ACT_ASSIGN == pData->action.outer_tpid_action)
            RT_PARAM_CHK(pData->action.outer_tpid_idx>= HAL_MAX_NUM_OF_SVLAN_TPID(unit), RT_ERR_VLAN_TPID_INDEX);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*search key*/
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_VALIDtf,\
        &pData->valid, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->match_key.inner_tag_sts)
    {
        case VLAN_CNVT_TAG_STS_UNTAGGED:
            care_tagSts = 0x1;
            tagSts = 0;
            break;
        case VLAN_CNVT_TAG_STS_TAGGED:
            care_tagSts = 0x1;
            tagSts = 1;
            break;
        case VLAN_CNVT_TAG_STS_ALL:
            care_tagSts = 0x0;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_ITAG_IFtf,\
        &tagSts, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_ITAG_IFtf,\
        &care_tagSts, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->match_key.outer_tag_sts)
    {
        case VLAN_CNVT_TAG_STS_UNTAGGED:
            care_tagSts = 0x1;
            tagSts = 0;
            break;
        case VLAN_CNVT_TAG_STS_TAGGED:
            care_tagSts = 0x1;
            tagSts = 1;
            break;
        case VLAN_CNVT_TAG_STS_ALL:
            care_tagSts = 0x0;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_OTAG_IFtf,\
        &tagSts, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_OTAG_IFtf,\
        &care_tagSts, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.inner_vid_ignore == 1)
        vid_mask = 0;
    else
        vid_mask = 0xFFF;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_IVIDtf,\
        &pData->match_key.inner_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_IVIDtf,\
        &vid_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.outer_vid_ignore == 1)
        vid_mask = 0;
    else
        vid_mask = 0xFFF;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_OVIDtf,\
        &pData->match_key.outer_vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_OVIDtf,\
        &vid_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_VID_RNG_CHKtf,\
        &pData->match_key.rngchk_result_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.inner_priority_ignore== 1)
        pri_mask = 0;
    else
        pri_mask = 0x7;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_IPRItf,\
        &pData->match_key.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_IPRItf,\
        &pri_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.outer_priority_ignore== 1)
        pri_mask = 0;
    else
        pri_mask = 0x7;
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_INT_OPRItf,\
        &pData->match_key.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_INT_OPRItf,\
        &pri_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    if (pData->match_key.port_ignore == 1)
    {
        port_mask = 0;
        portType_mask = 0;
    }
    else
    {
        port_mask = 0x3F;
        portType_mask = 0x1;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_PORT_IDtf,\
        &pData->match_key.port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_PORT_IDtf,\
        &port_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_PORT_TYPEtf,\
        &pData->match_key.port_type, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_BMSK_PORT_TYPEtf,\
        &portType_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    /*action*/
    switch(pData->action.inner_tag_sts)
    {
        case VLAN_TAG_STATUS_ACT_NONE:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_UNTAGGED:
            temp_var1 = 1;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_TAGGED:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITAG_STS_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITAG_STStf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.outer_tag_sts)
    {
        case VLAN_TAG_STATUS_ACT_NONE:
            temp_var1 = 0;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_UNTAGGED:
            temp_var1 = 1;
            temp_var2 = 0;
            break;
        case VLAN_TAG_STATUS_ACT_TAGGED:
            temp_var1 = 1;
            temp_var2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTAG_STS_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTAG_STStf,\
        &temp_var2, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.inner_vid_action)
    {
        case VLAN_TAG_VID_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_VID_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        case VLAN_TAG_VID_ACT_SHIFT:
            temp_var1 = 2;
            break;
        case VLAN_TAG_VID_ACT_COPY_FROM_OUTER:
            temp_var1 = 3;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IVID_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IVIDtf,\
        &pData->action.inner_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.outer_vid_action)
    {
        case VLAN_TAG_VID_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_VID_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        case VLAN_TAG_VID_ACT_SHIFT:
            temp_var1 = 2;
            break;
        case VLAN_TAG_VID_ACT_COPY_FROM_INNER:
            temp_var1 = 3;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OVID_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OVIDtf,\
        &pData->action.outer_vid_value, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.inner_priority_action)
    {
        case VLAN_TAG_PRIORITY_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_PRIORITY_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        case VLAN_TAG_PRIORITY_ACT_COPY_FROM_OUTER:
            temp_var1 = 2;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IPRI_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_IPRItf,\
        &pData->action.inner_priority, (uint32 *) &cnvt_entry), errHandle, ret);

    switch(pData->action.outer_priority_action)
    {
        case VLAN_TAG_PRIORITY_ACT_NONE:
            temp_var1 = 0;
            break;
        case VLAN_TAG_PRIORITY_ACT_ASSIGN:
            temp_var1 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OPRI_ASSIGNtf,\
        &temp_var1, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OPRItf,\
        &pData->action.outer_priority, (uint32 *) &cnvt_entry), errHandle, ret);


    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITPID_ASSIGNtf,\
        &pData->action.inner_tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_ITPID_IDXtf,\
        &pData->action.inner_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTPID_ASSIGNtf,\
        &pData->action.outer_tpid_action, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, LONGAN_VLAN_EGR_CNVTt, LONGAN_VLAN_EGR_CNVT_OTPID_IDXtf,\
        &pData->action.outer_tpid_idx, (uint32 *) &cnvt_entry), errHandle, ret);

    /* programming entry to chip */
    VLAN_SEM_LOCK(unit);
    if ((ret = table_write(unit, LONGAN_VLAN_EGR_CNVTt, index, (uint32 *) &cnvt_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
}

/* Function Name:
 *      dal_longan_vlan_portEgrVlanCnvtEnable_get
 * Description:
 *      Get enable status of egress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable   - pointer to enable status of egress VLAN conversion
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
dal_longan_vlan_portEgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d,port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    VLAN_SEM_LOCK(unit);
    if((ret = reg_array_field_read(unit, LONGAN_VLAN_EVC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_EVC_ENf, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        VLAN_SEM_UNLOCK(unit);
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_vlan_portEgrVlanCnvtEnable_get */

/* Function Name:
 *      dal_longan_vlan_portEgrVlanCnvtEnable_set
 * Description:
 *      Set enable status of egress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of egress VLAN conversion
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
dal_longan_vlan_portEgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d,port=%d,enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    VLAN_SEM_LOCK(unit);
    if((ret = reg_array_field_write(unit, LONGAN_VLAN_EVC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_EVC_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        VLAN_SEM_UNLOCK(unit);
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_vlan_portEgrVlanCnvtEnable_set */

/* Function Name:
 *      dal_longan_vlan_tpidEntry_get
 * Description:
 *      Get TPID value from global  TPID pool.
 * Input:
 *      unit     - unit id
 *      type      - vlan tag type
 *      tpid_idx - index of TPID entry
 * Output:
 *      pTpid    - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Global four inner TPID can be specified.
 *      Global four outer TPID can be specified
 *      Global one extra TPID can be specified
 */
int32
dal_longan_vlan_tpidEntry_get(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 *pTpid)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, type=%d tpid_idx=%d", unit, type,tpid_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    /* Need to update tpid idx check*/
    RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_CVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((VLAN_TAG_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pTpid), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case VLAN_TAG_TYPE_INNER:
            reg = LONGAN_VLAN_TAG_TPID_CTRLr;
            field = LONGAN_ITPIDf;
            break;
        case VLAN_TAG_TYPE_OUTER:
            reg = LONGAN_VLAN_TAG_TPID_CTRLr;
            field = LONGAN_OTPIDf;
            break;
        case VLAN_TAG_TYPE_EXTRA:
            reg = LONGAN_VLAN_ETAG_TPID_CTRLr;
            field = LONGAN_ETPIDf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, reg, REG_ARRAY_INDEX_NONE, tpid_idx, field, pTpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "TPID=0x%04x", *pTpid);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_tpidEntry_set
 * Description:
 *      Set TPID value to global TPID pool.
 * Input:
 *      unit     - unit id
 *      type      - vlan tag type
 *      tpid_idx - index of TPID entry
 *      tpid     - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      Global four inner TPID can be specified.
 *      Global four outer TPID can be specified
 *      Global one extra TPID can be specified
 */
int32
dal_longan_vlan_tpidEntry_set(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 tpid)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, type=%d tpid_idx=%d, tpid=0x%04x", unit, type, tpid_idx, tpid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((VLAN_TAG_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((tpid > RTK_ETHERTYPE_MAX), RT_ERR_OUT_OF_RANGE);
    if(VLAN_TAG_TYPE_INNER == type)
        RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_CVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    if(VLAN_TAG_TYPE_OUTER == type)
        RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_SVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    if(VLAN_TAG_TYPE_EXTRA == type)
        RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_EVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);

    switch(type)
    {
        case VLAN_TAG_TYPE_INNER:
            reg = LONGAN_VLAN_TAG_TPID_CTRLr;
            field = LONGAN_ITPIDf;
            break;
        case VLAN_TAG_TYPE_OUTER:
            reg = LONGAN_VLAN_TAG_TPID_CTRLr;
            field = LONGAN_OTPIDf;
            break;
        case VLAN_TAG_TYPE_EXTRA:
            reg = LONGAN_VLAN_ETAG_TPID_CTRLr;
            field = LONGAN_ETPIDf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, reg, REG_ARRAY_INDEX_NONE, tpid_idx, field, &tpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portIgrTpid_get
 * Description:
 *      Get inner/outer TPIDs comparsion configration on specified port.
 * Input:
 *      unit     - unit id
 *      port    - port id
 *      type    -   vlan type
 * Output:
 *      pTpid_idx_mask - pointer to mask for index of tpid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner/outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 */
int32
dal_longan_vlan_portIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx_mask)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pTpid_idx_mask), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            reg = LONGAN_VLAN_PORT_ITAG_TPID_CMP_MSKr;
            field = LONGAN_ITPID_CMP_MSKf;
            break;
        case OUTER_VLAN:
            reg = LONGAN_VLAN_PORT_OTAG_TPID_CMP_MSKr;
            field = LONGAN_OTPID_CMP_MSKf;
            break;
        default:
            return RT_ERR_FAILED;
    }
    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, reg, port, REG_ARRAY_INDEX_NONE,\
        field, pTpid_idx_mask)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTpid_idx_mask=0x%x", *pTpid_idx_mask);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portIgrTpid_set
 * Description:
 *      Set inner/outer TPIDs comparsion configration on specified port
 * Input:
 *      unit     - unit id
 *      port    - port id
 *      type    -   vlan type
 *      tpid_idx_mask - mask for index of tpid entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner/outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 */
int32
dal_longan_vlan_portIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx_mask)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d vlan type=%d tpid_idx_mask=0x%x", unit, port, type, tpid_idx_mask);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(tpid_idx_mask > HAL_TPID_ENTRY_MASK_MAX(unit), RT_ERR_INPUT);

    switch(type)
    {
        case INNER_VLAN:
            reg = LONGAN_VLAN_PORT_ITAG_TPID_CMP_MSKr;
            field = LONGAN_ITPID_CMP_MSKf;
            break;
        case OUTER_VLAN:
            reg = LONGAN_VLAN_PORT_OTAG_TPID_CMP_MSKr;
            field = LONGAN_OTPID_CMP_MSKf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, reg, port, REG_ARRAY_INDEX_NONE,\
        field, &tpid_idx_mask)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrTpid_get
 * Description:
 *      Get inner/outer TPID for inner/outer tag encapsulation when transmiiting a inner/outer-tagged pacekt.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTpid_idx - pointer to index of inner TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portEgrTpid_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 *pTpid_idx)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pTpid_idx), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_ITPID_IDXf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OTPID_IDXf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_EGR_TPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, pTpid_idx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTpid_idx=%x", *pTpid_idx);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portEgrInnerTpid_get */

/* Function Name:
 *      dal_longan_vlan_portEgrTpid_set
 * Description:
 *      Set inner/outer TPID for inner/outer tag encapsulation when transmiiting a inner/outer-tagged pacekt.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tpid_idx - index of inner TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_longan_vlan_portEgrTpid_set(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 tpid_idx)
{
    int32   ret = RT_ERR_FAILED;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, tpid_idx=%d", unit, port, tpid_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK(tpid_idx > HAL_TPID_ENTRY_IDX_MAX(unit), RT_ERR_OUT_OF_RANGE);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_ITPID_IDXf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OTPID_IDXf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_EGR_TPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, &tpid_idx)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portEgrInnerTpid_set */

/* Function Name:
 *      dal_longan_vlan_portEgrTpidSrc_get
 * Description:
 *      Get source of Inner/Outer TPID at egress.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pTpid_src - pointer to inner/outer TPID src at egress
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The TPID_SRC_FROM_ORIG_INNER/OUTER work only for inner/outer tagged packet.
 */
int32
dal_longan_vlan_portEgrTpidSrc_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t *pTpid_src)
{
    int32   ret = RT_ERR_FAILED;
    uint32   value,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d", unit, port, type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pTpid_src), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_ITPID_KEEPf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OTPID_KEEPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* program value to CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_EGR_TPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if(0 == value)
        *pTpid_src = VLAN_TPID_SRC_EGR;
    else if (1 == value)
        *pTpid_src = VLAN_TPID_SRC_ORIG;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTpid_src=%d", *pTpid_src);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrTpidSrc_set
 * Description:
 *      Set Inner/Outer TPID Source  at egress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      tpid_src - Inner/Outer TPID src at egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The TPID_SRC_FROM_ORIG_INNER/OUTER work only for inner/outer tagged packet.
 */
int32
dal_longan_vlan_portEgrTpidSrc_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t tpid_src)
{
    int32   ret = RT_ERR_FAILED;
    uint32   value,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d tpid src=%d", unit, port, type,tpid_src);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((VLAN_TPID_SRC_END <= tpid_src), RT_ERR_INPUT);


    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_ITPID_KEEPf;
            break;
        case OUTER_VLAN:
            field = LONGAN_OTPID_KEEPf;
            break;
        default:
            return RT_ERR_FAILED;
    }
    /* translate value to chip's definition */
    switch(tpid_src)
    {
        case VLAN_TPID_SRC_EGR:
            value = 0;
            break;
        case VLAN_TPID_SRC_ORIG:
            value = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_EGR_TPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, field, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}
/* Function Name:
 *      dal_longan_vlan_portIgrExtraTagEnable_get
 * Description:
 *      Get enable state of extra tag comparsion.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the extra tag comparsion state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only one extra tag is supported, refer to rtk_vlan_extraTpidEntry_set
 */
int32
dal_longan_vlan_portIgrExtraTagEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_ETAG_TPID_CMPr, port, REG_ARRAY_INDEX_NONE,\
        LONGAN_ETPID_CMPf, pEnable)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portIgrExtraTagEnable_get */

/* Function Name:
 *      dal_longan_vlan_portIgrExtraTagEnable_set
 * Description:
 *      Enable extra tag comparsion.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - extra tag comparsion state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Only one extra tag is supported, refer to rtk_vlan_extraTpidEntry_set
 */
int32
dal_longan_vlan_portIgrExtraTagEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_ETAG_TPID_CMPr, port, REG_ARRAY_INDEX_NONE,\
        LONGAN_ETPID_CMPf, &enable)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_portIgrExtraTagEnable_set */

/* Function Name:
 *      dal_longan_vlan_portVlanAggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support on egress port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portVlanAggrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_CNVT_ENf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pEnable = DISABLED;
            break;
        case 1:
            *pEnable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=%x", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portVlanAggrEnable_set
 * Description:
 *      Enable N:1 VLAN aggregation support on egress port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_vlan_portVlanAggrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (enable)
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

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                LONGAN_CNVT_ENf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portVlanAggrCtrl_get
 * Description:
 *      Get port vlan-aggragation configration.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pVlanAggrCtrl - pointer to vlan-aggr ctrl
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portVlanAggrCtrl_get(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value1,value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pVlanAggrCtrl), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                LONGAN_VID_SELf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                LONGAN_PRI_TAG_ACTf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value1)
    {
        case 0:
            pVlanAggrCtrl->vid_src = INNER_VLAN;
            break;
        case 1:
            pVlanAggrCtrl->vid_src = OUTER_VLAN;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (value2)
    {
        case 0:
            pVlanAggrCtrl->pri_src= LEARNING_VID_PRI;
            break;
        case 1:
            pVlanAggrCtrl->pri_src = LEARNING_VID_PBASED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "vid source=%d,pri source=%d", pVlanAggrCtrl->vid_src,pVlanAggrCtrl->pri_src);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portVlanAggrCtrl_set
 * Description:
 *      Set port vlan aggragation .
 * Input:
 *      unit - unit id
 *      port - port id
 *      vlanAggrCtrl  - vlan-aggr ctrl
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
dal_longan_vlan_portVlanAggrCtrl_set(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t vlanAggrCtrl)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value1,value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vid src=%d, pri src=%d",
        unit, port, vlanAggrCtrl.vid_src,vlanAggrCtrl.pri_src);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(vlanAggrCtrl.vid_src >= VLAN_TYPE_END,RT_ERR_INPUT);
    RT_PARAM_CHK(vlanAggrCtrl.pri_src >= LEARNING_VID_END,RT_ERR_INPUT);

    /* translate value to chip's definition */
    switch (vlanAggrCtrl.vid_src)
    {
        case INNER_VLAN:
            value1 = 0;
            break;
        case OUTER_VLAN:
            value1 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (vlanAggrCtrl.pri_src)
    {
        case LEARNING_VID_PRI:
            value2 = 0;
            break;
        case LEARNING_VID_PBASED:
            value2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                LONGAN_VID_SELf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                LONGAN_PRI_TAG_ACTf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrTagSts_get
 * Description:
 *      Get tag  status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 * Output:
 *      pSts - tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_tagSts_t *pSts)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d", unit, port,type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pSts), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_EGR_ITAG_STSf;
            break;
        case OUTER_VLAN:
            field = LONGAN_EGR_OTAG_STSf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                field, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pSts = TAG_STATUS_INTERNAL;
            break;
        case 1:
            *pSts = TAG_STATUS_UNTAG;
            break;
        case 2:
            *pSts = TAG_STATUS_TAGGED;
            break;
        case 3:
            *pSts = TAG_STATUS_PRIORITY_TAGGED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSts=%x", *pSts);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrTagSts_set
 * Description:
 *      Set tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      sts  - tag status
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
dal_longan_vlan_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type,rtk_vlan_tagSts_t sts)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value,field;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vlan type=%d, sts=%d", unit, port, type, sts);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type >= VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((sts >= TAG_STATUS_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (sts)
    {
        case TAG_STATUS_INTERNAL:
            value = 0;
            break;
        case TAG_STATUS_UNTAG:
            value = 1;
            break;
        case TAG_STATUS_TAGGED:
            value = 2;
            break;
        case TAG_STATUS_PRIORITY_TAGGED:
            value = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch(type)
    {
        case INNER_VLAN:
            field = LONGAN_EGR_ITAG_STSf;
            break;
        case OUTER_VLAN:
            field = LONGAN_EGR_OTAG_STSf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                field, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Internal Function Body */

/* Function Name:
 *      _dal_longan_vlan_init_config
 * Description:
 *      Initialize default config of vlan for the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling this API.
 */
static int32
_dal_longan_vlan_init_config(uint32 unit)
{

    int32       i, ret;
    dal_longan_vlan_data_t vlan_data_entry;
    rtk_port_t  port;

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        /* configure default vlan data entry */
        osal_memset(&vlan_data_entry, 0x00, sizeof(dal_longan_vlan_data_t));
        vlan_data_entry.vid         = RTK_DEFAULT_VLAN_ID;
        vlan_data_entry.msti    = RTK_DEFAULT_MSTI;
        vlan_data_entry.ucast_mode  = VLAN_L2_LOOKUP_MODE_VID;
        vlan_data_entry.mcast_mode  = VLAN_L2_LOOKUP_MODE_VID;
        vlan_data_entry.profile_idx = 0;
        HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.member_portmask);
        RTK_PORTMASK_PORT_CLEAR(vlan_data_entry.member_portmask, HWP_CPU_MACID(unit));
        HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.untag_portmask);

        if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "create default vlan entry failed");
            return ret;
        }

        /* configure vlan data entry other than default VLAN */
        osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
        vlan_data_entry.msti    = RTK_DEFAULT_MSTI;
        vlan_data_entry.ucast_mode  = VLAN_L2_LOOKUP_MODE_VID;
        vlan_data_entry.mcast_mode  = VLAN_L2_LOOKUP_MODE_VID;
        vlan_data_entry.profile_idx = 0;
        for (i = 0; i <= RTK_VLAN_ID_MAX; i++)
        {
            if (i == RTK_DEFAULT_VLAN_ID)
                continue;

            vlan_data_entry.vid = i;
            if ((ret = _dal_longan_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "Init 4K vlan entry failed");
                return ret;
            }
        }
    }

    /* Set vlan valid bit in software */
    /* http://172.21.12.139:8080/query/defects.htm?projectId=10002&cid=30373 */
    if(HWP_UNIT_VALID_IN_RANGE(unit))
    {
        VLANINFO_VALID_SET(unit, RTK_DEFAULT_VLAN_ID);
        pDal_longan_vlan_info[unit]->count++;
    }

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        HWP_PORT_TRAVS(unit, port)
        {
            /* Configure inner PVID to default VLAN */
            if ((ret = dal_longan_vlan_portPvid_set(unit, port,INNER_VLAN,RTK_DEFAULT_PORT_VID)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "set inner PVID to default VLAN failed");
                return ret;
            }

            /* Configure outer PVID to default VLAN */
            if ((ret = dal_longan_vlan_portPvid_set(unit, port,OUTER_VLAN,RTK_DEFAULT_PORT_VID)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "set inner PVID to default VLAN failed");
                return ret;
            }

            /* Turn on ingress VLAN filtering */
            if (!HWP_IS_CPU_PORT(unit, port))
            {
                if ((ret = dal_longan_vlan_portIgrFilter_set(unit, port, INGRESS_FILTER_DROP)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "turn on ingress VLAN filtering failed");
                    return ret;
                }
            }

            /* Turn on egress VLAN filtering */
            if ((ret = dal_longan_vlan_portEgrFilterEnable_set(unit, port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "turn on egress VLAN filtering failed");
                return ret;
            }

            /* Configure egress port inner tag status */
            if ((ret = dal_longan_vlan_portEgrTagSts_set(unit, port,INNER_VLAN, TAG_STATUS_INTERNAL)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure egress inner tag status failed");
                return ret;
            }

        }
    }

    return RT_ERR_OK;
} /* end of _dal_longan_vlan_init_config */

/* Function Name:
 *      _dal_longan_setVlan
 * Description:
 *      Set vlan entry to chip.
 * Input:
 *      unit        - unit id
 *      pVlan_entry - content of vlan entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
static int32 _dal_longan_setVlan(uint32 unit, dal_longan_vlan_data_t *pVlan_entry)
{
    int32   ret = RT_ERR_FAILED;
    vlan_entry_t        vlan_entry;
    vlan_untag_entry_t  vlan_untag_entry;
    uint32  temp_var;

    RT_PARAM_CHK((NULL == pVlan_entry), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, msti=%d, ucast_mode=%s, mcast_mode=%s, profile_idx=%d, member_portmask=%x,vlan group_mask=%x, untag_portmask=%x",
           unit, pVlan_entry->vid, pVlan_entry->msti, (pVlan_entry->ucast_mode)? "FID" : "VID", (pVlan_entry->mcast_mode)? "FID" : "VID", \
           pVlan_entry->profile_idx, pVlan_entry->member_portmask, pVlan_entry->groupMask,pVlan_entry->untag_portmask);

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));
    osal_memset(&vlan_untag_entry, 0, sizeof(vlan_untag_entry));

    /*** VLAN table ***/
    /* set  msti */
    temp_var = pVlan_entry->msti;
    if ((ret = table_field_set(unit, LONGAN_VLANt, LONGAN_VLAN_MSTItf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set unicast lookup mode */
    temp_var = pVlan_entry->ucast_mode;
    if ((ret = table_field_set(unit, LONGAN_VLANt, LONGAN_VLAN_L2_HKEY_UCASTtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set mulicast lookup mode */
    temp_var = pVlan_entry->mcast_mode;
    if ((ret = table_field_set(unit, LONGAN_VLANt, LONGAN_VLAN_L2_HKEY_MCASTtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set profile index */
    if ((ret = table_field_set(unit, LONGAN_VLANt, LONGAN_VLAN_VLAN_PROFILEtf, &pVlan_entry->profile_idx, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set member set */
    if ((ret = table_field_set(unit, LONGAN_VLANt, LONGAN_VLAN_MBRtf, pVlan_entry->member_portmask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    /* set group mask */
    if ((ret = table_field_set(unit, LONGAN_VLANt, LONGAN_VLAN_GROUP_MASKtf, pVlan_entry->groupMask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* programming vlan entry to chip */
    if ((ret = table_write(unit, LONGAN_VLANt, pVlan_entry->vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*** VLAN untag member table ***/
    /* set untagged member set */
    if ((ret = table_field_set(unit, LONGAN_UNTAGt, LONGAN_UNTAG_PMSKtf, pVlan_entry->untag_portmask.bits, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* programming vlan untag entry to chip */
    if ((ret = table_write(unit, LONGAN_UNTAGt, pVlan_entry->vid, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_longan_setVlan */

/* Function Name:
 *      _dal_longan_getVlan
 * Description:
 *      Get vlan entry from chip.
 * Input:
 *      unit        - unit id
 *      pVlan_entry - buffer of vlan entry
 * Output:
 *      pVlan_entry - content of vlan entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
static int32 _dal_longan_getVlan(uint32 unit, dal_longan_vlan_data_t *pVlan_entry)
{
    int32   ret = RT_ERR_FAILED;
    vlan_entry_t        vlan_entry;
    vlan_untag_entry_t  vlan_untag_entry;
    uint32  temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, pVlan_entry->vid);

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    /*** get VLAN entry from chip ***/
    if ((ret = table_read(unit, LONGAN_VLANt, pVlan_entry->vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* get msti */
    if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_MSTItf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->msti = temp_var;

    /* get unicast lookup mode */
    if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_L2_HKEY_UCASTtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->ucast_mode = temp_var;

    /* get multicast lookup mode */
    if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_L2_HKEY_MCASTtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->mcast_mode = temp_var;

    /* get profile index */
    if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_VLAN_PROFILEtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->profile_idx = temp_var;

    /* get member set from vlan_entry */
    if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_MBRtf, pVlan_entry->member_portmask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* get group mask from vlan_entry */
    if ((ret = table_field_get(unit, LONGAN_VLANt, LONGAN_VLAN_GROUP_MASKtf, pVlan_entry->groupMask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*** get VLAN untag member from chip ***/
    if ((ret = table_read(unit, LONGAN_UNTAGt, pVlan_entry->vid, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* get untagged member set */
    if ((ret = table_field_get(unit, LONGAN_UNTAGt, LONGAN_UNTAG_PMSKtf, pVlan_entry->untag_portmask.bits, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, msti=%d, ucast_mode=%s, mcast_mode=%s, profile_idx=%d, member_portmask=%x,vlan group_mask=%x, untag_portmask=%x",
           unit, pVlan_entry->vid, pVlan_entry->msti, (pVlan_entry->ucast_mode)? "FID" : "VID", (pVlan_entry->mcast_mode)? "FID" : "VID", \
           pVlan_entry->profile_idx, pVlan_entry->member_portmask, pVlan_entry->groupMask,pVlan_entry->untag_portmask);

    return RT_ERR_OK;
} /* end of _dal_longan_getVlan */

/* Function Name:
 *      _dal_longan_vlan_actionXlateRtk
 * Description:
 *      translate chip value to rtk action.
 * Input:
 *      value        - chip value
 *      pVlan_entry - buffer of vlan entry
 *     type      - action type
 * Output:
 *      pAction - rtk action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
static int32 _dal_longan_vlan_actionXlateRtk(uint32 value,dal_longan_vlan_action_type_t type,rtk_action_t *pAction)
{
    RT_PARAM_CHK((LONGAN_VLAN_ACTION_END <= type), RT_ERR_INPUT);

    if(LONGAN_VLAN_ACTION_BGD == type)
    {
        switch(value)
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
                return RT_ERR_INPUT;
        }
    }
    else if(LONGAN_VLAN_ACTION_RSVD_MC == type)
    {
        switch(value)
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
                return RT_ERR_INPUT;
        }
    }
    else if(LONGAN_VLAN_ACTION_URPF_FAIL == type)
    {
        switch(value)
        {
            case 0:
                *pAction = ACTION_DROP;
                break;
            case 1:
                *pAction = ACTION_TRAP2CPU;
                break;
            case 2:
                *pAction = ACTION_FORWARD;
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
                return RT_ERR_INPUT;
        }
    }
    else
    {
        switch(value)
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
                return RT_ERR_INPUT;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_vlan_actionXlateDal
 * Description:
 *      translate  rtk action to chip value.
 * Input:
 *     action        - rtk action
 *     type      - action type
 * Output:
 *      pValue - chip value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
static int32 _dal_longan_vlan_actionXlateDal(rtk_action_t action, dal_longan_vlan_action_type_t type,uint32 *pValue)
{
    RT_PARAM_CHK((LONGAN_VLAN_ACTION_END <= type), RT_ERR_INPUT);

    if(LONGAN_VLAN_ACTION_BGD == type)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pValue = 0 ;
                break;
            case ACTION_DROP:
                *pValue = 1 ;
                break;
            case ACTION_TRAP2CPU:
                *pValue = 2;
                break;
            case ACTION_COPY2CPU:
                *pValue = 3;
                break;
            case ACTION_TRAP2MASTERCPU:
                *pValue = 4;
                break;
            case ACTION_COPY2MASTERCPU:
                *pValue = 5;
                break;
            default:
                return RT_ERR_INPUT;
        }
    }
    else if(LONGAN_VLAN_ACTION_RSVD_MC == type)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pValue = 0 ;
                break;
            case ACTION_DROP:
                *pValue = 1;
                break;
            case ACTION_TRAP2CPU:
                *pValue = 2;
                break;
            case ACTION_COPY2CPU:
                *pValue = 3;
                break;
            case ACTION_TRAP2MASTERCPU:
                *pValue = 4;
                break;
            case ACTION_COPY2MASTERCPU:
                *pValue = 5;
                break;
            default:
                return RT_ERR_INPUT;
        }

    }
    else if(LONGAN_VLAN_ACTION_URPF_FAIL == type)
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pValue = 2 ;
                break;
            case ACTION_DROP:
                *pValue = 0;
                break;
            case ACTION_TRAP2CPU:
                *pValue = 1;
                break;
            case ACTION_COPY2CPU:
                *pValue = 3;
                break;
            case ACTION_TRAP2MASTERCPU:
                *pValue = 4;
                break;
            case ACTION_COPY2MASTERCPU:
                *pValue = 5;
                break;
            default:
                return RT_ERR_INPUT;
        }

    }
    else
    {
        switch(action)
        {
            case ACTION_FORWARD:
                *pValue = 0 ;
                break;
            case ACTION_DROP:
                *pValue = 1 ;
                break;
            case ACTION_TRAP2CPU:
                *pValue = 2;
                break;
            case ACTION_COPY2CPU:
                *pValue = 3;
                break;
            case ACTION_TRAP2MASTERCPU:
                *pValue = 4;
                break;
            case ACTION_COPY2MASTERCPU:
                *pValue = 5;
                break;
            default:
                return RT_ERR_INPUT;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_leakyStpFilter_get
 * Description:
 *      Get leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to status of leaky STP filter
 *            ENABLED :Enable STP filter for multicast leaky;
 *            DISABLED:Disable STP filter for multicast leaky.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_leakyStpFilter_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_VLAN_CTRLr, LONGAN_STP_LEAK_ENf,
            &val) ) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    if (1 == val)
        *pEnable = ENABLED;
    else if(0 == val)
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}    /* end of dal_longan_vlan_leakyStpFilter_get */

/* Function Name:
 *      dal_longan_vlan_leakyStpFilter_set
 * Description:
 *      Set leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit    - unit id
 *      enable  - status of leaky STP filter,
 *            ENABLED :Enable STP filter for multicast leaky;
 *            DISABLED:Disable STP filter for multicast leaky.
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
dal_longan_vlan_leakyStpFilter_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d enable:%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (DISABLED == enable)
        val = 0;
    else if (ENABLED == enable)
        val = 1;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "ret=0x%x");
        return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, LONGAN_VLAN_CTRLr, LONGAN_STP_LEAK_ENf,
            &val)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_longan_vlan_leakyStpFilter_set */

/* Function Name:
 *      dal_longan_vlan_portIgrVlanCnvtLuMisAct_get
 * Description:
 *      Get action for packet that doesn't hit any Ingress VLAN Conversion (IVC) entry.
 * Input:
 *      unit    - unit id
 *      port   - port id for configure
 *      type   - vlan type
 * Output:
 *      pAction - pointer to VLAN conversion default action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
dal_longan_vlan_portIgrVlanCnvtLuMisAct_get(
    uint32                                unit,
    rtk_port_t                            port,
    rtk_vlanType_t                       type,
    rtk_vlan_lookupMissAct_t          *pAction)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val,field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d port=%d vlan type=%d", unit, port, type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type>= VLAN_TYPE_END,RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case VLAN_TAG_TYPE_INNER:
            field = LONGAN_ITAG_LU_MIS_DROPf;
            break;
        case VLAN_TAG_TYPE_OUTER:
            field = LONGAN_OTAG_LU_MIS_DROPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_IVC_CTRLr,
                port, REG_ARRAY_INDEX_NONE, field, &val)) !=
                RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    if (0 == val)
        *pAction = LOOKUPMISS_FWD;
    else if(1 == val)
        *pAction = LOOKUPMISS_DROP;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "pAction=%d", *pAction);

    return RT_ERR_OK;
}    /* end of dal_longan_vlan_portIgrCnvtDfltAct_get */

/* Function Name:
 *      dal_longan_vlan_portIgrVlanCnvtLuMisAct_set
 * Description:
 *      Set action for packet that doesn't hit Ingress VLAN Conversion (IVC) entry.
 * Input:
 *      unit   - unit id
 *      port   - port id for configure
 *      type  - vlan type
 *      action - VLAN conversion default action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
dal_longan_vlan_portIgrVlanCnvtLuMisAct_set(
    uint32                               unit,
    rtk_port_t                           port,
    rtk_vlanType_t                      type,
    rtk_vlan_lookupMissAct_t           action)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val,field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d port=%d vlan type=%d action=%d",
            unit, port, type, action);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type),RT_ERR_INPUT);
    RT_PARAM_CHK((LOOKUPMISS_END <= action), RT_ERR_INPUT);

    if (LOOKUPMISS_FWD == action)
        val = 0;
    else if (LOOKUPMISS_DROP == action)
        val = 1;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "ret=0x%x");
        return RT_ERR_INPUT;
    }

    switch(type)
    {
        case VLAN_TAG_TYPE_INNER:
            field = LONGAN_ITAG_LU_MIS_DROPf;
            break;
        case VLAN_TAG_TYPE_OUTER:
            field = LONGAN_OTAG_LU_MIS_DROPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_IVC_CTRLr,
                port, REG_ARRAY_INDEX_NONE, field, &val)) !=
                RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_longan_vlan_portIgrCnvtDfltAct_set */

/* Function Name:
 *      dal_longan_vlan_portEgrVlanCnvtLuMisAct_get
 * Description:
 *      Get action for packet that doesn't hit any Egress VLAN Conversion (EVC) entry.
 * Input:
 *      unit    - unit id
 *      port   - port id for configure
 *      type   - vlan type
 * Output:
 *      pAction - pointer to VLAN conversion default action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
dal_longan_vlan_portEgrVlanCnvtLuMisAct_get(
    uint32                                unit,
    rtk_port_t                            port,
    rtk_vlanType_t                       type,
    rtk_vlan_lookupMissAct_t          *pAction)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val,field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d port=%d, vlan type=%d", unit, port, type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(type>= VLAN_TYPE_END,RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case VLAN_TAG_TYPE_INNER:
            field = LONGAN_ITAG_LU_MIS_DROPf;
            break;
        case VLAN_TAG_TYPE_OUTER:
            field = LONGAN_OTAG_LU_MIS_DROPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_EVC_CTRLr,
                port, REG_ARRAY_INDEX_NONE, field, &val)) !=
                RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    if (0 == val)
        *pAction = LOOKUPMISS_FWD;
    else if(1 == val)
        *pAction = LOOKUPMISS_DROP;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "pAction=%d", *pAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrVlanCnvtLuMisAct_set
 * Description:
 *      Set action for packet that doesn't hit Egress VLAN Conversion (EVC) entry.
 * Input:
 *      unit   - unit id
 *      port   - port id for configure
 *      type  - vlan type
 *      action - VLAN conversion default action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
int32
dal_longan_vlan_portEgrVlanCnvtLuMisAct_set(
    uint32                               unit,
    rtk_port_t                           port,
    rtk_vlanType_t                      type,
    rtk_vlan_lookupMissAct_t           action)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val,field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d port=%d vlan type=%d action=%d",
            unit, port, type, action);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type),RT_ERR_INPUT);
    RT_PARAM_CHK((LOOKUPMISS_END <= action), RT_ERR_INPUT);

    if (LOOKUPMISS_FWD == action)
        val = 0;
    else if (LOOKUPMISS_DROP == action)
        val = 1;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "ret=0x%x");
        return RT_ERR_INPUT;
    }

    switch(type)
    {
        case VLAN_TAG_TYPE_INNER:
            field = LONGAN_ITAG_LU_MIS_DROPf;
            break;
        case VLAN_TAG_TYPE_OUTER:
            field = LONGAN_OTAG_LU_MIS_DROPf;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_EVC_CTRLr,
                port, REG_ARRAY_INDEX_NONE, field, &val)) !=
                RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtEntry_delAll
 * Description:
 *      Delete all ingress VLAN conversion entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_longan_vlan_igrVlanCnvtEntry_delAll(uint32 unit)
{
    uint32                  entry_idx;
    int32                   ret;
    vlan_igrcnvt_entry_t    cnvt_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    VLAN_SEM_LOCK(unit);

    for (entry_idx = 0; entry_idx < HAL_MAX_NUM_OF_C2SC_ENTRY(unit); ++entry_idx)
    {
        ret = table_write(unit, LONGAN_VLAN_IGR_CNVTt, entry_idx, (uint32 *) &cnvt_entry);
        if (RT_ERR_OK != ret)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_vlan_igrVlanCnvtEntry_delAll */

/* Function Name:
 *      dal_longan_vlan_egrVlanCnvtEntry_delAll
 * Description:
 *      Delete all egress VLAN conversion entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_longan_vlan_egrVlanCnvtEntry_delAll(uint32 unit)
{
    uint32                  entry_idx;
    int32                   ret;
    vlan_egrcnvt_entry_t    cnvt_entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    osal_memset(&cnvt_entry, 0, sizeof(vlan_egrcnvt_entry_t));

    VLAN_SEM_LOCK(unit);

    for (entry_idx = 0; entry_idx < HAL_MAX_NUM_OF_SC2C_ENTRY(unit); ++entry_idx)
    {
        ret = table_write(unit, LONGAN_VLAN_EGR_CNVTt, entry_idx, (uint32 *) &cnvt_entry);
        if (RT_ERR_OK != ret)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_vlan_egrVlanCnvtEntry_delAll */

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtHitIndication_get
 * Description:
 *      Get Ingress VLAN Conversion  entry hit indication.
 * Input:
 *      unit        - unit id
 *      entry_idx   - IVC entry index
 *      flag       - hit indication flag(ex,reset the hit status)
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The hit status can be cleared by configuring para. reset 'flag' to 1.
 */
int32
dal_longan_vlan_igrVlanCnvtHitIndication_get(uint32 unit,uint32 entry_idx,uint32 flag,uint32 *pIsHit)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_IVC_ENTRY_INDICATIONr,
            REG_ARRAY_INDEX_NONE, entry_idx, LONGAN_HITf, pIsHit)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if(TRUE == flag)
    {
        /* reset Hit indication bit */
        if ((ret = reg_array_field_write(unit, LONGAN_VLAN_IVC_ENTRY_INDICATIONr,
                REG_ARRAY_INDEX_NONE, entry_idx, LONGAN_HITf, &flag)) !=
                RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pIsHit=%x", *pIsHit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_egrVlanCnvtHitIndication_get
 * Description:
 *      Get Egress VLAN Conversion  entry hit indication.
 * Input:
 *      unit        - unit id
 *      entry_idx   - EVC entry indexs
 *      flag       - hit indication flag(ex,reset the hit status)
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The hit status can be cleared by configuring para reset 'flag' to 1.
 */
int32
dal_longan_vlan_egrVlanCnvtHitIndication_get(uint32 unit,uint32 entry_idx,uint32 flag,uint32 *pIsHit)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, entry_idx=%d", unit, entry_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsHit), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_EVC_ENTRY_INDICATIONr,
            REG_ARRAY_INDEX_NONE, entry_idx, LONGAN_HITf, pIsHit)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if(TRUE == flag)
    {
        /* reset Hit indication bit */
        if ((ret = reg_array_field_write(unit, LONGAN_VLAN_EVC_ENTRY_INDICATIONr,
                REG_ARRAY_INDEX_NONE, entry_idx, LONGAN_HITf, &flag)) !=
                RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pIsHit=%x", *pIsHit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portIgrVlanCnvtRangeCheckSet_get
 * Description:
 *      Get the VID range check Set  for ingress VLAN conversion which the specified port refered.
 * Input:
 *      unit  - unit id
 *      port    - port id
 * Output:
 *      pSetIdx  - pointer to evc vlan range check set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    None
 */
int32
dal_longan_vlan_portIgrVlanCnvtRangeCheckSet_get(uint32 unit,rtk_port_t port,uint32 *pSetIdx)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSetIdx), RT_ERR_NULL_POINTER);

   VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_IVC_CTRLr,
            port, REG_ARRAY_INDEX_NONE, LONGAN_VID_RANGE_SET_IDXf, pSetIdx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSetIdx=%x", *pSetIdx);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portIgrVlanCnvtRangeCheckSet_set
 * Description:
 *      configure the VID range check Set  for ingress VLAN conversion which the specified port refered.
 * Input:
 *      unit  - unit id
 *      port    - port id
 *      setIdx  - ivc vlan range check set index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portIgrVlanCnvtRangeCheckSet_set(uint32 unit,rtk_port_t port,uint32 setIdx)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((setIdx >= HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_SET(unit)), RT_ERR_INPUT);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_IVC_CTRLr,
            port, REG_ARRAY_INDEX_NONE, LONGAN_VID_RANGE_SET_IDXf, &setIdx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtRangeCheckEntry_get
 * Description:
 *      Get the configuration of VID range check for ingress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      setIdx - vlan range check set index
 *      index - entry index
 * Output:
 *      pData - configuration of VID Range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_igrVlanCnvtRangeCheckEntry_get(
    uint32                                      unit,
    uint32                                      setIdx,
    uint32                                      index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t          *pData)
{
    uint32  reg;
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if(0 == setIdx)
        reg = LONGAN_VLAN_IGR_VID_RNG_CHK_SET_0r;
    else if (1 == setIdx)
        reg = LONGAN_VLAN_IGR_VID_RNG_CHK_SET_1r;
    else
        return RT_ERR_INPUT;

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_TYPEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_UPPERf,
            &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_LOWERf,
            &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            pData->vid_type = INNER_VLAN;
            break;
        case 1:
            pData->vid_type = OUTER_VLAN;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_igrVlanCnvtRangeCheckEntry_set
 * Description:
 *      Set the configuration of VID range check for ingress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      setIdx - vlan range check set index
 *      index - entry index
 *      pData - configuration of VID Range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_vlan_igrVlanCnvtRangeCheckEntry_set(
    uint32                                  unit,
    uint32                                  setIdx,
    uint32                                  index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t    *pData)
{
    uint32  reg;
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, index=%d", unit, index);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "upper_bound=%d, upper_bound=%d, vid_type=%d",\
        pData->vid_upper_bound, pData->vid_lower_bound, pData->vid_type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_C2SC_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK(((pData->vid_type != OUTER_VLAN) && (pData->vid_type != INNER_VLAN)), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_lower_bound > pData->vid_upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_upper_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_lower_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);

    if(0 == setIdx)
        reg = LONGAN_VLAN_IGR_VID_RNG_CHK_SET_0r;
    else if (1 == setIdx)
        reg = LONGAN_VLAN_IGR_VID_RNG_CHK_SET_1r;
      else
        return RT_ERR_INPUT;

    switch (pData->vid_type)
    {
        case INNER_VLAN:
            value = 0;
            break;
        case OUTER_VLAN:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_VLAN), "invalid VID type");
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_TYPEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_UPPERf,
            &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_LOWERf,
            &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrVlanCnvtRangeCheckSet_get
 * Description:
 *      Get the VID range check Set  for egress VLAN conversion which the specified port refered.
 * Input:
 *      unit  - unit id
 *      port    - port id
 * Output:
 *      pSetIdx  - pointer to evc vlan range check set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portEgrVlanCnvtRangeCheckSet_get(uint32 unit,rtk_port_t port,uint32 *pSetIdx)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSetIdx), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_EVC_CTRLr,
            port, REG_ARRAY_INDEX_NONE, LONGAN_VID_RANGE_SET_IDXf, pSetIdx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSetIdx=%x", *pSetIdx);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_portEgrVlanCnvtRangeCheckSet_set
 * Description:
 *      configure the VID range check Set  for egress VLAN conversion which the specified port refered.
 * Input:
 *      unit  - unit id
 *      port    - port id
 *      setIdx  - evc vlan range check set index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_portEgrVlanCnvtRangeCheckSet_set(uint32 unit,rtk_port_t port,uint32 setIdx)
{
    uint32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((setIdx >= HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_SET(unit)), RT_ERR_INPUT);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_EVC_CTRLr,
            port, REG_ARRAY_INDEX_NONE, LONGAN_VID_RANGE_SET_IDXf, &setIdx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_egrVlanCnvtRangeCheckEntry_get
 * Description:
 *      Get the configuration of VID range check for ingress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      setIdx - vlan range check set index
 *      index - entry index
 * Output:
 *      pData - configuration of VID Range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_egrVlanCnvtRangeCheckEntry_get(
    uint32                                      unit,
    uint32                                      setIdx,
    uint32                                      index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t          *pData)
{
    uint32  reg;
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if(0 == setIdx)
        reg = LONGAN_VLAN_EGR_VID_RNG_CHK_SET_0r;
    else if (1 == setIdx)
        reg = LONGAN_VLAN_EGR_VID_RNG_CHK_SET_1r;
    else
        return RT_ERR_INPUT;

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_TYPEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_UPPERf,
            &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_LOWERf,
            &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            pData->vid_type = INNER_VLAN;
            break;
        case 1:
            pData->vid_type = OUTER_VLAN;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_egrVlanCnvtRangeCheckEntry_set
 * Description:
 *      Set the configuration of VID range check for egress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      setIdx - vlan range check set index
 *      index - entry index
 *      pData - configuration of Vid range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID             - invalid vid
 * Note:
 *      None
 */
int32
dal_longan_vlan_egrVlanCnvtRangeCheckEntry_set(
    uint32                                  unit,
    uint32                                  setIdx,
    uint32                                  index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t    *pData)
{
    uint32  reg;
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, index=%d", unit, index);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, setIdx=%d, index=%d,upper_bound=%d, lower_bound=%d, vid_type=%d",
        pData->vid_upper_bound,pData->vid_lower_bound, pData->vid_type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_SC2C_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(((pData->vid_type != OUTER_VLAN) && (pData->vid_type != INNER_VLAN)), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_lower_bound > pData->vid_upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_upper_bound > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    if(0 == setIdx)
        reg = LONGAN_VLAN_EGR_VID_RNG_CHK_SET_0r;
    else if (1 == setIdx)
        reg = LONGAN_VLAN_EGR_VID_RNG_CHK_SET_1r;
    else
        return RT_ERR_INPUT;

    switch (pData->vid_type)
    {
        case INNER_VLAN:
            value = 0;
            break;
        case OUTER_VLAN:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_VLAN), "invalid VID type");
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_TYPEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_UPPERf,
            &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, reg,
            REG_ARRAY_INDEX_NONE, index, LONGAN_LOWERf,
            &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_trkVlanAggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support on egress trunk port.
 * Input:
 *      unit   - unit id
 *      tid    - trunk id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_TRUNK_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_trkVlanAggrEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tid=%d", unit, tid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_TRUNK_L2TBL_CNVT_CTRLr, REG_ARRAY_INDEX_NONE, tid, LONGAN_CNVT_ENf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
    {
        case 0:
            *pEnable = DISABLED;
            break;
        case 1:
            *pEnable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=%x", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_trkVlanAggrEnable_set
 * Description:
 *      Enable N:1 VLAN aggregation support on egress trunk port.
 * Input:
 *      unit   - unit id
 *      tid    - trunk id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_vlan_trkVlanAggrEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tid=%d, enable=%d", unit, tid, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (enable)
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

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_TRUNK_L2TBL_CNVT_CTRLr, REG_ARRAY_INDEX_NONE, tid, \
                LONGAN_CNVT_ENf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_trkVlanAggrCtrl_get
 * Description:
 *      Get port vlan-aggragation configration.
 * Input:
 *      unit - unit id
 *      tid  - trunk id
 * Output:
 *      pVlanAggrCtrl - pointer to vlan-aggr ctrl
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_vlan_trkVlanAggrCtrl_get(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value1,value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tid=%d", unit, tid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK((NULL == pVlanAggrCtrl), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_TRUNK_L2TBL_CNVT_CTRLr, REG_ARRAY_INDEX_NONE, tid, \
                LONGAN_VID_SELf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, LONGAN_VLAN_TRUNK_L2TBL_CNVT_CTRLr, REG_ARRAY_INDEX_NONE, tid, \
                LONGAN_PRI_TAG_ACTf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value1)
    {
        case 0:
            pVlanAggrCtrl->vid_src = INNER_VLAN;
            break;
        case 1:
            pVlanAggrCtrl->vid_src = OUTER_VLAN;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (value2)
    {
        case 0:
            pVlanAggrCtrl->pri_src= LEARNING_VID_PRI;
            break;
        case 1:
            pVlanAggrCtrl->pri_src = LEARNING_VID_PBASED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "vid source=%d,pri source=%d", pVlanAggrCtrl->vid_src,pVlanAggrCtrl->pri_src);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_vlan_trkVlanAggrCtrl_set
 * Description:
 *      Set port vlan aggragation .
 * Input:
 *      unit - unit id
 *      tid  - trunk id
 *      vlanAggrCtrl  - vlan-aggr ctrl
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
dal_longan_vlan_trkVlanAggrCtrl_set(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t vlanAggrCtrl)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value1,value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tid=%d, vid src=%d, pri src=%d",
        unit, tid, vlanAggrCtrl.vid_src,vlanAggrCtrl.pri_src);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tid >= HAL_MAX_NUM_OF_TRUNK(unit)), RT_ERR_TRUNK_ID);
    RT_PARAM_CHK(vlanAggrCtrl.vid_src >= VLAN_TYPE_END,RT_ERR_INPUT);
    RT_PARAM_CHK(vlanAggrCtrl.pri_src >= LEARNING_VID_END,RT_ERR_INPUT);

    /* translate value to chip's definition */
    switch (vlanAggrCtrl.vid_src)
    {
        case INNER_VLAN:
            value1 = 0;
            break;
        case OUTER_VLAN:
            value1 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (vlanAggrCtrl.pri_src)
    {
        case LEARNING_VID_PRI:
            value2 = 0;
            break;
        case LEARNING_VID_PBASED:
            value2 = 1;
            break;
        default:
            return RT_ERR_FAILED;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_TRUNK_L2TBL_CNVT_CTRLr, REG_ARRAY_INDEX_NONE, tid,\
                LONGAN_VID_SELf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, LONGAN_VLAN_TRUNK_L2TBL_CNVT_CTRLr, REG_ARRAY_INDEX_NONE, tid,\
                LONGAN_PRI_TAG_ACTf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_vlan_trkVlanAggrCtrl_set */


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
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_vlan.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/vlan.h>

/*
 * Symbol Definition
 */
/* vlan information structure */
typedef struct dal_maple_vlan_info_s
{
    uint32          count;           /* count of valid vlan number    */
    rtk_bitmap_t    *pValid_lists;   /* valid bit for this table      */
    uint16          *pVid2tblindex;  /* table index of vid, 0:invalid */
} dal_maple_vlan_info_t;

/* vlan entry */
typedef struct dal_maple_vlan_data_s
{
    rtk_vlan_t  vid;
    rtk_fid_t   fid_msti;               /* filtering db and multiple spanning tree instance */
    rtk_l2_ucastLookupMode_t ucast_mode;/* L2 lookup mode for unicast traffic */
    rtk_l2_ucastLookupMode_t mcast_mode;/* L2 lookup mode for L2/IP mulicast traffic */
    uint32      profile_idx;            /* VLAN profile index */
    rtk_portmask_t  member_portmask;
    rtk_portmask_t  untag_portmask;
} dal_maple_vlan_data_t;

/*
 * Data Declaration
 */
static uint32                   vlan_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t             vlan_sem[RTK_MAX_NUM_OF_UNIT];
static dal_maple_vlan_info_t    *pDal_maple_vlan_info[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
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

#define VLANINFO_VALID_IS_SET(unit, vid)    BITMAP_IS_SET(pDal_maple_vlan_info[unit]->pValid_lists, vid)
#define VLANINFO_VALID_IS_CLEAR(unit, vid)  BITMAP_IS_CLEAR(pDal_maple_vlan_info[unit]->pValid_lists, vid)
#define VLANINFO_VALID_SET(unit, vid)       BITMAP_SET(pDal_maple_vlan_info[unit]->pValid_lists, vid)
#define VLANINFO_VALID_CLEAR(unit, vid)     BITMAP_CLEAR(pDal_maple_vlan_info[unit]->pValid_lists, vid)

#define RSRVD_VLAN_START 4000

/*
 * Function Declaration
 */
static int32 _dal_maple_setVlan(uint32 unit, dal_maple_vlan_data_t *pVlan_entry);
static int32 _dal_maple_getVlan(uint32 unit, dal_maple_vlan_data_t *pVlan_entry);
static int32 _dal_maple_vlan_init_config(uint32 unit);

/* Function Name:
 *      dal_maple_vlanMapper_init
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
dal_maple_vlanMapper_init(dal_mapper_t *pMapper)
{
    pMapper->vlan_init = dal_maple_vlan_init;
    pMapper->vlan_create = dal_maple_vlan_create;
    pMapper->vlan_destroy = dal_maple_vlan_destroy;
    pMapper->vlan_destroyAll = dal_maple_vlan_destroyAll;
    pMapper->vlan_port_add = dal_maple_vlan_port_add;
    pMapper->vlan_port_del = dal_maple_vlan_port_del;
    pMapper->vlan_port_get = dal_maple_vlan_port_get;
    pMapper->vlan_port_set = dal_maple_vlan_port_set;
    pMapper->vlan_stg_get = dal_maple_vlan_stg_get;
    pMapper->vlan_stg_set = dal_maple_vlan_stg_set;
    pMapper->vlan_l2LookupMode_get = dal_maple_vlan_l2LookupMode_get;
    pMapper->vlan_l2LookupMode_set = dal_maple_vlan_l2LookupMode_set;
    pMapper->vlan_profileIdx_get = dal_maple_vlan_profileIdx_get;
    pMapper->vlan_profileIdx_set = dal_maple_vlan_profileIdx_set;
    pMapper->vlan_profile_get = dal_maple_vlan_profile_get;
    pMapper->vlan_profile_set = dal_maple_vlan_profile_set;
    pMapper->vlan_portFwdVlan_get = dal_maple_vlan_portFwdVlan_get;
    pMapper->vlan_portFwdVlan_set = dal_maple_vlan_portFwdVlan_set;
    pMapper->vlan_portAcceptFrameType_get = dal_maple_vlan_portAcceptFrameType_get;
    pMapper->vlan_portAcceptFrameType_set = dal_maple_vlan_portAcceptFrameType_set;
    pMapper->vlan_portIgrFilter_get = dal_maple_vlan_portIgrFilter_get;
    pMapper->vlan_portIgrFilter_set = dal_maple_vlan_portIgrFilter_set;
    pMapper->vlan_portEgrFilterEnable_get = dal_maple_vlan_portEgrFilterEnable_get;
    pMapper->vlan_portEgrFilterEnable_set = dal_maple_vlan_portEgrFilterEnable_set;
    pMapper->vlan_mcastLeakyEnable_get = dal_maple_vlan_mcastLeakyEnable_get;
    pMapper->vlan_mcastLeakyEnable_set = dal_maple_vlan_mcastLeakyEnable_set;
    pMapper->vlan_portPvidMode_get = dal_maple_vlan_portPvidMode_get;
    pMapper->vlan_portPvidMode_set = dal_maple_vlan_portPvidMode_set;
    pMapper->vlan_portPvid_get = dal_maple_vlan_portPvid_get;
    pMapper->vlan_portPvid_set = dal_maple_vlan_portPvid_set;
    pMapper->vlan_tpidEntry_get = dal_maple_vlan_tpidEntry_get;
    pMapper->vlan_tpidEntry_set = dal_maple_vlan_tpidEntry_set;
    pMapper->vlan_portIgrTpid_get = dal_maple_vlan_portIgrTpid_get;
    pMapper->vlan_portIgrTpid_set = dal_maple_vlan_portIgrTpid_set;
    pMapper->vlan_portEgrTpid_get = dal_maple_vlan_portEgrTpid_get;
    pMapper->vlan_portEgrTpid_set = dal_maple_vlan_portEgrTpid_set;
    pMapper->vlan_portIgrExtraTagEnable_get = dal_maple_vlan_portIgrExtraTagEnable_get;
    pMapper->vlan_portIgrExtraTagEnable_set = dal_maple_vlan_portIgrExtraTagEnable_set;
    pMapper->vlan_portEgrTagSts_get = dal_maple_vlan_portEgrTagSts_get;
    pMapper->vlan_portEgrTagSts_set = dal_maple_vlan_portEgrTagSts_set;
    pMapper->vlan_portIgrVlanTransparentEnable_get = dal_maple_vlan_portIgrVlanTransparentEnable_get;
    pMapper->vlan_portIgrVlanTransparentEnable_set = dal_maple_vlan_portIgrVlanTransparentEnable_set;
    pMapper->vlan_portEgrVlanTransparentEnable_get = dal_maple_vlan_portEgrVlanTransparentEnable_get;
    pMapper->vlan_portEgrVlanTransparentEnable_set = dal_maple_vlan_portEgrVlanTransparentEnable_set;
    pMapper->vlan_egrVlanCnvtEntry_get = dal_maple_vlan_egrVlanCnvtEntry_get;
    pMapper->vlan_egrVlanCnvtEntry_set = dal_maple_vlan_egrVlanCnvtEntry_set;
    pMapper->vlan_portVlanAggrEnable_get = dal_maple_vlan_portVlanAggrEnable_get;
    pMapper->vlan_portVlanAggrEnable_set = dal_maple_vlan_portVlanAggrEnable_set;
    pMapper->vlan_portVlanAggrCtrl_get = dal_maple_vlan_portVlanAggrCtrl_get;
    pMapper->vlan_portVlanAggrCtrl_set = dal_maple_vlan_portVlanAggrCtrl_set;
    pMapper->vlan_leakyStpFilter_get = dal_maple_vlan_leakyStpFilter_get;
    pMapper->vlan_leakyStpFilter_set = dal_maple_vlan_leakyStpFilter_set;
    pMapper->vlan_portEgrVlanCnvtLuMisAct_get = dal_maple_vlan_portEgrVlanCnvtLuMisAct_get;
    pMapper->vlan_portEgrVlanCnvtLuMisAct_set = dal_maple_vlan_portEgrVlanCnvtLuMisAct_set;
    pMapper->vlan_egrVlanCnvtEntry_delAll = dal_maple_vlan_egrVlanCnvtEntry_delAll;
    pMapper->vlan_portIgrTagKeepType_get = dal_maple_vlan_portIgrTagKeepType_get;
    pMapper->vlan_portIgrTagKeepType_set = dal_maple_vlan_portIgrTagKeepType_set;
    pMapper->vlan_portEgrTagKeepType_get = dal_maple_vlan_portEgrTagKeepType_get;
    pMapper->vlan_portEgrTagKeepType_set = dal_maple_vlan_portEgrTagKeepType_set;
    pMapper->vlan_portEgrVlanCnvtVidSource_get = dal_maple_vlan_portEgrVlanCnvtVidSource_get;
    pMapper->vlan_portEgrVlanCnvtVidSource_set = dal_maple_vlan_portEgrVlanCnvtVidSource_set;
    pMapper->vlan_portEgrVlanCnvtVidTarget_get = dal_maple_vlan_portEgrVlanCnvtVidTarget_get;
    pMapper->vlan_portEgrVlanCnvtVidTarget_set = dal_maple_vlan_portEgrVlanCnvtVidTarget_set;
    pMapper->vlan_egrVlanCnvtRangeCheckEntry_get = dal_maple_vlan_egrVlanCnvtRangeCheckEntry_get;
    pMapper->vlan_egrVlanCnvtRangeCheckEntry_set = dal_maple_vlan_egrVlanCnvtRangeCheckEntry_set;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_vlan_init
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
dal_maple_vlan_init(uint32 unit)
{
    int32   ret;
    uint32  vlan_tableSize;

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
    if ((ret = table_size_get(unit, MAPLE_VLANt, &vlan_tableSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "table size get failed");
        return ret;
    }

    VLAN_SEM_LOCK(unit);

    /* allocate memory for each database and initilize database */
    pDal_maple_vlan_info[unit] = (dal_maple_vlan_info_t *)osal_alloc(sizeof(dal_maple_vlan_info_t));
    if (0 == pDal_maple_vlan_info[unit])
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_FAILED, (MOD_VLAN|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pDal_maple_vlan_info[unit], 0, sizeof(dal_maple_vlan_info_t));

    pDal_maple_vlan_info[unit]->pValid_lists = (rtk_bitmap_t *)osal_alloc(BITMAP_ARRAY_SIZE(vlan_tableSize));
    if (0 == pDal_maple_vlan_info[unit]->pValid_lists)
    {
        osal_free(pDal_maple_vlan_info[unit]);
        pDal_maple_vlan_info[unit] = 0;
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_FAILED, (MOD_VLAN|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pDal_maple_vlan_info[unit]->pValid_lists, 0, BITMAP_ARRAY_SIZE(vlan_tableSize));

    pDal_maple_vlan_info[unit]->pVid2tblindex = (uint16 *)osal_alloc(vlan_tableSize * sizeof(uint16));
    if (0 == pDal_maple_vlan_info[unit]->pVid2tblindex)
    {
        osal_free(pDal_maple_vlan_info[unit]->pValid_lists);
        pDal_maple_vlan_info[unit]->pValid_lists = 0;
        osal_free(pDal_maple_vlan_info[unit]);
        pDal_maple_vlan_info[unit] = 0;
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_FAILED, (MOD_VLAN|MOD_DAL), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pDal_maple_vlan_info[unit]->pVid2tblindex, 0, (vlan_tableSize * sizeof(uint16)));
    VLAN_SEM_UNLOCK(unit);

    /* set init flag to complete init */
    vlan_init[unit] = INIT_COMPLETED;

    if ((ret = _dal_maple_vlan_init_config(unit)) != RT_ERR_OK)
    {
        vlan_init[unit] = INIT_NOT_COMPLETED;
        osal_free(pDal_maple_vlan_info[unit]->pVid2tblindex);
        pDal_maple_vlan_info[unit]->pVid2tblindex = 0;
        osal_free(pDal_maple_vlan_info[unit]->pValid_lists);
        pDal_maple_vlan_info[unit]->pValid_lists = 0;
        osal_free(pDal_maple_vlan_info[unit]);
        pDal_maple_vlan_info[unit] = 0;
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "Init default vlan config failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_vlan_init */

/* Function Name:
 *      dal_maple_vlan_create
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
 *      1) Default FID/STG is assigned to CIST after vlan creation and
 *         FID/STG can be re-assigned later by dal_maple_vlan_stg_set
 *      2) Default lookup mode for L2 unicast and L2/IP multicast is assigned
 *         to be based on VID
 */
int32
dal_maple_vlan_create(uint32 unit, rtk_vlan_t vid)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(VLANINFO_VALID_IS_SET(unit, vid), RT_ERR_VLAN_EXIST);

    /* create vlan in CHIP*/
    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid      = vid;
    vlan_data_entry.fid_msti = RTK_DEFAULT_MSTI;

    VLAN_SEM_LOCK(unit);

    /* write entry to CHIP*/
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* Set vlan valid bit in software */
    VLANINFO_VALID_SET(unit, vid);
    pDal_maple_vlan_info[unit]->count++;

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_create */

/* Function Name:
 *      dal_maple_vlan_destroy
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
dal_maple_vlan_destroy(uint32 unit, rtk_vlan_t vid)
{
    int32 ret;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);
    /* write entry to CHIP*/
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* Unset vlan valid bit in software */
    VLANINFO_VALID_CLEAR(unit, vid);
    pDal_maple_vlan_info[unit]->count--;

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maplew_vlan_destroy */

/* Function Name:
 *      dal_maple_vlan_destroyAll
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
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
int32
dal_maple_vlan_destroyAll(uint32 unit, uint32 keep_and_restore_default_vlan)
{
    int32       ret;
    dal_maple_vlan_data_t vlan_data_entry;
    rtk_vlan_t  vid;
    uint32      vlan_tableSize;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, keep_and_restore_default_vlan=%d",
           unit, keep_and_restore_default_vlan);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    if ((ret = table_size_get(unit, MAPLE_VLANt, &vlan_tableSize)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "table size get failed");
        return ret;
    }

    for (vid = 0; vid < vlan_tableSize; vid++)
    {
        if (VLANINFO_VALID_IS_SET(unit, vid))
        {
            if ((ret = dal_maple_vlan_destroy(unit, vid)) != RT_ERR_OK)
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
        vlan_data_entry.fid_msti    = RTK_DEFAULT_MSTI;
        HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.member_portmask);
        HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.untag_portmask);

        if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }

        /* Set vlan valid bit in software */
        VLANINFO_VALID_SET(unit, RTK_DEFAULT_VLAN_ID);
        pDal_maple_vlan_info[unit]->count++;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_destroyAll */

/* Function Name:
 *      dal_maple_vlan_port_add
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
 *      1) The valid value of is_untag are {0: tagged, 1: untagged}
 */
int32
dal_maple_vlan_port_add(uint32 unit, rtk_vlan_t vid, rtk_port_t port, uint32 is_untag)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

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
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
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
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_port_add */

/* Function Name:
 *      dal_maple_vlan_port_del
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
dal_maple_vlan_port_del(uint32 unit, rtk_vlan_t vid, rtk_port_t port)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

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
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* clear member_portmask and untag_portmask */
    RTK_PORTMASK_PORT_CLEAR(vlan_data_entry.member_portmask, port);
    RTK_PORTMASK_PORT_CLEAR(vlan_data_entry.untag_portmask, port);

    /* programming to chip */
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_port_del */

/* Function Name:
 *      dal_maple_vlan_port_get
 * Description:
 *      Get the vlan members from the specified device.
 * Input:
 *      unit             - unit id
 *      vid              - vlan id
 * Output:
 *      pMember_portmask - pointer buffer of member ports
 *      pUntag_portmask  - pointer buffer of untagged member ports
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
dal_maple_vlan_port_get(
    uint32         unit,
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(((NULL == pMember_portmask) || (NULL == pUntag_portmask)), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
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
} /* end of dal_maple_vlan_port_get */

/* Function Name:
 *      dal_maple_vlan_port_set
 * Description:
 *      Replace the vlan members in the specified device.
 * Input:
 *      unit             - unit id
 *      vid              - vlan id
 *      pMember_portmask - member ports
 *      pUntag_portmask  - untagged member ports
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
dal_maple_vlan_port_set(
    uint32         unit,
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

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
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* copy new portmask to vlan data entry */
    RTK_PORTMASK_ASSIGN(vlan_data_entry.member_portmask, *pMember_portmask);
    RTK_PORTMASK_ASSIGN(vlan_data_entry.untag_portmask, *pUntag_portmask);

    /* programming to chip */
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_port_set */

/* Function Name:
 *      dal_maple_vlan_stg_get
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
 *      STG instance also represents FID in RTL8380
 */
int32
dal_maple_vlan_stg_get(uint32 unit, rtk_vlan_t vid, rtk_stg_t *pStg)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

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
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    /* set fid/msti to new fid/msti */
    *pStg = vlan_data_entry.fid_msti;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pStg=%d", *pStg);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_stg_get */

/* Function Name:
 *      dal_maple_vlan_stg_set
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
 *      STG instance also represents FID in RTL8380
 */
int32
dal_maple_vlan_stg_set(uint32 unit, rtk_vlan_t vid, rtk_stg_t stg)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, stg=%d", unit, vid, stg);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((stg >= HAL_MAX_NUM_OF_MSTI(unit)), RT_ERR_MSTI);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set fid/msti to new fid/msti */
    vlan_data_entry.fid_msti = stg;

    /* programming to chip */
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_stg_set */

/* Function Name:
 *      dal_maple_vlan_l2UcastLookupMode_get
 * Description:
 *      Get L2 lookup mode for L2 unicast traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
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
 *      Each VLAN can have its own lookup mode for L2 unicast traffic
 */
int32
dal_maple_vlan_l2UcastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_l2_ucastLookupMode_t *pMode)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    *pMode = vlan_data_entry.ucast_mode;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pMode=%d", *pMode);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_l2UcastLookupMode_get */

/* Function Name:
 *      dal_maple_vlan_l2UcastLookupMode_set
 * Description:
 *      Set L2 lookup mode for L2 unicast traffic.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      mode - lookup mode
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
 *      Each VLAN can have its own lookup mode for L2 unicast traffic
 */
int32
dal_maple_vlan_l2UcastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_l2_ucastLookupMode_t mode)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, mode=%d", unit, vid, mode);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(mode >= UC_LOOKUP_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* configure vlan data entry */
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    vlan_data_entry.ucast_mode = mode;

    /* programming to chip */
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_l2UcastLookupMode_set */

/* Function Name:
 *      dal_maple_vlan_l2McastLookupMode_get
 * Description:
 *      Get L2 lookup mode for L2 multicast and IP multicast traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
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
 *      Each VLAN can have its own lookup mode for L2 mulicast and IP multicast traffic
 */
int32
dal_maple_vlan_l2McastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_l2_mcastLookupMode_t *pMode)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, vid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (vlan_data_entry.mcast_mode)
    {
        case 0:
            *pMode = MC_LOOKUP_ON_VID;
            break;
        case 1:
            *pMode = MC_LOOKUP_ON_FID;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pMode=%d", *pMode);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_l2McastLookupMode_get */

/* Function Name:
 *      dal_maple_vlan_l2McastLookupMode_set
 * Description:
 *      Set L2 lookup mode for L2 multicast and IP multicast traffic.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      mode - lookup mode
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
 *      Each VLAN can have its own lookup mode for L2 mulicast and IP multicast traffic
 */
int32
dal_maple_vlan_l2McastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_l2_mcastLookupMode_t mode)
{
    int32   ret;
    uint32  value;
    dal_maple_vlan_data_t vlan_data_entry;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, mode=%d", unit, vid, mode);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* VID check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(mode >= MC_LOOKUP_END, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));

    /* translate to chip's definition */
    switch (mode)
    {
        case MC_LOOKUP_ON_VID:
            value = 0;
            break;
        case MC_LOOKUP_ON_FID:
            value = 1;
            break;
        default:
            return RT_ERR_OUT_OF_RANGE;
    }

    /* configure vlan data entry */
    vlan_data_entry.vid = vid;

    VLAN_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    vlan_data_entry.mcast_mode = value;

    /* programming to chip */
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_l2McastLookupMode_set */

/* Function Name:
 *      dal_maple_vlan_profileIdx_get
 * Description:
 *      Get VLAN profile index of specified VLAN.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 * Output:
 *      pIdx - VLAN profile index
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
dal_maple_vlan_profileIdx_get(uint32 unit, rtk_vlan_t vid, uint32 *pIdx)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

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
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    *pIdx = vlan_data_entry.profile_idx;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pIdx=%d", *pIdx);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_profileIdx_get */

/* Function Name:
 *      dal_maple_vlan_profileIdx_set
 * Description:
 *      Set VLAN profile index of specified VLAN.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      idx  - VLAN profile index
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
dal_maple_vlan_profileIdx_set(uint32 unit, rtk_vlan_t vid, uint32 idx)
{
    int32   ret;
    dal_maple_vlan_data_t vlan_data_entry;

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
    if ((ret = _dal_maple_getVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    vlan_data_entry.profile_idx = idx;

    /* programming to chip */
    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_profileIdx_set */

/* Function Name:
 *      dal_maple_vlan_profile_get
 * Description:
 *      Get specific VLAN profile.
 * Input:
 *      unit     - unit id
 *      idx      - VLAN profile index
 * Output:
 *      pProfile - VLAN profile configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - profile index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Totally 8 profile (0~7) are supported
 */
int32
dal_maple_vlan_profile_get(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, idx=%d", unit, idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(idx >= HAL_MAX_NUM_OF_VLAN_PROFILE(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pProfile), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_read(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_L2_LRN_ENf, &pProfile->learn), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_L2_UNKN_MC_FLD_PMSKf, &pProfile->l2_mcast_dlf_pm_idx), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_IP4_UNKN_MC_FLD_PMSKf, &pProfile->ip4_mcast_dlf_pm_idx), errHandle, ret);
    RT_ERR_HDL(reg_array_field_read(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_IP6_UNKN_MC_FLD_PMSKf, &pProfile->ip6_mcast_dlf_pm_idx), errHandle, ret);

    VLAN_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "learning=%d, l2_mcast_dlf_pm_idx=%d, ip4_mcast_dlf_pm_idx=%d,\
        ip6_mcast_dlf_pm_idx=%d", pProfile->learn, pProfile->l2_mcast_dlf_pm_idx,\
        pProfile->ip4_mcast_dlf_pm_idx, pProfile->ip6_mcast_dlf_pm_idx);
    return RT_ERR_OK;

errHandle:
    VLAN_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");

    return ret;
} /* end of dal_maple_vlan_profile_get */

/* Function Name:
 *      dal_maple_vlan_profile_set
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - profile index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Totally 8 profile (0~7) are supported
 */
int32
dal_maple_vlan_profile_set(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "learning=%d, l2_mcast_dlf_pm_idx=%d, ip4_mcast_dlf_pm_idx=%d,\
        ip6_mcast_dlf_pm_idx=%d", pProfile->learn, pProfile->l2_mcast_dlf_pm_idx,\
        pProfile->ip4_mcast_dlf_pm_idx, pProfile->ip6_mcast_dlf_pm_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(idx >= HAL_MAX_NUM_OF_VLAN_PROFILE(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pProfile), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pProfile->learn > 1, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pProfile->l2_mcast_dlf_pm_idx >= HAL_MAX_NUM_OF_MCAST_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pProfile->ip4_mcast_dlf_pm_idx >= HAL_MAX_NUM_OF_MCAST_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pProfile->ip6_mcast_dlf_pm_idx >= HAL_MAX_NUM_OF_MCAST_ENTRY(unit), RT_ERR_OUT_OF_RANGE);

    VLAN_SEM_LOCK(unit);

    RT_ERR_HDL(reg_array_field_write(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_L2_LRN_ENf, &pProfile->learn), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_L2_UNKN_MC_FLD_PMSKf, &pProfile->l2_mcast_dlf_pm_idx), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_IP4_UNKN_MC_FLD_PMSKf, &pProfile->ip4_mcast_dlf_pm_idx), errHandle, ret);
    RT_ERR_HDL(reg_array_field_write(unit, MAPLE_VLAN_PROFILEr, REG_ARRAY_INDEX_NONE, idx,\
                          MAPLE_IP6_UNKN_MC_FLD_PMSKf, &pProfile->ip6_mcast_dlf_pm_idx), errHandle, ret);

    VLAN_SEM_UNLOCK(unit);
    return RT_ERR_OK;

errHandle:
    VLAN_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
} /* end of dal_maple_vlan_profile_set */


/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      dal_maple_vlan_portInnerAcceptFrameType_get
 * Description:
 *      Get inner vlan accept frame type of the port from the specified device.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pAccept_frame_type  - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The API is used for 802.1Q tagged  and if you want to get the 802.1ad
 *      accept frame type, please use dal_maple_vlan_portOuterAcceptFrameType_get
 */
int32
dal_maple_vlan_portInnerAcceptFrameType_get(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    int32   ret;
    uint32  accept_tag, accept_untag;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAccept_frame_type), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_ITAG_ACCEPTf,
                          &accept_tag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_ITAG_UNTAG_ACCEPTf,
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
} /* end of dal_maple_vlan_portAcceptFrameType_get */

/* Function Name:
 *      dal_maple_vlan_portInnerAcceptFrameType_set
 * Description:
 *      Set inner vlan accept frame type of the port to the specified device.
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
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 * Note:
 *      The API is used for 802.1Q tagged  and if you want to set the 802.1ad
 *      accept frame type, please use dal_maple_vlan_portOuterAcceptFrameType_set
 */
int32
dal_maple_vlan_portInnerAcceptFrameType_set(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t accept_frame_type)
{
    int32   ret;
    uint32  accept_tag, accept_untag;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, accept_frame_type=%d",
           unit, port, accept_frame_type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
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

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_ITAG_ACCEPTf,
                          &accept_tag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_ITAG_UNTAG_ACCEPTf,
                          &accept_untag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portAcceptFrameType_set */

/* Function Name:
 *      dal_maple_vlan_portOuterAcceptFrameType_get
 * Description:
 *      Get outer vlan accept frame type of the port from the specified device.
 * Input:
 *      unit               - unit id
 *      port               - port id
 * Output:
 *      pAccept_frame_type - pointer buffer of accept frame type
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
dal_maple_vlan_portOuterAcceptFrameType_get(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    int32   ret;
    uint32  accept_tag, accept_untag;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAccept_frame_type), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OTAG_ACCEPTf,
                          &accept_tag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OTAG_UNTAG_ACCEPTf,
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
} /* end of dal_maple_vlan_portOuterAcceptFrameType_get */

/* Function Name:
 *      dal_maple_vlan_portOuterAcceptFrameType_set
 * Description:
 *      Set outer vlan accept frame type of the port to the specified device.
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
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 * Note:
 *      None
 */
int32
dal_maple_vlan_portOuterAcceptFrameType_set(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t accept_frame_type)
{
    int32   ret;
    uint32  accept_tag, accept_untag;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, accept_frame_type=%d",
           unit, port, accept_frame_type);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
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

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OTAG_ACCEPTf,
                          &accept_tag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_ACCEPT_FRAME_TYPEr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OTAG_UNTAG_ACCEPTf,
                          &accept_untag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portOuterAcceptFrameType_set */

/* Function Name:
 *      dal_maple_vlan_portIgrFilter_get
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
dal_maple_vlan_portIgrFilter_get(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t *pIgr_filter)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIgr_filter), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_IGR_FLTRr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_FLTR_ACTf, &value)) != RT_ERR_OK)
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
} /* end of dal_maple_vlan_portIgrFilter_get */

/* Function Name:
 *      dal_maple_vlan_portIgrFilter_set
 * Description:
 *      Set vlan ingress filter configuration of the port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      igr_filter - ingress filter configuration
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
dal_maple_vlan_portIgrFilter_set(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t igr_filter)
{
    int32   ret;
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
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_IGR_FLTRr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_FLTR_ACTf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrFilter_set */

/* Function Name:
 *      dal_maple_vlan_portEgrFilterEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_vlan_portEgrFilterEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_EGR_FLTRr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_FLTR_ENf, &value)) != RT_ERR_OK)
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
} /* end of dal_maple_vlan_portEgrFilterEnable_get */

/* Function Name:
 *      dal_maple_vlan_portEgrFilterEnable_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_vlan_portEgrFilterEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
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
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_EGR_FLTRr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_FLTR_ENf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrFilterEnable_set */

/* Function Name:
 *      dal_maple_vlan_mcastLeakyEnable_get
 * Description:
 *      Get vlan egress leaky status of the system from the specified device.
 * Input:
 *      unit   - unit id
 * Output:
 *      pLeaky - pointer buffer of vlan leaky of egress
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
dal_maple_vlan_mcastLeakyEnable_get(uint32 unit, rtk_enable_t *pLeaky)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLeaky), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_VLAN_CTRLr, MAPLE_LKYf, pLeaky)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pLeaky=%d", *pLeaky);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_mcastLeakyEnable_get */

/* Function Name:
 *      dal_maple_vlan_mcastLeakyEnable_set
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
dal_maple_vlan_mcastLeakyEnable_set(uint32 unit, rtk_enable_t leaky)
{
    int32   ret;
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
    if ((ret = reg_field_write(unit, MAPLE_VLAN_CTRLr, MAPLE_LKYf, &en_leaky)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_mcastLeakyEnable_set */

/* Function Name:
 *      dal_maple_vlan_portInnerPvidMode_get
 * Description:
 *      Get the configuration of inner port-based VLAN mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to inner port-based VLAN mode configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Inner port-based VLAN can apply on different tag format, refer to
 *      rtk_vlan_pbVlan_mode_t
 */
int32
dal_maple_vlan_portInnerPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(NULL == pMode, RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_IPVID_FMTf,
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
} /* end of dal_maple_vlan_portPvidMode_get */

/* Function Name:
 *      dal_maple_vlan_portInnerPvidMode_set
 * Description:
 *      Set the configuration of inner port-based VLAN mode.
 * Input:
 *      unit - unit id
 *      port - port id
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
 *      Inner port-based VLAN can apply on different tag format, refer to
 *      rtk_vlan_pbVlan_mode_t
 */
int32
dal_maple_vlan_portInnerPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
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

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_IPVID_FMTf,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portPvidMode_set */

/* Function Name:
 *      dal_maple_vlan_portOuterPvidMode_get
 * Description:
 *      Get the configuration of outer port-based VLAN mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to outer port-based VLAN mode configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Outer port-based VLAN can apply on different tag format, refer to
 *      rtk_vlan_pbVlan_mode_t
 */
int32
dal_maple_vlan_portOuterPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(NULL == pMode, RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OPVID_FMTf,
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
} /* end of dal_maple_vlan_portOuterPvidMode_get */

/* Function Name:
 *      dal_maple_vlan_portOuterPvidMode_set
 * Description:
 *      Set the configuration of outer port-based VLAN mode.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - outer port-based VLAN mode configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Outer port-based VLAN can apply on different tag format, refer to
 *      rtk_vlan_pbVlan_mode_t
 */
int32
dal_maple_vlan_portOuterPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
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

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OPVID_FMTf,
                          &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portOuterPvidMode_set */

/* Function Name:
 *      dal_maple_vlan_portInnerPvid_get
 * Description:
 *      Get port default inner vlan id from the specified device.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default inner vlan id
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
dal_maple_vlan_portInnerPvid_get(uint32 unit, rtk_port_t port, uint32 *pPvid)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_IPVIDf,
                          pPvid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "inner pPvid=%d", *pPvid);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portPvid_get */

/* Function Name:
 *      dal_maple_vlan_portInnerPvid_set
 * Description:
 *      Set port default inner vlan id to the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
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
dal_maple_vlan_portInnerPvid_set(uint32 unit, rtk_port_t port, uint32 pvid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, inner pvid=%d",
           unit, port, pvid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_IPVIDf,
                          &pvid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portPvid_set */

/* Function Name:
 *      dal_maple_vlan_portOuterPvid_get
 * Description:
 *      Get port default outer vlan id from the specified device.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default outer vlan id
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
dal_maple_vlan_portOuterPvid_get(uint32 unit, rtk_port_t port, uint32 *pPvid)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OPVIDf,
                          pPvid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "outer pPvid=%d", *pPvid);
    return RT_ERR_OK;
} /* end of dal_maple_vlan_portOuterPvid_get */

/* Function Name:
 *      dal_maple_vlan_portOuterPvid_set
 * Description:
 *      Set port default outer vlan id to the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pvid - port default outer vlan id
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
dal_maple_vlan_portOuterPvid_set(uint32 unit, rtk_port_t port, uint32 pvid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, inner pvid=%d",
           unit, port, pvid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pvid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);

    VLAN_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit,
                          MAPLE_VLAN_PORT_PB_VLANr,
                          port,
                          REG_ARRAY_INDEX_NONE,
                          MAPLE_OPVIDf,
                          &pvid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portOuterPvid_set */

/* Function Name:
 *      dal_maple_vlan_portIgrTagKeepEnable_get
 * Description:
 *      Get enable status of keep tag format at ingress.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pKeepOuter - enable status of keep outer tag format
 *      pKeepInner - enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_maple_vlan_portIgrTagKeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pKeepOuter, rtk_enable_t *pKeepInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pKeepOuter) || (NULL == pKeepInner), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
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
            *pKeepOuter = DISABLED;
            break;
        case 1:
        case 2:
            *pKeepOuter = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (value2)
    {
        case 0:
            *pKeepInner = DISABLED;
            break;
        case 1:
        case 2:
            *pKeepInner = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pKeepOuter=%s, pKeepInner=%s", (*pKeepOuter)?"ENABLED":"DISABLED", (*pKeepInner)?"ENABLED":"DISABLED");

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrTagKeepEnable_get */

/* Function Name:
 *      dal_maple_vlan_portIgrTagKeepEnable_set
 * Description:
 *      Set enable status of keep tag format at ingress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      keepOuter - enable status of keep outer tag format
 *      keepInner - enable status of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_maple_vlan_portIgrTagKeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t keepOuter, rtk_enable_t keepInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, keepOuter=%d, keepInner=%d",
          unit, port, keepOuter, keepInner);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((keepOuter >= RTK_ENABLE_END) || (keepInner >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (keepOuter)
    {
        case DISABLED:
            value1 = 0;
            break;
        case ENABLED:
            value1 = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (keepInner)
    {
        case DISABLED:
            value2 = 0;
            break;
        case ENABLED:
            value2 = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrTagKeepEnable_set */

/* Function Name:
 *      dal_maple_vlan_portEgrTagKeepEnable_get
 * Description:
 *      Get enable status of keep tag format at egress.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pKeepOuter - pointer to enable status of keep outer tag format
 *      pKeepInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_maple_vlan_portEgrTagKeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pKeepOuter, rtk_enable_t *pKeepInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pKeepOuter) || (NULL == pKeepInner), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
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
            *pKeepOuter = DISABLED;
            break;
        case 1:
        case 2:
            *pKeepOuter = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (value2)
    {
        case 0:
            *pKeepInner = DISABLED;
            break;
        case 1:
        case 2:
            *pKeepInner = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pKeepOuter=%s, pKeepInner=%s", (*pKeepOuter)?"ENABLED":"DISABLED", (*pKeepInner)?"ENABLED":"DISABLED");

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrTagKeepEnable_get */

/* Function Name:
 *      dal_maple_vlan_portEgrTagKeepEnable_set
 * Description:
 *      Set enable status of keep tag format at egress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      keepOuter - enable status of keep outer tag format
 *      keepInner - enable status of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_maple_vlan_portEgrTagKeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t keepOuter, rtk_enable_t keepInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, keepOuter=%d, keepInner=%d",
          unit, port, keepOuter, keepInner);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((keepOuter >= RTK_ENABLE_END) || (keepInner >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (keepOuter)
    {
        case DISABLED:
            value1 = 0;
            break;
        case ENABLED:
            value1 = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (keepInner)
    {
        case DISABLED:
            value2 = 0;
            break;
        case ENABLED:
            value2 = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrTagKeepEnable_set */

/* Function Name:
 *      dal_maple_vlan_portIgrTagKeepType_get
 * Description:
 *      Get keep type of keep tag format at ingress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pKeeptypeOuter - pointer to enable status of keep outer tag format
 *      pKeeptypeInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_maple_vlan_portIgrTagKeepType_get(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t *pKeeptypeOuter, rtk_vlan_tagKeepType_t *pKeeptypeInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pKeeptypeOuter) || (NULL == pKeeptypeInner), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
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
            *pKeeptypeOuter = TAG_KEEP_TYPE_NOKEEP;
            break;
        case 1:
            *pKeeptypeOuter = TAG_KEEP_TYPE_FORMAT;
            break;
        case 2:
            *pKeeptypeOuter = TAG_KEEP_TYPE_CONTENT;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (value2)
    {
        case 0:
            *pKeeptypeInner = TAG_KEEP_TYPE_NOKEEP;
            break;
        case 1:
            *pKeeptypeInner = TAG_KEEP_TYPE_FORMAT;
            break;
        case 2:
            *pKeeptypeInner = TAG_KEEP_TYPE_CONTENT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pKeeptypeOuter=%s, pKeeptypeInner=%s", \
        ((*pKeeptypeOuter) - 1)?"KEEP CONTENT":"KEEP FORMAT", ((*pKeeptypeInner) - 1)?"KEEP CONTENT":"KEEP FORMAT");

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrTagKeepType_get */

/* Function Name:
 *      dal_maple_vlan_portIgrTagKeepType_set
 * Description:
 *      Set keep type of keep tag format at ingress.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      keeptypeOuter - keep type of keep outer tag format
 *      keeptypeInner - keep type of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Set keep type of keep tag format at ingress
 *      - Set keep type of keep tag format at egress
 */
int32
dal_maple_vlan_portIgrTagKeepType_set(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t keeptypeOuter, rtk_vlan_tagKeepType_t keeptypeInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, keeptypeOuter=%d, keeptypeInner=%d",
          unit, port, keeptypeOuter, keeptypeInner);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((keeptypeOuter >= TAG_KEEP_TYPE_END) || (keeptypeInner >= TAG_KEEP_TYPE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (keeptypeOuter)
    {
        case TAG_KEEP_TYPE_NOKEEP:
            value1 = 0;
            break;
        case TAG_KEEP_TYPE_FORMAT:
            value1 = 1;
            break;
        case TAG_KEEP_TYPE_CONTENT:
            value1 = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (keeptypeInner)
    {
        case TAG_KEEP_TYPE_NOKEEP:
            value2 = 0;
            break;
        case TAG_KEEP_TYPE_FORMAT:
            value2 = 1;
            break;
        case TAG_KEEP_TYPE_CONTENT:
            value2 = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_IGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrTagKeepType_set */

/* Function Name:
 *      dal_maple_vlan_portEgrTagKeepType_get
 * Description:
 *      Get keep type of keep tag format at egress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pKeeptypeOuter - pointer to enable status of keep outer tag format
 *      pKeeptypeInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_maple_vlan_portEgrTagKeepType_get(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t *pKeeptypeOuter, rtk_vlan_tagKeepType_t *pKeeptypeInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pKeeptypeOuter) || (NULL == pKeeptypeInner), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
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
            *pKeeptypeOuter = TAG_KEEP_TYPE_NOKEEP;
            break;
        case 1:
            *pKeeptypeOuter = TAG_KEEP_TYPE_FORMAT;
            break;
        case 2:
            *pKeeptypeOuter = TAG_KEEP_TYPE_CONTENT;
            break;
        default:
            return RT_ERR_FAILED;
    }
    switch (value2)
    {
        case 0:
            *pKeeptypeInner = TAG_KEEP_TYPE_NOKEEP;
            break;
        case 1:
            *pKeeptypeInner = TAG_KEEP_TYPE_FORMAT;
            break;
        case 2:
            *pKeeptypeInner = TAG_KEEP_TYPE_CONTENT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pKeeptypeOuter=%s, pKeeptypeInner=%s", \
        ((*pKeeptypeOuter) - 1)?"ALWAYS KEEP":"KEEP FORMAT", ((*pKeeptypeInner) - 1)?"ALWAYS KEEP":"KEEP FORMAT");

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrTagKeepType_get */

/* Function Name:
 *      dal_maple_vlan_portEgrTagKeepType_set
 * Description:
 *      Set keep type of keep tag format at egress.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      keeptypeOuter - keep type of keep outer tag format
 *      keeptypeInner - keep type of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Set keep type of keep tag format at ingress
 *      - Set keep type of keep tag format at egress
 */
int32
dal_maple_vlan_portEgrTagKeepType_set(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t keeptypeOuter, rtk_vlan_tagKeepType_t keeptypeInner)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, keeptypeOuter=%d, keeptypeInner=%d",
          unit, port, keeptypeOuter, keeptypeInner);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((keeptypeOuter >= TAG_KEEP_TYPE_END) || (keeptypeInner >= TAG_KEEP_TYPE_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (keeptypeOuter)
    {
        case TAG_KEEP_TYPE_NOKEEP:
            value1 = 0;
            break;
        case TAG_KEEP_TYPE_FORMAT:
            value1 = 1;
            break;
        case TAG_KEEP_TYPE_CONTENT:
            value1 = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }
    switch (keeptypeInner)
    {
        case TAG_KEEP_TYPE_NOKEEP:
            value2 = 0;
            break;
        case TAG_KEEP_TYPE_FORMAT:
            value2 = 1;
            break;
        case TAG_KEEP_TYPE_CONTENT:
            value2 = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_OTAG_KEEPf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_EGR_P_ITAG_KEEPf, &value2)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrTagKeepType_set */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtDblTagEnable_get
 * Description:
 *      Enable egress VLAN conversion for double(outer+inner) tagged packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_vlan_egrVlanCnvtDblTagEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MAPLE_VLAN_EGR_CNVT_TBL_CTRLr, MAPLE_DBL_TAG_DISf, &value)) != RT_ERR_OK)
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
} /* end of dal_maple_vlan_egrVlanCnvtDblTagEnable_get */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtDblTagEnable_set
 * Description:
 *      Enable egress VLAN conversion for double(outer+inner) tagged packet.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_vlan_egrVlanCnvtDblTagEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  en_dblTag;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        en_dblTag = 1;
    }
    else
    {
        en_dblTag = 0;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_VLAN_EGR_CNVT_TBL_CTRLr, MAPLE_DBL_TAG_DISf, &en_dblTag)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_egrVlanCnvtDblTagEnable_set */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtTblPowerEnable_get
 * Description:
 *      Get egress vlan conversion table power enable status.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_vlan_egrVlanCnvtTblPowerEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MAPLE_VLAN_EGR_CNVT_TBL_CTRLr, MAPLE_PWR_ENf, &value)) != RT_ERR_OK)
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
} /* end of dal_maple_vlan_egrVlanCnvtTblPowerEnable_get */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtTblPowerEnable_set
 * Description:
 *      Set egress vlan conversion table power enable status.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_vlan_egrVlanCnvtTblPowerEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  en_power;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        en_power = 1;
    }
    else
    {
        en_power = 0;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_VLAN_EGR_CNVT_TBL_CTRLr, MAPLE_PWR_ENf, &en_power)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_egrVlanCnvtTblPowerEnable_set */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidSource_get
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSrc - pointer to VID source
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
dal_maple_vlan_portEgrVlanCnvtVidSource_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pSrc)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ORGVIDf, &value)) != RT_ERR_OK)
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
            *pSrc = BASED_ON_INNER_VLAN;
            break;
        case 1:
            *pSrc = BASED_ON_OUTER_VLAN;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSrc=%x", *pSrc);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtVidSource_get */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidSource_set
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 *      src  - VID source
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
dal_maple_vlan_portEgrVlanCnvtVidSource_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t src)
{
    int32   ret;
    uint32  vid_src;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, src=%d", unit, src);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((src >= FWD_VLAN_MODE_END), RT_ERR_INPUT);

    if (BASED_ON_INNER_VLAN == src)
    {
        vid_src = 0;
    }
    else
    {
        vid_src = 1;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ORGVIDf, &vid_src)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtVidSource_set */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidTarget_get
 * Description:
 *      Specify the VID target for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pTgt - pointer to VID source
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
dal_maple_vlan_portEgrVlanCnvtVidTarget_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pTgt)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTgt), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_TGTVIDf, &value)) != RT_ERR_OK)
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
            *pTgt = BASED_ON_INNER_VLAN;
            break;
        case 1:
            *pTgt = BASED_ON_OUTER_VLAN;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTgt=%x", *pTgt);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtVidTarget_get */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidTarget_set
 * Description:
 *      Specify the VID target for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 *      tgt  - VID source
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
dal_maple_vlan_portEgrVlanCnvtVidTarget_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t tgt)
{
    int32   ret;
    uint32  vid_tgt;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tgt=%d", unit, tgt);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((tgt >= FWD_VLAN_MODE_END), RT_ERR_INPUT);

    if (BASED_ON_INNER_VLAN == tgt)
    {
        vid_tgt = 0;
    }
    else
    {
        vid_tgt = 1;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_TGTVIDf, &vid_tgt)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtVidTarget_set */

/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLookupMissAct_get
 * Description:
 *      Specify egress port VID conversion table lookup miss action.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAct - pointer to action selection
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
dal_maple_vlan_portEgrVlanCnvtLookupMissAct_get(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t *pAct)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAct), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_LKMISS_ACTf, &value)) != RT_ERR_OK)
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
            *pAct = LOOKUPMISS_FWD;
            break;
        case 1:
            *pAct = LOOKUPMISS_DROP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pAct=%x", *pAct);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtLookupMissAct_get */

/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLookupMissAct_set
 * Description:
 *      Specify egress port VID conversion table lookup miss action.
 * Input:
 *      unit - unit id
 *      port - port id
 *      act  - action selection
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
dal_maple_vlan_portEgrVlanCnvtLookupMissAct_set(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t act)
{
    int32   ret;
    uint32  lkmiss_act;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, act=%d", unit, act);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((act >= LOOKUPMISS_END), RT_ERR_INPUT);

    if (LOOKUPMISS_FWD == act)
    {
        lkmiss_act = 0;
    }
    else
    {
        lkmiss_act = 1;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_LKMISS_ACTf, &lkmiss_act)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtLookupMissAct_set */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtEntry_get
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
dal_maple_vlan_egrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData)
{
    int32   ret;
    vlan_egrcnvt_entry_t cnvt_entry;
    uint32  temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(((uint32)(index/2) >= HAL_MAX_NUM_OF_SC2C_ENTRY(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*** get entry from chip ***/
    VLAN_SEM_LOCK(unit);
    RT_ERR_HDL(table_read(unit, MAPLE_VLAN_EGR_CNVTt, (uint32)(index/2), (uint32 *) &cnvt_entry), errHandle1, ret);
    VLAN_SEM_UNLOCK(unit);

    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_VALID1tf : MAPLE_VLAN_EGR_CNVT_VALID0tf),\
        &pData->valid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_ORG_VID1tf : MAPLE_VLAN_EGR_CNVT_ORG_VID0tf),\
        &pData->vid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID1tf : MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID0tf),\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0xFFF)
        pData->vid_ignore = 0;
    else
        pData->vid_ignore = 1;
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_ORGPRI1tf : MAPLE_VLAN_EGR_CNVT_ORGPRI0tf),\
        &pData->orgpri, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_ORGPRI1tf : MAPLE_VLAN_EGR_CNVT_BMSK_ORGPRI0tf),\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x7)
        pData->orgpri_ignore = 0;
    else
        pData->orgpri_ignore = 1;
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_ORG_VID_RNG_CHK1tf : MAPLE_VLAN_EGR_CNVT_ORG_VID_RNG_CHK0tf),\
        &pData->rngchk_result, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID_RNG_CHK1tf : MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID_RNG_CHK0tf),\
        &pData->rngchk_result_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_PORT_ID1tf : MAPLE_VLAN_EGR_CNVT_PORT_ID0tf),\
        &pData->port, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_PORT_ID1tf : MAPLE_VLAN_EGR_CNVT_BMSK_PORT_ID0tf),\
        &temp_var, (uint32 *) &cnvt_entry), errHandle, ret);
    if (temp_var == 0x1F)
        pData->port_ignore = 0;
    else
        pData->port_ignore = 1;
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_VID_SHIFT1tf : MAPLE_VLAN_EGR_CNVT_VID_SHIFT0tf),\
        &pData->vid_shift, (uint32 *) &cnvt_entry), errHandle, ret);
    pData->vid_shift_sel = 0;
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_NEW_VID1tf : MAPLE_VLAN_EGR_CNVT_NEW_VID0tf),\
        &pData->vid_new, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_PRI_ASSIGN1tf : MAPLE_VLAN_EGR_CNVT_PRI_ASSIGN0tf),\
        &pData->pri_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_get(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_NEW_PRI1tf : MAPLE_VLAN_EGR_CNVT_NEW_PRI0tf),\
        &pData->pri_new, (uint32 *) &cnvt_entry), errHandle, ret);
    return RT_ERR_OK;

errHandle1:
    VLAN_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
} /* end of dal_maple_vlan_egrVlanCnvtEntry_get */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtEntry_set
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
dal_maple_vlan_egrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData)
{
    int32   ret;
    vlan_egrcnvt_entry_t cnvt_entry;
    uint32 vid_mask,port_mask;
    uint32 orgpri_mask;
    rtk_vlan_t  vid_value;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((uint32)(index/2) >= HAL_MAX_NUM_OF_SC2C_ENTRY(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pData->valid != 0 && pData->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_ignore != 0 && pData->vid_ignore != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pData->orgpri_ignore != 0 && pData->orgpri_ignore != 1), RT_ERR_INPUT);
    RT_PARAM_CHK(pData->orgpri > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);
    RT_PARAM_CHK((pData->port_ignore != 0 && pData->port_ignore != 1), RT_ERR_INPUT);
    RT_PARAM_CHK(pData->port_ignore != 1 && !HWP_ETHER_PORT(unit, pData->port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pData->vid_shift != 0 && pData->vid_shift != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_shift_sel != 0 && pData->vid_shift_sel != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_new > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((pData->pri_assign != 0 && pData->pri_assign != 1), RT_ERR_INPUT);
    RT_PARAM_CHK(pData->pri_new > RTK_DOT1P_PRIORITY_MAX, RT_ERR_QOS_1P_PRIORITY);

    osal_memset(&cnvt_entry, 0, sizeof(cnvt_entry));

    /*** get all  two logical entry ***/
    VLAN_SEM_LOCK(unit);
    RT_ERR_HDL(table_read(unit, MAPLE_VLAN_EGR_CNVTt, (uint32)(index/2), (uint32 *) &cnvt_entry), errHandle1, ret);
    VLAN_SEM_UNLOCK(unit);
    /*** get all  two logical entry ***/

    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_VALID1tf : MAPLE_VLAN_EGR_CNVT_VALID0tf),\
        &pData->valid, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_ORG_VID1tf : MAPLE_VLAN_EGR_CNVT_ORG_VID0tf),\
        &pData->vid, (uint32 *) &cnvt_entry), errHandle, ret);
    if (pData->vid_ignore == 1)
        vid_mask = 0;
    else
        vid_mask = 0xFFF;
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID1tf : MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID0tf),\
        &vid_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_ORGPRI1tf : MAPLE_VLAN_EGR_CNVT_ORGPRI0tf),\
        &pData->orgpri, (uint32 *) &cnvt_entry), errHandle, ret);
    if (pData->orgpri_ignore == 1)
        orgpri_mask = 0;
    else
        orgpri_mask = 0x7;
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_ORGPRI1tf : MAPLE_VLAN_EGR_CNVT_BMSK_ORGPRI0tf),\
        &orgpri_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_ORG_VID_RNG_CHK1tf : MAPLE_VLAN_EGR_CNVT_ORG_VID_RNG_CHK0tf),\
        &pData->rngchk_result, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID_RNG_CHK1tf : MAPLE_VLAN_EGR_CNVT_BMSK_ORG_VID_RNG_CHK0tf),\
        &pData->rngchk_result_mask, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_PORT_ID1tf : MAPLE_VLAN_EGR_CNVT_PORT_ID0tf),\
        &pData->port, (uint32 *) &cnvt_entry), errHandle, ret);
    if (pData->port_ignore == 1)
        port_mask = 0;
    else
        port_mask = 0x1F;
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_BMSK_PORT_ID1tf : MAPLE_VLAN_EGR_CNVT_BMSK_PORT_ID0tf),\
        &port_mask, (uint32 *) &cnvt_entry), errHandle, ret);

    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_VID_SHIFT1tf : MAPLE_VLAN_EGR_CNVT_VID_SHIFT0tf),\
        &pData->vid_shift, (uint32 *) &cnvt_entry), errHandle, ret);
    if (1 == pData->vid_shift_sel)
        vid_value = 4096 - pData->vid_new;
    else
        vid_value = pData->vid_new;
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_NEW_VID1tf : MAPLE_VLAN_EGR_CNVT_NEW_VID0tf),\
        &vid_value, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_PRI_ASSIGN1tf : MAPLE_VLAN_EGR_CNVT_PRI_ASSIGN0tf),\
        &pData->pri_assign, (uint32 *) &cnvt_entry), errHandle, ret);
    RT_ERR_HDL(table_field_set(unit, MAPLE_VLAN_EGR_CNVTt, ((index % 2) ? MAPLE_VLAN_EGR_CNVT_NEW_PRI1tf : MAPLE_VLAN_EGR_CNVT_NEW_PRI0tf),\
        &pData->pri_new, (uint32 *) &cnvt_entry), errHandle, ret);

    /* programming entry to chip */
    VLAN_SEM_LOCK(unit);
    RT_ERR_HDL(table_write(unit, MAPLE_VLAN_EGR_CNVTt, (index/2), (uint32 *) &cnvt_entry), errHandle1, ret);
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;

errHandle1:
    VLAN_SEM_UNLOCK(unit);
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;

errHandle:
    RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
    return ret;
} /* end of dal_maple_vlan_egrVlanCnvtEntry_set */

/* Function Name:
 *      dal_maple_vlan_innerTpidEntry_get
 * Description:
 *      Get inner TPID value from global inner TPID pool.
 * Input:
 *      unit     - unit id
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
 */
int32
dal_maple_vlan_innerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tpid_idx=%d", unit, tpid_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_CVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pTpid), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_TAG_TPID_CTRLr, REG_ARRAY_INDEX_NONE, tpid_idx, MAPLE_ITPIDf, pTpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "TPID=0x%04x", *pTpid);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_innerTpidEntry_get */

/* Function Name:
 *      dal_maple_vlan_innerTpidEntry_set
 * Description:
 *      Set inner TPID value to global inner TPID pool.
 * Input:
 *      unit     - unit id
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
 */
int32
dal_maple_vlan_innerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tpid_idx=%d, tpid=0x%04x", unit, tpid_idx, tpid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_CVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((tpid > RTK_ETHERTYPE_MAX), RT_ERR_OUT_OF_RANGE);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_TAG_TPID_CTRLr, REG_ARRAY_INDEX_NONE, tpid_idx, MAPLE_ITPIDf, &tpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_TAG_TPID_CTRL_MACr, REG_ARRAY_INDEX_NONE, tpid_idx, MAPLE_ITPIDf, &tpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_innerTpidEntry_set */

/* Function Name:
 *      dal_maple_vlan_outerTpidEntry_get
 * Description:
 *      Get outer TPID value from global outer TPID pool.
 * Input:
 *      unit     - unit id
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
 *      Global four outer TPID can be specified.
 */
int32
dal_maple_vlan_outerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tpid_idx=%d", unit, tpid_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_SVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pTpid), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_TAG_TPID_CTRLr, REG_ARRAY_INDEX_NONE, tpid_idx, MAPLE_OTPIDf, pTpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "TPID=0x%04x", *pTpid);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_outerTpidEntry_get */

/* Function Name:
 *      dal_maple_vlan_outerTpidEntry_set
 * Description:
 *      Set outer TPID value to global outer TPID pool.
 * Input:
 *      unit     - unit id
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
 *      Global four outer TPID can be specified.
 */
int32
dal_maple_vlan_outerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tpid_idx=%d, tpid=0x%04x", unit, tpid_idx, tpid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_SVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((tpid > RTK_ETHERTYPE_MAX), RT_ERR_OUT_OF_RANGE);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_TAG_TPID_CTRLr, REG_ARRAY_INDEX_NONE, tpid_idx, MAPLE_OTPIDf, &tpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_TAG_TPID_CTRL_MACr, REG_ARRAY_INDEX_NONE, tpid_idx, MAPLE_OTPIDf, &tpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_outerTpidEntry_set */

/* Function Name:
 *      dal_maple_vlan_extraTpidEntry_get
 * Description:
 *      Get the TPID value of extra tag.
 * Input:
 *      unit     - unit id
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
 *      1) Only one extra tag is supported. System will bypass extra tag
 *         and keeps parsing the remaining payload.
 *      2) Following tag combination are supported for parsing an extra tag:
 *         outer+innet+extra, outer+extra, inner+extra
 */
int32
dal_maple_vlan_extraTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_EVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pTpid), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_VLAN_ETAG_TPID_CTRLr, MAPLE_ETPIDf, pTpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "TPID=0x%04x", *pTpid);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_extraTpidEntry_get */

/* Function Name:
 *      dal_maple_vlan_extraTpidEntry_set
 * Description:
 *      Set TPID value of extra tag.
 * Input:
 *      unit     - unit id
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
 *      1) Only one extra tag is supported. System will bypass extra tag
 *         and keeps parsing the remaining payload.
 *      2) Following tag combination are supported for parsing an extra tag:
 *         outer+innet+extra, outer+extra, inner+extra
 */
int32
dal_maple_vlan_extraTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, tpid=0x%04x", tpid);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((tpid_idx >= HAL_MAX_NUM_OF_EVLAN_TPID(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((tpid > RTK_ETHERTYPE_MAX), RT_ERR_OUT_OF_RANGE);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_VLAN_ETAG_TPID_CTRLr, MAPLE_ETPIDf, &tpid)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_extraTpidEntry_set */

/* Function Name:
 *      dal_maple_vlan_portIgrInnerTpid_get
 * Description:
 *      Get TPIDs of inner tag at ingress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pTpid_idx_mask - pointer to mask for index of tpid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner-tagged packet
 */
int32
dal_maple_vlan_portIgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTpid_idx_mask), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_ITAG_TPID_CMP_MSKr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_ITPID_CMP_MSKf, pTpid_idx_mask)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTpid_idx_mask=0x%x", *pTpid_idx_mask);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrInnerTpid_get */

/* Function Name:
 *      dal_maple_vlan_portIgrInnerTpid_set
 * Description:
 *      Set TPIDs of inner tag at ingress.
 * Input:
 *      unit          - unit id
 *      port          - port id
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
 *      Specify which TPID to compare from TPID pool for parsing a inner-tagged packet
 */
int32
dal_maple_vlan_portIgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d tpid_idx_mask=0x%x", unit, port, tpid_idx_mask);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(tpid_idx_mask > HAL_TPID_ENTRY_MASK_MAX(unit), RT_ERR_INPUT);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_ITAG_TPID_CMP_MSKr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_ITPID_CMP_MSKf, &tpid_idx_mask)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrInnerTpid_set */

/* Function Name:
 *      dal_maple_vlan_portIgrOuterTpid_get
 * Description:
 *      Get TPIDs of outer tag at ingress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pTpid_idx_mask - pointer to mask for index of tpid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a outer-tagged packet
 */
int32
dal_maple_vlan_portIgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTpid_idx_mask), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_OTAG_TPID_CMP_MSKr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_OTPID_CMP_MSKf, pTpid_idx_mask)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTpid_idx_mask=0x%x", *pTpid_idx_mask);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrOuterTpid_get */

/* Function Name:
 *      dal_maple_vlan_portIgrOuterTpid_set
 * Description:
 *      Set TPIDs of outer tag at ingress.
 * Input:
 *      unit          - unit id
 *      port          - port id
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
 *      Specify which TPID to compare from TPID pool for parsing a outer-tagged packet
 */
int32
dal_maple_vlan_portIgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d tpid_idx_mask=0x%x", unit, port, tpid_idx_mask);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(tpid_idx_mask > HAL_TPID_ENTRY_MASK_MAX(unit), RT_ERR_INPUT);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_OTAG_TPID_CMP_MSKr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_OTPID_CMP_MSKf, &tpid_idx_mask)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrOuterTpid_set */

/* Function Name:
 *      dal_maple_vlan_portIgrExtraTagEnable_get
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
dal_maple_vlan_portIgrExtraTagEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_ETAG_TPID_CMPr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_ETPID_CMPf, pEnable)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrExtraTagEnable_get */

/* Function Name:
 *      dal_maple_vlan_portIgrExtraTagEnable_set
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
dal_maple_vlan_portIgrExtraTagEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_ETAG_TPID_CMPr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_ETPID_CMPf, &enable)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portIgrExtraTagEnable_set */

/* Function Name:
 *      dal_maple_vlan_portEgrInnerTagSts_get
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
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
dal_maple_vlan_portEgrInnerTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSts), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                MAPLE_ITAG_STSf, &value)) != RT_ERR_OK)
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
            *pSts = TAG_STATUS_UNTAG;
            break;
        case 1:
            *pSts = TAG_STATUS_TAGGED;
            break;
        case 2:
            *pSts = TAG_STATUS_PRIORITY_TAGGED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSts=%x", *pSts);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrInnerTagSts_get */

/* Function Name:
 *      dal_maple_vlan_portEgrInnerTagSts_set
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
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
dal_maple_vlan_portEgrInnerTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, sts=%d", unit, port, sts);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((sts >= TAG_STATUS_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (sts)
    {
        case TAG_STATUS_UNTAG:
            value = 0;
            break;
        case TAG_STATUS_TAGGED:
            value = 1;
            break;
        case TAG_STATUS_PRIORITY_TAGGED:
            value = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                MAPLE_ITAG_STSf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrInnerTagSts_set */

/* Function Name:
 *      dal_maple_vlan_portEgrOuterTagSts_get
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
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
dal_maple_vlan_portEgrOuterTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSts), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                MAPLE_OTAG_STSf, &value)) != RT_ERR_OK)
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
            *pSts = TAG_STATUS_UNTAG;
            break;
        case 1:
            *pSts = TAG_STATUS_TAGGED;
            break;
        case 2:
            *pSts = TAG_STATUS_PRIORITY_TAGGED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSts=%x", *pSts);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrOuterTagSts_get */

/* Function Name:
 *      dal_maple_vlan_portEgrOuterTagSts_set
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
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
dal_maple_vlan_portEgrOuterTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, sts=%d", unit, port, sts);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((sts >= TAG_STATUS_END), RT_ERR_INPUT);

    /* translate to chip's definition */
    switch (sts)
    {
        case TAG_STATUS_UNTAG:
            value = 0;
            break;
        case TAG_STATUS_TAGGED:
            value = 1;
            break;
        case TAG_STATUS_PRIORITY_TAGGED:
            value = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_TAG_STS_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                MAPLE_OTAG_STSf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrOuterTagSts_set */


/* Internal Function Body */

/* Function Name:
 *      _dal_maple_vlan_init_config
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
_dal_maple_vlan_init_config(uint32 unit)
{
    int32       ret;
    dal_maple_vlan_data_t vlan_data_entry;
    rtk_port_t  port;
    rtk_enable_t enable;
    uint32      index;
    rtk_vlan_profile_t profile;
#if 0
    int32       i;
#endif

    /* configure vlan data entry */
    osal_memset(&vlan_data_entry, 0x00, sizeof(dal_maple_vlan_data_t));
    vlan_data_entry.vid         = RTK_DEFAULT_VLAN_ID;
    vlan_data_entry.fid_msti    = RTK_DEFAULT_MSTI;
    vlan_data_entry.ucast_mode  = UC_LOOKUP_ON_VID;
    vlan_data_entry.mcast_mode  = MC_LOOKUP_ON_VID;
    vlan_data_entry.profile_idx = 0;
    HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.member_portmask);
    RTK_PORTMASK_PORT_CLEAR(vlan_data_entry.member_portmask, HWP_CPU_MACID(unit));
    HWP_GET_ALL_PORTMASK(unit, vlan_data_entry.untag_portmask);

    if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "create default vlan entry failed");
        return ret;
    }

#if 0
    /* For debug, configure vlan data entry other than default VLAN */
    osal_memset(&vlan_data_entry, 0, sizeof(vlan_data_entry));
    vlan_data_entry.fid_msti    = RTK_DEFAULT_MSTI;
    vlan_data_entry.ucast_mode  = UC_LOOKUP_ON_VID;
    vlan_data_entry.mcast_mode  = UC_LOOKUP_ON_VID;
    vlan_data_entry.profile_idx = 0;
    for (i = 0; i <= RTK_VLAN_ID_MAX; i++)
    {
        if (i == RTK_DEFAULT_VLAN_ID)
            continue;

        vlan_data_entry.vid = i;
        if ((ret = _dal_maple_setVlan(unit, &vlan_data_entry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "Init 4K vlan entry failed");
            return ret;
        }
    }
#endif

     /* Set vlan valid bit in software */
    VLANINFO_VALID_SET(unit, RTK_DEFAULT_VLAN_ID);
    pDal_maple_vlan_info[unit]->count++;

    HWP_PORT_TRAVS(unit, port)
    {
        /* Configure inner PVID to default VLAN */
        if ((ret = dal_maple_vlan_portInnerPvid_set(unit, port, RTK_DEFAULT_PORT_VID)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "set inner PVID to default VLAN failed");
            return ret;
        }

        /* Configure outer PVID to default VLAN */
        if ((ret = dal_maple_vlan_portOuterPvid_set(unit, port, RTK_DEFAULT_PORT_VID)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "set outer PVID to default VLAN failed");
            return ret;
        }

        /* Turn on ingress VLAN filtering */
        if (!HWP_IS_CPU_PORT(unit, port))
        {
            if ((ret = dal_maple_vlan_portIgrFilter_set(unit, port, INGRESS_FILTER_DROP)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_VLAN|MOD_DAL), "turn on ingress VLAN filtering failed");
                return ret;
            }
        }

        /* Turn on egress VLAN filtering */
        if ((ret = dal_maple_vlan_portEgrFilterEnable_set(unit, port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "turn on egress VLAN filtering failed");
            return ret;
        }

        /* Configure Ingress VLAN mode */
        if ((ret = dal_maple_vlan_portIgrTagKeepType_set(unit, port, TAG_KEEP_TYPE_NOKEEP, TAG_KEEP_TYPE_NOKEEP)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure inner VLAN mode failed");
            return ret;
        }

        /* Configure Egress VLAN mode */
        if ((ret = dal_maple_vlan_portEgrTagKeepType_set(unit, port, TAG_KEEP_TYPE_NOKEEP, TAG_KEEP_TYPE_NOKEEP)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure outer VLAN mode failed");
            return ret;
        }

        /* Configure egress port inner PVID mode */
        if ((ret = dal_maple_vlan_portInnerPvidMode_set(unit, port, PBVLAN_MODE_UNTAG_AND_PRITAG)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure inner VLAN PVID apply to packet failed");
            return ret;
        }

        /* Configure egress port outer PVID mode */
        if ((ret = dal_maple_vlan_portOuterPvidMode_set(unit, port, PBVLAN_MODE_UNTAG_AND_PRITAG)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure outer VLAN PVID apply to packet failed");
            return ret;
        }

        /* Configure egress port inner tag status */
        if ((ret = dal_maple_vlan_portEgrInnerTagSts_set(unit, port, TAG_STATUS_TAGGED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure egress inner tag status failed");
            return ret;
        }
    }

    /* Egress VLAN Translation Enable */
    enable = ENABLED;
    if ((ret = reg_field_write(unit, MAPLE_VLAN_EGR_CNVT_TBL_CTRLr, MAPLE_PWR_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure egress VLAN Translation power status failed");
        return ret;
    }

    if ((ret = reg_field_write(unit, MAPLE_VLAN_EGR_CNVT_TBL_CTRLr, MAPLE_OP_ENf, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure egress VLAN Translation operation status failed");
        return ret;
    }

    /* VLAN Profile Initial */
    osal_memset(&profile, 0, sizeof(rtk_vlan_profile_t));
    profile.learn = 1;
    for(index = 0; index < HAL_MAX_NUM_OF_VLAN_PROFILE(unit); ++index)
    {
        if ((ret = dal_maple_vlan_profile_set(unit, index, &profile)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "configure VLAN Profile %u failed", index);
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_maple_vlan_init_config */

/* Function Name:
 *      _dal_maple_setVlan
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
 * Note:
 */
static int32
_dal_maple_setVlan(uint32 unit, dal_maple_vlan_data_t *pVlan_entry)
{
    int32   ret;
    vlan_entry_t        vlan_entry;
    vlan_untag_entry_t  vlan_untag_entry;
    uint32  temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, fid_msti=%d, ucast_mode=%s, mcast_mode=%s, profile_idx=%d, member_portmask=%x, untag_portmask=%x",
           unit, pVlan_entry->vid, pVlan_entry->fid_msti, (pVlan_entry->ucast_mode)? "FID" : "VID", (pVlan_entry->mcast_mode)? "FID" : "VID", \
           pVlan_entry->profile_idx, pVlan_entry->member_portmask, pVlan_entry->untag_portmask);

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));
    osal_memset(&vlan_untag_entry, 0, sizeof(vlan_untag_entry));

    /*** VLAN table ***/
    /* set fid and msti */
    temp_var = pVlan_entry->fid_msti;
    if ((ret = table_field_set(unit, MAPLE_VLANt, MAPLE_VLAN_FID_MSTItf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set unicast lookup mode */
    temp_var = pVlan_entry->ucast_mode;
    if ((ret = table_field_set(unit, MAPLE_VLANt, MAPLE_VLAN_L2_HASH_KEY_UCtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set mulicast lookup mode */
    temp_var = pVlan_entry->mcast_mode;
    if ((ret = table_field_set(unit, MAPLE_VLANt, MAPLE_VLAN_L2_HASH_KEY_MCtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set profile index */
    if ((ret = table_field_set(unit, MAPLE_VLANt, MAPLE_VLAN_VLAN_PROFILEtf, &pVlan_entry->profile_idx, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* set member set */
    if ((ret = table_field_set(unit, MAPLE_VLANt, MAPLE_VLAN_MBRtf, pVlan_entry->member_portmask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* programming vlan entry to chip */
    if ((ret = table_write(unit, MAPLE_VLANt, pVlan_entry->vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*** VLAN untag member table ***/
    /* set untagged member set */
    if ((ret = table_field_set(unit, MAPLE_UNTAGt, MAPLE_UNTAG_PMSKtf, pVlan_entry->untag_portmask.bits, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* programming vlan untag entry to chip */
    if ((ret = table_write(unit, MAPLE_UNTAGt, pVlan_entry->vid, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_maple_setVlan */

/* Function Name:
 *      _dal_maple_getVlan
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
static int32
_dal_maple_getVlan(uint32 unit, dal_maple_vlan_data_t *pVlan_entry)
{
    int32   ret;
    vlan_entry_t        vlan_entry;
    vlan_untag_entry_t  vlan_untag_entry;
    uint32  temp_var;

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d", unit, pVlan_entry->vid);

    osal_memset(&vlan_entry, 0, sizeof(vlan_entry));

    /*** get VLAN entry from chip ***/
    if ((ret = table_read(unit, MAPLE_VLANt, pVlan_entry->vid, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* get fid and msti */
    if ((ret = table_field_get(unit, MAPLE_VLANt, MAPLE_VLAN_FID_MSTItf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->fid_msti = temp_var;

    /* get unicast lookup mode */
    if ((ret = table_field_get(unit, MAPLE_VLANt, MAPLE_VLAN_L2_HASH_KEY_UCtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->ucast_mode = temp_var;

    /* get multicast lookup mode */
    if ((ret = table_field_get(unit, MAPLE_VLANt, MAPLE_VLAN_L2_HASH_KEY_MCtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->mcast_mode = temp_var;

    /* get profile index */
    if ((ret = table_field_get(unit, MAPLE_VLANt, MAPLE_VLAN_VLAN_PROFILEtf, &temp_var, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    pVlan_entry->profile_idx = temp_var;

    /* get member set from vlan_entry */
    if ((ret = table_field_get(unit, MAPLE_VLANt, MAPLE_VLAN_MBRtf, pVlan_entry->member_portmask.bits, (uint32 *) &vlan_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /*** get VLAN untag member from chip ***/
    if ((ret = table_read(unit, MAPLE_UNTAGt, pVlan_entry->vid, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    /* get untagged member set */
    if ((ret = table_field_get(unit, MAPLE_UNTAGt, MAPLE_UNTAG_PMSKtf, pVlan_entry->untag_portmask.bits, (uint32 *) &vlan_untag_entry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }

    RT_LOG(LOG_TRACE, (MOD_VLAN|MOD_DAL), "unit=%d, vid=%d, fid_msti=%d, ucast_mode=%s, mcast_mode=%s, profile_idx=%d, member_portmask=%x, untag_portmask=%x",
           unit, pVlan_entry->vid, pVlan_entry->fid_msti, (pVlan_entry->ucast_mode)? "FID" : "VID", (pVlan_entry->mcast_mode)? "FID" : "VID", \
           pVlan_entry->profile_idx, pVlan_entry->member_portmask, pVlan_entry->untag_portmask);

    return RT_ERR_OK;
} /* end of _dal_maple_getVlan */

/* Function Name:
 *      dal_maple_vlan_portEgrInnerTpid_get
 * Description:
 *      Get TPID of inner tag at egress.
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
dal_maple_vlan_portEgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTpid_idx), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_EGR_ITPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, MAPLE_ITPID_IDXf, pTpid_idx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTpid_idx=%x", *pTpid_idx);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrInnerTpid_get */

/* Function Name:
 *      dal_maple_vlan_portEgrInnerTpid_set
 * Description:
 *      Set TPID of inner tag at egress.
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
dal_maple_vlan_portEgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, tpid_idx=%d", unit, port, tpid_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(tpid_idx > HAL_TPID_ENTRY_IDX_MAX(unit), RT_ERR_OUT_OF_RANGE);

    VLAN_SEM_LOCK(unit);
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_EGR_ITPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, MAPLE_ITPID_IDXf, &tpid_idx)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrInnerTpid_set */

/* Function Name:
 *      dal_maple_vlan_portEgrOuterTpid_get
 * Description:
 *      Get TPID of outer tag at egress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTpid_idx - pointer to index of outer TPID
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
dal_maple_vlan_portEgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTpid_idx), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_EGR_OTPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, MAPLE_OTPID_IDXf, pTpid_idx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pTpid_idx=%x", *pTpid_idx);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrOuterTpid_get */

/* Function Name:
 *      dal_maple_vlan_portEgrOuterTpid_set
 * Description:
 *      Set TPID of outer tag at egress.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tpid_idx - index of outer TPID
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
dal_maple_vlan_portEgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, tpid_idx=%d", unit, port, tpid_idx);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(tpid_idx > HAL_TPID_ENTRY_IDX_MAX(unit), RT_ERR_OUT_OF_RANGE);

    VLAN_SEM_LOCK(unit);
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_EGR_OTPID_CTRLr,
            port, REG_ARRAY_INDEX_NONE, MAPLE_OTPID_IDXf, &tpid_idx)) !=
            RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrOuterTpid_set */

/* Function Name:
 *      dal_maple_vlan_leakyStpFilter_get
 * Description:
 *      Get leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to status of leaky STP filter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_vlan_leakyStpFilter_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_VLAN_CTRLr, MAPLE_LKY_STP_FLTR_ENf,
            &val) ) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else if (1 == val)
        *pEnable = ENABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_leakyStpFilter_get */

/* Function Name:
 *      dal_maple_vlan_leakyStpFilter_set
 * Description:
 *      Set leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit   - unit id
 *      enable - status of leaky STP filter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_vlan_leakyStpFilter_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d enable:%d", unit, enable);

    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        val = 0;
    else if (ENABLED == enable)
        val = 1;
    else
    {
        RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_VLAN), "");
        return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_VLAN_CTRLr, MAPLE_LKY_STP_FLTR_ENf,
            &val)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_leakyStpFilter_set */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support on egress port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status
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
dal_maple_vlan_portVlanAggrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    int32   value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_LRN_CNVT_ENf, (uint32 *)&value)) != RT_ERR_OK)
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

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pEnable=0x%x", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrEnable_get */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrEnable_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_vlan_portVlanAggrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE,\
        MAPLE_LRN_CNVT_ENf, &enable)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrEnable_set */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrCtrl_get
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
dal_maple_vlan_portVlanAggrCtrl_get(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl)
{
    int32   ret;
    uint32  value1, value2;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pVlanAggrCtrl), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_VID_SELf, &value1)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI_TAG_ACTf, &value2)) != RT_ERR_OK)
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
            pVlanAggrCtrl->vid_src= OUTER_VLAN;
            break;
        default:
            return RT_ERR_FAILED;
    }

    /* translate chip's value to definition */
    switch (value2)
    {
        case 0:
            pVlanAggrCtrl->pri_src = LEARNING_VID_PRI;
            break;
        case 1:
            pVlanAggrCtrl->pri_src = LEARNING_VID_PBASED;
            break;
        default:
            return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrCtrl_get */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrCtrl_set
 * Description:
 *      Set port vlan-aggragation configration.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      vlanAggrCtrl - pointer to vlan-aggr ctrl
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
dal_maple_vlan_portVlanAggrCtrl_set(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t vlanAggrCtrl)
{
    int32   ret;
    uint32  vid_sel, val;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, vid src=%d, pri src=%d",
        unit, port, vlanAggrCtrl.vid_src,vlanAggrCtrl.pri_src);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((vlanAggrCtrl.vid_src>= VLAN_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((vlanAggrCtrl.pri_src>= LEARNING_VID_END), RT_ERR_INPUT);

    if (INNER_VLAN == vlanAggrCtrl.vid_src)
    {
        vid_sel = 0;
    }
    else
    {
        vid_sel = 1;
    }

    if (LEARNING_VID_PRI == vlanAggrCtrl.pri_src)
    {
        val = 0;
    }
    else
    {
        val = 1;
    }
    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_VID_SELf, &vid_sel)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI_TAG_ACTf, &val)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrCtrl_set */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrVidSource_get
 * Description:
 *      Get port ingress learning int address table and egress conversion VID selection inner VID or outer VID.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSrc - pointer to source selection
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
dal_maple_vlan_portVlanAggrVidSource_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pSrc)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_VID_SELf, &value)) != RT_ERR_OK)
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
            *pSrc = BASED_ON_INNER_VLAN;
            break;
        case 1:
            *pSrc = BASED_ON_OUTER_VLAN;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSrc=%x", *pSrc);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrVidSource_get */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrVidSource_set
 * Description:
 *      Set port ingress learning int address table and egress conversion VID selection inner VID or outer VID.
 * Input:
 *      unit - unit id
 *      port - port id
 *      src  - vlan mode source
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
dal_maple_vlan_portVlanAggrVidSource_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t src)
{
    int32   ret;
    uint32  vid_sel;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, src=%d", unit, port, src);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((src >= FWD_VLAN_MODE_END), RT_ERR_INPUT);

    if (BASED_ON_INNER_VLAN == src)
    {
        vid_sel = 0;
    }
    else
    {
        vid_sel = 1;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_VID_SELf, &vid_sel)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrVidSource_set */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrPriTagVidSource_get
 * Description:
 *      Get ingress port priority-tagged packet learning VID.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSrc - pointer to priority-tagged packet learning VID
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
dal_maple_vlan_portVlanAggrPriTagVidSource_get(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t *pSrc)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI_TAG_ACTf, &value)) != RT_ERR_OK)
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
            *pSrc = LEARNING_VID_PRI;
            break;
        case 1:
            *pSrc = LEARNING_VID_PBASED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pSrc=%x", *pSrc);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrPriTagVidSource_get */

/* Function Name:
 *      dal_maple_vlan_portVlanAggrPriTagVidSource_set
 * Description:
 *      Set ingress port priority-tagged packet learning VID 0 or port-based VID.
 * Input:
 *      unit - unit id
 *      port - port id
 *      src  - priority-tagged packet learning VID
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
dal_maple_vlan_portVlanAggrPriTagVidSource_set(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t src)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, port=%d, src=%d", unit, port, src);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((src >= LEARNING_VID_END), RT_ERR_INPUT);

    if (LEARNING_VID_PRI == src)
    {
        val = 0;
    }
    else
    {
        val = 1;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_L2TBL_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_PRI_TAG_ACTf, &val)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portVlanAggrPriTagVidSource_set */


/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLuMisAct_get
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
extern int32
dal_maple_vlan_portEgrVlanCnvtLuMisAct_get(
    uint32                                unit,
    rtk_port_t                            port,
    rtk_vlanType_t                       type,
    rtk_vlan_lookupMissAct_t          *pAction)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type),RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_LKMISS_ACTf, &value)) != RT_ERR_OK)
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
            *pAction = LOOKUPMISS_FWD;
            break;
        case 1:
            *pAction = LOOKUPMISS_DROP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "pAct=%x", *pAction);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtLuMisAct_get */

/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLuMisAct_set
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
dal_maple_vlan_portEgrVlanCnvtLuMisAct_set(
    uint32                               unit,
    rtk_port_t                           port,
    rtk_vlanType_t                      type,
    rtk_vlan_lookupMissAct_t           action)
{
    int32   ret;
    uint32  lkmiss_act;

    RT_LOG(LOG_DEBUG, (MOD_VLAN|MOD_DAL), "unit=%d, act=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_TYPE_END <= type),RT_ERR_INPUT);
    RT_PARAM_CHK((action >= LOOKUPMISS_END), RT_ERR_INPUT);

    if (LOOKUPMISS_FWD == action)
    {
        lkmiss_act = 0;
    }
    else
    {
        lkmiss_act = 1;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_CNVT_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_LKMISS_ACTf, &lkmiss_act)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_portEgrVlanCnvtLuMisAct_set */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtEntry_delAll
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
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
dal_maple_vlan_egrVlanCnvtEntry_delAll(uint32 unit)
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
        ret = table_write(unit, MAPLE_VLAN_EGR_CNVTt, entry_idx, (uint32 *) &cnvt_entry);
        if (RT_ERR_OK != ret)
        {
            VLAN_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_VLAN|MOD_DAL), "");
            return ret;
        }
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_egrVlanCnvtEntry_delAll */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtRangeCheckVid_get
 * Description:
 *      Get the configuration of VID range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of VID
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
dal_maple_vlan_egrVlanCnvtRangeCheckVid_get(
    uint32                                  unit,
    uint32                                  index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t      *pData)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_TYPEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_REVERSEf, &pData->reverse))
            != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_VID_UPPERf,
            &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_VID_LOWERf,
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
            pData->vid_type = VLAN_TAG_TYPE_INNER;
            break;
        case 1:
            pData->vid_type = VLAN_TAG_TYPE_OUTER;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_vlan_egrVlanCnvtRangeCheckVid_get */

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtRangeCheckVid_set
 * Description:
 *      Set the configuration of VID range check.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of VID
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
dal_maple_vlan_egrVlanCnvtRangeCheckVid_set(
    uint32                                  unit,
    uint32                                  index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t      *pData)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, index=%d", unit, index);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_RANGE_CHECK_VID(unit)), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "upper_bound=%d, upper_bound=%d, reverse=%d, vid_type=%d",\
        pData->vid_upper_bound, pData->vid_lower_bound, pData->reverse, pData->vid_type);

    RT_PARAM_CHK(((pData->vid_type != OUTER_VLAN) && (pData->vid_type != INNER_VLAN)), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_lower_bound > pData->vid_upper_bound), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_upper_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((pData->vid_lower_bound > RTK_VLAN_ID_MAX), RT_ERR_INPUT);

    switch (pData->vid_type)
    {
        case VLAN_TAG_TYPE_INNER:
            value = 0;
            break;
        case VLAN_TAG_TYPE_OUTER:
            value = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_VLAN), "invalid VID type");
            return RT_ERR_INPUT;
    }

    VLAN_SEM_LOCK(unit);
    if ((ret = reg_array_field_write(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_TYPEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_REVERSEf, &pData->reverse))
            != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_VID_UPPERf,
            &pData->vid_upper_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_RNG_CHK_VID_EGR_XLATE_CTRLr,
            REG_ARRAY_INDEX_NONE, index, MAPLE_VID_LOWERf,
            &pData->vid_lower_bound)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }
    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_vlan_egrVlanCnvtRangeCheckVid_set */

/* Function Name:
 *      dal_maple_vlan_l2LookupMode_get
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
dal_maple_vlan_l2LookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t *pMode)
{
    int32   ret = RT_ERR_OK;
    rtk_l2_ucastLookupMode_t ucLookupMode;
    rtk_l2_mcastLookupMode_t mcLookupMode;

    switch (type)
    {
        case VLAN_L2_MAC_TYPE_UC:
            if(RT_ERR_OK != (ret = dal_maple_vlan_l2UcastLookupMode_get(unit, vid, &ucLookupMode)))
                return ret;
            *pMode = (UC_LOOKUP_ON_VID == ucLookupMode)? VLAN_L2_LOOKUP_MODE_VID : VLAN_L2_LOOKUP_MODE_FID;
            break;

        case VLAN_L2_MAC_TYPE_MC:
            if(RT_ERR_OK != (ret = dal_maple_vlan_l2McastLookupMode_get(unit, vid, &mcLookupMode)))
                return ret;
            *pMode = (MC_LOOKUP_ON_VID == mcLookupMode)? VLAN_L2_LOOKUP_MODE_VID : VLAN_L2_LOOKUP_MODE_FID;
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_l2LookupMode_set
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
 *      Each VLAN can have its own lookup mode for L2 traffic
 */
int32
dal_maple_vlan_l2LookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t mode)
{
    int32   ret = RT_ERR_OK;
    rtk_l2_ucastLookupMode_t ucLookupMode;
    rtk_l2_mcastLookupMode_t mcLookupMode;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameters check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK(type >= VLAN_L2_MAC_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(mode >= VLAN_L2_LOOKUP_MODE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(VLANINFO_VALID_IS_CLEAR(unit, vid), RT_ERR_VLAN_ENTRY_NOT_FOUND);

    switch (type)
    {
        case VLAN_L2_MAC_TYPE_UC:
            ucLookupMode = (VLAN_L2_LOOKUP_MODE_VID == mode)? UC_LOOKUP_ON_VID : UC_LOOKUP_ON_FID;
            ret = dal_maple_vlan_l2UcastLookupMode_set(unit, vid, ucLookupMode);
            break;

        case VLAN_L2_MAC_TYPE_MC:
            mcLookupMode = (VLAN_L2_LOOKUP_MODE_VID == mode)? MC_LOOKUP_ON_VID : MC_LOOKUP_ON_FID;
            ret = dal_maple_vlan_l2McastLookupMode_set(unit, vid, mcLookupMode);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portAcceptFrameType_get
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
dal_maple_vlan_portAcceptFrameType_get(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlanType_t             type,
    rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAccept_frame_type), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portInnerAcceptFrameType_get(unit, port, pAccept_frame_type);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portOuterAcceptFrameType_get(unit, port, pAccept_frame_type);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

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
dal_maple_vlan_portAcceptFrameType_set(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlanType_t             type,
    rtk_vlan_acceptFrameType_t accept_frame_type)
{
    int32   ret = RT_ERR_OK;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portInnerAcceptFrameType_set(unit, port, accept_frame_type);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portOuterAcceptFrameType_set(unit, port, accept_frame_type);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portPvidMode_get
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
dal_maple_vlan_portPvidMode_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t *pMode)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portInnerPvidMode_get(unit, port, pMode);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portOuterPvidMode_get(unit, port, pMode);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portPvidMode_set
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
dal_maple_vlan_portPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t mode)
{
    int32   ret = RT_ERR_OK;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portInnerPvidMode_set(unit, port, mode);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portOuterPvidMode_set(unit, port, mode);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}


/* Function Name:
 *      dal_maple_vlan_portPvid_get
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
dal_maple_vlan_portPvid_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 *pPvid)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPvid), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portInnerPvid_get(unit, port, pPvid);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portOuterPvid_get(unit, port, pPvid);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portPvid_set
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
dal_maple_vlan_portPvid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 pvid)
{
    int32   ret = RT_ERR_OK;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portInnerPvid_set(unit, port, pvid);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portOuterPvid_set(unit, port, pvid);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_tpidEntry_get
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
dal_maple_vlan_tpidEntry_get(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 *pTpid)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTpid), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case VLAN_TAG_TYPE_INNER:
            ret = dal_maple_vlan_innerTpidEntry_get(unit, tpid_idx, pTpid);
            break;

        case VLAN_TAG_TYPE_OUTER:
            ret = dal_maple_vlan_outerTpidEntry_get(unit, tpid_idx, pTpid);
            break;

        case VLAN_TAG_TYPE_EXTRA:
            ret = dal_maple_vlan_extraTpidEntry_get(unit, tpid_idx, pTpid);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_tpidEntry_set
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
dal_maple_vlan_tpidEntry_set(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 tpid)
{
    int32   ret = RT_ERR_OK;

    switch (type)
    {
        case VLAN_TAG_TYPE_INNER:
            ret = dal_maple_vlan_innerTpidEntry_set(unit, tpid_idx, tpid);
            break;

        case VLAN_TAG_TYPE_OUTER:
            ret = dal_maple_vlan_outerTpidEntry_set(unit, tpid_idx, tpid);
            break;

        case VLAN_TAG_TYPE_EXTRA:
            ret = dal_maple_vlan_extraTpidEntry_set(unit, tpid_idx, tpid);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portIgrTpid_get
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
dal_maple_vlan_portIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx_mask)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTpid_idx_mask), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portIgrInnerTpid_get(unit, port, pTpid_idx_mask);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portIgrOuterTpid_get(unit, port, pTpid_idx_mask);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portIgrTpid_set
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
dal_maple_vlan_portIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx_mask)
{
    int32   ret = RT_ERR_OK;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portIgrInnerTpid_set(unit, port, tpid_idx_mask);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portIgrOuterTpid_set(unit, port, tpid_idx_mask);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portEgrTpid_get
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
dal_maple_vlan_portEgrTpid_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 *pTpid_idx)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTpid_idx), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portEgrInnerTpid_get(unit, port, pTpid_idx);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portEgrOuterTpid_get(unit, port, pTpid_idx);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portEgrTpid_set
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
dal_maple_vlan_portEgrTpid_set(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 tpid_idx)
{
    int32   ret = RT_ERR_OK;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portEgrInnerTpid_set(unit, port, tpid_idx);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portEgrOuterTpid_set(unit, port, tpid_idx);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portEgrTagSts_get
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
dal_maple_vlan_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_tagSts_t *pSts)
{
    int32   ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSts), RT_ERR_NULL_POINTER);

    switch(type)
    {
            case INNER_VLAN:
                ret = dal_maple_vlan_portEgrInnerTagSts_get(unit, port, pSts);
                break;

            case OUTER_VLAN:
                ret = dal_maple_vlan_portEgrOuterTagSts_get(unit, port, pSts);
                break;
            default:
                return RT_ERR_INPUT;

    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portEgrTagSts_set
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
dal_maple_vlan_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type,rtk_vlan_tagSts_t sts)
{
    int32   ret = RT_ERR_OK;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portEgrInnerTagSts_set(unit, port, sts);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portEgrOuterTagSts_set(unit, port, sts);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtRangeCheckEntry_get
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
dal_maple_vlan_egrVlanCnvtRangeCheckEntry_get(
    uint32                                      unit,
    uint32                                      setIdx,
    uint32                                      index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t          *pData)
{
    int32   ret;

    /* parameter check */
    RT_PARAM_CHK((setIdx > 0), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if(RT_ERR_OK !=(ret = dal_maple_vlan_egrVlanCnvtRangeCheckVid_get(unit, index, pData)))
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtRangeCheckEntry_set
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
dal_maple_vlan_egrVlanCnvtRangeCheckEntry_set(
    uint32                                  unit,
    uint32                                  setIdx,
    uint32                                  index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t      *pData)
{
    int32   ret = RT_ERR_OK;

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((setIdx > 0), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    ret = dal_maple_vlan_egrVlanCnvtRangeCheckVid_set(unit, index, pData);

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portIgrVlanTransparentEnable_get
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
dal_maple_vlan_portIgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    *pEnable)
{
    int32   ret;
    rtk_enable_t    enKeep;

    switch(type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portIgrTagKeepEnable_get(unit, port, &enKeep, pEnable);
            break;
        case OUTER_VLAN:
            ret = dal_maple_vlan_portIgrTagKeepEnable_get(unit, port, pEnable, &enKeep);
            break;
        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portIgrVlanTransparentEnable_set
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
dal_maple_vlan_portIgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    enable)
{
    int32   ret = RT_ERR_OK;
    rtk_enable_t enKeepInner, enKeepOuter;

    if (RT_ERR_OK != (ret = dal_maple_vlan_portIgrTagKeepEnable_get(unit, port, &enKeepOuter, &enKeepInner)))
        return ret;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portIgrTagKeepEnable_set(unit, port, enKeepOuter, enable);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portIgrTagKeepEnable_set(unit, port, enable, enKeepInner);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portEgrVlanTransparentEnable_get
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
dal_maple_vlan_portEgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    *pEnable)
{
    int32   ret = RT_ERR_OK;
    rtk_enable_t enKeep;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portEgrTagKeepEnable_get(unit, port, &enKeep, pEnable);
            break;
        case OUTER_VLAN:
            ret = dal_maple_vlan_portEgrTagKeepEnable_get(unit, port, pEnable, &enKeep);
            break;
        default:
            ret = RT_ERR_INPUT;
    }
    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portEgrVlanTransparentEnable_set
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
dal_maple_vlan_portEgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    enable)
{
    int32   ret = RT_ERR_OK;
    rtk_enable_t enKeepInner, enKeepOuter;

    if (RT_ERR_OK != (ret = dal_maple_vlan_portEgrTagKeepEnable_get(unit, port, &enKeepOuter, &enKeepInner)))
        return ret;

    switch (type)
    {
        case INNER_VLAN:
            ret = dal_maple_vlan_portEgrTagKeepEnable_set(unit, port, enKeepOuter, enable);
            break;

        case OUTER_VLAN:
            ret = dal_maple_vlan_portEgrTagKeepEnable_set(unit, port, enable, enKeepInner);
            break;

        default:
            return RT_ERR_INPUT;
    }

    return ret;
}

/* Function Name:
 *      dal_maple_vlan_portFwdVlan_get
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
dal_maple_vlan_portFwdVlan_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_vlan_pktTagMode_t        tagMode,
    rtk_vlanType_t            *pVlanType)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_PKT_TAG_MODE_ALL != tagMode), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pVlanType), RT_ERR_NULL_POINTER);

    VLAN_SEM_LOCK(unit);

    if((ret = reg_array_field_read(unit, MAPLE_VLAN_PORT_FWDr,
                    port, REG_ARRAY_INDEX_NONE, MAPLE_FWD_BASEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    *pVlanType = (0 == value)? INNER_VLAN : OUTER_VLAN;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "pVlanType=%d", *pVlanType);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_vlan_portFwdVlan_set
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
dal_maple_vlan_portFwdVlan_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_vlan_pktTagMode_t        tagMode,
    rtk_vlanType_t             vlanType)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_VLAN), "unit=%d, vlanType=%d", unit, vlanType);

    /* check Init status */
    RT_INIT_CHK(vlan_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((VLAN_PKT_TAG_MODE_ALL != tagMode), RT_ERR_INPUT);
    RT_PARAM_CHK((vlanType >= VLAN_TYPE_END), RT_ERR_INPUT);

    /* translate value to chip's definition */
    value = (INNER_VLAN == vlanType)? 0 : 1;

    VLAN_SEM_LOCK(unit);

    if((ret = reg_array_field_write(unit, MAPLE_VLAN_PORT_FWDr,
                    port, REG_ARRAY_INDEX_NONE, MAPLE_FWD_BASEf, &value)) != RT_ERR_OK)
    {
        VLAN_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_VLAN), "");
        return ret;
    }

    VLAN_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


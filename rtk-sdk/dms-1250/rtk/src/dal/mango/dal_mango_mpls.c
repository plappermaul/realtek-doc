/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose :
 *
 * Feature :
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <osal/sem.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_l2.h>
#include <dal/mango/dal_mango_vlan.h>
#include <dal/mango/dal_mango_l3.h>
#include <dal/mango/dal_mango_mpls.h>
#include <dal/mango/dal_mango_switch.h>
#include <rtk/switch.h>
#include <rtk/mpls.h>

/*
 * Symbol Definition
 */
#define MANGO_MPLS_ENCAPENTRY_MAX       2048
#define MANGO_MPLS_NHENTRY_MAX          8192
#define DAL_MANGO_MAX_ENTRY_RTL9311E_DIV    2

/*
 * Data Declaration
 */
static uint32       mpls_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t mpls_sem[RTK_MAX_NUM_OF_UNIT];

static rtk_bitmap_t mpls_EnacpEntry_status[BITMAP_ARRAY_CNT(MANGO_MPLS_ENCAPENTRY_MAX)];

static rtk_mac_t    mpls_nh_dmac[DAL_MANGO_L3_NEXTHOP_MAX];
static rtk_fid_t    mpls_nh_dmac_fid[DAL_MANGO_L3_NEXTHOP_MAX];
static uint16       mpls_nh_dmac_idx[DAL_MANGO_L3_NEXTHOP_MAX];


/*
 * Macro Declaration
 */
#define MPLS_SEM_LOCK(unit)                                                         \
do {                                                                                \
    if (osal_sem_mutex_take(mpls_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)    \
    {                                                                               \
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_MPLS),"semaphore lock failed"); \
        return RT_ERR_SEM_LOCK_FAILED;                                              \
    }                                                                               \
} while(0)

#define MPLS_SEM_UNLOCK(unit)                                                           \
do {                                                                                    \
    if (osal_sem_mutex_give(mpls_sem[unit]) != RT_ERR_OK)                               \
    {                                                                                   \
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_MPLS),"semaphore unlock failed"); \
        return RT_ERR_SEM_UNLOCK_FAILED;                                                \
    }                                                                                   \
} while(0)

#define MPLS_TABLE_FMT_CHECK(_unit) \
do { \
    int32   _ret;   \
    uint32  _val;   \
    if ((_ret = dal_mango_switch_flexTblFmt_get(_unit, (rtk_switch_flexTblFmt_t *)&_val)) != RT_ERR_OK) \
    { \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), "get table format failed"); \
        return _ret; \
    } \
    if (RTK_SWITCH_FLEX_TBL_FMT_MPLS != _val) \
    { \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), "table format is not for ECID"); \
        return RT_ERR_NOT_ALLOWED; \
    } \
} while(0)

#define MPLS_ERR_CHK_UNLOCK_MSG(_op, _ret, _fmt) \
do { \
    if ((_ret = (_op)) != RT_ERR_OK) \
    { \
        MPLS_SEM_UNLOCK(unit); \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), _fmt); \
        return _ret;\
    } \
} while(0)

#define MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_op, _ret, _fmt, args...) \
do { \
    if ((_ret = (_op)) != RT_ERR_OK) \
    { \
        MPLS_SEM_UNLOCK(unit); \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), _fmt, args); \
        return _ret;\
    } \
} while(0)

#define MPLS_OPER_CHK_UNLOCK_MSG_ARGS(_op, _ret, _fmt, args...) \
do { \
    if (_op) \
    { \
        MPLS_SEM_UNLOCK(unit); \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), _fmt, args); \
        return _ret;\
    } \
} while(0)

#define MPLS_ERR_CHK_MSG(_op, _ret, _fmt) \
do { \
    if ((_ret = (_op)) != RT_ERR_OK) \
    { \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), _fmt); \
        return _ret;\
    } \
} while(0)

#define MPLS_ERR_CHK_MSG_ARGS(_op, _ret, _fmt, args...) \
do { \
    if ((_ret = (_op)) != RT_ERR_OK) \
    { \
        RT_ERR(_ret, (MOD_DAL|MOD_MPLS), _fmt, args); \
        return _ret;\
    } \
} while(0)

/*
 * Function Declaration
 */
/* common */

/* Function Name:
 *      dal_mango_mplsMapper_init
 * Description:
 *      Hook mpls module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook mpls module before calling any mpls APIs.
 */
int32
dal_mango_mplsMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mpls_init = dal_mango_mpls_init;
    pMapper->mpls_trapTarget_get = dal_mango_mpls_trapTarget_get;
    pMapper->mpls_trapTarget_set = dal_mango_mpls_trapTarget_set;
    pMapper->mpls_exceptionCtrl_get = dal_mango_mpls_exceptionCtrl_get;
    pMapper->mpls_exceptionCtrl_set = dal_mango_mpls_exceptionCtrl_set;
    pMapper->mpls_nextHop_create = dal_mango_mpls_nextHop_create;
    pMapper->mpls_nextHop_destroy = dal_mango_mpls_nextHop_destroy;
    pMapper->mpls_nextHop_get = dal_mango_mpls_nextHop_get;
    pMapper->mpls_nextHop_set = dal_mango_mpls_nextHop_set;
    pMapper->mpls_encap_create = dal_mango_mpls_encap_create;
    pMapper->mpls_encap_destroy = dal_mango_mpls_encap_destroy;
    pMapper->mpls_encap_get = dal_mango_mpls_encap_get;
    pMapper->mpls_encap_set = dal_mango_mpls_encap_set;
    pMapper->mpls_encapId_find = dal_mango_mpls_encapId_find;
    pMapper->mpls_hashAlgo_get = dal_mango_mpls_hashAlgo_get;
    pMapper->mpls_hashAlgo_set = dal_mango_mpls_hashAlgo_set;
    pMapper->mpls_decap_create = dal_mango_mpls_decap_create;
    pMapper->mpls_decap_destroy = dal_mango_mpls_decap_destroy;
    pMapper->mpls_decap_get = dal_mango_mpls_decap_get;
    pMapper->mpls_decap_set = dal_mango_mpls_decap_set;
    pMapper->mpls_decapId_find = dal_mango_mpls_decapId_find;
    pMapper->mpls_egrTcMap_get = dal_mango_mpls_egrTcMap_get;
    pMapper->mpls_egrTcMap_set = dal_mango_mpls_egrTcMap_set;
    pMapper->mpls_nextHop_create_id = dal_mango_mpls_nextHop_create_id;
    pMapper->mpls_encap_create_id = dal_mango_mpls_encap_create_id;

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_mpls_maxEntry_get
 * Description:
 *      Get max number of MPLS entry
 * Input:
 *      unit    - unit id
 * Output:
 *      entry_num    - pointer to number of MPLS entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_dal_mango_mpls_maxEntry_get(uint32 unit, uint32 *entry_num)
{
    uint32  maxEntryNum;

    maxEntryNum = HAL_MAX_NUM_OF_MPLS_ENCAP(unit);

    if (RTL9311E_CHIP_ID == HWP_CHIP_ID(unit))
    {
        *entry_num = maxEntryNum / DAL_MANGO_MAX_ENTRY_RTL9311E_DIV;
    }
    else
    {
        *entry_num = maxEntryNum;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_mpls_maxEntry_get */

/* Function Name:
 *      dal_mango_mpls_init
 * Description:
 *      Initialize MPLS module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize MPLS module before calling any MPLS APIs.
 */
int32
dal_mango_mpls_init(uint32 unit)
{
    int32 i;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* check Init status */
    if (INIT_COMPLETED == mpls_init[unit])
        return RT_ERR_OK;

    RT_INIT_REENTRY_CHK(mpls_init[unit]);
    mpls_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    mpls_sem[unit] = osal_sem_mutex_create();
    if (0 == mpls_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_MPLS), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    for(i=0;i<DAL_MANGO_L3_NEXTHOP_MAX;i++)
    {
        mpls_nh_dmac_idx[i] = DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX;
    }

    /* set init flag to complete init */
    mpls_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_init */

/* Function Name:
 *      dal_mango_mpls_trapTarget_get
 * Description:
 *      Get information of MPLS trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of MPLS trap packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
dal_mango_mpls_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(reg_field_read(unit, MANGO_MPLS_GLB_CTRLr, MANGO_CPU_SELf, &val),
            ret, "field read fail");

    MPLS_SEM_UNLOCK(unit);

    if (0 == val)
    {
        *pTarget = RTK_TRAP_LOCAL;
    }
    else
    {
        *pTarget = RTK_TRAP_MASTER;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_trapTarget_get */

/* Function Name:
 *      dal_mango_mpls_trapTarget_set
 * Description:
 *      Set information of MPLS trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of MPLS trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 */
int32
dal_mango_mpls_trapTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,target=%d",unit, target);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_TRAP_END <= target), RT_ERR_INPUT);

    /* function body */
    switch (target)
    {
        case RTK_TRAP_LOCAL:
            val = 0;
            break;
        case RTK_TRAP_MASTER:
            val = 1;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "target %d is invalid.", target);
            return RT_ERR_INPUT;
    }

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(reg_field_write(unit, MANGO_MPLS_GLB_CTRLr, MANGO_CPU_SELf, &val),
            ret, "field write fail");

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_trapTarget_set */

/* Function Name:
 *      dal_mango_mpls_exceptionCtrl_get
 * Description:
 *      Get action of packet with exception situation.
 * Input:
 *      unit    - unit id
 *      type    - exception type
 * Output:
 *      pAction    - pointer to action of exception type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_MPLS_EXCEPT_TYPE     - invalid exception type
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
dal_mango_mpls_exceptionCtrl_get(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t *pAction)
{
    uint32  val, field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,type=%d",unit, type);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_MPLS_ET_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    /* function body */
    switch (type)
    {
        case RTK_MPLS_ET_LABEL_EXCEED:
            field = MANGO_LABEL_NUM_EXCEEDf;
            break;
        case RTK_MPLS_ET_LABEL_UNKNOWN:
            field = MANGO_LABEL_UNKWf;
            break;
        case RTK_MPLS_ET_TTL_FAIL:
            field = MANGO_TTL_FAILf;
            break;
        case RTK_MPLS_ET_LABEL_0_15:
            field = MANGO_LABEL_0_15_ACTf;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "type %d is not support.", type);
            return RT_ERR_INPUT;
    }

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(reg_field_read(unit, MANGO_MPLS_GLB_CTRLr, field, &val),
            ret, "field read fail");

    MPLS_SEM_UNLOCK(unit);

    if (RTK_MPLS_ET_LABEL_0_15 == type)
    {
        if (0 == val)
        {
            *pAction = ACTION_TRAP2CPU;
        }
        else
        {
            *pAction = ACTION_FORWARD;
        }
    }
    else
    {
        if (0 == val)
        {
            *pAction = ACTION_DROP;
        }
        else
        {
            *pAction = ACTION_TRAP2CPU;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_exceptionCtrl_get */

/* Function Name:
 *      dal_mango_mpls_exceptionCtrl_set
 * Description:
 *      Set action of packet with exception situation.
 * Input:
 *      unit    - unit id
 *      type    - exception type
 *      action  - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_MPLS_EXCEPT_TYPE     - invalid exception type
 *      RT_ERR_INPUT                - invalid input parameter
 * Note:
 *      (1) the action is as following:
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 */
int32
dal_mango_mpls_exceptionCtrl_set(uint32 unit, rtk_mpls_exceptionType_t type,
    rtk_action_t action)
{
    uint32  val, field;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,type=%d,action=%d",unit, type, action);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_MPLS_ET_END <= type), RT_ERR_INPUT);

    /* function body */
    switch (type)
    {
        case RTK_MPLS_ET_LABEL_EXCEED:
            field = MANGO_LABEL_NUM_EXCEEDf;
            break;
        case RTK_MPLS_ET_LABEL_UNKNOWN:
            field = MANGO_LABEL_UNKWf;
            break;
        case RTK_MPLS_ET_TTL_FAIL:
            field = MANGO_TTL_FAILf;
            break;
        case RTK_MPLS_ET_LABEL_0_15:
            field = MANGO_LABEL_0_15_ACTf;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "type %d is not support.", type);
            return RT_ERR_INPUT;
    }

    if (RTK_MPLS_ET_LABEL_0_15 == type)
    {
        if (ACTION_TRAP2CPU == action)
            val = 0;
        else if (ACTION_FORWARD == action)
            val = 1;
        else
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "action %d is not support.", action);
            return RT_ERR_INPUT;
        }
    }
    else
    {
        if (ACTION_DROP == action)
            val = 0;
        else if (ACTION_TRAP2CPU == action)
            val = 1;
        else
        {
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "action %d is not support.", action);
            return RT_ERR_INPUT;
        }
    }

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(reg_field_write(unit, MANGO_MPLS_GLB_CTRLr, field, &val),
            ret, "field write fail");

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_exceptionCtrl_set */

/* next hop */
/* Function Name:
 *      _dal_mango_mpls_nextHop_config
 * Description:
 *      Configure an MPLS nexthop by path ID
 * Input:
 *      unit    - unit id
 *      nhId    - NH index
 *      pNh     - pointer to nexthop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
_dal_mango_mpls_nextHop_config(uint32 unit, uint32 nhId,
    rtk_mpls_nextHop_t *pNh)
{
    dal_mango_l3_nhEntry_t      l3nh;
    rtk_l3_intf_t               l3Intf;
    dal_mango_l2_ucastNhAddr_t  l2Entry, l2EntryOld;
    rtk_vlan_l2LookupMode_t     mode;
    rtk_vlan_svlMode_t          svlMode;
    mpls_nextHop_entry_t        nh;
    uint32                      val;
    int32                       ret;

    /* find L2 MAC index */
    l3Intf.intf_id = pNh->intfIdx;
    MPLS_ERR_CHK_MSG(dal_mango_l3_intf_get(unit, RTK_L3_INTFKEYTYPE_INTF_ID, &l3Intf),
            ret, "Get L3 interface info fail");

    osal_memset(&l2Entry, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
    osal_memset(&svlMode, 0, sizeof(rtk_vlan_svlMode_t));

    MPLS_ERR_CHK_MSG(dal_mango_vlan_l2LookupMode_get(unit, l3Intf.vid, VLAN_L2_MAC_TYPE_UC, &mode),
            ret, "Get L2 lookup mode fail");

    if (VLAN_L2_LOOKUP_MODE_VID == mode)
    {
        l2Entry.fid = l3Intf.vid;
    }
    else
    {
        MPLS_ERR_CHK_MSG(dal_mango_vlan_svlMode_get(unit, &svlMode),
                ret, "Get SVL mode fail");

        if (VLAN_SVL_MODE_FID_MAC_TYPE == svlMode)
        {
            if (VLAN_L2_LOOKUP_MODE_FID == mode)
            {
                ret = dal_mango_vlan_l2LookupSvlFid_get(unit, VLAN_L2_MAC_TYPE_UC, &l2Entry.fid);
            }
        }
        else if (VLAN_SVL_MODE_FID_VLAN == svlMode)
        {
            ret = dal_mango_vlan_svlFid_get(unit, l3Intf.vid, &l2Entry.fid);
        }

        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_MPLS), "Get SVL FID fail");
            return ret;
        }
    }
    l2Entry.mac = pNh->mac_addr;

    /* release L2 entry if the old entry is useless */
    if ((mpls_nh_dmac_idx[nhId] < DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX) &&
        ((mpls_nh_dmac_fid[nhId] != l2Entry.fid) ||
         (0 != osal_memcmp(&mpls_nh_dmac[nhId], &l2Entry.mac, sizeof(rtk_mac_t))) ||
         (pNh->l3_act != RTK_L3_ACT_FORWARD))
       )
    {
        osal_memset(&l2EntryOld, 0x00, sizeof(dal_mango_l2_ucastNhAddr_t));
        l2EntryOld.fid = mpls_nh_dmac_fid[nhId];
        l2EntryOld.mac = mpls_nh_dmac[nhId];

        MPLS_ERR_CHK_MSG(_dal_mango_l2_nexthop_del(unit, &l2EntryOld),
                ret, "Can't delete next-hop MAC in L2");

        mpls_nh_dmac_idx[nhId] = DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX;
    }

    osal_memset(&l3nh, 0x00, sizeof(dal_mango_l3_nhEntry_t));
    if (pNh->l3_act == RTK_L3_ACT_FORWARD)
    {
        /* alloc if it's not been created */
        if (mpls_nh_dmac_idx[nhId] == DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX)
        {
            MPLS_ERR_CHK_MSG(_dal_mango_l2_nexthop_add(unit, &l2Entry),
                    ret, "Can't find next-hop MAC in L2");

            mpls_nh_dmac[nhId]      = l2Entry.mac;
            mpls_nh_dmac_fid[nhId]  = l2Entry.fid;
            mpls_nh_dmac_idx[nhId]  = l2Entry.l2_idx;
        }

        l3nh.dmac_idx = mpls_nh_dmac_idx[nhId];
    } else {
        /* update cache in case */
        mpls_nh_dmac[nhId]      = l2Entry.mac;
        mpls_nh_dmac_fid[nhId]  = l2Entry.fid;
        mpls_nh_dmac_idx[nhId]  = DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX;

        switch (pNh->l3_act)
        {
            case RTK_L3_ACT_TRAP2CPU:
                l3nh.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2CPU;
                break;

            case RTK_L3_ACT_TRAP2MASTERCPU:
                l3nh.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER;
                break;

            case RTK_L3_ACT_DROP:
            default:
                l3nh.dmac_idx = DAL_MANGO_L3_NEXTHOP_DMAC_IDX_DROP;
                break;
        }
    }
    l3nh.l3_egr_intf_idx = pNh->intfIdx;
    MPLS_ERR_CHK_MSG_ARGS(_dal_mango_l3_nhEntry_set(unit, nhId, &l3nh, DAL_MANGO_L3_API_FLAG_MOD_MPLS),
            ret, "Set L3 NH %u fail", nhId);

    /* create MPLS NH */
    osal_memset(&nh, 0, sizeof(nh));

    val = 1;
    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_NHt, MANGO_MPLS_NH_VALIDtf, &val, (uint32 *)&nh),
            ret, "table set %u field fail", nhId);

    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_NHt, MANGO_MPLS_NH_MPLS_ENCAP_IDXtf, &pNh->encapId, (uint32 *)&nh),
            ret, "table set %u field fail", nhId);

    MPLS_ERR_CHK_MSG_ARGS(table_write(unit, MANGO_MPLS_NHt, nhId, (uint32 *)&nh),
            ret, "table write %u field fail", nhId);

    return ret;
}   /* end of _dal_mango_mpls_nextHop_config */

/* Function Name:
 *      _dal_mango_mpls_nextHop_create
 * Description:
 *      Create an MPLS nexthop
 * Input:
 *      unit    - unit id
 *      nhId    - next hop ID
 *      pNh     - pointer to nexthop information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
_dal_mango_mpls_nextHop_create(uint32 unit, uint32 nhId, rtk_mpls_nextHop_t *pNh)
{
    int32   ret;

    ret = _dal_mango_mpls_nextHop_config(unit, nhId, pNh);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "Set NH %u", nhId);
        _dal_mango_l3_nhEntry_free(unit, nhId, DAL_MANGO_L3_API_FLAG_MOD_MPLS);
        return ret;
    }

    return ret;
}   /* end of _dal_mango_mpls_nextHop_create */

/* Function Name:
 *      dal_mango_mpls_nextHop_create
 * Description:
 *      Create an MPLS nexthop and get the returned path ID
 * Input:
 *      unit    - unit id
 *      pNh     - pointer to nexthop
 * Output:
 *      pPathId - pointer to L3 path ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_nextHop_create(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t *pPathId)
{
    uint32  nhId;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    /* function body */
    /* create L3 NH */
    MPLS_ERR_CHK_MSG(_dal_mango_l3_nhEntry_alloc(unit, &nhId, DAL_MANGO_L3_API_FLAG_MOD_MPLS),
            ret, "Create L3 NH fail");

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_mpls_nextHop_create(unit, nhId, pNh),
            ret, "Create MPLS NH fail", nhId);

    ret = _dal_mango_l3_nhEntryPathId_get(unit, nhId, pPathId);
    if (ret != RT_ERR_OK)
    {
        _dal_mango_l3_nhEntry_free(unit, nhId, DAL_MANGO_L3_API_FLAG_MOD_MPLS);
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_MPLS), "Get L3 NH %u pathId", nhId);
        return ret;
    }

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_nextHop_create */

/* Function Name:
 *      dal_mango_mpls_nextHop_destroy
 * Description:
 *      Destroy an MPLS Next-Hop
 * Input:
 *      unit    - unit id
 *      pathId  - pointer to MPLS path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_nextHop_destroy(uint32 unit, rtk_l3_pathId_t pathId)
{
    mpls_nextHop_entry_t    nh;
    uint32                  nhId;
    int32                   ret;
    dal_mango_l2_ucastNhAddr_t l2Entry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,pathId=%d",unit, pathId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */

    /* function body */
    osal_memset(&nh, 0, sizeof(mpls_nextHop_entry_t));

    MPLS_SEM_LOCK(unit);

    nhId = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pathId);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_write(unit, MANGO_MPLS_NHt, nhId, (uint32 *)&nh),
            ret, "table write %u field fail", nhId);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_l3_nhEntry_free(unit, nhId, DAL_MANGO_L3_API_FLAG_MOD_MPLS),
            ret, "L3 entry %u fail", nhId);

    if (mpls_nh_dmac_idx[nhId] < DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX) /* valid L2 index */
    {
        osal_memset(&l2Entry, 0, sizeof(dal_mango_l2_ucastNhAddr_t));
        l2Entry.fid = mpls_nh_dmac_fid[nhId];
        l2Entry.mac = mpls_nh_dmac[nhId];

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_l2_nexthop_del(unit, &l2Entry),
                ret, "Can't delete next-hop (%u) MAC in L2", nhId);

        mpls_nh_dmac_idx[nhId] = DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX;
    }

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_nextHop_destroy */

/* Function Name:
 *      dal_mango_mpls_nextHop_get
 * Description:
 *      Get an MPLS Next-Hop by path ID
 * Input:
 *      unit     - unit id
 *      pathId   - MPLS path ID
 * Output:
 *      pNextHop - pointer to nexthop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_nextHop_get(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh)
{
    mpls_nextHop_entry_t    nh;
    dal_mango_l3_nhEntry_t  l3nh;
    rtk_l2_entry_t          l2Entry;
    uint32                  nhId, val;
    int32                   ret;
    rtk_mac_t               mac_addr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,pathId=%d",unit, pathId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNh), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_SEM_LOCK(unit);

    nhId = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pathId);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_read(unit, MANGO_MPLS_NHt, nhId, (uint32 *)&nh),
            ret, "table read %u fail", nhId);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_NHt, MANGO_MPLS_NH_VALIDtf, &val, (uint32 *)&nh),
            ret, "table get %u field fail", nhId);

    MPLS_OPER_CHK_UNLOCK_MSG_ARGS((0 == val), RT_ERR_FAILED, "entry %u is invlaid", nhId);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_NHt, MANGO_MPLS_NH_MPLS_ENCAP_IDXtf, &pNh->encapId, (uint32 *)&nh),
            ret, "table set %u field fail", nhId);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_l3_nhEntry_get(unit, nhId, &l3nh, DAL_MANGO_L3_API_FLAG_MOD_MPLS),
            ret, "Get L3 nh %u fail", nhId);

    if (mpls_nh_dmac_idx[nhId] < DAL_MANGO_L3_NEXTHOP_INVALID_DMAC_IDX)   /* valid only if it's been allocated */
    {
        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(dal_mango_l2_addrEntry_get(unit, l3nh.dmac_idx, &l2Entry),
                ret, "Get L2 entry %u fail", l3nh.dmac_idx);

        mac_addr = l2Entry.unicast.mac;
    } else {
        /* get back from cache */
        mac_addr = mpls_nh_dmac[nhId];
    }

    MPLS_SEM_UNLOCK(unit);

    pNh->intfIdx = l3nh.l3_egr_intf_idx;
    pNh->mac_addr = mac_addr;

    switch (l3nh.dmac_idx)
    {
    case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2MASTER:
        pNh->l3_act = RTK_L3_ACT_COPY2MASTERCPU;
        break;
    case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_TRAP2CPU:
        pNh->l3_act = RTK_L3_ACT_COPY2CPU;
        break;
    case DAL_MANGO_L3_NEXTHOP_DMAC_IDX_DROP:
        pNh->l3_act = RTK_L3_ACT_DROP;
        break;

    default:
        pNh->l3_act = RTK_L3_ACT_FORWARD;
        break;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_nextHop_get */

/* Function Name:
 *      dal_mango_mpls_nextHop_set
 * Description:
 *      Set an MPLS Next-Hop by path ID
 * Input:
 *      unit     - unit id
 *      pathId   - MPLS path ID
 *      pNextHop - pointer to nexthop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_nextHop_set(uint32 unit, rtk_l3_pathId_t pathId,
    rtk_mpls_nextHop_t *pNh)
{
    uint32  nhId;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,pathId=%d",unit, pathId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNh), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_SEM_LOCK(unit);

    nhId = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pathId);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_mpls_nextHop_config(unit, nhId, pNh),
            ret, "Set NH %u", nhId);

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_nextHop_set */

/* encapsulation */

/* Function Name:
 *      _dal_mango_mpls_encap_config
 * Description:
 *      Configure an MPLS encapsulation entry by encapsulation ID
 * Input:
 *      unit    - unit id
 *      encapId - MPLS encapsulation entry ID
 *      pEncap  - pointer to nexthop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      semaphore and pointer check by caller.
 */
int32
_dal_mango_mpls_encap_config(uint32 unit, rtk_mpls_entryId_t encapId,
    rtk_mpls_encap_t *pEncap)
{
    mpls_encap_entry_t  entry;
    uint32              val;
    int32               ret;

    osal_memset(&entry, 0, sizeof(entry));

    /* label operation */
    switch (pEncap->labelAct)
    {
        case RTK_MPLS_LABELACT_PUSH:
            val = 0;
            break;
        case RTK_MPLS_LABELACT_SWAP:
            val = 1;
            break;
        default:
            RT_ERR(RT_ERR_FWD_ACTION, (MOD_DAL|MOD_MPLS),
                    "label action %d is not support", pEncap->labelAct);
            return RT_ERR_FWD_ACTION;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_LABEL_ACTtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", encapId);

    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_LABELtf, &pEncap->label, (uint32 *)&entry),
            ret, "table set %u field fail", encapId);

    /* TTL and TC operation */
    if (pEncap->flags & RTK_MPLS_FLAG_TTL_ASSIGN)
    {
        val = 1;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TTL_ASSIGNtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", encapId);

        val = pEncap->ttl;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TTLtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", encapId);
    }
    else
    {
        val = 0;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TTL_ASSIGNtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", encapId);
    }

    switch (pEncap->tcAct)
    {
        case RTK_MPLS_TC_INHERIT:
            val = 0;
            break;
        case RTK_MPLS_TC_ASSIGN:
            val = 1;
            break;
        case RTK_MPLS_TC_INTPRI:
            val = 2;
            break;
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "tc action %d is not support", pEncap->tcAct);
            return RT_ERR_INPUT;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TC_ASSIGNtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", encapId);

    if (RTK_MPLS_TC_ASSIGN == pEncap->tcAct)
    {
        val = pEncap->tc;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TCtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", encapId);
    }

    if (pEncap->flags & RTK_MPLS_FLAG_NEXTLABEL)
    {
        val = 1;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_NEXT_ENtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", encapId);

        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_NEXT_IDXtf, &pEncap->nextEntryId, (uint32 *)&entry),
                ret, "table set %u field fail", encapId);
    }
    else
    {
        val = 0;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_NEXT_ENtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", encapId);
    }

    MPLS_ERR_CHK_MSG_ARGS(table_write(unit, MANGO_MPLS_ENCAPt, encapId, (uint32 *)&entry),
            ret, "table write %u field fail", encapId);

    BITMAP_SET(mpls_EnacpEntry_status, encapId);

    return ret;
}   /* end of _dal_mango_mpls_encap_config */

/* Function Name:
 *      dal_mango_mpls_encap_create
 * Description:
 *      Create an MPLS encapsulation entry and get the returned encapsulation ID
 * Input:
 *      unit    - unit id
 *      pEncap  - pointer to nexthop
 * Output:
 *      pEncapId    - pointer to MPLS encapsulation entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 */
int32
dal_mango_mpls_encap_create(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId)
{
    uint32              idx, maxEntryNum;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEncap), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEncapId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_MPLS_TC_END <= pEncap->tcAct), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_MAX_MPLS_LABEL <= pEncap->label), RT_ERR_INPUT);

    /* function body */
    RT_ERR_CHK(_dal_mango_mpls_maxEntry_get(unit, &maxEntryNum), ret);

    MPLS_SEM_LOCK(unit);

    for (idx = 0; idx < maxEntryNum; ++idx)
    {
        if (BITMAP_IS_CLEAR(mpls_EnacpEntry_status, idx))
            break;
    }

    if (maxEntryNum == idx)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_TBL_FULL, (MOD_DAL|MOD_MPLS), "");
        return RT_ERR_TBL_FULL;
    }

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_mpls_encap_config(unit, idx, pEncap),
            ret, "create entry %u fail", idx);

    MPLS_SEM_UNLOCK(unit);

    *pEncapId = idx;

    return ret;
}   /* end of dal_mango_mpls_encap_create */

/* Function Name:
 *      dal_mango_mpls_encap_destroy
 * Description:
 *      Destory an MPLS encapsulation entry by encapsulation entry ID
 * Input:
 *      unit     - unit id
 *      encapId  - MPLS encapsulation entry ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_encap_destroy(uint32 unit, rtk_mpls_entryId_t encapId)
{
    mpls_encap_entry_t  entry;
    uint32              maxEntryNum;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,encapId=%d",unit, encapId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_ERR_CHK(_dal_mango_mpls_maxEntry_get(unit, &maxEntryNum), ret);
    RT_PARAM_CHK((maxEntryNum <= encapId), RT_ERR_INPUT);

    /* function body */
    osal_memset(&entry, 0, sizeof(entry));

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_write(unit, MANGO_MPLS_ENCAPt, encapId, (uint32 *)&entry),
            ret, "table write %u field fail", encapId);

    BITMAP_CLEAR(mpls_EnacpEntry_status, encapId);

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_encap_destroy */

/* Function Name:
 *      dal_mango_mpls_encap_get
 * Description:
 *      Get an MPLS encapsulation entry by encapsulation ID
 * Input:
 *      unit     - unit id
 *      encapId  - MPLS encapsulation entry ID
 * Output:
 *      pEncap   - pointer to MPLS encapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_encap_get(uint32 unit, rtk_mpls_entryId_t encapId,
    rtk_mpls_encap_t *pEncap)
{
    mpls_encap_entry_t  entry;
    uint32              val, maxEntryNum;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,encapId=%d",unit, encapId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_ERR_CHK(_dal_mango_mpls_maxEntry_get(unit, &maxEntryNum), ret);
    RT_PARAM_CHK((maxEntryNum <= encapId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEncap), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(pEncap, 0, sizeof(rtk_mpls_encap_t));

    MPLS_SEM_LOCK(unit);

    if (BITMAP_IS_CLEAR(mpls_EnacpEntry_status, encapId))
    {
        MPLS_SEM_UNLOCK(unit);
        return RT_ERR_ENTRY_INDEX;
    }

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_read(unit, MANGO_MPLS_ENCAPt, encapId, (uint32 *)&entry),
            ret, "table read %u fail", encapId);

    MPLS_SEM_UNLOCK(unit);

    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_LABEL_ACTtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", encapId);

    if (0 == val)
    {
        pEncap->labelAct = RTK_MPLS_LABELACT_PUSH;
    }
    else
    {
        pEncap->labelAct = RTK_MPLS_LABELACT_SWAP;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_LABELtf, &pEncap->label, (uint32 *)&entry),
            ret, "table get %u field fail", encapId);

    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TTL_ASSIGNtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", encapId);

    if (1 == val)
    {
        MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TTLtf, &val, (uint32 *)&entry),
                ret, "table get %u field fail", encapId);

        pEncap->flags |= RTK_MPLS_FLAG_TTL_ASSIGN;
        pEncap->ttl = val;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_NEXT_ENtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", encapId);

    if (1 == val)
    {
        MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_NEXT_IDXtf, &pEncap->nextEntryId, (uint32 *)&entry),
                ret, "table get %u field fail", encapId);

        pEncap->flags |= RTK_MPLS_FLAG_NEXTLABEL;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TC_ASSIGNtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", encapId);

    switch (val)
    {
        case 0:
            pEncap->tcAct = RTK_MPLS_TC_INHERIT;
            break;
        case 1:
            pEncap->tcAct = RTK_MPLS_TC_ASSIGN;
            break;
        case 2:
            pEncap->tcAct = RTK_MPLS_TC_INTPRI;
            break;
        default:
            break;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_TCtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", encapId);

    pEncap->tc = val;

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_encap_get */

/* Function Name:
 *      dal_mango_mpls_encap_set
 * Description:
 *      Set an MPLS encapsulation entry by encapsulation ID
 * Input:
 *      unit     - unit id
 *      encapId  - MPLS encapsulation entry ID
 * Output:
 *      pEncap   - pointer to MPLS encapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 */
int32
dal_mango_mpls_encap_set(uint32 unit, rtk_mpls_entryId_t encapId,
    rtk_mpls_encap_t *pEncap)
{
    uint32  maxEntryNum;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,encapId=%d",unit, encapId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_ERR_CHK(_dal_mango_mpls_maxEntry_get(unit, &maxEntryNum), ret);
    RT_PARAM_CHK((maxEntryNum <= encapId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEncap), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_MAX_MPLS_LABEL <= pEncap->label), RT_ERR_INPUT);

    /* function body */
    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_mpls_encap_config(unit, encapId, pEncap),
            ret, "set entry %u fail", encapId);

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_encap_set */

/* Function Name:
 *      dal_mango_mpls_encapId_find
 * Description:
 *      Find an ID points to a MPLS encapsulation entry
 * Input:
 *      unit    - unit id
 *      pEncap  - pointer to MPLS encapsulation entry
 * Output:
 *      pEncapId    - pointer to MPLS encapsulation entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      Find encapsulation entry index by label action and label.
 */
int32
dal_mango_mpls_encapId_find(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t *pEncapId)
{
    mpls_encap_entry_t  entry;
    uint32              val, idx, maxEntryNum;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEncap), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEncapId), RT_ERR_NULL_POINTER);

    /* function body */
    RT_ERR_CHK(_dal_mango_mpls_maxEntry_get(unit, &maxEntryNum), ret);

    MPLS_SEM_LOCK(unit);

    for (idx = 0; idx < maxEntryNum; ++idx)
    {
        if (BITMAP_IS_CLEAR(mpls_EnacpEntry_status, idx))
        {
            continue;
        }

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_read(unit, MANGO_MPLS_ENCAPt, idx, (uint32 *)&entry),
                ret, "table read %u fail", idx);

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_ENCAPt, MANGO_MPLS_ENCAP_LABELtf, &val, (uint32 *)&entry),
                ret, "table get %u field fail", idx);

        if (pEncap->label == val)
            break;
    }

    if (maxEntryNum == idx)
    {
        MPLS_SEM_UNLOCK(unit);
        RT_ERR(RT_ERR_ENTRY_NOTFOUND, (MOD_DAL|MOD_MPLS), "label %u action %u", pEncap->label, pEncap->labelAct);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    *pEncapId = idx;

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_encapId_find */

/* decapsulation */
/* Function Name:
 *      dal_mango_mpls_hashAlgo_get
 * Description:
 *      Get hash algorithm of MPLS decapsulation table
 * Input:
 *      unit        - unit id
 * Output:
 *      pHashAlgo   - pointer to hash algorithm of MPLS
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
_dal_mango_mpls_hashAlgo_get(uint32 unit, uint8 *pHashAlgo)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHashAlgo), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_ERR_CHK_MSG(reg_field_read(unit, MANGO_MPLS_GLB_CTRLr, MANGO_HASH_SELf, &val),
            ret, "field read fail");

    switch (val)
    {
        case 0:
            *pHashAlgo = RTK_MPLS_HASHALGO_0;
            break;
        //case 1:
        default:
            *pHashAlgo = RTK_MPLS_HASHALGO_1;
            break;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_mpls_hashAlgo_get */

/* Function Name:
 *      dal_mango_mpls_hashAlgo_get
 * Description:
 *      Get hash algorithm of MPLS decapsulation table
 * Input:
 *      unit        - unit id
 * Output:
 *      pHashAlgo   - pointer to hash algorithm of MPLS
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
dal_mango_mpls_hashAlgo_get(uint32 unit, uint8 *pHashAlgo)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHashAlgo), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(_dal_mango_mpls_hashAlgo_get(unit, pHashAlgo),
            ret, "hash algorithm get fail");

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_hashAlgo_get */

/* Function Name:
 *      dal_mango_mpls_hashAlgo_set
 * Description:
 *      Set hash algorithm of MPLS decapsulation table
 * Input:
 *      unit        - unit id
 *      hashAlgo    - hash algorithm of MPLS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The valid input is 0 and 1. 0 mean hash_algo_0; 1 mean hash_algo_1.
 */
int32
dal_mango_mpls_hashAlgo_set(uint32 unit, uint8 hashAlgo)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,hashAlgo=%d",unit, hashAlgo);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_MPLS_HASHALGO_END <= hashAlgo), RT_ERR_INPUT);

    /* function body */
    MPLS_SEM_LOCK(unit);

    switch (hashAlgo)
    {
        case RTK_MPLS_HASHALGO_0:
            val = 0;
            break;
        case RTK_MPLS_HASHALGO_1:
            val = 1;
            break;
        default:
            MPLS_SEM_UNLOCK(unit);
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_MPLS), "invalid type %u", hashAlgo);
            return RT_ERR_INPUT;
    }

    MPLS_ERR_CHK_UNLOCK_MSG(reg_field_write(unit, MANGO_MPLS_GLB_CTRLr, MANGO_HASH_SELf, &val),
            ret, "field read fail");

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_hashAlgo_set */

/* Function Name:
 *      _dal_mango_mpls_decap_config
 * Description:
 *      Configure an MPLS decapsulation entry by decapsulation ID
 * Input:
 *      unit    - unit id
 *      encapId - MPLS decapsulation entry ID
 *      pEncap  - pointer to nexthop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      semaphore and pointer check by caller.
 */
int32
_dal_mango_mpls_decap_config(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap)
{
    mpls_decap_entry_t  entry;
    uint32              table, entryId, val;
    int32               ret;

    osal_memset(&entry, 0, sizeof(entry));

    if (HAL_MAX_NUM_OF_MPLS_DECAP(unit) > decapId)
    {
        table = MANGO_MPLS_DECAPt;
        entryId = decapId;
    }
    else
    {
        table = MANGO_MPLS_DECAP_CAMt;
        entryId = decapId - HAL_MAX_NUM_OF_MPLS_DECAP(unit);
    }

    val = 1;
    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_VALIDtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    /* label */
    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_LABELtf, &pDecap->label, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    /* label action */
    switch (pDecap->labelAct)
    {
        case RTK_MPLS_LABELACT_POP:
            val = 0;
            break;
        case RTK_MPLS_LABELACT_SWAP:
            val = 1;
            break;
        case RTK_MPLS_LABELACT_PHP:
            val = 2;
            break;
        default:
            RT_ERR(RT_ERR_FWD_ACTION, (MOD_DAL|MOD_MPLS),
                    "label action %d is not support", pDecap->labelAct);
            return RT_ERR_FWD_ACTION;
    }
    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_DECAP_ACTtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    /* ECMP */
    if (pDecap->flags & RTK_MPLS_FLAG_ECMP)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_ECMP_ENtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    /* TTL inherit */
    if (pDecap->flags & RTK_MPLS_FLAG_TTL_INHERIT)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_TTL_INHERITtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    /* TC inherit */
    if (pDecap->flags & RTK_MPLS_FLAG_TC_INHERIT)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_DSCP_INHERITtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    /* priority select table index */
    val = pDecap->priSelTblId;
    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_PRI_SEL_TBL_IDXtf, &val, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    /* internal priority */
    if (pDecap->flags & RTK_MPLS_FLAG_INTPRI_ASSIGN)
    {
        val = 1;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_INT_PRI_ENtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", decapId);

        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_INT_PRItf, &pDecap->intPri, (uint32 *)&entry),
                ret, "table set %u field fail", decapId);
    }
    else
    {
        val = 0;
        MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_INT_PRI_ENtf, &val, (uint32 *)&entry),
                ret, "table set %u field fail", decapId);
    }

    /* interface, next hop, ECMP index */
    MPLS_ERR_CHK_MSG_ARGS(table_field_set(unit, table, MANGO_MPLS_DECAP_INTF_IDtf, &pDecap->intfId, (uint32 *)&entry),
            ret, "table set %u field fail", decapId);

    MPLS_ERR_CHK_MSG_ARGS(table_write(unit, table, entryId, (uint32 *)&entry),
            ret, "table write %u field fail", decapId);

    return ret;
}   /* end of _dal_mango_mpls_decap_config */

/* Function Name:
 *      _dal_mango_mpls_labelToHashId_get
 * Description:
 *      Get decapsulation hash index by MPLS label
 * Input:
 *      unit    - unit id
 *      label   - MPLS label
 * Output:
 *      pHashId - pointer to MPLS decapsultion hash ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      None
 */
static int32
_dal_mango_mpls_labelToHashId_get(uint32 unit, uint32 label,
    uint32 *pHashId)
{
    int32   ret;
    uint8   hashAlgo;

    /* parameter check */
    RT_PARAM_CHK((NULL == pHashId), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_ERR_CHK_MSG(_dal_mango_mpls_hashAlgo_get(unit, &hashAlgo),
            ret, "Get hash algorithm fail");

    switch (hashAlgo)
    {
        case RTK_MPLS_HASHALGO_0:
            *pHashId = ((label & 0xFF) ^ ((label >> 8) & 0xFF) ^ ((label >> 16) & 0xF));
            break;
        case RTK_MPLS_HASHALGO_1:
            *pHashId = ((label & 0xFF) ^ ((label >> 12) & 0xF) ^
                    ((((label >> 8) & 0xF) << 4) | ((label >> 19) & 0x1) |
                    (((label >> 18) & 0x1) << 1) | (((label >> 17) & 0x1) << 2) |
                    (((label >> 16) & 0x1) << 3)));
            break;
    }

    return RT_ERR_OK;
}   /* end of _dal_mango_mpls_labelToHashId_get */

/* Function Name:
 *      dal_mango_mpls_decap_create
 * Description:
 *      Create an MPLS decapsulation entry
 * Input:
 *      unit    - unit id
 *      pDecap  - pointer to MPLS decapsulation entry
 * Output:
 *      pDecapId    - pointer to MPLS decapsultion entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_INHERIT
 *          - RTK_MPLS_FLAG_TC_INHERIT
 *          - RTK_MPLS_FLAG_ECMP
 *          - RTK_MPLS_FLAG_INTPRI_ASSIGN
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_POP,
 *          - RTK_MPLS_LABELACT_SWAP
 *          - RTK_MPLS_LABELACT_PHP
 */
int32
dal_mango_mpls_decap_create(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId)
{
    mpls_decap_entry_t  entry;
    uint32              idx, val, hashId, entryId = 0;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    RT_PARAM_CHK((NULL == pDecap), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX(unit) <= pDecap->priSelTblId), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX(unit) < pDecap->intPri), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_MAX_MPLS_LABEL <= pDecap->label), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDecap), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDecapId), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_SEM_LOCK(unit);

    /* get idx by hash index */
    MPLS_ERR_CHK_UNLOCK_MSG(_dal_mango_mpls_labelToHashId_get(unit, pDecap->label, &hashId),
            ret, "Get hash index fail");

    hashId *= HAL_MPLS_HASHDEPTH(unit);
    for (idx = 0; idx < HAL_MPLS_HASHDEPTH(unit); ++idx)
    {
        entryId = hashId + idx;

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_read(unit, MANGO_MPLS_DECAPt, entryId, (uint32 *)&entry),
                ret, "table read %u fail", entryId);

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_DECAPt, MANGO_MPLS_DECAP_VALIDtf, &val, (uint32 *)&entry),
                ret, "table read %u valid field fail", entryId);

        if (0 == val)
            break;
    }

    if (HAL_MPLS_HASHDEPTH(unit) == idx)
    {
        /* find in cam */
        for (idx = 0; idx < HAL_MAX_NUM_OF_MPLS_DECAP_CAM(unit); ++idx)
        {
            MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_DECAP_CAMt, MANGO_MPLS_DECAP_VALIDtf, &val, (uint32 *)&entry),
                    ret, "table read %u valid field fail", entryId);

            if (0 == val)
                break;
        }

        if (HAL_MAX_NUM_OF_MPLS_DECAP_CAM(unit) == idx)
        {
            MPLS_SEM_UNLOCK(unit);
            RT_ERR(RT_ERR_TBL_FULL, (MOD_DAL|MOD_MPLS), "");
            return RT_ERR_TBL_FULL;
        }

        entryId = HAL_MAX_NUM_OF_MPLS_DECAP(unit) + idx;
    }

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_mpls_decap_config(unit, entryId, pDecap),
            ret, "create entry %u fail", entryId);

    *pDecapId = entryId;

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_decap_create */

/* Function Name:
 *      dal_mango_mpls_decap_destroy
 * Description:
 *      Destroy an MPLS decapsulation entry by decapsulation entry ID
 * Input:
 *      unit    - unit id
 *      decapId - MPLS decapsulation entry ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_decap_destroy(uint32 unit, rtk_mpls_entryId_t decapId)
{
    mpls_decap_entry_t  entry;
    uint32              table, entryId;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,decapId=%d",unit, decapId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_MPLS_DECAP_ENTRY(unit) <= decapId), RT_ERR_INPUT);

    /* function body */
    osal_memset(&entry, 0, sizeof(entry));

    if (HAL_MAX_NUM_OF_MPLS_DECAP(unit) > decapId)
    {
        table = MANGO_MPLS_DECAPt;
        entryId = decapId;
    }
    else
    {
        table = MANGO_MPLS_DECAP_CAMt;
        entryId = decapId - HAL_MAX_NUM_OF_MPLS_DECAP(unit);
    }

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_write(unit, table, entryId, (uint32 *)&entry),
            ret, "table write %u field fail", decapId);

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_decap_destroy */

/* Function Name:
 *      dal_mango_mpls_decap_get
 * Description:
 *      Get an MPLS decapsulation entry
 * Input:
 *      unit     - unit id
 *      decapId  - MPLS decapsulation entry ID
 * Output:
 *      pDecap   - pointer to MPLS decapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_decap_get(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap)
{
    mpls_decap_entry_t  entry;
    uint32              table, entryId, val;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,decapId=%d",unit, decapId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_MPLS_DECAP_ENTRY(unit) <= decapId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pDecap), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(pDecap, 0, sizeof(rtk_mpls_decapEntry_t));

    if (HAL_MAX_NUM_OF_MPLS_DECAP(unit) > decapId)
    {
        table = MANGO_MPLS_DECAPt;
        entryId = decapId;
    }
    else
    {
        table = MANGO_MPLS_DECAP_CAMt;
        entryId = decapId - HAL_MAX_NUM_OF_MPLS_DECAP(unit);
    }

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_read(unit, table, entryId, (uint32 *)&entry),
            ret, "table read %u fail", decapId);

    MPLS_SEM_UNLOCK(unit);

    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_VALIDtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    if (0 == val)
    {
        return RT_ERR_ENTRY_NOTFOUND;
    }

    /* label */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_LABELtf, &pDecap->label, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    /* label action */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_DECAP_ACTtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    switch (val)
    {
        case 0:
            pDecap->labelAct = RTK_MPLS_LABELACT_POP;
            break;
        case 1:
            pDecap->labelAct = RTK_MPLS_LABELACT_SWAP;
            break;
        case 2:
            pDecap->labelAct = RTK_MPLS_LABELACT_PHP;
            break;
        default:
            RT_ERR(RT_ERR_FWD_ACTION, (MOD_DAL|MOD_MPLS), "label action %u is not support", val);
            return RT_ERR_FWD_ACTION;
    }

    /* ECMP */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_ECMP_ENtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    if (1 == val)
    {
        pDecap->flags |= RTK_MPLS_FLAG_ECMP;
    }

    /* TTL inherit */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_TTL_INHERITtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    if (1 == val)
    {
        pDecap->flags |= RTK_MPLS_FLAG_TTL_INHERIT;
    }

    /* TC inherit */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_DSCP_INHERITtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    if (1 == val)
    {
        pDecap->flags |= RTK_MPLS_FLAG_TC_INHERIT;
    }

    /* priority select table index */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_PRI_SEL_TBL_IDXtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    pDecap->priSelTblId = val;

    /* internal priority */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_INT_PRI_ENtf, &val, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    if (1 == val)
    {
        pDecap->flags |= RTK_MPLS_FLAG_INTPRI_ASSIGN;

        MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_INT_PRItf, &pDecap->intPri, (uint32 *)&entry),
                ret, "table get %u field fail", decapId);
    }

    /* interface, next hop, ECMP index */
    MPLS_ERR_CHK_MSG_ARGS(table_field_get(unit, table, MANGO_MPLS_DECAP_INTF_IDtf, &pDecap->intfId, (uint32 *)&entry),
            ret, "table get %u field fail", decapId);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_decap_get */

/* Function Name:
 *      dal_mango_mpls_decap_set
 * Description:
 *      Set an MPLS decapsulation entry
 * Input:
 *      unit     - unit id
 *      decapId  - MPLS decapsulation entry ID
 * Output:
 *      pDecap   - pointer to MPLS decapsulation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_INHERIT
 *          - RTK_MPLS_FLAG_TC_INHERIT
 *          - RTK_MPLS_FLAG_ECMP
 *          - RTK_MPLS_FLAG_INTPRI_ASSIGN
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_POP,
 *          - RTK_MPLS_LABELACT_SWAP
 *          - RTK_MPLS_LABELACT_PHP
 */
int32
dal_mango_mpls_decap_set(uint32 unit, rtk_mpls_entryId_t decapId,
    rtk_mpls_decapEntry_t *pDecap)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,decapId=%d",unit, decapId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_MPLS_DECAP_ENTRY(unit) <= decapId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pDecap), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_PRIORITY_REMAP_GROUP_IDX_MAX(unit) <= pDecap->priSelTblId), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX(unit) < pDecap->intPri), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_MAX_MPLS_LABEL <= pDecap->label), RT_ERR_INPUT);

    /* function body */
    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_mpls_decap_config(unit, decapId, pDecap),
            ret, "create entry %u fail", decapId);

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_decap_set */

/* Function Name:
 *      dal_mango_mpls_decapId_find
 * Description:
 *      Find an ID points to a MPLS decapsulation entry
 * Input:
 *      unit    - unit id
 *      pDecap  - pointer to MPLS decapsulation entry
 * Output:
 *      pDecapId    - pointer to MPLS decapsulation entry ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_decapId_find(uint32 unit, rtk_mpls_decapEntry_t *pDecap,
    rtk_mpls_entryId_t *pDecapId)
{
    mpls_decap_entry_t  entry;
    uint32              hashId, entryId;
    uint32              val, idx;
    int32               ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);
    MPLS_TABLE_FMT_CHECK(unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pDecap), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDecapId), RT_ERR_NULL_POINTER);

    /* function body */
    MPLS_SEM_LOCK(unit);

    /* get idx by hash index */
    MPLS_ERR_CHK_UNLOCK_MSG(_dal_mango_mpls_labelToHashId_get(unit, pDecap->label, &hashId),
            ret, "Get hash index fail");

    hashId *= HAL_MPLS_HASHDEPTH(unit);

    for (idx = 0; idx < HAL_MPLS_HASHDEPTH(unit); ++idx)
    {
        entryId = hashId + idx;

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_read(unit, MANGO_MPLS_DECAPt, entryId, (uint32 *)&entry),
                ret, "table read %u fail", idx);

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_DECAPt, MANGO_MPLS_DECAP_VALIDtf, &val, (uint32 *)&entry),
                ret, "table read %u valid field fail", idx);

        if (0 == val)
            continue;

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_DECAPt, MANGO_MPLS_DECAP_LABELtf, &val, (uint32 *)&entry),
                ret, "table read %u valid field fail", idx);

        if (val == pDecap->label)
        {
            *pDecapId = entryId;
            MPLS_SEM_UNLOCK(unit);
            return RT_ERR_OK;
        }
    }

    /* find in cam */
    for (idx = 0; idx < HAL_MAX_NUM_OF_MPLS_DECAP_CAM(unit); ++idx)
    {
        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_read(unit, MANGO_MPLS_DECAP_CAMt, idx, (uint32 *)&entry),
                ret, "table read %u fail", idx);

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_DECAP_CAMt, MANGO_MPLS_DECAP_VALIDtf, &val, (uint32 *)&entry),
                ret, "table read %u valid field fail", idx);

        if (0 == val)
            continue;

        MPLS_ERR_CHK_UNLOCK_MSG_ARGS(table_field_get(unit, MANGO_MPLS_DECAP_CAMt, MANGO_MPLS_DECAP_LABELtf, &val, (uint32 *)&entry),
                ret, "table read %u valid field fail", idx);

        if (val == pDecap->label)
        {
            *pDecapId = idx + HAL_MAX_NUM_OF_MPLS_DECAP(unit);
            MPLS_SEM_UNLOCK(unit);
            return RT_ERR_OK;
        }
    }

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_ENTRY_NOTFOUND;
}   /* end of dal_mango_mpls_decapId_find */

/* Function Name:
 *      dal_mango_mpls_egrTcMap_get
 * Description:
 *      Get the map to egress MPLS TC configuration
 * Input:
 *      unit    - unit id
 *      src     - source map to egress MPLS TC
 * Output:
 *      pTc     - egress MPLS TC value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
int32
dal_mango_mpls_egrTcMap_get(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 *pTc)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTc), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pSrc->dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);

    /* function body */
    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(reg_array_field_read(unit, MANGO_MPLS_DPINTPRI2TC_CTRLr, pSrc->dp, pSrc->pri, MANGO_TCf, &val),
            ret, "get register fail");

    MPLS_SEM_UNLOCK(unit);

    *pTc = val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pTc=%u", *pTc);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_egrTcMap_get */

/* Function Name:
 *      dal_mango_mpls_egrTcMap_set
 * Description:
 *      Set the map to egress MPLS TC configuration
 * Input:
 *      unit    - unit id
 *      src     - source map to egress MPLS TC
 *      tc      - egress MPLS TC value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - Invalid input
 *      RT_ERR_DROP_PRECEDENCE  - invalid drop precedence
 * Note:
 *      None
 */
int32
dal_mango_mpls_egrTcMap_set(uint32 unit, rtk_mpls_egrTcMapSrc_t *pSrc, uint8 tc)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,tc=%d", unit, tc);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSrc), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pSrc->dp > RTK_DROP_PRECEDENCE_MAX), RT_ERR_DROP_PRECEDENCE);
    RT_PARAM_CHK((RTK_MAX_NUM_OF_PRIORITY <= tc), RT_ERR_INPUT);

    /* function body */
    val = tc;
    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(reg_array_field_write(unit, MANGO_MPLS_DPINTPRI2TC_CTRLr, pSrc->dp, pSrc->pri, MANGO_TCf, &val),
            ret, "get register fail");

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_egrTcMap_set */

/* Function Name:
 *      dal_mango_mpls_nextHop_create_id
 * Description:
 *      Create an MPLS nexthop by specific path ID
 * Input:
 *      unit    - unit id
 *      pNh     - pointer to nexthop
 *      pathId  - L3 path ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 */
int32
dal_mango_mpls_nextHop_create_id(uint32 unit, rtk_mpls_nextHop_t *pNh,
    rtk_l3_pathId_t pathId)
{
    uint32  nhId = DAL_MANGO_L3_PATH_ID_TO_NH_IDX(pathId);  /* L3 nexthop id */
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,pathId=%d", unit, pathId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((!DAL_MANGO_L3_PATH_ID_IS_MPLS(pathId)), RT_ERR_INPUT);
    RT_PARAM_CHK((DAL_MANGO_L3_NEXTHOP_MAX <= nhId), RT_ERR_INPUT);

    /* function body */
    MPLS_ERR_CHK_MSG_ARGS(_dal_mango_l3_nhEntry_alloc(unit, &nhId, (DAL_MANGO_L3_API_FLAG_MOD_MPLS | DAL_MANGO_L3_API_FLAG_WITH_ID)),
            ret, "Create L3 NH %u fail", nhId);

    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG(_dal_mango_mpls_nextHop_create(unit, nhId, pNh),
            ret, "Create MPLS NH fail");

    MPLS_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_mango_mpls_nextHop_create_id */

/* Function Name:
 *      dal_mango_mpls_encap_create_id
 * Description:
 *      Create an MPLS encapsulation entry by specific encapsulation ID
 * Input:
 *      unit    - unit id
 *      pEncap  - pointer to nexthop
 *      encapId - MPLS encapsulation entry ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_NOT_ALLOWED  - the module is not enabled
 * Note:
 *      (1) flags only support:
 *          - RTK_MPLS_FLAG_NONE,
 *          - RTK_MPLS_FLAG_TTL_ASSIGN
 *          - RTK_MPLS_FLAG_NEXTLABEL
 *      (2) labelAct only support:
 *          - RTK_MPLS_LABELACT_PUSH
 *          - RTK_MPLS_LABELACT_SWAP
 */
int32
dal_mango_mpls_encap_create_id(uint32 unit, rtk_mpls_encap_t *pEncap,
    rtk_mpls_entryId_t encapId)
{
    uint32  maxEntryNum;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_MPLS), "unit=%d,encapId=%d", unit, encapId);

    /* check Init status */
    RT_INIT_CHK(mpls_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEncap), RT_ERR_NULL_POINTER);
    RT_ERR_CHK(_dal_mango_mpls_maxEntry_get(unit, &maxEntryNum), ret);
    RT_PARAM_CHK((maxEntryNum <= encapId), RT_ERR_INPUT);

    /* function body */
    MPLS_SEM_LOCK(unit);

    MPLS_ERR_CHK_UNLOCK_MSG_ARGS(_dal_mango_mpls_encap_config(unit, encapId, pEncap),
            ret, "create entry %u fail", encapId);

    MPLS_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_mpls_encap_create_id */


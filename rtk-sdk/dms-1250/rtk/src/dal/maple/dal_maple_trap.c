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
 * Purpose : Definition those public Trap APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Packets trap to CPU setting.
 *            2) RMA (Reserved MAC address).
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <dal/maple/dal_maple_trap.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/trap.h>

/*
 * Symbol Definition
 */
#define MAPLE_UDL2RMA_ENTRY 6

/*
 * Data Declaration
 */
static uint32       trap_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t trap_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* trap semaphore handling */
#define TRAP_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(trap_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_TRAP), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define TRAP_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(trap_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_TRAP), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

static int32 _dal_maple_trap_init_config(uint32 unit);

/* Function Name:
 *      dal_maple_trapMapper_init
 * Description:
 *      Hook trap module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook trap module before calling any trap APIs.
 */
int32
dal_maple_trapMapper_init(dal_mapper_t *pMapper)
{
    pMapper->trap_init = dal_maple_trap_init;
    pMapper->trap_rmaGroupAction_get = dal_maple_trap_rmaGroupAction_get;
    pMapper->trap_rmaGroupAction_set = dal_maple_trap_rmaGroupAction_set;
    pMapper->trap_bypassStp_get = dal_maple_trap_bypassStp_get;
    pMapper->trap_bypassStp_set = dal_maple_trap_bypassStp_set;
    pMapper->trap_bypassVlan_get = dal_maple_trap_bypassVlan_get;
    pMapper->trap_bypassVlan_set = dal_maple_trap_bypassVlan_set;
    pMapper->trap_rmaGroupLearningEnable_get = dal_maple_trap_rmaGroupLearningEnable_get;
    pMapper->trap_rmaGroupLearningEnable_set = dal_maple_trap_rmaGroupLearningEnable_set;
    pMapper->trap_userDefineRma_get = dal_maple_trap_userDefineRma_get;
    pMapper->trap_userDefineRma_set = dal_maple_trap_userDefineRma_set;
    pMapper->trap_userDefineRmaEnable_get = dal_maple_trap_userDefineRmaEnable_get;
    pMapper->trap_userDefineRmaEnable_set = dal_maple_trap_userDefineRmaEnable_set;
    pMapper->trap_userDefineRmaAction_get = dal_maple_trap_userDefineRmaAction_get;
    pMapper->trap_userDefineRmaAction_set = dal_maple_trap_userDefineRmaAction_set;
    pMapper->trap_userDefineRmaLearningEnable_get = dal_maple_trap_userDefineRmaLearningEnable_get;
    pMapper->trap_userDefineRmaLearningEnable_set = dal_maple_trap_userDefineRmaLearningEnable_set;
    pMapper->trap_mgmtFrameAction_get = dal_maple_trap_mgmtFrameAction_get;
    pMapper->trap_mgmtFrameAction_set = dal_maple_trap_mgmtFrameAction_set;
    pMapper->trap_mgmtFramePri_get = dal_maple_trap_mgmtFramePri_get;
    pMapper->trap_mgmtFramePri_set = dal_maple_trap_mgmtFramePri_set;
    pMapper->trap_mgmtFrameLearningEnable_get = dal_maple_trap_mgmtFrameLearningEnable_get;
    pMapper->trap_mgmtFrameLearningEnable_set = dal_maple_trap_mgmtFrameLearningEnable_set;
    pMapper->trap_portMgmtFrameAction_get = dal_maple_trap_portMgmtFrameAction_get;
    pMapper->trap_portMgmtFrameAction_set = dal_maple_trap_portMgmtFrameAction_set;
    pMapper->trap_pktWithCFIAction_get = dal_maple_trap_pktWithCFIAction_get;
    pMapper->trap_pktWithCFIAction_set = dal_maple_trap_pktWithCFIAction_set;
    pMapper->trap_pktWithOuterCFIAction_get = dal_maple_trap_pktWithOuterCFIAction_get;
    pMapper->trap_pktWithOuterCFIAction_set = dal_maple_trap_pktWithOuterCFIAction_set;
    pMapper->trap_pktWithCFIPri_get = dal_maple_trap_pktWithCFIPri_get;
    pMapper->trap_pktWithCFIPri_set = dal_maple_trap_pktWithCFIPri_set;
    pMapper->trap_routeExceptionAction_get = dal_maple_trap_routeExceptionAction_get;
    pMapper->trap_routeExceptionAction_set = dal_maple_trap_routeExceptionAction_set;
    pMapper->trap_mgmtFrameMgmtVlanEnable_get = dal_maple_trap_mgmtFrameMgmtVlanEnable_get;
    pMapper->trap_mgmtFrameMgmtVlanEnable_set = dal_maple_trap_mgmtFrameMgmtVlanEnable_set;
    pMapper->trap_mgmtFrameSelfARPEnable_get = dal_maple_trap_mgmtFrameSelfARPEnable_get;
    pMapper->trap_mgmtFrameSelfARPEnable_set = dal_maple_trap_mgmtFrameSelfARPEnable_set;
    pMapper->trap_bpduFloodPortmask_get = dal_maple_trap_bpduFloodPortmask_get;
    pMapper->trap_bpduFloodPortmask_set = dal_maple_trap_bpduFloodPortmask_set;
    pMapper->trap_rmaLookupMissActionEnable_get = dal_maple_trap_rmaLookupMissActionEnable_get;
    pMapper->trap_rmaLookupMissActionEnable_set = dal_maple_trap_rmaLookupMissActionEnable_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_trap_init
 * Description:
 *      Initial the trap module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_maple_trap_init(uint32 unit)
{
    int32   ret;

    RT_INIT_REENTRY_CHK(trap_init[unit]);
    trap_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    trap_sem[unit] = osal_sem_mutex_create();
    if (0 == trap_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_TRAP), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    trap_init[unit] = INIT_COMPLETED;

    /* initialize default configuration */
    if ((ret = _dal_maple_trap_init_config(unit)) != RT_ERR_OK)
    {
        trap_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "init default configuration failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_maple_trap_init */

/* Function Name:
 *      dal_maple_trap_rmaGroupAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame in group.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 * Output:
 *      pRma_action        - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The supported Reserved Multicast Address action:
 *      - MGMT_ACTION_FORWARD
 *      - MGMT_ACTION_DROP
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
int32
dal_maple_trap_rmaGroupAction_get(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t *pRma_action)
{
    int32   ret;
    uint32  value;
    uint32  reg_field;

    /*BPDU & LLDP & PTP have per-port control register*/
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rmaGroup_frameType >= RMA_GROUP_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRma_action), RT_ERR_NULL_POINTER);

    /* get entry from CHIP*/
    switch (rmaGroup_frameType)
    {
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM:
            reg_field = MAPLE_OAM_ACTf;
            break;
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER:
            reg_field = MAPLE_RMA_02_ACTf;
            break;
        case RMA_GROUP_TYPE_03:
            reg_field = MAPLE_RMA_03_ACTf;
            break;
        case RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP:
            reg_field = MAPLE_RMA_0E_ACTf;
            break;
        case RMA_GROUP_TYPE_10:
            reg_field = MAPLE_RMA_10_ACTf;
            break;
        case RMA_GROUP_TYPE_GMRP:
            reg_field = MAPLE_RMA_20_ACTf;
            break;
        case RMA_GROUP_TYPE_GVRP:
            reg_field = MAPLE_RMA_21_ACTf;
            break;
        case RMA_GROUP_TYPE_MSRP:
            reg_field = MAPLE_RMA_22_ACTf;
            break;
        case RMA_GROUP_TYPE_0X:
            reg_field = MAPLE_RMA_0X_ACTf;
            break;
        case RMA_GROUP_TYPE_1X:
            reg_field = MAPLE_RMA_1X_ACTf;
            break;
        case RMA_GROUP_TYPE_2X:
            reg_field = MAPLE_RMA_2X_ACTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_RMA_CTRL_0r, reg_field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pRma_action = MGMT_ACTION_FORWARD;
            break;
        case 1:
            *pRma_action = MGMT_ACTION_DROP;
            break;
        case 2:
            *pRma_action = MGMT_ACTION_TRAP2CPU;
            break;
        case 3:
            *pRma_action = MGMT_ACTION_FLOOD_TO_ALL_PORT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pRma_action=%d", *pRma_action);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_trap_rmaGroupAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 *      rma_action         - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - Invalid input parameter
 *      RT_ERR_RMA_ACTION - Invalid RMA action
 * Note:
 *      The supported Reserved Multicast Address frame:
 *      Assignment                      Address
 *      RMA_SLOW_PROTOCOL_OAM           01-80-C2-00-00-02 & ethertype=0x8809 & subtype = 0x3
 *      RMA_SLOW_PROTOCOL_OTHER         01-80-C2-00-00-02 except OAM
 *      RMA_0E_EXCEPT_PTP_LLDP          01-80-C2-00-00-0E
 *      RMA_GMRP (GMRP Address)         01-80-C2-00-00-20
 *      RMA_GVRP (GVRP address)         01-80-C2-00-00-21
 *      RMA_MSRP                        01-80-C2-00-00-22
 *      RMA_0X                          01-80-C2-00-00-01~01-80-C2-00-00-0F, excluding listed above
 *      RMA_1X                          01-80-C2-00-00-01~10-80-C2-00-00-1F, excluding listed above
 *      RMA_2X                          01-80-C2-00-00-01~23-80-C2-00-00-2F, excluding listed above
 *
 *      The supported Reserved Multicast Address action:
 *      -   MGMT_ACTION_FORWARD
 *      -   MGMT_ACTION_DROP
 *      -   MGMT_ACTION_TRAP2CPU
 *      -   MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
int32
dal_maple_trap_rmaGroupAction_set(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t rma_action)
{
    int32   ret;
    uint32  value;
    uint32  reg_field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rmaGroup_frameType >= RMA_GROUP_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(rma_action >= MGMT_ACTION_END, RT_ERR_RMA_ACTION);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "rmaGroup_frameType=%d, rma_action=%d", rmaGroup_frameType, rma_action);

    /* get entry from CHIP*/
    switch (rmaGroup_frameType)
    {
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM:
            reg_field = MAPLE_OAM_ACTf;
            break;
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER:
            reg_field = MAPLE_RMA_02_ACTf;
            break;
        case RMA_GROUP_TYPE_03:
            reg_field = MAPLE_RMA_03_ACTf;
            break;
        case RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP:
            reg_field = MAPLE_RMA_0E_ACTf;
            break;
        case RMA_GROUP_TYPE_10:
            reg_field = MAPLE_RMA_10_ACTf;
            break;
        case RMA_GROUP_TYPE_GMRP:
            reg_field = MAPLE_RMA_20_ACTf;
            break;
        case RMA_GROUP_TYPE_GVRP:
            reg_field = MAPLE_RMA_21_ACTf;
            break;
        case RMA_GROUP_TYPE_MSRP:
            reg_field = MAPLE_RMA_22_ACTf;
            break;
        case RMA_GROUP_TYPE_0X:
            reg_field = MAPLE_RMA_0X_ACTf;
            break;
        case RMA_GROUP_TYPE_1X:
            reg_field = MAPLE_RMA_1X_ACTf;
            break;
        case RMA_GROUP_TYPE_2X:
            reg_field = MAPLE_RMA_2X_ACTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch (rma_action)
    {
        case MGMT_ACTION_FORWARD:
            value = 0;
            break;
        case MGMT_ACTION_DROP:
            value = 1;
            break;
        case MGMT_ACTION_TRAP2CPU:
            value = 2;
            break;
        case MGMT_ACTION_FLOOD_TO_ALL_PORT:
            value = 3;
            break;
        default:
            return RT_ERR_RMA_ACTION;
    }

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_RMA_CTRL_0r, reg_field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_trap_rmaGroupLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 * Output:
 *      pEnable            - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trap_rmaGroupLearningEnable_get(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;
    int32   index1,index2;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rmaGroup_frameType >= RMA_GROUP_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (rmaGroup_frameType)
    {   /*Not use array to find Action field, because enum maybe change*/
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM:
            index1 = 0;
            break;
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER:
            index1 = 1;
            break;
        case RMA_GROUP_TYPE_03:
            index1 = 2;
            break;
        case RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP:
            index1 = 3;
            break;
        case RMA_GROUP_TYPE_10:
            index1 = 4;
            break;
        case RMA_GROUP_TYPE_GMRP:
            index1 = 5;
            break;
        case RMA_GROUP_TYPE_GVRP:
            index1 = 6;
            break;
        case RMA_GROUP_TYPE_MSRP:
            index1 = 7;
            break;
        case RMA_GROUP_TYPE_0X:
            index1 = 8;
            break;
        case RMA_GROUP_TYPE_1X:
            index1 = 9;
            break;
        case RMA_GROUP_TYPE_2X:
            index1 = 10;
            break;
        default:
            return RT_ERR_INPUT;
    }

    index2 = REG_ARRAY_INDEX_NONE;

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, MAPLE_RMA_SMAC_LRN_CTRLr, index1, index2, MAPLE_LRNf, &value)) != RT_ERR_OK)  /*Fix you*/
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

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

    return RT_ERR_OK;
} /* end of dal_maple_trap_rmaGroupLearningEnable_get */

/* Function Name:
 *      dal_maple_trap_rmaGroupLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 *      enable             - enable status of SMAC learning
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
dal_maple_trap_rmaGroupLearningEnable_set(uint32 unit, rtk_trap_rmaGroup_frameType_t  rmaGroup_frameType, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;
    int32   index1, index2;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rmaGroup_frameType >= RMA_GROUP_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "rmaGroup_frameType=%d-enable=%d",rmaGroup_frameType, enable);

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

    switch (rmaGroup_frameType)
    {   /*Not use array to find Action field, because enum maybe change*/
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM:
            index1 = 0;
            break;
        case RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER:
            index1 = 1;
            break;
        case RMA_GROUP_TYPE_03:
            index1 = 2;
            break;
        case RMA_GROUP_TYPE_0E_EXCEPT_PTP_LLDP:
            index1 = 3;
            break;
        case RMA_GROUP_TYPE_10:
            index1 = 4;
            break;
        case RMA_GROUP_TYPE_GMRP:
            index1 = 5;
            break;
        case RMA_GROUP_TYPE_GVRP:
            index1 = 6;
            break;
        case RMA_GROUP_TYPE_MSRP:
            index1 = 7;
            break;
        case RMA_GROUP_TYPE_0X:
            index1 = 8;
            break;
        case RMA_GROUP_TYPE_1X:
            index1 = 9;
            break;
        case RMA_GROUP_TYPE_2X:
            index1 = 10;
            break;
        default:
            return RT_ERR_INPUT;
    }

    index2 = REG_ARRAY_INDEX_NONE;

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MAPLE_RMA_SMAC_LRN_CTRLr, index1, index2, MAPLE_LRNf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_rmaGroupLearningEnable_set */

/* Function Name:
 *      dal_maple_trap_bypassStp_get
 * Description:
 *      Get enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 * Output:
 *      pEnable   - pointer to enable status of bypassing STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_STP_TYPE_USER_DEF_0
 *      - BYPASS_STP_TYPE_USER_DEF_1
 *      - BYPASS_STP_TYPE_USER_DEF_2
 *      - BYPASS_STP_TYPE_USER_DEF_3
 *      - BYPASS_STP_TYPE_USER_DEF_4
 *      - BYPASS_STP_TYPE_USER_DEF_5
 *      - BYPASS_STP_TYPE_SLOW_PROTO
 *      - BYPASS_STP_TYPE_RMA_03
 *      - BYPASS_STP_TYPE_RMA_0X
 *      - BYPASS_STP_TYPE_EAPOL
 *      - BYPASS_STP_TYPE_PTP
 *      - BYPASS_STP_TYPE_LLDP
 */
int32
dal_maple_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_STP_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (frameType)
    {
        case BYPASS_STP_TYPE_USER_DEF_0:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_2:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET2r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_3:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET3r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_4:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET4r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_5:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET5r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_SLOW_PROTO:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_SLOW_PROTO_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_RMA_03:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_03_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_RMA_0X:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_0X_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_EAPOL:
            reg_idx = MAPLE_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_PTP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_PTP_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_LLDP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_LLDP_BYPASS_STPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, reg_idx, field_idx, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    /* chip's value translate */
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

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_trap_bypassStp_get */

/* Function Name:
 *      dal_maple_trap_bypassStp_set
 * Description:
 *      Set enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 *      enable    - enable status of bypassing STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_STP_TYPE_USER_DEF_0
 *      - BYPASS_STP_TYPE_USER_DEF_1
 *      - BYPASS_STP_TYPE_USER_DEF_2
 *      - BYPASS_STP_TYPE_USER_DEF_3
 *      - BYPASS_STP_TYPE_USER_DEF_4
 *      - BYPASS_STP_TYPE_USER_DEF_5
 *      - BYPASS_STP_TYPE_RMA_03
 *      - BYPASS_STP_TYPE_RMA_0X
 *      - BYPASS_STP_TYPE_SLOW_PROTO
 *      - BYPASS_STP_TYPE_EAPOL
 *      - BYPASS_STP_TYPE_PTP
 *      - BYPASS_STP_TYPE_LLDP
 */
int32
dal_maple_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, enable=%d", unit, frameType, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_STP_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (frameType)
    {
        case BYPASS_STP_TYPE_USER_DEF_0:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_2:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET2r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_3:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET3r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_4:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET4r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_5:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET5r;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_SLOW_PROTO:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_SLOW_PROTO_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_RMA_03:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_03_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_RMA_0X:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_0X_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_EAPOL:
            reg_idx = MAPLE_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = MAPLE_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_PTP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_PTP_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_LLDP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_LLDP_BYPASS_STPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* translate to chip value */
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

    TRAP_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, reg_idx, field_idx, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_bypassStp_set */

/* Function Name:
 *      dal_maple_trap_bypassVlan_get
 * Description:
 *      Get enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 * Output:
 *      pEnable   - pointer to enable status of bypassing  ingress fliter/accept frame type/vlan CFI=1
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_VLAN_TYPE_USER_DEF_0 = 0,
 *      - BYPASS_VLAN_TYPE_USER_DEF_1,
 *      - BYPASS_VLAN_TYPE_USER_DEF_2,
 *      - BYPASS_VLAN_TYPE_USER_DEF_3,
 *      - BYPASS_VLAN_TYPE_USER_DEF_4,
 *      - BYPASS_VLAN_TYPE_USER_DEF_5,
 *      - BYPASS_VLAN_TYPE_RMA_00,
 *      - BYPASS_VLAN_TYPE_RMA_02,
 *      - BYPASS_VLAN_TYPE_RMA_03,
 *      - BYPASS_VLAN_TYPE_RMA_0E,
 *      - BYPASS_VLAN_TYPE_RMA_0X,
 *      - BYPASS_VLAN_TYPE_OAM,
 *      - BYPASS_VLAN_TYPE_GMRP,
 *      - BYPASS_VLAN_TYPE_GVRP,
 *      - BYPASS_VLAN_TYPE_EAPOL,
 *      - BYPASS_VLAN_TYPE_PTP,
 *      - BYPASS_VLAN_TYPE_LLDP
 */
int32
dal_maple_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (frameType)
    {
        case BYPASS_VLAN_TYPE_USER_DEF_0:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_1:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_2:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET2r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_3:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET3r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_4:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET4r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_5:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET5r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_00:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_BPDU_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_02:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_SLOWPRO_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_03:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_03_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0E:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_0E_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0X:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_0X_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_OAM:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_OAM_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_GMRP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_GMRP_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_GVRP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_GVRP_BYPASS_VLANf;
            break;
       case BYPASS_VLAN_TYPE_EAPOL:
            reg_idx = MAPLE_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_PTP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_PTP_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_LLDP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_LLDP_BYPASS_VLANf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, reg_idx, field_idx, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    /* chip's value translate */
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

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_trap_bypassVlan_get */

/* Function Name:
 *      dal_maple_trap_bypassVlan_set
 * Description:
 *      Set enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 *      enable    - enable status of bypassing vlan ingress fliter/accept frame type/vlan CFI=1
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_VLAN_TYPE_USER_DEF_0 = 0,
 *      - BYPASS_VLAN_TYPE_USER_DEF_1,
 *      - BYPASS_VLAN_TYPE_USER_DEF_2,
 *      - BYPASS_VLAN_TYPE_USER_DEF_3,
 *      - BYPASS_VLAN_TYPE_USER_DEF_4,
 *      - BYPASS_VLAN_TYPE_USER_DEF_5,
 *      - BYPASS_VLAN_TYPE_RMA_00,
 *      - BYPASS_VLAN_TYPE_RMA_02,
 *      - BYPASS_VLAN_TYPE_RMA_03,
 *      - BYPASS_VLAN_TYPE_RMA_0E,
 *      - BYPASS_VLAN_TYPE_RMA_0X,
 *      - BYPASS_VLAN_TYPE_OAM,
 *      - BYPASS_VLAN_TYPE_GMRP,
 *      - BYPASS_VLAN_TYPE_GVRP,
 *      - BYPASS_VLAN_TYPE_EAPOL,
 *      - BYPASS_VLAN_TYPE_PTP,
 *      - BYPASS_VLAN_TYPE_LLDP
 */
int32
dal_maple_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, enable=%d", unit, frameType, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (frameType)
    {
        case BYPASS_VLAN_TYPE_USER_DEF_0:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_1:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_2:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET2r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_3:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET3r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_4:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET4r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_5:
            reg_idx = MAPLE_RMA_USR_DEF_CTRL_SET5r;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_00:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_BPDU_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_02:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_SLOWPRO_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_03:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_03_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0E:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_0E_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0X:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_0X_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_OAM:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_OAM_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_GMRP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_GMRP_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_GVRP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_GVRP_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_EAPOL:
            reg_idx = MAPLE_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = MAPLE_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_PTP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_PTP_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_LLDP:
            reg_idx = MAPLE_RMA_CTRL_1r;
            field_idx = MAPLE_RMA_LLDP_BYPASS_VLANf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* translate to chip value */
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

    TRAP_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, reg_idx, field_idx, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_bypassVlan_set */

/* Function Name:
 *      dal_maple_trap_userDefineRma_get
 * Description:
 *      Get user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pUserDefinedRma - pointer to content of user defined RMA
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma)
{
    int32   ret;
    uint32  val = 0;
    uint32  baseAddr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pUserDefinedRma), RT_ERR_NULL_POINTER);

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_0r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);

    if (userDefine_idx < 2)
    {
        /* get entry from CHIP*/
        if ((ret = reg_read(unit, baseAddr, &val)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
        pUserDefinedRma->mac.octet[2] = (val >> 24) & 0xff;
        pUserDefinedRma->mac.octet[3] = (val >> 16) & 0xff;
        pUserDefinedRma->mac.octet[4] = (val >> 8) & 0xff;
        pUserDefinedRma->mac.octet[5] = val & 0xff;
        if ((ret = reg_read(unit, baseAddr + 1, &val)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }

        pUserDefinedRma->mac.octet[0] = (val >> 8) & 0xff;
        pUserDefinedRma->mac.octet[1] = val & 0xff;
        osal_memset(pUserDefinedRma->macMask.octet, 0xff, ETHER_ADDR_LEN);
    }
    else
    {
        /* get entry from CHIP*/
        if ((ret = reg_read(unit, baseAddr, &val)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
        pUserDefinedRma->mac.octet[0] = 0x01;
        pUserDefinedRma->mac.octet[1] = 0x80;
        pUserDefinedRma->mac.octet[2] = 0xc2;
        pUserDefinedRma->mac.octet[3] = 0x00;
        pUserDefinedRma->mac.octet[4] = 0x00;

        pUserDefinedRma->mac.octet[5] = val & 0xff;

        osal_memset(pUserDefinedRma->macMask.octet, 0xff, ETHER_ADDR_LEN);
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac=%x-%x-%x-%x-%x-%x",
           pUserDefinedRma->mac.octet[0], pUserDefinedRma->mac.octet[1], pUserDefinedRma->mac.octet[2],
           pUserDefinedRma->mac.octet[3], pUserDefinedRma->mac.octet[4], pUserDefinedRma->mac.octet[5]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->macMask=%x-%x-%x-%x-%x-%x",
           pUserDefinedRma->macMask.octet[0], pUserDefinedRma->macMask.octet[1], pUserDefinedRma->macMask.octet[2],
           pUserDefinedRma->macMask.octet[3], pUserDefinedRma->macMask.octet[4], pUserDefinedRma->macMask.octet[5]);

    return RT_ERR_OK;
} /* end of dal_maple_trap_userDefineRma_get */

/* Function Name:
 *      dal_maple_trap_userDefineRma_set
 * Description:
 *      Set user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      pUserDefinedRma - to content of user defined RMA
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma)
{
    int32   ret;
    uint32  baseAddr;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d", unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pUserDefinedRma), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac=%x-%x-%x-%x-%x-%x",
       pUserDefinedRma->mac.octet[0], pUserDefinedRma->mac.octet[1], pUserDefinedRma->mac.octet[2],
       pUserDefinedRma->mac.octet[3], pUserDefinedRma->mac.octet[4], pUserDefinedRma->mac.octet[5]);

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_0r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);

    if (userDefine_idx < 2)
    {
        /* get entry from CHIP*/
        value = (pUserDefinedRma->mac.octet[2] << 24) | (pUserDefinedRma->mac.octet[3] << 16) | (pUserDefinedRma->mac.octet[4] << 8) | (pUserDefinedRma->mac.octet[5]);
        if ((ret = reg_write(unit, baseAddr, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }

        value = (pUserDefinedRma->mac.octet[0] << 8) | (pUserDefinedRma->mac.octet[1]);
        if ((ret = reg_field_write(unit, baseAddr + 1, MAPLE_ADDR_HIf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else
    {
        /*The prefix should be 0x01-80-c2-00-00*/
        if ((pUserDefinedRma->mac.octet[0]  != 0x01) && (pUserDefinedRma->mac.octet[1]  != 0x80) &&
            (pUserDefinedRma->mac.octet[2]  != 0xc2) && (pUserDefinedRma->mac.octet[3]  != 0x00) &&
            (pUserDefinedRma->mac.octet[4]  != 0x00) )
        {
            TRAP_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
        }

        value = pUserDefinedRma->mac.octet[5];
        if ((ret = reg_field_write(unit, baseAddr, MAPLE_ADDR_LSBf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_userDefineRma_set */

/* Function Name:
 *      dal_maple_trap_userDefineRmaEnable_get
 * Description:
 *      Get enable status of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pEnable        - pointer to enable status of RMA entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, baseAddr,  MAPLE_ENf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == value)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_trap_userDefineRmaEnable_get */

/* Function Name:
 *      dal_maple_trap_userDefineRmaEnable_set
 * Description:
 *      Set enable status of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      enable         - enable status of RMA entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable)
{
    int32   ret;
    uint32  baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d, enable=%d", unit, userDefine_idx, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        value = 0;
    else
        value = 1;

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);

    /* set entry from CHIP*/
    if((ret = reg_field_write(unit, baseAddr,  MAPLE_ENf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_userDefineRmaEnable_set */

/* Function Name:
 *      dal_maple_trap_userDefineRmaAction_get
 * Description:
 *      Get forwarding action of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pActoin        - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      -   MGMT_ACTION_FORWARD
 *      -   MGMT_ACTION_DROP
 *      -   MGMT_ACTION_TRAP2CPU
 *      -   MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
int32
dal_maple_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction)
{
    int32   ret;
    uint32  baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, baseAddr,  MAPLE_ACTf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pAction = MGMT_ACTION_FORWARD;
            break;
        case 1:
            *pAction = MGMT_ACTION_DROP;
            break;
        case 2:
            *pAction = MGMT_ACTION_TRAP2CPU;
            break;
        case 3:
            *pAction = MGMT_ACTION_FLOOD_TO_ALL_PORT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_maple_trap_userDefineRmaAction_get */

/* Function Name:
 *      dal_maple_trap_userDefineRmaAction_set
 * Description:
 *      Set forwarding action of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      actoin         - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_FWD_ACTION   - invalid forwarding action
 * Note:
 *      Forwarding action is as following:
 *      -   MGMT_ACTION_FORWARD
 *      -   MGMT_ACTION_DROP
 *      -   MGMT_ACTION_TRAP2CPU
 *      -   MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
int32
dal_maple_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action)
{
    int32   ret;
    uint32  baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d, action=%d", unit, userDefine_idx, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((action >= MGMT_ACTION_END), RT_ERR_FWD_ACTION);

    switch (action)
    {
        case MGMT_ACTION_FORWARD:
            value = 0;
            break;
        case MGMT_ACTION_DROP:
            value = 1;
            break;
        case MGMT_ACTION_TRAP2CPU:
            value = 2;
            break;
        case MGMT_ACTION_FLOOD_TO_ALL_PORT:
            value = 3;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);

    /* set entry from CHIP*/
    if ((ret = reg_field_write(unit, baseAddr,  MAPLE_ACTf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_userDefineRmaAction_set */

/* Function Name:
 *      dal_maple_trap_userDefineRmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for user-defined RMA frame.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pEnable        - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  baseAddr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, baseAddr, MAPLE_LRNf, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_maple_trap_userDefineRmaLearningEnable_get */

/* Function Name:
 *      dal_maple_trap_userDefineRmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this user-defined RMA frame.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      enable         - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable)
{
    int32   ret;
    uint32  baseAddr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= MAPLE_UDL2RMA_ENTRY), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (userDefine_idx < 2)
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    else
        baseAddr = MAPLE_RMA_USR_DEF_CTRL_SET2r + (userDefine_idx-2);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, baseAddr, MAPLE_LRNf,&enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_userDefineRmaLearningEnable_set */

/* Function Name:
 *      dal_maple_trap_mgmtFrameMgmtVlanEnable_get
 * Description:
 *      Get compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - status of trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_trap_mgmtFrameMgmtVlanEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_CTRLr, MAPLE_VID_CMPf,
            &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_maple_trap_mgmtFrameMgmtVlanEnable_get */

/* Function Name:
 *      dal_maple_trap_mgmtFrameMgmtVlanEnable_set
 * Description:
 *      Set compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit   - unit id
 *      enable - status of trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_trap_mgmtFrameMgmtVlanEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_CTRLr, MAPLE_VID_CMPf,
            &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_mgmtFrameMgmtVlanEnable_set */


/* Function Name:
 *      dal_maple_trap_mgmtFrameSelfARPEnable_get
 * Description:
 *      Get state of copy Self-ARP to CPU.
 * Input:
 *      unit   - unit id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      (1) The configuration only applies to ARP request(MGMT_TYPE_ARP) packet.
 *      (2) All the ARP Request packets are copied to CPU by setting rtk_trap_mgmtFrameAction_set(MGMT_TYPE_ARP).
 *          But if the function is enabled, Only ARP Request destined to switch's IP (rtk_switch_IPv4Addr_set)
 *          is copied to CPU.
 */
int32
dal_maple_trap_mgmtFrameSelfARPEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_CTRLr, MAPLE_IP_CMPf,
            &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == val)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_trap_mgmtFrameSelfARPEnable_set
 * Description:
 *      Set state of copy Self-ARP to CPU.
 * Input:
 *      unit   - unit id
 *      enable - enable state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      (1) The configuration only applies to ARP request(MGMT_TYPE_ARP) packet.
 *      (2) All the ARP Request packets are copied to CPU by setting rtk_trap_mgmtFrameAction_set(MGMT_TYPE_ARP).
 *          But if the function is enabled, Only ARP Request destined to switch's IP (rtk_switch_IPv4Addr_set)
 *          is copied to CPU.
 */
int32
dal_maple_trap_mgmtFrameSelfARPEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_CTRLr, MAPLE_IP_CMPf,
            &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_maple_trap_mgmtFrameAction_get
 * Description:
 *      Get forwarding action of management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pActoin   - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 */
int32
dal_maple_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType >= MGMT_TYPE_END), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    switch (frameType)
    {
        case MGMT_TYPE_MLD:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_IGMP_CTRLr, MAPLE_MLD_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else
                *pAction = ACTION_TRAP2CPU;
            break;
        case MGMT_TYPE_IGMP:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_IGMP_CTRLr, MAPLE_IGMP_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else
                *pAction = ACTION_TRAP2CPU;
            break;
        case MGMT_TYPE_EAPOL:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_EAPOL_CTRLr, MAPLE_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else
                *pAction = ACTION_TRAP2CPU;
            break;
        case MGMT_TYPE_ARP:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_ARP_CTRLr, MAPLE_REQ_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else
                *pAction = ACTION_COPY2CPU;
            break;
        case MGMT_TYPE_IPV6ND:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_IPV6_CTRLr, MAPLE_ND_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else
                *pAction = ACTION_COPY2CPU;
            break;
        case MGMT_TYPE_IPV6_HOP_POS_ERR:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_IPV6_CTRLr, MAPLE_HBHEXTHDRERR_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else
                *pAction = ACTION_TRAP2CPU;
            break;
        case MGMT_TYPE_IPV6_HDR_UNKWN:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_IPV6_CTRLr, MAPLE_UNKNOWNEXTHDR_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else
                *pAction = ACTION_TRAP2CPU;
            break;
        case MGMT_TYPE_SELFMAC:
            if ((ret = reg_field_read(unit, MAPLE_SPCL_TRAP_SWITCH_MAC_CTRLr, MAPLE_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            if (value == 0)
                *pAction = ACTION_FORWARD;
            else if (value == 1)
                *pAction = ACTION_DROP;
            else
                *pAction = ACTION_TRAP2CPU;
            break;
        default:
            TRAP_SEM_UNLOCK(unit);
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_maple_trap_mgmtFrameAction_get */

/* Function Name:
 *      dal_maple_trap_mgmtFrameAction_set
 * Description:
 *      Set forwarding action of management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      actoin    - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_FWD_ACTION    - invalid forwarding action
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 */
int32
dal_maple_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, action=%d", unit, frameType, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType >= MGMT_TYPE_END), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((action > MGMT_ACTION_COPY2CPU), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_MLD) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_IGMP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_EAPOL) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_ARP) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_IPV6ND) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_COPY2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_IPV6_HOP_POS_ERR)  && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_IPV6_HDR_UNKWN) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_SELFMAC) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);

    TRAP_SEM_LOCK(unit);

    if (action == MGMT_ACTION_FORWARD)
        value = 0;
    else
        value = 1;

    switch (frameType)
    {
        case MGMT_TYPE_MLD:
            if ((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_IGMP_CTRLr, MAPLE_MLD_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IGMP:
            if ((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_IGMP_CTRLr, MAPLE_IGMP_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_EAPOL:
            if ((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_EAPOL_CTRLr, MAPLE_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_ARP:
            if((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_ARP_CTRLr, MAPLE_REQ_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IPV6ND:
            if((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_IPV6_CTRLr, MAPLE_ND_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_SELFMAC:
            if (action == MGMT_ACTION_FORWARD)
                value = 0;
            else if (action == MGMT_ACTION_DROP)
                value = 1;
            else
                value = 2;
            if((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_SWITCH_MAC_CTRLr, MAPLE_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IPV6_HOP_POS_ERR:
            if((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_IPV6_CTRLr, MAPLE_HBHEXTHDRERR_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IPV6_HDR_UNKWN:
            if((ret = reg_field_write(unit, MAPLE_SPCL_TRAP_IPV6_CTRLr, MAPLE_UNKNOWNEXTHDR_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        default:
            TRAP_SEM_UNLOCK(unit);
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /*end of dal_maple_trap_mgmtFrameAction_set*/

/* Function Name:
 *      dal_maple_trap_mgmtFrameLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for the management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pEnable   - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_BPDU
 */
int32
dal_maple_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable)
{
    int32   ret = 0;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (frameType)
    {
        case MGMT_TYPE_PTP:
            regField = MAPLE_PTP_LRNf;
            break;
        case MGMT_TYPE_LLDP:
            regField = MAPLE_LLDP_LRNf;
            break;
        case MGMT_TYPE_BPDU:
            regField = MAPLE_BPDU_LRNf;
            break;
        default:
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_RMA_MGN_LRN_CTRLr, regField, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_mgmtFrameLearningEnable_get */

/* Function Name:
 *      dal_maple_trap_mgmtFrameLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for the management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      enable    - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 */
int32
dal_maple_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable)
{
    int32   ret = 0;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch (frameType)
    {
        case MGMT_TYPE_PTP:
            regField = MAPLE_PTP_LRNf;
            break;
        case MGMT_TYPE_LLDP:
            regField = MAPLE_LLDP_LRNf;
            break;
        case MGMT_TYPE_BPDU:
            regField = MAPLE_BPDU_LRNf;
            break;
        default:
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_RMA_MGN_LRN_CTRLr, regField, &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_mgmtFrameLearningEnable_set */

/* Function Name:
 *      dal_maple_trap_portMgmtFrameAction_get
 * Description:
 *      Get forwarding action of management frame on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      frameType - type of management frame
 * Output:
 *      pActoin   - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
int32
dal_maple_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, port=%d, frameType=%d",
                unit, port, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((frameType != MGMT_TYPE_PTP) && (frameType != MGMT_TYPE_LLDP)
                  && (frameType != MGMT_TYPE_BPDU), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /*Get entry From CHIP*/
    if (frameType == MGMT_TYPE_BPDU)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_RMA_PORT_BPDU_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if (frameType == MGMT_TYPE_PTP)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if (frameType == MGMT_TYPE_LLDP)
    {
        if ((ret = reg_array_field_read(unit, MAPLE_RMA_PORT_LLDP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }

    TRAP_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pAction = MGMT_ACTION_FORWARD;
            break;
        case 1:
            *pAction = MGMT_ACTION_DROP;
            break;
        case 2:
            *pAction = MGMT_ACTION_TRAP2CPU;
            break;
        case 3:
            *pAction = MGMT_ACTION_FLOOD_TO_ALL_PORT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_trap_portMgmtFrameAction_set
 * Description:
 *      Set forwarding action of management frame on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      frameType - type of management frame
 *      actoin    - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_FWD_ACTION    - invalid forwarding action
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
int32
dal_maple_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, port=%d, frameType=%d, action=%d",
                unit, port, frameType, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((frameType != MGMT_TYPE_PTP) && (frameType != MGMT_TYPE_LLDP)
                 && (frameType != MGMT_TYPE_BPDU), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK(((action > MGMT_ACTION_TRAP2CPU) && (action != MGMT_ACTION_FLOOD_TO_ALL_PORT)), RT_ERR_FWD_ACTION);

    switch (action)
    {
        case MGMT_ACTION_FORWARD:
            value = 0;
            break;
        case MGMT_ACTION_DROP:
            value = 1;
            break;
        case MGMT_ACTION_TRAP2CPU:
            value = 2;
            break;
        case MGMT_ACTION_FLOOD_TO_ALL_PORT:
            value = 3;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    TRAP_SEM_LOCK(unit);

    /* Set entry From CHIP */
    if (frameType == MGMT_TYPE_BPDU)
    {
        if ((ret = reg_array_field_write(unit, MAPLE_RMA_PORT_BPDU_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if (frameType == MGMT_TYPE_PTP)
    {
        if ((ret = reg_array_field_write(unit, MAPLE_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if (frameType == MGMT_TYPE_LLDP)
    {
        if ((ret = reg_array_field_write(unit, MAPLE_RMA_PORT_LLDP_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_trap_mgmtFramePri_get
 * Description:
 *      Get priority of trapped packet.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *     Supported management frame is as following:
 *     - MGMT_TYPE_SELFMAC,
 *     - MGMT_TYPE_OTHER,
 *     - MGMT_TYPE_IGR_VLAN_FLTR,
 *     - MGMT_TYPE_CFI,
 *     - MGMT_TYPE_UNKNOWN_DA,
 *     - MGMT_TYPE_RLDP_RLPP,
 *     - MGMT_TYPE_RMA,
 *     - MGMT_TYPE_SPECIAL_COPY,
 *     - MGMT_TYPE_SPECIAL_TRAP,
 *     - MGMT_TYPE_ROUT_EXCEPT,
 *     - MGMT_TYPE_MAC_CST_SYS,
 *     - MGMT_TYPE_MAC_CST_PORT,
 *     - MGMT_TYPE_MAC_CST_VLAN,
 */
int32
dal_maple_trap_mgmtFramePri_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  reg, field, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType >= MGMT_TYPE_END), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    switch (frameType)
    {
        case MGMT_TYPE_OTHER:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_0r;
            field = MAPLE_INVLDf;
            break;
        case MGMT_TYPE_IGR_VLAN_FLTR:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_0r;
            field = MAPLE_IGR_VLAN_FLTRf;
            break;
        case MGMT_TYPE_CFI:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_0r;
            field = MAPLE_CFIf;
            break;
        case MGMT_TYPE_RLDP_RLPP:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_RLDP_RLPPf;
            break;
        case MGMT_TYPE_MAC_CST_SYS:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_SYS_MAC_CONSTRTf;
            break;
        case MGMT_TYPE_MAC_CST_PORT:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_PORT_MAC_CONSTRTf;
            break;
        case MGMT_TYPE_MAC_CST_VLAN:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_VLAN_MAC_CONSTRTf;
            break;
        case MGMT_TYPE_RMA:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_RMAf;
            break;
        case MGMT_TYPE_SELFMAC:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_CPU_MACf;
            break;
        case MGMT_TYPE_UNKNOWN_DA:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_UNKNOWN_DAf;
            break;
        case MGMT_TYPE_SPECIAL_COPY:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_SPECIAL_COPYf;
            break;
        case MGMT_TYPE_SPECIAL_TRAP:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_SPECIAL_TRAPf;
            break;
        case MGMT_TYPE_ROUT_EXCEPT:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_ROUT_EXCEPTf;
            break;
        default:
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_LOCK(unit);

    /*Get entry from CHIP*/
    if((ret = reg_field_read(unit, reg, field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    *pPriority = value & 0x7;

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_mgmtFramePri_get */

/* Function Name:
 *      dal_maple_trap_mgmtFramePri_set
 * Description:
 *      Set priority of trapped packet.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      priority  - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_PRIORITY      - invalid priority value
 * Note:
 *     Supported management frame is as following:
 *     - MGMT_TYPE_SELFMAC,
 *     - MGMT_TYPE_OTHER,
 *     - MGMT_TYPE_IGR_VLAN_FLTR,
 *     - MGMT_TYPE_CFI,
 *     - MGMT_TYPE_UNKNOWN_DA,
 *     - MGMT_TYPE_RLDP_RLPP,
 *     - MGMT_TYPE_RMA,
 *     - MGMT_TYPE_SPECIAL_COPY,
 *     - MGMT_TYPE_SPECIAL_TRAP,
 *     - MGMT_TYPE_ROUT_EXCEPT,
 *     - MGMT_TYPE_MAC_CST_SYS,
 *     - MGMT_TYPE_MAC_CST_PORT,
 *     - MGMT_TYPE_MAC_CST_VLAN,
 */
int32
dal_maple_trap_mgmtFramePri_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t priority)
{
    int32   ret;
    uint32  reg, field, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, priority=%d", unit, frameType, priority);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType >= MGMT_TYPE_END), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((priority > RTK_DOT1P_PRIORITY_MAX), RT_ERR_PRIORITY);

    switch (frameType)
    {
        case MGMT_TYPE_OTHER:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_0r;
            field = MAPLE_INVLDf;
            break;
        case MGMT_TYPE_IGR_VLAN_FLTR:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_0r;
            field = MAPLE_IGR_VLAN_FLTRf;
            break;
        case MGMT_TYPE_CFI:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_0r;
            field = MAPLE_CFIf;
            break;
        case MGMT_TYPE_RLDP_RLPP:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_RLDP_RLPPf;
            break;
        case MGMT_TYPE_MAC_CST_SYS:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_SYS_MAC_CONSTRTf;
            break;
        case MGMT_TYPE_MAC_CST_PORT:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_PORT_MAC_CONSTRTf;
            break;
        case MGMT_TYPE_MAC_CST_VLAN:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_VLAN_MAC_CONSTRTf;
            break;
        case MGMT_TYPE_RMA:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_1r;
            field = MAPLE_RMAf;
            break;
        case MGMT_TYPE_SELFMAC:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_CPU_MACf;
            break;
        case MGMT_TYPE_UNKNOWN_DA:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_UNKNOWN_DAf;
            break;
        case MGMT_TYPE_SPECIAL_COPY:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_SPECIAL_COPYf;
            break;
        case MGMT_TYPE_SPECIAL_TRAP:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_SPECIAL_TRAPf;
            break;
        case MGMT_TYPE_ROUT_EXCEPT:
            reg   = MAPLE_QM_PKT2CPU_INTPRI_2r;
            field = MAPLE_ROUT_EXCEPTf;
            break;
        default:
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_LOCK(unit);

    /*Set entry to CHIP*/
    value = priority & 0x7;
    if((ret = reg_field_write(unit, reg, field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_mgmtFramePri_set */

/* Function Name:
 *      dal_maple_trap_pktWithCFIAction_get
 * Description:
 *      Get the configuration of inner CFI operation.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_maple_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pAction, RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MAPLE_VLAN_CTRLr, MAPLE_ICFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
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
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_maple_trap_pktWithCFIAction_get */

/* Function Name:
 *      dal_maple_trap_pktWithCFIAction_set
 * Description:
 *      Set the configuration of inner CFI operation.
 * Input:
 *      unit   - unit id
 *      action - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_maple_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "unit=%d, action=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    /* translate to chip's definition */
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
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_VLAN_CTRLr, MAPLE_ICFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_pktWithCFIAction_set */

/* Function Name:
 *      dal_maple_trap_pktWithOuterCFIAction_get
 * Description:
 *      Get the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_maple_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pAction, RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, MAPLE_VLAN_CTRLr, MAPLE_OCFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    switch (value)
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
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_maple_trap_pktWithOuterCFIAction_get */

/* Function Name:
 *      dal_maple_trap_pktWithOuterCFIAction_set
 * Description:
 *      Set the configuration of outer CFI operation.
 * Input:
 *      unit   - unit id
 *      action - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_maple_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "unit=%d, action=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    /* translate to chip's definition */
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
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, MAPLE_VLAN_CTRLr, MAPLE_OCFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_pktWithOuterCFIAction_set */

/* Function Name:
 *      dal_maple_trap_pktWithCFIPri_get
 * Description:
 *      Get priority of packets trapped to CPU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_trap_pktWithCFIPri_get(uint32 unit, rtk_pri_t *pPriority)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /*Get entry From CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_QM_PKT2CPU_INTPRI_0r, MAPLE_CFIf,
                    pPriority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pPriority=%d", *pPriority);

    return RT_ERR_OK;
} /* end of dal_maple_trap_pktWithCFIPri_get */

/* Function Name:
 *      dal_maple_trap_pktWithCFIPri_set
 * Description:
 *      Set priority of packets trapped to CPU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      None
 */
int32
dal_maple_trap_pktWithCFIPri_set(uint32 unit, rtk_pri_t priority)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, priority=%d",
                unit, priority);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((priority > RTK_DOT1P_PRIORITY_MAX), RT_ERR_PRIORITY);

    TRAP_SEM_LOCK(unit);

    /*Set entry to CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_QM_PKT2CPU_INTPRI_0r, MAPLE_CFIf,
                    &priority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_pktWithCFIPri_set */

/* Function Name:
 *      _dal_trap_routeExceptionActionRegField_get
 * Description:
 *      Get register and field for configure action of route exception packets.
 * Input:
 *      unit  - unit id
 *      type  - route exception type
 * Output:
 *      reg   - register for specfic route exception action.
 *      field - register field for specfic route exception action.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
static int32
_dal_trap_routeExceptionActionRegField_get(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    uint32                          *reg,
    uint32                          *field)
{
    /* parameter check */
    RT_PARAM_CHK((type >= ROUTE_EXCEPTION_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == reg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == field), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED:
            *reg     = MAPLE_ROUTING_EXCPT_CTRLr;
            *field   = MAPLE_IP6_HL_EXCEEDf;
            break;
        case ROUTE_EXCEPTION_TYPE_WITH_OPT:
            *reg     = MAPLE_ROUTING_EXCPT_CTRLr;
            *field   = MAPLE_IP4_OPTf;
            break;
        case ROUTE_EXCEPTION_TYPE_TTL_EXCEED:
            *reg     = MAPLE_ROUTING_EXCPT_CTRLr;
            *field   = MAPLE_IP4_TTL_EXECEEDf;
            break;
        case ROUTE_EXCEPTION_TYPE_GW_MAC_ERR:
            *reg     = MAPLE_ROUTING_EXCPT_CTRLr;
            *field   = MAPLE_GW_MAC_ERRf;
            break;
        case ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP:
            *reg     = MAPLE_ROUTING_EXCPT_CTRLr;
            *field   = MAPLE_IP6_HOPBYHOPf;
            break;
        case ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT:
             *reg = MAPLE_ROUTING_EXCPT_CTRLr;
             *field   = MAPLE_ROUTING_AGEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of _dal_trap_routeExceptionActionRegField_get */

/* Function Name:
 *      dal_maple_trap_routeExceptionAction_get
 * Description:
 *      Get the configuration of route exception operation.
 * Input:
 *      unit    - unit id
 *      type    - configure action for which route exception
 * Output:
 *      pAction - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_maple_trap_routeExceptionAction_get(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    *pAction)
{
    int32   ret;
    uint32  reg;
    uint32  field;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= ROUTE_EXCEPTION_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(((ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR == type) || (ROUTE_EXCEPTION_TYPE_HDR_ERR == type)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    ret = _dal_trap_routeExceptionActionRegField_get(unit, type, &reg, &field);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_LOCK(unit);

    /*Set entry to CHIP*/
    if ((ret = reg_field_read(unit, reg, field, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    switch (val)
    {
        case 0:
            *pAction = ACTION_DROP;
            break;
        case 1:
            *pAction = ACTION_FORWARD;
            break;
        case 2:
            *pAction = ACTION_TRAP2CPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_trap_routeExceptionAction_get */

/* Function Name:
 *      dal_maple_trap_routeExceptionAction_set
 * Description:
 *      Set the configuration of route exception  operation.
 * Input:
 *      unit   - unit id
 *      type   - configure action for which route exception
 *      action - route exception operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_maple_trap_routeExceptionAction_set(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    action)
{
    int32   ret;
    uint32  reg;
    uint32  field;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d type=%d action=%d",
            unit, type, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type >= ROUTE_EXCEPTION_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(((ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR == type) || (ROUTE_EXCEPTION_TYPE_HDR_ERR == type)), RT_ERR_INPUT);
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    ret = _dal_trap_routeExceptionActionRegField_get(unit, type, &reg, &field);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    switch (action)
    {
        case ACTION_DROP:
            val = 0;
            break;
        case ACTION_FORWARD:
            val = 1;
            break;
        case ACTION_TRAP2CPU:
            val = 2;
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /*Set entry to CHIP*/
    if ((ret = reg_field_write(unit, reg, field, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_trap_routeExceptionAction_set */


/* Function Name:
 *      dal_maple_trap_bpduFloodPortmask_get
 * Description:
 *      Get BPDU flooding portmask.
 * Input:
 *      unit   - unit id
 *      pBPDU_flood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_maple_trap_bpduFloodPortmask_get(uint32 unit,  rtk_portmask_t *pBPDU_flood_portmask)
{
    int32   ret;
    uint32  reg;
    uint32  field;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBPDU_flood_portmask), RT_ERR_NULL_POINTER);

    osal_memset(pBPDU_flood_portmask, 0 , sizeof(rtk_portmask_t));

    TRAP_SEM_LOCK(unit);

    reg = MAPLE_RMA_BPDU_FLD_PMSKr;
    field = MAPLE_PMSKf;

    /*Set  to CHIP*/
    if ((ret = reg_field_read(unit, reg, field, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    pBPDU_flood_portmask->bits[0] = val & 0x1fffffff;

    return RT_ERR_OK;
} /* end of dal_maple_trap_bpduFloodPortmask_get */



/* Function Name:
 *      dal_maple_trap_bpduFloodPortmask_set
 * Description:
 *      Set BPDU flooding portmask.
 * Input:
 *      unit   - unit id
 *      pBPDU_flood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_maple_trap_bpduFloodPortmask_set(uint32 unit,  rtk_portmask_t *pBPDU_flood_portmask)
{
    int32   ret;
    uint32  reg;
    uint32  field;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBPDU_flood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pBPDU_flood_portmask), RT_ERR_PORT_MASK);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK), "pBPDU_flood_portmask=%x", pBPDU_flood_portmask->bits[0]);

    TRAP_SEM_LOCK(unit);

    reg = MAPLE_RMA_BPDU_FLD_PMSKr;
    val = pBPDU_flood_portmask->bits[0] & 0x1fffffff;

    field = MAPLE_PMSKf;
    /*Set  to CHIP*/
    if ((ret = reg_field_write(unit, reg, field, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);


    return RT_ERR_OK;
} /* end of dal_maple_trap_bpduFloodPortmask_set */


/* Function Name:
 *      dal_maple_trap_rmaLookupMissActionEnable_get
 * Description:
 *      Get enable status of RMA care lookup miss action.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of RMA care lookup miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Enable is care lookup miss action.
 */
int32
dal_maple_trap_rmaLookupMissActionEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32           ret;
    rtk_enable_t    value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_RMA_CTRL_1r, MAPLE_RMA_BYPASS_LM_ACTf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (DISABLED == value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_maple_trap_rmaLookupMissActionEnable_get */

/* Function Name:
 *      dal_maple_trap_rmaLookupMissActionEnable_set
 * Description:
 *      Set enable status of RMA care lookup miss action.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of RMA care lookup miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Enable is care lookup miss action.
 */
int32
dal_maple_trap_rmaLookupMissActionEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32           ret;
    rtk_enable_t    value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,enable=%d",unit, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if (DISABLED == enable)
        value = ENABLED;
    else
        value = DISABLED;

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_RMA_CTRL_1r, MAPLE_RMA_BYPASS_LM_ACTf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_maple_trap_rmaLookupMissActionEnable_set */


/* Function Name:
 *      _dal_maple_trap_init_config
 * Description:
 *      Initialize default configuration for trap module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize trap module before calling this API
 */
static int32
_dal_maple_trap_init_config(uint32 unit)
{
    int32   ret;
    rtk_trap_rmaGroup_frameType_t rmaGroup_frameType;
    rtk_action_t rma_action;

    /*Set OAM Forward*/
    rmaGroup_frameType = RMA_GROUP_TYPE_SLOW_PROTOCOL_OAM;
    rma_action = ACTION_FORWARD;
    if ((ret = dal_maple_trap_rmaGroupAction_set(unit, rmaGroup_frameType, rma_action)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    /*Set Slow Protocol Forward*/
    rmaGroup_frameType = RMA_GROUP_TYPE_SLOW_PROTOCOL_OTHER;
    rma_action = ACTION_FORWARD;
    if ((ret = dal_maple_trap_rmaGroupAction_set(unit, rmaGroup_frameType, rma_action)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    /*Set 0x01-80-C2-00-00-03-0x2F forward*/
    rma_action = ACTION_FORWARD;
    for(rmaGroup_frameType = RMA_GROUP_TYPE_03; rmaGroup_frameType < RMA_GROUP_TYPE_END; rmaGroup_frameType++)
    {
        if ((ret = dal_maple_trap_rmaGroupAction_set(unit, rmaGroup_frameType, rma_action)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_maple_trap_init_config */



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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <dal/cypress/dal_cypress_trap.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/trap.h>
/*
 * Symbol Definition
 */
#define     RMA_ADDR_PREFIX_LEN         5

/*
 * Data Declaration
 */
static uint32               trap_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         trap_sem[RTK_MAX_NUM_OF_UNIT];

const static uint16 rmaControl_regidx[] = {CYPRESS_RMA_CTRL_0r, CYPRESS_RMA_CTRL_1r, CYPRESS_RMA_CTRL_2r, CYPRESS_RMA_CTRL_3r,};

const static uint16 rma_fieldidx[] = {0, CYPRESS_RMA_01_ACTf, CYPRESS_RMA_02_ACTf, CYPRESS_RMA_03_ACTf, CYPRESS_RMA_04_ACTf, CYPRESS_RMA_05_ACTf,
                                        CYPRESS_RMA_06_ACTf, CYPRESS_RMA_07_ACTf, CYPRESS_RMA_08_ACTf, CYPRESS_RMA_09_ACTf, CYPRESS_RMA_0A_ACTf,
                                        CYPRESS_RMA_0B_ACTf, CYPRESS_RMA_0C_ACTf, CYPRESS_RMA_0D_ACTf, CYPRESS_RMA_0E_ACTf, CYPRESS_RMA_0F_ACTf,
                                        CYPRESS_RMA_10_ACTf, CYPRESS_RMA_11_ACTf, CYPRESS_RMA_12_ACTf, CYPRESS_RMA_13_ACTf, CYPRESS_RMA_14_ACTf,
                                        CYPRESS_RMA_15_ACTf, CYPRESS_RMA_16_ACTf, CYPRESS_RMA_17_ACTf, CYPRESS_RMA_18_ACTf, CYPRESS_RMA_19_ACTf,
                                        CYPRESS_RMA_1A_ACTf, CYPRESS_RMA_1B_ACTf, CYPRESS_RMA_1C_ACTf, CYPRESS_RMA_1D_ACTf, CYPRESS_RMA_1E_ACTf,
                                        CYPRESS_RMA_1F_ACTf, CYPRESS_RMA_20_ACTf, CYPRESS_RMA_21_ACTf, CYPRESS_RMA_22_ACTf, CYPRESS_RMA_23_ACTf,
                                        CYPRESS_RMA_24_ACTf, CYPRESS_RMA_25_ACTf, CYPRESS_RMA_26_ACTf, CYPRESS_RMA_27_ACTf, CYPRESS_RMA_28_ACTf,
                                        CYPRESS_RMA_29_ACTf, CYPRESS_RMA_2A_ACTf, CYPRESS_RMA_2B_ACTf, CYPRESS_RMA_2C_ACTf, CYPRESS_RMA_2D_ACTf,
                                        CYPRESS_RMA_2E_ACTf, CYPRESS_RMA_2F_ACTf};


/* prefix of reserve multicast address */
const static uint8 rma_prefix[5] = {0x01, 0x80, 0xC2, 0x00, 0x00};

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

/* Function Name:
 *      dal_cypress_trapMapper_init
 * Description:
 *      Hook trap module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook trap module before calling any trap APIs.
 */
int32
dal_cypress_trapMapper_init(dal_mapper_t *pMapper)
{
    pMapper->trap_init = dal_cypress_trap_init;
    pMapper->trap_rmaAction_get = dal_cypress_trap_rmaAction_get;
    pMapper->trap_rmaAction_set = dal_cypress_trap_rmaAction_set;
    pMapper->trap_bypassStp_get = dal_cypress_trap_bypassStp_get;
    pMapper->trap_bypassStp_set = dal_cypress_trap_bypassStp_set;
    pMapper->trap_bypassVlan_get = dal_cypress_trap_bypassVlan_get;
    pMapper->trap_bypassVlan_set = dal_cypress_trap_bypassVlan_set;
    pMapper->trap_rmaLearningEnable_get = dal_cypress_trap_rmaLearningEnable_get;
    pMapper->trap_rmaLearningEnable_set = dal_cypress_trap_rmaLearningEnable_set;
    pMapper->trap_userDefineRma_get = dal_cypress_trap_userDefineRma_get;
    pMapper->trap_userDefineRma_set = dal_cypress_trap_userDefineRma_set;
    pMapper->trap_userDefineRmaEnable_get = dal_cypress_trap_userDefineRmaEnable_get;
    pMapper->trap_userDefineRmaEnable_set = dal_cypress_trap_userDefineRmaEnable_set;
    pMapper->trap_userDefineRmaAction_get = dal_cypress_trap_userDefineRmaAction_get;
    pMapper->trap_userDefineRmaAction_set = dal_cypress_trap_userDefineRmaAction_set;
    pMapper->trap_userDefineRmaLearningEnable_get = dal_cypress_trap_userDefineRmaLearningEnable_get;
    pMapper->trap_userDefineRmaLearningEnable_set = dal_cypress_trap_userDefineRmaLearningEnable_set;
    pMapper->trap_mgmtFrameAction_get = dal_cypress_trap_mgmtFrameAction_get;
    pMapper->trap_mgmtFrameAction_set = dal_cypress_trap_mgmtFrameAction_set;
    pMapper->trap_mgmtFramePri_get = dal_cypress_trap_mgmtFramePri_get;
    pMapper->trap_mgmtFramePri_set = dal_cypress_trap_mgmtFramePri_set;
    pMapper->trap_mgmtFrameLearningEnable_get = dal_cypress_trap_mgmtFrameLearningEnable_get;
    pMapper->trap_mgmtFrameLearningEnable_set = dal_cypress_trap_mgmtFrameLearningEnable_set;
    pMapper->trap_portMgmtFrameAction_get = dal_cypress_trap_portMgmtFrameAction_get;
    pMapper->trap_portMgmtFrameAction_set = dal_cypress_trap_portMgmtFrameAction_set;
    pMapper->trap_pktWithCFIAction_get = dal_cypress_trap_pktWithCFIAction_get;
    pMapper->trap_pktWithCFIAction_set = dal_cypress_trap_pktWithCFIAction_set;
    pMapper->trap_pktWithOuterCFIAction_get = dal_cypress_trap_pktWithOuterCFIAction_get;
    pMapper->trap_pktWithOuterCFIAction_set = dal_cypress_trap_pktWithOuterCFIAction_set;
    pMapper->trap_pktWithCFIPri_get = dal_cypress_trap_pktWithCFIPri_get;
    pMapper->trap_pktWithCFIPri_set = dal_cypress_trap_pktWithCFIPri_set;
    pMapper->trap_cfmAct_get = dal_cypress_trap_cfmAct_get;
    pMapper->trap_cfmAct_set = dal_cypress_trap_cfmAct_set;
    pMapper->trap_cfmFrameTrapPri_get = dal_cypress_trap_cfmFrameTrapPri_get;
    pMapper->trap_cfmFrameTrapPri_set = dal_cypress_trap_cfmFrameTrapPri_set;
    pMapper->trap_oamPDUAction_get = dal_cypress_trap_oamPDUAction_get;
    pMapper->trap_oamPDUAction_set = dal_cypress_trap_oamPDUAction_set;
    pMapper->trap_oamPDUPri_get = dal_cypress_trap_oamPDUPri_get;
    pMapper->trap_oamPDUPri_set = dal_cypress_trap_oamPDUPri_set;
    pMapper->trap_portOamLoopbackParAction_get = dal_cypress_trap_portOamLoopbackParAction_get;
    pMapper->trap_portOamLoopbackParAction_set = dal_cypress_trap_portOamLoopbackParAction_set;
    pMapper->trap_routeExceptionAction_get = dal_cypress_trap_routeExceptionAction_get;
    pMapper->trap_routeExceptionAction_set = dal_cypress_trap_routeExceptionAction_set;
    pMapper->trap_routeExceptionPri_get = dal_cypress_trap_routeExceptionPri_get;
    pMapper->trap_routeExceptionPri_set = dal_cypress_trap_routeExceptionPri_set;
    pMapper->trap_mgmtFrameMgmtVlanEnable_get = dal_cypress_trap_mgmtFrameMgmtVlanEnable_get;
    pMapper->trap_mgmtFrameMgmtVlanEnable_set = dal_cypress_trap_mgmtFrameMgmtVlanEnable_set;
    pMapper->trap_mgmtFrameSelfARPEnable_get = dal_cypress_trap_mgmtFrameSelfARPEnable_get;
    pMapper->trap_mgmtFrameSelfARPEnable_set = dal_cypress_trap_mgmtFrameSelfARPEnable_set;
    pMapper->trap_bpduFloodPortmask_get = dal_cypress_trap_bpduFloodPortmask_get;
    pMapper->trap_bpduFloodPortmask_set = dal_cypress_trap_bpduFloodPortmask_set;
    pMapper->trap_rmaLookupMissActionEnable_get = dal_cypress_trap_rmaLookupMissActionEnable_get;
    pMapper->trap_rmaLookupMissActionEnable_set = dal_cypress_trap_rmaLookupMissActionEnable_set;

    return RT_ERR_OK;
}
static int32 _dal_cypress_trap_init_config(uint32 unit);


/* Function Name:
 *      dal_cypress_trap_init
 * Description:
 *      Initial the trap module of the specified device..
 * Input:
 *      unit     - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_cypress_trap_init(uint32 unit)
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
    if ((ret = _dal_cypress_trap_init_config(unit)) != RT_ERR_OK)
    {
        trap_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "init default configuration failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_trap_init */

/* Function Name:
 *      dal_cypress_trap_rmaAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame.
 * Input:
 *      unit                - unit id
 *      pRma_frame         - Reserved multicast address.
 * Output:
 *      pRma_action        - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_RMA_ADDR      - invalid RMA address
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_trap_rmaAction_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t *pRma_action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRma_action), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRma_frame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(osal_memcmp(pRma_frame->octet, rma_prefix, RMA_ADDR_PREFIX_LEN), RT_ERR_RMA_ADDR);
    RT_PARAM_CHK(pRma_frame->octet[5] > 0x2f, RT_ERR_RMA_ADDR);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pRma_frame=%x-%x-%x-%x-%x-%x",
           pRma_frame->octet[0], pRma_frame->octet[1], pRma_frame->octet[2], pRma_frame->octet[3],
           pRma_frame->octet[4], pRma_frame->octet[5]);


    /*BPDU & LLDP have per-port control register*/
    if((pRma_frame->octet[5] == 0) || (pRma_frame->octet[5] == 0xcc))
    {
        /* *pRma_action = MGMT_ACTION_FORWARD; */
        return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, rmaControl_regidx[pRma_frame->octet[5] / 16], rma_fieldidx[pRma_frame->octet[5]], &value)) != RT_ERR_OK)
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
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pRma_action=%d", *pRma_action);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_rmaAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      unit                - unit id
 *      pRma_frame         - Reserved multicast address.
 *      rma_action          - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_RMA_ADDR      - invalid RMA address
 *      RT_ERR_RMA_ACTION    - Invalid RMA action
 * Note:
 *      The supported Reserved Multicast Address frame:
 *      Assignment                                                                  Address
 *      RMA_BRG_GROUP (Bridge Group Address)                                        01-80-C2-00-00-00
 *      RMA_FD_PAUSE (IEEE Std 802.3, 1988 Edition, Full Duplex PAUSE operation)    01-80-C2-00-00-01
 *      RMA_SP_MCAST (IEEE Std 802.3ad Slow Protocols-Multicast address)            01-80-C2-00-00-02
 *      RMA_1X_PAE (IEEE Std 802.1X PAE address)                                    01-80-C2-00-00-03
 *      RMA_RESERVED04 (Reserved)                                                   01-80-C2-00-00-04
 *      RMA_MEDIA_ACCESS_USE (Media Access Method Specific Use)                     01-80-C2-00-00-05
 *      RMA_RESERVED06 (Reserved)                                                   01-80-C2-00-00-06
 *      RMA_RESERVED07 (Reserved)                                                   01-80-C2-00-00-07
 *      RMA_PVD_BRG_GROUP (Provider Bridge Group Address)                           01-80-C2-00-00-08
 *      RMA_RESERVED09 (Reserved)                                                   01-80-C2-00-00-09
 *      RMA_RESERVED0A (Reserved)                                                   01-80-C2-00-00-0A
 *      RMA_RESERVED0B (Reserved)                                                   01-80-C2-00-00-0B
 *      RMA_RESERVED0C (Reserved)                                                   01-80-C2-00-00-0C
 *      RMA_MVRP (Provider Bridge MVRP Address)                                     01-80-C2-00-00-0D
 *      RMA_1ab_LL_DISCOVERY (802.1ab Link Layer Discover Protocol Address)         01-80-C2-00-00-0E
 *      RMA_RESERVED0F (Reserved)                                                   01-80-C2-00-00-0F
 *      RMA_BRG_MNGEMENT (All LANs Bridge Management Group Address)                 01-80-C2-00-00-10
 *      RMA_LOAD_SERV_GENERIC_ADDR (Load Server Generic Address)                    01-80-C2-00-00-11
 *      RMA_LOAD_DEV_GENERIC_ADDR (Loadable Device Generic Address)                 01-80-C2-00-00-12
 *      RMA_RESERVED13 (Reserved)                                                   01-80-C2-00-00-13
 *      RMA_RESERVED14 (Reserved)                                                   01-80-C2-00-00-14
 *      RMA_RESERVED15 (Reserved)                                                   01-80-C2-00-00-15
 *      RMA_RESERVED16 (Reserved)                                                   01-80-C2-00-00-16
 *      RMA_RESERVED17 (Reserved)                                                   01-80-C2-00-00-17
 *      RMA_MANAGER_STA_GENERIC_ADDR (Generic Address for All Manager Stations)     01-80-C2-00-00-18
 *      RMA_RESERVED19 (Reserved)                                                   01-80-C2-00-00-19
 *      RMA_AGENT_STA_GENERIC_ADDR (Generic Address for All Agent Stations)         01-80-C2-00-00-1A
 *      RMA_RESERVED1B (Reserved)                                                   01-80-C2-00-00-1B
 *      RMA_RESERVED1C (Reserved)                                                   01-80-C2-00-00-1C
 *      RMA_RESERVED1D (Reserved)                                                   01-80-C2-00-00-1D
 *      RMA_RESERVED1E (Reserved)                                                   01-80-C2-00-00-1E
 *      RMA_RESERVED1F (Reserved)                                                   01-80-C2-00-00-1F
 *      RMA_GMRP (GMRP Address)                                                     01-80-C2-00-00-20
 *      RMA_GVRP (GVRP address)                                                     01-80-C2-00-00-21
 *      RMA_UNDEF_GARP22~2F (Undefined GARP address)                                01-80-C2-00-00-22
 *                                                                                ~ 01-80-C2-00-00-2F
 *
 *      The supported Reserved Multicast Address action:
 *      -   MGMT_ACTION_FORWARD
 *      -   MGMT_ACTION_DROP
 *      -   MGMT_ACTION_TRAP2CPU
 */
int32
dal_cypress_trap_rmaAction_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t rma_action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRma_frame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(rma_action >= MGMT_ACTION_END, RT_ERR_RMA_ACTION);
    RT_PARAM_CHK(osal_memcmp(pRma_frame->octet, rma_prefix, RMA_ADDR_PREFIX_LEN), RT_ERR_RMA_ADDR);
    RT_PARAM_CHK(pRma_frame->octet[5] > 0x2f, RT_ERR_RMA_ADDR);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pRma_frame=%x-%x-%x-%x-%x-%x, rma_action=%d",
           pRma_frame->octet[0], pRma_frame->octet[1], pRma_frame->octet[2], pRma_frame->octet[3],
           pRma_frame->octet[4], pRma_frame->octet[5], rma_action);

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
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, rmaControl_regidx[pRma_frame->octet[5] / 16], rma_fieldidx[pRma_frame->octet[5]], &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_rmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_rmaLearningEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRma_frame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRma_frame->octet[5] > 0x2f, RT_ERR_RMA_ADDR);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pRma_frame=%x-%x-%x-%x-%x-%x",
           pRma_frame->octet[0], pRma_frame->octet[1], pRma_frame->octet[2], pRma_frame->octet[3],
           pRma_frame->octet[4], pRma_frame->octet[5]);


    TRAP_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_RMA_SMAC_LRN_CTRLr, pRma_frame->octet[5], REG_ARRAY_INDEX_NONE, CYPRESS_LRNf, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_rmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 *      enable     - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_trap_rmaLearningEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRma_frame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pRma_frame->octet[5] > 0x2f, RT_ERR_RMA_ADDR);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pRma_frame=%x-%x-%x-%x-%x-%x",
           pRma_frame->octet[0], pRma_frame->octet[1], pRma_frame->octet[2], pRma_frame->octet[3],
           pRma_frame->octet[4], pRma_frame->octet[5]);


    TRAP_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, CYPRESS_RMA_SMAC_LRN_CTRLr, pRma_frame->octet[5], REG_ARRAY_INDEX_NONE, CYPRESS_LRNf, &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_bypassStp_get
 * Description:
 *      Get enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 * Output:
 *      pEnable    - pointer to enable status of bypassing STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_RMA_0X
 *    - BYPASS_STP_TYPE_SLOW_PROTO
 *    - BYPASS_STP_TYPE_EAPOL
 *    - BYPASS_STP_TYPE_PTP
 *    - BYPASS_STP_TYPE_LLDP
 */
int32
dal_cypress_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable)
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
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = CYPRESS_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = CYPRESS_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_RMA_0X:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_0X_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_SLOW_PROTO:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_SLOW_PROTO_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_EAPOL:
            reg_idx = CYPRESS_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = CYPRESS_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_PTP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_PTP_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_LLDP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_LLDP_BYPASS_STPf;
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
}

/* Function Name:
 *      dal_cypress_trap_bypassStp_set
 * Description:
 *      Set enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 *      enable     - enable status of bypassing STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_RMA_0X
 *    - BYPASS_STP_TYPE_SLOW_PROTO
 *    - BYPASS_STP_TYPE_EAPOL
 *    - BYPASS_STP_TYPE_PTP
 *    - BYPASS_STP_TYPE_LLDP
 */
int32
dal_cypress_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable)
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
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = CYPRESS_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = CYPRESS_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_RMA_0X:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_0X_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_SLOW_PROTO:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_SLOW_PROTO_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_EAPOL:
            reg_idx = CYPRESS_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = CYPRESS_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_PTP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_PTP_BYPASS_STPf;
            break;
        case BYPASS_STP_TYPE_LLDP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_LLDP_BYPASS_STPf;
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
}

/* Function Name:
 *      dal_cypress_trap_bypassVlan_get
 * Description:
 *      Get enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 * Output:
 *      pEnable    - pointer to enable status of bypassing STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_VLAN_TYPE_USER_DEF_0
 *    - BYPASS_VLAN_TYPE_USER_DEF_1
 *    - BYPASS_VLAN_TYPE_RMA_00
 *    - BYPASS_VLAN_TYPE_RMA_02
 *    - BYPASS_VLAN_TYPE_RMA_0E
 *    - BYPASS_VLAN_TYPE_RMA_0X
 *    - BYPASS_VLAN_TYPE_EAPOL,
 *    - BYPASS_VLAN_TYPE_PTP
 *    - BYPASS_VLAN_TYPE_LLDP
 */
int32
dal_cypress_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable)
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
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = CYPRESS_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_1:
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = CYPRESS_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_00:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_00_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_02:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_02_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0E:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_0E_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0X:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_0X_BYPASS_VLANf;
            break;
       case BYPASS_VLAN_TYPE_EAPOL:
            reg_idx = CYPRESS_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = CYPRESS_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_PTP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_PTP_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_LLDP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_LLDP_BYPASS_VLANf;
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
            return RT_ERR_INPUT;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_bypassVlan_set
 * Description:
 *      Set enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 *      enable     - enable status of bypassing STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_VLAN_TYPE_USER_DEF_0
 *    - BYPASS_VLAN_TYPE_USER_DEF_1
 *    - BYPASS_VLAN_TYPE_RMA_00
 *    - BYPASS_VLAN_TYPE_RMA_02
 *    - BYPASS_VLAN_TYPE_RMA_0E
 *    - BYPASS_VLAN_TYPE_RMA_0X
 *    - BYPASS_VLAN_TYPE_EAPOL,
 *    - BYPASS_VLAN_TYPE_PTP
 *    - BYPASS_VLAN_TYPE_LLDP
 */
int32
dal_cypress_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable)
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
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r;
            field_idx = CYPRESS_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_USER_DEF_1:
            reg_idx = CYPRESS_RMA_USR_DEF_CTRL_SET1_1r;
            field_idx = CYPRESS_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_00:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_00_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_02:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_02_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0E:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_0E_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_RMA_0X:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_RMA_0X_BYPASS_VLANf;
            break;
       case BYPASS_VLAN_TYPE_EAPOL:
            reg_idx = CYPRESS_SPCL_TRAP_EAPOL_CTRLr;
            field_idx = CYPRESS_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_PTP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_PTP_BYPASS_VLANf;
            break;
        case BYPASS_VLAN_TYPE_LLDP:
            reg_idx = CYPRESS_RMA_CTRL_3r;
            field_idx = CYPRESS_LLDP_BYPASS_VLANf;
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
}

/* Function Name:
 *      dal_cypress_trap_userDefineRma_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_userDefineRma_get(
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
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pUserDefinedRma), RT_ERR_NULL_POINTER);

    baseAddr = CYPRESS_RMA_USR_DEF_CTRL_SET0_0r + userDefine_idx * 2;
    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if((ret = reg_read(unit, baseAddr, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    pUserDefinedRma->mac.octet[2] = (val >> 24) & 0xff;
    pUserDefinedRma->mac.octet[3] = (val >> 16) & 0xff;
    pUserDefinedRma->mac.octet[4] = (val >> 8) & 0xff;
    pUserDefinedRma->mac.octet[5] = val & 0xff;

    if((ret = reg_read(unit, baseAddr + 1, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    pUserDefinedRma->mac.octet[0] = (val >> 8) & 0xff;
    pUserDefinedRma->mac.octet[1] = val & 0xff;
    osal_memset(pUserDefinedRma->macMask.octet, 0xff, ETHER_ADDR_LEN);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac=%x-%x-%x-%x-%x-%x",
           pUserDefinedRma->mac.octet[0], pUserDefinedRma->mac.octet[1], pUserDefinedRma->mac.octet[2],
           pUserDefinedRma->mac.octet[3], pUserDefinedRma->mac.octet[4], pUserDefinedRma->mac.octet[5]);

     RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->macMask=%x-%x-%x-%x-%x-%x",
           pUserDefinedRma->macMask.octet[0], pUserDefinedRma->macMask.octet[1], pUserDefinedRma->macMask.octet[2],
           pUserDefinedRma->macMask.octet[3], pUserDefinedRma->macMask.octet[4], pUserDefinedRma->macMask.octet[5]);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_userDefineRma_get */

/* Function Name:
 *      dal_cypress_trap_userDefineRma_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_userDefineRma_set(
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
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pUserDefinedRma), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac=%x-%x-%x-%x-%x-%x",
       pUserDefinedRma->mac.octet[0], pUserDefinedRma->mac.octet[1], pUserDefinedRma->mac.octet[2],
       pUserDefinedRma->mac.octet[3], pUserDefinedRma->mac.octet[4], pUserDefinedRma->mac.octet[5]);

    baseAddr = CYPRESS_RMA_USR_DEF_CTRL_SET0_0r + userDefine_idx * 2;
    TRAP_SEM_LOCK(unit);

    value = (pUserDefinedRma->mac.octet[2] << 24) | (pUserDefinedRma->mac.octet[3] << 16) | (pUserDefinedRma->mac.octet[4] << 8) | (pUserDefinedRma->mac.octet[5]);
    if((ret = reg_write(unit, baseAddr, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    value = (pUserDefinedRma->mac.octet[0] << 8) | (pUserDefinedRma->mac.octet[1]);
    if((ret = reg_field_write(unit, baseAddr + 1, CYPRESS_ADDR_HIf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_userDefineRma_set */

/* Function Name:
 *      dal_cypress_trap_userDefineRmaEnable_get
 * Description:
 *      Get enable status of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pEnable         - pointer to enable status of RMA entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    baseAddr = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if((ret = reg_field_read(unit, baseAddr,  CYPRESS_ENf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_trap_userDefineRmaEnable_get */

/* Function Name:
 *      dal_cypress_trap_userDefineRmaEnable_set
 * Description:
 *      Set enable status of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      enable          - enable status of RMA entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable)
{
    int32   ret;
    uint32 baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d, enable=%d", unit, userDefine_idx, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (DISABLED == enable)
        value = 0;
    else
        value = 1;

    baseAddr = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    TRAP_SEM_LOCK(unit);

    /* set entry from CHIP*/
    if((ret = reg_field_write(unit, baseAddr,  CYPRESS_ENf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_userDefineRmaEnable_set */

/* Function Name:
 *      dal_cypress_trap_userDefineRmaAction_get
 * Description:
 *      Get forwarding action of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pActoin         - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - MGMT_ACTION_FORWARD
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_DROP
 */
int32
dal_cypress_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction)
{
    int32   ret;
    uint32 baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    baseAddr = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if((ret = reg_field_read(unit, baseAddr,  CYPRESS_ACTf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_trap_userDefineRmaAction_get */

/* Function Name:
 *      dal_cypress_trap_userDefineRmaAction_set
 * Description:
 *      Set forwarding action of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      actoin          - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_cypress_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action)
{
    int32   ret;
    uint32 baseAddr, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d, action=%d", unit, userDefine_idx, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);

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

    baseAddr = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    TRAP_SEM_LOCK(unit);

    /* set entry from CHIP*/
    if((ret = reg_field_write(unit, baseAddr,  CYPRESS_ACTf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_userDefineRmaAction_set */

/* Function Name:
 *      dal_cypress_trap_userDefineRmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for user-defined RMA frame.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pEnable         - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 baseAddr;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);


    baseAddr = CYPRESS_RMA_USR_DEF_CTRL_SET0_1r + userDefine_idx * 2;
    TRAP_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, baseAddr, CYPRESS_LRNf, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_userDefineRmaLearningEnable_get */

/* Function Name:
 *      dal_cypress_trap_userDefineRmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this user-defined RMA frame.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      enable          - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable)
{
    int32   ret;
    uint32  regIdx[] = {CYPRESS_RMA_USR_DEF_CTRL_SET0_1r,
                        CYPRESS_RMA_USR_DEF_CTRL_SET1_1r};

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, regIdx[userDefine_idx], CYPRESS_LRNf,
            &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_userDefineRmaLearningEnable_set */

/* Function Name:
 *      dal_cypress_trap_mgmtFrameAction_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
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
 *                                    	      8390
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 */
int32
dal_cypress_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
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
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_IGMP_CTRLr, CYPRESS_MLD_ACTf, &value)) != RT_ERR_OK)
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
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_IGMP_CTRLr, CYPRESS_IGMP_ACTf, &value)) != RT_ERR_OK)
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
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_EAPOL_CTRLr, CYPRESS_ACTf, &value)) != RT_ERR_OK)
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
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_ARP_CTRLr, CYPRESS_REQ_ACTf, &value)) != RT_ERR_OK)
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
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_IPV6_CTRLr, CYPRESS_ND_ACTf, &value)) != RT_ERR_OK)
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
        case MGMT_TYPE_SELFMAC:
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_SWITCH_MAC_CTRLr, CYPRESS_ACTf, &value)) != RT_ERR_OK)
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
        case MGMT_TYPE_IPV6_HOP_POS_ERR:
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_IPV6_CTRLr, CYPRESS_HBHEXTHDRERR_ACTf, &value)) != RT_ERR_OK)
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
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_IPV6_CTRLr, CYPRESS_UNKNOWNEXTHDR_ACTf, &value)) != RT_ERR_OK)
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
        case MGMT_TYPE_L2_CRC_ERR:
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_CRC_CTRLr, CYPRESS_L2_CRC_ERR_ACTf, &value)) != RT_ERR_OK)
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
        case MGMT_TYPE_IP4_CHKSUM_ERR:
            if((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_CRC_CTRLr, CYPRESS_IP4_CRC_ERR_ACTf, &value)) != RT_ERR_OK)
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
}   /*end of dal_cypress_trap_mgmtFrameAction_get*/

/* Function Name:
 *      dal_cypress_trap_mgmtFrameAction_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
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
 *                                    	      8390
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 */
int32
dal_cypress_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
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
    RT_PARAM_CHK(((frameType == MGMT_TYPE_SELFMAC) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_IPV6_HOP_POS_ERR) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_IPV6_HDR_UNKWN) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_L2_CRC_ERR) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);
    RT_PARAM_CHK(((frameType == MGMT_TYPE_IP4_CHKSUM_ERR) && ((action != MGMT_ACTION_FORWARD) && (action != MGMT_ACTION_DROP) && (action != MGMT_ACTION_TRAP2CPU))), RT_ERR_FWD_ACTION);

    TRAP_SEM_LOCK(unit);

    if (action == MGMT_ACTION_FORWARD)
        value = 0;
    else
        value = 1;

    switch (frameType)
    {
        case MGMT_TYPE_MLD:
            if ((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_IGMP_CTRLr, CYPRESS_MLD_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IGMP:
            if ((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_IGMP_CTRLr, CYPRESS_IGMP_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_EAPOL:
            if ((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_EAPOL_CTRLr, CYPRESS_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_ARP:
            if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_ARP_CTRLr, CYPRESS_REQ_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IPV6ND:
            if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_IPV6_CTRLr, CYPRESS_ND_ACTf, &value)) != RT_ERR_OK)
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
            if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_SWITCH_MAC_CTRLr, CYPRESS_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IPV6_HOP_POS_ERR:
            if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_IPV6_CTRLr, CYPRESS_HBHEXTHDRERR_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IPV6_HDR_UNKWN:
            if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_IPV6_CTRLr, CYPRESS_UNKNOWNEXTHDR_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_L2_CRC_ERR:
            if (action == MGMT_ACTION_FORWARD)
                value = 0;
            else if (action == MGMT_ACTION_DROP)
                value = 1;
            else
                value = 2;
            if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_CRC_CTRLr, CYPRESS_L2_CRC_ERR_ACTf, &value)) != RT_ERR_OK)
            {
                TRAP_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
                return ret;
            }
            break;
        case MGMT_TYPE_IP4_CHKSUM_ERR:
            if (action == MGMT_ACTION_FORWARD)
                value = 0;
            else if (action == MGMT_ACTION_DROP)
                value = 1;
            else
                value = 2;
            if((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_CRC_CTRLr, CYPRESS_IP4_CRC_ERR_ACTf, &value)) != RT_ERR_OK)
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
}   /*end of dal_cypress_trap_mgmtFrameAction_set*/

/* Function Name:
 *      dal_cypress_trap_mgmtFramePri_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_OTHER
 *      - MGMT_TYPE_OAM
 *      - MGMT_TYPE_CFM
 *      - MGMT_TYPE_IGR_VLAN_FLTR
 *      - MGMT_TYPE_VLAN_ERR
 *      - MGMT_TYPE_CFI
 *      - MGMT_TYPE_RMA_USR_DEF_1
 *      - MGMT_TYPE_RMA_USR_DEF_2
 *      - MGMT_TYPE_BPDU
 *      - MGMT_TYPE_LACP
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_RMA
 *      - MGMT_TYPE_IGMP
 *      - MGMT_TYPE_MLD
 *      - MGMT_TYPE_EAPOL
 *      - MGMT_TYPE_ARP
 *      - MGMT_TYPE_IPV6ND
 *      - MGMT_TYPE_UNKNOWN_DA
 *      - MGMT_TYPE_SELFMAC
 *      - MGMT_TYPE_INVALID_SA
 */
int32
dal_cypress_trap_mgmtFramePri_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  reg, field, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    switch (frameType)
    {
    case MGMT_TYPE_OTHER:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_OTHERf;
        break;

    case MGMT_TYPE_OAM:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_OAMf;
        break;

    case MGMT_TYPE_CFM:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_CFMf;
        break;

    case MGMT_TYPE_IGR_VLAN_FLTR:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_IGR_VLAN_FLTRf;
        break;

    case MGMT_TYPE_VLAN_ERR:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_VLAN_ERRf;
        break;

    case MGMT_TYPE_CFI:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_CFIf;
        break;

    case MGMT_TYPE_RMA_USR_DEF_1:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_USR_DEF_1f;
        break;

    case MGMT_TYPE_RMA_USR_DEF_2:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_USR_DEF_2f;
        break;

    case MGMT_TYPE_BPDU:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_BPDUf;
        break;

    case MGMT_TYPE_LACP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_LACPf;
        break;

    case MGMT_TYPE_PTP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_PTPf;
        break;

    case MGMT_TYPE_LLDP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_LLDPf;
        break;

    case MGMT_TYPE_RMA:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_RMAf;
        break;

    case MGMT_TYPE_IPV6_HOP_POS_ERR:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_IPV6_HBH_POS_ERRf;
        break;

    case MGMT_TYPE_IPV6_HDR_UNKWN:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_IPV6_UNKN_EXTHDRf;
        break;

    case MGMT_TYPE_IGMP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_IGMPf;
        break;

    case MGMT_TYPE_MLD:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_MLDf;
        break;

    case MGMT_TYPE_EAPOL:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_EAPOLf;
        break;

    case MGMT_TYPE_ARP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_ARPf;
        break;

    case MGMT_TYPE_IPV6ND:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_IPV6_NDf;
        break;

    case MGMT_TYPE_UNKNOWN_DA:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_UNKNOWN_DAf;
        break;

    case MGMT_TYPE_SELFMAC:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_CPU_MACf;
        break;

    case MGMT_TYPE_INVALID_SA:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_INVLD_SAf;
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
} /* end of dal_cypress_trap_mgmtFramePri_get */

/* Function Name:
 *      dal_cypress_trap_mgmtFramePri_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_OTHER
 *      - MGMT_TYPE_OAM
 *      - MGMT_TYPE_CFM
 *      - MGMT_TYPE_IGR_VLAN_FLTR
 *      - MGMT_TYPE_VLAN_ERR
 *      - MGMT_TYPE_CFI
 *      - MGMT_TYPE_RMA_USR_DEF_1
 *      - MGMT_TYPE_RMA_USR_DEF_2
 *      - MGMT_TYPE_BPDU
 *      - MGMT_TYPE_LACP
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_RMA
 *      - MGMT_TYPE_IGMP
 *      - MGMT_TYPE_MLD
 *      - MGMT_TYPE_EAPOL
 *      - MGMT_TYPE_ARP
 *      - MGMT_TYPE_IPV6ND
 *      - MGMT_TYPE_UNKNOWN_DA
 *      - MGMT_TYPE_SELFMAC
 *      - MGMT_TYPE_INVALID_SA
 */
int32
dal_cypress_trap_mgmtFramePri_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t priority)
{
    int32   ret;
    uint32  reg, field, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, priority=%d", unit, frameType, priority);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((priority > RTK_DOT1P_PRIORITY_MAX), RT_ERR_PRIORITY);

    switch (frameType)
    {
    case MGMT_TYPE_OTHER:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_OTHERf;
        break;

    case MGMT_TYPE_OAM:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_OAMf;
        break;

    case MGMT_TYPE_CFM:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_CFMf;
        break;

    case MGMT_TYPE_IGR_VLAN_FLTR:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_IGR_VLAN_FLTRf;
        break;

    case MGMT_TYPE_VLAN_ERR:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_VLAN_ERRf;
        break;

    case MGMT_TYPE_CFI:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_CFIf;
        break;

    case MGMT_TYPE_RMA_USR_DEF_1:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_USR_DEF_1f;
        break;

    case MGMT_TYPE_RMA_USR_DEF_2:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_USR_DEF_2f;
        break;

    case MGMT_TYPE_BPDU:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_BPDUf;
        break;

    case MGMT_TYPE_LACP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_0r;
        field = CYPRESS_LACPf;
        break;

    case MGMT_TYPE_PTP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_PTPf;
        break;

    case MGMT_TYPE_LLDP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_LLDPf;
        break;

    case MGMT_TYPE_RMA:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_RMAf;
        break;

    case MGMT_TYPE_IPV6_HOP_POS_ERR:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_IPV6_HBH_POS_ERRf;
        break;

    case MGMT_TYPE_IPV6_HDR_UNKWN:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_1r;
        field = CYPRESS_IPV6_UNKN_EXTHDRf;
        break;

    case MGMT_TYPE_IGMP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_IGMPf;
        break;

    case MGMT_TYPE_MLD:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_MLDf;
        break;

    case MGMT_TYPE_EAPOL:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_EAPOLf;
        break;

    case MGMT_TYPE_ARP:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_ARPf;
        break;

    case MGMT_TYPE_IPV6ND:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_IPV6_NDf;
        break;

    case MGMT_TYPE_UNKNOWN_DA:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_UNKNOWN_DAf;
        break;

    case MGMT_TYPE_SELFMAC:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_CPU_MACf;
        break;

    case MGMT_TYPE_INVALID_SA:
        reg   = CYPRESS_QM_PKT2CPU_INTPRI_2r;
        field = CYPRESS_INVLD_SAf;
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
} /* end of dal_cypress_trap_mgmtFramePri_set */

/* Function Name:
 *      dal_cypress_trap_mgmtFrameLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for the management frame.
 * Input:
 *      unit        - unit id
 *      frameType   - type of management frame
 * Output:
 *      pEnable     - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 */
int32
dal_cypress_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable)
{
    int32   ret = 0;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType != MGMT_TYPE_PTP) && (frameType != MGMT_TYPE_LLDP), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);


    regField = (frameType == MGMT_TYPE_PTP) ? CYPRESS_PTP_LRNf : CYPRESS_LLDP_LRNf;
    TRAP_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_RMA_MGN_LRN_CTRLr, regField, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_mgmtFrameLearningEnable_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 */
int32
dal_cypress_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable)
{
    int32   ret = 0;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType != MGMT_TYPE_PTP) && (frameType != MGMT_TYPE_LLDP), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);


    regField = (frameType == MGMT_TYPE_PTP) ? CYPRESS_PTP_LRNf : CYPRESS_LLDP_LRNf;
    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_RMA_MGN_LRN_CTRLr, regField, &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_portMgmtFrameAction_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
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
 *                                    	      8390
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
int32
dal_cypress_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, port=%d, frameType=%d",
                unit, port, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(frameType >= MGMT_TYPE_END, RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /*Get entry From CHIP*/
    if(frameType == MGMT_TYPE_BPDU)
    {
        if((ret = reg_array_field_read(unit, CYPRESS_RMA_PORT_BPDU_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP)
    {
        if((ret = reg_array_field_read(unit, CYPRESS_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_LLDP)
    {
        if((ret = reg_array_field_read(unit, CYPRESS_RMA_PORT_LLDP_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else
    {
        TRAP_SEM_UNLOCK(unit);
        return RT_ERR_RMA_MGMT_TYPE;
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
 *      dal_cypress_trap_portMgmtFrameAction_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
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
 *                                    	      8390
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
int32
dal_cypress_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
{
    int32   ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, port=%d, frameType=%d, action=%d",
                unit, port, frameType, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(frameType >= MGMT_TYPE_END, RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK(action >= MGMT_ACTION_END, RT_ERR_FWD_ACTION);
    RT_PARAM_CHK((frameType == MGMT_TYPE_PTP) && (action == MGMT_ACTION_FLOOD_TO_ALL_PORT), RT_ERR_RMA_ACTION);

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
            return RT_ERR_FAILED;
    }

    TRAP_SEM_LOCK(unit);

    if(frameType == MGMT_TYPE_BPDU)
    {
        if((ret = reg_array_field_write(unit, CYPRESS_RMA_PORT_BPDU_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP)
    {
        if((ret = reg_array_field_write(unit, CYPRESS_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_LLDP)
    {
        if((ret = reg_array_field_write(unit, CYPRESS_RMA_PORT_LLDP_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else
    {
        TRAP_SEM_UNLOCK(unit);
        return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_oamPDUAction_get
 * Description:
 *      Get forwarding action of trapped oam PDU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to forwarding action of trapped oam PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_cypress_trap_oamPDUAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_OAM_CTRLr, CYPRESS_ENf,
            &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    /* enable */
    if (1 == val)
    {
        TRAP_SEM_UNLOCK(unit);
        *pAction = ACTION_TRAP2CPU;
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);
        return RT_ERR_OK;
    }

    if ((ret = reg_field_read(unit, CYPRESS_OAM_CTRLr, CYPRESS_DIS_ACTf,
                    &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == val)
        *pAction = ACTION_DROP;
    else if(1 == val)
        *pAction = ACTION_FORWARD;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
}   /*end of dal_cypress_trap_oamPDUAction_get*/

/* Function Name:
 *      dal_cypress_trap_oamPDUAction_set
 * Description:
 *      Set forwarding action of trapped oam PDU.
 * Input:
 *      unit   - unit id
 *      action - forwarding action of trapped oam PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
int32
dal_cypress_trap_oamPDUAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  en_val;
    uint32  dis_val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, action=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    if (ACTION_TRAP2CPU == action)
    {
        en_val = 1;
    }
    else
    {
        en_val = 0;
        if (ACTION_FORWARD == action)
        {
            dis_val = 1;
        }
        else if (ACTION_DROP == action)
        {
            dis_val = 0;
        }
        else
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
            return RT_ERR_INPUT;
        }
    }

    TRAP_SEM_LOCK(unit);

    /*Set entry to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_OAM_CTRLr, CYPRESS_ENf, &en_val))
            != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if (0 == en_val)
    {
        if ((ret = reg_field_write(unit, CYPRESS_OAM_CTRLr, CYPRESS_DIS_ACTf,
                    &dis_val) ) != RT_ERR_OK)
        {
            /* roll back */
            en_val = 1;
            reg_field_write(unit, CYPRESS_OAM_CTRLr, CYPRESS_ENf, &en_val);

            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /*end of dal_cypress_trap_oamPDUAction_set*/

/* Function Name:
 *      dal_cypress_trap_oamPDUPri_get
 * Description:
 *      Get priority of trapped OAM PDU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_oamPDUPri_get(uint32 unit, rtk_pri_t *pPriority)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_QM_PKT2CPU_INTPRI_0r, CYPRESS_OAMf,
                    pPriority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pPriority=%d", *pPriority);

    return RT_ERR_OK;
}   /*end of dal_cypress_trap_oamPDUPri_get*/

/* Function Name:
 *      dal_cypress_trap_oamPDUPri_set
 * Description:
 *      Set priority of trapped OAM PDU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
int32
dal_cypress_trap_oamPDUPri_set(uint32 unit, rtk_pri_t priority)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, priority=%d",
                unit, priority);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((priority > RTK_DOT1P_PRIORITY_MAX), RT_ERR_PRIORITY);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_QM_PKT2CPU_INTPRI_0r, CYPRESS_OAMf,
                    &priority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /*end of dal_cypress_trap_oamPDUPri_set*/

/* Function Name:
 *      dal_cypress_trap_cfmUnknownFrameAct_get
 * Description:
 *      Get action for receive unknown type of CFM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      action  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      unknown CFM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
int32
dal_cypress_trap_cfmUnknownFrameAct_get(uint32 unit, rtk_action_t *action)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == action), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get action */
    if ((ret = reg_field_read(unit, CYPRESS_CFM_UNKN_RX_CTRLr,
                    CYPRESS_UNKN_RXf, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == val)
        *action = ACTION_DROP;
    else if (1 == val)
        *action = ACTION_TRAP2CPU;
    else if (2 == val)
        *action = ACTION_FORWARD;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmUnknownFrameAct_get */

/* Function Name:
 *      dal_cypress_trap_cfmUnknownFrameAct_set
 * Description:
 *      Set action for receive unknown type of CFM frame.
 * Input:
 *      unit    - unit id
 *      action  - receive unknown type of CFM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      unknown CFM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
int32
dal_cypress_trap_cfmUnknownFrameAct_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, action=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    if (ACTION_DROP == action)
        val = 0;
    else if (ACTION_TRAP2CPU == action)
        val = 1;
    else if (ACTION_FORWARD == action)
        val = 2;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* set action */
    if ((ret = reg_field_write(unit, CYPRESS_CFM_UNKN_RX_CTRLr,
                    CYPRESS_UNKN_RXf, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmUnknownFrameAct_set */

/* Function Name:
 *      dal_cypress_trap_cfmLoopbackAct_get
 * Description:
 *      Get action for receive CFM loopback frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 * Output:
 *      action  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      loopback action combine with linktrace in RTL8390
 *      loopback frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
int32
dal_cypress_trap_cfmLoopbackAct_get(uint32 unit, uint32 level,
                                    rtk_action_t *action)
{
    int32   ret;
    uint32  val;
    uint32  fieldName;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d level:%d", unit, level);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == action), RT_ERR_NULL_POINTER);

    /* get level field */
    switch (level)
    {
        case 0:
            fieldName = CYPRESS_MD_LV_0f;
            break;
        case 1:
            fieldName = CYPRESS_MD_LV_1f;
            break;
        case 2:
            fieldName = CYPRESS_MD_LV_2f;
            break;
        case 3:
            fieldName = CYPRESS_MD_LV_3f;
            break;
        case 4:
            fieldName = CYPRESS_MD_LV_4f;
            break;
        case 5:
            fieldName = CYPRESS_MD_LV_5f;
            break;
        case 6:
            fieldName = CYPRESS_MD_LV_6f;
            break;
        case 7:
            fieldName = CYPRESS_MD_LV_7f;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* get action */
    if ((ret = reg_field_read(unit, CYPRESS_CFM_LBLT_RX_CTRLr,
                    fieldName, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == val)
        *action = ACTION_DROP;
    else if (1 == val)
        *action = ACTION_TRAP2CPU;
    else if (2 == val)
        *action = ACTION_FORWARD;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmLoopbackAct_get */

/* Function Name:
 *      dal_cypress_trap_cfmLoopbackAct_set
 * Description:
 *      Set action for receive CFM loopback frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 *      action  - receive CFM loopback frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      loopback action combine with linktrace in RTL8390
 *      loopback frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
int32
dal_cypress_trap_cfmLoopbackAct_set(uint32 unit, uint32 level,
                                    rtk_action_t action)
{
    int32   ret;
    uint32  val;
    uint32  fieldName;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d level=%d action=%d",
           unit, level, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    /* get level field */
    switch (level)
    {
        case 0:
            fieldName = CYPRESS_MD_LV_0f;
            break;
        case 1:
            fieldName = CYPRESS_MD_LV_1f;
            break;
        case 2:
            fieldName = CYPRESS_MD_LV_2f;
            break;
        case 3:
            fieldName = CYPRESS_MD_LV_3f;
            break;
        case 4:
            fieldName = CYPRESS_MD_LV_4f;
            break;
        case 5:
            fieldName = CYPRESS_MD_LV_5f;
            break;
        case 6:
            fieldName = CYPRESS_MD_LV_6f;
            break;
        case 7:
            fieldName = CYPRESS_MD_LV_7f;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if (ACTION_DROP == action)
        val = 0;
    else if (ACTION_TRAP2CPU == action)
        val = 1;
    else if (ACTION_FORWARD == action)
        val = 2;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* set action */
    if ((ret = reg_field_write(unit, CYPRESS_CFM_LBLT_RX_CTRLr,
                    fieldName, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmLoopbackAct_set */

/* Function Name:
 *      dal_cypress_trap_cfmCcmAct_get
 * Description:
 *      Get action for receive CFM CCM frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 * Output:
 *      action  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      CCM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 *         chip value 3 - ACTION_LINK_FAULT_DETECT
 */
int32
dal_cypress_trap_cfmCcmAct_get(uint32 unit, uint32 level,
                               rtk_trap_oam_action_t *action)
{
    int32   ret;
    uint32  val;
    uint32  fieldName;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d level:%d", unit, level);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == action), RT_ERR_NULL_POINTER);

    /* get level field */
    switch (level)
    {
        case 0:
            fieldName = CYPRESS_MD_LV_0f;
            break;
        case 1:
            fieldName = CYPRESS_MD_LV_1f;
            break;
        case 2:
            fieldName = CYPRESS_MD_LV_2f;
            break;
        case 3:
            fieldName = CYPRESS_MD_LV_3f;
            break;
        case 4:
            fieldName = CYPRESS_MD_LV_4f;
            break;
        case 5:
            fieldName = CYPRESS_MD_LV_5f;
            break;
        case 6:
            fieldName = CYPRESS_MD_LV_6f;
            break;
        case 7:
            fieldName = CYPRESS_MD_LV_7f;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* get action */
    if ((ret = reg_field_read(unit, CYPRESS_CFM_CCM_RX_CTRLr,
                    fieldName, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == val)
        *action = TRAP_OAM_ACTION_DROP;
    else if (1 == val)
        *action = TRAP_OAM_ACTION_TRAP2CPU;
    else if (2 == val)
        *action = TRAP_OAM_ACTION_FORWARD;
    else if (3 == val)
        *action = TRAP_OAM_ACTION_LINK_FAULT_DETECT;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmCcmAct_get */

/* Function Name:
 *      dal_cypress_trap_cfmCcmAct_set
 * Description:
 *      Set action for receive CFM CCM frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 *      action  - receive CFM CCM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      CCM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 *         chip value 3 - ACTION_LINK_FAULT_DETECT
 */
int32
dal_cypress_trap_cfmCcmAct_set(uint32 unit, uint32 level,
                               rtk_trap_oam_action_t action)
{
    int32   ret;
    uint32  val;
    uint32  fieldName;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d level=%d action=%d",
           unit, level, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((action >= TRAP_OAM_ACTION_END), RT_ERR_INPUT);

    /* get level field */
    switch (level)
    {
        case 0:
            fieldName = CYPRESS_MD_LV_0f;
            break;
        case 1:
            fieldName = CYPRESS_MD_LV_1f;
            break;
        case 2:
            fieldName = CYPRESS_MD_LV_2f;
            break;
        case 3:
            fieldName = CYPRESS_MD_LV_3f;
            break;
        case 4:
            fieldName = CYPRESS_MD_LV_4f;
            break;
        case 5:
            fieldName = CYPRESS_MD_LV_5f;
            break;
        case 6:
            fieldName = CYPRESS_MD_LV_6f;
            break;
        case 7:
            fieldName = CYPRESS_MD_LV_7f;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if (TRAP_OAM_ACTION_DROP == action)
        val = 0;
    else if (TRAP_OAM_ACTION_TRAP2CPU == action)
        val = 1;
    else if (TRAP_OAM_ACTION_FORWARD == action)
        val = 2;
    else if (TRAP_OAM_ACTION_LINK_FAULT_DETECT == action)
        val = 3;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* set action */
    if ((ret = reg_field_write(unit, CYPRESS_CFM_CCM_RX_CTRLr,
                    fieldName, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmCcmAct_set */

/* Function Name:
 *      dal_cypress_trap_cfmEthDmAct_get
 * Description:
 *      Get action for receive CFM ETH-DM frame in specified MD level.
 * Input:
 *      unit    - unit id
 *      mdl     - MD level
 * Output:
 *      pAction - pointer to action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      ETH-DM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
int32
dal_cypress_trap_cfmEthDmAct_get(uint32 unit, uint32 mdl, rtk_action_t *pAction)
{
    int32   ret;
    uint32  val;
    uint32  fieldName[] = { \
        CYPRESS_MD_LV_0f, CYPRESS_MD_LV_1f, CYPRESS_MD_LV_2f, CYPRESS_MD_LV_3f, \
        CYPRESS_MD_LV_4f, CYPRESS_MD_LV_5f, CYPRESS_MD_LV_6f, CYPRESS_MD_LV_7f };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d level:%d", unit, mdl);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((mdl >= (sizeof(fieldName)/sizeof(uint32))), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get action */
    if ((ret = reg_field_read(unit, CYPRESS_ETH_DM_RX_CTRLr, fieldName[mdl], &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0x0 == val)
        *pAction = ACTION_DROP;
    else if (0x1 == val)
        *pAction = ACTION_TRAP2CPU;
    else if (0x2 == val)
        *pAction = ACTION_FORWARD;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_trap_cfmEthDmAct_get */

/* Function Name:
 *      dal_cypress_trap_cfmEthDmAct_set
 * Description:
 *      Set action for receive CFM ETH-DM frame in specified MD level.
 * Input:
 *      unit    - unit id
 *      mdl     - MD level
 *      action  - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      ETH-DM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
int32
dal_cypress_trap_cfmEthDmAct_set(uint32 unit, uint32 mdl, rtk_action_t action)
{
    int32   ret;
    uint32  val;
    uint32  fieldName[] = { \
        CYPRESS_MD_LV_0f, CYPRESS_MD_LV_1f, CYPRESS_MD_LV_2f, CYPRESS_MD_LV_3f, \
        CYPRESS_MD_LV_4f, CYPRESS_MD_LV_5f, CYPRESS_MD_LV_6f, CYPRESS_MD_LV_7f };

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d level=%d action=%d", unit, mdl, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((mdl >= (sizeof(fieldName)/sizeof(uint32))), RT_ERR_INPUT);
    RT_PARAM_CHK((action >= ACTION_END), RT_ERR_INPUT);

    if (ACTION_DROP == action)
        val = 0x0;
    else if (ACTION_TRAP2CPU == action)
        val = 0x1;
    else if (ACTION_FORWARD == action)
        val = 0x2;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* set action */
    if ((ret = reg_field_write(unit, CYPRESS_ETH_DM_RX_CTRLr, fieldName[mdl], &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_cfmEthDmAct_set */

/* Function Name:
 *      dal_cypress_trap_cfmFrameTrapPri_get
 * Description:
 *      Get priority of CFM packets trapped to CPU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_cfmFrameTrapPri_get(uint32 unit, rtk_pri_t *pPriority)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_QM_PKT2CPU_INTPRI_0r, CYPRESS_CFMf,
                    pPriority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pPriority=%d", *pPriority);

    return RT_ERR_OK;
}   /*end of dal_cypress_trap_cfmFrameTrapPri_get*/

/* Function Name:
 *      dal_cypress_trap_cfmFrameTrapPri_set
 * Description:
 *      Set priority of CFM packets trapped to CPU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
int32
dal_cypress_trap_cfmFrameTrapPri_set(uint32 unit, rtk_pri_t priority)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, priority=%d", unit, priority);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((priority > RTK_DOT1P_PRIORITY_MAX), RT_ERR_PRIORITY);

    TRAP_SEM_LOCK(unit);

    /* set entry to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_QM_PKT2CPU_INTPRI_0r, CYPRESS_CFMf,
                    &priority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /*end of dal_cypress_trap_cfmFrameTrapPri_set*/

/* Function Name:
 *      dal_cypress_trap_portOamLoopbackParAction_get
 * Description:
 *      Get action of parser on specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to parser action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Parser action is as following:
 *         chip value 0 - TRAP_OAM_ACTION_DROP
 *         chip value 1 - TRAP_OAM_ACTION_FORWARD
 *         chip value 2 - TRAP_OAM_ACTION_LOOPBACK
 *         chip value 3 - TRAP_OAM_ACTION_TRAP2CPU
 */
int32
dal_cypress_trap_portOamLoopbackParAction_get(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t *pAction)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, CYPRESS_OAM_PORT_ACT_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_PAR_ACTf,
                    pAction) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if (0 == *pAction)
        *pAction = TRAP_OAM_ACTION_DROP;
    else if(1 == *pAction)
        *pAction = TRAP_OAM_ACTION_FORWARD;
    else if(2 == *pAction)
        *pAction = TRAP_OAM_ACTION_LOOPBACK;
    else if(3 == *pAction)
        *pAction = TRAP_OAM_ACTION_TRAP2CPU;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_portOamLoopbackParAction_get */

/* Function Name:
 *      dal_cypress_trap_portOamLoopbackParAction_set
 * Description:
 *      Set action of parser on specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      action  - parser action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Parser action is as following:
 *         chip value 0 - TRAP_OAM_ACTION_DROP
 *         chip value 1 - TRAP_OAM_ACTION_FORWARD
 *         chip value 2 - TRAP_OAM_ACTION_LOOPBACK
 *         chip value 3 - TRAP_OAM_ACTION_TRAP2CPU
 */
int32
dal_cypress_trap_portOamLoopbackParAction_set(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t action)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, port=%d, action=%d",
            unit, port, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((action >= TRAP_OAM_ACTION_END), RT_ERR_INPUT);

    if (TRAP_OAM_ACTION_DROP == action)
        val = 0;
    else if (TRAP_OAM_ACTION_FORWARD == action)
        val = 1;
    else if (TRAP_OAM_ACTION_LOOPBACK == action)
        val = 2;
    else if (TRAP_OAM_ACTION_TRAP2CPU == action)
        val = 3;
    else
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "");
        return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* set forward action */
    if ((ret = reg_array_field_write(unit, CYPRESS_OAM_PORT_ACT_CTRLr,
                    port, REG_ARRAY_INDEX_NONE, CYPRESS_PAR_ACTf, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_portOamLoopbackParAction_set */

/* Function Name:
 *      dal_cypress_trap_pktWithCFIAction_get
 * Description:
 *      Get the configuration of inner CFI operation.
 * Input:
 *      unit                - unit id
 * Output:
 *      rtk_action_t    - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_cypress_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pAction, RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, CYPRESS_VLAN_CTRLr, CYPRESS_ICFI_ACTf,
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
} /* end of dal_cypress_trap_pktWithCFIAction_get */

/* Function Name:
 *      dal_cypress_trap_pktWithCFIAction_set
 * Description:
 *      Set the configuration of inner CFI operation.
 * Input:
 *      unit - unit id
 *      action   - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_cypress_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "unit=%d, action=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

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
    if ((ret = reg_field_write(unit, CYPRESS_VLAN_CTRLr, CYPRESS_ICFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_pktWithCFIAction_set */

/* Function Name:
 *      dal_cypress_trap_pktWithOuterCFIAction_get
 * Description:
 *      Get the configuration of outer CFI operation.
 * Input:
 *      unit                - unit id
 * Output:
 *      rtk_action_t    - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_cypress_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pAction, RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, CYPRESS_VLAN_CTRLr, CYPRESS_OCFI_ACTf,
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
} /* end of dal_cypress_trap_pktWithOuterCFIAction_get */

/* Function Name:
 *      dal_cypress_trap_pktWithOuterCFIAction_set
 * Description:
 *      Set the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 *      action  - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_cypress_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "unit=%d, action=%d", unit, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

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
    if ((ret = reg_field_write(unit, CYPRESS_VLAN_CTRLr, CYPRESS_OCFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_pktWithOuterCFIAction_set */

/* Function Name:
 *      dal_cypress_trap_pktWithCFIPri_get
 * Description:
 *      Get priority of packets trapped to CPU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_pktWithCFIPri_get(uint32 unit, rtk_pri_t *pPriority)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /*Get entry From CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_QM_PKT2CPU_INTPRI_0r, CYPRESS_CFIf,
                    pPriority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pPriority=%d", *pPriority);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_pktWithCFIPri_get */

/* Function Name:
 *      dal_cypress_trap_pktWithCFIPri_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
int32
dal_cypress_trap_pktWithCFIPri_set(uint32 unit, rtk_pri_t priority)
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
    if ((ret = reg_field_write(unit, CYPRESS_QM_PKT2CPU_INTPRI_0r, CYPRESS_CFIf,
                    &priority) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_trap_pktWithCFIPri_set */

/* Function Name:
 *      _dal_trap_routeExceptionActionRegField_get
 * Description:
 *      Get register and field for configure action of route exception packets.
 * Input:
 *      unit    - unit id
 * Output:
 *      reg     - register for specfic route exception action.
 *      field   - register field for specfic route exception action.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
static int32
_dal_trap_routeExceptionActionRegField_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, uint32 *reg, uint32 *field)
{
    RT_PARAM_CHK((NULL == reg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == field), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_IP6_HL_EXCEEDf;
            break;
        case ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_IP6_HDR_ERRf;
            break;
        case ROUTE_EXCEPTION_TYPE_WITH_OPT:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_IP4_OPTf;
            break;
        case ROUTE_EXCEPTION_TYPE_TTL_EXCEED:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_IP4_TTL_EXECEEDf;
            break;
        case ROUTE_EXCEPTION_TYPE_HDR_ERR:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_IP4_HDR_ERRf;
            break;
        case ROUTE_EXCEPTION_TYPE_GW_MAC_ERR:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_GW_MAC_ERRf;
            break;
        case ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_IP6_HOPBYHOPf;
            break;
        case ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT:
            *reg     = CYPRESS_ROUTING_EXCPT_CTRLr;
            *field   = CYPRESS_ROUTING_AGEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}   /* end of _dal_trap_routeExceptionActionRegField_get */

/* Function Name:
 *      dal_cypress_trap_routeExceptionAction_get
 * Description:
 *      Get the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 *      type    - configure action for which route exception
 * Output:
 *      rtk_action_t    - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_cypress_trap_routeExceptionAction_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_action_t *pAction)
{
    int32   ret;
    uint32  reg;
    uint32  field;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ROUTE_EXCEPTION_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    ret = _dal_trap_routeExceptionActionRegField_get(unit, type, &reg, &field);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_LOCK(unit);

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
}    /* end of dal_cypress_trap_routeExceptionAction_get */

/* Function Name:
 *      dal_cypress_trap_routeExceptionAction_set
 * Description:
 *      Set the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 *      type    - configure action for which route exception
 *      action  - route exception operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
int32
dal_cypress_trap_routeExceptionAction_set(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_action_t action)
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
    RT_PARAM_CHK((ROUTE_EXCEPTION_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((ACTION_END <= action), RT_ERR_INPUT);

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
}    /* end of dal_cypress_trap_routeExceptionAction_set */

/* Function Name:
 *      _dal_trap_routeExceptionPriRegField_get
 * Description:
 *      Get register and field for configure priority of route exception packets
 *      trapped to CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      reg     - register for specfic route exception trap priority.
 *      field   - register field for specfic route exception trap priority.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
static int32
_dal_trap_routeExceptionPriRegField_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, uint32 *reg, uint32 *field)
{
    RT_PARAM_CHK((NULL == reg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == field), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case ROUTE_EXCEPTION_TYPE_IP6_HL_EXCEED:
            *reg     = CYPRESS_QM_PKT2CPU_INTPRI_1r;
            *field   = CYPRESS_IPV6_HL_EXCEEDf;
            break;
        case ROUTE_EXCEPTION_TYPE_IP6_HDR_ERR:
            *reg     = CYPRESS_QM_PKT2CPU_INTPRI_1r;
            *field   = CYPRESS_IPV6_HDR_ERRf;
            break;
        case ROUTE_EXCEPTION_TYPE_WITH_OPT:
            *reg     = CYPRESS_QM_PKT2CPU_INTPRI_1r;
            *field   = CYPRESS_IPV4_OPTf;
            break;
        case ROUTE_EXCEPTION_TYPE_TTL_EXCEED:
            *reg     = CYPRESS_QM_PKT2CPU_INTPRI_1r;
            *field   = CYPRESS_IPV4_TTL_EXCEEDf;
            break;
        case ROUTE_EXCEPTION_TYPE_HDR_ERR:
            *reg     = CYPRESS_QM_PKT2CPU_INTPRI_1r;
            *field   = CYPRESS_IPV4_HDR_ERRf;
            break;
        case ROUTE_EXCEPTION_TYPE_GW_MAC_ERR:
        case ROUTE_EXCEPTION_TYPE_ENTRY_AGE_OUT:
            *reg     = CYPRESS_QM_PKT2CPU_INTPRI_2r;
            *field   = CYPRESS_GW_MAC_ERRf;
            break;
        case ROUTE_EXCEPTION_TYPE_IP6_HOP_BY_HOP:
            *reg     = CYPRESS_QM_PKT2CPU_INTPRI_2r;
            *field   = CYPRESS_IPV6_HOPBYHOPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}   /* end of _dal_trap_routeExceptionPriRegField_get */

/* Function Name:
 *      dal_cypress_trap_routeExceptionPri_get
 * Description:
 *      Get priority of route exception packets trapped to CPU.
 * Input:
 *      unit    - unit id
 *      type    - configure priority for which route exception
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_trap_routeExceptionPri_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  reg;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ROUTE_EXCEPTION_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    ret = _dal_trap_routeExceptionPriRegField_get(unit, type, &reg, &field);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, reg, field, pPriority)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pPriority=%d", *pPriority);

    return RT_ERR_OK;
}    /* end of dal_cypress_trap_routeExceptionPri_get */

/* Function Name:
 *      dal_cypress_trap_routeExceptionPri_set
 * Description:
 *      Set priority of route exception packets trapped to CPU.
 * Input:
 *      unit        - unit id
 *      type        - configure priority for which route exception
 *      priority    - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
int32
dal_cypress_trap_routeExceptionPri_set(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_pri_t priority)
{
    int32   ret;
    uint32  reg;
    uint32  field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ROUTE_EXCEPTION_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_DOT1P_PRIORITY_MAX < priority), RT_ERR_PRIORITY);

    ret = _dal_trap_routeExceptionPriRegField_get(unit, type, &reg, &field);
    if (RT_ERR_OK != ret)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_LOCK(unit);

    /*Set entry to CHIP*/
    if ((ret = reg_field_write(unit, reg, field, &priority)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_cypress_trap_routeExceptionPri_set */

/* Function Name:
 *      dal_cypress_trap_mgmtFrameMgmtVlanEnable_get
 * Description:
 *      Get compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit                - unit id
 * Output:
 *      pEnable            - status of trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_cypress_trap_mgmtFrameMgmtVlanEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_CTRLr, CYPRESS_VID_CMPf,
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
}    /* end of dal_cypress_trap_mgmtFrameMgmtVlanEnable_get */

/* Function Name:
 *      dal_cypress_trap_mgmtFrameMgmtVlanEnable_set
 * Description:
 *      Set compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit            - unit id
 *      enable          - status of trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT     - Invalid input parameter
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_cypress_trap_mgmtFrameMgmtVlanEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_CTRLr, CYPRESS_VID_CMPf,
            &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}    /* end of dal_cypress_trap_mgmtFrameMgmtVlanEnable_set */

/* Function Name:
 *      dal_cypress_trap_mgmtFrameSelfARPEnable_get
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
dal_cypress_trap_mgmtFrameSelfARPEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_SPCL_TRAP_CTRLr, CYPRESS_IP_CMPf,
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
 *      dal_cypress_trap_mgmtFrameSelfARPEnable_set
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
dal_cypress_trap_mgmtFrameSelfARPEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_SPCL_TRAP_CTRLr, CYPRESS_IP_CMPf,
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
 *      dal_cypress_trap_bpduFloodPortmask_get
 * Description:
 *      Get BPDU flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_cypress_trap_bpduFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, pmsk0=0x%x, pmsk1=0x%x",
           unit,
           RTK_PORTMASK_WORD_GET(*pflood_portmask, 0),
           RTK_PORTMASK_WORD_GET(*pflood_portmask, 1));

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_RMA_BPDU_FLD_PMSKr, CYPRESS_PMSK_0f, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, CYPRESS_RMA_BPDU_FLD_PMSKr, CYPRESS_PMSK_1f, &pflood_portmask->bits[1])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_bpduFloodPortmask_set
 * Description:
 *      Set BPDU flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_cypress_trap_bpduFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, pmsk0=0x%x, pmsk1=0x%x",
           unit,
           RTK_PORTMASK_WORD_GET(*pflood_portmask, 0),
           RTK_PORTMASK_WORD_GET(*pflood_portmask, 1));

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(hwp_portmaskValid_Chk(unit, pflood_portmask), RT_ERR_PORT_MASK);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_RMA_BPDU_FLD_PMSKr, CYPRESS_PMSK_0f, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_RMA_BPDU_FLD_PMSKr, CYPRESS_PMSK_1f, &pflood_portmask->bits[1])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_trap_rmaLookupMissActionEnable_get
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
dal_cypress_trap_rmaLookupMissActionEnable_get(uint32 unit, rtk_enable_t *pEnable)
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

    if ((ret = reg_field_read(unit, CYPRESS_RMA_CTRL_3r, CYPRESS_BYPASS_LM_ACT_ENf, &value)) != RT_ERR_OK)
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
}   /* end of dal_cypress_trap_rmaLookupMissActionEnable_get */

/* Function Name:
 *      dal_cypress_trap_rmaLookupMissActionEnable_set
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
dal_cypress_trap_rmaLookupMissActionEnable_set(uint32 unit, rtk_enable_t enable)
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

    if ((ret = reg_field_write(unit, CYPRESS_RMA_CTRL_3r, CYPRESS_BYPASS_LM_ACT_ENf, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_rmaLookupMissActionEnable_set */

/* Function Name:
 *      _dal_cypress_trap_init_config
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
 *      Must initialize trunk module before calling this API
 */
static int32
_dal_cypress_trap_init_config(uint32 unit)
{
    int32   ret;
    rtk_mac_t rma_slow_proto = {{0x01,0x80,0xC2,0x00,0x00,0x02}};

    if ((ret = dal_cypress_trap_rmaAction_set(unit, &rma_slow_proto, RTK_DEFAULT_TRAP_RMA_SP_MCAST_ACTION)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_trap_init_config */

/*
 * Function Name:
 *      dal_cypress_trap_cfmAct_get
 * Description:
 *      Get action for receive specified type of CFM frame.
 * Input:
 *      unit    - unit id
 *      type    - CFM frame type
 *      mdl     - MD level (for unknow type, the field is not used)
 * Output:
 *      pAction - pointer buffer of action for receive specified type of CFM frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_LINK_FAULT_DETECT (only for CCM)
 */
int32
dal_cypress_trap_cfmAct_get(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t *pAction)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,type=%d,mdl=%d",unit, type, mdl);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((TRAP_CFM_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case TRAP_CFM_TYPE_UNKN:
            return dal_cypress_trap_cfmUnknownFrameAct_get(unit, pAction);
        case TRAP_CFM_TYPE_CCM:
            return dal_cypress_trap_cfmCcmAct_get(unit, mdl, (rtk_trap_oam_action_t *)pAction);
        case TRAP_CFM_TYPE_LT:
        case TRAP_CFM_TYPE_LB:
            return dal_cypress_trap_cfmLoopbackAct_get(unit, mdl, pAction);
        case TRAP_CFM_TYPE_ETHDM:
            return dal_cypress_trap_cfmEthDmAct_get(unit, mdl, pAction);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_TRAP), "type %d is invalid.", type);
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmAct_get */

/* Function Name:
 *      dal_cypress_trap_cfmAct_set
 * Description:
 *      Set action for receive specified type of CFM frame.
 * Input:
 *      unit    - unit id
 *      type    - CFM frame type
 *      mdl     - MD level (for unknow type, the field is not used)
 *      action  - receive specified type of CFM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_LINK_FAULT_DETECT (only for CCM)
 */
int32
dal_cypress_trap_cfmAct_set(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t action)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,type=%d,mdl=%d,action=%d",unit, type, mdl, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((TRAP_CFM_TYPE_END <= type), RT_ERR_INPUT);

    /* set reg */
    switch (type)
    {
        case TRAP_CFM_TYPE_UNKN:
            return dal_cypress_trap_cfmUnknownFrameAct_set(unit, action);
        case TRAP_CFM_TYPE_CCM:
            return dal_cypress_trap_cfmCcmAct_set(unit, mdl, action);
        case TRAP_CFM_TYPE_LT:
        case TRAP_CFM_TYPE_LB:
            return dal_cypress_trap_cfmLoopbackAct_set(unit, mdl, action);
        case TRAP_CFM_TYPE_ETHDM:
            return dal_cypress_trap_cfmEthDmAct_set(unit, mdl, action);
        default:
            RT_ERR(RT_ERR_INPUT, (MOD_DAL|MOD_TRAP), "type %d is invalid.", type);
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}   /* end of dal_cypress_trap_cfmAct_set */


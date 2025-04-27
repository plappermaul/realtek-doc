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
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <dal/longan/dal_longan_trap.h>
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

const static uint16 rmaControl_regidx[] = {LONGAN_RMA_CTRL_0r, LONGAN_RMA_CTRL_1r, LONGAN_RMA_CTRL_2r};

const static uint16 rma_fieldidx[] = {0, LONGAN_RMA_01_ACTf, LONGAN_RMA_02_ACTf, LONGAN_RMA_03_ACTf, LONGAN_RMA_04_ACTf, LONGAN_RMA_05_ACTf,
                                        LONGAN_RMA_06_ACTf, LONGAN_RMA_07_ACTf, LONGAN_RMA_08_ACTf, LONGAN_RMA_09_ACTf, LONGAN_RMA_0A_ACTf,
                                        LONGAN_RMA_0B_ACTf, LONGAN_RMA_0C_ACTf, LONGAN_RMA_0D_ACTf, LONGAN_RMA_0E_ACTf, LONGAN_RMA_0F_ACTf,
                                        LONGAN_RMA_10_ACTf, LONGAN_RMA_11_ACTf, LONGAN_RMA_12_ACTf, LONGAN_RMA_13_ACTf, LONGAN_RMA_14_ACTf,
                                        LONGAN_RMA_15_ACTf, LONGAN_RMA_16_ACTf, LONGAN_RMA_17_ACTf, LONGAN_RMA_18_ACTf, LONGAN_RMA_19_ACTf,
                                        LONGAN_RMA_1A_ACTf, LONGAN_RMA_1B_ACTf, LONGAN_RMA_1C_ACTf, LONGAN_RMA_1D_ACTf, LONGAN_RMA_1E_ACTf,
                                        LONGAN_RMA_1F_ACTf, LONGAN_RMA_20_ACTf, LONGAN_RMA_21_ACTf, LONGAN_RMA_22_ACTf, LONGAN_RMA_23_ACTf,
                                        LONGAN_RMA_24_ACTf, LONGAN_RMA_25_ACTf, LONGAN_RMA_26_ACTf, LONGAN_RMA_27_ACTf, LONGAN_RMA_28_ACTf,
                                        LONGAN_RMA_29_ACTf, LONGAN_RMA_2A_ACTf, LONGAN_RMA_2B_ACTf, LONGAN_RMA_2C_ACTf, LONGAN_RMA_2D_ACTf,
                                        LONGAN_RMA_2E_ACTf, LONGAN_RMA_2F_ACTf};


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
static int32 _dal_longan_trap_init_config(uint32 unit);

/* Function Name:
 *      dal_longan_trapMapper_init
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
dal_longan_trapMapper_init(dal_mapper_t *pMapper)
{
    pMapper->trap_init = dal_longan_trap_init;
    pMapper->trap_rmaAction_get = dal_longan_trap_rmaAction_get;
    pMapper->trap_rmaAction_set = dal_longan_trap_rmaAction_set;
    pMapper->trap_bypassStp_get = dal_longan_trap_bypassStp_get;
    pMapper->trap_bypassStp_set = dal_longan_trap_bypassStp_set;
    pMapper->trap_bypassVlan_get = dal_longan_trap_bypassVlan_get;
    pMapper->trap_bypassVlan_set = dal_longan_trap_bypassVlan_set;
    pMapper->trap_rmaLearningEnable_get = dal_longan_trap_rmaLearningEnable_get;
    pMapper->trap_rmaLearningEnable_set = dal_longan_trap_rmaLearningEnable_set;
    pMapper->trap_userDefineRma_get = dal_longan_trap_userDefineRma_get;
    pMapper->trap_userDefineRma_set = dal_longan_trap_userDefineRma_set;
    pMapper->trap_userDefineRmaEnable_get = dal_longan_trap_userDefineRmaEnable_get;
    pMapper->trap_userDefineRmaEnable_set = dal_longan_trap_userDefineRmaEnable_set;
    pMapper->trap_userDefineRmaAction_get = dal_longan_trap_userDefineRmaAction_get;
    pMapper->trap_userDefineRmaAction_set = dal_longan_trap_userDefineRmaAction_set;
    pMapper->trap_userDefineRmaLearningEnable_get = dal_longan_trap_userDefineRmaLearningEnable_get;
    pMapper->trap_userDefineRmaLearningEnable_set = dal_longan_trap_userDefineRmaLearningEnable_set;
    pMapper->trap_oamPDUAction_get = dal_longan_trap_oamPDUAction_get;
    pMapper->trap_oamPDUAction_set = dal_longan_trap_oamPDUAction_set;
    pMapper->trap_portOamLoopbackParAction_get = dal_longan_trap_portOamLoopbackParAction_get;
    pMapper->trap_portOamLoopbackParAction_set = dal_longan_trap_portOamLoopbackParAction_set;
    pMapper->trap_oamTarget_get = dal_longan_trap_oamTarget_get;
    pMapper->trap_oamTarget_set = dal_longan_trap_oamTarget_set;
    pMapper->trap_mgmtFrameAction_get = dal_longan_trap_mgmtFrameAction_get;
    pMapper->trap_mgmtFrameAction_set = dal_longan_trap_mgmtFrameAction_set;
    pMapper->trap_mgmtFrameQueue_get = dal_longan_trap_mgmtFrameQueue_get;
    pMapper->trap_mgmtFrameQueue_set = dal_longan_trap_mgmtFrameQueue_set;
    pMapper->trap_mgmtFrameLearningEnable_get = dal_longan_trap_mgmtFrameLearningEnable_get;
    pMapper->trap_mgmtFrameLearningEnable_set = dal_longan_trap_mgmtFrameLearningEnable_set;
    pMapper->trap_portMgmtFrameAction_get = dal_longan_trap_portMgmtFrameAction_get;
    pMapper->trap_portMgmtFrameAction_set = dal_longan_trap_portMgmtFrameAction_set;
    pMapper->trap_mgmtFrameTarget_get = dal_longan_trap_mgmtFrameTarget_get;
    pMapper->trap_mgmtFrameTarget_set = dal_longan_trap_mgmtFrameTarget_set;
    pMapper->trap_pktWithCFIAction_get = dal_longan_trap_pktWithCFIAction_get;
    pMapper->trap_pktWithCFIAction_set = dal_longan_trap_pktWithCFIAction_set;
    pMapper->trap_pktWithOuterCFIAction_get = dal_longan_trap_pktWithOuterCFIAction_get;
    pMapper->trap_pktWithOuterCFIAction_set = dal_longan_trap_pktWithOuterCFIAction_set;
    pMapper->trap_bpduFloodPortmask_get = dal_longan_trap_bpduFloodPortmask_get;
    pMapper->trap_bpduFloodPortmask_set = dal_longan_trap_bpduFloodPortmask_set;
    pMapper->trap_eapolFloodPortmask_get = dal_longan_trap_eapolFloodPortmask_get;
    pMapper->trap_eapolFloodPortmask_set = dal_longan_trap_eapolFloodPortmask_set;
    pMapper->trap_lldpFloodPortmask_get = dal_longan_trap_lldpFloodPortmask_get;
    pMapper->trap_lldpFloodPortmask_set = dal_longan_trap_lldpFloodPortmask_set;
    pMapper->trap_userDefineFloodPortmask_get = dal_longan_trap_userDefineFloodPortmask_get;
    pMapper->trap_userDefineFloodPortmask_set = dal_longan_trap_userDefineFloodPortmask_set;
    pMapper->trap_rmaFloodPortmask_get = dal_longan_trap_rmaFloodPortmask_get;
    pMapper->trap_rmaFloodPortmask_set = dal_longan_trap_rmaFloodPortmask_set;
    pMapper->trap_rmaCancelMirror_get = dal_longan_trap_rmaCancelMirror_get;
    pMapper->trap_rmaCancelMirror_set = dal_longan_trap_rmaCancelMirror_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_init
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
dal_longan_trap_init(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;

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
    if ((ret = _dal_longan_trap_init_config(unit)) != RT_ERR_OK)
    {
        trap_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "init default configuration failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_longan_trap_init */

/* Function Name:
 *      dal_longan_trap_rmaAction_get
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
dal_longan_trap_rmaAction_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t *pRma_action)
{
    int32   ret = RT_ERR_FAILED;
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


    /*BPDU  have per-port control register*/
    if(pRma_frame->octet[5] == 0)
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
        case 3:
            *pRma_action = MGMT_ACTION_TRAP2MASTERCPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pRma_action=%d", *pRma_action);

    return RT_ERR_OK;
} /* end of dal_longan_trap_rmaAction_get */

/* Function Name:
 *      dal_longan_trap_rmaAction_set
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
 *      -   MGMT_ACTION_TRAP2MASTERCPU
 */
int32
dal_longan_trap_rmaAction_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t rma_action)
{
    int32   ret = RT_ERR_FAILED;
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

    /*BPDU  have per-port control register*/
    if(pRma_frame->octet[5] == 0)
    {
        /* *pRma_action = MGMT_ACTION_FORWARD; */
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
        case MGMT_ACTION_TRAP2MASTERCPU:
            value = 3;
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
} /* end of dal_longan_trap_rmaAction_set */

/* Function Name:
 *      dal_longan_trap_rmaLearningEnable_get
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
dal_longan_trap_rmaLearningEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

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
    if ((ret = reg_array_field_read(unit, LONGAN_RMA_SMAC_LRN_CTRLr, REG_ARRAY_INDEX_NONE, pRma_frame->octet[5], LONGAN_LRNf, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_rmaLearningEnable_set
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
dal_longan_trap_rmaLearningEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;

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

    if ((ret = reg_array_field_write(unit, LONGAN_RMA_SMAC_LRN_CTRLr, REG_ARRAY_INDEX_NONE, pRma_frame->octet[5], LONGAN_LRNf, &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_bypassStp_get
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
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
int32
dal_longan_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_STP_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (frameType)
    {
        case BYPASS_STP_TYPE_USER_DEF_0:
            reg_idx = 0;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = 1;
            break;
        case BYPASS_STP_TYPE_USER_DEF_2:
            reg_idx = 2;
            break;
        case BYPASS_STP_TYPE_USER_DEF_3:
            reg_idx = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          reg_idx,
                          LONGAN_BYPASS_STPf,
                          pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_bypassStp_set
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
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
int32
dal_longan_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, enable=%d", unit, frameType, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_STP_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (frameType)
    {
        case BYPASS_STP_TYPE_USER_DEF_0:
            reg_idx = 0;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = 1;
            break;
        case BYPASS_STP_TYPE_USER_DEF_2:
            reg_idx = 2;
            break;
        case BYPASS_STP_TYPE_USER_DEF_3:
            reg_idx = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          reg_idx,
                          LONGAN_BYPASS_STPf,
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
 *      dal_longan_trap_bypassVlan_get
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
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
int32
dal_longan_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (frameType)
    {
        case BYPASS_STP_TYPE_USER_DEF_0:
            reg_idx = 0;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = 1;
            break;
        case BYPASS_STP_TYPE_USER_DEF_2:
            reg_idx = 2;
            break;
        case BYPASS_STP_TYPE_USER_DEF_3:
            reg_idx = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          reg_idx,
                          LONGAN_BYPASS_VLANf,
                          pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_bypassVlan_set
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
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
int32
dal_longan_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, enable=%d", unit, frameType, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(frameType >= BYPASS_VLAN_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (frameType)
    {
        case BYPASS_STP_TYPE_USER_DEF_0:
            reg_idx = 0;
            break;
        case BYPASS_STP_TYPE_USER_DEF_1:
            reg_idx = 1;
            break;
        case BYPASS_STP_TYPE_USER_DEF_2:
            reg_idx = 2;
            break;
        case BYPASS_STP_TYPE_USER_DEF_3:
            reg_idx = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          reg_idx,
                          LONGAN_BYPASS_VLANf,
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
 *      dal_longan_trap_userDefineRma_get
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
dal_longan_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma)
{
    int32   ret = RT_ERR_FAILED;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pUserDefinedRma), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_TYPEf,
                          &pUserDefinedRma->cmpType)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ETHER_TYPEf,
                          &pUserDefinedRma->frameValue)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MIN_LOf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    pUserDefinedRma->mac_min.octet[2] = (value >> 24) & 0xff;
    pUserDefinedRma->mac_min.octet[3] = (value >> 16) & 0xff;
    pUserDefinedRma->mac_min.octet[4] = (value >> 8) & 0xff;
    pUserDefinedRma->mac_min.octet[5] = value & 0xff;

    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MIN_HIf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    pUserDefinedRma->mac_min.octet[0] = (value >> 8) & 0xff;
    pUserDefinedRma->mac_min.octet[1] = value & 0xff;

    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MAX_LOf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    pUserDefinedRma->mac_max.octet[2] = (value >> 24) & 0xff;
    pUserDefinedRma->mac_max.octet[3] = (value >> 16) & 0xff;
    pUserDefinedRma->mac_max.octet[4] = (value >> 8) & 0xff;
    pUserDefinedRma->mac_max.octet[5] = value & 0xff;

    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MAX_HIf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    pUserDefinedRma->mac_max.octet[0] = (value >> 8) & 0xff;
    pUserDefinedRma->mac_max.octet[1] = value & 0xff;

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->frameValue=%x",pUserDefinedRma->frameValue);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac_min=%x-%x-%x-%x-%x-%x",
           pUserDefinedRma->mac_min.octet[0], pUserDefinedRma->mac_min.octet[1], pUserDefinedRma->mac_min.octet[2],
           pUserDefinedRma->mac_min.octet[3], pUserDefinedRma->mac_min.octet[4], pUserDefinedRma->mac_min.octet[5]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac_max=%x-%x-%x-%x-%x-%x",
           pUserDefinedRma->mac_max.octet[0], pUserDefinedRma->mac_max.octet[1], pUserDefinedRma->mac_max.octet[2],
           pUserDefinedRma->mac_max.octet[3], pUserDefinedRma->mac_max.octet[4], pUserDefinedRma->mac_max.octet[5]);

    return RT_ERR_OK;
} /* end of dal_longan_trap_userDefineRma_get */

/* Function Name:
 *      dal_longan_trap_userDefineRma_set
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
dal_longan_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d", unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pUserDefinedRma), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->frameValue=%x",pUserDefinedRma->frameValue);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac_min=%x-%x-%x-%x-%x-%x",
       pUserDefinedRma->mac_min.octet[0], pUserDefinedRma->mac_min.octet[1], pUserDefinedRma->mac_min.octet[2],
       pUserDefinedRma->mac_min.octet[3], pUserDefinedRma->mac_min.octet[4], pUserDefinedRma->mac_min.octet[5]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pUserDefinedRma->mac_max=%x-%x-%x-%x-%x-%x",
       pUserDefinedRma->mac_max.octet[0], pUserDefinedRma->mac_max.octet[1], pUserDefinedRma->mac_max.octet[2],
       pUserDefinedRma->mac_max.octet[3], pUserDefinedRma->mac_max.octet[4], pUserDefinedRma->mac_max.octet[5]);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_TYPEf,
                          &pUserDefinedRma->cmpType)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ETHER_TYPEf,
                          &pUserDefinedRma->frameValue)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    value = (pUserDefinedRma->mac_min.octet[2] << 24) | (pUserDefinedRma->mac_min.octet[3] << 16) |
        (pUserDefinedRma->mac_min.octet[4] << 8) | (pUserDefinedRma->mac_min.octet[5]);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MIN_LOf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    value = (pUserDefinedRma->mac_min.octet[0] << 8) | (pUserDefinedRma->mac_min.octet[1]);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MIN_HIf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    value = (pUserDefinedRma->mac_max.octet[2] << 24) | (pUserDefinedRma->mac_max.octet[3] << 16) |
        (pUserDefinedRma->mac_max.octet[4] << 8) | (pUserDefinedRma->mac_max.octet[5]);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MAX_LOf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    value = (pUserDefinedRma->mac_max.octet[0] << 8) | (pUserDefinedRma->mac_max.octet[1]);
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ADDR_MAX_HIf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_userDefineRma_set */

/* Function Name:
 *      dal_longan_trap_userDefineRmaEnable_get
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
dal_longan_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ENf,
                          pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_LONGAN_trap_userDefineRmaEnable_get */

/* Function Name:
 *      dal_longan_trap_userDefineRmaEnable_set
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
dal_longan_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d, enable=%d", unit, userDefine_idx, enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ENf,
                          &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_userDefineRmaEnable_set */

/* Function Name:
 *      dal_longan_trap_userDefineRmaAction_get
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
 *      - MGMT_ACTION_DROP
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_TRAP2MASTERCPU
 *      - MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
int32
dal_longan_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, userDefine_idx=%d",
                unit, userDefine_idx);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ACTf,
                          &value)) != RT_ERR_OK)
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
            *pAction = MGMT_ACTION_TRAP2MASTERCPU;
            break;
        case 4:
            *pAction = MGMT_ACTION_FLOOD_TO_ALL_PORT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_longan_trap_userDefineRmaAction_get */

/* Function Name:
 *      dal_longan_trap_userDefineRmaAction_set
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
 *      - MGMT_ACTION_FORWARD
 *      - MGMT_ACTION_DROP
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_TRAP2MASTERCPU
 *      - MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
int32
dal_longan_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

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
        case MGMT_ACTION_TRAP2MASTERCPU:
            value = 3;
            break;
        case MGMT_ACTION_FLOOD_TO_ALL_PORT:
            value = 4;
            break;
        default:
            return RT_ERR_FWD_ACTION;
    }

    TRAP_SEM_LOCK(unit);

    /* set entry from CHIP*/
    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_ACTf,
                          &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_userDefineRmaAction_set */

/* Function Name:
 *      dal_longan_trap_userDefineRmaLearningEnable_get
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
dal_longan_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_array_field_read(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_LRNf,
                          pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_userDefineRmaLearningEnable_get */

/* Function Name:
 *      dal_longan_trap_userDefineRmaLearningEnable_set
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
dal_longan_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((userDefine_idx >= HAL_MAX_NUM_OF_RMA_USER_DEFINED(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit,
                          LONGAN_RMA_USR_DEF_CTRLr,
                          REG_ARRAY_INDEX_NONE,
                          userDefine_idx,
                          LONGAN_LRNf,
                          &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_userDefineRmaLearningEnable_set */


/* Function Name:
 *      dal_longan_trap_oamPDUAction_get
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
dal_longan_trap_oamPDUAction_get(uint32 unit, rtk_action_t *pAction)
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
    if ((ret = reg_field_read(unit, LONGAN_OAM_CTRLr, LONGAN_ENf,
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

    if ((ret = reg_field_read(unit, LONGAN_OAM_CTRLr, LONGAN_DIS_ACTf,
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
}   /*end of dal_longan_trap_oamPDUAction_get*/

/* Function Name:
 *      dal_longan_trap_oamPDUAction_set
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
dal_longan_trap_oamPDUAction_set(uint32 unit, rtk_action_t action)
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
    if ((ret = reg_field_write(unit, LONGAN_OAM_CTRLr, LONGAN_ENf, &en_val))
            != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    if (0 == en_val)
    {
        if ((ret = reg_field_write(unit, LONGAN_OAM_CTRLr, LONGAN_DIS_ACTf,
                &dis_val) ) != RT_ERR_OK)
        {
            /* roll back */
            en_val = 1;
            reg_field_write(unit, LONGAN_OAM_CTRLr, LONGAN_ENf, &en_val);

            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /*end of dal_longan_trap_oamPDUAction_set*/

/* Function Name:
 *      dal_longan_trap_portOamLoopbackParAction_get
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
dal_longan_trap_portOamLoopbackParAction_get(uint32 unit,
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
    if ((ret = reg_array_field_read(unit, LONGAN_OAM_PORT_ACT_CTRLr,
            port, REG_ARRAY_INDEX_NONE, LONGAN_PAR_ACTf,
            pAction)) != RT_ERR_OK)
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
}   /* end of dal_longan_trap_portOamLoopbackParAction_get */

/* Function Name:
 *      dal_longan_trap_portOamLoopbackParAction_set
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
dal_longan_trap_portOamLoopbackParAction_set(uint32 unit,
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
    if ((ret = reg_array_field_write(unit, LONGAN_OAM_PORT_ACT_CTRLr,
            port, REG_ARRAY_INDEX_NONE, LONGAN_PAR_ACTf, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_trap_portOamLoopbackParAction_set */

/*
 * Function Name:
 *      dal_longan_trap_oamTarget_get
 * Description:
 *      Get information of OAM PDU trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of OAM PDU trap packet
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
dal_longan_trap_oamTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_OAM_CTRLr, LONGAN_CPU_SELf,
            &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "read reg fail");
        return ret;
    }

    if (0 == val)
    {
        *pTarget = RTK_TRAP_LOCAL;
    }
    else
    {
        *pTarget = RTK_TRAP_MASTER;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_trap_oamTarget_get */

/* Function Name:
 *      dal_longan_trap_oamTarget_set
 * Description:
 *      Set information of OAM PDU trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of OAM PDU trap packet
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
dal_longan_trap_oamTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,target=%d",unit, target);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_TRAP_END <= target), RT_ERR_INPUT);

    /* function body */
    TRAP_SEM_LOCK(unit);

    if (RTK_TRAP_LOCAL == target)
    {
        val = 0;
    }
    else
    {
        val = 1;
    }

    if ((ret = reg_field_write(unit, LONGAN_OAM_CTRLr, LONGAN_CPU_SELf, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "write reg fail");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_trap_oamTarget_set */




/* Function Name:
 *      dal_longan_trap_mgmtFrameAction_get
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
 *                                          9300
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 *          - MGMT_TYPE_RLDP_RLPP           (1010100)
 */
int32
dal_longan_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg, field, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d", unit, frameType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType >= MGMT_TYPE_END), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    switch(frameType)
    {
        case MGMT_TYPE_L2_CRC_ERR:
            reg = LONGAN_SPCL_TRAP_CTRLr;
            field = LONGAN_L2_CRC_ERR_ACTf;
            break;

        case MGMT_TYPE_IP4_CHKSUM_ERR:
            reg = LONGAN_SPCL_TRAP_CTRLr;
            field = LONGAN_IP4_CHKSUM_ERR_ACTf;
            break;

        case MGMT_TYPE_RLDP_RLPP:
            reg = LONGAN_RLDP_RLPP_CTRLr;
            field = LONGAN_RLDP_RLPP_TRAPf;
            break;

        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, reg, field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    if(frameType == MGMT_TYPE_L2_CRC_ERR || frameType == MGMT_TYPE_IP4_CHKSUM_ERR)
    {
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
            default:
                return RT_ERR_FAILED;
        }
    }
    else if(frameType == MGMT_TYPE_RLDP_RLPP)
    {
        switch (value)
        {
            case 0:
                *pAction = MGMT_ACTION_FORWARD;
                break;
            case 1:
                *pAction = MGMT_ACTION_TRAP2CPU;
                break;
            case 2:
                *pAction = MGMT_ACTION_TRAP2MASTERCPU;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
}   /*end of dal_longan_trap_mgmtFrameAction_get*/

/* Function Name:
 *      dal_longan_trap_mgmtFrameAction_set
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
 *                                          9300
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 *          - MGMT_TYPE_RLDP_RLPP           (1010100)
 */
int32
dal_longan_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg, field, value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, frameType=%d, action=%d", unit, frameType, action);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((frameType >= MGMT_TYPE_END), RT_ERR_RMA_MGMT_TYPE);
    RT_PARAM_CHK((action >= MGMT_ACTION_END), RT_ERR_FWD_ACTION);

    /* action tranlate */
    if(frameType == MGMT_TYPE_L2_CRC_ERR || frameType == MGMT_TYPE_IP4_CHKSUM_ERR)
    {
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
            default:
                return RT_ERR_FWD_ACTION;
        }
    }
    else if(frameType == MGMT_TYPE_RLDP_RLPP)
    {
        switch (action)
        {
            case MGMT_ACTION_FORWARD:
                value = 0;
                break;
            case MGMT_ACTION_TRAP2CPU:
                value = 1;
                break;
            case MGMT_ACTION_TRAP2MASTERCPU:
                value = 2;
                break;
            default:
                break;
        }
    }


    switch(frameType)
    {
        case MGMT_TYPE_L2_CRC_ERR:
            reg = LONGAN_SPCL_TRAP_CTRLr;
            field = LONGAN_L2_CRC_ERR_ACTf;
            break;

        case MGMT_TYPE_IP4_CHKSUM_ERR:
            reg = LONGAN_SPCL_TRAP_CTRLr;
            field = LONGAN_IP4_CHKSUM_ERR_ACTf;
            break;

        case MGMT_TYPE_RLDP_RLPP:
            reg = LONGAN_RLDP_RLPP_CTRLr;
            field = LONGAN_RLDP_RLPP_TRAPf;
            break;

        default:
            return RT_ERR_INPUT;
    }


    TRAP_SEM_LOCK(unit);

    if((ret = reg_field_write(unit, reg, field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /*end of dal_longan_trap_mgmtFrameAction_set*/

/*
 * Function Name:
 *      dal_longan_trap_mgmtFrameTarget_get
 * Description:
 *      Get information of management frame trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of management frame trap
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
dal_longan_trap_mgmtFrameTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_SPCL_TRAP_CTRLr, LONGAN_CPU_SELf, &val)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "read reg fail");
        return ret;
    }

    if (0 == val)
    {
        *pTarget = RTK_TRAP_LOCAL;
    }
    else
    {
        *pTarget = RTK_TRAP_MASTER;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_trap_mgmtFrameTarget_get */

/* Function Name:
 *      dal_longan_trap_mgmtFrameTarget_set
 * Description:
 *      Set information of management frame trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of management frame trap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_longan_trap_mgmtFrameTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,target=%d",unit, target);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_TRAP_END <= target), RT_ERR_INPUT);

    /* function body */
    TRAP_SEM_LOCK(unit);

    if (RTK_TRAP_LOCAL == target)
    {
        val = 0;
    }
    else
    {
        val = 1;
    }

    if ((ret = reg_field_write(unit, LONGAN_SPCL_TRAP_CTRLr, LONGAN_CPU_SELf, &val) ) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "write reg fail");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_longan_trap_mgmtFrameTarget_set */

/* Function Name:
 *      dal_longan_trap_mgmtFrameQueue_get
 * Description:
 *      Get queue id of trapped packet.
 * Input:
 *      unit      - unit id
 *      qType - type of trapped packet
 * Output:
 *      pQid - pointer to queue id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_longan_trap_mgmtFrameQueue_get(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t *pQid)
{

    int32   ret = RT_ERR_FAILED;
    uint32  reg, field, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, qType=%d", unit, qType);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQid), RT_ERR_NULL_POINTER);

    switch (qType)
    {
        case TRAP_Q_ARP_REQ_REP_GRTS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_ARP_REQ_REPf;
            break;

        case TRAP_Q_DHCP_DHCP6:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_DHCP_DHCP6f;
            break;

        case TRAP_Q_IGMP_MLD:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IGMP_MLDf;
            break;

        case TRAP_Q_BPDU:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_BPDUf;
            break;

        case TRAP_Q_PTP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_PTPf;
            break;

        case TRAP_Q_LLDP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_LLDPf;
            break;

        case TRAP_Q_EAPOL:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_EAPOLf;
            break;

        case TRAP_Q_OAM:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_OAMPDUf;
            break;

        case TRAP_Q_LACP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_LACPf;
            break;

        case TRAP_Q_RLDP_RLPP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_RLDP_RLPPf;
            break;

        case TRAP_Q_RMA_USR_DEF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_RMA_USR_DEFf;
            break;

        case TRAP_Q_RMA:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_RMA_XXf;
            break;

        case TRAP_Q_IP4_IP6_HDR_ERR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_IP4_IP6_HDR_ERRf;
            break;

        case TRAP_Q_L2_CRC_ERR: /* TODO no register. */
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_L2_ERR_PKTf;
            break;

        case TRAP_Q_IP4_CHKSUM_ERR:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_L3_ERR_PKTf;
            break;

        case TRAP_Q_IPV6ND:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_NEIGHBOR_DISCOVERf;
            break;

        case TRAP_Q_IP4_IP6_RSVD_ADDR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_IP4_IP6_RESERVE_ADDRf;
            break;

        case TRAP_Q_IGR_VLAN_FLTR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_IGR_VLAN_FLTRf;
            break;

        case TRAP_Q_CFI:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_INN_OUTER_CFI_EQL_1f;
            break;

        case TRAP_Q_IVC:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_IVCf;
            break;

        case TRAP_Q_INVALID_SA:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_INVALID_SAf;
            break;

        case TRAP_Q_MAC_CST:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_MAC_CSTf;
            break;

        case TRAP_Q_NEW_SA:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_NEW_SAf;
            break;

        case TRAP_Q_PMV_FORBID:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_PMV_FORBIDf;
            break;

        case TRAP_Q_L2_STTC_PMV:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_L2_STTC_PMVf;
            break;

        case TRAP_Q_L2_DYN_PMV:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_L2_DYN_PMVf;
            break;

        case TRAP_Q_HASH_FULL:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_HASH_FULLf;
            break;

        case TRAP_Q_ATK:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_ATK_TYPEf;
            break;

        case TRAP_Q_ACL_HIT:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_ACL_HITf;
            break;

        case TRAP_Q_MIR_HIT:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_MIR_HITf;
            break;

        case TRAP_Q_CAPWAP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_CAPWAPf;
            break;

        case TRAP_Q_CPU2CPU_TALK:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_CPU2CPU_TALKf;
            break;

        case TRAP_Q_L3_IPUC_RPF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_L3_IPUC_RPFf;
            break;

        case TRAP_Q_L3_IPMC_RPF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_IPMC_RPFf;
            break;

        case TRAP_Q_L2_UC_MC_BDG_LUMIS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_L2_UC_MC_BRIDGE_LU_MISSf;
            break;

        case TRAP_Q_IP4_IP6_BDG_LUMIS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_IP4_IP6_BRIDGE_LU_MISSf;
            break;

        case TRAP_Q_ROUTER_MAC_IF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_ROUTER_MAC_IFf;
            break;

        case TRAP_Q_L3_IPUC_NON_IP_PKT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_L3_IPUC_NON_IP_PKTf;
            break;

        case TRAP_Q_ROUTE_IP_CHK:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_ROUTING_IP_CHKf;
            break;

        case TRAP_Q_L3_ROUTE_DIP_DMAC_MISMATCH:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_L3_ROUTING_DIP_DMAC_MISMATCHf;
            break;

        case TRAP_Q_IP6_UC_MC_HOP_BY_HOP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP6_UC_MC_HOP_BY_HOPf;
            break;

        case TRAP_Q_IP6_UC_MC_ROUTE_HDR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP6_UC_MC_ROUTING_HDRf;
            break;

        case TRAP_Q_IP4_IP_OPT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP4_IP_OPTf;
            break;

        case TRAP_Q_IP4_IP6_MC_ROUTE_LUMIS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP4_IP6_MC_ROUTING_LU_MISSf;
            break;

        case TRAP_Q_L3_IPUC_NULL_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_L3_IPUC_NULL_ROUTEf;
            break;

        case TRAP_Q_L3_IPUC_PBR_NULL_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_L3_IPUC_PBR_NULL_ROUTEf;
            break;

        case TRAP_Q_L3_UC_HOST_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_UC_HOST_ROUTEf;
            break;

        case TRAP_Q_L3_UC_NET_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_UC_NET_ROUTEf;
            break;

        case TRAP_Q_L3_MC_BDG_ENTRY:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_MC_BRIDGE_ENTRYf;
            break;

        case TRAP_Q_L3_MC_ROUTE_ENTRY:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_MC_ROUTE_ENTRYf;
            break;

        case TRAP_Q_ROUTE_EXCPT_NH_AGE_OUT_ACT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_ROUTING_EXCPT_NH_AGE_OUT_ACTf;
            break;

        case TRAP_Q_IP4_IP6_ICMP_REDRT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IP4_IP6_ICMP_REDRTf;
            break;

        case TRAP_Q_IPUC_MTU:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPUC_MTUf;
            break;

        case TRAP_Q_IPMC_MTU:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPMC_MTUf;
            break;

        case TRAP_Q_IPUC_TTL:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPUC_TTLf;
            break;

        case TRAP_Q_IPMC_TTL:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPMC_TTLf;
            break;

        case TRAP_Q_NOTIFICATION:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_L2_NTFYf;
            break;

        case TRAP_Q_NORMAL_FWD:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_8r;
            field = LONGAN_NORMAL_FWDf;
            break;

        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);

    /*Get entry from CHIP*/
    if((ret = reg_field_read(unit, reg, field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    *pQid = value & 0x7;

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_mgmtFrameQueue_get */

/* Function Name:
 *      dal_longan_trap_mgmtFrameQueue_set
 * Description:
 *      Set queue id of trapped packet.
 * Input:
 *      unit      - unit id
 *      qType - type of trapped packet
 *      qid      - qid
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID         - invalid queue id
 * Note:
 *      None
 */
int32
dal_longan_trap_mgmtFrameQueue_set(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t qid)
{

    int32   ret = RT_ERR_FAILED;
    uint32  reg, field, value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, qType=%d, qid=%d", unit, qType, qid);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((qid >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);

    switch (qType)
    {
        case TRAP_Q_ARP_REQ_REP_GRTS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_ARP_REQ_REPf;
            break;

        case TRAP_Q_DHCP_DHCP6:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_DHCP_DHCP6f;
            break;

        case TRAP_Q_IGMP_MLD:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IGMP_MLDf;
            break;

        case TRAP_Q_BPDU:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_BPDUf;
            break;

        case TRAP_Q_PTP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_PTPf;
            break;

        case TRAP_Q_LLDP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_LLDPf;
            break;

        case TRAP_Q_EAPOL:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_EAPOLf;
            break;

        case TRAP_Q_OAM:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_OAMPDUf;
            break;

        case TRAP_Q_LACP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_2r;
            field = LONGAN_RMA_LACPf;
            break;

        case TRAP_Q_RLDP_RLPP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_RLDP_RLPPf;
            break;

        case TRAP_Q_RMA_USR_DEF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_RMA_USR_DEFf;
            break;
        case TRAP_Q_RMA:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_RMA_XXf;
            break;

        case TRAP_Q_IP4_IP6_HDR_ERR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_IP4_IP6_HDR_ERRf;
            break;

        case TRAP_Q_L2_CRC_ERR:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_L2_ERR_PKTf;
            break;

        case TRAP_Q_IP4_CHKSUM_ERR:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_L3_ERR_PKTf;
            break;

        case TRAP_Q_IPV6ND:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_NEIGHBOR_DISCOVERf;
            break;

        case TRAP_Q_IP4_IP6_RSVD_ADDR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_IP4_IP6_RESERVE_ADDRf;
            break;

        case TRAP_Q_IGR_VLAN_FLTR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_IGR_VLAN_FLTRf;
            break;

        case TRAP_Q_CFI:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_INN_OUTER_CFI_EQL_1f;
            break;

        case TRAP_Q_IVC:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_IVCf;
            break;

        case TRAP_Q_INVALID_SA:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_INVALID_SAf;
            break;

        case TRAP_Q_MAC_CST:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_MAC_CSTf;
            break;

        case TRAP_Q_NEW_SA:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_NEW_SAf;
            break;

        case TRAP_Q_PMV_FORBID:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_PMV_FORBIDf;
            break;

        case TRAP_Q_L2_STTC_PMV:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_L2_STTC_PMVf;
            break;

        case TRAP_Q_L2_DYN_PMV:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_L2_DYN_PMVf;
            break;

        case TRAP_Q_HASH_FULL:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_0r;
            field = LONGAN_HASH_FULLf;
            break;

        case TRAP_Q_ATK:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_ATK_TYPEf;
            break;

        case TRAP_Q_ACL_HIT:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_ACL_HITf;
            break;

        case TRAP_Q_MIR_HIT:
            reg   = LONGAN_QM_FLAG2CPUQID_CTRL_1r;
            field = LONGAN_MIR_HITf;
            break;

        case TRAP_Q_CAPWAP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_CAPWAPf;
            break;

        case TRAP_Q_CPU2CPU_TALK:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_CPU2CPU_TALKf;
            break;

        case TRAP_Q_L3_IPUC_RPF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_0r;
            field = LONGAN_L3_IPUC_RPFf;
            break;

        case TRAP_Q_L3_IPMC_RPF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_IPMC_RPFf;
            break;

        case TRAP_Q_L2_UC_MC_BDG_LUMIS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_L2_UC_MC_BRIDGE_LU_MISSf;
            break;

        case TRAP_Q_IP4_IP6_BDG_LUMIS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_1r;
            field = LONGAN_IP4_IP6_BRIDGE_LU_MISSf;
            break;

        case TRAP_Q_ROUTER_MAC_IF:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_ROUTER_MAC_IFf;
            break;

        case TRAP_Q_L3_IPUC_NON_IP_PKT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_L3_IPUC_NON_IP_PKTf;
            break;

        case TRAP_Q_ROUTE_IP_CHK:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_ROUTING_IP_CHKf;
            break;

        case TRAP_Q_L3_ROUTE_DIP_DMAC_MISMATCH:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_3r;
            field = LONGAN_L3_ROUTING_DIP_DMAC_MISMATCHf;
            break;

        case TRAP_Q_IP6_UC_MC_HOP_BY_HOP:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP6_UC_MC_HOP_BY_HOPf;
            break;

        case TRAP_Q_IP6_UC_MC_ROUTE_HDR:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP6_UC_MC_ROUTING_HDRf;
            break;

        case TRAP_Q_IP4_IP_OPT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP4_IP_OPTf;
            break;

        case TRAP_Q_IP4_IP6_MC_ROUTE_LUMIS:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_IP4_IP6_MC_ROUTING_LU_MISSf;
            break;

        case TRAP_Q_L3_IPUC_NULL_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_L3_IPUC_NULL_ROUTEf;
            break;

        case TRAP_Q_L3_IPUC_PBR_NULL_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_4r;
            field = LONGAN_L3_IPUC_PBR_NULL_ROUTEf;
            break;

        case TRAP_Q_L3_UC_HOST_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_UC_HOST_ROUTEf;
            break;

        case TRAP_Q_L3_UC_NET_ROUTE:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_UC_NET_ROUTEf;
            break;

        case TRAP_Q_L3_MC_BDG_ENTRY:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_MC_BRIDGE_ENTRYf;
            break;

        case TRAP_Q_L3_MC_ROUTE_ENTRY:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_L3_MC_ROUTE_ENTRYf;
            break;

        case TRAP_Q_ROUTE_EXCPT_NH_AGE_OUT_ACT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_5r;
            field = LONGAN_ROUTING_EXCPT_NH_AGE_OUT_ACTf;
            break;

        case TRAP_Q_IP4_IP6_ICMP_REDRT:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IP4_IP6_ICMP_REDRTf;
            break;

        case TRAP_Q_IPUC_MTU:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPUC_MTUf;
            break;

        case TRAP_Q_IPMC_MTU:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPMC_MTUf;
            break;

        case TRAP_Q_IPUC_TTL:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPUC_TTLf;
            break;

        case TRAP_Q_IPMC_TTL:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_6r;
            field = LONGAN_IPMC_TTLf;
            break;

        case TRAP_Q_NOTIFICATION:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_7r;
            field = LONGAN_L2_NTFYf;
            break;

        case TRAP_Q_NORMAL_FWD:
            reg   = LONGAN_QM_RSN2CPUQID_CTRL_8r;
            field = LONGAN_NORMAL_FWDf;
            break;

        default:
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_LOCK(unit);

    /*Set entry to CHIP*/
    value = qid & 0x1F;
    if((ret = reg_field_write(unit, reg, field, &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_mgmtFrameQueue_set */

/* Function Name:
 *      dal_longan_trap_mgmtFrameLearningEnable_get
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
 *      - MGMT_TYPE_EAPOL
 */
int32
dal_longan_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable)
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
        case MGMT_TYPE_PTP_UDP:
        case MGMT_TYPE_PTP_ETH2:
            regField = LONGAN_PTP_LRNf;
            break;
        case MGMT_TYPE_LLDP:
            regField = LONGAN_LLDP_LRNf;
            break;
        case MGMT_TYPE_EAPOL:
            regField = LONGAN_EAPOL_LRNf;
            break;
        default:
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_LOCK(unit);
    /* get entry from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_RMA_MGN_LRN_CTRLr, regField, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_mgmtFrameLearningEnable_set
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
 *      - MGMT_TYPE_EAPOL
 */
int32
dal_longan_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable)
{
    int32   ret = 0;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    switch (frameType)
    {
        case MGMT_TYPE_PTP:
        case MGMT_TYPE_PTP_UDP:
        case MGMT_TYPE_PTP_ETH2:
            regField = LONGAN_PTP_LRNf;
            break;
        case MGMT_TYPE_LLDP:
            regField = LONGAN_LLDP_LRNf;
            break;
        case MGMT_TYPE_EAPOL:
            regField = LONGAN_EAPOL_LRNf;
            break;
        default:
            return RT_ERR_RMA_MGMT_TYPE;
    }

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_RMA_MGN_LRN_CTRLr, regField, &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_portMgmtFrameAction_get
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
 *                                    	      9300
 *          - MGMT_TYPE_BPDU                (1110101)
 *          - MGMT_TYPE_PTP                 (1110100)
 *          - MGMT_TYPE_PTP_UDP             (1110100)
 *          - MGMT_TYPE_PTP_ETH2            (1110100)
 *          - MGMT_TYPE_LLDP                (1110101)
 *          - MGMT_TYPE_EAPOL               (1110101)
 *          - MGMT_GRATUITOUS_ARP           (1111000)
 *
 *          MGMT_TYPE_PTP is accepted by 9300 and 9310 only when BACKWARD-COMPATIBLE is configured.
 */
int32
dal_longan_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction)
{
    int32   ret = RT_ERR_FAILED;
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
        if((ret = reg_array_field_read(unit, LONGAN_RMA_PORT_BPDU_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP)
    {
        if((ret = reg_array_field_read(unit, LONGAN_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_UDP_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP_UDP)
    {
        if((ret = reg_array_field_read(unit, LONGAN_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_UDP_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP_ETH2)
    {
        if((ret = reg_array_field_read(unit, LONGAN_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ETH2_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_LLDP)
    {
        if((ret = reg_array_field_read(unit, LONGAN_RMA_PORT_LLDP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_EAPOL)
    {
        if((ret = reg_array_field_read(unit, LONGAN_RMA_PORT_EAPOL_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_GRATUITOUS_ARP)
    {
        if ((ret = reg_array_field_read(unit, LONGAN_SPCL_TRAP_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_GARP_ACTf, &value)) != RT_ERR_OK)
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
            if(frameType == MGMT_GRATUITOUS_ARP)
                *pAction = MGMT_ACTION_COPY2CPU;
            else
                *pAction = MGMT_ACTION_TRAP2MASTERCPU;
            break;
        case 4:
            *pAction = MGMT_ACTION_FLOOD_TO_ALL_PORT;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_longan_trap_portMgmtFrameAction_get */

/* Function Name:
 *      dal_longan_trap_portMgmtFrameAction_set
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
 *                                    	      9300
 *          - MGMT_TYPE_BPDU                (1110101)
 *          - MGMT_TYPE_PTP                 (1110100)
 *          - MGMT_TYPE_PTP_UDP             (1110100)
 *          - MGMT_TYPE_PTP_ETH2            (1110100)
 *          - MGMT_TYPE_LLDP                (1110101)
 *          - MGMT_TYPE_EAPOL               (1110101)
 *          - MGMT_GRATUITOUS_ARP           (1111000)
 *
 *          MGMT_TYPE_PTP is accepted by 9300 and 9310 only when BACKWARD-COMPATIBLE is configured.
 */
int32
dal_longan_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action)
{
    int32   ret = RT_ERR_FAILED;
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
    RT_PARAM_CHK((frameType == MGMT_TYPE_PTP_UDP) && (action == MGMT_ACTION_FLOOD_TO_ALL_PORT), RT_ERR_RMA_ACTION);
    RT_PARAM_CHK((frameType == MGMT_TYPE_PTP_ETH2) && (action == MGMT_ACTION_FLOOD_TO_ALL_PORT), RT_ERR_RMA_ACTION);
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
        case MGMT_ACTION_COPY2CPU:
        case MGMT_ACTION_TRAP2MASTERCPU:
            value = 3;
            break;
        case MGMT_ACTION_FLOOD_TO_ALL_PORT:
            value = 4;
            break;
        default:
            return RT_ERR_FAILED;
    }

    TRAP_SEM_LOCK(unit);

    if(frameType == MGMT_TYPE_BPDU)
    {
        if((ret = reg_array_field_write(unit, LONGAN_RMA_PORT_BPDU_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP)
    {
        if((ret = reg_array_field_write(unit, LONGAN_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_UDP_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
        if((ret = reg_array_field_write(unit, LONGAN_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ETH2_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP_UDP)
    {
        if((ret = reg_array_field_write(unit, LONGAN_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_UDP_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_PTP_ETH2)
    {
        if((ret = reg_array_field_write(unit, LONGAN_RMA_PORT_PTP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ETH2_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_LLDP)
    {
        if((ret = reg_array_field_write(unit, LONGAN_RMA_PORT_LLDP_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_TYPE_EAPOL)
    {
        if((ret = reg_array_field_write(unit, LONGAN_RMA_PORT_EAPOL_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ACTf, &value)) != RT_ERR_OK)
        {
            TRAP_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }
    else if(frameType == MGMT_GRATUITOUS_ARP)
    {
        if ((ret = reg_array_field_write(unit, LONGAN_SPCL_TRAP_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_GARP_ACTf, &value)) != RT_ERR_OK)
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
} /* end of dal_longan_trap_portMgmtFrameAction_set */

/* Function Name:
 *      dal_longan_trap_pktWithCFIAction_get
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
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_DROP
 */
int32
dal_longan_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pAction, RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, LONGAN_VLAN_CTRLr, LONGAN_ICFI_ACTf,
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
        case 3:
            *pAction = ACTION_TRAP2MASTERCPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_longan_trap_pktWithCFIAction_get */

/* Function Name:
 *      dal_longan_trap_pktWithCFIAction_set
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
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_DROP
 */
int32
dal_longan_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret = RT_ERR_FAILED;
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
        case ACTION_TRAP2MASTERCPU:
            value = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, LONGAN_VLAN_CTRLr, LONGAN_ICFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_pktWithCFIAction_set */

/* Function Name:
 *      dal_longan_trap_pktWithOuterCFIAction_get
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
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_DROP
 */
int32
dal_longan_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pAction, RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_read(unit, LONGAN_VLAN_CTRLr, LONGAN_OCFI_ACTf,
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
        case 3:
            *pAction = ACTION_TRAP2MASTERCPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_TRAP|MOD_DAL), "pAction=%d", *pAction);

    return RT_ERR_OK;
} /* end of dal_longan_trap_pktWithOuterCFIAction_get */

/* Function Name:
 *      dal_longan_trap_pktWithOuterCFIAction_set
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
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_DROP
 */
int32
dal_longan_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action)
{
    int32   ret = RT_ERR_FAILED;
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
        case ACTION_TRAP2MASTERCPU:
            value = 3;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TRAP_SEM_LOCK(unit);
    if ((ret = reg_field_write(unit, LONGAN_VLAN_CTRLr, LONGAN_OCFI_ACTf,
            &value)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_TRAP|MOD_DAL), "");
        return ret;
    }
    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_longan_trap_pktWithOuterCFIAction_set */

/* Function Name:
 *      dal_longan_trap_bpduFloodPortmask_get
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
dal_longan_trap_bpduFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    osal_memset(pflood_portmask, 0, sizeof(rtk_portmask_t));
    if ((ret = reg_field_read(unit, LONGAN_RMA_BPDU_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_bpduFloodPortmask_set
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
dal_longan_trap_bpduFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pflood_portmask) , RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_RMA_BPDU_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_eapolFloodPortmask_get
 * Description:
 *      Get EAPOL flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - eapol flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_eapolFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;


    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    osal_memset(pflood_portmask, 0, sizeof(rtk_portmask_t));
    if ((ret = reg_field_read(unit, LONGAN_RMA_EAPOL_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_eapolFloodPortmask_set
 * Description:
 *      Set EAPOL flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - EAPOL flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_eapolFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pflood_portmask) , RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_RMA_EAPOL_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_lldpFloodPortmask_get
 * Description:
 *      Get LLDP flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - LLDP flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_lldpFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    osal_memset(pflood_portmask, 0, sizeof(rtk_portmask_t));
    if ((ret = reg_field_read(unit, LONGAN_RMA_LLDP_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_lldpFloodPortmask_set
 * Description:
 *      Set LLDP flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - LLDP flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_lldpFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pflood_portmask) , RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_RMA_LLDP_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_userDefineFloodPortmask_get
 * Description:
 *      Get user define flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - user define flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_userDefineFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    osal_memset(pflood_portmask, 0, sizeof(rtk_portmask_t));
    if((ret = reg_field_read(unit, LONGAN_RMA_USR_DEF_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_DAL|MOD_TRAP, "");
        TRAP_SEM_UNLOCK(unit);
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_userDefineFloodPortmask_set
 * Description:
 *      Set user define flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - user define flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_userDefineFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pflood_portmask) , RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    if((ret = reg_field_write(unit, LONGAN_RMA_USR_DEF_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, MOD_DAL|MOD_TRAP, "");
        TRAP_SEM_UNLOCK(unit);
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_rmaFloodPortmask_get
 * Description:
 *      Get RMA flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - rma flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_rmaFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d",unit);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);

    TRAP_SEM_LOCK(unit);

    osal_memset(pflood_portmask, 0, sizeof(rtk_portmask_t));
    if ((ret = reg_field_read(unit, LONGAN_RMA_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_eapolFloodPortmask_set
 * Description:
 *      Set RMA flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - rma  flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_rmaFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask)
{
    int32 ret = RT_ERR_FAILED;

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pflood_portmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PMSK_EXIST(unit, pflood_portmask) , RT_ERR_PORT_MASK);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d, portmask=0x%X", unit, (*pflood_portmask).bits[0]);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_RMA_FLD_PMSKr, LONGAN_PMSKf, &pflood_portmask->bits[0])) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_rmaCancelMirror_get
 * Description:
 *      Get RMA Cancel mirror configuration
 * Input:
 *      unit            - unit id
 * Output:
 *      pEnable        - pointer to enable status of RMA Cancel Mirror Setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
int32
dal_longan_trap_rmaCancelMirror_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_RMA_MIRROR_CTRLr, LONGAN_MIRROR_ACTf, pEnable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_trap_rmaCancelMirror_set
 * Description:
 *      Set enable status of RMA Cancel mirror
 * Input:
 *      unit            - unit id
 *      enable        - enable status of  RMA Cancel Mirror
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8388, 9310
 * Note:
 *      None
 */
int32
dal_longan_trap_rmaCancelMirror_set(uint32 unit, rtk_enable_t enable)
{
    int32 ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TRAP), "unit=%d,enable=%d", unit,enable);

    /* check Init status */
    RT_INIT_CHK(trap_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    TRAP_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, LONGAN_RMA_MIRROR_CTRLr, LONGAN_MIRROR_ACTf, &enable)) != RT_ERR_OK)
    {
        TRAP_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    TRAP_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_longan_trap_init_config
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
_dal_longan_trap_init_config(uint32 unit)
{
    int32   ret = RT_ERR_FAILED;
    rtk_mac_t rma_slow_proto = {{0x01,0x80,0xC2,0x00,0x00,0x02}};

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        if ((ret = dal_longan_trap_rmaAction_set(unit, &rma_slow_proto, RTK_DEFAULT_TRAP_RMA_SP_MCAST_ACTION)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of _dal_LONGAN_trap_init_config */


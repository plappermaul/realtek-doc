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
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature : (1) Include chip-supported conditions for flow control on/off
 *           (2) Get/set the threshold parameters for the flow control on/off
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
#include <dal/mango/dal_mango_flowctrl.h>
#include <dal/dal_waMon.h>
#include <rtk/default.h>
#include <rtk/flowctrl.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               flowctrl_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         flowctrl_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* flowctrl semaphore handling */
#define FLOWCTRL_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(flowctrl_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_FLOWCTRL), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define FLOWCTRL_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(flowctrl_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_FLOWCTRL), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_flowctrlMapper_init
 * Description:
 *      Hook flowctrl module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook flowctrl module before calling any flowctrl APIs.
 */
int32
dal_mango_flowctrlMapper_init(dal_mapper_t *pMapper)
{
    pMapper->flowctrl_init = dal_mango_flowctrl_init;
    pMapper->flowctrl_portPauseOnAction_get = dal_mango_flowctrl_portPauseOnAction_get;
    pMapper->flowctrl_portPauseOnAction_set = dal_mango_flowctrl_portPauseOnAction_set;
    pMapper->flowctrl_portPauseOnAllowedPageNum_get = dal_mango_flowctrl_portPauseOnAllowedPageNum_get;
    pMapper->flowctrl_portPauseOnAllowedPageNum_set = dal_mango_flowctrl_portPauseOnAllowedPageNum_set;
    pMapper->flowctrl_igrGuarEnable_get = dal_mango_flowctrl_igrGuarEnable_get;
    pMapper->flowctrl_igrGuarEnable_set = dal_mango_flowctrl_igrGuarEnable_set;
    pMapper->flowctrl_igrSystemPauseThresh_get = dal_mango_flowctrl_igrSystemPauseThresh_get;
    pMapper->flowctrl_igrSystemPauseThresh_set = dal_mango_flowctrl_igrSystemPauseThresh_set;
    pMapper->flowctrl_igrJumboSystemPauseThresh_get = dal_mango_flowctrl_igrJumboSystemPauseThresh_get;
    pMapper->flowctrl_igrJumboSystemPauseThresh_set = dal_mango_flowctrl_igrJumboSystemPauseThresh_set;
    pMapper->flowctrl_igrPauseThreshGroup_get = dal_mango_flowctrl_igrPauseThreshGroup_get;
    pMapper->flowctrl_igrPauseThreshGroup_set = dal_mango_flowctrl_igrPauseThreshGroup_set;
    pMapper->flowctrl_portIgrPortThreshGroupSel_get = dal_mango_flowctrl_portIgrPortThreshGroupSel_get;
    pMapper->flowctrl_portIgrPortThreshGroupSel_set = dal_mango_flowctrl_portIgrPortThreshGroupSel_set;
    pMapper->flowctrl_igrSystemCongestThresh_get = dal_mango_flowctrl_igrSystemCongestThresh_get;
    pMapper->flowctrl_igrSystemCongestThresh_set = dal_mango_flowctrl_igrSystemCongestThresh_set;
    pMapper->flowctrl_igrJumboSystemCongestThresh_get = dal_mango_flowctrl_igrJumboSystemCongestThresh_get;
    pMapper->flowctrl_igrJumboSystemCongestThresh_set = dal_mango_flowctrl_igrJumboSystemCongestThresh_set;
    pMapper->flowctrl_igrCongestThreshGroup_get = dal_mango_flowctrl_igrCongestThreshGroup_get;
    pMapper->flowctrl_igrCongestThreshGroup_set = dal_mango_flowctrl_igrCongestThreshGroup_set;
    pMapper->flowctrl_jumboModeStatus_get = dal_mango_flowctrl_jumboModeStatus_get;
    pMapper->flowctrl_jumboModeEnable_get = dal_mango_flowctrl_jumboModeEnable_get;
    pMapper->flowctrl_jumboModeEnable_set = dal_mango_flowctrl_jumboModeEnable_set;
    pMapper->flowctrl_jumboModeLength_get = dal_mango_flowctrl_jumboModeLength_get;
    pMapper->flowctrl_jumboModeLength_set = dal_mango_flowctrl_jumboModeLength_set;
    pMapper->flowctrl_egrSystemUtilThresh_get = dal_mango_flowctrl_egrSystemUtilThresh_get;
    pMapper->flowctrl_egrSystemUtilThresh_set = dal_mango_flowctrl_egrSystemUtilThresh_set;
    pMapper->flowctrl_egrSystemDropThresh_get = dal_mango_flowctrl_egrSystemDropThresh_get;
    pMapper->flowctrl_egrSystemDropThresh_set = dal_mango_flowctrl_egrSystemDropThresh_set;
    pMapper->flowctrl_egrQueueDropThresh_get = dal_mango_flowctrl_egrQueueDropThresh_get;
    pMapper->flowctrl_egrQueueDropThresh_set = dal_mango_flowctrl_egrQueueDropThresh_set;
    pMapper->flowctrl_egrCpuQueueDropThresh_get = dal_mango_flowctrl_egrCpuQueueDropThresh_get;
    pMapper->flowctrl_egrCpuQueueDropThresh_set = dal_mango_flowctrl_egrCpuQueueDropThresh_set;
    pMapper->flowctrl_portEgrDropRefCongestEnable_get = dal_mango_flowctrl_portEgrDropRefCongestEnable_get;
    pMapper->flowctrl_portEgrDropRefCongestEnable_set = dal_mango_flowctrl_portEgrDropRefCongestEnable_set;
    pMapper->flowctrl_egrQueueDropThreshGroup_get = dal_mango_flowctrl_egrQueueDropThreshGroup_get;
    pMapper->flowctrl_egrQueueDropThreshGroup_set = dal_mango_flowctrl_egrQueueDropThreshGroup_set;
    pMapper->flowctrl_portEgrDropThreshGroupSel_get = dal_mango_flowctrl_portEgrDropThreshGroupSel_get;
    pMapper->flowctrl_portEgrDropThreshGroupSel_set = dal_mango_flowctrl_portEgrDropThreshGroupSel_set;
    pMapper->flowctrl_portEgrQueueDropForceEnable_get = dal_mango_flowctrl_portEgrQueueDropForceEnable_get;
    pMapper->flowctrl_portEgrQueueDropForceEnable_set = dal_mango_flowctrl_portEgrQueueDropForceEnable_set;
    pMapper->flowctrl_igrQueueDropThresh_get = dal_mango_flowctrl_igrQueueDropThresh_get;
    pMapper->flowctrl_igrQueueDropThresh_set = dal_mango_flowctrl_igrQueueDropThresh_set;
    pMapper->flowctrl_portHolTrafficDropEnable_get = dal_mango_flowctrl_portHolTrafficDropEnable_get;
    pMapper->flowctrl_portHolTrafficDropEnable_set = dal_mango_flowctrl_portHolTrafficDropEnable_set;
    pMapper->flowctrl_holTrafficTypeDropEnable_get = dal_mango_flowctrl_holTrafficTypeDropEnable_get;
    pMapper->flowctrl_holTrafficTypeDropEnable_set = dal_mango_flowctrl_holTrafficTypeDropEnable_set;
    pMapper->flowctrl_tagPauseEnable_get = dal_mango_flowctrl_tagPauseEnable_get;
    pMapper->flowctrl_tagPauseEnable_set = dal_mango_flowctrl_tagPauseEnable_set;
    pMapper->flowctrl_halfConsecutiveRetryEnable_get = dal_mango_flowctrl_halfConsecutiveRetryEnable_get;
    pMapper->flowctrl_halfConsecutiveRetryEnable_set = dal_mango_flowctrl_halfConsecutiveRetryEnable_set;


    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_flowctrl_init
 * Description:
 *      Initialize flowctrl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize flowctrl module before calling any flowctrl APIs.
 */
int32
dal_mango_flowctrl_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(flowctrl_init[unit]);
    flowctrl_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    flowctrl_sem[unit] = osal_sem_mutex_create();
    if (0 == flowctrl_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_FLOWCTRL), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    flowctrl_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Module Name    : Flow Control               */
/* Sub-module Name: Flow control configuration */

/* Function Name:
 *      dal_mango_flowctrl_portPauseOnAction_get
 * Description:
 *      Get action for the scenario that packet keeps coming in after pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to action of packet receive
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE: after Pause ON frame is sent, incoming packets always can be received.
 *      - PAUSE_ON_DROP: after Pause ON frame is sent, incoming packets can be received up to one specified number.
 */
int32
dal_mango_flowctrl_portPauseOnAction_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    *pAction)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pAction=%x"
            , unit, port, pAction);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pAction = PAUSE_ON_RECEIVE;
            break;

        case 1:
            *pAction = PAUSE_ON_DROP;
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_portPauseOnAction_set
 * Description:
 *      Set action for the scenario that packet keeps coming in after pause on frame is sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action of packet receive
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
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE: after Pause ON frame is sent, incoming packets always can be received.
 *      - PAUSE_ON_DROP: after Pause ON frame is sent, incoming packets can be received up to one specified number.
 *        The specified number can be configured by rtk_flowctrl_pauseOnAllowedPageNum_set,
 *        rtk_flowctrl_pauseOnAllowedPktNum_set, rtk_flowctrl_pauseOnAllowedPktLen_set APIs
 */
int32
dal_mango_flowctrl_portPauseOnAction_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    action)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, action=%d"
            , unit, port, action);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((action >= PAUSE_ON_END), RT_ERR_INPUT);

    switch (action)
    {
        case PAUSE_ON_RECEIVE:
            value = 0;
            break;

        case PAUSE_ON_DROP:
            value = 1;
            break;

        default:
            return RT_ERR_INPUT;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_ACT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ACTf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_portPauseOnAllowedPageNum_get
 * Description:
 *      Get number of allowed page when pause on frame sent.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPageNum - pointer to number of received page
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The valid range of pageNum is 0 ~ 0xFF, mean allowed receive page number after pause on frame sent.
 */
int32
dal_mango_flowctrl_portPauseOnAllowedPageNum_get(uint32 unit, rtk_port_t port, uint32 *pPageNum)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pPageNum=%x"
            , unit, port, pPageNum);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPageNum), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_ACT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ALLOW_PAGE_CNTf, pPageNum)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_portPauseOnAllowedPageNum_set
 * Description:
 *      Set number of allowed page when pause on frame sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pageNum - number of received page
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The valid range of pageNum is 0 ~ 0xFF, mean allowed receive page number after pause on frame sent.
 */
int32
dal_mango_flowctrl_portPauseOnAllowedPageNum_set(uint32 unit, rtk_port_t port, uint32 pageNum)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pageNum=%d"
            , unit, port, pageNum);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pageNum > HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_ACT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ALLOW_PAGE_CNTf, &pageNum)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/*
 * Flow Control ON
 */

/* Function Name:
 *      dal_mango_flowctrl_igrGuarEnable_get
 * Description:
 *      Get state of ingress guarantee page for all ports
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of guarantee page
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
dal_mango_flowctrl_igrGuarEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_CTRLr, MANGO_GUAR_PAGE_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrGuarEnable_set
 * Description:
 *      Set state of ingress guarantee page for all ports
 * Input:
 *      unit    - unit id
 *      enable  - enable status of guarantee page
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
dal_mango_flowctrl_igrGuarEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, enable=%d\n", unit, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_FC_CTRLr, MANGO_GUAR_PAGE_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrSystemPauseThresh_get
 * Description:
 *      Get ingress system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the system used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_GLB_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_GLB_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_GLB_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_GLB_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrSystemPauseThresh_set
 * Description:
 *      Set ingress system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the system used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d \
           highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x", unit, pThresh->highOn, pThresh->highOff,
           pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_FC_GLB_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_GLB_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_GLB_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_GLB_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrJumboSystemPauseThresh_get
 * Description:
 *      Get ingress system used page high/low threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the system used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_igrJumboSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_flowctrl_igrJumboSystemPauseThresh_set
 * Description:
 *      Set ingress system used page high/low threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the system used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_igrJumboSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d \
           highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x", unit, pThresh->highOn, pThresh->highOff,
           pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrPauseThreshGroup_get
 * Description:
 *      Get ingress port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 */
int32
dal_mango_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_GUAR_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_THRf, &(pThresh->guar))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x, guar=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff, pThresh->guar);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrPauseThreshGroup_set
 * Description:
 *      Set ingress port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 */
int32
dal_mango_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->guar > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x, guar=0x%x",
           pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff, pThresh->guar);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_GUAR_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_THRf, &(pThresh->guar))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_portIgrPortThreshGroupSel_get
 * Description:
 *      Get ingress port used page pause threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
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
dal_mango_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pGrp_idx), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE,
                            MANGO_IDXf, pGrp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_portIgrPortThreshGroupSel_set
 * Description:
 *      Set ingress port used page pause threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
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
dal_mango_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, grp_idx=%d", unit, port, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE,
                            MANGO_IDXf, &grp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Module Name    : Flow Control              */
/* Sub-module Name: Egress drop configuration */

/*
 * Flow Control OFF
 */

/* Function Name:
 *      dal_mango_flowctrl_igrSystemCongestThresh_get
 * Description:
 *      Get system used page high/low drop threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the public used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_GLB_FCOFF_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_GLB_FCOFF_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_GLB_FCOFF_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_GLB_FCOFF_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_flowctrl_igrSystemCongestThresh_set
 * Description:
 *      Set system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the public used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d \
           highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x", unit, pThresh->highOn, pThresh->highOff,
           pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_FC_GLB_FCOFF_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_GLB_FCOFF_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_GLB_FCOFF_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_GLB_FCOFF_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrJumboSystemCongestThresh_get
 * Description:
 *      Get system used page high/low drop threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the public used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_igrJumboSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_FCOFF_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_FCOFF_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_FCOFF_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_FCOFF_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrJumboSystemCongestThresh_set
 * Description:
 *      Set system used page high/low threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the public used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x1FFF.
 */
int32
dal_mango_flowctrl_igrJumboSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d \
           highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x", unit, pThresh->highOn, pThresh->highOff,
           pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_HI_THRr, MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_HI_THRr, MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_LO_THRr, MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_FCOFF_LO_THRr, MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrCongestThreshGroup_get
 * Description:
 *      Get used page drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - the index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (2) Each port can map to a threshold group, refer to dal_mango_flowctrl_portIgrPortThreshGroupSel_set.
 */
int32
dal_mango_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_flowctrl_igrCongestThreshGroup_set
 * Description:
 *      Set used page drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - the index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (2) Each port can map to a threshold group, refer to dal_mango_flowctrl_portIgrPortThreshGroupSel_set.
 */
int32
dal_mango_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x",
           pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_ONf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MANGO_OFFf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_jumboModeStatus_get
 * Description:
 *      Get jumbo mode current status
 * Input:
 *      unit    - unit id
 * Output:
 *      pStatus - pointer to jumbo mode status
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
dal_mango_flowctrl_jumboModeStatus_get(uint32 unit, uint32 *pStatus)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_THR_ADJUSTr, MANGO_STSf, pStatus)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "pStatus=%d", pStatus);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_jumboModeEnable_get
 * Description:
 *      Get jumbo mode ability
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of jumbo mode
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
dal_mango_flowctrl_jumboModeEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_THR_ADJUSTr, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_flowctrl_jumboModeEnable_set
 * Description:
 *      Set enable status of jumbo mode.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of jumbo mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_flowctrl_jumboModeEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, enable=%d\n", unit, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_THR_ADJUSTr, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_jumboModeLength_get
 * Description:
 *      Get jumbo mode packet length boundary
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to packet length boundary of jumbo mode
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
dal_mango_flowctrl_jumboModeLength_get(uint32 unit, uint32 *pLength)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLength), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_JUMBO_THR_ADJUSTr, MANGO_PKT_LENf, pLength)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "pLength=%d", pLength);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_jumboModeLength_set
 * Description:
 *      Set packet length boundary of jumbo mode.
 * Input:
 *      unit    - unit id
 *      length  - packet length boundary of jumbo mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Note:
 *      None
 */
extern int32
dal_mango_flowctrl_jumboModeLength_set(uint32 unit, uint32 length)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, length=%d\n", unit, length);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((length > HAL_MAX_ACCEPT_FRAME_LEN(unit)), RT_ERR_OUT_OF_RANGE);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_FC_JUMBO_THR_ADJUSTr, MANGO_PKT_LENf, &length)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}
/* Function Name:
 *      dal_mango_flowctrl_egrSystemUtilThresh_get
 * Description:
 *      Get egress system utilization threshold for the specified unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the utilization threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      System only uses highOn threshold of rtk_flowctrl_thresh_t.
 */
int32
dal_mango_flowctrl_egrSystemUtilThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_GLB_SYS_UTIL_THRr, MANGO_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x", pThresh->highOn);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrSystemUtilThresh_set
 * Description:
 *      Set egress system utilization threshold for the specified unit
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the utilization threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      System only uses highOn threshold of rtk_flowctrl_thresh_t.
 */
int32
dal_mango_flowctrl_egrSystemUtilThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32 ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x", pThresh->highOn);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_FC_GLB_SYS_UTIL_THRr, MANGO_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrSystemDropThresh_get
 * Description:
 *      Get egress system drop threshold for the specified unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) System has only one threshold, mean high and low are equal.
 */
int32
dal_mango_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_GLB_DROP_THRr, MANGO_DROP_ALLf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    pThresh->low = pThresh->high;
    FLOWCTRL_SEM_UNLOCK(unit);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrSystemDropThresh_set
 * Description:
 *      Set egress system drop threshold for the specified unit
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) System has only one threshold, and only uses high threshold to set.
 *      (3) The valid range is 0 ~ 0x1FFF.
 */
int32
dal_mango_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_FC_GLB_DROP_THRr, MANGO_DROP_ALLf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrQueueDropThresh_get
 * Description:
 *      Get egress global drop threshold for the egress queue
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the global drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in the queue of all ports.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_egrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    return dal_mango_flowctrl_egrQueueDropThreshGroup_get(unit, 0, queue, pThresh);
}

/* Function Name:
 *      dal_mango_flowctrl_egrQueueDropThresh_set
 * Description:
 *      Set egress gloabl drop threshold for the egress queue
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0x1FFF
 */
int32
dal_mango_flowctrl_egrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;
    uint32 grpIdx;

    for (grpIdx = 0; grpIdx <= HAL_THRESH_OF_EGR_QUEUE_DROP_GROUP_IDX_MAX(unit); grpIdx++)
    {
        if ((ret = dal_mango_flowctrl_egrQueueDropThreshGroup_set(unit, grpIdx, queue, pThresh)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrCpuQueueDropThresh_get
 * Description:
 *      Get CPU port egress queue drop threshold
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the global drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) CPU port has dedicated egress queue drop threshold configuration different from normal port.
 */
int32
dal_mango_flowctrl_egrCpuQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, queue=%d",
           unit, queue);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        MANGO_ONf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        MANGO_OFFf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "queue=%d, high=0x%x, low=0x%x", queue,
           pThresh->high, pThresh->low);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrCpuQueueDropThresh_set
 * Description:
 *      Set egress gloabl drop threshold for the egress queue of CPU port
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) CPU port has dedicated egress queue drop threshold configuration different from normal port.
 */
int32
dal_mango_flowctrl_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "queue=%d, high=0x%x, low=0x%x", queue, pThresh->high, pThresh->low);

    FLOWCTRL_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        MANGO_ONf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_CPU_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        MANGO_OFFf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

#if defined(CONFIG_SFLOW_PG_THREAD)
    _dal_waMonPgMon_egrCpuQueueDropThresh_set(unit, queue, pThresh);
#endif

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrPortDropRefCongestEnable_get
 * Description:
 *      Get enable status of refering source port congest status for egress drop
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of refering source port congest status
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
dal_mango_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d"
            , unit, port);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_REF_RXCNGSTf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

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
}

/* Function Name:
 *      dal_mango_flowctrl_portEgrDropRefCongestEnable_set
 * Description:
 *      Set enable status of refering source port congest status for egress drop
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of refering source port congest status
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
dal_mango_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%d"
            , unit, port, enable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

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

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_EGR_DROP_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_REF_RXCNGSTf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrQueueDropThreshGroup_get
 * Description:
 *      Get egress queue drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portEgrDropThreshGroupSel_set.
 */
int32
dal_mango_flowctrl_egrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d, queue=%d",
           unit, grp_idx, queue);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_EGR_QUEUE_DROP_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_FC_Q_EGR_DROP_THRr, queue, grp_idx,
                        MANGO_ONf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_FC_Q_EGR_DROP_THRr, queue, grp_idx,
                        MANGO_OFFf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "grp_idx=%d, queue=%d, high=0x%x, low=0x%x", grp_idx, queue,
           pThresh->high, pThresh->low);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_egrQueueDropThreshGroup_set
 * Description:
 *      Set egress queue drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portEgrDropThreshGroupSel_set.
 */
int32
dal_mango_flowctrl_egrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_EGR_QUEUE_DROP_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "grp_idx=%d, queue=%d, high=0x%x, low=0x%x", grp_idx, queue,
           pThresh->high, pThresh->low);

    FLOWCTRL_SEM_LOCK(unit);

    if ((ret = reg_array_field_write(unit, MANGO_FC_Q_EGR_DROP_THRr, queue, grp_idx,
                        MANGO_ONf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_FC_Q_EGR_DROP_THRr, queue, grp_idx,
                        MANGO_OFFf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_portEgrDropThreshGroupSel_get
 * Description:
 *      Get egress port&queue used page drop threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
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
dal_mango_flowctrl_portEgrDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pGrp_idx), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_EGR_DROP_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE,
                            MANGO_IDXf, pGrp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portEgrDropThreshGroupSel_get */

/* Function Name:
 *      dal_mango_flowctrl_portEgrDropThreshGroupSel_set
 * Description:
 *      Set egress port&queue used page drop threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
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
dal_mango_flowctrl_portEgrDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, grp_idx=%d", unit, port, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_EGR_QUEUE_DROP_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_EGR_DROP_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE,
                            MANGO_IDXf, &grp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portEgrDropThreshGroupSel_get */


/* Function Name:
 *      dal_mango_flowctrl_portEgrQueueDropForceEnable_get
 * Description:
 *      Get enable status of egress queue force drop.
 * Input:
 *      unit    - unit id
 *      port    - queue id
 *      queue   - queue id
 * Output:
 *      pEnable - pointer to enable status of egress queue force drop.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_mango_flowctrl_portEgrQueueDropForceEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value, reg;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, queue=%d"
            , unit, port, queue);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (HWP_IS_CPU_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_FC_CPU_Q_EGR_FORCE_DROP_CTRLr;
    }
    else if (HWP_UPLINK_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET1r;
    }
    else
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET0r;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* read value from CHIP*/
    if (HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_read(unit, reg, REG_ARRAY_INDEX_NONE, queue, MANGO_ENf, &value)) != RT_ERR_OK)
        {
            FLOWCTRL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_read(unit, reg, port, queue, MANGO_ENf, &value)) != RT_ERR_OK)
        {
            FLOWCTRL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

    FLOWCTRL_SEM_UNLOCK(unit);

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
}

/* Function Name:
 *      dal_mango_flowctrl_portEgrQueueDropForceEnable_set
 * Description:
 *      Set enable status of egress queue force drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      queue  - queue id
 *      enable - enable status of egress queue force drop.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *     None
 */
int32
dal_mango_flowctrl_portEgrQueueDropForceEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    int32   ret;
    uint32  value, reg;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, queue=%d"
            , unit, port, queue);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (HWP_IS_CPU_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_FC_CPU_Q_EGR_FORCE_DROP_CTRLr;
    }
    else if (HWP_UPLINK_PORT(unit, port))
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET1r;
    }
    else
    {
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
        reg = MANGO_FC_PORT_Q_EGR_FORCE_DROP_CTRL_SET0r;
    }

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

    FLOWCTRL_SEM_LOCK(unit);

    /* read value from CHIP*/
    if (HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, reg, REG_ARRAY_INDEX_NONE, queue, MANGO_ENf, &value)) != RT_ERR_OK)
        {
            FLOWCTRL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_array_field_write(unit, reg, port, queue, MANGO_ENf, &value)) != RT_ERR_OK)
        {
            FLOWCTRL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_igrQueueDropThresh_get
 * Description:
 *      Get ingress queue drop threshold.
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the global drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_mango_flowctrl_igrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, queue=%d", unit, queue);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE,
                            queue, MANGO_HIGHf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE,
                            queue, MANGO_LOWf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    return RT_ERR_OK;
} /* end of dal_mango_flowctrl_igrQueueDropThresh_get */

/* Function Name:
 *      dal_mango_flowctrl_igrQueueDropThresh_set
 * Description:
 *      Set ingress queue drop threshold.
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
int32
dal_mango_flowctrl_igrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, queue=%d, high=0x%x, low=0x%x", unit, queue, pThresh->high, pThresh->low);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);

    FLOWCTRL_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE,
                            queue, MANGO_HIGHf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_Q_DROP_THRr, REG_ARRAY_INDEX_NONE,
                            queue, MANGO_LOWf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_flowctrl_igrQueueDropThresh_set */

/* Function Name:
 *      dal_mango_flowctrl_portHolTrafficDropEnable_get
 * Description:
 *      Get dropping ability for dropping flooding traffic when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Per ingress port can enable the drop function to drop flooding traffic.
 *      (2) The function takes effect only if the flow control of ingress port is enabled.
 *      (3) Refer to rtk_flowctrl_holTrafficTypeDropEnable_set for dropping specific traffic type.
 */
int32
dal_mango_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d"
            , unit, port);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_FC_PORT_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MANGO_HOL_PRVNT_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

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
}

/* Function Name:
 *      dal_mango_flowctrl_portHolTrafficDropEnable_set
 * Description:
 *      Set dropping ability for dropping flooding traffic when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Enable the function to prevent HOL by flooding traffic when flow control is enabled.
 *      (2) Per ingress port can enable the drop function to drop flooding traffic at the congested
 *          egress port.
 *      (3) Refer to rtk_flowctrl_holTrafficTypeDropEnable_set for dropping specific traffic type.
 */
int32
dal_mango_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%d"
            , unit, port, enable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

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

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_FC_PORT_EGR_DROP_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_HOL_PRVNT_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_holTrafficTypeDropEnable_get
 * Description:
 *      Get dropping ability for specific traffic type when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      type    - traffic type
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
int32
dal_mango_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;
    uint32 filed;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, type=%d"
            , unit, type);

    /* parameter check */
    RT_PARAM_CHK((type >= HOL_TRAFFIC_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(type)
    {
        case HOL_TRAFFIC_TYPE_UNKN_UC:
            filed = MANGO_UNKN_UC_ENf;
            break;
        case HOL_TRAFFIC_TYPE_L2_MC:
            filed = MANGO_L2_MC_ENf;
            break;
        case HOL_TRAFFIC_TYPE_IP_MC:
            filed = MANGO_IP_MC_ENf;
            break;
        case HOL_TRAFFIC_TYPE_BC:
            filed = MANGO_BC_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_FC_HOL_PRVNT_CTRLr, filed, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

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
}
/* Function Name:
 *      dal_mango_flowctrl_holTrafficTypeDropEnable_set
 * Description:
 *      Set dropping ability for specific traffic type when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      type    - traffic type
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
int32
dal_mango_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;
    uint32 filed;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, type=%d, enable=%d"
            , unit, type, enable);

    /* parameter check */
    RT_PARAM_CHK((type >= HOL_TRAFFIC_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch(type)
    {
        case HOL_TRAFFIC_TYPE_UNKN_UC:
            filed = MANGO_UNKN_UC_ENf;
            break;
        case HOL_TRAFFIC_TYPE_L2_MC:
            filed = MANGO_L2_MC_ENf;
            break;
        case HOL_TRAFFIC_TYPE_IP_MC:
            filed = MANGO_IP_MC_ENf;
            break;
        case HOL_TRAFFIC_TYPE_BC:
            filed = MANGO_BC_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

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

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_field_write(unit, MANGO_FC_HOL_PRVNT_CTRLr, filed, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_flowctrl_tagPauseEnable_get
 * Description:
 *      Get forwarding state of the pause frame with tag.
 * Input:
 *      unit   - unit id
 * Output:
 *      pEnable - forwarding state of the pause frame with tag.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Forwarding state is as following
 *      - ENABLED: forward the pause frame with tag
 *      - DISABLED: drop the pause fram with tag
 */
int32
dal_mango_flowctrl_tagPauseEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    FLOWCTRL_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
            MANGO_FWD_PAUSE_TAG_ENf, &val)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    if (0 == val)
    {
        *pEnable = DISABLED;
    }
    else
    {
        *pEnable = ENABLED;
    }

    return RT_ERR_OK;
}   /* end of dal_mango_flowctrl_tagPauseEnable_get */

/* Function Name:
 *      dal_mango_flowctrl_tagPauseEnable_set
 * Description:
 *      Set forwarding state of the pause frame with tag.
 * Input:
 *      unit   - unit id
 *      action - forwarding state of the pause frame with tag.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Forwarding state is as following
 *      - ENABLED: forward the pause frame with tag
 *      - DISABLED: drop the pause fram with tag
 */
int32
dal_mango_flowctrl_tagPauseEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d,enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    val = (enable == DISABLED) ? 0 : 1;

    /* function body */
    FLOWCTRL_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MANGO_MAC_L2_GLOBAL_CTRL1r,
            MANGO_FWD_PAUSE_TAG_ENf, &val)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}   /* end of dal_mango_flowctrl_tagPauseEnable_set */

/* Function Name:
 *      dal_mango_flowctrl_halfConsecutiveRetryEnable_get
 * Description:
 *      Get half mode collision consecutive retry transmit state.
 * Input:
 *      unit   - unit id
 *      pEnable - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      consecutive retry transmit state
 *      - ENABLED: no packet loss, it will retry continue after 16 times collision
 *      - DISABLED: consecutive collision 16 times will drop and the excessive collison will count
 */
int32
dal_mango_flowctrl_halfConsecutiveRetryEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    if((ret = reg_field_read(unit, MANGO_MAC_GLB_CTRLr, MANGO_IOL_MAX_RETRY_ENf,&val)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    *pEnable = (val == 0) ? ENABLED : DISABLED;

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_mango_flowctrl_halfConsecutiveRetryEnable_set
 * Description:
 *      Set half mode collision consecutive retry transmit state.
 * Input:
 *      unit   - unit id
 *      enable - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      consecutive retry transmit state
 *      - ENABLED: no packet loss, it will retry continue after 16 times collision
 *      - DISABLED: consecutive collision 16 times will drop and the excessive collison will count
 */
int32
dal_mango_flowctrl_halfConsecutiveRetryEnable_set(uint32 unit, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    FLOWCTRL_SEM_LOCK(unit);

    val = (enable == DISABLED) ? 1 : 0;
    if((ret = reg_field_write(unit, MANGO_MAC_GLB_CTRLr, MANGO_IOL_MAX_RETRY_ENf, &val)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

int32
_dal_mango_flowctrl_portEgrPktBufCntr_get(uint32 unit, rtk_port_t port, uint32 *pCntr, uint32 *pMaxCntr)
{
    int32 ret = RT_ERR_FAILED;
    out_q_entry_t outQ;
    uint32 value = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMaxCntr), RT_ERR_NULL_POINTER);

    osal_memset(&outQ, 0, sizeof(outQ));


    if (HWP_IS_CPU_PORT(unit, port))
    {
        FLOWCTRL_SEM_LOCK(unit);
        if((ret = reg_read(unit, MANGO_FC_CPU_EGR_PAGE_CNTr, &value)) != RT_ERR_OK)
        {
            FLOWCTRL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
        FLOWCTRL_SEM_UNLOCK(unit);

        *pCntr = (value & 0xFFFF);
        *pMaxCntr = (value >> 16) & 0xFFFF;
    }
    else
    {
        FLOWCTRL_SEM_LOCK(unit);
        if((ret = table_read(unit, MANGO_OUT_Qt, port, (uint32 *) &outQ)) != RT_ERR_OK)
        {
            FLOWCTRL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
        FLOWCTRL_SEM_UNLOCK(unit);
        if((ret = table_field_get(unit, MANGO_OUT_Qt, MANGO_OUT_Q_PORT_USED_PAGE_CNTtf, pCntr, (uint32 *) &outQ)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
        if((ret = table_field_get(unit, MANGO_OUT_Qt, MANGO_OUT_Q_PORT_MAX_USED_PAGE_CNTtf,  pMaxCntr, (uint32 *) &outQ)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}



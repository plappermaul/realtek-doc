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
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_flowctrl.h>
#include <dal/dal_mgmt.h>
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
#define GAP_MAX 0x1f


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
 * Function Declaration
 */
/* Function Name:
 *      dal_maple_flowctrlMapper_init
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
dal_maple_flowctrlMapper_init(dal_mapper_t *pMapper)
{
    pMapper->flowctrl_init = dal_maple_flowctrl_init;
    pMapper->flowctrl_portPauseOnAction_get = dal_maple_flowctrl_portPauseOnAction_get;
    pMapper->flowctrl_portPauseOnAction_set = dal_maple_flowctrl_portPauseOnAction_set;
    pMapper->flowctrl_portPauseOnAllowedPageNum_get = dal_maple_flowctrl_portPauseOnAllowedPageNum_get;
    pMapper->flowctrl_portPauseOnAllowedPageNum_set = dal_maple_flowctrl_portPauseOnAllowedPageNum_set;
    pMapper->flowctrl_igrSystemPauseThresh_get = dal_maple_flowctrl_igrSystemPauseThresh_get;
    pMapper->flowctrl_igrSystemPauseThresh_set = dal_maple_flowctrl_igrSystemPauseThresh_set;
    pMapper->flowctrl_igrPauseThreshGroup_get = dal_maple_flowctrl_igrPauseThreshGroup_get;
    pMapper->flowctrl_igrPauseThreshGroup_set = dal_maple_flowctrl_igrPauseThreshGroup_set;
    pMapper->flowctrl_portIgrPortThreshGroupSel_get = dal_maple_flowctrl_portIgrPortThreshGroupSel_get;
    pMapper->flowctrl_portIgrPortThreshGroupSel_set = dal_maple_flowctrl_portIgrPortThreshGroupSel_set;
    pMapper->flowctrl_igrSystemCongestThresh_get = dal_maple_flowctrl_igrSystemCongestThresh_get;
    pMapper->flowctrl_igrSystemCongestThresh_set = dal_maple_flowctrl_igrSystemCongestThresh_set;
    pMapper->flowctrl_igrCongestThreshGroup_get = dal_maple_flowctrl_igrCongestThreshGroup_get;
    pMapper->flowctrl_igrCongestThreshGroup_set = dal_maple_flowctrl_igrCongestThreshGroup_set;
    pMapper->flowctrl_egrSystemDropThresh_get = dal_maple_flowctrl_egrSystemDropThresh_get;
    pMapper->flowctrl_egrSystemDropThresh_set = dal_maple_flowctrl_egrSystemDropThresh_set;
    pMapper->flowctrl_egrPortDropEnable_get = dal_maple_flowctrl_egrPortDropEnable_get;
    pMapper->flowctrl_egrPortDropEnable_set = dal_maple_flowctrl_egrPortDropEnable_set;
    pMapper->flowctrl_portEgrDropRefCongestEnable_get = dal_maple_flowctrl_portEgrDropRefCongestEnable_get;
    pMapper->flowctrl_portEgrDropRefCongestEnable_set = dal_maple_flowctrl_portEgrDropRefCongestEnable_set;
    pMapper->flowctrl_egrPortDropThreshGroup_get = dal_maple_flowctrl_egrPortDropThreshGroup_get;
    pMapper->flowctrl_egrPortDropThreshGroup_set = dal_maple_flowctrl_egrPortDropThreshGroup_set;
    pMapper->flowctrl_egrQueueDropThreshGroup_get = dal_maple_flowctrl_egrQueueDropThreshGroup_get;
    pMapper->flowctrl_egrQueueDropThreshGroup_set = dal_maple_flowctrl_egrQueueDropThreshGroup_set;
    pMapper->flowctrl_portEgrDropThreshGroupSel_get = dal_maple_flowctrl_portEgrDropThreshGroupSel_get;
    pMapper->flowctrl_portEgrDropThreshGroupSel_set = dal_maple_flowctrl_portEgrDropThreshGroupSel_set;
    pMapper->flowctrl_egrQueueDropEnable_get = dal_maple_flowctrl_egrQueueDropEnable_get;
    pMapper->flowctrl_egrQueueDropEnable_set = dal_maple_flowctrl_egrQueueDropEnable_set;
    pMapper->flowctrl_igrQueueDropEnable_get = dal_maple_flowctrl_igrQueueDropEnable_get;
    pMapper->flowctrl_igrQueueDropEnable_set = dal_maple_flowctrl_igrQueueDropEnable_set;
    pMapper->flowctrl_igrQueuePauseDropThreshGroupSel_get = dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_get;
    pMapper->flowctrl_igrQueuePauseDropThreshGroupSel_set = dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set;
    pMapper->flowctrl_igrQueueDropThreshGroup_get = dal_maple_flowctrl_igrQueueDropThreshGroup_get;
    pMapper->flowctrl_igrQueueDropThreshGroup_set = dal_maple_flowctrl_igrQueueDropThreshGroup_set;
    pMapper->flowctrl_igrQueuePauseThreshGroup_get = dal_maple_flowctrl_igrQueuePauseThreshGroup_get;
    pMapper->flowctrl_igrQueuePauseThreshGroup_set = dal_maple_flowctrl_igrQueuePauseThreshGroup_set;
    pMapper->flowctrl_portHolTrafficDropEnable_get = dal_maple_flowctrl_portHolTrafficDropEnable_get;
    pMapper->flowctrl_portHolTrafficDropEnable_set = dal_maple_flowctrl_portHolTrafficDropEnable_set;
    pMapper->flowctrl_holTrafficTypeDropEnable_get = dal_maple_flowctrl_holTrafficTypeDropEnable_get;
    pMapper->flowctrl_holTrafficTypeDropEnable_set = dal_maple_flowctrl_holTrafficTypeDropEnable_set;
    pMapper->flowctrl_specialCongestThreshold_get = dal_maple_flowctrl_specialCongestThreshold_get;
    pMapper->flowctrl_specialCongestThreshold_set = dal_maple_flowctrl_specialCongestThreshold_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_flowctrl_init
 * Description:
 *      Initialize flowctrl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1) Module must be initialized before using all of APIs in this module
 */
int32
dal_maple_flowctrl_init(uint32 unit)
{
    int32 ret;
    uint32 port;
    uint32 max_queue, queue;

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

    max_queue = HAL_MAX_NUM_OF_QUEUE(unit);
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        for (queue = 0; queue < max_queue; queue++)
        {
            if (HWP_PORT_EXIST(unit, port) && (ret = dal_maple_flowctrl_egrQueueDropEnable_set(unit, port, queue, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_init */

/* Function Name:
 *      dal_maple_flowctrl_portPauseOnAction_get
 * Description:
 *      Get action when packet keeps receiving after pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to action of packet receive
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE
 *      - PAUSE_ON_DROP
 */
int32
dal_maple_flowctrl_portPauseOnAction_get(
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
    if ((ret = reg_array_field_read(unit, MAPLE_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_FC_ACTf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
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
} /* end of dal_maple_flowctrl_portPauseOnAction_get */

/* Function Name:
 *      dal_maple_flowctrl_portPauseOnAction_set
 * Description:
 *      Set action when packet keeps receiving after pause on frame is sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action of packet receive
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE
 *      - PAUSE_ON_DROP
 */
int32
dal_maple_flowctrl_portPauseOnAction_set(
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
    if ((ret = reg_array_field_write(unit, MAPLE_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_FC_ACTf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portPauseOnAction_set */

/* Function Name:
 *      dal_maple_flowctrl_portPauseOnAllowedPageNum_get
 * Description:
 *      Get number of allowed page when pause on frame sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pPktNum - pointer to number of received page
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
dal_maple_flowctrl_portPauseOnAllowedPageNum_get(uint32 unit, rtk_port_t port, uint32 *pPageNum)
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
    if ((ret = reg_array_field_read(unit, MAPLE_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_FC_ALLOW_PAGE_CNTf, pPageNum)) != RT_ERR_OK)
{
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portPauseOnAllowedPageNum_get */

/* Function Name:
 *      dal_maple_flowctrl_portPauseOnAllowedPageNum_set
 * Description:
 *      Set number of allowed page when pause on frame sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pktNum - number of received page
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
dal_maple_flowctrl_portPauseOnAllowedPageNum_set(uint32 unit, rtk_port_t port, uint32 pageNum)
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
    if ((ret = reg_array_field_write(unit, MAPLE_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_FC_ALLOW_PAGE_CNTf, &pageNum)) != RT_ERR_OK)
{
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portPauseOnAllowedPageNum_set */

/*
 * Flow Control ON
 */

/* Function Name:
 *      dal_maple_flowctrl_igrSystemPauseThresh_get
 * Description:
 *      Get ingress system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the system used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_HI_THRr, MAPLE_GLB_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_HI_THRr, MAPLE_GLB_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_LO_THRr, MAPLE_GLB_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_LO_THRr, MAPLE_GLB_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrSystemPauseThresh_get */

/* Function Name:
 *      dal_maple_flowctrl_igrSystemPauseThresh_set
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;
    uint32  value;

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

    /* get value from CHIP*/
    value = 0;
    if ((ret = reg_read(unit, MAPLE_FC_GLB_HI_THRr, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MAPLE_FC_GLB_HI_THRr, MAPLE_GLB_HI_ON_THRf, &(pThresh->highOn), &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MAPLE_FC_GLB_HI_THRr, MAPLE_GLB_HI_OFF_THRf, &(pThresh->highOff), &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* program value to CHIP*/
    if ((ret = reg_write(unit, MAPLE_FC_GLB_HI_THRr, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* get value from CHIP*/
    if ((ret = reg_read(unit, MAPLE_FC_GLB_LO_THRr, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MAPLE_FC_GLB_LO_THRr, MAPLE_GLB_LO_ON_THRf, &(pThresh->lowOn), &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_set(unit, MAPLE_FC_GLB_LO_THRr, MAPLE_GLB_LO_OFF_THRf, &(pThresh->lowOff), &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* program value to CHIP*/
    if ((ret = reg_write(unit, MAPLE_FC_GLB_LO_THRr, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrSystemPauseThresh_set */

/* Function Name:
 *      dal_maple_flowctrl_igrPauseThreshGroup_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      lowOff thresholds is unused.
 */
int32
dal_maple_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
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
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrPortPauseThresh_get */

/* Function Name:
 *      dal_maple_flowctrl_igrPauseThreshGroup_set
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      lowOff threshold is unused.
 */
int32
dal_maple_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
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
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrPortPauseThresh_set */

/* Function Name:
 *      dal_maple_flowctrl_portIgrPortThreshGroupSel_get
 * Description:
 *      Get ingress port used page pause and drop threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
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
dal_maple_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
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
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_IGR_THRr, port, REG_ARRAY_INDEX_NONE,
                            MAPLE_P_IGR_THR_SET_SELf, pGrp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portIgrPortThreshGroupSel_get */

/* Function Name:
 *      dal_maple_flowctrl_portIgrPortThreshGroupSel_set
 * Description:
 *      Set ingress port used page pause and congest threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
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
dal_maple_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
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
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_IGR_THRr, port, REG_ARRAY_INDEX_NONE,
                            MAPLE_P_IGR_THR_SET_SELf, &grp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portIgrPortThreshGroupSel_set */

/*
 * Flow Control OFF
 */

/* Function Name:
 *      dal_maple_flowctrl_igrSystemCongestThresh_get
 * Description:
 *      Get system used page high/low drop threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the public used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    osal_memset(pThresh, 0, sizeof(rtk_flowctrl_thresh_t));
    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_FCOFF_HI_THRr, MAPLE_GLB_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_FCOFF_HI_THRr, MAPLE_GLB_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_FCOFF_LO_THRr, MAPLE_GLB_FCOFF_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, MAPLE_FC_GLB_FCOFF_LO_THRr, MAPLE_GLB_FCOFF_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrSystemCongestThresh_get */

/* Function Name:
 *      dal_maple_flowctrl_igrSystemCongestThresh_set
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
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

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MAPLE_FC_GLB_FCOFF_HI_THRr, MAPLE_GLB_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MAPLE_FC_GLB_FCOFF_HI_THRr, MAPLE_GLB_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MAPLE_FC_GLB_FCOFF_LO_THRr, MAPLE_GLB_FCOFF_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, MAPLE_FC_GLB_FCOFF_LO_THRr, MAPLE_GLB_FCOFF_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrSystemCongestThresh_set */

/* Function Name:
 *      dal_maple_flowctrl_igrCongestThreshGroup_get
 * Description:
 *      Get used page high drop threshold for the specified threahold group
 * Input:
 *      unit     - unit id
 *      grp_idx  - the index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
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
dal_maple_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
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
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrCongestThreshGroup_get */

/* Function Name:
 *      dal_maple_flowctrl_igrCongestThreshGroup_set
 * Description:
 *      Set used page high drop threshold for the specified threahold group
 * Input:
 *      unit    - unit id
 *      grp_idx - the index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      lowOff threshold is unused.
 */
int32
dal_maple_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
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
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_FCOFF_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_FCOFF_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrCongestThreshGroup_set */

/* Function Name:
 *      dal_maple_flowctrl_egrSystemDropThresh_get
 * Description:
 *      Get egress system drop threshold for the specified unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_FC_DROP_THRr, MAPLE_DROP_ALL_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    pThresh->low = pThresh->high;
    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrSystemDropThresh_get */

/* Function Name:
 *      dal_maple_flowctrl_egrSystemDropThresh_set
 * Description:
 *      Set egress drop threshold for the specified egress port
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
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
    if ((ret = reg_field_write(unit, MAPLE_FC_DROP_THRr, MAPLE_DROP_ALL_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrSystemDropThresh_set */

/* Function Name:
 *      dal_maple_flowctrl_portEgrDropRefCongestEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_RXCNGST_IGNOREf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    /* value 1 is 'ignore' --> disable reference congest */
    switch (value)
    {
        case 1:
            *pEnable = DISABLED;
            break;

        case 0:
            *pEnable = ENABLED;
            break;

        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portEgrDropRefCongestEnable_get */

/* Function Name:
 *      dal_maple_flowctrl_portEgrDropRefCongestEnable_set
 * Description:
 *      Set enable status of refering source port congest status for egress drop
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable - enable status of refering source port congest status
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
dal_maple_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%d"
            , unit, port, enable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    /* disable reference congest --> "ignore" value is '1' */
    if (DISABLED == enable)
        value = 1;
    else
        value = 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_RXCNGST_IGNOREf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portEgrDropRefCongestEnable_set */


/* Function Name:
 *      dal_maple_flowctrl_egrPortDropThreshGroup_get
 * Description:
 *      Get egress port drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_portEgrDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_egrPortDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32 gap;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_EGR_DROP_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_EGR_DROP_GAPf, &gap)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    pThresh->low = pThresh->high - gap;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrPortDropThreshGroup_get */

/* Function Name:
 *      dal_maple_flowctrl_egrPortDropThreshGroup_set
 * Description:
 *      Set egress port drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_portEgrDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_egrPortDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32 gap;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > pThresh->high, RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x", pThresh->high);

    gap = pThresh->high - pThresh->low;
    //RT_PARAM_CHK(gap > GAP_MAX, RT_ERR_OUT_OF_RANGE);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_EGR_DROP_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            MAPLE_P_EGR_DROP_GAPf, &gap)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrPortDropThreshGroup_set */

/* Function Name:
 *      dal_maple_flowctrl_egrQueueDropThreshGroup_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_portEgrDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_egrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32  field;
    uint32  regField_gap;
    uint32 gap;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    switch(queue)
    {
        case 0:
            field = MAPLE_Q0_EGR_DROP_THRf;
            regField_gap = MAPLE_Q0_EGR_DROP_GAPf;
            break;
        case 1:
            field = MAPLE_Q1_EGR_DROP_THRf;
            regField_gap = MAPLE_Q1_EGR_DROP_GAPf;
            break;
        case 2:
            field = MAPLE_Q2_EGR_DROP_THRf;
            regField_gap = MAPLE_Q2_EGR_DROP_GAPf;
            break;
        case 3:
            field = MAPLE_Q3_EGR_DROP_THRf;
            regField_gap = MAPLE_Q3_EGR_DROP_GAPf;
            break;
        case 4:
            field = MAPLE_Q4_EGR_DROP_THRf;
            regField_gap = MAPLE_Q4_EGR_DROP_GAPf;
            break;
        case 5:
            field = MAPLE_Q5_EGR_DROP_THRf;
            regField_gap = MAPLE_Q5_EGR_DROP_GAPf;
            break;
        case 6:
            field = MAPLE_Q6_EGR_DROP_THRf;
            regField_gap = MAPLE_Q6_EGR_DROP_GAPf;
            break;
        case 7:
            field = MAPLE_Q7_EGR_DROP_THRf;
            regField_gap = MAPLE_Q7_EGR_DROP_GAPf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MAPLE_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            field, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            regField_gap, &gap)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    pThresh->low = pThresh->high - gap;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrQueueDropThreshGroup_get */

/* Function Name:
 *      dal_maple_flowctrl_egrQueueDropThreshGroup_set
 * Description:
 *      Set egress queue drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_portEgrDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_egrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32 field;
    uint32  regField_gap;
    uint32 gap;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low> HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low> pThresh->high, RT_ERR_OUT_OF_RANGE);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x", pThresh->high);

    switch(queue)
    {
        case 0:
            field = MAPLE_Q0_EGR_DROP_THRf;
            regField_gap = MAPLE_Q0_EGR_DROP_GAPf;
            break;
        case 1:
            field = MAPLE_Q1_EGR_DROP_THRf;
            regField_gap = MAPLE_Q1_EGR_DROP_GAPf;
            break;
        case 2:
            field = MAPLE_Q2_EGR_DROP_THRf;
            regField_gap = MAPLE_Q2_EGR_DROP_GAPf;
            break;
        case 3:
            field = MAPLE_Q3_EGR_DROP_THRf;
            regField_gap = MAPLE_Q3_EGR_DROP_GAPf;
            break;
        case 4:
            field = MAPLE_Q4_EGR_DROP_THRf;
            regField_gap = MAPLE_Q4_EGR_DROP_GAPf;
            break;
        case 5:
            field = MAPLE_Q5_EGR_DROP_THRf;
            regField_gap = MAPLE_Q5_EGR_DROP_GAPf;
            break;
        case 6:
            field = MAPLE_Q6_EGR_DROP_THRf;
            regField_gap = MAPLE_Q6_EGR_DROP_GAPf;
            break;
        case 7:
            field = MAPLE_Q7_EGR_DROP_THRf;
            regField_gap = MAPLE_Q7_EGR_DROP_GAPf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    gap = pThresh->high - pThresh->low;
    RT_PARAM_CHK(gap > GAP_MAX, RT_ERR_OUT_OF_RANGE);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_write(unit, MAPLE_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            field, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            regField_gap, &gap)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrQueueDropThreshGroup_set */

/* Function Name:
 *      dal_maple_flowctrl_igrQueueDropEnable_get
 * Description:
 *      Get enable status of ingress queue drop.
 * Input:
 *      unit    - unit id
 *      port    - queue id
 *      queue   - queue id
 * Output:
 *      pEnable - pointer to enable status of ingress queue drop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_maple_flowctrl_igrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;
    uint32  field;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d"
            , unit, port, pEnable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);

    switch (queue)
    {
        case 0:
            field = MAPLE_Q0_IGRDROP_ENf;
            break;
        case 1:
            field = MAPLE_Q1_IGRDROP_ENf;
            break;
        case 2:
            field = MAPLE_Q2_IGRDROP_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_P_IGRDROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, field, &value)) != RT_ERR_OK)
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

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueueDropEnable_get */

/* Function Name:
 *      dal_maple_flowctrl_igrQueueDropEnable_set
 * Description:
 *      Set enable status of ingress queue drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      queue  - queue id
 *      enable - enable status of ingress queue drop
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *     none
 */
int32
dal_maple_flowctrl_igrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;
    uint32  field;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%x"
            , unit, port, enable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);

    switch (queue)
    {
        case 0:
            field = MAPLE_Q0_IGRDROP_ENf;
            break;
        case 1:
            field = MAPLE_Q1_IGRDROP_ENf;
            break;
        case 2:
            field = MAPLE_Q2_IGRDROP_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_P_IGRDROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, field, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueueDropEnable_set */

/* Function Name:
 *      dal_maple_flowctrl_igrQueuePauseThreshGroup_get
 * Description:
 *      Get ingress queue Pause threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_igrQueuePauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32 field_hi;
    uint32 field_lo;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);

    switch (queue)
    {
        case 0:
            field_hi = MAPLE_Q0_ON_THRf;
            field_lo = MAPLE_Q0_OFF_THRf;
            break;
        case 1:
            field_hi = MAPLE_Q1_ON_THRf;
            field_lo = MAPLE_Q1_OFF_THRf;
            break;
        case 2:
            field_hi = MAPLE_Q2_ON_THRf;
            field_lo = MAPLE_Q2_OFF_THRf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            field_hi, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            field_lo, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueuePauseThreshGroup_get */

/* Function Name:
 *      dal_maple_flowctrl_igrQueuePauseThreshGroup_set
 * Description:
 *      Set ingress queue Pause threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_igrQueuePauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32  field_hi;
    uint32  field_lo;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > pThresh->high, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    switch (queue)
    {
        case 0:
            field_hi = MAPLE_Q0_ON_THRf;
            field_lo = MAPLE_Q0_OFF_THRf;
            break;
        case 1:
            field_hi = MAPLE_Q1_ON_THRf;
            field_lo = MAPLE_Q1_OFF_THRf;
            break;
        case 2:
            field_hi = MAPLE_Q2_ON_THRf;
            field_lo = MAPLE_Q2_OFF_THRf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            field_hi, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            field_lo, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueuePauseThreshGroup_set */

/* Function Name:
 *      dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_get
 * Description:
 *      Get ingress queue used page pause/drop threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
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
dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
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
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_IGRQ_THRr, port, REG_ARRAY_INDEX_NONE,
                            MAPLE_Q_IGR_THR_SET_SELf, pGrp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_get */

/* Function Name:
 *      dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set
 * Description:
 *      Set ingress queue used page pause/drop threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
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
dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
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
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_IGRQ_THRr, port, REG_ARRAY_INDEX_NONE,
                            MAPLE_Q_IGR_THR_SET_SELf, &grp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set */

/* Function Name:
 *      dal_maple_flowctrl_portEgrDropThreshGroupSel_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_portEgrDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
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
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_EGR_THRr, port, REG_ARRAY_INDEX_NONE,
                            MAPLE_P_EGR_THR_SET_SELf, pGrp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portEgrDropThreshGroupSel_get */

/* Function Name:
 *      dal_maple_flowctrl_portEgrDropThreshGroupSel_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_portEgrDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
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
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_THRr, port, REG_ARRAY_INDEX_NONE,
                            MAPLE_P_EGR_THR_SET_SELf, &grp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_portEgrDropThreshGroupSel_set */

/* Function Name:
 *      dal_maple_flowctrl_egrQueueDropEnable_get
 * Description:
 *      Get enable status of egress queue drop.
 * Input:
 *      unit    - unit id
 *      port    - queue id
 *      queue   - queue id
 * Output:
 *      pEnable - pointer to enable status of egress queue drop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_maple_flowctrl_egrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;
    uint32  field;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, queue=%d"
            , unit, port, queue);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (queue)
    {
        case 0:
            field = MAPLE_Q0_EGRDROP_ENf;
            break;
        case 1:
            field = MAPLE_Q1_EGRDROP_ENf;
            break;
        case 2:
            field = MAPLE_Q2_EGRDROP_ENf;
            break;
        case 3:
            field = MAPLE_Q3_EGRDROP_ENf;
            break;
        case 4:
            field = MAPLE_Q4_EGRDROP_ENf;
            break;
        case 5:
            field = MAPLE_Q5_EGRDROP_ENf;
            break;
        case 6:
            field = MAPLE_Q6_EGRDROP_ENf;
            break;
        case 7:
            field = MAPLE_Q7_EGRDROP_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, field, &value)) != RT_ERR_OK)
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

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrQueueDropEnable_get */

/* Function Name:
 *      dal_maple_flowctrl_egrQueueDropEnable_set
 * Description:
 *      Set enable status of egress queue drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      queue  - queue id
 *      enable - enable status of egress queue drop
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *     none
 */
int32
dal_maple_flowctrl_egrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;
    uint32  field;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, queue=%d, enable=%x"
            , unit, port, queue, enable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (queue)
    {
        case 0:
            field = MAPLE_Q0_EGRDROP_ENf;
            break;
        case 1:
            field = MAPLE_Q1_EGRDROP_ENf;
            break;
        case 2:
            field = MAPLE_Q2_EGRDROP_ENf;
            break;
        case 3:
            field = MAPLE_Q3_EGRDROP_ENf;
            break;
        case 4:
            field = MAPLE_Q4_EGRDROP_ENf;
            break;
        case 5:
            field = MAPLE_Q5_EGRDROP_ENf;
            break;
        case 6:
            field = MAPLE_Q6_EGRDROP_ENf;
            break;
        case 7:
            field = MAPLE_Q7_EGRDROP_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, field, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrQueueDropEnable_set */

/* Function Name:
 *      dal_maple_flowctrl_egrPortDropEnable_get
 * Description:
 *      Get enable status of egress port drop.
 * Input:
 *      unit    - unit id
 *      port    - queue id
 * Output:
 *      pEnable - pointer to enable status of egress port drop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_maple_flowctrl_egrPortDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d"
            , unit, port, pEnable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_FC_P_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_PB_EGRDROP_ENf, &value)) != RT_ERR_OK)
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

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrPortDropEnable_get */

/* Function Name:
 *      dal_maple_flowctrl_egrPortDropEnable_set
 * Description:
 *      Set enable status of egress port drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      enable - enable status of egress port drop
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *     none
 */
int32
dal_maple_flowctrl_egrPortDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%x"
            , unit, port, enable);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, MAPLE_PB_EGRDROP_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_egrPortDropEnable_set */

/* Function Name:
 *      dal_maple_flowctrl_specialCongestThreshold_get
 * Description:
 *      Get special congstion threshold
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure for special congstion
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_specialCongestThreshold_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_SC_P_CTRLr, MAPLE_DRAIN_OUT_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    pThresh->low = pThresh->high;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    return RT_ERR_OK;
}/* end of dal_maple_flowctrl_specialCongestThreshold_get */

/* Function Name:
 *      dal_maple_flowctrl_specialCongestThreshold_set
 * Description:
 *      Set special congstion threshold
 * Input:
 *      unit    - unit id
 *      pThresh -the pointer to threshold structure for special congstion
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_maple_flowctrl_specialCongestThreshold_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, high=%d, low=%d", unit, pThresh->high, pThresh->low);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > pThresh->high, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value from CHIP */
    if ((ret = reg_field_write(unit, MAPLE_SC_P_CTRLr, MAPLE_DRAIN_OUT_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_maple_flowctrl_specialCongestThreshold_set */

/* Function Name:
 *      dal_maple_flowctrl_igrQueueDropThreshGroup_get
 * Description:
 *      Get ingress queue Drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_igrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32 regField;
    uint32  regField_gap;
    uint32 gap;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);

    switch (queue)
    {
        case 0:
            regField = MAPLE_Q0_FCOFF_THRf;
            regField_gap = MAPLE_Q0_FCOFF_GAPf;
            break;
        case 1:
            regField = MAPLE_Q1_FCOFF_THRf;
            regField_gap = MAPLE_Q1_FCOFF_GAPf;
            break;
        case 2:
            regField = MAPLE_Q2_FCOFF_THRf;
            regField_gap = MAPLE_Q2_FCOFF_GAPf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            regField, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            regField_gap, &gap)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    pThresh->low = pThresh->high - gap;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueueDropThreshGroup_get */

/* Function Name:
 *      dal_maple_flowctrl_igrQueueDropThreshGroup_set
 * Description:
 *      Set ingress queue Drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      1) Each port can map to a threshold group, refer to dal_maple_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 */
int32
dal_maple_flowctrl_igrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32   ret;
    uint32  regField;
    uint32  regField_gap;
    uint32 gap;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d, high=%d, low=%d", unit, grp_idx, pThresh->high, pThresh->low);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > pThresh->high, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    switch (queue)
    {
        case 0:
            regField = MAPLE_Q0_FCOFF_THRf;
            regField_gap = MAPLE_Q0_FCOFF_GAPf;
            break;
        case 1:
            regField = MAPLE_Q1_FCOFF_THRf;
            regField_gap = MAPLE_Q1_FCOFF_GAPf;
            break;
        case 2:
            regField = MAPLE_Q2_FCOFF_THRf;
            regField_gap = MAPLE_Q2_FCOFF_GAPf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    gap = pThresh->high - pThresh->low;
    RT_PARAM_CHK(gap > GAP_MAX, RT_ERR_OUT_OF_RANGE);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_array_field_write(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            regField, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MAPLE_FC_Q_IGR_DROP_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            regField_gap, &gap)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_flowctrl_igrQueueDropThreshGroup_set */


/* Function Name:
 *      dal_maple_flowctrl_portHolTrafficDropEnable_get
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
dal_maple_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 val;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);


    {
        FLOWCTRL_SEM_LOCK(unit);

        if ((ret = reg_array_field_read(unit, MAPLE_FC_P_EGR_DROP_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                                        MAPLE_KNUC_FC_MODE_ENf, &val)) != RT_ERR_OK)
        {
            FLOWCTRL_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
            return ret;
        }

        FLOWCTRL_SEM_UNLOCK(unit);

        *pEnable = (val == 1) ? ENABLED : DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_flowctrl_portHolTrafficDropEnable_set
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
dal_maple_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 val;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    val = (enable == ENABLED) ? 1 : 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_FC_P_EGR_DROP_CTRLr, port, REG_ARRAY_INDEX_NONE,\
                                    MAPLE_KNUC_FC_MODE_ENf, &val)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_flowctrl_holTrafficTypeDropEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
int32
dal_maple_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 field;
    uint32 val;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case HOL_TRAFFIC_TYPE_UNKN_UC:
            field = MAPLE_UNKNUC_SELf;
            break;
        case HOL_TRAFFIC_TYPE_L2_MC:
            field = MAPLE_L2_MC_SELf;
            break;
        case HOL_TRAFFIC_TYPE_IP_MC:
            field = MAPLE_IP_MC_SELf;
            break;
        case HOL_TRAFFIC_TYPE_BC:
            field = MAPLE_BC_SELf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    FLOWCTRL_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_KNUC_FC_MODE_DROP_PKT_TYPEr, field, &val)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    *pEnable = (val == 1) ? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_flowctrl_holTrafficTypeDropEnable_set
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
int32
dal_maple_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable)
{
    int32 ret;
    uint32 field;
    uint32 val;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, type=%d, enable=%d", unit, type, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (type)
    {
        case HOL_TRAFFIC_TYPE_UNKN_UC:
            field = MAPLE_UNKNUC_SELf;
            break;
        case HOL_TRAFFIC_TYPE_L2_MC:
            field = MAPLE_L2_MC_SELf;
            break;
        case HOL_TRAFFIC_TYPE_IP_MC:
            field = MAPLE_IP_MC_SELf;
            break;
        case HOL_TRAFFIC_TYPE_BC:
            field = MAPLE_BC_SELf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    val = (enable == ENABLED) ? 1 : 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_KNUC_FC_MODE_DROP_PKT_TYPEr, field, &val)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


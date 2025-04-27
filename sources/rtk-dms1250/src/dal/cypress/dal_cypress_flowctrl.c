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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_flowctrl.h>
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
 *      dal_cypress_flowctrlMapper_init
 * Description:
 *      Hook flowctrl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook flowctrl module before calling any flowctrl APIs.
 */
int32
dal_cypress_flowctrlMapper_init(dal_mapper_t *pMapper)
{
    pMapper->flowctrl_init = dal_cypress_flowctrl_init;
    pMapper->flowctrl_portPauseOnAction_get = dal_cypress_flowctrl_portPauseOnAction_get;
    pMapper->flowctrl_portPauseOnAction_set = dal_cypress_flowctrl_portPauseOnAction_set;
    pMapper->flowctrl_pauseOnAllowedPktLen_get = dal_cypress_flowctrl_pauseOnAllowedPktLen_get;
    pMapper->flowctrl_pauseOnAllowedPktLen_set = dal_cypress_flowctrl_pauseOnAllowedPktLen_set;
    pMapper->flowctrl_pauseOnAllowedPktNum_get = dal_cypress_flowctrl_pauseOnAllowedPktNum_get;
    pMapper->flowctrl_pauseOnAllowedPktNum_set = dal_cypress_flowctrl_pauseOnAllowedPktNum_set;
    pMapper->flowctrl_igrSystemPauseThresh_get = dal_cypress_flowctrl_igrSystemPauseThresh_get;
    pMapper->flowctrl_igrSystemPauseThresh_set = dal_cypress_flowctrl_igrSystemPauseThresh_set;
    pMapper->flowctrl_igrPauseThreshGroup_get = dal_cypress_flowctrl_igrPauseThreshGroup_get;
    pMapper->flowctrl_igrPauseThreshGroup_set = dal_cypress_flowctrl_igrPauseThreshGroup_set;
    pMapper->flowctrl_portIgrPortThreshGroupSel_get = dal_cypress_flowctrl_portIgrPortThreshGroupSel_get;
    pMapper->flowctrl_portIgrPortThreshGroupSel_set = dal_cypress_flowctrl_portIgrPortThreshGroupSel_set;
    pMapper->flowctrl_igrSystemCongestThresh_get = dal_cypress_flowctrl_igrSystemCongestThresh_get;
    pMapper->flowctrl_igrSystemCongestThresh_set = dal_cypress_flowctrl_igrSystemCongestThresh_set;
    pMapper->flowctrl_igrCongestThreshGroup_get = dal_cypress_flowctrl_igrCongestThreshGroup_get;
    pMapper->flowctrl_igrCongestThreshGroup_set = dal_cypress_flowctrl_igrCongestThreshGroup_set;
    pMapper->flowctrl_egrSystemDropThresh_get = dal_cypress_flowctrl_egrSystemDropThresh_get;
    pMapper->flowctrl_egrSystemDropThresh_set = dal_cypress_flowctrl_egrSystemDropThresh_set;
    pMapper->flowctrl_egrPortDropThresh_get = dal_cypress_flowctrl_egrPortDropThresh_get;
    pMapper->flowctrl_egrPortDropThresh_set = dal_cypress_flowctrl_egrPortDropThresh_set;
    pMapper->flowctrl_egrPortQueueDropEnable_get = dal_cypress_flowctrl_egrPortQueueDropEnable_get;
    pMapper->flowctrl_egrPortQueueDropEnable_set = dal_cypress_flowctrl_egrPortQueueDropEnable_set;
    pMapper->flowctrl_egrQueueDropThresh_get = dal_cypress_flowctrl_egrQueueDropThresh_get;
    pMapper->flowctrl_egrQueueDropThresh_set = dal_cypress_flowctrl_egrQueueDropThresh_set;
    pMapper->flowctrl_egrCpuQueueDropThresh_get = dal_cypress_flowctrl_egrCpuQueueDropThresh_get;
    pMapper->flowctrl_egrCpuQueueDropThresh_set = dal_cypress_flowctrl_egrCpuQueueDropThresh_set;
    pMapper->flowctrl_portEgrDropRefCongestEnable_get = dal_cypress_flowctrl_portEgrDropRefCongestEnable_get;
    pMapper->flowctrl_portEgrDropRefCongestEnable_set = dal_cypress_flowctrl_portEgrDropRefCongestEnable_set;
    pMapper->flowctrl_portHolTrafficDropEnable_get = dal_cypress_flowctrl_portHolTrafficDropEnable_get;
    pMapper->flowctrl_portHolTrafficDropEnable_set = dal_cypress_flowctrl_portHolTrafficDropEnable_set;
    pMapper->flowctrl_holTrafficTypeDropEnable_get = dal_cypress_flowctrl_holTrafficTypeDropEnable_get;
    pMapper->flowctrl_holTrafficTypeDropEnable_set = dal_cypress_flowctrl_holTrafficTypeDropEnable_set;
    pMapper->flowctrl_specialCongestThreshold_get = dal_cypress_flowctrl_specialCongestThreshold_get;
    pMapper->flowctrl_specialCongestThreshold_set = dal_cypress_flowctrl_specialCongestThreshold_set;

    return RT_ERR_OK;
}
static int32 _dal_cypress_flowctrl_init_config(uint32 unit);

/* Function Name:
 *      dal_cypress_flowctrl_init
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
 *      1. Module must be initialized before using all of APIs in this module
 */
int32
dal_cypress_flowctrl_init(uint32 unit)
{
    int32   ret;

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


    /* Initialized configuration */
    if (( ret = _dal_cypress_flowctrl_init_config(unit)) != RT_ERR_OK)
    {
        flowctrl_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "default flowcontrol config failed");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_init */

/*
 * Flow Control ON
 */

/* Function Name:
 *      dal_cypress_flowctrl_portPauseOnAction_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE
 *      - PAUSE_ON_DROP
 */
int32
dal_cypress_flowctrl_portPauseOnAction_get(
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

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ACTf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_flowctrl_portPauseOnAction_get */

/* Function Name:
 *      dal_cypress_flowctrl_portPauseOnAction_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE
 *      - PAUSE_ON_DROP
 */
int32
dal_cypress_flowctrl_portPauseOnAction_set(
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

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

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
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ACTf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_portPauseOnAction_set */

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktLen_get
 * Description:
 *      Get number of allowed packet length after pause on frame is sent.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPktLen  - pointer to number of received packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The length unit is byte.
 */
int32
dal_cypress_flowctrl_pauseOnAllowedPktLen_get(uint32 unit, rtk_port_t port, uint32 *pPktLen)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pPktLen=%x"
            , unit, port, pPktLen);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPktLen), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ALLOW_PKT_LENf, pPktLen)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_pauseOnAllowedPktLen_get */

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktLen_set
 * Description:
 *      Set number of allowed packet length after pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pktLen  - number of received packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      The length unit is byte.
 */
int32
dal_cypress_flowctrl_pauseOnAllowedPktLen_set(uint32 unit, rtk_port_t port, uint32 pktLen)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pktLen=%d"
            , unit, port, pktLen);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pktLen > HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_LEN_MAX(unit)), RT_ERR_INPUT);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ALLOW_PKT_LENf, &pktLen)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_pauseOnAllowedPktLen_set */

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktNum_get
 * Description:
 *      Get number of allowed packet when pause on frame sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pPktNum - pointer to number of received packet
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
dal_cypress_flowctrl_pauseOnAllowedPktNum_get(uint32 unit, rtk_port_t port, uint32 *pPktNum)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pPktNum=%x"
            , unit, port, pPktNum);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPktNum), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ALLOW_PKT_CNTf, pPktNum)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_pauseOnAllowedPktNum_get */

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktNum_set
 * Description:
 *      Set number of allowed packet when pause on frame sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pktNum - number of received packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_pauseOnAllowedPktNum_set(uint32 unit, rtk_port_t port, uint32 pktNum)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pktNum=%d"
            , unit, port, pktNum);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((pktNum > HAL_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX(unit)), RT_ERR_OUT_OF_RANGE);

    FLOWCTRL_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_ACT_CTRLr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ALLOW_PKT_CNTf, &pktNum)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_DAL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_pauseOnAllowedPktNum_set */

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemPauseThresh_get
 * Description:
 *      Get ingress system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the system used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_HI_THRr, CYPRESS_GLB_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_HI_THRr, CYPRESS_GLB_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_LO_THRr, CYPRESS_GLB_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_LO_THRr, CYPRESS_GLB_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_igrSystemPauseThresh_get */

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemPauseThresh_set
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
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

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_HI_THRr, CYPRESS_GLB_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_HI_THRr, CYPRESS_GLB_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_LO_THRr, CYPRESS_GLB_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_LO_THRr, CYPRESS_GLB_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_igrSystemPauseThresh_set */

/* Function Name:
 *      dal_cypress_flowctrl_igrPauseThreshGroup_get
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      lowOff thresholds is unused.
 */
int32
dal_cypress_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_LO_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    pThresh->lowOff = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_igrPortPauseThresh_get */

/* Function Name:
 *      dal_cypress_flowctrl_igrPauseThreshGroup_set
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      lowOff threshold is unused.
 */
int32
dal_cypress_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x",
           pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_LO_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_igrPortPauseThresh_set */

/* Function Name:
 *      dal_cypress_flowctrl_portIgrPortThreshGroupSel_get
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
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGrp_idx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE,
                            CYPRESS_P_THR_SET_SELf, pGrp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_portIgrPortThreshGroupSel_set
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
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx)
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

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_THR_SET_SELr, port, REG_ARRAY_INDEX_NONE,
                            CYPRESS_P_THR_SET_SELf, &grp_idx)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/*
 * Flow Control OFF
 */

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemCongestThresh_get
 * Description:
 *      Get system used page high/low drop threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the public used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);

    osal_memset(pThresh, 0, sizeof(rtk_flowctrl_thresh_t));
    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_FCOFF_HI_THRr, CYPRESS_GLB_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_FCOFF_HI_THRr, CYPRESS_GLB_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_FCOFF_LO_THRr, CYPRESS_GLB_FCOFF_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, CYPRESS_FC_GLB_FCOFF_LO_THRr, CYPRESS_GLB_FCOFF_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_igrSystemCongestThresh_get */

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemCongestThresh_set
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh)
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

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_FCOFF_HI_THRr, CYPRESS_GLB_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_FCOFF_HI_THRr, CYPRESS_GLB_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_FCOFF_LO_THRr, CYPRESS_GLB_FCOFF_LO_ON_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_FC_GLB_FCOFF_LO_THRr, CYPRESS_GLB_FCOFF_LO_OFF_THRf, &(pThresh->lowOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_igrSystemCongestThresh_set */

/* Function Name:
 *      dal_cypress_flowctrl_igrCongestThreshGroup_get
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
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_FCOFF_LO_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    pThresh->lowOff = pThresh->lowOn;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x\
           lowOn=0x%x, lowOff=0x%x", pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_igrCongestThreshGroup_set
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      lowOff threshold is unused.
 */
int32
dal_cypress_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh)
{
    int32   ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, grp_idx=%d", unit, grp_idx);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->highOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->highOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOn > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->lowOff > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((grp_idx > HAL_THRESH_OF_IGR_PORT_PAUSE_CONGEST_GROUP_IDX_MAX(unit)), RT_ERR_INPUT);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "highOn=0x%x, highOff=0x%x, lowOn=0x%x, lowOff=0x%x",
           pThresh->highOn, pThresh->highOff, pThresh->lowOn, pThresh->lowOff);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_FCOFF_HI_ON_THRf, &(pThresh->highOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_FCOFF_HI_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_FCOFF_HI_OFF_THRf, &(pThresh->highOff))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_LO_THRr, REG_ARRAY_INDEX_NONE, grp_idx,
                            CYPRESS_P_FCOFF_LO_THRf, &(pThresh->lowOn))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_egrSystemDropThresh_get
 * Description:
 *      Get egress system drop threshold for the specified unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
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
    if ((ret = reg_field_read(unit, CYPRESS_FC_DROP_THRr, CYPRESS_DROP_ALL_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    pThresh->low = pThresh->high;
    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrSystemDropThresh_get */

/* Function Name:
 *      dal_cypress_flowctrl_egrSystemDropThresh_set
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
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
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

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_FC_DROP_THRr, CYPRESS_DROP_ALL_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrSystemDropThresh_set */

/* Function Name:
 *      dal_cypress_flowctrl_egrPortDropThresh_get
 * Description:
 *      Get egress drop threshold for the specified egress port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pThresh - pointer to the drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrPortDropThresh_get(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;
    out_q_entry_t outQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    osal_memset(&outQEntry, 0, sizeof(out_q_entry_t));

    if ((ret = table_read(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQEntry)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_P_EGR_DROP_ON_THRtf,
                    &(pThresh->high), (uint32 *) &outQEntry)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_P_EGR_DROP_OFF_THRtf,
                    &(pThresh->low), (uint32 *) &outQEntry)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrPortDropThresh_get */

/* Function Name:
 *      dal_cypress_flowctrl_egrPortDropThresh_set
 * Description:
 *      Set egress drop threshold for the specified egress port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      Low threshold is unused.
 */
int32
dal_cypress_flowctrl_egrPortDropThresh_set(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;
    out_q_entry_t outQEntry;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);


    FLOWCTRL_SEM_LOCK(unit);

    osal_memset(&outQEntry, 0, sizeof(out_q_entry_t));

    /* program value to CHIP*/
    if ((ret = table_read(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQEntry)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_P_EGR_DROP_ON_THRtf,
                    &(pThresh->high), (uint32 *) &outQEntry)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_OUT_Qt, CYPRESS_OUT_Q_P_EGR_DROP_OFF_THRtf,
                    &(pThresh->low), (uint32 *) &outQEntry)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_OUT_Qt, port, (uint32 *) &outQEntry)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrPortDropThresh_set */

/* Function Name:
 *      dal_cypress_flowctrl_portEgrDropRefCongestEnable_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, pEnable=%x"
            , unit, port, pEnable);

    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_RXCNGST_IGNOREr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_RXCNGST_IGNOREf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_flowctrl_portEgrDropRefCongestEnable_get */

/* Function Name:
 *      dal_cypress_flowctrl_portEgrDropRefCongestEnable_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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

    if (DISABLED == enable)
        value = 0;
    else
        value = 1;

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_RXCNGST_IGNOREr
                        , port, REG_ARRAY_INDEX_NONE, CYPRESS_RXCNGST_IGNOREf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_portEgrDropRefCongestEnable_set */

/* Function Name:
 *      dal_cypress_flowctrl_egrPortQueueDropEnable_get
 * Description:
 *      Get egress queue drop ability for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of egress queue drop ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrPortQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_Q_EGR_DROP_ENr, port, REG_ARRAY_INDEX_NONE,
                        CYPRESS_P_Q_EGR_DROP_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    if (value)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrQueueDropThresh_get */

/* Function Name:
 *      dal_cypress_flowctrl_egrPortQueueDropEnable_set
 * Description:
 *      Set egress queue drop ability for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of egress queue drop ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrPortQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 value;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    if (ENABLED == enable)
        value = 1;
    else
        value = 0;

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_Q_EGR_DROP_ENr, port, REG_ARRAY_INDEX_NONE,
                        CYPRESS_P_Q_EGR_DROP_ENf, &value)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrQueueDropThresh_set */

/* Function Name:
 *      dal_cypress_flowctrl_egrQueueDropThresh_get
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
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, queue=%d",
           unit, queue);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_Q_EGR_DROP_ON_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_Q_EGR_DROP_OFF_THRf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x",
           pThresh->high, pThresh->low);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrQueueDropThresh_get */

/* Function Name:
 *      dal_cypress_flowctrl_egrQueueDropThresh_set
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
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, queue=%d", unit, queue);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_Q_EGR_DROP_ON_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_FC_Q_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_Q_EGR_DROP_OFF_THRf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_flowctrl_egrQueueDropThresh_set */

/* Function Name:
 *      dal_cypress_flowctrl_egrCpuQueueDropThresh_get
 * Description:
 *      Get egress global drop threshold for the egress queue of CPU port
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the global drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrCpuQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, queue=%d",
           unit, queue);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);

    FLOWCTRL_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_FC_CPUQ_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_CPUQ_EGR_DROP_ON_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_FC_CPUQ_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_CPUQ_EGR_DROP_OFF_THRf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x",
           pThresh->high, pThresh->low);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_egrCpuQueueDropThresh_set
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh)
{
    int32 ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, queue=%d", unit, queue);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresh), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pThresh->high > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pThresh->low > HAL_FLOWCTRL_THRESH_MAX(unit), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);


    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_CPUQ_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_CPUQ_EGR_DROP_ON_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, CYPRESS_FC_CPUQ_EGR_DROP_THRr, REG_ARRAY_INDEX_NONE, queue,
                        CYPRESS_CPUQ_EGR_DROP_OFF_THRf, &(pThresh->low))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_portHolTrafficDropEnable_get
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
dal_cypress_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    FLOWCTRL_SEM_LOCK(unit);

    if ((ret = reg_array_field_read(unit, CYPRESS_FC_P_FLD_HOL_PRVNT_ENr, port, REG_ARRAY_INDEX_NONE,\
                                    CYPRESS_FLD_HOL_PRVNT_ENf, pEnable)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_portHolTrafficDropEnable_set
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
dal_cypress_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, CYPRESS_FC_P_FLD_HOL_PRVNT_ENr, port, \
                        REG_ARRAY_INDEX_NONE, CYPRESS_FLD_HOL_PRVNT_ENf, &enable)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_holTrafficTypeDropEnable_get
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
dal_cypress_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 field;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, type=%d", unit, type);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case HOL_TRAFFIC_TYPE_UNKN_UC:
            field = CYPRESS_UNKN_UC_EGR_DROP_ENf;
            break;
        case HOL_TRAFFIC_TYPE_L2_MC:
            field = CYPRESS_L2_MC_EGR_DROP_ENf;
            break;
        case HOL_TRAFFIC_TYPE_IP_MC:
            field = CYPRESS_IP_MC_EGR_DROP_ENf;
            break;
        case HOL_TRAFFIC_TYPE_BC:
            field = CYPRESS_BC_EGR_DROP_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    FLOWCTRL_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_FC_FLD_HOL_PRVNT_CTRLr, field, pEnable)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_holTrafficTypeDropEnable_set
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
dal_cypress_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable)
{
    int32 ret;
    uint32 field;

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "unit=%d, type=%d, enable=%d", unit, type, enable);

    /* check Init status */
    RT_INIT_CHK(flowctrl_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    switch (type)
    {
        case HOL_TRAFFIC_TYPE_UNKN_UC:
            field = CYPRESS_UNKN_UC_EGR_DROP_ENf;
            break;
        case HOL_TRAFFIC_TYPE_L2_MC:
            field = CYPRESS_L2_MC_EGR_DROP_ENf;
            break;
        case HOL_TRAFFIC_TYPE_IP_MC:
            field = CYPRESS_IP_MC_EGR_DROP_ENf;
            break;
        case HOL_TRAFFIC_TYPE_BC:
            field = CYPRESS_BC_EGR_DROP_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    FLOWCTRL_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_FC_FLD_HOL_PRVNT_CTRLr, field, &enable)) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_flowctrl_specialCongestThreshold_get
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
dal_cypress_flowctrl_specialCongestThreshold_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
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
    if ((ret = reg_field_read(unit, CYPRESS_SC_P_CTRLr, CYPRESS_DRAIN_OUT_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    pThresh->low = pThresh->high;
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_FLOWCTRL), "high=0x%x, low=0x%x", pThresh->high, pThresh->low);

    return RT_ERR_OK;
}/* end of dal_cypress_flowctrl_specialCongestThreshold_get */

/* Function Name:
 *      dal_cypress_flowctrl_specialCongestThreshold_set
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
dal_cypress_flowctrl_specialCongestThreshold_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh)
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
    if ((ret = reg_field_write(unit, CYPRESS_SC_P_CTRLr, CYPRESS_DRAIN_OUT_THRf, &(pThresh->high))) != RT_ERR_OK)
    {
        FLOWCTRL_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    FLOWCTRL_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}/* end of dal_cypress_flowctrl_specialCongestThreshold_set */

/* Function Name:
 *      _dal_cypress_flowctrl_init_config
 * Description:
 *      Initialize configuration of flowctrl module for the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Flowcontrol module must be initialized before using this API
 */
static int32
_dal_cypress_flowctrl_init_config(uint32 unit)
{
    int32   ret;
    uint32  value = FLOWCTRL_HALF_BKPRES_MTHD;

    /* set Half-duplex Backpressure = Jam mode */
    if ((ret = reg_field_write(unit, CYPRESS_MAC_GLB_CTRLr, CYPRESS_BKPRES_MTHD_SELf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_FLOWCTRL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_flowctrl_init_config */

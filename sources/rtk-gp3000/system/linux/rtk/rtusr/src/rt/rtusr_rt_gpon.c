/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of GPON Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) GPON parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/rt/rt_gpon.h>

/*
 * Function Declaration
 */

/* Module Name    : Gpon     */
/* Sub-module Name: Gpon parameter settings */

/* Function Name:
 *      rt_gpon_init
 * Description:
 *      Initialize GPON MAC.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
rt_gpon_init(void)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_RT_GPON_INIT, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_init */

/* Function Name:
 *      rt_gpon_serialNumber_set
 * Description:
 *      Set GPON serial number
 * Input:
 *      pSN   - pointer to the GPON serial number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_serialNumber_set(rt_gpon_serialNumber_t *pSN)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSN), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.sN, pSN, sizeof(rt_gpon_serialNumber_t));
    SETSOCKOPT(RTDRV_RT_GPON_SERIALNUMBER_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_serialNumber_set */

/* Function Name:
 *      rt_gpon_registrationId_set
 * Description:
 *      Set GPON registration ID (password in 984.3).
 * Input:
 *      pRegId   - pointer to the GPON registration id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_registrationId_set(rt_gpon_registrationId_t *pRegId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegId), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.regId, pRegId, sizeof(rt_gpon_registrationId_t));
    SETSOCKOPT(RTDRV_RT_GPON_REGISTRATIONID_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_registrationId_set */

/* Function Name:
 *      rt_gpon_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState   - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The GPON MAC is starting to work now.
 */
int32
rt_gpon_activate(rt_gpon_initialState_t initState)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* function body */
    osal_memcpy(&rt_gpon_cfg.initState, &initState, sizeof(rt_gpon_initialState_t));
    GETSOCKOPT(RTDRV_RT_GPON_ACTIVATE, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_activate */

/* Function Name:
 *      rt_gpon_deactivate
 * Description:
 *      GPON MAC Deactivate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      The GPON MAC is out of work now.
*/
int32
rt_gpon_deactivate(void)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* function body */
    GETSOCKOPT(RTDRV_RT_GPON_DEACTIVATE, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_deactivate */

/* Function Name:
 *      rt_gpon_onuState_get
 * Description:
 *      Get the GPON ONU state.
 * Input:
 *      None
 * Output:
 *      pOnuState  - pointer to the ONU state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If the device is not activated, an unknown status is returned.
 */
int32
rt_gpon_onuState_get(rt_gpon_onuState_t *pOnuState)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pOnuState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.onuState, pOnuState, sizeof(rt_gpon_onuState_t));
    GETSOCKOPT(RTDRV_RT_GPON_ONUSTATE_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pOnuState, &rt_gpon_cfg.onuState, sizeof(rt_gpon_onuState_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_onuState_get */

/* Function Name:
 *      rt_gpon_tcont_set
 * Description:
 *      Set TCONT by assigning an allocation id.
 * Input:
 *      allocId   - allocation id 
 * Output:
 *      pTcontId  - pointer to the logic TCONT id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_tcont_set(uint32 *pTcontId, uint32 allocId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTcontId), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.tcontId, pTcontId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.allocId, &allocId, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_GPON_TCONT_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    *pTcontId = rt_gpon_cfg.tcontId;

    return RT_ERR_OK;
}   /* end of rt_gpon_tcont_set */

/* Function Name:
 *      rt_gpon_tcont_get
 * Description:
 *      Get allocation id by assigned logic TCONT id.
 * Input:
 *      tcontId  -  logic TCONT id
 * Output:
 *      pAllocId   -  pointer to the allocation id 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAllocId), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.tcontId, &tcontId, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_TCONT_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pAllocId, &rt_gpon_cfg.allocId, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_gpon_tcont_get */

/* Function Name:
 *      rt_gpon_tcont_del
 * Description:
 *      Delete the TCONT id
 * Input:
 *      tcontId   - TCONT id 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_tcont_del(uint32 tcontId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.tcontId, &tcontId, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_GPON_TCONT_DEL, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_tcont_del */

/* Function Name:
 *      rt_gpon_usFlow_set
 * Description:
 *      Set the upstream flow
 * Input:
 *      usFlowId  - logical flow id
 *      pUsFlow   - pointer to the upstream flow configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_usFlow_set(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.usFlowId, &usFlowId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.usFlow, pUsFlow, sizeof(rt_gpon_usFlow_t));
    SETSOCKOPT(RTDRV_RT_GPON_USFLOW_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_usFlow_set */

/* Function Name:
 *      rt_gpon_usFlow_get
 * Description:
 *      Get the upstream flow
 * Input:
 *      usFlowId  - logical flow id
 * Output:
 *      pUsFlow   - pointer to the upstream flow configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_usFlow_get(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.usFlowId, &usFlowId, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_USFLOW_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pUsFlow, &rt_gpon_cfg.usFlow, sizeof(rt_gpon_usFlow_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_usFlow_get */

/* Function Name:
 *      rt_gpon_usFlow_del
 * Description:
 *      Delete the upstream flow
 * Input:
 *      usFlowId  - logical flow id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_usFlow_del(uint32 usFlowId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.usFlowId, &usFlowId, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_GPON_USFLOW_DEL, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_usFlow_del */

/* Function Name:
 *      rt_gpon_usFlow_delAll
 * Description:
 *      Delete all the upstream flows
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 
rt_gpon_usFlow_delAll(void)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_RT_GPON_USFLOW_DELALL, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_usFlow_delAll */

/* Function Name:
 *      rt_gpon_dsFlow_set
 * Description:
 *      Set the downstream flow 
 * Input:
 *      dsFlowId  - logical flow id
 *      pDsFlow   - pointer to the downstream flow configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_dsFlow_set(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.dsFlowId, &dsFlowId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.dsFlow, pDsFlow, sizeof(rt_gpon_dsFlow_t));
    SETSOCKOPT(RTDRV_RT_GPON_DSFLOW_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_dsFlow_set */

/* Function Name:
 *      rt_gpon_dsFlow_get
 * Description:
 *      Get the downstream flow 
 * Input:
 *      dsFlowId  - logical flow id
 * Output:
 *      pDsFlow   - pointer to the downstream flow configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_dsFlow_get(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.dsFlowId, &dsFlowId, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_DSFLOW_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pDsFlow, &rt_gpon_cfg.dsFlow, sizeof(rt_gpon_dsFlow_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_dsFlow_get */

/* Function Name:
 *      rt_gpon_dsFlow_del
 * Description:
 *      Delete the downstream flow
 * Input:
 *      dsFlowId  - logical flow id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_dsFlow_del(uint32 dsFlowId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.dsFlowId, &dsFlowId, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_GPON_DSFLOW_DEL, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_dsFlow_del */

/* Function Name:
 *      rt_gpon_dsFlow_delAll
 * Description:
 *      Delete all the downstream flows
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 
rt_gpon_dsFlow_delAll(void)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_RT_GPON_DSFLOW_DELALL, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_dsFlow_delAll */

/* Function Name:
 *      rt_gpon_loop_gemport_set
 * Description:
 *      Delete all the downstream flows
 * Input:
 *      gemPortId - gem port Index
 *      enable - enable/disable gem loop function 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_DRIVER_NOT_FOUND
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 
rt_gpon_loop_gemport_set(uint32 gemPortId, rt_enable_t enable)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.loopGemEn, &enable, sizeof(rt_enable_t));
    osal_memcpy(&rt_gpon_cfg.loopGemPortId, &gemPortId, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_GPON_LOOPGEM_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_loop_gemport_set */

/* Function Name:
 *      rt_gpon_loop_gemport_get
 * Description:
 *      Delete all the downstream flows
 * Input:
 *      gemPortId - gem port Index
 * Output:
 *      pEnable    - Enable/disble loop GEM port function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_DRIVER_NOT_FOUND
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 
rt_gpon_loop_gemport_get(uint32 gemPortId, rt_enable_t *pEnable)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.loopGemPortId, &gemPortId, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_LOOPGEM_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pEnable, &rt_gpon_cfg.loopGemEn, sizeof(rt_enable_t));

    return RT_ERR_OK;

}   /* end of rt_gpon_loop_gemport_get */

/* Function Name:
 *      rt_gpon_ponQueue_set
 * Description:
 *      Set the pon queue
 * Input:
 *      tcontId    - logical tcont index
 *      tcQueueId  - logical tcont queue id
 *      pQueuecfg  - pointer to the queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_ponQueue_set(uint32 tcontId, uint32 tcQueueId, rt_gpon_queueCfg_t *pQueuecfg)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.tcontId, &tcontId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.tcQueueId, &tcQueueId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.queuecfg, pQueuecfg, sizeof(rt_gpon_queueCfg_t));
    SETSOCKOPT(RTDRV_RT_GPON_PONQUEUE_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_ponQueue_set */

/* Function Name:
 *      rt_gpon_ponQueue_get
 * Description:
 *      Get the pon queue
 * Input:
 *      tcontId    - logical tcont index
 *      tcQueueId  - logical tcont queue id
 * Output:
 *      pQueuecfg - pointer to the queue configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_ponQueue_get(uint32 tcontId, uint32 tcQueueId, rt_gpon_queueCfg_t *pQueuecfg)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.tcontId, &tcontId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.tcQueueId, &tcQueueId, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_PONQUEUE_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pQueuecfg, &rt_gpon_cfg.queuecfg, sizeof(rt_gpon_queueCfg_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_ponQueue_get */

/* Function Name:
 *      rt_gpon_ponQueue_del
 * Description:
 *      Delete the pon queue
 * Input:
 *      tcontId    - logical tcont index
 *      tcQueueId  - logical tcont queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_ponQueue_del(uint32 tcontId, uint32 tcQueueId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.tcontId, &tcontId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.tcQueueId, &tcQueueId, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_GPON_PONQUEUE_DEL, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_ponQueue_del */

/* Function Name:
 *      rt_gpon_scheInfo_get
 * Description:
 *      Get scheduling information
 * Input:
 *      None
 * Output:
 *      pScheInfo  - pointer to the scheduling information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_scheInfo_get(rt_gpon_schedule_info_t *pScheInfo)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pScheInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.scheInfo, pScheInfo, sizeof(rt_gpon_schedule_info_t));
    GETSOCKOPT(RTDRV_RT_GPON_SCHEINFO_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pScheInfo, &rt_gpon_cfg.scheInfo, sizeof(rt_gpon_schedule_info_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_scheInfo_get */

/* Function Name:
 *      rt_gpon_flowCounter_get
 * Description:
 *      Get the flow counters.
 * Input:
 *      flowId   - logical flow ID
 *      type     - counter type
 * Output:
 *      pFlowCnt - pointer to the flow counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_flowCounter_get(uint32 flowId, rt_gpon_flow_counter_type_t flowCntType, rt_gpon_flow_counter_t *pFlowCnt)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFlowCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.flowId, &flowId, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.flowCntType, &flowCntType, sizeof(rt_gpon_flow_counter_type_t));
    GETSOCKOPT(RTDRV_RT_GPON_FLOWCOUNTER_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pFlowCnt, &rt_gpon_cfg.flowCnt, sizeof(rt_gpon_flow_counter_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_flowCounter_get */

/* Function Name:
 *      rt_gpon_pmCounter_get
 * Description:
 *      Get the pm counters.
 * Input:
 *      pmCntType  - counter type
 * Output:
 *      pPmCnt       - pointer to the global counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_pmCounter_get(rt_gpon_pm_type_t pmCntType, rt_gpon_pm_counter_t *pPmCnt)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPmCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.pmCntType, &pmCntType, sizeof(rt_gpon_pm_type_t));
    GETSOCKOPT(RTDRV_RT_GPON_PMCOUNTER_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pPmCnt, &rt_gpon_cfg.pmCnt, sizeof(rt_gpon_pm_counter_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_pmCounter_get */

/* Function Name:
 *      rt_gpon_ponTag_get
 * Description:
 *      Get the PON Tag.
 * Input:
 *      None
 * Output:
 *      pPonTag       - pointer to the PON TAG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_ponTag_get(rt_gpon_ponTag_t *pPonTag)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTag), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));
    GETSOCKOPT(RTDRV_RT_GPON_PONTAG_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pPonTag, &rt_gpon_cfg.ponTag, sizeof(rt_gpon_ponTag_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_ponTag_get */

/* Function Name:
 *      rt_gpon_msk_set
 * Description:
 *      Set the Master Session Key.
 * Input:
 *      pMsk - pointer to the Master Session Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_msk_set(rt_gpon_msk_t *pMsk)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsk), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.msk, pMsk, sizeof(rt_gpon_msk_t));
    SETSOCKOPT(RTDRV_RT_GPON_MSK_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_msk_set */

/* Function Name:
 *      rt_gpon_omci_mic_generate
 * Description:
 *      Generate the OMCI MIC value according to the inputs.
 * Input:
 *      dir     - the direction of the OMCI msg, 0x01 for Downstream and 0x02 for Upstream
 *      pMsg    - pointer to the OMCI message data
 *      msgLen  - length of the OMCI message, not include MIC
 * Output:
 *      mic     - pointer to the generated MIC
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_omci_mic_generate(uint32 dir, uint8 *pMsg, uint32 msgLen, uint32 *mic)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == mic), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.dir, &dir, sizeof(uint32));
    osal_memcpy(&rt_gpon_cfg.msgLen, &msgLen, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_OMCI_MIC_GENERATE, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pMsg, &rt_gpon_cfg.msg, sizeof(uint8));
    osal_memcpy(mic, &rt_gpon_cfg.mic, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_gpon_omci_mic_generate */

/* Function Name:
 *      rt_gpon_mcKey_set
 * Description:
 *      Set the Multicast Key.
 * Input:
 *      pMcKey - pointer to the Multicast Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_mcKey_set(rt_gpon_mc_key_t *pMcKey)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcKey), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.mcKey, pMcKey, sizeof(rt_gpon_msk_t));
    SETSOCKOPT(RTDRV_RT_GPON_MCKEY_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_mcKey_set */

/* Function Name:
 *      rt_gpon_egrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      None
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32
rt_gpon_egrBandwidthCtrlRate_get(uint32 *pRate)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.rate, pRate, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_EGRBANDWIDTHCTRLRATE_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pRate, &rt_gpon_cfg.rate, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_gpon_egrBandwidthCtrlRate_get */

/* Function Name:
 *      rt_gpon_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32
rt_gpon_egrBandwidthCtrlRate_set(uint32 rate)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.rate, &rate, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_GPON_EGRBANDWIDTHCTRLRATE_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_egrBandwidthCtrlRate_set */

/* Function Name:
 *      rt_gpon_attribute_get
 * Description:
 *      Get the GPON related attribute.
 * Input:
 *      attributeType  - attribute type
 * Output:
 *      pAttributeValue       - pointer to the attribute value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_attribute_get(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAttributeValue), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.attributeType, &attributeType, sizeof(rt_gpon_attribute_type_t));
    GETSOCKOPT(RTDRV_RT_GPON_ATTRIBUTE_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pAttributeValue, &rt_gpon_cfg.attributeValue, sizeof(rt_gpon_attribute_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_attribute_get */

/* Function Name:
 *      rt_gpon_attribute_set
 * Description:
 *      Set the GPON related attribute.
 * Input:
 *      attributeType  - attribute type
 * Output:
 *      pAttributeValue       - pointer to the attribute value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_attribute_set(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAttributeValue), RT_ERR_NULL_POINTER);
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.attributeType, &attributeType, sizeof(rt_gpon_attribute_type_t));
    osal_memcpy(&rt_gpon_cfg.attributeValue, pAttributeValue, sizeof(rt_gpon_attribute_t));
    SETSOCKOPT(RTDRV_RT_GPON_ATTRIBUTE_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_attribute_set */

/* Function Name:
 *      rt_gpon_omci_mirror_set
 * Description:
 *      Set the GPON OMCI mirror function.
 * Input:
 *      enable - enable/disable mirro function 
 *      mirroringPort - mirroring port 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - Invalid input 
 * Note:
 */

int32
rt_gpon_omci_mirror_set(rt_enable_t enable, rt_port_t mirroringPort)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));

    /* function body */
    osal_memcpy(&rt_gpon_cfg.mirrorEn, &enable, sizeof(rt_enable_t));
    osal_memcpy(&rt_gpon_cfg.mirroringPort, &mirroringPort, sizeof(rt_port_t));
    SETSOCKOPT(RTDRV_RT_GPON_OMCI_MIRROR_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_omci_mirror_set */

/* Function Name:
 *      rt_gpon_omcc_get
 * Description:
 *      get omcc information.
 * Input:
 *      
 * Output:
 *      pOmcc                 -   omcc information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 */
int32 
rt_gpon_omcc_get(rt_gpon_omcc_t *pOmcc)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));
    
    /* parameter check */
    RT_PARAM_CHK((NULL == pOmcc), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_GPON_OMCC_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pOmcc, &rt_gpon_cfg.omcc, sizeof(rt_gpon_omcc_t));
    return RT_ERR_OK;
}   /* end of rt_gpon_omcc_get */

/* Function Name:
 *      rt_gpon_fec_get
 * Description:
 *      Get the GPON FEC status function.
 * Input:
 *
 * Output:
 *      pFecStatus          -   fec status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_fec_get(rt_gpon_fec_status_t *pFecStatus)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFecStatus), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_gpon_cfg, 0x0, sizeof(rtdrv_rt_gponCfg_t));
    GETSOCKOPT(RTDRV_RT_GPON_FEC_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pFecStatus, &rt_gpon_cfg.fec, sizeof(rt_gpon_fec_status_t));

    return RT_ERR_OK;
} /* end of rt_gpon_fec_get */

/* Function Name:
 *      rt_gpon_serialNumber_get
 * Description:
 *      Get GPON serial number
 * Input:
 *      pSN   - pointer to the GPON serial number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_serialNumber_get(rt_gpon_serialNumber_t *pSN)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSN), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_GPON_SERIALNUMBER_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pSN, &rt_gpon_cfg.sN, sizeof(rt_gpon_serialNumber_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_serialNumber_get */

/* Function Name:
 *      rt_gpon_registrationId_get
 * Description:
 *      Get GPON registration ID (password in 984.3).
 * Input:
 *      pRegId   - pointer to the GPON registration id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_registrationId_get(rt_gpon_registrationId_t *pRegId)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegId), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_GPON_REGISTRATIONID_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pRegId, &rt_gpon_cfg.regId, sizeof(rt_gpon_registrationId_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_registrationId_get */

/* Function Name:
 *      rt_gpon_forceEmergencyStop_set
 * Description:
 *      Set the GPON force emergency Stop state.
 * Input:
 *      State  - enable/disable emergency stop state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
rt_gpon_forceEmergencyStop_set(rt_enable_t state)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_INPUT);

    memset(&rt_gpon_cfg, 0x0, sizeof(rt_gpon_cfg));
    /* function body */
    osal_memcpy(&rt_gpon_cfg.emergencyStop, &state, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_GPON_EMERGENCY_STOP_SET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_gpon_forceEmergencyStop_set */

/* Function Name:
 *      rt_gpon_tcontCounter_get
 * Description:
 *      Get the tcont counters.
 * Input:
 *      pTcontCnt->tconId  - tcont ID
 * Output:
 *      pTcontCnt - pointer to the flow counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_tcontCounter_get(rt_gpon_tcont_counter_t *pTcontCnt)
{
    rtdrv_rt_gponCfg_t rt_gpon_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTcontCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_gpon_cfg.tcontCnt.tcont_id, &pTcontCnt->tcont_id, sizeof(uint32));
    GETSOCKOPT(RTDRV_RT_GPON_TCONTCOUNTER_GET, &rt_gpon_cfg, rtdrv_rt_gponCfg_t, 1);
    osal_memcpy(pTcontCnt, &rt_gpon_cfg.tcontCnt, sizeof(rt_gpon_tcont_counter_t));

    return RT_ERR_OK;
}   /* end of rt_gpon_tcontCounter_get */
